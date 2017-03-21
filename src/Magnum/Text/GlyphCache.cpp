/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include "GlyphCache.h"

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/Image.h"
#include "Magnum/TextureFormat.h"
#include "Magnum/TextureTools/Atlas.h"

namespace Magnum { namespace Text {

GlyphCache::GlyphCache(const TextureFormat internalFormat, const Vector2i& size, const Vector2i& padding): GlyphCache{internalFormat, size, size, padding} {}

GlyphCache::GlyphCache(const TextureFormat internalFormat, const Vector2i& originalSize, const Vector2i& size, const Vector2i& padding): _size(originalSize), _padding(padding) {
    initialize(internalFormat, size);
}

GlyphCache::GlyphCache(const Vector2i& size, const Vector2i& padding): GlyphCache{size, size, padding} {}

GlyphCache::GlyphCache(const Vector2i& originalSize, const Vector2i& size, const Vector2i& padding): _size(originalSize), _padding(padding) {
    #ifndef MAGNUM_TARGET_GLES
    MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::ARB::texture_rg);
    #endif

    /** @todo Is there any better way to select proper sized/unsized format on ES2? */
    #ifndef MAGNUM_TARGET_GLES2
    const TextureFormat internalFormat = TextureFormat::R8;
    #elif !defined(MAGNUM_TARGET_WEBGL)
    TextureFormat internalFormat;
    if(Context::current().isExtensionSupported<Extensions::GL::EXT::texture_rg>()) {
        internalFormat = Context::current().isExtensionSupported<Extensions::GL::EXT::texture_storage>() ?
            TextureFormat::R8 : TextureFormat::Red;
    } else internalFormat = TextureFormat::Luminance;
    #else
    const TextureFormat internalFormat = TextureFormat::Luminance;
    #endif

    initialize(internalFormat, size);
}

GlyphCache::~GlyphCache() = default;

void GlyphCache::initialize(const TextureFormat internalFormat, const Vector2i& size) {
    /* Initialize texture */
    _texture.setWrapping(Sampler::Wrapping::ClampToEdge)
        .setMinificationFilter(Sampler::Filter::Linear)
        .setMagnificationFilter(Sampler::Filter::Linear)
        .setStorage(1, internalFormat, size);

    /* Default "Not Found" glyph */
    glyphs.insert({0, {}});
}

std::vector<Range2Di> GlyphCache::reserve(const std::vector<Vector2i>& sizes) {
    CORRADE_ASSERT((glyphs.size() == 1 && glyphs.at(0) == std::pair<Vector2i, Range2Di>()),
        "Text::GlyphCache::reserve(): reserving space in non-empty cache is not yet implemented", {});
    glyphs.reserve(glyphs.size() + sizes.size());
    return TextureTools::atlas(_size, sizes, _padding);
}

void GlyphCache::insert(const UnsignedInt glyph, const Vector2i& position, const Range2Di& rectangle) {
    const std::pair<Vector2i, Range2Di> glyphData = {position-_padding, rectangle.padded(_padding)};

    /* Overwriting "Not Found" glyph */
    if(glyph == 0) glyphs[0] = glyphData;

    /* Inserting new glyph */
    else CORRADE_INTERNAL_ASSERT_OUTPUT(glyphs.insert({glyph, glyphData}).second);
}

void GlyphCache::setImage(const Vector2i& offset, const ImageView2D& image) {
    /** @todo some internalformat/format checking also here (if querying internal format is not slow) */
    _texture.setSubImage(0, offset, image);
}

}}
