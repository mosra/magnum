/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/Containers/ArrayTuple.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StringStl.h> /** @todo remove once file callbacks are <string>-free */
#include <Corrade/Containers/Pair.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Path.h>
#include <Corrade/Utility/Resource.h>

#include "Magnum/FileCallback.h"
#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/Mesh.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Animation/Player.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Swizzle.h"
#include "Magnum/MeshTools/Interleave.h"
#include "Magnum/MeshTools/Transform.h"
#include "Magnum/Trade/AbstractImageConverter.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/AbstractSceneConverter.h"
#include "Magnum/Trade/AnimationData.h"
#include "Magnum/Trade/ImageData.h"
#include "Magnum/Trade/LightData.h"
#include "Magnum/Trade/MaterialData.h"
#include "Magnum/Trade/MeshData.h"
#include "Magnum/Trade/PbrClearCoatMaterialData.h"
#include "Magnum/Trade/PbrSpecularGlossinessMaterialData.h"
#include "Magnum/Trade/PbrMetallicRoughnessMaterialData.h"
#include "Magnum/Trade/PhongMaterialData.h"
#include "Magnum/Trade/SceneData.h"
#include "Magnum/SceneGraph/Drawable.h"
#include "Magnum/SceneGraph/Scene.h"
#include "Magnum/SceneGraph/Object.h"
#include "Magnum/SceneGraph/MatrixTransformation3D.h"
#ifdef MAGNUM_TARGET_GL
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/MeshTools/Compile.h"
#include "Magnum/Shaders/PhongGL.h"
#endif
#ifdef MAGNUM_TARGET_VK
#include "Magnum/Vk/Vulkan.h"
#endif

#ifdef MAGNUM_BUILD_DEPRECATED
#define _MAGNUM_NO_DEPRECATED_MESHDATA /* So it doesn't yell here */
#define _MAGNUM_NO_DEPRECATED_OBJECTDATA /* So it doesn't yell here */

#include "Magnum/Trade/MeshData2D.h"
#include "Magnum/Trade/MeshData3D.h"
#include "Magnum/Trade/MeshObjectData3D.h"
#include "Magnum/Trade/ObjectData2D.h"
#endif

#define DOXYGEN_ELLIPSIS(...) __VA_ARGS__
#define DOXYGEN_IGNORE(...) __VA_ARGS__

using namespace Magnum;
using namespace Magnum::Math::Literals;

int main() {

{
Vector2i size;
/* [AbstractImageConverter-usage-file] */
PluginManager::Manager<Trade::AbstractImageConverter> manager;
Containers::Pointer<Trade::AbstractImageConverter> converter =
    manager.loadAndInstantiate("AnyImageConverter");

Image2D image{PixelFormat::RGBA8Unorm, size, DOXYGEN_ELLIPSIS({})};
if(!converter || !converter->convertToFile(image, "image.png"))
    Fatal{} << "Can't save image.png with AnyImageConverter";
/* [AbstractImageConverter-usage-file] */
}

{
/* [AbstractImageConverter-usage-file-levels] */
PluginManager::Manager<Trade::AbstractImageConverter> manager;
Containers::Pointer<Trade::AbstractImageConverter> converter =
    manager.loadAndInstantiate("AnyImageConverter");

Image2D level0{PixelFormat::RGBA16F, {256, 256}, DOXYGEN_ELLIPSIS({})};
Image2D level1{PixelFormat::RGBA16F, {128, 128}, DOXYGEN_ELLIPSIS({})};
Image2D level2{PixelFormat::RGBA16F, {64, 64}, DOXYGEN_ELLIPSIS({})};

if(!converter || !converter->convertToFile({level0, level1, level2}, "image.exr"))
    Fatal{} << "Can't save image.exr with AnyImageConverter";
/* [AbstractImageConverter-usage-file-levels] */
}

{
Image2D image{{}, {}, {}};
/* [AbstractImageConverter-usage-image] */
PluginManager::Manager<Trade::AbstractImageConverter> manager;
Containers::Pointer<Trade::AbstractImageConverter> converter =
    manager.loadAndInstantiate("StbDxtImageConverter");

Containers::Optional<Trade::ImageData2D> compressed;
if(!converter || !(compressed = converter->convert(image)))
    Fatal{} << "Can't convert the image with StbDxtImageConverter";
CORRADE_INTERNAL_ASSERT(compressed->isCompressed());
/* [AbstractImageConverter-usage-image] */
}

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

{
Containers::Pointer<Trade::AbstractImporter> importer;
/* [AbstractImporter-usage-data] */
Utility::Resource rs{"data"};
Containers::ArrayView<const char> data = rs.getRaw("image.png");
if(!importer->openData(data)) /* or openMemory() */
    Fatal{} << "Can't open image data with AnyImageImporter";

// import & use the image like above ...
/* [AbstractImporter-usage-data] */
}

#if defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT))
{
Containers::Pointer<Trade::AbstractImporter> importer;
/* [AbstractImporter-usage-callbacks] */
struct Data {
    std::unordered_map<std::string, Containers::Optional<
        Containers::Array<const char, Utility::Path::MapDeleter>>> files;
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

        /* Load if not there yet. If the mapping fails, remember that to not
           attempt to load the same file again next time. */
        if(found == data.files.end()) found = data.files.emplace(
            filename, Utility::Path::mapRead(filename)).first;

        if(!found->second) return {};
        return Containers::arrayView(*found->second);
    }, data);

importer->openFile("scene.gltf"); // memory-maps all files
/* [AbstractImporter-usage-callbacks] */
}
#endif

