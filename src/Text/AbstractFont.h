#ifndef Magnum_Text_AbstractFont_h
#define Magnum_Text_AbstractFont_h
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
        explicit AbstractFont();
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
        explicit AbstractLayouter();
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
