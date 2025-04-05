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

#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/Triple.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/Math/Range.h"
#include "Magnum/Text/AbstractGlyphCache.h"
#include "Magnum/TextureTools/Atlas.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <vector>
#include <Corrade/Containers/ArrayViewStl.h>
#include <Corrade/Utility/DebugStl.h> /* std::pair */
#endif

namespace Magnum { namespace Text { namespace Test { namespace {

struct AbstractGlyphCacheTest: TestSuite::Tester {
    explicit AbstractGlyphCacheTest();

    void debugFeature();
    void debugFeatures();
    void debugFeaturesSupersets();

    void construct();
    void constructNoPadding();
    void construct2D();
    void construct2DNoPadding();
    void constructProcessed();
    void constructProcessedNoPadding();
    void constructProcessed2D();
    void constructProcessed2DNoPadding();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void constructDeprecated();
    void constructDeprecatedNoPadding();
    #endif
    void constructImageRowPadding();
    void constructZeroSize();
    void constructNoCreate();

    void constructCopy();
    void constructMove();

    void features();

    #ifdef MAGNUM_BUILD_DEPRECATED
    void textureSizeNot2D();
    #endif

    void setInvalidGlyph();
    void setInvalidGlyph2D();
    void setInvalidGlyphOutOfRange();
    void setInvalidGlyphOutOfRangePadded();
    void setInvalidGlyph2DNot2D();

    void addFont();
    void addFontDuplicatePointer();
    void fontOutOfRange();

    #ifdef MAGNUM_BUILD_DEPRECATED
    void reserve();
    void reserveIncremental();
    void reserveTooSmall();
    void reserveNot2D();
    #endif

    void addGlyph();
    void addGlyph2D();
    void addGlyphIndexOutOfRange();
    void addGlyphAlreadyAdded();
    void addGlyphOutOfRange();
    void addGlyphOutOfRangePadded();
    void addGlyphTooMany();
    void addGlyph2DNot2D();

    #ifdef MAGNUM_BUILD_DEPRECATED
    void insert();
    void insertNot2D();
    void insertMultiFont();
    #endif

    void flushImage();
    void flushImageWholeArea();
    void flushImageLayer();
    void flushImage2D();
    void flushImage2DPassthrough2D();
    void flushImageNotImplemented();
    void flushImagePassthrough2DNotImplemented();
    void flushImageOutOfRange();
    void flushImage2DNot2D();

    #ifdef MAGNUM_BUILD_DEPRECATED
    void setImage();
    void setImageOutOfRange();
    void setImageInvalidFormat();
    void setImageNot2D();
    #endif

    void processedImage();
    void processedImageNotSupported();
    void processedImageNotImplemented();

    void setProcessedImage();
    void setProcessedImage2D();
    void setProcessedImage2DPassthrough2D();
    void setProcessedImageNotImplemented();
    void setProcessedImagePassthrough2DNotImplemented();
    void setProcessedImageOutOfRange();
    void setProcessedImageInvalidFormat();
    void setProcessedImage2DNot2D();

    void access();
    void accessBatch();
    void accessInvalid();
    void accessBatchInvalid();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void accessDeprecated();
    void accessDeprecatedNot2D();
    #endif
};

const struct {
    const char* name;
    Vector2i padding;
    bool differentProcessedFormatSize;
} FlushImageData[]{
    {"", {}, false},
    {"with padding", {2, 3}, false},
    {"with different processed format and size", {}, false}
};

const struct {
    const char* name;
    GlyphCacheFeatures features;
} ProcessedImageNotSupportedData[]{
    {"no processing", {}},
    {"no processed image download", GlyphCacheFeature::ImageProcessing},
};

const struct {
    const char* name;
    Vector2i padding;
} SetProcessedImageOutOfRangeData[]{
    {"", {}},
    {"with padding", {2, 3}},
};

AbstractGlyphCacheTest::AbstractGlyphCacheTest() {
    addTests({&AbstractGlyphCacheTest::debugFeature,
              &AbstractGlyphCacheTest::debugFeatures,
              &AbstractGlyphCacheTest::debugFeaturesSupersets,

              &AbstractGlyphCacheTest::construct,
              &AbstractGlyphCacheTest::constructNoPadding,
              &AbstractGlyphCacheTest::construct2D,
              &AbstractGlyphCacheTest::construct2DNoPadding,
              &AbstractGlyphCacheTest::constructProcessed,
              &AbstractGlyphCacheTest::constructProcessedNoPadding,
              &AbstractGlyphCacheTest::constructProcessed2D,
              &AbstractGlyphCacheTest::constructProcessed2DNoPadding,
              #ifdef MAGNUM_BUILD_DEPRECATED
              &AbstractGlyphCacheTest::constructDeprecated,
              &AbstractGlyphCacheTest::constructDeprecatedNoPadding,
              #endif
              &AbstractGlyphCacheTest::constructImageRowPadding,
              &AbstractGlyphCacheTest::constructZeroSize,
              &AbstractGlyphCacheTest::constructNoCreate,

              &AbstractGlyphCacheTest::constructCopy,
              &AbstractGlyphCacheTest::constructMove,

              &AbstractGlyphCacheTest::features,

              #ifdef MAGNUM_BUILD_DEPRECATED
              &AbstractGlyphCacheTest::textureSizeNot2D,
              #endif

              &AbstractGlyphCacheTest::setInvalidGlyph,
              &AbstractGlyphCacheTest::setInvalidGlyph2D,
              &AbstractGlyphCacheTest::setInvalidGlyphOutOfRange,
              &AbstractGlyphCacheTest::setInvalidGlyphOutOfRangePadded,
              &AbstractGlyphCacheTest::setInvalidGlyph2DNot2D,

              &AbstractGlyphCacheTest::addFont,
              &AbstractGlyphCacheTest::addFontDuplicatePointer,
              &AbstractGlyphCacheTest::fontOutOfRange,

              #ifdef MAGNUM_BUILD_DEPRECATED
              &AbstractGlyphCacheTest::reserve,
              &AbstractGlyphCacheTest::reserveIncremental,
              &AbstractGlyphCacheTest::reserveTooSmall,
              &AbstractGlyphCacheTest::reserveNot2D,
              #endif

              &AbstractGlyphCacheTest::addGlyph,
              &AbstractGlyphCacheTest::addGlyph2D,
              &AbstractGlyphCacheTest::addGlyphIndexOutOfRange,
              &AbstractGlyphCacheTest::addGlyphAlreadyAdded,
              &AbstractGlyphCacheTest::addGlyphOutOfRange,
              &AbstractGlyphCacheTest::addGlyphOutOfRangePadded,
              &AbstractGlyphCacheTest::addGlyphTooMany,
              &AbstractGlyphCacheTest::addGlyph2DNot2D,

              #ifdef MAGNUM_BUILD_DEPRECATED
              &AbstractGlyphCacheTest::insert,
              &AbstractGlyphCacheTest::insertNot2D,
              &AbstractGlyphCacheTest::insertMultiFont,
              #endif
              });

    addInstancedTests({&AbstractGlyphCacheTest::flushImage,
                       &AbstractGlyphCacheTest::flushImageWholeArea,
                       &AbstractGlyphCacheTest::flushImageLayer,
                       &AbstractGlyphCacheTest::flushImage2D,
                       &AbstractGlyphCacheTest::flushImage2DPassthrough2D},
        Containers::arraySize(FlushImageData));

    addTests({&AbstractGlyphCacheTest::flushImageNotImplemented,
              &AbstractGlyphCacheTest::flushImagePassthrough2DNotImplemented});

    addInstancedTests({&AbstractGlyphCacheTest::flushImageOutOfRange},
        Containers::arraySize(FlushImageData));

    addTests({&AbstractGlyphCacheTest::flushImage2DNot2D});

    #ifdef MAGNUM_BUILD_DEPRECATED
    addInstancedTests({&AbstractGlyphCacheTest::setImage},
        Containers::arraySize(FlushImageData));

    addTests({&AbstractGlyphCacheTest::setImageOutOfRange,
              &AbstractGlyphCacheTest::setImageInvalidFormat,
              &AbstractGlyphCacheTest::setImageNot2D});
    #endif

    addTests({&AbstractGlyphCacheTest::processedImage});

    addInstancedTests({&AbstractGlyphCacheTest::processedImageNotSupported},
        Containers::arraySize(ProcessedImageNotSupportedData));

    addTests({&AbstractGlyphCacheTest::processedImageNotImplemented,

              &AbstractGlyphCacheTest::setProcessedImage,
              &AbstractGlyphCacheTest::setProcessedImage2D,
              &AbstractGlyphCacheTest::setProcessedImage2DPassthrough2D,
              &AbstractGlyphCacheTest::setProcessedImageNotImplemented,
              &AbstractGlyphCacheTest::setProcessedImagePassthrough2DNotImplemented});

    addInstancedTests({&AbstractGlyphCacheTest::setProcessedImageOutOfRange},
        Containers::arraySize(SetProcessedImageOutOfRangeData));

    addTests({&AbstractGlyphCacheTest::setProcessedImageInvalidFormat,
              &AbstractGlyphCacheTest::setProcessedImage2DNot2D,

              &AbstractGlyphCacheTest::access,
              &AbstractGlyphCacheTest::accessBatch,
              &AbstractGlyphCacheTest::accessInvalid,
              &AbstractGlyphCacheTest::accessBatchInvalid,

              #ifdef MAGNUM_BUILD_DEPRECATED
              &AbstractGlyphCacheTest::accessDeprecated,
              &AbstractGlyphCacheTest::accessDeprecatedNot2D,
              #endif
              });
}

void AbstractGlyphCacheTest::debugFeature() {
    Containers::String out;
    Debug{&out} << GlyphCacheFeature::ImageProcessing << GlyphCacheFeature(0xca);
    CORRADE_COMPARE(out, "Text::GlyphCacheFeature::ImageProcessing Text::GlyphCacheFeature(0xca)\n");
}

void AbstractGlyphCacheTest::debugFeatures() {
    Containers::String out;
    Debug{&out} << (GlyphCacheFeature::ImageProcessing|GlyphCacheFeature(0xf0)) << GlyphCacheFeatures{};
    CORRADE_COMPARE(out, "Text::GlyphCacheFeature::ImageProcessing|Text::GlyphCacheFeature(0xf0) Text::GlyphCacheFeatures{}\n");
}

void AbstractGlyphCacheTest::debugFeaturesSupersets() {
    /* ProcessedImageDownload is a superset of ImageProcessing, only one should
       be printed */
    Containers::String out;
    Debug{&out} << (GlyphCacheFeature::ImageProcessing|GlyphCacheFeature::ProcessedImageDownload);
    CORRADE_COMPARE(out, "Text::GlyphCacheFeature::ProcessedImageDownload\n");
}

struct DummyGlyphCache: AbstractGlyphCache {
    using AbstractGlyphCache::AbstractGlyphCache;

    GlyphCacheFeatures doFeatures() const override { return {}; }
    void doSetImage(const Vector2i&, const ImageView2D&) override {}
};

struct DummyProcessingGlyphCache: AbstractGlyphCache {
    using AbstractGlyphCache::AbstractGlyphCache;

    GlyphCacheFeatures doFeatures() const override {
        return GlyphCacheFeature::ImageProcessing;
    }
    void doSetImage(const Vector2i&, const ImageView2D&) override {}
};

void AbstractGlyphCacheTest::construct() {
    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512, 3}, {2, 5}};
    CORRADE_COMPARE(cache.format(), PixelFormat::R32F);
    CORRADE_COMPARE(cache.size(), (Vector3i{1024, 512, 3}));
    CORRADE_COMPARE(cache.processedFormat(), PixelFormat::R32F);
    CORRADE_COMPARE(cache.processedSize(), (Vector3i{1024, 512, 3}));
    CORRADE_COMPARE(cache.padding(), (Vector2i{2, 5}));
    CORRADE_COMPARE(cache.fontCount(), 0);
    CORRADE_COMPARE(cache.glyphCount(), 1);
    CORRADE_COMPARE(cache.atlas().size(), (Vector3i{1024, 512, 3}));
    CORRADE_COMPARE(cache.atlas().filledSize(), (Vector3i{1024, 512, 0}));
    CORRADE_COMPARE(cache.atlas().flags(), TextureTools::AtlasLandfillFlag::WidestFirst);
    CORRADE_COMPARE(cache.atlas().padding(), (Vector2i{2, 5}));
    CORRADE_COMPARE(cache.image().format(), PixelFormat::R32F);
    CORRADE_COMPARE(cache.image().size(), (Vector3i{1024, 512, 3}));

