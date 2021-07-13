/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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

#include <numeric>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/ArrayViewStl.h>
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/ColorMap.h"
#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/DefaultFramebuffer.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/MeshView.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/GL/Renderbuffer.h"
#include "Magnum/GL/RenderbufferFormat.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/GL/Version.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Math/FunctionsBatch.h"
#include "Magnum/MeshTools/Duplicate.h"
#include "Magnum/Shaders/DistanceFieldVectorGL.h"
#include "Magnum/Shaders/FlatGL.h"
#include "Magnum/Shaders/MeshVisualizerGL.h"
#include "Magnum/Shaders/PhongGL.h"
#include "Magnum/Shaders/VectorGL.h"
#include "Magnum/Shaders/VertexColorGL.h"
#include "Magnum/Trade/LightData.h"

#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/TextureArray.h"
#include "Magnum/Shaders/DistanceFieldVector.h"
#include "Magnum/Shaders/Flat.h"
#include "Magnum/Shaders/Generic.h"
#include "Magnum/Shaders/MeshVisualizer.h"
#include "Magnum/Shaders/Phong.h"
#include "Magnum/Shaders/Vector.h"
#endif

#define DOXYGEN_IGNORE(...) __VA_ARGS__

using namespace Magnum;
using namespace Magnum::Math::Literals;

