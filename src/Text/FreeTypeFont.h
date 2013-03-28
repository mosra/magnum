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

@section FreeTypeFont-usage Usage

You need to maintain instance of FreeTypeFontRenderer during the lifetime of
all FreeTypeFont instances. The font can be created either from file or from
memory location of format supported by [FreeType](http://www.freetype.org/)
library.
@code
Text::FreeTypeFontRenderer fontRenderer;

Text::FreeTypeFont font(fontRenderer, "MyFreeTypeFont.ttf", 48.0f);
@endcode
Next step is to prerender all the glyphs which will be used in text rendering
later, see GlyphCache for more information. See TextRenderer for information
about text rendering.
*/
class MAGNUM_TEXT_EXPORT FreeTypeFont: public AbstractFont {
    public:
        /**
         * @brief Create font from file
         * @param renderer      Font renderer
         * @param fontFile      Font file
         * @param size          Font size
         */
        explicit FreeTypeFont(FreeTypeFontRenderer& renderer, const std::string& fontFile, Float size);

        /**
         * @brief Create font from memory
         * @param renderer      Font renderer
         * @param data          Font data
         * @param dataSize      Font data size
         * @param size          Font size
         */
        explicit FreeTypeFont(FreeTypeFontRenderer& renderer, const unsigned char* data, std::size_t dataSize, Float size);

        ~FreeTypeFont();

        /** @brief Font size */
        inline Float size() const { return _size; }

        /** @brief FreeType font handle */
        inline FT_Face font() { return _ftFont; }

        void createGlyphCache(GlyphCache* const cache, const std::string& characters) override;
        AbstractLayouter* layout(const GlyphCache* const cache, const Float size, const std::string& text) override;

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #else
    protected:
    #endif
        FT_Face _ftFont;

    private:
        Float _size;
};

}}

#endif
