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

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/Utility/Resource.h>

#include "Magnum/FileCallback.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Shaders/Vector.h"
#include "Magnum/Text/AbstractFont.h"
#include "Magnum/Text/DistanceFieldGlyphCache.h"
#include "Magnum/Text/Renderer.h"

using namespace Magnum;
using namespace Magnum::Math::Literals;

int main() {

{
/* [AbstractFont-usage] */
PluginManager::Manager<Text::AbstractFont> manager;
Containers::Pointer<Text::AbstractFont> font =
    manager.loadAndInstantiate("StbTrueTypeFont");
if(!font || !font->openFile("font.ttf", 16.0f))
    Fatal{} << "Can't open font.ttf with StbTrueTypeFont";

Text::GlyphCache cache{Vector2i{512}};
font->fillGlyphCache(cache, "abcdefghijklmnopqrstuvwxyz"
                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                            "0123456789?!:;,. ");
/* [AbstractFont-usage] */
}

#if defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT))
{
Containers::Pointer<Text::AbstractFont> font;
/* [AbstractFont-usage-callbacks] */
struct Data {
    std::unordered_map<std::string,
        Containers::Array<const char, Utility::Directory::MapDeleter>> files;
} data;

font->setFileCallback([](const std::string& filename,
    InputFileCallbackPolicy policy, Data& data)
        -> Containers::Optional<Containers::ArrayView<const char>>
    {
        auto found = data.files.find(filename);

        /* Discard the memory mapping, if not needed anymore */
        if(policy == InputFileCallbackPolicy::Close) {
            if(found != data.files.end()) data.files.erase(found);
            return {};
        }

        /* Load if not there yet */
        if(found == data.files.end()) found = data.files.emplace(
            filename, Utility::Directory::mapRead(filename)).first;

        return Containers::arrayView(found->second);
    }, data);

font->openFile("magnum-font.conf", 13.0f);
/* [AbstractFont-usage-callbacks] */
}
#endif

{
Containers::Pointer<Text::AbstractFont> font;
/* [AbstractFont-setFileCallback] */
font->setFileCallback([](const std::string& filename,
    InputFileCallbackPolicy, void*) {
        Utility::Resource rs{"data"};
        return Containers::optional(rs.getRaw(filename));
    });
/* [AbstractFont-setFileCallback] */
}

{
Containers::Pointer<Text::AbstractFont> font;
/* [AbstractFont-setFileCallback-template] */
const Utility::Resource rs{"data"};
font->setFileCallback([](const std::string& filename,
    InputFileCallbackPolicy, const Utility::Resource& rs) {
        return Containers::optional(rs.getRaw(filename));
    }, rs);
/* [AbstractFont-setFileCallback-template] */
}

{
/* [DistanceFieldGlyphCache-usage] */
Containers::Pointer<Text::AbstractFont> font;
Text::DistanceFieldGlyphCache cache{Vector2i{2048}, Vector2i{384}, 16};
font->fillGlyphCache(cache, "abcdefghijklmnopqrstuvwxyz"
                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                            "0123456789?!:;,. ");
/* [DistanceFieldGlyphCache-usage] */
}

{
/* [GlyphCache-usage] */
Containers::Pointer<Text::AbstractFont> font;
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
Containers::Pointer<Text::AbstractFont> font;

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
    .bindVectorTexture(cache.texture())
    .draw(mesh);
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
    .bindVectorTexture(cache.texture())
    .draw(renderer.mesh());
/* [Renderer-usage2] */
}

}
