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

#include <string>
#include <tuple>
#include <vector>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/StaticArray.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/Triple.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/Format.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/Text/AbstractFont.h"
#include "Magnum/Text/AbstractGlyphCache.h"
#include "Magnum/Text/AbstractShaper.h"
#include "Magnum/Text/Direction.h"
#include "Magnum/Text/Feature.h"
#include "Magnum/Text/Renderer.h"

namespace Magnum { namespace Text { namespace Test { namespace {

struct RendererTest: TestSuite::Tester {
    explicit RendererTest();

    /* Unlike the order in Renderer.h, the low-level utils are tested first
       since the higher-level APIs rely on them */

    void lineGlyphPositions();
    void lineGlyphPositionsAliasedViews();
    void lineGlyphPositionsInvalidViewSizes();
    void lineGlyphPositionsInvalidDirection();
    void lineGlyphPositionsNoFontOpened();

    void glyphQuads();
    void glyphQuadsAliasedViews();
    void glyphQuadsInvalidViewSizes();
    void glyphQuadsNoFontOpened();
    void glyphQuadsFontNotFoundInCache();

    void glyphQuads2D();
    void glyphQuads2DArrayGlyphCache();

    void glyphQuadBounds();
    void glyphQuadBoundsInvalidViewSizes();

    void alignLine();
    void alignLineInvalidDirection();

    void alignBlock();
    void alignBlockInvalidDirection();

    template<class T> void glyphQuadIndices();
    void glyphQuadIndicesTypeTooSmall();

    void glyphRangeForBytes();

    void debugFlagCore();
    void debugFlagsCore();

    void constructCore();
    void constructCoreAllocator();
    void constructCoreNoCreate();

    void constructCopyCore();
    void constructMoveCore();

    void propertiesCore();
    void propertiesCoreInvalid();
    void propertiesCoreRenderingInProgress();

    void glyphsForRuns();
    void glyphsForRunsInvalid();

    void allocateCore();
    void allocateCoreGlyphAllocator();
    void allocateCoreGlyphAllocatorInvalid();
    void allocateCoreRunAllocator();
    void allocateCoreRunAllocatorInvalid();

    void addSingleLine();
    void addSingleLineAlign();
    void addMultipleLines();
    void addMultipleLinesAlign();
    void addFontNotFoundInCache();

    void multipleBlocks();

    void clearResetCore();
    void clearResetCoreAllocators();

    #ifdef MAGNUM_TARGET_GL
    void renderData();

    void multiline();

