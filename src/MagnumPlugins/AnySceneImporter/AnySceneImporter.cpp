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

#include "AnySceneImporter.h"

#include <Corrade/Containers/Optional.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/String.h>

#include "Magnum/Trade/AbstractMaterialData.h"
#include "Magnum/Trade/AnimationData.h"
#include "Magnum/Trade/CameraData.h"
#include "Magnum/Trade/ImageData.h"
#include "Magnum/Trade/LightData.h"
#include "Magnum/Trade/MeshData2D.h"
#include "Magnum/Trade/MeshData3D.h"
#include "Magnum/Trade/ObjectData2D.h"
#include "Magnum/Trade/ObjectData3D.h"
#include "Magnum/Trade/SceneData.h"
#include "Magnum/Trade/TextureData.h"

namespace Magnum { namespace Trade {

AnySceneImporter::AnySceneImporter(PluginManager::Manager<AbstractImporter>& manager): AbstractImporter{manager} {}

AnySceneImporter::AnySceneImporter(PluginManager::AbstractManager& manager, const std::string& plugin): AbstractImporter{manager, plugin} {}

AnySceneImporter::~AnySceneImporter() = default;

auto AnySceneImporter::doFeatures() const -> Features { return {}; }

bool AnySceneImporter::doIsOpened() const { return !!_in; }

void AnySceneImporter::doClose() {
    _in = nullptr;
}

void AnySceneImporter::doOpenFile(const std::string& filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /** @todo lowercase only the extension, once Directory::split() is done */
    const std::string normalized = Utility::String::lowercase(filename);

    /* Detect type from extension */
    std::string plugin;
    if(Utility::String::endsWith(normalized, ".3ds") ||
       Utility::String::endsWith(normalized, ".ase"))
        plugin = "3dsImporter";
    else if(Utility::String::endsWith(normalized, ".ac"))
        plugin = "Ac3dImporter";
    else if(Utility::String::endsWith(normalized, ".blend"))
        plugin = "BlenderImporter";
    else if(Utility::String::endsWith(normalized, ".bvh"))
        plugin = "BvhImporter";
    else if(Utility::String::endsWith(normalized, ".csm"))
        plugin = "CsmImporter";
    else if(Utility::String::endsWith(normalized, ".dae"))
        plugin = "ColladaImporter";
    else if(Utility::String::endsWith(normalized, ".x"))
        plugin = "DirectXImporter";
    else if(Utility::String::endsWith(normalized, ".dxf"))
        plugin = "DxfImporter";
    else if(Utility::String::endsWith(normalized, ".fbx"))
        plugin = "FbxImporter";
    else if(Utility::String::endsWith(normalized, ".gltf"))
        plugin = "GltfImporter";
    else if(Utility::String::endsWith(normalized, ".glb"))
        plugin = "GlbImporter";
    else if(Utility::String::endsWith(normalized, ".ifc"))
        plugin = "IfcImporter";
    else if(Utility::String::endsWith(normalized, ".irrmesh") ||
            Utility::String::endsWith(normalized, ".irr"))
        plugin = "IrrlichtImporter";
    else if(Utility::String::endsWith(normalized, ".lwo") ||
            Utility::String::endsWith(normalized, ".lws"))
        plugin = "LightWaveImporter";
    else if(Utility::String::endsWith(normalized, ".lxo"))
        plugin = "ModoImporter";
    else if(Utility::String::endsWith(normalized, ".ms3d"))
        plugin = "MilkshapeImporter";
    else if(Utility::String::endsWith(normalized, ".obj"))
        plugin = "ObjImporter";
    else if(Utility::String::endsWith(normalized, ".xml"))
        plugin = "OgreImporter";
    else if(Utility::String::endsWith(normalized, ".ogex"))
        plugin = "OpenGexImporter";
    else if(Utility::String::endsWith(normalized, ".ply"))
        plugin = "StanfordImporter";
    else if(Utility::String::endsWith(normalized, ".stl"))
        plugin = "StlImporter";
    else if(Utility::String::endsWith(normalized, ".cob") ||
            Utility::String::endsWith(normalized, ".scn"))
        plugin = "TrueSpaceImporter";
    else if(Utility::String::endsWith(normalized, ".3d"))
        plugin = "UnrealImporter";
    else if(Utility::String::endsWith(normalized, ".smd") ||
            Utility::String::endsWith(normalized, ".vta"))
        plugin = "ValveImporter";
    else if(Utility::String::endsWith(normalized, ".xgl") ||
            Utility::String::endsWith(normalized, ".zgl"))
        plugin = "XglImporter";
    else {
        Error() << "Trade::AnySceneImporter::openFile(): cannot determine type of file" << filename;
        return;
    }

    /* Try to load the plugin */
    if(!(manager()->load(plugin) & PluginManager::LoadState::Loaded)) {
        Error() << "Trade::AnySceneImporter::openFile(): cannot load" << plugin << "plugin";
        return;
    }

    /* Try to open the file (error output should be printed by the plugin
       itself) */
    Containers::Pointer<AbstractImporter> importer = static_cast<PluginManager::Manager<AbstractImporter>*>(manager())->instantiate(plugin);
    if(!importer->openFile(filename)) return;

    /* Success, save the instance */
    _in = std::move(importer);
}

UnsignedInt AnySceneImporter::doAnimationCount() const { return _in->animationCount(); }
Int AnySceneImporter::doAnimationForName(const std::string& name) { return _in->animationForName(name); }
std::string AnySceneImporter::doAnimationName(const UnsignedInt id) { return _in->animationName(id); }
Containers::Optional<AnimationData> AnySceneImporter::doAnimation(const UnsignedInt id) { return _in->animation(id); }

Int AnySceneImporter::doDefaultScene() { return _in->defaultScene(); }

UnsignedInt AnySceneImporter::doSceneCount() const { return _in->sceneCount(); }
Int AnySceneImporter::doSceneForName(const std::string& name) { return _in->sceneForName(name); }
std::string AnySceneImporter::doSceneName(const UnsignedInt id) { return _in->sceneName(id); }
Containers::Optional<SceneData> AnySceneImporter::doScene(const UnsignedInt id) { return _in->scene(id); }

UnsignedInt AnySceneImporter::doLightCount() const { return _in->lightCount(); }
Int AnySceneImporter::doLightForName(const std::string& name) { return _in->lightForName(name); }
std::string AnySceneImporter::doLightName(const UnsignedInt id) { return _in->lightName(id); }
Containers::Optional<LightData> AnySceneImporter::doLight(const UnsignedInt id) { return _in->light(id); }

UnsignedInt AnySceneImporter::doCameraCount() const { return _in->cameraCount(); }
Int AnySceneImporter::doCameraForName(const std::string& name) { return _in->cameraForName(name); }
std::string AnySceneImporter::doCameraName(const UnsignedInt id) { return _in->cameraName(id); }
Containers::Optional<CameraData> AnySceneImporter::doCamera(const UnsignedInt id) { return _in->camera(id); }

UnsignedInt AnySceneImporter::doObject2DCount() const { return _in->object2DCount(); }
Int AnySceneImporter::doObject2DForName(const std::string& name) { return _in->object2DForName(name); }
std::string AnySceneImporter::doObject2DName(const UnsignedInt id) { return _in->object2DName(id); }
Containers::Pointer<ObjectData2D> AnySceneImporter::doObject2D(const UnsignedInt id) { return _in->object2D(id); }

UnsignedInt AnySceneImporter::doObject3DCount() const { return _in->object3DCount(); }
Int AnySceneImporter::doObject3DForName(const std::string& name) { return _in->object3DForName(name); }
std::string AnySceneImporter::doObject3DName(const UnsignedInt id) { return _in->object3DName(id); }
Containers::Pointer<ObjectData3D> AnySceneImporter::doObject3D(const UnsignedInt id) { return _in->object3D(id); }

UnsignedInt AnySceneImporter::doMesh2DCount() const { return _in->mesh2DCount(); }
Int AnySceneImporter::doMesh2DForName(const std::string& name) { return _in->mesh2DForName(name); }
std::string AnySceneImporter::doMesh2DName(const UnsignedInt id) { return _in->mesh2DName(id); }
Containers::Optional<MeshData2D> AnySceneImporter::doMesh2D(const UnsignedInt id) { return _in->mesh2D(id); }

UnsignedInt AnySceneImporter::doMesh3DCount() const { return _in->mesh3DCount(); }
Int AnySceneImporter::doMesh3DForName(const std::string& name) { return _in->mesh3DForName(name); }
std::string AnySceneImporter::doMesh3DName(const UnsignedInt id) { return _in->mesh3DName(id); }
Containers::Optional<MeshData3D> AnySceneImporter::doMesh3D(const UnsignedInt id) { return _in->mesh3D(id); }

UnsignedInt AnySceneImporter::doMaterialCount() const { return _in->materialCount(); }
Int AnySceneImporter::doMaterialForName(const std::string& name) { return _in->materialForName(name); }
std::string AnySceneImporter::doMaterialName(const UnsignedInt id) { return _in->materialName(id); }
Containers::Pointer<AbstractMaterialData> AnySceneImporter::doMaterial(const UnsignedInt id) { return _in->material(id); }

UnsignedInt AnySceneImporter::doTextureCount() const { return _in->textureCount(); }
Int AnySceneImporter::doTextureForName(const std::string& name) { return _in->textureForName(name); }
std::string AnySceneImporter::doTextureName(const UnsignedInt id) { return _in->textureName(id); }
Containers::Optional<TextureData> AnySceneImporter::doTexture(const UnsignedInt id) { return _in->texture(id); }

UnsignedInt AnySceneImporter::doImage1DCount() const { return _in->image1DCount(); }
Int AnySceneImporter::doImage1DForName(const std::string& name) { return _in->image1DForName(name); }
std::string AnySceneImporter::doImage1DName(const UnsignedInt id) { return _in->image1DName(id); }
Containers::Optional<ImageData1D> AnySceneImporter::doImage1D(const UnsignedInt id) { return _in->image1D(id); }

UnsignedInt AnySceneImporter::doImage2DCount() const { return _in->image2DCount(); }
Int AnySceneImporter::doImage2DForName(const std::string& name) { return _in->image2DForName(name); }
std::string AnySceneImporter::doImage2DName(const UnsignedInt id) { return _in->image2DName(id); }
Containers::Optional<ImageData2D> AnySceneImporter::doImage2D(const UnsignedInt id) { return _in->image2D(id); }

UnsignedInt AnySceneImporter::doImage3DCount() const { return _in->image3DCount(); }
Int AnySceneImporter::doImage3DForName(const std::string& name) { return _in->image3DForName(name); }
std::string AnySceneImporter::doImage3DName(const UnsignedInt id) { return _in->image3DName(id); }
Containers::Optional<ImageData3D> AnySceneImporter::doImage3D(const UnsignedInt id) { return _in->image3D(id); }

}}

CORRADE_PLUGIN_REGISTER(AnySceneImporter, Magnum::Trade::AnySceneImporter,
    "cz.mosra.magnum.Trade.AbstractImporter/0.3")