{
Containers::Pointer<Trade::AbstractImporter> importer;
/* [AbstractImporter-setFileCallback] */
importer->setFileCallback([](const std::string& filename,
    InputFileCallbackPolicy, void*) {
        Utility::Resource rs{"data"};
        return Containers::optional(rs.getRaw(filename));
    });
/* [AbstractImporter-setFileCallback] */
}

{
Containers::Pointer<Trade::AbstractImporter> importer;
/* [AbstractImporter-setFileCallback-template] */
const Utility::Resource rs{"data"};
importer->setFileCallback([](const std::string& filename,
    InputFileCallbackPolicy, const Utility::Resource& rs) {
        return Containers::optional(rs.getRaw(filename));
    }, rs);
/* [AbstractImporter-setFileCallback-template] */
}

{
struct: Trade::AbstractImporter {
    Trade::ImporterFeatures doFeatures() const override { return {}; }
    bool doIsOpened() const override { return false; }
    void doClose() override {}

    Containers::Array<char> _in;

/* [AbstractImporter-doOpenData-ownership] */
void doOpenData(Containers::Array<char>&& data, Trade::DataFlags dataFlags) override
{
    /* Take over the existing array or copy the data if we can't */
    if(dataFlags & (Trade::DataFlag::Owned|Trade::DataFlag::ExternallyOwned)) {
        _in = std::move(data);
    } else {
        _in = Containers::Array<char>{NoInit, data.size()};
        Utility::copy(data, _in);
    }

    DOXYGEN_ELLIPSIS()
}
/* [AbstractImporter-doOpenData-ownership] */
} importer;
}

{
/* [AbstractSceneConverter-usage-file] */
PluginManager::Manager<Trade::AbstractSceneConverter> manager;
Containers::Pointer<Trade::AbstractSceneConverter> converter =
    manager.loadAndInstantiate("AnySceneConverter");

Trade::MeshData mesh = DOXYGEN_ELLIPSIS(Trade::MeshData{{}, {}});
if(!converter || !converter->convertToFile(mesh, "mesh.ply"))
    Fatal{} << "Can't save mesh.ply with AnySceneConverter";
/* [AbstractSceneConverter-usage-file] */
}

{
Trade::MeshData mesh{{}, {}};
/* [AbstractSceneConverter-usage-mesh] */
PluginManager::Manager<Trade::AbstractSceneConverter> manager;
Containers::Pointer<Trade::AbstractSceneConverter> converter =
    manager.loadAndInstantiate("MeshOptimizerSceneConverter");

Containers::Optional<Trade::MeshData> optimized;
if(!converter || !(optimized = converter->convert(mesh)))
    Fatal{} << "Can't optimize the mesh with MeshOptimizerSceneConverter";
/* [AbstractSceneConverter-usage-mesh] */
}

{
Trade::MeshData mesh{{}, {}};
Containers::Pointer<Trade::AbstractSceneConverter> converter;
/* [AbstractSceneConverter-usage-mesh-in-place] */
if(!converter || !converter->convertInPlace(mesh))
    Fatal{} << "Can't optimize the mesh with MeshOptimizerSceneConverter";
/* [AbstractSceneConverter-usage-mesh-in-place] */
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
Trade::AnimationData data{nullptr, {}};
/* [AnimationData-usage-mutable] */
for(UnsignedInt i = 0; i != data.trackCount(); ++i) {
    if(data.trackTargetType(i) != Trade::AnimationTrackTargetType::Translation3D)
        continue;
    /* Check prerequisites */
    if(!(data.dataFlags() & Trade::DataFlag::Mutable) ||
       data.trackType(i) != Trade::AnimationTrackType::Vector2)
        Fatal{} << "Oops";

    MeshTools::transformVectorsInPlace(Matrix4::scaling(Vector3::yScale(-1.0f)),
        data.mutableTrack<Vector3>(i).values());
}
/* [AnimationData-usage-mutable] */
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
DOXYGEN_ELLIPSIS()
texture.setStorage(1, GL::textureFormat(image->format()), image->size());
if(!image->isCompressed())
    texture.setSubImage(0, {}, *image);
else
    texture.setCompressedSubImage(0, {}, *image);
/* [ImageData-usage] */
}
#endif