int main() {

/* internal compiler error: in gimplify_init_constructor, at gimplify.c:4271
   on GCC 4.8 in the [60] array */
#if !defined(__GNUC__) || defined(__clang__) || __GNUC__*100 + __GNUC_MINOR__ >= 500
{
/* [shaders-setup] */
struct Vertex {
    Vector3 position;
    Vector3 normal;
    Vector2 textureCoordinates;
};
Vertex data[60]{
    // ...
};

GL::Buffer vertices;
vertices.setData(data, GL::BufferUsage::StaticDraw);

GL::Mesh mesh;
mesh.addVertexBuffer(vertices, 0,
    Shaders::PhongGL::Position{},
    Shaders::PhongGL::Normal{},
    Shaders::PhongGL::TextureCoordinates{})
     //...
     ;
/* [shaders-setup] */
}
#endif

{
GL::Mesh mesh;
/* [shaders-classic] */
Matrix4 transformationMatrix{DOXYGEN_IGNORE()}, projectionMatrix{DOXYGEN_IGNORE()};

Shaders::PhongGL shader;
shader
    .setTransformationMatrix(transformationMatrix)
    .setProjectionMatrix(projectionMatrix)
    .setNormalMatrix(transformationMatrix.normalMatrix())
    .setDiffuseColor(0x2f83cc_rgbf)
    .setLightColors({0xe9ecae_rgbf})
    .draw(mesh);
/* [shaders-classic] */
}

#ifndef MAGNUM_TARGET_GLES2
{
GL::Mesh mesh;
Matrix4 transformationMatrix, projectionMatrix;
/* [shaders-ubo] */
GL::Buffer projectionUniform, lightUniform, materialUniform,
    transformationUniform, drawUniform;
projectionUniform.setData({
    Shaders::ProjectionUniform3D{}
        .setProjectionMatrix(projectionMatrix)
});
lightUniform.setData({
    Shaders::PhongLightUniform{}
        .setColor(0xe9ecae_rgbf)
});
materialUniform.setData({
    Shaders::PhongMaterialUniform{}
        .setDiffuseColor(0x2f83cc_rgbf)
});
transformationUniform.setData({
    Shaders::TransformationUniform3D{}
        .setTransformationMatrix(transformationMatrix)
});
drawUniform.setData({
    Shaders::PhongDrawUniform{}
        .setNormalMatrix(transformationMatrix.normalMatrix())
});

Shaders::PhongGL shader{Shaders::PhongGL::Flag::UniformBuffers};
shader
    .bindProjectionBuffer(projectionUniform)
    .bindLightBuffer(lightUniform)
    .bindMaterialBuffer(materialUniform)
    .bindTransformationBuffer(transformationUniform)
    .bindDrawBuffer(drawUniform)
    .draw(mesh);
/* [shaders-ubo] */
}

{
GL::Buffer projectionUniform, transformationUniform, drawUniform, lightUniform,
    materialUniform;
/* [shaders-multi] */
GL::Mesh redCone{DOXYGEN_IGNORE()}, yellowCube{DOXYGEN_IGNORE()}, redSphere{DOXYGEN_IGNORE()};
Matrix4 redConeTransformation{DOXYGEN_IGNORE()},
    yellowCubeTransformation{DOXYGEN_IGNORE()},
    redSphereTransformation{DOXYGEN_IGNORE()};

materialUniform.setData({
    Shaders::PhongMaterialUniform{}
        .setDiffuseColor(0xcd3431_rgbf),
    Shaders::PhongMaterialUniform{}
        .setDiffuseColor(0xc7cf2f_rgbf),
});
transformationUniform.setData({
    Shaders::TransformationUniform3D{}
        .setTransformationMatrix(redConeTransformation),
    Shaders::TransformationUniform3D{}
        .setTransformationMatrix(yellowCubeTransformation),
    Shaders::TransformationUniform3D{}
        .setTransformationMatrix(redSphereTransformation),
});
drawUniform.setData({
    Shaders::PhongDrawUniform{}
        .setNormalMatrix(redConeTransformation.normalMatrix())
        .setMaterialId(0),
    Shaders::PhongDrawUniform{}
        .setNormalMatrix(yellowCubeTransformation.normalMatrix())
        .setMaterialId(1),
    Shaders::PhongDrawUniform{}
        .setNormalMatrix(redSphereTransformation.normalMatrix())
        .setMaterialId(0),
});

/* One light, two materials, three draws */
Shaders::PhongGL shader{Shaders::PhongGL::Flag::UniformBuffers, 1, 2, 3};
shader
    .bindProjectionBuffer(projectionUniform)
    .bindTransformationBuffer(transformationUniform)
    .bindDrawBuffer(drawUniform)
    .bindLightBuffer(lightUniform)
    .bindMaterialBuffer(materialUniform)
    .setDrawOffset(0)
    .draw(redCone)
    .setDrawOffset(1)
    .draw(yellowCube)
    .setDrawOffset(2)
    .draw(redSphere);
/* [shaders-multi] */
}

{
GL::Mesh mesh;
/* [shaders-multidraw] */
GL::MeshView redConeView{DOXYGEN_IGNORE(mesh)}, yellowCubeView{DOXYGEN_IGNORE(mesh)}, redSphereView{DOXYGEN_IGNORE(mesh)};
DOXYGEN_IGNORE()

/* One light, two materials, three draws; with multidraw enabled */
Shaders::PhongGL shader{Shaders::PhongGL::Flag::MultiDraw, 1, 2, 3};
shader
    DOXYGEN_IGNORE()
    .draw({redConeView, yellowCubeView, redSphereView});
/* [shaders-multidraw] */
}
#endif

{
Matrix4 projectionMatrix;
/* [shaders-instancing] */
Matrix4 redSphereTransformation{DOXYGEN_IGNORE()},
    yellowSphereTransformation{DOXYGEN_IGNORE()},
    greenSphereTransformation{DOXYGEN_IGNORE()};

struct {
    Matrix4 transformationMatrix;
    Matrix3x3 normalMatrix;
    Color3 color;
} instanceData[]{
    {redSphereTransformation,
     redSphereTransformation.normalMatrix(),
     0xcd3431_rgbf},
    {yellowSphereTransformation,
     yellowSphereTransformation.normalMatrix(),
     0xc7cf2f_rgbf},
    {greenSphereTransformation,
     greenSphereTransformation.normalMatrix(),
     0x3bd267_rgbf},
};

GL::Mesh sphereInstanced{DOXYGEN_IGNORE()};
sphereInstanced.addVertexBufferInstanced(GL::Buffer{instanceData}, 1, 0,
    Shaders::PhongGL::TransformationMatrix{},
    Shaders::PhongGL::NormalMatrix{},
    Shaders::PhongGL::Color3{});
sphereInstanced.setInstanceCount(3);

Shaders::PhongGL shader{Shaders::PhongGL::Flag::InstancedTransformation|
                        Shaders::PhongGL::Flag::VertexColor};
shader
    .setProjectionMatrix(projectionMatrix)
    DOXYGEN_IGNORE()
    .draw(sphereInstanced);
/* [shaders-instancing] */
}

{
GL::Mesh mesh;
/* [shaders-textures] */
GL::Texture2D diffuseTexture;
DOXYGEN_IGNORE()

Shaders::PhongGL shader{Shaders::PhongGL::Flag::DiffuseTexture};
shader.bindDiffuseTexture(diffuseTexture)
    DOXYGEN_IGNORE()
    .draw(mesh);
/* [shaders-textures] */
}

#ifndef MAGNUM_TARGET_GLES2
{
GL::Mesh mesh;
GL::MeshView redConeView{DOXYGEN_IGNORE(mesh)}, yellowCubeView{DOXYGEN_IGNORE(mesh)}, redSphereView{DOXYGEN_IGNORE(mesh)};
/* [shaders-texture-arrays] */
ImageView2D coneDiffuse{DOXYGEN_IGNORE({}, {})}, cubeDiffuse{DOXYGEN_IGNORE({}, {})}, sphereDiffuse{DOXYGEN_IGNORE({}, {})};

GL::Texture2DArray diffuseTexture;
diffuseTexture
    DOXYGEN_IGNORE()
    /* Assuming all iamges have the same format and size */
    .setStorage(1, GL::textureFormat(coneDiffuse.format()),
        {coneDiffuse.size(), 3})
    .setSubImage(0, {}, coneDiffuse)
    .setSubImage(1, {}, cubeDiffuse)
    .setSubImage(2, {}, sphereDiffuse);

GL::Buffer textureTransformationUniform;
textureTransformationUniform.setData({
    Shaders::TextureTransformationUniform{}
        .setLayer(0),
    Shaders::TextureTransformationUniform{}
        .setLayer(1),
    Shaders::TextureTransformationUniform{}
        .setLayer(2),
});

Shaders::PhongGL shader{
    Shaders::PhongGL::Flag::MultiDraw|
    Shaders::PhongGL::Flag::DiffuseTexture|
    Shaders::PhongGL::Flag::TextureArrays,
    1, 2, 3};
shader
    DOXYGEN_IGNORE()
    .bindDiffuseTexture(diffuseTexture)
    .bindTextureTransformationBuffer(textureTransformationUniform)
    .draw({redConeView, yellowCubeView, redSphereView});
/* [shaders-texture-arrays] */
}
#endif

{
GL::Buffer vertices;
GL::Mesh mesh;
/* [shaders-generic] */
mesh.addVertexBuffer(vertices, 0,
    Shaders::GenericGL3D::Position{},
    Shaders::GenericGL3D::Normal{},
    Shaders::GenericGL3D::TextureCoordinates{});
/* [shaders-generic] */
}

{
GL::Mesh mesh;
Matrix4 transformationMatrix, projectionMatrix;
/* [shaders-meshvisualizer] */
Shaders::MeshVisualizerGL3D shader{Shaders::MeshVisualizerGL3D::Flag::Wireframe};
shader
    .setColor(0x2f83cc_rgbf)
    .setWireframeColor(0xdcdcdc_rgbf)
    .setViewportSize(Vector2{GL::defaultFramebuffer.viewport().size()})
    .setTransformationMatrix(transformationMatrix)
    .setProjectionMatrix(projectionMatrix)
    .draw(mesh);
/* [shaders-meshvisualizer] */
}

/* internal compiler error: in gimplify_init_constructor, at gimplify.c:4271
   on GCC 4.8 in the [60] array */
#if !defined(__GNUC__) || defined(__clang__) || __GNUC__*100 + __GNUC_MINOR__ >= 500
{
/* [DistanceFieldVectorGL-usage1] */
struct Vertex {
    Vector2 position;
    Vector2 textureCoordinates;
};
Vertex data[60]{
    // ...
};

GL::Buffer vertices;
vertices.setData(data, GL::BufferUsage::StaticDraw);

GL::Mesh mesh;
mesh.addVertexBuffer(vertices, 0,
    Shaders::DistanceFieldVectorGL2D::Position{},
    Shaders::DistanceFieldVectorGL2D::TextureCoordinates{})
    // ...
    ;
/* [DistanceFieldVectorGL-usage1] */
}

{
GL::Mesh mesh;
/* [DistanceFieldVectorGL-usage2] */
Matrix3 transformationMatrix, projectionMatrix;
GL::Texture2D texture;

Shaders::DistanceFieldVectorGL2D shader;
shader.setColor(0x2f83cc_rgbf)
    .setOutlineColor(0xdcdcdc_rgbf)
    .setOutlineRange(0.6f, 0.4f)
    .setTransformationProjectionMatrix(projectionMatrix*transformationMatrix)
    .bindVectorTexture(texture)
    .draw(mesh);
/* [DistanceFieldVectorGL-usage2] */
}
#endif

#ifndef MAGNUM_TARGET_GLES2
{
GL::Mesh mesh;
Matrix3 transformationMatrix, projectionMatrix;
GL::Texture2D texture;
/* [DistanceFieldVectorGL-ubo] */
GL::Buffer projectionTransformationUniform, materialUniform, drawUniform;
projectionTransformationUniform.setData({
    Shaders::TransformationProjectionUniform2D{}
        .setTransformationProjectionMatrix(transformationMatrix*projectionMatrix)
});
materialUniform.setData({
    Shaders::DistanceFieldVectorMaterialUniform{}
        .setColor(0x2f83cc_rgbf)
        .setOutlineColor(0xdcdcdc_rgbf)
        .setOutlineRange(0.6f, 0.4f)
});
drawUniform.setData({
    Shaders::DistanceFieldVectorDrawUniform{}
        .setMaterialId(0)
});

Shaders::DistanceFieldVectorGL2D shader{
    Shaders::DistanceFieldVectorGL2D::Flag::UniformBuffers
};
shader
    .bindTransformationProjectionBuffer(projectionTransformationUniform)
    .bindMaterialBuffer(materialUniform)
    .bindDrawBuffer(drawUniform)
    .bindVectorTexture(texture)
    .draw(mesh);
/* [DistanceFieldVectorGL-ubo] */
}
#endif

/* internal compiler error: in gimplify_init_constructor, at gimplify.c:4271
   on GCC 4.8 in the [60] array */
#if !defined(__GNUC__) || defined(__clang__) || __GNUC__*100 + __GNUC_MINOR__ >= 500
{
/* [FlatGL-usage-colored1] */
struct Vertex {
    Vector3 position;
};
Vertex data[60]{
    //...
};

GL::Buffer vertices;
vertices.setData(data, GL::BufferUsage::StaticDraw);

GL::Mesh mesh;
mesh.addVertexBuffer(vertices, 0, Shaders::FlatGL3D::Position{})
    // ...
    ;
/* [FlatGL-usage-colored1] */

/* [FlatGL-usage-colored2] */
Matrix4 transformationMatrix = Matrix4::translation(Vector3::zAxis(-5.0f));
Matrix4 projectionMatrix =
    Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.001f, 100.0f);

Shaders::FlatGL3D shader;
shader.setColor(0x2f83cc_rgbf)
    .setTransformationProjectionMatrix(projectionMatrix*transformationMatrix)
    .draw(mesh);
/* [FlatGL-usage-colored2] */
}

