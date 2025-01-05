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

#include "AnySceneConverter.h"

#include <Corrade/Containers/Iterable.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/PluginManager/PluginMetadata.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Macros.h> /* CORRADE_UNUSED */
#include <Corrade/Utility/Path.h>
#include <Corrade/Utility/String.h> /* lowercase() */

#include "Magnum/Trade/MeshData.h"
#include "Magnum/Trade/SceneData.h"
#include "MagnumPlugins/Implementation/propagateConfiguration.h"

namespace Magnum { namespace Trade {

using namespace Containers::Literals;

AnySceneConverter::AnySceneConverter(PluginManager::Manager<AbstractSceneConverter>& manager): AbstractSceneConverter{manager} {}

AnySceneConverter::AnySceneConverter(PluginManager::AbstractManager& manager, const Containers::StringView& plugin): AbstractSceneConverter{manager, plugin} {}

AnySceneConverter::~AnySceneConverter() = default;

SceneConverterFeatures AnySceneConverter::doFeatures() const {
    /* Report that we can convert meshes and scenes to files, because that the
       plugin can do always as it dispatches there. But everything else is
       reported only once an actual converter plugin is loaded -- i.e., after beginFile() is called.

       Additionally, to allow using beginFile() + add(MeshData) + endFile()
       with converters that only support ConvertMeshToFile, we "fake" report
       AddMesh supported as well. If we wouldn't, the base class would try to
       go through convertToFile() instead, causing the mesh to not be delegated
       to _converter->add() but instead supplied to a whole new plugin
       instance. Then, upon delegating to _converter->endFile(), it would fail
       due to having 0 meshes. */

    SceneConverterFeatures delegatedFeatures =
        _converter ? _converter->features() : SceneConverterFeatures{};
    if(delegatedFeatures & SceneConverterFeature::ConvertMeshToFile)
        delegatedFeatures |= SceneConverterFeature::AddMeshes;

    return SceneConverterFeature::ConvertMeshToFile|
           SceneConverterFeature::ConvertMultipleToFile|
           delegatedFeatures;
}

bool AnySceneConverter::doConvertToFile(const MeshData& mesh, const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /* We don't detect any double extensions yet, so we can normalize just the
       extension. In case we eventually might, it'd have to be split() instead
       to save at least by normalizing just the filename and not the path. */
    const Containers::String normalizedExtension = Utility::String::lowercase(Utility::Path::splitExtension(filename).second());

    /* Detect the plugin from extension */
    Containers::StringView plugin;
    if(normalizedExtension == ".gltf"_s ||
       normalizedExtension == ".glb"_s)
        plugin = "GltfSceneConverter"_s;
    else if(normalizedExtension == ".ply"_s)
        plugin = "StanfordSceneConverter"_s;
    else {
        Error{} << "Trade::AnySceneConverter::convertToFile(): cannot determine the format of" << filename;
        return false;
    }

    /* Try to load the plugin */
    if(!(manager()->load(plugin) & PluginManager::LoadState::Loaded)) {
        Error{} << "Trade::AnySceneConverter::convertToFile(): cannot load the" << plugin << "plugin";
        return false;
    }

    const PluginManager::PluginMetadata* const metadata = manager()->metadata(plugin);
    CORRADE_INTERNAL_ASSERT(metadata);
    if(flags() & SceneConverterFlag::Verbose) {
        Debug d;
        d << "Trade::AnySceneConverter::convertToFile(): using" << plugin;
        if(plugin != metadata->name())
            d << "(provided by" << metadata->name() << Debug::nospace << ")";
    }

    /* Instantiate the plugin, propagate flags */
    Containers::Pointer<AbstractSceneConverter> converter = static_cast<PluginManager::Manager<AbstractSceneConverter>*>(manager())->instantiate(plugin);
    converter->setFlags(flags());

    /* Propagate configuration */
    Magnum::Implementation::propagateConfiguration("Trade::AnySceneConverter::convertToFile():", {}, metadata->name(), configuration(), converter->configuration(), !(flags() & SceneConverterFlag::Quiet));

    /* Try to convert the file (error output should be printed by the plugin
       itself) */
    return converter->convertToFile(mesh, filename);
}

void AnySceneConverter::doAbort() {
    _converter->abort();
    _converter = {};
}

bool AnySceneConverter::doBeginFile(const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /* We don't detect any double extensions yet, so we can normalize just the
       extension. In case we eventually might, it'd have to be split() instead
       to save at least by normalizing just the filename and not the path. */
    const Containers::String normalizedExtension = Utility::String::lowercase(Utility::Path::splitExtension(filename).second());

    /* Detect the plugin from extension */
    Containers::StringView plugin;
    if(normalizedExtension == ".gltf"_s ||
       normalizedExtension == ".glb"_s)
        plugin = "GltfSceneConverter"_s;
    else if(normalizedExtension == ".ply"_s)
        plugin = "StanfordSceneConverter"_s;
    else {
        Error{} << "Trade::AnySceneConverter::beginFile(): cannot determine the format of" << filename;
        return false;
    }

    /* Try to load the plugin */
    if(!(manager()->load(plugin) & PluginManager::LoadState::Loaded)) {
        Error{} << "Trade::AnySceneConverter::beginFile(): cannot load the" << plugin << "plugin";
        return false;
    }

    const PluginManager::PluginMetadata* const metadata = manager()->metadata(plugin);
    CORRADE_INTERNAL_ASSERT(metadata);
    if(flags() & SceneConverterFlag::Verbose) {
        Debug d;
        d << "Trade::AnySceneConverter::beginFile(): using" << plugin;
        if(plugin != metadata->name())
            d << "(provided by" << metadata->name() << Debug::nospace << ")";
    }

    /* Instantiate the plugin, propagate flags */
    Containers::Pointer<AbstractSceneConverter> converter = static_cast<PluginManager::Manager<AbstractSceneConverter>*>(manager())->instantiate(plugin);
    converter->setFlags(flags());

    /* Propagate configuration */
    Magnum::Implementation::propagateConfiguration("Trade::AnySceneConverter::beginFile():", {}, metadata->name(), configuration(), converter->configuration(), !(flags() & SceneConverterFlag::Quiet));

    /* Try to begin the file (error output should be printed by the plugin
       itself) */
    if(!converter->beginFile(filename)) return false;

    /* Success, save the instance */
    _converter = Utility::move(converter);
    return true;
}

bool AnySceneConverter::doEndFile(Containers::StringView) {
    /* Destroy the converter instance after the operation finishes to avoid
       keeping now-useless state around */
    const bool out = _converter->endFile();
    _converter = {};
    return out;
}

/* It's easier to use CORRADE_UNUSED than to have #ifndef CORRADE_NO_ASSERT */

bool AnySceneConverter::doAdd(CORRADE_UNUSED const UnsignedInt id, const SceneData& scene, const Containers::StringView name) {
    CORRADE_INTERNAL_ASSERT(id == _converter->sceneCount());
    return !!_converter->add(scene, name);
}

void AnySceneConverter::doSetSceneFieldName(const SceneField field, const Containers::StringView name) {
    return _converter->setSceneFieldName(field, name);
}

void AnySceneConverter::doSetObjectName(const UnsignedLong object, const Containers::StringView name) {
    return _converter->setObjectName(object, name);
}

void AnySceneConverter::doSetDefaultScene(const UnsignedInt id) {
    return _converter->setDefaultScene(id);
}

bool AnySceneConverter::doAdd(CORRADE_UNUSED const UnsignedInt id, const AnimationData& animation, const Containers::StringView name) {
    CORRADE_INTERNAL_ASSERT(id == _converter->animationCount());
    return !!_converter->add(animation, name);
}

void AnySceneConverter::doSetAnimationTrackTargetName(const AnimationTrackTarget target, const Containers::StringView name) {
    return _converter->setAnimationTrackTargetName(target, name);
}

bool AnySceneConverter::doAdd(CORRADE_UNUSED const UnsignedInt id, const LightData& light, const Containers::StringView name) {
    CORRADE_INTERNAL_ASSERT(id == _converter->lightCount());
    return !!_converter->add(light, name);
}

bool AnySceneConverter::doAdd(CORRADE_UNUSED const UnsignedInt id, const CameraData& camera, const Containers::StringView name) {
    CORRADE_INTERNAL_ASSERT(id == _converter->cameraCount());
    return !!_converter->add(camera, name);
}

bool AnySceneConverter::doAdd(CORRADE_UNUSED const UnsignedInt id, const SkinData2D& skin, const Containers::StringView name) {
    CORRADE_INTERNAL_ASSERT(id == _converter->skin2DCount());
    return !!_converter->add(skin, name);
}

bool AnySceneConverter::doAdd(CORRADE_UNUSED const UnsignedInt id, const SkinData3D& skin, const Containers::StringView name) {
    CORRADE_INTERNAL_ASSERT(id == _converter->skin3DCount());
    return !!_converter->add(skin, name);
}

bool AnySceneConverter::doAdd(CORRADE_UNUSED const UnsignedInt id, const MeshData& mesh, const Containers::StringView name) {
    CORRADE_INTERNAL_ASSERT(id == _converter->meshCount());
    return !!_converter->add(mesh, name);
}

bool AnySceneConverter::doAdd(CORRADE_UNUSED const UnsignedInt id, const Containers::Iterable<const MeshData>& meshLevels, const Containers::StringView name) {
    CORRADE_INTERNAL_ASSERT(id == _converter->meshCount());
    return !!_converter->add(meshLevels, name);
}

void AnySceneConverter::doSetMeshAttributeName(const MeshAttribute attribute, const Containers::StringView name) {
    return _converter->setMeshAttributeName(attribute, name);
}

bool AnySceneConverter::doAdd(CORRADE_UNUSED const UnsignedInt id, const MaterialData& material, const Containers::StringView name) {
    CORRADE_INTERNAL_ASSERT(id == _converter->materialCount());
    return !!_converter->add(material, name);
}

bool AnySceneConverter::doAdd(CORRADE_UNUSED const UnsignedInt id, const TextureData& texture, const Containers::StringView name) {
    CORRADE_INTERNAL_ASSERT(id == _converter->textureCount());
    return !!_converter->add(texture, name);
}

bool AnySceneConverter::doAdd(CORRADE_UNUSED const UnsignedInt id, const ImageData1D& image, const Containers::StringView name) {
    CORRADE_INTERNAL_ASSERT(id == _converter->image1DCount());
    return !!_converter->add(image, name);
}

bool AnySceneConverter::doAdd(CORRADE_UNUSED const UnsignedInt id, const Containers::Iterable<const ImageData1D>& imageLevels, const Containers::StringView name) {
    CORRADE_INTERNAL_ASSERT(id == _converter->image1DCount());
    return !!_converter->add(imageLevels, name);
}

bool AnySceneConverter::doAdd(CORRADE_UNUSED const UnsignedInt id, const ImageData2D& image, const Containers::StringView name) {
    CORRADE_INTERNAL_ASSERT(id == _converter->image2DCount());
    return !!_converter->add(image, name);
}

bool AnySceneConverter::doAdd(CORRADE_UNUSED const UnsignedInt id, const Containers::Iterable<const ImageData2D>& imageLevels, const Containers::StringView name) {
    CORRADE_INTERNAL_ASSERT(id == _converter->image2DCount());
    return !!_converter->add(imageLevels, name);
}

bool AnySceneConverter::doAdd(CORRADE_UNUSED const UnsignedInt id, const ImageData3D& image, const Containers::StringView name) {
    CORRADE_INTERNAL_ASSERT(id == _converter->image3DCount());
    return !!_converter->add(image, name);
}

bool AnySceneConverter::doAdd(CORRADE_UNUSED const UnsignedInt id, const Containers::Iterable<const ImageData3D>& imageLevels, const Containers::StringView name) {
    CORRADE_INTERNAL_ASSERT(id == _converter->image3DCount());
    return !!_converter->add(imageLevels, name);
}

}}

CORRADE_PLUGIN_REGISTER(AnySceneConverter, Magnum::Trade::AnySceneConverter,
    MAGNUM_TRADE_ABSTRACTSCENECONVERTER_PLUGIN_INTERFACE)
