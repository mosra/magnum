/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#include "MagnumFont.h"

#include <sstream>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Utility/Configuration.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/Utility/Unicode.h>

#include "Magnum/ImageView.h"
#include "Magnum/Math/ConfigurationValue.h"
#include "Magnum/Text/GlyphCache.h"
#include "Magnum/Trade/ImageData.h"
#include "MagnumPlugins/TgaImporter/TgaImporter.h"

namespace Magnum { namespace Text {

struct MagnumFont::Data {
    /* Otherwise Clang complains about Utility::Configuration having explicit
       constructor when emplace()ing the Pointer. Using = default works on
       newer Clang but not older versions. */
    explicit Data() {}

    Utility::Configuration conf;
    Containers::Optional<Trade::ImageData2D> image;
    Containers::Optional<std::string> filePath;
    std::unordered_map<char32_t, UnsignedInt> glyphId;
    std::vector<Vector2> glyphAdvance;
};

namespace {
    class MagnumFontLayouter: public AbstractLayouter {
        public:
            explicit MagnumFontLayouter(const std::vector<Vector2>& glyphAdvance, const AbstractGlyphCache& cache, Float fontSize, Float textSize, std::vector<UnsignedInt>&& glyphs);

        private:
            std::tuple<Range2D, Range2D, Vector2> doRenderGlyph(UnsignedInt i) override;