    /* Invalid glyph is always present */
    CORRADE_COMPARE(cache.glyph(0), Containers::triple(Vector2i{}, 0, Range2Di{}));
    CORRADE_COMPARE_AS(cache.glyphOffsets(), Containers::arrayView({
        Vector2i{}
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(cache.glyphLayers(), Containers::arrayView({
        0
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(cache.glyphRectangles(), Containers::arrayView({
        Range2Di{}
    }), TestSuite::Compare::Container);

    /* Const overloads */
    const DummyGlyphCache& ccache = cache;
    CORRADE_COMPARE(ccache.atlas().size(), (Vector3i{1024, 512, 3}));
    CORRADE_COMPARE(ccache.atlas().filledSize(), (Vector3i{1024, 512, 0}));
    CORRADE_COMPARE(ccache.atlas().flags(), TextureTools::AtlasLandfillFlag::WidestFirst);
    CORRADE_COMPARE(ccache.atlas().padding(), (Vector2i{2, 5}));
    CORRADE_COMPARE(ccache.image().format(), PixelFormat::R32F);
    CORRADE_COMPARE(ccache.image().size(), (Vector3i{1024, 512, 3}));
}

void AbstractGlyphCacheTest::constructNoPadding() {
    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512, 3}};
    CORRADE_COMPARE(cache.format(), PixelFormat::R32F);
    CORRADE_COMPARE(cache.size(), (Vector3i{1024, 512, 3}));
    CORRADE_COMPARE(cache.processedFormat(), PixelFormat::R32F);
    CORRADE_COMPARE(cache.processedSize(), (Vector3i{1024, 512, 3}));
    /* 1 by default to avoid artifacts */
    CORRADE_COMPARE(cache.padding(), Vector2i{1});
    CORRADE_COMPARE(cache.fontCount(), 0);
    /* Invalid glyph is always present */
    CORRADE_COMPARE(cache.glyphCount(), 1);
    CORRADE_COMPARE(cache.glyph(0), Containers::triple(Vector2i{}, 0, Range2Di{}));
    CORRADE_COMPARE(cache.atlas().size(), (Vector3i{1024, 512, 3}));
    CORRADE_COMPARE(cache.atlas().filledSize(), (Vector3i{1024, 512, 0}));
    CORRADE_COMPARE(cache.atlas().flags(), TextureTools::AtlasLandfillFlag::WidestFirst);
    CORRADE_COMPARE(cache.atlas().padding(), Vector2i{1});
    CORRADE_COMPARE(cache.image().format(), PixelFormat::R32F);
    CORRADE_COMPARE(cache.image().size(), (Vector3i{1024, 512, 3}));

    /* Invalid glyph is always present, has zero size in this case as well */
    CORRADE_COMPARE(cache.glyph(0), Containers::triple(Vector2i{}, 0, Range2Di{}));
    CORRADE_COMPARE_AS(cache.glyphOffsets(), Containers::arrayView({
        Vector2i{}
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(cache.glyphLayers(), Containers::arrayView({
        0
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(cache.glyphRectangles(), Containers::arrayView({
        Range2Di{}
    }), TestSuite::Compare::Container);

    /* The rest shouldn't be any different */
}

void AbstractGlyphCacheTest::construct2D() {
    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512}, {2, 5}};
    CORRADE_COMPARE(cache.format(), PixelFormat::R32F);
    CORRADE_COMPARE(cache.size(), (Vector3i{1024, 512, 1}));
    CORRADE_COMPARE(cache.processedFormat(), PixelFormat::R32F);
    CORRADE_COMPARE(cache.processedSize(), (Vector3i{1024, 512, 1}));
    CORRADE_COMPARE(cache.padding(), (Vector2i{2, 5}));
    CORRADE_COMPARE(cache.fontCount(), 0);
    /* Invalid glyph is always present */
    CORRADE_COMPARE(cache.glyphCount(), 1);
    CORRADE_COMPARE(cache.atlas().size(), (Vector3i{1024, 512, 1}));
    CORRADE_COMPARE(cache.atlas().filledSize(), (Vector3i{1024, 0, 1}));
    CORRADE_COMPARE(cache.atlas().flags(), TextureTools::AtlasLandfillFlag::WidestFirst);
    CORRADE_COMPARE(cache.atlas().padding(), (Vector2i{2, 5}));

    /* The rest shouldn't be any different */
}

void AbstractGlyphCacheTest::construct2DNoPadding() {
    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512}};
    CORRADE_COMPARE(cache.format(), PixelFormat::R32F);
    CORRADE_COMPARE(cache.size(), (Vector3i{1024, 512, 1}));
    CORRADE_COMPARE(cache.processedFormat(), PixelFormat::R32F);
    CORRADE_COMPARE(cache.processedSize(), (Vector3i{1024, 512, 1}));
    /* 1 by default to avoid artifacts */
    CORRADE_COMPARE(cache.padding(), Vector2i{1});
    CORRADE_COMPARE(cache.fontCount(), 0);
    /* Invalid glyph is always present */
    CORRADE_COMPARE(cache.glyphCount(), 1);
    CORRADE_COMPARE(cache.atlas().size(), (Vector3i{1024, 512, 1}));
    CORRADE_COMPARE(cache.atlas().filledSize(), (Vector3i{1024, 0, 1}));
    CORRADE_COMPARE(cache.atlas().flags(), TextureTools::AtlasLandfillFlag::WidestFirst);
    CORRADE_COMPARE(cache.atlas().padding(), Vector2i{1});

    /* The rest shouldn't be any different */
}

void AbstractGlyphCacheTest::constructProcessed() {
    DummyProcessingGlyphCache cache{PixelFormat::R32F, {1024, 512, 3}, PixelFormat::R16F, {256, 128}, {2, 5}};
    CORRADE_COMPARE(cache.format(), PixelFormat::R32F);
    CORRADE_COMPARE(cache.size(), (Vector3i{1024, 512, 3}));
    CORRADE_COMPARE(cache.processedFormat(), PixelFormat::R16F);
    CORRADE_COMPARE(cache.processedSize(), (Vector3i{256, 128, 3}));
    CORRADE_COMPARE(cache.padding(), (Vector2i{2, 5}));
    CORRADE_COMPARE(cache.fontCount(), 0);
    /* Invalid glyph is always present */
    CORRADE_COMPARE(cache.glyphCount(), 1);
    CORRADE_COMPARE(cache.atlas().size(), (Vector3i{1024, 512, 3}));
    CORRADE_COMPARE(cache.atlas().filledSize(), (Vector3i{1024, 512, 0}));
    CORRADE_COMPARE(cache.atlas().flags(), TextureTools::AtlasLandfillFlag::WidestFirst);
    CORRADE_COMPARE(cache.atlas().padding(), (Vector2i{2, 5}));

    /* The rest shouldn't be any different */
}

void AbstractGlyphCacheTest::constructProcessedNoPadding() {
    DummyProcessingGlyphCache cache{PixelFormat::R32F, {1024, 512, 3}, PixelFormat::R16F, {256, 128}};
    CORRADE_COMPARE(cache.format(), PixelFormat::R32F);
    CORRADE_COMPARE(cache.size(), (Vector3i{1024, 512, 3}));
    CORRADE_COMPARE(cache.processedFormat(), PixelFormat::R16F);
    CORRADE_COMPARE(cache.processedSize(), (Vector3i{256, 128, 3}));
    /* 1 by default to avoid artifacts */
    CORRADE_COMPARE(cache.padding(), Vector2i{1});
    CORRADE_COMPARE(cache.fontCount(), 0);
    /* Invalid glyph is always present */
    CORRADE_COMPARE(cache.glyphCount(), 1);
    CORRADE_COMPARE(cache.atlas().size(), (Vector3i{1024, 512, 3}));
    CORRADE_COMPARE(cache.atlas().filledSize(), (Vector3i{1024, 512, 0}));
    CORRADE_COMPARE(cache.atlas().flags(), TextureTools::AtlasLandfillFlag::WidestFirst);
    CORRADE_COMPARE(cache.atlas().padding(), Vector2i{1});

    /* The rest shouldn't be any different */
}

void AbstractGlyphCacheTest::constructProcessed2D() {
    DummyProcessingGlyphCache cache{PixelFormat::R32F, {1024, 512}, PixelFormat::R16F, {256, 128}, {2, 5}};
    CORRADE_COMPARE(cache.format(), PixelFormat::R32F);
    CORRADE_COMPARE(cache.size(), (Vector3i{1024, 512, 1}));
    CORRADE_COMPARE(cache.processedFormat(), PixelFormat::R16F);
    CORRADE_COMPARE(cache.processedSize(), (Vector3i{256, 128, 1}));
    CORRADE_COMPARE(cache.padding(), (Vector2i{2, 5}));
    CORRADE_COMPARE(cache.fontCount(), 0);
    /* Invalid glyph is always present */
    CORRADE_COMPARE(cache.glyphCount(), 1);
    CORRADE_COMPARE(cache.atlas().size(), (Vector3i{1024, 512, 1}));
    CORRADE_COMPARE(cache.atlas().filledSize(), (Vector3i{1024, 0, 1}));
    CORRADE_COMPARE(cache.atlas().flags(), TextureTools::AtlasLandfillFlag::WidestFirst);
    CORRADE_COMPARE(cache.atlas().padding(), (Vector2i{2, 5}));

    /* The rest shouldn't be any different */
}

void AbstractGlyphCacheTest::constructProcessed2DNoPadding() {
    DummyProcessingGlyphCache cache{PixelFormat::R32F, {1024, 512}, PixelFormat::R16F, {256, 128}};
    CORRADE_COMPARE(cache.format(), PixelFormat::R32F);
    CORRADE_COMPARE(cache.size(), (Vector3i{1024, 512, 1}));
    CORRADE_COMPARE(cache.processedFormat(), PixelFormat::R16F);
    CORRADE_COMPARE(cache.processedSize(), (Vector3i{256, 128, 1}));
    /* 1 by default to avoid artifacts */
    CORRADE_COMPARE(cache.padding(), Vector2i{1});
    CORRADE_COMPARE(cache.fontCount(), 0);
    /* Invalid glyph is always present */
    CORRADE_COMPARE(cache.glyphCount(), 1);
    CORRADE_COMPARE(cache.atlas().size(), (Vector3i{1024, 512, 1}));
    CORRADE_COMPARE(cache.atlas().filledSize(), (Vector3i{1024, 0, 1}));
    CORRADE_COMPARE(cache.atlas().flags(), TextureTools::AtlasLandfillFlag::WidestFirst);
    CORRADE_COMPARE(cache.atlas().padding(), Vector2i{1});

    /* The rest shouldn't be any different */
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractGlyphCacheTest::constructDeprecated() {
    /* Testing just the minimal set of getters as the deprecated constructor
       should delegate */

    CORRADE_IGNORE_DEPRECATED_PUSH
    /* Not using the DummyGlyphCache as it'd warn about the deprecated
       constructor even with the IGNORE macros */
    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
        void doSetImage(const Vector2i&, const ImageView2D&) override {}
    } cache{{1024, 512}, {2, 5}};
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(cache.format(), PixelFormat::R8Unorm);
    CORRADE_COMPARE(cache.size(), (Vector3i{1024, 512, 1}));
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(cache.textureSize(), (Vector2i{1024, 512}));
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(cache.padding(), (Vector2i{2, 5}));
    CORRADE_COMPARE(cache.fontCount(), 0);
    CORRADE_COMPARE(cache.glyphCount(), 1);
    CORRADE_COMPARE(cache.atlas().size(), (Vector3i{1024, 512, 1}));
    CORRADE_COMPARE(cache.atlas().filledSize(), (Vector3i{1024, 0, 1}));
    CORRADE_COMPARE(cache.atlas().flags(), TextureTools::AtlasLandfillFlag::WidestFirst);
    CORRADE_COMPARE(cache.atlas().padding(), (Vector2i{2, 5}));
}

void AbstractGlyphCacheTest::constructDeprecatedNoPadding() {
    /* Testing just the minimal set of getters as the deprecated constructor
       should delegate */

    CORRADE_IGNORE_DEPRECATED_PUSH
    /* Not using the DummyGlyphCache as it'd warn about the deprecated
       constructor even with the IGNORE macros */
    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
        void doSetImage(const Vector2i&, const ImageView2D&) override {}
    } cache{{1024, 512}};
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(cache.format(), PixelFormat::R8Unorm);
    CORRADE_COMPARE(cache.size(), (Vector3i{1024, 512, 1}));
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(cache.textureSize(), (Vector2i{1024, 512}));
    CORRADE_IGNORE_DEPRECATED_POP
    /* 1 by default to avoid artifacts */
    CORRADE_COMPARE(cache.padding(), Vector2i{1});
    CORRADE_COMPARE(cache.fontCount(), 0);
    CORRADE_COMPARE(cache.glyphCount(), 1);
    CORRADE_COMPARE(cache.atlas().size(), (Vector3i{1024, 512, 1}));
    CORRADE_COMPARE(cache.atlas().filledSize(), (Vector3i{1024, 0, 1}));
    CORRADE_COMPARE(cache.atlas().flags(), TextureTools::AtlasLandfillFlag::WidestFirst);
    CORRADE_COMPARE(cache.atlas().padding(), Vector2i{1});
}
#endif

void AbstractGlyphCacheTest::constructImageRowPadding() {
    /* This shouldn't assert due to the data for the image being too small */

    DummyGlyphCache cache{PixelFormat::RGB8Unorm, {2, 3, 5}};
    CORRADE_COMPARE(cache.size(), (Vector3i{2, 3, 5}));
    CORRADE_COMPARE(cache.image().format(), PixelFormat::RGB8Unorm);
    CORRADE_COMPARE(cache.image().size(), (Vector3i{2, 3, 5}));
    CORRADE_COMPARE(cache.image().data().size(), 8*3*5); /* not 6*3*5 */
}

void AbstractGlyphCacheTest::constructZeroSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    DummyGlyphCache{PixelFormat::R8Unorm, {2, 0, 1}};
    DummyGlyphCache{PixelFormat::R8Unorm, {0, 2, 1}};
    DummyGlyphCache{PixelFormat::R8Unorm, {2, 2, 0}};
    DummyGlyphCache{PixelFormat::R8Unorm, {2, 2}, PixelFormat::R8Unorm, {2, 0}};
    DummyGlyphCache{PixelFormat::R8Unorm, {2, 2}, PixelFormat::R8Unorm, {0, 2}};
    CORRADE_COMPARE(out,
        "Text::AbstractGlyphCache: expected non-zero size, got {2, 0, 1}\n"
        "Text::AbstractGlyphCache: expected non-zero size, got {0, 2, 1}\n"
        "Text::AbstractGlyphCache: expected non-zero size, got {2, 2, 0}\n"
        "Text::AbstractGlyphCache: expected non-zero processed size, got {2, 0}\n"
        "Text::AbstractGlyphCache: expected non-zero processed size, got {0, 2}\n");
}

void AbstractGlyphCacheTest::constructNoCreate() {
    DummyGlyphCache cache{NoCreate};

    /* Shouldn't crash */
    CORRADE_VERIFY(true);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoCreateT, DummyGlyphCache>::value);
}

void AbstractGlyphCacheTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<DummyGlyphCache>{});
    CORRADE_VERIFY(!std::is_copy_assignable<DummyGlyphCache>{});
}

void AbstractGlyphCacheTest::constructMove() {
    DummyGlyphCache a{PixelFormat::R16F, {1024, 512, 3}, {2, 5}};

    DummyGlyphCache b = Utility::move(a);
    CORRADE_COMPARE(b.size(), (Vector3i{1024, 512, 3}));

    DummyGlyphCache c{PixelFormat::R8Unorm, {2, 3}};
    c = Utility::move(b);
    CORRADE_COMPARE(c.size(), (Vector3i{1024, 512, 3}));

    CORRADE_VERIFY(std::is_nothrow_move_constructible<DummyGlyphCache>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<DummyGlyphCache>::value);
}

void AbstractGlyphCacheTest::features() {
    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return GlyphCacheFeature::ImageProcessing; }
        void doSetImage(const Vector2i&, const ImageView2D&) override {}
    } cache{PixelFormat::R8Unorm, {2, 3}};

