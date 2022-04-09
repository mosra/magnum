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

#include "AnySceneImporter.h"

#include <Corrade/Containers/Optional.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/PluginManager/PluginMetadata.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/DebugStl.h> /* for PluginMetadata::name() */
#include <Corrade/Utility/Path.h>
#include <Corrade/Utility/String.h>

#include "Magnum/Trade/AnimationData.h"
#include "Magnum/Trade/CameraData.h"
#include "Magnum/Trade/ImageData.h"
#include "Magnum/Trade/LightData.h"
#include "Magnum/Trade/MaterialData.h"
#include "Magnum/Trade/MeshData.h"
#include "Magnum/Trade/SceneData.h"
#include "Magnum/Trade/SkinData.h"
#include "Magnum/Trade/TextureData.h"
#include "MagnumPlugins/Implementation/propagateConfiguration.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#define _MAGNUM_NO_DEPRECATED_MESHDATA /* So it doesn't yell here */
#define _MAGNUM_NO_DEPRECATED_OBJECTDATA /* So it doesn't yell here */

#include "Magnum/Trade/MeshData2D.h"
#include "Magnum/Trade/MeshData3D.h"
#include "Magnum/Trade/ObjectData2D.h"
#include "Magnum/Trade/ObjectData3D.h"
#endif

