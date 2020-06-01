/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/ArrayViewStl.h>

#include "Magnum/Mesh.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/Math/Functions.h"
#include "Magnum/Shaders/AbstractVector.h"
#include "Magnum/Text/AbstractFont.h"
#include "Magnum/Text/GlyphCache.h"

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

        const T vertex = T(i)*4;
        const UnsignedInt pos = T(i)*6;
        out[pos]   = vertex;
        out[pos+1] = vertex+1;
        out[pos+2] = vertex+2;
        out[pos+3] = vertex+1;
        out[pos+4] = vertex+3;
        out[pos+5] = vertex+2;
    }
}

struct Vertex {
    Vector2 position, textureCoordinates;
};

std::tuple<std::vector<Vertex>, Range2D> renderVerticesInternal(AbstractFont& font, const GlyphCache& cache, const Float size, const std::string& text, const Alignment alignment) {
    /* Output data, reserve memory as when the text would be ASCII-only. In
       reality the actual vertex count will be smaller, but allocating more at
       once is better than reallocating many times later. */
    std::vector<Vertex> vertices;
    vertices.reserve(text.size()*4);

    /* Total rendered bounds, intial line position, line increment, last+1
       vertex on previous line */
    Range2D rectangle;
    Vector2 linePosition;
    const Vector2 lineAdvance = Vector2::yAxis(font.lineHeight()*size/font.size());
    std::size_t lastLineLastVertex = 0;

    /* Temp buffer so we don't allocate for each new line */
    /**
     * @todo C++1z: use std::string_view to avoid the one allocation and all
     *      the copying altogether
     */
    std::string line;
    line.reserve(text.size());

    /* Render each line separately and align it horizontally */
    std::size_t pos, prevPos = 0;
    do {
        /* Empty line, nothing to do (the rest is done below in while expression) */
        if((pos = text.find('\n', prevPos)) == prevPos) continue;

        /* Copy the line into the temp buffer */
        line.assign(text, prevPos, pos-prevPos);

        /* Layout the line */
        Containers::Pointer<AbstractLayouter> layouter = font.layout(cache, size, line);

        /* Verify that we don't reallocate anything. The only problem might
           arise when the layouter decides to compose one character from more
           than one glyph (i.e. accents). Will remove the assert when this
           issue arises. */
        CORRADE_INTERNAL_ASSERT(vertices.size() + layouter->glyphCount()*4 <= vertices.capacity());

        /* Bounds of rendered line */
        Range2D lineRectangle;

        /* Render all glyphs */
        Vector2 cursorPosition(linePosition);
        for(UnsignedInt i = 0; i != layouter->glyphCount(); ++i) {
            Range2D quadPosition, textureCoordinates;
            std::tie(quadPosition, textureCoordinates) = layouter->renderGlyph(i, cursorPosition, lineRectangle);

            /* 0---2
               |   |
               |   |
               |   |
               1---3 */

            vertices.insert(vertices.end(), {
                {quadPosition.topLeft(), textureCoordinates.topLeft()},
                {quadPosition.bottomLeft(), textureCoordinates.bottomLeft()},
                {quadPosition.topRight(), textureCoordinates.topRight()},
                {quadPosition.bottomRight(), textureCoordinates.bottomRight()}
            });
        }

        /** @todo What about top-down text? */

        /* Horizontally align the rendered line */
        Float alignmentOffsetX = 0.0f;
        if((UnsignedByte(alignment) & Implementation::AlignmentHorizontal) == Implementation::AlignmentCenter)
            alignmentOffsetX = -lineRectangle.centerX();
        else if((UnsignedByte(alignment) & Implementation::AlignmentHorizontal) == Implementation::AlignmentRight)
            alignmentOffsetX = -lineRectangle.right();

        /* Integer alignment */
        if(UnsignedByte(alignment) & Implementation::AlignmentIntegral)
            alignmentOffsetX = Math::round(alignmentOffsetX);

        /* Align positions and bounds on current line */
        lineRectangle = lineRectangle.translated(Vector2::xAxis(alignmentOffsetX));
        for(auto it = vertices.begin()+lastLineLastVertex; it != vertices.end(); ++it)
            it->position.x() += alignmentOffsetX;

        /* Add final line bounds to total bounds, similarly to AbstractFont::renderGlyph() */
        if(!rectangle.size().isZero()) {
            rectangle.bottomLeft() = Math::min(rectangle.bottomLeft(), lineRectangle.bottomLeft());
            rectangle.topRight() = Math::max(rectangle.topRight(), lineRectangle.topRight());
        } else rectangle = lineRectangle;

    /* Move to next line */
    } while(prevPos = pos+1,
            linePosition -= lineAdvance,
            lastLineLastVertex = vertices.size(),
            pos != std::string::npos);

    /* Vertically align the rendered text */
    Float alignmentOffsetY = 0.0f;
    if((UnsignedByte(alignment) & Implementation::AlignmentVertical) == Implementation::AlignmentMiddle)
        alignmentOffsetY = -rectangle.centerY();
    else if((UnsignedByte(alignment) & Implementation::AlignmentVertical) == Implementation::AlignmentTop)
        alignmentOffsetY = -rectangle.top();

    /* Integer alignment */
    if(UnsignedByte(alignment) & Implementation::AlignmentIntegral)
        alignmentOffsetY = Math::round(alignmentOffsetY);

    /* Align positions and bounds */
    rectangle = rectangle.translated(Vector2::yAxis(alignmentOffsetY));
    for(auto& v: vertices) v.position.y() += alignmentOffsetY;

    return std::make_tuple(std::move(vertices), rectangle);
}

