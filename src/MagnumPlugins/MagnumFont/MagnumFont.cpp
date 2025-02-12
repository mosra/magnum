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

#include "MagnumFont.h"

#include <sstream>
#include <unordered_map>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/Triple.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/Configuration.h>
#include <Corrade/Utility/Path.h>
#include <Corrade/Utility/Unicode.h>

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/ConfigurationValue.h"
#include "Magnum/Text/AbstractShaper.h"
#include "Magnum/Text/GlyphCacheGL.h"
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
    Containers::Optional<Containers::String> filePath;
    std::unordered_map<char32_t, UnsignedInt> glyphId;
    struct Glyph {
        Vector2i size;
        Vector2 advance;
    };
    Containers::Array<Glyph> glyphs;
};

MagnumFont::MagnumFont(): _opened(nullptr) {}

MagnumFont::MagnumFont(PluginManager::AbstractManager& manager, const Containers::StringView& plugin): AbstractFont{manager, plugin}, _opened(nullptr) {}

MagnumFont::~MagnumFont() { close(); }

FontFeatures MagnumFont::doFeatures() const { return FontFeature::OpenData|FontFeature::FileCallback|FontFeature::PreparedGlyphCache; }

bool MagnumFont::doIsOpened() const { return _opened && _opened->image; }

void MagnumFont::doClose() { _opened = nullptr; }

auto MagnumFont::doOpenData(const Containers::ArrayView<const char> data, const Float) -> Properties {
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
    if(!importer.openFile(Utility::Path::join(_opened->filePath ? *_opened->filePath : "", conf.value("image")))) return {};
    _opened->image = importer.image2D(0);
    if(!_opened->image) return {};

    /* Everything okay, save the data internally */
    _opened->conf = Utility::move(conf);

    /* Glyph advances */
    const std::vector<Utility::ConfigurationGroup*> glyphs = _opened->conf.groups("glyph");
    _opened->glyphs = Containers::Array<Data::Glyph>{NoInit, glyphs.size()};
    for(std::size_t i = 0; i != glyphs.size(); ++i)
        _opened->glyphs[i] = {
            glyphs[i]->value<Range2Di>("rectangle").size(),
            glyphs[i]->value<Vector2>("advance")
        };

    /* Fill character->glyph map */
    const std::vector<Utility::ConfigurationGroup*> chars = _opened->conf.groups("char");
    for(const Utility::ConfigurationGroup* const c: chars) {
        const UnsignedInt glyphId = c->value<UnsignedInt>("glyph");
        CORRADE_INTERNAL_ASSERT(glyphId < _opened->glyphs.size());
        _opened->glyphId.emplace(c->value<char32_t>("unicode"), glyphId);
    }

    return {_opened->conf.value<Float>("fontSize"),
            _opened->conf.value<Float>("ascent"),
            _opened->conf.value<Float>("descent"),
            _opened->conf.value<Float>("lineHeight"),
            UnsignedInt(glyphs.size())};
}

auto MagnumFont::doOpenFile(const Containers::StringView filename, const Float size) -> Properties {
    _opened.emplace();
    _opened->filePath.emplace(Utility::Path::path(filename));

    return AbstractFont::doOpenFile(filename, size);
}

void MagnumFont::doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>& characters, const Containers::StridedArrayView1D<UnsignedInt>& glyphs) {
    for(std::size_t i = 0; i != characters.size(); ++i) {
        auto it = _opened->glyphId.find(characters[i]);
        glyphs[i] = it != _opened->glyphId.end() ? it->second : 0;
    }
}

Vector2 MagnumFont::doGlyphSize(const UnsignedInt glyph) {
    return Vector2{_opened->glyphs[glyph].size};
}

Vector2 MagnumFont::doGlyphAdvance(const UnsignedInt glyph) {
    return _opened->glyphs[glyph].advance;
}

