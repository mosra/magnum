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

#include "TextRenderer.h"

#ifdef MAGNUM_USE_HARFBUZZ
#include <hb.h>
#else
#include <ft2build.h>
#include FT_FREETYPE_H
#include <Utility/Unicode.h>
#endif

#include "Context.h"
#include "Extensions.h"
#include "Mesh.h"
#include "Swizzle.h"
#include "Shaders/AbstractVectorShader.h"
#include "Text/Font.h"

namespace Magnum { namespace Text {

namespace {

class TextLayouter {
    public:
        TextLayouter(Font& font, const Float size, const std::string& text);

        ~TextLayouter();

        inline UnsignedInt glyphCount() {
            #ifdef MAGNUM_USE_HARFBUZZ
            return _glyphCount;
            #else
            return glyphs.size();
            #endif
        }

        std::tuple<Rectangle, Rectangle, Vector2> renderGlyph(const Vector2& cursorPosition, const UnsignedInt i);

    private:
        #ifdef MAGNUM_USE_HARFBUZZ
        const Font& font;
        hb_buffer_t* buffer;
        hb_glyph_info_t* glyphInfo;
        hb_glyph_position_t* glyphPositions;
        UnsignedInt _glyphCount;
        #else
        Font& font;
        std::vector<FT_UInt> glyphs;
        #endif