    CORRADE_COMPARE(cache.features(), GlyphCacheFeature::ImageProcessing);
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractGlyphCacheTest::textureSizeNot2D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512, 3}};

    Containers::String out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    cache.textureSize();
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out, "Text::AbstractGlyphCache::textureSize(): can't be used on an array glyph cache\n");
}
#endif

void AbstractGlyphCacheTest::setInvalidGlyph() {
    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512, 3}, {2, 3}};

    cache.setInvalidGlyph({3, 5}, 2, {{15, 30}, {45, 35}});
    CORRADE_COMPARE(cache.glyphCount(), 1);
    CORRADE_COMPARE(cache.glyph(0), Containers::triple(
        Vector2i{1, 2},
        2,
        Range2Di{{13, 27}, {47, 38}}));

    /* Invalid glyph spanning the whole area (with padding) shouldn't assert */
    cache.setInvalidGlyph({3, 5}, 2, {{2, 3}, {1022, 509}});
    CORRADE_COMPARE(cache.glyphCount(), 1);
    CORRADE_COMPARE(cache.glyph(0), Containers::triple(
        Vector2i{1, 2},
        2,
        Range2Di{{}, {1024, 512}}));
}

void AbstractGlyphCacheTest::setInvalidGlyph2D() {
    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512}, {2, 3}};

    cache.setInvalidGlyph({3, 5}, {{15, 30}, {45, 35}});
    CORRADE_COMPARE(cache.glyphCount(), 1);
    CORRADE_COMPARE(cache.glyph(0), Containers::triple(
        Vector2i{1, 2},
        0,
        Range2Di{{13, 27}, {47, 38}}));

    /* Invalid glyph spanning the whole area is tested above already */
}

void AbstractGlyphCacheTest::setInvalidGlyphOutOfRange() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* Default padding is 1, test that it works for zero as well */
    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512, 3}, {}};

    Containers::String out;
    Error redirectError{&out};
    cache.setInvalidGlyph({}, -1, {{15, 30}, {45, 35}});
    cache.setInvalidGlyph({}, 3, {{15, 30}, {45, 35}});
    cache.setInvalidGlyph({}, 0, {{15, -1}, {45, 35}});
    cache.setInvalidGlyph({}, 0, {{-1, 30}, {45, 35}});
    cache.setInvalidGlyph({}, 0, {{15, 30}, {1025, 35}});
    cache.setInvalidGlyph({}, 0, {{15, 30}, {45, 513}});
    /* Negative rect size */
    cache.setInvalidGlyph({}, 0, {{45, 30}, {15, 35}});
    cache.setInvalidGlyph({}, 0, {{15, 35}, {45, 30}});
    CORRADE_COMPARE_AS(out,
        "Text::AbstractGlyphCache::setInvalidGlyph(): layer -1 and rectangle {{15, 30}, {45, 35}} out of range for size {1024, 512, 3} and padding {0, 0}\n"
        "Text::AbstractGlyphCache::setInvalidGlyph(): layer 3 and rectangle {{15, 30}, {45, 35}} out of range for size {1024, 512, 3} and padding {0, 0}\n"

        "Text::AbstractGlyphCache::setInvalidGlyph(): layer 0 and rectangle {{15, -1}, {45, 35}} out of range for size {1024, 512, 3} and padding {0, 0}\n"
        "Text::AbstractGlyphCache::setInvalidGlyph(): layer 0 and rectangle {{-1, 30}, {45, 35}} out of range for size {1024, 512, 3} and padding {0, 0}\n"
        "Text::AbstractGlyphCache::setInvalidGlyph(): layer 0 and rectangle {{15, 30}, {1025, 35}} out of range for size {1024, 512, 3} and padding {0, 0}\n"
        "Text::AbstractGlyphCache::setInvalidGlyph(): layer 0 and rectangle {{15, 30}, {45, 513}} out of range for size {1024, 512, 3} and padding {0, 0}\n"

        "Text::AbstractGlyphCache::setInvalidGlyph(): layer 0 and rectangle {{45, 30}, {15, 35}} out of range for size {1024, 512, 3} and padding {0, 0}\n"
        "Text::AbstractGlyphCache::setInvalidGlyph(): layer 0 and rectangle {{15, 35}, {45, 30}} out of range for size {1024, 512, 3} and padding {0, 0}\n",
        TestSuite::Compare::String);
}

void AbstractGlyphCacheTest::setInvalidGlyphOutOfRangePadded() {
    CORRADE_SKIP_IF_NO_ASSERT();

    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512, 3}, {2, 3}};

    Containers::String out;
    Error redirectError{&out};
    /* Padding has no effect on layers */
    cache.setInvalidGlyph({}, -1, {{15, 30}, {45, 35}});
    cache.setInvalidGlyph({}, 3, {{15, 30}, {45, 35}});
    /* These four pass if padding is not included in the check */
    cache.setInvalidGlyph({}, 0, {{15, 1}, {45, 35}});
    cache.setInvalidGlyph({}, 0, {{1, 30}, {45, 35}});
    cache.setInvalidGlyph({}, 0, {{15, 30}, {1023, 35}});
    cache.setInvalidGlyph({}, 0, {{15, 30}, {45, 510}});
    /* Negative rect size. The second would pass if it was checked with
       padding included. */
    cache.setInvalidGlyph({}, 0, {{45, 30}, {15, 35}});
    cache.setInvalidGlyph({}, 0, {{15, 35}, {45, 30}});
    CORRADE_COMPARE_AS(out,
        "Text::AbstractGlyphCache::setInvalidGlyph(): layer -1 and rectangle {{15, 30}, {45, 35}} out of range for size {1024, 512, 3} and padding {2, 3}\n"
        "Text::AbstractGlyphCache::setInvalidGlyph(): layer 3 and rectangle {{15, 30}, {45, 35}} out of range for size {1024, 512, 3} and padding {2, 3}\n"

        "Text::AbstractGlyphCache::setInvalidGlyph(): layer 0 and rectangle {{15, 1}, {45, 35}} out of range for size {1024, 512, 3} and padding {2, 3}\n"
        "Text::AbstractGlyphCache::setInvalidGlyph(): layer 0 and rectangle {{1, 30}, {45, 35}} out of range for size {1024, 512, 3} and padding {2, 3}\n"
        "Text::AbstractGlyphCache::setInvalidGlyph(): layer 0 and rectangle {{15, 30}, {1023, 35}} out of range for size {1024, 512, 3} and padding {2, 3}\n"
        "Text::AbstractGlyphCache::setInvalidGlyph(): layer 0 and rectangle {{15, 30}, {45, 510}} out of range for size {1024, 512, 3} and padding {2, 3}\n"

        "Text::AbstractGlyphCache::setInvalidGlyph(): layer 0 and rectangle {{45, 30}, {15, 35}} out of range for size {1024, 512, 3} and padding {2, 3}\n"
        "Text::AbstractGlyphCache::setInvalidGlyph(): layer 0 and rectangle {{15, 35}, {45, 30}} out of range for size {1024, 512, 3} and padding {2, 3}\n",
        TestSuite::Compare::String);
}

