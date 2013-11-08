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

#include "Renderer.h"

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

Vector2 alignmentOffset(Rectangle& bounds, Alignment alignment) {
    const Vector2 size = bounds.size();
    const auto value = UnsignedByte(alignment);

    /** @todo What about top-down text? */

    Vector2 offset;

    /* Horizontal alignment */
    if((value & Implementation::AlignmentHorizontal) == Implementation::AlignmentCenter) offset -= Vector2::xAxis(size.x()*0.5f);
    else if((value & Implementation::AlignmentHorizontal) == Implementation::AlignmentRight) offset -= Vector2::xAxis(size.x());

    /* Vertical alignment */
    if((value & Implementation::AlignmentVertical) == Implementation::AlignmentMiddle) offset -= Vector2::yAxis(size.y()*0.5f);
    else if((value & Implementation::AlignmentVertical) == Implementation::AlignmentTop) offset -= Vector2::yAxis(size.y());

    /* Integer alignment */
    if(value & Implementation::AlignmentIntegral) offset = Math::round(offset);

    /* Update also the bounds */
    bounds.bottomLeft() += offset;
    bounds.topRight() += offset;
    return offset;
}

struct Vertex {
    Vector2 position, texcoords;
};

}

std::tuple<std::vector<Vector2>, std::vector<Vector2>, std::vector<UnsignedInt>, Rectangle> AbstractRenderer::render(AbstractFont& font, const GlyphCache& cache, Float size, const std::string& text, Alignment alignment) {
    const auto layouter = font.layout(cache, size, text);
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
        Rectangle quadPosition, textureCoordinates;
        std::tie(quadPosition, textureCoordinates) = layouter->renderGlyph(i, cursorPosition, rectangle);

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
    }

    /* Respect the alignment */
    const Vector2 offset = alignmentOffset(rectangle, alignment);
    for(auto& p: positions) p += offset;

    /* Create indices */
    std::vector<UnsignedInt> indices(layouter->glyphCount()*6);
    createIndices<UnsignedInt>(indices.data(), layouter->glyphCount());

    return std::make_tuple(std::move(positions), std::move(texcoords), std::move(indices), rectangle);
}

std::tuple<Mesh, Rectangle> AbstractRenderer::render(AbstractFont& font, const GlyphCache& cache, Float size, const std::string& text, Buffer& vertexBuffer, Buffer& indexBuffer, Buffer::Usage usage, Alignment alignment) {
    const auto layouter = font.layout(cache, size, text);

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
        Rectangle quadPosition, textureCoordinates;
        std::tie(quadPosition, textureCoordinates) = layouter->renderGlyph(i, cursorPosition, rectangle);

        vertices.insert(vertices.end(), {
            {quadPosition.topLeft(), textureCoordinates.topLeft()},
            {quadPosition.bottomLeft(), textureCoordinates.bottomLeft()},
            {quadPosition.topRight(), textureCoordinates.topRight()},
            {quadPosition.bottomRight(), textureCoordinates.bottomRight()}
        });
    }

    /* Respect the alignment */
    const Vector2 offset = alignmentOffset(rectangle, alignment);
    for(auto& v: vertices) v.position += offset;

    vertexBuffer.setData(vertices, usage);

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
    indexBuffer.setData({indices, indicesSize}, usage);
    delete indices;

    /* Configure mesh except for vertex buffer (depends on dimension count, done
       in subclass) */
    Mesh mesh;
    mesh.setPrimitive(Mesh::Primitive::Triangles)
        .setIndexCount(indexCount)
        .setIndexBuffer(indexBuffer, 0, indexType, 0, vertexCount);

    return std::make_tuple(std::move(mesh), rectangle);
}