{
/* [FlatGL-usage-textured1] */
struct Vertex {
    Vector3 position;
    Vector2 textureCoordinates;
};
Vertex data[60]{
    // ...
};

GL::Buffer vertices;
vertices.setData(data, GL::BufferUsage::StaticDraw);

GL::Mesh mesh;
mesh.addVertexBuffer(vertices, 0,
    Shaders::FlatGL3D::Position{},
    Shaders::FlatGL3D::TextureCoordinates{})
    // ...
    ;
/* [FlatGL-usage-textured1] */

/* [FlatGL-usage-textured2] */
Matrix4 transformationMatrix, projectionMatrix;
GL::Texture2D texture;

Shaders::FlatGL3D shader{Shaders::FlatGL3D::Flag::Textured};
shader.setTransformationProjectionMatrix(projectionMatrix*transformationMatrix)
    .bindTexture(texture)
    .draw(mesh);
/* [FlatGL-usage-textured2] */
}
#endif

#ifndef MAGNUM_TARGET_GLES2
{
GL::Framebuffer framebuffer{{}};
GL::Mesh mesh;
Vector2i size;
UnsignedInt meshId{};
/* [FlatGL-usage-object-id] */
GL::Renderbuffer color, objectId;
color.setStorage(GL::RenderbufferFormat::RGBA8, size);
objectId.setStorage(GL::RenderbufferFormat::R16UI, size); // large as needed
framebuffer.attachRenderbuffer(GL::Framebuffer::ColorAttachment{0}, color)
    .attachRenderbuffer(GL::Framebuffer::ColorAttachment{1}, objectId);

Shaders::FlatGL3D shader{Shaders::FlatGL3D::Flag::ObjectId};

// ...

framebuffer.mapForDraw({
        {Shaders::FlatGL3D::ColorOutput, GL::Framebuffer::ColorAttachment{0}},
        {Shaders::FlatGL3D::ObjectIdOutput, GL::Framebuffer::ColorAttachment{1}}})
    .clearColor(0, 0x1f1f1f_rgbf)
    .clearColor(1, Vector4ui{0})
    .bind();

shader.setObjectId(meshId)
    .draw(mesh);
/* [FlatGL-usage-object-id] */

/* [shaders-generic-object-id] */
framebuffer.mapForDraw({
    {Shaders::GenericGL3D::ColorOutput, GL::Framebuffer::ColorAttachment{0}},
    {Shaders::GenericGL3D::ObjectIdOutput, GL::Framebuffer::ColorAttachment{1}}});
/* [shaders-generic-object-id] */
}
#endif

