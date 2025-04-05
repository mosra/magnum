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

/* In order to have the CORRADE_PLUGIN_REGISTER() macro not a no-op. Doesn't
   affect anything else. */
#define CORRADE_STATIC_PLUGIN

#include <unordered_map>
#include <Corrade/Containers/ArrayTuple.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StringIterable.h>
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
#include "Magnum/MeshTools/RemoveDuplicates.h"
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

namespace MyNamespace {

struct MyImporter: Trade::AbstractImporter {
    explicit MyImporter(PluginManager::AbstractManager& manager, Containers::StringView plugin): Trade::AbstractImporter{manager, plugin} {}

    Trade::ImporterFeatures doFeatures() const override { return {}; }
    bool doIsOpened() const override { return false; }
    void doClose() override {}
};
struct MyImageConverter: Trade::AbstractImageConverter {
    explicit MyImageConverter(PluginManager::AbstractManager& manager, Containers::StringView plugin): Trade::AbstractImageConverter{manager, plugin} {}

    Trade::ImageConverterFeatures doFeatures() const override { return {}; }
};
struct MySceneConverter: Trade::AbstractSceneConverter {
    explicit MySceneConverter(PluginManager::AbstractManager& manager, Containers::StringView plugin): Trade::AbstractSceneConverter{manager, plugin} {}

    Trade::SceneConverterFeatures doFeatures() const override { return {}; }
};

}

/* [MAGNUM_TRADE_ABSTRACTIMPORTER_PLUGIN_INTERFACE] */
CORRADE_PLUGIN_REGISTER(MyImporter, MyNamespace::MyImporter,
    MAGNUM_TRADE_ABSTRACTIMPORTER_PLUGIN_INTERFACE)
/* [MAGNUM_TRADE_ABSTRACTIMPORTER_PLUGIN_INTERFACE] */

/* [MAGNUM_TRADE_ABSTRACTIMAGECONVERTER_PLUGIN_INTERFACE] */
CORRADE_PLUGIN_REGISTER(MyImageConverter, MyNamespace::MyImageConverter,
    MAGNUM_TRADE_ABSTRACTIMAGECONVERTER_PLUGIN_INTERFACE)
/* [MAGNUM_TRADE_ABSTRACTIMAGECONVERTER_PLUGIN_INTERFACE] */

/* [MAGNUM_TRADE_ABSTRACTSCENECONVERTER_PLUGIN_INTERFACE] */
CORRADE_PLUGIN_REGISTER(MySceneConverter, MyNamespace::MySceneConverter,
    MAGNUM_TRADE_ABSTRACTSCENECONVERTER_PLUGIN_INTERFACE)
/* [MAGNUM_TRADE_ABSTRACTSCENECONVERTER_PLUGIN_INTERFACE] */

/* Make sure the name doesn't conflict with any other snippets to avoid linker
   warnings, unlike with `int main()` there now has to be a declaration to
   avoid -Wmisssing-prototypes */
void mainTrade();
void mainTrade() {
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
/* -Wnonnull in GCC 11+  "helpfully" says "this is null" if I don't initialize
   the converter pointer. I don't care, I just want you to check compilation
   errors, not more! */
PluginManager::Manager<Trade::AbstractImporter> manager;
Containers::Pointer<Trade::AbstractImporter> importer = manager.loadAndInstantiate("SomethingWhatever");
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
/* -Wnonnull in GCC 11+  "helpfully" says "this is null" if I don't initialize
   the converter pointer. I don't care, I just want you to check compilation
   errors, not more! */
PluginManager::Manager<Trade::AbstractImporter> manager;
Containers::Pointer<Trade::AbstractImporter> importer = manager.loadAndInstantiate("SomethingWhatever");
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
/* -Wnonnull in GCC 11+  "helpfully" says "this is null" if I don't initialize
   the converter pointer. I don't care, I just want you to check compilation
   errors, not more! */
PluginManager::Manager<Trade::AbstractImporter> manager;
Containers::Pointer<Trade::AbstractImporter> importer = manager.loadAndInstantiate("SomethingWhatever");
/* [AbstractImporter-setFileCallback] */
importer->setFileCallback([](const std::string& filename,
    InputFileCallbackPolicy, void*) {
        Utility::Resource rs{"data"};
        return Containers::optional(rs.getRaw(filename));
    });
/* [AbstractImporter-setFileCallback] */
}