void AbstractGlyphCacheTest::setInvalidGlyph2DNot2D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512, 3}};

    Containers::String out;
    Error redirectError{&out};
    cache.setInvalidGlyph({}, {});
    CORRADE_COMPARE(out, "Text::AbstractGlyphCache::setInvalidGlyph(): use the layer overload for an array glyph cache\n");
}

void AbstractGlyphCacheTest::addFont() {
    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512}};

    const AbstractFont* font = reinterpret_cast<const AbstractFont*>(std::size_t{0xdeadbeef});
    CORRADE_COMPARE(cache.findFont(*font), Containers::NullOpt);

    CORRADE_COMPARE(cache.addFont(35, nullptr), 0);
    CORRADE_COMPARE(cache.fontCount(), 1);
    CORRADE_COMPARE(cache.glyphCount(), 1);
    CORRADE_COMPARE(cache.fontGlyphCount(0), 35);
    CORRADE_COMPARE(cache.fontPointer(0), nullptr);
    CORRADE_COMPARE(cache.findFont(*font), Containers::NullOpt);

    CORRADE_COMPARE(cache.addFont(12, font), 1);
    CORRADE_COMPARE(cache.fontCount(), 2);
    CORRADE_COMPARE(cache.glyphCount(), 1);
    CORRADE_COMPARE(cache.fontGlyphCount(1), 12);
    CORRADE_COMPARE(cache.fontPointer(1), font);
    CORRADE_COMPARE(cache.findFont(*font), 1);
}

void AbstractGlyphCacheTest::addFontDuplicatePointer() {
    CORRADE_SKIP_IF_NO_ASSERT();
    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512}};

    cache.addFont(7, nullptr);

    const AbstractFont* font = reinterpret_cast<const AbstractFont*>(std::size_t{0xdeadbeef});
    cache.addFont(35, font);

    Containers::String out;
    Error redirectError{&out};
    cache.addFont(12, font);
    CORRADE_COMPARE(out, "Text::AbstractGlyphCache::addFont(): pointer 0xdeadbeef already used for font 1\n");
}

void AbstractGlyphCacheTest::fontOutOfRange() {
    CORRADE_SKIP_IF_NO_ASSERT();
    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512}};

    const AbstractFont* font = reinterpret_cast<const AbstractFont*>(std::size_t{0xdeadbeef});
    cache.addFont(35, nullptr);
    cache.addFont(12, font);
    CORRADE_COMPARE(cache.fontCount(), 2);

    Containers::String out;
    Error redirectError{&out};
    cache.fontGlyphCount(2);
    cache.fontPointer(2);
    CORRADE_COMPARE(out,
        "Text::AbstractGlyphCache::fontGlyphCount(): index 2 out of range for 2 fonts\n"
        "Text::AbstractGlyphCache::fontPointer(): index 2 out of range for 2 fonts\n");
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractGlyphCacheTest::reserve() {
    DummyGlyphCache cache{PixelFormat::R8Unorm, {24, 20}, {1, 2}};

    /* Padding should get applied to all */
    CORRADE_IGNORE_DEPRECATED_PUSH
    std::vector<Range2Di> out = cache.reserve({
        {5, 3},
        /* Landscape glyphs shouldn't get rotated */
        {12, 6},
        {10, 5},
        /* Zero-sized glyphs should get preserved */
        {0, 1},
        {3, 0},
    });
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE_AS(Containers::arrayView(out), Containers::arrayView<Range2Di>({
        Range2Di::fromSize({6, 12}, {5, 3}),
        Range2Di::fromSize({1, 2}, {12, 6}),
        Range2Di::fromSize({13, 12}, {10, 5}),
        Range2Di::fromSize({4, 12}, {0, 1}),
        Range2Di::fromSize({1, 17}, {3, 0}),
    }), TestSuite::Compare::Container);
}

void AbstractGlyphCacheTest::reserveIncremental() {
    CORRADE_SKIP_IF_NO_ASSERT();

    DummyGlyphCache cache{PixelFormat::R8Unorm, {24, 20}, {1, 2}};

    /* insert() is what triggers the assert, not reserve() alone */
    CORRADE_IGNORE_DEPRECATED_PUSH
    cache.insert(34, {3, 5}, {{10, 10}, {23, 10}});
    CORRADE_IGNORE_DEPRECATED_POP

    Containers::String out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    cache.reserve({{12, 6}});
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out, "Text::AbstractGlyphCache::reserve(): reserving space in non-empty cache is not yet implemented\n");
}

void AbstractGlyphCacheTest::reserveTooSmall() {
    DummyGlyphCache cache{PixelFormat::R8Unorm, {24, 18}, {1, 2}};

    Containers::String out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_VERIFY(cache.reserve({{5, 3}, {12, 6}, {10, 5}}).empty());
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out, "Text::AbstractGlyphCache::reserve(): requested atlas size Vector(24, 18) is too small to fit 3 textures. Generated atlas will be empty.\n");
}

void AbstractGlyphCacheTest::reserveNot2D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512, 3}};

    Containers::String out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    cache.reserve({});
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out, "Text::AbstractGlyphCache::reserve(): can't be used on an array glyph cache\n");
}
#endif

void AbstractGlyphCacheTest::addGlyph() {
    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512, 3}, {2, 3}};

    UnsignedInt font9 = cache.addFont(9);
    UnsignedInt font3 = cache.addFont(3);

    /* The queried values are with padding applied */
    UnsignedInt font9Glyph6 = cache.addGlyph(font9, 6, {3, 4}, 2, {{15, 30}, {45, 35}});
    CORRADE_COMPARE(font9Glyph6, 1);
    CORRADE_COMPARE(cache.glyph(font9Glyph6), Containers::triple(
        Vector2i{1, 1},
        2,
        Range2Di{{13, 27}, {47, 38}}));

    /* Glyph in another font */
    UnsignedInt font3Glyph1 = cache.addGlyph(font3, 1, {5, 6}, 1, {{10, 15}, {25, 30}});
    CORRADE_COMPARE(font3Glyph1, 2);
    CORRADE_COMPARE(cache.glyph(font3Glyph1), Containers::triple(
        Vector2i{3, 3},
        1,
        Range2Di{{8, 12}, {27, 33}}));

    /* Glyph adding order shouldn't matter; glyph spanning the whole area (with
       padding) shouldn't assert */
    UnsignedInt font3Glyph0 = cache.addGlyph(font3, 0, {3, 5}, 2, {{2, 3}, {1022, 509}});
    CORRADE_COMPARE(font3Glyph0, 3);
    CORRADE_COMPARE(cache.glyph(font3Glyph0), Containers::triple(
        Vector2i{1, 2},
        2,
        Range2Di{{}, {1024, 512}}));

    /* Another glyph in an earlier font */
    UnsignedInt font9Glyph3 = cache.addGlyph(font9, 3, {5, 7}, 0, {{5, 10}, {15, 30}});
    CORRADE_COMPARE(font9Glyph3, 4);
    CORRADE_COMPARE(cache.glyph(font9Glyph3), Containers::triple(
        Vector2i{3, 4},
        0,
        Range2Di{{3, 7}, {17, 33}}));
}

void AbstractGlyphCacheTest::addGlyph2D() {
    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512}, {2, 3}};

    cache.addFont(9);
    UnsignedInt fontId = cache.addFont(3);
    CORRADE_COMPARE(cache.addGlyph(fontId, 2, {3, 5}, {{15, 30}, {45, 35}}), 1);
    CORRADE_COMPARE(cache.glyphCount(), 2);
    CORRADE_COMPARE(cache.glyph(1), Containers::triple(
        Vector2i{1, 2},
        0,
        Range2Di{{13, 27}, {47, 38}}));
}

void AbstractGlyphCacheTest::addGlyphIndexOutOfRange() {
    CORRADE_SKIP_IF_NO_ASSERT();

    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512, 3}};

    cache.addFont(9);
    UnsignedInt fontId = cache.addFont(3);

    Containers::String out;
    Error redirectError{&out};
    cache.addGlyph(cache.fontCount(), 0, {}, 2, {});
    cache.addGlyph(fontId, cache.fontGlyphCount(fontId), {}, 2, {});
    CORRADE_COMPARE(out,
        "Text::AbstractGlyphCache::addGlyph(): index 2 out of range for 2 fonts\n"
        "Text::AbstractGlyphCache::addGlyph(): index 3 out of range for 3 glyphs in font 1\n");
}

void AbstractGlyphCacheTest::addGlyphAlreadyAdded() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* Default padding of 1 makes it impossible to add a glyph at zero offset
       as it's out of range. Don't want to bother with that here so resetting
       it to 0. */
    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512, 3}, {}};

    cache.addFont(9);
    UnsignedInt fontId = cache.addFont(3);
    cache.addGlyph(fontId, 0, {}, 2, {});
    cache.addGlyph(fontId, 1, {}, 2, {});
    cache.addGlyph(fontId, 2, {}, 2, {});

    Containers::String out;
    Error redirectError{&out};
    cache.addGlyph(fontId, 2, {}, 2, {});
    CORRADE_COMPARE(out,
        "Text::AbstractGlyphCache::addGlyph(): glyph 2 in font 1 already added at index 3\n");
}

void AbstractGlyphCacheTest::addGlyphOutOfRange() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* Default padding is 1, test that it works for zero as well */
    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512, 3}, {}};

    UnsignedInt fontId = cache.addFont(9);

    Containers::String out;
    Error redirectError{&out};
    cache.addGlyph(fontId, 1, {}, -1, {{15, 30}, {45, 35}});
    cache.addGlyph(fontId, 2, {}, 3, {{15, 30}, {45, 35}});
    cache.addGlyph(fontId, 3, {}, 0, {{15, -1}, {45, 35}});
    cache.addGlyph(fontId, 4, {}, 0, {{-1, 30}, {45, 35}});
    cache.addGlyph(fontId, 5, {}, 0, {{15, 30}, {1025, 35}});
    cache.addGlyph(fontId, 6, {}, 0, {{15, 30}, {45, 513}});
    /* Negative rect size */
    cache.addGlyph(fontId, 8, {}, 0, {{45, 30}, {15, 35}});
    cache.addGlyph(fontId, 7, {}, 0, {{15, 35}, {45, 30}});
    CORRADE_COMPARE_AS(out,
        "Text::AbstractGlyphCache::addGlyph(): layer -1 and rectangle {{15, 30}, {45, 35}} out of range for size {1024, 512, 3} and padding {0, 0}\n"
        "Text::AbstractGlyphCache::addGlyph(): layer 3 and rectangle {{15, 30}, {45, 35}} out of range for size {1024, 512, 3} and padding {0, 0}\n"

        "Text::AbstractGlyphCache::addGlyph(): layer 0 and rectangle {{15, -1}, {45, 35}} out of range for size {1024, 512, 3} and padding {0, 0}\n"
        "Text::AbstractGlyphCache::addGlyph(): layer 0 and rectangle {{-1, 30}, {45, 35}} out of range for size {1024, 512, 3} and padding {0, 0}\n"
        "Text::AbstractGlyphCache::addGlyph(): layer 0 and rectangle {{15, 30}, {1025, 35}} out of range for size {1024, 512, 3} and padding {0, 0}\n"
        "Text::AbstractGlyphCache::addGlyph(): layer 0 and rectangle {{15, 30}, {45, 513}} out of range for size {1024, 512, 3} and padding {0, 0}\n"

        "Text::AbstractGlyphCache::addGlyph(): layer 0 and rectangle {{45, 30}, {15, 35}} out of range for size {1024, 512, 3} and padding {0, 0}\n"
        "Text::AbstractGlyphCache::addGlyph(): layer 0 and rectangle {{15, 35}, {45, 30}} out of range for size {1024, 512, 3} and padding {0, 0}\n",
        TestSuite::Compare::String);
}