std::pair<Containers::Array<char>, MeshIndexType> renderIndicesInternal(const UnsignedInt glyphCount) {
    const UnsignedInt vertexCount = glyphCount*4;
    const UnsignedInt indexCount = glyphCount*6;

    Containers::Array<char> indices;
    MeshIndexType indexType;
    if(vertexCount <= 256) {
        indexType = MeshIndexType::UnsignedByte;
        indices = Containers::Array<char>(indexCount*sizeof(UnsignedByte));
        createIndices<UnsignedByte>(indices, glyphCount);
    } else if(vertexCount <= 65536) {
        indexType = MeshIndexType::UnsignedShort;
        indices = Containers::Array<char>(indexCount*sizeof(UnsignedShort));
        createIndices<UnsignedShort>(indices, glyphCount);
    } else {
        indexType = MeshIndexType::UnsignedInt;
        indices = Containers::Array<char>(indexCount*sizeof(UnsignedInt));
        createIndices<UnsignedInt>(indices, glyphCount);
    }

    return {std::move(indices), indexType};
}

std::tuple<GL::Mesh, Range2D> renderInternal(AbstractFont& font, const GlyphCache& cache, Float size, const std::string& text, GL::Buffer& vertexBuffer, GL::Buffer& indexBuffer, GL::BufferUsage usage, Alignment alignment) {
    /* Render vertices and upload them */
    std::vector<Vertex> vertices;
    Range2D rectangle;
    std::tie(vertices, rectangle) = renderVerticesInternal(font, cache, size, text, alignment);
    vertexBuffer.setData(vertices, usage);

    const UnsignedInt glyphCount = vertices.size()/4;
    const UnsignedInt indexCount = glyphCount*6;

    /* Render indices and upload them */
    Containers::Array<char> indices;
    MeshIndexType indexType;
    std::tie(indices, indexType) = renderIndicesInternal(glyphCount);
    indexBuffer.setData(indices, usage);

    /* Configure mesh except for vertex buffer (depends on dimension count, done
       in subclass) */
    GL::Mesh mesh;
    mesh.setPrimitive(MeshPrimitive::Triangles)
        .setCount(indexCount)
        .setIndexBuffer(indexBuffer, 0, indexType, 0, vertices.size());

    return std::make_tuple(std::move(mesh), rectangle);
}

}

