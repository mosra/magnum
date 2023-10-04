/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Magnum/PixelFormat.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/TextureFormat.h"

#ifdef MAGNUM_TARGET_GLES2
#include "Magnum/ImageView.h"
#endif

namespace Magnum { namespace Text {

GlyphCache::GlyphCache(const GL::TextureFormat internalFormat, const Vector2i& size, const Vector2i& padding): GlyphCache{internalFormat, size, size, padding} {}

/* The unconditional Optional unwrap in here two may assert in rare cases.
   Let's hope it doesn't in practice. */
GlyphCache::GlyphCache(const GL::TextureFormat internalFormat, const Vector2i& originalSize, const Vector2i& size, const Vector2i& padding): AbstractGlyphCache{*GL::genericPixelFormat(internalFormat), originalSize, padding} {
    /* Initialize the texture */
    _texture.setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setStorage(1, internalFormat, size);
}

GlyphCache::GlyphCache(const Vector2i& size, const Vector2i& padding): GlyphCache{size, size, padding} {}

GlyphCache::GlyphCache(const Vector2i& originalSize, const Vector2i& size, const Vector2i& padding): GlyphCache{
    #ifndef MAGNUM_TARGET_GLES2
    GL::TextureFormat::R8,
    #else
    GL::TextureFormat::Luminance,
    #endif
    originalSize, size, padding}
{
    #ifndef MAGNUM_TARGET_GLES
    MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::texture_rg);
    #endif
}

GlyphCache::~GlyphCache() = default;

GlyphCacheFeatures GlyphCache::doFeatures() const { return {}; }

void GlyphCache::doSetImage(const Vector2i& offset, const ImageView2D& image) {
    /* On ES2 without EXT_unpack_subimage and on WebGL 1 there's no possibility
       to upload just a slice of the input, upload the whole image instead by
       ignoring the PixelStorage properties of the input */
    #ifdef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::unpack_subimage>())
    #endif
    {
        _texture.setSubImage(0, {}, ImageView2D{image.format(), size().xy(), image.data()});
        #ifdef MAGNUM_TARGET_WEBGL
        static_cast<void>(offset);
        #endif
    }
    #ifndef MAGNUM_TARGET_WEBGL
    else
    #endif
    #endif
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    {
        _texture.setSubImage(0, offset, image);
    }
    #endif
}

}}