        const Float size;
};

TextLayouter::TextLayouter(Font& font, const Float size, const std::string& text): font(font), size(size) {
    #ifdef MAGNUM_USE_HARFBUZZ
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
    #else
    /* Get glyph codes from characters */
    glyphs.reserve(text.size()+1);
    for(std::size_t i = 0; i != text.size(); ) {
        UnsignedInt codepoint;
        std::tie(codepoint, i) = Corrade::Utility::Unicode::nextChar(text, i);
        glyphs.push_back(FT_Get_Char_Index(font.font(), codepoint));
    }
    #endif
}

TextLayouter::~TextLayouter() {
    #ifdef MAGNUM_USE_HARFBUZZ
    /* Destroy HarfBuzz buffer */
    hb_buffer_destroy(buffer);
    #endif
}

std::tuple<Rectangle, Rectangle, Vector2> TextLayouter::renderGlyph(const Vector2& cursorPosition, const UnsignedInt i) {
    /* Position of the texture in the resulting glyph, texture coordinates */
    Rectangle texturePosition, textureCoordinates;
    #ifdef MAGNUM_USE_HARFBUZZ
    std::tie(texturePosition, textureCoordinates) = font[glyphInfo[i].codepoint];
    #else
    std::tie(texturePosition, textureCoordinates) = font[glyphs[i]];
    #endif

    #ifdef MAGNUM_USE_HARFBUZZ
    /* Glyph offset and advance to next glyph in normalized coordinates */
    Vector2 offset = Vector2(glyphPositions[i].x_offset,
                             glyphPositions[i].y_offset)/(64*font.size());
    Vector2 advance = Vector2(glyphPositions[i].x_advance,
                              glyphPositions[i].y_advance)/(64*font.size());
    #else
    /* Load glyph */
    CORRADE_INTERNAL_ASSERT_OUTPUT(FT_Load_Glyph(font.font(), glyphs[i], FT_LOAD_DEFAULT) == 0);
    const FT_GlyphSlot  slot = font.font()->glyph;
    Vector2 offset = Vector2(0, 0); /** @todo really? */
    Vector2 advance = Vector2(slot->advance.x, slot->advance.y)/(64*font.size());
    #endif

    /* Absolute quad position, composed from cursor position, glyph offset
        and texture position, denormalized to requested text size */
    Rectangle quadPosition = Rectangle::fromSize(
        (cursorPosition + offset + Vector2(texturePosition.left(), texturePosition.bottom()))*size,
        texturePosition.size()*size);

    return std::make_tuple(quadPosition, textureCoordinates, advance);
}

template<class T> void createIndices(void* output, const UnsignedInt glyphCount) {
    T* const out = reinterpret_cast<T*>(output);
    for(UnsignedInt i = 0; i != glyphCount; ++i) {
        /* 0---2 2
           |  / /|
           | / / |
           |/ /  |
           1 1---3 */

        const T vertex = i*4;
        const T pos = i*6;
        out[pos]   = vertex;
        out[pos+1] = vertex+1;
        out[pos+2] = vertex+2;
        out[pos+3] = vertex+1;
        out[pos+4] = vertex+3;
        out[pos+5] = vertex+2;
    }
}

template<UnsignedInt dimensions> typename DimensionTraits<dimensions>::VectorType point(const Vector2& vec);

template<> inline Vector2 point<2>(const Vector2& vec) {
    return vec;
}

template<> inline Vector3 point<3>(const Vector2& vec) {
    return {vec, 1.0f};
}

template<UnsignedInt dimensions> struct Vertex {
    typename DimensionTraits<dimensions>::VectorType position;
    Vector2 texcoords;
};

}

template<UnsignedInt dimensions> std::tuple<std::vector<typename DimensionTraits<dimensions>::VectorType>, std::vector<Vector2>, std::vector<UnsignedInt>, Rectangle> TextRenderer<dimensions>::render(Font& font, Float size, const std::string& text) {
    TextLayouter layouter(font, size, text);

    const UnsignedInt vertexCount = layouter.glyphCount()*4;

    /* Output data */
    std::vector<typename DimensionTraits<dimensions>::VectorType> positions;
    std::vector<Vector2> texcoords;
    positions.reserve(vertexCount);
    texcoords.reserve(vertexCount);

    /* Render all glyphs */
    Vector2 cursorPosition;
    for(UnsignedInt i = 0; i != layouter.glyphCount(); ++i) {
        /* Position of the texture in the resulting glyph, texture coordinates */
        Rectangle quadPosition, textureCoordinates;
        Vector2 advance;
        std::tie(quadPosition, textureCoordinates, advance) = layouter.renderGlyph(cursorPosition, i);

        positions.insert(positions.end(), {
            point<dimensions>(quadPosition.topLeft()),
            point<dimensions>(quadPosition.bottomLeft()),
            point<dimensions>(quadPosition.topRight()),
            point<dimensions>(quadPosition.bottomRight()),
        });
        texcoords.insert(texcoords.end(), {
            textureCoordinates.topLeft(),
            textureCoordinates.bottomLeft(),
            textureCoordinates.topRight(),
            textureCoordinates.bottomRight()
        });

        /* Advance cursor position to next character */
        cursorPosition += advance;
    }

    /* Create indices */
    std::vector<UnsignedInt> indices(layouter.glyphCount()*6);
    createIndices<UnsignedInt>(indices.data(), layouter.glyphCount());

    /* Rendered rectangle */
    Rectangle rectangle;
    if(layouter.glyphCount()) rectangle =
        {swizzle<'x', 'y'>(positions[1]), swizzle<'x', 'y'>(positions[positions.size()-2])};

    return std::make_tuple(std::move(positions), std::move(texcoords), std::move(indices), rectangle);
}

template<UnsignedInt dimensions> std::tuple<Mesh, Rectangle> TextRenderer<dimensions>::render(Font& font, Float size, const std::string& text, Buffer* vertexBuffer, Buffer* indexBuffer, Buffer::Usage usage) {
    TextLayouter layouter(font, size, text);

    const UnsignedInt vertexCount = layouter.glyphCount()*4;
    const UnsignedInt indexCount = layouter.glyphCount()*6;

    /* Vertex buffer */
    std::vector<Vertex<dimensions>> vertices;
    vertices.reserve(vertexCount);

    /* Render all glyphs */
    Vector2 cursorPosition;
    for(UnsignedInt i = 0; i != layouter.glyphCount(); ++i) {
        /* Position of the texture in the resulting glyph, texture coordinates */
        Rectangle quadPosition, textureCoordinates;
        Vector2 advance;
        std::tie(quadPosition, textureCoordinates, advance) = layouter.renderGlyph(cursorPosition, i);

        vertices.insert(vertices.end(), {
            {point<dimensions>(quadPosition.topLeft()), textureCoordinates.topLeft()},
            {point<dimensions>(quadPosition.bottomLeft()), textureCoordinates.bottomLeft()},
            {point<dimensions>(quadPosition.topRight()), textureCoordinates.topRight()},
            {point<dimensions>(quadPosition.bottomRight()), textureCoordinates.bottomRight()}
        });

        /* Advance cursor position to next character */
        cursorPosition += advance;
    }
    vertexBuffer->setData(vertices, usage);

    /* Fill index buffer */
    Mesh::IndexType indexType;
    std::size_t indicesSize;
    char* indices;
    if(vertexCount < 255) {
        indexType = Mesh::IndexType::UnsignedByte;
        indicesSize = indexCount*sizeof(UnsignedByte);
        indices = new char[indicesSize];
        createIndices<UnsignedByte>(indices, layouter.glyphCount());
    } else if(vertexCount < 65535) {
        indexType = Mesh::IndexType::UnsignedShort;
        indicesSize = indexCount*sizeof(UnsignedShort);
        indices = new char[indicesSize];
        createIndices<UnsignedShort>(indices, layouter.glyphCount());
    } else {
        indexType = Mesh::IndexType::UnsignedInt;
        indicesSize = indexCount*sizeof(UnsignedInt);
        indices = new char[indicesSize];
        createIndices<UnsignedInt>(indices, layouter.glyphCount());
    }
    indexBuffer->setData(indicesSize, indices, usage);
    delete indices;

    /* Rendered rectangle */
    Rectangle rectangle;
    if(layouter.glyphCount()) rectangle =
        {swizzle<'x', 'y'>(vertices[1].position), swizzle<'x', 'y'>(vertices[vertices.size()-2].position)};

    /* Configure mesh */
    Mesh mesh;
    mesh.setPrimitive(Mesh::Primitive::Triangles)
        ->setIndexCount(indexCount)
        ->addInterleavedVertexBuffer(vertexBuffer, 0,
            typename Shaders::AbstractVectorShader<dimensions>::Position(),
            typename Shaders::AbstractVectorShader<dimensions>::TextureCoordinates())
        ->setIndexBuffer(indexBuffer, 0, indexType, 0, vertexCount);

    return std::make_tuple(std::move(mesh), rectangle);
}

template<UnsignedInt dimensions> TextRenderer<dimensions>::TextRenderer(Font& font, const Float size): font(font), size(size), _capacity(0), vertexBuffer(Buffer::Target::Array), indexBuffer(Buffer::Target::ElementArray) {
    #ifndef MAGNUM_TARGET_GLES
    MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::ARB::map_buffer_range);
    #else
    #ifdef MAGNUM_TARGET_GLES2
    MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::EXT::map_buffer_range);
    #endif
    #endif

