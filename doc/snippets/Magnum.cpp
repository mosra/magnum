/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

#include "Magnum/AbstractShaderProgram.h"
#include "Magnum/Buffer.h"
#include "Magnum/Context.h"
#include "Magnum/CubeMapTexture.h"
#include "Magnum/DefaultFramebuffer.h"
#include "Magnum/Extensions.h"
#include "Magnum/Framebuffer.h"
#include "Magnum/Image.h"
#include "Magnum/Mesh.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Renderer.h"
#include "Magnum/Renderbuffer.h"
#include "Magnum/Shader.h"
#include "Magnum/Texture.h"
#include "Magnum/TextureFormat.h"
#include "Magnum/Version.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/MeshTools/Interleave.h"
#include "Magnum/MeshTools/CompressIndices.h"
#include "Magnum/Primitives/Cube.h"
#include "Magnum/Primitives/Plane.h"
#include "Magnum/Shaders/Phong.h"
#include "Magnum/Trade/MeshData3D.h"

#if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
#include "Magnum/SampleQuery.h"
#endif

#ifndef MAGNUM_TARGET_WEBGL
#include "Magnum/DebugOutput.h"
#ifndef CORRADE_TARGET_ANDROID
#include "Magnum/OpenGLTester.h"
#endif
#include "Magnum/TimeQuery.h"
#endif

#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/BufferImage.h"
#include "Magnum/PrimitiveQuery.h"
#include "Magnum/TextureArray.h"
#include "Magnum/TransformFeedback.h"
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
#include "Magnum/BufferTexture.h"
#include "Magnum/BufferTextureFormat.h"
#include "Magnum/CubeMapTextureArray.h"
#include "Magnum/MultisampleTexture.h"
#endif

#ifndef MAGNUM_TARGET_GLES
#include "Magnum/RectangleTexture.h"
#endif

using namespace Magnum;
using namespace Magnum::Math::Literals;

int main() {

{
auto importSomeMesh() -> std::tuple<Mesh, Buffer, Buffer>;
/* [opengl-wrapping-nocreate] */
Mesh mesh{NoCreate};
Buffer vertices{NoCreate}, indices{NoCreate};
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
    Buffer buffer;
    buffer.setData(someData, BufferUsage::StaticDraw);
    GLuint id = buffer.release();
    externalLib.setSomeBuffer(id); /* The library is responsible for deletion */
}

/* Acquiring an instance from external library */
{
    GLuint id = externalLib.someBuffer();
    Buffer buffer = Buffer::wrap(id, ObjectFlag::DeleteOnDestruction);
    /* The buffer instance now handles deletion */
}
/* [opengl-wrapping-transfer] */
}

#ifndef MAGNUM_TARGET_GLES
{
struct: AbstractShaderProgram {} someShader;
/* [opengl-wrapping-state] */
Buffer buffer;
Mesh mesh;
// ...
mesh.draw(someShader);

{
    /* Entering a section with 3rd-party OpenGL code -- clean up all state that
       could cause accidental modifications of our objects from outside */
    Context::current().resetState(Context::State::EnterExternal);

    /* Raw OpenGL calls */
    glBindBuffer(GL_ARRAY_BUFFER, buffer.id());
    glBufferStorage(GL_ARRAY_BUFFER, 32768, nullptr, GL_MAP_READ_BIT|GL_MAP_WRITE_BIT);
    // ...

    /* Exiting a section with 3rd-party OpenGL code -- reset our state tracker */
    Context::current().resetState(Context::State::ExitExternal);
}

/* Use the buffer through Magnum again */
auto data = buffer.map(0, 32768, Buffer::MapFlag::Read|Buffer::MapFlag::Write);
// ...
/* [opengl-wrapping-state] */
static_cast<void>(data);
}
#endif

#ifndef MAGNUM_TARGET_GLES
{
/* [opengl-wrapping-extensions] */
TextureFormat format;
if(Context::current().isExtensionSupported<Extensions::GL::ARB::depth_buffer_float>())
    format = TextureFormat::DepthComponent32F;
else
    format = TextureFormat::DepthComponent24;
/* [opengl-wrapping-extensions] */
static_cast<void>(format);
}
#endif

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
{
/* [opengl-wrapping-dsa] */
Texture2D texture;

/* - on OpenGL 4.5+/ARB_direct_state_access this calls glTextureStorage2D()
   - if EXT_direct_state_access is available, calls glTextureStorage2DEXT()
   - on OpenGL 4.2+/ARB_texture_storage and OpenGL ES 3.0+ calls glTexStorage2D()
   - on OpenGL ES 2.0 with EXT_texture_storage calls glTexStorage2DEXT()
   - otherwise emulated using a sequence of four glTexImage2D() calls */
texture.setStorage(4, TextureFormat::RGBA8, {256, 256});
/* [opengl-wrapping-dsa] */
}
#endif