{
Trade::ImageData2D data{PixelFormat::RGB8Unorm, {}, nullptr};
/* [ImageData-usage-mutable] */
if(data.isCompressed() ||
   data.format() != PixelFormat::RGB8Unorm ||
   !(data.dataFlags() & Trade::DataFlag::Mutable))
    Fatal{} << ":(";

for(auto&& row: data.mutablePixels<Color3ub>())
    for(Color3ub& pixel: row)
        pixel = Math::gather<'b', 'g', 'r'>(pixel);
/* [ImageData-usage-mutable] */
}

{
/* [LightData-populating-range] */
Trade::LightData data{Trade::LightData::Type::Point,
    0xfff3d6_srgbf, 1.0f,
    15.0f};
/* [LightData-populating-range] */
}

{
/* [LightData-populating-attenuation] */
Trade::LightData data{Trade::LightData::Type::Spot,
    0xf3d6ff_srgbf, 10.0f,
    {0.01f, 0.5f, 2.0f},
    25.0_degf, 55.0_degf};
/* [LightData-populating-attenuation] */
}

{
/* [LightData-populating-none] */
Trade::LightData data{Trade::LightData::Type::Directional,
    0xd6fff3_srgbf, 0.25f};
/* [LightData-populating-none] */
}

{
/* [MaterialAttributeData-name] */
Trade::MaterialAttributeData a{
    Trade::MaterialAttribute::DiffuseColor, 0x3bd267ff_srgbaf};
Trade::MaterialAttributeData b{"DiffuseColor", 0x3bd267ff_srgbaf};
/* [MaterialAttributeData-name] */
}

{
/* [MaterialData-usage] */
Trade::MaterialData data = DOXYGEN_ELLIPSIS(Trade::MaterialData{{}, {}});

// Assumes the attribute exists
Float roughness = data.attribute<Float>(Trade::MaterialAttribute::Roughness);

// Optional access
Color4 color = data.attributeOr(Trade::MaterialAttribute::BaseColor,
                                0x3bd267ff_srgbaf);
if(Containers::Optional<UnsignedInt> texture =
   data.tryAttribute<UnsignedInt>(Trade::MaterialAttribute::BaseColorTexture))
{
    // ...
}
/* [MaterialData-usage] */
static_cast<void>(roughness);
static_cast<void>(color);
}

{
Trade::MaterialData data{{}, {}};
/* [MaterialData-usage-types] */
/* Prefer a specular/glossiness workflow, if present */
if(data.types() & Trade::MaterialType::PbrSpecularGlossiness) {
    const auto& pbr = data.as<Trade::PbrSpecularGlossinessMaterialData>();

    Color4 diffuse = pbr.diffuseColor();
    Color4 specular = pbr.specularColor();
    Float glossiness = pbr.glossiness();

    DOXYGEN_ELLIPSIS(static_cast<void>(diffuse), static_cast<void>(specular), static_cast<void>(glossiness);)

/* Otherwise use metallic/roughness (or defaults if no attributes present) */
} else {
    const auto& pbr = data.as<Trade::PbrMetallicRoughnessMaterialData>();

    Color4 base = pbr.baseColor();
    Float metalness = pbr.metalness();
    Float roughness = pbr.roughness();

    DOXYGEN_ELLIPSIS(static_cast<void>(base), static_cast<void>(metalness), static_cast<void>(roughness);)
}
/* [MaterialData-usage-types] */
}

{
/* [MaterialData-usage-texture-complexity] */
Trade::PbrSpecularGlossinessMaterialData data = DOXYGEN_ELLIPSIS(Trade::PbrSpecularGlossinessMaterialData{{}, {}});

/* Simple case for diffuse + packed specular/glossiness texture, the default
   coordinate set and a common coordinate transformation for all textures */
if(data.hasAttribute(Trade::MaterialAttribute::DiffuseTexture) &&
   data.hasSpecularGlossinessTexture() &&
   data.hasCommonTextureTransformation() && !data.hasTextureCoordinates())
{
    UnsignedInt diffuse = data.diffuseTexture();
    UnsignedInt specularGlossiness = data.specularTexture();
    Matrix3 textureMatrix = data.commonTextureMatrix();

    DOXYGEN_ELLIPSIS(static_cast<void>(diffuse), static_cast<void>(specularGlossiness), static_cast<void>(textureMatrix);)

/* Extra work needed when using a non-default texture coordinate set */
} else if(data.hasTextureCoordinates() && data.hasCommonTextureCoordinates()) {
    DOXYGEN_ELLIPSIS()

/* Etc... */
} else Fatal{} << "Material too complex, giving up";
/* [MaterialData-usage-texture-complexity] */
}

