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

#include "HarfBuzzFont.h"

#include <hb-ft.h>

namespace Magnum { namespace Text {

namespace {

class HarfBuzzLayouter: public AbstractLayouter {
    public:
        HarfBuzzLayouter(HarfBuzzFont& font, const Float size, const std::string& text);
        ~HarfBuzzLayouter();

        std::tuple<Rectangle, Rectangle, Vector2> renderGlyph(const Vector2& cursorPosition, const UnsignedInt i) override;

    private:
        const HarfBuzzFont& font;
        hb_buffer_t* buffer;
        hb_glyph_info_t* glyphInfo;
        hb_glyph_position_t* glyphPositions;
        const Float size;
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
    _hbFont = hb_ft_font_create(_ftFont, nullptr);
}

HarfBuzzFont::~HarfBuzzFont() {
    hb_font_destroy(_hbFont);
}

AbstractLayouter* HarfBuzzFont::layout(const Float size, const std::string& text) {
    return new HarfBuzzLayouter(*this, size, text);
}

namespace {

HarfBuzzLayouter::HarfBuzzLayouter(HarfBuzzFont& font, const Float size, const std::string& text): font(font), size(size) {
    /* Prepare HarfBuzz buffer */
    buffer = hb_buffer_create();
    hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
    hb_buffer_set_script(buffer, HB_SCRIPT_LATIN);
    hb_buffer_set_language(buffer, hb_language_from_string("en", 2));

    /* Layout the text */
    hb_buffer_add_utf8(buffer, text.c_str(), -1, 0, -1);
    hb_shape(font.font(), buffer, nullptr, 0);

    glyphInfo = hb_buffer_get_glyph_infos(buffer, &_glyphCount);
    glyphPositions = hb_buffer_get_glyph_positions(buffer, &_glyphCount);
}

HarfBuzzLayouter::~HarfBuzzLayouter() {
    /* Destroy HarfBuzz buffer */
    hb_buffer_destroy(buffer);
}

std::tuple<Rectangle, Rectangle, Vector2> HarfBuzzLayouter::renderGlyph(const Vector2& cursorPosition, const UnsignedInt i) {
    /* Position of the texture in the resulting glyph, texture coordinates */
    Rectangle texturePosition, textureCoordinates;
    std::tie(texturePosition, textureCoordinates) = font[glyphInfo[i].codepoint];

    /* Glyph offset and advance to next glyph in normalized coordinates */
    Vector2 offset = Vector2(glyphPositions[i].x_offset,
                             glyphPositions[i].y_offset)/(64*font.size());
    Vector2 advance = Vector2(glyphPositions[i].x_advance,
                              glyphPositions[i].y_advance)/(64*font.size());

    /* Absolute quad position, composed from cursor position, glyph offset
        and texture position, denormalized to requested text size */
    Rectangle quadPosition = Rectangle::fromSize(
        (cursorPosition + offset + Vector2(texturePosition.left(), texturePosition.bottom()))*size,
        texturePosition.size()*size);

    return std::make_tuple(quadPosition, textureCoordinates, advance);
}

}

}}