{
GL::Mesh mesh;
/* [FlatGL-usage-instancing] */
struct {
    Matrix4 transformation;
    Color3 color;
} instanceData[] {
    {Matrix4::translation({1.0f, 2.0f, 0.0f}), 0xff3333_rgbf},
    {Matrix4::translation({2.0f, 1.0f, 0.0f}), 0x33ff33_rgbf},
    {Matrix4::translation({3.0f, 0.0f, 1.0f}), 0x3333ff_rgbf},
    // ...
};

mesh.setInstanceCount(Containers::arraySize(instanceData))
    .addVertexBufferInstanced(GL::Buffer{instanceData}, 1, 0,
        Shaders::FlatGL3D::TransformationMatrix{},
        Shaders::FlatGL3D::Color3{});
/* [FlatGL-usage-instancing] */
}

#ifndef MAGNUM_TARGET_GLES2
{
GL::Mesh mesh;
Matrix4 transformationMatrix, projectionMatrix;
/* [FlatGL-ubo] */
GL::Buffer projectionTransformationUniform, materialUniform, drawUniform;
projectionTransformationUniform.setData({
    Shaders::TransformationProjectionUniform3D{}
        .setTransformationProjectionMatrix(transformationMatrix*projectionMatrix)
});
materialUniform.setData({
    Shaders::FlatMaterialUniform{}
        .setColor(0x2f83cc_rgbf)
});
drawUniform.setData({
    Shaders::FlatDrawUniform{}
        .setMaterialId(0)
});

Shaders::FlatGL3D shader{Shaders::FlatGL3D::Flag::UniformBuffers};
shader
    .bindTransformationProjectionBuffer(projectionTransformationUniform)
    .bindMaterialBuffer(materialUniform)
    .bindDrawBuffer(drawUniform)
    .draw(mesh);
/* [FlatGL-ubo] */
}
#endif

