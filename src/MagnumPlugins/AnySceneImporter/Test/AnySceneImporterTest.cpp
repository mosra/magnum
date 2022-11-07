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

#include <sstream>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StringIterable.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/ConfigurationGroup.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/FormatStl.h>
#include <Corrade/Utility/Path.h>

#include "Magnum/Math/Vector3.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/MeshData.h"
#include "Magnum/Trade/SceneData.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#define _MAGNUM_NO_DEPRECATED_MESHDATA /* So it doesn't yell here */

#include "Magnum/Trade/MeshData3D.h"
#endif

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct AnySceneImporterTest: TestSuite::Tester {
    explicit AnySceneImporterTest();

    void load();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void loadDeprecatedMeshData();
    #endif
    void detect();

    void unknown();

    void propagateFlags();
    void propagateConfiguration();
    void propagateConfigurationUnknown();
    void propagateConfigurationUnknownInEmptySubgroup();
    void propagateFileCallback();

    void sceneFieldName();
    void sceneFieldNameNoFileOpened();
    void meshAttributeName();
    void meshAttributeNameNoFileOpened();

    /* Explicitly forbid system-wide plugin dependencies */
    PluginManager::Manager<AbstractImporter> _manager{"nonexistent"};
};

const struct {
    const char* name;
    Containers::String filename;
} LoadData[]{
    {"OBJ", Utility::Path::join(OBJIMPORTER_TEST_DIR, "mesh-primitive-points.obj")},
};

constexpr struct {
    const char* name;
    const char* filename;
    const char* plugin;
} DetectData[]{
    {"Blender", "suzanne.blend", "BlenderImporter"},
    {"COLLADA", "xml.dae", "ColladaImporter"},
    {"3MF", "print.3mf", "3mfImporter"},
    {"FBX", "autodesk.fbx", "FbxImporter"},
    {"glTF", "khronos.gltf", "GltfImporter"},
    {"glTF binary", "khronos.glb", "GltfImporter"},
    {"OpenGEX", "eric.ogex", "OpenGexImporter"},
    {"Stanford PLY", "bunny.ply", "StanfordImporter"},
    {"Stanford PLY uppercase", "ARMADI~1.PLY", "StanfordImporter"},
    {"STL", "robot.stl", "StlImporter"},
    /* Not testing everything, only the most important ones */
};

AnySceneImporterTest::AnySceneImporterTest() {
    addInstancedTests({&AnySceneImporterTest::load},
        Containers::arraySize(LoadData));

    #ifdef MAGNUM_BUILD_DEPRECATED
    addTests({&AnySceneImporterTest::loadDeprecatedMeshData});
    #endif

    addInstancedTests({&AnySceneImporterTest::detect},
        Containers::arraySize(DetectData));

    addTests({&AnySceneImporterTest::unknown,

              &AnySceneImporterTest::propagateFlags,
              &AnySceneImporterTest::propagateConfiguration,
              &AnySceneImporterTest::propagateConfigurationUnknown,
              &AnySceneImporterTest::propagateConfigurationUnknownInEmptySubgroup,
              &AnySceneImporterTest::propagateFileCallback,

              &AnySceneImporterTest::sceneFieldName,
              &AnySceneImporterTest::sceneFieldNameNoFileOpened,
              &AnySceneImporterTest::meshAttributeName,
              &AnySceneImporterTest::meshAttributeNameNoFileOpened});

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
    CORRADE_COMPARE(mesh->vertexCount(), 3);

    importer->close();
    CORRADE_VERIFY(!importer->isOpened());
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AnySceneImporterTest::loadDeprecatedMeshData() {
    if(!(_manager.loadState("ObjImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("ObjImporter plugin not enabled, cannot test");

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnySceneImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(OBJIMPORTER_TEST_DIR, "mesh-primitive-points.obj")));

    /* Check only size, as it is good enough proof that it is working */

    /* MSVC warns also on positions() */
    CORRADE_IGNORE_DEPRECATED_PUSH
    Containers::Optional<MeshData3D> mesh = importer->mesh3D(0);
    CORRADE_VERIFY(mesh);
    CORRADE_COMPARE(mesh->positions(0).size(), 3);
    CORRADE_IGNORE_DEPRECATED_POP

    importer->close();
    CORRADE_VERIFY(!importer->isOpened());
}
#endif

void AnySceneImporterTest::detect() {
    auto&& data = DetectData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnySceneImporter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->openFile(data.filename));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\n"
        "Trade::AnySceneImporter::openFile(): cannot load the {0} plugin\n",
        data.plugin));
    #else
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "PluginManager::Manager::load(): plugin {0} was not found\n"
        "Trade::AnySceneImporter::openFile(): cannot load the {0} plugin\n",
        data.plugin));
    #endif
}

