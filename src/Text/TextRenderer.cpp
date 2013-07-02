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

#include "TextRenderer.h"

#include "Context.h"
#include "Extensions.h"
#include "Mesh.h"
#include "Shaders/AbstractVector.h"
#include "Text/AbstractFont.h"

namespace Magnum { namespace Text {

namespace {

template<class T> void createIndices(void* output, const UnsignedInt glyphCount) {
    T* const out = reinterpret_cast<T*>(output);
    for(UnsignedInt i = 0; i != glyphCount; ++i) {
        /* 0---2 0---2 5
           |   | |  / /|
           |   | | / / |
           |   | |/ /  |
           1---3 1 3---4 */

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

struct Vertex {
    Vector2 position, texcoords;
};

}

std::tuple<std::vector<Vector2>, std::vector<Vector2>, std::vector<UnsignedInt>, Rectangle> AbstractTextRenderer::render(AbstractFont* const font, const GlyphCache* const cache, Float size, const std::string& text) {
    AbstractLayouter* const layouter = font->layout(cache, size, text);
    const UnsignedInt vertexCount = layouter->glyphCount()*4;

    /* Output data */
    std::vector<Vector2> positions, texcoords;
    positions.reserve(vertexCount);
    texcoords.reserve(vertexCount);

    /* Rendered rectangle */
    Rectangle rectangle;

    /* Render all glyphs */
    Vector2 cursorPosition;
    for(UnsignedInt i = 0; i != layouter->glyphCount(); ++i) {
        /* Position of the texture in the resulting glyph, texture coordinates */
        Rectangle quadPosition, textureCoordinates;
        Vector2 advance;
        std::tie(quadPosition, textureCoordinates, advance) = layouter->renderGlyph(cursorPosition, i);

        /* 0---2
           |   |
           |   |
           |   |
           1---3 */

        positions.insert(positions.end(), {
            quadPosition.topLeft(),
            quadPosition.bottomLeft(),
            quadPosition.topRight(),
            quadPosition.bottomRight(),
        });
        texcoords.insert(texcoords.end(), {
            textureCoordinates.topLeft(),
            textureCoordinates.bottomLeft(),
            textureCoordinates.topRight(),
            textureCoordinates.bottomRight()
        });

        /* Extend rectangle with current quad bounds */
        rectangle.bottomLeft() = Math::min(rectangle.bottomLeft(), quadPosition.bottomLeft());
        rectangle.topRight() = Math::max(rectangle.topRight(), quadPosition.topRight());

        /* Advance cursor position to next character */
        cursorPosition += advance;
    }

    /* Create indices */
    std::vector<UnsignedInt> indices(layouter->glyphCount()*6);
    createIndices<UnsignedInt>(indices.data(), layouter->glyphCount());

    delete layouter;
    return std::make_tuple(std::move(positions), std::move(texcoords), std::move(indices), rectangle);
}

std::tuple<Mesh, Rectangle> AbstractTextRenderer::render(AbstractFont* const font, const GlyphCache* const cache, Float size, const std::string& text, Buffer* vertexBuffer, Buffer* indexBuffer, Buffer::Usage usage) {
    AbstractLayouter* const layouter = font->layout(cache, size, text);

    const UnsignedInt vertexCount = layouter->glyphCount()*4;
    const UnsignedInt indexCount = layouter->glyphCount()*6;

    /* Vertex buffer */
    std::vector<Vertex> vertices;
    vertices.reserve(vertexCount);

    /* Rendered rectangle */
    Rectangle rectangle;

    /* Render all glyphs */
    Vector2 cursorPosition;
    for(UnsignedInt i = 0; i != layouter->glyphCount(); ++i) {
        /* Position of the texture in the resulting glyph, texture coordinates */
        Rectangle quadPosition, textureCoordinates;
        Vector2 advance;
        std::tie(quadPosition, textureCoordinates, advance) = layouter->renderGlyph(cursorPosition, i);

        vertices.insert(vertices.end(), {
            {quadPosition.topLeft(), textureCoordinates.topLeft()},
            {quadPosition.bottomLeft(), textureCoordinates.bottomLeft()},
            {quadPosition.topRight(), textureCoordinates.topRight()},
            {quadPosition.bottomRight(), textureCoordinates.bottomRight()}
        });

        /* Extend rectangle with current quad bounds */
        rectangle.bottomLeft() = Math::min(rectangle.bottomLeft(), quadPosition.bottomLeft());
        rectangle.topRight() = Math::max(rectangle.topRight(), quadPosition.topRight());

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
        createIndices<UnsignedByte>(indices, layouter->glyphCount());
    } else if(vertexCount < 65535) {
        indexType = Mesh::IndexType::UnsignedShort;
        indicesSize = indexCount*sizeof(UnsignedShort);
        indices = new char[indicesSize];
        createIndices<UnsignedShort>(indices, layouter->glyphCount());
    } else {
        indexType = Mesh::IndexType::UnsignedInt;
        indicesSize = indexCount*sizeof(UnsignedInt);
        indices = new char[indicesSize];
        createIndices<UnsignedInt>(indices, layouter->glyphCount());
    }
    indexBuffer->setData(indicesSize, indices, usage);
    delete indices;

    /* Configure mesh except for vertex buffer (depends on dimension count, done
       in subclass) */
    Mesh mesh;
    mesh.setPrimitive(Mesh::Primitive::Triangles)
        ->setIndexCount(indexCount)
        ->setIndexBuffer(indexBuffer, 0, indexType, 0, vertexCount);

    delete layouter;
    return std::make_tuple(std::move(mesh), rectangle);
}

template<UnsignedInt dimensions> std::tuple<Mesh, Rectangle> TextRenderer<dimensions>::render(AbstractFont* const font, const GlyphCache* const cache, Float size, const std::string& text, Buffer* vertexBuffer, Buffer* indexBuffer, Buffer::Usage usage) {
    /* Finalize mesh configuration and return the result */
    auto r = AbstractTextRenderer::render(font, cache, size, text, vertexBuffer, indexBuffer, usage);
    Mesh& mesh = std::get<0>(r);
    mesh.addInterleavedVertexBuffer(vertexBuffer, 0,
            typename Shaders::AbstractVector<dimensions>::Position(
                Shaders::AbstractVector<dimensions>::Position::Components::Two),
            typename Shaders::AbstractVector<dimensions>::TextureCoordinates());
    return std::move(r);
}

AbstractTextRenderer::AbstractTextRenderer(AbstractFont* const font, const GlyphCache* const cache, Float size): _vertexBuffer(Buffer::Target::Array), _indexBuffer(Buffer::Target::ElementArray), font(font), cache(cache), size(size), _capacity(0) {
    #ifndef MAGNUM_TARGET_GLES
    MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::ARB::map_buffer_range);
    #else
    #ifdef MAGNUM_TARGET_GLES2
    MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::EXT::map_buffer_range);
    #endif
    #endif