template<UnsignedInt dimensions> std::tuple<Mesh, Rectangle> Renderer<dimensions>::render(AbstractFont& font, const GlyphCache& cache, Float size, const std::string& text, Buffer& vertexBuffer, Buffer& indexBuffer, Buffer::Usage usage, Alignment alignment) {
    /* Finalize mesh configuration and return the result */
    auto r = AbstractRenderer::render(font, cache, size, text, vertexBuffer, indexBuffer, usage, alignment);
    Mesh& mesh = std::get<0>(r);
    mesh.addVertexBuffer(vertexBuffer, 0,
            typename Shaders::AbstractVector<dimensions>::Position(
                Shaders::AbstractVector<dimensions>::Position::Components::Two),
            typename Shaders::AbstractVector<dimensions>::TextureCoordinates());
    return std::move(r);
}

#if defined(MAGNUM_TARGET_GLES2) && !defined(CORRADE_TARGET_EMSCRIPTEN)
AbstractRenderer::BufferMapImplementation AbstractRenderer::bufferMapImplementation = &AbstractRenderer::bufferMapImplementationFull;
AbstractRenderer::BufferUnmapImplementation AbstractRenderer::bufferUnmapImplementation = &AbstractRenderer::bufferUnmapImplementationDefault;

void* AbstractRenderer::bufferMapImplementationFull(Buffer& buffer, GLsizeiptr) {
    return buffer.map(Buffer::MapAccess::WriteOnly);
}

void* AbstractRenderer::bufferMapImplementationSub(Buffer& buffer, GLsizeiptr length) {
    return buffer.mapSub(0, length, Buffer::MapAccess::WriteOnly);
}

void AbstractRenderer::bufferUnmapImplementationSub(Buffer& buffer) {
    buffer.unmapSub();
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) || defined(CORRADE_TARGET_EMSCRIPTEN)
inline void* AbstractRenderer::bufferMapImplementation(Buffer& buffer, GLsizeiptr length)
#else
void* AbstractRenderer::bufferMapImplementationRange(Buffer& buffer, GLsizeiptr length)
#endif
{
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    return buffer.map(0, length, Buffer::MapFlag::InvalidateBuffer|Buffer::MapFlag::Write);
    #else
    static_cast<void>(length);
    return &buffer == &_indexBuffer ? _indexBufferData : _vertexBufferData;
    #endif
}

#if !defined(MAGNUM_TARGET_GLES2) || defined(CORRADE_TARGET_EMSCRIPTEN)
inline void AbstractRenderer::bufferUnmapImplementation(Buffer& buffer)
#else
void AbstractRenderer::bufferUnmapImplementationDefault(Buffer& buffer)
#endif
{
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    buffer.unmap();
    #else
    buffer.setSubData(0, &buffer == &_indexBuffer ? _indexBufferData : _vertexBufferData);
    #endif
}

AbstractRenderer::AbstractRenderer(AbstractFont& font, const GlyphCache& cache, const Float size, const Alignment alignment): _vertexBuffer(Buffer::Target::Array), _indexBuffer(Buffer::Target::ElementArray), font(font), cache(cache), size(size), _alignment(alignment), _capacity(0) {
    #ifndef MAGNUM_TARGET_GLES
    MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::ARB::map_buffer_range);
    #elif defined(MAGNUM_TARGET_GLES2) && !defined(CORRADE_TARGET_EMSCRIPTEN)
    if(Context::current()->isExtensionSupported<Extensions::GL::EXT::map_buffer_range>()) {
        bufferMapImplementation = &AbstractRenderer::bufferMapImplementationRange;
    } else if(Context::current()->isExtensionSupported<Extensions::GL::CHROMIUM::map_sub>()) {
        bufferMapImplementation = &AbstractRenderer::bufferMapImplementationSub;
        bufferUnmapImplementation = &AbstractRenderer::bufferUnmapImplementationSub;
    } else {
        MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::OES::mapbuffer);
        Warning() << "Text::Renderer: neither" << Extensions::GL::EXT::map_buffer_range::string()
                  << "nor" << Extensions::GL::CHROMIUM::map_sub::string()
                  << "is supported, using inefficient" << Extensions::GL::OES::mapbuffer::string()
                  << "instead";
    }
    #endif

    /* Vertex buffer configuration depends on dimension count, done in subclass */
    _mesh.setPrimitive(Mesh::Primitive::Triangles);
}

AbstractRenderer::~AbstractRenderer() {}

