#ifndef Magnum_Text_GlyphCacheGL_h
#define Magnum_Text_GlyphCacheGL_h
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

/** @file
 * @brief Class @ref Magnum::Text::GlyphCacheGL
 * @m_since_latest
 */

#include "Magnum/configure.h"

#ifdef MAGNUM_TARGET_GL
#include "Magnum/GL/Texture.h"
#include "Magnum/Text/AbstractGlyphCache.h"

namespace Magnum { namespace Text {

/**
@brief OpenGL glyph cache
@m_since_latest

Contains font glyphs rendered into a texture atlas.

@section Text-GlyphCacheGL-usage Usage

Create the @ref GlyphCacheGL object with sufficient size and then call
@ref AbstractFont::createGlyphCache() to fill it with glyphs.

@snippet Text-gl.cpp GlyphCacheGL-usage

See the @ref Renderer class for information about text rendering. The
@ref AbstractGlyphCache base class has more information about general glyph
cache usage.

@section Text-GlyphCacheGL-internal-format Internal texture format

The @ref GL::TextureFormat used by @ref texture() is implicitly coming from
@ref GL::textureFormat(Magnum::PixelFormat) applied to @ref format(), or if
@ref GlyphCacheFeature::ImageProcessing is supported, to @ref processedFormat()
instead.

If @ref PixelFormat::R8Unorm is used for @ref format() or if
@ref GlyphCacheFeature::ImageProcessing is supported and
@ref PixelFormat::R8Unorm is used for @ref processedFormat(), on desktop OpenGL
the class expects that @gl_extension{ARB,texture_rg} (OpenGL 3.0) is supported
and uses @ref GL::TextureFormat::R8. On OpenGL ES 2.0, if
@gl_extension{EXT,texture_rg} is supported, @ref GL::TextureFormat::Red /
@ref GL::TextureFormat::R8 is used instead of @ref GL::TextureFormat::Luminance
for @ref PixelFormat::R8Unorm. On WebGL 1 @ref GL::TextureFormat::Luminance is
used for @ref PixelFormat::R8Unorm always.

While this is abstracted away to not affect common use through @ref image(),
@ref processedImage() or @ref setProcessedImage(), code interacting directly
with @ref texture() may need to special-case this. In particular, if image
processing needs to render to the texture, it may need to choose a different
format as luminance usually cannot be rendered to.

@todo Default glyph 0 with rect 0 0 0 0 will result in negative dimensions when
    nonzero padding is removed

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
class MAGNUM_TEXT_EXPORT GlyphCacheGL: public AbstractGlyphCache {
    public:
        /**
         * @brief Constructor
         * @param format            Source image format
         * @param size              Source image size size in pixels
         * @param padding           Padding around every glyph in pixels
         * @m_since_latest
         *
         * The @p size is expected to be non-zero. If the implementation
         * advertises @ref GlyphCacheFeature::ImageProcessing, the
         * @ref processedFormat() and @ref processedSize() is the same as
         * @p format and @p size, use @ref AbstractGlyphCache(PixelFormat, const Vector3i&, PixelFormat, const Vector2i&, const Vector2i&)
         * to specify different values.
         */
        explicit GlyphCacheGL(PixelFormat format, const Vector2i& size, const Vector2i& padding = Vector2i{1});

        /**
         * @brief Construct with a specific processed format and size
         * @param format            Source image format
         * @param size              Source image size size in pixels
         * @param processedFormat   Processed image format
         * @param processedSize     Processed glyph cache texture size in
         *      pixels
         * @param padding           Padding around every glyph in pixels. See
         *      @ref Text-AbstractGlyphCache-padding for more information about
         *      the default.
         * @m_since_latest
         *
         * The @p size and @p processedSize is expected to be non-zero. All
         * glyphs are saved in @p format relative to @p size and with
         * @p padding, although the actual glyph cache texture is in
         * @p processedFormat and has @p processedSize.
         * @see @ref AbstractGlyphCache(PixelFormat, const Vector2i&, const Vector2i&)
         */
        explicit GlyphCacheGL(PixelFormat format, const Vector2i& size, PixelFormat processedFormat, const Vector2i& processedSize, const Vector2i& padding = Vector2i{1});

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Constructor
         * @m_deprecated_since_latest Use @ref GlyphCacheGL(PixelFormat, const Vector2i&, const Vector2i&)
         *      instead.
         */
        CORRADE_DEPRECATED("use GlyphCacheGL(PixelFormat, const Vector2i&, const Vector2i&, const Vector2i&) instead") explicit GlyphCacheGL(GL::TextureFormat internalFormat, const Vector2i& size, const Vector2i& padding = Vector2i{1});

