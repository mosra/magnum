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

#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/ColorMap.h"
#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/DebugTools/ForceRenderer.h"
#include "Magnum/DebugTools/FrameProfiler.h"
#include "Magnum/DebugTools/ResourceManager.h"
#include "Magnum/DebugTools/ObjectRenderer.h"
#include "Magnum/DebugTools/TextureImage.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/CubeMapTexture.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/Math/Range.h"
#include "Magnum/SceneGraph/Drawable.h"
#include "Magnum/SceneGraph/Object.h"
#include "Magnum/SceneGraph/MatrixTransformation3D.h"

#ifndef MAGNUM_TARGET_GLES
#include "Magnum/GL/SampleQuery.h"
#endif
#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/BufferImage.h"
#endif

using namespace Magnum;
using namespace Magnum::Math::Literals;

int main() {
{
SceneGraph::Object<SceneGraph::MatrixTransformation3D>* object{};
/* [debug-tools-renderers] */
// Global instance of debug resource manager, drawable group for the renderers
DebugTools::ResourceManager manager;
SceneGraph::DrawableGroup3D debugDrawables;

// Create renderer options which will be referenced later by "my" resource key
manager.set("my", DebugTools::ObjectRendererOptions{}.setSize(0.3f));

// Create debug renderer for given object, use "my" options for it. The
// renderer is automatically added to the object features and also to
// specified drawable group.
new DebugTools::ObjectRenderer3D{manager, *object, "my", &debugDrawables};
/* [debug-tools-renderers] */
}

#if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
{
/* [ColorMap] */
const auto map = DebugTools::ColorMap::turbo();
const Vector2i size{Int(map.size()), 1};

GL::Texture2D colorMapTexture;
colorMapTexture
    .setMinificationFilter(SamplerFilter::Linear)
    .setMagnificationFilter(SamplerFilter::Linear)
    .setWrapping(SamplerWrapping::ClampToEdge) // or Repeat
    .setStorage(1, GL::TextureFormat::RGB8, size) // or SRGB8
    .setSubImage(0, {}, ImageView2D{PixelFormat::RGB8Srgb, size, map});
/* [ColorMap] */
}
#endif

{
DebugTools::ResourceManager manager;
SceneGraph::Object<SceneGraph::MatrixTransformation3D>* object{};
SceneGraph::DrawableGroup3D debugDrawables;
/* [ForceRenderer] */
manager.set("my", DebugTools::ForceRendererOptions{}
    .setSize(5.0f)
    .setColor(Color3::fromHsv({120.0_degf, 1.0f, 0.7f})));

Vector3 force; // taken as a reference, has to be kept in scope

// Create debug renderer for given force, use "my" options for it
new DebugTools::ForceRenderer3D(manager, *object, {0.3f, 1.5f, -0.7f}, force,
    "my", &debugDrawables);
/* [ForceRenderer] */
}

#ifndef MAGNUM_TARGET_GLES
{
/* [FrameProfiler-setup-delayed] */
GL::SampleQuery queries[3]{
    GL::SampleQuery{GL::SampleQuery::Target::SamplesPassed},
    GL::SampleQuery{GL::SampleQuery::Target::SamplesPassed},
    GL::SampleQuery{GL::SampleQuery::Target::SamplesPassed}
};
DebugTools::FrameProfiler profiler{{
    DebugTools::FrameProfiler::Measurement{"Samples",
        DebugTools::FrameProfiler::Units::Count,
        UnsignedInt(Containers::arraySize(queries)),
        [](void* state, UnsignedInt current) {
            static_cast<GL::SampleQuery*>(state)[current].begin();
        },
        [](void* state, UnsignedInt current) {
            static_cast<GL::SampleQuery*>(state)[current].end();
        },
        [](void* state, UnsignedInt previous, UnsignedInt) {
            return static_cast<GL::SampleQuery*>(state)[previous]
                .result<UnsignedLong>();
        }, queries}
}, 50};
/* [FrameProfiler-setup-delayed] */
}
#endif

{
SceneGraph::Object<SceneGraph::MatrixTransformation3D>* object{};
/* [ObjectRenderer] */
DebugTools::ResourceManager manager;
SceneGraph::DrawableGroup3D debugDrawables;

// Create some options
manager.set("my", DebugTools::ObjectRendererOptions{}.setSize(0.3f));

// Create debug renderer for given object, use "my" options for it
new DebugTools::ObjectRenderer3D{manager, *object, "my", &debugDrawables};
/* [ObjectRenderer] */
}

{
/* [GLFrameProfiler-usage] */
DebugTools::GLFrameProfiler profiler{
    DebugTools::GLFrameProfiler::Value::FrameTime|
    DebugTools::GLFrameProfiler::Value::GpuDuration, 50};
/* [GLFrameProfiler-usage] */
}

{
GL::Texture2D texture;
Range2Di rect;
/* [textureSubImage-2D-rvalue] */
Image2D image = DebugTools::textureSubImage(texture, 0, rect,
    {PixelFormat::RGBA8Unorm});
/* [textureSubImage-2D-rvalue] */
}

#ifndef MAGNUM_TARGET_GLES2
{
GL::Texture2D texture;
Range2Di rect;
/* [textureSubImage-2D-rvalue-buffer] */
GL::BufferImage2D image = DebugTools::textureSubImage(texture, 0, rect,
    {PixelFormat::RGBA8Unorm}, GL::BufferUsage::StaticRead);
/* [textureSubImage-2D-rvalue-buffer] */
}
#endif

{
GL::CubeMapTexture texture;
Range2Di rect;
/* [textureSubImage-cubemap-rvalue] */
Image2D image = DebugTools::textureSubImage(texture,
    GL::CubeMapCoordinate::PositiveX, 0, rect, {PixelFormat::RGBA8Unorm});
/* [textureSubImage-cubemap-rvalue] */
}

#ifndef MAGNUM_TARGET_GLES2
{
GL::CubeMapTexture texture;
Range2Di rect;
/* [textureSubImage-cubemap-rvalue-buffer] */
GL::BufferImage2D image = DebugTools::textureSubImage(texture,
    GL::CubeMapCoordinate::PositiveX, 0, rect, {PixelFormat::RGBA8Unorm},
    GL::BufferUsage::StaticRead);
/* [textureSubImage-cubemap-rvalue-buffer] */
}
#endif
}

struct Foo: TestSuite::Tester {
void foo() {
{
GL::Framebuffer fb{{}};
ImageView2D expected{PixelFormat::RGB8Unorm, {}};
/* [CompareImage-pixels-rgb] */
Image2D image = fb.read(fb.viewport(), {PixelFormat::RGBA8Unorm});

CORRADE_COMPARE_AS(Containers::arrayCast<Color3ub>(image.pixels<Color4ub>()),
    "expected.png", DebugTools::CompareImageToFile);
/* [CompareImage-pixels-rgb] */
}
}
};