template<UnsignedInt dimensions> Renderer<dimensions>::Renderer(AbstractFont& font, const GlyphCache& cache, const Float size, const Alignment alignment): AbstractRenderer(font, cache, size, alignment) {
    /* Finalize mesh configuration */
    _mesh.addVertexBuffer(_vertexBuffer, 0,
            typename Shaders::AbstractVector<dimensions>::Position(Shaders::AbstractVector<dimensions>::Position::Components::Two),
            typename Shaders::AbstractVector<dimensions>::TextureCoordinates());
}

void AbstractRenderer::reserve(const uint32_t glyphCount, const Buffer::Usage vertexBufferUsage, const Buffer::Usage indexBufferUsage) {
    _capacity = glyphCount;

    const UnsignedInt vertexCount = glyphCount*4;
    const UnsignedInt indexCount = glyphCount*6;

    /* Allocate vertex buffer, reset vertex count */
    _vertexBuffer.setData({nullptr, vertexCount*sizeof(Vertex)}, vertexBufferUsage);
    #ifdef CORRADE_TARGET_EMSCRIPTEN
    _vertexBufferData = Containers::Array<UnsignedByte>(vertexCount*sizeof(Vertex));
    #endif
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
    _indexBuffer.setData({nullptr, indicesSize}, indexBufferUsage);
    #ifdef CORRADE_TARGET_EMSCRIPTEN
    _indexBufferData = Containers::Array<UnsignedByte>(indicesSize);
    #endif
    _mesh.setIndexCount(0)
        .setIndexBuffer(_indexBuffer, 0, indexType, 0, vertexCount);

    /* Map buffer for filling */
    void* const indices = bufferMapImplementation(_indexBuffer, indicesSize);
    CORRADE_INTERNAL_ASSERT(indices);

    /* Prefill index buffer */
    if(vertexCount < 255)
        createIndices<UnsignedByte>(indices, glyphCount);
    else if(vertexCount < 65535)
        createIndices<UnsignedShort>(indices, glyphCount);
    else
        createIndices<UnsignedInt>(indices, glyphCount);
    bufferUnmapImplementation(_indexBuffer);
}

void AbstractRenderer::render(const std::string& text) {
    const auto layouter = font.layout(cache, size, text);

    CORRADE_ASSERT(layouter->glyphCount() <= _capacity,
        "Text::Renderer::render(): capacity" << _capacity << "too small to render" << layouter->glyphCount() << "glyphs", );

    /* Reset rendered rectangle */
    _rectangle = {};

    /* Map buffer for rendering */
    Containers::ArrayReference<Vertex> vertices(static_cast<Vertex*>(bufferMapImplementation(_vertexBuffer,
        layouter->glyphCount()*4*sizeof(Vertex))), layouter->glyphCount()*4);
    CORRADE_INTERNAL_ASSERT_OUTPUT(vertices);

    /* Render all glyphs */
    Vector2 cursorPosition;
    for(UnsignedInt i = 0; i != layouter->glyphCount(); ++i) {
        Rectangle quadPosition, textureCoordinates;
        std::tie(quadPosition, textureCoordinates) = layouter->renderGlyph(i, cursorPosition, _rectangle);

        const std::size_t vertex = i*4;
        vertices[vertex]   = {quadPosition.topLeft(), textureCoordinates.topLeft()};
        vertices[vertex+1] = {quadPosition.bottomLeft(), textureCoordinates.bottomLeft()};
        vertices[vertex+2] = {quadPosition.topRight(), textureCoordinates.topRight()};
        vertices[vertex+3] = {quadPosition.bottomRight(), textureCoordinates.bottomRight()};
    }

    /* Respect the alignment */
    const Vector2 offset = alignmentOffset(_rectangle, _alignment);
    for(auto& v: vertices) v.position += offset;

    bufferUnmapImplementation(_vertexBuffer);

    /* Update index count */
    _mesh.setIndexCount(layouter->glyphCount()*6);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template class MAGNUM_TEXT_EXPORT Renderer<2>;
template class MAGNUM_TEXT_EXPORT Renderer<3>;
#endif

}}
