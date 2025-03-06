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

#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StringIterable.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/ConfigurationGroup.h>
#include <Corrade/Utility/Format.h>
#include <Corrade/Utility/Path.h>

#include "Magnum/Math/Vector3.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/AnimationData.h"
#include "Magnum/Trade/CameraData.h"
#include "Magnum/Trade/ImageData.h"
#include "Magnum/Trade/LightData.h"
#include "Magnum/Trade/MaterialData.h"
#include "Magnum/Trade/MeshData.h"
#include "Magnum/Trade/SceneData.h"
#include "Magnum/Trade/SkinData.h"
#include "Magnum/Trade/TextureData.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/DebugStl.h>

#define _MAGNUM_NO_DEPRECATED_MESHDATA /* So it doesn't yell here */
#define _MAGNUM_NO_DEPRECATED_OBJECTDATA

#include "Magnum/Trade/MeshData3D.h"
#include "Magnum/Trade/ObjectData3D.h"
#endif

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct AnySceneImporterTest: TestSuite::Tester {
    explicit AnySceneImporterTest();

    void load();
    void detect();
    void reject();

    void unknown();

    void propagateFlags();
    void propagateConfiguration();
    void propagateConfigurationUnknown();
    void propagateConfigurationUnknownInEmptySubgroup();
    void propagateFileCallback();

    void animations();
    void animationTrackTargetNameNoFileOpened();

    void scenes();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void scenesDeprecated2D();
    void scenesDeprecated3D();
    #endif
    void sceneFieldNameNoFileOpened();

    void lights();
    void cameras();
    void skins2D();
    void skins3D();

    void meshes();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void meshesDeprecated2D();
    void meshesDeprecated3D();
    #endif
    void meshLevels();
    void meshAttributeNameNoFileOpened();

    void materials();
    void textures();
    void images1D();
    void images2D();
    void images3D();
    void imageLevels1D();
    void imageLevels2D();
    void imageLevels3D();

    /* Explicitly forbid system-wide plugin dependencies */
    PluginManager::Manager<AbstractImporter> _manager{"nonexistent"};
};

const struct {
    const char* name;
    Containers::String filename;
} LoadData[]{
    {"OBJ", Utility::Path::join(OBJIMPORTER_TEST_DIR, "mesh-multiple.obj")},
};

const struct {
    const char* name;
    const char* filename;
    const char* plugin;
} DetectData[]{
    /* Try to keep the order the same as in the documentation, and use all
       variants if there are */
    {"3ds Max", "autodesk.3ds", "3dsImporter"},
    {"3ds Max ASE", "autodesk.ase", "3dsImporter"},
    {"3MF", "print.3mf", "3mfImporter"},
    {"AC3D", "file.ac", "Ac3dImporter"},
    {"Blender", "suzanne.blend", "BlenderImporter"},
    {"Biovision BVH", "scene.bvh", "BvhImporter"},
    {"CharacterStudio Motion", "motion.csm", "CsmImporter"},
    {"COLLADA", "xml.dae", "ColladaImporter"},
    {"DirectX X", "microsoft.x", "DirectXImporter"},
    {"AutoCAD DXF", "autodesk.dxf", "DxfImporter"},
    {"FBX", "autodesk.fbx", "FbxImporter"},
    {"glTF", "khronos.gltf", "GltfImporter"},
    {"glTF binary", "khronos.glb", "GltfImporter"},
    {"VRM", "humanoid.vrm", "GltfImporter"},
    {"IFC", "step.ifc", "IfcImporter"},
    {"Irrlicht", "venerable.irr", "IrrlichtImporter"},
    {"Irrlicht Mesh", "venerable.irrmesh", "IrrlichtImporter"},
    {"LightWave", "magnum.lwo", "LightWaveImporter"},
    {"LightWave Scene", "magnum.lws", "LightWaveImporter"},
    {"Modo", "magnum.lxo", "ModoImporter"},
    {"Milkshape 3D", "latte.ms3d", "MilkshapeImporter"},
    {"Ogre XML", "weapon.mesh.xml", "OgreImporter"},
    {"OpenGEX", "eric.ogex", "OpenGexImporter"},
    {"Stanford PLY", "bunny.ply", "StanfordImporter"},
    {"Stanford PLY uppercase", "ARMADI~1.PLY", "StanfordImporter"},
    {"STL", "robot.stl", "StlImporter"},
    {"TrueSpace COB", "huh.cob", "TrueSpaceImporter"},
    {"TrueSpace SCN", "huh.scn", "TrueSpaceImporter"},
    {"USD", "model.usd", "UsdImporter"},
    {"USD ASCII", "model.usda", "UsdImporter"},
    {"USD binary", "model.usdc", "UsdImporter"},
    {"USD zipped", "model.usdz", "UsdImporter"},
    {"Unreal", "tournament.3d", "UnrealImporter"},
    {"Valve Model SMD", "hl3.smd", "ValveImporter"},
    {"Valve Model VTA", "hl3.vta", "ValveImporter"},
    {"XGL", "thingy.xgl", "XglImporter"},
    {"XGL compressed", "thingy.zgl", "XglImporter"},
};