{
/* -Wnonnull in GCC 11+  "helpfully" says "this is null" if I don't initialize
   the converter pointer. I don't care, I just want you to check compilation
   errors, not more! */
PluginManager::Manager<Trade::AbstractImporter> manager;
Containers::Pointer<Trade::AbstractImporter> importer = manager.loadAndInstantiate("SomethingWhatever");
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
/* [AbstractSceneConverter-usage-mesh-file] */
PluginManager::Manager<Trade::AbstractSceneConverter> manager;
Containers::Pointer<Trade::AbstractSceneConverter> converter =
    manager.loadAndInstantiate("AnySceneConverter");

Trade::MeshData mesh = DOXYGEN_ELLIPSIS(Trade::MeshData{{}, {}});
if(!converter || !converter->convertToFile(mesh, "mesh.ply"))
    Fatal{} << "Can't save mesh.ply with AnySceneConverter";
/* [AbstractSceneConverter-usage-mesh-file] */
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
/* [AbstractSceneConverter-usage-multiple-file] */
PluginManager::Manager<Trade::AbstractImporter> importerManager;
Containers::Pointer<Trade::AbstractImporter> importer =
    importerManager.loadAndInstantiate("AnySceneImporter");
if(!importer || importer->openFile("file.dae"))
    Fatal{} << "Can't open the input file";

PluginManager::Manager<Trade::AbstractSceneConverter> manager;
Containers::Pointer<Trade::AbstractSceneConverter> converter =
    manager.loadAndInstantiate("AnySceneConverter");

if(!converter->beginFile("file.gltf") ||
   !converter->addSupportedImporterContents(*importer) ||
   !converter->endFile())
    Fatal{} << "Can't save the output file";
/* [AbstractSceneConverter-usage-multiple-file] */
}

{
Containers::Pointer<Trade::AbstractImporter> importer;
Containers::Pointer<Trade::AbstractSceneConverter> converter;
/* [AbstractSceneConverter-usage-multiple-file-selective] */
if(!converter->beginFile("file.gltf"))
    Fatal{} << "Can't begin the output file";

/* Add meshes manually, removing duplicates in each in the process */
for(UnsignedInt i = 0; i != importer->meshCount(); ++i) {
    Containers::Optional<Trade::MeshData> mesh = importer->mesh(i);
    if(!mesh ||
       !converter->add(MeshTools::removeDuplicates(*mesh), importer->meshName(i)))
        Fatal{} << "Can't add mesh" << i;
}

/* Add the rest of the input file and finish */
if(!converter->addSupportedImporterContents(*importer, ~Trade::SceneContent::Meshes) ||
   !converter->endFile())
    Fatal{} << "Can't save the output file";
/* [AbstractSceneConverter-usage-multiple-file-selective] */
}