{
Trade::MaterialData data{{}, {}};
/* [MaterialData-usage-layers] */
if(data.hasLayer(Trade::MaterialLayer::ClearCoat)) {
    Float clearCoatFactor = data.attributeOr(Trade::MaterialLayer::ClearCoat,
        Trade::MaterialAttribute::LayerFactor, 1.0f);
    Float clearCoatRoughness = data.attributeOr(Trade::MaterialLayer::ClearCoat,
        Trade::MaterialAttribute::Roughness, 0.0f);

    DOXYGEN_ELLIPSIS(static_cast<void>(clearCoatFactor), static_cast<void>(clearCoatRoughness);)
}
/* [MaterialData-usage-layers] */
}

{
Trade::MaterialData data{{}, {}};
/* [MaterialData-usage-layers-types] */
if(data.types() & Trade::MaterialType::PbrClearCoat) {
    const auto& clearCoat = data.as<Trade::PbrClearCoatMaterialData>();

    Float clearCoatFactor = clearCoat.layerFactor();
    Float clearCoatRoughness = clearCoat.roughness();

    DOXYGEN_ELLIPSIS(static_cast<void>(clearCoatFactor), static_cast<void>(clearCoatRoughness);)
}
/* [MaterialData-usage-layers-types] */
}

{
Trade::MaterialData data{{}, {}};
/* [MaterialData-usage-mutable] */
Color4& color = data.mutableAttribute<Color4>(Trade::MaterialAttribute::BaseColor);
ColorHsv hsv = color.toHsv();
color.rgb() = Color3::fromHsv({hsv.hue, hsv.saturation*0.85f, hsv.value});
/* [MaterialData-usage-mutable] */
}

{
/* [MaterialData-populating] */
Trade::MaterialData data{Trade::MaterialType::PbrMetallicRoughness, {
    {Trade::MaterialAttribute::DoubleSided, true},
    {Trade::MaterialAttribute::BaseColor, 0x3bd267ff_srgbaf},
    {Trade::MaterialAttribute::BaseColorTexture, 17u},
    {Trade::MaterialAttribute::TextureMatrix, Matrix3::scaling({0.5f, 1.0f})}
}};
/* [MaterialData-populating] */
}

{
/* [MaterialData-populating-non-owned] */
using namespace Containers::Literals;

constexpr Trade::MaterialAttributeData attributes[]{
    {"BaseColor"_s, Color4{0.043735f, 0.64448f, 0.135633f, 1.0f}},
    {"BaseColorTexture"_s, 5u},
    {"DoubleSided"_s, true},
    {"TextureMatrix"_s, Matrix3{{0.5f, 0.0f, 0.0f},
                                {0.0f, 1.0f, 0.0f},
                                {0.0f, 0.0f, 1.0f}}}
};

Trade::MaterialData data{Trade::MaterialType::Phong, {}, attributes};
/* [MaterialData-populating-non-owned] */
}