            const std::vector<Vector2>& glyphAdvance;
            const AbstractGlyphCache& cache;
            const Float fontSize, textSize;
            const std::vector<UnsignedInt> glyphs;
    };
}

MagnumFont::MagnumFont(): _opened(nullptr) {}

MagnumFont::MagnumFont(PluginManager::AbstractManager& manager, const std::string& plugin): AbstractFont{manager, plugin}, _opened(nullptr) {}

MagnumFont::~MagnumFont() { close(); }

FontFeatures MagnumFont::doFeatures() const { return FontFeature::OpenData|FontFeature::FileCallback|FontFeature::PreparedGlyphCache; }

bool MagnumFont::doIsOpened() const { return _opened && _opened->image; }

void MagnumFont::doClose() { _opened = nullptr; }

auto MagnumFont::doOpenData(const Containers::ArrayView<const char> data, const Float) -> Metrics {
    if(!_opened) _opened.emplace();

    if(!_opened->filePath && !fileCallback()) {
        Error{} << "Text::MagnumFont::openData(): the font can be opened only from the filesystem or if a file callback is present";
        return {};
    }

    /* Open the configuration file */
    /* MSVC 2017 requires explicit std::string constructor. MSVC 2015 doesn't. */
    std::istringstream in(std::string{data.begin(), data.size()});
    Utility::Configuration conf(in, Utility::Configuration::Flag::SkipComments);
    if(!conf.isValid() || conf.isEmpty()) {
        Error{} << "Text::MagnumFont::openData(): font file is not valid";
        return {};
    }

    /* Check version */
    if(conf.value<UnsignedInt>("version") != 1) {
        Error() << "Text::MagnumFont::openData(): unsupported file version, expected 1 but got"
                << conf.value<UnsignedInt>("version");
        return {};
    }

    /* Open and load image file. Error messages should be printed by the
       TgaImporter already, no need to repeat them again. */
    Trade::TgaImporter importer;
    importer.setFileCallback(fileCallback(), fileCallbackUserData());
    if(!importer.openFile(Utility::Directory::join(_opened->filePath ? *_opened->filePath : "", conf.value("image")))) return {};
    _opened->image = importer.image2D(0);
    if(!_opened->image) return {};

    /* Everything okay, save the data internally */
    _opened->conf = std::move(conf);

    /* Glyph advances */
    const std::vector<Utility::ConfigurationGroup*> glyphs = _opened->conf.groups("glyph");
    _opened->glyphAdvance.reserve(glyphs.size());
    for(const Utility::ConfigurationGroup* const g: glyphs)
        _opened->glyphAdvance.push_back(g->value<Vector2>("advance"));

    /* Fill character->glyph map */
    const std::vector<Utility::ConfigurationGroup*> chars = _opened->conf.groups("char");
    for(const Utility::ConfigurationGroup* const c: chars) {
        const UnsignedInt glyphId = c->value<UnsignedInt>("glyph");
        CORRADE_INTERNAL_ASSERT(glyphId < _opened->glyphAdvance.size());
        _opened->glyphId.emplace(c->value<char32_t>("unicode"), glyphId);
    }

    return {_opened->conf.value<Float>("fontSize"),
            _opened->conf.value<Float>("ascent"),
            _opened->conf.value<Float>("descent"),
            _opened->conf.value<Float>("lineHeight")};
}

auto MagnumFont::doOpenFile(const std::string& filename, Float size) -> Metrics {
    _opened.emplace();
    _opened->filePath = Utility::Directory::path(filename);

    return AbstractFont::doOpenFile(filename, size);
}

UnsignedInt MagnumFont::doGlyphId(const char32_t character) {
    auto it = _opened->glyphId.find(character);
    return it != _opened->glyphId.end() ? it->second : 0;
}

Vector2 MagnumFont::doGlyphAdvance(const UnsignedInt glyph) {
    return glyph < _opened->glyphAdvance.size() ? _opened->glyphAdvance[glyph] : Vector2();
}

Containers::Pointer<AbstractGlyphCache> MagnumFont::doCreateGlyphCache() {
    /* Set cache image */
    Containers::Pointer<AbstractGlyphCache> cache(new Text::GlyphCache(
        _opened->conf.value<Vector2i>("originalImageSize"),
        _opened->image->size(),
        _opened->conf.value<Vector2i>("padding")));
    cache->setImage({}, *_opened->image);

    /* Fill glyph map */
    const std::vector<Utility::ConfigurationGroup*> glyphs = _opened->conf.groups("glyph");
    for(std::size_t i = 0; i != glyphs.size(); ++i)
        cache->insert(i, glyphs[i]->value<Vector2i>("position"), glyphs[i]->value<Range2Di>("rectangle"));

    return cache;
}

Containers::Pointer<AbstractLayouter> MagnumFont::doLayout(const AbstractGlyphCache& cache, Float size, const std::string& text) {
    /* Get glyph codes from characters */
    std::vector<UnsignedInt> glyphs;
    glyphs.reserve(text.size());
    for(std::size_t i = 0; i != text.size(); ) {
        UnsignedInt codepoint;
        std::tie(codepoint, i) = Utility::Unicode::nextChar(text, i);
        const auto it = _opened->glyphId.find(codepoint);
        glyphs.push_back(it == _opened->glyphId.end() ? 0 : it->second);
    }

    return Containers::Pointer<MagnumFontLayouter>(new MagnumFontLayouter(_opened->glyphAdvance, cache, this->size(), size, std::move(glyphs)));
}

namespace {

MagnumFontLayouter::MagnumFontLayouter(const std::vector<Vector2>& glyphAdvance, const AbstractGlyphCache& cache, const Float fontSize, const Float textSize, std::vector<UnsignedInt>&& glyphs): AbstractLayouter(glyphs.size()), glyphAdvance(glyphAdvance), cache(cache), fontSize(fontSize), textSize(textSize), glyphs(std::move(glyphs)) {}

std::tuple<Range2D, Range2D, Vector2> MagnumFontLayouter::doRenderGlyph(const UnsignedInt i) {
    /* Position of the texture in the resulting glyph, texture coordinates */
    Vector2i position;
    Range2Di rectangle;
    std::tie(position, rectangle) = cache[glyphs[i]];

    /* Normalized texture coordinates */
    const auto textureCoordinates = Range2D(rectangle).scaled(1.0f/Vector2(cache.textureSize()));

    /* Quad rectangle, computed from texture rectangle, denormalized to
       requested text size */
    const auto quadRectangle = Range2D(Range2Di::fromSize(position, rectangle.size())).scaled(Vector2(textSize/fontSize));

    /* Advance for given glyph, denormalized to requested text size */
    const Vector2 advance = glyphAdvance[glyphs[i]]*(textSize/fontSize);

    return std::make_tuple(quadRectangle, textureCoordinates, advance);
}

}

}}

CORRADE_PLUGIN_REGISTER(MagnumFont, Magnum::Text::MagnumFont,
    "cz.mosra.magnum.Text.AbstractFont/0.3")