{
struct: GL::AbstractShaderProgram {
void foo() {
/* [GenericGL-custom-bind] */
bindAttributeLocation(Shaders::GenericGL3D::Position::Location, "position");
bindAttributeLocation(Shaders::GenericGL3D::Normal::Location, "normal");
/* [GenericGL-custom-bind] */
}
} shader;
}

{
GL::Shader vert{GL::Version::None, GL::Shader::Type::Vertex};
/* [GenericGL-custom-preprocessor] */
vert.addSource(Utility::formatString(
    "#define POSITION_ATTRIBUTE_LOCATION {}\n"
    "#define NORMAL_ATTRIBUTE_LOCATION {}\n",
    Shaders::GenericGL3D::Position::Location,
    Shaders::GenericGL3D::Normal::Location))
    // …
    .addFile("MyShader.vert");
/* [GenericGL-custom-preprocessor] */
}

{
GL::Mesh mesh;
/* [PhongGL-usage-instancing] */
struct {
    Matrix4 transformation;
    Matrix3x3 normal;
} instanceData[] {
    {Matrix4::translation({1.0f, 2.0f, 0.0f})*Matrix4::rotationX(90.0_degf), {}},
    {Matrix4::translation({2.0f, 1.0f, 0.0f})*Matrix4::rotationY(90.0_degf), {}},
    {Matrix4::translation({3.0f, 0.0f, 1.0f})*Matrix4::rotationZ(90.0_degf), {}},
    // ...
};
for(auto& instance: instanceData)
    instance.normal = instance.transformation.normalMatrix();

mesh.setInstanceCount(Containers::arraySize(instanceData))
    .addVertexBufferInstanced(GL::Buffer{instanceData}, 1, 0,
        Shaders::PhongGL::TransformationMatrix{},
        Shaders::PhongGL::NormalMatrix{});
/* [PhongGL-usage-instancing] */
}

/* internal compiler error: in gimplify_init_constructor, at gimplify.c:4271
   on GCC 4.8 in the [60] array */
#if !defined(__GNUC__) || defined(__clang__) || __GNUC__*100 + __GNUC_MINOR__ >= 500
{
/* [MeshVisualizerGL3D-usage-geom1] */
struct Vertex {
    Vector3 position;
};
Vertex data[60]{
    // ...
};

GL::Buffer vertices;
vertices.setData(data, GL::BufferUsage::StaticDraw);

GL::Mesh mesh;
mesh.addVertexBuffer(vertices, 0, Shaders::MeshVisualizerGL3D::Position{});
/* [MeshVisualizerGL3D-usage-geom1] */

/* [MeshVisualizerGL3D-usage-geom2] */
Matrix4 transformationMatrix = Matrix4::translation(Vector3::zAxis(-5.0f));
Matrix4 projectionMatrix =
    Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.001f, 100.0f);

Shaders::MeshVisualizerGL3D shader{Shaders::MeshVisualizerGL3D::Flag::Wireframe};
shader.setColor(0x2f83cc_rgbf)
    .setWireframeColor(0xdcdcdc_rgbf)
    .setViewportSize(Vector2{GL::defaultFramebuffer.viewport().size()})
    .setTransformationMatrix(transformationMatrix)
    .setProjectionMatrix(projectionMatrix)
    .draw(mesh);
/* [MeshVisualizerGL3D-usage-geom2] */

/* [MeshVisualizerGL3D-usage-no-geom-old] */
Containers::Array<Float> vertexIndex{Containers::arraySize(data)};
std::iota(vertexIndex.begin(), vertexIndex.end(), 0.0f);

GL::Buffer vertexIndices;
vertexIndices.setData(vertexIndex, GL::BufferUsage::StaticDraw);

mesh.addVertexBuffer(vertexIndices, 0, Shaders::MeshVisualizerGL3D::VertexIndex{});
/* [MeshVisualizerGL3D-usage-no-geom-old] */
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
/* internal compiler error: in gimplify_init_constructor, at gimplify.c:4271
   on GCC 4.8 in the [60] array */
