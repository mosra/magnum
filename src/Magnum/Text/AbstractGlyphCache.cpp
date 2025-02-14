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

#include "AbstractGlyphCache.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Triple.h>
#include <Corrade/Containers/StridedArrayView.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Range.h"
#include "Magnum/TextureTools/Atlas.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Containers/ArrayViewStl.h>
#include <Corrade/Utility/Algorithms.h>
#endif

namespace Magnum { namespace Text {

Debug& operator<<(Debug& debug, const GlyphCacheFeature value) {
    debug << "Text::GlyphCacheFeature" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case GlyphCacheFeature::v: return debug << "::" #v;
        _c(ImageProcessing)
        _c(ProcessedImageDownload)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << Debug::hex << UnsignedByte(value) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const GlyphCacheFeatures value) {
    return Containers::enumSetDebugOutput(debug, value, "Text::GlyphCacheFeatures{}", {
        /* ProcessedImageDownload is a superset of ImageProcessing, has to be
           first */
        GlyphCacheFeature::ProcessedImageDownload,
        GlyphCacheFeature::ImageProcessing
    });
}

struct AbstractGlyphCache::State {
    explicit State(PixelFormat format, const Vector3i& size, PixelFormat processedFormat, const Vector2i& processedSize, const Vector2i& padding): image{format, size, Containers::Array<char>{ValueInit, 4*((pixelFormatSize(format)*size.x() + 3)/4)*size.y()*size.z()}}, atlas{size}, processedFormat{processedFormat}, processedSize{processedSize}, padding{padding} {
        /* Flags are currently cleared as well, will be enabled back in a later
           step once the behavior is specified (with negative ranges) and
           Math::join() is fixed to handle those correctly. */
        atlas.setPadding(padding)
             .clearFlags(TextureTools::AtlasLandfillFlag::RotatePortrait|
                         TextureTools::AtlasLandfillFlag::RotateLandscape);
    }

    Image3D image;
    TextureTools::AtlasLandfill atlas;

    PixelFormat processedFormat;
    Vector2i processedSize;
    Vector2i padding;

    /* 0/4 bytes free */

