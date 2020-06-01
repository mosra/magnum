/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/Math/Vector3.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/MeshData.h"

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

    void verbose();

    /* Explicitly forbid system-wide plugin dependencies */
    PluginManager::Manager<AbstractImporter> _manager{"nonexistent"};
};

constexpr struct {
    const char* name;
    const char* filename;
} LoadData[]{
    {"OBJ", OBJ_FILE},
};

constexpr struct {
    const char* name;
    const char* filename;
    const char* plugin;
} DetectData[]{
    {"Blender", "suzanne.blend", "BlenderImporter"},
    {"COLLADA", "xml.dae", "ColladaImporter"},
    {"FBX", "autodesk.fbx", "FbxImporter"},
    {"glTF", "khronos.gltf", "GltfImporter"},
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

              &AnySceneImporterTest::verbose});

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
    CORRADE_VERIFY(importer->openFile(OBJ_FILE));

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
    /* Can't use raw string literals in macros on GCC 4.8 */
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out.str(), Utility::formatString(
"PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\nTrade::AnySceneImporter::openFile(): cannot load the {0} plugin\n", data.plugin));
    #else
    CORRADE_COMPARE(out.str(), Utility::formatString(
"PluginManager::Manager::load(): plugin {0} was not found\nTrade::AnySceneImporter::openFile(): cannot load the {0} plugin\n", data.plugin));
    #endif
}

void AnySceneImporterTest::unknown() {
    std::ostringstream output;
    Error redirectError{&output};

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnySceneImporter");
    CORRADE_VERIFY(!importer->openFile("mesh.wtf"));

    CORRADE_COMPARE(output.str(), "Trade::AnySceneImporter::openFile(): cannot determine the format of mesh.wtf\n");
}

void AnySceneImporterTest::verbose() {
    if(!(_manager.loadState("ObjImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("ObjImporter plugin not enabled, cannot test");

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnySceneImporter");
    importer->setFlags(ImporterFlag::Verbose);

    std::ostringstream out;
    {
        Debug redirectOutput{&out};
        CORRADE_VERIFY(importer->openFile(OBJ_FILE));
        CORRADE_VERIFY(importer->mesh(0));
    }
    CORRADE_COMPARE(out.str(),
        "Trade::AnySceneImporter::openFile(): using ObjImporter\n");

    /* We tested AnySceneImporter's verbose output, but can't actually test
       the flag propagation in any way yet */
    CORRADE_SKIP("No plugin with verbose output available to test flag propagation.");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AnySceneImporterTest)