const struct {
    const char* name;
    const char* filename;
} RejectData[]{
    /* This lists pairs of filenames where, just based on extension, any
       detection cannot be done */
    {"COLLADA with a *.xml extension", "collada.xml"},
    {"OGRE XML with just a *.xml extension", "mesh.xml"},
    {"OGRE *.mesh", "ogre.mesh"},
    {"Meshwork *.mesh", "foo.mesh"},
    {"OBJ-like *.ter file", "terrain.ter"},
    {"Terragen *.ter", "terragen.ter"},
    {"Quake 1 *.mdl", "quake.mdl"},
    {"3D Game Studio (3DGS) *.mdl", "3dgs.mdl"},
};

const struct {
    const char* name;
    ImporterFlags flags;
    bool quiet;
} PropagateConfigurationUnknownData[]{
    {"", {}, false},
    {"quiet", ImporterFlag::Quiet, true}
};

AnySceneImporterTest::AnySceneImporterTest() {
    addInstancedTests({&AnySceneImporterTest::load},
        Containers::arraySize(LoadData));

    addInstancedTests({&AnySceneImporterTest::detect},
        Containers::arraySize(DetectData));

    addInstancedTests({&AnySceneImporterTest::reject},
        Containers::arraySize(RejectData));

    addTests({&AnySceneImporterTest::unknown,

              &AnySceneImporterTest::propagateFlags,
              &AnySceneImporterTest::propagateConfiguration});

    addInstancedTests({&AnySceneImporterTest::propagateConfigurationUnknown},
        Containers::arraySize(PropagateConfigurationUnknownData));

    addTests({&AnySceneImporterTest::propagateConfigurationUnknownInEmptySubgroup,
              &AnySceneImporterTest::propagateFileCallback,

              &AnySceneImporterTest::animations,
              &AnySceneImporterTest::animationTrackTargetNameNoFileOpened,

              &AnySceneImporterTest::scenes,
              #ifdef MAGNUM_BUILD_DEPRECATED
              &AnySceneImporterTest::scenesDeprecated2D,
              &AnySceneImporterTest::scenesDeprecated3D,
              #endif
              &AnySceneImporterTest::sceneFieldNameNoFileOpened,

              &AnySceneImporterTest::lights,
              &AnySceneImporterTest::cameras,
              &AnySceneImporterTest::skins2D,
              &AnySceneImporterTest::skins3D,

              &AnySceneImporterTest::meshes,
              #ifdef MAGNUM_BUILD_DEPRECATED
              &AnySceneImporterTest::meshesDeprecated2D,
              &AnySceneImporterTest::meshesDeprecated3D,
              #endif
              &AnySceneImporterTest::meshLevels,
              &AnySceneImporterTest::meshAttributeNameNoFileOpened,

              &AnySceneImporterTest::materials,
              &AnySceneImporterTest::textures,
              &AnySceneImporterTest::images1D,
              &AnySceneImporterTest::images2D,
              &AnySceneImporterTest::images3D,
              &AnySceneImporterTest::imageLevels1D,
              &AnySceneImporterTest::imageLevels2D,
              &AnySceneImporterTest::imageLevels3D});

    /* Load the plugin directly from the build tree. Otherwise it's static and
       already loaded. */
    #ifdef ANYSCENEIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(ANYSCENEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    /* Optional plugins that don't have to be here */
    #ifdef OBJIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(OBJIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
}

