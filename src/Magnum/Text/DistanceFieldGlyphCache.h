#ifndef Magnum_Text_DistanceFieldGlyphCache_h
#define Magnum_Text_DistanceFieldGlyphCache_h
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

/** @file
 * @brief Class @ref Magnum::Text::DistanceFieldGlyphCache
 */

#include "Magnum/Text/GlyphCache.h"

namespace Magnum { namespace Text {

/**
@brief Glyph cache with distance field rendering

Unlike original @ref GlyphCache converts each binary image to distance field.
It is not possible to use non-binary colors with this cache, internal texture
format is red channel only.

## Usage

Usage is similar to @ref GlyphCache, additionally you need to specify size of
resulting distance field texture.
@code
Text::AbstractFont* font;
Text::GlyphCache* cache = new Text::DistanceFieldGlyphCache(Vector2i(2048), Vector2i(384));
font->createGlyphCache(cache, "abcdefghijklmnopqrstuvwxyz"
                              "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                              "0123456789?!:;,. ");
@endcode

@see @ref TextureTools::distanceField()
*/
class MAGNUM_TEXT_EXPORT DistanceFieldGlyphCache: public GlyphCache {
    public:
        /**
         * @brief Constructor
         * @param originalSize      Unscaled glyph cache texture size
         * @param size              Actual glyph cache texture size
         * @param radius            Distance field computation radius
         *
         * See @ref TextureTools::distanceField() for more information about
         * the parameters. Sets internal texture format to red channel only. On
         * desktop OpenGL requires @extension{ARB,texture_rg} (also part of
         * OpenGL ES 3.0), in ES2 uses @extension{EXT,texture_rg} if
         * available or @ref TextureFormat::RGB as fallback.
         * @todo Is Luminance format renderable anywhere? Also would it be
         *      possible to convert the RGB texture to Luminance after it has
         *      been rendered when blitting is not supported to save memory?
         */
        explicit DistanceFieldGlyphCache(const Vector2i& originalSize, const Vector2i& size, UnsignedInt radius);

        /**
         * @brief Set cache image
         *
         * Uploads image for one or more glyphs to given offset in original
         * cache texture. The texture is then converted to distance field.
         */
        void setImage(const Vector2i& offset, const ImageView2D& image) override;

        /**
         * @brief Set distance field cache image
         *
         * Uploads already computed distance field image to given offset in
         * distance field texture.
         */
        void setDistanceFieldImage(const Vector2i& offset, const ImageView2D& image);

    private:
        const Vector2 scale;
        const UnsignedInt radius;
};

}}

#endif
