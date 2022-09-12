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
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/File.h>
#include <Corrade/Utility/ConfigurationGroup.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/FormatStl.h>
#include <Corrade/Utility/Path.h>

#include "Magnum/Math/Vector3.h"
#include "Magnum/Trade/AbstractSceneConverter.h"
#include "Magnum/Trade/MeshData.h"

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct AnySceneConverterTest: TestSuite::Tester {
    explicit AnySceneConverterTest();

    void convert();
    void convertBeginEnd();

    void detectConvert();
    void detectBeginEnd();

    void unknownConvert();
    void unknownBeginEnd();

    void propagateFlagsConvert();
    void propagateFlagsBeginEnd();

    void propagateConfigurationConvert();
    void propagateConfigurationBeginEnd();
    void propagateConfigurationUnknownConvert();
    void propagateConfigurationUnknownBeginEnd();
    /* configuration propagation fully tested in AnySceneImporter, as there the
       plugins have configuration subgroups as well */

    /* Explicitly forbid system-wide plugin dependencies */
    PluginManager::Manager<AbstractSceneConverter> _manager{"nonexistent"};
};

constexpr struct {
    const char* name;
    const char* filename;
    const char* plugin;
} DetectConvertData[]{
    {"glTF", "khronos.gltf", "GltfSceneConverter"},
    {"glTF binary", "khronos.glb", "GltfSceneConverter"},
    {"Stanford PLY", "bunny.ply", "StanfordSceneConverter"},
    /* Have at least one test case with uppercase */
    {"Stanford PLY uppercase", "ARMADI~1.PLY", "StanfordSceneConverter"}
};

constexpr struct {
    const char* name;
    const char* filename;
    const char* plugin;
} DetectBeginEndData[]{
    {"glTF", "khronos.gltf", "GltfSceneConverter"},
    {"glTF binary", "khronos.glb", "GltfSceneConverter"},
    {"Stanford PLY", "bunny.ply", "StanfordSceneConverter"},
    /* Have at least one test case with uppercase */
    {"Stanford PLY uppercase", "ARMADI~1.PLY", "StanfordSceneConverter"}
};

AnySceneConverterTest::AnySceneConverterTest() {
    addTests({&AnySceneConverterTest::convert,
              &AnySceneConverterTest::convertBeginEnd});

    addInstancedTests({&AnySceneConverterTest::detectConvert},
        Containers::arraySize(DetectConvertData));

    addInstancedTests({&AnySceneConverterTest::detectBeginEnd},
        Containers::arraySize(DetectBeginEndData));

    addTests({&AnySceneConverterTest::unknownConvert,
              &AnySceneConverterTest::unknownBeginEnd,

              &AnySceneConverterTest::propagateFlagsConvert,
              &AnySceneConverterTest::propagateFlagsBeginEnd,

              &AnySceneConverterTest::propagateConfigurationConvert,
              &AnySceneConverterTest::propagateConfigurationBeginEnd,
              &AnySceneConverterTest::propagateConfigurationUnknownConvert,
              &AnySceneConverterTest::propagateConfigurationUnknownBeginEnd});

    /* Load the plugin directly from the build tree. Otherwise it's static and
       already loaded. */
    #ifdef ANYSCENECONVERTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(ANYSCENECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Create the output directory if it doesn't exist yet */
    CORRADE_INTERNAL_ASSERT_OUTPUT(Utility::Path::make(ANYSCENECONVERTER_TEST_OUTPUT_DIR));
}

void AnySceneConverterTest::convert() {
    PluginManager::Manager<AbstractSceneConverter> manager{MAGNUM_PLUGINS_SCENECONVERTER_INSTALL_DIR};
    #ifdef ANYSCENECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("StanfordSceneConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("StanfordSceneConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYSCENECONVERTER_TEST_OUTPUT_DIR, "file.ply");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    const Vector3 positions[] {
        {-0.5f, -0.5f, 0.0f},
        { 0.5f, -0.5f, 0.0f},
        { 0.0f,  0.5f, 0.0f}
    };
    const Trade::MeshData mesh{MeshPrimitive::Triangles, {}, positions, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::arrayView(positions)}
    }};

    Containers::Pointer<AbstractSceneConverter> converter = manager.instantiate("AnySceneConverter");
    CORRADE_VERIFY(converter->convertToFile(mesh, filename));
    /* This file is reused in AnySceneImporter tests, so it's worth to save it
       here */
    CORRADE_COMPARE_AS(filename, Utility::Path::join(ANYSCENEIMPORTER_TEST_DIR, "triangle.ply"), TestSuite::Compare::File);
}

