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

#include "GlyphCache.h"

#include "Magnum/Image.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/TextureTools/Atlas.h"

namespace Magnum { namespace Text {

GlyphCache::GlyphCache(const GL::TextureFormat internalFormat, const Vector2i& size, const Vector2i& padding): GlyphCache{internalFormat, size, size, padding} {}

GlyphCache::GlyphCache(const GL::TextureFormat internalFormat, const Vector2i& originalSize, const Vector2i& size, const Vector2i& padding): AbstractGlyphCache{originalSize, padding} {
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

GlyphCacheFeatures GlyphCache::doFeatures() const {
    #ifndef MAGNUM_TARGET_GLES
    return GlyphCacheFeature::ImageDownload;
    #else
    return {};
    #endif
}

void GlyphCache::doSetImage(const Vector2i& offset, const ImageView2D& image) {
    /** @todo some internalformat/format checking also here (if querying internal format is not slow) */
    _texture.setSubImage(0, offset, image);
}

#ifndef MAGNUM_TARGET_GLES
Image2D GlyphCache::doImage() { return _texture.image(0, PixelFormat::R8Unorm); }
#endif

}}
