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

#include <numeric> /* std::iota() */
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/ArrayViewStl.h>
#include <Corrade/Containers/Iterable.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Utility/Format.h>

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
#include "Magnum/GL/Renderer.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/GL/Version.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Math/FunctionsBatch.h"
#include "Magnum/MeshTools/Compile.h"
#include "Magnum/MeshTools/Duplicate.h"
#include "Magnum/MeshTools/GenerateLines.h"
#include "Magnum/Shaders/DistanceFieldVectorGL.h"
#include "Magnum/Shaders/FlatGL.h"
#include "Magnum/Shaders/MeshVisualizerGL.h"
#include "Magnum/Shaders/PhongGL.h"
#include "Magnum/Shaders/VectorGL.h"
#include "Magnum/Shaders/VertexColorGL.h"
#include "Magnum/Trade/LightData.h"
#include "Magnum/Trade/MeshData.h"
#include "Magnum/Trade/SkinData.h"

#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/TextureArray.h"
#include "Magnum/Primitives/Circle.h"
#include "Magnum/MeshTools/CompileLines.h"
#include "Magnum/Shaders/DistanceFieldVector.h"
#include "Magnum/Shaders/Flat.h"
#include "Magnum/Shaders/Generic.h"
#include "Magnum/Shaders/Line.h"
#include "Magnum/Shaders/LineGL.h"
#include "Magnum/Shaders/MeshVisualizer.h"
#include "Magnum/Shaders/Phong.h"
#include "Magnum/Shaders/Vector.h"
#endif

#define DOXYGEN_ELLIPSIS(...) __VA_ARGS__

using namespace Magnum;
using namespace Magnum::Math::Literals;

/* Make sure the name doesn't conflict with any other snippets to avoid linker
   warnings, unlike with `int main()` there now has to be a declaration to
   avoid -Wmisssing-prototypes */
