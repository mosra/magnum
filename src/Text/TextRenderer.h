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
 * @brief Class Magnum::Text::AbstractTextRenderer, Magnum::Text::TextRenderer, typedef Magnum::Text::TextRenderer2D, Magnum::Text::TextRenderer3D
 */

#include "Math/Geometry/Rectangle.h"
#include "Buffer.h"
#include "DimensionTraits.h"
#include "Mesh.h"
#include "Text/Text.h"

#include "magnumTextVisibility.h"

#include <string>
#include <tuple>
#include <vector>

namespace Magnum { namespace Text {

/**
@brief Base for text renderers

Not meant to be used directly, see TextRenderer for more information.
@see TextRenderer2D, TextRenderer3D
*/
class MAGNUM_TEXT_EXPORT AbstractTextRenderer {
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
        static std::tuple<std::vector<Vector2>, std::vector<Vector2>, std::vector<UnsignedInt>, Rectangle> render(AbstractFont& font, Float size, const std::string& text);

        /**
         * @brief Constructor
         * @param font          %Font to use
         * @param size          %Font size
         */
        AbstractTextRenderer(AbstractFont& font, Float size);

        virtual ~AbstractTextRenderer() = 0;

        /**
         * @brief Capacity for rendered glyphs
         *
         * @see reserve()
         */
        inline UnsignedInt capacity() const { return _capacity; }

        /** @brief Rectangle spanning the rendered text */
        inline Rectangle rectangle() const { return _rectangle; }

        /** @brief Text mesh */
        inline Mesh* mesh() { return &_mesh; }

        /**
         * @brief Reserve capacity for rendered glyphs
         *
         * Reallocates memory in buffers to hold @p glyphCount glyphs and
         * prefills index buffer. Consider using appropriate @p vertexBufferUsage
         * if the text will be changed frequently. Index buffer is changed
         * only by calling this function, thus @p indexBufferUsage generally
         * doesn't need to be so dynamic if the capacity won't be changed much.
         *
         * Initially zero capacity is reserved.
         * @see capacity()
         */
        void reserve(const UnsignedInt glyphCount, const Buffer::Usage vertexBufferUsage, const Buffer::Usage indexBufferUsage);

        /**
         * @brief Render text
         *
         * Renders the text to vertex buffer, reusing index buffer already
         * filled with reserve(). Rectangle spanning the rendered text is
         * available through rectangle().
         *
         * Initially no text is rendered.
         * @attention The capacity must be large enough to contain all glyphs,
         *      see reserve() for more information.
         */
        void render(const std::string& text);

    #ifndef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        static std::tuple<Mesh, Rectangle> MAGNUM_LOCAL render(AbstractFont& font, Float size, const std::string& text, Buffer* vertexBuffer, Buffer* indexBuffer, Buffer::Usage usage);

        Mesh _mesh;
        Buffer vertexBuffer, indexBuffer;

    private:
        AbstractFont& font;
        Float size;
        UnsignedInt _capacity;
        Rectangle _rectangle;
};

/**
@brief %Text renderer

Lays out the text into mesh using given Font. Use of ligatures, kerning etc.
depends on features supported by particular font and its layouter.

@section TextRenderer-usage Usage

Immutable text (e.g. menu items, credits) can be simply rendered using static
methods, returning result either as data arrays or as fully configured mesh.
The text can be then drawn by configuring text shader, binding font texture
and drawing the mesh:
@code
Text::Font font;
Shaders::VectorShader2D shader;
Buffer vertexBuffer, indexBuffer;
Mesh mesh;

// Render the text
Rectangle rectangle;
std::tie(mesh, rectangle) = Text::TextRenderer2D::render(font, 0.15f,
    "Hello World!", &vertexBuffer, &indexBuffer, Buffer::Usage::StaticDraw);

// Draw white text centered on the screen
shader.setTransformationProjectionMatrix(projection*Matrix3::translation(-rectangle.width()/2.0f))
    ->setColor(Color3<>(1.0f));
    ->use();
font.texture()->bind(Shaders::VectorShader2D::FontTextureLayer);
mesh.draw();
@endcode
See render(Font&, Float, const std::string&) and
render(Font&, Float, const std::string&, Buffer*, Buffer*, Buffer::Usage)
for more information.

While this method is sufficient for one-shot rendering of static texts, for
mutable texts (e.g. FPS counters, chat messages) there is another approach
that doesn't recreate everything on each text change:
@code
Text::Font font;
Shaders::VectorShader2D shader;

// Initialize renderer and reserve memory for enough glyphs
Text::TextRenderer2D renderer(font, 0.15f);
renderer.reserve(32, Buffer::Usage::DynamicDraw, Buffer::Usage::StaticDraw);

// Update the text occasionally
renderer.render("Hello World Countdown: 10");

// Draw the text centered on the screen
shader.setTransformationProjectionMatrix(projection*Matrix3::translation(-renderer.rectangle().width()/2.0f))
    ->setColor(Color3<>(1.0f));
    ->use();
font.texture()->bind(Shaders::VectorShader2D::FontTextureLayer);
renderer.mesh().draw();
@endcode

@section TextRenderer-extensions Required OpenGL functionality

Mutable text rendering requires @extension{ARB,map_buffer_range} (also part of
OpenGL ES 3.0 or available as @es_extension{EXT,map_buffer_range} in ES 2.0)
for asynchronous buffer updates.

@see TextRenderer2D, TextRenderer3D, Font, Shaders::AbstractVectorShader
*/
template<UnsignedInt dimensions> class MAGNUM_TEXT_EXPORT TextRenderer: public AbstractTextRenderer {
    public:
        /**
         * @brief Render text
         * @param font          %Font to use
         * @param size          %Font size
         * @param text          %Text to render
         * @param vertexBuffer  %Buffer where to store vertices
         * @param indexBuffer   %Buffer where to store indices
         * @param usage         Usage of vertex and index buffer
         *
         * Returns mesh prepared for use with Shaders::AbstractVectorShader
         * subclasses and rectangle spanning the rendered text.
         */
        static std::tuple<Mesh, Rectangle> render(AbstractFont& font, Float size, const std::string& text, Buffer* vertexBuffer, Buffer* indexBuffer, Buffer::Usage usage);

        /**
         * @brief Constructor
         * @param font          %Font to use
         * @param size          %Font size
         */
        TextRenderer(AbstractFont& font, Float size);

        using AbstractTextRenderer::render;
};

/** @brief Two-dimensional text renderer */
typedef TextRenderer<2> TextRenderer2D;

/** @brief Three-dimensional text renderer */
typedef TextRenderer<3> TextRenderer3D;

}}

#endif
