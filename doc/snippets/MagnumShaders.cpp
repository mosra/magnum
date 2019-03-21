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

#include <numeric>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/ArrayViewStl.h>

#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/DefaultFramebuffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/Math/Color.h"
#include "Magnum/MeshTools/Duplicate.h"
#include "Magnum/Shaders/DistanceFieldVector.h"
#include "Magnum/Shaders/Flat.h"
#include "Magnum/Shaders/MeshVisualizer.h"
#include "Magnum/Shaders/Phong.h"
#include "Magnum/Shaders/Vector.h"
#include "Magnum/Shaders/VertexColor.h"

using namespace Magnum;
using namespace Magnum::Math::Literals;

int main() {

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
    Shaders::Phong::Position{},
    Shaders::Phong::Normal{},
    Shaders::Phong::TextureCoordinates{})
     //...
     ;
/* [shaders-setup] */

/* [shaders-rendering] */
Matrix4 transformationMatrix, projectionMatrix;
GL::Texture2D diffuseTexture, specularTexture;

Shaders::Phong shader{Shaders::Phong::Flag::DiffuseTexture};
shader.bindDiffuseTexture(diffuseTexture)
    .setLightPosition({5.0f, 5.0f, 7.0f})
    .setTransformationMatrix(transformationMatrix)
    .setNormalMatrix(transformationMatrix.rotation())
    .setProjectionMatrix(projectionMatrix);

mesh.draw(shader);
/* [shaders-rendering] */

/* [shaders-generic] */
mesh.addVertexBuffer(vertices, 0,
    Shaders::Generic3D::Position{},
    Shaders::Generic3D::Normal{},
    Shaders::Generic3D::TextureCoordinates{});
/* [shaders-generic] */

/* [shaders-meshvisualizer] */
Shaders::MeshVisualizer visualizerShader{Shaders::MeshVisualizer::Flag::Wireframe};
visualizerShader
    .setColor(0x2f83cc_rgbf)
    .setWireframeColor(0xdcdcdc_rgbf)
    .setViewportSize(Vector2{GL::defaultFramebuffer.viewport().size()})
    .setTransformationProjectionMatrix(projectionMatrix*transformationMatrix);

mesh.draw(visualizerShader);
/* [shaders-meshvisualizer] */
}

{
/* [DistanceFieldVector-usage1] */
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
    Shaders::DistanceFieldVector2D::Position{},
    Shaders::DistanceFieldVector2D::TextureCoordinates{})
    // ...
    ;
/* [DistanceFieldVector-usage1] */
}

{
GL::Mesh mesh;
/* [DistanceFieldVector-usage2] */
Matrix3 transformationMatrix, projectionMatrix;
GL::Texture2D texture;

Shaders::DistanceFieldVector2D shader;
shader.setColor(0x2f83cc_rgbf)
    .setOutlineColor(0xdcdcdc_rgbf)
    .setOutlineRange(0.6f, 0.4f)
    .bindVectorTexture(texture)
    .setTransformationProjectionMatrix(projectionMatrix*transformationMatrix);

mesh.draw(shader);
/* [DistanceFieldVector-usage2] */
}

{
/* [Flat-usage-colored1] */
struct Vertex {
    Vector3 position;
};
Vertex data[60]{
    //...
};

GL::Buffer vertices;
vertices.setData(data, GL::BufferUsage::StaticDraw);

GL::Mesh mesh;
mesh.addVertexBuffer(vertices, 0, Shaders::Flat3D::Position{})
    // ...
    ;
/* [Flat-usage-colored1] */

/* [Flat-usage-colored2] */
Matrix4 transformationMatrix = Matrix4::translation(Vector3::zAxis(-5.0f));
Matrix4 projectionMatrix = Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.001f, 100.0f);

Shaders::Flat3D shader;
shader.setColor(0x2f83cc_rgbf)
    .setTransformationProjectionMatrix(projectionMatrix*transformationMatrix);

mesh.draw(shader);
/* [Flat-usage-colored2] */
}

{
/* [Flat-usage-textured1] */
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
    Shaders::Flat3D::Position{},
    Shaders::Flat3D::TextureCoordinates{})
    // ...
    ;
/* [Flat-usage-textured1] */

/* [Flat-usage-textured2] */
Matrix4 transformationMatrix, projectionMatrix;
GL::Texture2D texture;

Shaders::Flat3D shader{Shaders::Flat3D::Flag::Textured};
shader.setTransformationProjectionMatrix(projectionMatrix*transformationMatrix)
    .bindTexture(texture);

mesh.draw(shader);
/* [Flat-usage-textured2] */
}

{
/* [MeshVisualizer-usage-geom1] */
struct Vertex {
    Vector3 position;
};
Vertex data[60]{
    // ...
};

GL::Buffer vertices;
vertices.setData(data, GL::BufferUsage::StaticDraw);

GL::Mesh mesh;
mesh.addVertexBuffer(vertices, 0, Shaders::MeshVisualizer::Position{});
/* [MeshVisualizer-usage-geom1] */

/* [MeshVisualizer-usage-geom2] */
Matrix4 transformationMatrix = Matrix4::translation(Vector3::zAxis(-5.0f));
Matrix4 projectionMatrix = Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.001f, 100.0f);

Shaders::MeshVisualizer shader{Shaders::MeshVisualizer::Flag::Wireframe};
shader.setColor(0x2f83cc_rgbf)
    .setWireframeColor(0xdcdcdc_rgbf)
    .setViewportSize(Vector2{GL::defaultFramebuffer.viewport().size()})
    .setTransformationProjectionMatrix(projectionMatrix*transformationMatrix);

