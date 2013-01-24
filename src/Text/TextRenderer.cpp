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

#include <hb.h>
#include <hb-icu.h>

#include "Math/Point2D.h"
#include "Math/Point3D.h"
#include "Swizzle.h"
#include "MeshTools/CompressIndices.h"
#include "MeshTools/Interleave.h"
#include "Shaders/AbstractTextShader.h"
#include "Text/Font.h"

namespace Magnum { namespace Text {

namespace {

std::tuple<std::vector<Vector2>, std::vector<Vector2>, std::vector<std::uint32_t>, Rectangle> renderInternal(Font& font, GLfloat size, const std::string& text) {
    /* Prepare HarfBuzz buffer */
    hb_buffer_t *buffer = hb_buffer_create();
    hb_buffer_set_unicode_funcs(buffer, hb_icu_get_unicode_funcs());
    hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
    hb_buffer_set_script(buffer, HB_SCRIPT_LATIN);
    hb_buffer_set_language(buffer, hb_language_from_string("en", 2));

    /* Layout the text */
    hb_buffer_add_utf8(buffer, text.c_str(), -1, 0, -1);
    hb_shape(font.font(), buffer, nullptr, 0);

    std::uint32_t glyphCount;
    hb_glyph_info_t* glyphInfo = hb_buffer_get_glyph_infos(buffer, &glyphCount);
    hb_glyph_position_t* glyphPositions = hb_buffer_get_glyph_positions(buffer, &glyphCount);

    /* Output data */
    std::vector<Vector2> positions, texcoords;
    std::vector<std::uint32_t> indices;
    positions.reserve(glyphCount*4);
    texcoords.reserve(glyphCount*4);
    indices.reserve(glyphCount*6);

    /* Starting cursor position */
    Vector2 cursorPosition;

    /* Create quads for all glyphs */
    for(std::uint32_t i = 0; i != glyphCount; ++i) {
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

        /* 0---2 2
           |  / /|
           | / / |
           |/ /  |
           1 1---3 */

        /* Quad with texture coordinates */
        indices.insert(indices.end(), {
            std::uint32_t(positions.size()),
            std::uint32_t(positions.size()+1),
            std::uint32_t(positions.size()+2),
            std::uint32_t(positions.size()+1),
            std::uint32_t(positions.size()+3),
            std::uint32_t(positions.size()+2)
        });
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

        /* Advance cursor position to next character */
        cursorPosition += advance;
    }

    /* Rendered rectangle */
    Rectangle rectangle;
    if(glyphCount) rectangle = {positions[1], positions[positions.size()-2]};

    /* Destroy HarfBuzz buffer */
    hb_buffer_destroy(buffer);

    return std::make_tuple(std::move(positions), std::move(texcoords), std::move(indices), rectangle);
}

template<std::uint8_t dimensions> typename DimensionTraits<dimensions>::PointType point(const Vector2& vec);

template<> inline Point2D point<2>(const Vector2& vec) {
    return swizzle<'x', 'y', '1'>(vec);
}

template<> inline Point3D point<3>(const Vector2& vec) {
    return swizzle<'x', 'y', '0', '1'>(vec);
}

}

template<std::uint8_t dimensions> std::tuple<std::vector<typename DimensionTraits<dimensions>::PointType>, std::vector<Vector2>, std::vector<std::uint32_t>, Rectangle> TextRenderer<dimensions>::render(Font& font, GLfloat size, const std::string& text) {
    std::vector<Vector2> positions, textureCoordinates;
    std::vector<std::uint32_t> indices;
    Rectangle rectangle;
    std::tie(positions, textureCoordinates, indices, rectangle) = renderInternal(font, size, text);

    /* Create PointXD from Vector2 */
    std::vector<typename DimensionTraits<dimensions>::PointType> positionsXD;
    positionsXD.reserve(positions.size());
    for(const Vector2& position: positions)
        positionsXD.push_back(point<dimensions>(position));

    return std::make_tuple(std::move(positionsXD), std::move(textureCoordinates), std::move(indices), rectangle);
}

template<std::uint8_t dimensions> std::tuple<Mesh, Rectangle> TextRenderer<dimensions>::render(Font& font, GLfloat size, const std::string& text, Buffer* vertexBuffer, Buffer* indexBuffer, Buffer::Usage usage) {
    Mesh mesh;

    std::vector<typename DimensionTraits<dimensions>::PointType> positions;
    std::vector<Vector2> textureCoordinates;
    std::vector<std::uint32_t> indices;
    Rectangle rectangle;
    std::tie(positions, textureCoordinates, indices, rectangle) = render(font, size, text);

    MeshTools::interleave(&mesh, vertexBuffer, usage, positions, textureCoordinates);
    MeshTools::compressIndices(&mesh, indexBuffer, usage, indices);
    mesh.setPrimitive(Mesh::Primitive::Triangles)
        ->addInterleavedVertexBuffer(vertexBuffer, 0,
            typename Shaders::AbstractTextShader<dimensions>::Position(),
            typename Shaders::AbstractTextShader<dimensions>::TextureCoordinates());

    return std::make_tuple(std::move(mesh), rectangle);
}

template class TextRenderer<2>;
template class TextRenderer<3>;

}}