    void arrayGlyphCache();
    void fontNotFoundInCache();
    #endif
};

const struct {
    const char* name;
    bool globalIds;
} GlyphQuadsData[]{
    {"font-specific glyph IDs", false},
    {"cache-global glyph IDs", true}
};

const struct {
    const char* name;
    Alignment alignment;
    Float offset;
} AlignLineData[]{
    /* The vertical alignment and GlyphBounds has no effect here */
    /* Left is the default (0) value, thus should result in no shift */
    {"left", Alignment::BottomLeft, -10.0f},
    {"right", Alignment::LineRightGlyphBounds, -13.5f},
    /* Integral should be handled only for Center */
    {"right, integral", Alignment::MiddleRightGlyphBoundsIntegral, -13.5f},
    {"center", Alignment::TopCenter, -11.75f},
    {"center, integral", Alignment::TopCenterIntegral, -12.0f},
};

const struct {
    const char* name;
    Alignment alignment;
    Float offset;
} AlignBlockData[]{
    /* The horizontal alignment and GlyphBounds has no effect here */
    /* Line is the default (0) value, thus should result in no shift */
    {"line", Alignment::LineCenterGlyphBounds, 0.0f},
    {"bottom", Alignment::BottomRight, -9.5f},
    {"top", Alignment::TopLeftGlyphBounds, -19.5f},
    /* Integral should be handled only for Middle */
    {"top, integral", Alignment::TopCenterGlyphBoundsIntegral, -19.5f},
    {"middle", Alignment::MiddleLeft, -14.5f},
    {"middle, integral", Alignment::MiddleLeftIntegral, -15.0f}
};

const struct {
    const char* name;
    bool ascending;
    Containers::Pair<UnsignedInt, UnsignedInt>(*function)(const Containers::StridedArrayView1D<const UnsignedInt>&, UnsignedInt, UnsignedInt);
} GlyphRangeForBytesData[]{
    {"", true,
        glyphRangeForBytes},
    {"reverse direction", false,
        glyphRangeForBytes},
    {"swapped begin & end", true,
        [](const Containers::StridedArrayView1D<const UnsignedInt>& clusters, UnsignedInt begin, UnsignedInt end) {
            /* If begin > end, the output should be also swapped, so swapping
               it back should result in the same thing as with non-swapped
               input */
            Containers::Pair<UnsignedInt, UnsignedInt> out = glyphRangeForBytes(clusters, end, begin);
            return Containers::pair(out.second(), out.first());
        }},
    {"swapped begin & end, reverse direction", false,
        [](const Containers::StridedArrayView1D<const UnsignedInt>& clusters, UnsignedInt begin, UnsignedInt end) {
            Containers::Pair<UnsignedInt, UnsignedInt> out = glyphRangeForBytes(clusters, end, begin);
            return Containers::pair(out.second(), out.first());
        }},
};

const struct {
    const char* name;
    RendererCoreFlags flags;
} ConstructCoreData[]{
    {"", {}},
    {"with glyph clusters", RendererCoreFlag::GlyphClusters}
};

const struct {
    const char* name;
    void(*glyphAllocator)(void*, UnsignedInt, Containers::StridedArrayView1D<Vector2>&, Containers::StridedArrayView1D<UnsignedInt>&, Containers::StridedArrayView1D<UnsignedInt>*, Containers::StridedArrayView1D<Vector2>&);
    void(*runAllocator)(void*, UnsignedInt, Containers::StridedArrayView1D<Float>&, Containers::StridedArrayView1D<UnsignedInt>&);
    RendererCoreFlags flags;
} ConstructCoreAllocatorData[]{
    {"no allocators", nullptr, nullptr, {}},
    {"no allocators, with glyph clusters", nullptr, nullptr, RendererCoreFlag::GlyphClusters},
    {"glyph allocator", [](void* called, UnsignedInt, Containers::StridedArrayView1D<Vector2>&, Containers::StridedArrayView1D<UnsignedInt>&, Containers::StridedArrayView1D<UnsignedInt>*, Containers::StridedArrayView1D<Vector2>&){
        ++*static_cast<int*>(called);
    }, nullptr, {}},
    {"glyph allocator, with glyph clusters", [](void* called, UnsignedInt, Containers::StridedArrayView1D<Vector2>&, Containers::StridedArrayView1D<UnsignedInt>&, Containers::StridedArrayView1D<UnsignedInt>*, Containers::StridedArrayView1D<Vector2>&){
        ++*static_cast<int*>(called);
    }, nullptr, RendererCoreFlag::GlyphClusters},
    {"run allocator", nullptr, [](void* called, UnsignedInt, Containers::StridedArrayView1D<Float>&, Containers::StridedArrayView1D<UnsignedInt>&){
        ++*static_cast<int*>(called);
    }, {}},
    {"both allocators", [](void* called, UnsignedInt, Containers::StridedArrayView1D<Vector2>&, Containers::StridedArrayView1D<UnsignedInt>&, Containers::StridedArrayView1D<UnsignedInt>*, Containers::StridedArrayView1D<Vector2>&){
        ++*static_cast<int*>(called);
    }, [](void* called, UnsignedInt, Containers::StridedArrayView1D<Float>&, Containers::StridedArrayView1D<UnsignedInt>&){
        ++*static_cast<int*>(called);
    }, {}},
    {"both allocators, with glyph clusters", [](void* called, UnsignedInt, Containers::StridedArrayView1D<Vector2>&, Containers::StridedArrayView1D<UnsignedInt>&, Containers::StridedArrayView1D<UnsignedInt>*, Containers::StridedArrayView1D<Vector2>&){
        ++*static_cast<int*>(called);
    }, [](void* called, UnsignedInt, Containers::StridedArrayView1D<Float>&, Containers::StridedArrayView1D<UnsignedInt>&){
        ++*static_cast<int*>(called);
    }, RendererCoreFlag::GlyphClusters},
};

const struct {
    TestSuite::TestCaseDescriptionSourceLocation name;
    RendererCoreFlags flagsCore;
    UnsignedInt reserveGlyphs, reserveRuns,
        secondReserveGlyphs, secondReserveRuns;
    bool render, renderAddOnly, expectNoGlyphReallocation, expectNoRunReallocation;
    UnsignedInt expectedGlyphCapacity, expectedRunCapacity;
} AllocateData[]{
    {"second reserve() same as first",
        {}, 26, 3, 26, 3, false, false, true, true, 26, 3},
    {"second reserve() less glyphs than first",
        {}, 26, 3, 23, 3, false, false, true, true, 26, 3},
    {"second reserve() less runs than first",
        {}, 26, 3, 26, 1, false, false, true, true, 26, 3},
    {"second reserve() reallocates glyphs",
        {}, 3, 3, 26, 3, false, false, false, true, 26, 3},
    {"second reserve() reallocates runs",
        {}, 26, 1, 26, 3, false, false, true, false, 26, 3},
    {"render",
        {}, 26, 3,  0, 0, true, false, true, true, 26, 3},
    {"render, second reserve() reallocates glyphs",
        {},  3, 3, 26, 3, true, false, false, true, 26, 3},
    {"render, second reserve() reallocates runs",
        {}, 26, 1, 26, 3, true, false, true, false, 26, 3},
    {"render, second render() reallocates glyphs",
        {},  3, 3,  0, 0, true, false, false, true, 26, 3},
    {"render, second render() reallocates runs",
        {}, 26, 1,  0, 0, true, false, true, false, 26, 3},
    {"render, second reserve() reallocates both, second render() also",
        {},  3, 1, 13, 2, true, false, false, false, 13, 2},
    {"render, second reserve() while in progress reallocates glyphs",
        {},  3, 3, 26, 3, true, true, false, true, 26, 3},
    {"render, second reserve() while in progress reallocates runs",
        {}, 26, 1, 26, 3, true, true, true, false, 26, 3},
    {"render, second render() while in progress reallocates glyphs",
        {},  3, 3,  0, 0, true, true, false, true, 26, 3},
    {"render, second render() while in progress reallocates runs",
        {}, 26, 1,  0, 0, true, true, true, false, 26, 3},
    {"render, second reserve() while in progress reallocates both, second render() also",
        {},  3, 1, 13, 2, true, true, false, false, 13, 2},
    /* The flag affects only glyph allocation, not runs, so their variants are
       not tested below */
    {"with glyph (positions and) clusters, second reserve() same as first",
        RendererCoreFlag::GlyphClusters,
        26, 3, 26, 3, false, false, true, true, 26, 3},
    {"with glyph (positions and) clusters, second reserve() less glyphs than first",
        RendererCoreFlag::GlyphClusters,
        26, 3, 23, 3, false, false, true, true, 26, 3},
    {"with glyph (positions and) clusters, second reserve() reallocates glyphs",
        RendererCoreFlag::GlyphClusters,
        3, 3, 26, 3, false, false, false, true, 26, 3},
    {"with glyph (positions and) clusters, render",
        RendererCoreFlag::GlyphClusters,
        26, 3,  0, 0, true, false, true, true, 26, 3},
    {"with glyph (positions and) clusters, render, second render() reallocates glyphs",
        RendererCoreFlag::GlyphClusters,
        3, 3,  0, 0, true, false, false, true, 26, 3},
    {"with glyph (positions and) clusters, render, second render() while in progress reallocates glyphs",
        RendererCoreFlag::GlyphClusters,
        3, 3,  0, 0, true, true, false, true, 26, 3}
};

const struct {
    TestSuite::TestCaseDescriptionSourceLocation name;
    RendererCoreFlags flags;
    UnsignedInt reserve, secondReserve;
    bool render, renderAddOnly, expectNoReallocation;
    UnsignedInt
        positionSize,
        idSize,
        clusterSize,
        advanceSize,
        expectedCapacity;
} AllocateCoreGlyphAllocatorData[]{
    {"second reserve() same as first",
        {}, 26, 26, false, false, true,
        0, 0, 0, 0, 26},
    {"second reserve() smaller than first",
        {}, 26, 23, false, false, true,
        0, 0, 0, 0, 26},
    {"second reserve() reallocates, positions smallest",
        {}, 3, 26, false, false, false,
        27, 30, 0, 28, 27},
    {"second reserve() reallocates, IDs smallest",
        {}, 3, 26, false, false, false,
        29, 28, 0, 30, 28},
    {"second reserve() reallocates, advances smallest",
        {}, 3, 26, false, false, false,
        31, 30, 0, 29, 29},
    {"render",
        {}, 26, 26, true, false, true,
        0, 0, 0, 0, 26},
    {"render, second render() reallocates, positions smallest",
        {}, 3, 26, true, false, false,
        /* Size of advances excludes the already-rendered glyphs, same below */
        27, 30, 0, 28 - 3, 27},
    {"render, second render() reallocates, IDs smallest",
        {}, 3, 26, true, false, false,
        28, 27, 0, 30 - 3, 27},
    {"render, second render() reallocates, advances smallest",
        {}, 3, 26, true, false, false,
        31, 32, 0, 30 - 3, 30},
    {"render, second render() while in progress reallocates",
        {}, 3, 26, true, true, false,
        26, 26, 0, 26 - 3, 26},
    {"with glyph clusters, second reserve() same as first",
        RendererCoreFlag::GlyphClusters, 26, 26, false, false, true,
        0, 0, 0, 0, 26},
    {"with glyph clusters, second reserve() reallocates, IDs smallest",
        RendererCoreFlag::GlyphClusters, 3, 26, false, false, false,
        28, 27, 32, 30, 27},
    {"with glyph clusters, second reserve() reallocates, clusters smallest",
        RendererCoreFlag::GlyphClusters, 3, 26,  false, false, false,
        30, 28, 27, 32, 27},
    {"with glyph clusters, render",
        RendererCoreFlag::GlyphClusters, 26, 26, true, false, true,
        0, 0, 0, 0, 26},
    {"with glyph clusters, second render() reallocates, IDs smallest",
        RendererCoreFlag::GlyphClusters, 3, 26, true, false, false,
        /* Size of advances excludes the already-rendered glyphs, same below */
        28, 27, 32, 30 - 3, 27},
    {"with glyph clusters, second render() reallocates, clusters smallest",
        RendererCoreFlag::GlyphClusters, 3, 26,  true, false, false,
        30, 28, 27, 32 - 3, 27},
    {"with glyph clusters, second render() while in progress reallocates",
        RendererCoreFlag::GlyphClusters, 3, 26,  true, true, false,
        26, 26, 26, 26 - 3, 26}
};

const struct {
    TestSuite::TestCaseDescriptionSourceLocation name;
    RendererCoreFlags flags;
    bool render;
    std::size_t positionSize, idSize, clusterSize, advanceSize;
    const char* expected;
} AllocateCoreGlyphAllocatorInvalidData[]{
    {"reserve, positions too small",
        {}, false, 16, 17, 0, 8,
        "Text::RendererCore::reserve(): expected allocated glyph positions and IDs to have at least 17 elements and advances 7 but got 16, 17 and 8\n"},
    {"render, positions too small",
        {}, true, 16, 17, 0, 8,
        "Text::RendererCore::add(): expected allocated glyph positions and IDs to have at least 17 elements and advances 7 but got 16, 17 and 8\n"},
    {"reserve, IDs too small",
        {}, false, 20, 16, 0, 7,
        "Text::RendererCore::reserve(): expected allocated glyph positions and IDs to have at least 17 elements and advances 7 but got 20, 16 and 7\n"},
    {"render, IDs too small",
        {}, true, 20, 16, 0, 7,
        "Text::RendererCore::add(): expected allocated glyph positions and IDs to have at least 17 elements and advances 7 but got 20, 16 and 7\n"},
    {"reserve, advances too small",
        {}, false, 17, 20, 0, 6,
        "Text::RendererCore::reserve(): expected allocated glyph positions and IDs to have at least 17 elements and advances 7 but got 17, 20 and 6\n"},
    {"reserve, advances too small",
        {}, true, 17, 20, 0, 6,
        "Text::RendererCore::add(): expected allocated glyph positions and IDs to have at least 17 elements and advances 7 but got 17, 20 and 6\n"},
    {"with glyph clusters, reserve, IDs too small",
        RendererCoreFlag::GlyphClusters, false, 20, 16, 18, 7,
        "Text::RendererCore::reserve(): expected allocated glyph positions, IDs and clusters to have at least 17 elements and advances 7 but got 20, 16, 18 and 7\n"},
    {"with glyph clusters, render, IDs too small",
        RendererCoreFlag::GlyphClusters, true, 20, 16, 18, 7,
        "Text::RendererCore::add(): expected allocated glyph positions, IDs and clusters to have at least 17 elements and advances 7 but got 20, 16, 18 and 7\n"},
    {"with glyph clusters, reserve, clusters too small",
        RendererCoreFlag::GlyphClusters, false, 17, 20, 16, 9,
        "Text::RendererCore::reserve(): expected allocated glyph positions, IDs and clusters to have at least 17 elements and advances 7 but got 17, 20, 16 and 9\n"},
    {"with glyph clusters, render, clusters too small",
        RendererCoreFlag::GlyphClusters, true, 17, 20, 16, 9,
        "Text::RendererCore::add(): expected allocated glyph positions, IDs and clusters to have at least 17 elements and advances 7 but got 17, 20, 16 and 9\n"},
};

const struct {
    TestSuite::TestCaseDescriptionSourceLocation name;
    UnsignedInt reserve, secondReserve;
    bool render, renderAddOnly, expectNoReallocation;
    UnsignedInt
        scaleSize,
        endSize,
        expectedCapacity;
} AllocateCoreRunAllocatorData[]{
    {"second reserve() same as first",
        5, 5, false, false, true,
        0, 0, 5},
    {"second reserve() smaller than first",
        5, 3, false, false, true,
        0, 0, 5},
    {"second reserve() reallocates, scales smallest",
        3, 5, false, false, false,
        7, 8, 7},
    {"second reserve() reallocates, ends smallest",
        3, 5, false, false, false,
        7, 6, 6},
    {"render",
        5, 5, true, false, true,
        0, 0, 5},
    {"render, second render() reallocates, scales smallest",
        3, 5, true, false, false,
        7, 8, 7},
    {"render, second render() reallocates, ends smallest",
        3, 5, true, false, false,
        7, 6, 6},
    {"render, second render() reallocates while in progress",
        3, 5, true, true, false,
        5, 5, 5},
};

const struct {
    TestSuite::TestCaseDescriptionSourceLocation name;
    bool render;
    std::size_t scaleSize, endSize;
    const char* expected;
} AllocateCoreRunAllocatorInvalidData[]{
    {"reserve, scales too small",
        false, 3, 5,
        "Text::RendererCore::reserve(): expected allocated run scales and ends to have at least 5 elements but got 3 and 5\n"},
    {"render, scales too small",
        true, 4, 5,
        "Text::RendererCore::add(): expected allocated run scales and ends to have at least 5 elements but got 4 and 5\n"},
    {"reserve, ends too small",
        false, 5, 3,
        "Text::RendererCore::reserve(): expected allocated run scales and ends to have at least 5 elements but got 5 and 3\n"},
    {"render, ends too small",
        true, 5, 4,
        "Text::RendererCore::add(): expected allocated run scales and ends to have at least 5 elements but got 5 and 4\n"},
};

const struct {
    TestSuite::TestCaseDescriptionSourceLocation name;
    /* Char begin, end, size multiplier */
    Containers::Array<Containers::Triple<UnsignedInt, UnsignedInt, Float>> items;
    RendererCoreFlags flags;
    Alignment alignment;
    ShapeDirection shapeDirection;
    UnsignedInt advertiseShapeDirectionAt;
    bool direct;
    Float expectedRectHeight;
    Containers::Array<Containers::Pair<Float, UnsignedInt>> expectedRuns;
    UnsignedInt expectedGlyphIds[10];
} AddSingleLineData[]{
    {"all at once", {InPlaceInit, {
        {3, 8, 1.0f},
    }}, {}, Alignment::LineRight, ShapeDirection{}, 0, false, 24.0f, {InPlaceInit, {
        {1.0f, 10}
    }}, {
     /* H  h  E  e  L  l  L  l  O  o */
        4, 6, 2, 1, 5, 3, 5, 3, 7, 8
    }},
    {"all at once, direct render()", {InPlaceInit, {
        {0, 5, 1.0f},
    }}, {}, Alignment::LineRight, ShapeDirection{}, 0, true, 24.0f, {InPlaceInit, {
        {1.0f, 10}
    }}, {
        4, 6, 2, 1, 5, 3, 5, 3, 7, 8
    }},
    {"all at once, with glyph clusters", {InPlaceInit, {
        {3, 8, 1.0f},
    }}, RendererCoreFlag::GlyphClusters, Alignment::LineRight, ShapeDirection{}, 0, false, 24.0f, {InPlaceInit, {
        {1.0f, 10}
    }}, {
        4, 6, 2, 1, 5, 3, 5, 3, 7, 8
    }},
    {"all at once, with glyph clusters, direct render()", {InPlaceInit, {
        {0, 5, 1.0f},
    }}, RendererCoreFlag::GlyphClusters, Alignment::LineRight, ShapeDirection{}, 0, true, 24.0f, {InPlaceInit, {
        {1.0f, 10}
    }}, {
        4, 6, 2, 1, 5, 3, 5, 3, 7, 8
    }},
    /* Direction-based alignment resolve, should end up being LineRight in all
       cases */
    {"all at once, top begin, RTL", {InPlaceInit, {
        {3, 8, 1.0f},
    }}, {}, Alignment::LineBegin, ShapeDirection::RightToLeft, 3, false, 24.0f, {InPlaceInit, {
        {1.0f, 10}
    }}, {
        4, 6, 2, 1, 5, 3, 5, 3, 7, 8
    }},
    {"all at once, top end, LTR", {InPlaceInit, {
        {3, 8, 1.0f},
    }}, {}, Alignment::LineEnd, ShapeDirection::LeftToRight, 3, false, 24.0f, {InPlaceInit, {
        {1.0f, 10}
    }}, {
        4, 6, 2, 1, 5, 3, 5, 3, 7, 8
    }},
    {"all at once, top end, unspecified", {InPlaceInit, {
        {3, 8, 1.0f},
    }}, {}, Alignment::LineEnd, ShapeDirection::Unspecified, 0, false, 24.0f, {InPlaceInit, {
        {1.0f, 10}
    }}, {
        4, 6, 2, 1, 5, 3, 5, 3, 7, 8
    }},
    /* The direction should only affect Start / End alignment */
    {"all at once, top right, RTL", {InPlaceInit, {
        {3, 8, 1.0f},
    }}, {}, Alignment::LineRight, ShapeDirection::RightToLeft, 3, false, 24.0f, {InPlaceInit, {
        {1.0f, 10}
    }}, {
        4, 6, 2, 1, 5, 3, 5, 3, 7, 8
    }},
    /* These verify that submission in parts doesn't cause problems */
    {"three parts", {InPlaceInit, {
        {3, 5, 1.0f},
        {5, 7, 2.0f},
        {7, 8, 1.0f},
    }}, {}, Alignment::LineRight, ShapeDirection{}, 0, false, 24.0f, {InPlaceInit, {
        {1.0f, 4},
        {1.0f, 8},
        {1.0f, 10},
    }}, {
     /* H  h  E  e  L   l   L   l   O  o
        first ----  second -------  first */
        4, 6, 2, 1, 13, 11, 13, 11, 7, 8
    }},
    {"three parts, with glyph clusters", {InPlaceInit, {
        {3, 5, 1.0f},
        {5, 7, 2.0f},
        {7, 8, 1.0f},
    }}, RendererCoreFlag::GlyphClusters, Alignment::LineRight, ShapeDirection{}, 0, false, 24.0f, {InPlaceInit, {
        {1.0f, 4},
        {1.0f, 8},
        {1.0f, 10}
    }}, {
        4, 6, 2, 1, 13, 11, 13, 11, 7, 8
    }},
    /* These verify that direction-based alignment resolve works no matter when
       it happens on given line */
    {"three parts, top begin, RTL, detected at the begining", {InPlaceInit, {
        {3, 5, 1.0f},
        {5, 7, 2.0f},
        {7, 8, 1.0f},
    }}, {}, Alignment::LineBegin, ShapeDirection::RightToLeft, 3, false, 24.0f, {InPlaceInit, {
        {1.0f, 4},
        {1.0f, 8},
        {1.0f, 10},
    }}, {
        4, 6, 2, 1, 13, 11, 13, 11, 7, 8
    }},
    {"three parts, top begin, RTL, detected at the end", {InPlaceInit, {
        {3, 5, 1.0f},
        {5, 7, 2.0f},
        {7, 8, 1.0f},
    }}, {}, Alignment::LineBegin, ShapeDirection::RightToLeft, 7, false, 24.0f, {InPlaceInit, {
        {1.0f, 4},
        {1.0f, 8},
        {1.0f, 10},
    }}, {
        4, 6, 2, 1, 13, 11, 13, 11, 7, 8
    }},
    /* Empty parts shouldn't affect anything */
    {"empty parts", {InPlaceInit, {
        {3, 3, 1.0f},
        {3, 6, 2.0f},
        {6, 6, 1.0f},
        {6, 8, 2.0f},
        {8, 8, 1.0f}
    }}, {}, Alignment::LineRight, ShapeDirection{}, 0, false, 24.0f, {InPlaceInit, {
        {1.0f, 6},
        {1.0f, 10},
    }}, {
     /* H   h   E   e  L   l   L   l   O  o
        second ------------------------------ */
        12, 14, 10, 9, 13, 11, 13, 11, 15, 16
    }},
    /* These verify that scaling is correctly accounted for */
    {"first part with taller font", {InPlaceInit, {
        {3, 5, 5.0f},
        {5, 7, 2.0f},
        {7, 8, 1.0f},
    }}, {}, Alignment::LineRight, ShapeDirection{}, 0, false, 120.0f, {InPlaceInit, {
        {5.0f, 4},
        {1.0f, 8},
        {1.0f, 10},
    }}, {
     /* H  h  E  e  L   l   L   l   O  o
        first ----  second -------  first */
        4, 6, 2, 1, 13, 11, 13, 11, 7, 8
    }},
    {"all but last part with shorter font", {InPlaceInit, {
        {3, 5, 0.5f},
        {5, 7, 1.0f},
        {7, 8, 0.75f},
    }}, {}, Alignment::LineRight, ShapeDirection{}, 0, false, 18.0f, {InPlaceInit, {
        {0.5f, 4},
        {0.5f, 8},
        {0.75f, 10},
    }}, {
     /* H  h  E  e  L   l   L   l   O  o
        first ----  second -------  first */
        4, 6, 2, 1, 13, 11, 13, 11, 7, 8
    }},
    /* Empty parts have their font metrics ignored */
    {"an empty part with taller font", {InPlaceInit, {
        {3, 5, 1.0f},
        {5, 5, 10.0f},
        {5, 8, 1.0f},
    }}, {}, Alignment::LineRight, ShapeDirection{}, 0, false, 24.0f, {InPlaceInit, {
        {1.0f, 4},
        {1.0f, 10},
    }}, {
        4, 6, 2, 1, 5, 3, 5, 3, 7, 8
    }},
    {"all at once, direct render(), with taller font", {InPlaceInit, {
        {0, 5, 5.0f},
    }}, {}, Alignment::LineRight, ShapeDirection{}, 0, true, 120.0f, {InPlaceInit, {
        {5.0f, 10}
    }}, {
        4, 6, 2, 1, 5, 3, 5, 3, 7, 8
    }},
};

const struct {
    TestSuite::TestCaseDescriptionSourceLocation name;
    Alignment alignment;
    ShapeDirection shapeDirection;
    Vector2 offset;
} AddSingleLineAlignData[]{
    /* The individual alignment values are tested in alignLine() and
       alignBlock() already, here just making sure that the output makes sense
       when everything is combined together, including shape direction */
    {"line left",
        Alignment::LineLeft, ShapeDirection::Unspecified,
        /* This is the default (0) value, thus should result in no shift */
        {}},
    {"top right",
        Alignment::TopRight, ShapeDirection::Unspecified,
        /* Advances were 1, 2, 3, so 6 in total, ascent is 4.5; scaled by
           0.5 */
        {-3.0f, -2.25f}},
    {"middle left, glyph bounds, integral",
        Alignment::MiddleLeftGlyphBoundsIntegral, ShapeDirection::Unspecified,
        /* The X shift isn't whole units but only Y is rounded here */
        {-2.5f, -7.0f}},
    {"bottom center, integral",
        Alignment::BottomCenterIntegral, ShapeDirection::Unspecified,
        /* The Y shift isn't whole units but only X is rounded here */
        {-2.0f, 1.25f}},
    {"line right",
        Alignment::LineRight, ShapeDirection::Unspecified,
        {-3.0f, 0.0f}},
    {"line begin, RTL",
        Alignment::LineBegin, ShapeDirection::RightToLeft,
        {-3.0f, 0.0f}}, /* Same as line right */
};

const struct {
    TestSuite::TestCaseDescriptionSourceLocation name;
    /* Char begin, end, actual begin/end passed to the shaper. Cannot use a
       nested Array because construction from a r-value array is broken on MSVC
       2017, so it's Array3 instead with a default-constructed suffix if only
       0, 1 or 2 calls are done. Ugh. */
    Containers::Array<Containers::Triple<UnsignedInt, UnsignedInt, Containers::Array3<Containers::Pair<UnsignedInt, UnsignedInt>>>> items;
    RendererCoreFlags flags;
    Alignment alignment;
    ShapeDirection shapeDirection;
    UnsignedInt advertiseShapeDirectionAt;
    bool direct;
    Float lineAdvance;
    Float expectedLineAdvance, expectedRectHeight;
    Containers::Array<Containers::Pair<Float, UnsignedInt>> expectedRuns;
    UnsignedInt expectedGlyphIds[10];
} AddMultipleLinesData[]{
    /* These verify only what's not already sufficiently tested in
       AddSingleLineData */
    {"all at once", {InPlaceInit, {
        {3, 11, {{{3, 5}, {6, 8}, {10, 11}}}}
    }}, {}, Alignment::LineRight, ShapeDirection{}, 0, false, 0.0f, 32.0f, 3*32.0f + 24.0f, {InPlaceInit, {
        {1.0f, 10}
    }}, {
     /* H  h  E  e  L  l  L  l  O  o */
        4, 6, 2, 1, 5, 3, 5, 3, 7, 8
    }},
    {"all at once, direct render()", {InPlaceInit, {
        {0, 8, {{{0, 2}, {3, 5}, {7, 8}}}}
    }}, {}, Alignment::LineRight, ShapeDirection{}, 0, true, 0.0f, 32.0f, 3*32.0f + 24.0f, {InPlaceInit, {
        {1.0f, 10}
    }}, {
        4, 6, 2, 1, 5, 3, 5, 3, 7, 8
    }},
    {"all at once, with glyph clusters", {InPlaceInit, {
        {3, 11, {{{3, 5}, {6, 8}, {10, 11}}}}
    }}, RendererCoreFlag::GlyphClusters, Alignment::LineRight, ShapeDirection{}, 0, false, 0.0f, 32.0f, 3*32.0f + 24.0f, {InPlaceInit, {
        {1.0f, 10}
    }}, {
        4, 6, 2, 1, 5, 3, 5, 3, 7, 8
    }},
    {"all at once, with glyph clusters, direct render()", {InPlaceInit, {
        {0, 8, {{{0, 2}, {3, 5}, {7, 8}}}}
    }}, RendererCoreFlag::GlyphClusters, Alignment::LineRight, ShapeDirection{}, 0, true, 0.0f, 32.0f, 3*32.0f + 24.0f, {InPlaceInit, {
        {1.0f, 10}
    }}, {
        4, 6, 2, 1, 5, 3, 5, 3, 7, 8
    }},
    {"each line separately with \\n at the end", {InPlaceInit, {
        { 3,  6, {{{3, 5}, {}, {}}}}, /* he\n */
        { 6, 10, {{{6, 8}, {}, {}}}}, /* ll\n\n */
        {10, 11, {{{10, 11}, {}, {}}}}, /* o */
    }}, {}, Alignment::LineRight, ShapeDirection{}, 0, false, 0.0f, 32.0f, 3*32.0f + 24.0f, {InPlaceInit, {
        {1.0f, 4},
        {0.5f, 8},
        {1.0f, 10},
    }}, {
     /* H  h  E  e  L   l   L   l   O  o
        first ----  second -------  first */
        4, 6, 2, 1, 13, 11, 13, 11, 7, 8
    }},
    {"each successive line separately with \\n at the beginning", {InPlaceInit, {
        { 3,  5, {{{3, 5}, {}, {}}}}, /* he */
        { 5,  8, {{{6, 8}, {}, {}}}}, /* \nll */
        { 8, 11, {{{10, 11}, {}, {}}}}, /* \n\no */
    }}, {}, Alignment::LineRight, ShapeDirection{}, 0, false, 0.0f, 32.0f, 3*32.0f + 24.0f, {InPlaceInit, {
        {1.0f, 4},
        {0.5f, 8},
        {1.0f, 10},
    }}, {
     /* H  h  E  e  L   l   L   l   O  o
        first ----  second -------  first */
        4, 6, 2, 1, 13, 11, 13, 11, 7, 8
    }},
    {"\\n alone", {InPlaceInit, {
        { 3,  5, {{{3, 5}, {}, {}}}}, /* he */
        { 5,  6, {}}, /* \n */
        { 6,  8, {{{6, 8}, {}, {}}}}, /* ll */
        { 8,  9, {}}, /* \n */
        { 9, 10, {}}, /* \n */
        {10, 11, {{{10, 11}, {}, {}}}}, /* o */
    }}, {}, Alignment::LineRight, ShapeDirection{}, 0, false, 0.0f, 32.0f, 3*32.0f + 24.0f, {InPlaceInit, {
        {1.0f, 4},
        {1.0f, 8},
        {0.5f, 10},
    }}, {
     /* H  h  E  e  L  l  L  l  O  o
        first ----------------  second */
        4, 6, 2, 1, 5, 3, 5, 3, 15, 16
    }},
    {"\\n alone and completely empty parts mixed", {InPlaceInit, {
        { 3,  6, {{{3, 5}, {}, {}}}},   /* he\n */
        { 6,  6, {}},
        { 6,  8, {{{6, 8}, {}, {}}}},   /* ll */
        { 8,  9, {}}, /* \n */
        { 9,  9, {}},
        { 9, 10, {}}, /* \n */
        {10, 11, {{{10, 11}, {}, {}}}}, /* o */
    }}, {}, Alignment::LineRight, ShapeDirection{}, 0, false, 0.0f, 32.0f, 3*32.0f + 24.0f, {InPlaceInit, {
        {1.0f, 4},
        {1.0f, 8},
        {1.0f, 10},
    }}, {
     /* H  h  E  e  L  l  L  l  O  o */
        4, 6, 2, 1, 5, 3, 5, 3, 7, 8
    }},
    {"continuing from the middle of a line", {InPlaceInit, {
        { 3,  4, {{{3, 4}, {}, {}}}},       /* h */
        { 4,  7, {{{4, 5}, {6, 7}, {}}}},   /* e\nl */
        { 7, 11, {{{7, 8}, {10, 11}, {}}}}, /* l\n\no */
    }}, {}, Alignment::LineRight, ShapeDirection{}, 0, false, 0.0f, 32.0f, 3*32.0f + 24.0f, {InPlaceInit, {
        {1.0f, 2},
        {0.5f, 6},
        {1.0f, 10},
    }}, {
     /* H  h  E   e  L   l   L  l  O  o
        first second ------  first ---- */
        4, 6, 10, 9, 13, 11, 5, 3, 7, 8
    }},
    /* This should correctly make it LineRight */
    {"each line separately, RTL", {InPlaceInit, {
        { 3,  6, {{{3, 5}, {}, {}}}}, /* he\n */
        { 6, 10, {{{6, 8}, {}, {}}}}, /* ll\n\n */
        {10, 11, {{{10, 11}, {}, {}}}}, /* o */
    }}, {}, Alignment::LineBegin, ShapeDirection::RightToLeft, 3, false, 0.0f, 32.0f, 3*32.0f + 24.0f, {InPlaceInit, {
        {1.0f, 4},
        {0.5f, 8},
        {1.0f, 10},
    }}, {
     /* H  h  E  e  L   l   L   l   O  o
        first ----  second -------  first */
        4, 6, 2, 1, 13, 11, 13, 11, 7, 8
    }},
    /* These two should fall back to Unspecified for the first line, resulting
       in LineRight even though on the second line it'd resolve to LineLeft */
    {"each line separately with \\n at the end, RTL detected only at the second line", {InPlaceInit, {
        { 3,  6, {{{3, 5}, {}, {}}}}, /* he\n */
        { 6, 10, {{{6, 8}, {}, {}}}}, /* ll\n\n */
        {10, 11, {{{10, 11}, {}, {}}}}, /* o */
    }}, {}, Alignment::LineEnd, ShapeDirection::RightToLeft, 6, false, 0.0f, 32.0f, 3*32.0f + 24.0f, {InPlaceInit, {
        {1.0f, 4},
        {0.5f, 8},
        {1.0f, 10},
    }}, {
     /* H  h  E  e  L   l   L   l   O  o
        first ----  second -------  first */
        4, 6, 2, 1, 13, 11, 13, 11, 7, 8
    }},
    {"each successive line separately with \\n at the beginning, RTL detected at the second line", {InPlaceInit, {
        { 3,  5, {{{3, 5}, {}, {}}}}, /* he */
        { 5,  8, {{{6, 8}, {}, {}}}}, /* \nll */
        { 8, 11, {{{10, 11}, {}, {}}}}, /* \n\no */
    }}, {}, Alignment::LineEnd, ShapeDirection::RightToLeft, 5, false, 0.0f, 32.0f, 3*32.0f + 24.0f, {InPlaceInit, {
        {1.0f, 4},
        {0.5f, 8},
        {1.0f, 10},
    }}, {
     /* H  h  E  e  L   l   L   l   O  o
        first ----  second -------  first */
        4, 6, 2, 1, 13, 11, 13, 11, 7, 8
    }},
    /* Overriding line advance */
    {"all at once, custom line advance", {InPlaceInit, {
        {3, 11, {{{3, 5}, {6, 8}, {10, 11}}}}
    }}, {}, Alignment::LineRight, ShapeDirection{}, 0, false, 29.0f, 29.0f, 3*29.0f + 24.0f, {InPlaceInit, {
        {1.0f, 10}
    }}, {
     /* H  h  E  e  L  l  L  l  O  o */
        4, 6, 2, 1, 5, 3, 5, 3, 7, 8
    }},
    {"each line separately, custom line advance", {InPlaceInit, {
        { 3,  6, {{{3, 5}, {}, {}}}}, /* he\n */
        { 6, 10, {{{6, 8}, {}, {}}}}, /* ll\n\n */
        {10, 11, {{{10, 11}, {}, {}}}}, /* o */
    }}, {}, Alignment::LineRight, ShapeDirection{}, 0, false, 29.0f, 29.0f, 3*29.0f + 24.0f, {InPlaceInit, {
        {1.0f, 4},
        {0.5f, 8},
        {1.0f, 10},
    }}, {
     /* H  h  E  e  L   l   L   l   O  o
        first ----  second -------  first */
        4, 6, 2, 1, 13, 11, 13, 11, 7, 8
    }},
};

const struct {
    TestSuite::TestCaseDescriptionSourceLocation name;
    Alignment alignment;
    /* The Y offset value could be calculated, but this is easier to grasp and
       makes it possible to test overrideable line height later, for example */
    Vector2 offset0, offset1, offset2;
} AddMultipleLinesAlignData[]{
    /* The individual alignment values are tested in alignLine() and
       alignBlock() already, here just making sure that the output makes sense
       when everything is combined together */
    {"line left", Alignment::LineLeft,
        {0.0f, -0.0f},
        {0.0f, -4.0f},
        {0.0f, -12.0f}},
    {"bottom right, glyph bounds", Alignment::BottomRightGlyphBounds,
        {-7.0f, 12.0f},
        {-3.0f, 8.0f},
        {-5.0f, 0.0f}},
    {"middle center, glyph bounds, integral", Alignment::MiddleCenterGlyphBoundsIntegral,
        {-4.0f, 6.0f},
        {-2.0f, 2.0f},
        {-3.0f, -6.0f}},
    {"top right", Alignment::TopRight,
        {-8.0f, -0.5f},
        {-4.0f, -4.5f},
        {-6.0f, -12.5f}},
};

const struct {
    const char* name;
    RendererCoreFlags flags;
} MultipleBlocksData[]{
    {"", {}},
    {"with glyph clusters", RendererCoreFlag::GlyphClusters}
};

const struct {
    const char* name;
    RendererCoreFlags flags;
    bool renderAddOnly, reset;
    UnsignedInt expectedBuiltinGlyphAllocatorCapacity;
} ClearResetCoreData[]{
    /* After clear() it needs more space for the advances, so the capacity will
       not be 3 even though it contained 3 glyphs before */
    {"clear", {}, false, false, 2},
    /* Here the glyph advances alias other memory so 3 can fit */
    {"clear, with glyph clusters", RendererCoreFlag::GlyphClusters, false, false, 3},
    {"reset", {}, false, true, 2},
    {"clear while in progress", {}, true, false, 2},
    /* Here the glyph advances alias other memory so 3 can fit */
    {"clear while in progress, with glyph clusters", RendererCoreFlag::GlyphClusters, true, false, 3},
    {"reset while in progress", {}, true, true, 2},
};

#ifdef MAGNUM_TARGET_GL
const struct {
    TestSuite::TestCaseDescriptionSourceLocation name;
    Alignment alignment;
    ShapeDirection shapeDirection;
    Vector2 offset;
} RenderDataData[]{
    /* Not testing all combinations, just making sure that each horizontal,
       vertical, glyph bounds and integer variant is covered */
    {"line left",
        Alignment::LineLeft, ShapeDirection::Unspecified,
        /* This is the default (0) value, thus should result in no shift */
        {}},
    {"line left, glyph bounds",
        Alignment::LineLeftGlyphBounds, ShapeDirection::Unspecified,
        /* The first glyph has X offset of 2.5, which is subtracted */
        {-2.5f, 0.0f}},
    {"top left",
        Alignment::TopLeft, ShapeDirection::Unspecified,
        /* Ascent is 4.5, scaled by 0.5 */
        {0.0f, -2.25f}},
    {"top left, glyph bounds",
        Alignment::TopLeftGlyphBounds, ShapeDirection::Unspecified,
        /* Largest Y value is 10.5f */
        {-2.5f, -10.5f}},
    {"top right",
        Alignment::TopRight, ShapeDirection::Unspecified,
        /* Advances were 1, 2, 3, so 6 in total, ascent is 4.5; scaled by
           0.5 */
        {-3.0f, -2.25f}},
    {"top right, glyph bounds",
        Alignment::TopRightGlyphBounds, ShapeDirection::Unspecified,
        /* Basically subtracting the largest vertex value */
        {-12.5f, -10.5f}},
    {"top center",
        Alignment::TopCenter, ShapeDirection::Unspecified,
        /* Advances were 1, 2, 3, so 6 in total, center is 3, scaled by 0.5 */
        {-1.5f, -2.25f}},
    {"top center, integral",
        Alignment::TopCenterIntegral, ShapeDirection::Unspecified,
        /* The Y shift isn't whole units but only X is rounded here */
        {-2.0f, -2.25f}},
    {"top center, glyph bounds",
        Alignment::TopCenterGlyphBounds, ShapeDirection::Unspecified,
        {-7.5f, -10.5f}},
    {"top center, glyph bounds, integral",
        Alignment::TopCenterGlyphBoundsIntegral, ShapeDirection::Unspecified,
        /* The Y shift isn't whole units but only X is rounded here */
        {-8.0f, -10.5f}},
    {"middle left, glyph bounds",
        Alignment::MiddleLeftGlyphBounds, ShapeDirection::Unspecified,
        {-2.5f, -7.125f}},
    {"middle left, glyph bounds, integral",
        Alignment::MiddleLeftGlyphBoundsIntegral, ShapeDirection::Unspecified,
        /* The X shift isn't whole units but only Y is rounded here */
        {-2.5f, -7.0f}},
    {"middle center",
        Alignment::MiddleCenter, ShapeDirection::Unspecified,
        {-1.5f, -0.5f}},
    {"middle center, integral",
        Alignment::MiddleCenterIntegral, ShapeDirection::Unspecified,
        /* Rounding happens on both X and Y in this case */
        {-2.0f, -1.0f}},
    {"middle center, glyph bounds",
        Alignment::MiddleCenterGlyphBounds, ShapeDirection::Unspecified,
        /* Half size of the bounds quad */
        {-7.5f, -7.125f}},
    {"middle center, glyph bounds, integral",
        Alignment::MiddleCenterGlyphBoundsIntegral, ShapeDirection::Unspecified,
        {-8.0f, -7.0f}},
    {"bottom left",
        Alignment::BottomLeft, ShapeDirection::Unspecified,
        /* Descent is -2.5; scaled by 0.5 */
        {0.0f, 1.25f}},
    {"bottom right",
        Alignment::BottomRight, ShapeDirection::Unspecified,
        {-3.0f, 1.25f}},
    {"bottom right, glyph bounds",
        Alignment::BottomRightGlyphBounds, ShapeDirection::Unspecified,
        {-12.5f, -3.75f}},
    {"line begin, direction unspecified",
        Alignment::LineBegin, ShapeDirection::Unspecified,
        {}}, /* Same as line left, thus no shift */
    {"bottom begin, LTR",
        Alignment::BottomBegin, ShapeDirection::LeftToRight,
        {0.0f, 1.25f}}, /* Same as bottom left */
    {"line end, RTL",
        Alignment::LineEnd, ShapeDirection::RightToLeft,
        {}}, /* Again same as line left, thus no shift */
    {"line end, direction unspecified",
        Alignment::LineEnd, ShapeDirection::Unspecified,
        {-3.0f, 0.0f}}, /* Same as line right */
    {"top end, LTR",
        Alignment::TopEnd, ShapeDirection::LeftToRight,
        {-3.0f, -2.25f}}, /* Same as top right */
    {"line begin, RTL",
        Alignment::LineBegin, ShapeDirection::RightToLeft,
        {-3.0f, 0.0f}}, /* Same as line right */
    {"line left, RTL",
        Alignment::LineLeft, ShapeDirection::RightToLeft,
        {}}, /* Line left with no change */
    {"middle center, RTL",
        Alignment::MiddleCenter, ShapeDirection::RightToLeft,
        {-1.5f, -0.5f}}, /* Middle center with no change */
};

const struct {
    const char* name;
    Alignment alignment;
    /* The Y offset value could be calculated, but this is easier to grasp and
       makes it possible to test overrideable line height later, for example */
    Vector2 offset0, offset1, offset2;
} MultilineData[]{
    {"line left", Alignment::LineLeft,
        {0.0f, -0.0f},
        {0.0f, -4.0f},
        {0.0f, -12.0f}},
    {"line left, glyph bounds", Alignment::LineLeftGlyphBounds,
        {0.0f, 0.0f},
        {0.0f, -4.0f},
        {0.0f, -12.0f}},
    {"middle left", Alignment::MiddleLeft,
        {0.0f, 6.0f},
        {0.0f, 2.0f},
        {0.0f, -6.0f}},
    {"middle left, glyph bounds", Alignment::MiddleLeftGlyphBounds,
        {0.0f, 5.5f},
        {0.0f, 1.5f},
        {0.0f, -6.5f}},
    {"middle left, glyph bounds, integral", Alignment::MiddleLeftGlyphBoundsIntegral,
        {0.0f, 6.0f},
        {0.0f, 2.0f},
        {0.0f, -6.0f}},
    {"middle center", Alignment::MiddleCenter,
        /* The advance for the rightmost glyph is one unit larger than the
           actual bounds which makes it different */
        {-4.0f, 6.0f},
        {-2.0f, 2.0f},
        {-3.0f, -6.0f}},
    {"middle center, integral", Alignment::MiddleCenterIntegral,
        {-4.0f, 6.0f},
        {-2.0f, 2.0f},
        {-3.0f, -6.0f}},
    {"middle center, glyph bounds", Alignment::MiddleCenterGlyphBounds,
        {-3.5f, 5.5f},
        {-1.5f, 1.5f},
        {-2.5f, -6.5f}},
    {"middle center, glyph bounds, integral", Alignment::MiddleCenterGlyphBoundsIntegral,
        {-4.0f, 6.0f},
        {-2.0f, 2.0f},
        {-3.0f, -6.0f}},
    {"top right", Alignment::TopRight,
        {-8.0f, -0.5f},
        {-4.0f, -4.5f},
        {-6.0f, -12.5f}},
    {"top right, glyph bounds", Alignment::TopRightGlyphBounds,
        {-7.0f, -1.0f},
        {-3.0f, -5.0f},
        {-5.0f, -13.0f}},
    {"top center", Alignment::TopCenter,
        /* The advance for the rightmost glyph is one unit larger than the
           actual bounds which makes it different */
        {-4.0f, -0.5f},
        {-2.0f, -4.5f},
        {-3.0f, -12.5f}},
    {"top center, integral", Alignment::TopCenterIntegral,
        /* The Y shift isn't whole units but only X (which is already whole
           units) would be rounded here */
        {-4.0f, -0.5f},
        {-2.0f, -4.5f},
        {-3.0f, -12.5f}},
    {"top center, glyph bounds", Alignment::TopCenterGlyphBounds,
        {-3.5f, -1.0f},
        {-1.5f, -5.0f},
        {-2.5f, -13.0f}},
    {"top center, integral", Alignment::TopCenterGlyphBoundsIntegral,
        {-4.0f, -1.0f},
        {-2.0f, -5.0f},
        {-3.0f, -13.0f}},
};
#endif

RendererTest::RendererTest() {
    addTests({&RendererTest::lineGlyphPositions,
              &RendererTest::lineGlyphPositionsAliasedViews,
              &RendererTest::lineGlyphPositionsInvalidViewSizes,
              &RendererTest::lineGlyphPositionsInvalidDirection,
              &RendererTest::lineGlyphPositionsNoFontOpened});

    addInstancedTests({&RendererTest::glyphQuads,
                       &RendererTest::glyphQuadsAliasedViews},
        Containers::arraySize(GlyphQuadsData));

    addTests({&RendererTest::glyphQuadsInvalidViewSizes,
              &RendererTest::glyphQuadsNoFontOpened,
              &RendererTest::glyphQuadsFontNotFoundInCache});

    addInstancedTests({&RendererTest::glyphQuads2D},
        Containers::arraySize(GlyphQuadsData));

    addTests({&RendererTest::glyphQuads2DArrayGlyphCache,

              &RendererTest::glyphQuadBounds,
              &RendererTest::glyphQuadBoundsInvalidViewSizes});

    addInstancedTests({&RendererTest::alignLine},
        Containers::arraySize(AlignLineData));

    addTests({&RendererTest::alignLineInvalidDirection});

    addInstancedTests({&RendererTest::alignBlock},
        Containers::arraySize(AlignBlockData));

    addTests({&RendererTest::alignBlockInvalidDirection,

              &RendererTest::glyphQuadIndices<UnsignedInt>,
              &RendererTest::glyphQuadIndices<UnsignedShort>,
              &RendererTest::glyphQuadIndices<UnsignedByte>,
              &RendererTest::glyphQuadIndicesTypeTooSmall});

    addInstancedTests({&RendererTest::glyphRangeForBytes},
        Containers::arraySize(GlyphRangeForBytesData));

    addTests({&RendererTest::debugFlagCore,
              &RendererTest::debugFlagsCore});

    addInstancedTests({&RendererTest::constructCore,
                       &RendererTest::constructCoreAllocator},
        Containers::arraySize(ConstructCoreData));

    addTests({&RendererTest::constructCoreNoCreate});

    addTests({&RendererTest::constructCopyCore,
              &RendererTest::constructMoveCore,

              &RendererTest::propertiesCore,
              &RendererTest::propertiesCoreInvalid,
              &RendererTest::propertiesCoreRenderingInProgress,

              &RendererTest::glyphsForRuns,
              &RendererTest::glyphsForRunsInvalid});

    addInstancedTests({&RendererTest::allocateCore},
        Containers::arraySize(AllocateData));

    addInstancedTests({&RendererTest::allocateCoreGlyphAllocator},
        Containers::arraySize(AllocateCoreGlyphAllocatorData));

    addInstancedTests({&RendererTest::allocateCoreGlyphAllocatorInvalid},
        Containers::arraySize(AllocateCoreGlyphAllocatorInvalidData));

    addInstancedTests({&RendererTest::allocateCoreRunAllocator},
        Containers::arraySize(AllocateCoreRunAllocatorData));

    addInstancedTests({&RendererTest::allocateCoreRunAllocatorInvalid},
        Containers::arraySize(AllocateCoreRunAllocatorInvalidData));

    addInstancedTests({&RendererTest::addSingleLine},
        Containers::arraySize(AddSingleLineData));

    addInstancedTests({&RendererTest::addSingleLineAlign},
        Containers::arraySize(AddSingleLineAlignData));

    addInstancedTests({&RendererTest::addMultipleLines},
        Containers::arraySize(AddMultipleLinesData));

    addInstancedTests({&RendererTest::addMultipleLinesAlign},
        Containers::arraySize(AddMultipleLinesAlignData));

    addTests({&RendererTest::addFontNotFoundInCache});

    addInstancedTests({&RendererTest::multipleBlocks},
        Containers::arraySize(MultipleBlocksData));

    addInstancedTests({&RendererTest::clearResetCore,
                       &RendererTest::clearResetCoreAllocators},
        Containers::arraySize(ClearResetCoreData));

    #ifdef MAGNUM_TARGET_GL
    addInstancedTests({&RendererTest::renderData},
        Containers::arraySize(RenderDataData));

    addInstancedTests({&RendererTest::multiline},
        Containers::arraySize(MultilineData));

    addTests({&RendererTest::arrayGlyphCache,
              &RendererTest::fontNotFoundInCache});
    #endif
}

/* Used by addSingleLineAlign() / addMultipleLinesAlign() */
struct TestShaper: AbstractShaper {
    explicit TestShaper(AbstractFont& font, ShapeDirection direction): AbstractShaper{font}, _direction{direction} {}

