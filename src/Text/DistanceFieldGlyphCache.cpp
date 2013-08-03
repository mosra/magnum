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

#include "DistanceFieldGlyphCache.h"

#include "Extensions.h"
#ifndef CORRADE_NO_ASSERT
#include "ImageFormat.h"
#endif
#include "ImageReference.h"
#include "TextureFormat.h"
#include "TextureTools/DistanceField.h"

namespace Magnum { namespace Text {

DistanceFieldGlyphCache::DistanceFieldGlyphCache(const Vector2i& originalSize, const Vector2i& size, const UnsignedInt radius):
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES3)
    GlyphCache(TextureFormat::R8, originalSize, size, Vector2i(radius)),
    #else
    /* Luminance is not renderable in most cases */
    GlyphCache(Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_rg>() ?
        TextureFormat::Red : TextureFormat::RGB, originalSize, size, Vector2i(radius)),
    #endif
    scale(Vector2(size)/originalSize), radius(radius)
{
    #ifndef MAGNUM_TARGET_GLES
    MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::ARB::texture_rg);
    #endif

    #ifdef MAGNUM_TARGET_GLES2
    /* Luminance is not renderable in most cases */
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_rg>())
        Warning() << "Text::DistanceFieldGlyphCache:" << Extensions::GL::EXT::texture_rg::string() << "not supported, using inefficient RGB format for glyph cache texture";
    #endif
}

void DistanceFieldGlyphCache::setImage(const Vector2i& offset, const ImageReference2D& image) {
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES3)
    const TextureFormat internalFormat = TextureFormat::R8;
    CORRADE_ASSERT(image.format() == ImageFormat::Red,
        "Text::DistanceFieldGlyphCache::setImage(): expected" << ImageFormat::Red << "but got" << image.format(), );
    #else
    TextureFormat internalFormat;
    if(Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_rg>()) {
        internalFormat = TextureFormat::Red;
        CORRADE_ASSERT(image.format() == ImageFormat::Red,
            "Text::DistanceFieldGlyphCache::setImage(): expected" << ImageFormat::Red << "but got" << image.format(), );
    } else {
        internalFormat = TextureFormat::Luminance;
        CORRADE_ASSERT(image.format() == ImageFormat::Luminance,
            "Text::DistanceFieldGlyphCache::setImage(): expected" << ImageFormat::Luminance << "but got" << image.format(), );
    }
    #endif

    Texture2D input;
    input.setWrapping(Sampler::Wrapping::ClampToEdge)
        .setMinificationFilter(Sampler::Filter::Linear)
        .setMagnificationFilter(Sampler::Filter::Linear)
        .setImage(0, internalFormat, image);

    /* Create distance field from input texture */
    TextureTools::distanceField(input, texture(), Rectanglei::fromSize(offset*scale, image.size()*scale), radius, image.size());
}

void DistanceFieldGlyphCache::setDistanceFieldImage(const Vector2i& offset, const ImageReference2D& image) {
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES3)
    CORRADE_ASSERT(image.format() == ImageFormat::Red,
        "Text::DistanceFieldGlyphCache::setDistanceFieldImage(): expected" << ImageFormat::Red << "but got" << image.format(), );
    #else
    if(Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_rg>())
        CORRADE_ASSERT(image.format() == ImageFormat::Red,
            "Text::DistanceFieldGlyphCache::setDistanceFieldImage(): expected" << ImageFormat::Red << "but got" << image.format(), );

    /* Luminance is not renderable in most cases */
    else CORRADE_ASSERT(image.format() == ImageFormat::RGB,
            "Text::DistanceFieldGlyphCache::setDistanceFieldImage(): expected" << ImageFormat::RGB << "but got" << image.format(), );
    #endif

    texture().setSubImage(0, offset, image);
}

}}
