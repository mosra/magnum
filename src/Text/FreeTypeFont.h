#ifndef Magnum_Text_FreeTypeFont_h
#define Magnum_Text_FreeTypeFont_h
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
 * @brief Class Magnum::Text::FreeTypeFontRenderer, Magnum::Text::FreeTypeFont
 */

#include <unordered_map>

#include "Math/Geometry/Rectangle.h"
#include "Texture.h"
#include "Text/AbstractFont.h"
#include "Text/magnumTextVisibility.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
struct FT_LibraryRec_;
typedef FT_LibraryRec_* FT_Library;
struct FT_FaceRec_;
typedef FT_FaceRec_*  FT_Face;
#endif

namespace Magnum { namespace Text {

/**
@brief FreeType font renderer

Contains global instance of font renderer. See FreeTypeFont class documentation
for more information.
*/
class MAGNUM_TEXT_EXPORT FreeTypeFontRenderer {
    public:
        explicit FreeTypeFontRenderer();

        ~FreeTypeFontRenderer();

        /** @brief FreeType library handle */
        inline FT_Library library() { return _library; }

    private:
        FT_Library _library;
};

/**
@brief FreeType font

Contains font with characters prerendered into texture atlas.

@section FreeTypeFont-usage Usage

You need to maintain instance of FreeTypeFontRenderer during the lifetime of all FreeTypeFont
instances. The font can be created either from file or from memory location of
format supported by [FreeType](http://www.freetype.org/) library. Next step is
to prerender all the glyphs which will be used in text rendering later.
@code
Text::FreeTypeFontRenderer fontRenderer;

Text::FreeTypeFont font(fontRenderer, "MyFreeTypeFont.ttf", 48.0f);
font.prerender("abcdefghijklmnopqrstuvwxyz"
               "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
               "0123456789 ", Vector2i(512));
@endcode
See TextRenderer for information about text rendering.

@section FreeTypeFont-extensions Required OpenGL functionality

%Font texture uses one-component internal format, which requires
@extension{ARB,texture_rg} (also part of OpenGL ES 3.0 or available as
@es_extension{EXT,texture_rg} in ES 2.0).
*/
class MAGNUM_TEXT_EXPORT FreeTypeFont: public AbstractFont {
    public:
        /**
         * @brief Create font from file
         * @param renderer      %Font renderer
         * @param fontFile      %Font file
         * @param size          %Font size
         */
        explicit FreeTypeFont(FreeTypeFontRenderer& renderer, const std::string& fontFile, Float size);

        /**
         * @brief Create font from memory
         * @param renderer      %Font renderer
         * @param data          %Font data
         * @param dataSize      %Font data size
         * @param size          %Font size
         */
        explicit FreeTypeFont(FreeTypeFontRenderer& renderer, const unsigned char* data, std::size_t dataSize, Float size);

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

        ~FreeTypeFont();

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

        /** @brief FreeType font handle */
        inline FT_Face font() { return _ftFont; }

        AbstractLayouter* layout(const Float size, const std::string& text) override;

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #else
    protected:
    #endif
        FT_Face _ftFont;

    private:
        void MAGNUM_TEXT_LOCAL finishConstruction();
        void MAGNUM_TEXT_LOCAL prerenderInternal(const std::string& characters, const Vector2i& atlasSize, const Int radius, Texture2D* output);

        std::unordered_map<char32_t, std::tuple<Rectangle, Rectangle>> glyphs;
        Float _size;
};

}}

#endif