    UnsignedInt doShape(Containers::StringView text, UnsignedInt, UnsignedInt, Containers::ArrayView<const FeatureRange>) override {
        return text.size();
    }

    ShapeDirection doDirection() const override {
        return _direction;
    }

    void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>& ids) const override {
        for(UnsignedInt i = 0; i != ids.size(); ++i) {
            /* It just rotates between the three glyphs */
            if(i % 3 == 0)
                ids[i] = 3;
            else if(i % 3 == 1)
                ids[i] = 7;
            else
                ids[i] = 9;
        }
    }
    void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>& offsets, const Containers::StridedArrayView1D<Vector2>& advances) const override {
        for(UnsignedInt i = 0; i != offsets.size(); ++i) {
            /* Offset Y and advance X is getting larger with every glyph,
               advance Y is flipping its sign with every glyph */
            offsets[i] = Vector2::yAxis(i + 1);
            /* This is always to the right, independent of ShapeDirection */
            advances[i] = {Float(i + 1), i % 2 ? -0.5f : +0.5f};
        }
    }
    void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {
        /* Nothing in the renderer uses this API */
        CORRADE_FAIL("This shouldn't be called.");
    }

    ShapeDirection _direction;
};

struct TestFont: AbstractFont {
    FontFeatures doFeatures() const override { return {}; }

    bool doIsOpened() const override { return _opened; }
    void doClose() override { _opened = false; }

    Properties doOpenFile(Containers::StringView, Float size) override {
        _opened = true;
        /* Line height isn't used for anything here so can be arbitrary */
        return {size, 4.5f, -2.5f, 10000.0f, 10};
    }

    void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>& glyphs) override {
        for(UnsignedInt& i: glyphs)
            i = 0;
    }
    Vector2 doGlyphSize(UnsignedInt) override { return {}; }
    Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }

    Containers::Pointer<AbstractShaper> doCreateShaper() override {
        return Containers::pointer<TestShaper>(*this, direction);
    }

    ShapeDirection direction = ShapeDirection::Unspecified;

    bool _opened = false;
};

struct DummyGlyphCache: AbstractGlyphCache {
    using AbstractGlyphCache::AbstractGlyphCache;

    GlyphCacheFeatures doFeatures() const override { return {}; }
    void doSetImage(const Vector2i&, const ImageView2D&) override {}
};

DummyGlyphCache testGlyphCache(AbstractFont& font) {
    /* Default padding is 1 to avoid artifacts, set that to 0 to simplify */
    DummyGlyphCache cache{PixelFormat::R8Unorm, {20, 20}, {}};

    /* Add one more font to verify the right one gets picked */
    cache.addFont(96);
    UnsignedInt fontId = cache.addFont(font.glyphCount(), &font);

    /* Three glyphs, covering bottom, top right and top left of the cache.
       Adding them in a shuffled order to verify non-trivial font-specific to
       cache-global glyph mapping in glyphQuads() below. */
    cache.addGlyph(fontId, 3, {5, 10}, {{}, {20, 10}});
    cache.addGlyph(fontId, 9, {5, 5}, {{10, 10}, {20, 20}});
    cache.addGlyph(fontId, 7, {10, 5}, {{0, 10}, {10, 20}});

    return cache;
}

DummyGlyphCache testGlyphCacheArray(AbstractFont& font) {
    /* Default padding is 1 to avoid artifacts, set that to 0 to simplify */
    DummyGlyphCache cache{PixelFormat::R8Unorm, {20, 20, 3}, {}};

    /* Add one more font to verify the right one gets picked */
    cache.addFont(96);
    UnsignedInt fontId = cache.addFont(font.glyphCount(), &font);

    /* Three glyphs, covering bottom, top right and top left of the cache.
       Adding them in a shuffled order to verify non-trivial font-specific to
       cache-global glyph mapping in glyphQuads() below. */
    cache.addGlyph(fontId, 3, {5, 10}, 2, {{}, {20, 10}});
    cache.addGlyph(fontId, 9, {5, 5}, 1, {{10, 10}, {20, 20}});
    cache.addGlyph(fontId, 7, {10, 5}, 0, {{0, 10}, {10, 20}});

    return cache;
}

void RendererTest::lineGlyphPositions() {
    TestFont font;
    font.openFile({}, 2.5f);

    Vector2 glyphOffsets[]{
        {0.2f, -0.4f},
        {0.4f, 0.8f},
        {-0.2f, 0.4f},
    };
    Vector2 glyphAdvances[]{
        {1.0f, 0.0f},
        {2.0f, 0.2f},
        {3.0f, -0.2f}
    };
    Vector2 cursor{100.0f, 200.0f};

    /* The font is opened at 2.5, rendering at 1.25, so everything will be
       scaled by 0.5 */
    Vector2 glyphPositions[3];
    Range2D rectangle = renderLineGlyphPositionsInto(font, 1.25f, LayoutDirection::HorizontalTopToBottom, glyphOffsets, glyphAdvances, cursor, glyphPositions);
    /* The rectangle contains the cursor range and descent to ascent */
    CORRADE_COMPARE(rectangle, (Range2D{{100.0f, 198.75f}, {103.0f, 202.25}}));
    CORRADE_COMPARE(cursor, (Vector2{103.0f, 200.0f}));
    CORRADE_COMPARE_AS(Containers::arrayView(glyphPositions), Containers::arrayView<Vector2>({
        {100.1f, 199.8f},
        {100.7f, 200.4f},
        {101.4f, 200.3f}
    }), TestSuite::Compare::Container);
}

void RendererTest::lineGlyphPositionsAliasedViews() {
    /* Like lineGlyphPositions(), but with the input data stored in the output
       array. The internals should be written in a way that doesn't overwrite
       the input before it's read. */
    TestFont font;
    font.openFile({}, 2.5f);

    Vector2 glyphOffsetsPositions[]{
        {0.2f, -0.4f},
        {0.4f, 0.8f},
        {-0.2f, 0.4f},
    };
    Vector2 glyphAdvances[]{
        {1.0f, 0.0f},
        {2.0f, 0.2f},
        {3.0f, -0.2f}
    };
    Vector2 cursor{100.0f, 200.0f};

    Range2D rectangle = renderLineGlyphPositionsInto(font, 1.25f, LayoutDirection::HorizontalTopToBottom, glyphOffsetsPositions, glyphAdvances, cursor, glyphOffsetsPositions);
    CORRADE_COMPARE(rectangle, (Range2D{{100.0f, 198.75f}, {103.0f, 202.25}}));
    CORRADE_COMPARE(cursor, (Vector2{103.0f, 200.0f}));
    CORRADE_COMPARE_AS(Containers::arrayView(glyphOffsetsPositions), Containers::arrayView<Vector2>({
        {100.1f, 199.8f},
        {100.7f, 200.4f},
        {101.4f, 200.3f}
    }), TestSuite::Compare::Container);
}

void RendererTest::lineGlyphPositionsInvalidViewSizes() {
    CORRADE_SKIP_IF_NO_ASSERT();

    TestFont font;
    Vector2 data[5];
    Vector2 dataInvalid[4];
    Vector2 cursor;

    Containers::String out;
    Error redirectError{&out};
    renderLineGlyphPositionsInto(font, 10.0f, LayoutDirection::HorizontalTopToBottom, data, data, cursor, dataInvalid);
    renderLineGlyphPositionsInto(font, 10.0f, LayoutDirection::HorizontalTopToBottom, data, dataInvalid, cursor, data);
    renderLineGlyphPositionsInto(font, 10.0f, LayoutDirection::HorizontalTopToBottom, dataInvalid, data, cursor, data);
    CORRADE_COMPARE(out,
        "Text::renderLineGlyphPositionsInto(): expected glyphOffsets, glyphAdvances and output views to have the same size, got 5, 5 and 4\n"
        "Text::renderLineGlyphPositionsInto(): expected glyphOffsets, glyphAdvances and output views to have the same size, got 5, 4 and 5\n"
        "Text::renderLineGlyphPositionsInto(): expected glyphOffsets, glyphAdvances and output views to have the same size, got 4, 5 and 5\n");
}

void RendererTest::lineGlyphPositionsInvalidDirection() {
    CORRADE_SKIP_IF_NO_ASSERT();

    TestFont font;
    Vector2 cursor;

    Containers::String out;
    Error redirectError{&out};
    renderLineGlyphPositionsInto(font, 10.0f, LayoutDirection::VerticalLeftToRight, {}, {}, cursor, {});
    CORRADE_COMPARE(out, "Text::renderLineGlyphPositionsInto(): only Text::LayoutDirection::HorizontalTopToBottom is supported right now, got Text::LayoutDirection::VerticalLeftToRight\n");
}

void RendererTest::lineGlyphPositionsNoFontOpened() {
    CORRADE_SKIP_IF_NO_ASSERT();

    TestFont font;
    Vector2 cursor;

    Containers::String out;
    Error redirectError{&out};
    renderLineGlyphPositionsInto(font, 10.0f, LayoutDirection::HorizontalTopToBottom, {}, {}, cursor, {});
    CORRADE_COMPARE(out, "Text::renderLineGlyphPositionsInto(): no font opened\n");
}

void RendererTest::glyphQuads() {
    auto&& data = GlyphQuadsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    TestFont font;
    font.openFile({}, 2.5f);
    DummyGlyphCache cache = testGlyphCacheArray(font);

    Vector2 glyphPositions[]{
        {100.0f, 200.0f},
        {103.0f, 202.0f},
        {107.0f, 196.0f}
    };
    UnsignedInt fontGlyphIds[]{
        3, 7, 9
    };
    UnsignedInt glyphIds[]{
        /* Glyph 0 is the cache-global invalid glyph */
        1, 3, 2
    };

    Vector2 positions[3*4];
    Vector3 textureCoordinates[3*4];
    /* The font is opened at 2.5, rendering at 1.25, so everything will be
       scaled by 0.5 */
    Range2D rectangle = data.globalIds ?
        renderGlyphQuadsInto(cache, 1.25f/2.5f, glyphPositions, glyphIds, positions, textureCoordinates) :
        renderGlyphQuadsInto(font, 1.25f, cache, glyphPositions, fontGlyphIds, positions, textureCoordinates);
    CORRADE_COMPARE(rectangle, (Range2D{{102.5f, 198.5f}, {114.5f, 210.0f}}));

    /* 2---3
       |   |
       0---1 */
    CORRADE_COMPARE_AS(Containers::arrayView(positions), Containers::arrayView<Vector2>({
        {102.5f, 205.0f}, /* Offset {5, 10}, size {20, 10}, scaled by 0.5 */
        {112.5f, 205.0f},
        {102.5f, 210.0f},
        {112.5f, 210.0f},

        {108.0f, 204.5f}, /* Offset {10, 5}, size {10, 10}, scaled by 0.5 */
        {113.0f, 204.5f},
        {108.0f, 209.5f},
        {113.0f, 209.5f},

        {109.5f, 198.5f}, /* Offset {5, 5}, size {10, 10}, scaled by 0.5 */
        {114.5f, 198.5f},
        {109.5f, 203.5f},
        {114.5f, 203.5f},
    }), TestSuite::Compare::Container);

    /* First glyph is bottom, second top left, third top right; layer is
       different for each.

       +-+-+
       |b|c|
       2---3
       | a |
       0---1 */
    CORRADE_COMPARE_AS(Containers::arrayView(textureCoordinates), Containers::arrayView<Vector3>({
        {0.0f, 0.0f, 2.0f},
        {1.0f, 0.0f, 2.0f},
        {0.0f, 0.5f, 2.0f},
        {1.0f, 0.5f, 2.0f},

        {0.0f, 0.5f, 0.0f},
        {0.5f, 0.5f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.5f, 1.0f, 0.0f},

        {0.5f, 0.5f, 1.0f},
        {1.0f, 0.5f, 1.0f},
        {0.5f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},
    }), TestSuite::Compare::Container);
}