void AnySceneImporterTest::load() {
    auto&& data = LoadData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!(_manager.loadState("ObjImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("ObjImporter plugin not enabled, cannot test");

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnySceneImporter");
    CORRADE_VERIFY(importer->openFile(data.filename));

    /* Check only size, as it is good enough proof that it is working */
    Containers::Optional<MeshData> mesh = importer->mesh(0);
    CORRADE_VERIFY(mesh);
    CORRADE_COMPARE(mesh->vertexCount(), 2);

    importer->close();
    CORRADE_VERIFY(!importer->isOpened());
}

void AnySceneImporterTest::detect() {
    auto&& data = DetectData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnySceneImporter");

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->openFile(data.filename));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\n"
        "Trade::AnySceneImporter::openFile(): cannot load the {0} plugin\n",
        data.plugin));
    #else
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} was not found\n"
        "Trade::AnySceneImporter::openFile(): cannot load the {0} plugin\n",
        data.plugin));
    #endif
}

void AnySceneImporterTest::reject() {
    auto&& data = RejectData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnySceneImporter");

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->openFile(data.filename));
    CORRADE_COMPARE(out, Utility::format("Trade::AnySceneImporter::openFile(): cannot determine the format of {}\n", data.filename));
}

void AnySceneImporterTest::unknown() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnySceneImporter");

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->openFile("mesh.wtf"));
    CORRADE_COMPARE(out, "Trade::AnySceneImporter::openFile(): cannot determine the format of mesh.wtf\n");
}

void AnySceneImporterTest::propagateFlags() {
    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYSCENEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("AssimpImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("AssimpImporter plugin can't be loaded.");
    /* Ensure Assimp is used for PLY files and not our StanfordImporter */
    manager.setPreferredPlugins("StanfordImporter", {"AssimpImporter"});

    Containers::String filename = Utility::Path::join(ANYSCENEIMPORTER_TEST_DIR, "per-face-colors-be.ply");

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnySceneImporter");
    importer->setFlags(ImporterFlag::Verbose);

    Containers::String out;
    {
        Debug redirectOutput{&out};
        CORRADE_VERIFY(importer->openFile(filename));
        CORRADE_VERIFY(importer->mesh(0));
    }

    CORRADE_COMPARE_AS(out, Utility::format(
        "Trade::AnySceneImporter::openFile(): using StanfordImporter (provided by AssimpImporter)\n"
        "Trade::AssimpImporter: Info,  T0: Load {}\n", filename),
        TestSuite::Compare::StringHasPrefix);
}

void AnySceneImporterTest::propagateConfiguration() {
    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYSCENEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("AssimpImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("AssimpImporter plugin can't be loaded.");
    /* Ensure Assimp is used for PLY files and not our StanfordImporter */
    manager.setPreferredPlugins("StanfordImporter", {"AssimpImporter"});

    Containers::String filename = Utility::Path::join(ANYSCENEIMPORTER_TEST_DIR, "per-face-colors-be.ply");

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnySceneImporter");

    {
        CORRADE_VERIFY(importer->openFile(filename));

        Containers::Optional<Trade::MeshData> mesh = importer->mesh(0);
        CORRADE_VERIFY(mesh);
        CORRADE_VERIFY(!mesh->hasAttribute(Trade::MeshAttribute::Normal));
    } {
        importer->configuration().addGroup("postprocess")->setValue("GenNormals", true);
        CORRADE_VERIFY(importer->openFile(filename));

        Containers::Optional<Trade::MeshData> mesh = importer->mesh(0);
        CORRADE_VERIFY(mesh);
        CORRADE_VERIFY(mesh->hasAttribute(Trade::MeshAttribute::Normal));
    }
}

void AnySceneImporterTest::propagateConfigurationUnknown() {
    auto&& data = PropagateConfigurationUnknownData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYSCENEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("AssimpImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("AssimpImporter plugin can't be loaded.");
    /* Ensure Assimp is used for PLY files and not our StanfordImporter. This
       thus also accidentally checks that correct plugin name (and not the
       alias) is used in the warning messages. */
    manager.setPreferredPlugins("StanfordImporter", {"AssimpImporter"});

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnySceneImporter");
    importer->configuration().setValue("noSuchOption", "isHere");
    importer->configuration().addGroup("postprocess");
    importer->configuration().group("postprocess")->setValue("notHere", false);
    importer->configuration().group("postprocess")->addGroup("feh")->setValue("noHereNotEither", false);
    importer->setFlags(data.flags);

    Containers::String out;
    Warning redirectWarning{&out};
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYSCENEIMPORTER_TEST_DIR, "per-face-colors-be.ply")));
    if(data.quiet)
        CORRADE_COMPARE(out, "");
    else
        CORRADE_COMPARE(out,
        "Trade::AnySceneImporter::openFile(): option noSuchOption not recognized by AssimpImporter\n"
        "Trade::AnySceneImporter::openFile(): option postprocess/notHere not recognized by AssimpImporter\n"
        "Trade::AnySceneImporter::openFile(): option postprocess/feh/noHereNotEither not recognized by AssimpImporter\n");
}

