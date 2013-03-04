#ifndef Magnum_Text_HarfBuzzFont_h
#define Magnum_Text_HarfBuzzFont_h
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
         * @param renderer      %Font renderer
         * @param fontFile      %Font file
         * @param size          %Font size
         */
        explicit HarfBuzzFont(FreeTypeFontRenderer& renderer, const std::string& fontFile, Float size);

        /**
         * @brief Create font from memory
         * @param renderer      %Font renderer
         * @param data          %Font data
         * @param dataSize      %Font data size
         * @param size          %Font size
         */
        explicit HarfBuzzFont(FreeTypeFontRenderer& renderer, const unsigned char* data, std::size_t dataSize, Float size);

        ~HarfBuzzFont();

        /** @brief HarfBuzz font handle */
        inline hb_font_t* font() { return _hbFont; }

        AbstractLayouter* layout(const Float size, const std::string& text) override;

    private:
        void MAGNUM_TEXT_LOCAL finishConstruction();

        hb_font_t* _hbFont;
};

}}

#endif