void RendererTest::glyphQuadsAliasedViews() {
    auto&& data = GlyphQuadsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Like lineGlyphPositions(), but with the input data stored in the output
       array. The internals should be written in a way that doesn't overwrite
       the input before it's read. */

    TestFont font;
    font.openFile({}, 2.5f);
    DummyGlyphCache cache = testGlyphCacheArray(font);

    Vector2 positions[3*4];
    Vector3 textureCoordinates[3*4];

    Containers::StridedArrayView1D<Vector2> glyphPositions = Containers::stridedArrayView(positions).every(4);
    Utility::copy({
        {100.0f, 200.0f},
        {103.0f, 202.0f},
        {107.0f, 196.0f}
    }, glyphPositions);

    Containers::StridedArrayView1D<UnsignedInt> glyphIds = Containers::arrayCast<UnsignedInt>(Containers::stridedArrayView(textureCoordinates).every(4));
    data.globalIds ?
        Utility::copy({1, 3, 2}, glyphIds) :
        Utility::copy({3, 7, 9}, glyphIds);

    Range2D rectangle = data.globalIds ?
        renderGlyphQuadsInto(cache, 1.25f/2.5f, glyphPositions, glyphIds, positions, textureCoordinates) :
        renderGlyphQuadsInto(font, 1.25f, cache, glyphPositions, glyphIds, positions, textureCoordinates);
    CORRADE_COMPARE(rectangle, (Range2D{{102.5f, 198.5f}, {114.5f, 210.0f}}));

    CORRADE_COMPARE_AS(Containers::arrayView(positions), Containers::arrayView<Vector2>({
        {102.5f, 205.0f},
        {112.5f, 205.0f},
        {102.5f, 210.0f},
        {112.5f, 210.0f},

        {108.0f, 204.5f},
        {113.0f, 204.5f},
        {108.0f, 209.5f},
        {113.0f, 209.5f},

        {109.5f, 198.5f},
        {114.5f, 198.5f},
        {109.5f, 203.5f},
        {114.5f, 203.5f},
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(Containers::arrayView(textureCoordinates), Containers::arrayView<Vector3>({
        {0.0f, 0.0f, 2.0f},
        {1.0f, 0.0f, 2.0f},
        {0.0f, 0.5f, 2.0f},
        {1.0f, 0.5f, 2.0f},

        {0.0f, 0.5f, 0.0f},
        {0.5f, 0.5f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.5f, 1.0f, 0.0f},

        {0.5f, 0.5f, 1.0f},
        {1.0f, 0.5f, 1.0f},
        {0.5f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},
    }), TestSuite::Compare::Container);
}

void RendererTest::glyphQuadsInvalidViewSizes() {
    CORRADE_SKIP_IF_NO_ASSERT();

    TestFont font;
    font.openFile({}, 5.0f);
    DummyGlyphCache cache{PixelFormat::R8Unorm, {20, 20}};
    cache.addFont(96, &font);
    Vector2 glyphPositions[4];
    Vector2 glyphPositionsInvalid[5];
    UnsignedInt glyphIds[4]{};
    UnsignedInt glyphIdsInvalid[3];
    Vector2 positions[16];
    Vector2 positionsInvalid[15];
    Vector3 textureCoordinates[16];
    Vector3 textureCoordinatesInvalid[17];

    Containers::String out;
    Error redirectError{&out};
    renderGlyphQuadsInto(font, 10.0f, cache, glyphPositions, glyphIdsInvalid, positions, textureCoordinates);
    renderGlyphQuadsInto(cache, 2.0f, glyphPositions, glyphIdsInvalid, positions, textureCoordinates);
    renderGlyphQuadsInto(font, 10.0f, cache, glyphPositionsInvalid, glyphIds, positions, textureCoordinates);
    renderGlyphQuadsInto(cache, 2.0f, glyphPositionsInvalid, glyphIds, positions, textureCoordinates);
    renderGlyphQuadsInto(font, 10.0f, cache, glyphPositions, glyphIds, positions, textureCoordinatesInvalid);
    renderGlyphQuadsInto(cache, 2.0f, glyphPositions, glyphIds, positions, textureCoordinatesInvalid);
    renderGlyphQuadsInto(font, 10.0f, cache, glyphPositions, glyphIds, positionsInvalid, textureCoordinates);
    renderGlyphQuadsInto(cache, 2.0f, glyphPositions, glyphIds, positionsInvalid, textureCoordinates);
    CORRADE_COMPARE_AS(out,
        "Text::renderGlyphQuadsInto(): expected fontGlyphIds and glyphPositions views to have the same size, got 3 and 4\n"
        "Text::renderGlyphQuadsInto(): expected glyphIds and glyphPositions views to have the same size, got 3 and 4\n"
        "Text::renderGlyphQuadsInto(): expected fontGlyphIds and glyphPositions views to have the same size, got 4 and 5\n"
        "Text::renderGlyphQuadsInto(): expected glyphIds and glyphPositions views to have the same size, got 4 and 5\n"
        "Text::renderGlyphQuadsInto(): expected vertexPositions and vertexTextureCoordinates views to have 16 elements, got 16 and 17\n"
        "Text::renderGlyphQuadsInto(): expected vertexPositions and vertexTextureCoordinates views to have 16 elements, got 16 and 17\n"
        "Text::renderGlyphQuadsInto(): expected vertexPositions and vertexTextureCoordinates views to have 16 elements, got 15 and 16\n"
        "Text::renderGlyphQuadsInto(): expected vertexPositions and vertexTextureCoordinates views to have 16 elements, got 15 and 16\n",
        TestSuite::Compare::String);
}

void RendererTest::glyphQuadsNoFontOpened() {
    CORRADE_SKIP_IF_NO_ASSERT();

    TestFont font;
    DummyGlyphCache cache{PixelFormat::R8Unorm, {20, 20}};

    Containers::String out;
    Error redirectError{&out};
    renderGlyphQuadsInto(font, 10.0f, cache, nullptr, nullptr, nullptr, Containers::StridedArrayView1D<Vector3>{});
    CORRADE_COMPARE(out, "Text::renderGlyphQuadsInto(): no font opened\n");
}

void RendererTest::glyphQuadsFontNotFoundInCache() {
    CORRADE_SKIP_IF_NO_ASSERT();

    TestFont font;
    font.openFile({}, 0.5f);
    DummyGlyphCache cache{PixelFormat::R8Unorm, {20, 20}};
    cache.addFont(56);
    cache.addFont(13);

    Containers::String out;
    Error redirectError{&out};
    renderGlyphQuadsInto(font, 10.0f, cache, nullptr, nullptr, nullptr, Containers::StridedArrayView1D<Vector3>{});
    CORRADE_COMPARE(out, "Text::renderGlyphQuadsInto(): font not found among 2 fonts in passed glyph cache\n");
}

void RendererTest::glyphQuads2D() {
    auto&& data = GlyphQuadsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Like lineGlyphPositions(), but with just a 2D glyph cache and using the
       three-component overload. */

    TestFont font;
    font.openFile({}, 2.5f);
    DummyGlyphCache cache = testGlyphCache(font);

    Vector2 glyphPositions[]{
        {100.0f, 200.0f},
        {103.0f, 202.0f},
        {107.0f, 196.0f}
    };
    UnsignedInt fontGlyphIds[]{
        3, 7, 9
    };
    UnsignedInt glyphIds[]{
        1, 3, 2
    };

    Vector2 positions[3*4];
    Vector2 textureCoordinates[3*4];
    Range2D rectangle = data.globalIds ?
        renderGlyphQuadsInto(cache, 1.25f/2.5f, glyphPositions, glyphIds, positions, textureCoordinates) :
        renderGlyphQuadsInto(font, 1.25f, cache, glyphPositions, fontGlyphIds, positions, textureCoordinates);
    CORRADE_COMPARE(rectangle, (Range2D{{102.5f, 198.5f}, {114.5f, 210.0f}}));

    CORRADE_COMPARE_AS(Containers::arrayView(positions), Containers::arrayView<Vector2>({
        {102.5f, 205.0f},
        {112.5f, 205.0f},
        {102.5f, 210.0f},
        {112.5f, 210.0f},

        {108.0f, 204.5f},
        {113.0f, 204.5f},
        {108.0f, 209.5f},
        {113.0f, 209.5f},

        {109.5f, 198.5f},
        {114.5f, 198.5f},
        {109.5f, 203.5f},
        {114.5f, 203.5f},
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(Containers::arrayView(textureCoordinates), Containers::arrayView<Vector2>({
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {0.0f, 0.5f},
        {1.0f, 0.5f},

        {0.0f, 0.5f},
        {0.5f, 0.5f},
        {0.0f, 1.0f},
        {0.5f, 1.0f},

        {0.5f, 0.5f},
        {1.0f, 0.5f},
        {0.5f, 1.0f},
        {1.0f, 1.0f},
    }), TestSuite::Compare::Container);
}

void RendererTest::glyphQuads2DArrayGlyphCache() {
    CORRADE_SKIP_IF_NO_ASSERT();

    TestFont font;
    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
    } cache{PixelFormat::R8Unorm, {20, 20, 2}};

    Containers::String out;
    Error redirectError{&out};
    renderGlyphQuadsInto(font, 10.0f, cache, nullptr, nullptr, nullptr, Containers::StridedArrayView1D<Vector2>{});
    CORRADE_COMPARE(out, "Text::renderGlyphQuadsInto(): can't use this overload with an array glyph cache\n");
}

void RendererTest::glyphQuadBounds() {
    /* Input like in glyphQuads(), verifying just the output rectangle */

    TestFont font;
    font.openFile({}, 2.5f);
    DummyGlyphCache cache = testGlyphCacheArray(font);

    Vector2 glyphPositions[]{
        {100.0f, 200.0f},
        {103.0f, 202.0f},
        {107.0f, 196.0f}
    };
    UnsignedInt glyphIds[]{
        /* Glyph 0 is the cache-global invalid glyph */
        1, 3, 2
    };

    /* The font is opened at 2.5, rendering at 1.25, so everything will be
       scaled by 0.5 */
    Range2D rectangle = Text::glyphQuadBounds(cache, 1.25f/2.5f, glyphPositions, glyphIds);
    CORRADE_COMPARE(rectangle, (Range2D{{102.5f, 198.5f}, {114.5f, 210.0f}}));
}

void RendererTest::glyphQuadBoundsInvalidViewSizes() {
    CORRADE_SKIP_IF_NO_ASSERT();

    TestFont font;
    font.openFile({}, 5.0f);
    DummyGlyphCache cache{PixelFormat::R8Unorm, {20, 20}};
    cache.addFont(96, &font);
    Vector2 glyphPositions[4];
    Vector2 glyphPositionsInvalid[5];
    UnsignedInt glyphIds[4]{};
    UnsignedInt glyphIdsInvalid[3];

    Containers::String out;
    Error redirectError{&out};
    Text::glyphQuadBounds(cache, 2.0f, glyphPositions, glyphIdsInvalid);
    Text::glyphQuadBounds(cache, 2.0f, glyphPositionsInvalid, glyphIds);
    CORRADE_COMPARE_AS(out,
        "Text::glyphQuadBounds(): expected glyphIds and glyphPositions views to have the same size, got 3 and 4\n"
        "Text::glyphQuadBounds(): expected glyphIds and glyphPositions views to have the same size, got 4 and 5\n",
        TestSuite::Compare::String);
}

void RendererTest::alignLine() {
    auto&& data = AlignLineData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Range2D rectangle{{10.0f, 200.0f}, {13.5f, -960.0f}};

    /* The positions aren't taken into account, so they can be arbitrary */
    Vector2 positions[]{
        {100.0f, 200.0f},
        {300.0f, -60.0f},
        {-10.0f, 100.0f},
    };
    Range2D alignedRectangle = alignRenderedLine(rectangle, LayoutDirection::HorizontalTopToBottom, data.alignment, positions);
    CORRADE_COMPARE(alignedRectangle, rectangle.translated({data.offset, 0.0f}));
    CORRADE_COMPARE_AS(Containers::arrayView(positions), Containers::arrayView<Vector2>({
        {100.0f + data.offset, 200.0f},
        {300.0f + data.offset, -60.0f},
        {-10.0f + data.offset, 100.0f}
    }), TestSuite::Compare::Container);
}

void RendererTest::alignLineInvalidDirection() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    alignRenderedLine({}, LayoutDirection::VerticalRightToLeft, Alignment::LineLeft, nullptr);
    alignRenderedLine({}, LayoutDirection::HorizontalTopToBottom, Alignment::BottomEnd, nullptr);
    CORRADE_COMPARE_AS(out,
        "Text::alignRenderedLine(): only Text::LayoutDirection::HorizontalTopToBottom is supported right now, got Text::LayoutDirection::VerticalRightToLeft\n"
        "Text::alignRenderedLine(): Text::Alignment::BottomEnd has to be resolved to *Left / *Right before being passed to this function\n",
        TestSuite::Compare::String);
}

void RendererTest::alignBlock() {
    auto&& data = AlignBlockData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Range2D rectangle{{100.0f, 9.5f}, {-70.0f, 19.5f}};

    /* The positions aren't taken into account, so they can be arbitrary */
    Vector2 positions[]{
        {100.0f, 200.0f},
        {-10.0f, 100.0f},
        {300.0f, -60.0f},
    };
    Range2D alignedRectangle = alignRenderedBlock(rectangle, LayoutDirection::HorizontalTopToBottom, data.alignment, positions);
    CORRADE_COMPARE(alignedRectangle, rectangle.translated({0.0f, data.offset}));
    CORRADE_COMPARE_AS(Containers::arrayView(positions), Containers::arrayView<Vector2>({
        {100.0f, 200.0f + data.offset},
        {-10.0f, 100.0f + data.offset},
        {300.0f, -60.0f + data.offset},
    }), TestSuite::Compare::Container);
}

void RendererTest::alignBlockInvalidDirection() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    alignRenderedBlock({}, LayoutDirection::VerticalRightToLeft, Alignment::LineLeft, nullptr);
    alignRenderedBlock({}, LayoutDirection::HorizontalTopToBottom, Alignment::LineBeginGlyphBounds, nullptr);
    CORRADE_COMPARE_AS(out,
        "Text::alignRenderedBlock(): only Text::LayoutDirection::HorizontalTopToBottom is supported right now, got Text::LayoutDirection::VerticalRightToLeft\n"
        "Text::alignRenderedBlock(): Text::Alignment::LineBeginGlyphBounds has to be resolved to *Left / *Right before being passed to this function\n",
        TestSuite::Compare::String);
}

template<class T> void RendererTest::glyphQuadIndices() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* 2---3 2 3---5
       |   | |\ \  |
       |   | | \ \ |
       |   | |  \ \|
       0---1 0---1 4 */
    T indices[3*6];
    renderGlyphQuadIndicesInto(60, indices);
    CORRADE_COMPARE_AS(Containers::arrayView(indices), Containers::arrayView<T>({
        240, 241, 242, 242, 241, 243,
        244, 245, 246, 246, 245, 247,
        248, 249, 250, 250, 249, 251
    }), TestSuite::Compare::Container);
}

void RendererTest::glyphQuadIndicesTypeTooSmall() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* This should be fine */
    UnsignedByte indices8[18];
    UnsignedShort indices16[18];
    UnsignedInt indices32[18];
    renderGlyphQuadIndicesInto(256/4 - 3, indices8);
    renderGlyphQuadIndicesInto(65536/4 - 3, indices16);
    renderGlyphQuadIndicesInto(4294967296u/4 - 3, indices32);
    CORRADE_COMPARE(indices8[17], 255);
    CORRADE_COMPARE(indices16[17], 65535);
    CORRADE_COMPARE(indices32[17], 4294967295);

    /* Empty view also */
    renderGlyphQuadIndicesInto(256/4, Containers::ArrayView<UnsignedByte>{});
    renderGlyphQuadIndicesInto(65536/4, Containers::ArrayView<UnsignedShort>{});
    renderGlyphQuadIndicesInto(4294967296u/4, Containers::ArrayView<UnsignedInt>{});

    Containers::String out;
    Error redirectError{&out};
    renderGlyphQuadIndicesInto(256/4 - 3 + 1, indices8);
    renderGlyphQuadIndicesInto(65536/4 - 3 + 1, indices16);
    renderGlyphQuadIndicesInto(4294967296u/4 - 3 + 1, indices32);
    /* Should assert even if there's actually no indices to write */
    renderGlyphQuadIndicesInto(256/4 + 1, Containers::ArrayView<UnsignedByte>{});
    renderGlyphQuadIndicesInto(65536/4 + 1, Containers::ArrayView<UnsignedShort>{});
    renderGlyphQuadIndicesInto(4294967296u/4 + 1, Containers::ArrayView<UnsignedInt>{});
    CORRADE_COMPARE(out,
        "Text::renderGlyphQuadIndicesInto(): max index value of 259 cannot fit into a 8-bit type\n"
        "Text::renderGlyphQuadIndicesInto(): max index value of 65539 cannot fit into a 16-bit type\n"
        "Text::renderGlyphQuadIndicesInto(): max index value of 4294967299 cannot fit into a 32-bit type\n"
        "Text::renderGlyphQuadIndicesInto(): max index value of 259 cannot fit into a 8-bit type\n"
        "Text::renderGlyphQuadIndicesInto(): max index value of 65539 cannot fit into a 16-bit type\n"
        "Text::renderGlyphQuadIndicesInto(): max index value of 4294967299 cannot fit into a 32-bit type\n");
}

void RendererTest::glyphRangeForBytes() {
    auto&& data = GlyphRangeForBytesData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Offset from the start, some characters decomposed/reordered, some
       multi-byte, and then also multi-byte to decomposed */
    UnsignedInt clusterData[]{
        3,  /* 0 9 */
        4,  /* 1 8 */
        5,  /* 2 7 */
        5,  /* 3 6 */
        5,  /* 4 5 */
        6,  /* 5 4 */
        6,  /* 6 3 */
        9,  /* 7 2 */
        12, /* 8 1 */
        13  /* 9 0 */
    };
    Containers::StridedArrayView1D<const UnsignedInt> clusters = clusterData;
    if(!data.ascending) clusters = clusters.flipped<0>();

    /* With empty clusters it means there are no glyphs, so returning 0 means
       both before and after the glyph run */
    CORRADE_COMPARE(data.function(nullptr, 0, 3), Containers::pair(0u, 0u));
    CORRADE_COMPARE(data.function(nullptr, 10, 13), Containers::pair(0u, 0u));

    /* Bytes before everything return 0, same for an empty range at the
       start; if the other direction then it returns the size */
    for(Containers::Pair<UnsignedInt, UnsignedInt> i: {
        Containers::pair(1u, 1u),
        Containers::pair(2u, 3u),
        Containers::pair(3u, 3u)
    }) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(data.function(clusters, i.first(), i.second()),
            data.ascending ? Containers::pair(0u, 0u) :
                             Containers::pair(10u, 10u));
    }

    /* Bytes after everything return the size (or 0 if reverse direction). Size
       of the last cluster in bytes is unknown so there's no empty range at the
       end */
    for(Containers::Pair<UnsignedInt, UnsignedInt> i: {
        Containers::pair(14u, 14u),
        Containers::pair(14u, 16u)
    }) {
        CORRADE_COMPARE(data.function(clusters, i.first(), i.second()),
            data.ascending ? Containers::pair(10u, 10u) :
                             Containers::pair(0u, 0u));
    }

    /* Empty ranges inside, i.e. for a cursor. In reverse direction it means
       the cursor is from *the other side* of the same glyph, so +1. In other
       words, if you do backspace (which always goes backwards in the byte
       stream, but to the left for LTR text and to the right for RTL text), it
       deletes the same glyph regardless of direction */
    CORRADE_COMPARE(data.function(clusters, 4, 4),
        data.ascending ? Containers::pair(1u, 1u) :
                         Containers::pair(9u, 9u));
    /* This one maps from one byte to multiple glyphs */
    CORRADE_COMPARE(data.function(clusters, 5, 5),
        data.ascending ? Containers::pair(2u, 2u) :
                         Containers::pair(8u, 8u));
    /* This one maps from multiple bytes to a single glyph, should return the
       same for any byte inside that sequence */
    for(UnsignedInt i: {9, 10, 11}) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(data.function(clusters, i, i),
            data.ascending ? Containers::pair(7u, 7u) :
                             Containers::pair(3u, 3u));
    }
    /* This one maps from multiple bytes to multiple glyphs, again should
       return the same for any byte inside that sequence */
    for(UnsignedInt i: {6, 7, 8}) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(data.function(clusters, i, i),
            data.ascending ? Containers::pair(5u, 5u) :
                             Containers::pair(5u, 5u));
    }

    /* Single byte mapped to a single glyph, i.e. an Insert mode or a
       selection. Again, in reverse direction it should cover the same glyph,
       just from the other side. */
    CORRADE_COMPARE(data.function(clusters, 3, 4),
        data.ascending ? Containers::pair(0u, 1u) :
                         Containers::pair(9u, 10u));
    CORRADE_COMPARE(data.function(clusters, 4, 5),
        data.ascending ? Containers::pair(1u, 2u) :
                         Containers::pair(8u, 9u));
    CORRADE_COMPARE(data.function(clusters, 12, 13),
        data.ascending ? Containers::pair(8u, 9u) :
                         Containers::pair(1u, 2u));

    /* Multiple bytes mapped to a single glyph, as well as any subranges of
       those */
    for(Containers::Pair<UnsignedInt, UnsignedInt> i: {
        Containers::pair(9u, 10u),
        Containers::pair(9u, 11u),
        Containers::pair(9u, 12u),
        Containers::pair(10u, 11u),
        Containers::pair(10u, 12u),
        Containers::pair(11u, 12u)
    }) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(data.function(clusters, i.first(), i.second()),
            data.ascending ? Containers::pair(7u, 8u) :
                             Containers::pair(2u, 3u));
    }

    /* Single byte mapped to multiple glyphs */
    CORRADE_COMPARE(data.function(clusters, 5, 6),
        data.ascending ? Containers::pair(2u, 5u) :
                         Containers::pair(5u, 8u));

    /* Multiple bytes mapped to multiple glyphs, as well as any subranges of
       those */
    for(Containers::Pair<UnsignedInt, UnsignedInt> i: {
        Containers::pair(6u, 7u),
        Containers::pair(6u, 8u),
        Containers::pair(6u, 9u),
        Containers::pair(7u, 8u),
        Containers::pair(7u, 9u),
        Containers::pair(8u, 9u)
    }) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(data.function(clusters, i.first(), i.second()),
            data.ascending ? Containers::pair(5u, 7u) :
                             Containers::pair(3u, 5u));
    }

    /* Larger ranges */
    CORRADE_COMPARE(data.function(clusters, 4, 9),
        data.ascending ? Containers::pair(1u, 7u) :
                         Containers::pair(3u, 9u));
    CORRADE_COMPARE(data.function(clusters, 5, 12),
        data.ascending ? Containers::pair(2u, 8u) :
                         Containers::pair(2u, 8u));
    CORRADE_COMPARE(data.function(clusters, 3, 14),
        data.ascending ? Containers::pair(0u, 10u) :
                         Containers::pair(0u, 10u));
    CORRADE_COMPARE(data.function(clusters, 0, 20),
        data.ascending ? Containers::pair(0u, 10u) :
                         Containers::pair(0u, 10u));

    /* Subsets of multi-byte ranges plus bytes after */
    for(Containers::Pair<UnsignedInt, UnsignedInt> i: {
        Containers::pair(7u, 12u),
        Containers::pair(8u, 12u)
    }) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(data.function(clusters, i.first(), i.second()),
            data.ascending ? Containers::pair(5u, 8u) :
                             Containers::pair(2u, 5u));
    }

    /* Subsets of multi-byte ranges plus bytes before */
    for(Containers::Pair<UnsignedInt, UnsignedInt> i: {
        Containers::pair(4u, 7u),
        Containers::pair(4u, 8u)
    }) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(data.function(clusters, i.first(), i.second()),
            data.ascending ? Containers::pair(1u, 7u) :
                             Containers::pair(3u, 9u));
    }
}

void RendererTest::debugFlagCore() {
    Containers::String out;
    Debug{&out} << RendererCoreFlag::GlyphClusters << RendererCoreFlag(0xca);
    CORRADE_COMPARE(out, "Text::RendererCoreFlag::GlyphClusters Text::RendererCoreFlag(0xca)\n");
}

void RendererTest::debugFlagsCore() {
    Containers::String out;
    Debug{&out} << (RendererCoreFlag::GlyphClusters|RendererCoreFlag(0xf0)) << RendererCoreFlags{};
    CORRADE_COMPARE(out, "Text::RendererCoreFlag::GlyphClusters|Text::RendererCoreFlag(0xf0) Text::RendererCoreFlags{}\n");
}

void RendererTest::constructCore() {
    auto&& data = ConstructCoreData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
    } glyphCache{PixelFormat::R8Unorm, {16, 16, 2}};

    RendererCore renderer{glyphCache, data.flags};
    CORRADE_COMPARE(&renderer.glyphCache(), &glyphCache);
    CORRADE_COMPARE(renderer.flags(), data.flags);
    CORRADE_COMPARE(renderer.glyphCount(), 0);
    CORRADE_COMPARE(renderer.glyphCapacity(), 0);
    CORRADE_COMPARE(renderer.runCount(), 0);
    CORRADE_COMPARE(renderer.runCapacity(), 0);
    CORRADE_VERIFY(!renderer.isRendering());
    CORRADE_COMPARE(renderer.renderingGlyphCount(), 0);
    CORRADE_COMPARE(renderer.renderingRunCount(), 0);
    CORRADE_COMPARE(renderer.cursor(), Vector2{});
    CORRADE_COMPARE(renderer.alignment(), Alignment::MiddleCenter);
    CORRADE_COMPARE(renderer.layoutDirection(), LayoutDirection::HorizontalTopToBottom);
    CORRADE_COMPARE(renderer.glyphPositions().size(), 0);
    CORRADE_COMPARE(renderer.glyphIds().size(), 0);
    if(data.flags >= RendererCoreFlag::GlyphClusters)
        CORRADE_COMPARE(renderer.glyphClusters().size(), 0);
}

void RendererTest::constructCoreAllocator() {
    auto&& data = ConstructCoreAllocatorData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
    } glyphCache{PixelFormat::R8Unorm, {16, 16, 2}};

    int called = 0;
    RendererCore renderer{glyphCache, data.glyphAllocator, &called, data.runAllocator, &called, data.flags};

    CORRADE_COMPARE(&renderer.glyphCache(), &glyphCache);
    CORRADE_COMPARE(renderer.flags(), data.flags);
    CORRADE_COMPARE(renderer.glyphCount(), 0);
    CORRADE_COMPARE(renderer.glyphCapacity(), 0);
    CORRADE_COMPARE(renderer.runCount(), 0);
    CORRADE_COMPARE(renderer.runCapacity(), 0);
    CORRADE_VERIFY(!renderer.isRendering());
    CORRADE_COMPARE(renderer.renderingGlyphCount(), 0);
    CORRADE_COMPARE(renderer.renderingRunCount(), 0);
    CORRADE_COMPARE(renderer.cursor(), Vector2{});
    CORRADE_COMPARE(renderer.alignment(), Alignment::MiddleCenter);
    CORRADE_COMPARE(renderer.lineAdvance(), 0.0f);
    CORRADE_COMPARE(renderer.layoutDirection(), LayoutDirection::HorizontalTopToBottom);
    CORRADE_COMPARE(renderer.glyphPositions().size(), 0);
    CORRADE_COMPARE(renderer.glyphIds().size(), 0);
    if(data.flags >= RendererCoreFlag::GlyphClusters)
        CORRADE_COMPARE(renderer.glyphClusters().size(), 0);

    /* The allocators should not be called by default */
    CORRADE_COMPARE(called, 0);
}

void RendererTest::constructCoreNoCreate() {
    RendererCore renderer{NoCreate};

    /* Shouldn't crash */
    CORRADE_VERIFY(true);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoCreateT, RendererCore>::value);
}

void RendererTest::constructCopyCore() {
    CORRADE_VERIFY(!std::is_copy_constructible<RendererCore>{});
    CORRADE_VERIFY(!std::is_copy_assignable<RendererCore>{});
}

void RendererTest::constructMoveCore() {
    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
    } glyphCache{PixelFormat::R8Unorm, {16, 16, 2}},
        anotherGlyphCache{PixelFormat::RGBA8Unorm, {4, 4}};

    RendererCore a{glyphCache, RendererCoreFlag::GlyphClusters};

    RendererCore b = Utility::move(a);
    CORRADE_COMPARE(&b.glyphCache(), &glyphCache);
    CORRADE_COMPARE(b.flags(), RendererCoreFlag::GlyphClusters);

    RendererCore c{anotherGlyphCache};
    c = Utility::move(b);
    CORRADE_COMPARE(&c.glyphCache(), &glyphCache);
    CORRADE_COMPARE(c.flags(), RendererCoreFlag::GlyphClusters);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<RendererCore>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<RendererCore>::value);
}

void RendererTest::propertiesCore() {
    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
    } glyphCache{PixelFormat::R8Unorm, {16, 16}};

    RendererCore renderer{glyphCache};
    CORRADE_VERIFY(!renderer.isRendering());
    CORRADE_COMPARE(renderer.cursor(), Vector2{});
    CORRADE_COMPARE(renderer.alignment(), Alignment::MiddleCenter);
    CORRADE_COMPARE(renderer.lineAdvance(), 0.0f);
    CORRADE_COMPARE(renderer.layoutDirection(), LayoutDirection::HorizontalTopToBottom);

    renderer.setCursor({15.7f, -2.3f});
    CORRADE_COMPARE(renderer.cursor(), (Vector2{15.7f, -2.3f}));

    renderer.setAlignment(Alignment::BottomLeftGlyphBounds);
    CORRADE_COMPARE(renderer.alignment(), Alignment::BottomLeftGlyphBounds);

    renderer.setLineAdvance(3.0f);
    CORRADE_COMPARE(renderer.lineAdvance(), 3.0f);

    /* Layout direction has just one allowed value right now */
    /** @todo update once it's not just one anymore */
    renderer.setLayoutDirection(LayoutDirection::HorizontalTopToBottom);
    CORRADE_COMPARE(renderer.layoutDirection(), LayoutDirection::HorizontalTopToBottom);
}

void RendererTest::propertiesCoreInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
    } glyphCache{PixelFormat::R8Unorm, {16, 16}};

    RendererCore renderer{glyphCache};

    Containers::String out;
    Error redirectError{&out};
    renderer.setLayoutDirection(LayoutDirection::VerticalLeftToRight);
    renderer.glyphClusters();
    CORRADE_COMPARE(out,
        "Text::RendererCore::setLayoutDirection(): only Text::LayoutDirection::HorizontalTopToBottom is supported right now, got Text::LayoutDirection::VerticalLeftToRight\n"
        "Text::RendererCore::glyphClusters(): glyph clusters not enabled\n");
}

void RendererTest::propertiesCoreRenderingInProgress() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
    } glyphCache{PixelFormat::R8Unorm, {16, 16}};

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;
    glyphCache.addFont(0, &font);

    struct: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(Containers::StringView, UnsignedInt, UnsignedInt, Containers::ArrayView<const FeatureRange>) override {
            return 0;
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&) const override {}
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}
    } shaper{font};

    RendererCore renderer{glyphCache};

    /* It should be marked as in progress even if there aren't any glyphs, to
       enforce correct usage in all cases */
    renderer.add(shaper, 1.0f, "hello");
    CORRADE_COMPARE(renderer.glyphCount(), 0);
    CORRADE_COMPARE(renderer.runCount(), 0);
    CORRADE_VERIFY(renderer.isRendering());
    CORRADE_COMPARE(renderer.renderingGlyphCount(), 0);
    CORRADE_COMPARE(renderer.renderingRunCount(), 0);

    /* It should blow up even if the properties are set to exactly the same as
       before */
    Containers::String out;
    Error redirectError{&out};
    renderer.setCursor({});
    renderer.setAlignment(Alignment::MiddleCenter);
    renderer.setLineAdvance({});
    renderer.setLayoutDirection(LayoutDirection::HorizontalTopToBottom);
    CORRADE_COMPARE_AS(out,
        "Text::RendererCore::setCursor(): rendering in progress\n"
        "Text::RendererCore::setAlignment(): rendering in progress\n"
        "Text::RendererCore::setLineAdvance(): rendering in progress\n"
        "Text::RendererCore::setLayoutDirection(): rendering in progress\n",
        TestSuite::Compare::String);
}

void RendererTest::glyphsForRuns() {
    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
    } glyphCache{PixelFormat::R8Unorm, {16, 16}, {}};

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        Properties doOpenFile(Containers::StringView, Float) override {
            _opened = true;
            return {1.0f, 1.0f, -1.0f, 1.0f, 0};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        private:
            bool _opened = false;
    } font;
    font.openFile("", 1.0f);
    glyphCache.addFont(1, &font);

    struct: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(Containers::StringView text, UnsignedInt, UnsignedInt, Containers::ArrayView<const FeatureRange>) override {
            return text.size();
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>& ids) const override {
            for(UnsignedInt i = 0; i != ids.size(); ++i)
                ids[i] = 0;
        }
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&) const override {}
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}
    } shaper{font};

    RendererCore renderer{glyphCache};
    CORRADE_COMPARE(renderer.glyphCount(), 0);
    CORRADE_COMPARE(renderer.runCount(), 0);
    CORRADE_COMPARE(renderer.renderingGlyphCount(), 0);
    CORRADE_COMPARE(renderer.renderingRunCount(), 0);

    /* With no runs this is the only value it accepts */
    CORRADE_COMPARE(renderer.glyphsForRuns({}), Range1Dui{});

    /* A single finished run */
    CORRADE_COMPARE(renderer.render(shaper, 1.0f, "abcd").second(), (Range1Dui{0, 1}));
    CORRADE_COMPARE(renderer.glyphCount(), 4);
    CORRADE_COMPARE(renderer.runCount(), 1);
    CORRADE_COMPARE(renderer.renderingGlyphCount(), 4);
    CORRADE_COMPARE(renderer.renderingRunCount(), 1);
    CORRADE_COMPARE(renderer.glyphsForRuns({0, 1}), (Range1Dui{0, 4}));

    /* Should work for unfinished runs as well, and across them */
    renderer
        .add(shaper, 1.0f, "ef")
        .add(shaper, 1.0f, "ghi");
    CORRADE_COMPARE(renderer.glyphCount(), 4);
    CORRADE_COMPARE(renderer.runCount(), 1);
    CORRADE_COMPARE(renderer.renderingGlyphCount(), 9);
    CORRADE_COMPARE(renderer.renderingRunCount(), 3);
    CORRADE_COMPARE(renderer.glyphsForRuns({0, 3}), (Range1Dui{0, 9}));
    CORRADE_COMPARE(renderer.glyphsForRuns({1, 2}), (Range1Dui{4, 6}));
    CORRADE_COMPARE(renderer.glyphsForRuns({1, 3}), (Range1Dui{4, 9}));
    CORRADE_COMPARE(renderer.glyphsForRuns({2, 3}), (Range1Dui{6, 9}));

    /* Zero-size, at both begin and end, and end < begin should also work */
    CORRADE_COMPARE(renderer.glyphsForRuns({2, 2}), (Range1Dui{6, 6}));
    CORRADE_COMPARE(renderer.glyphsForRuns({0, 0}), (Range1Dui{0, 0}));
    CORRADE_COMPARE(renderer.glyphsForRuns({3, 3}), (Range1Dui{9, 9}));
    CORRADE_COMPARE(renderer.glyphsForRuns({3, 1}), (Range1Dui{9, 4}));
    CORRADE_COMPARE(renderer.glyphsForRuns({2, 0}), (Range1Dui{6, 0}));
}