void AbstractGlyphCacheTest::addGlyphOutOfRangePadded() {
    CORRADE_SKIP_IF_NO_ASSERT();

    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512, 3}, {2, 3}};

    UnsignedInt fontId = cache.addFont(9);

    Containers::String out;
    Error redirectError{&out};
    /* Padding has no effect on layers */
    cache.addGlyph(fontId, 1, {}, -1, {{15, 30}, {45, 35}});
    cache.addGlyph(fontId, 2, {}, 3, {{15, 30}, {45, 35}});
    /* These four pass if padding is not included in the check */
    cache.addGlyph(fontId, 3, {}, 0, {{15, 1}, {45, 35}});
    cache.addGlyph(fontId, 4, {}, 0, {{1, 30}, {45, 35}});
    cache.addGlyph(fontId, 5, {}, 0, {{15, 30}, {1023, 35}});
    cache.addGlyph(fontId, 6, {}, 0, {{15, 30}, {45, 510}});
    /* Negative rect size. The second would pass if it was checked with
       padding included. */
    cache.addGlyph(fontId, 8, {}, 0, {{45, 30}, {15, 35}});
    cache.addGlyph(fontId, 7, {}, 0, {{15, 35}, {45, 30}});
    CORRADE_COMPARE_AS(out,
        "Text::AbstractGlyphCache::addGlyph(): layer -1 and rectangle {{15, 30}, {45, 35}} out of range for size {1024, 512, 3} and padding {2, 3}\n"
        "Text::AbstractGlyphCache::addGlyph(): layer 3 and rectangle {{15, 30}, {45, 35}} out of range for size {1024, 512, 3} and padding {2, 3}\n"

        "Text::AbstractGlyphCache::addGlyph(): layer 0 and rectangle {{15, 1}, {45, 35}} out of range for size {1024, 512, 3} and padding {2, 3}\n"
        "Text::AbstractGlyphCache::addGlyph(): layer 0 and rectangle {{1, 30}, {45, 35}} out of range for size {1024, 512, 3} and padding {2, 3}\n"
        "Text::AbstractGlyphCache::addGlyph(): layer 0 and rectangle {{15, 30}, {1023, 35}} out of range for size {1024, 512, 3} and padding {2, 3}\n"
        "Text::AbstractGlyphCache::addGlyph(): layer 0 and rectangle {{15, 30}, {45, 510}} out of range for size {1024, 512, 3} and padding {2, 3}\n"

        "Text::AbstractGlyphCache::addGlyph(): layer 0 and rectangle {{45, 30}, {15, 35}} out of range for size {1024, 512, 3} and padding {2, 3}\n"
        "Text::AbstractGlyphCache::addGlyph(): layer 0 and rectangle {{15, 35}, {45, 30}} out of range for size {1024, 512, 3} and padding {2, 3}\n",
        TestSuite::Compare::String);
}

void AbstractGlyphCacheTest::addGlyphTooMany() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* Default padding of 1 makes it impossible to add a glyph at zero offset
       as it's out of range. Don't want to bother with that here so resetting
       it to 0. */
    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512}, {}};

    /* Adding a font with over 65k potential glyphs is okay */
    UnsignedInt fontId = cache.addFont(100000);

    for(UnsignedInt i = 0; i != 65535; ++i)
        cache.addGlyph(fontId, i, {}, {});

    CORRADE_COMPARE(cache.glyphCount(), 65536);

    /* But adding 65k actual glyphs isn't */
    Containers::String out;
    Error redirectError{&out};
    cache.addGlyph(fontId, 65536, {}, {});
    CORRADE_COMPARE(out, "Text::AbstractGlyphCache::addGlyph(): only at most 65536 glyphs can be added\n");
}

void AbstractGlyphCacheTest::addGlyph2DNot2D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512, 3}};

    Containers::String out;
    Error redirectError{&out};
    cache.addGlyph(0, 0, {}, {});
    CORRADE_COMPARE(out, "Text::AbstractGlyphCache::addGlyph(): use the layer overload for an array glyph cache\n");
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractGlyphCacheTest::insert() {
    DummyGlyphCache cache{PixelFormat::R8Unorm, {100, 200}, {2, 3}};

    /* Overwriting the "Not Found" glyph. Shouldn't result in any font or glyph
       being added. */
    CORRADE_IGNORE_DEPRECATED_PUSH
    cache.insert(0, {3, 5}, {{10, 10}, {23, 45}});
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(cache.glyphCount(), 1);
    CORRADE_COMPARE(cache.fontCount(), 0);
    CORRADE_COMPARE(cache.glyph(0), Containers::triple(
        Vector2i{1, 2},
        0,
        Range2Di{{8, 7}, {25, 48}}));

    /* Adding a new glyph adds the first font if not there yet, setting its
       glyph count to fit the glyph ID */
    CORRADE_IGNORE_DEPRECATED_PUSH
    cache.insert(25, {3, 4}, {{15, 30}, {45, 35}});
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(cache.glyphCount(), 2);
    CORRADE_COMPARE(cache.fontCount(), 1);
    CORRADE_COMPARE(cache.fontGlyphCount(0), 26);
    CORRADE_COMPARE(cache.glyph(0, 25), Containers::triple(
        Vector2i{1, 1},
        0,
        Range2Di{{13, 27}, {47, 38}}));

    /* Adding another glyph with a lower ID doesn't change the font in any
       way  */
    CORRADE_IGNORE_DEPRECATED_PUSH
    cache.insert(5, {5, 6}, {{10, 15}, {25, 30}});
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(cache.glyphCount(), 3);
    CORRADE_COMPARE(cache.fontCount(), 1);
    CORRADE_COMPARE(cache.fontGlyphCount(0), 26);
    CORRADE_COMPARE(cache.glyph(0, 5), Containers::triple(
        Vector2i{3, 3},
        0,
        Range2Di{{8, 12}, {27, 33}}));

    /* Adding a glyph with greater ID expands the font glyph count again */
    CORRADE_IGNORE_DEPRECATED_PUSH
    cache.insert(35, {5, 7}, {{5, 10}, {15, 30}});
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(cache.glyphCount(), 4);
    CORRADE_COMPARE(cache.fontCount(), 1);
    CORRADE_COMPARE(cache.fontGlyphCount(0), 36);
    CORRADE_COMPARE(cache.glyph(0, 35), Containers::triple(
        Vector2i{3, 4},
        0,
        Range2Di{{3, 7}, {17, 33}}));
}

void AbstractGlyphCacheTest::insertNot2D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512, 3}};

    Containers::String out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    cache.insert(0, {}, {});
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out, "Text::AbstractGlyphCache::insert(): can't be used on an array glyph cache\n");
}

void AbstractGlyphCacheTest::insertMultiFont() {
    CORRADE_SKIP_IF_NO_ASSERT();

    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512}};

    cache.addFont(15);
    cache.addFont(35);

    Containers::String out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    cache.insert(0, {}, {});
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out, "Text::AbstractGlyphCache::insert(): can't be used on a multi-font glyph cache\n");
}
#endif

void AbstractGlyphCacheTest::flushImage() {
    auto&& data = FlushImageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct Cache: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
        void doSetImage(const Vector3i& offset, const ImageView3D& image) override {
            called = true;

            CORRADE_COMPARE(offset, (Vector3i{15, 30, 3} - Vector3i{padding(), 0}));
            CORRADE_COMPARE(image.size(), (Vector3i{3, 2, 2} + Vector3i{2*padding(), 0}));

            if(padding().isZero()) {
                char pixels0[]{
                    'a', 'b', 'c', 0,
                    'd', 'e', 'f', 0,
                };
                char pixels1[]{
                    '0', '1', '2', 0,
                    '3', '4', '5', 0,
                };
                CORRADE_COMPARE_AS(image.pixels<Byte>()[0],
                    (ImageView2D{PixelFormat::R8Snorm, {3, 2}, pixels0}),
                    DebugTools::CompareImage);
                CORRADE_COMPARE_AS(image.pixels<Byte>()[1],
                    (ImageView2D{PixelFormat::R8Snorm, {3, 2}, pixels1}),
                    DebugTools::CompareImage);
            } else {
                char pixels0[]{
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0, 'a', 'b', 'c', 0, 0, 0,
                    0, 0, 'd', 'e', 'f', 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0
                };
                char pixels1[]{
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0, '0', '1', '2', 0, 0, 0,
                    0, 0, '3', '4', '5', 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0
                };
                CORRADE_COMPARE_AS(image.pixels<Byte>()[0],
                    (ImageView2D{PixelFormat::R8Snorm, {7, 8}, pixels0}),
                    DebugTools::CompareImage);
                CORRADE_COMPARE_AS(image.pixels<Byte>()[1],
                    (ImageView2D{PixelFormat::R8Snorm, {7, 8}, pixels1}),
                    DebugTools::CompareImage);
            }
        }

        bool called = false;
    } cache{NoCreate};

    if(data.differentProcessedFormatSize)
        cache = Cache{PixelFormat::R8Snorm, {45, 35, 5}, PixelFormat::RG32F, {12, 34}};
    else
        cache = Cache{PixelFormat::R8Snorm, {45, 35, 5}, data.padding};

    /* Capture correct function name */
    CORRADE_VERIFY(true);

    /* Copy two slices of the image */
    char pixels[]{
        'a', 'b', 'c',
        'd', 'e', 'f',
        '0', '1', '2',
        '3', '4', '5',
    };
    Utility::copy(
        Containers::StridedArrayView3D<char>{pixels, {2, 2, 3}},
        cache.image().pixels<char>().sliceSize({3, 30, 15}, {2, 2, 3}));

    cache.flushImage(Range3Di::fromSize({15, 30, 3}, {3, 2, 2}));
    CORRADE_VERIFY(cache.called);
}

void AbstractGlyphCacheTest::flushImageWholeArea() {
    auto&& data = FlushImageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Like above, but calling flushImage() with the whole size to test bounds
       checking. The padding doesn't affect the call in this case -- the actual
       range is always the whole image. */

    struct Cache: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
        void doSetImage(const Vector3i& offset, const ImageView3D& image) override {
            called = true;

            char pixels0[]{
                'a', 'b', 'c', 0,
                'd', 'e', 'f', 0,
            };
            char pixels1[]{
                '0', '1', '2', 0,
                '3', '4', '5', 0,
            };
            CORRADE_COMPARE(offset, Vector3i{});
            CORRADE_COMPARE(image.size(), (Vector3i{3, 2, 2}));
            CORRADE_COMPARE_AS(image.pixels<Byte>()[0],
                (ImageView2D{PixelFormat::R8Snorm, {3, 2}, pixels0}),
                DebugTools::CompareImage);
            CORRADE_COMPARE_AS(image.pixels<Byte>()[1],
                (ImageView2D{PixelFormat::R8Snorm, {3, 2}, pixels1}),
                DebugTools::CompareImage);
        }

        bool called = false;
    } cache{NoCreate};

    if(data.differentProcessedFormatSize)
        cache = Cache{PixelFormat::R8Snorm, {3, 2, 5}, PixelFormat::RG32F, {1, 1}};
    else
        cache = Cache{PixelFormat::R8Snorm, {3, 2, 2}, data.padding};

    /* Capture correct function name */
    CORRADE_VERIFY(true);

    /* Copy two slices of the image */
    char pixels[]{
        'a', 'b', 'c',
        'd', 'e', 'f',
        '0', '1', '2',
        '3', '4', '5',
    };
    Utility::copy(
        Containers::StridedArrayView3D<char>{pixels, {2, 2, 3}},
        cache.image().pixels<char>());

    cache.flushImage({{}, {3, 2, 2}});
    CORRADE_VERIFY(cache.called);
}