void AnySceneImporterTest::propagateConfigurationUnknownInEmptySubgroup() {
    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYSCENEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GltfImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GltfImporter plugin can't be loaded.");

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnySceneImporter");
    importer->configuration().addGroup("customSceneFieldTypes");
    importer->configuration().group("customSceneFieldTypes")->setValue("field", "Float");
    importer->configuration().group("customSceneFieldTypes")->setValue("another", "Int");
    importer->configuration().group("customSceneFieldTypes")->addGroup("notFound")->setValue("noHereNotEither", false);

    Containers::String out;
    Warning redirectWarning{&out};
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYSCENEIMPORTER_TEST_DIR, "scenes.gltf")));
    CORRADE_COMPARE(out,
        /* Should not warn for values added to the empty customSceneFieldTypes
           group, but should warn if a subgroup is added there. This is
           consistent with how the magnum-*converter -i / -c options are
           handled in Magnum/Implementation/converterUtilities.h. */
        "Trade::AnySceneImporter::openFile(): option customSceneFieldTypes/notFound/noHereNotEither not recognized by GltfImporter\n");
}

void AnySceneImporterTest::propagateFileCallback() {
    if(!(_manager.loadState("ObjImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("ObjImporter plugin not enabled, cannot test");

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnySceneImporter");

    Containers::Array<char> storage;
    importer->setFileCallback([](const std::string&, InputFileCallbackPolicy, Containers::Array<char>& storage) -> Containers::Optional<Containers::ArrayView<const char>> {
        Containers::Optional<Containers::Array<char>> data = Utility::Path::read(Utility::Path::join(OBJIMPORTER_TEST_DIR, "mesh-multiple.obj"));
        CORRADE_VERIFY(data);
        storage = *Utility::move(data);
        return Containers::ArrayView<const char>{storage};
    }, storage);

    CORRADE_VERIFY(true); /* Capture correct function name first */

    CORRADE_VERIFY(importer->openFile("you-know-where-the-file-is.obj"));
    CORRADE_COMPARE(importer->meshCount(), 3);

    /* Check only size, as it is good enough proof that it is working */
    Containers::Optional<MeshData> mesh = importer->mesh(0);
    CORRADE_VERIFY(mesh);
    CORRADE_COMPARE(mesh->vertexCount(), 2);

    importer->close();
    CORRADE_VERIFY(!importer->isOpened());
}

void AnySceneImporterTest::animations() {
    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYSCENEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("UfbxImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("UfbxImporter plugin can't be loaded.");

    /* Make sure UfbxImporter is preferred over Assimp */
    manager.setPreferredPlugins("FbxImporter", {"UfbxImporter"});

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnySceneImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYSCENEIMPORTER_TEST_DIR, "animation-visibility.fbx")));
    /* Would be better to have a file with multiple animations but it's FBX
       we're dealing with here. Too painful. */
    CORRADE_COMPARE(importer->animationCount(), 1);
    CORRADE_COMPARE(importer->animationName(0), "Take 001");
    CORRADE_COMPARE(importer->animationForName("Take 001"), 0);
    CORRADE_COMPARE(importer->animationForName("nonexistent"), -1);

    /* Custom track target name mapping */
    CORRADE_COMPARE(importer->animationTrackTargetForName("visibility"), animationTrackTargetCustom(0));
    CORRADE_COMPARE(importer->animationTrackTargetForName("nonexistent"), AnimationTrackTarget{});
    CORRADE_COMPARE(importer->animationTrackTargetName(animationTrackTargetCustom(0)), "visibility");
    CORRADE_COMPARE(importer->animationTrackTargetName(animationTrackTargetCustom(3)), "");

    /* Check only track count, a good enough proof that it's working */
    Containers::Optional<AnimationData> animation = importer->animation(0);
    CORRADE_COMPARE(animation->trackCount(), 5);
}

void AnySceneImporterTest::animationTrackTargetNameNoFileOpened() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnySceneImporter");

    /* Shouldn't crash if no file is opened */
    CORRADE_COMPARE(importer->animationTrackTargetForName(""), AnimationTrackTarget{});
    CORRADE_COMPARE(importer->animationTrackTargetName(animationTrackTargetCustom(0)), "");
}