#ifndef MAGNUM_TARGET_GLES
struct MyShader: AbstractShaderProgram {
/* [AbstractShaderProgram-input-attributes] */
typedef Attribute<0, Vector3> Position;
typedef Attribute<1, Vector3> Normal;
typedef Attribute<2, Vector2> TextureCoordinates;
/* [AbstractShaderProgram-input-attributes] */

/* [AbstractShaderProgram-output-attributes] */
enum: UnsignedInt {
    ColorOutput = 0,
    NormalOutput = 1
};
/* [AbstractShaderProgram-output-attributes] */

/* [AbstractShaderProgram-constructor] */
explicit MyShader() {
    /* Load shader sources */
    Shader vert{Version::GL430, Shader::Type::Vertex};
    Shader frag{Version::GL430, Shader::Type::Fragment};
    vert.addFile("MyShader.vert");
    frag.addFile("MyShader.frag");

    /* Invoke parallel compilation for best performance */
    CORRADE_INTERNAL_ASSERT_OUTPUT(Shader::compile({vert, frag}));

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
MyShader& bindDiffuseTexture(Texture2D& texture) {
    texture.bind(0);
    return *this;
}
MyShader& bindSpecularTexture(Texture2D& texture) {
    texture.bind(1);
    return *this;
}
/* [AbstractShaderProgram-textures] */

/* [AbstractShaderProgram-xfb] */
MyShader& setTransformFeedback(TransformFeedback& feedback, Buffer& positions, Buffer& data) {
    feedback.attachBuffers(0, {&positions, &data});
    return *this;
}
MyShader& setTransformFeedback(TransformFeedback& feedback, Int totalCount, Buffer& positions, GLintptr positionsOffset, Buffer& data, GLintptr dataOffset) {
    feedback.attachBuffers(0, {
        std::make_tuple(&positions, positionsOffset, totalCount*sizeof(Vector3)),
        std::make_tuple(&data, dataOffset, totalCount*sizeof(Vector2ui))
    });
    return *this;
}
/* [AbstractShaderProgram-xfb] */

void foo() {
/* [AbstractShaderProgram-binding] */
// Shaders attached...

bindAttributeLocation(Position::Location, "position");
bindAttributeLocation(Normal::Location, "normal");
bindAttributeLocation(TextureCoordinates::Location, "textureCoordinates");

bindFragmentDataLocationIndexed(ColorOutput, 0, "color");
bindFragmentDataLocationIndexed(NormalOutput, 1, "normal");

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
Mesh mesh;
Matrix4 transformation, projection;
Texture2D diffuseTexture, specularTexture;
/* [AbstractShaderProgram-rendering] */
shader.setTransformationMatrix(transformation)
    .setProjectionMatrix(projection)
    .bindDiffuseTexture(diffuseTexture)
    .bindSpecularTexture(specularTexture);

mesh.draw(shader);
/* [AbstractShaderProgram-rendering] */
}
#endif

{
Framebuffer framebuffer{{}};
/* [AbstractFramebuffer-read1] */
Image2D image = framebuffer.read(framebuffer.viewport(),
    {PixelFormat::RGBA, PixelType::UnsignedByte});
/* [AbstractFramebuffer-read1] */
}

#ifndef MAGNUM_TARGET_GLES2
{
Framebuffer framebuffer{{}};
/* [AbstractFramebuffer-read2] */
BufferImage2D image = framebuffer.read(framebuffer.viewport(),
    {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
/* [AbstractFramebuffer-read2] */
}
#endif

{
Buffer buffer;
/* [Buffer-setdata] */
Containers::ArrayView<Vector3> data;
buffer.setData(data, BufferUsage::StaticDraw);
/* [Buffer-setdata] */
}

{
Buffer buffer;
/* [Buffer-setdata-stl] */
std::vector<Vector3> data;
buffer.setData(data, BufferUsage::StaticDraw);
/* [Buffer-setdata-stl] */

/* [Buffer-setdata-allocate] */
buffer.setData({nullptr, 200*sizeof(Vector3)}, BufferUsage::StaticDraw);
/* [Buffer-setdata-allocate] */
}

#ifndef MAGNUM_TARGET_WEBGL
{
Buffer buffer;
/* [Buffer-map] */
Containers::ArrayView<Vector3> data = Containers::arrayCast<Vector3>(buffer.map(0,
    200*sizeof(Vector3), Buffer::MapFlag::Write|Buffer::MapFlag::InvalidateBuffer));
CORRADE_INTERNAL_ASSERT(data);
for(Vector3& d: data)
    d = {/*...*/};
CORRADE_INTERNAL_ASSERT_OUTPUT(buffer.unmap());
/* [Buffer-map] */
}

{
Buffer buffer;
/* [Buffer-flush] */
Containers::ArrayView<Vector3> data = Containers::arrayCast<Vector3>(buffer.map(0,
    200*sizeof(Vector3), Buffer::MapFlag::Write|Buffer::MapFlag::FlushExplicit));
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
/* [Buffer-webgl] */
Buffer vertices{Buffer::TargetHint::Array};
Buffer indices{Buffer::TargetHint::ElementArray};
/* [Buffer-webgl] */
}

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
{
/* [BufferTexture-usage] */
Buffer buffer;
BufferTexture texture;
texture.setBuffer(BufferTextureFormat::RGB32F, buffer);

Vector3 data[200]{
    // ...
};
buffer.setData(data, BufferUsage::StaticDraw);
/* [BufferTexture-usage] */
}
#endif

#ifndef MAGNUM_TARGET_GLES
{
/* [Context-supportedVersion] */
Version v1 = Context::current().isVersionSupported(Version::GL330) ?
    Version::GL330 : Version::GL210;
Version v2 = Context::current().supportedVersion({Version::GL330, Version::GL210});
/* [Context-supportedVersion] */
static_cast<void>(v1);
static_cast<void>(v2);

/* [Context-isExtensionSupported] */
if(Context::current().isExtensionSupported<Extensions::GL::ARB::tessellation_shader>()) {
    // draw fancy detailed model
} else {
    // texture fallback
}
/* [Context-isExtensionSupported] */

/* [Context-isExtensionSupported-version] */
const Version version = Context::current().supportedVersion({
    Version::GL320, Version::GL300, Version::GL210});
if(Context::current().isExtensionSupported<Extensions::GL::ARB::explicit_attrib_location>(version)) {
    // Called only if ARB_explicit_attrib_location is supported
    // *and* version is higher than GL 3.1
}
/* [Context-isExtensionSupported-version] */

/* [Context-MAGNUM_ASSERT_VERSION_SUPPORTED] */
MAGNUM_ASSERT_VERSION_SUPPORTED(Version::GL330);
/* [Context-MAGNUM_ASSERT_VERSION_SUPPORTED] */

/* [Context-MAGNUM_ASSERT_EXTENSION_SUPPORTED] */
MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::ARB::geometry_shader4);
/* [Context-MAGNUM_ASSERT_EXTENSION_SUPPORTED] */
}
#endif

#if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
{
char data[1]{};
ImageView2D negativeX(PixelFormat::RGBA, PixelType::UnsignedByte, {256, 256}, data);
/* [CubeMapTexture-usage] */
ImageView2D positiveX(PixelFormat::RGBA, PixelType::UnsignedByte, {256, 256}, data);
// ...

CubeMapTexture texture;
texture.setMagnificationFilter(Sampler::Filter::Linear)
    // ...
    .setStorage(Math::log2(256)+1, TextureFormat::RGBA8, {256, 256})
    .setSubImage(CubeMapCoordinate::PositiveX, 0, {}, positiveX)
    .setSubImage(CubeMapCoordinate::NegativeX, 0, {}, negativeX)
    // ...
/* [CubeMapTexture-usage] */
    ;
}
#endif

#ifndef MAGNUM_TARGET_GLES
{
CubeMapTexture texture;
/* [CubeMapTexture-image1] */
Image3D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte});
/* [CubeMapTexture-image1] */
}

{
CubeMapTexture texture;
/* [CubeMapTexture-image2] */
BufferImage3D image = texture.image(0,
    {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
/* [CubeMapTexture-image2] */
}

{
CubeMapTexture texture;
/* [CubeMapTexture-compressedImage1] */
CompressedImage3D image = texture.compressedImage(0, {});
/* [CubeMapTexture-compressedImage1] */
}

{
CubeMapTexture texture;
/* [CubeMapTexture-compressedImage2] */
CompressedBufferImage3D image = texture.compressedImage(0, {},
    BufferUsage::StaticRead);
/* [CubeMapTexture-compressedImage2] */
}

{
CubeMapTexture texture;
/* [CubeMapTexture-image3] */
Image2D image = texture.image(CubeMapCoordinate::PositiveX, 0,
    {PixelFormat::RGBA, PixelType::UnsignedByte});
/* [CubeMapTexture-image3] */
}

{
CubeMapTexture texture;
/* [CubeMapTexture-image4] */
BufferImage2D image = texture.image(CubeMapCoordinate::PositiveX, 0,
    {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
/* [CubeMapTexture-image4] */
}

{
CubeMapTexture texture;
/* [CubeMapTexture-compressedImage3] */
CompressedImage2D image = texture.compressedImage(CubeMapCoordinate::PositiveX,
    0, {});
/* [CubeMapTexture-compressedImage3] */
}

{
CubeMapTexture texture;
/* [CubeMapTexture-compressedImage4] */
CompressedBufferImage2D image = texture.compressedImage(CubeMapCoordinate::PositiveX,
    0, {}, BufferUsage::StaticRead);
/* [CubeMapTexture-compressedImage4] */
}

{
CubeMapTexture texture;
Range3Di range;
/* [CubeMapTexture-subImage1] */
Image3D image = texture.subImage(0, range,
    {PixelFormat::RGBA, PixelType::UnsignedByte});
/* [CubeMapTexture-subImage1] */
}

{
CubeMapTexture texture;
Range3Di range;
/* [CubeMapTexture-subImage2] */
BufferImage3D image = texture.subImage(0, range,
    {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
/* [CubeMapTexture-subImage2] */
}

{
CubeMapTexture texture;
Range3Di range;
/* [CubeMapTexture-compressedSubImage1] */
CompressedImage3D image = texture.compressedSubImage(0, range, {});
/* [CubeMapTexture-compressedSubImage1] */
}

{
CubeMapTexture texture;
Range3Di range;
/* [CubeMapTexture-compressedSubImage2] */
CompressedBufferImage3D image = texture.compressedSubImage(0, range, {},
    BufferUsage::StaticRead);
/* [CubeMapTexture-compressedSubImage2] */
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
{
char data[1]{};
ImageView3D imageNegativeX(PixelFormat::RGBA, PixelType::UnsignedByte, {64, 64, 1}, data);
ImageView3D imagePositiveY(PixelFormat::RGBA, PixelType::UnsignedByte, {64, 64, 1}, data);
/* [CubeMapTextureArray-usage] */
CubeMapTextureArray texture;
texture.setMagnificationFilter(Sampler::Filter::Linear)
    // ...
    .setStorage(Math::log2(64)+1, TextureFormat::RGBA8, {64, 64, 24});

for(std::size_t i = 0; i != 4; i += 6) {
    ImageView3D imagePositiveX(PixelFormat::RGBA, PixelType::UnsignedByte, {64, 64, 1}, data);
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
CubeMapTextureArray texture;
/* [CubeMapTextureArray-image1] */
Image3D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte});
/* [CubeMapTextureArray-image1] */
}

{
CubeMapTextureArray texture;
/* [CubeMapTextureArray-image2] */
BufferImage3D image = texture.image(0,
    {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
/* [CubeMapTextureArray-image2] */
}

{
CubeMapTextureArray texture;
/* [CubeMapTextureArray-compressedImage1] */
CompressedImage3D image = texture.compressedImage(0, {});
/* [CubeMapTextureArray-compressedImage1] */
}

{
CubeMapTextureArray texture;
/* [CubeMapTextureArray-compressedImage2] */
CompressedBufferImage3D image = texture.compressedImage(0, {}, BufferUsage::StaticRead);
/* [CubeMapTextureArray-compressedImage2] */
}

{
CubeMapTextureArray texture;
Range3Di range;
/* [CubeMapTextureArray-subImage1] */
Image3D image = texture.subImage(0, range,
    {PixelFormat::RGBA, PixelType::UnsignedByte});
/* [CubeMapTextureArray-subImage1] */
}

{
CubeMapTextureArray texture;
Range3Di range;
/* [CubeMapTextureArray-subImage2] */
BufferImage3D image = texture.subImage(0, range,
    {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
/* [CubeMapTextureArray-subImage2] */
}

{
CubeMapTextureArray texture;
Range3Di range;
/* [CubeMapTextureArray-compressedSubImage1] */
CompressedImage3D image = texture.compressedSubImage(0, range, {});
/* [CubeMapTextureArray-compressedSubImage1] */
}

{
CubeMapTextureArray texture;
Range3Di range;
/* [CubeMapTextureArray-compressedSubImage2] */
CompressedBufferImage3D image = texture.compressedSubImage(0, range, {},
    BufferUsage::StaticRead);
/* [CubeMapTextureArray-compressedSubImage2] */
}
#endif
#endif

#ifndef MAGNUM_TARGET_WEBGL
{
Mesh mesh;
struct: AbstractShaderProgram {} shader;
/* [DebugOutput-usage] */
Renderer::enable(Renderer::Feature::DebugOutput);
Renderer::enable(Renderer::Feature::DebugOutputSynchronous);
DebugOutput::setDefaultCallback();

/* Disable rather spammy "Buffer detailed info" debug messages on NVidia drivers */
DebugOutput::setEnabled(DebugOutput::Source::Api, DebugOutput::Type::Other, {131185}, false);

{
    DebugGroup group{DebugGroup::Source::Application, 42, "Scene rendering"};

    DebugMessage::insert(DebugMessage::Source::Application, DebugMessage::Type::Marker,
        1337, DebugOutput::Severity::Notification, "Rendering transparent mesh");

    Renderer::enable(Renderer::Feature::Blending);
    mesh.draw(shader);
    Renderer::disable(Renderer::Feature::Blending);

    // ...
}
/* [DebugOutput-usage] */
}

{
/* [DebugOutput-setDefaultCallback] */
DebugMessage::insert(DebugMessage::Source::Application,
    DebugMessage::Type::Marker, 1337, DebugOutput::Severity::Notification,
    "Hello from OpenGL command stream!");
/* [DebugOutput-setDefaultCallback] */
}

{
/* [DebugMessage-usage] */
DebugMessage::insert(DebugMessage::Source::Application,
    DebugMessage::Type::Marker, 1337, DebugOutput::Severity::Notification,
    "Hello from OpenGL command stream!");
/* [DebugMessage-usage] */
}

{
Mesh mesh;
struct: AbstractShaderProgram {} shader;
/* [DebugGroup-usage1] */
{
    /* Push debug group */
    DebugGroup group{DebugGroup::Source::Application, 42, "Scene rendering"};

    Renderer::enable(Renderer::Feature::Blending);
    mesh.draw(shader);
    Renderer::disable(Renderer::Feature::Blending);

    /* The debug group is popped automatically at the end of the scope */
}
/* [DebugGroup-usage1] */
}

{
Mesh mesh;
struct: AbstractShaderProgram {} shader;
/* [DebugGroup-usage2] */
DebugGroup group;

group.push(DebugGroup::Source::Application, 42, "Scene rendering");

Renderer::enable(Renderer::Feature::Blending);
mesh.draw(shader);
Renderer::disable(Renderer::Feature::Blending);

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
defaultFramebuffer.mapForDraw({
    {MyShader::ColorOutput, DefaultFramebuffer::DrawAttachment::Back},
    {MyShader::NormalOutput, DefaultFramebuffer::DrawAttachment::None}});
/* [DefaultFramebuffer-usage-map] */
}

#ifndef MAGNUM_TARGET_GLES2
{
/* [Framebuffer-usage-attach] */
Framebuffer framebuffer{defaultFramebuffer.viewport()};
Texture2D color, normal;
Renderbuffer depthStencil;

// configure the textures and allocate texture memory...

framebuffer.attachTexture(Framebuffer::ColorAttachment{0}, color, 0);
framebuffer.attachTexture(Framebuffer::ColorAttachment{1}, normal, 0);
framebuffer.attachRenderbuffer(Framebuffer::BufferAttachment::DepthStencil, depthStencil);
/* [Framebuffer-usage-attach] */
}
#endif

{
struct MyShader {
    enum: UnsignedInt {
        ColorOutput = 0,
        NormalOutput = 1
    };
};
Framebuffer framebuffer{{}};
/* [Framebuffer-usage-map] */
framebuffer.mapForDraw({{MyShader::ColorOutput, Framebuffer::ColorAttachment(0)},
                        {MyShader::NormalOutput, Framebuffer::ColorAttachment(1)}});
/* [Framebuffer-usage-map] */
}

{
/* [Mesh-nonindexed] */
/* Custom shader, needing only position data */
class MyShader: public AbstractShaderProgram {
    public:
        typedef Attribute<0, Vector3> Position;

    // ...
};

/* Fill vertex buffer with position data */
Vector3 positions[30]{
    // ...
};
Buffer vertexBuffer;
vertexBuffer.setData(positions, BufferUsage::StaticDraw);

/* Configure the mesh, add vertex buffer */
Mesh mesh;
mesh.setPrimitive(MeshPrimitive::Triangles)
    .addVertexBuffer(vertexBuffer, 0, MyShader::Position{})
    .setCount(30);
/* [Mesh-nonindexed] */
}

{
/* [Mesh-interleaved] */
/* Non-indexed primitive with positions and normals */
Trade::MeshData3D plane = Primitives::planeSolid();

/* Fill a vertex buffer with interleaved position and normal data */
Buffer buffer;
buffer.setData(MeshTools::interleave(plane.positions(0), plane.normals(0)), BufferUsage::StaticDraw);

/* Configure the mesh, add the vertex buffer */
Mesh mesh;
mesh.setPrimitive(plane.primitive())
    .setCount(plane.positions(0).size())
    .addVertexBuffer(buffer, 0, Shaders::Phong::Position{}, Shaders::Phong::Normal{});
/* [Mesh-interleaved] */
}

{
/* [Mesh-indexed] */
/* Custom shader */
class MyShader: public AbstractShaderProgram {
    public:
        typedef Attribute<0, Vector3> Position;

    // ...
};

/* Fill vertex buffer with position data */
Vector3 positions[240]{
    // ...
};
Buffer vertexBuffer;
vertexBuffer.setData(positions, BufferUsage::StaticDraw);

/* Fill index buffer with index data */
UnsignedByte indices[75]{
    // ...
};
Buffer indexBuffer;
indexBuffer.setData(indices, BufferUsage::StaticDraw);

/* Configure the mesh, add both vertex and index buffer */
Mesh mesh;
mesh.setPrimitive(MeshPrimitive::Triangles)
    .setCount(75)
    .addVertexBuffer(vertexBuffer, 0, MyShader::Position{})
    .setIndexBuffer(indexBuffer, 0, Mesh::IndexType::UnsignedByte, 176, 229);
/* [Mesh-indexed] */
}

{
/* [Mesh-indexed-tools] */
// Indexed primitive
Trade::MeshData3D cube = Primitives::cubeSolid();

// Fill vertex buffer with interleaved position and normal data
Buffer vertexBuffer;
vertexBuffer.setData(MeshTools::interleave(cube.positions(0), cube.normals(0)), BufferUsage::StaticDraw);

// Compress index data
Containers::Array<char> indexData;
Mesh::IndexType indexType;
UnsignedInt indexStart, indexEnd;
std::tie(indexData, indexType, indexStart, indexEnd) = MeshTools::compressIndices(cube.indices());

// Fill index buffer
Buffer indexBuffer;
indexBuffer.setData(indexData, BufferUsage::StaticDraw);

// Configure the mesh, add both vertex and index buffer
Mesh mesh;
mesh.setPrimitive(cube.primitive())
    .setCount(cube.indices().size())
    .addVertexBuffer(vertexBuffer, 0, Shaders::Phong::Position{}, Shaders::Phong::Normal{})
    .setIndexBuffer(indexBuffer, 0, indexType, indexStart, indexEnd);
/* [Mesh-indexed-tools] */
}

#ifndef MAGNUM_TARGET_GLES
{
/* [Mesh-formats] */
// Custom shader with colors specified as four floating-point values
class MyShader: public AbstractShaderProgram {
    public:
        typedef Attribute<0, Vector3> Position;
        typedef Attribute<1, Color4> Color;

    // ...
};

/* Initial mesh configuration */
Mesh mesh;
mesh.setPrimitive(MeshPrimitive::Triangles)
    .setCount(30);

/* Fill position buffer with positions specified as two-component XY (i.e.,
   no Z component, which is meant to be always 0) */
Vector2 positions[30]{
    // ...
};
Buffer positionBuffer;
positionBuffer.setData(positions, BufferUsage::StaticDraw);

/* Specify layout of positions buffer -- only two components, unspecified Z
   component will be automatically set to 0 */
mesh.addVertexBuffer(positionBuffer, 0,
    MyShader::Position{MyShader::Position::Components::Two});

/* Fill color buffer with colors specified as four-byte BGRA (i.e., directly
   from a TGA file) */
UnsignedByte colors[4*30]{
    // ...
};
Buffer colorBuffer;
colorBuffer.setData(colors, BufferUsage::StaticDraw);

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
Mesh mesh;
Buffer colorBuffer;
/* [Mesh-dynamic] */
mesh.addVertexBuffer(colorBuffer, 0, 4, DynamicAttribute{
    DynamicAttribute::Kind::GenericNormalized, 3,
    DynamicAttribute::Components::Three,
    DynamicAttribute::DataType::UnsignedByte});
/* [Mesh-dynamic] */
}

{
/* [Mesh-addVertexBuffer1] */
Buffer buffer;
Mesh mesh;
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
MultisampleTexture2D texture;
texture.setStorage(16, TextureFormat::RGBA8, {1024, 1024});
/* [MultisampleTexture-usage] */
}
#endif

#if !defined(MAGNUM_TARGET_WEBGL) && !defined(CORRADE_TARGET_ANDROID)
struct A: TestSuite::Tester {
void foo() {
/* [OpenGLTester-MAGNUM_VERIFY_NO_ERROR] */
CORRADE_COMPARE(Magnum::Renderer::error(), Magnum::Renderer::Error::NoError);
/* [OpenGLTester-MAGNUM_VERIFY_NO_ERROR] */
}
};
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
{
/* [PrimitiveQuery-usage] */
PrimitiveQuery q{PrimitiveQuery::Target::PrimitivesGenerated};

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
ImageView2D image{PixelFormat::RGBA, PixelType::UnsignedByte, {526, 137}, data};

RectangleTexture texture;
texture.setMagnificationFilter(Sampler::Filter::Linear)
    .setMinificationFilter(Sampler::Filter::Linear)
    .setWrapping(Sampler::Wrapping::ClampToEdge)
    .setMaxAnisotropy(Sampler::maxMaxAnisotropy())
    .setStorage(TextureFormat::RGBA8, {526, 137})
    .setSubImage({}, image);
/* [RectangleTexture-usage] */
}

{
RectangleTexture texture;
/* [RectangleTexture-image1] */
Image2D image = texture.image({PixelFormat::RGBA, PixelType::UnsignedByte});
/* [RectangleTexture-image1] */
}

{
RectangleTexture texture;
/* [RectangleTexture-image2] */
BufferImage2D image = texture.image(
    {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
/* [RectangleTexture-image2] */
}

{
RectangleTexture texture;
/* [RectangleTexture-compressedImage1] */
CompressedImage2D image = texture.compressedImage({});
/* [RectangleTexture-compressedImage1] */
}

{
RectangleTexture texture;
/* [RectangleTexture-compressedImage2] */
CompressedBufferImage2D image = texture.compressedImage({},
    BufferUsage::StaticRead);
/* [RectangleTexture-compressedImage2] */
}

{
RectangleTexture texture;
Range2Di range;
/* [RectangleTexture-subImage1] */
Image2D image = texture.subImage(range,
    {PixelFormat::RGBA, PixelType::UnsignedByte});
/* [RectangleTexture-subImage1] */
}

{
RectangleTexture texture;
Range2Di range;
/* [RectangleTexture-subImage2] */
BufferImage2D image = texture.subImage(range,
    {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
/* [RectangleTexture-subImage2] */
}

{
RectangleTexture texture;
Range2Di range;
/* [RectangleTexture-compressedSubImage1] */
CompressedImage2D image = texture.compressedSubImage(range, {});
/* [RectangleTexture-compressedSubImage1] */
}

{
RectangleTexture texture;
Range2Di range;
/* [RectangleTexture-compressedSubImage2] */
CompressedBufferImage2D image = texture.compressedSubImage(range, {},
    BufferUsage::StaticRead);
/* [RectangleTexture-compressedSubImage2] */
}
#endif

#if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
{
/* [SampleQuery-usage] */
SampleQuery q{SampleQuery::Target::AnySamplesPassed};

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
SampleQuery q{SampleQuery::Target::AnySamplesPassed};

q.begin();
/* Render simplified object to test whether it is visible at all */
// ...
q.end();

q.beginConditionalRender(SampleQuery::ConditionalRenderMode::Wait);
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
ImageView2D image(PixelFormat::RGBA, PixelType::UnsignedByte, {4096, 4096}, data);

Texture2D texture;
texture.setMagnificationFilter(Sampler::Filter::Linear)
    .setMinificationFilter(Sampler::Filter::Linear, Sampler::Mipmap::Linear)
    .setWrapping(Sampler::Wrapping::ClampToEdge)
    .setMaxAnisotropy(Sampler::maxMaxAnisotropy())
    .setStorage(Math::log2(4096)+1, TextureFormat::RGBA8, {4096, 4096})
    .setSubImage(0, {}, image)
    .generateMipmap();
/* [Texture-usage] */
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
{
Texture2D texture;
/* [Texture-setSwizzle] */
texture.setSwizzle<'b', 'g', 'r', '0'>();
/* [Texture-setSwizzle] */
}
#endif

#ifndef MAGNUM_TARGET_GLES
{
Texture2D texture;
/* [Texture-image1] */
Image2D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte});
/* [Texture-image1] */
}

{
Texture2D texture;
/* [Texture-image2] */
BufferImage2D image = texture.image(0,
    {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
/* [Texture-image2] */
}

{
Texture2D texture;
/* [Texture-compressedImage1] */
CompressedImage2D image = texture.compressedImage(0, {});
/* [Texture-compressedImage1] */
}

{
Texture2D texture;
/* [Texture-compressedImage2] */
CompressedBufferImage2D image = texture.compressedImage(0, {},
    BufferUsage::StaticRead);
/* [Texture-compressedImage2] */
}

{
Texture2D texture;
Range2Di range;
/* [Texture-subImage1] */
Image2D image = texture.subImage(0, range,
    {PixelFormat::RGBA, PixelType::UnsignedByte});
/* [Texture-subImage1] */
}

{
Texture2D texture;
Range2Di range;
/* [Texture-subImage2] */
BufferImage2D image = texture.subImage(0, range,
    {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
/* [Texture-subImage2] */
}

{
Texture2D texture;
Range2Di range;
/* [Texture-compressedSubImage1] */
CompressedImage2D image = texture.compressedSubImage(0, range, {});
/* [Texture-compressedSubImage1] */
}

{
Texture2D texture;
Range2Di range;
/* [Texture-compressedSubImage2] */
CompressedBufferImage2D image = texture.compressedSubImage(0, range, {},
    BufferUsage::StaticRead);
/* [Texture-compressedSubImage2] */
}
#endif

#ifndef MAGNUM_TARGET_GLES2
{
/* [TextureArray-usage1] */
Texture2DArray texture;
texture.setMagnificationFilter(Sampler::Filter::Linear)
    .setMinificationFilter(Sampler::Filter::Linear, Sampler::Mipmap::Linear)
    .setWrapping(Sampler::Wrapping::ClampToEdge)
    .setMaxAnisotropy(Sampler::maxMaxAnisotropy());;
/* [TextureArray-usage1] */

Int levels{};
char data[1]{};
/* [TextureArray-usage2] */
texture.setStorage(levels, TextureFormat::RGBA8, {64, 64, 16});

for(std::size_t i = 0; i != 16; ++i) {
    ImageView3D image(PixelFormat::RGBA, PixelType::UnsignedByte, {64, 64, 1}, data);
    texture.setSubImage(0, Vector3i::zAxis(i), image);
}
/* [TextureArray-usage2] */
}

#ifndef MAGNUM_TARGET_GLES
{
Texture2DArray texture;
/* [TextureArray-image1] */
Image3D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte});
/* [TextureArray-image1] */
}

{
Texture2DArray texture;
/* [TextureArray-image2] */
BufferImage3D image = texture.image(0,
    {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
/* [TextureArray-image2] */
}

{
Texture2DArray texture;
/* [TextureArray-compressedImage1] */
CompressedImage3D image = texture.compressedImage(0, {});
/* [TextureArray-compressedImage1] */
}

{
Texture2DArray texture;
/* [TextureArray-compressedImage2] */
CompressedBufferImage3D image = texture.compressedImage(0, {},
    BufferUsage::StaticRead);
/* [TextureArray-compressedImage2] */
}

{
Texture2DArray texture;
Range3Di range;
/* [TextureArray-subImage1] */
Image3D image = texture.subImage(0, range,
    {PixelFormat::RGBA, PixelType::UnsignedByte});
/* [TextureArray-subImage1] */
}

{
Texture2DArray texture;
Range3Di range;
/* [TextureArray-subImage2] */
BufferImage3D image = texture.subImage(0, range,
    {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
/* [TextureArray-subImage2] */
}

{
Texture2DArray texture;
Range3Di range;
/* [TextureArray-compressedSubImage1] */
CompressedImage3D image = texture.compressedSubImage(0, range, {});
/* [TextureArray-compressedSubImage1] */
}

{
Texture2DArray texture;
Range3Di range;
/* [TextureArray-compressedSubImage2] */
CompressedBufferImage3D image = texture.compressedSubImage(0, range, {},
    BufferUsage::StaticRead);
/* [TextureArray-compressedSubImage2] */
}
#endif
#endif

#ifndef MAGNUM_TARGET_WEBGL
{
/* [TimeQuery-usage1] */
TimeQuery q1{TimeQuery::Target::TimeElapsed},
          q2{TimeQuery::Target::TimeElapsed};

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
TimeQuery q1{TimeQuery::Target::Timestamp},
          q2{TimeQuery::Target::Timestamp},
          q3{TimeQuery::Target::Timestamp};

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
