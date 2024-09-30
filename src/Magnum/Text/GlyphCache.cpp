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

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Containers/Optional.h>
#endif

#include "Magnum/ImageView.h"
#include "Magnum/GL/TextureFormat.h"
#ifdef MAGNUM_BUILD_DEPRECATED
#include "Magnum/PixelFormat.h"
#endif
#if !defined(MAGNUM_TARGET_GLES) || (defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
#include "Magnum/PixelFormat.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#endif
#if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
#include "Magnum/GL/PixelFormat.h"
#endif

namespace Magnum { namespace Text {

GlyphCache::GlyphCache(const PixelFormat format, const Vector2i& size, const PixelFormat processedFormat, const Vector2i& processedSize, const Vector2i& padding): AbstractGlyphCache{format, size, processedFormat, processedSize, padding} {
    #ifndef MAGNUM_TARGET_GLES
    if(processedFormat == PixelFormat::R8Unorm)
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::texture_rg);
    #endif

    /* Initialize the texture */
    _texture.setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear);

    /* ES2 special-casing. WebGL 1 has neither EXT_texture_rg nor
       EXT_texture_storage so it can use the common code path without
       issues. */
    #if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    /* Prefer to use Red instead of Luminance if available, as Luminance isn't
       renderable */
    GL::TextureFormat textureFormat = GL::textureFormat(processedFormat);
    GL::PixelFormat pixelFormat = GL::pixelFormat(processedFormat);
    if(textureFormat == GL::TextureFormat::Luminance && GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_rg>()) {
        textureFormat = GL::TextureFormat::Red;
        pixelFormat = GL::PixelFormat::Red;
    }

    /* And use setImage() instead of setStorage() if the format is unsized, as
       EXT_texture_storage doesn't allow those */
    if(textureFormat == GL::TextureFormat::Red ||
       textureFormat == GL::TextureFormat::Luminance ||
       textureFormat == GL::TextureFormat::RG ||
       textureFormat == GL::TextureFormat::LuminanceAlpha ||
       textureFormat == GL::TextureFormat::RGB ||
       textureFormat == GL::TextureFormat::SRGB ||
       textureFormat == GL::TextureFormat::RGBA ||
       textureFormat == GL::TextureFormat::SRGBAlpha)
        _texture.setImage(0, textureFormat, ImageView2D{pixelFormat, GL::PixelType::UnsignedByte, processedSize});
    else
        _texture.setStorage(1, textureFormat, processedSize);
    #else
    _texture.setStorage(1, GL::textureFormat(processedFormat), processedSize);
    #endif
}

GlyphCache::GlyphCache(const PixelFormat format, const Vector2i& size, const Vector2i& padding): GlyphCache{format, size, format, size, padding} {}

#ifdef MAGNUM_BUILD_DEPRECATED
/* The unconditional Optional unwrap in these two may assert in rare cases.
   Let's hope it doesn't in practice. */

GlyphCache::GlyphCache(const GL::TextureFormat internalFormat, const Vector2i& size, const Vector2i& padding): GlyphCache{*GL::genericPixelFormat(internalFormat), size, padding} {}

GlyphCache::GlyphCache(const GL::TextureFormat internalFormat, const Vector2i& size, const Vector2i& processedSize, const Vector2i& padding): GlyphCache{*GL::genericPixelFormat(internalFormat), size, *GL::genericPixelFormat(internalFormat), processedSize, padding} {}

GlyphCache::GlyphCache(const Vector2i& size, const Vector2i& padding): GlyphCache{PixelFormat::R8Unorm, size, padding} {}

GlyphCache::GlyphCache(const Vector2i& size, const Vector2i& processedSize, const Vector2i& padding): GlyphCache{PixelFormat::R8Unorm, size, PixelFormat::R8Unorm, processedSize, padding} {}
#endif

GlyphCache::GlyphCache(NoCreateT) noexcept: AbstractGlyphCache{NoCreate}, _texture{NoCreate} {}

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
        /* On ES2 if EXT_texture_rg is present, the single-channel texture
           format is Red instead of Luminance. Have to duplicate the logic here
           in addition to below because it's easier than extracting
           formatExtra() and everything else from the view afterwards. */
        #ifndef MAGNUM_TARGET_WEBGL
        if(image.format() == PixelFormat::R8Unorm && GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_rg>()) {
            _texture.setSubImage(0, {}, ImageView2D{GL::PixelFormat::Red, GL::PixelType::UnsignedByte, size().xy(), image.data()});
        } else
        #endif
        {
            _texture.setSubImage(0, {}, ImageView2D{image.format(), size().xy(), image.data()});
        }
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
        /* On ES2 if EXT_texture_rg is present, the single-channel texture
           format is Red instead of Luminance */
        #ifdef MAGNUM_TARGET_GLES2
        if(image.format() == PixelFormat::R8Unorm && GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_rg>()) {
            _texture.setSubImage(0, offset, ImageView2D{image.storage(), GL::PixelFormat::Red, GL::PixelType::UnsignedByte, image.size(), image.data()});
        } else
        #endif
        {
            _texture.setSubImage(0, offset, image);
        }
    }
    #endif
}

}}
