/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
#include <Containers/Array.h>
#include <Utility/Directory.h>
#include <Utility/Unicode.h>
#include <Text/GlyphCache.h>
#include <Trade/ImageData.h>

#include "TgaImporter/TgaImporter.h"

namespace Magnum { namespace Text {

struct MagnumFont::Data {
    Utility::Configuration conf;
    Trade::ImageData2D image;
    std::unordered_map<char32_t, UnsignedInt> glyphId;
    std::vector<Vector2> glyphAdvance;
};

namespace {
    class MagnumFontLayouter: public AbstractLayouter {
        public:
            explicit MagnumFontLayouter(const std::unordered_map<char32_t, UnsignedInt>& glyphId, const std::vector<Vector2>& glyphAdvance, const GlyphCache* cache, Float fontSize, Float textSize, const std::string& text);

            std::tuple<Rectangle, Rectangle, Vector2> renderGlyph(UnsignedInt i) override;

        private:
            const std::vector<Vector2>& glyphAdvance;
            const GlyphCache* const cache;
            const Float fontSize, textSize;
            std::vector<UnsignedInt> glyphs;
    };
}

MagnumFont::MagnumFont(): _opened(nullptr) {}

MagnumFont::MagnumFont(PluginManager::AbstractManager* const manager, std::string plugin): AbstractFont(manager, std::move(plugin)), _opened(nullptr) {}

MagnumFont::~MagnumFont() { close(); }

auto MagnumFont::doFeatures() const -> Features { return Feature::OpenData|Feature::MultiFile|Feature::PreparedGlyphCache; }

bool MagnumFont::doIsOpened() const { return _opened; }

void MagnumFont::doOpenData(const std::vector<std::pair<std::string, Containers::ArrayReference<const unsigned char>>>& data, const Float) {
    /* We need just the configuration file and image file */
    if(data.size() != 2) {
        Error() << "Text::MagnumFont::openData(): wanted two files, got" << data.size();
        return;
    }

    /* Open the configuration file */
    std::istringstream in({reinterpret_cast<const char*>(data[0].second.begin()), data[0].second.size()});
    Utility::Configuration conf(in, Utility::Configuration::Flag::SkipComments);
    if(!conf.isValid() || conf.isEmpty()) {
        Error() << "Text::MagnumFont::openData(): cannot open file" << data[0].first << conf.isValid();
        return;
    }

    /* Check version */
    if(conf.value<UnsignedInt>("version") != 1) {
        Error() << "Text::MagnumFont::openData(): unsupported file version, expected 1 but got"
                << conf.value<UnsignedInt>("version");
        return;
    }

    /* Check that we have also the image file */
    if(conf.value("image") != data[1].first) {
        Error() << "Text::MagnumFont::openData(): expected file"
                << conf.value("image") << "but got" << data[1].first;
        return;
    }

    /* Open and load image file */
    Trade::TgaImporter importer;
    if(!importer.openData(data[1].second)) {
        Error() << "Text::MagnumFont::openData(): cannot open image file";
        return;
    }
    Trade::ImageData2D* image = importer.image2D(0);
    if(!image) {
        Error() << "Text::MagnumFont::openData(): cannot load image file";
        return;
    }

    openInternal(std::move(conf), std::move(*image));
    delete image;
}

void MagnumFont::doOpenFile(const std::string& filename, Float) {
    /* Open the configuration file */
    Utility::Configuration conf(filename, Utility::Configuration::Flag::ReadOnly|Utility::Configuration::Flag::SkipComments);
    if(!conf.isValid() || conf.isEmpty()) {
        Error() << "Text::MagnumFont::openFile(): cannot open file" << filename << conf.isValid();
        return;
    }

    /* Check version */
    if(conf.value<UnsignedInt>("version") != 1) {
        Error() << "Text::MagnumFont::openFile(): unsupported file version, expected 1 but got"
                << conf.value<UnsignedInt>("version");
        return;
    }

    /* Open and load image file */
    const std::string imageFilename = Utility::Directory::join(Utility::Directory::path(filename), conf.value("image"));
    Trade::TgaImporter importer;
    if(!importer.openFile(imageFilename)) {
        Error() << "Text::MagnumFont::openFile(): cannot open image file" << imageFilename;
        return;
    }
    Trade::ImageData2D* image = importer.image2D(0);
    if(!image) {
        Error() << "Text::MagnumFont::openFile(): cannot load image file";
        return;
    }

    openInternal(std::move(conf), std::move(*image));
    delete image;
}

void MagnumFont::openInternal(Utility::Configuration&& conf, Trade::ImageData2D&& image) {
    /* Everything okay, save the data internally */
    _opened = new Data{std::move(conf), std::move(image), {}, {}};
    _size = _opened->conf.value<Float>("fontSize");

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
}

void MagnumFont::doClose() {
    delete _opened;
    _opened = nullptr;
}

UnsignedInt MagnumFont::doGlyphId(const char32_t character) {
    auto it = _opened->glyphId.find(character);
    return it != _opened->glyphId.end() ? it->second : 0;
}

Vector2 MagnumFont::doGlyphAdvance(const UnsignedInt glyph) {
    return glyph < _opened->glyphAdvance.size() ? _opened->glyphAdvance[glyph] : Vector2();
}

GlyphCache* MagnumFont::doCreateGlyphCache() {
    /* Set cache image */
    auto cache = new Text::GlyphCache(
        _opened->conf.value<Vector2i>("originalImageSize"),
        _opened->image.size(),
        _opened->conf.value<Vector2i>("padding"));
    cache->setImage({}, _opened->image);

    /* Fill glyph map */
    const std::vector<Utility::ConfigurationGroup*> glyphs = _opened->conf.groups("glyph");
    for(std::size_t i = 0; i != glyphs.size(); ++i)
        cache->insert(i, glyphs[i]->value<Vector2i>("position"), glyphs[i]->value<Rectanglei>("rectangle"));

    return cache;
}

AbstractLayouter* MagnumFont::doLayout(const GlyphCache* cache, Float size, const std::string& text) {
    return new MagnumFontLayouter(_opened->glyphId, _opened->glyphAdvance, cache, this->size(), size, text);
}

namespace {

MagnumFontLayouter::MagnumFontLayouter(const std::unordered_map<char32_t, UnsignedInt>& glyphId, const std::vector<Vector2>& glyphAdvance, const GlyphCache* cache, Float fontSize, Float textSize, const std::string& text): glyphAdvance(glyphAdvance), cache(cache), fontSize(fontSize), textSize(textSize) {
    /* Get glyph codes from characters */
    glyphs.reserve(text.size());
    for(std::size_t i = 0; i != text.size(); ) {
        UnsignedInt codepoint;
        std::tie(codepoint, i) = Utility::Unicode::nextChar(text, i);
        const auto it = glyphId.find(codepoint);
        glyphs.push_back(it == glyphId.end() ? 0 : it->second);
    }
    _glyphCount = glyphs.size();
}

std::tuple<Rectangle, Rectangle, Vector2> MagnumFontLayouter::renderGlyph(UnsignedInt i) {
    /* Position of the texture in the resulting glyph, texture coordinates */
    Vector2i position;
    Rectanglei rectangle;
    std::tie(position, rectangle) = (*cache)[glyphs[i]];

    const Rectangle texturePosition = Rectangle::fromSize(Vector2(position)/fontSize,
                                                          Vector2(rectangle.size())/fontSize);
    const Rectangle textureCoordinates(Vector2(rectangle.bottomLeft())/cache->textureSize(),
                                       Vector2(rectangle.topRight())/cache->textureSize());

    /* Absolute quad position, composed from cursor position, glyph offset
       and texture position, denormalized to requested text size */
    Rectangle quadPosition = Rectangle::fromSize(
        Vector2(texturePosition.left(), texturePosition.bottom())*textSize,
        texturePosition.size()*textSize);

    /* Advance for given glyph, denormalized to requested text size */
    const Vector2 advance = glyphAdvance[glyphs[i]]*textSize/fontSize;

    return std::make_tuple(quadPosition, textureCoordinates, advance);
}

}

}}