std::tuple<std::vector<Vector2>, std::vector<Vector2>, std::vector<UnsignedInt>, Range2D> AbstractRenderer::render(AbstractFont& font, const GlyphCache& cache, Float size, const std::string& text, Alignment alignment) {
    /* Render vertices */
    std::vector<Vertex> vertices;
    Range2D rectangle;
    std::tie(vertices, rectangle) = renderVerticesInternal(font, cache, size, text, alignment);

    /* Deinterleave the vertices */
    std::vector<Vector2> positions, textureCoordinates;
    positions.reserve(vertices.size());
    positions.reserve(textureCoordinates.size());
    for(const auto& v: vertices) {
        positions.push_back(v.position);
        textureCoordinates.push_back(v.textureCoordinates);
    }

    /* Render indices */
    const UnsignedInt glyphCount = vertices.size()/4;
    std::vector<UnsignedInt> indices(glyphCount*6);
    createIndices<UnsignedInt>(indices.data(), glyphCount);

    return std::make_tuple(std::move(positions), std::move(textureCoordinates), std::move(indices), rectangle);
}

template<UnsignedInt dimensions> std::tuple<GL::Mesh, Range2D> Renderer<dimensions>::render(AbstractFont& font, const GlyphCache& cache, Float size, const std::string& text, GL::Buffer& vertexBuffer, GL::Buffer& indexBuffer, GL::BufferUsage usage, Alignment alignment) {
    /* Finalize mesh configuration and return the result */
    auto r = renderInternal(font, cache, size, text, vertexBuffer, indexBuffer, usage, alignment);
    GL::Mesh& mesh = std::get<0>(r);
    mesh.addVertexBuffer(vertexBuffer, 0,
            typename Shaders::AbstractVector<dimensions>::Position(
                Shaders::AbstractVector<dimensions>::Position::Components::Two),
            typename Shaders::AbstractVector<dimensions>::TextureCoordinates());
    return r;
}

#if defined(MAGNUM_TARGET_GLES2) && !defined(CORRADE_TARGET_EMSCRIPTEN)
AbstractRenderer::BufferMapImplementation AbstractRenderer::bufferMapImplementation = &AbstractRenderer::bufferMapImplementationFull;
AbstractRenderer::BufferUnmapImplementation AbstractRenderer::bufferUnmapImplementation = &AbstractRenderer::bufferUnmapImplementationDefault;

void* AbstractRenderer::bufferMapImplementationFull(GL::Buffer& buffer, GLsizeiptr) {
    return buffer.map(GL::Buffer::MapAccess::WriteOnly);
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) || defined(CORRADE_TARGET_EMSCRIPTEN)
inline void* AbstractRenderer::bufferMapImplementation(GL::Buffer& buffer, GLsizeiptr length)
#else
void* AbstractRenderer::bufferMapImplementationRange(GL::Buffer& buffer, GLsizeiptr length)
#endif
{
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    return buffer.map(0, length, GL::Buffer::MapFlag::InvalidateBuffer|GL::Buffer::MapFlag::Write);
    #else
    static_cast<void>(length);
    return &buffer == &_indexBuffer ? _indexBufferData : _vertexBufferData;
    #endif
}

#if !defined(MAGNUM_TARGET_GLES2) || defined(CORRADE_TARGET_EMSCRIPTEN)
inline void AbstractRenderer::bufferUnmapImplementation(GL::Buffer& buffer)
#else
void AbstractRenderer::bufferUnmapImplementationDefault(GL::Buffer& buffer)
#endif
{
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    buffer.unmap();
    #else
    buffer.setSubData(0, &buffer == &_indexBuffer ? _indexBufferData : _vertexBufferData);
    #endif
}

AbstractRenderer::AbstractRenderer(AbstractFont& font, const GlyphCache& cache, const Float size, const Alignment alignment): _vertexBuffer{GL::Buffer::TargetHint::Array}, _indexBuffer{GL::Buffer::TargetHint::ElementArray}, font(font), cache(cache), size(size), _alignment(alignment), _capacity(0) {
    #ifndef MAGNUM_TARGET_GLES
    MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::map_buffer_range);
    #elif defined(MAGNUM_TARGET_GLES2) && !defined(CORRADE_TARGET_EMSCRIPTEN)
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::map_buffer_range>()) {
        bufferMapImplementation = &AbstractRenderer::bufferMapImplementationRange;
    } else {
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::OES::mapbuffer);
        Warning() << "Text::Renderer:" << GL::Extensions::EXT::map_buffer_range::string()
                  << "is not supported, using inefficient" << GL::Extensions::OES::mapbuffer::string()
                  << "instead";
    }
    #endif

    /* Vertex buffer configuration depends on dimension count, done in subclass */
    _mesh.setPrimitive(MeshPrimitive::Triangles);
}