mesh.draw(shader);
/* [MeshVisualizer-usage-geom2] */

/* [MeshVisualizer-usage-no-geom-old1] */
Containers::Array<Float> vertexIndex{Containers::arraySize(data)};
std::iota(vertexIndex.begin(), vertexIndex.end(), 0.0f);

GL::Buffer vertexIndices;
vertexIndices.setData(vertexIndex, GL::BufferUsage::StaticDraw);

mesh.addVertexBuffer(vertexIndices, 0, Shaders::MeshVisualizer::VertexIndex{});
/* [MeshVisualizer-usage-no-geom-old1] */
}
#endif

{
GL::Mesh mesh;
/* [MeshVisualizer-usage-no-geom-old2] */
Matrix4 transformationMatrix, projectionMatrix;

Shaders::MeshVisualizer shader{Shaders::MeshVisualizer::Flag::Wireframe|
                               Shaders::MeshVisualizer::Flag::NoGeometryShader};
shader.setColor(0x2f83cc_rgbf)
    .setWireframeColor(0xdcdcdc_rgbf)
    .setTransformationProjectionMatrix(projectionMatrix*transformationMatrix);

mesh.draw(shader);
/* [MeshVisualizer-usage-no-geom-old2] */
}

{
/* [MeshVisualizer-usage-no-geom] */
std::vector<UnsignedInt> indices{
    // ...
};
std::vector<Vector3> indexedPositions{
    // ...
};

/* De-indexing the position array */
GL::Buffer vertices;
vertices.setData(MeshTools::duplicate(indices, indexedPositions),
                 GL::BufferUsage::StaticDraw);

GL::Mesh mesh;
mesh.addVertexBuffer(vertices, 0, Shaders::MeshVisualizer::Position{});
/* [MeshVisualizer-usage-no-geom] */
}

#if !defined(__GNUC__) || defined(__clang__) || __GNUC__*100 + __GNUC_MINOR__ >= 500
{
/* [Phong-usage-colored1] */
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
    Shaders::Phong::Position{},
    Shaders::Phong::Normal{});
/* [Phong-usage-colored1] */

/* [Phong-usage-colored2] */
Matrix4 transformationMatrix = Matrix4::translation(Vector3::zAxis(-5.0f));
Matrix4 projectionMatrix = Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.001f, 100.0f);

Shaders::Phong shader;
shader.setDiffuseColor(0x2f83cc_rgbf)
    .setShininess(200.0f)
    .setLightPosition({5.0f, 5.0f, 7.0f})
    .setTransformationMatrix(transformationMatrix)
    .setNormalMatrix(transformationMatrix.rotation())
    .setProjectionMatrix(projectionMatrix);

mesh.draw(shader);
/* [Phong-usage-colored2] */
}

{
/* [Phong-usage-texture1] */
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
    Shaders::Phong::Position{},
    Shaders::Phong::Normal{},
    Shaders::Phong::TextureCoordinates{});
/* [Phong-usage-texture1] */

/* [Phong-usage-texture2] */
Matrix4 transformationMatrix, projectionMatrix;
GL::Texture2D diffuseTexture, specularTexture;

Shaders::Phong shader{Shaders::Phong::Flag::DiffuseTexture|
                      Shaders::Phong::Flag::SpecularTexture};
shader.bindTextures(nullptr, &diffuseTexture, &specularTexture)
    .setLightPosition({5.0f, 5.0f, 7.0f})
    .setTransformationMatrix(transformationMatrix)
    .setNormalMatrix(transformationMatrix.rotation())
    .setProjectionMatrix(projectionMatrix);

mesh.draw(shader);
/* [Phong-usage-texture2] */
}
#endif

{
GL::Texture2D diffuseAlphaTexture;
Color3 diffuseRgb, specularRgb;
/* [Phong-usage-alpha] */
Shaders::Phong shader{Shaders::Phong::Flag::AmbientTexture|
                      Shaders::Phong::Flag::DiffuseTexture};
shader.bindTextures(&diffuseAlphaTexture, &diffuseAlphaTexture, nullptr)
    .setAmbientColor(0x000000ff_rgbaf)
    .setDiffuseColor(Color4{diffuseRgb, 0.0f})
    .setSpecularColor(Color4{specularRgb, 0.0f});
/* [Phong-usage-alpha] */
}

#if !defined(__GNUC__) || defined(__clang__) || __GNUC__*100 + __GNUC_MINOR__ >= 500
{
/* [Vector-usage1] */
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
    Shaders::Vector2D::Position{},
    Shaders::Vector2D::TextureCoordinates{});
/* [Vector-usage1] */

/* [Vector-usage2] */
Matrix3 transformationMatrix, projectionMatrix;
GL::Texture2D texture;

Shaders::Vector2D shader;
shader.setColor(0x2f83cc_rgbf)
    .bindVectorTexture(texture)
    .setTransformationProjectionMatrix(projectionMatrix*transformationMatrix);

mesh.draw(shader);
/* [Vector-usage2] */
}

{
/* [VertexColor-usage1] */
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
    Shaders::VertexColor3D::Position{},
    Shaders::VertexColor3D::Color3{});
/* [VertexColor-usage1] */

/* [VertexColor-usage2] */
Matrix4 transformationMatrix = Matrix4::translation(Vector3::zAxis(-5.0f));
Matrix4 projectionMatrix = Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.001f, 100.0f);

Shaders::VertexColor3D shader;
shader.setTransformationProjectionMatrix(projectionMatrix*transformationMatrix);

mesh.draw(shader);
/* [VertexColor-usage2] */
}
#endif

}
