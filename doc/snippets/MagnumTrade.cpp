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

#include <unordered_map>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/Utility/Resource.h>

#include "Magnum/FileCallback.h"
#include "Magnum/ImageView.h"
#include "Magnum/Mesh.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/MeshTools/Interleave.h"
#include "Magnum/Animation/Player.h"
#include "Magnum/MeshTools/Transform.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/AnimationData.h"
#include "Magnum/Trade/ImageData.h"
#include "Magnum/Trade/MeshData.h"
#include "Magnum/Trade/MeshData2D.h"
#include "Magnum/Trade/MeshData3D.h"
#include "Magnum/Trade/ObjectData2D.h"
#include "Magnum/Trade/ObjectData3D.h"
#include "Magnum/Trade/PhongMaterialData.h"
#ifdef MAGNUM_TARGET_GL
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/Shaders/Phong.h"
#endif

using namespace Magnum;
using namespace Magnum::Math::Literals;

int main() {

{
/* [AbstractImporter-usage] */
PluginManager::Manager<Trade::AbstractImporter> manager;
Containers::Pointer<Trade::AbstractImporter> importer =
    manager.loadAndInstantiate("AnyImageImporter");
if(!importer || !importer->openFile("image.png"))
    Fatal{} << "Can't open image.png with AnyImageImporter";

Containers::Optional<Trade::ImageData2D> image = importer->image2D(0);
if(!image) Fatal{} << "Importing the image failed";

// use the image ...
/* [AbstractImporter-usage] */
}

#if defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT))
{
Containers::Pointer<Trade::AbstractImporter> importer;
/* [AbstractImporter-usage-callbacks] */
struct Data {
    std::unordered_map<std::string,
        Containers::Array<const char, Utility::Directory::MapDeleter>> files;
} data;

importer->setFileCallback([](const std::string& filename,
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

importer->openFile("scene.gltf"); // memory-maps all files
/* [AbstractImporter-usage-callbacks] */
}
#endif

{
Containers::Pointer<Trade::AbstractImporter> importer;
Float shininess;
/* [AbstractImporter-usage-cast] */
Containers::Pointer<Trade::AbstractMaterialData> data = importer->material(12);
if(data && data->type() == Trade::MaterialType::Phong) {
    auto& phong = static_cast<Trade::PhongMaterialData&>(*data);

    shininess = phong.shininess();
    // ...
}
/* [AbstractImporter-usage-cast] */
static_cast<void>(shininess);
}

{
Containers::Pointer<Trade::AbstractImporter> importer;
/* [AbstractImporter-setFileCallback] */
importer->setFileCallback([](const std::string& filename,
    InputFileCallbackPolicy, void*) {
        Utility::Resource rs("data");
        return Containers::optional(rs.getRaw(filename));
    });
/* [AbstractImporter-setFileCallback] */
}

{
Containers::Pointer<Trade::AbstractImporter> importer;
/* [AbstractImporter-setFileCallback-template] */
struct Data {
    std::unordered_map<std::string, Containers::Array<char>> files;
} data;

importer->setFileCallback([](const std::string& filename,
    InputFileCallbackPolicy, Data& data)
        -> Containers::Optional<Containers::ArrayView<const char>>
    {
        auto found = data.files.find(filename);
        if(found == data.files.end()) {
            if(!Utility::Directory::exists(filename))
                return Containers::NullOpt;
            found = data.files.emplace(filename, Utility::Directory::read(filename)).first;
        }
        return Containers::ArrayView<const char>{found->second};
    }, data);
/* [AbstractImporter-setFileCallback-template] */
}

{
UnsignedInt id{};
Containers::Pointer<Trade::AbstractImporter> importer;
/* [AnimationData-usage] */

Containers::Optional<Trade::AnimationData> data = importer->animation(id);

Animation::Player<Float> player;
Containers::Array<Vector3> positions; /* Translations for all objects */
for(UnsignedInt i = 0; i != data->trackCount(); ++i) {
    if(data->trackTargetType(i) == Trade::AnimationTrackTargetType::Translation3D) {
        CORRADE_INTERNAL_ASSERT(data->trackType(i) ==
            Trade::AnimationTrackType::Vector3);
        player.add(data->track<Vector3>(i), positions[data->trackTarget(i)]);
    }

    // similarly for rotation / scaling ...
}

Containers::Array<char> animationData = data->release(); /* Take ownership */
/* [AnimationData-usage] */
}

{
/* [ImageData-construction] */
Containers::Array<char> data;
Trade::ImageData2D image{PixelFormat::RGB8Unorm, {32, 32}, std::move(data)};
/* [ImageData-construction] */
}

{
/* [ImageData-construction-compressed] */
Containers::Array<char> data;
Trade::ImageData2D image{CompressedPixelFormat::Bc1RGBUnorm,
    {32, 32}, std::move(data)};
/* [ImageData-construction-compressed] */
}

#ifdef MAGNUM_TARGET_GL
{
/* [ImageData-usage] */
Containers::Pointer<Trade::AbstractImporter> importer;
Containers::Optional<Trade::ImageData2D> image = importer->image2D(0);
if(!image) Fatal{} << "Oopsie!";

GL::Texture2D texture;
// ...
if(!image->isCompressed())
    texture.setSubImage(0, {}, *image);
else
    texture.setCompressedSubImage(0, {}, *image);
/* [ImageData-usage] */
}
#endif

#ifdef MAGNUM_TARGET_GL
{
Trade::MeshData data{MeshPrimitive::Points, 0};
/* [MeshData-usage] */
/* Check that we have at least positions and normals */
GL::Mesh mesh{data.primitive()};
if(!data.hasAttribute(Trade::MeshAttribute::Position) ||
   !data.hasAttribute(Trade::MeshAttribute::Normal))
    Fatal{} << "Oh well";

/* Interleave vertex data */
GL::Buffer vertices;
vertices.setData(MeshTools::interleave(data.positions3DAsArray(),
                                       data.normalsAsArray()));
mesh.addVertexBuffer(std::move(vertices), 0,
    Shaders::Phong::Position{}, Shaders::Phong::Normal{});

/* Set up an index buffer, if the mesh is indexed*/
if(data.isIndexed()) {
    GL::Buffer indices;
    indices.setData(data.indicesAsArray());
    mesh.setIndexBuffer(std::move(indices), 0, MeshIndexType::UnsignedInt)
        .setCount(data.indexCount());
} else mesh.setCount(data.vertexCount());
/* [MeshData-usage] */
}

{
Trade::MeshData data{MeshPrimitive::Points, 0};
GL::Mesh mesh{data.primitive()};
/* [MeshData-usage-advanced] */
/* Upload the original packed vertex data */
GL::Buffer vertices;
vertices.setData(data.vertexData());

/* Set up the position attribute */
Shaders::Phong::Position position;
auto positionFormat = data.attributeFormat(Trade::MeshAttribute::Position);
if(positionFormat == VertexFormat::Vector2)
    position = {Shaders::Phong::Position::Components::Two};
else if(positionFormat == VertexFormat::Vector3)
    position = {Shaders::Phong::Position::Components::Three};
else Fatal{} << "Huh?";
mesh.addVertexBuffer(vertices,
    data.attributeOffset(Trade::MeshAttribute::Position),
    data.attributeStride(Trade::MeshAttribute::Position), position);

// Set up other attributes ...

/* Upload the original packed index data */
if(data.isIndexed()) {
    GL::Buffer indices;
    indices.setData(data.indexData());
    mesh.setIndexBuffer(std::move(indices), 0, data.indexType())
        .setCount(data.indexCount());
} else mesh.setCount(data.vertexCount());
/* [MeshData-usage-advanced] */
}
#endif

{
Trade::MeshData data{MeshPrimitive::Points, 0};
/* [MeshData-usage-mutable] */
/* Check prerequisites */
if(!(data.vertexDataFlags() & Trade::DataFlag::Mutable) ||
   !data.hasAttribute(Trade::MeshAttribute::Position) ||
   data.attributeFormat(Trade::MeshAttribute::Position) != VertexFormat::Vector3)
    Fatal{} << "Oh well";

/* Scale the mesh two times */
MeshTools::transformPointsInPlace(Matrix4::scaling(Vector3{2.0f}),
    data.mutableAttribute<Vector3>(Trade::MeshAttribute::Position));
/* [MeshData-usage-mutable] */
}

{
Trade::MeshData2D& foo();
Trade::MeshData2D& data = foo();
/* [MeshData2D-transform] */
Matrix3 transformation =
    Matrix3::translation({3.0f, -2.0f})*
    Matrix3::scaling(Vector2{2.0f})*
    Matrix3::rotation(45.0_degf);
MeshTools::transformPointsInPlace(transformation, data.positions(0));
/* [MeshData2D-transform] */
}

{
Trade::ObjectData2D& baz();
Trade::ObjectData2D& data = baz();
/* [ObjectData2D-transformation] */
Matrix3 transformation =
    Matrix3::from(data.rotation().toMatrix(), data.translation())*
    Matrix3::scaling(data.scaling());
/* [ObjectData2D-transformation] */
static_cast<void>(transformation);
}

{
Trade::MeshData3D& bar();
Trade::MeshData3D& data = bar();
/* [MeshData3D-transform] */
Matrix4 transformation =
    Matrix4::translation({3.0f, 1.5f, -2.0f})*
    Matrix4::rotationX(45.0_degf);
MeshTools::transformPointsInPlace(transformation, data.positions(0));
MeshTools::transformVectorsInPlace(transformation, data.normals(0));
/* [MeshData3D-transform] */
}

{
Trade::ObjectData3D& fizz();
Trade::ObjectData3D& data = fizz();
/* [ObjectData3D-transformation] */
Matrix4 transformation =
    Matrix4::from(data.rotation().toMatrix(), data.translation())*
    Matrix4::scaling(data.scaling());
/* [ObjectData3D-transformation] */
static_cast<void>(transformation);
}

}
