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

/* In order to have the CORRADE_PLUGIN_REGISTER() macro not a no-op. Doesn't
   affect anything else. */
#define CORRADE_STATIC_PLUGIN

#include <string>
#include <unordered_map>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/Containers/StringStl.h> /** @todo remove once file callbacks are <string>-free */
#include <Corrade/Containers/Triple.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/Path.h>
#include <Corrade/Utility/Resource.h>

#include "Magnum/FileCallback.h"
#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Range.h"
#include "Magnum/Text/Alignment.h"
#include "Magnum/Text/AbstractFont.h"
#include "Magnum/Text/AbstractFontConverter.h"
#include "Magnum/Text/AbstractGlyphCache.h"
#include "Magnum/Text/AbstractShaper.h"
#include "Magnum/Text/Direction.h"
#include "Magnum/Text/Feature.h"
#include "Magnum/Text/Renderer.h"
#include "Magnum/Text/Script.h"
#include "Magnum/TextureTools/Atlas.h"

#define DOXYGEN_ELLIPSIS(...) __VA_ARGS__
#define DOXYGEN_IGNORE(...) __VA_ARGS__

using namespace Magnum;
using namespace Magnum::Math::Literals;

namespace {
    Vector2i windowSize() { return {}; }
    Vector2i framebufferSize() { return {}; }
    Vector2 dpiScaling() { return {}; }
}

namespace MyNamespace {

struct MyFont: Text::AbstractFont {
    explicit MyFont(PluginManager::AbstractManager& manager, Containers::StringView plugin): Text::AbstractFont{manager, plugin} {}

    Text::FontFeatures doFeatures() const override { return {}; }
    bool doIsOpened() const override { return false; }
    void doClose() override {}
    void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
    Vector2 doGlyphSize(UnsignedInt) override { return {}; }
    Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
    Containers::Pointer<Text::AbstractShaper> doCreateShaper() override { return nullptr; }
};
struct MyFontConverter: Text::AbstractFontConverter {
    explicit MyFontConverter(PluginManager::AbstractManager& manager, Containers::StringView plugin): Text::AbstractFontConverter{manager, plugin} {}

    Text::FontConverterFeatures doFeatures() const override { return {}; }
};

}

/* [MAGNUM_TEXT_ABSTRACTFONT_PLUGIN_INTERFACE] */
CORRADE_PLUGIN_REGISTER(MyFont, MyNamespace::MyFont,
    MAGNUM_TEXT_ABSTRACTFONT_PLUGIN_INTERFACE)
/* [MAGNUM_TEXT_ABSTRACTFONT_PLUGIN_INTERFACE] */

/* [MAGNUM_TEXT_ABSTRACTFONTCONVERTER_PLUGIN_INTERFACE] */
CORRADE_PLUGIN_REGISTER(MyFontConverter, MyNamespace::MyFontConverter,
    MAGNUM_TEXT_ABSTRACTFONTCONVERTER_PLUGIN_INTERFACE)
/* [MAGNUM_TEXT_ABSTRACTFONTCONVERTER_PLUGIN_INTERFACE] */

/* Make sure the name doesn't conflict with any other snippets to avoid linker
   warnings, unlike with `int main()` there now has to be a declaration to
   avoid -Wmisssing-prototypes */