{
UnsignedInt id{};
Containers::Pointer<Trade::AbstractImporter> importer;
/* [AnimationData-usage] */

Containers::Optional<Trade::AnimationData> data = importer->animation(id);

Animation::Player<Float> player;
Containers::Array<Vector3> positions; /* Translations for all objects */
for(UnsignedInt i = 0; i != data->trackCount(); ++i) {
    if(data->trackTargetName(i) == Trade::AnimationTrackTarget::Translation3D) {
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
    if(data.trackTargetName(i) != Trade::AnimationTrackTarget::Translation3D)
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
/* [ImageData-populating] */
Containers::Array<char> uncompressedData = DOXYGEN_ELLIPSIS({});
Trade::ImageData2D uncompressed{PixelFormat::RGB8Unorm,
    {32, 32}, std::move(uncompressedData)};

Containers::Array<char> compressedData = DOXYGEN_ELLIPSIS({});
Trade::ImageData2D compressed{CompressedPixelFormat::Bc1RGBUnorm,
    {32, 32}, std::move(compressedData)};
/* [ImageData-populating] */
}

{
/* [ImageData-populating-non-owned] */
Color3ub uncompressedData[]{DOXYGEN_ELLIPSIS({})};
Trade::ImageData2D uncompressed{PixelFormat::RGB8Unorm,
    {32, 32}, Trade::DataFlag::Mutable, uncompressedData};

Containers::ArrayView<const char> compressedData = DOXYGEN_ELLIPSIS({});
Trade::ImageData2D compressed{CompressedPixelFormat::Bc1RGBUnorm,
    {32, 32}, Trade::DataFlags{}, compressedData};
/* [ImageData-populating-non-owned] */
}

{
/* [ImageData-populating-padding] */
PixelFormat format = DOXYGEN_ELLIPSIS({});
Vector2i size = DOXYGEN_ELLIPSIS({});
std::size_t rowStride = 4*((size.x()*pixelFormatSize(format) + 3)/4);
Containers::Array<char> data{ValueInit, std::size_t(size.y()*rowStride)};
DOXYGEN_ELLIPSIS()

Trade::ImageData2D image{format, size, std::move(data)};
/* [ImageData-populating-padding] */
}

{
/* [ImageData-populating-alignment] */
PixelFormat format = DOXYGEN_ELLIPSIS({});
Vector2i size = DOXYGEN_ELLIPSIS({});
std::size_t rowLength = size.x()*pixelFormatSize(format);
Containers::Array<char> data{ValueInit, std::size_t(size.y()*rowLength)};
DOXYGEN_ELLIPSIS()

Trade::ImageData2D image{
    PixelStorage{}.setAlignment(rowLength % 4 == 0 ? 4 : 1),
    format, size, std::move(data)};
/* [ImageData-populating-alignment] */
}

#ifdef MAGNUM_TARGET_GL
{
/* [ImageData-usage] */
Trade::ImageData2D image = DOXYGEN_ELLIPSIS(Trade::ImageData2D{PixelFormat{}, {}, nullptr});

GL::Texture2D texture;
DOXYGEN_ELLIPSIS()
texture.setStorage(1, GL::textureFormat(image.format()), image.size());
if(!image.isCompressed())
    texture.setSubImage(0, {}, image);
else
    texture.setCompressedSubImage(0, {}, image);
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
Trade::LightData data{Trade::LightType::Point,
    0xfff3d6_srgbf, 1.0f,
    15.0f};
/* [LightData-populating-range] */
}

{
/* [LightData-populating-attenuation] */
Trade::LightData data{Trade::LightType::Spot,
    0xf3d6ff_srgbf, 10.0f,
    {0.01f, 0.5f, 2.0f},
    25.0_degf, 55.0_degf};
/* [LightData-populating-attenuation] */
}

{
/* [LightData-populating-none] */
Trade::LightData data{Trade::LightType::Directional,
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
   data.findAttribute<UnsignedInt>(Trade::MaterialAttribute::BaseColorTexture))
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
char sha1[20]{DOXYGEN_ELLIPSIS()};

Trade::MaterialData data{Trade::MaterialType::PbrMetallicRoughness, {
    {Trade::MaterialAttribute::BaseColor, 0x3bd267ff_srgbaf},
    {Trade::MaterialAttribute::TextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
    {"baseColorTexturePointer", &baseColorTexture},
    {"highlightColor", 0x00ffff_srgbf},
    {"name", "Canary Green Plastic, really ugly"},
    {"hash", Containers::ArrayView<const void>{sha1}},
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
UnsignedInt vertexCount{};
/* [MeshAttributeData-usage-offset-only] */
struct Vertex {
    Vector3 position;
    Vector4 color;
};

Trade::MeshAttributeData positions{Trade::MeshAttribute::Position,
    VertexFormat::Vector3, offsetof(Vertex, position), vertexCount, sizeof(Vertex)};
Trade::MeshAttributeData colors{Trade::MeshAttribute::Color,
    VertexFormat::Vector4, offsetof(Vertex, color), vertexCount, sizeof(Vertex)};
/* [MeshAttributeData-usage-offset-only] */
static_cast<void>(positions);
static_cast<void>(colors);
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
/* [MeshData-gpu-opengl] */
Trade::MeshData data = DOXYGEN_ELLIPSIS(Trade::MeshData{MeshPrimitive::Points, 0});

GL::Mesh mesh = MeshTools::compile(data);
/* [MeshData-gpu-opengl] */
}

{
Trade::MeshData data{MeshPrimitive::Points, 0};
/* [MeshData-gpu-opengl-direct] */
GL::Mesh mesh{data.primitive()};
mesh.setCount(data.indexCount());

/* Upload index data and configure their layout */
GL::Buffer indices{data.indexData()};
mesh.setIndexBuffer(indices, 0, data.indexType());

/* Upload vertex data and set up position and normal attributes */
GL::Buffer vertices{data.vertexData()};
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
/* [MeshData-gpu-opengl-direct] */
}
#endif

{
Trade::MeshData data{MeshPrimitive::Points, 0};
/* [MeshData-access] */
if(data.primitive() != MeshPrimitive::Triangles ||
   !data.isIndexed() ||
   !data.hasAttribute(Trade::MeshAttribute::Position))
    Fatal{} << "Oh well";

/* Calculate the face area */
Containers::Array<UnsignedInt> indices = data.indicesAsArray();
Containers::Array<Vector3> positions = data.positions3DAsArray();
Float area = 0.0f;
for(std::size_t i = 0; i < indices.size(); i += 3)
    area += Math::cross(
        positions[indices[i + 1]] - positions[indices[i]],
        positions[indices[i + 2]] - positions[indices[i]]).length()*0.5f;
/* [MeshData-access] */
static_cast<void>(area);
}

{
Trade::MeshData data{MeshPrimitive::Points, 0};
/* [MeshData-access-direct] */
DOXYGEN_ELLIPSIS()

if(data.indexType() != MeshIndexType::UnsignedInt ||
   data.attributeFormat(Trade::MeshAttribute::Position) != VertexFormat::Vector3)
    Fatal{} << "Dang";

Containers::StridedArrayView1D<const UnsignedInt> indices =
    data.indices<UnsignedInt>();
Containers::StridedArrayView1D<const Vector3> positions =
    data.attribute<Vector3>(Trade::MeshAttribute::Position);
/* [MeshData-access-direct] */
static_cast<void>(indices);
static_cast<void>(positions);
}

{
Trade::MeshData data{MeshPrimitive::Points, 0};
/* [MeshData-access-mutable] */
DOXYGEN_ELLIPSIS()

if(data.attributeFormat(Trade::MeshAttribute::Position) != VertexFormat::Vector3)
    Fatal{} << "Sigh";

/* Scale the mesh two times */
Matrix4 transformation = Matrix4::scaling(Vector3{2.0f});
for(Vector3& i: data.mutableAttribute<Vector3>(Trade::MeshAttribute::Position))
    i = transformation.transformPoint(i);
/* [MeshData-access-mutable] */
}

{
Trade::MeshData data{MeshPrimitive::Points, 0};
/* [MeshData-access-morph-targets] */
if(!data.hasAttribute(Trade::MeshAttribute::Position, 0) ||
   !data.hasAttribute(Trade::MeshAttribute::Position, 1))
    Fatal{} << "Positions not present in morph targets 0 and 1";

Float weights[]{0.25f, 0.5f};

/* Calculate morphed positions with the above weights */
Containers::Array<Vector3> positions = data.positions3DAsArray(0, -1);
for(Int morphTargetId: {0, 1}) {
    Containers::StridedArrayView1D<const Vector3> morphed =
        data.attribute<Vector3>(Trade::MeshAttribute::Position, 0, morphTargetId);
    for(std::size_t i = 0; i != data.vertexCount(); ++i)
        positions[i] += morphed[i]*weights[morphTargetId];
}
/* [MeshData-access-morph-targets] */
}

{
Trade::MeshData data{MeshPrimitive::Points, 0};
/* [MeshData-special-layouts] */
if(data.attributeStride(Trade::MeshAttribute::Position) <= 0 ||
   data.attributeStride(Trade::MeshAttribute::Normal) <= 0 ||
   (data.isIndexed() && !data.indices().isContiguous()))
    Fatal{} << "Uh oh";

// Now it's safe to use the Position and Normal attributes and the index buffer
// in a GPU mesh
/* [MeshData-special-layouts] */
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
Containers::StridedArrayView1D<const Vertex> vertices =
    Containers::arrayCast<const Vertex>(vertexData);
Containers::ArrayView<const UnsignedShort> indices =
    Containers::arrayCast<const UnsignedShort>(indexData);

Trade::MeshData data{MeshPrimitive::Triangles,
    std::move(indexData), Trade::MeshIndexData{indices},
    std::move(vertexData), {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            vertices.slice(&Vertex::position)},
        Trade::MeshAttributeData{Trade::MeshAttribute::Color,
            vertices.slice(&Vertex::color)}
    }};
/* [MeshData-populating] */
}

{
struct Vertex {
    Vector3 position;
    Vector4 color;
};
/* [MeshData-populating-non-owned] */
const UnsignedShort indices[]{
    DOXYGEN_ELLIPSIS(0)
};
Vertex vertices[]{
    DOXYGEN_ELLIPSIS({})
};

Trade::MeshData data{MeshPrimitive::Triangles,
    Trade::DataFlags{}, indices, Trade::MeshIndexData{indices},
    Trade::DataFlag::Mutable, vertices, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            Containers::stridedArrayView(vertices).slice(&Vertex::position)},
        Trade::MeshAttributeData{Trade::MeshAttribute::Color,
            Containers::stridedArrayView(vertices).slice(&Vertex::color)}
    }};
/* [MeshData-populating-non-owned] */
}

{
UnsignedInt vertexCount{};
/* [MeshData-populating-offset-only] */
struct Vertex {
    Vector3 position;
    Vector4 color;
};

/* Layout known in advance, except for vertex count */
constexpr Trade::MeshAttributeData attributes[]{
    Trade::MeshAttributeData{Trade::MeshAttribute::Position,
        VertexFormat::Vector3, offsetof(Vertex, position), 0, sizeof(Vertex)},
    Trade::MeshAttributeData{Trade::MeshAttribute::Color,
        VertexFormat::Vector4, offsetof(Vertex, color), 15, sizeof(Vertex)}
};

/* Actual data populated later */
Containers::Array<char> vertexData{vertexCount*sizeof(Vertex)};
DOXYGEN_ELLIPSIS()

/* Using the statically defined attribute layout together with explicitly
   passed vertex count */
Trade::MeshData mesh{MeshPrimitive::Triangles, std::move(vertexData),
    Trade::meshAttributeDataNonOwningArray(attributes), vertexCount};
/* [MeshData-populating-offset-only] */
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
        Containers::StridedArrayView2D<const UnsignedShort>{
            stridedArrayView(faces).slice(&Face::triangleIds)}},
    Trade::MeshAttributeData{TriangleCount,
        stridedArrayView(faces).slice(&Face::triangleCount)}
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

{
/* [MeshData-jointIdsAsArray] */
Trade::MeshData data = DOXYGEN_ELLIPSIS(Trade::MeshData{{}, 0});

Containers::Array<UnsignedInt> array = data.jointIdsAsArray();
Containers::StridedArrayView2D<UnsignedInt> array2D{array,
    {data.vertexCount(), data.attributeArraySize(Trade::MeshAttribute::JointIds)}};

for(Containers::StridedArrayView1D<UnsignedInt> i: array2D) {
    for(UnsignedInt j: i) {
        DOXYGEN_IGNORE(static_cast<void>(j);)// do something with joint ID j in vertex i
    }
}
/* [MeshData-jointIdsAsArray] */
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
std::size_t objectCount = 120;
/* [SceneFieldData-usage-offset-only] */
struct Node {
    UnsignedInt mapping;
    Int parent;
    Matrix4 transformation;
};

Trade::SceneFieldData parents{Trade::SceneField::Parent, objectCount,
    Trade::SceneMappingType::UnsignedInt, offsetof(Node, mapping), sizeof(Node),
    Trade::SceneFieldType::Int, offsetof(Node, parent), sizeof(Node)};
Trade::SceneFieldData transformations{Trade::SceneField::Transformation, objectCount,
    Trade::SceneMappingType::UnsignedInt, offsetof(Node, mapping), sizeof(Node),
    Trade::SceneFieldType::Matrix4x4, offsetof(Node, transformation), sizeof(Node)};
/* [SceneFieldData-usage-offset-only] */
static_cast<void>(parents);
static_cast<void>(transformations);
}

{
/* [SceneFieldData-usage-strings] */
Containers::StridedArrayView1D<UnsignedInt> mapping = DOXYGEN_ELLIPSIS({});
Containers::StringView string = DOXYGEN_ELLIPSIS({});
Containers::StridedArrayView1D<Containers::Pair<UnsignedInt, UnsignedInt>> ranges = DOXYGEN_ELLIPSIS({});

Trade::SceneFieldData field{Trade::sceneFieldCustom(35), mapping,
    string.data(), Trade::SceneFieldType::StringRange32, ranges};
/* [SceneFieldData-usage-strings] */
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
    UnsignedShort mapping;
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
            common.slice(&Common::mapping),
            common.slice(&Common::parent)},
        Trade::SceneFieldData{Trade::SceneField::Transformation,
            common.slice(&Common::mapping), common.slice(&Common::transformation)},
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            meshMaterialMapping,
            meshes},
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
            meshMaterialMapping,
            meshMaterials}
    }};
