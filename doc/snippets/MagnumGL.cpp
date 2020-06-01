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

#include <tuple> /* for std::tie() :( */
#include <Corrade/Containers/ArrayViewStl.h>
#include <Corrade/Containers/Reference.h>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/CubeMapTexture.h"
#include "Magnum/GL/DefaultFramebuffer.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/GL/Renderer.h"
#include "Magnum/GL/Renderbuffer.h"
#include "Magnum/GL/RenderbufferFormat.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/GL/Version.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/MeshTools/Interleave.h"
#include "Magnum/MeshTools/CompressIndices.h"
#include "Magnum/Primitives/Cube.h"
#include "Magnum/Primitives/Plane.h"
#include "Magnum/Shaders/Phong.h"
#include "Magnum/Trade/MeshData.h"

#if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
#include "Magnum/GL/SampleQuery.h"
#endif

#ifndef MAGNUM_TARGET_WEBGL
#include "Magnum/GL/DebugOutput.h"
#include "Magnum/GL/TimeQuery.h"
#endif

#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/BufferImage.h"
#include "Magnum/GL/PrimitiveQuery.h"
#include "Magnum/GL/TextureArray.h"
#include "Magnum/GL/TransformFeedback.h"
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
#include "Magnum/GL/BufferTexture.h"
#include "Magnum/GL/BufferTextureFormat.h"
#include "Magnum/GL/CubeMapTextureArray.h"
#include "Magnum/GL/MultisampleTexture.h"
#endif

#ifndef MAGNUM_TARGET_GLES
#include "Magnum/GL/RectangleTexture.h"
#endif

using namespace Magnum;
using namespace Magnum::Math::Literals;

int main() {

#ifndef MAGNUM_TARGET_GLES2
{
ImageView2D diffuse{PixelFormat::RGBA8Unorm, {}};
ImageView2D specular{PixelFormat::RGBA8Unorm, {}};
ImageView2D bump{PixelFormat::RGBA8Unorm, {}};
/* [method-chaining-texture] */
GL::Texture2D carDiffuseTexture, carSpecularTexture, carBumpTexture;

carDiffuseTexture.setStorage(5, GL::TextureFormat::SRGB8, {256, 256});
carSpecularTexture.setStorage(3, GL::TextureFormat::R8, {256, 256});
carBumpTexture.setStorage(5, GL::TextureFormat::RGB8, {256, 256});
carDiffuseTexture.setSubImage(0, {}, diffuse);
carSpecularTexture.setSubImage(0, {}, specular);
carBumpTexture.setSubImage(0, {}, bump);
carDiffuseTexture.generateMipmap();
carSpecularTexture.generateMipmap();
carBumpTexture.generateMipmap();
/* [method-chaining-texture] */

/* [method-chaining-texture-chained] */
carDiffuseTexture.setStorage(5, GL::TextureFormat::SRGB8, {256, 256})
    .setSubImage(0, {}, diffuse)
    .generateMipmap();
carSpecularTexture.setStorage(3, GL::TextureFormat::R8, {256, 256})
    .setSubImage(0, {}, diffuse)
    .generateMipmap();
carBumpTexture.setStorage(5, GL::TextureFormat::RGB8, {256, 256})
    .setSubImage(0, {}, bump)
    .generateMipmap();
/* [method-chaining-texture-chained] */
}
#endif

{
auto importSomeMesh() -> std::tuple<GL::Mesh, GL::Buffer, GL::Buffer>;
/* [opengl-wrapping-nocreate] */
GL::Mesh mesh{NoCreate};
GL::Buffer vertices{NoCreate}, indices{NoCreate};
std::tie(mesh, vertices, indices) = importSomeMesh();
/* [opengl-wrapping-nocreate] */
}

{
struct Foo {
    void setSomeBuffer(GLuint) {}
    GLuint someBuffer() { return {}; }
} externalLib;
char someData[1];
/* [opengl-wrapping-transfer] */
/* Transferring the instance to external library */
{
    GL::Buffer buffer;
    buffer.setData(someData, GL::BufferUsage::StaticDraw);
    GLuint id = buffer.release();
    externalLib.setSomeBuffer(id); /* The library is responsible for deletion */
}

/* Acquiring an instance from external library */
{
    GLuint id = externalLib.someBuffer();
    GL::Buffer buffer = GL::Buffer::wrap(id, GL::ObjectFlag::DeleteOnDestruction);
    /* The buffer instance now handles deletion */
}
/* [opengl-wrapping-transfer] */
}

#ifndef MAGNUM_TARGET_GLES
{
struct: GL::AbstractShaderProgram {} someShader;
/* [opengl-wrapping-state] */
GL::Buffer buffer;
GL::Mesh mesh;
// ...
someShader.draw(mesh);

{
    /* Entering a section with 3rd-party OpenGL code -- clean up all state that
       could cause accidental modifications of our objects from outside */
    GL::Context::current().resetState(GL::Context::State::EnterExternal);

    /* Raw OpenGL calls */
    glBindBuffer(GL_ARRAY_BUFFER, buffer.id());
    glBufferStorage(GL_ARRAY_BUFFER, 32768, nullptr, GL_MAP_READ_BIT|GL_MAP_WRITE_BIT);
    // ...

    /* Exiting a section with 3rd-party OpenGL code -- reset our state tracker */
    GL::Context::current().resetState(GL::Context::State::ExitExternal);
}

/* Use the buffer through Magnum again */
auto data = buffer.map(0, 32768, GL::Buffer::MapFlag::Read|GL::Buffer::MapFlag::Write);
// ...
/* [opengl-wrapping-state] */
static_cast<void>(data);
}
#endif

#ifndef MAGNUM_TARGET_GLES
{
/* [opengl-wrapping-extensions] */
GL::TextureFormat format;
if(GL::Context::current().isExtensionSupported<GL::Extensions::ARB::depth_buffer_float>())
    format = GL::TextureFormat::DepthComponent32F;
else
    format = GL::TextureFormat::DepthComponent24;
/* [opengl-wrapping-extensions] */
static_cast<void>(format);
}
#endif

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
{
/* [opengl-wrapping-dsa] */
GL::Texture2D texture;

/* - on OpenGL 4.5+/ARB_direct_state_access this calls glTextureStorage2D()
   - on OpenGL 4.2+/ARB_texture_storage and OpenGL ES 3.0+ calls glTexStorage2D()
   - on OpenGL ES 2.0 with EXT_texture_storage calls glTexStorage2DEXT()
   - otherwise emulated using a sequence of four glTexImage2D() calls */
texture.setStorage(4, GL::TextureFormat::RGBA8, {256, 256});
/* [opengl-wrapping-dsa] */
}
#endif

{
/* [portability-targets] */
#ifndef MAGNUM_TARGET_GLES
GL::Renderer::setPolygonMode(GL::Renderer::PolygonMode::Line);
// draw mesh as wireframe...
#else
// use different mesh, as polygon mode is not supported in OpenGL ES...
#endif
/* [portability-targets] */
}

#ifndef MAGNUM_TARGET_GLES
{
/* [portability-extensions] */
if(GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>()) {
    // draw mesh with wireframe on top in one pass using geometry shader...
} else {
    // draw underlying mesh...
    GL::Renderer::setPolygonMode(GL::Renderer::PolygonMode::Line);
    // draw mesh as wirefreame in second pass...
}
/* [portability-extensions] */
}

{
/* [portability-extension-assert] */
MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::geometry_shader4);
// just use geometry shader and don't care about old hardware
/* [portability-extension-assert] */
}

