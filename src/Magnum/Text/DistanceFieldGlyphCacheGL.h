#ifndef Magnum_Text_DistanceFieldGlyphCacheGL_h
#define Magnum_Text_DistanceFieldGlyphCacheGL_h
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

/** @file
 * @brief Class @ref Magnum::Text::DistanceFieldGlyphCacheGL, @ref Magnum::Text::DistanceFieldGlyphCacheArrayGL
 * @m_since_latest
 */

#include "Magnum/configure.h"

#ifdef MAGNUM_TARGET_GL
#include "Magnum/Text/GlyphCacheGL.h"

namespace Magnum { namespace Text {

/**
@brief OpenGL glyph cache with distance field rendering
@m_since_latest

Extends @ref GlyphCacheGL by processing rendered glyphs to a signed distance
field texture using @ref TextureTools::DistanceFieldGL, allowing them to be
drawn at different sizes and with various transformations without aliasing
artifacts. @ref DistanceFieldGlyphCacheArrayGL is then using a
@ref GL::Texture2DArray instead of a @ref GL::Texture2D. It's possible to only
use this cache for monochrome glyphs as the internal texture format is
single-channel.

@section Text-DistanceFieldGlyphCacheGL-usage Usage

In order to create a distance field glyph cache, the font has to be loaded at a
size significantly larger than what the resulting text will be. The distance
field conversion, then converts the input to a fraction of its size again,
transferring the the extra spatial resolution to distance values. The distance
values are then used to render an arbitrarily sized text without it being jaggy
at small sizes and blurry when large.

The process requires three input parameters, size of the source image, size of
the resulting glyph cache image and a radius for the distance field creation.
Their relation and effect on output quality and memory use is described in
detail in @ref TextureTools-DistanceFieldGL-parameters "TextureTools::DistanceFieldGL docs".
In short, the ratio between the input and output image size is usually four or
eight times, and the size of the font should match the larger size. So, for
example, if a @cpp {128, 128} @ce @ref GlyphCacheGL would be filled with a 12
pt font, a @cpp {512, 512} @ce source image for the distance field should use a
48 pt font. The radius should then be chosen so it's at least one or two pixels
in the scaled-down result, so in this case at least @cpp 4 @ce:

@snippet Text-gl.cpp DistanceFieldGlyphCacheGL-usage

As long as the cache size allows, you can call
@ref AbstractFont::fillGlyphCache() multiple times with additional glyphs and
other fonts, each time the input will be incrementally converted to a distance
field texture. See the @ref Text-AbstractFont-glyph-cache "AbstractFont documentation"
for more options for glyph cache filling. The @ref AbstractGlyphCache base
class has more information about general glyph cache usage. Finally, assuming a
@ref RendererGL is used with this cache for rendering the text, its
@relativeref{RendererGL,mesh()} can be then drawn using
@ref Shaders::DistanceFieldVectorGL, together with binding @ref texture() for
drawing:

@snippet Text-gl.cpp DistanceFieldGlyphCacheGL-usage-draw

@section Text-DistanceFieldGlyphCacheGL-internal-format Internal texture format

The @ref format() is always @ref PixelFormat::R8Unorm.

On desktop OpenGL, OpenGL ES 3.0+, WebGL 2, and OpenGL ES 2.0 if
@gl_extension{EXT,texture_rg} is supported, the @ref processedFormat() is
always @ref PixelFormat::R8Unorm, which maps to @ref GL::TextureFormat::R8 for
the @ref texture(), matching
@ref Text-GlyphCacheGL-internal-format "the behavior listed in GlyphCacheGL docs".

On OpenGL ES 2.0 without @gl_extension{EXT,texture_rg} and on WebGL 1,
@ref PixelFormat::R8Unorm maps to @ref GL::TextureFormat::Luminance, which
isn't renderable and thus cannot be used for calculating the distance field.
Instead, @ref PixelFormat::RGBA8Unorm is used for @ref processedFormat(). This
shouldn't affect common use through @ref image(), but code interacting with
@ref processedImage() or @ref setProcessedImage() may need to be aware of this.

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@see @ref TextureTools::DistanceFieldGL
*/
class MAGNUM_TEXT_EXPORT DistanceFieldGlyphCacheGL: public GlyphCacheGL {
    public:
        /**
         * @brief Constructor
         * @param size              Size of the source image
         * @param processedSize     Resulting distance field texture size
         * @param radius            Distance field calculation radius
         *
         * See @ref TextureTools::DistanceFieldGL for more information about
         * the parameters. Size restrictions from it apply here as well, in
         * particular the ratio of @p size and @p processedSize is expected to
         * be a multiple of 2.
         *
         * Sets the @ref processedFormat() to @ref PixelFormat::R8Unorm, if
         * available. On OpenGL ES 3.0+ and WebGL 2 uses
         * @ref PixelFormat::R8Unorm always. On desktop OpenGL requires
         * @gl_extension{ARB,texture_rg} (part of OpenGL 3.0), on ES2 uses
         * @gl_extension{EXT,texture_rg} if available and uses
         * @ref PixelFormat::RGB8Unorm as fallback if not, on WebGL 1 uses
         * @ref PixelFormat::RGB8Unorm always.
         */
        explicit DistanceFieldGlyphCacheGL(const Vector2i& size, const Vector2i& processedSize, UnsignedInt radius);

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
        explicit DistanceFieldGlyphCacheGL(NoCreateT) noexcept;