namespace Magnum { namespace Trade {

using namespace Containers::Literals;

AnySceneImporter::AnySceneImporter(PluginManager::Manager<AbstractImporter>& manager): AbstractImporter{manager} {}

AnySceneImporter::AnySceneImporter(PluginManager::AbstractManager& manager, const Containers::StringView& plugin): AbstractImporter{manager, plugin} {}

AnySceneImporter::~AnySceneImporter() = default;

ImporterFeatures AnySceneImporter::doFeatures() const {
    return ImporterFeature::FileCallback;
}

bool AnySceneImporter::doIsOpened() const { return !!_in; }

void AnySceneImporter::doClose() {
    _in = nullptr;
}

void AnySceneImporter::doOpenFile(const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /* We don't detect any double extensions yet, so we can normalize just the
       extension. In case we eventually might, it'd have to be split() instead
       to save at least by normalizing just the filename and not the path. */
    const Containers::String normalizedExtension = Utility::String::lowercase(Utility::Path::splitExtension(filename).second());

    /* Detect the plugin from extension */
    Containers::StringView plugin;
    if(normalizedExtension == ".3ds"_s ||
       normalizedExtension == ".ase"_s)
        plugin = "3dsImporter"_s;
    else if(normalizedExtension == ".3mf"_s)
        plugin = "3mfImporter"_s;
    else if(normalizedExtension == ".ac"_s)
        plugin = "Ac3dImporter"_s;
    else if(normalizedExtension == ".blend"_s)
        plugin = "BlenderImporter"_s;
    else if(normalizedExtension == ".bvh"_s)
        plugin = "BvhImporter"_s;
    else if(normalizedExtension == ".csm"_s)
        plugin = "CsmImporter"_s;
    else if(normalizedExtension == ".dae"_s)
        plugin = "ColladaImporter"_s;
    else if(normalizedExtension == ".x"_s)
        plugin = "DirectXImporter"_s;
    else if(normalizedExtension == ".dxf"_s)
        plugin = "DxfImporter"_s;
    else if(normalizedExtension == ".fbx"_s)
        plugin = "FbxImporter"_s;
    else if(normalizedExtension == ".gltf"_s ||
            normalizedExtension == ".glb"_s)
        plugin = "GltfImporter"_s;
    else if(normalizedExtension == ".ifc"_s)
        plugin = "IfcImporter"_s;
    else if(normalizedExtension == ".irrmesh"_s ||
            normalizedExtension == ".irr"_s)
        plugin = "IrrlichtImporter"_s;
    else if(normalizedExtension == ".lwo"_s ||
            normalizedExtension == ".lws"_s)
        plugin = "LightWaveImporter"_s;
    else if(normalizedExtension == ".lxo"_s)
        plugin = "ModoImporter"_s;
    else if(normalizedExtension == ".ms3d"_s)
        plugin = "MilkshapeImporter"_s;
    else if(normalizedExtension == ".obj"_s)
        plugin = "ObjImporter"_s;
    else if(normalizedExtension == ".xml"_s)
        plugin = "OgreImporter"_s;
    else if(normalizedExtension == ".ogex"_s)
        plugin = "OpenGexImporter"_s;
    else if(normalizedExtension == ".ply"_s)
        plugin = "StanfordImporter"_s;
    else if(normalizedExtension == ".stl"_s)
        plugin = "StlImporter"_s;
    else if(normalizedExtension == ".cob"_s ||
            normalizedExtension == ".scn"_s)
        plugin = "TrueSpaceImporter"_s;
    else if(normalizedExtension == ".3d"_s)
        plugin = "UnrealImporter"_s;
    else if(normalizedExtension == ".smd"_s ||
            normalizedExtension == ".vta"_s)
        plugin = "ValveImporter"_s;
    else if(normalizedExtension == ".xgl"_s ||
            normalizedExtension == ".zgl"_s)
        plugin = "XglImporter"_s;
    else {
        Error{} << "Trade::AnySceneImporter::openFile(): cannot determine the format of" << filename;
        return;
    }

    /* Try to load the plugin */
    if(!(manager()->load(plugin) & PluginManager::LoadState::Loaded)) {
        Error{} << "Trade::AnySceneImporter::openFile(): cannot load the" << plugin << "plugin";
        return;
    }

    const PluginManager::PluginMetadata* const metadata = manager()->metadata(plugin);
    CORRADE_INTERNAL_ASSERT(metadata);
    if(flags() & ImporterFlag::Verbose) {
        Debug d;
        d << "Trade::AnySceneImporter::openFile(): using" << plugin;
        if(plugin != metadata->name())
            d << "(provided by" << metadata->name() << Debug::nospace << ")";
    }

    /* Instantiate the plugin, propagate flags and the file callback, if set */
    Containers::Pointer<AbstractImporter> importer = static_cast<PluginManager::Manager<AbstractImporter>*>(manager())->instantiate(plugin);
    importer->setFlags(flags());
    if(fileCallback()) importer->setFileCallback(fileCallback(), fileCallbackUserData());

    /* Propagate configuration */
    Magnum::Implementation::propagateConfiguration("Trade::AnySceneImporter::openFile():", {}, metadata->name(), configuration(), importer->configuration());

    /* Try to open the file (error output should be printed by the plugin
       itself) */
    if(!importer->openFile(filename)) return;

    /* Success, save the instance */
    _in = std::move(importer);
}

UnsignedInt AnySceneImporter::doAnimationCount() const { return _in->animationCount(); }
Int AnySceneImporter::doAnimationForName(const Containers::StringView name) { return _in->animationForName(name); }
Containers::String AnySceneImporter::doAnimationName(const UnsignedInt id) { return _in->animationName(id); }
Containers::Optional<AnimationData> AnySceneImporter::doAnimation(const UnsignedInt id) { return _in->animation(id); }

Int AnySceneImporter::doDefaultScene() const { return _in->defaultScene(); }

UnsignedInt AnySceneImporter::doSceneCount() const { return _in->sceneCount(); }
UnsignedLong AnySceneImporter::doObjectCount() const { return _in->objectCount(); }
Int AnySceneImporter::doSceneForName(const Containers::StringView name) { return _in->sceneForName(name); }
Long AnySceneImporter::doObjectForName(const Containers::StringView name) { return _in->objectForName(name); }
Containers::String AnySceneImporter::doSceneName(const UnsignedInt id) { return _in->sceneName(id); }
Containers::String AnySceneImporter::doObjectName(const UnsignedLong id) { return _in->objectName(id); }
Containers::Optional<SceneData> AnySceneImporter::doScene(const UnsignedInt id) { return _in->scene(id); }

UnsignedInt AnySceneImporter::doLightCount() const { return _in->lightCount(); }
Int AnySceneImporter::doLightForName(const Containers::StringView name) { return _in->lightForName(name); }
Containers::String AnySceneImporter::doLightName(const UnsignedInt id) { return _in->lightName(id); }
Containers::Optional<LightData> AnySceneImporter::doLight(const UnsignedInt id) { return _in->light(id); }

UnsignedInt AnySceneImporter::doCameraCount() const { return _in->cameraCount(); }
Int AnySceneImporter::doCameraForName(const Containers::StringView name) { return _in->cameraForName(name); }
Containers::String AnySceneImporter::doCameraName(const UnsignedInt id) { return _in->cameraName(id); }
Containers::Optional<CameraData> AnySceneImporter::doCamera(const UnsignedInt id) { return _in->camera(id); }

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
UnsignedInt AnySceneImporter::doObject2DCount() const { return _in->object2DCount(); }
Int AnySceneImporter::doObject2DForName(const std::string& name) { return _in->object2DForName(name); }
std::string AnySceneImporter::doObject2DName(const UnsignedInt id) { return _in->object2DName(id); }
Containers::Pointer<ObjectData2D> AnySceneImporter::doObject2D(const UnsignedInt id) { return _in->object2D(id); }

UnsignedInt AnySceneImporter::doObject3DCount() const { return _in->object3DCount(); }
Int AnySceneImporter::doObject3DForName(const std::string& name) { return _in->object3DForName(name); }
std::string AnySceneImporter::doObject3DName(const UnsignedInt id) { return _in->object3DName(id); }
Containers::Pointer<ObjectData3D> AnySceneImporter::doObject3D(const UnsignedInt id) { return _in->object3D(id); }
CORRADE_IGNORE_DEPRECATED_POP
#endif

UnsignedInt AnySceneImporter::doSkin2DCount() const { return _in->skin2DCount(); }
Int AnySceneImporter::doSkin2DForName(const Containers::StringView name) { return _in->skin2DForName(name); }
Containers::String AnySceneImporter::doSkin2DName(const UnsignedInt id) { return _in->skin2DName(id); }
Containers::Optional<SkinData2D> AnySceneImporter::doSkin2D(const UnsignedInt id) { return _in->skin2D(id); }

UnsignedInt AnySceneImporter::doSkin3DCount() const { return _in->skin3DCount(); }
Int AnySceneImporter::doSkin3DForName(const Containers::StringView name) { return _in->skin3DForName(name); }
Containers::String AnySceneImporter::doSkin3DName(const UnsignedInt id) { return _in->skin3DName(id); }
Containers::Optional<SkinData3D> AnySceneImporter::doSkin3D(const UnsignedInt id) { return _in->skin3D(id); }

UnsignedInt AnySceneImporter::doMeshCount() const { return _in->meshCount(); }
Int AnySceneImporter::doMeshForName(const Containers::StringView name) { return _in->meshForName(name); }
Containers::String AnySceneImporter::doMeshName(const UnsignedInt id) { return _in->meshName(id); }
Containers::Optional<MeshData> AnySceneImporter::doMesh(const UnsignedInt id, const UnsignedInt level) { return _in->mesh(id, level); }

MeshAttribute AnySceneImporter::doMeshAttributeForName(const Containers::StringView name) { return _in->meshAttributeForName(name); }
Containers::String AnySceneImporter::doMeshAttributeName(const UnsignedShort id) { return _in->meshAttributeName(meshAttributeCustom(id)); }

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
UnsignedInt AnySceneImporter::doMesh2DCount() const { return _in->mesh2DCount(); }
Int AnySceneImporter::doMesh2DForName(const std::string& name) { return _in->mesh2DForName(name); }
std::string AnySceneImporter::doMesh2DName(const UnsignedInt id) { return _in->mesh2DName(id); }
Containers::Optional<MeshData2D> AnySceneImporter::doMesh2D(const UnsignedInt id) { return _in->mesh2D(id); }

UnsignedInt AnySceneImporter::doMesh3DCount() const { return _in->mesh3DCount(); }
Int AnySceneImporter::doMesh3DForName(const std::string& name) { return _in->mesh3DForName(name); }
std::string AnySceneImporter::doMesh3DName(const UnsignedInt id) { return _in->mesh3DName(id); }
Containers::Optional<MeshData3D> AnySceneImporter::doMesh3D(const UnsignedInt id) { return _in->mesh3D(id); }
CORRADE_IGNORE_DEPRECATED_POP
#endif

UnsignedInt AnySceneImporter::doMaterialCount() const { return _in->materialCount(); }
Int AnySceneImporter::doMaterialForName(const Containers::StringView name) { return _in->materialForName(name); }
Containers::String AnySceneImporter::doMaterialName(const UnsignedInt id) { return _in->materialName(id); }
Containers::Optional<MaterialData> AnySceneImporter::doMaterial(const UnsignedInt id) { return _in->material(id); }

UnsignedInt AnySceneImporter::doTextureCount() const { return _in->textureCount(); }
Int AnySceneImporter::doTextureForName(const Containers::StringView name) { return _in->textureForName(name); }
Containers::String AnySceneImporter::doTextureName(const UnsignedInt id) { return _in->textureName(id); }
Containers::Optional<TextureData> AnySceneImporter::doTexture(const UnsignedInt id) { return _in->texture(id); }

UnsignedInt AnySceneImporter::doImage1DCount() const { return _in->image1DCount(); }
UnsignedInt AnySceneImporter::doImage1DLevelCount(UnsignedInt id) { return _in->image1DLevelCount(id); }
Int AnySceneImporter::doImage1DForName(const Containers::StringView name) { return _in->image1DForName(name); }
Containers::String AnySceneImporter::doImage1DName(const UnsignedInt id) { return _in->image1DName(id); }
Containers::Optional<ImageData1D> AnySceneImporter::doImage1D(const UnsignedInt id, const UnsignedInt level) { return _in->image1D(id, level); }

UnsignedInt AnySceneImporter::doImage2DCount() const { return _in->image2DCount(); }
UnsignedInt AnySceneImporter::doImage2DLevelCount(UnsignedInt id) { return _in->image2DLevelCount(id); }
Int AnySceneImporter::doImage2DForName(const Containers::StringView name) { return _in->image2DForName(name); }
Containers::String AnySceneImporter::doImage2DName(const UnsignedInt id) { return _in->image2DName(id); }
Containers::Optional<ImageData2D> AnySceneImporter::doImage2D(const UnsignedInt id, const UnsignedInt level) { return _in->image2D(id, level); }

UnsignedInt AnySceneImporter::doImage3DCount() const { return _in->image3DCount(); }
UnsignedInt AnySceneImporter::doImage3DLevelCount(UnsignedInt id) { return _in->image3DLevelCount(id); }
Int AnySceneImporter::doImage3DForName(const Containers::StringView name) { return _in->image3DForName(name); }
Containers::String AnySceneImporter::doImage3DName(const UnsignedInt id) { return _in->image3DName(id); }
Containers::Optional<ImageData3D> AnySceneImporter::doImage3D(const UnsignedInt id, const UnsignedInt level) { return _in->image3D(id, level); }

}}

CORRADE_PLUGIN_REGISTER(AnySceneImporter, Magnum::Trade::AnySceneImporter,
    "cz.mosra.magnum.Trade.AbstractImporter/0.5")