void mainShadersGL();
void mainShadersGL() {
{
/* [shaders-setup] */
struct {
    Vector3 position;
    Vector3 normal;
    Vector2 textureCoordinates;
} vertices[]{
    DOXYGEN_ELLIPSIS({})
};

GL::Mesh mesh;
mesh.addVertexBuffer(GL::Buffer{vertices}, 0,
        Shaders::PhongGL::Position{},
        Shaders::PhongGL::Normal{},
        Shaders::PhongGL::TextureCoordinates{})
    .setCount(Containers::arraySize(vertices));
/* [shaders-setup] */
}

{
GL::Mesh mesh;
/* [shaders-classic] */
Matrix4 transformationMatrix{DOXYGEN_ELLIPSIS()}, projectionMatrix{DOXYGEN_ELLIPSIS()};

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

Shaders::PhongGL shader{Shaders::PhongGL::Configuration{}
    .setFlags(Shaders::PhongGL::Flag::UniformBuffers)};
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
GL::Mesh redCone{DOXYGEN_ELLIPSIS()}, yellowCube{DOXYGEN_ELLIPSIS()}, redSphere{DOXYGEN_ELLIPSIS()};
Matrix4 redConeTransformation{DOXYGEN_ELLIPSIS()},
    yellowCubeTransformation{DOXYGEN_ELLIPSIS()},
    redSphereTransformation{DOXYGEN_ELLIPSIS()};

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

Shaders::PhongGL shader{Shaders::PhongGL::Configuration{}
    .setFlags(Shaders::PhongGL::Flag::UniformBuffers)
    .setLightCount(1)
    .setMaterialCount(2)
    .setDrawCount(3)};
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
GL::MeshView redConeView{DOXYGEN_ELLIPSIS(mesh)}, yellowCubeView{DOXYGEN_ELLIPSIS(mesh)}, redSphereView{DOXYGEN_ELLIPSIS(mesh)};
DOXYGEN_ELLIPSIS()

Shaders::PhongGL shader{Shaders::PhongGL::Configuration{}
    .setFlags(Shaders::PhongGL::Flag::MultiDraw)
    .setLightCount(1)
    .setMaterialCount(2)
    .setDrawCount(3)};
shader
    DOXYGEN_ELLIPSIS()
    .draw({redConeView, yellowCubeView, redSphereView});
/* [shaders-multidraw] */
}
#endif

{
Matrix4 projectionMatrix;
/* [shaders-instancing] */
Matrix4 redSphereTransformation{DOXYGEN_ELLIPSIS()},
    yellowSphereTransformation{DOXYGEN_ELLIPSIS()},
    greenSphereTransformation{DOXYGEN_ELLIPSIS()};

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

GL::Mesh sphereInstanced;
sphereInstanced
    DOXYGEN_ELLIPSIS()
    .addVertexBufferInstanced(GL::Buffer{instanceData}, 1, 0,
        Shaders::PhongGL::TransformationMatrix{},
        Shaders::PhongGL::NormalMatrix{},
        Shaders::PhongGL::Color3{})
    .setInstanceCount(3);

Shaders::PhongGL shader{Shaders::PhongGL::Configuration{}
    .setFlags(Shaders::PhongGL::Flag::InstancedTransformation|
              Shaders::PhongGL::Flag::VertexColor)};
shader
    .setProjectionMatrix(projectionMatrix)
    DOXYGEN_ELLIPSIS()
    .draw(sphereInstanced);
/* [shaders-instancing] */
}

#ifndef MAGNUM_TARGET_GLES2
{
/* [shaders-skinning] */
/* Import and compile the mesh */
Trade::MeshData meshData = DOXYGEN_ELLIPSIS(Trade::MeshData{MeshPrimitive::Triangles, 0});
GL::Mesh mesh = MeshTools::compile(meshData);
Containers::Pair<UnsignedInt, UnsignedInt> meshPerVertexJointCount =
    MeshTools::compiledPerVertexJointCount(meshData);

/* Import the skin associated with the mesh */
Trade::SkinData3D skin = DOXYGEN_ELLIPSIS(Trade::SkinData3D{{}, {}});

/* Set up a skinned shader */
Shaders::PhongGL shader{Shaders::PhongGL::Configuration{}
    .setJointCount(skin.joints().size(), meshPerVertexJointCount.first(),
                                         meshPerVertexJointCount.second())};

DOXYGEN_ELLIPSIS()

/* Absolute transformations for all nodes in the scene, possibly animated */
Containers::Array<Matrix4> absoluteTransformations{DOXYGEN_ELLIPSIS()};
DOXYGEN_ELLIPSIS()

/* Gather joint transformations for this skin, upload and draw */
Containers::Array<Matrix4> jointTransformations{NoInit, skin.joints().size()};
for(std::size_t i = 0; i != jointTransformations.size(); ++i)
    jointTransformations[i] = absoluteTransformations[skin.joints()[i]]*
                              skin.inverseBindMatrices()[i];
shader
    .setJointMatrices(jointTransformations)
    DOXYGEN_ELLIPSIS()
    .draw(mesh);
/* [shaders-skinning] */
}

{
Matrix4 jointTransformations[2];
UnsignedInt maxSkinJointCount{};
Containers::Pair<UnsignedInt, UnsignedInt> meshPerVertexJointCount;
GL::Mesh mesh;
/* [shaders-skinning-dynamic] */
Shaders::PhongGL shader{Shaders::PhongGL::Configuration{}
    .setFlags(Shaders::PhongGL::Flag::DynamicPerVertexJointCount)
    .setJointCount(maxSkinJointCount, 4, 4)};

DOXYGEN_ELLIPSIS()

shader
    .setJointMatrices(jointTransformations)
    .setPerVertexJointCount(meshPerVertexJointCount.first(),
                            meshPerVertexJointCount.second())
    DOXYGEN_ELLIPSIS()
    .draw(mesh);
/* [shaders-skinning-dynamic] */
}
#endif

{
GL::Mesh mesh;
/* [shaders-textures] */
GL::Texture2D diffuseTexture;
DOXYGEN_ELLIPSIS()

Shaders::PhongGL shader{Shaders::PhongGL::Configuration{}
    .setFlags(Shaders::PhongGL::Flag::DiffuseTexture)};
shader.bindDiffuseTexture(diffuseTexture)
    DOXYGEN_ELLIPSIS()
    .draw(mesh);
/* [shaders-textures] */
}

#ifndef MAGNUM_TARGET_GLES2
{
GL::Mesh mesh;
GL::MeshView redConeView{DOXYGEN_ELLIPSIS(mesh)}, yellowCubeView{DOXYGEN_ELLIPSIS(mesh)}, redSphereView{DOXYGEN_ELLIPSIS(mesh)};
/* [shaders-texture-arrays] */
ImageView2D coneDiffuse{DOXYGEN_ELLIPSIS({}, {})}, cubeDiffuse{DOXYGEN_ELLIPSIS({}, {})}, sphereDiffuse{DOXYGEN_ELLIPSIS({}, {})};

GL::Texture2DArray diffuseTexture;
diffuseTexture
    DOXYGEN_ELLIPSIS()
    /* Assuming all images have the same format and size */
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

Shaders::PhongGL shader{Shaders::PhongGL::Configuration{}
    .setFlags(Shaders::PhongGL::Flag::MultiDraw|
              Shaders::PhongGL::Flag::DiffuseTexture|
              Shaders::PhongGL::Flag::TextureArrays)
    .setLightCount(1)
    .setMaterialCount(2)
    .setDrawCount(3)};
shader
    DOXYGEN_ELLIPSIS()
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
Shaders::MeshVisualizerGL3D shader{Shaders::MeshVisualizerGL3D::Configuration{}
    .setFlags(Shaders::MeshVisualizerGL3D::Flag::Wireframe)};
shader
    .setColor(0x2f83cc_rgbf)
    .setWireframeColor(0xdcdcdc_rgbf)
    .setViewportSize(Vector2{GL::defaultFramebuffer.viewport().size()})
    .setTransformationMatrix(transformationMatrix)
    .setProjectionMatrix(projectionMatrix)
    .draw(mesh);
/* [shaders-meshvisualizer] */
}

{
/* [shaders-async] */
Shaders::FlatGL3D::CompileState flatState =
    Shaders::FlatGL3D::compile();
Shaders::FlatGL3D::CompileState flatTexturedState =
    Shaders::FlatGL3D::compile(
        Shaders::FlatGL3D::Configuration{}
            .setFlags(Shaders::FlatGL3D::Flag::Textured));
Shaders::MeshVisualizerGL3D::CompileState meshVisualizerState =
    Shaders::MeshVisualizerGL3D::compile(DOXYGEN_ELLIPSIS(Shaders::MeshVisualizerGL3D::Configuration{}));

while(!flatState.isLinkFinished() ||
      !flatTexturedState.isLinkFinished() ||
      !meshVisualizerState.isLinkFinished()) {
    // Do other work ...
}

Shaders::FlatGL3D flat{std::move(flatState)};
Shaders::FlatGL3D flatTextured{std::move(flatTexturedState)};
Shaders::MeshVisualizerGL3D meshVisualizer{std::move(meshVisualizerState)};
/* [shaders-async] */
}

{
/* [DistanceFieldVectorGL-usage1] */
struct {
    Vector2 position;
    Vector2 textureCoordinates;
} vertices[]{
    DOXYGEN_ELLIPSIS({})
};

GL::Mesh mesh;
mesh.addVertexBuffer(GL::Buffer{vertices}, 0,
        Shaders::DistanceFieldVectorGL2D::Position{},
        Shaders::DistanceFieldVectorGL2D::TextureCoordinates{})
    .setCount(Containers::arraySize(vertices));
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

#ifndef MAGNUM_TARGET_GLES2
{
GL::Mesh mesh;
Matrix3 transformationMatrix, projectionMatrix;
GL::Texture2D texture;
/* [DistanceFieldVectorGL-ubo] */
GL::Buffer transformationProjectionUniform, materialUniform, drawUniform;
transformationProjectionUniform.setData({
    Shaders::TransformationProjectionUniform2D{}
        .setTransformationProjectionMatrix(projectionMatrix*transformationMatrix)
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
    Shaders::DistanceFieldVectorGL2D::Configuration{}
        .setFlags(Shaders::DistanceFieldVectorGL2D::Flag::UniformBuffers)};
shader
    .bindTransformationProjectionBuffer(transformationProjectionUniform)
    .bindMaterialBuffer(materialUniform)
    .bindDrawBuffer(drawUniform)
    .bindVectorTexture(texture)
    .draw(mesh);
/* [DistanceFieldVectorGL-ubo] */
}
#endif

{
/* [FlatGL-usage-colored1] */
struct {
    Vector3 position;
} vertices[]{
    DOXYGEN_ELLIPSIS({})
};

GL::Mesh mesh;
mesh.addVertexBuffer(GL::Buffer{vertices}, 0,
        Shaders::FlatGL3D::Position{})
    .setCount(Containers::arraySize(vertices));
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
struct {
    Vector3 position;
    Vector2 textureCoordinates;
} vertices[]{
    DOXYGEN_ELLIPSIS({})
};

GL::Mesh mesh;
mesh.addVertexBuffer(GL::Buffer{vertices}, 0,
        Shaders::FlatGL3D::Position{},
        Shaders::FlatGL3D::TextureCoordinates{})
    .setCount(Containers::arraySize(vertices));
/* [FlatGL-usage-textured1] */

/* [FlatGL-usage-textured2] */
Matrix4 transformationMatrix, projectionMatrix;
GL::Texture2D texture;

Shaders::FlatGL3D shader{Shaders::FlatGL3D::Configuration{}
    .setFlags(Shaders::FlatGL3D::Flag::Textured)};
shader.setTransformationProjectionMatrix(projectionMatrix*transformationMatrix)
    .bindTexture(texture)
    .draw(mesh);
/* [FlatGL-usage-textured2] */
}

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

Shaders::FlatGL3D shader{Shaders::FlatGL3D::Configuration{}
    .setFlags(Shaders::FlatGL3D::Flag::ObjectId)};

DOXYGEN_ELLIPSIS()

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
    DOXYGEN_ELLIPSIS()
};

mesh.addVertexBufferInstanced(GL::Buffer{instanceData}, 1, 0,
        Shaders::FlatGL3D::TransformationMatrix{},
        Shaders::FlatGL3D::Color3{})
    .setInstanceCount(Containers::arraySize(instanceData));
/* [FlatGL-usage-instancing] */
}

#ifndef MAGNUM_TARGET_GLES2
{
GL::Mesh mesh;
Matrix4 transformationMatrix, projectionMatrix;
/* [FlatGL-ubo] */
GL::Buffer transformationProjectionUniform, materialUniform, drawUniform;
transformationProjectionUniform.setData({
    Shaders::TransformationProjectionUniform3D{}
        .setTransformationProjectionMatrix(projectionMatrix*transformationMatrix)
});
materialUniform.setData({
    Shaders::FlatMaterialUniform{}
        .setColor(0x2f83cc_rgbf)
});
drawUniform.setData({
    Shaders::FlatDrawUniform{}
        .setMaterialId(0)
});

Shaders::FlatGL3D shader{Shaders::FlatGL3D::Configuration{}
    .setFlags(Shaders::FlatGL3D::Flag::UniformBuffers)};
shader
    .bindTransformationProjectionBuffer(transformationProjectionUniform)
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
vert.addSource(Utility::format(
    "#define POSITION_ATTRIBUTE_LOCATION {}\n"
    "#define NORMAL_ATTRIBUTE_LOCATION {}\n",
    Shaders::GenericGL3D::Position::Location,
    Shaders::GenericGL3D::Normal::Location))
    // …
    .addFile("MyShader.vert");
/* [GenericGL-custom-preprocessor] */
}

#ifndef MAGNUM_TARGET_GLES2
{
/* [LineGL-usage] */
Trade::MeshData circle = Primitives::circle2DWireframe(16);
GL::Mesh mesh = MeshTools::compileLines(MeshTools::generateLines(circle));
/* [LineGL-usage] */
}

{
GL::Mesh mesh;
Matrix3 transformationMatrix, projectionMatrix;
/* [LineGL-usage2] */
Shaders::LineGL2D shader;
shader
    .setViewportSize(Vector2{GL::defaultFramebuffer.viewport().size()})
    .setTransformationProjectionMatrix(projectionMatrix*transformationMatrix)
    .setColor(0x2f83cc_rgbf)
    .setWidth(4.0f)
    .draw(mesh);
/* [LineGL-usage2] */
}

{
GL::Mesh mesh;
/* [LineGL-usage-antialiasing] */
GL::Renderer::enable(GL::Renderer::Feature::Blending);
GL::Renderer::setBlendFunction(
    GL::Renderer::BlendFunction::One,
    GL::Renderer::BlendFunction::OneMinusSourceAlpha);

Shaders::LineGL2D shader;
shader
    DOXYGEN_ELLIPSIS()
    .setSmoothness(1.0f)
    .draw(mesh);
/* [LineGL-usage-antialiasing] */
}

{
GL::Mesh mesh;
Matrix3 transformationMatrix, projectionMatrix;
/* [LineGL-ubo] */
GL::Buffer transformationProjectionUniform, materialUniform, drawUniform;
transformationProjectionUniform.setData({
    Shaders::TransformationProjectionUniform2D{}
        .setTransformationProjectionMatrix(projectionMatrix*transformationMatrix)
});
materialUniform.setData({
    Shaders::LineMaterialUniform{}
        .setColor(0x2f83cc_rgbf)
});
drawUniform.setData({
    Shaders::LineDrawUniform{}
        .setMaterialId(0)
});

Shaders::LineGL2D shader{Shaders::LineGL2D::Configuration{}
    .setFlags(Shaders::LineGL2D::Flag::UniformBuffers)};
shader
    .setViewportSize(Vector2{GL::defaultFramebuffer.viewport().size()})
    .bindTransformationProjectionBuffer(transformationProjectionUniform)
    .bindMaterialBuffer(materialUniform)
    .bindDrawBuffer(drawUniform)
    .draw(mesh);
/* [LineGL-ubo] */
}
#endif

{
GL::Mesh mesh;
/* [MeshVisualizerGL2D-usage-instancing] */
Matrix3 instancedTransformations[]{
    Matrix3::translation({1.0f, 2.0f}),
    Matrix3::translation({2.0f, 1.0f}),
    Matrix3::translation({3.0f, 0.0f}),
    DOXYGEN_ELLIPSIS()
};

mesh.addVertexBufferInstanced(GL::Buffer{instancedTransformations}, 1, 0,
        Shaders::MeshVisualizerGL2D::TransformationMatrix{})
    .setInstanceCount(Containers::arraySize(instancedTransformations));
/* [MeshVisualizerGL2D-usage-instancing] */
}

{
/* [MeshVisualizerGL3D-usage-geom1] */
struct {
    Vector3 position;
} vertices[]{
    DOXYGEN_ELLIPSIS({})
};

GL::Mesh mesh;
mesh.addVertexBuffer(GL::Buffer{vertices}, 0,
        Shaders::MeshVisualizerGL3D::Position{})
    .setCount(Containers::arraySize(vertices));
/* [MeshVisualizerGL3D-usage-geom1] */

/* [MeshVisualizerGL3D-usage-geom2] */
Matrix4 transformationMatrix = Matrix4::translation(Vector3::zAxis(-5.0f));
Matrix4 projectionMatrix =
    Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.001f, 100.0f);

Shaders::MeshVisualizerGL3D shader{Shaders::MeshVisualizerGL3D::Configuration{}
    .setFlags(Shaders::MeshVisualizerGL3D::Flag::Wireframe)};
shader.setColor(0x2f83cc_rgbf)
    .setWireframeColor(0xdcdcdc_rgbf)
    .setViewportSize(Vector2{GL::defaultFramebuffer.viewport().size()})
    .setTransformationMatrix(transformationMatrix)
    .setProjectionMatrix(projectionMatrix)
    .draw(mesh);
/* [MeshVisualizerGL3D-usage-geom2] */

/* [MeshVisualizerGL3D-usage-no-geom-old] */
Containers::Array<Float> vertexIndex{Containers::arraySize(vertices)};
std::iota(vertexIndex.begin(), vertexIndex.end(), 0.0f);

GL::Buffer vertexIndices;
vertexIndices.setData(vertexIndex, GL::BufferUsage::StaticDraw);

mesh.addVertexBuffer(vertexIndices, 0, Shaders::MeshVisualizerGL3D::VertexIndex{});
/* [MeshVisualizerGL3D-usage-no-geom-old] */
}

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
{
/* [MeshVisualizerGL3D-usage-tbn1] */
struct {
    Vector3 position;
    Vector4 tangent;
    Vector3 normal;
} vertices[]{
    DOXYGEN_ELLIPSIS({})
};

GL::Mesh mesh;
mesh.addVertexBuffer(GL::Buffer{vertices}, 0,
        Shaders::MeshVisualizerGL3D::Position{},
        Shaders::MeshVisualizerGL3D::Tangent4{},
        Shaders::MeshVisualizerGL3D::Normal{})
    .setCount(Containers::arraySize(vertices));
/* [MeshVisualizerGL3D-usage-tbn1] */

/* [MeshVisualizerGL3D-usage-tbn2] */
Matrix4 transformationMatrix, projectionMatrix;

Shaders::MeshVisualizerGL3D shader{Shaders::MeshVisualizerGL3D::Configuration{}
    .setFlags(Shaders::MeshVisualizerGL3D::Flag::TangentDirection|
              Shaders::MeshVisualizerGL3D::Flag::BitangentFromTangentDirection|
              Shaders::MeshVisualizerGL3D::Flag::NormalDirection)};
shader.setViewportSize(Vector2{GL::defaultFramebuffer.viewport().size()})
    .setTransformationMatrix(transformationMatrix)
    .setProjectionMatrix(projectionMatrix)
    .setNormalMatrix(transformationMatrix.normalMatrix())
    .setLineLength(0.3f)
    .draw(mesh);
/* [MeshVisualizerGL3D-usage-tbn2] */
}
#endif