{
/* [portability-shaders] */
// MyShader.cpp
GL::Version version = GL::Context::current().supportedVersion({
    GL::Version::GL430, GL::Version::GL330, GL::Version::GL210});
GL::Shader vert{version, GL::Shader::Type::Vertex};
vert.addFile("MyShader.vert");
/* [portability-shaders] */
}
#endif

#ifndef MAGNUM_TARGET_GLES
struct MyShader: GL::AbstractShaderProgram {
/* [AbstractShaderProgram-input-attributes] */
typedef GL::Attribute<0, Vector3> Position;
typedef GL::Attribute<1, Vector3> Normal;
typedef GL::Attribute<2, Vector2> TextureCoordinates;
/* [AbstractShaderProgram-input-attributes] */

/* [AbstractShaderProgram-output-attributes] */
enum: UnsignedInt {
    ColorOutput = 0,
    NormalOutput = 1
};
/* [AbstractShaderProgram-output-attributes] */

#if !defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
/* [AbstractShaderProgram-hide-irrelevant] */
private:
    using GL::AbstractShaderProgram::drawTransformFeedback;
    using GL::AbstractShaderProgram::dispatchCompute;
/* [AbstractShaderProgram-hide-irrelevant] */
public:
#endif

/* [AbstractShaderProgram-constructor] */
explicit MyShader() {
    /* Load shader sources */
    GL::Shader vert{GL::Version::GL430, GL::Shader::Type::Vertex};
    GL::Shader frag{GL::Version::GL430, GL::Shader::Type::Fragment};
    vert.addFile("MyShader.vert");
    frag.addFile("MyShader.frag");

    /* Invoke parallel compilation for best performance */
    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, frag}));

    /* Attach the shaders */
    attachShaders({vert, frag});

    /* Link the program together */
    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}
/* [AbstractShaderProgram-constructor] */

/* [AbstractShaderProgram-uniforms] */
MyShader& setProjectionMatrix(const Matrix4& matrix) {
    setUniform(0, matrix);
    return *this;
}
MyShader& setTransformationMatrix(const Matrix4& matrix) {
    setUniform(1, matrix);
    return *this;
}
MyShader& setNormalMatrix(const Matrix3x3& matrix) {
    setUniform(2, matrix);
    return *this;
}
/* [AbstractShaderProgram-uniforms] */

/* [AbstractShaderProgram-textures] */
MyShader& bindDiffuseTexture(GL::Texture2D& texture) {
    texture.bind(0);
    return *this;
}
MyShader& bindSpecularTexture(GL::Texture2D& texture) {
    texture.bind(1);
    return *this;
}
/* [AbstractShaderProgram-textures] */

/* [AbstractShaderProgram-xfb] */
MyShader& setTransformFeedback(GL::TransformFeedback& feedback,
    GL::Buffer& positions, GL::Buffer& data)
{
    feedback.attachBuffers(0, {&positions, &data});
    return *this;
}
MyShader& setTransformFeedback(GL::TransformFeedback& feedback, Int totalCount,
    GL::Buffer& positions, GLintptr positionsOffset, GL::Buffer& data,
    GLintptr dataOffset)
{
    feedback.attachBuffers(0, {
        std::make_tuple(&positions, positionsOffset, totalCount*sizeof(Vector3)),
        std::make_tuple(&data, dataOffset, totalCount*sizeof(Vector2ui))
    });
    return *this;
}
/* [AbstractShaderProgram-xfb] */

void foo() {
{
GL::Version version{};
/* [portability-shaders-bind] */
if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::explicit_attrib_location>(version)) {
    bindAttributeLocation(Position::Location, "position");
    // ...
}
/* [portability-shaders-bind] */
}

/* [AbstractShaderProgram-binding] */
// Shaders attached...

bindAttributeLocation(Position::Location, "position");
bindAttributeLocation(Normal::Location, "normal");
bindAttributeLocation(TextureCoordinates::Location, "textureCoordinates");

bindFragmentDataLocation(ColorOutput, "color");
bindFragmentDataLocation(NormalOutput, "normal");

// Link...
/* [AbstractShaderProgram-binding] */

/* [AbstractShaderProgram-uniform-location] */
Int projectionMatrixUniform = uniformLocation("projectionMatrix");
Int transformationMatrixUniform = uniformLocation("transformationMatrix");
Int normalMatrixUniform = uniformLocation("normalMatrix");
/* [AbstractShaderProgram-uniform-location] */
static_cast<void>(projectionMatrixUniform);
static_cast<void>(transformationMatrixUniform);
static_cast<void>(normalMatrixUniform);

/* [AbstractShaderProgram-uniform-block-binding] */
setUniformBlockBinding(uniformBlockIndex("matrices"), 0);
setUniformBlockBinding(uniformBlockIndex("material"), 1);
/* [AbstractShaderProgram-uniform-block-binding] */

/* [AbstractShaderProgram-texture-uniforms] */
setUniform(uniformLocation("diffuseTexture"), 0);
setUniform(uniformLocation("specularTexture"), 1);
/* [AbstractShaderProgram-texture-uniforms] */

/* [AbstractShaderProgram-xfb-outputs] */
setTransformFeedbackOutputs({
        // Buffer 0
        "position", "gl_SkipComponents1", "normal", "gl_SkipComponents1",
        // Buffer 1
        "gl_NextBuffer", "velocity"
    }, TransformFeedbackBufferMode::InterleavedAttributes);
/* [AbstractShaderProgram-xfb-outputs] */
}
};
#endif

#ifndef MAGNUM_TARGET_GLES
{
MyShader shader;
GL::Mesh mesh;
Matrix4 transformation, projection;
GL::Texture2D diffuseTexture, specularTexture;
/* [AbstractShaderProgram-rendering] */
shader.setTransformationMatrix(transformation)
    .setProjectionMatrix(projection)
    .bindDiffuseTexture(diffuseTexture)
    .bindSpecularTexture(specularTexture)
    .draw(mesh);
/* [AbstractShaderProgram-rendering] */
}
#endif

