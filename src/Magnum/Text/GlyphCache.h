#ifndef Magnum_Text_GlyphCache_h
#define Magnum_Text_GlyphCache_h
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

/** @file
 * @brief Class @ref Magnum::Text::GlyphCache
 */

#include "Magnum/configure.h"

#ifdef MAGNUM_TARGET_GL
#include "Magnum/GL/Texture.h"
#include "Magnum/Text/AbstractGlyphCache.h"

namespace Magnum { namespace Text {

/**
@brief Glyph cache

Contains font glyphs prerendered into texture atlas.

@section Text-GlyphCache-usage Usage

Create GlyphCache object with sufficient size and then call
@ref AbstractFont::createGlyphCache() to fill it with glyphs.

@snippet MagnumText.cpp GlyphCache-usage

See @ref Renderer for information about text rendering.

This class supports the @ref GlyphCacheFeature::ImageDownload (and thus calling
@ref image()) only on desktop OpenGL, due to using @ref GL::Texture::image(),
which is not available on @ref MAGNUM_TARGET_GLES "OpenGL ES" platforms.

@todo Some way for Font to negotiate or check internal texture format
@todo Default glyph 0 with rect 0 0 0 0 will result in negative dimensions when
    nonzero padding is removed

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
class MAGNUM_TEXT_EXPORT GlyphCache: public AbstractGlyphCache {
    public:
        /**
         * @brief Constructor
         * @param internalFormat    Internal texture format
         * @param originalSize      Unscaled glyph cache texture size
         * @param size              Actual glyph cache texture size
         * @param padding           Padding around every glyph
         *
         * All glyphs parameters are saved relative to @p originalSize,
         * although the actual glyph cache texture has @p size. Glyph
         * @p padding can be used to account for e.g. glyph shadows.
         */
        explicit GlyphCache(GL::TextureFormat internalFormat, const Vector2i& originalSize, const Vector2i& size, const Vector2i& padding);

        /**
         * @brief Constructor
         *
         * Same as calling the above with @p originalSize and @p size the same.
         */
        explicit GlyphCache(GL::TextureFormat internalFormat, const Vector2i& size, const Vector2i& padding = {});

        /**
         * @brief Constructor
         *
         * Sets internal texture format to red channel only. On desktop OpenGL
         * requires @gl_extension{ARB,texture_rg} (also part of OpenGL ES 3.0 and
         * WebGL 2), on ES2 unconditionally uses @ref GL::TextureFormat::Luminance.
         * This is done for consistency with @ref GL::pixelFormat(), which
         * unconditionally returns @ref GL::PixelFormat::Luminance for
         * @ref PixelFormat::R8Unorm. See
         * @ref GlyphCache(GL::TextureFormat, const Vector2i&, const Vector2i&)
         * for an alternative.
         */
        explicit GlyphCache(const Vector2i& originalSize, const Vector2i& size, const Vector2i& padding);

        /**
         * @brief Constructor
         *
         * Same as calling the above with @p originalSize and @p size the same.
         */
        explicit GlyphCache(const Vector2i& size, const Vector2i& padding = {});

        ~GlyphCache();

        /** @brief Cache texture */
        GL::Texture2D& texture() { return _texture; }

    private:
        GlyphCacheFeatures MAGNUM_LOCAL doFeatures() const override;
        void MAGNUM_LOCAL doSetImage(const Vector2i& offset, const ImageView2D& image) override;
        #ifndef MAGNUM_TARGET_GLES
        Image2D MAGNUM_LOCAL doImage() override;
        #endif

        GL::Texture2D _texture;
};

}}
#else
#error this header is available only in the OpenGL build
#endif

#endif