AbstractRenderer::~AbstractRenderer() = default;

template<UnsignedInt dimensions> Renderer<dimensions>::Renderer(AbstractFont& font, const GlyphCache& cache, const Float size, const Alignment alignment): AbstractRenderer(font, cache, size, alignment) {
    /* Finalize mesh configuration */
    _mesh.addVertexBuffer(_vertexBuffer, 0,
            typename Shaders::AbstractVector<dimensions>::Position(Shaders::AbstractVector<dimensions>::Position::Components::Two),
            typename Shaders::AbstractVector<dimensions>::TextureCoordinates());
}

void AbstractRenderer::reserve(const uint32_t glyphCount, const GL::BufferUsage vertexBufferUsage, const GL::BufferUsage indexBufferUsage) {
    _capacity = glyphCount;

    const UnsignedInt vertexCount = glyphCount*4;

    /* Allocate vertex buffer, reset vertex count */
    _vertexBuffer.setData({nullptr, vertexCount*sizeof(Vertex)}, vertexBufferUsage);
    #ifdef CORRADE_TARGET_EMSCRIPTEN
    _vertexBufferData = Containers::Array<UnsignedByte>(vertexCount*sizeof(Vertex));
    #endif
    _mesh.setCount(0);

    /* Render indices */
    Containers::Array<char> indexData;
    MeshIndexType indexType;
    std::tie(indexData, indexType) = renderIndicesInternal(glyphCount);

    /* Allocate index buffer, reset index count and reconfigure buffer binding */
    _indexBuffer.setData({nullptr, indexData.size()}, indexBufferUsage);
    #ifdef CORRADE_TARGET_EMSCRIPTEN
    _indexBufferData = Containers::Array<UnsignedByte>(indexData.size());
    #endif
    _mesh.setCount(0)
        .setIndexBuffer(_indexBuffer, 0, indexType, 0, vertexCount);

    /* Prefill index buffer */
    char* const indices = static_cast<char*>(bufferMapImplementation(_indexBuffer, indexData.size()));
    CORRADE_INTERNAL_ASSERT(indices);
    /** @todo Emscripten: it can be done without this copying altogether */
    std::copy(indexData.begin(), indexData.end(), indices);
    bufferUnmapImplementation(_indexBuffer);
}

void AbstractRenderer::render(const std::string& text) {
    /* Render vertex data */
    std::vector<Vertex> vertexData;
    _rectangle = {};
    std::tie(vertexData, _rectangle) = renderVerticesInternal(font, cache, size, text, _alignment);

    const UnsignedInt glyphCount = vertexData.size()/4;
    const UnsignedInt vertexCount = glyphCount*4;
    const UnsignedInt indexCount = glyphCount*6;

    CORRADE_ASSERT(glyphCount <= _capacity,
        "Text::Renderer::render(): capacity" << _capacity << "too small to render" << glyphCount << "glyphs", );

    /* Interleave the data into mapped buffer*/
    Containers::ArrayView<Vertex> vertices(static_cast<Vertex*>(bufferMapImplementation(_vertexBuffer,
        vertexCount*sizeof(Vertex))), vertexCount);
    CORRADE_INTERNAL_ASSERT_OUTPUT(vertices);
    std::copy(vertexData.begin(), vertexData.end(), vertices.begin());
    bufferUnmapImplementation(_vertexBuffer);

    /* Update index count */
    _mesh.setCount(indexCount);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template class MAGNUM_TEXT_EXPORT Renderer<2>;
template class MAGNUM_TEXT_EXPORT Renderer<3>;
#endif

}}