Containers::Pointer<AbstractGlyphCache> MagnumFont::doCreateGlyphCache() {
    /* Set cache image. Have to create a custom subclass in order to have
       control over both the source and processed format (where
       DistanceFieldGlyphCache may set the processed format to RGBA if there's
       no renderable single-channel format). */
    /** @todo figure out a nicer way, and ideally how to do this with
        fillGlyphCache() instead */
    struct Cache: GlyphCacheGL {
        explicit Cache(PixelFormat format, const Vector2i& size, PixelFormat processedFormat, const Vector2i& processedSize, const Vector2i& padding): GlyphCacheGL{format, size, processedFormat, processedSize, padding} {}

        GlyphCacheFeatures doFeatures() const override {
            return GlyphCacheFeature::ImageProcessing;
        }
    };
    Containers::Pointer<Cache> cache{InPlaceInit,
        PixelFormat::R8Unorm,
        _opened->conf.value<Vector2i>("originalImageSize"),
        PixelFormat::R8Unorm,
        _opened->image->size(),
        _opened->conf.value<Vector2i>("padding")};
    cache->setProcessedImage({}, *_opened->image);

    const std::vector<Utility::ConfigurationGroup*> glyphs = _opened->conf.groups("glyph");

    /* Set the global invalid glyph to the same as the per-font invalid
       glyph. */
    if(!glyphs.empty())
        cache->setInvalidGlyph(glyphs[0]->value<Vector2i>("position"), glyphs[0]->value<Range2Di>("rectangle"));

    /* Add a font, fill the glyph map */
    const UnsignedInt fontId = cache->addFont(glyphs.size(), this);
    for(std::size_t i = 0; i < glyphs.size(); ++i)
        cache->addGlyph(fontId, i, glyphs[i]->value<Vector2i>("position"), glyphs[i]->value<Range2Di>("rectangle"));

    /* GCC 4.8 needs extra help here */
    return Containers::Pointer<AbstractGlyphCache>{Utility::move(cache)};
}

Containers::Pointer<AbstractShaper> MagnumFont::doCreateShaper() {
    struct Shaper: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(const Containers::StringView textFull, const UnsignedInt begin, const UnsignedInt end, Containers::ArrayView<const FeatureRange>) override {
            const Data& fontData = *static_cast<const MagnumFont&>(font())._opened;
            const Containers::StringView text = textFull.slice(begin, end == ~UnsignedInt{} ? textFull.size() : end);

            /* Get glyph codes from characters */
            arrayResize(_glyphs, 0);
            arrayReserve(_glyphs, text.size());
            for(std::size_t i = 0; i != text.size(); ) {
                const Containers::Pair<char32_t, std::size_t> codepointNext = Utility::Unicode::nextChar(text, i);
                const auto it = fontData.glyphId.find(codepointNext.first());
                arrayAppend(_glyphs, InPlaceInit,
                    it == fontData.glyphId.end() ? 0 : it->second,
                    begin + UnsignedInt(i));
                i = codepointNext.second();
            }

            return _glyphs.size();
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>& ids) const override {
            Utility::copy(stridedArrayView(_glyphs).slice(&Containers::Pair<UnsignedInt, UnsignedInt>::first), ids);
        }
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>& offsets, const Containers::StridedArrayView1D<Vector2>& advances) const override {
            const Data& fontData = *static_cast<const MagnumFont&>(font())._opened;
            for(std::size_t i = 0; i != _glyphs.size(); ++i) {
                /* There's no glyph offsets in addition to advances */
                offsets[i] = {};
                advances[i] = fontData.glyphs[_glyphs[i].first()].advance;
            }
        }
        void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>& clusters) const override {
            Utility::copy(stridedArrayView(_glyphs).slice(&Containers::Pair<UnsignedInt, UnsignedInt>::second), clusters);
        }

        Containers::StridedArrayView1D<const Vector2> _glyphAdvance;
        Containers::Array<Containers::Pair<UnsignedInt, UnsignedInt>> _glyphs;
    };

    return Containers::pointer<Shaper>(*this);
}

}}

CORRADE_PLUGIN_REGISTER(MagnumFont, Magnum::Text::MagnumFont,
    MAGNUM_TEXT_ABSTRACTFONT_PLUGIN_INTERFACE)