    /* Vertex buffer configuration depends on dimension count, done in subclass */
    _mesh.setPrimitive(Mesh::Primitive::Triangles);
}

AbstractTextRenderer::~AbstractTextRenderer() {}

template<UnsignedInt dimensions> TextRenderer<dimensions>::TextRenderer(AbstractFont* const font, const GlyphCache* const cache, const Float size): AbstractTextRenderer(font, cache, size) {
    /* Finalize mesh configuration */
    _mesh.addInterleavedVertexBuffer(&_vertexBuffer, 0,
            typename Shaders::AbstractVector<dimensions>::Position(Shaders::AbstractVector<dimensions>::Position::Components::Two),
            typename Shaders::AbstractVector<dimensions>::TextureCoordinates());
}

void AbstractTextRenderer::reserve(const uint32_t glyphCount, const Buffer::Usage vertexBufferUsage, const Buffer::Usage indexBufferUsage) {
    _capacity = glyphCount;

    const UnsignedInt vertexCount = glyphCount*4;
    const UnsignedInt indexCount = glyphCount*6;

    /* Allocate vertex buffer, reset vertex count */
    _vertexBuffer.setData(vertexCount*sizeof(Vertex), nullptr, vertexBufferUsage);
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
    _indexBuffer.setData(indicesSize, nullptr, indexBufferUsage);
    _mesh.setIndexCount(0)
        ->setIndexBuffer(&_indexBuffer, 0, indexType, 0, vertexCount);

    /* Prefill index buffer */
    void* indices = _indexBuffer.map(0, indicesSize, Buffer::MapFlag::InvalidateBuffer|Buffer::MapFlag::Write);
    if(vertexCount < 255)
        createIndices<UnsignedByte>(indices, glyphCount);
    else if(vertexCount < 65535)
        createIndices<UnsignedShort>(indices, glyphCount);
    else
        createIndices<UnsignedInt>(indices, glyphCount);
    CORRADE_INTERNAL_ASSERT_OUTPUT(_indexBuffer.unmap());
}

void AbstractTextRenderer::render(const std::string& text) {
    AbstractLayouter* layouter = font->layout(cache, size, text);

    CORRADE_ASSERT(layouter->glyphCount() <= _capacity,
        "Text::TextRenderer::render(): capacity" << _capacity << "too small to render" << layouter->glyphCount() << "glyphs", );

    /* Reset rendered rectangle */
    _rectangle = {};

    /* Render all glyphs */
    Vertex* const vertices = static_cast<Vertex*>(_vertexBuffer.map(0, layouter->glyphCount()*4*sizeof(Vertex),
        Buffer::MapFlag::InvalidateBuffer|Buffer::MapFlag::Write));
    Vector2 cursorPosition;
    for(UnsignedInt i = 0; i != layouter->glyphCount(); ++i) {
        /* Position of the texture in the resulting glyph, texture coordinates */
        Rectangle quadPosition, textureCoordinates;
        Vector2 advance;
        std::tie(quadPosition, textureCoordinates, advance) = layouter->renderGlyph(cursorPosition, i);

        /* Extend rectangle with current quad bounds */
        _rectangle.bottomLeft() = Math::min(_rectangle.bottomLeft(), quadPosition.bottomLeft());
        _rectangle.topRight() = Math::max(_rectangle.topRight(), quadPosition.topRight());

        const std::size_t vertex = i*4;
        vertices[vertex]   = {quadPosition.topLeft(), textureCoordinates.topLeft()};
        vertices[vertex+1] = {quadPosition.bottomLeft(), textureCoordinates.bottomLeft()};
        vertices[vertex+2] = {quadPosition.topRight(), textureCoordinates.topRight()};
        vertices[vertex+3] = {quadPosition.bottomRight(), textureCoordinates.bottomRight()};

        /* Advance cursor position to next character */
        cursorPosition += advance;
    }
    CORRADE_INTERNAL_ASSERT_OUTPUT(_vertexBuffer.unmap());

    /* Update index count */
    _mesh.setIndexCount(layouter->glyphCount()*6);

    delete layouter;
}

template class TextRenderer<2>;
template class TextRenderer<3>;

}}