#if !defined(__GNUC__) || defined(__clang__) || __GNUC__*100 + __GNUC_MINOR__ >= 500
{
/* [MeshVisualizerGL3D-usage-tbn1] */
struct Vertex {
    Vector3 position;
    Vector4 tangent;
    Vector3 normal;
};
Vertex data[60]{
    // ...
};

GL::Buffer vertices;
vertices.setData(data);

GL::Mesh mesh;
mesh.addVertexBuffer(vertices, 0,
    Shaders::MeshVisualizerGL3D::Position{},
    Shaders::MeshVisualizerGL3D::Tangent4{},
    Shaders::MeshVisualizerGL3D::Normal{});
/* [MeshVisualizerGL3D-usage-tbn1] */

/* [MeshVisualizerGL3D-usage-tbn2] */
Matrix4 transformationMatrix, projectionMatrix;

Shaders::MeshVisualizerGL3D shader{
    Shaders::MeshVisualizerGL3D::Flag::TangentDirection|
    Shaders::MeshVisualizerGL3D::Flag::BitangentFromTangentDirection|
    Shaders::MeshVisualizerGL3D::Flag::NormalDirection};
shader.setViewportSize(Vector2{GL::defaultFramebuffer.viewport().size()})
    .setTransformationMatrix(transformationMatrix)
    .setProjectionMatrix(projectionMatrix)
    .setNormalMatrix(transformationMatrix.normalMatrix())
    .setLineLength(0.3f)
    .draw(mesh);
/* [MeshVisualizerGL3D-usage-tbn2] */

}
#endif
#endif

{
/* [MeshVisualizerGL3D-usage-no-geom1] */
Containers::StridedArrayView1D<const UnsignedInt> indices;
Containers::StridedArrayView1D<const Vector3> indexedPositions;

/* De-indexing the position array */
GL::Buffer vertices{MeshTools::duplicate(indices, indexedPositions)};

GL::Mesh mesh;
mesh.addVertexBuffer(vertices, 0, Shaders::MeshVisualizerGL3D::Position{});
/* [MeshVisualizerGL3D-usage-no-geom1] */
}

{
GL::Mesh mesh;
/* [MeshVisualizerGL3D-usage-no-geom2] */
Matrix4 transformationMatrix, projectionMatrix;

Shaders::MeshVisualizerGL3D shader{
    Shaders::MeshVisualizerGL3D::Flag::Wireframe|
    Shaders::MeshVisualizerGL3D::Flag::NoGeometryShader};
shader.setColor(0x2f83cc_rgbf)
    .setWireframeColor(0xdcdcdc_rgbf)
    .setTransformationMatrix(transformationMatrix)
    .setProjectionMatrix(projectionMatrix)
    .draw(mesh);
/* [MeshVisualizerGL3D-usage-no-geom2] */
}

#ifndef MAGNUM_TARGET_GLES2
{
GL::Mesh mesh;
Containers::ArrayView<UnsignedInt> objectIds;
Matrix4 transformationMatrix, projectionMatrix;
/* [MeshVisualizerGL3D-usage-object-id] */
const auto map = DebugTools::ColorMap::turbo();
const Vector2i size{Int(map.size()), 1};

GL::Texture2D colorMapTexture;
colorMapTexture
    .setMinificationFilter(SamplerFilter::Linear)
    .setMagnificationFilter(SamplerFilter::Linear)
    .setWrapping(SamplerWrapping::ClampToEdge)
    .setStorage(1, GL::TextureFormat::RGBA8, size)
    .setSubImage(0, {}, ImageView2D{PixelFormat::RGB8Srgb, size, map});

Shaders::MeshVisualizerGL3D shader{
    Shaders::MeshVisualizerGL3D::Flag::InstancedObjectId};
shader.setColorMapTransformation(0.0f, 1.0f/Math::max(objectIds))
    .setTransformationMatrix(transformationMatrix)
    .setProjectionMatrix(projectionMatrix)
    .bindColorMapTexture(colorMapTexture)
    .draw(mesh);
/* [MeshVisualizerGL3D-usage-object-id] */
}
#endif

#ifndef MAGNUM_TARGET_GLES2
{
GL::Mesh mesh;
Matrix4 transformationMatrix, projectionMatrix;
GL::Texture2D texture;
/* [MeshVisualizerGL3D-ubo] */
GL::Buffer projectionUniform, materialUniform, transformationUniform,
    drawUniform;
projectionUniform.setData({
    Shaders::ProjectionUniform3D{}
        .setProjectionMatrix(projectionMatrix)
});
materialUniform.setData({
    Shaders::MeshVisualizerMaterialUniform{}
        .setColor(0x2f83cc_rgbf)
        .setWireframeColor(0xdcdcdc_rgbf)
});
transformationUniform.setData({
    Shaders::TransformationUniform3D{}
        .setTransformationMatrix(transformationMatrix)
});
drawUniform.setData({
    Shaders::MeshVisualizerDrawUniform3D{}
        .setMaterialId(0)
});

Shaders::MeshVisualizerGL3D shader{
    Shaders::MeshVisualizerGL3D::Flag::Wireframe|
    Shaders::MeshVisualizerGL3D::Flag::UniformBuffers
};
shader
    .setViewportSize(Vector2{GL::defaultFramebuffer.viewport().size()})
    .bindProjectionBuffer(projectionUniform)
    .bindMaterialBuffer(materialUniform)
    .bindTransformationBuffer(transformationUniform)
    .bindDrawBuffer(drawUniform)
    .draw(mesh);
/* [MeshVisualizerGL3D-ubo] */
}
#endif