    /* First element is glyph position relative to a point on the baseline,
       second layer in the texture atlas, third a region in the atlas
       slice. Index of the item is ID of the glyph in the cache, refered to
       from the fontGlyphMapping array. Index 0 is reserved for an invalid
       glyph. */
    Containers::Array<Containers::Triple<Vector2i, Int, Range2Di>> glyphs;
    /* `fontRanges[i]` to `fontRanges[i + 1]` is the range in
       `fontGlyphMapping` containing a mapping for glyphs from font `i`,
       `fontGlyphMapping[fontRanges[i]] + j` is then mapping from glyph ID `j`
       from font `i` to index in the `glyphs` array, or is 0 if given
       glyph isn't present in the cache (which then maps to the invalid
       glyph). */
    struct Font {
        UnsignedInt offset;
        /* 4 bytes free on 64b, but not so critical I think */
        const AbstractFont* pointer;
    };
    Containers::Array<Font> fonts;
    /* With an assumption that majority of font glyphs get put into a cache,
       this achieves O(1) mapping from a font ID + font-specific glyph ID pair
       to a cache-global glyph ID with far less overhead than a hashmap would,
       and much less memory used as well compared to storing a key, value and a
       hash for each mapping entry.

       Another assumption is that there's no more than 64k glyphs in total,
       which makes the mapping save half memory compared to storing 32-bit
       ints. 64K glyphs is enough to fill a 4K texture with 16x16 glyphs, which
       seems enough for now. It however might get reached at some point in
       practice, in which case the type would simply get changed to a 32-bit
       one (and the assertion in addGlyph() then removed). */
    Containers::Array<UnsignedShort> fontGlyphMapping;
};

AbstractGlyphCache::AbstractGlyphCache(const PixelFormat format, const Vector3i& size, const PixelFormat processedFormat, const Vector2i& processedSize, const Vector2i& padding) {
    CORRADE_ASSERT(size.product(),
        "Text::AbstractGlyphCache: expected non-zero size, got" << Debug::packed << size, );
    CORRADE_ASSERT(processedSize.product(),
        "Text::AbstractGlyphCache: expected non-zero processed size, got" << Debug::packed << processedSize, );

    /* Creating the state only after the assert as the AtlasLandfill would
       assert on zero size as well */
    _state.emplace(format, size, processedFormat, processedSize, padding);

    /* Default invalid glyph -- empty / zero-area */
    arrayAppend(_state->glyphs, InPlaceInit);

    /* There are no fonts yet */
    arrayAppend(_state->fonts, InPlaceInit, 0u, nullptr);
}

AbstractGlyphCache::AbstractGlyphCache(const PixelFormat format, const Vector3i& size, const PixelFormat processedFormat, const Vector2i& processedSize): AbstractGlyphCache{format, size, processedFormat, processedSize, Vector2i{1}} {}

AbstractGlyphCache::AbstractGlyphCache(const PixelFormat format, const Vector2i& size, const PixelFormat processedFormat, const Vector2i& processedSize, const Vector2i& padding): AbstractGlyphCache{format, Vector3i{size, 1}, processedFormat, processedSize, padding} {}

AbstractGlyphCache::AbstractGlyphCache(const PixelFormat format, const Vector2i& size, const PixelFormat processedFormat, const Vector2i& processedSize): AbstractGlyphCache{format, size, processedFormat, processedSize, Vector2i{1}} {}

AbstractGlyphCache::AbstractGlyphCache(const PixelFormat format, const Vector3i& size, const Vector2i& padding): AbstractGlyphCache{format, size, format, size.xy(), padding} {}

AbstractGlyphCache::AbstractGlyphCache(const PixelFormat format, const Vector3i& size): AbstractGlyphCache{format, size, Vector2i{1}} {}

AbstractGlyphCache::AbstractGlyphCache(const PixelFormat format, const Vector2i& size, const Vector2i& padding): AbstractGlyphCache{format, Vector3i{size, 1}, padding} {}

AbstractGlyphCache::AbstractGlyphCache(const PixelFormat format, const Vector2i& size): AbstractGlyphCache{format, size, Vector2i{1}} {}

#ifdef MAGNUM_BUILD_DEPRECATED
AbstractGlyphCache::AbstractGlyphCache(const Vector2i& size, const Vector2i& padding): AbstractGlyphCache{PixelFormat::R8Unorm, size, padding} {}

AbstractGlyphCache::AbstractGlyphCache(const Vector2i& size): AbstractGlyphCache{PixelFormat::R8Unorm, size, Vector2i{1}} {}
#endif

AbstractGlyphCache::AbstractGlyphCache(NoCreateT) noexcept {}

AbstractGlyphCache::AbstractGlyphCache(AbstractGlyphCache&&) noexcept = default;

AbstractGlyphCache::~AbstractGlyphCache() = default;

AbstractGlyphCache& AbstractGlyphCache::operator=(AbstractGlyphCache&&) noexcept = default;

PixelFormat AbstractGlyphCache::format() const {
    return _state->image.format();
}

PixelFormat AbstractGlyphCache::processedFormat() const {
    return _state->processedFormat;
}

Vector3i AbstractGlyphCache::size() const {
    return _state->image.size();
}

Vector3i AbstractGlyphCache::processedSize() const {
    return {_state->processedSize, _state->image.size().z()};
}

#ifdef MAGNUM_BUILD_DEPRECATED
Vector2i AbstractGlyphCache::textureSize() const {
    CORRADE_ASSERT(_state->image.size().z() == 1,
        "Text::AbstractGlyphCache::textureSize(): can't be used on an array glyph cache", {});
    return _state->image.size().xy();
}
#endif

Vector2i AbstractGlyphCache::padding() const {
    return _state->padding;
}

UnsignedInt AbstractGlyphCache::fontCount() const {
    return _state->fonts.size() - 1;
}

UnsignedInt AbstractGlyphCache::glyphCount() const {
    return _state->glyphs.size();
}

TextureTools::AtlasLandfill& AbstractGlyphCache::atlas() {
    return _state->atlas;
}

const TextureTools::AtlasLandfill& AbstractGlyphCache::atlas() const {
    return _state->atlas;
}

void AbstractGlyphCache::setInvalidGlyph(const Vector2i& offset, const Int layer, const Range2Di& rectangle) {
    State& state = *_state;
    /** @todo expand once rotations (and thus negative rectangle sizes) are
        supported */
    const Range2Di rectanglePadded = rectangle.padded(state.padding);
    #ifndef CORRADE_NO_ASSERT
    const Range2Dui rectangleu{rectangle};
    const Range2Dui rectanglePaddedu{rectanglePadded};
    #endif
    CORRADE_ASSERT(UnsignedInt(layer) < UnsignedInt(state.image.size().z()) && (rectangleu.min() <= rectangleu.max()).all() && (rectanglePaddedu.min() <= Vector2ui{state.image.size().xy()}).all() && (rectanglePaddedu.max() <= Vector2ui{state.image.size().xy()}).all(),
        "Text::AbstractGlyphCache::setInvalidGlyph(): layer" << layer << "and rectangle" << Debug::packed << rectangle << "out of range for size" << Debug::packed << state.image.size() << "and padding" << Debug::packed << state.padding, );

    state.glyphs[0] = {offset - _state->padding, layer, rectanglePadded};
}

void AbstractGlyphCache::setInvalidGlyph(const Vector2i& offset, const Range2Di& rectangle) {
    CORRADE_ASSERT(_state->image.size().z() == 1,
        "Text::AbstractGlyphCache::setInvalidGlyph(): use the layer overload for an array glyph cache", );
    setInvalidGlyph(offset, 0, rectangle);
}

UnsignedInt AbstractGlyphCache::addFont(const UnsignedInt glyphCount, const AbstractFont* const pointer) {
    State& state = *_state;

    #ifndef CORRADE_NO_ASSERT
    for(UnsignedInt i = 0; i != state.fonts.size() - 1; ++i)
        CORRADE_ASSERT(!pointer || state.fonts[i].pointer != pointer,
            "Text::AbstractGlyphCache::addFont(): pointer" << pointer << "already used for font" << i, {});
    #endif

    /* The last item in the font array now becomes the new font (and its offset
       should be the size of the fontGlyphMapping array), assign the pointer to
       it. Add a new item after which is the end offset sentinel. */
    CORRADE_INTERNAL_ASSERT(state.fontGlyphMapping.size() == state.fonts.back().offset);
    state.fonts.back().pointer = pointer;
    arrayAppend(state.fonts, InPlaceInit,
        state.fonts.back().offset + glyphCount,
        nullptr);

    /** @todo er, some arrayAppend with ValueInit + count? there's already
        arrayResize() with the same signature */
    for(UnsignedShort& i: arrayAppend(state.fontGlyphMapping, NoInit, glyphCount))
        i = 0;
    return state.fonts.size() - 2;
}

UnsignedInt AbstractGlyphCache::fontGlyphCount(const UnsignedInt fontId) const {
    const State& state = *_state;
    CORRADE_ASSERT(fontId < state.fonts.size() - 1,
        "Text::AbstractGlyphCache::fontGlyphCount(): index" << fontId << "out of range for" << state.fonts.size() - 1 << "fonts", {});
    return state.fonts[fontId + 1].offset - state.fonts[fontId].offset;
}

const AbstractFont* AbstractGlyphCache::fontPointer(const UnsignedInt fontId) const {
    const State& state = *_state;
    CORRADE_ASSERT(fontId < state.fonts.size() - 1,
        "Text::AbstractGlyphCache::fontPointer(): index" << fontId << "out of range for" << state.fonts.size() - 1 << "fonts", {});
    return state.fonts[fontId].pointer;
}

Containers::Optional<UnsignedInt> AbstractGlyphCache::findFont(const AbstractFont& font) const {
    const State& state = *_state;
    for(UnsignedInt i = 0; i != state.fonts.size() - 1; ++i)
        if(state.fonts[i].pointer == &font) return i;
    return {};
}

#ifdef MAGNUM_BUILD_DEPRECATED
std::vector<Range2Di> AbstractGlyphCache::reserve(const std::vector<Vector2i>& sizes) {
    State& state = *_state;
    CORRADE_ASSERT(state.image.size().z() == 1,
        "Text::AbstractGlyphCache::reserve(): can't be used on an array glyph cache", {});
    /* This is technically possible now, but we just don't bother for the
       compatibility API as it would need to be additionally tested */
    CORRADE_ASSERT(state.fonts.size() == 1,
        "Text::AbstractGlyphCache::reserve(): reserving space in non-empty cache is not yet implemented", {});

    /* Append an empty font range just to prevent reserve() from being called
       again */
    arrayAppend(state.fonts, InPlaceInit, 0u, nullptr);

    /* Disable rotations in the atlas as the old API doesn't expect them */
    const TextureTools::AtlasLandfillFlags previousFlags = state.atlas.flags();
    state.atlas.clearFlags(TextureTools::AtlasLandfillFlag::RotatePortrait|
                           TextureTools::AtlasLandfillFlag::RotateLandscape);

    /* Create the output array */
    std::vector<Range2Di> out(sizes.size());
    for(std::size_t i = 0; i != sizes.size(); ++i)
        out[i].max() = sizes[i];

    const bool succeeded = !!state.atlas.add(
        Containers::stridedArrayView(out).slice(&Range2Di::max),
        Containers::stridedArrayView(out).slice(&Range2Di::min));

    /* Restore previous flags back */
    state.atlas.setFlags(previousFlags);

    /* The error message matches what the old TextureTools::atlas() did. Not
       great, but that's the interface we should stay compatible with. */
    if(!succeeded) {
        Error{} << "Text::AbstractGlyphCache::reserve(): requested atlas size"
                << state.image.size().xy() << "is too small to fit"
                << sizes.size() << "textures. Generated atlas will be empty.";
        return {};
    }

    /* Update the ranges max values to match the new offsets */
    for(std::size_t i = 0; i != sizes.size(); ++i)
        out[i].max() += out[i].min();

    return out;
}
#endif

UnsignedInt AbstractGlyphCache::addGlyph(const UnsignedInt fontId, const UnsignedInt fontGlyphId, const Vector2i& offset, const Int layer, const Range2Di& rectangle) {
    State& state = *_state;
    CORRADE_ASSERT(fontId < state.fonts.size() - 1,
        "Text::AbstractGlyphCache::addGlyph(): index" << fontId << "out of range for" << state.fonts.size() - 1 << "fonts", {});
    const UnsignedInt fontOffset = state.fonts[fontId].offset;
    CORRADE_ASSERT(fontGlyphId < state.fonts[fontId + 1].offset - fontOffset,
        "Text::AbstractGlyphCache::addGlyph(): index" << fontGlyphId << "out of range for" << state.fonts[fontId + 1].offset - fontOffset << "glyphs in font" << fontId, {});
    CORRADE_ASSERT(!state.fontGlyphMapping[fontOffset + fontGlyphId],
        "Text::AbstractGlyphCache::addGlyph(): glyph" << fontGlyphId << "in font" << fontId << "already added at index" << state.fontGlyphMapping[fontOffset + fontGlyphId], {});
    /** @todo expand once rotations (and thus negative rectangle sizes) are
        supported */
    #ifndef CORRADE_NO_ASSERT
    const Range2Dui rectangleu{rectangle};
    const Range2Dui rectanglePaddedu{rectangle.padded(state.padding)};
    #endif
    CORRADE_ASSERT(UnsignedInt(layer) < UnsignedInt(state.image.size().z()) && (rectangleu.min() <= rectangleu.max()).all() && (rectanglePaddedu.min() <= Vector2ui{state.image.size().xy()}).all() && (rectanglePaddedu.max() <= Vector2ui{state.image.size().xy()}).all(),
        "Text::AbstractGlyphCache::addGlyph(): layer" << layer << "and rectangle" << Debug::packed << rectangle << "out of range for size" << Debug::packed << state.image.size() << "and padding" << Debug::packed << state.padding, {});

    const UnsignedInt glyphId = state.glyphs.size();
    /* The fontGlyphMapping entries are 16-bit to save memory, can't have IDs
       beyond that. See its documentation for more reasoning. */
    CORRADE_ASSERT(glyphId < 65536,
        "Text::AbstractGlyphCache::addGlyph(): only at most 65536 glyphs can be added", {});
    state.fontGlyphMapping[fontOffset + fontGlyphId] = glyphId;
    arrayAppend(state.glyphs, InPlaceInit, offset - _state->padding, layer, rectangle.padded(_state->padding));
    return glyphId;
}

UnsignedInt AbstractGlyphCache::addGlyph(const UnsignedInt fontId, const UnsignedInt fontGlyphId, const Vector2i& offset, const Range2Di& rectangle) {
    CORRADE_ASSERT(_state->image.size().z() == 1,
        "Text::AbstractGlyphCache::addGlyph(): use the layer overload for an array glyph cache", {});
    return addGlyph(fontId, fontGlyphId, offset, 0, rectangle);
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractGlyphCache::insert(const UnsignedInt glyph, const Vector2i& offset, const Range2Di& rectangle) {
    State& state = *_state;
    CORRADE_ASSERT(state.image.size().z() == 1,
        "Text::AbstractGlyphCache::insert(): can't be used on an array glyph cache", );
    CORRADE_ASSERT(state.fonts.size() <= 2,
        "Text::AbstractGlyphCache::insert(): can't be used on a multi-font glyph cache", );

    /* Overwriting "Not Found" glyph */
    if(glyph == 0) {
        setInvalidGlyph(offset, rectangle);

    /* Inserting new glyph. Add the first ever font and ajust the font range if
       needed. */
    } else {
        if(state.fonts.size() == 1)
            arrayAppend(_state->fonts, InPlaceInit, 0u, nullptr);
        if(glyph >= state.fonts[1].offset) {
            arrayResize(state.fontGlyphMapping, glyph + 1);
            state.fonts[1].offset = glyph + 1;
        }

        addGlyph(0, glyph, offset, rectangle);
    }
}
#endif

MutableImageView3D AbstractGlyphCache::image() {
    return _state->image;
}

ImageView3D AbstractGlyphCache::image() const {
    return _state->image;
}

void AbstractGlyphCache::flushImage(const Range3Di& range) {
    State& state = *_state;
    #ifndef CORRADE_NO_ASSERT
    const Range3Dui rangeu{range};
    #endif
    CORRADE_ASSERT((rangeu.min() <= rangeu.max()).all() && (rangeu.max() <= Vector3ui{state.image.size()}).all(),
        "Text::AbstractGlyphCache::flushImage():" << Debug::packed << range << "out of range for size" << Debug::packed << state.image.size(), );

    /* Set the image including padding, to make sure the sampled glyph area
       doesn't contain potentially uninitialized GPU memory */
    const Vector3i paddedMin = Math::max(Vector3i{0},
        range.min() - Vector3i{padding(), 0});
    const Vector3i paddedMax = Math::min(size(),
        range.max() + Vector3i{padding(), 0});

    /** @todo ugh have slicing on images directly already */
    PixelStorage storage;
    storage.setRowLength(state.image.size().x())
        .setSkip(paddedMin);
    /* Set image height only if it's an array glyph cache, as otherwise it'd
       cause errors on ES2 that doesn't support this pixel storage state */
    if(state.image.size().z() != 1)
        storage.setImageHeight(state.image.size().y());
    doSetImage(paddedMin, ImageView3D{
        storage,
        state.image.format(),
        paddedMax - paddedMin,
        state.image.data()});
}

void AbstractGlyphCache::flushImage(Int layer, const Range2Di& range) {
    flushImage({{range.min(), layer},
                {range.max(), layer + 1}});
}

void AbstractGlyphCache::flushImage(const Range2Di& range) {
    CORRADE_ASSERT(_state->image.size().z() == 1,
        "Text::AbstractGlyphCache::flushImage(): use the 3D or layer overload for an array glyph cache", );
    flushImage(0, range);
}

void AbstractGlyphCache::doSetImage(const Vector3i& offset, const ImageView3D& image) {
    if(_state->image.size().z() == 1)
        /** @todo ugh have slicing on images directly already */
        return doSetImage(offset.xy(), ImageView2D{image.storage(), image.format(), image.size().xy(), image.data()});

    CORRADE_ASSERT_UNREACHABLE("Text::AbstractGlyphCache::image(): not implemented by derived class", );
}

void AbstractGlyphCache::doSetImage(const Vector2i&, const ImageView2D&) {
    CORRADE_ASSERT_UNREACHABLE("Text::AbstractGlyphCache::image(): not implemented by derived class", );
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractGlyphCache::setImage(const Vector2i& offset, const ImageView2D& image) {
    State& state = *_state;
    CORRADE_ASSERT(state.image.size().z() == 1,
        "Text::AbstractGlyphCache::setImage(): can't be used on an array glyph cache", );
    CORRADE_ASSERT((offset >= Vector2i{} && offset + image.size() <= state.image.size().xy()).all(),
        "Text::AbstractGlyphCache::setImage():" << Range2Di::fromSize(offset, image.size()) << "out of range for glyph cache of size" << state.image.size().xy(), );
    CORRADE_ASSERT(image.format() == state.image.format(),
        "Text::AbstractGlyphCache::setImage(): expected" << state.image.format() << "but got" << image.format(), );
    const Containers::StridedArrayView3D<const char> src = image.pixels();
    Utility::copy(src,
        state.image.pixels()[0].sliceSize({std::size_t(offset.y()),
                                           std::size_t(offset.x()),
                                           0}, src.size()));
    flushImage(Range2Di::fromSize(offset, image.size()));
}
#endif

Image3D AbstractGlyphCache::processedImage() {
    CORRADE_ASSERT(features() >= GlyphCacheFeature::ProcessedImageDownload,
        "Text::AbstractGlyphCache::processedImage(): feature not supported", Image3D{PixelFormat::R8Unorm});

    return doProcessedImage();
}

Image3D AbstractGlyphCache::doProcessedImage() {
    CORRADE_ASSERT_UNREACHABLE("Text::AbstractGlyphCache::processedImage(): feature advertised but not implemented", Image3D{PixelFormat::R8Unorm});
}

void AbstractGlyphCache::setProcessedImage(const Vector3i& offset, const ImageView3D& image) {
    #ifndef CORRADE_NO_ASSERT
    State& state = *_state;
    #endif
    CORRADE_ASSERT(features() >= GlyphCacheFeature::ImageProcessing,
        "Text::AbstractGlyphCache::setProcessedImage(): feature not supported", );
    CORRADE_ASSERT((offset >= Vector3i{} && offset + image.size() <= processedSize()).all(),
        "Text::AbstractGlyphCache::setProcessedImage():" << Debug::packed << Range3Di::fromSize(offset, image.size()) << "out of range for size" << Debug::packed << processedSize(), );
    CORRADE_ASSERT(image.format() == state.processedFormat,
        "Text::AbstractGlyphCache::setProcessedImage(): expected" << state.processedFormat << "but got" << image.format(), );
    doSetProcessedImage(offset, image);
}

void AbstractGlyphCache::setProcessedImage(const Vector2i& offset, const ImageView2D& image) {
    CORRADE_ASSERT(_state->image.size().z() == 1,
        "Text::AbstractGlyphCache::setProcessedImage(): use the 3D overload for an array glyph cache", );
    setProcessedImage({offset, 0}, image);
}

void AbstractGlyphCache::doSetProcessedImage(const Vector3i& offset, const ImageView3D& image) {
    if(_state->image.size().z() == 1)
        /** @todo ugh have slicing on images directly already */
        return doSetProcessedImage(offset.xy(), ImageView2D{image.storage(), image.format(), image.size().xy(), image.data()});

    CORRADE_ASSERT_UNREACHABLE("Text::AbstractGlyphCache::setProcessedImage(): feature advertised but not implemented", );
}

void AbstractGlyphCache::doSetProcessedImage(const Vector2i&, const ImageView2D&) {
    CORRADE_ASSERT_UNREACHABLE("Text::AbstractGlyphCache::setProcessedImage(): feature advertised but not implemented", );
}

UnsignedInt AbstractGlyphCache::glyphId(const UnsignedInt fontId, const UnsignedInt fontGlyphId) const {
    const State& state = *_state;
    CORRADE_DEBUG_ASSERT(fontId < state.fonts.size() - 1,
        "Text::AbstractGlyphCache::glyphId(): index" << fontId << "out of range for" << state.fonts.size() - 1 << "fonts", {});
    const UnsignedInt fontOffset = state.fonts[fontId].offset;
    CORRADE_DEBUG_ASSERT(fontGlyphId < state.fonts[fontId + 1].offset - fontOffset,
        "Text::AbstractGlyphCache::glyphId(): index" << fontGlyphId << "out of range for" << state.fonts[fontId + 1].offset - fontOffset << "glyphs in font" << fontId, {});
    return state.fontGlyphMapping[fontOffset + fontGlyphId];
}

void AbstractGlyphCache::glyphIdsInto(const UnsignedInt fontId, const Containers::StridedArrayView1D<const UnsignedInt>& fontGlyphIds, const Containers::StridedArrayView1D<UnsignedInt>& glyphIds) const {
    CORRADE_ASSERT(fontGlyphIds.size() == glyphIds.size(),
        "Text::AbstractGlyphCache::glyphIdsInto(): expected fontGlyphIds and glyphIds views to have the same size but got" << fontGlyphIds.size() << "and" << glyphIds.size(), );
    const State& state = *_state;
    CORRADE_ASSERT(fontId < state.fonts.size() - 1,
        "Text::AbstractGlyphCache::glyphIdsInto(): index" << fontId << "out of range for" << state.fonts.size() - 1 << "fonts", );
    const UnsignedInt fontOffset = state.fonts[fontId].offset;
    #ifndef CORRADE_NO_DEBUG_ASSERT
    const UnsignedInt fontGlyphCount = state.fonts[fontId + 1].offset - fontOffset;
    #endif

    for(std::size_t i = 0; i != fontGlyphIds.size(); ++i) {
        const UnsignedInt fontGlyphId = fontGlyphIds[i];
        CORRADE_DEBUG_ASSERT(fontGlyphId < fontGlyphCount,
            "Text::AbstractGlyphCache::glyphIdsInto(): glyph" << i << "index" << fontGlyphId << "out of range for" << fontGlyphCount << "glyphs in font" << fontId, );
        glyphIds[i] = state.fontGlyphMapping[fontOffset + fontGlyphId];
    }
}

void AbstractGlyphCache::glyphIdsInto(const UnsignedInt fontId, const std::initializer_list<UnsignedInt> fontGlyphIds, const Containers::StridedArrayView1D<UnsignedInt>& glyphIds) const {
    return glyphIdsInto(fontId, Containers::arrayView(fontGlyphIds), glyphIds);
}

Containers::StridedArrayView1D<const Vector2i> AbstractGlyphCache::glyphOffsets() const {
    return stridedArrayView(_state->glyphs).slice(&Containers::Triple<Vector2i, Int, Range2Di>::first);
}

Containers::StridedArrayView1D<const Int> AbstractGlyphCache::glyphLayers() const {
    return stridedArrayView(_state->glyphs).slice(&Containers::Triple<Vector2i, Int, Range2Di>::second);
}

Containers::StridedArrayView1D<const Range2Di> AbstractGlyphCache::glyphRectangles() const {
    return stridedArrayView(_state->glyphs).slice(&Containers::Triple<Vector2i, Int, Range2Di>::third);
}

Containers::Triple<Vector2i, Int, Range2Di> AbstractGlyphCache::glyph(const UnsignedInt fontId, const UnsignedInt fontGlyphId) const {
    const State& state = *_state;
    CORRADE_DEBUG_ASSERT(fontId < state.fonts.size() - 1,
        "Text::AbstractGlyphCache::glyph(): index" << fontId << "out of range for" << state.fonts.size() - 1 << "fonts", {});
    const UnsignedInt fontOffset = state.fonts[fontId].offset;
    CORRADE_DEBUG_ASSERT(fontGlyphId < state.fonts[fontId + 1].offset - fontOffset,
        "Text::AbstractGlyphCache::glyph(): index" << fontGlyphId << "out of range for" << state.fonts[fontId + 1].offset - fontOffset << "glyphs in font" << fontId, {});
    return state.glyphs[state.fontGlyphMapping[fontOffset + fontGlyphId]];
}

Containers::Triple<Vector2i, Int, Range2Di> AbstractGlyphCache::glyph(const UnsignedInt glyphId) const {
    const State& state = *_state;
    CORRADE_DEBUG_ASSERT(glyphId < state.glyphs.size(),
        "Text::AbstractGlyphCache::glyph(): index" << glyphId << "out of range for" << state.glyphs.size() << "glyphs", {});
    return state.glyphs[glyphId];
}

#ifdef MAGNUM_BUILD_DEPRECATED
std::pair<Vector2i, Range2Di> AbstractGlyphCache::operator[](const UnsignedInt glyphId) const {
    const State& state = *_state;
    CORRADE_ASSERT(state.image.size().z() == 1,
        "Text::AbstractGlyphCache::operator[](): can't be used on an array glyph cache", {});
    const Containers::Triple<Vector2i, Int, Range2Di> out =
        glyphId && glyphId < state.fonts[1].offset ? glyph(0, glyphId) : glyph(0);
    return {out.first(), out.third()};
}
#endif

}}
