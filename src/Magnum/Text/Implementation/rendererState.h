#ifndef Magnum_Text_Implementation_rendererState_h
#define Magnum_Text_Implementation_rendererState_h
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

#include "Magnum/Text/Renderer.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StridedArrayView.h>

#include "Magnum/Mesh.h"
#include "Magnum/Math/Range.h"
#include "Magnum/Text/Alignment.h"
#include "Magnum/Text/Direction.h"

namespace Magnum { namespace Text {

/* Is inherited by RendererCore::AllocatorState, Renderer::State and then
   RendererGL::State to avoid extra allocations for each class' state */
struct RendererCore::State {
    /* Gets called by RendererCore only if both allocators are specified by the
       user. If not, AllocatorState is constructed instead. */
    explicit State(const AbstractGlyphCache& glyphCache, void(*glyphAllocator)(void*, UnsignedInt, Containers::StridedArrayView1D<Vector2>&, Containers::StridedArrayView1D<UnsignedInt>&, Containers::StridedArrayView1D<UnsignedInt>*, Containers::StridedArrayView1D<Vector2>&), void* glyphAllocatorState, void(*const runAllocator)(void*, UnsignedInt, Containers::StridedArrayView1D<Float>&, Containers::StridedArrayView1D<UnsignedInt>&), void* runAllocatorState, RendererCoreFlags flags): glyphCache(glyphCache), glyphAllocator{glyphAllocator}, glyphAllocatorState{glyphAllocatorState}, runAllocator{runAllocator}, runAllocatorState{runAllocatorState}, flags{flags} {
        CORRADE_INTERNAL_DEBUG_ASSERT(glyphAllocator && runAllocator);
    }

    /* AllocatorState, Renderer::State etc. inherit from this with the instance
       deleted through the base pointer */
    virtual ~State() = default;

    const AbstractGlyphCache& glyphCache;
    void(*const glyphAllocator)(void*, UnsignedInt, Containers::StridedArrayView1D<Vector2>&, Containers::StridedArrayView1D<UnsignedInt>&, Containers::StridedArrayView1D<UnsignedInt>*, Containers::StridedArrayView1D<Vector2>&);
    void* const glyphAllocatorState;
    void(*const runAllocator)(void*, UnsignedInt, Containers::StridedArrayView1D<Float>&, Containers::StridedArrayView1D<UnsignedInt>&);
    void* const runAllocatorState;
    const RendererCoreFlags flags;

    /* These are controllable by various setters. Keep these values in sync
       with code in reset().  */
    Alignment alignment = Alignment::MiddleCenter;
    LayoutDirection layoutDirection = LayoutDirection::HorizontalTopToBottom;
    /* 1 byte free */
    Vector2 cursor;
    Float lineAdvance = 0.0f;

    /* Capacity is the array size. The "rendering" value is glyphs from the
       add() calls since the last render() or clear(), i.e. ones that aren't
       fully aligned and such yet. */
    UnsignedInt glyphCount = 0, renderingGlyphCount = 0;
    Containers::StridedArrayView1D<Vector2> glyphPositions;
    Containers::StridedArrayView1D<UnsignedInt> glyphIds;
    /* Non-empty only if RendererFlag::GlyphClusters is set */
    Containers::StridedArrayView1D<UnsignedInt> glyphClusters;
    Containers::StridedArrayView1D<Vector2> glyphAdvances;

    /* Capacity is the array size. The "rendering" value is again runs from the
       add() calls since the last render() or clear().  */
    UnsignedInt runCount = 0, renderingRunCount = 0;
    Containers::StridedArrayView1D<Float> runScales;
    Containers::StridedArrayView1D<UnsignedInt> runEnds;