#if !defined(__GNUC__) || defined(__clang__) || __GNUC__*100 + __GNUC_MINOR__ >= 500
{
/* [PhongGL-usage-colored1] */
struct Vertex {
    Vector3 position;
    Vector3 normal;
};
Vertex data[60]{
    // ...
};

GL::Buffer vertices;
vertices.setData(data, GL::BufferUsage::StaticDraw);

GL::Mesh mesh;
mesh.addVertexBuffer(vertices, 0,
    Shaders::PhongGL::Position{},
    Shaders::PhongGL::Normal{});
/* [PhongGL-usage-colored1] */

/* [PhongGL-usage-colored2] */
Matrix4 transformationMatrix = Matrix4::translation(Vector3::zAxis(-5.0f));
Matrix4 projectionMatrix =
    Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.001f, 100.0f);

Shaders::PhongGL shader;
shader.setDiffuseColor(0x2f83cc_rgbf)
    .setShininess(200.0f)
    .setTransformationMatrix(transformationMatrix)
    .setNormalMatrix(transformationMatrix.normalMatrix())
    .setProjectionMatrix(projectionMatrix)
    .draw(mesh);
/* [PhongGL-usage-colored2] */
}

{
/* [PhongGL-usage-texture1] */
struct Vertex {
    Vector3 position;
    Vector3 normal;
    Vector2 textureCoordinates;
};
Vertex data[60]{
    // ...
};

GL::Buffer vertices;
vertices.setData(data, GL::BufferUsage::StaticDraw);

GL::Mesh mesh;
mesh.addVertexBuffer(vertices, 0,
    Shaders::PhongGL::Position{},
    Shaders::PhongGL::Normal{},
    Shaders::PhongGL::TextureCoordinates{});
/* [PhongGL-usage-texture1] */

/* [PhongGL-usage-texture2] */
Matrix4 transformationMatrix, projectionMatrix;
GL::Texture2D diffuseTexture, specularTexture;

Shaders::PhongGL shader{Shaders::PhongGL::Flag::DiffuseTexture|
                      Shaders::PhongGL::Flag::SpecularTexture};
shader.bindTextures(nullptr, &diffuseTexture, &specularTexture, nullptr)
    .setTransformationMatrix(transformationMatrix)
    .setNormalMatrix(transformationMatrix.normalMatrix())
    .setProjectionMatrix(projectionMatrix)
    .draw(mesh);
/* [PhongGL-usage-texture2] */
}
#endif

{
/* [PhongGL-usage-lights] */
Matrix4 directionalLight, pointLight1, pointLight2; // camera-relative

Shaders::PhongGL shader{{}, 3}; // 3 lights
shader
    .setLightPositions({Vector4{directionalLight.up(), 0.0f},
                        Vector4{pointLight1.translation(), 1.0f},
                        Vector4{pointLight2.translation(), 1.0f}})
    .setLightColors({0xf0f0ff_srgbf*0.1f,
                     0xff8080_srgbf*10.0f,
                     0x80ff80_srgbf*10.0f})
    .setLightColors(DOXYGEN_IGNORE({0xf0f0ff_srgbf}))
    .setLightRanges({Constants::inf(),
                     2.0f,
                     2.0f});
/* [PhongGL-usage-lights] */
}

{
Color3 ambientColor;
GL::Texture2D diffuseTexture;
/* [PhongGL-usage-lights-ambient] */
Trade::LightData ambientLight = DOXYGEN_IGNORE(Trade::LightData{{}, {}, {}});

Shaders::PhongGL shader{Shaders::PhongGL::Flag::AmbientTexture|DOXYGEN_IGNORE(Shaders::PhongGL::Flag::DiffuseTexture), DOXYGEN_IGNORE(3)};
shader
    .setAmbientColor(ambientColor + ambientLight.color()*ambientLight.intensity())
    .bindAmbientTexture(diffuseTexture)
    .bindDiffuseTexture(diffuseTexture);
/* [PhongGL-usage-lights-ambient] */
}

{
GL::Texture2D diffuseAlphaTexture;
Color3 diffuseRgb, specularRgb;
/* [PhongGL-usage-alpha] */
Shaders::PhongGL shader{Shaders::PhongGL::Flag::AmbientTexture|
                      Shaders::PhongGL::Flag::DiffuseTexture};
shader.bindTextures(&diffuseAlphaTexture, &diffuseAlphaTexture, nullptr, nullptr)
    .setAmbientColor(0x000000ff_rgbaf)
    .setDiffuseColor(Color4{diffuseRgb, 0.0f})
    .setSpecularColor(Color4{specularRgb, 0.0f});
/* [PhongGL-usage-alpha] */
}