void AnySceneConverterTest::convertBeginEnd() {
    PluginManager::Manager<AbstractSceneConverter> manager{MAGNUM_PLUGINS_SCENECONVERTER_INSTALL_DIR};
    #ifdef ANYSCENECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("StanfordSceneConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("StanfordSceneConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYSCENECONVERTER_TEST_OUTPUT_DIR, "file.ply");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    const Vector3 positions[] {
        {-0.5f, -0.5f, 0.0f},
        { 0.5f, -0.5f, 0.0f},
        { 0.0f,  0.5f, 0.0f}
    };
    const Trade::MeshData mesh{MeshPrimitive::Triangles, {}, positions, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::arrayView(positions)}
    }};

    Containers::Pointer<AbstractSceneConverter> converter = manager.instantiate("AnySceneConverter");
    CORRADE_VERIFY(converter->beginFile(filename));
    CORRADE_COMPARE(converter->add(mesh), 0);
    CORRADE_VERIFY(converter->endFile());

    /* This file is reused in AnySceneImporter tests, so it's worth to save it
       here */
    CORRADE_COMPARE_AS(filename, Utility::Path::join(ANYSCENEIMPORTER_TEST_DIR, "triangle.ply"), TestSuite::Compare::File);
}

void AnySceneConverterTest::detectConvert() {
    auto&& data = DetectConvertData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractSceneConverter> converter = _manager.instantiate("AnySceneConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile(MeshData{MeshPrimitive::Triangles, 0}, data.filename));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\n"
        "Trade::AnySceneConverter::convertToFile(): cannot load the {0} plugin\n",
        data.plugin));
    #else
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "PluginManager::Manager::load(): plugin {0} was not found\n"
        "Trade::AnySceneConverter::convertToFile(): cannot load the {0} plugin\n",
        data.plugin));
    #endif
}

void AnySceneConverterTest::detectBeginEnd() {
    auto&& data = DetectConvertData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractSceneConverter> converter = _manager.instantiate("AnySceneConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->beginFile(data.filename));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\n"
        "Trade::AnySceneConverter::beginFile(): cannot load the {0} plugin\n",
        data.plugin));
    #else
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "PluginManager::Manager::load(): plugin {0} was not found\n"
        "Trade::AnySceneConverter::beginFile(): cannot load the {0} plugin\n", data.plugin));
    #endif
}

void AnySceneConverterTest::unknownConvert() {
    Containers::Pointer<AbstractSceneConverter> converter = _manager.instantiate("AnySceneConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile(MeshData{MeshPrimitive::Triangles, 0}, "mesh.obj"));
    CORRADE_COMPARE(out.str(), "Trade::AnySceneConverter::convertToFile(): cannot determine the format of mesh.obj\n");
}

void AnySceneConverterTest::unknownBeginEnd() {
    Containers::Pointer<AbstractSceneConverter> converter = _manager.instantiate("AnySceneConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->beginFile("mesh.obj"));
    CORRADE_COMPARE(out.str(), "Trade::AnySceneConverter::beginFile(): cannot determine the format of mesh.obj\n");
}

void AnySceneConverterTest::propagateFlagsConvert() {
    PluginManager::Manager<AbstractSceneConverter> manager{MAGNUM_PLUGINS_SCENECONVERTER_INSTALL_DIR};
    #ifdef ANYSCENECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("StanfordSceneConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("StanfordSceneConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYSCENECONVERTER_TEST_OUTPUT_DIR, "file.ply");

    const Vector3 positions[] {
        {-0.5f, -0.5f, 0.0f},
        { 0.5f, -0.5f, 0.0f},
        { 0.0f,  0.5f, 0.0f}
    };
    const Trade::MeshData mesh{MeshPrimitive::Triangles, {}, positions, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::arrayView(positions)}
    }};

    Containers::Pointer<AbstractSceneConverter> converter = manager.instantiate("AnySceneConverter");
    converter->setFlags(SceneConverterFlag::Verbose);

    std::ostringstream out;
    {
        Debug redirectOutput{&out};
        CORRADE_VERIFY(converter->convertToFile(mesh, filename));
        CORRADE_VERIFY(Utility::Path::exists(filename));
    }
    CORRADE_COMPARE(out.str(),
        "Trade::AnySceneConverter::convertToFile(): using StanfordSceneConverter\n");

    /* We tested AnySceneConverter's verbose output, but can't actually test
       the flag propagation in any way yet */
    CORRADE_SKIP("No plugin with verbose output available to test flag propagation.");
}