void AbstractGlyphCacheTest::flushImageLayer() {
    auto&& data = FlushImageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Single slice subset of flushImage() */

    struct Cache: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
        void doSetImage(const Vector3i& offset, const ImageView3D& image) override {
            called = true;

            CORRADE_COMPARE(offset, (Vector3i{15, 30, 3} - Vector3i{padding(), 0}));
            CORRADE_COMPARE(image.size(), (Vector3i{3, 2, 1} + Vector3i{2*padding(), 0}));

            if(padding().isZero()) {
                char pixels[]{
                    'a', 'b', 'c', 0,
                    'd', 'e', 'f', 0,
                };
                CORRADE_COMPARE_AS(image.pixels<Byte>()[0],
                    (ImageView2D{PixelFormat::R8Snorm, {3, 2}, pixels}),
                    DebugTools::CompareImage);
            } else {
                char pixels[]{
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0, 'a', 'b', 'c', 0, 0, 0,
                    0, 0, 'd', 'e', 'f', 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0
                };
                CORRADE_COMPARE_AS(image.pixels<Byte>()[0],
                    (ImageView2D{PixelFormat::R8Snorm, {7, 8}, pixels}),
                    DebugTools::CompareImage);
            }
        }

        bool called = false;
    } cache{NoCreate};

    if(data.differentProcessedFormatSize)
        cache = Cache{PixelFormat::R8Snorm, {45, 35, 5}, PixelFormat::RG32F, {12, 34}};
    else
        cache = Cache{PixelFormat::R8Snorm, {45, 35, 5}, data.padding};

    /* Capture correct function name */
    CORRADE_VERIFY(true);

    char pixels[]{
        'a', 'b', 'c',
        'd', 'e', 'f',
    };
    Utility::copy(
        Containers::StridedArrayView3D<char>{pixels, {1, 2, 3}},
        cache.image().pixels<char>().sliceSize({3, 30, 15}, {1, 2, 3}));

    cache.flushImage(3, Range2Di::fromSize({15, 30}, {3, 2}));
    CORRADE_VERIFY(cache.called);
}

void AbstractGlyphCacheTest::flushImage2D() {
    auto&& data = FlushImageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Like flushImageLayer() but reduced to two dimensions */

    struct Cache: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
        void doSetImage(const Vector3i& offset, const ImageView3D& image) override {
            called = true;

            CORRADE_COMPARE(offset, (Vector3i{15, 30, 0} - Vector3i{padding(), 0}));
            CORRADE_COMPARE(image.size(), (Vector3i{3, 2, 1} + Vector3i{2*padding(), 0}));

            if(padding().isZero()) {
                char pixels[]{
                    'a', 'b', 'c', 0,
                    'd', 'e', 'f', 0,
                };
                CORRADE_COMPARE_AS(image.pixels<Byte>()[0],
                    (ImageView2D{PixelFormat::R8Snorm, {3, 2}, pixels}),
                    DebugTools::CompareImage);
            } else {
                char pixels[]{
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0, 'a', 'b', 'c', 0, 0, 0,
                    0, 0, 'd', 'e', 'f', 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0
                };
                CORRADE_COMPARE_AS(image.pixels<Byte>()[0],
                    (ImageView2D{PixelFormat::R8Snorm, {7, 8}, pixels}),
                    DebugTools::CompareImage);
            }
        }

        bool called = false;
    } cache{NoCreate};

    if(data.differentProcessedFormatSize)
        cache = Cache{PixelFormat::R8Snorm, {45, 35}, PixelFormat::RG32F, {12, 34}};
    else
        cache = Cache{PixelFormat::R8Snorm, {45, 35}, data.padding};

    /* Capture correct function name */
    CORRADE_VERIFY(true);

    char pixels[]{
        'a', 'b', 'c',
        'd', 'e', 'f',
    };
    Utility::copy(
        Containers::StridedArrayView2D<char>{pixels, {2, 3}},
        cache.image().pixels<char>()[0].sliceSize({30, 15}, {2, 3}));

    cache.flushImage(Range2Di::fromSize({15, 30}, {3, 2}));
    CORRADE_VERIFY(cache.called);
}

void AbstractGlyphCacheTest::flushImage2DPassthrough2D() {
    auto&& data = FlushImageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Like flushImage2D() but with 2D doSetImage() */

    struct Cache: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
        void doSetImage(const Vector2i& offset, const ImageView2D& image) override {
            called = true;

            CORRADE_COMPARE(offset, (Vector2i{15, 30}) - padding());
            CORRADE_COMPARE(image.size(), (Vector2i{3, 2}) + 2*padding());

            if(padding().isZero()) {
                char pixels[]{
                    'a', 'b', 'c', 0,
                    'd', 'e', 'f', 0,
                };
                CORRADE_COMPARE_AS(image.pixels<Byte>(),
                    (ImageView2D{PixelFormat::R8Snorm, {3, 2}, pixels}),
                    DebugTools::CompareImage);
            } else {
                char pixels[]{
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0, 'a', 'b', 'c', 0, 0, 0,
                    0, 0, 'd', 'e', 'f', 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0
                };
                CORRADE_COMPARE_AS(image.pixels<Byte>(),
                    (ImageView2D{PixelFormat::R8Snorm, {7, 8}, pixels}),
                    DebugTools::CompareImage);
            }
        }

        bool called = false;
    } cache{NoCreate};

    if(data.differentProcessedFormatSize)
        cache = Cache{PixelFormat::R8Snorm, {45, 35}, PixelFormat::RG32F, {12, 34}};
    else
        cache = Cache{PixelFormat::R8Snorm, {45, 35}, data.padding};

    /* Capture correct function name */
    CORRADE_VERIFY(true);

    char pixels[]{
        'a', 'b', 'c',
        'd', 'e', 'f',
    };
    Utility::copy(
        Containers::StridedArrayView2D<char>{pixels, {2, 3}},
        cache.image().pixels<char>()[0].sliceSize({30, 15}, {2, 3}));

    cache.flushImage(Range2Di::fromSize({15, 30}, {3, 2}));
    CORRADE_VERIFY(cache.called);
}

void AbstractGlyphCacheTest::flushImageNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }

        /* The 2D variant shouldn't be called on an array cache */
        void doSetImage(const Vector2i&, const ImageView2D&) override {
            CORRADE_FAIL("This should not be called");
        }
    } cache{PixelFormat::R32F, {1024, 512, 8}};

    Containers::String out;
    Error redirectError{&out};
    cache.flushImage(0, {});
    CORRADE_COMPARE(out, "Text::AbstractGlyphCache::image(): not implemented by derived class\n");
}

void AbstractGlyphCacheTest::flushImagePassthrough2DNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }

        /* Should try the 3D variant, and from that one call into the 2D where
           it'd assert */
    } cache{PixelFormat::R32F, {1024, 512}};

    Containers::String out;
    Error redirectError{&out};
    cache.flushImage(0, {});
    CORRADE_COMPARE(out, "Text::AbstractGlyphCache::image(): not implemented by derived class\n");
}

void AbstractGlyphCacheTest::flushImageOutOfRange() {
    auto&& data = FlushImageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    DummyGlyphCache cache{NoCreate};

    /* Neither the padding nor the processed size should not have any effect on
       the check */
    if(data.differentProcessedFormatSize)
        cache = DummyGlyphCache{PixelFormat::R32F, {1024, 512, 8}, PixelFormat::R8Snorm, {1536, 768}};
    else
        cache = DummyGlyphCache{PixelFormat::R32F, {1024, 512, 8}, data.padding};

    Containers::String out;
    Error redirectError{&out};
    /* Negative min X, Y, layer */
    cache.flushImage({{-1, 30, 4}, {45, 35, 6}});
    cache.flushImage({{15, -1, 4}, {45, 35, 6}});
    cache.flushImage({{15, 30, -1}, {45, 35, 6}});
    cache.flushImage(-1, {{15, 30}, {45, 35}});
    /* Too large max X, Y, layer */
    cache.flushImage({{15, 30, 4}, {1025, 35, 6}});
    cache.flushImage({{15, 30, 4}, {45, 513, 6}});
    cache.flushImage({{15, 30, 4}, {45, 35, 9}});
    cache.flushImage(8, {{15, 30}, {45, 35}});
    /* Negative range size on X, Y, layer */
    cache.flushImage({{45, 30, 4}, {15, 35, 6}});
    cache.flushImage({{15, 35, 4}, {45, 30, 6}});
    cache.flushImage({{15, 30, 6}, {45, 35, 4}});
    CORRADE_COMPARE_AS(out,
        "Text::AbstractGlyphCache::flushImage(): {{-1, 30, 4}, {45, 35, 6}} out of range for size {1024, 512, 8}\n"
        "Text::AbstractGlyphCache::flushImage(): {{15, -1, 4}, {45, 35, 6}} out of range for size {1024, 512, 8}\n"
        "Text::AbstractGlyphCache::flushImage(): {{15, 30, -1}, {45, 35, 6}} out of range for size {1024, 512, 8}\n"
        "Text::AbstractGlyphCache::flushImage(): {{15, 30, -1}, {45, 35, 0}} out of range for size {1024, 512, 8}\n"

        "Text::AbstractGlyphCache::flushImage(): {{15, 30, 4}, {1025, 35, 6}} out of range for size {1024, 512, 8}\n"
        "Text::AbstractGlyphCache::flushImage(): {{15, 30, 4}, {45, 513, 6}} out of range for size {1024, 512, 8}\n"
        "Text::AbstractGlyphCache::flushImage(): {{15, 30, 4}, {45, 35, 9}} out of range for size {1024, 512, 8}\n"
        "Text::AbstractGlyphCache::flushImage(): {{15, 30, 8}, {45, 35, 9}} out of range for size {1024, 512, 8}\n"

        "Text::AbstractGlyphCache::flushImage(): {{45, 30, 4}, {15, 35, 6}} out of range for size {1024, 512, 8}\n"
        "Text::AbstractGlyphCache::flushImage(): {{15, 35, 4}, {45, 30, 6}} out of range for size {1024, 512, 8}\n"
        "Text::AbstractGlyphCache::flushImage(): {{15, 30, 6}, {45, 35, 4}} out of range for size {1024, 512, 8}\n",
        TestSuite::Compare::String);
}

void AbstractGlyphCacheTest::flushImage2DNot2D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512, 3}};

    Containers::String out;
    Error redirectError{&out};
    cache.flushImage(Range2Di{});
    CORRADE_COMPARE(out, "Text::AbstractGlyphCache::flushImage(): use the 3D or layer overload for an array glyph cache\n");
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractGlyphCacheTest::setImage() {
    auto&& data = FlushImageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_IGNORE_DEPRECATED_PUSH
    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
        void doSetImage(const Vector2i& offset, const ImageView2D& image) override {
            called = true;

            CORRADE_COMPARE(offset, (Vector2i{15, 30}) - padding());
            CORRADE_COMPARE(image.size(), (Vector2i{3, 2}) + 2*padding());

            if(padding().isZero()) {
                char pixels[]{
                    'a', 'b', 'c', 0,
                    'd', 'e', 'f', 0,
                };
                CORRADE_COMPARE_AS(image.pixels<Byte>(),
                    (ImageView2D{PixelFormat::R8Snorm, {3, 2}, pixels}),
                    DebugTools::CompareImage);
            } else {
                char pixels[]{
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0, 'a', 'b', 'c', 0, 0, 0,
                    0, 0, 'd', 'e', 'f', 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0,
                    0, 0,   0,   0,   0, 0, 0, 0
                };
                CORRADE_COMPARE_AS(image.pixels<Byte>(),
                    (ImageView2D{PixelFormat::R8Snorm, {7, 8}, pixels}),
                    DebugTools::CompareImage);
            }
        }

        bool called = false;
    /* Deliberately using the deprecated PixelFormat-less constructor to verify
       that passing a R8Unorm image "just works" */
    } cache{{45, 35}, data.padding};
    CORRADE_IGNORE_DEPRECATED_POP

    /* Capture correct function name */
    CORRADE_VERIFY(true);

    char pixels[]{
        0,   0,   0,   0, 0,
        0, 'a', 'b', 'c', 0,
        0, 'd', 'e', 'f', 0,
        0,   0,   0,   0, 0
    };

    /* Testing with a custom PixelStorage to verify the right area gets copied
       to the internal image */
    CORRADE_IGNORE_DEPRECATED_PUSH
    cache.setImage({15, 30}, ImageView2D{
        PixelStorage{}
            .setAlignment(1)
            .setRowLength(5)
            .setSkip({1, 1, 0}),
        PixelFormat::R8Unorm,
        {3, 2},
        pixels});
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_VERIFY(cache.called);
}

