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

#include "Font.h"

#include <algorithm>
#include <ft2build.h>
#include FT_FREETYPE_H
#ifdef MAGNUM_USE_HARFBUZZ
#include <hb-ft.h>
#endif
#include <Utility/Unicode.h>

#include "Extensions.h"
#include "Image.h"
#include "TextureTools/Atlas.h"

namespace Magnum { namespace Text {

Font::Font(FontRenderer& renderer, const std::string& fontFile, Float size): _size(size) {
    CORRADE_INTERNAL_ASSERT_OUTPUT(FT_New_Face(renderer.library(), fontFile.c_str(), 0, &_ftFont) == 0);

    finishConstruction();
}

Font::Font(FontRenderer& renderer, const unsigned char* data, std::size_t dataSize, Float size): _size(size) {
    CORRADE_INTERNAL_ASSERT_OUTPUT(FT_New_Memory_Face(renderer.library(), data, dataSize, 0, &_ftFont) == 0);

    finishConstruction();
}

void Font::finishConstruction() {
    CORRADE_INTERNAL_ASSERT_OUTPUT(FT_Set_Char_Size(_ftFont, 0, _size*64, 100, 100) == 0);

    #ifdef MAGNUM_USE_HARFBUZZ
    /* Create Harfbuzz font */
    _hbFont = hb_ft_font_create(_ftFont, nullptr);
    #endif

    #ifndef MAGNUM_TARGET_GLES
    MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::ARB::texture_rg);
    #else
    MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::EXT::texture_rg);
    #endif

    /* Set up the texture */
    _texture.setWrapping(Texture2D::Wrapping::ClampToEdge)
        ->setMinificationFilter(Texture2D::Filter::LinearInterpolation)
        ->setMagnificationFilter(Texture2D::Filter::LinearInterpolation);
}

void Font::prerender(const std::string& characters, const Vector2i& atlasSize) {
    glyphs.clear();

    /** @bug Crash when atlas is too small */

    /* Get glyph codes from characters */
    std::vector<FT_UInt> charIndices;
    charIndices.reserve(characters.size()+1);
    charIndices.push_back(0);
    for(std::size_t i = 0; i != characters.size(); ) {
        UnsignedInt codepoint;
        std::tie(codepoint, i) = Corrade::Utility::Unicode::nextChar(characters, i);
        charIndices.push_back(FT_Get_Char_Index(_ftFont, codepoint));
    }

    /* Remove duplicates (e.g. uppercase and lowercase mapped to same glyph) */
    std::sort(charIndices.begin(), charIndices.end());
    charIndices.erase(std::unique(charIndices.begin(), charIndices.end()), charIndices.end());

    /* Sizes of all characters */
    std::vector<Vector2i> charSizes;
    charSizes.reserve(charIndices.size());
    for(FT_UInt c: charIndices) {
        CORRADE_INTERNAL_ASSERT_OUTPUT(FT_Load_Glyph(_ftFont, c, FT_LOAD_DEFAULT) == 0);
        charSizes.push_back(Vector2i(_ftFont->glyph->metrics.width, _ftFont->glyph->metrics.height)/64);
    }

    /* Create texture atlas */
    const std::vector<Rectanglei> charPositions = TextureTools::atlas(atlasSize, charSizes);

    /* Render all characters to the atlas and create character map */
    glyphs.reserve(charPositions.size());
    unsigned char* pixmap = new unsigned char[atlasSize.product()]();
    Image2D image(atlasSize, Image2D::Format::Red, Image2D::Type::UnsignedByte, pixmap);
    for(std::size_t i = 0; i != charPositions.size(); ++i) {
        /* Load and render glyph */
        /** @todo B&W only */
        FT_GlyphSlot glyph = _ftFont->glyph;
        CORRADE_INTERNAL_ASSERT_OUTPUT(FT_Load_Glyph(_ftFont, charIndices[i], FT_LOAD_DEFAULT) == 0);
        CORRADE_INTERNAL_ASSERT_OUTPUT(FT_Render_Glyph(glyph, FT_RENDER_MODE_NORMAL) == 0);

        /* Copy rendered bitmap to texture image */
        const FT_Bitmap& bitmap = glyph->bitmap;
        CORRADE_INTERNAL_ASSERT(std::abs(bitmap.width-charPositions[i].width()) <= 2);
        CORRADE_INTERNAL_ASSERT(std::abs(bitmap.rows-charPositions[i].height()) <= 2);
        for(Int yin = 0, yout = charPositions[i].bottom(), ymax = bitmap.rows; yin != ymax; ++yin, ++yout)
            for(Int xin = 0, xout = charPositions[i].left(), xmax = bitmap.width; xin != xmax; ++xin, ++xout)
                pixmap[yout*atlasSize.x() + xout] = bitmap.buffer[(bitmap.rows-yin-1)*bitmap.width + xin];

        /* Save character texture position and texture coordinates for given character index */
        CORRADE_INTERNAL_ASSERT_OUTPUT(glyphs.insert({charIndices[i], std::make_tuple(
            Rectangle::fromSize(Vector2(glyph->bitmap_left, glyph->bitmap_top-charPositions[i].height())/_size,
                                Vector2(charPositions[i].size())/_size),
            Rectangle(Vector2(charPositions[i].bottomLeft())/atlasSize,
                      Vector2(charPositions[i].topRight())/atlasSize)
        )}).second);
    }

    /* Set texture data */
    #ifndef MAGNUM_TARGET_GLES
    _texture.setImage(0, Texture2D::InternalFormat::R8, &image);
    #else
    _texture.setImage(0, Texture2D::InternalFormat::Red, &image);
    #endif
}

void Font::destroy() {
    if(!_ftFont) return;

    #ifdef MAGNUM_USE_HARFBUZZ
    hb_font_destroy(_hbFont);
    #endif
    FT_Done_Face(_ftFont);
}

void Font::move() {
    #ifdef MAGNUM_USE_HARFBUZZ
    _hbFont = nullptr;
    #endif
    _ftFont = nullptr;
}

Font::~Font() { destroy(); }

Font::Font(Font&& other): glyphs(std::move(other.glyphs)), _texture(std::move(other._texture)), _ftFont(other._ftFont), _size(other._size)
    #ifdef MAGNUM_USE_HARFBUZZ
    , _hbFont(other._hbFont)
    #endif
{
    other.move();
}

Font& Font::operator=(Font&& other) {
    destroy();

    glyphs = std::move(other.glyphs);
    _texture = std::move(other._texture);
    _ftFont = other._ftFont;
    #ifdef MAGNUM_USE_HARFBUZZ
    _hbFont = other._hbFont;
    #endif
    _size = other._size;

    other.move();
    return *this;
}

const std::tuple<Rectangle, Rectangle>& Font::operator[](char32_t character) const {
    auto it = glyphs.find(character);

    if(it == glyphs.end())
        return glyphs.at(0);
    return it->second;
}

}}