void AnySceneConverterTest::propagateFlagsBeginEnd() {
    PluginManager::Manager<AbstractSceneConverter> manager{MAGNUM_PLUGINS_SCENECONVERTER_INSTALL_DIR};
    #ifdef ANYSCENECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("StanfordSceneConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("StanfordSceneConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYSCENECONVERTER_TEST_OUTPUT_DIR, "file.ply");

    const Vector3 positions[] {
        {-0.5f, -0.5f, 0.0f},
        { 0.5f, -0.5f, 0.0f},
        { 0.0f,  0.5f, 0.0f}
    };
    const Trade::MeshData mesh{MeshPrimitive::Triangles, {}, positions, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::arrayView(positions)}
    }};

    Containers::Pointer<AbstractSceneConverter> converter = manager.instantiate("AnySceneConverter");
    converter->setFlags(SceneConverterFlag::Verbose);

    std::ostringstream out;
    {
        Debug redirectOutput{&out};
        CORRADE_VERIFY(converter->beginFile(filename));
    }
    CORRADE_VERIFY(converter->add(mesh));
    CORRADE_VERIFY(converter->endFile());
    CORRADE_VERIFY(Utility::Path::exists(filename));
    CORRADE_COMPARE(out.str(),
        "Trade::AnySceneConverter::beginFile(): using StanfordSceneConverter\n");

    /* We tested AnySceneConverter's verbose output, but can't actually test
       the flag propagation in any way yet */
    CORRADE_SKIP("No plugin with verbose output available to test flag propagation.");
}

void AnySceneConverterTest::propagateConfigurationConvert() {
    PluginManager::Manager<AbstractSceneConverter> manager{MAGNUM_PLUGINS_SCENECONVERTER_INSTALL_DIR};
    #ifdef ANYSCENECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("StanfordSceneConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("StanfordSceneConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYSCENECONVERTER_TEST_OUTPUT_DIR, "file.ply");

    const struct Data {
        Vector3 position;
        UnsignedInt objectId;
    } data[] {
        {{-0.5f, -0.5f, 0.0f}, 4678},
        {{ 0.5f, -0.5f, 0.0f}, 3232},
        {{ 0.0f,  0.5f, 0.0f}, 1536}
    };
    const Trade::MeshData mesh{MeshPrimitive::Triangles, {}, data, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::stridedArrayView(data).slice(&Data::position)},
        Trade::MeshAttributeData{Trade::MeshAttribute::ObjectId, Containers::stridedArrayView(data).slice(&Data::objectId)},
    }};

    Containers::Pointer<AbstractSceneConverter> converter = manager.instantiate("AnySceneConverter");
    converter->configuration().setValue("objectIdAttribute", "OID");
    CORRADE_VERIFY(converter->convertToFile(mesh, filename));
    /* Compare to an expected output to ensure the custom attribute name was
       used */
    CORRADE_COMPARE_AS(filename, Utility::Path::join(ANYSCENECONVERTER_TEST_DIR, "objectid.ply"), TestSuite::Compare::File);
}