#ifdef MAGNUM_TARGET_GL
{
GL::Texture2D baseColorTexture;
/* [MaterialData-populating-custom] */
Trade::MaterialData data{Trade::MaterialType::PbrMetallicRoughness, {
    {Trade::MaterialAttribute::BaseColor, 0x3bd267ff_srgbaf},
    {Trade::MaterialAttribute::TextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
    {"baseColorTexturePointer", &baseColorTexture},
    {"highlightColor", 0x00ffff_srgbf},
    {"name", "Canary Green Plastic, really ugly"}
}};

// Retrieving the texture pointer
GL::Texture2D* texture = data.attribute<GL::Texture2D*>("baseColorTexturePointer");
/* [MaterialData-populating-custom] */
static_cast<void>(texture);
}
#endif

{
/* [MaterialData-populating-layers] */
Trade::MaterialData data{
    Trade::MaterialType::PbrMetallicRoughness|Trade::MaterialType::PbrClearCoat,
    {
        {Trade::MaterialAttribute::BaseColor, 0xffcc33_srgbf},
        {Trade::MaterialAttribute::NoneRoughnessMetallicTexture, 0u},

        {Trade::MaterialLayer::ClearCoat},
        {Trade::MaterialAttribute::LayerFactorTexture, 1u},
        {Trade::MaterialAttribute::RoughnessTexture,  1u},
        {Trade::MaterialAttribute::RoughnessTextureSwizzle,
            Trade::MaterialTextureSwizzle::G}
    },

    {2, 6}
};
/* [MaterialData-populating-layers] */
}

{
UnsignedInt a, b, c, d, sandTile, grassTile, rockTile;
/* [MaterialData-populating-layers-custom] */
Trade::MaterialData proceduralLandscape{
    Trade::MaterialTypes{}, // Doesn't match any builtin material type
    {
        // Rock layer
        {Trade::MaterialAttribute::LayerFactorTexture, a},
        {Trade::MaterialAttribute::BaseColorTexture, rockTile},

        // Sand layer
        {Trade::MaterialAttribute::LayerFactorTexture, b},
        {"blendType", "mix"},
        {Trade::MaterialAttribute::BaseColorTexture, sandTile},

        // Grass layer
        {Trade::MaterialAttribute::LayerFactorTexture, c},
        {"blendType", "overlay"},
        {"strandLengthTexture", d},
        {Trade::MaterialAttribute::BaseColorTexture, grassTile},
    },

    // There's no base material, everything is in layers
    {0, 2, 5, 9}
};
/* [MaterialData-populating-layers-custom] */
}

{
/* [MeshIndexData-usage] */
Containers::ArrayView<const UnsignedShort> indices;

Trade::MeshIndexData data{indices};
/* [MeshIndexData-usage] */
}

{
Vector3 normal;
Vector4 tangent;
/* [MeshAttribute-bitangent-from-tangent] */
Vector3 bitangent = Math::cross(normal, tangent.xyz())*tangent.w();
/* [MeshAttribute-bitangent-from-tangent] */
static_cast<void>(bitangent);
}

{
/* [MeshAttributeData-usage] */
Containers::StridedArrayView1D<const Vector3> positions;

Trade::MeshAttributeData data{Trade::MeshAttribute::Position, positions};
/* [MeshAttributeData-usage] */
}

{
/* [MeshAttributeData-usage-offset-only] */
struct Vertex {
    Vector3 position;
    Vector4 color;
};

/* Layout defined statically, 15 vertices in total */
constexpr Trade::MeshAttributeData positions{Trade::MeshAttribute::Position,
    VertexFormat::Vector3, offsetof(Vertex, position), 15, sizeof(Vertex)};
constexpr Trade::MeshAttributeData colors{Trade::MeshAttribute::Color,
    VertexFormat::Vector4, offsetof(Vertex, color), 15, sizeof(Vertex)};

/* Actual data populated later */
Containers::Array<char> vertexData{15*sizeof(Vertex)};
DOXYGEN_ELLIPSIS()
Trade::MeshData{MeshPrimitive::Triangles, std::move(vertexData),
    {positions, colors}};
/* [MeshAttributeData-usage-offset-only] */
}

#ifdef MAGNUM_TARGET_VK
{
Containers::StridedArrayView1D<const void> data;
/* [MeshAttributeData-custom-vertex-format] */
Trade::MeshAttributeData normals{Trade::MeshAttribute::Normal,
    vertexFormatWrap(VK_FORMAT_B10G11R11_UFLOAT_PACK32),
    data};
/* [MeshAttributeData-custom-vertex-format] */
}
#endif

#ifdef MAGNUM_TARGET_GL
{
/* This snippet is also used by GL::Mesh, bear that in mind when updating */
/* [MeshData-usage-compile] */
Trade::MeshData data = DOXYGEN_ELLIPSIS(Trade::MeshData{MeshPrimitive::Points, 0});

GL::Mesh mesh = MeshTools::compile(data);
/* [MeshData-usage-compile] */
}

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
    Shaders::PhongGL::Position{}, Shaders::PhongGL::Normal{});

/* Set up an index buffer, if the mesh is indexed */
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

/* Set up the position and normal attributes */
mesh.addVertexBuffer(vertices,
    data.attributeOffset(Trade::MeshAttribute::Position),
    data.attributeStride(Trade::MeshAttribute::Position),
    GL::DynamicAttribute{Shaders::PhongGL::Position{},
        data.attributeFormat(Trade::MeshAttribute::Position)});
mesh.addVertexBuffer(vertices,
    data.attributeOffset(Trade::MeshAttribute::Normal),
    data.attributeStride(Trade::MeshAttribute::Normal),
    GL::DynamicAttribute{Shaders::PhongGL::Normal{},
        data.attributeFormat(Trade::MeshAttribute::Normal)});

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
Trade::MeshData data{MeshPrimitive::Points, 0};
/* [MeshData-usage-special-layouts] */
if(data.attributeStride(Trade::MeshAttribute::Position) <= 0 ||
   data.attributeStride(Trade::MeshAttribute::Normal) <= 0 ||
   (data.isIndexed() && !data.indices().isContiguous()))
    Fatal{} << "Uh oh";

// Now it's safe to use the Position and Normal attributes and the index buffer
// in a GPU mesh
/* [MeshData-usage-special-layouts] */
}