void mainText();
void mainText() {
{
PluginManager::Manager<Text::AbstractFont> manager;
Containers::Pointer<Text::AbstractFont> font =
    manager.loadAndInstantiate("StbTrueTypeFont");
struct: Text::AbstractGlyphCache {
    using Text::AbstractGlyphCache::AbstractGlyphCache;

    Text::GlyphCacheFeatures doFeatures() const override { return {}; }
} cache{PixelFormat::R8Unorm, Vector2i{256}};
/* [AbstractFont-glyph-cache-by-id] */
CORRADE_INTERNAL_ASSERT_OUTPUT(font->fillGlyphCache(cache, {
    font->glyphForName("fi"),
    font->glyphForName("f_f"),
    font->glyphForName("fl"),
    DOXYGEN_ELLIPSIS()
}));
/* [AbstractFont-glyph-cache-by-id] */

/* [AbstractFont-glyph-cache-all] */
Containers::Array<UnsignedInt> glyphs{NoInit, font->glyphCount()};
for(UnsignedInt i = 0; i != font->glyphCount(); ++i)
    glyphs[i] = i;

CORRADE_INTERNAL_ASSERT_OUTPUT(font->fillGlyphCache(cache, glyphs));
/* [AbstractFont-glyph-cache-all] */
}

{
PluginManager::Manager<Text::AbstractFont> manager;
Containers::Pointer<Text::AbstractFont> font =
    manager.loadAndInstantiate("StbTrueTypeFont");
/* [AbstractFont-usage-data] */
Utility::Resource rs{"data"};
Containers::ArrayView<const char> data = rs.getRaw("font.ttf");
if(!font->openData(data, 12.0f))
    Fatal{} << "Can't open font data with StbTrueTypeFont";
/* [AbstractFont-usage-data] */
}

#if defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT))
{
/* -Wnonnull in GCC 11+  "helpfully" says "this is null" if I don't initialize
   the font pointer. I don't care, I just want you to check compilation errors,
   not more! */
PluginManager::Manager<Text::AbstractFont> manager;
Containers::Pointer<Text::AbstractFont> font = manager.loadAndInstantiate("SomethingWhatever");
/* [AbstractFont-usage-callbacks] */
struct Data {
    std::unordered_map<std::string, Containers::Optional<
        Containers::Array<const char, Utility::Path::MapDeleter>>> files;
} data;

font->setFileCallback([](const std::string& filename,
    InputFileCallbackPolicy policy, Data& data)
        -> Containers::Optional<Containers::ArrayView<const char>>
    {
        auto found = data.files.find(filename);

        /* Discard the memory mapping, if not needed anymore */
        if(policy == InputFileCallbackPolicy::Close) {
            if(found != data.files.end()) data.files.erase(found);
            return {};
        }

        /* Load if not there yet. If the mapping fails, remember that to not
           attempt to load the same file again next time. */
        if(found == data.files.end()) found = data.files.emplace(
            filename, Utility::Path::mapRead(filename)).first;

        if(!found->second) return {};
        return Containers::arrayView(*found->second);
    }, data);

font->openFile("magnum-font.conf", 13.0f);
/* [AbstractFont-usage-callbacks] */
}
#endif

{
/* -Wnonnull in GCC 11+  "helpfully" says "this is null" if I don't initialize
   the font pointer. I don't care, I just want you to check compilation errors,
   not more! */
PluginManager::Manager<Text::AbstractFont> manager;
Containers::Pointer<Text::AbstractFont> font = manager.loadAndInstantiate("SomethingWhatever");
/* [AbstractFont-setFileCallback] */
font->setFileCallback([](const std::string& filename,
    InputFileCallbackPolicy, void*) {
        Utility::Resource rs{"data"};
        return Containers::optional(rs.getRaw(filename));
    });
/* [AbstractFont-setFileCallback] */
}

{
/* -Wnonnull in GCC 11+  "helpfully" says "this is null" if I don't initialize
   the font pointer. I don't care, I just want you to check compilation errors,
   not more! */
PluginManager::Manager<Text::AbstractFont> manager;
Containers::Pointer<Text::AbstractFont> font = manager.loadAndInstantiate("SomethingWhatever");
/* [AbstractFont-setFileCallback-template] */
const Utility::Resource rs{"data"};
font->setFileCallback([](const std::string& filename,
    InputFileCallbackPolicy, const Utility::Resource& rs) {
        return Containers::optional(rs.getRaw(filename));
    }, rs);
/* [AbstractFont-setFileCallback-template] */
}

{
/* -Wnonnull in GCC 11+  "helpfully" says "this is null" if I don't initialize
   the font pointer. I don't care, I just want you to check compilation errors,
   not more! */
PluginManager::Manager<Text::AbstractFont> manager;
struct: Text::AbstractGlyphCache {
    using Text::AbstractGlyphCache::AbstractGlyphCache;

    Text::GlyphCacheFeatures doFeatures() const override { return {}; }
} cache{PixelFormat::R8Unorm, Vector2i{256}};
/* [AbstractGlyphCache-usage-fill] */
Containers::Pointer<Text::AbstractFont> font = DOXYGEN_ELLIPSIS(manager.loadAndInstantiate(""));

if(!font->fillGlyphCache(cache, "abcdefghijklmnopqrstuvwxyz"
                                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                "0123456789?!:;,. "))
    Fatal{} << "Glyph cache too small to fit all characters";
/* [AbstractGlyphCache-usage-fill] */
}

{
struct: Text::AbstractGlyphCache {
    using Text::AbstractGlyphCache::AbstractGlyphCache;

    Text::GlyphCacheFeatures doFeatures() const override { return {}; }
} cache{PixelFormat::R8Unorm, Vector2i{256}};
/* [AbstractGlyphCache-filling-images] */
Containers::Array<Image2D> images = DOXYGEN_ELLIPSIS({}); /* or ImageView2D, Trade::ImageData2D... */
/* [AbstractGlyphCache-filling-images] */

/* [AbstractGlyphCache-filling-font] */
UnsignedInt fontId = cache.addFont(images.size());
/* [AbstractGlyphCache-filling-font] */

/* [AbstractGlyphCache-filling-atlas] */
Containers::Array<Vector2i> offsets{NoInit, images.size()};

cache.atlas().clearFlags(
    TextureTools::AtlasLandfillFlag::RotatePortrait|
    TextureTools::AtlasLandfillFlag::RotateLandscape);
Containers::Optional<Range2Di> range = cache.atlas().add(
    stridedArrayView(images).slice(&Image2D::size),
    offsets);
CORRADE_INTERNAL_ASSERT(range);
/* [AbstractGlyphCache-filling-atlas] */

/* [AbstractGlyphCache-filling-glyphs] */
/* The glyph cache is just 2D, so copying to the first slice */
Containers::StridedArrayView3D<char> dst = cache.image().pixels()[0];
for(UnsignedInt i = 0; i != images.size(); ++i) {
    Range2Di rectangle = Range2Di::fromSize(offsets[i], images[i].size());
    cache.addGlyph(fontId, i, {}, rectangle);

    /* Copy assuming all input images have the same pixel format */
    Containers::StridedArrayView3D<const char> src = images[i].pixels();
    Utility::copy(src, dst.sliceSize({
        std::size_t(offsets[i].y()),
        std::size_t(offsets[i].x()),
        0}, src.size()));
}

/* Reflect the updated image range to the actual GPU-side texture */
cache.flushImage(*range);
/* [AbstractGlyphCache-filling-glyphs] */
}

{
struct: Text::AbstractGlyphCache {
    using Text::AbstractGlyphCache::AbstractGlyphCache;

    Text::GlyphCacheFeatures doFeatures() const override { return {}; }
} cacheInstance{PixelFormat::R8Unorm, Vector2i{256}};
/* [AbstractGlyphCache-querying] */
Containers::Pointer<Text::AbstractFont> font = DOXYGEN_ELLIPSIS({});
Text::AbstractGlyphCache& cache = DOXYGEN_ELLIPSIS(cacheInstance);

Containers::ArrayView<const UnsignedInt> fontGlyphIds = DOXYGEN_ELLIPSIS({});

Containers::Optional<UnsignedInt> fontId = cache.findFont(*font);
DOXYGEN_ELLIPSIS()
for(std::size_t i = 0; i != fontGlyphIds.size(); ++i) {
    Containers::Triple<Vector2i, Int, Range2Di> glyph =
        cache.glyph(*fontId, fontGlyphIds[i]);
    DOXYGEN_ELLIPSIS(static_cast<void>(glyph);)
}
/* [AbstractGlyphCache-querying] */

/* [AbstractGlyphCache-querying-batch] */
Containers::Array<UnsignedInt> glyphIds{NoInit, fontGlyphIds.size()};
cache.glyphIdsInto(*fontId, fontGlyphIds, glyphIds);

Containers::StridedArrayView1D<const Vector2i> offsets = cache.glyphOffsets();
Containers::StridedArrayView1D<const Range2Di> rects = cache.glyphRectangles();
for(std::size_t i = 0; i != fontGlyphIds.size(); ++i) {
    Vector2i offset = offsets[glyphIds[i]];
    Range2Di rectangle = rects[glyphIds[i]];
    DOXYGEN_ELLIPSIS(static_cast<void>(offset); static_cast<void>(rectangle);)
}
/* [AbstractGlyphCache-querying-batch] */
}

{
/* -Wnonnull in GCC 11+  "helpfully" says "this is null" if I don't initialize
   the font pointer. I don't care, I just want you to check compilation errors,
   not more! */
PluginManager::Manager<Text::AbstractFont> manager;
/* [AbstractShaper-shape] */
Containers::Pointer<Text::AbstractFont> font = DOXYGEN_ELLIPSIS(manager.loadAndInstantiate("SomethingWhatever"));
Containers::Pointer<Text::AbstractShaper> shaper = font->createShaper();

/* Shape a piece of text */
shaper->shape("Hello, world!");

/* Get the glyph info back */
struct GlyphInfo {
    UnsignedInt id;
    Vector2 offset;
    Vector2 advance;
};
Containers::Array<GlyphInfo> glyphs{NoInit, shaper->glyphCount()};
shaper->glyphIdsInto(
    stridedArrayView(glyphs).slice(&GlyphInfo::id));
shaper->glyphOffsetsAdvancesInto(
    stridedArrayView(glyphs).slice(&GlyphInfo::offset),
    stridedArrayView(glyphs).slice(&GlyphInfo::advance));
/* [AbstractShaper-shape] */

/* [AbstractShaper-shape-properties] */
shaper->setScript(Text::Script::Latin);
shaper->setDirection(Text::ShapeDirection::LeftToRight);
shaper->setLanguage("en");
shaper->shape("Hello, world!");
/* [AbstractShaper-shape-properties] */
}

{
/* -Wnonnull in GCC 11+  "helpfully" says "this is null" if I don't initialize
   the font pointer. I don't care, I just want you to check compilation errors,
   not more! */
PluginManager::Manager<Text::AbstractFont> manager;
Containers::Pointer<Text::AbstractFont> font = manager.loadAndInstantiate("SomethingWhatever");
Containers::Pointer<Text::AbstractShaper> shaper = font->createShaper();
/* [AbstractShaper-shape-features] */
shaper->shape("Hello, world!", {
    {Text::Feature::SmallCapitals, 7, 12}
});
/* [AbstractShaper-shape-features] */
}

{
struct GlyphInfo {
    UnsignedInt id;
    Vector2 offset;
    Vector2 advance;
};
/* -Wnonnull in GCC 11+  "helpfully" says "this is null" if I don't initialize
   the font pointer. I don't care, I just want you to check compilation errors,
   not more! */
PluginManager::Manager<Text::AbstractFont> manager;
/* [AbstractShaper-shape-multiple] */
Containers::Pointer<Text::AbstractFont> font = DOXYGEN_ELLIPSIS(manager.loadAndInstantiate("SomethingWhatever"));
Containers::Pointer<Text::AbstractFont> boldFont = DOXYGEN_ELLIPSIS(manager.loadAndInstantiate("SomethingWhatever"));
Containers::Pointer<Text::AbstractShaper> shaper = font->createShaper();
Containers::Pointer<Text::AbstractShaper> boldShaper = boldFont->createShaper();
DOXYGEN_ELLIPSIS()

Containers::StringView text = "Hello, world!";
Containers::Array<GlyphInfo> glyphs;

/* Shape "Hello, " with a regular font */
shaper->shape(text, 0, 7);
Containers::StridedArrayView1D<GlyphInfo> glyphs1 =
    arrayAppend(glyphs, NoInit, shaper->glyphCount());
shaper->glyphIdsInto(
    glyphs1.slice(&GlyphInfo::id));
shaper->glyphOffsetsAdvancesInto(
    glyphs1.slice(&GlyphInfo::offset),
    glyphs1.slice(&GlyphInfo::advance));

/* Append "world" shaped with a bold font */
boldShaper->shape(text, 7, 12);
Containers::StridedArrayView1D<GlyphInfo> glyphs2 =
    arrayAppend(glyphs, NoInit, boldShaper->glyphCount());
shaper->glyphIdsInto(
    glyphs2.slice(&GlyphInfo::id));
shaper->glyphOffsetsAdvancesInto(
    glyphs2.slice(&GlyphInfo::offset),
    glyphs2.slice(&GlyphInfo::advance));

/* Finally shape "!" with a regular font again */
shaper->shape(text, 12, 13);
Containers::StridedArrayView1D<GlyphInfo> glyphs3 =
    arrayAppend(glyphs, NoInit, shaper->glyphCount());
shaper->glyphIdsInto(
    glyphs3.slice(&GlyphInfo::id));
shaper->glyphOffsetsAdvancesInto(
    glyphs3.slice(&GlyphInfo::offset),
    glyphs3.slice(&GlyphInfo::advance));
/* [AbstractShaper-shape-multiple] */
}

{
/* -Wnonnull in GCC 11+  "helpfully" says "this is null" if I don't initialize
   the font pointer. I don't care, I just want you to check compilation errors,
   not more! */
PluginManager::Manager<Text::AbstractFont> manager;
Containers::Pointer<Text::AbstractFont> font = manager.loadAndInstantiate("SomethingWhatever");
Containers::Pointer<Text::AbstractShaper> shaper = font->createShaper();
/* [AbstractShaper-shape-clusters-to-bytes] */
Containers::StringView text = DOXYGEN_ELLIPSIS({});

shaper->shape(text);
DOXYGEN_ELLIPSIS()

Containers::Array<UnsignedInt> clusters{NoInit, shaper->glyphCount()};
shaper->glyphClustersInto(clusters);

Containers::StringView selection = text.slice(clusters[2], clusters[5]);
/* [AbstractShaper-shape-clusters-to-bytes] */

/* [AbstractShaper-shape-bytes-to-clusters] */
Containers::Pair<UnsignedInt, UnsignedInt> selectionGlyphs =
    Text::glyphRangeForBytes(clusters, selection.begin() - text.begin(),
                                       selection.end() - text.begin());
/* [AbstractShaper-shape-bytes-to-clusters] */
static_cast<void>(selectionGlyphs);
}

{
struct: Text::AbstractGlyphCache {
    using Text::AbstractGlyphCache::AbstractGlyphCache;

    Text::GlyphCacheFeatures doFeatures() const override { return {}; }
} cache{PixelFormat::R8Unorm, Vector2i{256}};
PluginManager::Manager<Text::AbstractFont> manager;
Containers::Pointer<Text::AbstractFont> font = manager.loadAndInstantiate("");
Containers::Pointer<Text::AbstractShaper> shaperPointer = font->createShaper();
Text::AbstractShaper& shaper = *shaperPointer;
Float size{};
/* [RendererCore-usage] */
Text::RendererCore renderer{cache};

renderer.render(shaper, size, "Hello, world!");
/* [RendererCore-usage] */

/* [RendererCore-usage-quads] */
Range1Dui runs = renderer.render(DOXYGEN_ELLIPSIS(shaper, size, "Hello, world!")).second();

struct Vertex {
    Vector2 position;
    Vector2 textureCoordinates; /* or Vector3 for an array glyph cache */
};
Containers::Array<Vertex> vertices;
for(UnsignedInt run = runs.min(); run != runs.max(); ++run) {
    Range1Dui glyphs = renderer.glyphsForRuns({run, run + 1});
    Containers::StridedArrayView1D<Vertex> runVertices =
        arrayAppend(vertices, NoInit, glyphs.size());
    Text::renderGlyphQuadsInto(renderer.glyphCache(),
        renderer.runScales()[run],
        renderer.glyphPositions().slice(glyphs.min(), glyphs.max()),
        renderer.glyphIds().slice(glyphs.min(), glyphs.max()),
        runVertices.slice(&Vertex::position),
        runVertices.slice(&Vertex::textureCoordinates));
}
/* [RendererCore-usage-quads] */
}

{
struct: Text::AbstractGlyphCache {
    using Text::AbstractGlyphCache::AbstractGlyphCache;

    Text::GlyphCacheFeatures doFeatures() const override { return {}; }
} cache{PixelFormat::R8Unorm, Vector2i{256}};
/* [RendererCore-allocators-static] */
struct Glyph {
    Vector2 position;
    UnsignedInt id;
    Vector2 advance;
} glyphs[256];
struct Run {
    Float scale;
    UnsignedInt end;
} runs[16];

Text::RendererCore renderer{cache,
    [](void* state, UnsignedInt glyphCount,
       Containers::StridedArrayView1D<Vector2>& glyphPositions,
       Containers::StridedArrayView1D<UnsignedInt>& glyphIds,
       Containers::StridedArrayView1D<UnsignedInt>*,
       Containers::StridedArrayView1D<Vector2>& glyphAdvances
    ) {
        Containers::ArrayView<Glyph> glyphs = *static_cast<Glyph(*)[256]>(state);
        CORRADE_INTERNAL_ASSERT(glyphCount <= glyphs.size());DOXYGEN_IGNORE(static_cast<void>(glyphCount));
        glyphPositions = stridedArrayView(glyphs).slice(&Glyph::position);
        glyphIds = stridedArrayView(glyphs).slice(&Glyph::id);
        glyphAdvances = stridedArrayView(glyphs).slice(&Glyph::advance);
    }, glyphs,
    [](void* state, UnsignedInt runCount,
       Containers::StridedArrayView1D<Float>& runScales,
       Containers::StridedArrayView1D<UnsignedInt>& runEnds
    ) {
        Containers::ArrayView<Run> runs = *static_cast<Run(*)[16]>(state);
        CORRADE_INTERNAL_ASSERT(runCount <= runs.size());DOXYGEN_IGNORE(static_cast<void>(runCount));
        runScales = Containers::stridedArrayView(runs).slice(&Run::scale);
        runEnds = Containers::stridedArrayView(runs).slice(&Run::end);
    }, runs
};
/* [RendererCore-allocators-static] */
}

{
struct: Text::AbstractGlyphCache {
    using Text::AbstractGlyphCache::AbstractGlyphCache;

    Text::GlyphCacheFeatures doFeatures() const override { return {}; }
} cache{PixelFormat::R8Unorm, Vector2i{256}};
struct Glyph {
    Vector2 position;
    UnsignedInt id;
    Vector2 advance;
};
PluginManager::Manager<Text::AbstractFont> manager;
Containers::Pointer<Text::AbstractFont> font = manager.loadAndInstantiate("");
Containers::Pointer<Text::AbstractShaper> shaperPointer = font->createShaper();
Text::AbstractShaper& shaper = *shaperPointer;
Float size{};
/* [RendererCore-allocators-redirect] */
struct Allocation {
    UnsignedInt current = 0;
    /* Using just a fixed set of texts for brevity */
    Containers::Array<Glyph> texts[5];
} allocation;

Text::RendererCore renderer{cache,
    [](void* state, UnsignedInt glyphCount,
       Containers::StridedArrayView1D<Vector2>& glyphPositions,
       Containers::StridedArrayView1D<UnsignedInt>& glyphIds,
       Containers::StridedArrayView1D<UnsignedInt>*,
       Containers::StridedArrayView1D<Vector2>& glyphAdvances
    ) {
        auto& allocation = *static_cast<Allocation*>(state);
        Containers::Array<Glyph>& glyphs = allocation.texts[allocation.current];
        if(glyphCount > glyphs.size())
            arrayResize(glyphs, glyphCount);

        glyphPositions = stridedArrayView(glyphs).slice(&Glyph::position);
        glyphIds = stridedArrayView(glyphs).slice(&Glyph::id);
        glyphAdvances = stridedArrayView(glyphs).slice(&Glyph::advance);
    }, &allocation,
    /* Text runs use the renderer's default allocator */
    nullptr, nullptr
};

DOXYGEN_ELLIPSIS()

/* Updating text 3 */
allocation.current = 3;
renderer
    .clear()
    .render(shaper, size, "Hello, world!");

/* Updating text 1 */
allocation.current = 1;
renderer
    .clear()
    .render(shaper, size, "This doesn't replace text 3!");
/* [RendererCore-allocators-redirect] */
}

{
struct: Text::AbstractGlyphCache {
    using Text::AbstractGlyphCache::AbstractGlyphCache;

    Text::GlyphCacheFeatures doFeatures() const override { return {}; }
} cache{PixelFormat::R8Unorm, Vector2i{256}};
PluginManager::Manager<Text::AbstractFont> manager;
/* [Renderer-usage-fill] */
Containers::Pointer<Text::AbstractFont> font = DOXYGEN_ELLIPSIS(manager.loadAndInstantiate(""));

if(!font->fillGlyphCache(cache, "abcdefghijklmnopqrstuvwxyz"
                                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                "0123456789?!:;,. "))
    Fatal{} << "Glyph cache too small to fit all characters";
/* [Renderer-usage-fill] */

Text::Renderer renderer{cache};
/* [Renderer-usage-render] */
renderer.render(*font->createShaper(), font->size(), "Hello, world!");
/* [Renderer-usage-render] */

/* [Renderer-usage-layout-options] */
renderer
    .setCursor({+windowSize().x()*0.5f - 10.0f,
                -windowSize().y()*0.5f + 10.0f})
    .setAlignment(Text::Alignment::BottomRight)
    .render(*font->createShaper(), font->size(), "Hello,\nworld!");
/* [Renderer-usage-layout-options] */

/* [Renderer-usage-shape-properties] */
Containers::Pointer<Text::AbstractShaper> shaper = font->createShaper();
shaper->setScript(Text::Script::Latin);
shaper->setLanguage("en");
shaper->setDirection(Text::ShapeDirection::LeftToRight);

renderer.render(*shaper, shaper->font().size(), "Hello, world!");
/* [Renderer-usage-shape-properties] */

/* [Renderer-usage-shape-features] */
renderer.render(*shaper, shaper->font().size(), "Hello, world!", {
    {Text::Feature::SmallCapitals, 7, 12}
});
/* [Renderer-usage-shape-features] */

{
Containers::Pointer<Text::AbstractShaper> shaper = font->createShaper();
/* [Renderer-usage-blocks] */
renderer
    .setCursor({-windowSize().x()*0.5f + 10.0f,
                -windowSize().y()*0.5f + 10.0f})
    .setAlignment(Text::Alignment::BottomLeft)
    .render(*shaper, shaper->font().size(), "Hello,");

renderer
    .setCursor({+windowSize().x()*0.5f - 10.0f,
                -windowSize().y()*0.5f + 10.0f})
    .setAlignment(Text::Alignment::BottomRight)
    .render(*shaper, shaper->font().size(), "world!");
/* [Renderer-usage-blocks] */
}

{
Containers::Pointer<Text::AbstractFont> boldFont = DOXYGEN_ELLIPSIS(manager.loadAndInstantiate(""));
/* [Renderer-usage-runs] */
Containers::Pointer<Text::AbstractShaper> shaper = font->createShaper();
Containers::Pointer<Text::AbstractShaper> boldShaper = boldFont->createShaper();
DOXYGEN_ELLIPSIS()

renderer
    .add(*shaper, shaper->font().size(), "Hello, ")
    .add(*boldShaper, boldShaper->font().size(), "world")
    .add(*shaper, shaper->font().size(), "!")
    .render();
/* [Renderer-usage-runs] */

/* [Renderer-usage-runs-begin-end] */
Containers::StringView text = "Hello, world!";

renderer
    .add(*shaper, shaper->font().size(), text, 0, 7)
    .add(*boldShaper, boldShaper->font().size(), text, 7, 12)
    .add(*shaper, shaper->font().size(), text, 12, 13)
    .render();
/* [Renderer-usage-runs-begin-end] */
}
}

{
PluginManager::Manager<Text::AbstractFont> manager;
Float size{};
struct: Text::AbstractGlyphCache {
    using Text::AbstractGlyphCache::AbstractGlyphCache;

    Text::GlyphCacheFeatures doFeatures() const override { return {}; }
} cache{PixelFormat::R8Unorm, Vector2i{256}};
Text::Renderer renderer{cache};
/* [Renderer-dpi-supersampling] */
Containers::Pointer<Text::AbstractFont> font = DOXYGEN_ELLIPSIS(manager.loadAndInstantiate(""));
if(!font->openFile("font.ttf", size*2.0f)) /* Supersample 2x */
    DOXYGEN_ELLIPSIS({})

DOXYGEN_ELLIPSIS()
renderer.render(*font->createShaper(), size, DOXYGEN_ELLIPSIS(""));
/* [Renderer-dpi-supersampling] */
}

{
/* [Renderer-dpi-interface-size] */
Vector2 interfaceSize = Vector2{windowSize()}/dpiScaling();
/* [Renderer-dpi-interface-size] */
/* [Renderer-dpi-size-multiplier] */
Float sizeMultiplier =
    (Vector2{framebufferSize()}*dpiScaling()/Vector2{windowSize()}).max();
/* [Renderer-dpi-size-multiplier] */
static_cast<void>(interfaceSize);
static_cast<void>(sizeMultiplier);
}

{
struct: Text::AbstractGlyphCache {
    using Text::AbstractGlyphCache::AbstractGlyphCache;

    Text::GlyphCacheFeatures doFeatures() const override { return {}; }
} cache{PixelFormat::R8Unorm, Vector2i{256}};
PluginManager::Manager<Text::AbstractFont> manager;
Containers::Pointer<Text::AbstractFont> font = manager.loadAndInstantiate("");
Containers::StringView text;
/* [Renderer-clusters] */
Text::Renderer renderer{cache, Text::RendererFlag::GlyphPositionsClusters};

Range1Dui runs = renderer.render(DOXYGEN_ELLIPSIS(*font->createShaper(), 0.0f), text).second();
Range1Dui glyphs = renderer.glyphsForRuns(runs);
Containers::StridedArrayView1D<const UnsignedInt> clusters =
    renderer.glyphClusters().slice(glyphs.min(), glyphs.max());

/* Input text corresponding to glyphs 2 to 5 */
Containers::StringView selection = text.slice(clusters[2], clusters[5]);

/* Or glyphs corresponding to a concrete text selection */
Containers::Pair<UnsignedInt, UnsignedInt> selectionGlyphs =
    Text::glyphRangeForBytes(clusters, selection.begin() - text.begin(),
                                       selection.end() - text.begin());
/* [Renderer-clusters] */
static_cast<void>(selectionGlyphs);
}

{
struct: Text::AbstractGlyphCache {
    using Text::AbstractGlyphCache::AbstractGlyphCache;

    Text::GlyphCacheFeatures doFeatures() const override { return {}; }
} cache{PixelFormat::R8Unorm, Vector2i{256}};
/* [Renderer-allocators-vertex] */
struct Vertex {
    Vector2 position;
    Vector2 textureCoordinates;
    Color4 color;
};
Containers::Array<Vertex> vertices;

Text::Renderer renderer{cache,
    /* Glyphs, runs and indices use renderer's default allocators */
    nullptr, nullptr,
    nullptr, nullptr,
    nullptr, nullptr,
    [](void* state, UnsignedInt vertexCount,
       Containers::StridedArrayView1D<Vector2>& vertexPositions,
       Containers::StridedArrayView1D<Vector2>& vertexTextureCoordinates
    ) {
        auto& vertices = *static_cast<Containers::Array<Vertex>*>(state);
        if(vertexCount > vertices.size())
            arrayResize(vertices, vertexCount);

        vertexPositions = stridedArrayView(vertices).slice(&Vertex::position);
        vertexTextureCoordinates =
            stridedArrayView(vertices).slice(&Vertex::textureCoordinates);
    }, &vertices
};

/* Render a text and fill vertex colors. Each glyph quad is four vertices. */
Range1Dui runs = renderer.render(DOXYGEN_ELLIPSIS()).second();
Range1Dui glyphs = renderer.glyphsForRuns(runs);
for(Vertex& vertex: vertices.slice(glyphs.min()*4, glyphs.max()*4))
    vertex.color = 0x3bd267_rgbf;
/* [Renderer-allocators-vertex] */
}

{
struct: Text::AbstractGlyphCache {
    using Text::AbstractGlyphCache::AbstractGlyphCache;

    Text::GlyphCacheFeatures doFeatures() const override { return {}; }
} cache{PixelFormat::R8Unorm, Vector2i{256}};
/* [Renderer-allocators-index] */
/* A 2-byte index type can index at most 65k vertices, which is enough for 16k
   glyph quads, and each glyph quad needs six indices */
char indices[2*16384*6];

Text::Renderer renderer{cache,
    nullptr, nullptr,
    nullptr, nullptr,
    [](void* state, UnsignedInt size, Containers::ArrayView<char>& indices) {
        indices = *static_cast<char(*)[2*16384*6]>(state);
        CORRADE_INTERNAL_ASSERT(size <= indices.size());DOXYGEN_IGNORE(static_cast<void>(size));
    }, indices,
    nullptr, nullptr
};
/* [Renderer-allocators-index] */
}
}
