/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
              Vladimír Vondruš <mosra@centrum.cz>

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

#include "Magnum/Shaders/Vector.h"
#include "Magnum/Text/AbstractFont.h"
#include "Magnum/Text/DistanceFieldGlyphCache.h"
#include "Magnum/Text/Renderer.h"

using namespace Magnum;
using namespace Magnum::Math::Literals;

int main() {

{
/* [DistanceFieldGlyphCache-usage] */
std::unique_ptr<Text::AbstractFont> font;
Text::DistanceFieldGlyphCache cache{Vector2i{2048}, Vector2i{384}, 16};
font->fillGlyphCache(cache, "abcdefghijklmnopqrstuvwxyz"
                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                            "0123456789?!:;,. ");
/* [DistanceFieldGlyphCache-usage] */
}

{
/* [GlyphCache-usage] */
std::unique_ptr<Text::AbstractFont> font;
Text::GlyphCache cache{Vector2i{512}};
font->fillGlyphCache(cache, "abcdefghijklmnopqrstuvwxyz"
                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                            "0123456789?!:;,. ");
/* [GlyphCache-usage] */
}

{
Matrix3 projectionMatrix;
/* [Renderer-usage1] */
/* Font instance, received from a plugin manager */
std::unique_ptr<Text::AbstractFont> font;

/* Configured glyph cache */
Text::GlyphCache cache{Vector2i{512}};
font->fillGlyphCache(cache, "abcdefghijklmnopqrstuvwxyz"
                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                            "0123456789?!:;,. ");

Shaders::Vector2D shader;
GL::Buffer vertexBuffer, indexBuffer;
GL::Mesh mesh;

/* Render the text, centered */
std::tie(mesh, std::ignore) = Text::Renderer2D::render(*font, cache, 0.15f,
    "Hello World!", vertexBuffer, indexBuffer, GL::BufferUsage::StaticDraw,
    Text::Alignment::LineCenter);

/* Draw the text on the screen */
shader.setTransformationProjectionMatrix(projectionMatrix)
    .setColor(0xffffff_rgbf)
    .bindVectorTexture(cache.texture());
mesh.draw(shader);
/* [Renderer-usage1] */

/* [Renderer-usage2] */
/* Initialize the renderer and reserve memory for enough glyphs */
Text::Renderer2D renderer{*font, cache, 0.15f, Text::Alignment::LineCenter};
renderer.reserve(32, GL::BufferUsage::DynamicDraw, GL::BufferUsage::StaticDraw);

/* Update the text occasionally */
renderer.render("Hello World Countdown: 10");

/* Draw the text on the screen */
shader.setTransformationProjectionMatrix(projectionMatrix)
    .setColor(0xffffff_rgbf)
    .bindVectorTexture(cache.texture());
renderer.mesh().draw(shader);
/* [Renderer-usage2] */
}

}