#ifndef MAGNUM_TARGET_GLES2
{
GL::Mesh mesh;
Matrix4 transformationMatrix, projectionMatrix;
GL::Texture2D texture;
/* [PhongGL-ubo] */
GL::Buffer projectionUniform, lightUniform, materialUniform,
    transformationUniform, drawUniform;
projectionUniform.setData({
    Shaders::ProjectionUniform3D{}
        .setProjectionMatrix(projectionMatrix)
});
lightUniform.setData({
    Shaders::PhongLightUniform{}
});
materialUniform.setData({
    Shaders::PhongMaterialUniform{}
        .setDiffuseColor(0x2f83cc_rgbf)
        .setShininess(200.0f)
});
transformationUniform.setData({
    Shaders::TransformationUniform3D{}
        .setTransformationMatrix(transformationMatrix)
});
drawUniform.setData({
    Shaders::PhongDrawUniform{}
        .setNormalMatrix(transformationMatrix.normalMatrix())
        .setMaterialId(0)
});

Shaders::PhongGL shader{Shaders::PhongGL::Flag::UniformBuffers};
shader
    .bindProjectionBuffer(projectionUniform)
    .bindLightBuffer(lightUniform)
    .bindMaterialBuffer(materialUniform)
    .bindTransformationBuffer(transformationUniform)
    .bindDrawBuffer(drawUniform)
    .draw(mesh);
/* [PhongGL-ubo] */
}
#endif

#if !defined(__GNUC__) || defined(__clang__) || __GNUC__*100 + __GNUC_MINOR__ >= 500
{
/* [VectorGL-usage1] */
struct Vertex {
    Vector2 position;
    Vector2 textureCoordinates;
};
Vertex data[60]{
    // ...
};

GL::Buffer vertices;
vertices.setData(data, GL::BufferUsage::StaticDraw);

GL::Mesh mesh;
mesh.addVertexBuffer(vertices, 0,
    Shaders::VectorGL2D::Position{},
    Shaders::VectorGL2D::TextureCoordinates{});
/* [VectorGL-usage1] */

/* [VectorGL-usage2] */
Matrix3 transformationMatrix, projectionMatrix;
GL::Texture2D texture;

Shaders::VectorGL2D shader;
shader.setColor(0x2f83cc_rgbf)
    .bindVectorTexture(texture)
    .setTransformationProjectionMatrix(projectionMatrix*transformationMatrix)
    .draw(mesh);
/* [VectorGL-usage2] */
}
#endif

#ifndef MAGNUM_TARGET_GLES2
{
GL::Mesh mesh;
Matrix3 transformationMatrix, projectionMatrix;
GL::Texture2D texture;
/* [VectorGL-ubo] */
GL::Buffer projectionTransformationUniform, materialUniform, drawUniform;
projectionTransformationUniform.setData({
    Shaders::TransformationProjectionUniform2D{}
        .setTransformationProjectionMatrix(transformationMatrix*projectionMatrix)
});
materialUniform.setData({
    Shaders::VectorMaterialUniform{}
        .setColor(0x2f83cc_rgbf)
});
drawUniform.setData({
    Shaders::VectorDrawUniform{}
        .setMaterialId(0)
});

Shaders::VectorGL2D shader{Shaders::VectorGL2D::Flag::UniformBuffers};
shader
    .bindTransformationProjectionBuffer(projectionTransformationUniform)
    .bindMaterialBuffer(materialUniform)
    .bindDrawBuffer(drawUniform)
    .bindVectorTexture(texture)
    .draw(mesh);
/* [VectorGL-ubo] */
}
#endif

#if !defined(__GNUC__) || defined(__clang__) || __GNUC__*100 + __GNUC_MINOR__ >= 500
{
/* [VertexColorGL-usage1] */
struct Vertex {
    Vector3 position;
    Color3 color;
};
Vertex data[60]{
    // ...
};

GL::Buffer vertices;
vertices.setData(data, GL::BufferUsage::StaticDraw);

GL::Mesh mesh;
mesh.addVertexBuffer(vertices, 0,
    Shaders::VertexColorGL3D::Position{},
    Shaders::VertexColorGL3D::Color3{});
/* [VertexColorGL-usage1] */

/* [VertexColorGL-usage2] */
Matrix4 transformationMatrix = Matrix4::translation(Vector3::zAxis(-5.0f));
Matrix4 projectionMatrix =
    Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.001f, 100.0f);

Shaders::VertexColorGL3D shader;
shader.setTransformationProjectionMatrix(projectionMatrix*transformationMatrix)
    .draw(mesh);
/* [VertexColorGL-usage2] */
}
#endif

#ifndef MAGNUM_TARGET_GLES2
{
GL::Mesh mesh;
Matrix4 transformationMatrix, projectionMatrix;
/* [VertexColorGL-ubo] */
GL::Buffer projectionTransformationUniform;
projectionTransformationUniform.setData({
    Shaders::TransformationProjectionUniform3D{}
        .setTransformationProjectionMatrix(transformationMatrix*projectionMatrix)
});

Shaders::VertexColorGL3D shader{Shaders::VertexColorGL3D::Flag::UniformBuffers};
shader
    .bindTransformationProjectionBuffer(projectionTransformationUniform)
    .draw(mesh);
/* [VertexColorGL-ubo] */
}
#endif

}