{
GL::Framebuffer framebuffer{{}};
/* [AbstractFramebuffer-read1] */
Image2D image = framebuffer.read(framebuffer.viewport(),
    {PixelFormat::RGBA8Unorm});
/* [AbstractFramebuffer-read1] */
}

#ifndef MAGNUM_TARGET_GLES2
{
GL::Framebuffer framebuffer{{}};
/* [AbstractFramebuffer-read2] */
GL::BufferImage2D image = framebuffer.read(framebuffer.viewport(),
    {PixelFormat::RGBA8Unorm}, GL::BufferUsage::StaticRead);
/* [AbstractFramebuffer-read2] */
}
#endif

{
GL::Buffer buffer;
/* [Buffer-setdata] */
Containers::ArrayView<Vector3> data;
buffer.setData(data);

GL::Buffer buffer2{data}; // or construct & fill in a single step
/* [Buffer-setdata] */
}

{
GL::Buffer buffer;
/* [Buffer-setdata-stl] */
std::vector<Vector3> data;
buffer.setData(data);
/* [Buffer-setdata-stl] */

/* [Buffer-setdata-allocate] */
buffer.setData({nullptr, 200*sizeof(Vector3)});
/* [Buffer-setdata-allocate] */
}

#ifndef MAGNUM_TARGET_WEBGL
{
GL::Buffer buffer;
/* [Buffer-map] */
Containers::ArrayView<Vector3> data = Containers::arrayCast<Vector3>(buffer.map(0,
    200*sizeof(Vector3), GL::Buffer::MapFlag::Write|GL::Buffer::MapFlag::InvalidateBuffer));
CORRADE_INTERNAL_ASSERT(data);
for(Vector3& d: data)
    d = {/*...*/};
CORRADE_INTERNAL_ASSERT_OUTPUT(buffer.unmap());
/* [Buffer-map] */
}

{
GL::Buffer buffer;
/* [Buffer-flush] */
Containers::ArrayView<Vector3> data = Containers::arrayCast<Vector3>(buffer.map(0,
    200*sizeof(Vector3), GL::Buffer::MapFlag::Write|GL::Buffer::MapFlag::FlushExplicit));
CORRADE_INTERNAL_ASSERT(data);
for(std::size_t i: {7, 27, 56, 128}) {
    data[i] = {/*...*/};
    buffer.flushMappedRange(i*sizeof(Vector3), sizeof(Vector3));
}
CORRADE_INTERNAL_ASSERT_OUTPUT(buffer.unmap());
/* [Buffer-flush] */
}
#endif

{
/* [Buffer-webgl-nope] */
GL::Buffer vertices, indices;
/* [Buffer-webgl-nope] */
}

{
/* [Buffer-webgl] */
GL::Buffer vertices{GL::Buffer::TargetHint::Array},
    indices{GL::Buffer::TargetHint::ElementArray};
/* [Buffer-webgl] */
}

#ifndef MAGNUM_TARGET_GLES2
{
char data[3];
/* [BufferImage-usage] */
GL::BufferImage2D image{GL::PixelFormat::RGBA, GL::PixelType::UnsignedByte,
    {512, 256}, data, GL::BufferUsage::StaticDraw};
/* [BufferImage-usage] */
}

{
/* [BufferImage-usage-wrap] */
GL::Buffer buffer;
GL::BufferImage2D image{GL::PixelFormat::RGBA, GL::PixelType::UnsignedByte,
    {512, 256}, std::move(buffer), 524288};
/* [BufferImage-usage-wrap] */
}

#ifndef MAGNUM_TARGET_GLES
{
/* [BufferImage-usage-query] */
GL::Texture2D texture;
GL::BufferImage2D image = texture.image(0, {GL::PixelFormat::RGBA,
    GL::PixelType::UnsignedByte}, GL::BufferUsage::StaticRead);
/* [BufferImage-usage-query] */
}
#endif

{
char data[3];
/* [CompressedBufferImage-usage] */
GL::CompressedBufferImage2D image{GL::CompressedPixelFormat::RGBS3tcDxt1,
    {512, 256}, data, GL::BufferUsage::StaticDraw};
/* [CompressedBufferImage-usage] */
}

{
/* [CompressedBufferImage-usage-wrap] */
GL::Buffer buffer;
GL::CompressedBufferImage2D image{GL::CompressedPixelFormat::RGBS3tcDxt1,
    {512, 256}, std::move(buffer), 65536};
/* [CompressedBufferImage-usage-wrap] */
}

#ifndef MAGNUM_TARGET_GLES
{
/* [CompressedBufferImage-usage-query] */
GL::Texture2D texture;
GL::CompressedBufferImage2D image = texture.compressedImage(0, {},
    GL::BufferUsage::StaticRead);
/* [CompressedBufferImage-usage-query] */
}
#endif
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
{
/* [BufferTexture-usage] */
GL::Buffer buffer;
GL::BufferTexture texture;
texture.setBuffer(GL::BufferTextureFormat::RGB32F, buffer);

Vector3 data[200]{
    // ...
};
buffer.setData(data, GL::BufferUsage::StaticDraw);
/* [BufferTexture-usage] */
}
#endif

#ifndef MAGNUM_TARGET_GLES
{
/* [Context-supportedVersion] */
GL::Version v1 = GL::Context::current().isVersionSupported(GL::Version::GL330) ?
    GL::Version::GL330 : GL::Version::GL210;
GL::Version v2 = GL::Context::current().supportedVersion({
    GL::Version::GL330, GL::Version::GL210});
/* [Context-supportedVersion] */
static_cast<void>(v1);
static_cast<void>(v2);

/* [Context-isExtensionSupported] */
if(GL::Context::current().isExtensionSupported<GL::Extensions::ARB::tessellation_shader>()) {
    // draw fancy detailed model
} else {
    // texture fallback
}
/* [Context-isExtensionSupported] */

/* [Context-isExtensionSupported-version] */
const GL::Version version = GL::Context::current().supportedVersion({
    GL::Version::GL320, GL::Version::GL300, GL::Version::GL210});
if(GL::Context::current().isExtensionSupported<GL::Extensions::ARB::explicit_attrib_location>(version)) {
    // Called only if ARB_explicit_attrib_location is supported
    // *and* version is higher than GL 3.1
}
/* [Context-isExtensionSupported-version] */

/* [Context-MAGNUM_ASSERT_GL_VERSION_SUPPORTED] */
MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL330);
/* [Context-MAGNUM_ASSERT_GL_VERSION_SUPPORTED] */

/* [Context-MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED] */
MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::geometry_shader4);
/* [Context-MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED] */
}
#endif