    _mesh.setPrimitive(Mesh::Primitive::Triangles)
        ->addInterleavedVertexBuffer(&vertexBuffer, 0,
            typename Shaders::AbstractVectorShader<dimensions>::Position(),
            typename Shaders::AbstractVectorShader<dimensions>::TextureCoordinates());
}

template<UnsignedInt dimensions> void TextRenderer<dimensions>::reserve(const uint32_t glyphCount, const Buffer::Usage vertexBufferUsage, const Buffer::Usage indexBufferUsage) {
    _capacity = glyphCount;

    const UnsignedInt vertexCount = glyphCount*4;
    const UnsignedInt indexCount = glyphCount*6;

    /* Allocate vertex buffer, reset vertex count */
    vertexBuffer.setData(vertexCount*sizeof(Vertex<dimensions>), nullptr, vertexBufferUsage);
    _mesh.setVertexCount(0);

    /* Allocate index buffer, reset index count and reconfigure buffer binding */
    Mesh::IndexType indexType;
    std::size_t indicesSize;
    if(vertexCount < 255) {
        indexType = Mesh::IndexType::UnsignedByte;
        indicesSize = indexCount*sizeof(UnsignedByte);
    } else if(vertexCount < 65535) {
        indexType = Mesh::IndexType::UnsignedShort;
        indicesSize = indexCount*sizeof(UnsignedShort);
    } else {
        indexType = Mesh::IndexType::UnsignedInt;
        indicesSize = indexCount*sizeof(UnsignedInt);
    }
    indexBuffer.setData(indicesSize, nullptr, indexBufferUsage);
    _mesh.setIndexCount(0)
        ->setIndexBuffer(&indexBuffer, 0, indexType, 0, vertexCount);

    /* Prefill index buffer */
    void* indices = indexBuffer.map(0, indicesSize, Buffer::MapFlag::InvalidateBuffer|Buffer::MapFlag::Write);
    if(vertexCount < 255)
        createIndices<UnsignedByte>(indices, glyphCount);
    else if(vertexCount < 65535)
        createIndices<UnsignedShort>(indices, glyphCount);
    else
        createIndices<UnsignedInt>(indices, glyphCount);
    CORRADE_INTERNAL_ASSERT_OUTPUT(indexBuffer.unmap());
}

template<UnsignedInt dimensions> void TextRenderer<dimensions>::render(const std::string& text) {
    TextLayouter layouter(font, size, text);

    CORRADE_ASSERT(layouter.glyphCount() <= _capacity, "Text::TextRenderer::render(): capacity" << _capacity << "too small to render" << layouter.glyphCount() << "glyphs", );

    /* Render all glyphs */
    Vertex<dimensions>* const vertices = static_cast<Vertex<dimensions>*>(vertexBuffer.map(0, layouter.glyphCount()*4*sizeof(Vertex<dimensions>),
        Buffer::MapFlag::InvalidateBuffer|Buffer::MapFlag::Write));
    Vector2 cursorPosition;
    for(UnsignedInt i = 0; i != layouter.glyphCount(); ++i) {
        /* Position of the texture in the resulting glyph, texture coordinates */
        Rectangle quadPosition, textureCoordinates;
        Vector2 advance;
        std::tie(quadPosition, textureCoordinates, advance) = layouter.renderGlyph(cursorPosition, i);

        if(i == 0)
            _rectangle.bottomLeft() = quadPosition.bottomLeft();
        else if(i == layouter.glyphCount()-1)
            _rectangle.topRight() = quadPosition.topRight();

        const std::size_t vertex = i*4;
        vertices[vertex]   = {point<dimensions>(quadPosition.topLeft()), textureCoordinates.topLeft()};
        vertices[vertex+1] = {point<dimensions>(quadPosition.bottomLeft()), textureCoordinates.bottomLeft()};
        vertices[vertex+2] = {point<dimensions>(quadPosition.topRight()), textureCoordinates.topRight()};
        vertices[vertex+3] = {point<dimensions>(quadPosition.bottomRight()), textureCoordinates.bottomRight()};

        /* Advance cursor position to next character */
        cursorPosition += advance;
    }
    CORRADE_INTERNAL_ASSERT_OUTPUT(vertexBuffer.unmap());

    /* Update index count */
    _mesh.setIndexCount(layouter.glyphCount()*6);
}

template class TextRenderer<2>;
template class TextRenderer<3>;

}}
