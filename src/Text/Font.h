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

@section Font-usage Usage

You need to maintain instance of FontRenderer during the lifetime of all Font
instances. The font can be created either from file or from memory location of
format supported by [FreeType](http://www.freetype.org/) library. Next step is
to prerender all the glyphs which will be used in text rendering later.
@code
Text::FontRenderer fontRenderer;

Text::Font font(fontRenderer, "MyFont.ttf", 48.0f);
font.prerender("abcdefghijklmnopqrstuvwxyz"
               "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
               "0123456789 ", Vector2i(512));
@endcode
See TextRenderer for information about text rendering.

@section Font-extensions Required OpenGL functionality

%Font texture uses one-component internal format, which requires
@extension{ARB,texture_rg} (also part of OpenGL ES 3.0 or available as
@es_extension{EXT,texture_rg} in ES 2.0).
*/
class MAGNUM_TEXT_EXPORT Font {
    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;

    public:
        /**
         * @brief Create font from file
         * @param renderer      %Font renderer
         * @param fontFile      %Font file
         * @param size          %Font size
         */
        explicit Font(FontRenderer& renderer, const std::string& fontFile, Float size);

        /**
         * @brief Create font from memory
         * @param renderer      %Font renderer
         * @param data          %Font data
         * @param dataSize      %Font data size
         * @param size          %Font size
         */
        explicit Font(FontRenderer& renderer, const unsigned char* data, std::size_t dataSize, Float size);

        /**
         * @brief Prerender given character set
         * @param characters    UTF-8 characters to render
         * @param atlasSize     Size of resulting atlas
         *
         * Creates new atlas with prerendered characters, replacing the
         * previous one (if any).
         * @attention @p atlasSize must be large enough to contain all
         *      rendered glyphs.
         */
        void prerender(const std::string& characters, const Vector2i& atlasSize);

        /**
         * @brief Prerender given character set for use with distance-field rendering
         * @param characters        UTF-8 characters to render
         * @param sourceAtlasSize   Size of distance field source atlas
         * @param atlasSize         Size of resulting atlas
         * @param radius            Max lookup radius for distance-field creation
         *
         * Creates new atlas with prerendered characters, replacing the
         * previous one (if any). See TextureTools::distanceField() for more
         * information.
         * @attention @p sourceAtlasSize must be large enough to contain all
         *      rendered glyphs with padding given by @p radius.
         */
        void prerenderDistanceField(const std::string& characters, const Vector2i& sourceAtlasSize, const Vector2i& atlasSize, Int radius);

        ~Font();

        /** @brief Move constructor */
        Font(Font&& other);

        /** @brief Move assignment */
        Font& operator=(Font&& other);

        /** @brief %Font size */
        inline Float size() const { return _size; }

        /** @brief Count of prerendered glyphs in the font */
        inline std::size_t glyphCount() const { return glyphs.size(); }

        /**
         * @brief Position of given character in the texture
         * @param character     Unicode character code (UTF-32)
         *
         * First returned rectangle is texture position relative to point on
         * baseline, second is position of the texture in texture atlas.
         */
        const std::tuple<Rectangle, Rectangle>& operator[](char32_t character) const;

        /** @brief %Font texture atlas */
        inline Texture2D& texture() { return _texture; }

        /** @brief Font handle */
        #ifdef MAGNUM_USE_HARFBUZZ
        inline hb_font_t* font() { return _hbFont; }
        #else
        inline FT_Face font() { return _ftFont; }
        #endif

    private:
        void MAGNUM_TEXT_LOCAL finishConstruction();
        void MAGNUM_TEXT_LOCAL destroy();
        void MAGNUM_TEXT_LOCAL move();
        void MAGNUM_TEXT_LOCAL prerenderInternal(const std::string& characters, const Vector2i& atlasSize, const Int radius, Texture2D* output);

        std::unordered_map<char32_t, std::tuple<Rectangle, Rectangle>> glyphs;
        Texture2D _texture;
        FT_Face _ftFont;
        Float _size;
        #ifdef MAGNUM_USE_HARFBUZZ
        hb_font_t* _hbFont;
        #endif
};

}}

#endif