void RendererTest::glyphsForRunsInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
    } glyphCache{PixelFormat::R8Unorm, {16, 16}, {}};

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        Properties doOpenFile(Containers::StringView, Float) override {
            _opened = true;
            return {1.0f, 1.0f, -1.0f, 1.0f, 0};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        private:
            bool _opened = false;
    } font;
    font.openFile("", 1.0f);
    glyphCache.addFont(1, &font);

    struct: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(Containers::StringView text, UnsignedInt, UnsignedInt, Containers::ArrayView<const FeatureRange>) override {
            return text.size();
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>& ids) const override {
            for(UnsignedInt i = 0; i != ids.size(); ++i)
                ids[i] = 0;
        }
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&) const override {}
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}
    } shaper{font};

    /* Have some runs finished and some still rendering */
    RendererCore renderer{glyphCache};
    renderer.render(shaper, 1.0f, "ab");
    renderer
        .add(shaper, 1.0f, "cde")
        .add(shaper, 1.0f, "fg");
    CORRADE_COMPARE(renderer.glyphCount(), 2);
    CORRADE_COMPARE(renderer.runCount(), 1);
    CORRADE_COMPARE(renderer.renderingGlyphCount(), 7);
    CORRADE_COMPARE(renderer.renderingRunCount(), 3);

    /* This is still fine */
    renderer.glyphsForRuns({3, 3});

    Containers::String out;
    Error redirectError{&out};
    renderer.glyphsForRuns({3, 4});
    renderer.glyphsForRuns({4, 3});
    CORRADE_COMPARE_AS(out,
        "Text::RendererCore::glyphsForRuns(): runs {3, 4} out of range for 3 runs\n"
        "Text::RendererCore::glyphsForRuns(): runs {4, 3} out of range for 3 runs\n",
        TestSuite::Compare::String);
}

void RendererTest::allocateCore() {
    auto&& data = AllocateData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
    /* Set padding to zero for easier dummy glyph addition below */
    } glyphCache{PixelFormat::R8Unorm, {16, 16}, {}};

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        Properties doOpenFile(Containers::StringView, Float size) override {
            _opened = true;
            /* The size is used to scale advances, ascent & descent is used to
               align the block. Line height is used for multi-line text which
               we don't test here, glyph count is overriden in addFont()
               below. */
            return {size, 2.5f, -1.0f, 10000.0f, 0};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        private:
            bool _opened = false;
    } font;
    font.openFile("", 1.0f);
    UnsignedInt fontId = glyphCache.addFont(23*2, &font);
    /* Add just the first few glyphs, in shuffled order to not have their IDs
       match the clusters */
    glyphCache.addGlyph(fontId, 4, {}, {}); /* 1 */
    glyphCache.addGlyph(fontId, 0, {}, {}); /* 2 */
    glyphCache.addGlyph(fontId, 2, {}, {}); /* 3 */

    struct: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(Containers::StringView text, UnsignedInt, UnsignedInt, Containers::ArrayView<const FeatureRange>) override {
            return text.size();
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>& ids) const override {
            for(UnsignedInt i = 0; i != ids.size(); ++i)
                ids[i] = i*2;
        }
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>& offsets, const Containers::StridedArrayView1D<Vector2>& advances) const override {
            for(UnsignedInt i = 0; i != offsets.size(); ++i) {
                advances[i] = {1.5f, 0.0f};
                offsets[i] = {0.0f, i % 2 ? 0.0f : 0.5f};
            }
        }
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>& clusters) const override {
            for(UnsignedInt i = 0; i != clusters.size(); ++i)
                clusters[i] = 10 + i;
        }
    } shaper{font};

    RendererCore renderer{glyphCache, data.flagsCore};
    CORRADE_COMPARE(renderer.glyphCount(), 0);
    CORRADE_COMPARE(renderer.glyphCapacity(), 0);
    CORRADE_COMPARE(renderer.runCount(), 0);
    CORRADE_COMPARE(renderer.runCapacity(), 0);
    CORRADE_COMPARE(renderer.renderingGlyphCount(), 0);
    CORRADE_COMPARE(renderer.renderingRunCount(), 0);
    CORRADE_COMPARE(renderer.glyphPositions().size(), 0);
    CORRADE_COMPARE(renderer.glyphPositions().data(), nullptr);
    CORRADE_COMPARE(renderer.glyphIds().size(), 0);
    CORRADE_COMPARE(renderer.glyphIds().data(), nullptr);
    if(data.flagsCore >= RendererCoreFlag::GlyphClusters) {
        CORRADE_COMPARE(renderer.glyphClusters().size(), 0);
        CORRADE_COMPARE(renderer.glyphClusters().data(), nullptr);
    }

    /* Reserving with 0 should be a no-op */
    renderer.reserve(0, 0);
    CORRADE_COMPARE(renderer.glyphCount(), 0);
    CORRADE_COMPARE(renderer.glyphCapacity(), 0);
    CORRADE_COMPARE(renderer.runCount(), 0);
    CORRADE_COMPARE(renderer.runCapacity(), 0);
    CORRADE_COMPARE(renderer.renderingGlyphCount(), 0);
    CORRADE_COMPARE(renderer.renderingRunCount(), 0);
    CORRADE_COMPARE(renderer.glyphPositions().size(), 0);
    CORRADE_COMPARE(renderer.glyphPositions().data(), nullptr);
    CORRADE_COMPARE(renderer.glyphIds().size(), 0);
    CORRADE_COMPARE(renderer.glyphIds().data(), nullptr);
    if(data.flagsCore >= RendererCoreFlag::GlyphClusters) {
        CORRADE_COMPARE(renderer.glyphClusters().size(), 0);
        CORRADE_COMPARE(renderer.glyphClusters().data(), nullptr);
    }

    /* The views should be non-null now even if no glyphs are rendered */
    renderer.reserve(data.reserveGlyphs, data.reserveRuns);
    CORRADE_COMPARE(renderer.glyphCount(), 0);
    CORRADE_COMPARE(renderer.glyphCapacity(), data.reserveGlyphs);
    CORRADE_COMPARE(renderer.runCount(), 0);
    CORRADE_COMPARE(renderer.runCapacity(), data.reserveRuns);
    CORRADE_COMPARE(renderer.renderingGlyphCount(), 0);
    CORRADE_COMPARE(renderer.renderingRunCount(), 0);
    CORRADE_COMPARE(renderer.glyphPositions().size(), 0);
    CORRADE_VERIFY(renderer.glyphPositions().data());
    CORRADE_COMPARE(renderer.glyphIds().size(), 0);
    CORRADE_VERIFY(renderer.glyphIds().data());
    if(data.flagsCore >= RendererCoreFlag::GlyphClusters) {
        CORRADE_COMPARE(renderer.glyphClusters().size(), 0);
        CORRADE_VERIFY(renderer.glyphClusters().data());
    }

    /* Rendering shouldn't reallocate anything */
    if(data.render) {
        renderer.add(shaper, 1.0f, "abc");
        CORRADE_COMPARE(renderer.glyphCapacity(), data.reserveGlyphs);
        CORRADE_COMPARE(renderer.runCapacity(), data.reserveRuns);
        if(data.renderAddOnly) {
            CORRADE_COMPARE(renderer.glyphCount(), 0);
            CORRADE_COMPARE(renderer.runCount(), 0);
            CORRADE_VERIFY(renderer.isRendering());
            CORRADE_COMPARE(renderer.glyphPositions().size(), 0);
            CORRADE_COMPARE(renderer.glyphIds().size(), 0);
            if(data.flagsCore >= RendererCoreFlag::GlyphClusters)
                CORRADE_COMPARE(renderer.glyphClusters().size(), 0);
            CORRADE_COMPARE(renderer.runScales().size(), 0);
            CORRADE_COMPARE(renderer.runEnds().size(), 0);
        } else {
            renderer.render();
            CORRADE_COMPARE(renderer.glyphCount(), 3);
            CORRADE_COMPARE(renderer.runCount(), 1);
            CORRADE_VERIFY(!renderer.isRendering());
            /* 3 letters, which is 4.5 units with advance being 1.5, so
               starting at -2.25 when centered, vertical center is at 0.25. */
            CORRADE_COMPARE_AS(renderer.glyphPositions(), Containers::arrayView<Vector2>({
                {-2.25f, -0.25f},
                {-0.75f, -0.75f},
                { 0.75f, -0.25f}
            }), TestSuite::Compare::Container);
            CORRADE_COMPARE_AS(renderer.glyphIds(), Containers::arrayView<UnsignedInt>({
                2, 3, 1 /* font glyphs 0, 2, 4 */
            }), TestSuite::Compare::Container);
            if(data.flagsCore >= RendererCoreFlag::GlyphClusters)
                CORRADE_COMPARE_AS(renderer.glyphClusters(), Containers::arrayView<UnsignedInt>({
                    10, 11, 12
                }), TestSuite::Compare::Container);
            CORRADE_COMPARE_AS(renderer.runScales(), Containers::arrayView({
                1.0f
            }), TestSuite::Compare::Container);
            CORRADE_COMPARE_AS(renderer.runEnds(), Containers::arrayView({
                3u
            }), TestSuite::Compare::Container);
        }
        CORRADE_COMPARE(renderer.renderingGlyphCount(), 3);
        CORRADE_COMPARE(renderer.renderingRunCount(), 1);
    }

    /* Reserving / rendering again should copy the existing data if not
       reserved enough */
    const void* currentPositions = renderer.glyphPositions().data();
    const void* currentIds = renderer.glyphIds().data();
    const void* currentClusters = data.flagsCore >= RendererCoreFlag::GlyphClusters ? renderer.glyphClusters().data() : nullptr;
    const void* currentRunScales = renderer.runScales().data();
    const void* currentRunEnds = renderer.runEnds().data();
    /* Reserving while a render is in progress shouldn't reset any internal
       state */
    if(data.secondReserveGlyphs || data.secondReserveRuns) {
        renderer.reserve(data.secondReserveGlyphs, data.secondReserveRuns);
        CORRADE_COMPARE(renderer.glyphCapacity(), data.expectedGlyphCapacity);
        CORRADE_COMPARE(renderer.runCapacity(), data.expectedRunCapacity);
        CORRADE_COMPARE(renderer.isRendering(), data.renderAddOnly);
    }
    if(data.render) {
        /* Make two more runs */
        renderer
            .add(shaper, 4.0f/3.0f, "defghijk")
            .render(shaper, 4.0f/3.0f, "lmnopqrstuvwxyz");
        CORRADE_COMPARE(renderer.glyphCount(), 26);
        CORRADE_VERIFY(!renderer.isRendering());
        CORRADE_COMPARE(renderer.renderingGlyphCount(), 26);
        CORRADE_COMPARE(renderer.renderingRunCount(), 3);
    } else {
        CORRADE_COMPARE(renderer.renderingGlyphCount(), 0);
        CORRADE_COMPARE(renderer.renderingRunCount(), 0);
    }
    CORRADE_COMPARE(renderer.glyphCapacity(), 26);
    CORRADE_COMPARE(renderer.runCapacity(), 3);

    /* If it shouldn't reallocate, the views should stay the same */
    if(data.expectNoGlyphReallocation) {
        CORRADE_COMPARE(renderer.glyphPositions().data(), currentPositions);
        CORRADE_COMPARE(renderer.glyphIds().data(), currentIds);
        if(data.flagsCore >= RendererCoreFlag::GlyphClusters)
            CORRADE_COMPARE(renderer.glyphClusters().data(), currentClusters);
    }
    if(data.expectNoRunReallocation) {
        CORRADE_COMPARE(renderer.runScales().data(), currentRunScales);
        CORRADE_COMPARE(renderer.runEnds().data(), currentRunEnds);
    }

    /* Verify that both the original data and (prefix of) the new is there. If
       only reserving, we have no way to know. */
    if(data.render) {
        /* If the first part wasn't finalized, it's 26 letters in total, which
           is 50.5 units with advance being 1.5 for the first 3 and 2.0 for the
           rest, so starting at -25.25 when centered, vertical center is
           -0.16667. */
        if(data.renderAddOnly)
            CORRADE_COMPARE_AS(renderer.glyphPositions().prefix(5), Containers::arrayView<Vector2>({
                {-25.25f, -0.5f},
                {-23.75f, -1.0f},
                {-22.25f, -0.5f},
                {-20.75f, -0.3333333f}, /* Second part starts here */
                {-18.75f, -1.0f},
            }), TestSuite::Compare::Container);
        /* Otherwise the first part is the same as already finalized above, and
           the second part is 23 letters with advance 2.0, so starting at -23
           when centered */
        else
            CORRADE_COMPARE_AS(renderer.glyphPositions().prefix(5), Containers::arrayView<Vector2>({
                {-2.25f, -0.25f},
                {-0.75f, -0.75f},
                { 0.75f, -0.25f},
                {-23.0f, -0.3333333f}, /* Second part starts here */
                {-21.0f, -1.0f},
            }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(renderer.glyphIds().prefix(5), Containers::arrayView<UnsignedInt>({
            2, 3, 1, 2, 3 /* font glyphs 0, 2, 4, 0, 2 */
        }), TestSuite::Compare::Container);
        if(data.flagsCore >= RendererCoreFlag::GlyphClusters) {
            CORRADE_COMPARE_AS(renderer.glyphClusters().prefix(5), Containers::arrayView<UnsignedInt>({
                10, 11, 12, 10, 11
            }), TestSuite::Compare::Container);
        }
        CORRADE_COMPARE_AS(renderer.runScales(), Containers::arrayView({
            1.0f,
            4.0f/3.0f,
            4.0f/3.0f
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(renderer.runEnds(), Containers::arrayView({
            3u,
            11u,
            26u
        }), TestSuite::Compare::Container);
    }
}

void RendererTest::allocateCoreGlyphAllocator() {
    auto&& data = AllocateCoreGlyphAllocatorData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
    } glyphCache{PixelFormat::R8Unorm, {16, 16}};

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;
    glyphCache.addFont(1, &font);

    struct: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(Containers::StringView text, UnsignedInt, UnsignedInt, Containers::ArrayView<const FeatureRange>) override {
            return text.size();
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>& ids) const override {
            /* Zero the IDs to not hit an OOB assert in the glyph cache */
            for(UnsignedInt& id: ids)
                id = 0;
        }
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&) const override {
            /* The data don't matter in this case */
        }
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {
            /* The data don't matter in this case */
        }
    } shaper{font};

    struct Allocation {
        bool expectedGlyphClusters;
        const Vector2* expectedGlyphPositionData;
        const UnsignedInt* expectedGlyphIdData;
        const UnsignedInt* expectedGlyphClusterData;
        const Vector2* expectedGlyphAdvanceData;

        UnsignedInt expectedViewSize;
        UnsignedInt expectedGlyphCount;

        Containers::StridedArrayView1D<Vector2> glyphPositions;
        Containers::StridedArrayView1D<Vector2> glyphAdvances;
        Containers::StridedArrayView1D<UnsignedInt> glyphIds;
        Containers::StridedArrayView1D<UnsignedInt> glyphClusters;
        int called = 0;
    } allocation;

    RendererCore renderer{glyphCache, [](void* state, UnsignedInt glyphCount, Containers::StridedArrayView1D<Vector2>& glyphPositions, Containers::StridedArrayView1D<UnsignedInt>& glyphIds, Containers::StridedArrayView1D<UnsignedInt>* glyphClusters, Containers::StridedArrayView1D<Vector2>& glyphAdvances){
        Allocation& allocation = *static_cast<Allocation*>(state);
        CORRADE_COMPARE(glyphCount, allocation.expectedGlyphCount);
        CORRADE_COMPARE(glyphPositions.data(), allocation.expectedGlyphPositionData);
        CORRADE_COMPARE(glyphPositions.size(), allocation.expectedViewSize);
        CORRADE_COMPARE(glyphIds.data(), allocation.expectedGlyphIdData);
        CORRADE_COMPARE(glyphIds.size(), allocation.expectedViewSize);
        CORRADE_COMPARE(glyphClusters, allocation.expectedGlyphClusters);
        if(glyphClusters) {
            CORRADE_COMPARE(glyphClusters->data(), allocation.expectedGlyphClusterData);
            CORRADE_COMPARE(glyphClusters->size(), allocation.expectedViewSize);
        }
        CORRADE_COMPARE(glyphAdvances.data(), allocation.expectedGlyphAdvanceData);
        /* The advances are never needed to be preserved, so it's always
           empty */
        CORRADE_COMPARE(glyphAdvances.size(), 0);

        glyphPositions = allocation.glyphPositions;
        glyphIds = allocation.glyphIds;
        if(glyphClusters)
            *glyphClusters = allocation.glyphClusters;
        glyphAdvances = allocation.glyphAdvances;
        ++allocation.called;
    }, &allocation, nullptr, nullptr, data.flags};

    /* Capture correct function name */
    CORRADE_VERIFY(true);

    /* Initially it should pass all null views */
    allocation.expectedViewSize = 0;
    allocation.expectedGlyphClusters = data.flags >= RendererCoreFlag::GlyphClusters;
    allocation.expectedGlyphPositionData = nullptr;
    allocation.expectedGlyphIdData = nullptr;
    allocation.expectedGlyphClusterData = nullptr;
    allocation.expectedGlyphAdvanceData = nullptr;

    /* Reserving with 0 should be a no-op */
    renderer.reserve(0, 0);
    CORRADE_COMPARE(allocation.called, 0);
    CORRADE_COMPARE(renderer.glyphCount(), 0);
    CORRADE_COMPARE(renderer.glyphCapacity(), 0);
    CORRADE_COMPARE(renderer.runCount(), 0);
    CORRADE_COMPARE(renderer.runCapacity(), 0);
    CORRADE_COMPARE(renderer.renderingGlyphCount(), 0);
    CORRADE_COMPARE(renderer.renderingRunCount(), 0);
    CORRADE_COMPARE(renderer.glyphPositions().size(), 0);
    CORRADE_COMPARE(renderer.glyphPositions().data(), nullptr);
    CORRADE_COMPARE(renderer.glyphIds().size(), 0);
    CORRADE_COMPARE(renderer.glyphIds().data(), nullptr);
    if(data.flags >= RendererCoreFlag::GlyphClusters) {
        CORRADE_COMPARE(renderer.glyphClusters().size(), 0);
        CORRADE_COMPARE(renderer.glyphClusters().data(), nullptr);
    }

    /* Rendering an empty text should be a no-op as well */
    if(data.render) {
        CORRADE_ITERATION(Utility::format("{}:{}", __FILE__, __LINE__));
        renderer.render(shaper, 0.0f, "");
        CORRADE_COMPARE(allocation.called, 0);
        CORRADE_COMPARE(renderer.glyphCount(), 0);
        CORRADE_COMPARE(renderer.glyphCapacity(), 0);
        CORRADE_COMPARE(renderer.runCount(), 0);
        CORRADE_COMPARE(renderer.runCapacity(), 0);
        CORRADE_COMPARE(renderer.renderingGlyphCount(), 0);
        CORRADE_COMPARE(renderer.renderingRunCount(), 0);
        CORRADE_COMPARE(renderer.glyphPositions().size(), 0);
        CORRADE_COMPARE(renderer.glyphPositions().data(), nullptr);
        CORRADE_COMPARE(renderer.glyphIds().size(), 0);
        CORRADE_COMPARE(renderer.glyphIds().data(), nullptr);
        if(data.flags >= RendererCoreFlag::GlyphClusters) {
            CORRADE_COMPARE(renderer.glyphClusters().size(), 0);
            CORRADE_COMPARE(renderer.glyphClusters().data(), nullptr);
        }
    }

    /* Reserve an initial size to have somewhere to render to, pass each view
       the same size */
    Vector2 glyphPositions[32];
    UnsignedInt glyphIds[32];
    UnsignedInt glyphClusters[32];
    Vector2 glyphAdvances[32];
    allocation.expectedViewSize = 0;
    allocation.expectedGlyphCount = data.reserve;
    allocation.glyphPositions = Containers::arrayView(glyphPositions)
        .prefix(data.reserve);
    allocation.glyphIds = Containers::arrayView(glyphIds)
        .prefix(data.reserve);
    allocation.glyphClusters = Containers::arrayView(glyphClusters)
        .prefix(data.reserve);
    allocation.glyphAdvances = Containers::arrayView(glyphAdvances)
        .prefix(data.reserve);
    {
        CORRADE_ITERATION(Utility::format("{}:{}", __FILE__, __LINE__));
        renderer.reserve(data.reserve, 0);
    }
    CORRADE_COMPARE(allocation.called, 1);
    CORRADE_COMPARE(renderer.glyphCount(), 0);
    CORRADE_COMPARE(renderer.glyphCapacity(), data.reserve);
    CORRADE_COMPARE(renderer.runCount(), 0);
    CORRADE_COMPARE(renderer.runCapacity(), 0);
    CORRADE_COMPARE(renderer.renderingGlyphCount(), 0);
    CORRADE_COMPARE(renderer.renderingRunCount(), 0);

    /* Rendering with enough capacity shouldn't reallocate anything */
    if(data.render) {
        CORRADE_ITERATION(Utility::format("{}:{}", __FILE__, __LINE__));
        renderer.add(shaper, 0.0f, "abc");
        if(data.renderAddOnly) {
            CORRADE_VERIFY(renderer.isRendering());
            CORRADE_COMPARE(renderer.glyphCount(), 0);
            CORRADE_COMPARE(renderer.runCount(), 0);
            CORRADE_COMPARE(renderer.glyphPositions().size(), 0);
            CORRADE_COMPARE(renderer.glyphIds().size(), 0);
            if(data.flags >= RendererCoreFlag::GlyphClusters)
                CORRADE_COMPARE(renderer.glyphClusters().size(), 0);
        } else {
            renderer.render();
            CORRADE_VERIFY(!renderer.isRendering());
            CORRADE_COMPARE(renderer.glyphCount(), 3);
            CORRADE_COMPARE(renderer.runCount(), 1);
            CORRADE_COMPARE(renderer.glyphPositions().size(), 3);
            CORRADE_COMPARE(renderer.glyphIds().size(), 3);
            if(data.flags >= RendererCoreFlag::GlyphClusters)
                CORRADE_COMPARE(renderer.glyphClusters().size(), 3);
        }
        CORRADE_COMPARE(allocation.called, 1);
        CORRADE_COMPARE(renderer.glyphCapacity(), data.reserve);
        CORRADE_COMPARE(renderer.runCapacity(), 1);
        CORRADE_COMPARE(renderer.renderingGlyphCount(), 3);
        CORRADE_COMPARE(renderer.renderingRunCount(), 1);
        /* No need to verify the actual contents, just that the views didn't
           change since last time */
        CORRADE_COMPARE(renderer.glyphPositions().data(), glyphPositions);
        CORRADE_COMPARE(renderer.glyphIds().data(), glyphIds);
        if(data.flags >= RendererCoreFlag::GlyphClusters)
            CORRADE_COMPARE(renderer.glyphClusters().data(), glyphClusters);
    }

    /* Reserve / render second time. Pass each with a different size, it should
       pick the smallest as capacity. */
    allocation.expectedGlyphPositionData = glyphPositions;
    allocation.expectedGlyphIdData = glyphIds;
    allocation.expectedGlyphClusterData = glyphClusters;
    allocation.expectedGlyphAdvanceData = glyphAdvances;
    Vector2 glyphPositions2[32];
    UnsignedInt glyphIds2[32];
    UnsignedInt glyphClusters2[32];
    Vector2 glyphAdvances2[32];
    allocation.glyphPositions = Containers::arrayView(glyphPositions2)
        .prefix(data.positionSize);
    allocation.glyphIds = Containers::arrayView(glyphIds2)
        .prefix(data.idSize);
    allocation.glyphClusters = Containers::arrayView(glyphClusters2)
        .prefix(data.clusterSize);
    allocation.glyphAdvances = Containers::arrayView(glyphAdvances2)
        .prefix(data.advanceSize);
    if(data.render) {
        CORRADE_ITERATION(Utility::format("{}:{}", __FILE__, __LINE__));
        allocation.expectedViewSize = 3;
        allocation.expectedGlyphCount = data.secondReserve - 3;
        renderer.render(shaper, 0.0f, "defghijklmnopqrstuvwxyz");
        CORRADE_COMPARE(renderer.glyphCount(), 26);
        CORRADE_COMPARE(renderer.runCount(), 2);
        CORRADE_COMPARE(renderer.runCapacity(), 2);
        CORRADE_COMPARE(renderer.renderingGlyphCount(), 26);
        CORRADE_COMPARE(renderer.renderingRunCount(), 2);
    } else {
        CORRADE_ITERATION(Utility::format("{}:{}", __FILE__, __LINE__));
        allocation.expectedViewSize = 0;
        allocation.expectedGlyphCount = data.secondReserve;
        renderer.reserve(data.secondReserve, 0);
        CORRADE_COMPARE(renderer.glyphCount(), 0);
        CORRADE_COMPARE(renderer.runCount(), 0);
        CORRADE_COMPARE(renderer.runCapacity(), 0);
        CORRADE_COMPARE(renderer.renderingGlyphCount(), 0);
        CORRADE_COMPARE(renderer.renderingRunCount(), 0);
    }
    CORRADE_COMPARE(renderer.glyphCapacity(), data.expectedCapacity);

    /* If it shouldn't reallocate, the views should stay the same as before,
       otherwise they should be what was passed above. The allocator is assumed
       to perform the data copy, the one in this test deliberately doesn't. */
    if(data.expectNoReallocation) {
        CORRADE_COMPARE(allocation.called, 1);
        CORRADE_COMPARE(renderer.glyphPositions().data(), glyphPositions);
        CORRADE_COMPARE(renderer.glyphIds().data(), glyphIds);
        if(data.flags >= RendererCoreFlag::GlyphClusters)
            CORRADE_COMPARE(renderer.glyphClusters().data(), glyphClusters);
    } else {
        CORRADE_COMPARE(allocation.called, 2);
        CORRADE_COMPARE(renderer.glyphPositions().data(), glyphPositions2);
        CORRADE_COMPARE(renderer.glyphIds().data(), glyphIds2);
        if(data.flags >= RendererCoreFlag::GlyphClusters)
            CORRADE_COMPARE(renderer.glyphClusters().data(), glyphClusters2);
    }
}

void RendererTest::allocateCoreGlyphAllocatorInvalid() {
    auto&& data = AllocateCoreGlyphAllocatorInvalidData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
    } glyphCache{PixelFormat::R8Unorm, {16, 16}};

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;
    glyphCache.addFont(1, &font);

    struct: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(Containers::StringView text, UnsignedInt, UnsignedInt, Containers::ArrayView<const FeatureRange>) override {
            return text.size();
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>& ids) const override {
            /* Zero the IDs to not hit an OOB assert in the glyph cache */
            for(UnsignedInt& id: ids)
                id = 0;
        }
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&) const override {
            /* The data don't matter in this case */
        }
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {
            /* The data don't matter in this case */
        }
    } shaper{font};

    struct Allocation {
        Vector2 glyphPositions[20];
        UnsignedInt glyphIds[20];
        UnsignedInt glyphClusters[20];
        Vector2 glyphAdvances[20];

        /* For the initial render() */
        UnsignedInt positionSize = 10;
        UnsignedInt idSize = 10;
        UnsignedInt clusterSize = 10;
        UnsignedInt advanceSize = 10;
    } allocation;

    RendererCore renderer{glyphCache, [](void* state, UnsignedInt, Containers::StridedArrayView1D<Vector2>& glyphPositions, Containers::StridedArrayView1D<UnsignedInt>& glyphIds, Containers::StridedArrayView1D<UnsignedInt>* glyphClusters, Containers::StridedArrayView1D<Vector2>& glyphAdvances){
        Allocation& allocation = *static_cast<Allocation*>(state);

        glyphPositions = Containers::arrayView(allocation.glyphPositions).prefix(allocation.positionSize);
        glyphIds = Containers::arrayView(allocation.glyphIds).prefix(allocation.idSize);
        if(glyphClusters)
            *glyphClusters = Containers::arrayView(allocation.glyphClusters).prefix(allocation.clusterSize);
        glyphAdvances = Containers::arrayView(allocation.glyphAdvances).prefix(allocation.advanceSize);
    }, &allocation, nullptr, nullptr, data.flags};

    /* Render something to have a non-zero glyph count */
    renderer.render(shaper, 0.0f, "abcdefghij");
    CORRADE_COMPARE(renderer.glyphCount(), 10);
    CORRADE_COMPARE(renderer.glyphCapacity(), 10);

    /* Next reserve / render should be with these */
    allocation.positionSize = data.positionSize;
    allocation.idSize = data.idSize;
    allocation.clusterSize = data.clusterSize;
    allocation.advanceSize = data.advanceSize;
    {
        Containers::String out;
        Error redirectError{&out};
        if(data.render)
            renderer.render(shaper, 0.0f, "klmnopq");
        else
            renderer.reserve(17, 0);
        CORRADE_COMPARE_AS(out, data.expected,
            TestSuite::Compare::String);
    }

    /* Just to verify it's okay when the sizes are exactly right */
    allocation.positionSize = 17;
    allocation.idSize = 17;
    allocation.clusterSize = 17;
    allocation.advanceSize = 7; /* This one in particular */
    if(data.render) {
        renderer.render(shaper, 0.0f, "klmnopq");
        CORRADE_COMPARE(renderer.glyphCount(), 17);
    } else {
        renderer.reserve(17, 0);
        CORRADE_COMPARE(renderer.glyphCount(), 10);
    }
    CORRADE_COMPARE(renderer.glyphCapacity(), 17);
}

