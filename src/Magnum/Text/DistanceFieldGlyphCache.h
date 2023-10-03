#ifndef Magnum_Text_DistanceFieldGlyphCache_h
#define Magnum_Text_DistanceFieldGlyphCache_h
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
 * @brief Class @ref Magnum::Text::DistanceFieldGlyphCache
 */

#include "Magnum/configure.h"

#ifdef MAGNUM_TARGET_GL
#include "Magnum/Text/GlyphCache.h"
#include "Magnum/TextureTools/DistanceField.h"

namespace Magnum { namespace Text {

/**
@brief Glyph cache with distance field rendering

Unlike the base @ref GlyphCache, this class converts each binary image to a
distance field. It's not possible to use non-binary colors with this cache as
the internal texture format is single-channel.

@section Text-DistanceFieldGlyphCache-usage Usage

In order to create a distance field glyph cache, the font has to be loaded at a
size significantly larger than what the resulting text will be. The distance
field conversion process then converts the input to a fraction of its size
again, transferring the the extra spatial resolution to distance values. The
distance values are then used to render an arbitrarily sized text without it
being jaggy at small sizes and blurry when large.

The process requires three input parameters, size of the source image, size of
the resulting glyph cache image and a radius for the distance field creation.
The ratio between the input and output image size is usually four or eight
times, and the size of the font should match the larger size. So, for example,
if a @cpp {128, 128} @ce @ref GlyphCache was filled with a 12 pt font, a
@cpp {1024, 1024} @ce source image for the distance field should use a 96 pt
font. The radius should then be chosen so it's at least one or two pixels in
the scaled-down result, so in this case at least 8. Values less than that will
result in aliasing artifacts. Very high radius values are needed only if
outlining, thinning, thickening or shadow effects will be used when rendering,
using them leads to precision loss when the distance field is stored in 8-bit
channels.

@snippet MagnumText-gl.cpp DistanceFieldGlyphCache-usage

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@see @ref TextureTools::DistanceField
*/
class MAGNUM_TEXT_EXPORT DistanceFieldGlyphCache: public GlyphCache {
    public:
        /**
         * @brief Constructor
         * @param sourceSize        Size of the source image
         * @param size              Resulting distance field texture size
         * @param radius            Distance field computation radius
         *
         * See @ref TextureTools::DistanceField for more information about the
         * parameters. Sets the internal texture format to single-channel.
         * On OpenGL ES 3.0+ and WebGL 2 uses @ref GL::TextureFormat::R8. On
         * desktop OpenGL requires @gl_extension{ARB,texture_rg} (part of
         * OpenGL 3.0), on ES2 uses @gl_extension{EXT,texture_rg} if available
         * or @ref GL::TextureFormat::RGB as fallback, on WebGL 1 uses
         * @ref GL::TextureFormat::RGB always.
         */
        explicit DistanceFieldGlyphCache(const Vector2i& sourceSize, const Vector2i& size, UnsignedInt radius);

        /**
         * @brief Distance field texture size
         *
         * Compared to @ref textureSize(), which is the size of the source
         * image, this function returns size of the resulting distance field
         * texture.
         */
        Vector2i distanceFieldTextureSize() const { return _size; }

        /**
         * @brief Set a distance field cache image
         *
         * Compared to @ref setImage() uploads an already computed distance
         * field image to given offset in the distance field texture. The
         * @p offset and @ref ImageView::size() are expected to be in bounds
         * for @ref distanceFieldTextureSize().
         */
        void setDistanceFieldImage(const Vector2i& offset, const ImageView2D& image);

    private:
        MAGNUM_TEXT_LOCAL void doSetImage(const Vector2i& offset, const ImageView2D& image) override;

        Vector2i _size;
        TextureTools::DistanceField _distanceField;
};

}}
#else
#error this header is available only in the OpenGL build
#endif

#endif