{
std::size_t vertexCount{}, indexCount{};
/* [MeshData-populating] */
struct Vertex {
    Vector3 position;
    Vector4 color;
};

Containers::Array<char> indexData{indexCount*sizeof(UnsignedShort)};
Containers::Array<char> vertexData{vertexCount*sizeof(Vertex)};
DOXYGEN_ELLIPSIS()
auto vertices = Containers::arrayCast<const Vertex>(vertexData);
auto indices = Containers::arrayCast<const UnsignedShort>(indexData);

Trade::MeshData data{MeshPrimitive::Triangles,
    std::move(indexData), Trade::MeshIndexData{indices},
    std::move(vertexData), {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            Containers::StridedArrayView1D<const Vector3>{vertices,
                &vertices[0].position, vertexCount, sizeof(Vertex)}},
        Trade::MeshAttributeData{Trade::MeshAttribute::Color,
            Containers::StridedArrayView1D<const Vector4>{vertices,
                &vertices[0].color, vertexCount, sizeof(Vertex)}}
    }};
/* [MeshData-populating] */
}

{
struct Vertex {
    Vector3 position;
    Vector4 color;
};
/* [MeshData-populating-non-owned] */
const UnsignedShort indices[] {
    0, 1, 2,
    2, 1, 3,
    3, 4, 5,
    5, 4, 6
};
Vertex vertices[7];

Trade::MeshData data{MeshPrimitive::Triangles,
    Trade::DataFlags{}, indices, Trade::MeshIndexData{indices},
    Trade::DataFlag::Mutable, vertices, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            Containers::StridedArrayView1D<const Vector3>{
                Containers::arrayView(vertices), &vertices[0].position,
                Containers::arraySize(vertices), sizeof(Vertex)}},
        Trade::MeshAttributeData{Trade::MeshAttribute::Color,
            Containers::StridedArrayView1D<const Vector4>{
                Containers::arrayView(vertices), &vertices[0].color,
                Containers::arraySize(vertices), sizeof(Vertex)}}
    }};
/* [MeshData-populating-non-owned] */
}

{
/* [MeshData-populating-custom] */
/* Each face can consist of 15 triangles at most, triangleCount says how many
   indices in triangleIds are valid */
struct Face {
    UnsignedShort triangleIds[15];
    UnsignedByte triangleCount;
};

constexpr Trade::MeshAttribute TriangleIds = Trade::meshAttributeCustom(0x01);
constexpr Trade::MeshAttribute TriangleCount = Trade::meshAttributeCustom(0x02);

Containers::Array<char> vertexData;
auto faces = Containers::arrayCast<const Face>(vertexData);

Trade::MeshData data{MeshPrimitive::Faces, std::move(vertexData), {
    Trade::MeshAttributeData{TriangleIds,
        Containers::StridedArrayView2D<const UnsignedShort>{faces,
            &faces[0].triangleIds[0],
            {faces.size(), 15},
            {sizeof(Face), sizeof(UnsignedShort)}}},
    Trade::MeshAttributeData{TriangleCount,
        Containers::StridedArrayView1D<const UnsignedByte>{faces,
            &faces[0].triangleCount, faces.size(), sizeof(Face)}}
}};
/* [MeshData-populating-custom] */

/* [MeshData-populating-custom-retrieve] */
Containers::StridedArrayView2D<const UnsignedShort> triangleIds =
    data.attribute<UnsignedShort[]>(TriangleIds);
Containers::StridedArrayView1D<const UnsignedByte> triangleCounts =
    data.attribute<UnsignedByte>(TriangleCount);
/* [MeshData-populating-custom-retrieve] */
static_cast<void>(triangleIds);
static_cast<void>(triangleCounts);
}

#ifdef MAGNUM_BUILD_DEPRECATED
{
CORRADE_IGNORE_DEPRECATED_PUSH
Trade::MeshData2D& foo();
Trade::MeshData2D& data = foo();
/* [MeshData2D-transform] */
Matrix3 transformation =
    Matrix3::translation({3.0f, -2.0f})*
    Matrix3::scaling(Vector2{2.0f})*
    Matrix3::rotation(45.0_degf);
MeshTools::transformPointsInPlace(transformation, data.positions(0));
/* [MeshData2D-transform] */
CORRADE_IGNORE_DEPRECATED_POP
}

{
CORRADE_IGNORE_DEPRECATED_PUSH
Trade::ObjectData2D& baz();
Trade::ObjectData2D& data = baz();
/* [ObjectData2D-transformation] */
Matrix3 transformation =
    Matrix3::from(data.rotation().toMatrix(), data.translation())*
    Matrix3::scaling(data.scaling());
/* [ObjectData2D-transformation] */
static_cast<void>(transformation);
CORRADE_IGNORE_DEPRECATED_POP
}

{
CORRADE_IGNORE_DEPRECATED_PUSH
Trade::MeshData3D& bar();
Trade::MeshData3D& data = bar();
/* [MeshData3D-transform] */
Matrix4 transformation =
    Matrix4::translation({3.0f, 1.5f, -2.0f})*
    Matrix4::rotationX(45.0_degf);
MeshTools::transformPointsInPlace(transformation, data.positions(0));
MeshTools::transformVectorsInPlace(transformation, data.normals(0));
/* [MeshData3D-transform] */
CORRADE_IGNORE_DEPRECATED_POP
}

