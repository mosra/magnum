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
#include <PluginManager/AbstractPlugin.h>

#include "Magnum.h"
#include "Texture.h"
#include "Text/Text.h"
#include "Text/magnumTextVisibility.h"

namespace Magnum { namespace Text {

/**
@brief Base for font plugins

@section AbstractFont-usage Usage

First step is to open the font using open(), next step is to prerender all the
glyphs which will be used in text rendering later, see GlyphCache for more
information. See TextRenderer for information about text rendering.

@section AbstractFont-subclassing Subclassing

Plugin implements functions open(), close(), createGlyphCache() and layout().
*/
class MAGNUM_TEXT_EXPORT AbstractFont: public Corrade::PluginManager::AbstractPlugin {
    PLUGIN_INTERFACE("cz.mosra.magnum.Text.AbstractFont/0.1")

    public:
        /** @brief Default constructor */
        explicit AbstractFont();

        /** @brief Plugin manager constructor */
        explicit AbstractFont(Corrade::PluginManager::AbstractManager* manager, std::string plugin);

        /**
         * @brief Open font from file
         * @param filename      Font file
         * @param size          Font size
         *
         * Closes previous file, if it was opened, and tries to open given
         * file. Returns `true` on success, `false` otherwise.
         */
        virtual bool open(const std::string& filename, Float size) = 0;

        /**
         * @brief Open font from memory
         * @param data          Font data
         * @param dataSize      Font data size
         * @param size          Font size
         *
         * Closes previous file, if it was opened, and tries to open given
         * file. Returns `true` on success, `false` otherwise.
         */
        virtual bool open(const unsigned char* data, std::size_t dataSize, Float size) = 0;

        /** @brief Close font */
        virtual void close() = 0;

        /** @brief Font size */
        inline Float size() const { return _size; }

        /**
         * @brief Create glyph cache for given character set
         * @param cache         Glyph cache instance
         * @param characters    UTF-8 characters to render
         *
         * Fills the cache with given characters.
         */
        virtual void createGlyphCache(GlyphCache* cache, const std::string& characters) = 0;

        /**
         * @brief Layout the text using font own layouter
         * @param cache     Glyph cache
         * @param size      Font size
         * @param text      %Text to layout
         *
         * @see createGlyphCache()
         */
        virtual AbstractLayouter* layout(const GlyphCache* cache, Float size, const std::string& text) = 0;

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #else
    protected:
    #endif
        Float _size;
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
        virtual std::tuple<Rectangle, Rectangle, Vector2> renderGlyph(const Vector2& cursorPosition, UnsignedInt i) = 0;

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #else
    protected:
    #endif
        UnsignedInt _glyphCount;
};

}}

#endif
