#ifndef Magnum_Text_Font_h
#define Magnum_Text_Font_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Class Magnum::Text::Font
 */

#include <unordered_map>

#include "Math/Geometry/Rectangle.h"
#include "Texture.h"
#include "Text/FontRenderer.h"

#include "magnumTextVisibility.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
struct FT_FaceRec_;
typedef FT_FaceRec_*  FT_Face;
struct hb_font_t;
#endif

namespace Magnum { namespace Text {

/**
@brief %Font

Contains font with characters prerendered into texture atlas.
*/
class MAGNUM_TEXT_EXPORT Font {
    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;

    public:
        /**
         * @brief Empty font constructor
         * @param renderer      %Font renderer
         * @param fontFile      %Font file
         * @param size          %Font size
         *
         * Creates font with no prerendered characters.
         */
        Font(FontRenderer& renderer, const std::string& fontFile, GLfloat size);

        /**
         * @brief Create font for given character set
         * @param renderer      %Font renderer
         * @param fontFile      %Font file
         * @param size          %Font size
         * @param characters    Characters to render
         * @param atlasSize     Size of resulting atlas
         */
        Font(FontRenderer& renderer, const std::string& fontFile, GLfloat size, const std::string& characters, const Vector2i& atlasSize);

        ~Font();

        /** @brief Move constructor */
        Font(Font&& other);

        /** @brief Move assignment */
        Font& operator=(Font&& other);

        /** @brief %Font size */
        inline GLfloat size() const { return _size; }

        /** @brief Count of prerendered glyphs in the font */
        inline std::size_t glyphCount() const { return glyphs.size(); }

        /**
         * @brief Position of given character in the texture
         * @param character     Unicode character code (UTF-32)
         *
         * First returned rectangle is texture position relative to point on
         * baseline, second is position of the texture in texture atlas.
         */
        const std::tuple<Rectangle, Rectangle>& operator[](std::uint32_t character) const;

        /** @brief %Font texture atlas */
        inline Texture2D& texture() { return _texture; }

        /** @brief HarfBuzz font handle */
        inline hb_font_t* font() { return _hbFont; }

    private:
        void MAGNUM_TEXT_LOCAL create(FontRenderer& renderer, const std::string& fontFile);
        void MAGNUM_TEXT_LOCAL destroy();
        void MAGNUM_TEXT_LOCAL move();

        std::unordered_map<std::uint32_t, std::tuple<Rectangle, Rectangle>> glyphs;
        Texture2D _texture;
        FT_Face _ftFont;
        hb_font_t* _hbFont;
        GLfloat _size;
};

}}

#endif