{
CORRADE_IGNORE_DEPRECATED_PUSH
Trade::ObjectData3D& fizz();
Trade::ObjectData3D& data = fizz();
/* [ObjectData3D-transformation] */
Matrix4 transformation =
    Matrix4::from(data.rotation().toMatrix(), data.translation())*
    Matrix4::scaling(data.scaling());
/* [ObjectData3D-transformation] */
static_cast<void>(transformation);
CORRADE_IGNORE_DEPRECATED_POP
}
#endif

{
/* [SceneFieldData-usage] */
Containers::StridedArrayView1D<UnsignedInt> transformationMapping = DOXYGEN_ELLIPSIS({});
Containers::StridedArrayView1D<Matrix4> transformations = DOXYGEN_ELLIPSIS({});

Trade::SceneFieldData field{Trade::SceneField::Transformation,
    transformationMapping, transformations};
/* [SceneFieldData-usage] */
}

{
/* [SceneFieldData-usage-offset-only] */
struct Node {
    UnsignedInt object;
    Int parent;
    Matrix4 transform;
};

/* Layout defined statically, 120 objects in total */
constexpr Trade::SceneFieldData parents{Trade::SceneField::Parent, 120,
    Trade::SceneMappingType::UnsignedInt, offsetof(Node, object), sizeof(Node),
    Trade::SceneFieldType::Int, offsetof(Node, parent), sizeof(Node)};
constexpr Trade::SceneFieldData transforms{Trade::SceneField::Transformation, 120,
    Trade::SceneMappingType::UnsignedInt, offsetof(Node, object), sizeof(Node),
    Trade::SceneFieldType::Matrix4x4, offsetof(Node, transform), sizeof(Node)};

/* Actual data populated later */
Containers::Array<char> data{120*sizeof(Node)};
DOXYGEN_ELLIPSIS()
Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 120, std::move(data),
    {parents, transforms}};
/* [SceneFieldData-usage-offset-only] */
}

{
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;
typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
/* [SceneData-usage1] */
Trade::SceneData data = DOXYGEN_ELLIPSIS(Trade::SceneData{{}, 0, nullptr, nullptr});
if(!data.is3D() ||
   !data.hasField(Trade::SceneField::Parent) ||
   !data.hasField(Trade::SceneField::Mesh))
    Fatal{} << "Oh noes!";

Scene3D scene;
Containers::Array<Object3D*> objects{std::size_t(data.mappingBound())};
/* [SceneData-usage1] */

/* [SceneData-usage2] */
auto parents = data.parentsAsArray();
for(Containers::Pair<UnsignedInt, Int>& parent: parents)
    objects[parent.first()] = new Object3D{};
/* [SceneData-usage2] */

/* [SceneData-usage3] */
for(Containers::Pair<UnsignedInt, Int>& parent: parents)
    objects[parent.first()]->setParent(
        parent.second() == -1 ? &scene : objects[parent.second()]
    );
/* [SceneData-usage3] */

/* [SceneData-usage4] */
for(Containers::Pair<UnsignedInt, Matrix4>& transformation:
    data.transformations3DAsArray())
{
    if(Object3D* const object = objects[transformation.first()])
        object->setTransformation(transformation.second());
}
/* [SceneData-usage4] */

/* [SceneData-usage5] */
class Drawable: public SceneGraph::Drawable3D {
    public:
        explicit Drawable(Object3D& object, UnsignedInt mesh, Int material, DOXYGEN_ELLIPSIS(int))DOXYGEN_IGNORE(: SceneGraph::Drawable3D{object} {
            static_cast<void>(mesh);
            static_cast<void>(material);
        } int foo);

    DOXYGEN_ELLIPSIS(void draw(const Matrix4&, SceneGraph::Camera3D&) override {})
};

for(const Containers::Pair<UnsignedInt, Containers::Pair<UnsignedInt, Int>>&
    meshMaterial: data.meshesMaterialsAsArray())
{
    if(Object3D* const object = objects[meshMaterial.first()])
        new Drawable{*object, meshMaterial.second().first(),
                              meshMaterial.second().second(), DOXYGEN_ELLIPSIS(0)};
}
/* [SceneData-usage5] */

/* [SceneData-usage-advanced] */
Containers::StridedArrayView1D<const UnsignedInt> transformationMapping =
    data.mapping<UnsignedInt>(Trade::SceneField::Transformation);
Containers::StridedArrayView1D<const Matrix4> transformations =
    data.field<Matrix4>(Trade::SceneField::Transformation);
for(std::size_t i = 0; i != transformationMapping.size(); ++i) {
    if(Object3D* const object = objects[transformationMapping[i]])
        object->setTransformation(transformations[i]);
}
/* [SceneData-usage-advanced] */
}