void AbstractGlyphCacheTest::setImageOutOfRange() {
    CORRADE_SKIP_IF_NO_ASSERT();

    DummyGlyphCache cache{PixelFormat::R8Unorm, {100, 200}};

    /* This is fine */
    CORRADE_IGNORE_DEPRECATED_PUSH
    cache.setImage({80, 175}, ImageView2D{PixelFormat::R8Unorm, {20, 25}});
    CORRADE_IGNORE_DEPRECATED_POP

    Containers::String out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    cache.setImage({81, 175}, ImageView2D{PixelFormat::R8Unorm, {20, 25}});
    cache.setImage({80, 176}, ImageView2D{PixelFormat::R8Unorm, {20, 25}});
    cache.setImage({-1, 175}, ImageView2D{PixelFormat::R8Unorm, {20, 25}});
    cache.setImage({80, -1}, ImageView2D{PixelFormat::R8Unorm, {20, 25}});
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE_AS(out,
        "Text::AbstractGlyphCache::setImage(): Range({81, 175}, {101, 200}) out of range for glyph cache of size Vector(100, 200)\n"
        "Text::AbstractGlyphCache::setImage(): Range({80, 176}, {100, 201}) out of range for glyph cache of size Vector(100, 200)\n"
        "Text::AbstractGlyphCache::setImage(): Range({-1, 175}, {19, 200}) out of range for glyph cache of size Vector(100, 200)\n"
        "Text::AbstractGlyphCache::setImage(): Range({80, -1}, {100, 24}) out of range for glyph cache of size Vector(100, 200)\n",
        TestSuite::Compare::String);
}

void AbstractGlyphCacheTest::setImageInvalidFormat() {
    CORRADE_SKIP_IF_NO_ASSERT();

    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512}};

    Containers::String out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    cache.setImage({15, 30}, ImageView2D{PixelFormat::R8Unorm, {45, 35}});
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out, "Text::AbstractGlyphCache::setImage(): expected PixelFormat::R32F but got PixelFormat::R8Unorm\n");
}

void AbstractGlyphCacheTest::setImageNot2D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512, 3}};

    Containers::String out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    cache.setImage({}, ImageView2D{PixelFormat::R32F, {}, nullptr});
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out, "Text::AbstractGlyphCache::setImage(): can't be used on an array glyph cache\n");
}
#endif

void AbstractGlyphCacheTest::processedImage() {
    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override {
            return GlyphCacheFeature::ProcessedImageDownload;
        }
        void doSetImage(const Vector2i&, const ImageView2D&) override {}

        Image3D doProcessedImage() override { return Image3D{PixelFormat::RG8Unorm, {2, 3, 1}, Containers::Array<char>{NoInit, 6*2}}; }
    /* Using a different format or size for the source image shouldn't cause
       any problem */
    } cache{PixelFormat::RG8Srgb, {200, 300}};

    Image3D image = cache.processedImage();
    CORRADE_COMPARE(image.format(), PixelFormat::RG8Unorm);
    CORRADE_COMPARE(image.size(), (Vector3i{2, 3, 1}));
}

void AbstractGlyphCacheTest::processedImageNotSupported() {
    auto&& data = ProcessedImageNotSupportedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    struct Cache: AbstractGlyphCache {
        explicit Cache(const Vector2i& size, GlyphCacheFeatures features): AbstractGlyphCache{PixelFormat::R8Unorm, size}, _features{features} {}

        GlyphCacheFeatures doFeatures() const override { return _features; }
        void doSetImage(const Vector2i&, const ImageView2D&) override {}

        GlyphCacheFeatures _features;
    } cache{{200, 300}, data.features};

    Containers::String out;
    Error redirectError{&out};
    cache.processedImage();
    CORRADE_COMPARE(out, "Text::AbstractGlyphCache::processedImage(): feature not supported\n");
}

void AbstractGlyphCacheTest::processedImageNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override {
            return GlyphCacheFeature::ProcessedImageDownload;
        }
        void doSetImage(const Vector2i&, const ImageView2D&) override {}
    } cache{PixelFormat::R8Unorm, {200, 300}};

    Containers::String out;
    Error redirectError{&out};
    cache.processedImage();
    CORRADE_COMPARE(out, "Text::AbstractGlyphCache::processedImage(): feature advertised but not implemented\n");
}

void AbstractGlyphCacheTest::setProcessedImage() {
    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override {
            return GlyphCacheFeature::ImageProcessing;
        }
        void doSetProcessedImage(const Vector3i& offset, const ImageView3D& image) override {
            called = true;

            CORRADE_COMPARE(offset, (Vector3i{15, 30, 3}));
            CORRADE_COMPARE(image.size(), (Vector3i{3, 2, 2}));

            char pixels0[]{
                'a', 'b', 'c', 0,
                'd', 'e', 'f', 0,
            };
            char pixels1[]{
                '0', '1', '2', 0,
                '3', '4', '5', 0,
            };
            CORRADE_COMPARE_AS(image.pixels<Byte>()[0],
                (ImageView2D{PixelFormat::R8Snorm, {3, 2}, pixels0}),
                DebugTools::CompareImage);
            CORRADE_COMPARE_AS(image.pixels<Byte>()[1],
                (ImageView2D{PixelFormat::R8Snorm, {3, 2}, pixels1}),
                DebugTools::CompareImage);
        }

        bool called = false;
    } cache{PixelFormat::RGB16Unorm, {4, 3, 5}, PixelFormat::R8Snorm, {45, 35}};

    /* Capture correct function name */
    CORRADE_VERIFY(true);

    char pixels[]{
        'a', 'b', 'c', 0,
        'd', 'e', 'f', 0,
        '0', '1', '2', 0,
        '3', '4', '5', 0,
    };
    cache.setProcessedImage({15, 30, 3}, ImageView3D{PixelFormat::R8Snorm, {3, 2, 2}, pixels});
    CORRADE_VERIFY(cache.called);
}

void AbstractGlyphCacheTest::setProcessedImage2D() {
    /* Like setProcessedImage() but reduced to two dimensions */

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override {
            return GlyphCacheFeature::ImageProcessing;
        }
        void doSetProcessedImage(const Vector3i& offset, const ImageView3D& image) override {
            called = true;

            CORRADE_COMPARE(offset, (Vector3i{15, 30, 0}));
            CORRADE_COMPARE(image.size(), (Vector3i{3, 2, 1}));

            char pixels0[]{
                'a', 'b', 'c', 0,
                'd', 'e', 'f', 0,
            };
            CORRADE_COMPARE_AS(image.pixels<Byte>()[0],
                (ImageView2D{PixelFormat::R8Snorm, {3, 2}, pixels0}),
                DebugTools::CompareImage);
        }

        bool called = false;
    } cache{PixelFormat::RGB16Unorm, {4, 3}, PixelFormat::R8Snorm, {45, 35}};

    /* Capture correct function name */
    CORRADE_VERIFY(true);

    char pixels[]{
        'a', 'b', 'c', 0,
        'd', 'e', 'f', 0,
    };
    cache.setProcessedImage({15, 30}, ImageView2D{PixelFormat::R8Snorm, {3, 2}, pixels});
    CORRADE_VERIFY(cache.called);
}

void AbstractGlyphCacheTest::setProcessedImage2DPassthrough2D() {
    /* Like setProcessedImage2D() but with 2D doSetProcessedImage() */

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override {
            return GlyphCacheFeature::ImageProcessing;
        }
        void doSetProcessedImage(const Vector2i& offset, const ImageView2D& image) override {
            called = true;

            CORRADE_COMPARE(offset, (Vector2i{15, 30}));
            CORRADE_COMPARE(image.size(), (Vector2i{3, 2}));

            char pixels0[]{
                'a', 'b', 'c', 0,
                'd', 'e', 'f', 0,
            };
            CORRADE_COMPARE_AS(image.pixels<Byte>(),
                (ImageView2D{PixelFormat::R8Snorm, {3, 2}, pixels0}),
                DebugTools::CompareImage);
        }

        bool called = false;
    } cache{PixelFormat::RGB16Unorm, {4, 3}, PixelFormat::R8Snorm, {45, 35}};

    /* Capture correct function name */
    CORRADE_VERIFY(true);

    char pixels[]{
        'a', 'b', 'c', 0,
        'd', 'e', 'f', 0,
    };
    cache.setProcessedImage({15, 30}, ImageView2D{PixelFormat::R8Snorm, {3, 2}, pixels});
    CORRADE_VERIFY(cache.called);
}

void AbstractGlyphCacheTest::setProcessedImageNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override {
            return GlyphCacheFeature::ImageProcessing;
        }

        /* The 2D variant shouldn't be called on an array cache */
        void doSetProcessedImage(const Vector2i&, const ImageView2D&) override {
            CORRADE_FAIL("This should not be called");
        }
    } cache{PixelFormat::R32F, {1024, 512, 8}};

    Containers::String out;
    Error redirectError{&out};
    cache.setProcessedImage({}, ImageView3D{PixelFormat::R32F, {}});
    CORRADE_COMPARE(out, "Text::AbstractGlyphCache::setProcessedImage(): feature advertised but not implemented\n");
}

void AbstractGlyphCacheTest::setProcessedImagePassthrough2DNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override {
            return GlyphCacheFeature::ImageProcessing;
        }
    } cache{PixelFormat::R32F, {1024, 512}};

    Containers::String out;
    Error redirectError{&out};
    cache.setProcessedImage({}, ImageView3D{PixelFormat::R32F, {}});
    CORRADE_COMPARE(out, "Text::AbstractGlyphCache::setProcessedImage(): feature advertised but not implemented\n");
}

void AbstractGlyphCacheTest::setProcessedImageOutOfRange() {
    auto&& data = SetProcessedImageOutOfRangeData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    /* Like flushImage(), but for setProcessedImage() */

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override {
            return GlyphCacheFeature::ImageProcessing;
        }
    /* The source size and padding should not have any effect on the check */
    } cache{PixelFormat::RGBA32F, {1536, 768, 8}, PixelFormat::R8Snorm, {1024, 512}, data.padding};

    /* Large enough data to fit in all cases below, 4-byte aligned rows */
    Containers::Array<char> image{NoInit, 1012*5*2};

    Containers::String out;
    Error redirectError{&out};
    /* Negative min X, Y, layer */
    cache.setProcessedImage({-1, 30, 4},
        ImageView3D{PixelFormat::R8Snorm, {46, 5, 2}, image});
    cache.setProcessedImage({15, -1, 4},
        ImageView3D{PixelFormat::R8Snorm, {30, 36, 2}, image});
    cache.setProcessedImage({15, 30, -1},
        ImageView3D{PixelFormat::R8Snorm, {30, 5, 7}, image});
    /* Too large max X, Y, layer */
    cache.setProcessedImage({15, 30, 4},
        ImageView3D{PixelFormat::R8Snorm, {1010, 5, 2}});
    cache.setProcessedImage({15, 30, 4},
        ImageView3D{PixelFormat::R8Snorm, {30, 483, 2}});
    cache.setProcessedImage({15, 30, 4},
        ImageView3D{PixelFormat::R8Snorm, {30, 5, 5}});
    CORRADE_COMPARE_AS(out,
        "Text::AbstractGlyphCache::setProcessedImage(): {{-1, 30, 4}, {45, 35, 6}} out of range for size {1024, 512, 8}\n"
        "Text::AbstractGlyphCache::setProcessedImage(): {{15, -1, 4}, {45, 35, 6}} out of range for size {1024, 512, 8}\n"
        "Text::AbstractGlyphCache::setProcessedImage(): {{15, 30, -1}, {45, 35, 6}} out of range for size {1024, 512, 8}\n"

        "Text::AbstractGlyphCache::setProcessedImage(): {{15, 30, 4}, {1025, 35, 6}} out of range for size {1024, 512, 8}\n"
        "Text::AbstractGlyphCache::setProcessedImage(): {{15, 30, 4}, {45, 513, 6}} out of range for size {1024, 512, 8}\n"
        "Text::AbstractGlyphCache::setProcessedImage(): {{15, 30, 4}, {45, 35, 9}} out of range for size {1024, 512, 8}\n",
        TestSuite::Compare::String);
}