void AnySceneImporterTest::scenes() {
    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYSCENEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GltfImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GltfImporter plugin can't be loaded.");

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnySceneImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYSCENEIMPORTER_TEST_DIR, "scenes.gltf")));
    CORRADE_COMPARE(importer->sceneCount(), 3);
    CORRADE_COMPARE(importer->defaultScene(), 2);
    CORRADE_COMPARE(importer->sceneName(1), "A scene with all nodes");
    CORRADE_COMPARE(importer->sceneForName("A scene with all nodes"), 1);
    CORRADE_COMPARE(importer->sceneForName("nonexistent"), -1);
    CORRADE_COMPARE(importer->objectCount(), 4);
    CORRADE_COMPARE(importer->objectName(1), "Custom fields");
    CORRADE_COMPARE(importer->objectForName("Custom fields"), 1);
    CORRADE_COMPARE(importer->objectForName("nonexistent"), -1);

    /* Custom field name mapping */
    CORRADE_COMPARE(importer->sceneFieldForName("radius"), sceneFieldCustom(0));
    CORRADE_COMPARE(importer->sceneFieldForName("nonexistent"), SceneField{});
    CORRADE_COMPARE(importer->sceneFieldName(sceneFieldCustom(0)), "radius");
    CORRADE_COMPARE(importer->sceneFieldName(sceneFieldCustom(3)), "");

    /* Check only mapping bound, a good enough proof that it's working */
    Containers::Optional<SceneData> scene = importer->scene(1);
    CORRADE_COMPARE(scene->mappingBound(), 3);
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AnySceneImporterTest::scenesDeprecated2D() {
    /* PrimitiveImporter has 2D scenes, but that one isn't usable from here */
    CORRADE_SKIP("No 2D scene plugin that AnySceneImporter would delegate to");
}

void AnySceneImporterTest::scenesDeprecated3D() {
    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYSCENEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GltfImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GltfImporter plugin can't be loaded.");

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnySceneImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYSCENEIMPORTER_TEST_DIR, "scenes.gltf")));

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(importer->object3DCount(), 4);
    CORRADE_COMPARE(importer->object3DName(1), "Custom fields");
    CORRADE_COMPARE(importer->object3DForName("Custom fields"), 1);
    CORRADE_COMPARE(importer->object3DForName("nonexistent"), -1);

    /* Check only the children list, a good enough proof that it's working */
    Containers::Pointer<ObjectData3D> object = importer->object3D(1);
    CORRADE_COMPARE(object->children(), std::vector<UnsignedInt>{2});
    CORRADE_IGNORE_DEPRECATED_POP
}
#endif

