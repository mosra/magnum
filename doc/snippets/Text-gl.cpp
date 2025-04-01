/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include <Corrade/Containers/StringView.h>
#include <Corrade/PluginManager/Manager.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Range.h"
#include "Magnum/GL/MeshView.h"
#include "Magnum/GL/Renderer.h"
#include "Magnum/Shaders/VectorGL.h"
#include "Magnum/Shaders/DistanceFieldVectorGL.h"
#include "Magnum/Text/AbstractFont.h"
#include "Magnum/Text/AbstractShaper.h"
#include "Magnum/Text/DistanceFieldGlyphCacheGL.h"
#include "Magnum/Text/RendererGL.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <string>
#include <tuple>

#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Mesh.h"
#endif

#define DOXYGEN_ELLIPSIS(...) __VA_ARGS__

using namespace Magnum;
using namespace Magnum::Math::Literals;

namespace {
    Vector2i windowSize() { return {}; }
}

/* Make sure the name doesn't conflict with any other snippets to avoid linker
   warnings, unlike with `int main()` there now has to be a declaration to
   avoid -Wmisssing-prototypes */
void mainTextGL();
void mainTextGL() {
{
/* [AbstractFont-usage] */
PluginManager::Manager<Text::AbstractFont> manager;
Containers::Pointer<Text::AbstractFont> font =
    manager.loadAndInstantiate("StbTrueTypeFont");
if(!font->openFile("font.ttf", 12.0f))
    Fatal{} << "Can't open font.ttf with StbTrueTypeFont";

Text::GlyphCacheGL cache{PixelFormat::R8Unorm, {256, 256}};
if(!font->fillGlyphCache(cache, "abcdefghijklmnopqrstuvwxyz"
                                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                "0123456789?!:;,. "))
    Fatal{} << "Glyph cache too small to fit all characters";
/* [AbstractFont-usage] */
}

{
/* [AbstractGlyphCache-usage-construct] */
Text::GlyphCacheGL cache{PixelFormat::R8Unorm, {256, 256}};
/* [AbstractGlyphCache-usage-construct] */

/* [AbstractGlyphCache-usage-draw] */
Text::RendererGL renderer{cache};
DOXYGEN_ELLIPSIS()

Shaders::VectorGL2D shader;
shader
    DOXYGEN_ELLIPSIS()
    .bindVectorTexture(cache.texture())
    .draw(renderer.mesh());
/* [AbstractGlyphCache-usage-draw] */
}

#ifndef MAGNUM_TARGET_GLES2
{
PluginManager::Manager<Text::AbstractFont> manager;
/* [GlyphCacheArrayGL-usage] */
Containers::Pointer<Text::AbstractFont> font = DOXYGEN_ELLIPSIS(manager.loadAndInstantiate(""));

Text::GlyphCacheArrayGL cache{PixelFormat::R8Unorm, {256, 256, 8}};
if(!font->fillGlyphCache(cache, "abcdefghijklmnopqrstuvwxyz"
                                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                "0123456789?!:;,. "))
    Fatal{} << "Glyph cache too small to fit all characters";
/* [GlyphCacheArrayGL-usage] */

/* [GlyphCacheArrayGL-usage-draw] */
Text::RendererGL renderer{cache};
DOXYGEN_ELLIPSIS()

Shaders::VectorGL2D shader{Shaders::VectorGL2D::Configuration{}
    .setFlags(Shaders::VectorGL2D::Flag::TextureArrays)
};
shader
    DOXYGEN_ELLIPSIS()
    .bindVectorTexture(cache.texture())
    .draw(renderer.mesh());
/* [GlyphCacheArrayGL-usage-draw] */
}
#endif

{
/* -Wnonnull in GCC 11+  "helpfully" says "this is null" if I don't initialize
   the font pointer. I don't care, I just want you to check compilation errors,
   not more! */
PluginManager::Manager<Text::AbstractFont> manager;
/* [DistanceFieldGlyphCacheGL-usage] */
Containers::Pointer<Text::AbstractFont> font = DOXYGEN_ELLIPSIS(manager.loadAndInstantiate(""));
font->openFile("font.ttf", 96.0f);

Text::DistanceFieldGlyphCacheGL cache{Vector2i{1024}, Vector2i{128}, 12};
if(!font->fillGlyphCache(cache, "abcdefghijklmnopqrstuvwxyz"
                                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                "0123456789?!:;,. "))
    Fatal{} << "Glyph cache too small to fit all characters";
/* [DistanceFieldGlyphCacheGL-usage] */

/* [DistanceFieldGlyphCacheGL-usage-draw] */
Text::RendererGL renderer{cache};
DOXYGEN_ELLIPSIS()

Shaders::DistanceFieldVectorGL2D shader;
shader
    DOXYGEN_ELLIPSIS()
    .bindVectorTexture(cache.texture())
    .draw(renderer.mesh());
/* [DistanceFieldGlyphCacheGL-usage-draw] */
}

{
/* [Renderer-usage-construct] */
Text::GlyphCacheGL cache{PixelFormat::R8Unorm, {256, 256}};
DOXYGEN_ELLIPSIS()

Text::RendererGL renderer{cache};
/* [Renderer-usage-construct] */

/* [Renderer-usage-draw] */
GL::Renderer::enable(GL::Renderer::Feature::Blending);
GL::Renderer::setBlendFunction(
    GL::Renderer::BlendFunction::One,
    GL::Renderer::BlendFunction::OneMinusSourceAlpha);

Shaders::VectorGL2D shader;
shader
    .setTransformationProjectionMatrix(Matrix3::projection(Vector2{windowSize()}))
    .bindVectorTexture(cache.texture())
    .draw(renderer.mesh());
/* [Renderer-usage-draw] */
}

{

PluginManager::Manager<Text::AbstractFont> manager;
Containers::Pointer<Text::AbstractFont> font = manager.loadAndInstantiate("");
Text::GlyphCacheGL cache{PixelFormat::R8Unorm, {256, 256}};
Text::RendererGL renderer{cache};
Containers::Pointer<Text::AbstractShaper> shaper = font->createShaper();
Shaders::VectorGL2D shader;
/* [Renderer-usage-blocks-draw] */
Range1Dui helloRuns = renderer
    DOXYGEN_ELLIPSIS()
    .render(*shaper, shaper->font().size(), "Hello,").second();
Range1Dui helloGlyphs = renderer.glyphsForRuns(helloRuns);

Range1Dui worldRuns = renderer
    DOXYGEN_ELLIPSIS()
    .render(*shaper, shaper->font().size(), "world!").second();
Range1Dui worldGlyphs = renderer.glyphsForRuns(worldRuns);

shader
    .setTransformationProjectionMatrix(Matrix3::projection(Vector2{windowSize()}))
    .bindVectorTexture(cache.texture())
    .setColor(0x3bd267_rgbf)
    .draw(GL::MeshView{renderer.mesh()}
        .setIndexOffset(helloGlyphs.min()*6)
        .setCount(helloGlyphs.size()*6))
    .setColor(0x2f83cc_rgbf)
    .draw(GL::MeshView{renderer.mesh()}
        .setIndexOffset(worldGlyphs.min()*6)
        .setCount(worldGlyphs.size()*6));
/* [Renderer-usage-blocks-draw] */
}

#ifdef MAGNUM_BUILD_DEPRECATED
{
/* -Wnonnull in GCC 11+  "helpfully" says "this is null" if I don't initialize
   the font pointer. I don't care, I just want you to check compilation errors,
   not more! */
PluginManager::Manager<Text::AbstractFont> manager;
CORRADE_IGNORE_DEPRECATED_PUSH
/* [Renderer2D-usage1] */
/* Font instance, received from a plugin manager */
Containers::Pointer<Text::AbstractFont> font = DOXYGEN_ELLIPSIS(manager.loadAndInstantiate(""));

/* Open a 12 pt font */
font->openFile("font.ttf", 12.0f);

/* Populate a glyph cache */
Text::GlyphCacheGL cache{PixelFormat::R8Unorm, Vector2i{128}};
font->fillGlyphCache(cache, "abcdefghijklmnopqrstuvwxyz"
                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                            "0123456789?!:;,. ");

Shaders::VectorGL2D shader;
GL::Buffer vertexBuffer, indexBuffer;
GL::Mesh mesh;

/* Render a 12 pt text, centered */
std::tie(mesh, std::ignore) = Text::Renderer2D::render(*font, cache, 12.0f,
    "Hello World!", vertexBuffer, indexBuffer, GL::BufferUsage::StaticDraw,
    Text::Alignment::LineCenter);

/* Projection matrix is matching application window size to have the size match
   12 pt in other applications, assuming a 96 DPI display and no UI scaling. */
Matrix3 projectionMatrix = Matrix3::projection(Vector2{windowSize()});

/* Draw the text on the screen */
shader
    .setTransformationProjectionMatrix(projectionMatrix)
    .setColor(0xffffff_rgbf)
    .bindVectorTexture(cache.texture())
    .draw(mesh);
/* [Renderer2D-usage1] */
CORRADE_IGNORE_DEPRECATED_POP

CORRADE_IGNORE_DEPRECATED_PUSH
/* [Renderer2D-usage2] */
/* Initialize the renderer and reserve memory for enough glyphs */
Text::Renderer2D renderer{*font, cache, 12.0f, Text::Alignment::LineCenter};
renderer.reserve(32, GL::BufferUsage::DynamicDraw, GL::BufferUsage::StaticDraw);

/* Update the text occasionally */
renderer.render("Hello World Countdown: 10");

/* Draw the text on the screen */
shader.setTransformationProjectionMatrix(projectionMatrix)
    .setColor(0xffffff_rgbf)
    .bindVectorTexture(cache.texture())
    .draw(renderer.mesh());
/* [Renderer2D-usage2] */
CORRADE_IGNORE_DEPRECATED_POP
}
#endif
}