/* [SceneData-populating] */
}

{
constexpr std::size_t objectCount = 1;
/* [SceneData-populating-non-owned] */
constexpr struct Data {
    UnsignedShort mapping;
    Short parent;
    Matrix4 transformation;
} data[objectCount]{
    DOXYGEN_ELLIPSIS({})
};

Trade::SceneData scene{
    Trade::SceneMappingType::UnsignedShort, objectCount,
    Trade::DataFlag::Global, data, {
        Trade::SceneFieldData{Trade::SceneField::Parent,
            Containers::stridedArrayView(data).slice(&Data::mapping),
            Containers::stridedArrayView(data).slice(&Data::parent)},
        DOXYGEN_ELLIPSIS()
    }};
/* [SceneData-populating-non-owned] */
}

{
constexpr std::size_t objectCount = 1;
/* [SceneData-populating-offset-only] */
struct Data {
    UnsignedInt mapping;
    Int parent;
    Matrix4 transformation;
};

/* Layout defined statically */
constexpr Trade::SceneFieldData fields[]{
    Trade::SceneFieldData{Trade::SceneField::Parent, objectCount,
        Trade::SceneMappingType::UnsignedInt, offsetof(Data, mapping), sizeof(Data),
        Trade::SceneFieldType::Int, offsetof(Data, parent), sizeof(Data)},
    Trade::SceneFieldData{Trade::SceneField::Transformation, objectCount,
        Trade::SceneMappingType::UnsignedInt, offsetof(Data, mapping), sizeof(Data),
        Trade::SceneFieldType::Matrix4x4, offsetof(Data, transformation), sizeof(Data)}
};

/* Actual data populated later */
Containers::Array<char> data{objectCount*sizeof(Data)};
DOXYGEN_ELLIPSIS()

/* Using the statically defined field layout */
Trade::SceneData scene{Trade::SceneMappingType::UnsignedInt, objectCount,
    std::move(data), Trade::sceneFieldDataNonOwningArray(fields)};
/* [SceneData-populating-offset-only] */
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
constexpr Trade::SceneField SceneFieldCellFrustum = Trade::sceneFieldCustom(0);
constexpr Trade::SceneField SceneFieldCellLights = Trade::sceneFieldCustom(1);

Trade::SceneData scene{
    Trade::SceneMappingType::UnsignedShort, nodeCount + cellMapping.size(),
    std::move(data), {
        DOXYGEN_ELLIPSIS()
        Trade::SceneFieldData{SceneFieldCellFrustum, cellMapping, cellFrustums},
        Trade::SceneFieldData{SceneFieldCellLights, cellMapping, cellLights},
    }};
/* [SceneData-populating-custom2] */
}