#if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
{
char data[1]{};
ImageView2D negativeX(GL::PixelFormat::RGBA, GL::PixelType::UnsignedByte, {256, 256}, data);
/* [CubeMapTexture-usage] */
ImageView2D positiveX(GL::PixelFormat::RGBA, GL::PixelType::UnsignedByte, {256, 256}, data);
// ...

GL::CubeMapTexture texture;
texture.setMagnificationFilter(GL::SamplerFilter::Linear)
    // ...
    .setStorage(Math::log2(256)+1, GL::TextureFormat::RGBA8, {256, 256})
    .setSubImage(GL::CubeMapCoordinate::PositiveX, 0, {}, positiveX)
    .setSubImage(GL::CubeMapCoordinate::NegativeX, 0, {}, negativeX)
    // ...
/* [CubeMapTexture-usage] */
    ;
}
#endif

#ifndef MAGNUM_TARGET_GLES
{
GL::CubeMapTexture texture;
/* [CubeMapTexture-image1] */
Image3D image = texture.image(0, {PixelFormat::RGBA8Unorm});
/* [CubeMapTexture-image1] */
}

{
GL::CubeMapTexture texture;
/* [CubeMapTexture-image2] */
GL::BufferImage3D image = texture.image(0,
    {PixelFormat::RGBA8Unorm}, GL::BufferUsage::StaticRead);
/* [CubeMapTexture-image2] */
}

{
GL::CubeMapTexture texture;
/* [CubeMapTexture-compressedImage1] */
CompressedImage3D image = texture.compressedImage(0, {});
/* [CubeMapTexture-compressedImage1] */
}

{
GL::CubeMapTexture texture;
/* [CubeMapTexture-compressedImage2] */
GL::CompressedBufferImage3D image = texture.compressedImage(0, {},
    GL::BufferUsage::StaticRead);
/* [CubeMapTexture-compressedImage2] */
}

{
GL::CubeMapTexture texture;
/* [CubeMapTexture-image3] */
Image2D image = texture.image(GL::CubeMapCoordinate::PositiveX, 0,
    {PixelFormat::RGBA8Unorm});
/* [CubeMapTexture-image3] */
}

{
GL::CubeMapTexture texture;
/* [CubeMapTexture-image4] */
GL::BufferImage2D image = texture.image(GL::CubeMapCoordinate::PositiveX, 0,
    {PixelFormat::RGBA8Unorm}, GL::BufferUsage::StaticRead);
/* [CubeMapTexture-image4] */
}

{
GL::CubeMapTexture texture;
/* [CubeMapTexture-compressedImage3] */
CompressedImage2D image = texture.compressedImage(GL::CubeMapCoordinate::PositiveX,
    0, {});
/* [CubeMapTexture-compressedImage3] */
}

{
GL::CubeMapTexture texture;
/* [CubeMapTexture-compressedImage4] */
GL::CompressedBufferImage2D image = texture.compressedImage(GL::CubeMapCoordinate::PositiveX,
    0, {}, GL::BufferUsage::StaticRead);
/* [CubeMapTexture-compressedImage4] */
}

{
GL::CubeMapTexture texture;
Range3Di range;
/* [CubeMapTexture-subImage1] */
Image3D image = texture.subImage(0, range, {PixelFormat::RGBA8Unorm});
/* [CubeMapTexture-subImage1] */
}

{
GL::CubeMapTexture texture;
Range3Di range;
/* [CubeMapTexture-subImage2] */
GL::BufferImage3D image = texture.subImage(0, range,
    {PixelFormat::RGBA8Unorm}, GL::BufferUsage::StaticRead);
/* [CubeMapTexture-subImage2] */
}

{
GL::CubeMapTexture texture;
Range3Di range;
/* [CubeMapTexture-compressedSubImage1] */
CompressedImage3D image = texture.compressedSubImage(0, range, {});
/* [CubeMapTexture-compressedSubImage1] */
}