void AnySceneImporterTest::sceneFieldNameNoFileOpened() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnySceneImporter");

    /* Shouldn't crash if no file is opened */
    CORRADE_COMPARE(importer->sceneFieldForName(""), SceneField{});
    CORRADE_COMPARE(importer->sceneFieldName(sceneFieldCustom(0)), "");
}

void AnySceneImporterTest::lights() {
    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYSCENEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GltfImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GltfImporter plugin can't be loaded.");

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnySceneImporter");

    CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYSCENEIMPORTER_TEST_DIR, "lights.gltf")));
    CORRADE_COMPARE(importer->lightCount(), 2);
    CORRADE_COMPARE(importer->lightName(1), "Point with everything implicit");
    CORRADE_COMPARE(importer->lightForName("Point with everything implicit"), 1);
    CORRADE_COMPARE(importer->lightForName("nonexistent"), -1);

    /* Check only light type, a good enough proof that it's working */
    Containers::Optional<Trade::LightData> light = importer->light(1);
    CORRADE_VERIFY(light);
    CORRADE_COMPARE(light->type(), Trade::LightType::Point);
}

void AnySceneImporterTest::cameras() {
    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYSCENEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GltfImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GltfImporter plugin can't be loaded.");

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnySceneImporter");

    CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYSCENEIMPORTER_TEST_DIR, "cameras.gltf")));
    CORRADE_COMPARE(importer->cameraCount(), 2);
    CORRADE_COMPARE(importer->cameraName(1), "Perspective 1:1 75° hFoV");
    CORRADE_COMPARE(importer->cameraForName("Perspective 1:1 75° hFoV"), 1);
    CORRADE_COMPARE(importer->cameraForName("nonexistent"), -1);

    /* Check only camera type, a good enough proof that it's working */
    Containers::Optional<Trade::CameraData> camera = importer->camera(1);
    CORRADE_VERIFY(camera);
    CORRADE_COMPARE(camera->type(), Trade::CameraType::Perspective3D);
}

void AnySceneImporterTest::skins2D() {
    CORRADE_SKIP("No plugin imports 2D skins");
}

void AnySceneImporterTest::skins3D() {
    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYSCENEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GltfImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GltfImporter plugin can't be loaded.");

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnySceneImporter");

    CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYSCENEIMPORTER_TEST_DIR, "skins.gltf")));
    CORRADE_COMPARE(importer->skin3DCount(), 2);
    CORRADE_COMPARE(importer->skin3DName(1), "A skin with two joints");
    CORRADE_COMPARE(importer->skin3DForName("A skin with two joints"), 1);
    CORRADE_COMPARE(importer->skin3DForName("nonexistent"), -1);

    /* Check only joint count, a good enough proof that it's working */
    Containers::Optional<Trade::SkinData3D> skin = importer->skin3D(1);
    CORRADE_VERIFY(skin);
    CORRADE_COMPARE(skin->joints().size(), 2);
}

void AnySceneImporterTest::meshes() {
    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYSCENEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GltfImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GltfImporter plugin can't be loaded.");

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnySceneImporter");

    CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYSCENEIMPORTER_TEST_DIR, "meshes.gltf")));
    CORRADE_COMPARE(importer->meshCount(), 2);
    CORRADE_COMPARE(importer->meshName(1), "Custom attributes");
    CORRADE_COMPARE(importer->meshForName("Custom attributes"), 1);
    CORRADE_COMPARE(importer->meshForName("nonexistent"), -1);

    /* Custom attribute name mapping */
    CORRADE_COMPARE(importer->meshAttributeForName("_TBN"), meshAttributeCustom(3));
    CORRADE_COMPARE(importer->meshAttributeForName("nonexistent"), MeshAttribute{});
    CORRADE_COMPARE(importer->meshAttributeName(meshAttributeCustom(3)), "_TBN");
    CORRADE_COMPARE(importer->meshAttributeName(meshAttributeCustom(4)), "");

    /* Check only attribute count, a good enough proof that it's working */
    Containers::Optional<Trade::MeshData> mesh = importer->mesh(1);
    CORRADE_VERIFY(mesh);
    CORRADE_COMPARE(mesh->attributeCount(), 2);
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AnySceneImporterTest::meshesDeprecated2D() {
    /* PrimitiveImporter has 2D scenes, but that one isn't usable from here */
    CORRADE_SKIP("No 2D mesh plugin that AnySceneImporter would delegate to");
}

