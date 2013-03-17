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

#include "FreeTypeFont.h"

#include <algorithm>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <Utility/Unicode.h>

#include "Extensions.h"
#include "Image.h"
#include "TextureTools/Atlas.h"
#include "TextureTools/DistanceField.h"

namespace Magnum { namespace Text {

namespace {

class FreeTypeLayouter: public AbstractLayouter {
    public:
        FreeTypeLayouter(FreeTypeFont& font, const Float size, const std::string& text);

        std::tuple<Rectangle, Rectangle, Vector2> renderGlyph(const Vector2& cursorPosition, const UnsignedInt i) override;

    private:
        FreeTypeFont& font;
        std::vector<FT_UInt> glyphs;
        const Float size;
};

}

FreeTypeFontRenderer::FreeTypeFontRenderer() {
    CORRADE_INTERNAL_ASSERT_OUTPUT(FT_Init_FreeType(&_library) == 0);
}

FreeTypeFontRenderer::~FreeTypeFontRenderer() {
    FT_Done_FreeType(_library);
}

FreeTypeFont::FreeTypeFont(FreeTypeFontRenderer& renderer, const std::string& fontFile, Float size): _size(size) {
    CORRADE_INTERNAL_ASSERT_OUTPUT(FT_New_Face(renderer.library(), fontFile.c_str(), 0, &_ftFont) == 0);

    finishConstruction();
}

FreeTypeFont::FreeTypeFont(FreeTypeFontRenderer& renderer, const unsigned char* data, std::size_t dataSize, Float size): _size(size) {
    CORRADE_INTERNAL_ASSERT_OUTPUT(FT_New_Memory_Face(renderer.library(), data, dataSize, 0, &_ftFont) == 0);

    finishConstruction();
}

void FreeTypeFont::finishConstruction() {
    CORRADE_INTERNAL_ASSERT_OUTPUT(FT_Set_Char_Size(_ftFont, 0, _size*64, 100, 100) == 0);

    #ifndef MAGNUM_TARGET_GLES
    MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::ARB::texture_rg);
    #else
    MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::EXT::texture_rg);
    #endif

    /* Set up the texture */
    _texture.setWrapping(Texture2D::Wrapping::ClampToEdge)
        ->setMinificationFilter(Texture2D::Filter::Linear)
        ->setMagnificationFilter(Texture2D::Filter::Linear);
}

void FreeTypeFont::prerenderInternal(const std::string& characters, const Vector2i& atlasSize, const Int radius, Texture2D* output) {
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
    const Vector2i padding = Vector2i(radius);
    std::vector<Vector2i> charSizes;
    charSizes.reserve(charIndices.size());
    for(auto it = charIndices.begin(); it != charIndices.end(); ++it) {
        CORRADE_INTERNAL_ASSERT_OUTPUT(FT_Load_Glyph(_ftFont, *it, FT_LOAD_DEFAULT) == 0);
        charSizes.push_back(Vector2i(_ftFont->glyph->metrics.width, _ftFont->glyph->metrics.height)/64);
    }

    /* Create texture atlas */
    const std::vector<Rectanglei> charPositions = TextureTools::atlas(atlasSize, charSizes, padding);

    /* Render all characters to the atlas and create character map */
    #ifndef CORRADE_GCC44_COMPATIBILITY
    glyphs.reserve(charPositions.size());
    #endif
    unsigned char* pixmap = new unsigned char[atlasSize.product()]();
    Image2D image(atlasSize, Image2D::Format::Red, Image2D::Type::UnsignedByte, pixmap);
    for(std::size_t i = 0; i != charPositions.size(); ++i) {
        /* Load and render glyph */
        /** @todo B&W only if radius != 0 */
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
            Rectangle::fromSize((Vector2(glyph->bitmap_left, glyph->bitmap_top-charPositions[i].height()) - Vector2(radius))/_size,
                                Vector2(charPositions[i].size() + 2*padding)/_size),
            Rectangle(Vector2(charPositions[i].bottomLeft() - padding)/atlasSize,
                      Vector2(charPositions[i].topRight() + padding)/atlasSize)
        )}).second);
    }

    /* Set texture data */
    #ifndef MAGNUM_TARGET_GLES
    output->setImage(0, Texture2D::InternalFormat::R8, &image);
    #else
    output->setImage(0, Texture2D::InternalFormat::Red, &image);
    #endif
}

void FreeTypeFont::prerender(const std::string& characters, const Vector2i& atlasSize) {
    prerenderInternal(characters, atlasSize, 0, &_texture);
}

void FreeTypeFont::prerenderDistanceField(const std::string& characters, const Vector2i& sourceAtlasSize, const Vector2i& atlasSize, Int radius) {
    MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::ARB::texture_storage);

    /* Render input texture */
    Texture2D input;
    input.setWrapping(Texture2D::Wrapping::ClampToEdge)
        ->setMinificationFilter(Texture2D::Filter::Linear)
        ->setMagnificationFilter(Texture2D::Filter::Linear);
    prerenderInternal(characters, sourceAtlasSize, radius, &input);

    /* Create distance field from input texture */
    _texture.setStorage(1, Texture2D::InternalFormat::R8, atlasSize);
    TextureTools::distanceField(&input, &_texture, Rectanglei::fromSize({}, atlasSize), radius);
}

FreeTypeFont::~FreeTypeFont() {
    FT_Done_Face(_ftFont);
}

const std::tuple<Rectangle, Rectangle>& FreeTypeFont::operator[](char32_t character) const {
    auto it = glyphs.find(character);

    if(it == glyphs.end())
        return glyphs.at(0);
    return it->second;
}

AbstractLayouter* FreeTypeFont::layout(const Float size, const std::string& text) {
    return new FreeTypeLayouter(*this, size, text);
}

namespace {

FreeTypeLayouter::FreeTypeLayouter(FreeTypeFont& font, const Float size, const std::string& text): font(font), size(size) {
    /* Get glyph codes from characters */
    glyphs.reserve(text.size());
    _glyphCount = text.size();
    for(std::size_t i = 0; i != text.size(); ) {
        UnsignedInt codepoint;
        std::tie(codepoint, i) = Corrade::Utility::Unicode::nextChar(text, i);
        glyphs.push_back(FT_Get_Char_Index(font.font(), codepoint));
    }
}

std::tuple<Rectangle, Rectangle, Vector2> FreeTypeLayouter::renderGlyph(const Vector2& cursorPosition, const UnsignedInt i) {
    /* Position of the texture in the resulting glyph, texture coordinates */
    Rectangle texturePosition, textureCoordinates;
    std::tie(texturePosition, textureCoordinates) = font[glyphs[i]];

    /* Load glyph */
    CORRADE_INTERNAL_ASSERT_OUTPUT(FT_Load_Glyph(font.font(), glyphs[i], FT_LOAD_DEFAULT) == 0);
    const FT_GlyphSlot  slot = font.font()->glyph;
    Vector2 offset = Vector2(0, 0); /** @todo really? */
    Vector2 advance = Vector2(slot->advance.x, slot->advance.y)/(64*font.size());

    /* Absolute quad position, composed from cursor position, glyph offset
        and texture position, denormalized to requested text size */
    Rectangle quadPosition = Rectangle::fromSize(
        (cursorPosition + offset + Vector2(texturePosition.left(), texturePosition.bottom()))*size,
        texturePosition.size()*size);

    return std::make_tuple(quadPosition, textureCoordinates, advance);
}

}

}}
