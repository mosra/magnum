#ifndef Magnum_Text_DistanceFieldGlyphCache_h
#define Magnum_Text_DistanceFieldGlyphCache_h
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

/** @file
 * @brief Class Magnum::Text::DistanceFieldGlyphCache
 */

#include "Text/GlyphCache.h"

namespace Magnum { namespace Text {

/**
@brief Glyph cache with distance field rendering

Unlike original GlyphCache converts each binary image to distance field. It is
not possible to use non-binary colors with this cache, internal texture format
is red channel only.

@section GlyphCache-usage Usage

Usage is similar to GlyphCache, additionaly you need to specify size of
resulting distance field texture.
@code
Text::AbstractFont* font;
Text::GlyphCache* cache = new Text::DistanceFieldGlyphCache(Vector2i(2048), Vector2i(384));
font->createGlyphCache(cache, "abcdefghijklmnopqrstuvwxyz"
                              "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                              "0123456789 ");
@endcode

@see TextureTools::distanceField()
*/
class MAGNUM_TEXT_EXPORT DistanceFieldGlyphCache: public GlyphCache {
    public:
        /**
         * @brief Constructor
         * @param originalSize      Original cache texture size
         * @param distanceFieldSize Size of computed distance field texture
         * @param radius            Distance field computation radius
         *
         * See TextureTools::distanceField() for more information about the
         * parameters.
         */
        explicit DistanceFieldGlyphCache(const Vector2i& originalSize, const Vector2i& distanceFieldSize, UnsignedInt radius);

        /**
         * @brief Set cache image
         *
         * Uploads image for one or more glyphs to given offset in original
         * cache texture. The texture is then converted to distance field.
         */
        void setImage(const Vector2i& offset, Image2D* const image) override;

        /**
         * @brief Set distance field cache image
         *
         * Uploads already computed distance field image to given offset in
         * distance field texture.
         */
        void setDistanceFieldImage(const Vector2i& offset, Image2D* const image);

    private:
        const Vector2 scale;
        const UnsignedInt radius;
};

}}

#endif