        /**
         * @brief Construct with a specific processed size
         * @m_deprecated_since_latest Use @ref GlyphCacheGL(PixelFormat, const Vector2i&, PixelFormat, const Vector2i&, const Vector2i&)
         *      instead.
         */
        CORRADE_DEPRECATED("use GlyphCacheGL(PixelFormat, const Vector2i&, const Vector2i&, const Vector2i&) instead") explicit GlyphCacheGL(GL::TextureFormat internalFormat, const Vector2i& size, const Vector2i& processedSize, const Vector2i& padding);

        /**
         * @brief Construct with an implicit format
         *
         * Calls @ref GlyphCacheGL(PixelFormat, const Vector2i&, const Vector2i&)
         * with @p format set to @ref PixelFormat::R8Unorm.
         * @m_deprecated_since_latest Use @ref GlyphCacheGL(PixelFormat, const Vector2i&, const Vector2i&)
         *      and explicitly pass the format instead.
         */
        CORRADE_DEPRECATED("use GlyphCacheGL(PixelFormat, const Vector2i&, const Vector2i&, const Vector2i&) instead") explicit GlyphCacheGL(const Vector2i& size, const Vector2i& padding = Vector2i{1});

        /**
         * @brief Construct with an implicit format and a specific processed size
         *
         * Calls @ref GlyphCacheGL(PixelFormat, const Vector2i&, PixelFormat, const Vector2i&, const Vector2i&)
         * with @p format and @p processedFormat set to
         * @ref PixelFormat::R8Unorm.
         * @m_deprecated_since_latest Use @ref GlyphCacheGL(PixelFormat, const Vector2i&, PixelFormat, const Vector2i&, const Vector2i&)
         *      and explicitly pass the format instead.
         */
        CORRADE_DEPRECATED("use GlyphCacheGL(PixelFormat, const Vector2i&, const Vector2i&, const Vector2i&) instead") explicit GlyphCacheGL(const Vector2i& size, const Vector2i& processedSize, const Vector2i& padding);
        #endif

        /**
         * @brief Construct without creating the internal state and the OpenGL texture object
         * @m_since_latest
         *
         * The constructed instance is equivalent to moved-from state, i.e. no
         * APIs can be safely called on the object. Useful in cases where you
         * will overwrite the instance later anyway. Move another object over
         * it to make it useful.
         *
         * This function can be safely used for constructing (and later
         * destructing) objects even without any OpenGL context being active.
         * However note that this is a low-level and a potentially dangerous
         * API, see the documentation of @ref NoCreate for alternatives.
         */
        explicit GlyphCacheGL(NoCreateT) noexcept;

        /** @brief Cache texture */
        GL::Texture2D& texture() { return _texture; }

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #else
    protected:
    #endif
        GL::Texture2D _texture;

    private:
        MAGNUM_TEXT_LOCAL GlyphCacheFeatures doFeatures() const override;
        MAGNUM_TEXT_LOCAL void doSetImage(const Vector2i& offset, const ImageView2D& image) override;
        /* Used if a subclass advertises GlyphCacheFeature::ImageProcessing /
           ProcessedImageDownload in its doFeatures() */
        MAGNUM_TEXT_LOCAL void doSetProcessedImage(const Vector2i& offset, const ImageView2D& image) override;
        #ifndef MAGNUM_TARGET_GLES
        MAGNUM_TEXT_LOCAL Image3D doProcessedImage() override;
        #endif
};

}}
#else
#error this header is available only in the OpenGL build
#endif

#endif
