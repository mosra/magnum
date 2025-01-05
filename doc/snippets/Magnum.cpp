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

#include <Corrade/Containers/StridedArrayView.h>

#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/VertexFormat.h"
#ifdef MAGNUM_TARGET_GL
#include "Magnum/ResourceManager.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/GL/Texture.h"
#endif

#define DOXYGEN_ELLIPSIS(...) __VA_ARGS__

using namespace Magnum;
using namespace Magnum::Math::Literals;

#ifdef MAGNUM_TARGET_GL
namespace Wew {
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
}
#endif

/* Make sure the name doesn't conflict with any other snippets to avoid linker
   warnings, unlike with `int main()` there now has to be a declaration to
   avoid -Wmisssing-prototypes */
void mainMagnum();
void mainMagnum() {
{
/* [features-using-namespace] */
using namespace Corrade;
using namespace Magnum;
/* [features-using-namespace] */
}

{
/* [features-using-namespace-alias] */
namespace Cr = Corrade;
namespace Mn = Magnum;
/* [features-using-namespace-alias] */
}

{
/* The same #include is already above so this shouldn't hurt */
/* [features-forward-declaration-use] */
#include <Magnum/Magnum.h>        /* only a Matrix4 forward declaration */
#include <Magnum/Math/Matrix4.h>  /* the actual definition */

DOXYGEN_ELLIPSIS()

Matrix4 a = Matrix4::translation({3.0f, 1.0f, 0.5f});
/* [features-forward-declaration-use] */
static_cast<void>(a);
}

{
/* [features-debug-output] */
Image2D image = DOXYGEN_ELLIPSIS(Image2D{{}, {}, {}});

Debug{} << "Image format is" << image.format() << "and size" << image.size();
Debug{} << "Color of the first pixel is" << image.pixels<Color4ub>()[0][0];
/* [features-debug-output] */
}

{
/* data{} makes GCC 4.8 warn about zero as null pointer constant */
std::nullptr_t data = nullptr;
/* [Image-pixels] */
Image2D image{PixelFormat::RGB8Unorm, {128, 128}, data};

Containers::StridedArrayView2D<Color3ub> pixels = image.pixels<Color3ub>();
for(auto row: pixels.sliceSize({48, 48}, {32, 32})) {
    for(Color3ub& pixel: row) pixel *= 1.1f;
}
/* [Image-pixels] */
}

{
char data[3];
/* [ImageView-usage] */
ImageView2D view{PixelFormat::RGBA8Unorm, {512, 256}, data};
/* [ImageView-usage] */
}

{
char tightlyPackedData[3];
/* [ImageView-usage-alignment] */
PixelFormat format = DOXYGEN_ELLIPSIS({});
Vector2i size = DOXYGEN_ELLIPSIS({});
std::size_t rowLength = size.x()*pixelFormatSize(format);

ImageView2D view{
    PixelStorage{}.setAlignment(rowLength % 4 == 0 ? 4 : 1),
    format, size, tightlyPackedData};
/* [ImageView-usage-alignment] */
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
ImageView2D view{
    PixelStorage{}
        .setAlignment(1)
        .setRowLength(75)
        .setSkip({25, 25, 0}),
    PixelFormat::RGBA8Unorm, {25, 25}, data};
/* [ImageView-usage-storage] */
}

#ifdef MAGNUM_TARGET_GL
{
char data[3];
/* [ImageView-usage-gl] */
ImageView2D view{GL::PixelFormat::DepthComponent,
                 GL::PixelType::UnsignedInt, {512, 256}, data};
/* [ImageView-usage-gl] */

/* [ImageView-usage-gl-extract] */
auto format = pixelFormatUnwrap<GLenum>(view.format());
auto type = GLenum(view.formatExtra());
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
CompressedImageView2D view{CompressedPixelFormat::Bc1RGBUnorm,
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
CompressedImageView2D view{
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
CompressedImageView2D view{GL::CompressedPixelFormat::SignedRGRgtc2,
    {512, 256}, data};
/* [CompressedImageView-usage-gl] */

/* [CompressedImageView-usage-gl-extract] */
auto format = compressedPixelFormatUnwrap<GLenum>(view.format());
/* [CompressedImageView-usage-gl-extract] */
static_cast<void>(format);
}
#endif

{
/* [Image-usage] */
Vector2i size{512, 256};
PixelFormat format = PixelFormat::RGBA8Unorm;

Image2D image{format, size, Containers::Array<char>{ValueInit,
    std::size_t(size.product()*pixelFormatSize(format))}};
/* [Image-usage] */
}

{
/* [Image-usage-padding] */
PixelFormat format = PixelFormat::RGB8Unorm;
Vector2i size{173, 232};
std::size_t rowStride = 4*((size.x()*pixelFormatSize(format) + 3)/4);

Image2D image{format, size,
    Containers::Array<char>{ValueInit, std::size_t(size.y()*rowStride)}};
/* [Image-usage-padding] */
}

{
/* [Image-usage-alignment] */
Vector2i size{173, 232};
PixelFormat format = PixelFormat::RGB8Unorm;
std::size_t rowLength = size.x()*pixelFormatSize(format);

Image2D image{
    PixelStorage{}.setAlignment(rowLength % 4 == 0 ? 4 : 1), format, size,
    Containers::Array<char>{ValueInit, std::size_t(size.y()*rowLength)}};
/* [Image-usage-alignment] */
}

#if defined(MAGNUM_TARGET_GL) && !defined(MAGNUM_TARGET_GLES)
{
/* [Image-usage-query] */
GL::Texture2D texture;
Image2D image = texture.image(0, Image2D{PixelFormat::Depth32F});
/* [Image-usage-query] */
}
#endif

{
/* [CompressedImage-usage] */
CompressedImage2D image{CompressedPixelFormat::Bc1RGBUnorm,
    {512, 256}, Containers::Array<char>{ValueInit, DOXYGEN_ELLIPSIS(0)}};
/* [CompressedImage-usage] */
}

#if defined(MAGNUM_TARGET_GL) && !defined(MAGNUM_TARGET_GLES)
{
/* [CompressedImage-usage-query] */
GL::Texture2D texture;
CompressedImage2D image = texture.compressedImage(0, CompressedImage2D{});
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
using namespace Wew;
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

{
/* [vertexFormat] */
VertexFormat normalFormat = DOXYGEN_ELLIPSIS({});

VertexFormat tangentFormat = vertexFormat(
    normalFormat,
    4,
    isVertexFormatNormalized(normalFormat));
/* [vertexFormat] */
static_cast<void>(tangentFormat);
}

{
/* [pixelFormat] */
PixelFormat grayscaleFormat = DOXYGEN_ELLIPSIS({});

PixelFormat rgbFormat = pixelFormat(
    grayscaleFormat,
    pixelFormatChannelCount(grayscaleFormat) == 2 ? 4 : 3,
    isPixelFormatSrgb(grayscaleFormat));
/* [pixelFormat] */
static_cast<void>(rgbFormat);
}

}
