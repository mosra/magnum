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

#include "DistanceFieldGlyphCache.h"

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/ImageView.h"
#ifndef CORRADE_NO_ASSERT
#include "Magnum/PixelFormat.h"
#endif
#include "Magnum/TextureFormat.h"
#include "Magnum/TextureTools/DistanceField.h"

namespace Magnum { namespace Text {

DistanceFieldGlyphCache::DistanceFieldGlyphCache(const Vector2i& originalSize, const Vector2i& size, const UnsignedInt radius):
    #if !(defined(MAGNUM_TARGET_GLES) && defined(MAGNUM_TARGET_GLES2))
    GlyphCache(TextureFormat::R8, originalSize, size, Vector2i(radius)),
    #elif !defined(MAGNUM_TARGET_WEBGL)
    /* Luminance is not renderable in most cases */
    GlyphCache(Context::current().isExtensionSupported<Extensions::GL::EXT::texture_rg>() ?
        TextureFormat::Red : TextureFormat::RGB, originalSize, size, Vector2i(radius)),
    #else
    GlyphCache(TextureFormat::RGB, originalSize, size, Vector2i(radius)),
    #endif
    scale(Vector2(size)/Vector2(originalSize)), radius(radius)
{
    #ifndef MAGNUM_TARGET_GLES
    MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::ARB::texture_rg);
    #endif

    #if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    /* Luminance is not renderable in most cases */
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_rg>())
        Warning() << "Text::DistanceFieldGlyphCache:" << Extensions::GL::EXT::texture_rg::string() << "not supported, using inefficient RGB format for glyph cache texture";
    #endif
}

void DistanceFieldGlyphCache::setImage(const Vector2i& offset, const ImageView2D& image) {
    #if !(defined(MAGNUM_TARGET_GLES) && defined(MAGNUM_TARGET_GLES2))
    const TextureFormat internalFormat = TextureFormat::R8;
    CORRADE_ASSERT(image.format() == PixelFormat::Red,
        "Text::DistanceFieldGlyphCache::setImage(): expected" << PixelFormat::Red << "but got" << image.format(), );
    #else
    TextureFormat internalFormat;
    #ifndef MAGNUM_TARGET_WEBGL
    if(Context::current().isExtensionSupported<Extensions::GL::EXT::texture_rg>()) {
        internalFormat = TextureFormat::Red;
        CORRADE_ASSERT(image.format() == PixelFormat::Red,
            "Text::DistanceFieldGlyphCache::setImage(): expected" << PixelFormat::Red << "but got" << image.format(), );
    } else
    #endif
    {
        internalFormat = TextureFormat::Luminance;
        CORRADE_ASSERT(image.format() == PixelFormat::Luminance,
            "Text::DistanceFieldGlyphCache::setImage(): expected" << PixelFormat::Luminance << "but got" << image.format(), );
    }
    #endif

    Texture2D input;
    input.setWrapping(Sampler::Wrapping::ClampToEdge)
        .setMinificationFilter(Sampler::Filter::Linear)
        .setMagnificationFilter(Sampler::Filter::Linear)
        .setImage(0, internalFormat, image);

    /* Create distance field from input texture */
    TextureTools::distanceField(input, texture(), Range2Di::fromSize(offset*scale, image.size()*scale), radius, image.size());
}

void DistanceFieldGlyphCache::setDistanceFieldImage(const Vector2i& offset, const ImageView2D& image) {
    #if !(defined(MAGNUM_TARGET_GLES) && defined(MAGNUM_TARGET_GLES2))
    CORRADE_ASSERT(image.format() == PixelFormat::Red,
        "Text::DistanceFieldGlyphCache::setDistanceFieldImage(): expected" << PixelFormat::Red << "but got" << image.format(), );
    #else
    #ifndef MAGNUM_TARGET_WEBGL
    if(Context::current().isExtensionSupported<Extensions::GL::EXT::texture_rg>())
        CORRADE_ASSERT(image.format() == PixelFormat::Red,
            "Text::DistanceFieldGlyphCache::setDistanceFieldImage(): expected" << PixelFormat::Red << "but got" << image.format(), );
    else
    #endif
    {
        /* Luminance is not renderable in most cases */
        CORRADE_ASSERT(image.format() == PixelFormat::RGB,
            "Text::DistanceFieldGlyphCache::setDistanceFieldImage(): expected" << PixelFormat::RGB << "but got" << image.format(), );
    }
    #endif

    texture().setSubImage(0, offset, image);
}

}}