{
constexpr Trade::SceneField SceneFieldCellFrustum = Trade::sceneFieldCustom(0);
constexpr Trade::SceneField SceneFieldCellLights = Trade::sceneFieldCustom(1);
Trade::SceneData scene{{}, 0, nullptr, nullptr};
/* [SceneData-populating-custom-retrieve] */
Containers::StridedArrayView1D<const Matrix4> cellFrustums =
    scene.field<Matrix4>(SceneFieldCellFrustum);
Containers::StridedArrayView2D<const Int> cellLights =
    scene.field<Int[]>(SceneFieldCellLights);
/* [SceneData-populating-custom-retrieve] */
static_cast<void>(cellFrustums);
static_cast<void>(cellLights);
}

{
using namespace Containers::Literals;
/* [SceneData-populating-strings] */
constexpr Containers::StringView CategoryStrings =
    "wall\0furniture\0lighting\0artwork"_s;
constexpr UnsignedByte CategoryWall = 0;
constexpr UnsignedByte CategoryFurniture = 5;
constexpr UnsignedByte CategoryLighting = 15;
constexpr UnsignedByte CategoryArtwork = 24;

Containers::MutableStringView categoryStrings;
Containers::ArrayView<UnsignedInt> mapping;
Containers::ArrayView<UnsignedByte> categories;
Containers::ArrayTuple data{
    {CategoryStrings.size(), categoryStrings},
    {DOXYGEN_ELLIPSIS(5), mapping},
    {DOXYGEN_ELLIPSIS(5), categories},
};

Utility::copy(CategoryStrings, categoryStrings);
mapping[0] = 7;
categories[0] = CategoryWall;
mapping[1] = 19;
categories[1] = CategoryFurniture;
DOXYGEN_ELLIPSIS(static_cast<void>(CategoryLighting); static_cast<void>(CategoryArtwork);)

constexpr Trade::SceneField SceneFieldCategory = Trade::sceneFieldCustom(25);
Trade::SceneData scene{Trade::SceneMappingType::UnsignedInt, DOXYGEN_ELLIPSIS(5), std::move(data), {
    Trade::SceneFieldData{SceneFieldCategory, mapping,
        categoryStrings.data(), Trade::SceneFieldType::StringRangeNullTerminated8,
        categories}
}};
/* [SceneData-populating-strings] */
}

{
constexpr Trade::SceneField SceneFieldCategory = Trade::sceneFieldCustom(25);
Trade::SceneData scene{{}, 0, nullptr, nullptr};
/* [SceneData-populating-strings-retrieve] */
Containers::StringIterable categories = scene.fieldStrings(SceneFieldCategory);

// Prints "furniture"
Debug{} << categories[scene.fieldObjectOffset(SceneFieldCategory, 19)];
/* [SceneData-populating-strings-retrieve] */
}

}