{
/* [MeshVisualizerGL3D-usage-no-geom1] */
Containers::StridedArrayView1D<const UnsignedInt> indices = DOXYGEN_ELLIPSIS({});
Containers::StridedArrayView1D<const Vector3> indexedPositions = DOXYGEN_ELLIPSIS({});

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

Shaders::MeshVisualizerGL3D shader{Shaders::MeshVisualizerGL3D::Configuration{}
    .setFlags(Shaders::MeshVisualizerGL3D::Flag::Wireframe|
              Shaders::MeshVisualizerGL3D::Flag::NoGeometryShader)};
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

Shaders::MeshVisualizerGL3D shader{Shaders::MeshVisualizerGL3D::Configuration{}
    .setFlags(Shaders::MeshVisualizerGL3D::Flag::InstancedObjectId)};
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

Shaders::MeshVisualizerGL3D shader{Shaders::MeshVisualizerGL3D::Configuration{}
    .setFlags(Shaders::MeshVisualizerGL3D::Flag::Wireframe|
              Shaders::MeshVisualizerGL3D::Flag::UniformBuffers)
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

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
{
GL::Mesh mesh;
/* [MeshVisualizerGL3D-usage-instancing] */
struct {
    Matrix4 transformation;
    Matrix3x3 normal;
} instanceData[]{
    {Matrix4::translation({1.0f, 2.0f, 0.0f}), {}},
    {Matrix4::translation({2.0f, 1.0f, 0.0f}), {}},
    {Matrix4::translation({3.0f, 0.0f, 1.0f}), {}},
    DOXYGEN_ELLIPSIS()
};
for(auto& instance: instanceData)
    instance.normal = instance.transformation.normalMatrix();

mesh.addVertexBufferInstanced(GL::Buffer{instanceData}, 1, 0,
        Shaders::MeshVisualizerGL3D::TransformationMatrix{},
        Shaders::MeshVisualizerGL3D::NormalMatrix{})
    .setInstanceCount(Containers::arraySize(instanceData));
/* [MeshVisualizerGL3D-usage-instancing] */
}
#endif

{
/* [PhongGL-usage-colored1] */
struct {
    Vector3 position;
    Vector3 normal;
} vertices[]{
    DOXYGEN_ELLIPSIS({})
};

GL::Mesh mesh;
mesh.addVertexBuffer(GL::Buffer{vertices}, 0,
        Shaders::PhongGL::Position{},
        Shaders::PhongGL::Normal{})
    .setCount(Containers::arraySize(vertices));
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
struct {
    Vector3 position;
    Vector3 normal;
    Vector2 textureCoordinates;
} vertices[]{
    DOXYGEN_ELLIPSIS({})
};

GL::Mesh mesh;
mesh.addVertexBuffer(GL::Buffer{vertices}, 0,
        Shaders::PhongGL::Position{},
        Shaders::PhongGL::Normal{},
        Shaders::PhongGL::TextureCoordinates{})
    .setCount(Containers::arraySize(vertices));
/* [PhongGL-usage-texture1] */

/* [PhongGL-usage-texture2] */
Matrix4 transformationMatrix, projectionMatrix;
GL::Texture2D diffuseTexture, specularTexture;

Shaders::PhongGL shader{Shaders::PhongGL::Configuration{}
    .setFlags(Shaders::PhongGL::Flag::DiffuseTexture|
              Shaders::PhongGL::Flag::SpecularTexture)};
shader.bindTextures(nullptr, &diffuseTexture, &specularTexture, nullptr)
    .setTransformationMatrix(transformationMatrix)
    .setNormalMatrix(transformationMatrix.normalMatrix())
    .setProjectionMatrix(projectionMatrix)
    .draw(mesh);
/* [PhongGL-usage-texture2] */
}

{
/* [PhongGL-usage-lights] */
Matrix4 directionalLight, pointLight1, pointLight2; // camera-relative

Shaders::PhongGL shader{Shaders::PhongGL::Configuration{}
    .setLightCount(3)};
shader
    .setLightPositions({Vector4{directionalLight.up(), 0.0f},
                        Vector4{pointLight1.translation(), 1.0f},
                        Vector4{pointLight2.translation(), 1.0f}})
    .setLightColors({0xf0f0ff_srgbf*0.1f,
                     0xff8080_srgbf*10.0f,
                     0x80ff80_srgbf*10.0f})
    .setLightColors(DOXYGEN_ELLIPSIS({0xf0f0ff_srgbf}))
    .setLightRanges({Constants::inf(),
                     2.0f,
                     2.0f});
/* [PhongGL-usage-lights] */
}

{
Color3 ambientColor;
GL::Texture2D diffuseTexture;
/* [PhongGL-usage-lights-ambient] */
Trade::LightData ambientLight = DOXYGEN_ELLIPSIS(Trade::LightData{{}, {}, {}});

Shaders::PhongGL shader{Shaders::PhongGL::Configuration{}
    .setFlags(Shaders::PhongGL::Flag::AmbientTexture|DOXYGEN_ELLIPSIS(Shaders::PhongGL::Flag::DiffuseTexture))};
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
Shaders::PhongGL shader{Shaders::PhongGL::Configuration{}
    .setFlags(Shaders::PhongGL::Flag::AmbientTexture|
              Shaders::PhongGL::Flag::DiffuseTexture)};
shader.bindTextures(&diffuseAlphaTexture, &diffuseAlphaTexture, nullptr, nullptr)
    .setAmbientColor(0x000000ff_rgbaf)
    .setDiffuseColor(Color4{diffuseRgb, 0.0f})
    .setSpecularColor(Color4{specularRgb, 0.0f});
/* [PhongGL-usage-alpha] */
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
    DOXYGEN_ELLIPSIS()
};
for(auto& instance: instanceData)
    instance.normal = instance.transformation.normalMatrix();

mesh.setInstanceCount(Containers::arraySize(instanceData))
    .addVertexBufferInstanced(GL::Buffer{instanceData}, 1, 0,
        Shaders::PhongGL::TransformationMatrix{},
        Shaders::PhongGL::NormalMatrix{});
/* [PhongGL-usage-instancing] */
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

Shaders::PhongGL shader{Shaders::PhongGL::Configuration{}
    .setFlags(Shaders::PhongGL::Flag::UniformBuffers)};
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

#if !defined(CORRADE_TARGET_GCC) || defined(CORRADE_TARGET_CLANG) || __GNUC__ >= 5
{
/* [VectorGL-usage1] */
struct {
    Vector2 position;
    Vector2 textureCoordinates;
} vertices[]{
    DOXYGEN_ELLIPSIS({})
};

GL::Mesh mesh;
mesh.addVertexBuffer(GL::Buffer{vertices}, 0,
        Shaders::VectorGL2D::Position{},
        Shaders::VectorGL2D::TextureCoordinates{})
    .setCount(Containers::arraySize(vertices));
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
GL::Buffer transformationProjectionUniform, materialUniform, drawUniform;
transformationProjectionUniform.setData({
    Shaders::TransformationProjectionUniform2D{}
        .setTransformationProjectionMatrix(projectionMatrix*transformationMatrix)
});
materialUniform.setData({
    Shaders::VectorMaterialUniform{}
        .setColor(0x2f83cc_rgbf)
});
drawUniform.setData({
    Shaders::VectorDrawUniform{}
        .setMaterialId(0)
});

Shaders::VectorGL2D shader{Shaders::VectorGL2D::Configuration{}
    .setFlags(Shaders::VectorGL2D::Flag::UniformBuffers)};
shader
    .bindTransformationProjectionBuffer(transformationProjectionUniform)
    .bindMaterialBuffer(materialUniform)
    .bindDrawBuffer(drawUniform)
    .bindVectorTexture(texture)
    .draw(mesh);
/* [VectorGL-ubo] */
}
#endif

#if !defined(CORRADE_TARGET_GCC) || defined(CORRADE_TARGET_CLANG) || __GNUC__ >= 5
{
/* [VertexColorGL-usage1] */
struct {
    Vector3 position;
    Color3 color;
} vertices[]{
    DOXYGEN_ELLIPSIS({})
};

GL::Mesh mesh;
mesh.addVertexBuffer(GL::Buffer{vertices}, 0,
        Shaders::VertexColorGL3D::Position{},
        Shaders::VertexColorGL3D::Color3{})
    .setCount(Containers::arraySize(vertices));
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
GL::Buffer transformationProjectionUniform;
transformationProjectionUniform.setData({
    Shaders::TransformationProjectionUniform3D{}
        .setTransformationProjectionMatrix(projectionMatrix*transformationMatrix)
});

Shaders::VertexColorGL3D shader{Shaders::VertexColorGL3D::Configuration{}
    .setFlags(Shaders::VertexColorGL3D::Flag::UniformBuffers)};
shader
    .bindTransformationProjectionBuffer(transformationProjectionUniform)
    .draw(mesh);
/* [VertexColorGL-ubo] */
}
#endif

}