{
GL::CubeMapTexture texture;
Range3Di range;
/* [CubeMapTexture-compressedSubImage2] */
GL::CompressedBufferImage3D image = texture.compressedSubImage(0, range, {},
    GL::BufferUsage::StaticRead);
/* [CubeMapTexture-compressedSubImage2] */
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
{
char data[1]{};
ImageView3D imageNegativeX(PixelFormat::RGBA8Unorm, {64, 64, 1}, data);
ImageView3D imagePositiveY(PixelFormat::RGBA8Unorm, {64, 64, 1}, data);
/* [CubeMapTextureArray-usage] */
GL::CubeMapTextureArray texture;
texture.setMagnificationFilter(GL::SamplerFilter::Linear)
    // ...
    .setStorage(Math::log2(64)+1, GL::TextureFormat::RGBA8, {64, 64, 24});

for(std::size_t i = 0; i != 4; i += 6) {
    ImageView3D imagePositiveX(PixelFormat::RGBA8Unorm, {64, 64, 1}, data);
    // ...
    texture.setSubImage(0, Vector3i::zAxis(i+0), imagePositiveX);
    texture.setSubImage(0, Vector3i::zAxis(i+1), imageNegativeX);
    texture.setSubImage(0, Vector3i::zAxis(i+2), imagePositiveY);
    // ...
}

texture.generateMipmap();
/* [CubeMapTextureArray-usage] */
}

#ifndef MAGNUM_TARGET_GLES
{
GL::CubeMapTextureArray texture;
/* [CubeMapTextureArray-image1] */
Image3D image = texture.image(0, {PixelFormat::RGBA8Unorm});
/* [CubeMapTextureArray-image1] */
}

{
GL::CubeMapTextureArray texture;
/* [CubeMapTextureArray-image2] */
GL::BufferImage3D image = texture.image(0,
    {PixelFormat::RGBA8Unorm}, GL::BufferUsage::StaticRead);
/* [CubeMapTextureArray-image2] */
}

{
GL::CubeMapTextureArray texture;
/* [CubeMapTextureArray-compressedImage1] */
CompressedImage3D image = texture.compressedImage(0, {});
/* [CubeMapTextureArray-compressedImage1] */
}

{
GL::CubeMapTextureArray texture;
/* [CubeMapTextureArray-compressedImage2] */
GL::CompressedBufferImage3D image = texture.compressedImage(0, {},
    GL::BufferUsage::StaticRead);
/* [CubeMapTextureArray-compressedImage2] */
}

{
GL::CubeMapTextureArray texture;
Range3Di range;
/* [CubeMapTextureArray-subImage1] */
Image3D image = texture.subImage(0, range, {PixelFormat::RGBA8Unorm});
/* [CubeMapTextureArray-subImage1] */
}

{
GL::CubeMapTextureArray texture;
Range3Di range;
/* [CubeMapTextureArray-subImage2] */
GL::BufferImage3D image = texture.subImage(0, range,
    {PixelFormat::RGBA8Unorm}, GL::BufferUsage::StaticRead);
/* [CubeMapTextureArray-subImage2] */
}

{
GL::CubeMapTextureArray texture;
Range3Di range;
/* [CubeMapTextureArray-compressedSubImage1] */
CompressedImage3D image = texture.compressedSubImage(0, range, {});
/* [CubeMapTextureArray-compressedSubImage1] */
}

{
GL::CubeMapTextureArray texture;
Range3Di range;
/* [CubeMapTextureArray-compressedSubImage2] */
GL::CompressedBufferImage3D image = texture.compressedSubImage(0, range, {},
    GL::BufferUsage::StaticRead);
/* [CubeMapTextureArray-compressedSubImage2] */
}
#endif
#endif

#ifndef MAGNUM_TARGET_WEBGL
{
GL::Mesh mesh;
struct: GL::AbstractShaderProgram {} shader;
/* [DebugOutput-usage] */
GL::Renderer::enable(GL::Renderer::Feature::DebugOutput);
GL::Renderer::enable(GL::Renderer::Feature::DebugOutputSynchronous);
GL::DebugOutput::setDefaultCallback();

/* Disable rather spammy "Buffer detailed info" debug messages on NVidia drivers */
GL::DebugOutput::setEnabled(
    GL::DebugOutput::Source::Api, GL::DebugOutput::Type::Other, {131185}, false);

{
    GL::DebugGroup group{GL::DebugGroup::Source::Application, 42,
        "Scene rendering"};

    GL::DebugMessage::insert(GL::DebugMessage::Source::Application,
        GL::DebugMessage::Type::Marker, 1337,
        GL::DebugOutput::Severity::Notification, "Rendering a transparent mesh");

    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    shader.draw(mesh);
    GL::Renderer::disable(GL::Renderer::Feature::Blending);

    // ...
}
/* [DebugOutput-usage] */
}

{
/* [DebugOutput-setDefaultCallback] */
GL::DebugMessage::insert(GL::DebugMessage::Source::Application,
    GL::DebugMessage::Type::Marker, 1337, GL::DebugOutput::Severity::Notification,
    "Hello from OpenGL command stream!");
/* [DebugOutput-setDefaultCallback] */
}

{
/* [DebugMessage-usage] */
GL::DebugMessage::insert(GL::DebugMessage::Source::Application,
    GL::DebugMessage::Type::Marker, 1337, GL::DebugOutput::Severity::Notification,
    "Hello from OpenGL command stream!");
/* [DebugMessage-usage] */
}

{
GL::Mesh mesh;
struct: GL::AbstractShaderProgram {} shader;
/* [DebugGroup-usage1] */
{
    /* Push debug group */
    GL::DebugGroup group{GL::DebugGroup::Source::Application, 42, "Scene rendering"};

    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    shader.draw(mesh);
    GL::Renderer::disable(GL::Renderer::Feature::Blending);

    /* The debug group is popped automatically at the end of the scope */
}
/* [DebugGroup-usage1] */
}

{
GL::Mesh mesh;
struct: GL::AbstractShaderProgram {} shader;
/* [DebugGroup-usage2] */
GL::DebugGroup group;

group.push(GL::DebugGroup::Source::Application, 42, "Scene rendering");

GL::Renderer::enable(GL::Renderer::Feature::Blending);
shader.draw(mesh);
GL::Renderer::disable(GL::Renderer::Feature::Blending);

group.pop();
/* [DebugGroup-usage2] */
}
#endif

{
struct MyShader {
    enum: UnsignedInt {
        ColorOutput = 0,
        NormalOutput = 1
    };
};
/* [DefaultFramebuffer-usage-map] */
GL::defaultFramebuffer.mapForDraw({
    {MyShader::ColorOutput, GL::DefaultFramebuffer::DrawAttachment::Back},
    {MyShader::NormalOutput, GL::DefaultFramebuffer::DrawAttachment::None}});
/* [DefaultFramebuffer-usage-map] */
}

#ifndef MAGNUM_TARGET_GLES2
{
struct MyShader {
    void bindTexture(GL::Texture2D&) {}
} myShader;
Vector2i size;
/* [Framebuffer-usage] */
GL::Texture2D color;
GL::Renderbuffer depthStencil;
color.setStorage(1, GL::TextureFormat::RGBA8, size);
depthStencil.setStorage(GL::RenderbufferFormat::Depth24Stencil8, size);

GL::Framebuffer framebuffer{{{}, size}};
framebuffer.attachTexture(GL::Framebuffer::ColorAttachment{0}, color, 0);
framebuffer.attachRenderbuffer(
    GL::Framebuffer::BufferAttachment::DepthStencil, depthStencil);
/* [Framebuffer-usage] */

/* [Framebuffer-usage-rendering] */
framebuffer
    .clear(GL::FramebufferClear::Color|GL::FramebufferClear::Depth)
    .bind();

// draw to this framebuffer ...

/* Switch back to the default framebuffer */
GL::defaultFramebuffer
    .clear(GL::FramebufferClear::Color|GL::FramebufferClear::Depth)
    .bind();

// use the rendered texture in a shader ...
myShader.bindTexture(color);
/* [Framebuffer-usage-rendering] */
}
#endif

#ifndef MAGNUM_TARGET_GLES2
{
/* [Framebuffer-usage-multisample] */
Vector2i size = GL::defaultFramebuffer.viewport().size();

/* 8x MSAA */
GL::Renderbuffer color, depthStencil;
color.setStorageMultisample(8, GL::RenderbufferFormat::RGBA8, size);
depthStencil.setStorageMultisample(8,
    GL::RenderbufferFormat::Depth24Stencil8, size);

GL::Framebuffer framebuffer{{{}, size}};
framebuffer.attachRenderbuffer(GL::Framebuffer::ColorAttachment{0}, color);
framebuffer.attachRenderbuffer(
    GL::Framebuffer::BufferAttachment::DepthStencil, depthStencil);

framebuffer.clear(GL::FramebufferClear::Color|GL::FramebufferClear::Depth)
    .bind();

// draw to the multisampled framebuffer ...

/* Resolve the color output to a single-sampled default framebuffer */
GL::defaultFramebuffer.clear(GL::FramebufferClear::Color)
    .bind();
GL::Framebuffer::blit(framebuffer, GL::defaultFramebuffer,
    {{}, size}, GL::FramebufferBlit::Color);
/* [Framebuffer-usage-multisample] */
}
#endif

#ifndef MAGNUM_TARGET_GLES2
{
struct MyShader {
    enum: UnsignedInt {
        ColorOutput = 0,
        NormalOutput = 1
    };
};
/* [Framebuffer-usage-deferred] */
GL::Framebuffer framebuffer{GL::defaultFramebuffer.viewport()};
GL::Texture2D color, normal;
GL::Renderbuffer depthStencil;
// setStorage() ...

framebuffer.attachTexture(GL::Framebuffer::ColorAttachment{0}, color, 0);
framebuffer.attachTexture(GL::Framebuffer::ColorAttachment{1}, normal, 0);
framebuffer.attachRenderbuffer(
    GL::Framebuffer::BufferAttachment::DepthStencil, depthStencil);
framebuffer.mapForDraw({
    {MyShader::ColorOutput, GL::Framebuffer::ColorAttachment(0)},
    {MyShader::NormalOutput, GL::Framebuffer::ColorAttachment(1)}});
/* [Framebuffer-usage-deferred] */

/* [Framebuffer-mapForDraw] */
framebuffer.mapForDraw({
    {MyShader::ColorOutput, GL::Framebuffer::ColorAttachment(0)},
    {MyShader::NormalOutput, GL::Framebuffer::DrawAttachment::None}});
/* [Framebuffer-mapForDraw] */
}
#endif

{
/* [Mesh-nonindexed] */
/* Custom shader, needing only position data */
class MyShader: public GL::AbstractShaderProgram {
    public:
        typedef GL::Attribute<0, Vector3> Position;

    // ...
};

/* Fill vertex buffer with position data */
Vector3 positions[30]{
    // ...
};
GL::Buffer vertexBuffer;
vertexBuffer.setData(positions, GL::BufferUsage::StaticDraw);

/* Configure the mesh, add vertex buffer */
GL::Mesh mesh;
mesh.setPrimitive(MeshPrimitive::Triangles)
    .addVertexBuffer(vertexBuffer, 0, MyShader::Position{})
    .setCount(30);
/* [Mesh-nonindexed] */
}

{
/* [Mesh-interleaved] */
/* Non-indexed primitive with positions and normals */
Trade::MeshData plane = Primitives::planeSolid();

/* Fill a vertex buffer with interleaved position and normal data */
GL::Buffer buffer;
buffer.setData(MeshTools::interleave(plane.positions3DAsArray(),
                                     plane.normalsAsArray()));

/* Configure the mesh, add the vertex buffer */
GL::Mesh mesh;
mesh.setPrimitive(plane.primitive())
    .setCount(plane.vertexCount())
    .addVertexBuffer(buffer, 0, Shaders::Phong::Position{}, Shaders::Phong::Normal{});
/* [Mesh-interleaved] */
}

{
/* [Mesh-indexed] */
/* Custom shader */
class MyShader: public GL::AbstractShaderProgram {
    public:
        typedef GL::Attribute<0, Vector3> Position;

    // ...
};

/* Fill vertex buffer with position data */
Vector3 positions[240]{
    // ...
};
GL::Buffer vertexBuffer;
vertexBuffer.setData(positions);

/* Fill index buffer with index data */
UnsignedByte indices[75]{
    // ...
};
GL::Buffer indexBuffer;
indexBuffer.setData(indices);

/* Configure the mesh, add both vertex and index buffer */
GL::Mesh mesh;
mesh.setPrimitive(MeshPrimitive::Triangles)
    .setCount(75)
    .addVertexBuffer(vertexBuffer, 0, MyShader::Position{})
    .setIndexBuffer(indexBuffer, 0, GL::MeshIndexType::UnsignedByte, 176, 229);
/* [Mesh-indexed] */
}

{
/* [Mesh-indexed-tools] */
// Indexed primitive
Trade::MeshData cube = Primitives::cubeSolid();

// Fill vertex buffer with interleaved position and normal data
GL::Buffer vertexBuffer;
vertexBuffer.setData(MeshTools::interleave(cube.positions3DAsArray(),
                                           cube.normalsAsArray()));

// Compress index data
Containers::Array<char> indexData;
MeshIndexType indexType;
std::tie(indexData, indexType) = MeshTools::compressIndices(cube.indices());

// Fill index buffer
GL::Buffer indexBuffer;
indexBuffer.setData(indexData);

// Configure the mesh, add both vertex and index buffer
GL::Mesh mesh;
mesh.setPrimitive(cube.primitive())
    .setCount(cube.indexCount())
    .addVertexBuffer(vertexBuffer, 0, Shaders::Phong::Position{}, Shaders::Phong::Normal{})
    .setIndexBuffer(indexBuffer, 0, indexType);
/* [Mesh-indexed-tools] */
}

#ifndef MAGNUM_TARGET_GLES
{
/* [Mesh-formats] */
// Custom shader with colors specified as four floating-point values
class MyShader: public GL::AbstractShaderProgram {
    public:
        typedef GL::Attribute<0, Vector3> Position;
        typedef GL::Attribute<1, Color4> Color;

    // ...
};

/* Initial mesh configuration */
GL::Mesh mesh;
mesh.setPrimitive(MeshPrimitive::Triangles)
    .setCount(30);

/* Fill position buffer with positions specified as two-component XY (i.e.,
   no Z component, which is meant to be always 0) */
Vector2 positions[30]{
    // ...
};
GL::Buffer positionBuffer;
positionBuffer.setData(positions, GL::BufferUsage::StaticDraw);

/* Specify layout of positions buffer -- only two components, unspecified Z
   component will be automatically set to 0 */
mesh.addVertexBuffer(positionBuffer, 0,
    MyShader::Position{MyShader::Position::Components::Two});

/* Fill color buffer with colors specified as four-byte BGRA (i.e., directly
   from a TGA file) */
UnsignedByte colors[4*30]{
    // ...
};
GL::Buffer colorBuffer;
colorBuffer.setData(colors, GL::BufferUsage::StaticDraw);

/* Specify color buffer layout -- BGRA, each component unsigned byte and we
   want to normalize them from [0, 255] to [0.0f, 1.0f] */
mesh.addVertexBuffer(colorBuffer, 0, MyShader::Color{
    MyShader::Color::Components::BGRA,
    MyShader::Color::DataType::UnsignedByte,
    MyShader::Color::DataOption::Normalized});
/* [Mesh-formats] */
}
#endif

{
GL::Mesh mesh;
GL::Buffer colorBuffer;
/* [Mesh-dynamic] */
mesh.addVertexBuffer(colorBuffer, 0, 4, GL::DynamicAttribute{
    GL::DynamicAttribute::Kind::GenericNormalized, 3,
    GL::DynamicAttribute::Components::Three,
    GL::DynamicAttribute::DataType::UnsignedByte});
/* [Mesh-dynamic] */
}

{
GL::Mesh mesh;
/* [Mesh-buffer-ownership] */
GL::Buffer vertices, indices;
// ...
mesh.addVertexBuffer(std::move(vertices), 0,
        Shaders::Phong::Position{},
        Shaders::Phong::Normal{})
    .setIndexBuffer(std::move(indices), 0, MeshIndexType::UnsignedInt);
/* [Mesh-buffer-ownership] */

/* [Mesh-buffer-ownership-multiple] */
mesh.addVertexBuffer(vertices, 0, Shaders::Phong::Position{}, 20)
    .addVertexBuffer(std::move(vertices), 0, 20, Shaders::Phong::Normal{});
/* [Mesh-buffer-ownership-multiple] */
}

{
/* [Mesh-addVertexBuffer1] */
GL::Buffer buffer;
GL::Mesh mesh;
mesh.addVertexBuffer(buffer, 76,    /* initial array offset */
    4,                              /* skip vertex weight (Float) */
    Shaders::Phong::Position(),     /* vertex position */
    8,                              /* skip texture coordinates (Vector2) */
    Shaders::Phong::Normal());      /* vertex normal */
/* [Mesh-addVertexBuffer1] */

/* [Mesh-addVertexBuffer2] */
mesh.addVertexBuffer(buffer, 76, 4, Shaders::Phong::Position{}, 20)
    .addVertexBuffer(buffer, 76, 24, Shaders::Phong::Normal{}, 0);
/* [Mesh-addVertexBuffer2] */

/* [Mesh-addVertexBuffer3] */
Int vertexCount = 352;
mesh.addVertexBuffer(buffer, 76 + 4*vertexCount, Shaders::Phong::Position{})
    .addVertexBuffer(buffer, 76 + 24*vertexCount, Shaders::Phong::Normal{});
/* [Mesh-addVertexBuffer3] */
}

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
{
/* [MultisampleTexture-usage] */
GL::MultisampleTexture2D texture;
texture.setStorage(16, GL::TextureFormat::RGBA8, {1024, 1024});
/* [MultisampleTexture-usage] */
}
#endif

struct A: TestSuite::Tester {
void foo() {
/* [OpenGLTester-MAGNUM_VERIFY_NO_GL_ERROR] */
CORRADE_COMPARE(Magnum::GL::Renderer::error(), Magnum::GL::Renderer::Error::NoError);
/* [OpenGLTester-MAGNUM_VERIFY_NO_GL_ERROR] */
}
};

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
{
/* [PrimitiveQuery-usage] */
GL::PrimitiveQuery q{GL::PrimitiveQuery::Target::PrimitivesGenerated};

q.begin();
// rendering...
q.end();

if(!q.resultAvailable()) {
    // do some work until to give OpenGL some time...
}

// ...or block until the result is available
UnsignedInt primitiveCount = q.result<UnsignedInt>();
/* [PrimitiveQuery-usage] */
static_cast<void>(primitiveCount);
}
#endif

#ifndef MAGNUM_TARGET_GLES
{
char data[1]{};
/* [RectangleTexture-usage] */
ImageView2D image{PixelFormat::RGBA8Unorm, {526, 137}, data};

GL::RectangleTexture texture;
texture.setMagnificationFilter(GL::SamplerFilter::Linear)
    .setMinificationFilter(GL::SamplerFilter::Linear)
    .setWrapping(GL::SamplerWrapping::ClampToEdge)
    .setMaxAnisotropy(GL::Sampler::maxMaxAnisotropy())
    .setStorage(GL::TextureFormat::RGBA8, {526, 137})
    .setSubImage({}, image);
/* [RectangleTexture-usage] */
}

{
GL::RectangleTexture texture;
/* [RectangleTexture-image1] */
Image2D image = texture.image({PixelFormat::RGBA8Unorm});
/* [RectangleTexture-image1] */
}

{
GL::RectangleTexture texture;
/* [RectangleTexture-image2] */
GL::BufferImage2D image = texture.image(
    {PixelFormat::RGBA8Unorm}, GL::BufferUsage::StaticRead);
/* [RectangleTexture-image2] */
}

{
GL::RectangleTexture texture;
/* [RectangleTexture-compressedImage1] */
CompressedImage2D image = texture.compressedImage({});
/* [RectangleTexture-compressedImage1] */
}

{
GL::RectangleTexture texture;
/* [RectangleTexture-compressedImage2] */
GL::CompressedBufferImage2D image = texture.compressedImage({},
    GL::BufferUsage::StaticRead);
/* [RectangleTexture-compressedImage2] */
}

{
GL::RectangleTexture texture;
Range2Di range;
/* [RectangleTexture-subImage1] */
Image2D image = texture.subImage(range, {PixelFormat::RGBA8Unorm});
/* [RectangleTexture-subImage1] */
}

{
GL::RectangleTexture texture;
Range2Di range;
/* [RectangleTexture-subImage2] */
GL::BufferImage2D image = texture.subImage(range,
    {PixelFormat::RGBA8Unorm}, GL::BufferUsage::StaticRead);
/* [RectangleTexture-subImage2] */
}

{
GL::RectangleTexture texture;
Range2Di range;
/* [RectangleTexture-compressedSubImage1] */
CompressedImage2D image = texture.compressedSubImage(range, {});
/* [RectangleTexture-compressedSubImage1] */
}

{
GL::RectangleTexture texture;
Range2Di range;
/* [RectangleTexture-compressedSubImage2] */
GL::CompressedBufferImage2D image = texture.compressedSubImage(range, {},
    GL::BufferUsage::StaticRead);
/* [RectangleTexture-compressedSubImage2] */
}
#endif

{
GL::Renderer::Feature feature = GL::Renderer::Feature::Blending;
bool enabled{};
/* [Renderer-setFeature] */
enabled ? GL::Renderer::enable(feature) : GL::Renderer::disable(feature)
/* [Renderer-setFeature] */
;
}

{
/* [Renderer-setBlendFunction] */
GL::Renderer::enable(GL::Renderer::Feature::Blending);
GL::Renderer::setBlendFunction(
    GL::Renderer::BlendFunction::One, /* or SourceAlpha for non-premultiplied */
    GL::Renderer::BlendFunction::OneMinusSourceAlpha);
/* [Renderer-setBlendFunction] */
}

#if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
{
/* [SampleQuery-usage] */
GL::SampleQuery q{GL::SampleQuery::Target::AnySamplesPassed};

q.begin();
/* Render simplified object to test whether it is visible at all */
// ...
q.end();

/* Render full version of the object only if it is visible */
if(q.result<bool>()) {
    // ...
}
/* [SampleQuery-usage] */
}

#ifndef MAGNUM_TARGET_GLES
{
/* [SampleQuery-conditional-render] */
GL::SampleQuery q{GL::SampleQuery::Target::AnySamplesPassed};

q.begin();
/* Render simplified object to test whether it is visible at all */
// ...
q.end();

q.beginConditionalRender(GL::SampleQuery::ConditionalRenderMode::Wait);
/* Render full version of the object only if the query returns nonzero result */
// ...
q.endConditionalRender();
/* [SampleQuery-conditional-render] */
}
#endif
#endif

#if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
{
char data[1]{};
/* [Texture-usage] */
ImageView2D image(PixelFormat::RGBA8Unorm, {4096, 4096}, data);

GL::Texture2D texture;
texture.setMagnificationFilter(GL::SamplerFilter::Linear)
    .setMinificationFilter(GL::SamplerFilter::Linear, GL::SamplerMipmap::Linear)
    .setWrapping(GL::SamplerWrapping::ClampToEdge)
    .setMaxAnisotropy(GL::Sampler::maxMaxAnisotropy())
    .setStorage(Math::log2(4096)+1, GL::TextureFormat::RGBA8, {4096, 4096})
    .setSubImage(0, {}, image)
    .generateMipmap();
/* [Texture-usage] */
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
{
GL::Texture2D texture;
/* [Texture-setSwizzle] */
texture.setSwizzle<'b', 'g', 'r', '0'>();
/* [Texture-setSwizzle] */
}
#endif

#ifndef MAGNUM_TARGET_GLES
{
GL::Texture2D texture;
/* [Texture-image1] */
Image2D image = texture.image(0, {PixelFormat::RGBA8Unorm});
/* [Texture-image1] */
}

{
GL::Texture2D texture;
/* [Texture-image2] */
GL::BufferImage2D image = texture.image(0,
    {PixelFormat::RGBA8Unorm}, GL::BufferUsage::StaticRead);
/* [Texture-image2] */
}

{
GL::Texture2D texture;
/* [Texture-compressedImage1] */
CompressedImage2D image = texture.compressedImage(0, {});
/* [Texture-compressedImage1] */
}

{
GL::Texture2D texture;
/* [Texture-compressedImage2] */
GL::CompressedBufferImage2D image = texture.compressedImage(0, {},
    GL::BufferUsage::StaticRead);
/* [Texture-compressedImage2] */
}

{
GL::Texture2D texture;
Range2Di range;
/* [Texture-subImage1] */
Image2D image = texture.subImage(0, range, {PixelFormat::RGBA8Unorm});
/* [Texture-subImage1] */
}

{
GL::Texture2D texture;
Range2Di range;
/* [Texture-subImage2] */
GL::BufferImage2D image = texture.subImage(0, range,
    {PixelFormat::RGBA8Unorm}, GL::BufferUsage::StaticRead);
/* [Texture-subImage2] */
}

{
GL::Texture2D texture;
Range2Di range;
/* [Texture-compressedSubImage1] */
CompressedImage2D image = texture.compressedSubImage(0, range, {});
/* [Texture-compressedSubImage1] */
}

{
GL::Texture2D texture;
Range2Di range;
/* [Texture-compressedSubImage2] */
GL::CompressedBufferImage2D image = texture.compressedSubImage(0, range, {},
    GL::BufferUsage::StaticRead);
/* [Texture-compressedSubImage2] */
}
#endif

#ifndef MAGNUM_TARGET_GLES2
{
/* [TextureArray-usage1] */
GL::Texture2DArray texture;
texture.setMagnificationFilter(GL::SamplerFilter::Linear)
    .setMinificationFilter(GL::SamplerFilter::Linear, GL::SamplerMipmap::Linear)
    .setWrapping(GL::SamplerWrapping::ClampToEdge)
    .setMaxAnisotropy(GL::Sampler::maxMaxAnisotropy());;
/* [TextureArray-usage1] */

Int levels{};
char data[1]{};
/* [TextureArray-usage2] */
texture.setStorage(levels, GL::TextureFormat::RGBA8, {64, 64, 16});

for(std::size_t i = 0; i != 16; ++i) {
    ImageView3D image(PixelFormat::RGBA8Unorm, {64, 64, 1}, data);
    texture.setSubImage(0, Vector3i::zAxis(i), image);
}
/* [TextureArray-usage2] */
}

#ifndef MAGNUM_TARGET_GLES
{
GL::Texture2DArray texture;
/* [TextureArray-image1] */
Image3D image = texture.image(0, {PixelFormat::RGBA8Unorm});
/* [TextureArray-image1] */
}

{
GL::Texture2DArray texture;
/* [TextureArray-image2] */
GL::BufferImage3D image = texture.image(0,
    {PixelFormat::RGBA8Unorm}, GL::BufferUsage::StaticRead);
/* [TextureArray-image2] */
}

{
GL::Texture2DArray texture;
/* [TextureArray-compressedImage1] */
CompressedImage3D image = texture.compressedImage(0, {});
/* [TextureArray-compressedImage1] */
}

{
GL::Texture2DArray texture;
/* [TextureArray-compressedImage2] */
GL::CompressedBufferImage3D image = texture.compressedImage(0, {},
    GL::BufferUsage::StaticRead);
/* [TextureArray-compressedImage2] */
}

{
GL::Texture2DArray texture;
Range3Di range;
/* [TextureArray-subImage1] */
Image3D image = texture.subImage(0, range, {PixelFormat::RGBA8Unorm});
/* [TextureArray-subImage1] */
}

{
GL::Texture2DArray texture;
Range3Di range;
/* [TextureArray-subImage2] */
GL::BufferImage3D image = texture.subImage(0, range,
    {PixelFormat::RGBA8Unorm}, GL::BufferUsage::StaticRead);
/* [TextureArray-subImage2] */
}

{
GL::Texture2DArray texture;
Range3Di range;
/* [TextureArray-compressedSubImage1] */
CompressedImage3D image = texture.compressedSubImage(0, range, {});
/* [TextureArray-compressedSubImage1] */
}

{
GL::Texture2DArray texture;
Range3Di range;
/* [TextureArray-compressedSubImage2] */
GL::CompressedBufferImage3D image = texture.compressedSubImage(0, range, {},
    GL::BufferUsage::StaticRead);
/* [TextureArray-compressedSubImage2] */
}
#endif
#endif

#ifndef MAGNUM_TARGET_WEBGL
{
/* [TimeQuery-usage1] */
GL::TimeQuery q1{GL::TimeQuery::Target::TimeElapsed},
              q2{GL::TimeQuery::Target::TimeElapsed};

q1.begin();
// rendering...
q1.end();

q2.begin();
// another rendering...
q2.end();

UnsignedInt timeElapsed1 = q1.result<UnsignedInt>();
UnsignedInt timeElapsed2 = q2.result<UnsignedInt>();
/* [TimeQuery-usage1] */
static_cast<void>(timeElapsed1);
static_cast<void>(timeElapsed2);
}

{
/* [TimeQuery-usage2] */
GL::TimeQuery q1{GL::TimeQuery::Target::Timestamp},
              q2{GL::TimeQuery::Target::Timestamp},
              q3{GL::TimeQuery::Target::Timestamp};

q1.timestamp();
// rendering...
q2.timestamp();
// another rendering...
q3.timestamp();

UnsignedInt tmp = q2.result<UnsignedInt>();
UnsignedInt timeElapsed1 = tmp - q1.result<UnsignedInt>();
UnsignedInt timeElapsed2 = q3.result<UnsignedInt>() - tmp;
/* [TimeQuery-usage2] */
static_cast<void>(timeElapsed1);
static_cast<void>(timeElapsed2);
}
#endif

}
