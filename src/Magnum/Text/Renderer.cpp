/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include "Renderer.h"

#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/StringView.h>

#include "Magnum/Math/Functions.h"
#include "Magnum/Math/FunctionsBatch.h"
#include "Magnum/Math/Range.h"
#include "Magnum/Text/AbstractFont.h"
#include "Magnum/Text/AbstractGlyphCache.h"
#include "Magnum/Text/AbstractShaper.h"
#include "Magnum/Text/Alignment.h"
#include "Magnum/Text/Direction.h"
#include "Magnum/Text/Implementation/rendererState.h"

/* Somehow on GCC 4.8 to 7 the {} passed as a default argument for
   ArrayView<const FeatureRange> causes "error: elements of array 'const class
   Magnum::Text::FeatureRange [0]' have incomplete type". GCC 9 is fine, no
   idea about version 8, but including the definition for it as well to be
   safe. Similar problem happens with MSVC STL, where the initializer_list is
   implemented as a (begin, end) pair and size() is a difference of those two
   pointers. Which needs to know the type size to calculate the actual element
   count. */
#if (defined(CORRADE_TARGET_GCC) && __GNUC__ <= 8) || defined(CORRADE_TARGET_DINKUMWARE)
#include "Magnum/Text/Feature.h"
#endif

namespace Magnum { namespace Text {

Debug& operator<<(Debug& debug, const RendererCoreFlag value) {
    debug << "Text::RendererCoreFlag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case RendererCoreFlag::v: return debug << "::" #v;
        _c(GlyphClusters)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << Debug::hex << UnsignedByte(value) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const RendererCoreFlags value) {
    return Containers::enumSetDebugOutput(debug, value, "Text::RendererCoreFlags{}", {
        RendererCoreFlag::GlyphClusters
    });
}

namespace {

struct Glyph {
    Vector2 position;
    UnsignedInt id;
};

struct GlyphCluster {
    Vector2 position;
    UnsignedInt id;
    UnsignedInt cluster;
};

auto defaultGlyphAllocatorFor(const RendererCoreFlags flags) -> void(*)(void*, UnsignedInt, Containers::StridedArrayView1D<Vector2>&, Containers::StridedArrayView1D<UnsignedInt>&, Containers::StridedArrayView1D<UnsignedInt>*, Containers::StridedArrayView1D<Vector2>&) {
    if(flags >= RendererCoreFlag::GlyphClusters)
        return [](void* const state, const UnsignedInt glyphCount, Containers::StridedArrayView1D<Vector2>& glyphPositions, Containers::StridedArrayView1D<UnsignedInt>& glyphIds, Containers::StridedArrayView1D<UnsignedInt>* const glyphClusters, Containers::StridedArrayView1D<Vector2>& glyphAdvances) {
            Containers::Array<char>& data = *static_cast<Containers::Array<char>*>(state);
            /* The array may not be fully used yet, or it might have been reset
               back to empty. Append only if the desired capacity is more than
               what's there. */
            const std::size_t existingSize = glyphPositions.size();
            const std::size_t desiredByteSize = (existingSize + glyphCount)*sizeof(GlyphCluster);
            if(desiredByteSize > data.size()) {
                /* Using arrayAppend() as it reallocates with a growth
                   strategy, arrayResize() would take the size literally */
                arrayAppend(data, NoInit, desiredByteSize - data.size());
            }
            /* The new capacity is the actual array size, not just
               `desiredByteSize`. If the array got enlarged by exactly the
               requested `size`, it'll be the same as `desiredByteSize`. If the
               array was larger, such as after a clear(), the capacity will
               again use up all of it. */
            const Containers::StridedArrayView1D<GlyphCluster> glyphs = Containers::arrayCast<GlyphCluster>(data);
            glyphPositions = glyphs.slice(&GlyphCluster::position);
            glyphIds = glyphs.slice(&GlyphCluster::id);
            *glyphClusters = glyphs.slice(&GlyphCluster::cluster);
            /* As IDs and clusters are right after each other and have the same
               size as a Vector2, we can abuse them to store advances. Those
               are guaranteed to be always filled only once advances are no
               longer needed, so that's fine -- but we need to ensure that we
               point to the new memory, not to the existing, where it'd
               overwrite existing IDs and clusters. */
            glyphAdvances = Containers::arrayCast<Vector2>(glyphs.slice(&GlyphCluster::id).exceptPrefix(existingSize));
        };
    else
        return [](void* const state, const UnsignedInt glyphCount, Containers::StridedArrayView1D<Vector2>& glyphPositions, Containers::StridedArrayView1D<UnsignedInt>& glyphIds, Containers::StridedArrayView1D<UnsignedInt>*, Containers::StridedArrayView1D<Vector2>& glyphAdvances) {
            Containers::Array<char>& data = *static_cast<Containers::Array<char>*>(state);
            /* The array may not be fully used yet, or it might have been reset
               back to empty. Append only if the desired capacity is more than
               what's there. Unlike above we don't have any place to alias
               advances with, so append them at the end. */
            const std::size_t desiredByteSize = glyphPositions.size()*sizeof(Glyph) + glyphCount*(sizeof(Glyph) + sizeof(Vector2));
            if(desiredByteSize > data.size()) {
                /* Using arrayAppend() as it reallocates with a growth
                   strategy, arrayResize() would take the size literally */
                arrayAppend(data, NoInit, desiredByteSize - data.size());
            }
            /* Calculate the new capacity from the actual array size. Compared
               to the above, we need to make sure the unused space at the end
               is correctly divided between the Glyph and the Vector2 for
               advances. */
            const std::size_t newCapacity = glyphPositions.size() + (data.size() - glyphPositions.size()*sizeof(Glyph))/(sizeof(Glyph) + sizeof(Vector2));
            const std::size_t newSize = newCapacity - glyphPositions.size();

            const Containers::StridedArrayView1D<Glyph> glyphs = Containers::arrayCast<Glyph>(data.prefix(newCapacity*sizeof(Glyph)));
            glyphPositions = glyphs.slice(&Glyph::position);
            glyphIds = glyphs.slice(&Glyph::id);
            /* Don't take just the suffix for advances as the size may not be
               divisible by sizeof(Vector2), especially after clear() */
            glyphAdvances = Containers::arrayCast<Vector2>(data.sliceSize(newCapacity*sizeof(Glyph), newSize*sizeof(Vector2)));
        };
}

struct TextRun {
    Float scale;
    UnsignedInt end;
};

void defaultRunAllocator(void* const state, const UnsignedInt runCount, Containers::StridedArrayView1D<Float>& runScales, Containers::StridedArrayView1D<UnsignedInt>& runEnds) {
    Containers::Array<char>& data = *static_cast<Containers::Array<char>*>(state);

    const std::size_t newSize = runScales.size() + runCount;
    const std::size_t desiredByteSize = newSize*sizeof(TextRun);
    if(desiredByteSize > data.size()) {
        /* Using arrayAppend() as it reallocates with a growth strategy,
           arrayResize() would take the size literally */
        arrayAppend(data, NoInit, desiredByteSize - data.size());
    }

    const Containers::StridedArrayView1D<TextRun> runs = Containers::arrayCast<TextRun>(data);
    runScales = runs.slice(&TextRun::scale);
    runEnds = runs.slice(&TextRun::end);
}

}

RendererCore::AllocatorState::AllocatorState(const AbstractGlyphCache& glyphCache, void(*glyphAllocator)(void*, UnsignedInt, Containers::StridedArrayView1D<Vector2>&, Containers::StridedArrayView1D<UnsignedInt>&, Containers::StridedArrayView1D<UnsignedInt>*, Containers::StridedArrayView1D<Vector2>&), void* glyphAllocatorState, void(*const runAllocator)(void*, UnsignedInt, Containers::StridedArrayView1D<Float>&, Containers::StridedArrayView1D<UnsignedInt>&), void* runAllocatorState, RendererCoreFlags flags): State{glyphCache,
    glyphAllocator ? glyphAllocator : defaultGlyphAllocatorFor(flags),
    glyphAllocator ? glyphAllocatorState : &glyphData,
    runAllocator ? runAllocator : defaultRunAllocator,
    runAllocator ? runAllocatorState : &runData, flags} {}

RendererCore::RendererCore(const AbstractGlyphCache& glyphCache, void(*glyphAllocator)(void*, UnsignedInt, Containers::StridedArrayView1D<Vector2>&, Containers::StridedArrayView1D<UnsignedInt>&, Containers::StridedArrayView1D<UnsignedInt>*, Containers::StridedArrayView1D<Vector2>&), void* glyphAllocatorState, void(*runAllocator)(void*, UnsignedInt, Containers::StridedArrayView1D<Float>&, Containers::StridedArrayView1D<UnsignedInt>&), void* runAllocatorState, const RendererCoreFlags flags): _state{
    /* If either allocator is left at the default, create a state that includes
       the data arrays for use by the internal allocators. If both are
       user-specified, there's no need to have them as they're unused. */
    glyphAllocator && runAllocator ?
        Containers::pointer<State>(glyphCache, glyphAllocator, glyphAllocatorState, runAllocator, runAllocatorState, flags) :
        Containers::pointer<AllocatorState>(glyphCache, glyphAllocator, glyphAllocatorState, runAllocator, runAllocatorState, flags)} {}

RendererCore::RendererCore(Containers::Pointer<State>&& state): _state{Utility::move(state)} {}

RendererCore::RendererCore(NoCreateT) noexcept {}

RendererCore::RendererCore(RendererCore&&) noexcept = default;

RendererCore::~RendererCore() = default;

RendererCore& RendererCore::operator=(RendererCore&&) noexcept = default;

const AbstractGlyphCache& RendererCore::glyphCache() const {
    return _state->glyphCache;
}

RendererCoreFlags RendererCore::flags() const {
    /* Subclasses inherit and add their own flags, mask them away */
    return _state->flags & RendererCoreFlags{0x1};
}

UnsignedInt RendererCore::glyphCount() const {
    return _state->glyphCount;
}

UnsignedInt RendererCore::glyphCapacity() const {
    return _state->glyphPositions.size();
}

UnsignedInt RendererCore::runCount() const {
    return _state->runCount;
}

UnsignedInt RendererCore::runCapacity() const {
    return _state->runScales.size();
}

bool RendererCore::isRendering() const {
    return _state->rendering;
}

UnsignedInt RendererCore::renderingGlyphCount() const {
    return _state->renderingGlyphCount;
}

UnsignedInt RendererCore::renderingRunCount() const {
    return _state->renderingRunCount;
}

Vector2 RendererCore::cursor() const {
    return _state->cursor;
}

RendererCore& RendererCore::setCursor(const Vector2& cursor) {
    State& state = *_state;
    CORRADE_ASSERT(!state.rendering,
        "Text::RendererCore::setCursor(): rendering in progress", *this);
    state.cursor = cursor;
    return *this;
}

Alignment RendererCore::alignment() const {
    return _state->alignment;
}

RendererCore& RendererCore::setAlignment(const Alignment alignment) {
    State& state = *_state;
    CORRADE_ASSERT(!state.rendering,
        "Text::RendererCore::setAlignment(): rendering in progress", *this);
    state.alignment = alignment;
    return *this;
}

Float RendererCore::lineAdvance() const {
    return _state->lineAdvance;
}

RendererCore& RendererCore::setLineAdvance(const Float advance) {
    State& state = *_state;
    CORRADE_ASSERT(!state.rendering,
        "Text::RendererCore::setLineAdvance(): rendering in progress", *this);
    state.lineAdvance = advance;
    return *this;
}

LayoutDirection RendererCore::layoutDirection() const {
    return _state->layoutDirection;
}

RendererCore& RendererCore::setLayoutDirection(const LayoutDirection direction) {
    State& state = *_state;
    CORRADE_ASSERT(!state.rendering,
        "Text::RendererCore::setLayoutDirection(): rendering in progress", *this);
    CORRADE_ASSERT(direction == LayoutDirection::HorizontalTopToBottom,
        "Text::RendererCore::setLayoutDirection(): only" << LayoutDirection::HorizontalTopToBottom << "is supported right now, got" << direction, *this);
    state.layoutDirection = direction;
    return *this;
}

Containers::StridedArrayView1D<const Vector2> RendererCore::glyphPositions() const {
    const State& state = *_state;
    return state.glyphPositions.prefix(state.glyphCount);
}

Containers::StridedArrayView1D<const UnsignedInt> RendererCore::glyphIds() const {
    const State& state = *_state;
    return state.glyphIds.prefix(state.glyphCount);
}

Containers::StridedArrayView1D<const UnsignedInt> RendererCore::glyphClusters() const {
    const State& state = *_state;
    CORRADE_ASSERT(state.flags & RendererCoreFlag::GlyphClusters,
        "Text::RendererCore::glyphClusters(): glyph clusters not enabled", {});
    return state.glyphClusters.prefix(state.glyphCount);
}

Containers::StridedArrayView1D<const Float> RendererCore::runScales() const {
    const State& state = *_state;
    return state.runScales.prefix(state.runCount);
}

Containers::StridedArrayView1D<const UnsignedInt> RendererCore::runEnds() const {
    const State& state = *_state;
    return state.runEnds.prefix(state.runCount);
}

Range1Dui RendererCore::glyphsForRuns(const Range1Dui& runRange) const {
    const State& state = *_state;
    CORRADE_ASSERT(runRange.min() <= state.renderingRunCount &&
                   runRange.max() <= state.renderingRunCount,
        /* The Vector2ui is to avoid double {}s in the printed value */
        /** @todo fix the printer itself, maybe? */
        "Text::RendererCore::glyphsForRuns(): runs" << Debug::packed << (Vector2ui{runRange.min(), runRange.max()}) << "out of range for" << state.renderingRunCount << "runs", {});
    return {runRange.min() ? state.runEnds[runRange.min() - 1] : 0,
            runRange.max() ? state.runEnds[runRange.max() - 1] : 0};
}

void RendererCore::allocateGlyphs(
    #ifndef CORRADE_NO_ASSERT
    const char* const messagePrefix,
    #endif
    const UnsignedInt totalGlyphCount)
{
    State& state = *_state;

    /* This function should only be called if we need more memory or from
       clear() with everything empty */
    CORRADE_INTERNAL_DEBUG_ASSERT(totalGlyphCount > state.glyphPositions.size() || (state.glyphCount == 0 && state.renderingGlyphCount == 0 && totalGlyphCount == 0));

    /* Sliced copies of the views for the allocator to update. As this is
       called from add(), all glyph contents until `state.renderingGlyphCount`
       should be preserved, not just `state.glyphCount`. */
    Containers::StridedArrayView1D<Vector2> glyphPositions =
        state.glyphPositions.prefix(state.renderingGlyphCount);
    Containers::StridedArrayView1D<UnsignedInt> glyphIds =
        state.glyphIds.prefix(state.renderingGlyphCount);
    Containers::StridedArrayView1D<UnsignedInt> glyphClusters = state.flags & RendererCoreFlag::GlyphClusters ?
        state.glyphClusters.prefix(state.renderingGlyphCount) : nullptr;
    /* Advances are just temporary and thus we don't need to preserve existing
       contents. But the allocator may still want to know where it's coming
       from so give it a non-null empty view if possible */
    Containers::StridedArrayView1D<Vector2> glyphAdvances =
        state.glyphAdvances.prefix(0);

    /* While this function gets total glyph count, the allocator gets glyph
       count to grow by instead */
    state.glyphAllocator(state.glyphAllocatorState,
        totalGlyphCount - state.renderingGlyphCount,
        glyphPositions,
        glyphIds,
        state.flags & RendererCoreFlag::GlyphClusters ? &glyphClusters : nullptr,
        glyphAdvances);
    /* Take the smallest size of all as the new capacity. Again the advances
       don't preserve the previous contents so they're just the new size. Add
       the existing glyph count to that instead of subtracting glyph count from
       all others to avoid an underflow. */
    std::size_t minCapacity = Math::min({
        glyphPositions.size(),
        glyphIds.size(),
        state.renderingGlyphCount + glyphAdvances.size()});
    /* These assertions are present even for the builtin allocator but
       shouldn't fire. If they do, the whole thing is broken, but it's better
       to blow up with a nice message than with some strange OOB error later */
    if(state.flags & RendererCoreFlag::GlyphClusters) {
        minCapacity = Math::min(minCapacity, glyphClusters.size());
        CORRADE_ASSERT(minCapacity >= totalGlyphCount,
            messagePrefix << "expected allocated glyph positions, IDs and clusters to have at least" << totalGlyphCount << "elements and advances" << totalGlyphCount - state.renderingGlyphCount << "but got" << glyphPositions.size() << Debug::nospace << "," << glyphIds.size() << Debug::nospace << "," << glyphClusters.size() << "and" << glyphAdvances.size(), );
    } else {
        CORRADE_ASSERT(minCapacity >= totalGlyphCount,
            messagePrefix << "expected allocated glyph positions and IDs to have at least" << totalGlyphCount << "elements and advances" << totalGlyphCount - state.renderingGlyphCount << "but got" << glyphPositions.size() << Debug::nospace << "," << glyphIds.size() << "and" << glyphAdvances.size(), );
    }

    /* Keep just the minimal size for all, which is the new capacity */
    state.glyphPositions = glyphPositions.prefix(minCapacity);
    state.glyphIds = glyphIds.prefix(minCapacity);
    if(state.flags & RendererCoreFlag::GlyphClusters)
        state.glyphClusters = glyphClusters.prefix(minCapacity);
    /* Again the advances are just the size alone, not the full capacity */
    state.glyphAdvances = glyphAdvances.prefix(minCapacity - state.renderingGlyphCount);
}

void RendererCore::allocateRuns(
    #ifndef CORRADE_NO_ASSERT
    const char* const messagePrefix,
    #endif
    const UnsignedInt totalRunCount)
{
    State& state = *_state;

    /* This function should only be called if we need more memory or from
       clear() with everything empty */
    CORRADE_INTERNAL_DEBUG_ASSERT(totalRunCount > state.runScales.size() || (state.runCount == 0 && state.renderingRunCount == 0 && totalRunCount == 0));

    /* Sliced copies of the views for the allocator to update. As this is
       called from add(), all run contents until `state.renderingRunCount`
       should be preserved, not just `state.runCount`. */
    Containers::StridedArrayView1D<Float> runScales =
        state.runScales.prefix(state.renderingRunCount);
    Containers::StridedArrayView1D<UnsignedInt> runEnds =
        state.runEnds.prefix(state.renderingRunCount);

    /* While this function gets total run count, the allocator gets run count
       to grow by instead */
    state.runAllocator(state.runAllocatorState,
        totalRunCount - state.renderingRunCount,
        runScales,
        runEnds);
    /* Take the smallest size of all as the new capacity */
    const std::size_t minCapacity = Math::min(
        runScales.size(),
        runEnds.size());
    /* These assertions are present even for the builtin allocator but
       shouldn't fire. If they do, the whole thing is broken, but it's better
       to blow up with a nice message than with some strange OOB error later */
    CORRADE_ASSERT(minCapacity >= totalRunCount,
        messagePrefix << "expected allocated run scales and ends to have at least" << totalRunCount << "elements but got" << runScales.size() << "and" << runEnds.size(), );

    /* Keep just the minimal size for all, which is the new capacity */
    state.runScales = runScales.prefix(minCapacity);
    state.runEnds = runEnds.prefix(minCapacity);
}

RendererCore& RendererCore::reserve(const UnsignedInt glyphCapacity, const UnsignedInt runCapacity) {
    State& state = *_state;

    if(state.glyphPositions.size() < glyphCapacity)
        allocateGlyphs(
            #ifndef CORRADE_NO_ASSERT
            "Text::RendererCore::reserve():",
            #endif
            glyphCapacity);
    if(state.runScales.size() < runCapacity)
        allocateRuns(
            #ifndef CORRADE_NO_ASSERT
            "Text::RendererCore::reserve():",
            #endif
            runCapacity);

    return *this;
}

RendererCore& RendererCore::clear() {
    State& state = *_state;

    /* Reset the glyph / run count to 0 and call the allocators, requesting 0
       glyphs and runs as well. It may make use of that to refresh itself. */
    state.glyphCount = 0;
    state.renderingGlyphCount = 0;
    state.runCount = 0;
    state.renderingRunCount = 0;
    allocateGlyphs(
        #ifndef CORRADE_NO_ASSERT
        "", /* Asserts won't happen as returned sizes will be always >= 0 */
        #endif
        0);
    allocateRuns(
        #ifndef CORRADE_NO_ASSERT
        "", /* Asserts won't happen as returned sizes will be always >= 0 */
        #endif
        0);

    /* All in-progress rendering, both for the block and for the line, should
       be cleaned up */
    state.rendering = false;
    state.resolvedAlignment = {};
    state.renderingLineStart = {};
    state.renderingLineCursor = {};
    state.renderingLineAdvance = {};
    state.blockRunBegin = {};
    state.blockRectangle = {};
    state.lineGlyphBegin = {};
    state.lineRectangle = {};

    return *this;
}

void RendererCore::resetInternal() {
    State& state = *_state;

    /* Keep in sync with the initializers in the State struct */
    state.alignment = Alignment::MiddleCenter;
    state.layoutDirection = LayoutDirection::HorizontalTopToBottom;
    state.cursor = {};
    state.lineAdvance = {};
}

RendererCore& RendererCore::reset() {
    clear();
    /* Reset also all other settable state to defaults. Is in a separate helper
       because it gets called from Renderer::reset() as well. */
    resetInternal();

    return *this;
}

void RendererCore::alignAndFinishLine() {
    State& state = *_state;
    CORRADE_INTERNAL_DEBUG_ASSERT(state.lineGlyphBegin != state.renderingGlyphCount && state.resolvedAlignment);

    const Range2D alignedLineRectangle = alignRenderedLine(
        state.lineRectangle,
        state.layoutDirection,
        *state.resolvedAlignment,
        state.glyphPositions.slice(state.lineGlyphBegin, state.renderingGlyphCount));

    /* Extend the block rectangle with final line bounds */
    state.blockRectangle = Math::join(state.blockRectangle, alignedLineRectangle);

    /* New line starts after all existing glyphs and is empty */
    state.lineGlyphBegin = state.renderingGlyphCount;
    state.lineRectangle = {};
}

RendererCore& RendererCore::add(AbstractShaper& shaper, const Float size, const Containers::StringView text, const UnsignedInt begin, const UnsignedInt end, const Containers::ArrayView<const FeatureRange> features) {
    State& state = *_state;

    /* Mark as rendering in progress if not already */
    state.rendering = true;

    /* Query ID of shaper font in the cache for performing glyph ID mapping */
    const Containers::Optional<UnsignedInt> glyphCacheFontId = state.glyphCache.findFont(shaper.font());
    CORRADE_ASSERT(glyphCacheFontId,
        "Text::RendererCore::add(): shaper font not found among" << state.glyphCache.fontCount() << "fonts in associated glyph cache", *this);

    /* Scaling factor, line advance taken from the font if not specified
       externally. Currently assuming just horizontal layout direction, so the
       line advance is vertical. */
    /** @todo update once allowing other directions */
    const AbstractFont& font = shaper.font();
    const Float scale = size/font.size();
    CORRADE_INTERNAL_DEBUG_ASSERT(state.layoutDirection == LayoutDirection::HorizontalTopToBottom);
    if(state.renderingLineAdvance == Vector2{}) {
        if(state.lineAdvance != 0.0f)
            state.renderingLineAdvance = Vector2::yAxis(-state.lineAdvance);
        else
            state.renderingLineAdvance = Vector2::yAxis(-font.lineHeight()*scale);
    }

    Containers::StringView line = text.slice(begin, end);
    while(line) {
        const Containers::StringView lineEnd = line.findOr('\n', line.end());
        /* Comparing like this to avoid an unnecessary memcmp(). If we reach
           the end of the input text, it's *not* an end of the line, because
           the next add() call may continue with it. */
        const bool isEndOfLine = lineEnd.size() == 1 && lineEnd[0] == '\n';

        /* If the line is not empty and produced some glyphs, render them */
        if(const UnsignedInt glyphCount = lineEnd.begin() != line.begin() ? shaper.shape(text, line.begin() - text.begin(), lineEnd.begin() - text.begin(), features) : 0) {
            /* If we need to add more glyphs than what's in the capacity,
               allocate more */
            if(state.glyphPositions.size() < state.renderingGlyphCount + glyphCount) {
                allocateGlyphs(
                    #ifndef CORRADE_NO_ASSERT
                    "Text::RendererCore::add():",
                    #endif
                    state.renderingGlyphCount + glyphCount);
                #ifdef CORRADE_GRACEFUL_ASSERT
                /* For testing only -- if allocation failed, bail */
                if(state.glyphPositions.size() < state.renderingGlyphCount + glyphCount)
                    return *this;
                #endif
            }

            const Containers::StridedArrayView1D<Vector2> glyphOffsetsPositions = state.glyphPositions.sliceSize(state.renderingGlyphCount, glyphCount);
            /* The glyph advance array may be aliasing IDs and clusters. Pick
               only a suffix of the same size as the remaining capacity -- that
               memory is guaranteed to be unused yet. */
            const std::size_t remainingCapacity = state.glyphPositions.size() - state.renderingGlyphCount;
            const Containers::StridedArrayView1D<Vector2> glyphAdvances = state.glyphAdvances.sliceSize(state.glyphAdvances.size() - remainingCapacity, glyphCount);
            shaper.glyphOffsetsAdvancesInto(
                glyphOffsetsPositions,
                glyphAdvances);

            /* Render line glyph positions, aliasing the offsets */
            const Range2D rectangle = renderLineGlyphPositionsInto(
                shaper.font(),
                size,
                state.layoutDirection,
                glyphOffsetsPositions,
                glyphAdvances,
                state.renderingLineCursor,
                glyphOffsetsPositions);

            /* Retrieve the glyph IDs and clusters, convert the glyph IDs to
               cache-global. Do it only after finalizing the positions so the
               glyphAdvances array can alias the IDs. */
            const Containers::StridedArrayView1D<UnsignedInt> glyphIds = state.glyphIds.sliceSize(state.renderingGlyphCount, glyphCount);
            shaper.glyphIdsInto(glyphIds);
            state.glyphCache.glyphIdsInto(*glyphCacheFontId, glyphIds, glyphIds);
            if(state.flags & RendererCoreFlag::GlyphClusters)
                shaper.glyphClustersInto(state.glyphClusters.sliceSize(state.renderingGlyphCount, glyphCount));

            /* If we're aligning based on glyph bounds, calculate a rectangle
               from scratch instead of using a rectangle based on advances and
               font metrics. Join the resulting rectangle with one that's
               maintained for the line so far. */
            state.lineRectangle = Math::join(state.lineRectangle,
                (UnsignedByte(state.alignment) & Implementation::AlignmentGlyphBounds) ?
                    glyphQuadBounds(state.glyphCache, scale, glyphOffsetsPositions, glyphIds) :
                    rectangle);

            state.renderingGlyphCount += glyphCount;
        }

        /* If the alignment isn't resolved yet and the shaper detected any
           usable direction (or we're at the end of the line where we need
           it), resolve it. If there's no usable direction detected yet, maybe
           it will be next time. */
        if(!state.resolvedAlignment) {
            /* In this case it may happen that we query direction on a shaper
               for which shape() wasn't called yet, for example if shaping a
               text starting with \n and the previous text shaping gave back
               ShapeDirection::Unspecified as well. In such case it likely
               returns ShapeDirection::Unspecified too. */
            const ShapeDirection shapeDirection = shaper.direction();
            if(shapeDirection != ShapeDirection::Unspecified || isEndOfLine)
                state.resolvedAlignment = alignmentForDirection(
                    state.alignment,
                    state.layoutDirection,
                    shapeDirection);
        }

        /* If a newline follows, wrap up the existing line. This can happen
           independently of whether any glyphs were processed in this
           iteration, as add() can be called with a string that starts with a
           \n, for example. */
        if(isEndOfLine) {
            /* If there are any glyphs on the current line, either added right
               above or being there from the previous add() call, align them.
               If alignment based on bounds is requested, calculate a special
               rectangle for it. */
            if(state.lineGlyphBegin != state.renderingGlyphCount)
                alignAndFinishLine();

            /* Move the cursor for the next line */
            state.renderingLineStart += state.renderingLineAdvance;
            state.renderingLineCursor = state.renderingLineStart;
        }

        /* For the next iteration cut away everything that got processed,
           including the \n */
        line = line.suffix(lineEnd.end());
    }

    /* Final alignment of the whole block happens in render() below */

    /* Save the whole thing as a new run, if any glyphs were added at all.
       Right now it's just a single run each time add() is called, but
       eventually it might get split by lines or by shaping direction. */
    if((state.renderingRunCount ? state.runEnds[state.renderingRunCount - 1] : 0) < state.renderingGlyphCount) {
        if(state.runScales.size() <= state.renderingRunCount) {
            allocateRuns(
                #ifndef CORRADE_NO_ASSERT
                "Text::RendererCore::add():",
                #endif
                state.renderingRunCount + 1);
            #ifdef CORRADE_GRACEFUL_ASSERT
            /* For testing only -- if allocation failed, bail */
            if(state.runScales.size() <= state.renderingRunCount)
                return *this;
            #endif
        }
        state.runScales[state.renderingRunCount] = scale;
        state.runEnds[state.renderingRunCount] = state.renderingGlyphCount;
        ++state.renderingRunCount;
    }

    return *this;
}

RendererCore& RendererCore::add(AbstractShaper& shaper, const Float size, const Containers::StringView text, const UnsignedInt begin, const UnsignedInt end) {
    return add(shaper, size, text, begin, end, {});
}

RendererCore& RendererCore::add(AbstractShaper& shaper, const Float size, const Containers::StringView text, const UnsignedInt begin, const UnsignedInt end, const std::initializer_list<FeatureRange> features) {
    return add(shaper, size, text, begin, end, Containers::arrayView(features));
}

RendererCore& RendererCore::add(AbstractShaper& shaper, const Float size, const Containers::StringView text, const Containers::ArrayView<const FeatureRange> features) {
    return add(shaper, size, text, 0, text.size(), features);
}

RendererCore& RendererCore::add(AbstractShaper& shaper, const Float size, const Containers::StringView text) {
    return add(shaper, size, text, {});
}

RendererCore& RendererCore::add(AbstractShaper& shaper, const Float size, const Containers::StringView text, const std::initializer_list<FeatureRange> features) {
    return add(shaper, size, text, Containers::arrayView(features));
}

Containers::Pair<Range2D, Range1Dui> RendererCore::render() {
    State& state = *_state;

    /* If the alignment still isn't resolved at this point, it means it either
       stayed at ShapeDirection::Unspecified for all text added so far, or
       there wasn't any text actually added. Go with whatever
       alignmentForDirection() picks, then. Also, state.resolvedAlignment is
       going to get reset right at the end, but let's just write there
       temporarily, the logic is easier that way. */
    if(!state.resolvedAlignment)
        state.resolvedAlignment = alignmentForDirection(
            state.alignment,
            state.layoutDirection,
            ShapeDirection::Unspecified);

    /* Align the last unfinished line. In most cases there will be, unless the
       last text passed to add() was ending with a \n. */
    if(state.lineGlyphBegin != state.renderingGlyphCount)
        alignAndFinishLine();

    /* Align the block. Now it's respecting the alignment relative to the
       origin, move everything relative to the actual desired cursor. Could
       probably do that in the alignRendered*() by passing a specially crafted
       rect that's shifted by the cursor, but that'd become a testing nightmare
       with vertical text or when per-line advance is implemented. So just do
       that after. */
    const Containers::StridedArrayView1D<Vector2> blockGlyphPositions = state.glyphPositions.slice(state.blockRunBegin ? state.runEnds[state.blockRunBegin - 1] : 0, state.renderingRunCount ? state.runEnds[state.renderingRunCount - 1] : 0);
    const Range2D alignedBlockRectangle = alignRenderedBlock(
        state.blockRectangle,
        state.layoutDirection,
        *state.resolvedAlignment,
        blockGlyphPositions);
    for(Vector2& i: blockGlyphPositions)
        i += state.cursor;

    /* Reset all block-related state, marking the renderer as not in progress
       anymore. Line-related state should be reset after the alignLine() above
       already. */
    const UnsignedInt blockRunBegin = state.blockRunBegin;
    state.rendering = false;
    state.resolvedAlignment = {};
    state.renderingLineStart = {};
    state.renderingLineCursor = {};
    state.renderingLineAdvance = {};
    CORRADE_INTERNAL_DEBUG_ASSERT(state.lineGlyphBegin == state.renderingGlyphCount &&
                                  state.lineRectangle == Range2D{});
    state.glyphCount = state.renderingGlyphCount;
    state.runCount = state.renderingRunCount;
    state.blockRunBegin = state.runCount;
    state.blockRectangle = {};

    return {alignedBlockRectangle.translated(state.cursor), {blockRunBegin, state.runCount}};
}

Containers::Pair<Range2D, Range1Dui> RendererCore::render(AbstractShaper& shaper, const Float size, const Containers::StringView text, const Containers::ArrayView<const FeatureRange> features) {
    add(shaper, size, text, features);
    return render();
}

Containers::Pair<Range2D, Range1Dui> RendererCore::render(AbstractShaper& shaper, const Float size, const Containers::StringView text) {
    return render(shaper, size, text, {});
}

Containers::Pair<Range2D, Range1Dui> RendererCore::render(AbstractShaper& shaper, const Float size, const Containers::StringView text, const std::initializer_list<FeatureRange> features) {
    return render(shaper, size, text, Containers::arrayView(features));
}

Debug& operator<<(Debug& debug, const RendererFlag value) {
    debug << "Text::RendererFlag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case RendererFlag::v: return debug << "::" #v;
        _c(GlyphPositionsClusters)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << Debug::hex << UnsignedByte(value) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const RendererFlags value) {
    return Containers::enumSetDebugOutput(debug, value, "Text::RendererFlags{}", {
        RendererFlag::GlyphPositionsClusters
    });
}

namespace {

template<class Vertex> auto defaultGlyphAllocatorFor(const RendererFlags flags, const bool hasCustomVertexAllocator) -> void(*)(void*, UnsignedInt, Containers::StridedArrayView1D<Vector2>&, Containers::StridedArrayView1D<UnsignedInt>&, Containers::StridedArrayView1D<UnsignedInt>*, Containers::StridedArrayView1D<Vector2>&) {
    /* If glyph positions and clusters are meant to be preserved, or if a
       custom vertex allocator is used and thus shouldn't allocate the whole
       vertex data again just to store glyph data inside, use the default
       RendererCore allocator */
    /** @todo it will still result in IDs being allocated and then never used
        after, provide a custom allocator for this case as well */
    if(flags >= RendererFlag::GlyphPositionsClusters || hasCustomVertexAllocator)
        return nullptr;

    return [](void* const state, const UnsignedInt glyphCount, Containers::StridedArrayView1D<Vector2>& glyphPositions, Containers::StridedArrayView1D<UnsignedInt>& glyphIds, Containers::StridedArrayView1D<UnsignedInt>*, Containers::StridedArrayView1D<Vector2>& glyphAdvances) {
        Containers::Array<char>& vertexData = *static_cast<Containers::Array<char>*>(state);

        const std::size_t existingSize = glyphPositions.size();
        const std::size_t desiredByteSize = 4*(existingSize + glyphCount)*sizeof(Vertex);
        if(desiredByteSize > vertexData.size()) {
            /* Using arrayAppend() as it reallocates with a growth strategy,
               arrayResize() would take the size literally */
            arrayAppend(vertexData, NoInit, desiredByteSize - vertexData.size());
        }

        const Containers::StridedArrayView1D<Vertex> vertices = Containers::arrayCast<Vertex>(vertexData);
        /* As each glyph turns into four vertices, we have plenty of space to
           store everything. Glyph positions occupy the position of each first
           vertex, */
        glyphPositions = vertices.slice(&Vertex::position).every(4);
        /* glyph IDs the first four bytes of the texture coordinates of each
           first vertex, */
        glyphIds = Containers::arrayCast<UnsignedInt>(vertices.slice(&Vertex::textureCoordinates)).every(4);
        /* and advances the position of each *second* vertex from the
           yet-unused suffix. If we have no vertex data at all however, which
           can happen when calling clear() right after construction, don't
           slice away any prefix to avoid OOB access. */
        glyphAdvances = vertices.slice(&Vertex::position).exceptPrefix(
            vertexData.size() ? existingSize*4 + 1 : 0
        ).every(4);
    };
}

void defaultIndexAllocator(void* state, UnsignedInt size, Containers::ArrayView<char>& indices) {
    Containers::Array<char>& indexData = *static_cast<Containers::Array<char>*>(state);

    const std::size_t desiredByteSize = indices.size() + size;
    if(desiredByteSize > indexData.size()) {
        /* Using arrayAppend() as it reallocates with a growth strategy,
           arrayResize() would take the size literally */
        arrayAppend(indexData, NoInit, desiredByteSize - indexData.size());
    }

    indices = indexData;
}

template<class Vertex> auto defaultVertexAllocatorFor(const RendererFlags flags, const bool hasCustomGlyphAllocator) -> void(*)(void*, UnsignedInt, Containers::StridedArrayView1D<Vector2>&, Containers::StridedArrayView1D<Vector2>&) {
    /* If glyph positions and clusters are meant to be preserved, or if a
       custom glyph allocator is used so there's no data sharing between the
       two, vertices are in a separate allocation. The second branch part
       is explicitly verified in the indicesVertices(custom glyph allocator)
       test. */
    if(flags >= RendererFlag::GlyphPositionsClusters || hasCustomGlyphAllocator)
        return [](void* const state, const UnsignedInt vertexCount, Containers::StridedArrayView1D<Vector2>& vertexPositions, Containers::StridedArrayView1D<Vector2>& vertexTextureCoordinates) {
            Containers::Array<char>& vertexData = *static_cast<Containers::Array<char>*>(state);

            const std::size_t desiredByteSize = (vertexPositions.size() + vertexCount)*sizeof(Vertex);
            if(desiredByteSize > vertexData.size()) {
                /* Using arrayAppend() as it reallocates with a growth
                   strategy, arrayResize() would take the size literally */
                arrayAppend(vertexData, NoInit, desiredByteSize - vertexData.size());
            }

            const Containers::StridedArrayView1D<Vertex> vertices = Containers::arrayCast<Vertex>(vertexData);
            vertexPositions = vertices.slice(&Vertex::position);
            /* The texture coordinates are Vector3 for array glyph caches, the
               allocator wants just a two-component prefix with an assumption
               that the third component is there too. Can't use
               .slice(&Vector3::xy) because the type may be Vector2. */
            vertexTextureCoordinates = Containers::arrayCast<Vector2>(vertices.slice(&Vertex::textureCoordinates));
        };
    /* If not, vertices share the allocation with glyph properties, and since
       they're always allocated after, the size should be sufficient and it's
       just about redirecting the views to new memory */
    else
        return [](void* const state, const UnsignedInt
            #ifndef CORRADE_NO_ASSERT
            vertexCount
            #endif
            , Containers::StridedArrayView1D<Vector2>& vertexPositions, Containers::StridedArrayView1D<Vector2>& vertexTextureCoordinates)
        {
            Containers::Array<char>& vertexData = *static_cast<Containers::Array<char>*>(state);

            /* As both the glyph allocator and vertex allocator share the same
               array, the assumption is that the glyph allocator already
               enlarged the array for all needed glyphs. Or this allocator is
               called from clear() with zero vertex count, in which case the
               array size can be whatever. */
            CORRADE_INTERNAL_ASSERT((vertexPositions.size() + vertexCount)*sizeof(Vertex) == vertexData.size() || vertexCount == 0);

            const Containers::StridedArrayView1D<Vertex> vertices = Containers::arrayCast<Vertex>(vertexData);
            vertexPositions = vertices.slice(&Vertex::position);
            /* The texture coordinates are Vector3 for array glyph caches, the
               allocator wants just a two-component prefix with an assumption
               that the third component is there too. Can't use
               .slice(&Vector3::xy) because the type may be Vector2. */
            vertexTextureCoordinates = Containers::arrayCast<Vector2>(vertices.slice(&Vertex::textureCoordinates));
        };
}

}

Renderer::State::State(const AbstractGlyphCache& glyphCache, void(*glyphAllocator)(void*, UnsignedInt, Containers::StridedArrayView1D<Vector2>&, Containers::StridedArrayView1D<UnsignedInt>&, Containers::StridedArrayView1D<UnsignedInt>*, Containers::StridedArrayView1D<Vector2>&), void* glyphAllocatorState, void(*const runAllocator)(void*, UnsignedInt, Containers::StridedArrayView1D<Float>&, Containers::StridedArrayView1D<UnsignedInt>&), void* runAllocatorState, void(*indexAllocator)(void*, UnsignedInt, Containers::ArrayView<char>&), void* indexAllocatorState, void(*vertexAllocator)(void*, UnsignedInt, Containers::StridedArrayView1D<Vector2>&, Containers::StridedArrayView1D<Vector2>&), void* vertexAllocatorState, RendererFlags flags):
    RendererCore::AllocatorState{glyphCache,
        glyphAllocator ? glyphAllocator :
            glyphCache.size().z() == 1 ?
                defaultGlyphAllocatorFor<Implementation::Vertex>(flags, !!vertexAllocator) :
                defaultGlyphAllocatorFor<Implementation::VertexArray>(flags, !!vertexAllocator),
        /* The defaultGlyphAllocatorFor() puts glyph data into the same
           allocation as vertex data so it's `&vertexData`, not `&glyphData`
           here. If such sharing isn't desired because the glyph data need to
           be accessible etc., defaultGlyphAllocatorFor() returns nullptr,
           which then causes `&vertexData` to be ignored and RendererCore then
           picks its own default allocator and `&glyphData`. */
        glyphAllocator ? glyphAllocatorState : &vertexData,
        runAllocator, runAllocatorState,
        RendererCoreFlags{UnsignedByte(flags)}},
    indexAllocator{indexAllocator ? indexAllocator : defaultIndexAllocator},
    indexAllocatorState{indexAllocator ? indexAllocatorState : &indexData},
    vertexAllocator{vertexAllocator ? vertexAllocator :
        glyphCache.size().z() == 1 ?
            defaultVertexAllocatorFor<Implementation::Vertex>(flags, !!glyphAllocator) :
            defaultVertexAllocatorFor<Implementation::VertexArray>(flags, !!glyphAllocator)},
    vertexAllocatorState{vertexAllocator ? vertexAllocatorState : &vertexData} {}

Renderer::Renderer(const AbstractGlyphCache& glyphCache, void(*glyphAllocator)(void*, UnsignedInt, Containers::StridedArrayView1D<Vector2>&, Containers::StridedArrayView1D<UnsignedInt>&, Containers::StridedArrayView1D<UnsignedInt>*, Containers::StridedArrayView1D<Vector2>&), void* glyphAllocatorState, void(*runAllocator)(void*, UnsignedInt, Containers::StridedArrayView1D<Float>&, Containers::StridedArrayView1D<UnsignedInt>&), void* runAllocatorState, void(*indexAllocator)(void*, UnsignedInt, Containers::ArrayView<char>&), void* indexAllocatorState, void(*vertexAllocator)(void*, UnsignedInt, Containers::StridedArrayView1D<Vector2>&, Containers::StridedArrayView1D<Vector2>&), void* vertexAllocatorState, RendererFlags flags): RendererCore{Containers::pointer<State>(glyphCache, glyphAllocator, glyphAllocatorState, runAllocator, runAllocatorState, indexAllocator, indexAllocatorState, vertexAllocator, vertexAllocatorState, flags)} {}

Renderer::Renderer(Containers::Pointer<State>&& state): RendererCore{Utility::move(state)} {}

Renderer::Renderer(Renderer&&) noexcept = default;

Renderer::~Renderer() = default;

Renderer& Renderer::operator=(Renderer&&) noexcept = default;

RendererFlags Renderer::flags() const {
    /* Subclasses inherit and add their own flags, mask them away */
    return RendererFlags{UnsignedByte(_state->flags)} & RendererFlags{0x1};
}

namespace {
    /* Like meshIndexTypeSize() but inline, constexpr, branchless and without
       assertions */
    constexpr UnsignedInt indexTypeSize(MeshIndexType type) {
        return 1 << (int(type) - 1);
    }
    static_assert(
        indexTypeSize(MeshIndexType::UnsignedByte) == sizeof(UnsignedByte) &&
        indexTypeSize(MeshIndexType::UnsignedShort) == sizeof(UnsignedShort) &&
        indexTypeSize(MeshIndexType::UnsignedInt) == sizeof(UnsignedInt),
        "broken assumptions about MeshIndexType values matching type sizes");
}

UnsignedInt Renderer::glyphIndexCapacity() const {
    const State& state = static_cast<const State&>(*_state);
    CORRADE_INTERNAL_DEBUG_ASSERT(state.indices.size() % 6 == 0);
    return state.indices.size()/(6*indexTypeSize(state.indexType));
}

UnsignedInt Renderer::glyphVertexCapacity() const {
    const State& state = static_cast<const State&>(*_state);
    CORRADE_INTERNAL_DEBUG_ASSERT(state.vertexPositions.size() % 4 == 0);
    return state.vertexPositions.size()/4;
}

MeshIndexType Renderer::indexType() const {
    return static_cast<const State&>(*_state).indexType;
}

namespace {
    /* used by setIndexType() and allocateIndices() */
    MeshIndexType indexTypeFor(MeshIndexType minType, UnsignedInt glyphCount) {
        MeshIndexType minTypeForGlyphCount;
        if(glyphCount > 16384)
            minTypeForGlyphCount = MeshIndexType::UnsignedInt;
        else if(glyphCount > 64)
            minTypeForGlyphCount = MeshIndexType::UnsignedShort;
        else
            minTypeForGlyphCount = MeshIndexType::UnsignedByte;
        return Utility::max(minType, minTypeForGlyphCount);
    }
}

Renderer& Renderer::setIndexType(const MeshIndexType type) {
    State& state = static_cast<State&>(*_state);
    CORRADE_ASSERT(!state.rendering,
        "Text::Renderer::setIndexType(): rendering in progress", *this);

    /* Remember the type as the smallest index type we can use going forward */
    state.minIndexType = type;

    /* If the capacity is zero, just update the currently used index type
       without calling an allocator */
    if(state.glyphPositions.isEmpty()) {
        state.indexType = type;

    /* Otherwise, if the index type for current capacity is now different from
       what's currently used, reallocate the indices fully */
    } else if(indexTypeFor(type, state.glyphPositions.size()) != state.indexType) {
        /* In particular, the allocator gets a zero-sized prefix of the view
           it returned last time (*not* just nullptr), to hint that it can
           reallocate without preserving any contents at all */
        state.indices = state.indices.prefix(0);
        allocateIndices(
            #ifndef CORRADE_NO_ASSERT
            "Text::Renderer::setIndexType():",
            #endif
            state.glyphPositions.size()
        );
    }

    return *this;
}

Containers::StridedArrayView1D<const Vector2> Renderer::glyphPositions() const {
    const State& state = static_cast<const State&>(*_state);
    CORRADE_ASSERT(RendererFlags(UnsignedByte(state.flags)) >= RendererFlag::GlyphPositionsClusters,
        "Text::Renderer::glyphPositions(): glyph positions and clusters not enabled", {});
    return state.glyphPositions.prefix(state.glyphCount);
}

Containers::StridedArrayView1D<const UnsignedInt> Renderer::glyphClusters() const {
    const State& state = static_cast<const State&>(*_state);
    CORRADE_ASSERT(RendererFlags(UnsignedByte(state.flags)) >= RendererFlag::GlyphPositionsClusters,
        "Text::Renderer::glyphClusters(): glyph positions and clusters not enabled", {});
    return state.glyphClusters.prefix(state.glyphCount);
}

Containers::StridedArrayView2D<const char> Renderer::indices() const {
    const State& state = static_cast<const State&>(*_state);
    const UnsignedInt typeSize = indexTypeSize(state.indexType);
    return stridedArrayView(state.indices.prefix(state.glyphCount*6*typeSize)).expanded<0, 2>({state.glyphCount*6, typeSize});
}

/* On Windows (MSVC, clang-cl and MinGw) these need an explicit export
   otherwise the specializations don't get exported */
template<> MAGNUM_TEXT_EXPORT Containers::ArrayView<const UnsignedByte> Renderer::indices<UnsignedByte>() const {
    const State& state = static_cast<const State&>(*_state);
    CORRADE_ASSERT(state.indexType == MeshIndexType::UnsignedByte,
        "Text::Renderer::indices(): cannot retrieve" << state.indexType << "as an UnsignedByte", {});
    return Containers::arrayCast<UnsignedByte>(state.indices).prefix(state.glyphCount*6);
}

template<> MAGNUM_TEXT_EXPORT Containers::ArrayView<const UnsignedShort> Renderer::indices<UnsignedShort>() const {
    const State& state = static_cast<const State&>(*_state);
    CORRADE_ASSERT(state.indexType == MeshIndexType::UnsignedShort,
        "Text::Renderer::indices(): cannot retrieve" << state.indexType << "as an UnsignedShort", {});
    return Containers::arrayCast<UnsignedShort>(state.indices).prefix(state.glyphCount*6);
}

template<> MAGNUM_TEXT_EXPORT Containers::ArrayView<const UnsignedInt> Renderer::indices<UnsignedInt>() const {
    const State& state = static_cast<const State&>(*_state);
    CORRADE_ASSERT(state.indexType == MeshIndexType::UnsignedInt,
        "Text::Renderer::indices(): cannot retrieve" << state.indexType << "as an UnsignedInt", {});
    return Containers::arrayCast<UnsignedInt>(state.indices).prefix(state.glyphCount*6);
}

Containers::StridedArrayView1D<const Vector2> Renderer::vertexPositions() const {
    const State& state = static_cast<const State&>(*_state);
    return state.vertexPositions.prefix(state.glyphCount*4);
}

Containers::StridedArrayView1D<const Vector2> Renderer::vertexTextureCoordinates() const {
    const State& state = static_cast<const State&>(*_state);
    CORRADE_ASSERT(state.glyphCache.size().z() == 1,
        "Text::Renderer::vertexTextureCoordinates(): cannot retrieve two-dimensional coordinates with an array glyph cache", {});
    return state.vertexTextureCoordinates.prefix(state.glyphCount*4);
}

Containers::StridedArrayView1D<const Vector3> Renderer::vertexTextureArrayCoordinates() const {
    const State& state = static_cast<const State&>(*_state);
    CORRADE_ASSERT(state.glyphCache.size().z() != 1,
        "Text::Renderer::vertexTextureArrayCoordinates(): cannot retrieve three-dimensional coordinates with a non-array glyph cache", {});
    return Containers::arrayCast<Vector3>(state.vertexTextureCoordinates.prefix(state.glyphCount*4));
}

void Renderer::allocateIndices(
    #ifndef CORRADE_NO_ASSERT
    const char* const messagePrefix,
    #endif
    const UnsignedInt totalGlyphCount)
{
    State& state = static_cast<State&>(*_state);

    /* The data allocated by RendererCore should already be at this size or
       more, since allocateGlyphs() is always called before this function. */
    CORRADE_INTERNAL_ASSERT(state.glyphPositions.size() >= totalGlyphCount);

    /* This function should only be called if we need more memory, from clear()
       with everything empty or from setIndexType() if the type changes (where
       it sets `state.indices` to an empty prefix).

       The expectation is that `state.indices` is only as large as makes sense
       for given `state.indexType`, as is done below. */
    CORRADE_INTERNAL_DEBUG_ASSERT(6*totalGlyphCount*indexTypeSize(state.indexType) > state.indices.size() || (state.glyphCount == 0 && state.renderingGlyphCount == 0 && totalGlyphCount == 0));

    /* Figure out index type needed for this glyph count. If it's different or
       we're called from clear() with totalGlyphCount being 0, we're replacing
       the whole index array. If it's not, we're generating just the extra
       indices. */
    const MeshIndexType indexType = indexTypeFor(state.minIndexType, totalGlyphCount);
    const UnsignedInt typeSize = indexTypeSize(indexType);
    UnsignedInt previousFilledSize;
    if(indexType != state.indexType || totalGlyphCount == 0) {
        previousFilledSize = 0;
        state.indexType = indexType;
    } else {
        previousFilledSize = state.indices.size();
    }

    /* Sliced copy of the view for the allocator to update */
    Containers::ArrayView<char> indices = state.indices.prefix(previousFilledSize);

    /* While this function gets total glyph count, the allocator gets byte
       count to grow by */
    state.indexAllocator(state.indexAllocatorState,
        totalGlyphCount*6*typeSize - previousFilledSize,
        indices);

    /* Cap the returned capacity to just what's possible to represent with
       given type size. E.g., for an 8-bit type it can represent indices only
       for 256 vertices / 64 glyphs at most, which is 384 indices, thus is
       never larger than 384 bytes. */
    const UnsignedInt glyphCapacity = Math::min(
        /* 64 for 1-byte indices, 16k for 2-byte, 1M for 4-byte */
        1u << (8*typeSize - 2),
        UnsignedInt(indices.size()/(6*typeSize)));

    /* These assertions are present even for the builtin allocator but
       shouldn't fire. If they do, the whole thing is broken, but it's better
       to blow up with a nice message than with some strange OOB error later */
    CORRADE_ASSERT(glyphCapacity >= totalGlyphCount,
        messagePrefix << "expected allocated indices to have at least" << totalGlyphCount*6*typeSize << "bytes but got" << indices.size(), );

    state.indices = indices.prefix(glyphCapacity*6*typeSize);

    /* Fill the indices during allocation already as they're not dependent on
       the contents in any way */
    const UnsignedInt glyphOffset = previousFilledSize/(6*typeSize);
    const Containers::ArrayView<char> indicesToFill = state.indices.exceptPrefix(previousFilledSize);
    if(indexType == MeshIndexType::UnsignedByte)
        renderGlyphQuadIndicesInto(glyphOffset, Containers::arrayCast<UnsignedByte>(indicesToFill));
    else if(indexType == MeshIndexType::UnsignedShort)
        renderGlyphQuadIndicesInto(glyphOffset, Containers::arrayCast<UnsignedShort>(indicesToFill));
    else if(indexType == MeshIndexType::UnsignedInt)
        renderGlyphQuadIndicesInto(glyphOffset, Containers::arrayCast<UnsignedInt>(indicesToFill));
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

void Renderer::allocateVertices(
    #ifndef CORRADE_NO_ASSERT
    const char* const messagePrefix,
    #endif
    const UnsignedInt totalGlyphCount)
{
    State& state = static_cast<State&>(*_state);

    /* The data allocated by RendererCore should already be at this size or
       more, since allocateGlyphs() is always called before this function */
    CORRADE_INTERNAL_ASSERT(state.glyphPositions.size() >= totalGlyphCount);

    /* This function should only be called if we need more memory or from
       clear() with everything empty */
    CORRADE_INTERNAL_DEBUG_ASSERT(4*totalGlyphCount > state.vertexPositions.size() || (state.glyphCount == 0 && totalGlyphCount == 0));

    /* Sliced copies of the views for the allocator to update. Unlike with
       allocateGlyphs(), where `state.renderingGlyphCount` is used because it
       gets called from add(), this is called with `state.glyphCount` because
       it's only called from render(), and so the vertex capacity may not yet
       include space for the in-progress glyphs. */
    Containers::StridedArrayView1D<Vector2> vertexPositions =
        state.vertexPositions.prefix(state.glyphCount*4);
    Containers::StridedArrayView1D<Vector2> vertexTextureCoordinates =
        state.vertexTextureCoordinates.prefix(state.glyphCount*4);

    /* While this function gets total glyph count, the allocator gets vertex
       count to grow by instead */
    state.vertexAllocator(state.vertexAllocatorState, (totalGlyphCount - state.glyphCount)*4,
        vertexPositions,
        vertexTextureCoordinates);
    /* Take the smallest size of both as the new vertex capacity */
    const std::size_t minGlyphCapacity = Math::min({
        vertexPositions.size()/4,
        vertexTextureCoordinates.size()/4});
    /* These assertions are present even for the builtin allocator but
       shouldn't fire. If they do, the whole thing is broken, but it's better
       to blow up with a nice message than with some strange OOB error later */
    CORRADE_ASSERT(minGlyphCapacity >= totalGlyphCount,
        messagePrefix << "expected allocated vertex positions and texture coordinates to have at least" << totalGlyphCount*4 << "elements but got" << vertexPositions.size() << "and" << vertexTextureCoordinates.size(), );
    CORRADE_ASSERT(state.glyphCache.size().z() == 1 || std::size_t(Math::abs(vertexTextureCoordinates.stride())) >= sizeof(Vector3),
        messagePrefix << "expected allocated texture coordinates to have a stride large enough to fit a Vector3 but got only" << Math::abs(vertexTextureCoordinates.stride()) << "bytes", );

    /* Keep just the minimal size for both, which is the new capacity */
    state.vertexPositions = vertexPositions.prefix(minGlyphCapacity *4);
    state.vertexTextureCoordinates = vertexTextureCoordinates.prefix(minGlyphCapacity *4);
}

Renderer& Renderer::clear() {
    RendererCore::clear();

    /* Not calling allocateIndices() with 0 because it makes no sense to
       regenerate the index buffer to the exact same contents on every clear */
    allocateVertices(
        #ifndef CORRADE_NO_ASSERT
        "", /* Asserts won't happen as returned sizes will be always >= 0 */
        #endif
        0);

    return *this;
}

Renderer& Renderer::reset() {
    /* Compared to RendererCore::reset() this calls our clear() instead of
       RendererCore::clear() */
    clear();
    resetInternal();
    return *this;
}

Renderer& Renderer::reserve(const UnsignedInt glyphCapacity, const UnsignedInt runCapacity) {
    State& state = static_cast<State&>(*_state);

    /* Reserve glyph and run capacity. It's possible that there's already
       enough glyph/run capacity but the index/vertex capacity not yet because
       glyphs/runs get allocated during add() already and index/vertex only
       during the final render(). */
    RendererCore::reserve(glyphCapacity, runCapacity);

    /* Reserve (and fill) indices if there's too little of them for the
       required glyph capacity. Done separately from vertex allocation because
       each of the allocations can have a different growth pattern and the
       index type can change during the renderer lifetime.

       The expectation is that `state.indices` is only as large as makes sense
       for given `state.indexType` (e.g., for an 8-bit type it can represent
       indices only for 256 vertices / 64 glyphs at most, which is 384 indices,
       thus is never larger than 384 bytes). */
    if(state.indices.size() < glyphCapacity*6*indexTypeSize(state.indexType))
        allocateIndices(
            #ifndef CORRADE_NO_ASSERT
            "Text::Renderer::reserve():",
            #endif
            glyphCapacity);

    /* Reserve vertices if there's too little of them for the required glyph
       capacity */
    if(state.vertexPositions.size() < glyphCapacity*4)
        allocateVertices(
            #ifndef CORRADE_NO_ASSERT
            "Text::Renderer::reserve():",
            #endif
            glyphCapacity);

    return *this;
}

Containers::Pair<Range2D, Range1Dui> Renderer::render() {
    State& state = static_cast<State&>(*_state);

    /* If we need to generate more indices / vertices than what's in the
       capacity, allocate more. The logic is the same as in reserve(), see
       there for more information.

       This has to be called before RendererCore::render() in order to know
       which glyphs have only positions + IDs (state.renderingGlyphCount) and
       which have also index and vertex data (state.glyphCount). The
       RendererCore::render() then makes both values the same. */
    if(state.indices.size() < state.renderingGlyphCount *6*indexTypeSize(state.indexType))
        allocateIndices(
            #ifndef CORRADE_NO_ASSERT
            "Text::Renderer::render():",
            #endif
            state.renderingGlyphCount);
    if(state.vertexPositions.size() < state.renderingGlyphCount *4)
        allocateVertices(
            #ifndef CORRADE_NO_ASSERT
            "Text::Renderer::render():",
            #endif
            state.renderingGlyphCount);
    #ifdef CORRADE_GRACEFUL_ASSERT
    /* For testing only -- if vertex allocation failed, bail. Indices are only
       touched in allocateIndices(), so if allocateIndices() fails we don't
       need to exit here. */
    if(state.vertexPositions.size() < state.renderingGlyphCount *4)
        return {};
    #endif

    /* Finish rendering of glyph positions and IDs */
    const bool isArray = state.glyphCache.size().z() > 1;
    const Containers::Pair<Range2D, Range1Dui> out = RendererCore::render();

    /* Populate vertex data for all runs */
    UnsignedInt glyphBegin = out.second().min() ? state.runEnds[out.second().min() - 1] : 0;
    for(UnsignedInt run = out.second().min(), runEnd = out.second().max(); run != runEnd; ++run) {
        const UnsignedInt glyphEnd = state.runEnds[run];

        const Containers::StridedArrayView1D<const Vector2> glyphPositions = state.glyphPositions.slice(glyphBegin, glyphEnd);
        const Containers::StridedArrayView1D<const UnsignedInt> glyphIds = state.glyphIds.slice(glyphBegin, glyphEnd);
        const Containers::StridedArrayView1D<Vector2> vertexPositions = state.vertexPositions.slice(4*glyphBegin, 4*glyphEnd);
        const Containers::StridedArrayView1D<Vector2> vertexTextureCoordinates = state.vertexTextureCoordinates.slice(4*glyphBegin, 4*glyphEnd);
        if(!isArray) renderGlyphQuadsInto(state.glyphCache,
                state.runScales[run],
                glyphPositions,
                glyphIds,
                vertexPositions,
                vertexTextureCoordinates);
        else renderGlyphQuadsInto(state.glyphCache,
                state.runScales[run],
                glyphPositions,
                glyphIds,
                vertexPositions,
                Containers::arrayCast<Vector3>(vertexTextureCoordinates));

        glyphBegin = glyphEnd;
    }

    return out;
}

Renderer& Renderer::add(AbstractShaper& shaper, const Float size, const Containers::StringView text, const UnsignedInt begin, const UnsignedInt end, const Containers::ArrayView<const FeatureRange> features) {
    return static_cast<Renderer&>(RendererCore::add(shaper, size, text, begin, end, features));
}

Renderer& Renderer::add(AbstractShaper& shaper, const Float size, const Containers::StringView text, const UnsignedInt begin, const UnsignedInt end) {
    return static_cast<Renderer&>(RendererCore::add(shaper, size, text, begin, end));
}

Renderer& Renderer::add(AbstractShaper& shaper, const Float size, const Containers::StringView text, const UnsignedInt begin, const UnsignedInt end, const std::initializer_list<FeatureRange> features) {
    return static_cast<Renderer&>(RendererCore::add(shaper, size, text, begin, end, features));
}

Renderer& Renderer::add(AbstractShaper& shaper, const Float size, const Containers::StringView text, const Containers::ArrayView<const FeatureRange> features) {
    return static_cast<Renderer&>(RendererCore::add(shaper, size, text, features));
}

Renderer& Renderer::add(AbstractShaper& shaper, const Float size, const Containers::StringView text) {
    return static_cast<Renderer&>(RendererCore::add(shaper, size, text));
}

Renderer& Renderer::add(AbstractShaper& shaper, const Float size, const Containers::StringView text, const std::initializer_list<FeatureRange> features) {
    return static_cast<Renderer&>(RendererCore::add(shaper, size, text, features));
}

Containers::Pair<Range2D, Range1Dui> Renderer::render(AbstractShaper& shaper, const Float size, const Containers::StringView text, const Containers::ArrayView<const FeatureRange> features) {
    /* Compared to RendererCore::render() this calls our render() instead of
       RendererCore::render() */
    add(shaper, size, text, features);
    return render();
}

Containers::Pair<Range2D, Range1Dui> Renderer::render(AbstractShaper& shaper, const Float size, const Containers::StringView text) {
    return render(shaper, size, text, {});
}

Containers::Pair<Range2D, Range1Dui> Renderer::render(AbstractShaper& shaper, const Float size, const Containers::StringView text, const std::initializer_list<FeatureRange> features) {
    return render(shaper, size, text, Containers::arrayView(features));
}

Range2D renderLineGlyphPositionsInto(const AbstractFont& font, const Float size, const LayoutDirection direction, const Containers::StridedArrayView1D<const Vector2>& glyphOffsets, const Containers::StridedArrayView1D<const Vector2>& glyphAdvances, Vector2& cursor, const Containers::StridedArrayView1D<Vector2>& glyphPositions) {
    CORRADE_ASSERT(glyphAdvances.size() == glyphOffsets.size() &&
                   glyphPositions.size() == glyphOffsets.size(),
        "Text::renderLineGlyphPositionsInto(): expected glyphOffsets, glyphAdvances and output views to have the same size, got" << glyphOffsets.size() << Debug::nospace << "," << glyphAdvances.size() << "and" << glyphPositions.size(), {});
    CORRADE_ASSERT(direction == LayoutDirection::HorizontalTopToBottom,
        "Text::renderLineGlyphPositionsInto(): only" << LayoutDirection::HorizontalTopToBottom << "is supported right now, got" << direction, {});
    #ifdef CORRADE_NO_ASSERT
    static_cast<void>(direction); /** @todo drop once implemented */
    #endif

    CORRADE_ASSERT(font.isOpened(),
        "Text::renderLineGlyphPositionsInto(): no font opened", {});
    const Float scale = size/font.size();

    /* Combine the offsets and cursor advances and calculate the line rectangle
       along the way. Initially the cursor is at origin and rectangle is empty,
       with just the Y bounds from font metrics. */
    /** @todo this needs an update for vertical text */
    Range2D rectangle{cursor + Vector2::yAxis(font.descent()*scale),
                      cursor + Vector2::yAxis(font.ascent()*scale)};
    for(UnsignedInt i = 0; i != glyphOffsets.size(); ++i) {
        /* The glyphOffsets and output are allowed to be aliased, so make sure
           the value isn't stomped on when writing the output */
        glyphPositions[i] = cursor + glyphOffsets[i]*scale;
        cursor += glyphAdvances[i]*scale;

        /* Extend the line rectangle with the cursor range */
        /** @todo this assumes left-to-right / top-to-bottom ordering of
            shaped glyphs (which HarfBuzz does for both directions however),
            add some checks to verify that assumption */
        rectangle.max() = Math::max(rectangle.max(), cursor);
    }

    return rectangle;
}

namespace {

Range2D renderGlyphQuadsInto(const AbstractGlyphCache& cache, const Float scale, const Containers::StridedArrayView1D<const Vector2>& glyphPositions, const Containers::StridedArrayView1D<const UnsignedInt>& glyphIds, const Containers::StridedArrayView1D<Vector2>& vertexPositions, const Containers::StridedArrayView1D<Vector2>& vertexTextureCoordinates, const Containers::StridedArrayView1D<Float>& vertexTextureLayers) {
    CORRADE_ASSERT(glyphIds.size() == glyphPositions.size(),
        "Text::renderGlyphQuadsInto(): expected glyphIds and glyphPositions views to have the same size, got" << glyphIds.size() << "and" << glyphPositions.size(), {});
    CORRADE_ASSERT(vertexPositions.size() == glyphPositions.size()*4 &&
                   vertexTextureCoordinates.size() == glyphPositions.size()*4,
        "Text::renderGlyphQuadsInto(): expected vertexPositions and vertexTextureCoordinates views to have" << glyphPositions.size()*4 << "elements, got" << vertexPositions.size() << "and" << vertexTextureCoordinates.size(), {});
    /* Should be ensured by the callers below */
    CORRADE_INTERNAL_ASSERT(!vertexTextureLayers || vertexTextureLayers.size() == vertexTextureCoordinates.size());

    /* Direct views on the cache data */
    const Vector2 inverseCacheSize = 1.0f/Vector2{cache.size().xy()};
    const Containers::StridedArrayView1D<const Vector2i> cacheGlyphOffsets = cache.glyphOffsets();
    const Containers::StridedArrayView1D<const Int> cacheGlyphLayers = cache.glyphLayers();
    const Containers::StridedArrayView1D<const Range2Di> cacheGlyphRectangles = cache.glyphRectangles();

    /* Create quads for each glyph and calculate the glyph bound rectangle
       along the way. */
    Range2D rectangle;
    for(std::size_t i = 0; i != glyphIds.size(); ++i) {
        /* 2---3
           |   |
           |   |
           |   |
           0---1 */
        const UnsignedInt glyphId = glyphIds[i];
        const Range2D quad = Range2D::fromSize(
            glyphPositions[i] + Vector2{cacheGlyphOffsets[glyphId]}*scale,
            Vector2{cacheGlyphRectangles[glyphId].size()}*scale);
        const Range2D texture = Range2D{cacheGlyphRectangles[glyphId]}
            .scaled(inverseCacheSize);
        const std::size_t i4 = i*4;
        for(UnsignedByte j = 0; j != 4; ++j) {
            /* ✨ */
            vertexPositions[i4 + j] = Math::lerp(quad.min(), quad.max(), BitVector2{j});
            vertexTextureCoordinates[i4 + j] = Math::lerp(texture.min(), texture.max(), BitVector2{j});
        }

        /* Fill also a texture layer if desirable. For 2D output the caller
           already checked that the cache is 2D. */
        if(vertexTextureLayers) for(std::size_t j = 0; j != 4; ++j)
            vertexTextureLayers[i4 + j] = cacheGlyphLayers[glyphId];

        /* Extend the rectangle with current glyph bounds */
        rectangle = Math::join(rectangle, quad);
    }

    return rectangle;
}

Range2D renderGlyphQuadsInto(const AbstractFont& font, const Float size, const AbstractGlyphCache& cache, const Containers::StridedArrayView1D<const Vector2>& glyphPositions, const Containers::StridedArrayView1D<const UnsignedInt>& fontGlyphIds, const Containers::StridedArrayView1D<Vector2>& vertexPositions, const Containers::StridedArrayView1D<Vector2>& vertexTextureCoordinates, const Containers::StridedArrayView1D<Float>& vertexTextureLayers) {
    CORRADE_ASSERT(font.isOpened(),
        "Text::renderGlyphQuadsInto(): no font opened", {});

    const Containers::Optional<UnsignedInt> fontId = cache.findFont(font);
    CORRADE_ASSERT(fontId,
        "Text::renderGlyphQuadsInto(): font not found among" << cache.fontCount() << "fonts in passed glyph cache", {});

    /* First map the font-local glyph IDs to cache-global, abusing the texture
       coordinate output array as the storage. Not vertex positions, as those
       are allowed to be aliased with glyphPositions by the caller and this
       process would overwrite them.

       This also means we need to duplicate the size assertions here, to avoid
       asserting inside glyphIdsInto() instead and confusing the user. */
    CORRADE_ASSERT(fontGlyphIds.size() == glyphPositions.size(),
        "Text::renderGlyphQuadsInto(): expected fontGlyphIds and glyphPositions views to have the same size, got" << fontGlyphIds.size() << "and" << glyphPositions.size(), {});
    CORRADE_ASSERT(vertexPositions.size() == glyphPositions.size()*4 &&
                   vertexTextureCoordinates.size() == glyphPositions.size()*4,
        "Text::renderGlyphQuadsInto(): expected vertexPositions and vertexTextureCoordinates views to have" << glyphPositions.size()*4 << "elements, got" << vertexPositions.size() << "and" << vertexTextureCoordinates.size(), {});
    const Containers::StridedArrayView1D<UnsignedInt> glyphIds = Containers::arrayCast<UnsignedInt>(vertexTextureCoordinates.every(4));
    cache.glyphIdsInto(*fontId, fontGlyphIds, glyphIds);

    /* Delegate to the above */
    return renderGlyphQuadsInto(cache, size/font.size(), glyphPositions, glyphIds, vertexPositions, vertexTextureCoordinates, vertexTextureLayers);
}

}

Range2D renderGlyphQuadsInto(const AbstractFont& font, const Float size, const AbstractGlyphCache& cache, const Containers::StridedArrayView1D<const Vector2>& glyphPositions, const Containers::StridedArrayView1D<const UnsignedInt>& glyphIds, const Containers::StridedArrayView1D<Vector2>& vertexPositions, const Containers::StridedArrayView1D<Vector3>& vertexTextureCoordinates) {
    return renderGlyphQuadsInto(font, size, cache, glyphPositions, glyphIds, vertexPositions, vertexTextureCoordinates.slice(&Vector3::xy), vertexTextureCoordinates.slice(&Vector3::z));
}

Range2D renderGlyphQuadsInto(const AbstractFont& font, const Float size, const AbstractGlyphCache& cache, const Containers::StridedArrayView1D<const Vector2>& glyphPositions, const Containers::StridedArrayView1D<const UnsignedInt>& glyphIds, const Containers::StridedArrayView1D<Vector2>& vertexPositions, const Containers::StridedArrayView1D<Vector2>& vertexTextureCoordinates) {
    CORRADE_ASSERT(cache.size().z() == 1,
        "Text::renderGlyphQuadsInto(): can't use this overload with an array glyph cache", {});
    return renderGlyphQuadsInto(font, size, cache, glyphPositions, glyphIds, vertexPositions, vertexTextureCoordinates, nullptr);
}

Range2D renderGlyphQuadsInto(const AbstractGlyphCache& cache, const Float scale, const Containers::StridedArrayView1D<const Vector2>& glyphPositions, const Containers::StridedArrayView1D<const UnsignedInt>& glyphIds, const Containers::StridedArrayView1D<Vector2>& vertexPositions, const Containers::StridedArrayView1D<Vector3>& vertexTextureCoordinates) {
    return renderGlyphQuadsInto(cache, scale, glyphPositions, glyphIds, vertexPositions, vertexTextureCoordinates.slice(&Vector3::xy), vertexTextureCoordinates.slice(&Vector3::z));
}

Range2D renderGlyphQuadsInto(const AbstractGlyphCache& cache, const Float scale, const Containers::StridedArrayView1D<const Vector2>& glyphPositions, const Containers::StridedArrayView1D<const UnsignedInt>& glyphIds, const Containers::StridedArrayView1D<Vector2>& vertexPositions, const Containers::StridedArrayView1D<Vector2>& vertexTextureCoordinates) {
    CORRADE_ASSERT(cache.size().z() == 1,
        "Text::renderGlyphQuadsInto(): can't use this overload with an array glyph cache", {});
    return renderGlyphQuadsInto(cache, scale, glyphPositions, glyphIds, vertexPositions, vertexTextureCoordinates, nullptr);
}

Range2D glyphQuadBounds(const AbstractGlyphCache& cache, const Float scale, const Containers::StridedArrayView1D<const Vector2>& glyphPositions, const Containers::StridedArrayView1D<const UnsignedInt>& glyphIds) {
    CORRADE_ASSERT(glyphIds.size() == glyphPositions.size(),
        "Text::glyphQuadBounds(): expected glyphIds and glyphPositions views to have the same size, got" << glyphIds.size() << "and" << glyphPositions.size(), {});

    /* Direct views on the cache data */
    const Containers::StridedArrayView1D<const Vector2i> cacheGlyphOffsets = cache.glyphOffsets();
    const Containers::StridedArrayView1D<const Range2Di> cacheGlyphRectangles = cache.glyphRectangles();

    /* Basically what renderGlyphQuadsInto() does above, but producing just the
       rectangle */
    Range2D rectangle;
    for(std::size_t i = 0; i != glyphIds.size(); ++i) {
        const UnsignedInt glyphId = glyphIds[i];
        const Range2D quad = Range2D::fromSize(
            glyphPositions[i] + Vector2{cacheGlyphOffsets[glyphId]}*scale,
            Vector2{cacheGlyphRectangles[glyphId].size()}*scale);
        rectangle = Math::join(rectangle, quad);
    }

    return rectangle;
}

Range2D alignRenderedLine(const Range2D& lineRectangle, const LayoutDirection direction, const Alignment alignment, const Containers::StridedArrayView1D<Vector2>& positions) {
    CORRADE_ASSERT(direction == LayoutDirection::HorizontalTopToBottom,
        "Text::alignRenderedLine(): only" << LayoutDirection::HorizontalTopToBottom << "is supported right now, got" << direction, {});
    CORRADE_ASSERT(
        (UnsignedByte(alignment) & Implementation::AlignmentHorizontal) != Implementation::AlignmentBegin &&
        (UnsignedByte(alignment) & Implementation::AlignmentHorizontal) != Implementation::AlignmentEnd,
        "Text::alignRenderedLine():" << alignment << "has to be resolved to *Left / *Right before being passed to this function", {});
    #ifdef CORRADE_NO_ASSERT
    static_cast<void>(direction); /** @todo drop once implemented */
    #endif

    /** @todo this again assumes horizontal direction, needs to be updated once
        vertical (and possibly mixed horizontal/vertical) text is possible */

    Float alignmentOffsetX;
    if((UnsignedByte(alignment) & Implementation::AlignmentHorizontal) == Implementation::AlignmentLeft)
        alignmentOffsetX = -lineRectangle.left();
    else if((UnsignedByte(alignment) & Implementation::AlignmentHorizontal) == Implementation::AlignmentCenter) {
        alignmentOffsetX = -lineRectangle.centerX();
        /* Integer alignment */
        if(UnsignedByte(alignment) & Implementation::AlignmentIntegral)
            alignmentOffsetX = Math::round(alignmentOffsetX);
    }
    else if((UnsignedByte(alignment) & Implementation::AlignmentHorizontal) == Implementation::AlignmentRight)
        alignmentOffsetX = -lineRectangle.right();
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

    /* Shift all positions */
    for(Vector2& i: positions)
        i.x() += alignmentOffsetX;

    return lineRectangle.translated(Vector2::xAxis(alignmentOffsetX));
}

Range2D alignRenderedBlock(const Range2D& blockRectangle, const LayoutDirection direction, const Alignment alignment, const Containers::StridedArrayView1D<Vector2>& positions) {
    CORRADE_ASSERT(direction == LayoutDirection::HorizontalTopToBottom,
        "Text::alignRenderedBlock(): only" << LayoutDirection::HorizontalTopToBottom << "is supported right now, got" << direction, {});
    CORRADE_ASSERT(
        (UnsignedByte(alignment) & Implementation::AlignmentHorizontal) != Implementation::AlignmentBegin &&
        (UnsignedByte(alignment) & Implementation::AlignmentHorizontal) != Implementation::AlignmentEnd,
        "Text::alignRenderedBlock():" << alignment << "has to be resolved to *Left / *Right before being passed to this function", {});
    #ifdef CORRADE_NO_ASSERT
    static_cast<void>(direction); /** @todo drop once implemented */
    #endif

    /** @todo this assumes vertical layout advance, needs to be updated once
        other directions are possible */

    Float alignmentOffsetY;
    if((UnsignedByte(alignment) & Implementation::AlignmentVertical) == Implementation::AlignmentLine)
        alignmentOffsetY = 0.0f;
    else if((UnsignedByte(alignment) & Implementation::AlignmentVertical) == Implementation::AlignmentBottom)
        alignmentOffsetY = -blockRectangle.bottom();
    else if((UnsignedByte(alignment) & Implementation::AlignmentVertical) == Implementation::AlignmentMiddle) {
        alignmentOffsetY = -blockRectangle.centerY();
        /* Integer alignment */
        if(UnsignedByte(alignment) & Implementation::AlignmentIntegral)
            alignmentOffsetY = Math::round(alignmentOffsetY);
    }
    else if((UnsignedByte(alignment) & Implementation::AlignmentVertical) == Implementation::AlignmentTop)
        alignmentOffsetY = -blockRectangle.top();
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

    /* Shift all positions */
    for(Vector2& i: positions)
        i.y() += alignmentOffsetY;

    return blockRectangle.translated(Vector2::yAxis(alignmentOffsetY));
}

namespace {

template<class T> void renderGlyphQuadIndicesIntoInternal(const UnsignedInt glyphOffset, const Containers::StridedArrayView1D<T>& indices) {
    CORRADE_ASSERT(indices.size() % 6 == 0,
        "Text::renderGlyphQuadIndicesInto(): expected the indices view size to be divisible by 6, got" << indices.size(), );
    const UnsignedInt glyphCount = indices.size()/6;
    #ifndef CORRADE_NO_ASSERT
    const UnsignedLong maxValue = UnsignedLong(glyphOffset)*4 + UnsignedLong(glyphCount)*4;
    #endif
    CORRADE_ASSERT(maxValue <= (1ull << 8*sizeof(T)),
        "Text::renderGlyphQuadIndicesInto(): max index value of" << maxValue - 1 << "cannot fit into a" << 8*sizeof(T) << Debug::nospace << "-bit type", );

    for(UnsignedInt i = 0; i != glyphCount; ++i) {
        /* 2---3 2 3---5
           |   | |\ \  |
           |   | | \ \ |
           |   | |  \ \|
           0---1 0---1 4 */
        const UnsignedInt i4 = (glyphOffset + i)*4;
        const UnsignedInt i6 = i*6;
        indices[i6 + 0] = i4 + 0;
        indices[i6 + 1] = i4 + 1;
        indices[i6 + 2] = i4 + 2;
        indices[i6 + 3] = i4 + 2;
        indices[i6 + 4] = i4 + 1;
        indices[i6 + 5] = i4 + 3;
    }
}

}

void renderGlyphQuadIndicesInto(UnsignedInt glyphOffset, const Containers::StridedArrayView1D<UnsignedInt>& indices) {
    renderGlyphQuadIndicesIntoInternal(glyphOffset, indices);
}

void renderGlyphQuadIndicesInto(UnsignedInt glyphOffset, const Containers::StridedArrayView1D<UnsignedShort>& indices) {
    renderGlyphQuadIndicesIntoInternal(glyphOffset, indices);
}

void renderGlyphQuadIndicesInto(UnsignedInt glyphOffset, const Containers::StridedArrayView1D<UnsignedByte>& indices) {
    renderGlyphQuadIndicesIntoInternal(glyphOffset, indices);
}

Containers::Pair<UnsignedInt, UnsignedInt> glyphRangeForBytes(const Containers::StridedArrayView1D<const UnsignedInt>& clusters, const UnsignedInt begin, const UnsignedInt end) {
    if(clusters.isEmpty())
        return {};

    /* Make the begin always less than or equal to end */
    const bool reverseBeginEnd = begin > end;
    const UnsignedInt beginForward = reverseBeginEnd ? end : begin;
    const UnsignedInt endForward = reverseBeginEnd ? begin : end;

    /* Make the cluster array always in an ascending order as well */
    const bool reverseClusters = clusters.front() > clusters.back();
    const Containers::StridedArrayView1D<const UnsignedInt> clustersForward =
        reverseClusters ? clusters.flipped<0>() : clusters;

    /* The glyph begin is the last glyph that has the cluster ID not larger
       than `begin`, or the end */
    UnsignedInt glyphBegin = 0;
    while(glyphBegin != clustersForward.size() && clustersForward[glyphBegin] < beginForward && (glyphBegin + 1 == clustersForward.size() || clustersForward[glyphBegin + 1] <= beginForward))
        ++glyphBegin;

    /* If `begin` was pointing in the middle of a cluster, for example of a
       ligature, or (wrongly) inside a multi-byte UTF-8 char, go back to find
       the cluster begin */
    if(glyphBegin != clustersForward.size()) while(glyphBegin && clustersForward[glyphBegin - 1] == clustersForward[glyphBegin])
        --glyphBegin;

    /* The end is then the first glyph after glyph begin that has the cluster
       ID larger or equal to `end`. Unless `begin` was the same as `end`, then
       the returned glyph end is same as returned glyph begin. */
    UnsignedInt glyphEnd = glyphBegin;
    if(beginForward != endForward) while(glyphEnd != clustersForward.size() && clustersForward[glyphEnd] < endForward)
        ++glyphEnd;

    /* If the clusters were in reverse direction, reverse the actual glyph IDs
       as well. And this way the begin is greater or equal to end, so they're
       swapped too. */
    const Containers::Pair<UnsignedInt, UnsignedInt> out = reverseClusters ?
        Containers::pair(UnsignedInt(clustersForward.size()) - glyphEnd,
                         UnsignedInt(clustersForward.size()) - glyphBegin) :
        Containers::pair(glyphBegin, glyphEnd);

    /* Then, if the begin and end was swapped, swap the output again as well */
    return reverseBeginEnd ?
        Containers::pair(out.second(), out.first()) :
        out;
}

}}