void RendererTest::allocateCoreRunAllocator() {
    auto&& data = AllocateCoreRunAllocatorData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
    } glyphCache{PixelFormat::R8Unorm, {16, 16}};

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;
    glyphCache.addFont(1, &font);

    struct: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(Containers::StringView text, UnsignedInt, UnsignedInt, Containers::ArrayView<const FeatureRange>) override {
            return text.size();
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>& ids) const override {
            /* Zero the IDs to not hit an OOB assert in the glyph cache */
            for(UnsignedInt& id: ids)
                id = 0;
        }
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&) const override {
            /* The data don't matter in this case */
        }
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {
            /* The data don't matter in this case */
        }
    } shaper{font};

    struct Allocation {
        const Float* expectedRunScaleData;
        const UnsignedInt* expectedRunEndData;

        UnsignedInt expectedViewSize;
        UnsignedInt expectedRunCount;

        Containers::StridedArrayView1D<Float> runScales;
        Containers::StridedArrayView1D<UnsignedInt> runEnds;
        int called = 0;
    } allocation;

    RendererCore renderer{glyphCache, nullptr, nullptr, [](void* state, UnsignedInt runCount, Containers::StridedArrayView1D<Float>& runScales, Containers::StridedArrayView1D<UnsignedInt>& runEnds){
        Allocation& allocation = *static_cast<Allocation*>(state);
        CORRADE_COMPARE(runCount, allocation.expectedRunCount);
        CORRADE_COMPARE(runScales.data(), allocation.expectedRunScaleData);
        CORRADE_COMPARE(runScales.size(), allocation.expectedViewSize);
        CORRADE_COMPARE(runEnds.data(), allocation.expectedRunEndData);
        CORRADE_COMPARE(runEnds.size(), allocation.expectedViewSize);

        runScales = allocation.runScales;
        runEnds = allocation.runEnds;
        ++allocation.called;
    }, &allocation};

    /* Capture correct function name */
    CORRADE_VERIFY(true);

    /* Initially it should pass all null views */
    allocation.expectedViewSize = 0;
    allocation.expectedRunScaleData = nullptr;
    allocation.expectedRunEndData = nullptr;

    /* Reserving with 0 should be a no-op */
    renderer.reserve(0, 0);
    CORRADE_COMPARE(allocation.called, 0);
    CORRADE_COMPARE(renderer.glyphCount(), 0);
    CORRADE_COMPARE(renderer.glyphCapacity(), 0);
    CORRADE_COMPARE(renderer.runCount(), 0);
    CORRADE_COMPARE(renderer.runCapacity(), 0);
    CORRADE_COMPARE(renderer.renderingGlyphCount(), 0);
    CORRADE_COMPARE(renderer.renderingRunCount(), 0);
    CORRADE_COMPARE(renderer.runScales().size(), 0);
    CORRADE_COMPARE(renderer.runScales().data(), nullptr);
    CORRADE_COMPARE(renderer.runEnds().size(), 0);
    CORRADE_COMPARE(renderer.runEnds().data(), nullptr);

    /* Rendering an empty text should be a no-op as well, even with multiple
       add() calls */
    if(data.render) {
        CORRADE_ITERATION(Utility::format("{}:{}", __FILE__, __LINE__));
        renderer
            .add(shaper, 0.0f, "")
            .add(shaper, 0.0f, "")
            .render(shaper, 0.0f, "");
        CORRADE_COMPARE(allocation.called, 0);
        CORRADE_COMPARE(renderer.glyphCount(), 0);
        CORRADE_COMPARE(renderer.runCount(), 0);
        CORRADE_COMPARE(renderer.runCapacity(), 0);
        CORRADE_COMPARE(renderer.renderingGlyphCount(), 0);
        CORRADE_COMPARE(renderer.renderingRunCount(), 0);
        CORRADE_COMPARE(renderer.runScales().size(), 0);
        CORRADE_COMPARE(renderer.runScales().data(), nullptr);
        CORRADE_COMPARE(renderer.runEnds().size(), 0);
        CORRADE_COMPARE(renderer.runEnds().data(), nullptr);
    }

    /* Reserve an initial size to have somewhere to render to, pass each view
       the same size */
    Float runScales[8];
    UnsignedInt runEnds[8];
    allocation.expectedViewSize = 0;
    allocation.expectedRunCount = data.reserve;
    allocation.runScales = Containers::arrayView(runScales)
        .prefix(data.reserve);
    allocation.runEnds = Containers::arrayView(runEnds)
        .prefix(data.reserve);
    {
        CORRADE_ITERATION(Utility::format("{}:{}", __FILE__, __LINE__));
        renderer.reserve(0, data.reserve);
    }
    CORRADE_COMPARE(allocation.called, 1);
    CORRADE_COMPARE(renderer.glyphCount(), 0);
    CORRADE_COMPARE(renderer.glyphCapacity(), 0);
    CORRADE_COMPARE(renderer.runCount(), 0);
    CORRADE_COMPARE(renderer.runCapacity(), data.reserve);
    CORRADE_COMPARE(renderer.renderingGlyphCount(), 0);
    CORRADE_COMPARE(renderer.renderingRunCount(), 0);

    /* Rendering with enough capacity shouldn't reallocate anything */
    if(data.render) {
        CORRADE_ITERATION(Utility::format("{}:{}", __FILE__, __LINE__));
        renderer
            .add(shaper, 0.0f, "a")
            .add(shaper, 0.0f, "b")
            .add(shaper, 0.0f, "c");
        if(data.renderAddOnly) {
            CORRADE_VERIFY(renderer.isRendering());
            CORRADE_COMPARE(renderer.glyphCount(), 0);
            CORRADE_COMPARE(renderer.runCount(), 0);
            CORRADE_COMPARE(renderer.runScales().size(), 0);
            CORRADE_COMPARE(renderer.runEnds().size(), 0);
        } else {
            renderer.render();
            CORRADE_VERIFY(!renderer.isRendering());
            CORRADE_COMPARE(renderer.glyphCount(), 3);
            CORRADE_COMPARE(renderer.runCount(), 3);
            CORRADE_COMPARE(renderer.runScales().size(), 3);
            CORRADE_COMPARE(renderer.runEnds().size(), 3);
        }
        CORRADE_COMPARE(allocation.called, 1);
        CORRADE_COMPARE(renderer.glyphCapacity(), 3);
        CORRADE_COMPARE(renderer.runCapacity(), data.reserve);
        CORRADE_COMPARE(renderer.renderingGlyphCount(), 3);
        CORRADE_COMPARE(renderer.renderingRunCount(), 3);
        /* No need to verify the actual contents, just that the views didn't
           change since last time */
        CORRADE_COMPARE(renderer.runScales().data(), runScales);
        CORRADE_COMPARE(renderer.runEnds().data(), runEnds);
    }

    /* Reserve / render second time. Pass each with a different size, it should
       pick the smallest as capacity. */
    allocation.expectedRunScaleData = runScales;
    allocation.expectedRunEndData = runEnds;
    Float runScales2[8];
    UnsignedInt runEnds2[8];
    allocation.runScales = Containers::arrayView(runScales2)
        .prefix(data.scaleSize);
    allocation.runEnds = Containers::arrayView(runEnds2)
        .prefix(data.endSize);
    if(data.render) {
        CORRADE_ITERATION(Utility::format("{}:{}", __FILE__, __LINE__));
        allocation.expectedViewSize = 3;
        allocation.expectedRunCount = 1;
        renderer.render(shaper, 0.0f, "defghijklmnopqrstuvwxyz");
        CORRADE_COMPARE(renderer.glyphCount(), 26);
        CORRADE_COMPARE(renderer.glyphCapacity(), 26);
        CORRADE_COMPARE(renderer.runCount(), 4);
        CORRADE_COMPARE(renderer.renderingGlyphCount(), 26);
        CORRADE_COMPARE(renderer.renderingRunCount(), 4);
    } else {
        CORRADE_ITERATION(Utility::format("{}:{}", __FILE__, __LINE__));
        allocation.expectedViewSize = 0;
        allocation.expectedRunCount = data.secondReserve;
        renderer.reserve(0, data.secondReserve);
        CORRADE_COMPARE(renderer.glyphCount(), 0);
        CORRADE_COMPARE(renderer.glyphCapacity(), 0);
        CORRADE_COMPARE(renderer.runCount(), 0);
        CORRADE_COMPARE(renderer.renderingGlyphCount(), 0);
        CORRADE_COMPARE(renderer.renderingRunCount(), 0);
    }
    CORRADE_COMPARE(renderer.runCapacity(), data.expectedCapacity);

    /* If it shouldn't reallocate, the views should stay the same as before,
       otherwise they should be what was passed above. The allocator is assumed
       to perform the data copy, the one in this test deliberately doesn't. */
    if(data.expectNoReallocation) {
        CORRADE_COMPARE(allocation.called, 1);
        CORRADE_COMPARE(renderer.runScales().data(), runScales);
        CORRADE_COMPARE(renderer.runEnds().data(), runEnds);
    } else {
        CORRADE_COMPARE(allocation.called, 2);
        CORRADE_COMPARE(renderer.runScales().data(), runScales2);
        CORRADE_COMPARE(renderer.runEnds().data(), runEnds2);
    }
}

void RendererTest::allocateCoreRunAllocatorInvalid() {
    auto&& data = AllocateCoreRunAllocatorInvalidData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
    } glyphCache{PixelFormat::R8Unorm, {16, 16}};

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font;
    glyphCache.addFont(1, &font);

    struct: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(Containers::StringView text, UnsignedInt, UnsignedInt, Containers::ArrayView<const FeatureRange>) override {
            return text.size();
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>& ids) const override {
            /* Zero the IDs to not hit an OOB assert in the glyph cache */
            for(UnsignedInt& id: ids)
                id = 0;
        }
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&) const override {
            /* The data don't matter in this case */
        }
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {
            /* The data don't matter in this case */
        }
    } shaper{font};

    struct Allocation {
        Float runScales[8];
        UnsignedInt runEnds[8];

        /* For the initial render() */
        UnsignedInt scaleSize = 2;
        UnsignedInt endSize = 2;
    } allocation;

    RendererCore renderer{glyphCache, nullptr, nullptr, [](void* state, UnsignedInt, Containers::StridedArrayView1D<Float>& runScales, Containers::StridedArrayView1D<UnsignedInt>& runEnds){
        Allocation& allocation = *static_cast<Allocation*>(state);

        runScales = Containers::arrayView(allocation.runScales).prefix(allocation.scaleSize);
        runEnds = Containers::arrayView(allocation.runEnds).prefix(allocation.endSize);
    }, &allocation};

    /* Render something to have a non-zero run count */
    renderer
        .add(shaper, 0.0f, "abcde")
        .render(shaper, 0.0f, "fghij");
    CORRADE_COMPARE(renderer.runCount(), 2);
    CORRADE_COMPARE(renderer.runCapacity(), 2);

    /* Next reserve / render should be with these */
    allocation.scaleSize = data.scaleSize;
    allocation.endSize = data.endSize;
    {
        if(data.render)
            renderer
                .add(shaper, 0.0f, "kl")
                .render(shaper, 0.0f, "mn");

        Containers::String out;
        Error redirectError{&out};
        if(data.render)
            renderer.render(shaper, 0.0f, "opq");
        else
            renderer.reserve(0, 5);
        CORRADE_COMPARE_AS(out, data.expected,
            TestSuite::Compare::String);
    }

    /* Just to verify it's okay when the sizes are exactly right */
    allocation.scaleSize = 5;
    allocation.endSize = 5;
    if(data.render) {
        renderer.render(shaper, 0.0f, "opq");
        CORRADE_COMPARE(renderer.runCount(), 5);
    } else {
        renderer.reserve(0, 5);
        CORRADE_COMPARE(renderer.runCount(), 2);
    }
    CORRADE_COMPARE(renderer.runCapacity(), 5);
}