    /* Rendering state */
    bool rendering = false;
    /* 1 byte free */
    Containers::Optional<Alignment> resolvedAlignment;
    /* Both are a zero vector initially, the first track the current line start
       and the second position within the current line. The actual `cursor` is
       added to all glyph positions only at the end. */
    Vector2 renderingLineStart, renderingLineCursor;
    /* On add(), if zero, is set to lineAdvance (if non-zero) or (scaled) line
       advance of the first used font */
    /** @todo might want to vary line advance per line based on the actual
        metrics of used fonts (it looks uglily uneven though!), would need to
        keep track of max descent, max ascent and max line gap of fonts used on
        previous and next line and calculate the actual line advance from
        prev line max descent + max of prev and current line line gap + max of
        current line ascent, and then shift the whole like by that once it's
        done instead of advancing directly the cursor before */
    Vector2 renderingLineAdvance;
    /* Everything until runCount is a block that needs to be aligned */
    UnsignedInt blockRunBegin = 0;
    Range2D blockRectangle;
    /* Everything until runCount is a line that needs to be aligned */
    UnsignedInt lineGlyphBegin = 0;
    Range2D lineRectangle;
};

/* Instantiated only if the builtin allocators are used. See their contents for
   the types being stored here. */
struct RendererCore::AllocatorState: RendererCore::State {
    /* Defined in Renderer.cpp because it needs access to default allocator
       implementations */
    explicit AllocatorState(const AbstractGlyphCache& glyphCache, void(*glyphAllocator)(void*, UnsignedInt, Containers::StridedArrayView1D<Vector2>&, Containers::StridedArrayView1D<UnsignedInt>&, Containers::StridedArrayView1D<UnsignedInt>*, Containers::StridedArrayView1D<Vector2>&), void* glyphAllocatorState, void(*const runAllocator)(void*, UnsignedInt, Containers::StridedArrayView1D<Float>&, Containers::StridedArrayView1D<UnsignedInt>&), void* runAllocatorState, RendererCoreFlags flags);

    Containers::Array<char> glyphData;
    Containers::Array<char> runData;
};

/** @todo this includes the glyphData + runData (+ indexData, vertexData)
    members even when they're unused because custom allocators are used, have
    some templated RendererCore::AllocatorState that inherits either the
    RendererCore::State or Renderer::State and adds one or more of those based
    on what all builtin allocators are used? or am I overdoing it for measly 96
    byte savings? */
struct Renderer::State: RendererCore::AllocatorState {
    /* Defined in Renderer.cpp because it needs access to default allocator
       implementations */
    explicit State(const AbstractGlyphCache& glyphCache, void(*glyphAllocator)(void*, UnsignedInt, Containers::StridedArrayView1D<Vector2>&, Containers::StridedArrayView1D<UnsignedInt>&, Containers::StridedArrayView1D<UnsignedInt>*, Containers::StridedArrayView1D<Vector2>&), void* glyphAllocatorState, void(*const runAllocator)(void*, UnsignedInt, Containers::StridedArrayView1D<Float>&, Containers::StridedArrayView1D<UnsignedInt>&), void* runAllocatorState, void(*indexAllocator)(void*, UnsignedInt, Containers::ArrayView<char>&), void* indexAllocatorState, void(*vertexAllocator)(void*, UnsignedInt, Containers::StridedArrayView1D<Vector2>&, Containers::StridedArrayView1D<Vector2>&), void* vertexAllocatorState, RendererFlags flags);

    void(*const indexAllocator)(void*, UnsignedInt, Containers::ArrayView<char>&);
    void* const indexAllocatorState;
    void(*const vertexAllocator)(void*, UnsignedInt, Containers::StridedArrayView1D<Vector2>&, Containers::StridedArrayView1D<Vector2>&);
    void* const vertexAllocatorState;

    MeshIndexType minIndexType = MeshIndexType::UnsignedByte;
    MeshIndexType indexType = MeshIndexType::UnsignedByte;
    Containers::ArrayView<char> indices;
    Containers::StridedArrayView1D<Vector2> vertexPositions;
    /* If using an array glyph cache, it can be cast to Vector3 */
    Containers::StridedArrayView1D<Vector2> vertexTextureCoordinates;

    /* Used only if the builtin vertex allocator is used */
    Containers::Array<char> indexData;
    Containers::Array<char> vertexData;
};

namespace Implementation {

/* Not used in the state structs above but needed by both Renderer and
   RendererGL */
struct Vertex {
    Vector2 position;
    Vector2 textureCoordinates;
};

struct VertexArray {
    Vector2 position;
    Vector3 textureCoordinates;
};

}

}}

#endif