void AnySceneImporterTest::meshesDeprecated3D() {
    if(!(_manager.loadState("ObjImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("ObjImporter plugin not enabled, cannot test");

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnySceneImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(OBJIMPORTER_TEST_DIR, "mesh-multiple.obj")));

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(importer->mesh3DCount(), 3);
    CORRADE_COMPARE(importer->mesh3DName(1), "LineMesh");
    CORRADE_COMPARE(importer->mesh3DForName("LineMesh"), 1);

    /* MSVC warns also on positions() */
    Containers::Optional<MeshData3D> mesh = importer->mesh3D(1);
    CORRADE_VERIFY(mesh);
    CORRADE_COMPARE(mesh->positions(0).size(), 2);
    CORRADE_IGNORE_DEPRECATED_POP
}
#endif

void AnySceneImporterTest::meshLevels() {
    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYSCENEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("StanfordImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("StanfordImporter plugin can't be loaded.");

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnySceneImporter");
    importer->configuration().setValue("perFaceToPerVertex", false);

    CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYSCENEIMPORTER_TEST_DIR, "per-face-colors-be.ply")));
    CORRADE_COMPARE(importer->meshCount(), 1);
    CORRADE_COMPARE(importer->meshLevelCount(0), 2);

    /* Check only primitive, a good enough proof that it's working */
    Containers::Optional<Trade::MeshData> mesh = importer->mesh(0, 1);
    CORRADE_VERIFY(mesh);
    CORRADE_COMPARE(mesh->primitive(), MeshPrimitive::Faces);
}

void AnySceneImporterTest::meshAttributeNameNoFileOpened() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnySceneImporter");

    /* Shouldn't crash if no file is opened */
    CORRADE_COMPARE(importer->meshAttributeForName(""), MeshAttribute{});
    CORRADE_COMPARE(importer->meshAttributeName(meshAttributeCustom(0)), "");
}

void AnySceneImporterTest::materials() {
    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYSCENEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GltfImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GltfImporter plugin can't be loaded.");

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnySceneImporter");

    CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYSCENEIMPORTER_TEST_DIR, "materials.gltf")));
    CORRADE_COMPARE(importer->materialCount(), 2);
    CORRADE_COMPARE(importer->materialName(1), "Alpha masked and double sided");
    CORRADE_COMPARE(importer->materialForName("Alpha masked and double sided"), 1);
    CORRADE_COMPARE(importer->materialForName("nonexistent"), -1);

    /* Check only attribute count, a good enough proof that it's working */
    Containers::Optional<Trade::MaterialData> material = importer->material(1);
    CORRADE_VERIFY(material);
    CORRADE_COMPARE(material->attributeCount(), 2);
}

void AnySceneImporterTest::textures() {
    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYSCENEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GltfImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GltfImporter plugin can't be loaded.");

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnySceneImporter");

    CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYSCENEIMPORTER_TEST_DIR, "textures.gltf")));
    CORRADE_COMPARE(importer->textureCount(), 2);
    CORRADE_COMPARE(importer->textureName(1), "Texture that references a PNG");
    CORRADE_COMPARE(importer->textureForName("Texture that references a PNG"), 1);
    CORRADE_COMPARE(importer->textureForName("nonexistent"), -1);

    /* Check only the image reference, a good enough proof that it's working */
    Containers::Optional<Trade::TextureData> texture = importer->texture(1);
    CORRADE_VERIFY(texture);
    CORRADE_COMPARE(texture->type(), Trade::TextureType::Texture2D);
    CORRADE_COMPARE(texture->image(), 1);
}

