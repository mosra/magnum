#ifndef Magnum_Text_HarfBuzzFont_h
#define Magnum_Text_HarfBuzzFont_h
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
 * @brief Class Magnum::Text::HarfBuzzFont
 */

#include "Text/FreeTypeFont.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
struct hb_font_t;
#endif

#ifndef MAGNUM_USE_HARFBUZZ
#error Magnum is not compiled with HarfBuzz support
#endif

namespace Magnum { namespace Text {

/**
@brief HarfBuzz font

Improves FreeTypeFont with [HarfBuzz](http://www.freedesktop.org/wiki/Software/HarfBuzz)
text layouting capabilities, such as kerning, ligatures etc. See FreeTypeFont
class documentation for more information about usage.
*/
class MAGNUM_TEXT_EXPORT HarfBuzzFont: public FreeTypeFont {
    public:
        /**
         * @brief Create font from file
         * @param renderer      Font renderer
         * @param fontFile      Font file
         * @param size          Font size
         */
        explicit HarfBuzzFont(FreeTypeFontRenderer& renderer, const std::string& fontFile, Float size);

        /**
         * @brief Create font from memory
         * @param renderer      Font renderer
         * @param data          Font data
         * @param dataSize      Font data size
         * @param size          Font size
         */
        explicit HarfBuzzFont(FreeTypeFontRenderer& renderer, const unsigned char* data, std::size_t dataSize, Float size);

        ~HarfBuzzFont();

        AbstractLayouter* layout(const GlyphCache* const cache, const Float size, const std::string& text) override;

    private:
        void MAGNUM_TEXT_LOCAL finishConstruction();

        hb_font_t* hbFont;
};

}}

#endif