void RendererTest::addSingleLine() {
    auto&& data = AddSingleLineData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
    /* Set padding to zero for easier dummy glyph addition below */
    } glyphCache{PixelFormat::R8Unorm, {16, 16}, {}};

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        Properties doOpenFile(Containers::StringView, Float size) override {
            _opened = true;
            /* The size is used to scale everything. Ascent, descent is used
               for the bounds rect. Line height isn't used for anything, glyph
               count is overriden in addFont() below. */
            return {size, 16.0f, -8.0f, 1000.0f, 0};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        private:
            bool _opened = false;
    } font1, font2;
    /* Two fonts that do the same but each is opened with a different size */
    font1.openFile("", 1.0f);
    font2.openFile("", 2.0f);
    for(AbstractFont* font: {&font1, &font2}) {
        UnsignedInt fontId = glyphCache.addFont('o' + 1, font);
        /* Shuffled order to not have their IDs match the clusters */
        glyphCache.addGlyph(fontId, 'e', {}, {}); /* 1 or 9 */
        glyphCache.addGlyph(fontId, 'E', {}, {}); /* 2 or 10 */
        glyphCache.addGlyph(fontId, 'l', {}, {}); /* 3 or 11 */
        glyphCache.addGlyph(fontId, 'H', {}, {}); /* 4 or 12 */
        glyphCache.addGlyph(fontId, 'L', {}, {}); /* 5 or 13 */
        glyphCache.addGlyph(fontId, 'h', {}, {}); /* 6 or 14 */
        glyphCache.addGlyph(fontId, 'O', {}, {}); /* 7 or 15 */
        glyphCache.addGlyph(fontId, 'o', {}, {}); /* 8 or 16 */
    }

    struct Shaper: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(Containers::StringView text, UnsignedInt begin, UnsignedInt end, Containers::ArrayView<const FeatureRange> features) override {
            if(begin == advertiseShapeDirectionAt)
                _direction = shapeDirectionToAdvertise;
            else
                _direction = ShapeDirection::Unspecified;

            /* The text is always the same, the begin / end is different */
            CORRADE_COMPARE(text, expectedText);
            CORRADE_COMPARE(begin, expectedBegin);
            CORRADE_COMPARE(end, expectedEnd);

            /* Verify just that these are passed at all, it's always the same */
            CORRADE_COMPARE(features.size(), 2);
            CORRADE_COMPARE(features[1].feature(), Feature::CharacterVariants66);

            /* Produce twice as many glyphs for the input to verify it's not a
               1:1 mapping from bytes to glyphs */
            return (end - begin)*2;
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>& ids) const override {
            /* Each input letter is mapped to a pair of uppercase and
               lowercase chars, which act as glyph IDs */
            for(UnsignedInt i = 0; i != ids.size(); ++i) {
                ids[i] = expectedText[expectedBegin + i/2];
                if(i % 2 == 0)
                    ids[i] &= ~('A' ^ 'a');
            }
        }
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>& offsets, const Containers::StridedArrayView1D<Vector2>& advances) const override {
            /* Uppercase letters have bigger advance than lowercase, L is
               special, lowercase additionally have an Y offset, except L.

               Undoing the size multiplier here so the final output has still
               the same absolute advances and only scales the ascent/descent. */
            for(UnsignedInt i = 0; i != offsets.size(); ++i) {
                char glyphId = expectedText[expectedBegin + i/2];
                if(glyphId == 'h' || glyphId == 'e' || glyphId == 'o')
                    advances[i] = {i % 2 ? 4.0f/(sizeMultiplier/font().size()) : 6.0f/(sizeMultiplier/font().size()), 0.0f};
                else if(glyphId == 'l')
                    advances[i] = {3.0f/(sizeMultiplier/font().size()), 0.0f};
                else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

                if(i % 2 && (glyphId == 'h' || glyphId == 'e' || glyphId == 'o'))
                    offsets[i] = {0.0f, -1.0f/(sizeMultiplier/font().size())};
                else
                    offsets[i] = {0.0f, 0.0f};
            }
        }
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>& clusters) const override {
            for(UnsignedInt i = 0; i != clusters.size(); ++i)
                clusters[i] = expectedBegin + i/2;
        }
        ShapeDirection doDirection() const override {
            /* In case of a single line shape() should always get called before
               direction is queried. In a multi-line scenario not, which is
               verified in addMultipleLines() below. */
            CORRADE_FAIL_IF(_direction == ShapeDirection(0xff),
                "Shape direction queried before calling shape()");
            return _direction;
        }

        ShapeDirection shapeDirectionToAdvertise;
        UnsignedInt advertiseShapeDirectionAt;
        Float sizeMultiplier;

        const char* expectedText;
        UnsignedInt expectedBegin, expectedEnd;

        private:
            ShapeDirection _direction = ShapeDirection(0xff);
    } shaper1{font1}, shaper2{font2};
    for(Shaper* shaper: {&shaper1, &shaper2}) {
        shaper->shapeDirectionToAdvertise = data.shapeDirection;
        shaper->advertiseShapeDirectionAt = data.advertiseShapeDirectionAt;
    }

    RendererCore renderer{glyphCache, data.flags};
    renderer
        /* Non-default cursor position */
        .setCursor({-50.0f, 100.0f})
        /* Alignment to LineRight, but can be specified as start / end and then
           it'd depend on used LayoutDirection */
        .setAlignment(data.alignment);

    /* Capture correct function name */
    CORRADE_VERIFY(true);

    Containers::Pair<Range2D, Range1Dui> out;
    if(data.direct) {
        CORRADE_ITERATION(Utility::format("{}:{}", __FILE__, __LINE__));
        CORRADE_COMPARE(data.items.size(), 1);
        auto& item = data.items[0];
        shaper1.sizeMultiplier = item.third();
        shaper1.expectedText = "hello";
        shaper1.expectedBegin = item.first();
        shaper1.expectedEnd = item.second();
        out = renderer.render(shaper1, item.third(), "hello", {
            Feature::Kerning,
            Feature::CharacterVariants66
        });
    } else {
        CORRADE_ITERATION(Utility::format("{}:{}", __FILE__, __LINE__));
        for(std::size_t i = 0; i != data.items.size(); ++i) {
            auto& item = data.items[i];
            CORRADE_ITERATION(item);

            Shaper& shaper = i % 2 ? shaper2 : shaper1;

            shaper.sizeMultiplier = item.third();
            shaper.expectedText = "___hello--";
            shaper.expectedBegin = item.first();
            shaper.expectedEnd = item.second();
            renderer.add(shaper, item.third(), "___hello--", item.first(), item.second(), {
                Feature::Kerning,
                Feature::CharacterVariants66
            });

            /* The cursor should stay as set initially, only the "rendering"
               count gets updated */
            CORRADE_COMPARE(renderer.cursor(), (Vector2{-50.0f, 100.0f}));
            CORRADE_COMPARE(renderer.glyphCount(), 0);
            CORRADE_COMPARE(renderer.glyphPositions().size(), 0);
            CORRADE_COMPARE(renderer.glyphIds().size(), 0);
            if(data.flags >= RendererCoreFlag::GlyphClusters)
                CORRADE_COMPARE(renderer.glyphClusters().size(), 0);
            CORRADE_COMPARE(renderer.runCount(), 0);
            /* Not testing the "rendering" counts here as it's too laborous,
               only at the end */
            CORRADE_COMPARE(renderer.runScales().size(), 0);
            CORRADE_COMPARE(renderer.runEnds().size(), 0);
        }

        out = renderer.render();
    }

    /* At the end, it shouldn't be in progress anymore. The cursor should be
       still as set initially. */
    CORRADE_VERIFY(!renderer.isRendering());
    CORRADE_COMPARE(renderer.glyphCount(), 10);
    CORRADE_COMPARE(renderer.runCount(), data.expectedRuns.size());
    CORRADE_COMPARE(renderer.renderingGlyphCount(), 10);
    CORRADE_COMPARE(renderer.renderingRunCount(), data.expectedRuns.size());
    CORRADE_COMPARE(renderer.cursor(), (Vector2{-50.0f, 100.0f}));
    CORRADE_COMPARE_AS(renderer.glyphCapacity(), 10,
        TestSuite::Compare::GreaterOrEqual);
    CORRADE_COMPARE_AS(renderer.runCapacity(), data.expectedRuns.size(),
        TestSuite::Compare::GreaterOrEqual);

    /* The rendered output should have 2x as many glyphs as input bytes, should
       have the right baseline at the cursor in all cases and the rect height
       should be depending on the largest font size. */
    CORRADE_COMPARE(out, Containers::pair(
        Range2D::fromSize({-42.0f, -data.expectedRectHeight/3.0f},
                          {42.0f, data.expectedRectHeight})
            .translated({-50.0f, 100.0f}),
        Range1Dui{0, UnsignedInt(data.expectedRuns.size())}));

    /* The contents should be the same independently of how many pieces was
       added. All glyph positions are shifted based on the cursor. */
    CORRADE_COMPARE_AS(renderer.glyphPositions(), Containers::arrayView<Vector2>({
        {-50.0f - 42.0f, 100.0f - 0.0f}, /* H */
        {-50.0f - 36.0f, 100.0f - 1.0f}, /* h */
        {-50.0f - 32.0f, 100.0f - 0.0f}, /* E */
        {-50.0f - 26.0f, 100.0f - 1.0f}, /* e */
        {-50.0f - 22.0f, 100.0f - 0.0f}, /* L */
        {-50.0f - 19.0f, 100.0f - 0.0f}, /* l */
        {-50.0f - 16.0f, 100.0f - 0.0f}, /* L */
        {-50.0f - 13.0f, 100.0f - 0.0f}, /* l */
        {-50.0f - 10.0f, 100.0f - 0.0f}, /* O */
        {-50.0f -  4.0f, 100.0f - 1.0f}, /* o */
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(renderer.glyphIds(),
        Containers::arrayView(data.expectedGlyphIds),
        TestSuite::Compare::Container);
    if(data.flags >= RendererCoreFlag::GlyphClusters) {
        if(data.direct)
            CORRADE_COMPARE_AS(renderer.glyphClusters(), Containers::arrayView<UnsignedInt>({
                0, 0, 1, 1, 2, 2, 3, 3, 4, 4
            }), TestSuite::Compare::Container);
        else
            CORRADE_COMPARE_AS(renderer.glyphClusters(), Containers::arrayView<UnsignedInt>({
                3, 3, 4, 4, 5, 5, 6, 6, 7, 7
            }), TestSuite::Compare::Container);
    }
    CORRADE_COMPARE_AS(renderer.runScales(),
        stridedArrayView(data.expectedRuns).slice(&Containers::Pair<Float, UnsignedInt>::first),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(renderer.runEnds(),
        stridedArrayView(data.expectedRuns).slice(&Containers::Pair<Float, UnsignedInt>::second),
        TestSuite::Compare::Container);
}

void RendererTest::addSingleLineAlign() {
    auto&& data = AddSingleLineAlignData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    TestFont font;
    font.openFile({}, 0.5f);
    DummyGlyphCache glyphCache = testGlyphCache(font);
    TestShaper shaper{font, data.shapeDirection};

    RendererCore renderer{glyphCache};
    renderer.setAlignment(data.alignment);

    /* Bounds are different depending on whether or not GlyphBounds alignment
       is used */
    CORRADE_COMPARE(renderer.render(shaper, 0.25f, "abc"), Containers::pair(
        (UnsignedByte(data.alignment) & Implementation::AlignmentGlyphBounds ?
            Range2D{{2.5f, 3.75f}, {12.5f, 10.5f}} :
            Range2D{{0.0f, -1.25f}, {3.0f, 2.25f}}).translated(data.offset),
        Range1Dui{0, 1}));

    CORRADE_COMPARE_AS(renderer.glyphPositions(), Containers::arrayView<Vector2>({
        /* Cursor is {0, 0}. Glyph offset {0, 1}, scaled by 0.5. */
        Vector2{0.0f, 0.5f} + data.offset,

        /* Advance was {1, 0.5}*0.5, cursor is {0.5, 0.25}. Glyph offset is
           {0, 2}, scaled by 0.5. */
        Vector2{0.5f, 1.25f} + data.offset,

        /* Advance was {2, -0.5}*0.5, cursor is {1.5, 0}. Glyph offset is
           {0, 3}, scaled by 0.5. */
        Vector2{1.5f, 1.5f} + data.offset,
    }), TestSuite::Compare::Container);
}

void RendererTest::addMultipleLines() {
    auto&& data = AddMultipleLinesData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Expanded variant of addSingleLine() with newlines being a part of the
       text and optional line advance adjustment in exchange for dropped size
       multiplication */

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
    /* Set padding to zero for easier dummy glyph addition below */
    } glyphCache{PixelFormat::R8Unorm, {16, 16}, {}};

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        Properties doOpenFile(Containers::StringView, Float size) override {
            _opened = true;
            /* The size is used to scale everything. Ascent, descent, line
               height is used for the bounds rect. Glyph count is overriden in
               addFont() below. */
            return {size, 16.0f*size, -8.0f*size, 32.0f*size, 0};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        private:
            bool _opened = false;
    } font1, font2;
    font1.openFile("", 1.0f);
    font2.openFile("", 2.0f);
    for(AbstractFont* font: {&font1, &font2}) {
        UnsignedInt fontId = glyphCache.addFont('o' + 1, font);
        /* Shuffled order to not have their IDs match the clusters */
        glyphCache.addGlyph(fontId, 'e', {}, {}); /* 1 or 9 */
        glyphCache.addGlyph(fontId, 'E', {}, {}); /* 2 or 10 */
        glyphCache.addGlyph(fontId, 'l', {}, {}); /* 3 or 11 */
        glyphCache.addGlyph(fontId, 'H', {}, {}); /* 4 or 12 */
        glyphCache.addGlyph(fontId, 'L', {}, {}); /* 5 or 13 */
        glyphCache.addGlyph(fontId, 'h', {}, {}); /* 6 or 14 */
        glyphCache.addGlyph(fontId, 'O', {}, {}); /* 7 or 15 */
        glyphCache.addGlyph(fontId, 'o', {}, {}); /* 8 or 16 */
    }

    struct Shaper: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(Containers::StringView text, UnsignedInt begin, UnsignedInt end, Containers::ArrayView<const FeatureRange> features) override {
            if(begin == advertiseShapeDirectionAt)
                _direction = shapeDirectionToAdvertise;
            else
                _direction = ShapeDirection::Unspecified;

            /* The text is always the same, the begin / end is different */
            CORRADE_COMPARE(text, expectedText);
            currentBegin = begin;
            arrayAppend(calls, InPlaceInit, begin, end);

            /* Verify just that these are passed at all, it's always the same */
            CORRADE_COMPARE(features.size(), 2);
            CORRADE_COMPARE(features[1].feature(), Feature::CharacterVariants66);

            /* Produce twice as many glyphs for the input to verify it's not a
               1:1 mapping from bytes to glyphs */
            return (end - begin)*2;
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>& ids) const override {
            /* Each input letter is mapped to a pair of uppercase and
               lowercase chars, which act as glyph IDs */
            for(UnsignedInt i = 0; i != ids.size(); ++i) {
                ids[i] = expectedText[currentBegin + i/2];
                if(i % 2 == 0)
                    ids[i] &= ~('A' ^ 'a');
            }
        }
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>& offsets, const Containers::StridedArrayView1D<Vector2>& advances) const override {
            /* Uppercase letters have bigger advance than lowercase, L is
               special, lowercase additionally have an Y offset, except L. */
            for(UnsignedInt i = 0; i != offsets.size(); ++i) {
                char glyphId = expectedText[currentBegin + i/2];
                if(glyphId == 'h' || glyphId == 'e' || glyphId == 'o')
                    advances[i] = {i % 2 ? 4.0f*font().size() : 6.0f*font().size(), 0.0f};
                else if(glyphId == 'l')
                    advances[i] = {3.0f*font().size(), 0.0f};
                else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

                if(i % 2 && (glyphId == 'h' || glyphId == 'e' || glyphId == 'o'))
                    offsets[i] = {0.0f, -1.0f*font().size()};
                else
                    offsets[i] = {0.0f, 0.0f};
            }
        }
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>& clusters) const override {
            for(UnsignedInt i = 0; i != clusters.size(); ++i)
                clusters[i] = currentBegin + i/2;
        }
        ShapeDirection doDirection() const override {
            return _direction;
        }

        ShapeDirection shapeDirectionToAdvertise;
        UnsignedInt advertiseShapeDirectionAt;

        const char* expectedText;
        UnsignedInt currentBegin;
        Containers::Array<Containers::Pair<UnsignedInt, UnsignedInt>> calls;

        private:
            /* It may happen that direction is queried even before shape(), in
               particular in the "each successive line separately with \n at
               the beginning" case, so provide a non-random value there */
            ShapeDirection _direction = ShapeDirection::Unspecified;
    } shaper1{font1}, shaper2{font2};
    for(Shaper* shaper: {&shaper1, &shaper2}) {
        shaper->shapeDirectionToAdvertise = data.shapeDirection;
        shaper->advertiseShapeDirectionAt = data.advertiseShapeDirectionAt;
    }

    RendererCore renderer{glyphCache, data.flags};
    renderer
        /* Non-default cursor position */
        .setCursor({-50.0f, 100.0f})
        /* Alignment to the right / bottom, but can be specified as start / end
           and then it'd depend on used LayoutDirection */
        .setAlignment(data.alignment);
    if(data.lineAdvance != 0.0f)
        renderer.setLineAdvance(data.lineAdvance);

    /* Capture correct function name */
    CORRADE_VERIFY(true);

    Containers::Pair<Range2D, Range1Dui> out;
    if(data.direct) {
        CORRADE_ITERATION(Utility::format("{}:{}", __FILE__, __LINE__));
        CORRADE_COMPARE(data.items.size(), 1);
        shaper1.expectedText = "he\nll\n\no";
        out = renderer.render(shaper1, 1.0f, "he\nll\n\no", {
            Feature::Kerning,
            Feature::CharacterVariants66
        });
        CORRADE_COMPARE_AS(shaper1.calls,
            /* This is always three items for three lines */
            Containers::arrayView(data.items[0].third()),
            TestSuite::Compare::Container);
    } else {
        CORRADE_ITERATION(Utility::format("{}:{}", __FILE__, __LINE__));
        for(std::size_t i = 0; i != data.items.size(); ++i) {
            auto& item = data.items[i];
            CORRADE_ITERATION(Containers::pair(item.first(), item.second()));

            Shaper& shaper = i % 2 ? shaper2 : shaper1;

            /* Extra newline characters outside of the desired range shouldn't
               be taken into account in any way */
            shaper.calls = {};
            shaper.expectedText = "\n\n_he\nll\n\no-\n";
            renderer.add(shaper, 1.0f, "\n\n_he\nll\n\no-\n", item.first(), item.second(), {
                Feature::Kerning,
                Feature::CharacterVariants66
            });
            /* Consider only the non-empty prefix in the expected output */
            std::size_t prefix = 0;
            for(Containers::Pair<UnsignedInt, UnsignedInt> j: item.third())
                if(j == Containers::pair(0u, 0u))
                    break;
                else
                    ++prefix;
            CORRADE_COMPARE_AS(shaper.calls,
                Containers::arrayView(item.third()).prefix(prefix),
                TestSuite::Compare::Container);

            /* The cursor should stay as set initially, only the "rendering"
               count gets updated */
            CORRADE_COMPARE(renderer.cursor(), (Vector2{-50.0f, 100.0f}));
            CORRADE_COMPARE(renderer.glyphCount(), 0);
            CORRADE_COMPARE(renderer.glyphPositions().size(), 0);
            CORRADE_COMPARE(renderer.glyphIds().size(), 0);
            if(data.flags >= RendererCoreFlag::GlyphClusters)
                CORRADE_COMPARE(renderer.glyphClusters().size(), 0);
            CORRADE_COMPARE(renderer.runCount(), 0);
            /* Not testing the "rendering" counts here as it's too laborous,
               only at the end */
            CORRADE_COMPARE(renderer.runScales().size(), 0);
            CORRADE_COMPARE(renderer.runEnds().size(), 0);
            CORRADE_COMPARE(renderer.cursor(), (Vector2{-50.0f, 100.0f}));
        }

        out = renderer.render();
    }

    /* At the end, it shouldn't be in progress anymore. The cursor should be
       still as set initially. */
    CORRADE_VERIFY(!renderer.isRendering());
    CORRADE_COMPARE(renderer.glyphCount(), 10);
    CORRADE_COMPARE(renderer.runCount(), data.expectedRuns.size());
    CORRADE_COMPARE(renderer.renderingGlyphCount(), 10);
    CORRADE_COMPARE(renderer.renderingRunCount(), data.expectedRuns.size());
    CORRADE_COMPARE(renderer.cursor(), (Vector2{-50.0f, 100.0f}));
    CORRADE_COMPARE_AS(renderer.glyphCapacity(), 10,
        TestSuite::Compare::GreaterOrEqual);
    CORRADE_COMPARE_AS(renderer.runCapacity(), data.expectedRuns.size(),
        TestSuite::Compare::GreaterOrEqual);

    /* The rendered output should have 2x as many glyphs as input bytes, should
       have the right baseline at the cursor in all cases and the rect height
       should be depending on the largest font size. */
    CORRADE_COMPARE(out, Containers::pair(
        Range2D::fromSize({-20.0f, -data.expectedRectHeight + 16.0f},
                          {20.0f, data.expectedRectHeight})
            .translated({-50.0f, 100.0f}),
        Range1Dui{0, UnsignedInt(data.expectedRuns.size())}));

    /* The contents should be the same independently of how many pieces was
       added. All glyph positions are shifted based on the cursor. */
    CORRADE_COMPARE_AS(renderer.glyphPositions(), Containers::arrayView<Vector2>({
        {-50.0f - 20.0f, 100.0f - 0.0f*data.expectedLineAdvance - 0.0f}, /* H */
        {-50.0f - 14.0f, 100.0f - 0.0f*data.expectedLineAdvance - 1.0f}, /* h */
        {-50.0f - 10.0f, 100.0f - 0.0f*data.expectedLineAdvance - 0.0f}, /* E */
        {-50.0f -  4.0f, 100.0f - 0.0f*data.expectedLineAdvance - 1.0f}, /* e */
        /* One newline here */
        {-50.0f - 12.0f, 100.0f - 1.0f*data.expectedLineAdvance - 0.0f}, /* L */
        {-50.0f -  9.0f, 100.0f - 1.0f*data.expectedLineAdvance - 0.0f}, /* l */
        {-50.0f -  6.0f, 100.0f - 1.0f*data.expectedLineAdvance - 0.0f}, /* L */
        {-50.0f -  3.0f, 100.0f - 1.0f*data.expectedLineAdvance - 0.0f}, /* l */
        /* Two newlines here */
        {-50.0f - 10.0f, 100.0f - 3.0f*data.expectedLineAdvance - 0.0f}, /* O */
        {-50.0f -  4.0f, 100.0f - 3.0f*data.expectedLineAdvance - 1.0f}, /* o */
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(renderer.glyphIds(),
        Containers::arrayView(data.expectedGlyphIds),
        TestSuite::Compare::Container);
    if(data.flags >= RendererCoreFlag::GlyphClusters) {
        if(data.direct)
            CORRADE_COMPARE_AS(renderer.glyphClusters(), Containers::arrayView<UnsignedInt>({
                /* 2, 5, 6 is a \n */
                0, 0, 1, 1, 3, 3, 4, 4, 7, 7
            }), TestSuite::Compare::Container);
        else
            CORRADE_COMPARE_AS(renderer.glyphClusters(), Containers::arrayView<UnsignedInt>({
                /* 5, 8, 9 is a \n */
                3, 3, 4, 4, 6, 6, 7, 7, 10, 10
            }), TestSuite::Compare::Container);
    }
    CORRADE_COMPARE_AS(renderer.runScales(),
        stridedArrayView(data.expectedRuns).slice(&Containers::Pair<Float, UnsignedInt>::first),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(renderer.runEnds(),
        stridedArrayView(data.expectedRuns).slice(&Containers::Pair<Float, UnsignedInt>::second),
        TestSuite::Compare::Container);
}

void RendererTest::addMultipleLinesAlign() {
    auto&& data = AddMultipleLinesAlignData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        Properties doOpenFile(Containers::StringView, Float size) override {
            _opened = true;
            /* Compared to the glyph bounds, which are from 0 to 2, this is
               shifted by one unit, thus by 0.5 in the output */
            return {size, 1.0f, -1.0f, 8.0f, 10};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>& glyphs) override {
            for(UnsignedInt& i: glyphs)
                i = 0;
        }
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }

        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool _opened = false;
    } font;
    font.openFile({}, 0.5f);

    struct: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(Containers::StringView, UnsignedInt begin, UnsignedInt end, Containers::ArrayView<const FeatureRange>) override {
            return end - begin;
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>& ids) const override {
            for(UnsignedInt i = 0; i != ids.size(); ++i)
                ids[i] = 0;
        }
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>& offsets, const Containers::StridedArrayView1D<Vector2>& advances) const override {
            for(UnsignedInt i = 0; i != offsets.size(); ++i) {
                offsets[i] = {};
                advances[i] = Vector2::xAxis(4.0f);
            }
        }
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}
    } shaper{font};

    /* Just a single glyph that scales to {1, 1} in the end. Default padding is
       1 which would prevent this, set it back to 0. */
    DummyGlyphCache glyphCache{PixelFormat::R8Unorm, {20, 20}, {}};
    UnsignedInt fontId = glyphCache.addFont(1, &font);
    glyphCache.addGlyph(fontId, 0, {}, {{}, {2, 2}});

    RendererCore renderer{glyphCache};
    renderer.setAlignment(data.alignment);

    /* We're rendering text at 0.25f size and the font is scaled to 0.5f, so
       the line advance should be 8.0f*0.25f/0.5f = 4.0f */
    CORRADE_COMPARE(font.size(), 0.5f);
    CORRADE_COMPARE(font.lineHeight(), 8.0f);

    /* Bounds are different depending on whether or not GlyphBounds alignment
       is used. The advance for the rightmost glyph is one unit larger than the
       actual bounds so it's different on X between the two variants */
    CORRADE_COMPARE(renderer.render(shaper, 0.25f, "abcd\nef\n\nghi"), Containers::pair(
        (UnsignedByte(data.alignment) & Implementation::AlignmentGlyphBounds ?
            Range2D{{0.0f, -12.0f}, {7.0f, 1.0f}} :
            Range2D{{0.0f, -12.5f}, {8.0f, 0.5f}}).translated(data.offset0),
        Range1Dui{0, 1}));

    /* Vertices
       [a] [b] [c] [d]
           [e] [f]

         [g] [h] [i]   */
    CORRADE_COMPARE_AS(renderer.glyphPositions(), Containers::arrayView<Vector2>({
        Vector2{0.0f, 0.0f} + data.offset0, /* a */
        Vector2{2.0f, 0.0f} + data.offset0, /* b */
        Vector2{4.0f, 0.0f} + data.offset0, /* c */
        Vector2{6.0f, 0.0f} + data.offset0, /* d */

        Vector2{0.0f, 0.0f} + data.offset1, /* e */
        Vector2{2.0f, 0.0f} + data.offset1, /* f */

        /* Two linebreaks here */

        Vector2{0.0f, 0.0f} + data.offset2, /* g */
        Vector2{2.0f, 0.0f} + data.offset2, /* h */
        Vector2{4.0f, 0.0f} + data.offset2, /* i */
    }), TestSuite::Compare::Container);
}

void RendererTest::addFontNotFoundInCache() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
    } glyphCache{PixelFormat::R8Unorm, {16, 16}};

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }
    } font1, font2, font3;
    glyphCache.addFont(0, &font1);
    /* font2 not */
    glyphCache.addFont(0, &font3);

    struct: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(Containers::StringView, UnsignedInt, UnsignedInt, Containers::ArrayView<const FeatureRange>) override {
            return 0;
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&) const override {}
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {}
    } shaper{font2};

    RendererCore renderer{glyphCache};

    Containers::String out;
    Error redirectError{&out};
    renderer.add(shaper, 0.0f, "hello");
    CORRADE_COMPARE(out, "Text::RendererCore::add(): shaper font not found among 2 fonts in associated glyph cache\n");
}