void AbstractGlyphCacheTest::setProcessedImageInvalidFormat() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override {
            return GlyphCacheFeature::ImageProcessing;
        }
    /* The source format should not have any effect on the check */
    } cache{PixelFormat::RGBA32F, {1024, 512, 8}, PixelFormat::R8Snorm, {3, 2}};

    Containers::String out;
    Error redirectError{&out};
    cache.setProcessedImage({}, ImageView3D{PixelFormat::R8Unorm, {3, 2, 1}, "abcdefgh"});
    CORRADE_COMPARE(out, "Text::AbstractGlyphCache::setProcessedImage(): expected PixelFormat::R8Snorm but got PixelFormat::R8Unorm\n");
}

void AbstractGlyphCacheTest::setProcessedImage2DNot2D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override {
            return GlyphCacheFeature::ImageProcessing;
        }
    } cache{PixelFormat::R8Unorm, {3, 2, 8}};

    Containers::String out;
    Error redirectError{&out};
    cache.setProcessedImage({}, ImageView2D{PixelFormat::R8Unorm, {3, 2}, "abcdefgh"});
    CORRADE_COMPARE(out, "Text::AbstractGlyphCache::setProcessedImage(): use the 3D overload for an array glyph cache\n");
}

void AbstractGlyphCacheTest::access() {
    /* Padding tested well enough in addGlyph(), resetting it back to 0 here */
    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512, 3}, {}};

    cache.setInvalidGlyph({5, 7}, 2, {{5, 10}, {15, 30}});

    UnsignedInt font9 = cache.addFont(9);
    UnsignedInt font3 = cache.addFont(3);
    UnsignedInt font9Glyph6 = cache.addGlyph(font9, 6, {3, 4}, 0, {{15, 30}, {45, 35}});
    UnsignedInt font3Glyph1 = cache.addGlyph(font3, 1, {5, 6}, 1, {{10, 15}, {25, 30}});
    UnsignedInt font9Glyph3 = cache.addGlyph(font9, 3, {6, 9}, 2, {{10, 5}, {25, 10}});
    CORRADE_COMPARE(font9Glyph6, 1);
    CORRADE_COMPARE(font3Glyph1, 2);
    CORRADE_COMPARE(font9Glyph3, 3);

    /* Mapping to the global glyph ID */
    CORRADE_COMPARE(cache.glyphId(font9, 6), font9Glyph6);
    CORRADE_COMPARE(cache.glyphId(font3, 1), font3Glyph1);
    CORRADE_COMPARE(cache.glyphId(font9, 3), font9Glyph3);

    /* Both overloads should return the same */
    CORRADE_COMPARE(cache.glyph(font9, 6), Containers::triple(
        Vector2i{3, 4},
        0,
        Range2Di{{15, 30}, {45, 35}}));
    CORRADE_COMPARE(cache.glyph(font9Glyph6), Containers::triple(
        Vector2i{3, 4},
        0,
        Range2Di{{15, 30}, {45, 35}}));

    CORRADE_COMPARE(cache.glyph(font3, 1), Containers::triple(
        Vector2i{5, 6},
        1,
        Range2Di{{10, 15}, {25, 30}}));
    CORRADE_COMPARE(cache.glyph(font3Glyph1), Containers::triple(
        Vector2i{5, 6},
        1,
        Range2Di{{10, 15}, {25, 30}}));

    CORRADE_COMPARE(cache.glyph(font9, 3), Containers::triple(
        Vector2i{6, 9},
        2,
        Range2Di{{10, 5}, {25, 10}}));
    CORRADE_COMPARE(cache.glyph(font9Glyph3), Containers::triple(
        Vector2i{6, 9},
        2,
        Range2Di{{10, 5}, {25, 10}}));

    /* Mapping to the invalid glyph ID if it hasn't been added yet */
    CORRADE_COMPARE(cache.glyphId(font9, 5), 0);
    CORRADE_COMPARE(cache.glyphId(font3, 2), 0);

    /* Querying glyphs that haven't been added yet gives back the invalid
       glyph properties */
    CORRADE_COMPARE(cache.glyph(font9, 5), Containers::triple(
        Vector2i{5, 7},
        2,
        Range2Di{{5, 10}, {15, 30}}));
    CORRADE_COMPARE(cache.glyph(font3, 2), Containers::triple(
        Vector2i{5, 7},
        2,
        Range2Di{{5, 10}, {15, 30}}));
    CORRADE_COMPARE(cache.glyph(0), Containers::triple(
        Vector2i{5, 7},
        2,
        Range2Di{{5, 10}, {15, 30}}));
}

void AbstractGlyphCacheTest::accessBatch() {
    /* Padding tested well enough in addGlyph(), resetting it back to 0 here */
    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512, 3}, {}};

    cache.setInvalidGlyph({5, 7}, 2, {{5, 10}, {15, 30}});

    UnsignedInt font9 = cache.addFont(9);
    UnsignedInt font3 = cache.addFont(3);
    UnsignedInt font9Glyph6 = cache.addGlyph(font9, 6, {3, 4}, 0, {{15, 30}, {45, 35}});
    UnsignedInt font3Glyph1 = cache.addGlyph(font3, 1, {5, 6}, 1, {{10, 15}, {25, 30}});
    UnsignedInt font9Glyph3 = cache.addGlyph(font9, 3, {6, 9}, 2, {{10, 5}, {25, 10}});
    CORRADE_COMPARE(font9Glyph6, 1);
    CORRADE_COMPARE(font3Glyph1, 2);
    CORRADE_COMPARE(font9Glyph3, 3);

    /* Direct data access */
    CORRADE_COMPARE_AS(cache.glyphOffsets(), Containers::arrayView<Vector2i>({
        {5, 7},
        {3, 4},
        {5, 6},
        {6, 9},
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(cache.glyphLayers(), Containers::arrayView({
        2,
        0,
        1,
        2
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(cache.glyphRectangles(), Containers::arrayView<Range2Di>({
        {{5, 10}, {15, 30}},
        {{15, 30}, {45, 35}},
        {{10, 15}, {25, 30}},
        {{10, 5}, {25, 10}}
    }), TestSuite::Compare::Container);

    /* Querying glyph IDs in a batch way. Invalid IDs are set to 0. */
    UnsignedInt glyphIds9[5];
    cache.glyphIdsInto(font9, {5, 6, 3, 6, 1}, glyphIds9);
    CORRADE_COMPARE_AS(Containers::arrayView(glyphIds9), Containers::arrayView({
        0u, 1u, 3u, 1u, 0u
    }), TestSuite::Compare::Container);

    UnsignedInt glyphIds3[3];
    cache.glyphIdsInto(font3, {2, 0, 1}, glyphIds3);
    CORRADE_COMPARE_AS(Containers::arrayView(glyphIds3), Containers::arrayView({
        0u, 0u, 2u
    }), TestSuite::Compare::Container);
}

void AbstractGlyphCacheTest::accessInvalid() {
    CORRADE_SKIP_IF_NO_DEBUG_ASSERT();

    /* Silly test name, but these all test debug asserts while
       accessBatchInvalid() tests non-debug asserts */

    /* Default padding of 1 makes it impossible to add a glyph at zero offset
       as it's out of range. Don't want to bother with that here so resetting
       it to 0. */
    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512, 3}, {}};

    cache.addFont(9);
    UnsignedInt fontId = cache.addFont(3);
    cache.addGlyph(0, 1, {}, 2, {});
    cache.addGlyph(fontId, 1, {}, 2, {});
    cache.addGlyph(fontId, 2, {}, 2, {});

    UnsignedInt fontGlyphIds[]{
        0, 0, cache.fontGlyphCount(fontId), 0
    };
    UnsignedInt glyphIds[4];

    Containers::String out;
    Error redirectError{&out};
    cache.glyph(cache.fontCount(), 0);
    cache.glyphId(cache.fontCount(), 0);
    cache.glyph(fontId, cache.fontGlyphCount(fontId));
    cache.glyphId(fontId, cache.fontGlyphCount(fontId));
    cache.glyphIdsInto(fontId, fontGlyphIds, glyphIds);
    cache.glyph(cache.glyphCount());
    CORRADE_COMPARE_AS(out,
        "Text::AbstractGlyphCache::glyph(): index 2 out of range for 2 fonts\n"
        "Text::AbstractGlyphCache::glyphId(): index 2 out of range for 2 fonts\n"
        "Text::AbstractGlyphCache::glyph(): index 3 out of range for 3 glyphs in font 1\n"
        "Text::AbstractGlyphCache::glyphId(): index 3 out of range for 3 glyphs in font 1\n"
        "Text::AbstractGlyphCache::glyphIdsInto(): glyph 2 index 3 out of range for 3 glyphs in font 1\n"
        "Text::AbstractGlyphCache::glyph(): index 4 out of range for 4 glyphs\n",
        TestSuite::Compare::String);
}

void AbstractGlyphCacheTest::accessBatchInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* Default padding of 1 makes it impossible to add a glyph at zero offset
       as it's out of range. Don't want to bother with that here so resetting
       it to 0. */
    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512, 3}, {}};

    cache.addFont(9);
    UnsignedInt fontId = cache.addFont(3);
    cache.addGlyph(fontId, 1, {}, 2, {});
    cache.addGlyph(fontId, 2, {}, 2, {});

    UnsignedInt fontGlyphIds[4];
    UnsignedInt glyphIds[4];
    UnsignedInt glyphIdsInvalid[3];

    Containers::String out;
    Error redirectError{&out};
    cache.glyphIdsInto(cache.fontCount(), fontGlyphIds, glyphIds);
    cache.glyphIdsInto(fontId, fontGlyphIds, glyphIdsInvalid);
    CORRADE_COMPARE(out,
        "Text::AbstractGlyphCache::glyphIdsInto(): index 2 out of range for 2 fonts\n"
        "Text::AbstractGlyphCache::glyphIdsInto(): expected fontGlyphIds and glyphIds views to have the same size but got 4 and 3\n");
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractGlyphCacheTest::accessDeprecated() {
    DummyGlyphCache cache{PixelFormat::R8Unorm, {100, 200}, {2, 3}};

    cache.setInvalidGlyph({3, 5}, {{10, 10}, {23, 45}});

    UnsignedInt fontId = cache.addFont(25);
    cache.addGlyph(fontId, 15, {3, 4}, {{15, 30}, {45, 35}});
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(cache[15], std::make_pair(
        Vector2i{1, 1},
        Range2Di{{13, 27}, {47, 38}}
    ));
    CORRADE_IGNORE_DEPRECATED_POP

    /* ID 0 gets the invalid glyph */
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(cache[0], std::make_pair(
        Vector2i{1, 2},
        Range2Di{{8, 7}, {25, 48}}
    ));
    CORRADE_IGNORE_DEPRECATED_POP

    /* Glyph IDs out of bounds get the invalid glyph too */
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(cache[45], std::make_pair(
        Vector2i{1, 2},
        Range2Di{{8, 7}, {25, 48}}
    ));
    CORRADE_IGNORE_DEPRECATED_POP
}

void AbstractGlyphCacheTest::accessDeprecatedNot2D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    DummyGlyphCache cache{PixelFormat::R32F, {1024, 512, 3}};

    Containers::String out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    cache[5];
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out, "Text::AbstractGlyphCache::operator[](): can't be used on an array glyph cache\n");
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::AbstractGlyphCacheTest)
