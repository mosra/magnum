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

#include "Magnum/Math/Color.h"
#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#ifdef MAGNUM_TARGET_GL
#include "Magnum/ResourceManager.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/GL/Texture.h"
#endif

using namespace Magnum;
using namespace Magnum::Math::Literals;

#ifdef MAGNUM_TARGET_GL
Containers::Pointer<GL::Mesh> mesh;
bool found = false;

/* [AbstractResourceLoader-implementation] */
class MeshResourceLoader: public AbstractResourceLoader<GL::Mesh> {
    void doLoad(ResourceKey key) override {
        // Load the mesh...

        // Not found
        if(!found) {
            setNotFound(key);
            return;
        }

        // Found, pass it to resource manager
        set(key, std::move(mesh));
    }
};
/* [AbstractResourceLoader-implementation] */
#endif

int main() {

{
std::nullptr_t data{};
/* [Image-pixels] */
Image2D image{PixelFormat::RGB8Unorm, {128, 128}, data};

Containers::StridedArrayView2D<Color3ub> pixels = image.pixels<Color3ub>();
for(auto row: pixels.slice({48, 48}, {80, 80})) {
    for(Color3ub& pixel: row) pixel *= 1.1f;
}
/* [Image-pixels] */
}

{
char data[3];
/* [ImageView-usage] */
ImageView2D image{PixelFormat::RGBA8Unorm, {512, 256}, data};
/* [ImageView-usage] */
}

{
char evenFrameData[3], oddFrameData[3];
/* [ImageView-usage-streaming] */
ImageView2D frame{PixelFormat::RGBA8Unorm, {512, 256}};

frame.setData(evenFrameData);
// Use even frame data ...

frame.setData(oddFrameData);
// Use odd frame data ...
/* [ImageView-usage-streaming] */
}

{
char data[3];
/* [ImageView-usage-storage] */
ImageView2D image{
    PixelStorage{}
        .setRowLength(75)
        .setAlignment(4)
        .setSkip({25, 25, 0}),
    PixelFormat::RGBA8Unorm, {25, 25}, data};
/* [ImageView-usage-storage] */
}

#ifdef MAGNUM_TARGET_GL
{
char data[3];
/* [ImageView-usage-gl] */
ImageView2D image{GL::PixelFormat::DepthComponent,
                  GL::PixelType::UnsignedInt, {512, 256}, data};
/* [ImageView-usage-gl] */

/* [ImageView-usage-gl-extract] */
auto format = pixelFormatUnwrap<GLenum>(image.format());
auto type = GLenum(image.formatExtra());
/* [ImageView-usage-gl-extract] */
static_cast<void>(format);
static_cast<void>(type);
}
#endif

{
char data[3];
#define MTLPixelFormatRGBA8Unorm_sRGB 71
/* [ImageView-usage-metal] */
/* Default pixel storage, 8-bit sRGB + alpha, four bytes per pixel */
ImageView2D view{{}, MTLPixelFormatRGBA8Unorm_sRGB, {}, 4, {256, 256}, data};
/* [ImageView-usage-metal] */
}

{
char data[3];
/* [CompressedImageView-usage] */
CompressedImageView2D image{CompressedPixelFormat::Bc1RGBUnorm,
    {512, 256}, data};
/* [CompressedImageView-usage] */
}

{
char evenFrameData[3], oddFrameData[3];
/* [CompressedImageView-usage-streaming] */
CompressedImageView2D frame{CompressedPixelFormat::Bc1RGBUnorm, {512, 256}};

frame.setData(evenFrameData);
// Use even frame data ...

frame.setData(oddFrameData);
// Use odd frame data ...
/* [CompressedImageView-usage-streaming] */
}

{
char data[3];
/* [CompressedImageView-usage-storage] */
CompressedImageView2D image{
    CompressedPixelStorage{}
        .setRowLength(64)
        .setCompressedBlockSize({4, 4, 1})
        .setCompressedBlockDataSize(8)
        .setSkip({32, 32, 0}),
    CompressedPixelFormat::Bc1RGBUnorm, {32, 32}, data};
/* [CompressedImageView-usage-storage] */
}

#if defined(MAGNUM_TARGET_GL) && !defined(MAGNUM_TARGET_GLES)
{
char data[3];
/* [CompressedImageView-usage-gl] */
CompressedImageView2D image{GL::CompressedPixelFormat::SignedRGRgtc2,
    {512, 256}, data};
/* [CompressedImageView-usage-gl] */

/* [CompressedImageView-usage-gl-extract] */
auto format = compressedPixelFormatUnwrap<GLenum>(image.format());
/* [CompressedImageView-usage-gl-extract] */
static_cast<void>(format);
}
#endif

{
/* [Image-usage] */
Containers::Array<char> data;
Image2D image{PixelFormat::RGBA8Unorm, {512, 256}, std::move(data)};
/* [Image-usage] */
}

#if defined(MAGNUM_TARGET_GL) && !defined(MAGNUM_TARGET_GLES)
{
/* [Image-usage-query] */
GL::Texture2D texture;
Image2D image = texture.image(0, {GL::PixelFormat::DepthComponent,
                                  GL::PixelType::UnsignedInt});
/* [Image-usage-query] */
}
#endif

{
/* [CompressedImage-usage] */
Containers::Array<char> data;
CompressedImage2D image{CompressedPixelFormat::Bc1RGBUnorm,
    {512, 256}, std::move(data)};
/* [CompressedImage-usage] */
}

#if defined(MAGNUM_TARGET_GL) && !defined(MAGNUM_TARGET_GLES)
{
/* [CompressedImage-usage-query] */
GL::Texture2D texture;
CompressedImage2D image = texture.compressedImage(0, {});
/* [CompressedImage-usage-query] */
}
#endif

#ifdef MAGNUM_TARGET_GL
{
/* [ResourceManager-typedef] */
typedef ResourceManager<GL::Mesh, GL::Texture2D, GL::AbstractShaderProgram>
    MyResourceManager;
MyResourceManager manager;
/* [ResourceManager-typedef] */
}

{
typedef ResourceManager<GL::Mesh, GL::Texture2D, GL::AbstractShaderProgram>
    MyResourceManager;
struct MyShader: GL::AbstractShaderProgram {
    MyShader& bindTexture(GL::Texture2D&) { return *this; }
};
/* [ResourceManager-fill] */
MyResourceManager manager;
Resource<GL::Texture2D> texture{manager.get<GL::Texture2D>("texture")};
Resource<GL::AbstractShaderProgram, MyShader> shader =
    manager.get<GL::AbstractShaderProgram, MyShader>("shader");
Resource<GL::Mesh> cube = manager.get<GL::Mesh>("cube");

// The manager doesn't have data for the cube yet, add them
if(!cube) {
    GL::Mesh mesh;
    // ...
    manager.set(cube.key(), std::move(mesh));
}
/* [ResourceManager-fill] */

/* [ResourceManager-use] */
(*shader)
    .bindTexture(*texture)
    .draw(*cube);
/* [ResourceManager-use] */
}

{
ResourceManager<GL::AbstractShaderProgram> manager;
struct MyShader: GL::AbstractShaderProgram {};
/* [ResourceManager-get-derived] */
Resource<GL::AbstractShaderProgram, MyShader> shader =
    manager.get<GL::AbstractShaderProgram, MyShader>("shader");
/* [ResourceManager-get-derived] */
}

{
typedef ResourceManager<GL::Mesh> MyResourceManager;
/* [AbstractResourceLoader-use] */
MyResourceManager manager;
Containers::Pointer<MeshResourceLoader> loader;

manager.setLoader<GL::Mesh>(std::move(loader));

// This will now automatically request the mesh from loader by calling load()
Resource<GL::Mesh> myMesh = manager.get<GL::Mesh>("my-mesh");
/* [AbstractResourceLoader-use] */
}
#endif

}
