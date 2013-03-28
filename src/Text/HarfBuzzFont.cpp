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

#include "HarfBuzzFont.h"

#include <hb-ft.h>

#include "GlyphCache.h"

namespace Magnum { namespace Text {

namespace {

class HarfBuzzLayouter: public AbstractLayouter {
    public:
        explicit HarfBuzzLayouter(hb_font_t* const font, const GlyphCache* const cache, const Float fontSize, const Float textSize, const std::string& text);
        ~HarfBuzzLayouter();

        std::tuple<Rectangle, Rectangle, Vector2> renderGlyph(const Vector2& cursorPosition, const UnsignedInt i) override;

    private:
        const hb_font_t* const font;
        const GlyphCache* const cache;
        const Float fontSize, textSize;
        hb_buffer_t* buffer;
        hb_glyph_info_t* glyphInfo;
        hb_glyph_position_t* glyphPositions;
};

}

HarfBuzzFont::HarfBuzzFont(FreeTypeFontRenderer& renderer, const std::string& fontFile, Float size): FreeTypeFont(renderer, fontFile, size) {
    finishConstruction();
}

HarfBuzzFont::HarfBuzzFont(FreeTypeFontRenderer& renderer, const unsigned char* data, std::size_t dataSize, Float size): FreeTypeFont(renderer, data, dataSize, size) {
    finishConstruction();
}

void HarfBuzzFont::finishConstruction() {
    /* Create Harfbuzz font */
    hbFont = hb_ft_font_create(ftFont, nullptr);
}

HarfBuzzFont::~HarfBuzzFont() {
    hb_font_destroy(hbFont);
}

AbstractLayouter* HarfBuzzFont::layout(const GlyphCache* const cache, const Float size, const std::string& text) {
    return new HarfBuzzLayouter(hbFont, cache, this->size(), size, text);
}

namespace {

HarfBuzzLayouter::HarfBuzzLayouter(hb_font_t* const font, const GlyphCache* const cache, const Float fontSize, const Float textSize, const std::string& text): font(font), cache(cache), fontSize(fontSize), textSize(textSize) {
    /* Prepare HarfBuzz buffer */
    buffer = hb_buffer_create();
    hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
    hb_buffer_set_script(buffer, HB_SCRIPT_LATIN);
    hb_buffer_set_language(buffer, hb_language_from_string("en", 2));

    /* Layout the text */
    hb_buffer_add_utf8(buffer, text.c_str(), -1, 0, -1);
    hb_shape(font, buffer, nullptr, 0);

    glyphInfo = hb_buffer_get_glyph_infos(buffer, &_glyphCount);
    glyphPositions = hb_buffer_get_glyph_positions(buffer, &_glyphCount);
}

HarfBuzzLayouter::~HarfBuzzLayouter() {
    /* Destroy HarfBuzz buffer */
    hb_buffer_destroy(buffer);
}

std::tuple<Rectangle, Rectangle, Vector2> HarfBuzzLayouter::renderGlyph(const Vector2& cursorPosition, const UnsignedInt i) {
    /* Position of the texture in the resulting glyph, texture coordinates */
    Vector2i position;
    Rectanglei rectangle;
    std::tie(position, rectangle) = (*cache)[glyphInfo[i].codepoint];

    Rectangle texturePosition = Rectangle::fromSize(Vector2(position)/fontSize,
                                                    Vector2(rectangle.size())/fontSize);
    Rectangle textureCoordinates(Vector2(rectangle.bottomLeft())/cache->textureSize(),
                                 Vector2(rectangle.topRight())/cache->textureSize());

    /* Glyph offset and advance to next glyph in normalized coordinates */
    Vector2 offset = Vector2(glyphPositions[i].x_offset,
                             glyphPositions[i].y_offset)/(64*fontSize);
    Vector2 advance = Vector2(glyphPositions[i].x_advance,
                              glyphPositions[i].y_advance)/(64*fontSize);

    /* Absolute quad position, composed from cursor position, glyph offset
        and texture position, denormalized to requested text size */
    Rectangle quadPosition = Rectangle::fromSize(
        (cursorPosition + offset + Vector2(texturePosition.left(), texturePosition.bottom()))*textSize,
        texturePosition.size()*textSize);

    return std::make_tuple(quadPosition, textureCoordinates, advance);
}

}

}}
