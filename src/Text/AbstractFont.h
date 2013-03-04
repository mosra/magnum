#ifndef Magnum_Text_AbstractFont_h
#define Magnum_Text_AbstractFont_h
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
 * @brief Class Magnum::Text::AbstractFont, Magnum::Text::AbstractLayouter
 */

#include <tuple>
#include <string>

#include "Magnum.h"
#include "Texture.h"
#include "Text/Text.h"
#include "Text/magnumTextVisibility.h"

namespace Magnum { namespace Text {

/**
@brief Base for fonts
*/
class MAGNUM_TEXT_EXPORT AbstractFont {
    AbstractFont(const AbstractFont&) = delete;
    AbstractFont(AbstractFont&&) = delete;
    AbstractFont& operator=(const AbstractFont&) = delete;
    AbstractFont& operator=(const AbstractFont&&) = delete;

    public:
        AbstractFont();
        virtual ~AbstractFont() = 0;

        /** @brief %Font texture atlas */
        inline Texture2D& texture() { return _texture; }

        /**
         * @brief Layout the text using fon't own layouter
         * @param size      %Font size
         * @param text      Text to layout
         */
        virtual AbstractLayouter* layout(const Float size, const std::string& text) = 0;

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #else
    protected:
    #endif
        Texture2D _texture;
};

/**
@brief Base for text layouters

Returned by AbstractFont::layout().
*/
class MAGNUM_TEXT_EXPORT AbstractLayouter {
    AbstractLayouter(const AbstractLayouter&) = delete;
    AbstractLayouter(AbstractLayouter&&) = delete;
    AbstractLayouter& operator=(const AbstractLayouter&) = delete;
    AbstractLayouter& operator=(const AbstractLayouter&&) = delete;

    public:
        AbstractLayouter();
        virtual ~AbstractLayouter() = 0;

        /** @brief Count of glyphs in laid out text */
        inline UnsignedInt glyphCount() const {
            return _glyphCount;
        }

        /**
         * @brief Render glyph
         * @param i                 Glyph index
         * @param cursorPosition    Cursor position
         *
         * Returns quad position, texture coordinates and advance to next
         * glyph.
         */
        virtual std::tuple<Rectangle, Rectangle, Vector2> renderGlyph(const Vector2& cursorPosition, const UnsignedInt i) = 0;

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #else
    protected:
    #endif
        UnsignedInt _glyphCount;
};

}}

#endif