void AnySceneConverterTest::propagateConfigurationBeginEnd() {
    PluginManager::Manager<AbstractSceneConverter> manager{MAGNUM_PLUGINS_SCENECONVERTER_INSTALL_DIR};
    #ifdef ANYSCENECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("StanfordSceneConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("StanfordSceneConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYSCENECONVERTER_TEST_OUTPUT_DIR, "file.ply");

    const struct Data {
        Vector3 position;
        UnsignedInt objectId;
    } data[] {
        {{-0.5f, -0.5f, 0.0f}, 4678},
        {{ 0.5f, -0.5f, 0.0f}, 3232},
        {{ 0.0f,  0.5f, 0.0f}, 1536}
    };
    const Trade::MeshData mesh{MeshPrimitive::Triangles, {}, data, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::stridedArrayView(data).slice(&Data::position)},
        Trade::MeshAttributeData{Trade::MeshAttribute::ObjectId, Containers::stridedArrayView(data).slice(&Data::objectId)},
    }};

    Containers::Pointer<AbstractSceneConverter> converter = manager.instantiate("AnySceneConverter");
    converter->configuration().setValue("objectIdAttribute", "OID");
    CORRADE_VERIFY(converter->beginFile(filename));
    CORRADE_VERIFY(converter->add(mesh));
    CORRADE_VERIFY(converter->endFile());
    /* Compare to an expected output to ensure the custom attribute name was
       used */
    CORRADE_COMPARE_AS(filename, Utility::Path::join(ANYSCENECONVERTER_TEST_DIR, "objectid.ply"), TestSuite::Compare::File);
}

void AnySceneConverterTest::propagateConfigurationUnknownConvert() {
    PluginManager::Manager<AbstractSceneConverter> manager{MAGNUM_PLUGINS_SCENECONVERTER_INSTALL_DIR};
    #ifdef ANYSCENECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("StanfordSceneConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("StanfordSceneConverter plugin can't be loaded.");

    const Vector3 positions[] {
        {-0.5f, -0.5f, 0.0f},
        { 0.5f, -0.5f, 0.0f},
        { 0.0f,  0.5f, 0.0f}
    };
    const Trade::MeshData mesh{MeshPrimitive::Triangles, {}, positions, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::arrayView(positions)}
    }};

    Containers::Pointer<AbstractSceneConverter> converter = manager.instantiate("AnySceneConverter");
    converter->configuration().setValue("noSuchOption", "isHere");

    std::ostringstream out;
    Warning redirectWarning{&out};
    CORRADE_VERIFY(converter->convertToFile(mesh, Utility::Path::join(ANYSCENECONVERTER_TEST_OUTPUT_DIR, "file.ply")));
    CORRADE_COMPARE(out.str(), "Trade::AnySceneConverter::convertToFile(): option noSuchOption not recognized by StanfordSceneConverter\n");
}

void AnySceneConverterTest::propagateConfigurationUnknownBeginEnd() {
    PluginManager::Manager<AbstractSceneConverter> manager{MAGNUM_PLUGINS_SCENECONVERTER_INSTALL_DIR};
    #ifdef ANYSCENECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("StanfordSceneConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("StanfordSceneConverter plugin can't be loaded.");

    const Vector3 positions[] {
        {-0.5f, -0.5f, 0.0f},
        { 0.5f, -0.5f, 0.0f},
        { 0.0f,  0.5f, 0.0f}
    };
    const Trade::MeshData mesh{MeshPrimitive::Triangles, {}, positions, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::arrayView(positions)}
    }};

    Containers::Pointer<AbstractSceneConverter> converter = manager.instantiate("AnySceneConverter");
    converter->configuration().setValue("noSuchOption", "isHere");

    std::ostringstream out;
    {
        Warning redirectWarning{&out};
        CORRADE_VERIFY(converter->beginFile(Utility::Path::join(ANYSCENECONVERTER_TEST_OUTPUT_DIR, "file.ply")));
    }
    CORRADE_VERIFY(converter->add(mesh));
    CORRADE_VERIFY(converter->endFile());
    CORRADE_COMPARE(out.str(), "Trade::AnySceneConverter::beginFile(): option noSuchOption not recognized by StanfordSceneConverter\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AnySceneConverterTest)
