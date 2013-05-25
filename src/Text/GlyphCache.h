#ifndef Magnum_Text_GlyphCache_h
#define Magnum_Text_GlyphCache_h
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
 * @brief Class Magnum::Text::GlyphCache
 */

#include <unordered_map>

#include "Math/Geometry/Rectangle.h"
#include "Texture.h"
#include "Text/magnumTextVisibility.h"

namespace Magnum { namespace Text {

/**
@brief Glyph cache

Contains font glyphs prerendered into texture atlas.

@section GlyphCache-usage Usage

Create %GlyphCache object with sufficient size and then call
AbstractFont::createGlyphCache() to fill it with glyphs.
@code
Text::AbstractFont* font;
Text::GlyphCache* cache = new GlyphCache(Vector2i(512));
font->createGlyphCache(cache, "abcdefghijklmnopqrstuvwxyz"
                              "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                              "0123456789 ");
@endcode

See TextRenderer for information about text rendering.
*/
class MAGNUM_TEXT_EXPORT GlyphCache {
    public:
        /**
         * @brief Constructor
         * @param size              Glyph cache texture size
         * @param internalFormat    Internal texture format
         */
        explicit GlyphCache(const Vector2i& size, TextureFormat internalFormat);

        /**
         * @brief Constructor
         * @param size              Glyph cache texture size
         *
         * Sets internal texture format to red channel only. Requires
         * @extension{ARB,texture_rg} (also part of OpenGL ES 3.0 or available
         * as @es_extension{EXT,texture_rg} in ES 2.0).
         */
        explicit GlyphCache(const Vector2i& size);

        virtual ~GlyphCache();

        /**
         * @brief Cache size
         *
         * Size of unscaled glyph cache texture.
         */
        Vector2i textureSize() const { return _size; }

        /** @brief Count of glyphs in the cache */
        std::size_t glyphCount() const { return glyphs.size(); }

        /** @brief Cache texture */
        Texture2D* texture() { return &_texture; }

        /**
         * @brief Parameters of given glyph
         * @param glyph         Glyph ID
         *
         * First tuple element is glyph position relative to point on baseline,
         * second element is glyph region in texture atlas. If no glyph is
         * found, glyph on zero index is returned.
         */
        std::pair<Vector2i, Rectanglei> operator[](UnsignedInt glyph) const {
            auto it = glyphs.find(glyph);
            return it == glyphs.end() ? glyphs.at(0) : it->second;
        }

        /**
         * @brief Layout glyphs with given sizes to the cache
         *
         * Returns non-overlapping regions in cache texture to store glyphs.
         * The reserved space is reused on next call to reserve() if no glyph
         * was stored there, use insert() to store actual glyph on given
         * position and setImage() to upload glyph image.
         *
         * @attention Cache size must be large enough to contain all rendered
         *      glyphs.
         */
        std::vector<Rectanglei> reserve(const std::vector<Vector2i>& sizes);

        /**
         * @brief Insert glyph to cache
         * @param glyph         Glyph ID
         * @param position      Position relative to point on baseline
         * @param rectangle     Region in texture atlas
         *
         * You can obtain unused non-overlapping regions with reserve(). See
         * also setImage() to upload glyph image.
         */
        void insert(UnsignedInt glyph, Vector2i position, Rectanglei rectangle);

        /**
         * @brief Set cache image
         *
         * Uploads image for one or more glyphs to given offset in cache
         * texture.
         */
        virtual void setImage(const Vector2i& offset, Image2D* image);

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #else
    protected:
    #endif
        /* Used from DistanceFieldGlyphCache */
        explicit MAGNUM_LOCAL GlyphCache(const Vector2i& size, const Vector2i& padding);

        void MAGNUM_LOCAL initialize(TextureFormat internalFormat, const Vector2i& size);

        const Vector2i _size;
        Texture2D _texture;

    private:
        const Vector2i _padding;
        std::unordered_map<UnsignedInt, std::pair<Vector2i, Rectanglei>> glyphs;
};

}}

#endif