void AnySceneImporterTest::images1D() {
    CORRADE_SKIP("No scene plugin imports 1D images");
}

void AnySceneImporterTest::images2D() {
    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYSCENEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GltfImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GltfImporter plugin can't be loaded.");
    if(manager.load("PngImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("PngImporter plugin can't be loaded.");

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnySceneImporter");

    CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYSCENEIMPORTER_TEST_DIR, "textures.gltf")));
    CORRADE_COMPARE(importer->image1DCount(), 0);
    CORRADE_COMPARE(importer->image2DCount(), 2);
    CORRADE_COMPARE(importer->image3DCount(), 0);
    CORRADE_COMPARE(importer->image2DName(1), "A PNG image");
    CORRADE_COMPARE(importer->image2DForName("A PNG image"), 1);
    CORRADE_COMPARE(importer->image2DForName("nonexistent"), -1);

    /* Check only size, a good enough proof that it's working */
    Containers::Optional<Trade::ImageData2D> image = importer->image2D(1);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->size(), (Vector2i{3, 2}));
}

void AnySceneImporterTest::images3D() {
    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYSCENEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GltfImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GltfImporter plugin can't be loaded.");
    if(manager.load("KtxImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("KtxImporter plugin can't be loaded.");

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnySceneImporter");
    importer->configuration().setValue("experimentalKhrTextureKtx", true);

    CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYSCENEIMPORTER_TEST_DIR, "images3d-levels.gltf")));
    CORRADE_COMPARE(importer->image1DCount(), 0);
    CORRADE_COMPARE(importer->image2DCount(), 0);
    CORRADE_COMPARE(importer->image3DCount(), 2);
    CORRADE_COMPARE(importer->image3DName(1), "A 3D KTX2 image");
    CORRADE_COMPARE(importer->image3DForName("A 3D KTX2 image"), 1);
    CORRADE_COMPARE(importer->image3DForName("nonexistent"), -1);

    /* Check only size, a good enough proof that it's working */
    Containers::Optional<Trade::ImageData3D> image = importer->image3D(1);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->size(), (Vector3i{4, 3, 3}));
}

void AnySceneImporterTest::imageLevels1D() {
    CORRADE_SKIP("No scene plugin imports 1D images");
}

void AnySceneImporterTest::imageLevels2D() {
    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYSCENEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GltfImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GltfImporter plugin can't be loaded.");
    if(manager.load("KtxImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("KtxImporter plugin can't be loaded.");

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnySceneImporter");
    importer->configuration().setValue("experimentalKhrTextureKtx", true);

    CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYSCENEIMPORTER_TEST_DIR, "images2d-levels.gltf")));
    CORRADE_COMPARE(importer->image2DCount(), 2);
    CORRADE_COMPARE(importer->image2DLevelCount(1), 3);

    /* Check only size, a good enough proof that it's working */
    Containers::Optional<Trade::ImageData2D> image = importer->image2D(1, 2);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->size(), (Vector2i{1, 1}));
}

void AnySceneImporterTest::imageLevels3D() {
    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYSCENEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GltfImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GltfImporter plugin can't be loaded.");
    if(manager.load("KtxImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("KtxImporter plugin can't be loaded.");

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnySceneImporter");
    importer->configuration().setValue("experimentalKhrTextureKtx", true);

    CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYSCENEIMPORTER_TEST_DIR, "images3d-levels.gltf")));
    CORRADE_COMPARE(importer->image3DCount(), 2);
    CORRADE_COMPARE(importer->image3DLevelCount(1), 3);

    /* Check only size, a good enough proof that it's working */
    Containers::Optional<Trade::ImageData3D> image = importer->image3D(1, 2);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->size(), (Vector3i{1, 1, 3}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AnySceneImporterTest)