void RendererTest::multipleBlocks() {
    auto&& data = MultipleBlocksData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
    /* Set padding to zero for easier dummy glyph addition below */
    } glyphCache{PixelFormat::R8Unorm, {16, 16}, {}};

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        Properties doOpenFile(Containers::StringView, Float size) override {
            _opened = true;
            return {size, 2.0f*size, -1.0f*size, 4.0f*size, 0};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>& glyphs) override {
            for(UnsignedInt& i: glyphs)
                i = 0;
        }
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }

        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        bool _opened = false;
    } font1, font2;
    /* Two fonts that do the same but each is opened with a different size */
    font1.openFile("", 1.0f);
    font2.openFile("", 2.0f);
    for(AbstractFont* font: {&font1, &font2}) {
        UnsignedInt fontId = glyphCache.addFont('l' + 1, font);
        /* Shuffled order to not have their IDs match the clusters */
        glyphCache.addGlyph(fontId, 'a', {}, {}); /* 1  or 13 */
        glyphCache.addGlyph(fontId, 'c', {}, {}); /* 2  or 14 */
        glyphCache.addGlyph(fontId, 'e', {}, {}); /* 3  or 15 */
        glyphCache.addGlyph(fontId, 'j', {}, {}); /* 4  or 16 */
        glyphCache.addGlyph(fontId, 'b', {}, {}); /* 5  or 17 */
        glyphCache.addGlyph(fontId, 'f', {}, {}); /* 6  or 18 */
        glyphCache.addGlyph(fontId, 'd', {}, {}); /* 7  or 19 */
        glyphCache.addGlyph(fontId, 'g', {}, {}); /* 8  or 20 */
        glyphCache.addGlyph(fontId, 'h', {}, {}); /* 9  or 21 */
        glyphCache.addGlyph(fontId, 'k', {}, {}); /* 10 or 22 */
        glyphCache.addGlyph(fontId, 'i', {}, {}); /* 11 or 23 */
        glyphCache.addGlyph(fontId, 'l', {}, {}); /* 12 or 24 */
    }

    struct: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(Containers::StringView text, UnsignedInt begin, UnsignedInt end, Containers::ArrayView<const FeatureRange>) override {
            _text = text;
            _begin = begin;
            return end - begin;
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>& ids) const override {
            for(UnsignedInt i = 0; i != ids.size(); ++i)
                ids[i] = _text[_begin + i];
        }
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>& offsets, const Containers::StridedArrayView1D<Vector2>& advances) const override {
            for(UnsignedInt i = 0; i != offsets.size(); ++i) {
                offsets[i] = {};
                advances[i] = Vector2::xAxis(2.0f)*font().size();
            }
        }
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>& clusters) const override {
            for(UnsignedInt i = 0; i != clusters.size(); ++i)
                clusters[i] = _begin + i;
        }

        ShapeDirection doDirection() const override {
            return direction;
        }

        ShapeDirection direction;

        private:
            Containers::StringView _text;
            UnsignedInt _begin;
    } shaper1{font1}, shaper2{font2};

    RendererCore renderer{glyphCache, data.flags};

    /* Right alignment and custom line advance */
    shaper1.direction = ShapeDirection::RightToLeft;
    renderer
        .setCursor({50, 100})
        .setAlignment(Alignment::LineBegin)
        .setLineAdvance(30.0f);
    CORRADE_COMPARE(renderer.render(shaper1, 2.0f, "ab\nc"),
        Containers::pair(Range2D{{42.0f, 68.0f}, {50.0f, 104.0f}},
                         Range1Dui{0, 1}));
    CORRADE_COMPARE(renderer.glyphCount(), 3);
    CORRADE_COMPARE(renderer.glyphPositions().size(), 3);
    CORRADE_COMPARE(renderer.glyphIds().size(), 3);
    if(data.flags >= RendererCoreFlag::GlyphClusters)
        CORRADE_COMPARE(renderer.glyphClusters().size(), 3);
    CORRADE_COMPARE(renderer.runCount(), 1);
    CORRADE_COMPARE(renderer.renderingGlyphCount(), 3);
    CORRADE_COMPARE(renderer.renderingRunCount(), 1);
    CORRADE_COMPARE(renderer.runScales().size(), 1);
    CORRADE_COMPARE(renderer.runEnds().size(), 1);

    /* Left alignment and default line advance */
    shaper2.direction = ShapeDirection::RightToLeft;
    renderer
        .setCursor({-100, 50.0f})
        .setAlignment(Alignment::TopEnd)
        .setLineAdvance(0.0f)
        .add(shaper2, 4.0f, "de\nfgh\ni", 0, 3)
        .add(shaper2, 4.0f, "de\nfgh\ni", 3, 6)
        .add(shaper2, 4.0f, "de\nfgh\ni", 6, 8);
    CORRADE_COMPARE(renderer.renderingGlyphCount(), 9);
    CORRADE_COMPARE(renderer.renderingRunCount(), 4);
    CORRADE_COMPARE(renderer.render(),
        Containers::pair(Range2D{{-100.0f, 6.0f}, {-76.0f, 50.0f}},
                         Range1Dui{1, 4}));
    CORRADE_COMPARE(renderer.glyphCount(), 9);
    CORRADE_COMPARE(renderer.glyphPositions().size(), 9);
    CORRADE_COMPARE(renderer.glyphIds().size(), 9);
    if(data.flags >= RendererCoreFlag::GlyphClusters)
        CORRADE_COMPARE(renderer.glyphClusters().size(), 9);
    CORRADE_COMPARE(renderer.runCount(), 4);
    CORRADE_COMPARE(renderer.renderingGlyphCount(), 9);
    CORRADE_COMPARE(renderer.renderingRunCount(), 4);
    CORRADE_COMPARE(renderer.runScales().size(), 4);
    CORRADE_COMPARE(renderer.runEnds().size(), 4);

    /* Right alignment, custom line advance again */
    shaper1.direction = ShapeDirection::Unspecified;
    renderer
        .setCursor({0, -50.0f})
        .setAlignment(Alignment::BottomEnd)
        .setLineAdvance(10.0f);
    CORRADE_COMPARE(renderer.render(shaper1, 1.0f, "j\nkl"),
        Containers::pair(Range2D{{-4.0f, -50.0f}, {0.0f, -37.0f}},
                         Range1Dui{4, 5}));
    CORRADE_COMPARE(renderer.glyphCount(), 12);
    CORRADE_COMPARE(renderer.glyphPositions().size(), 12);
    CORRADE_COMPARE(renderer.glyphIds().size(), 12);
    if(data.flags >= RendererCoreFlag::GlyphClusters)
        CORRADE_COMPARE(renderer.glyphClusters().size(), 12);
    CORRADE_COMPARE(renderer.runCount(), 5);
    CORRADE_COMPARE(renderer.renderingGlyphCount(), 12);
    CORRADE_COMPARE(renderer.renderingRunCount(), 5);
    CORRADE_COMPARE(renderer.runScales().size(), 5);
    CORRADE_COMPARE(renderer.runEnds().size(), 5);

    /* Glyph data of previous blocks shouldn't get corrupted by new renders */
    CORRADE_COMPARE_AS(renderer.glyphPositions(), Containers::arrayView<Vector2>({
        {42.0f, 100.0f}, /* a */
        {46.0f, 100.0f}, /* b */
        {46.0f,  70.0f}, /* c */

        {-100.0f, 42.0f}, /* d */
        { -92.0f, 42.0f}, /* e */
        {-100.0f, 26.0f}, /* f */
        { -92.0f, 26.0f}, /* g */
        { -84.0f, 26.0f}, /* h */
        {-100.0f, 10.0f}, /* i */

        {-2.0f, -39.0f}, /* j */
        {-4.0f, -49.0f}, /* k */
        {-2.0f, -49.0f}, /* l */
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(renderer.glyphIds(), Containers::arrayView<UnsignedInt>({
     /* a  b  c  d   e   f   g   h   i   j  k   l
        first -  second ---------------  first -- */
        1, 5, 2, 19, 15, 18, 20, 21, 23, 4, 10, 12
    }), TestSuite::Compare::Container);
    if(data.flags & RendererCoreFlag::GlyphClusters)
        CORRADE_COMPARE_AS(renderer.glyphClusters(), Containers::arrayView<UnsignedInt>({
         /* a  b  c  d  e  f  g  h  i  j  k  l */
            0, 1, 3, 0, 1, 3, 4, 5, 7, 0, 2, 3
        }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(renderer.runScales(), Containers::arrayView({
        2.0f,
        2.0f,
        2.0f,
        2.0f,
        1.0f
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(renderer.runEnds(), Containers::arrayView({
        3u,
        5u,
        8u,
        9u,
        12u
    }), TestSuite::Compare::Container);
}

void RendererTest::clearResetCore() {
    auto&& data = ClearResetCoreData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
    } glyphCache{PixelFormat::R8Unorm, {16, 16}};

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        Properties doOpenFile(Containers::StringView, Float size) override {
            _opened = true;
            /* The size is used to scale advances. Ascent & descent is used
               just for vertical rect size which isn't needed as we can check
               just that the horizontal size got reset. Line height is used to
               test that line advance is correctly reset as well. Glyph count
               is overriden in addFont() below. */
            return {size, 0.0f, 0.0f, 2.0f, 0};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        private:
            bool _opened = false;
    } font;
    font.openFile("", 1.0f);
    glyphCache.addFont(1, &font);

    struct: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(Containers::StringView, UnsignedInt begin, UnsignedInt end, Containers::ArrayView<const FeatureRange>) override {
            return end - begin;
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>& ids) const override {
            /* Zero the IDs to not hit an OOB assert in the glyph cache */
            for(UnsignedInt& id: ids)
                id = 0;
        }
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>& offsets, const Containers::StridedArrayView1D<Vector2>& advances) const override {
            for(UnsignedInt i = 0; i != offsets.size(); ++i) {
                advances[i] = {1.0f, 0.0f};
                offsets[i] = {};
            }
        }
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {
            /* The data don't matter in this case */
        }
        ShapeDirection doDirection() const override {
            return direction;
        }

        ShapeDirection direction = ShapeDirection::Unspecified;
    } shaper{font};

    RendererCore renderer{glyphCache, data.flags};

    /* Clearing right from the initial state should be a no-op */
    if(data.reset)
        renderer.reset();
    else
        renderer.clear();
    CORRADE_COMPARE(renderer.glyphCapacity(), 0);
    CORRADE_COMPARE(renderer.glyphCount(), 0);
    CORRADE_COMPARE(renderer.runCapacity(), 0);
    CORRADE_COMPARE(renderer.runCount(), 0);
    CORRADE_VERIFY(!renderer.isRendering());
    CORRADE_COMPARE(renderer.renderingGlyphCount(), 0);
    CORRADE_COMPARE(renderer.renderingRunCount(), 0);

    shaper.direction = ShapeDirection::RightToLeft;
    renderer
        .setAlignment(Alignment::LineEnd)
        .setCursor({100.0f, 50.0f})
        .setLineAdvance(30.0f)
        .add(shaper, 1.0f, "ab\nc");
    if(data.renderAddOnly) {
        CORRADE_VERIFY(renderer.isRendering());
        CORRADE_COMPARE(renderer.glyphCount(), 0);
        CORRADE_COMPARE(renderer.runCount(), 0);
    } else {
        renderer.render();
        CORRADE_VERIFY(!renderer.isRendering());
        CORRADE_COMPARE(renderer.glyphCount(), 3);
        CORRADE_COMPARE(renderer.runCount(), 1);
        /* Verify initial glyph position values to be sure that the offset
           doesn't leak to after clear() */
        CORRADE_COMPARE_AS(renderer.glyphPositions(), Containers::arrayView<Vector2>({
            {100.0f, 50.0f},
            {101.0f, 50.0f},
            {100.0f, 20.0f}, /* On another line with custom advance */
        }), TestSuite::Compare::Container);
        /* Similarly, per-run glyph offset shouldn't leak to after clear() */
        CORRADE_COMPARE_AS(renderer.runEnds(), Containers::arrayView({
            3u
        }), TestSuite::Compare::Container);
    }
    CORRADE_COMPARE(renderer.glyphCapacity(), 3);
    CORRADE_COMPARE(renderer.runCapacity(), 1);
    CORRADE_COMPARE(renderer.renderingGlyphCount(), 3);
    CORRADE_COMPARE(renderer.renderingRunCount(), 1);

    /* Clearing should call the allocator with 0, which should then give back
       the existing capacity it has, and then reset all in-progress rendering
       state. */
    if(data.reset)
        renderer.reset();
    else
        renderer.clear();
    CORRADE_COMPARE(renderer.glyphCapacity(), data.expectedBuiltinGlyphAllocatorCapacity);
    CORRADE_COMPARE(renderer.glyphCount(), 0);
    CORRADE_COMPARE(renderer.runCapacity(), 1);
    CORRADE_COMPARE(renderer.runCount(), 0);
    CORRADE_VERIFY(!renderer.isRendering());
    CORRADE_COMPARE(renderer.renderingGlyphCount(), 0);
    CORRADE_COMPARE(renderer.renderingRunCount(), 0);

    /* Resetting goes back to the initial cursor, alignment and layout
       direction */
    if(data.reset) {
        CORRADE_COMPARE(renderer.cursor(), Vector2{});
        CORRADE_COMPARE(renderer.alignment(), Alignment::MiddleCenter);
        CORRADE_COMPARE(renderer.lineAdvance(), 0.0f);
        CORRADE_COMPARE(renderer.layoutDirection(), LayoutDirection::HorizontalTopToBottom);
    } else {
        CORRADE_COMPARE(renderer.cursor(), (Vector2{100.0f, 50.0f}));
        CORRADE_COMPARE(renderer.alignment(), Alignment::LineEnd);
        CORRADE_COMPARE(renderer.lineAdvance(), 30.0f);
        /** @todo verify with a different value once vertical layout direction
            is supported */
        CORRADE_COMPARE(renderer.layoutDirection(), LayoutDirection::HorizontalTopToBottom);
    }

    /* Clear the custom line advance if it wasn't reset, to not have it affect
       the next. The clear() should have internally cleared the detected one
       as well. */
    /** @todo eh, this could theoretically hide a bug where this actually does
        reset the detected one as well (and reset() does so as well), silently
        keeping the last detected if setLineAdvance() wasn't called at all ..
        but I'm being paranoid now */
    if(!data.reset)
        renderer.setLineAdvance(0.0f);

    /* Rendering again at a different cursor and alignment shouldn't have the
       previous cursor, previous rectangles, resolved alignment, line advance
       or run glyph offsets leaking in any way. The three glyphs should now be
       at -53, -52, -51 because it's a RTL text aligned to the right. */
    shaper.direction = ShapeDirection::RightToLeft;
    renderer
        .setAlignment(Alignment::LineBegin)
        .setCursor({-50.0f, 100.0f});
    CORRADE_COMPARE(renderer.render(shaper, 1.0f, "a\nbc"), Containers::pair(
        Range2D::fromSize({-52.0f, 98.0f}, {2.0f, 2.0f}), Range1Dui{0, 1}));
    CORRADE_COMPARE(renderer.glyphCapacity(), 3);
    CORRADE_COMPARE(renderer.glyphCount(), 3);
    CORRADE_COMPARE(renderer.runCapacity(), 1);
    CORRADE_COMPARE(renderer.runCount(), 1);
    CORRADE_VERIFY(!renderer.isRendering());
    CORRADE_COMPARE(renderer.renderingGlyphCount(), 3);
    CORRADE_COMPARE(renderer.renderingRunCount(), 1);
    CORRADE_COMPARE_AS(renderer.glyphPositions(), Containers::arrayView<Vector2>({
        {-51.0f, 100.0f},
        /* On a new line (advance is negative Y), advance is font's default
           {0, 6} */
        {-52.0f, 98.0f},
        {-51.0f, 98.0f},
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(renderer.runEnds(), Containers::arrayView({
        3u
    }), TestSuite::Compare::Container);
}

void RendererTest::clearResetCoreAllocators() {
    auto&& data = ClearResetCoreData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
    } glyphCache{PixelFormat::R8Unorm, {16, 16}};

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        Properties doOpenFile(Containers::StringView, Float size) override {
            _opened = true;
            /* The size is used to scale advances, ascent, descent and line
               height is used for vertical alignment which we don't need and
               can stay zero. Glyph count is overriden in addFont() below. */
            return {size, 0.0f, 0.0f, 0.0f, 0};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return {}; }

        private:
            bool _opened = false;
    } font;
    font.openFile("", 1.0f);
    glyphCache.addFont(1, &font);

    struct: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(Containers::StringView text, UnsignedInt, UnsignedInt, Containers::ArrayView<const FeatureRange>) override {
            return text.size();
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>& ids) const override {
            /* Zero the IDs to not hit an OOB assert in the glyph cache */
            for(UnsignedInt& id: ids)
                id = 0;
        }
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>& offsets, const Containers::StridedArrayView1D<Vector2>& advances) const override {
            for(UnsignedInt i = 0; i != offsets.size(); ++i) {
                advances[i] = {1.0f, 0.0f};
                offsets[i] = {};
            }
        }
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {
            /* The data don't matter in this case */
        }
    } shaper{font};

    struct Allocation {
        Vector2 glyphPositions[20];
        UnsignedInt glyphIds[18]; /* deliberately smaller */
        UnsignedInt glyphClusters[20];
        Vector2 glyphAdvances[20];

        Float runScales[4];
        UnsignedInt runEnds[3]; /* deliberately smaller */

        UnsignedInt expectedGlyphCount, expectedRunCount;
        int glyphCalled = 0, runCalled = 0;
    } allocation;

    /* Capture correct function name */
    CORRADE_VERIFY(true);

    RendererCore renderer{glyphCache, [](void* state, UnsignedInt glyphCount, Containers::StridedArrayView1D<Vector2>& glyphPositions, Containers::StridedArrayView1D<UnsignedInt>& glyphIds, Containers::StridedArrayView1D<UnsignedInt>* glyphClusters, Containers::StridedArrayView1D<Vector2>& glyphAdvances){
        Allocation& allocation = *static_cast<Allocation*>(state);
        CORRADE_COMPARE(glyphCount, allocation.expectedGlyphCount);
        CORRADE_COMPARE(glyphPositions.size(), 0);
        CORRADE_COMPARE(glyphIds.size(), 0);
        if(glyphClusters)
            CORRADE_COMPARE(glyphClusters->size(), 0);
        CORRADE_COMPARE(glyphAdvances.size(), 0);

        glyphPositions = allocation.glyphPositions;
        glyphIds = allocation.glyphIds;
        if(glyphClusters)
            *glyphClusters = allocation.glyphClusters;
        glyphAdvances = allocation.glyphAdvances;
        ++allocation.glyphCalled;
    }, &allocation, [](void* state, UnsignedInt runCount, Containers::StridedArrayView1D<Float>& runScales, Containers::StridedArrayView1D<UnsignedInt>& runEnds) {
        Allocation& allocation = *static_cast<Allocation*>(state);
        CORRADE_COMPARE(runCount, allocation.expectedRunCount);
        CORRADE_COMPARE(runScales.size(), 0);
        CORRADE_COMPARE(runEnds.size(), 0);

        runScales = allocation.runScales;
        runEnds = allocation.runEnds;
        ++allocation.runCalled;
    }, &allocation, data.flags};

    allocation.expectedGlyphCount = 3;
    allocation.expectedRunCount = 1;
    renderer.add(shaper, 1.0f, "abc");
    CORRADE_COMPARE(renderer.renderingGlyphCount(), 3);
    CORRADE_COMPARE(renderer.renderingRunCount(), 1);
    if(data.renderAddOnly) {
        CORRADE_VERIFY(renderer.isRendering());
        CORRADE_COMPARE(renderer.glyphCount(), 0);
        CORRADE_COMPARE(renderer.runCount(), 0);
    } else {
        renderer.render();
        CORRADE_VERIFY(!renderer.isRendering());
        CORRADE_COMPARE(renderer.glyphCount(), 3);
        CORRADE_COMPARE(renderer.runCount(), 1);
    }
    CORRADE_COMPARE(allocation.glyphCalled, 1);
    CORRADE_COMPARE(allocation.runCalled, 1);
    /* Minimum of all returned view sizes */
    CORRADE_COMPARE(renderer.glyphCapacity(), 18);
    CORRADE_COMPARE(renderer.runCapacity(), 3);
    CORRADE_COMPARE(renderer.renderingGlyphCount(), 3);
    CORRADE_COMPARE(renderer.renderingRunCount(), 1);

    /* Clearing should call the allocator with 0, and then calculate the
       capacity the same way as before. The capacity calculation was tested
       sufficiently in allocateAllocator() already, and as clear() uses the
       same helper internally, we just test a single case of one array being
       shorter. */
    allocation.expectedGlyphCount = 0;
    allocation.expectedRunCount = 0;
    if(data.reset)
        renderer.reset();
    else
        renderer.clear();
    CORRADE_COMPARE(allocation.glyphCalled, 2);
    CORRADE_COMPARE(allocation.runCalled, 2);
    CORRADE_COMPARE(renderer.glyphCount(), 0);
    CORRADE_COMPARE(renderer.runCount(), 0);
    /* Minimum of all returned view sizes */
    CORRADE_COMPARE(renderer.glyphCapacity(), 18);
    CORRADE_COMPARE(renderer.runCapacity(), 3);
    CORRADE_VERIFY(!renderer.isRendering());
    CORRADE_COMPARE(renderer.renderingGlyphCount(), 0);
    CORRADE_COMPARE(renderer.renderingRunCount(), 0);

    /* Clearing again should not result in anything different, but the
       allocators get called again */
    if(data.reset)
        renderer.reset();
    else
        renderer.clear();
    CORRADE_COMPARE(allocation.glyphCalled, 3);
    CORRADE_COMPARE(allocation.runCalled, 3);
    /* Minimum of all returned view sizes */
    CORRADE_COMPARE(renderer.glyphCapacity(), 18);
    CORRADE_COMPARE(renderer.runCapacity(), 3);

    /* Other resetting behavior is sufficiently tested by clearResetCore()
       already */
}

#ifdef MAGNUM_TARGET_GL
void RendererTest::renderData() {
    auto&& data = RenderDataData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    TestFont font;
    font.direction = data.shapeDirection;
    font.openFile({}, 0.5f);
    DummyGlyphCache cache = testGlyphCache(font);

    /* Capture the correct function name */
    CORRADE_VERIFY(true);

    std::vector<Vector2> positions;
    std::vector<Vector2> textureCoordinates;
    std::vector<UnsignedInt> indices;
    Range2D bounds;
    std::tie(positions, textureCoordinates, indices, bounds) = AbstractRenderer::render(font, cache, 0.25f, "abc", data.alignment);

    /* Three glyphs, three quads -> 12 vertices, 18 indices */
    CORRADE_COMPARE(positions.size(), 12);
    CORRADE_COMPARE(textureCoordinates.size(), 12);
    CORRADE_COMPARE(indices.size(), 18);

    /* Vertex positions. Rectangles coming from the cache and offsets +
       advances from the layouter are scaled by 0.5. First glyph is moved by
       (scaled) 1 up and has advance of (scaled) {1, ±0.5}, every next glyph is
       moved up and further distanced by (scaled) {1, ±0.5}. First glyph is
       wide, the other two are square.

                   +-+
              +-+  |c|
        2---3 |b|  +-+
        | a | +-+
        0---1          */
    CORRADE_COMPARE_AS(positions, (std::vector<Vector2>{
        /* Cursor is {0, 0}. Offset from the cache is {5, 10}, offset from the
           renderer is {0, 1}, size is {20, 10}; all scaled by 0.5 */
        Vector2{ 2.5f,  5.5f} + data.offset,
        Vector2{12.5f,  5.5f} + data.offset,
        Vector2{ 2.5f, 10.5f} + data.offset,
        Vector2{12.5f, 10.5f} + data.offset,

        /* Advance was {1, 0.5}, cursor is {1, 0.5}. Offset from the cache is
           {10, 5}, offset from the renderer is {0, 2}, size is {10, 10}; all
           scaled by 0.5 */
        Vector2{ 5.5f, 3.75f} + data.offset,
        Vector2{10.5f, 3.75f} + data.offset,
        Vector2{ 5.5f, 8.75f} + data.offset,
        Vector2{10.5f, 8.75f} + data.offset,

        /* Advance was {2, -0.5}, cursor is {3, 0}. Offset from the cache is
           {5, 5}, offset from the renderer is {0, 3}, size is {10, 10}; all
           scaled by 0.5 */
        Vector2{ 4.0f,  4.0f} + data.offset,
        Vector2{ 9.0f,  4.0f} + data.offset,
        Vector2{ 4.0f,  9.0f} + data.offset,
        Vector2{ 9.0f,  9.0f} + data.offset,
    }), TestSuite::Compare::Container);

    /* Bounds. Different depending on whether or not GlyphBounds alignment is
       used. */
    if(UnsignedByte(data.alignment) & Implementation::AlignmentGlyphBounds)
        CORRADE_COMPARE(bounds, (Range2D{{2.5f, 3.75f}, {12.5f, 10.5f}}.translated(data.offset)));
    else
        CORRADE_COMPARE(bounds, (Range2D{{0.0f, -1.25f}, {3.0f, 2.25f}}.translated(data.offset)));

    /* Texture coordinates. First glyph is bottom, second top left, third top
       right.
       +-+-+
       |b|c|
       2---3
       | a |
       0---1 */
    CORRADE_COMPARE_AS(textureCoordinates, (std::vector<Vector2>{
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {0.0f, 0.5f},
        {1.0f, 0.5f},

        {0.0f, 0.5f},
        {0.5f, 0.5f},
        {0.0f, 1.0f},
        {0.5f, 1.0f},

        {0.5f, 0.5f},
        {1.0f, 0.5f},
        {0.5f, 1.0f},
        {1.0f, 1.0f},
    }), TestSuite::Compare::Container);

    /* Indices
       2---3 2 3---5
       |   | |\ \  |
       |   | | \ \ |
       |   | |  \ \|
       0---1 0---1 4 */
    CORRADE_COMPARE_AS(indices, (std::vector<UnsignedInt>{
        0,  1,  2,  2,  1,  3,
        4,  5,  6,  6,  5,  7,
        8,  9, 10, 10,  9, 11,
    }), TestSuite::Compare::Container);
}

void RendererTest::multiline() {
    auto&& data = MultilineData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct Shaper: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(Containers::StringView text, UnsignedInt, UnsignedInt, Containers::ArrayView<const FeatureRange>) override {
            return text.size();
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>& ids) const override {
            for(UnsignedInt i = 0; i != ids.size(); ++i) {
                ids[i] = 0;
            }
        }
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>& offsets, const Containers::StridedArrayView1D<Vector2>& advances) const override {
            for(UnsignedInt i = 0; i != offsets.size(); ++i) {
                offsets[i] = {};
                advances[i] = Vector2::xAxis(4.0f);
            }
        }
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>&) const override {
            /* Nothing in the renderer uses this API */
            CORRADE_FAIL("This shouldn't be called.");
        }
    };

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        Properties doOpenFile(Containers::StringView, Float size) override {
            _opened = true;
            /* Compared to the glyph bounds, which are from 0 to 2, this is
               shifted by one unit, thus by 0.5 in the output */
            return {size, 1.0f, -1.0f, 8.0f, 10};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>& glyphs) override {
            for(UnsignedInt& i: glyphs)
                i = 0;
        }
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }

        Containers::Pointer<AbstractShaper> doCreateShaper() override {
            return Containers::pointer<Shaper>(*this);
        }

        bool _opened = false;
    } font;
    font.openFile({}, 0.5f);

    /* Just a single glyph that scales to {1, 1} in the end. Default padding is
       1 which would prevent this, set it back to 0. */
    DummyGlyphCache cache{PixelFormat::R8Unorm, {20, 20}, {}};
    UnsignedInt fontId = cache.addFont(1, &font);
    cache.addGlyph(fontId, 0, {}, {{}, {2, 2}});

    /* Capture the correct function name */
    CORRADE_VERIFY(true);

    Range2D rectangle;
    std::vector<UnsignedInt> indices;
    std::vector<Vector2> positions, textureCoordinates;
    std::tie(positions, textureCoordinates, indices, rectangle) = Renderer2D::render(font,
        cache, 0.25f, "abcd\nef\n\nghi", data.alignment);

    /* We're rendering text at 0.25f size and the font is scaled to 0.5f, so
       the line advance should be 8.0f*0.25f/0.5f = 4.0f */
    CORRADE_COMPARE(font.size(), 0.5f);
    CORRADE_COMPARE(font.lineHeight(), 8.0f);

    /* Bounds. The advance for the rightmost glyph is one unit larger than the
       actual bounds so it's different on X between the two variants */
    if(UnsignedByte(data.alignment) & Implementation::AlignmentGlyphBounds)
        CORRADE_COMPARE(rectangle, Range2D({0.0f, -12.0f}, {7.0f, 1.0f}).translated(data.offset0));
    else
        CORRADE_COMPARE(rectangle, Range2D({0.0f, -12.5f}, {8.0f, 0.5f}).translated(data.offset0));

    /* Vertices
       [a] [b] [c] [d]
           [e] [f]

         [g] [h] [i]   */
    CORRADE_COMPARE_AS(positions, (std::vector<Vector2>{
        Vector2{0.0f, 0.0f} + data.offset0, /* a */
        Vector2{1.0f, 0.0f} + data.offset0,
        Vector2{0.0f, 1.0f} + data.offset0,
        Vector2{1.0f, 1.0f} + data.offset0,

        Vector2{2.0f, 0.0f} + data.offset0, /* b */
        Vector2{3.0f, 0.0f} + data.offset0,
        Vector2{2.0f, 1.0f} + data.offset0,
        Vector2{3.0f, 1.0f} + data.offset0,

        Vector2{4.0f, 0.0f} + data.offset0, /* c */
        Vector2{5.0f, 0.0f} + data.offset0,
        Vector2{4.0f, 1.0f} + data.offset0,
        Vector2{5.0f, 1.0f} + data.offset0,

        Vector2{6.0f, 0.0f} + data.offset0, /* d */
        Vector2{7.0f, 0.0f} + data.offset0,
        Vector2{6.0f, 1.0f} + data.offset0,
        Vector2{7.0f, 1.0f} + data.offset0,

        Vector2{0.0f, 0.0f} + data.offset1, /* e */
        Vector2{1.0f, 0.0f} + data.offset1,
        Vector2{0.0f, 1.0f} + data.offset1,
        Vector2{1.0f, 1.0f} + data.offset1,

        Vector2{2.0f, 0.0f} + data.offset1, /* f */
        Vector2{3.0f, 0.0f} + data.offset1,
        Vector2{2.0f, 1.0f} + data.offset1,
        Vector2{3.0f, 1.0f} + data.offset1,

        /* Two linebreaks here */

        Vector2{0.0f, 0.0f} + data.offset2, /* g */
        Vector2{1.0f, 0.0f} + data.offset2,
        Vector2{0.0f, 1.0f} + data.offset2,
        Vector2{1.0f, 1.0f} + data.offset2,

        Vector2{2.0f, 0.0f} + data.offset2, /* h */
        Vector2{3.0f, 0.0f} + data.offset2,
        Vector2{2.0f, 1.0f} + data.offset2,
        Vector2{3.0f, 1.0f} + data.offset2,

        Vector2{4.0f, 0.0f} + data.offset2, /* i */
        Vector2{5.0f, 0.0f} + data.offset2,
        Vector2{4.0f, 1.0f} + data.offset2,
        Vector2{5.0f, 1.0f} + data.offset2,
    }), TestSuite::Compare::Container);

    /* Indices
       2---3 2 3---5
       |   | |\ \  |
       |   | | \ \ |
       |   | |  \ \|
       0---1 0---1 4 */
    CORRADE_COMPARE_AS(indices, (std::vector<UnsignedInt>{
         0,  1,  2,  2,  1,  3,
         4,  5,  6,  6,  5,  7,
         8,  9, 10, 10,  9, 11,
        12, 13, 14, 14, 13, 15,
        16, 17, 18, 18, 17, 19,
        20, 21, 22, 22, 21, 23,
        24, 25, 26, 26, 25, 27,
        28, 29, 30, 30, 29, 31,
        32, 33, 34, 34, 33, 35,
    }), TestSuite::Compare::Container);
}

void RendererTest::arrayGlyphCache() {
    CORRADE_SKIP_IF_NO_ASSERT();

    TestFont font;
    font.openFile({}, 0.5f);
    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
    } cache{PixelFormat::R8Unorm, {100, 100, 3}};

    Containers::String out;
    Error redirectError{&out};
    AbstractRenderer::render(font, cache, 0.25f, "abc");
    CORRADE_COMPARE(out, "Text::Renderer: array glyph caches are not supported\n");
}

void RendererTest::fontNotFoundInCache() {
    CORRADE_SKIP_IF_NO_ASSERT();

    TestFont font;
    font.openFile({}, 0.5f);
    DummyGlyphCache cache{PixelFormat::R8Unorm, {100, 100}};

    cache.addFont(34);
    cache.addFont(25);

    Containers::String out;
    Error redirectError{&out};
    AbstractRenderer::render(font, cache, 0.25f, "abc");
    CORRADE_COMPARE(out, "Text::Renderer: font not found among 2 fonts in passed glyph cache\n");
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::RendererTest)