        /**
         * @brief Distance field calculation radius
         * @m_since_latest
         */
        UnsignedInt radius() const;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Distance field texture size
         *
         * Compared to @ref textureSize(), which is the size of the source
         * image, this function returns size of the resulting distance field
         * texture.
         * @m_deprecated_since_latest Use @ref processedSize() instead.
         */
        CORRADE_DEPRECATED("use processedSize() instead") Vector2i distanceFieldTextureSize() const;

        /**
         * @brief Set a distance field cache image
         *
         * Compared to @ref setImage() uploads an already computed distance
         * field image to given offset in the distance field texture. The
         * @p offset and @ref ImageView::size() are expected to be in bounds
         * for @ref distanceFieldTextureSize().
         * @m_deprecated_since_latest Use @ref setProcessedImage() instead.
         */
        CORRADE_DEPRECATED("use setProcessedImage() instead") void setDistanceFieldImage(const Vector2i& offset, const ImageView2D& image);
        #endif

    private:
        struct State;

        MAGNUM_TEXT_LOCAL GlyphCacheFeatures doFeatures() const override;
        MAGNUM_TEXT_LOCAL void doSetImage(const Vector2i& offset, const ImageView2D& image) override;
};

#ifndef MAGNUM_TARGET_GLES2
/**
@brief OpenGL array glyph cache with distance field rendering
@m_since_latest

Like @ref DistanceFieldGlyphCacheGL, but backed by a @ref GL::Texture2DArray
instead of @ref GL::Texture2D. See the @ref AbstractGlyphCache class
documentation for information about setting up a glyph cache instance and
filling it with glyphs, and @ref DistanceFieldGlyphCacheGL for details specific
to distance field processing and used internal texture format. The setup
differs from @ref DistanceFieldGlyphCacheGL only in specifying one extra
dimension for size:

@snippet Text-gl.cpp DistanceFieldGlyphCacheArrayGL-usage

Assuming a @ref RendererGL is used with this cache for rendering the text, its
@relativeref{RendererGL,mesh()} can be then drawn using
@ref Shaders::DistanceFieldVectorGL that has
@ref Shaders::DistanceFieldVectorGL::Flag::TextureArrays enabled, together with
binding @ref texture() for drawing:

@snippet Text-gl.cpp DistanceFieldGlyphCacheArrayGL-usage-draw

@requires_gl30 Extension @gl_extension{EXT,texture_array}
@requires_gles30 Texture arrays are not available in OpenGL ES 2.0.
@requires_webgl20 Texture arrays are not available in WebGL 1.0.

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
class MAGNUM_TEXT_EXPORT DistanceFieldGlyphCacheArrayGL: public GlyphCacheArrayGL {
    public:
        /**
         * @brief Constructor
         * @param size              Size of the source image
         * @param processedSize     Resulting distance field texture size.
         *      Depth of the resulting texture is @cpp size.z() @ce.
         * @param radius            Distance field calculation radius
         *
         * See @ref TextureTools::DistanceFieldGL for more information about
         * the parameters. Size restrictions from it apply here as well, in
         * particular the ratio of @cpp size.xy() @ce and @p processedSize is
         * expected to be a multiple of 2.
         *
         * Sets the @ref processedFormat() to @ref PixelFormat::R8Unorm, if
         * available. On OpenGL ES 3.0+ and WebGL 2 uses
         * @ref PixelFormat::R8Unorm always. On desktop OpenGL requires
         * @gl_extension{ARB,texture_rg} (part of OpenGL 3.0), on ES2 uses
         * @gl_extension{EXT,texture_rg} if available and uses
         * @ref PixelFormat::RGB8Unorm as fallback if not, on WebGL 1 uses
         * @ref PixelFormat::RGB8Unorm always.
         */
        explicit DistanceFieldGlyphCacheArrayGL(const Vector3i& size, const Vector2i& processedSize, UnsignedInt radius);

        /**
         * @brief Construct without creating the internal state and the OpenGL texture object
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
        explicit DistanceFieldGlyphCacheArrayGL(NoCreateT) noexcept;

        /** @brief Distance field calculation radius */
        UnsignedInt radius() const;

    private:
        struct State;

        MAGNUM_TEXT_LOCAL GlyphCacheFeatures doFeatures() const override;
        MAGNUM_TEXT_LOCAL void doSetImage(const Vector3i& offset, const ImageView3D& image) override;
};
#endif

}}
#else
#error this header is available only in the OpenGL build
#endif

#endif
