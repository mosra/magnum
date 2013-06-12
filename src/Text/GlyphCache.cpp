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

#include "GlyphCache.h"

#include "Extensions.h"
#include "Image.h"
#include "TextureFormat.h"
#include "TextureTools/Atlas.h"

namespace Magnum { namespace Text {

GlyphCache::GlyphCache(const Vector2i& size): _size(size) {
    #ifndef MAGNUM_TARGET_GLES
    MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::ARB::texture_rg);
    #endif

    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES3)
    const TextureFormat internalFormat = TextureFormat::R8;
    #else
    const TextureFormat internalFormat =
        Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_rg>() ?
        TextureFormat::Red : TextureFormat::Luminance;
    #endif

    initialize(internalFormat, size);
}

GlyphCache::GlyphCache(const Vector2i& size, const TextureFormat internalFormat): _size(size) {
    initialize(internalFormat, size);
}

GlyphCache::GlyphCache(const Vector2i& size, const Vector2i& padding): _size(size), _padding(padding) {}

GlyphCache::~GlyphCache() = default;

/** @todo Delegating constructor when support for GCC 4.6 is dropped */
void GlyphCache::initialize(const TextureFormat internalFormat, const Vector2i& size) {
    _texture.setWrapping(Sampler::Wrapping::ClampToEdge)
        ->setMinificationFilter(Sampler::Filter::Linear)
        ->setMagnificationFilter(Sampler::Filter::Linear)
        ->setStorage(1, internalFormat, size);
}

std::vector<Rectanglei> GlyphCache::reserve(const std::vector<Vector2i>& sizes) {
    CORRADE_ASSERT(glyphs.empty(), "Text::GlyphCache::reserve(): reserving space in non-empty cache is not yet implemented", {});
    #ifndef CORRADE_GCC44_COMPATIBILITY
    glyphs.reserve(glyphs.size() + sizes.size());
    #endif
    return TextureTools::atlas(_size, sizes, _padding);
}

void GlyphCache::insert(const UnsignedInt glyph, Vector2i position, Rectanglei rectangle) {
    position -= _padding;
    rectangle.bottomLeft() -= _padding;
    rectangle.topRight() += _padding;

    glyphs.insert({glyph, {position, rectangle}});
}

void GlyphCache::setImage(const Vector2i& offset, Image2D* const image) {
    _texture.setSubImage(0, offset, image);
}

}}