{
Trade::SceneData data{{}, 0, nullptr, nullptr};
/* [SceneData-per-object] */
Containers::Pointer<Trade::AbstractImporter> importer = DOXYGEN_ELLIPSIS({});

for(const Containers::Pair<UnsignedInt, Int>& meshMaterial:
   data.meshesMaterialsFor(importer->objectForName("Chair")))
{
    Debug{} << "Mesh:" << importer->meshName(meshMaterial.first());
    if(meshMaterial.second() != -1)
        Debug{} << "With a material:" << importer->materialName(meshMaterial.second());
}
/* [SceneData-per-object] */
}

{
Trade::SceneData data{{}, 0, nullptr, nullptr};
typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
Containers::Array<Object3D*> objects;
/* [SceneData-usage-mutable] */
Containers::StridedArrayView1D<const UnsignedInt> transformationMapping =
    data.mapping<UnsignedInt>(Trade::SceneField::Transformation);
Containers::StridedArrayView1D<Matrix4> mutableTransformations =
    data.mutableField<Matrix4>(Trade::SceneField::Transformation);
for(std::size_t i = 0; i != transformationMapping.size(); ++i) {
    if(Object3D* const object = objects[transformationMapping[i]])
        mutableTransformations[i] = object->transformation();
}
/* [SceneData-usage-mutable] */
}

{
const std::size_t nodeCount{}, meshAssignmentCount{};
/* [SceneData-populating] */
struct Common {
    UnsignedShort object;
    Short parent;
    Matrix4 transformation;
};

Containers::StridedArrayView1D<Common> common;
Containers::ArrayView<UnsignedShort> meshMaterialMapping;
Containers::ArrayView<UnsignedShort> meshes;
Containers::ArrayView<UnsignedShort> meshMaterials;
Containers::Array<char> data = Containers::ArrayTuple{
    {nodeCount, common},
    {meshAssignmentCount, meshMaterialMapping},
    {meshAssignmentCount, meshes},
    {meshAssignmentCount, meshMaterials}
};

// populate the views ...

Trade::SceneData scene{
    Trade::SceneMappingType::UnsignedShort, nodeCount,
    std::move(data), {
        Trade::SceneFieldData{Trade::SceneField::Parent,
            common.slice(&Common::object), common.slice(&Common::parent)},
        Trade::SceneFieldData{Trade::SceneField::Transformation,
            common.slice(&Common::object), common.slice(&Common::transformation)},
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            meshMaterialMapping, meshes},
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
            meshMaterialMapping, meshMaterials}
    }};
/* [SceneData-populating] */
}

{
std::size_t nodeCount{};
/* [SceneData-populating-custom1] */
DOXYGEN_ELLIPSIS()
Containers::ArrayView<UnsignedShort> cellMapping;
Containers::ArrayView<Matrix4> cellFrustums;
Containers::StridedArrayView2D<Int> cellLights;
Containers::Array<char> data = Containers::ArrayTuple{
    DOXYGEN_ELLIPSIS()
    {32*24, cellMapping},
    {32*24, cellFrustums},
    {{32*24, 8}, cellLights},
};

for(std::size_t i = 0; i != cellMapping.size(); ++i) {
    cellMapping[i] = nodeCount + i;
    cellFrustums[i] = DOXYGEN_ELLIPSIS({});
    for(std::size_t j = 0; j != cellLights[i].size(); ++j)
        cellLights[i][j] = DOXYGEN_ELLIPSIS({});
}
/* [SceneData-populating-custom1] */

/* [SceneData-populating-custom2] */
constexpr Trade::SceneField CellFrustum = Trade::sceneFieldCustom(0x00);
constexpr Trade::SceneField CellLights = Trade::sceneFieldCustom(0x01);

Trade::SceneData scene{
    Trade::SceneMappingType::UnsignedShort, nodeCount + cellMapping.size(),
    std::move(data), {
        DOXYGEN_ELLIPSIS()
        Trade::SceneFieldData{CellFrustum, cellMapping, cellFrustums},
        Trade::SceneFieldData{CellLights, cellMapping, cellLights},
    }};
/* [SceneData-populating-custom2] */
}

{
constexpr Trade::SceneField CellFrustum = Trade::sceneFieldCustom(0);
constexpr Trade::SceneField CellLights = Trade::sceneFieldCustom(1);
Trade::SceneData scene{{}, 0, nullptr, nullptr};
/* [SceneData-populating-custom-retrieve] */
Containers::StridedArrayView1D<const Matrix4> cellFrustums =
    scene.field<Matrix4>(CellFrustum);
Containers::StridedArrayView2D<const Int> cellLights =
    scene.field<Int[]>(CellLights);
/* [SceneData-populating-custom-retrieve] */
static_cast<void>(cellFrustums);
static_cast<void>(cellLights);
}

}
