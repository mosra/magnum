#ifndef Magnum_Text_TextRenderer_h
#define Magnum_Text_TextRenderer_h
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

/** @file
 * @brief Class Magnum::Text::TextRenderer
 */

#include "Math/Vector2.h"
#include "Buffer.h"
#include "DimensionTraits.h"
#include "Text/Text.h"

#include "magnumTextVisibility.h"

#include <string>
#include <tuple>
#include <vector>

namespace Magnum { namespace Text {

/**
@brief %Text renderer

Lays out the text into mesh using [HarfBuzz](http://www.freedesktop.org/wiki/Software/HarfBuzz)
library. Use of ligatures, kerning etc. depends on features supported by the
particular font.
@see TextRenderer2D, TextRenderer3D, Font, Shaders::AbstractTextShader
*/
template<std::uint8_t dimensions> class MAGNUM_TEXT_EXPORT TextRenderer {
    public:
        /**
         * @brief Render text
         * @param font          %Font to use
         * @param size          %Font size
         * @param text          %Text to render
         *
         * Returns tuple with vertex positions, texture coordinates, indices
         * and rectangle spanning the rendered text.
         */
        static std::tuple<std::vector<typename DimensionTraits<dimensions>::PointType>, std::vector<Vector2>, std::vector<std::uint32_t>, Rectangle> render(Font& font, GLfloat size, const std::string& text);

        /**
         * @brief Render text
         * @param font          %Font to use
         * @param size          %Font size
         * @param text          %Text to render
         * @param vertexBuffer  %Buffer where to store vertices
         * @param indexBuffer   %Buffer where to store indices
         * @param usage         Usage of vertex and index buffer
         *
         * Returns mesh prepared for use with Shaders::AbstractTextShader
         * subclasses and rectangle spanning the rendered text.
         */
        static std::tuple<Mesh, Rectangle> render(Font& font, GLfloat size, const std::string& text, Buffer* vertexBuffer, Buffer* indexBuffer, Buffer::Usage usage);

    private:
        #ifndef DOXYGEN_GENERATING_OUTPUT
        struct Vertex {
            typename DimensionTraits<dimensions>::PointType position;
            Vector2 textureCoordinates;
        };
        #endif
};

/** @brief Two-dimensional text renderer */
typedef TextRenderer<2> TextRenderer2D;

/** @brief Three-dimensional text renderer */
typedef TextRenderer<3> TextRenderer3D;

}}

#endif