void AnySceneImporterTest::unknown() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnySceneImporter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->openFile("mesh.wtf"));
    CORRADE_COMPARE(out.str(), "Trade::AnySceneImporter::openFile(): cannot determine the format of mesh.wtf\n");
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

    Containers::String filename = Utility::Path::join(ANYSCENEIMPORTER_TEST_DIR, "triangle.ply");

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnySceneImporter");
    importer->setFlags(ImporterFlag::Verbose);

    std::ostringstream out;
    {
        Debug redirectOutput{&out};
        CORRADE_VERIFY(importer->openFile(filename));
        CORRADE_VERIFY(importer->mesh(0));
    }

    CORRADE_COMPARE_AS(out.str(), Utility::formatString(
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

    Containers::String filename = Utility::Path::join(ANYSCENEIMPORTER_TEST_DIR, "triangle.ply");

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

    std::ostringstream out;
    Warning redirectWarning{&out};
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYSCENEIMPORTER_TEST_DIR, "triangle.ply")));
    CORRADE_COMPARE(out.str(),
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

    std::ostringstream out;
    Warning redirectWarning{&out};
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYSCENEIMPORTER_TEST_DIR, "empty.gltf")));
    CORRADE_COMPARE(out.str(),
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
        Containers::Optional<Containers::Array<char>> data = Utility::Path::read(Utility::Path::join(OBJIMPORTER_TEST_DIR, "mesh-primitive-points.obj"));
        CORRADE_VERIFY(data);
        storage = *std::move(data);
        return Containers::ArrayView<const char>{storage};
    }, storage);

    CORRADE_VERIFY(true); /* Capture correct function name first */

    CORRADE_VERIFY(importer->openFile("you-know-where-the-file-is.obj"));
    CORRADE_COMPARE(importer->meshCount(), 1);

    /* Check only size, as it is good enough proof that it is working */
    Containers::Optional<MeshData> mesh = importer->mesh(0);
    CORRADE_VERIFY(mesh);
    CORRADE_COMPARE(mesh->vertexCount(), 3);

    importer->close();
    CORRADE_VERIFY(!importer->isOpened());
}

void AnySceneImporterTest::sceneFieldName() {
    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYSCENEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GltfImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GltfImporter plugin can't be loaded.");

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnySceneImporter");

    CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYSCENEIMPORTER_TEST_DIR, "scene-custom-field.gltf")));
    CORRADE_COMPARE(importer->sceneFieldForName("radius"), sceneFieldCustom(0));
    CORRADE_COMPARE(importer->sceneFieldForName("nonexistent"), SceneField{});
    CORRADE_COMPARE(importer->sceneFieldName(sceneFieldCustom(0)), "radius");
    CORRADE_COMPARE(importer->sceneFieldName(sceneFieldCustom(3)), "");
}

void AnySceneImporterTest::sceneFieldNameNoFileOpened() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnySceneImporter");

    CORRADE_COMPARE(importer->sceneFieldForName(""), SceneField{});
    CORRADE_COMPARE(importer->sceneFieldName(sceneFieldCustom(0)), "");
}

void AnySceneImporterTest::meshAttributeName() {
    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYSCENEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GltfImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GltfImporter plugin can't be loaded.");

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnySceneImporter");

    CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYSCENEIMPORTER_TEST_DIR, "mesh-custom-attribute.gltf")));
    CORRADE_COMPARE(importer->meshAttributeForName("_TBN"), meshAttributeCustom(2));
    CORRADE_COMPARE(importer->meshAttributeForName("nonexistent"), MeshAttribute{});
    CORRADE_COMPARE(importer->meshAttributeName(meshAttributeCustom(2)), "_TBN");
    CORRADE_COMPARE(importer->meshAttributeName(meshAttributeCustom(3)), "");
}

void AnySceneImporterTest::meshAttributeNameNoFileOpened() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnySceneImporter");

    CORRADE_COMPARE(importer->meshAttributeForName(""), MeshAttribute{});
    CORRADE_COMPARE(importer->meshAttributeName(meshAttributeCustom(0)), "");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AnySceneImporterTest)
