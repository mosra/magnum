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

#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/Containers/StridedBitArrayView.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/File.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/ConfigurationGroup.h>
#include <Corrade/Utility/Format.h>
#include <Corrade/Utility/Path.h>

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Trade/AbstractImageConverter.h"
#include "Magnum/Trade/AbstractSceneConverter.h"
#include "Magnum/Trade/MaterialData.h"
#include "Magnum/Trade/MeshData.h"
#include "Magnum/Trade/SceneData.h"
#include "Magnum/Trade/TextureData.h"

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

    void animations();
    void scenes();
    void lights();
    void cameras();
    void skins2D();
    void skins3D();
    void meshes();
    void meshLevels();
    void materials();
    void textures();
    void images1D();
    void images2D();
    void images3D();
    void imageLevels1D();
    void imageLevels2D();
    void imageLevels3D();

    /* Explicitly forbid system-wide plugin dependencies */
    PluginManager::Manager<AbstractSceneConverter> _manager{"nonexistent"};
};

const struct {
    const char* name;
    bool abort;
} ConvertBeginEndData[]{
    {"", false},
    {"abort and retry", true},
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

const struct {
    const char* name;
    SceneConverterFlags flags;
    bool quiet;
} PropagateConfigurationUnknownData[]{
    {"", {}, false},
    {"quiet", SceneConverterFlag::Quiet, true}
};

AnySceneConverterTest::AnySceneConverterTest() {
    addTests({&AnySceneConverterTest::convert});

    addInstancedTests({&AnySceneConverterTest::convertBeginEnd},
        Containers::arraySize(ConvertBeginEndData));

    addInstancedTests({&AnySceneConverterTest::detectConvert},
        Containers::arraySize(DetectConvertData));

    addInstancedTests({&AnySceneConverterTest::detectBeginEnd},
        Containers::arraySize(DetectBeginEndData));

    addTests({&AnySceneConverterTest::unknownConvert,
              &AnySceneConverterTest::unknownBeginEnd,

              &AnySceneConverterTest::propagateFlagsConvert,
              &AnySceneConverterTest::propagateFlagsBeginEnd,

              &AnySceneConverterTest::propagateConfigurationConvert,
              &AnySceneConverterTest::propagateConfigurationBeginEnd});

    addInstancedTests({
        &AnySceneConverterTest::propagateConfigurationUnknownConvert,
        &AnySceneConverterTest::propagateConfigurationUnknownBeginEnd},
        Containers::arraySize(PropagateConfigurationUnknownData));

    addTests({&AnySceneConverterTest::animations,
              &AnySceneConverterTest::scenes,
              &AnySceneConverterTest::lights,
              &AnySceneConverterTest::cameras,
              &AnySceneConverterTest::skins2D,
              &AnySceneConverterTest::skins3D,
              &AnySceneConverterTest::meshes,
              &AnySceneConverterTest::meshLevels,
              &AnySceneConverterTest::materials,
              &AnySceneConverterTest::textures,
              &AnySceneConverterTest::images1D,
              &AnySceneConverterTest::images2D,
              &AnySceneConverterTest::images3D,
              &AnySceneConverterTest::imageLevels1D,
              &AnySceneConverterTest::imageLevels2D,
              &AnySceneConverterTest::imageLevels3D});

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
    CORRADE_COMPARE_AS(filename, Utility::Path::join(ANYSCENECONVERTER_TEST_DIR, "triangle.ply"), TestSuite::Compare::File);
}

void AnySceneConverterTest::convertBeginEnd() {
    auto&& data = ConvertBeginEndData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

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

    /* Aborting should abort the internal converter as well */
    if(data.abort) {
        converter->abort();
        CORRADE_VERIFY(converter->beginFile(filename));
        CORRADE_COMPARE(converter->add(mesh), 0);
    }

    CORRADE_VERIFY(converter->endFile());

    CORRADE_COMPARE_AS(filename, Utility::Path::join(ANYSCENECONVERTER_TEST_DIR, "triangle.ply"), TestSuite::Compare::File);
}

void AnySceneConverterTest::detectConvert() {
    auto&& data = DetectConvertData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractSceneConverter> converter = _manager.instantiate("AnySceneConverter");

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile(MeshData{MeshPrimitive::Triangles, 0}, data.filename));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\n"
        "Trade::AnySceneConverter::convertToFile(): cannot load the {0} plugin\n",
        data.plugin));
    #else
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} was not found\n"
        "Trade::AnySceneConverter::convertToFile(): cannot load the {0} plugin\n",
        data.plugin));
    #endif
}

void AnySceneConverterTest::detectBeginEnd() {
    auto&& data = DetectConvertData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractSceneConverter> converter = _manager.instantiate("AnySceneConverter");

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->beginFile(data.filename));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\n"
        "Trade::AnySceneConverter::beginFile(): cannot load the {0} plugin\n",
        data.plugin));
    #else
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} was not found\n"
        "Trade::AnySceneConverter::beginFile(): cannot load the {0} plugin\n", data.plugin));
    #endif
}

void AnySceneConverterTest::unknownConvert() {
    Containers::Pointer<AbstractSceneConverter> converter = _manager.instantiate("AnySceneConverter");

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile(MeshData{MeshPrimitive::Triangles, 0}, "mesh.obj"));
    CORRADE_COMPARE(out, "Trade::AnySceneConverter::convertToFile(): cannot determine the format of mesh.obj\n");
}

void AnySceneConverterTest::unknownBeginEnd() {
    Containers::Pointer<AbstractSceneConverter> converter = _manager.instantiate("AnySceneConverter");

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->beginFile("mesh.obj"));
    CORRADE_COMPARE(out, "Trade::AnySceneConverter::beginFile(): cannot determine the format of mesh.obj\n");
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
    converter->setFlags(SceneConverterFlag::Verbose);

    Containers::String out;
    {
        Debug redirectOutput{&out};
        CORRADE_VERIFY(converter->convertToFile(mesh, filename));
        CORRADE_VERIFY(Utility::Path::exists(filename));
    }
    CORRADE_COMPARE(out,
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
    converter->setFlags(SceneConverterFlag::Verbose);

    Containers::String out;
    {
        Debug redirectOutput{&out};
        CORRADE_VERIFY(converter->beginFile(filename));
    }
    CORRADE_VERIFY(converter->add(mesh));
    CORRADE_VERIFY(converter->endFile());
    CORRADE_VERIFY(Utility::Path::exists(filename));
    CORRADE_COMPARE(out,
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
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

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
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

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
    auto&& data = PropagateConfigurationUnknownData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

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
    converter->setFlags(data.flags);

    Containers::String out;
    Warning redirectWarning{&out};
    CORRADE_VERIFY(converter->convertToFile(mesh, Utility::Path::join(ANYSCENECONVERTER_TEST_OUTPUT_DIR, "file.ply")));
    if(data.quiet)
        CORRADE_COMPARE(out, "");
    else
        CORRADE_COMPARE(out, "Trade::AnySceneConverter::convertToFile(): option noSuchOption not recognized by StanfordSceneConverter\n");
}

void AnySceneConverterTest::propagateConfigurationUnknownBeginEnd() {
    auto&& data = PropagateConfigurationUnknownData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

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
    converter->setFlags(data.flags);

    Containers::String out;
    {
        Warning redirectWarning{&out};
        CORRADE_VERIFY(converter->beginFile(Utility::Path::join(ANYSCENECONVERTER_TEST_OUTPUT_DIR, "file.ply")));
    }
    CORRADE_VERIFY(converter->add(mesh));
    CORRADE_VERIFY(converter->endFile());
    if(data.quiet)
        CORRADE_COMPARE(out, "");
    else
        CORRADE_COMPARE(out, "Trade::AnySceneConverter::beginFile(): option noSuchOption not recognized by StanfordSceneConverter\n");
}

void AnySceneConverterTest::animations() {
    CORRADE_SKIP("No plugin supports animation conversion");
}

void AnySceneConverterTest::scenes() {
    PluginManager::Manager<AbstractSceneConverter> manager{MAGNUM_PLUGINS_SCENECONVERTER_INSTALL_DIR};
    #ifdef ANYSCENECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("GltfSceneConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("GltfSceneConverter plugin can't be loaded.");

    Containers::Pointer<AbstractSceneConverter> converter = manager.instantiate("AnySceneConverter");

    Containers::String filename = Utility::Path::join(ANYSCENECONVERTER_TEST_OUTPUT_DIR, "scene.gltf");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    const struct Data {
        UnsignedInt mapping;
        Int parent;
        bool visible;
    } data[]{
        {15, -1, true}
    };
    auto view = Containers::stridedArrayView(data);

    CORRADE_VERIFY(converter->beginFile(filename));
    converter->setSceneFieldName(sceneFieldCustom(667), "veryNiceVisibility");
    converter->setObjectName(15, "Very nice object");
    CORRADE_VERIFY(converter->add(SceneData{SceneMappingType::UnsignedInt, 16, {}, data, {
        /* To mark the scene as 3D */
        SceneFieldData{SceneField::Transformation,
            SceneMappingType::UnsignedInt, nullptr,
            SceneFieldType::Matrix4x4, nullptr},
        SceneFieldData{SceneField::Parent,
            view.slice(&Data::mapping),
            view.slice(&Data::parent)},
        SceneFieldData{sceneFieldCustom(667),
            view.slice(&Data::mapping),
            view.slice(&Data::visible).sliceBit(0)},
    }}, "A very nice scene"));
    converter->setDefaultScene(0);
    CORRADE_VERIFY(converter->endFile());

    /* Load the file and check that it contains both object and scene name,
       the custom scene field and the default scene index */
    Containers::Optional<Containers::String> loaded = Utility::Path::readString(filename);
    CORRADE_VERIFY(loaded);
    CORRADE_COMPARE_AS(*loaded,
        "\"name\": \"Very nice object\"",
        TestSuite::Compare::StringContains);
    CORRADE_COMPARE_AS(*loaded,
        "\"name\": \"A very nice scene\"",
        TestSuite::Compare::StringContains);
    CORRADE_COMPARE_AS(*loaded,
        "\"veryNiceVisibility\": true",
        TestSuite::Compare::StringContains);
    CORRADE_COMPARE_AS(*loaded,
        "\"scene\": 0",
        TestSuite::Compare::StringContains);
}

void AnySceneConverterTest::lights() {
    CORRADE_SKIP("No plugin supports light conversion");
}

void AnySceneConverterTest::cameras() {
    CORRADE_SKIP("No plugin supports camera conversion");
}

void AnySceneConverterTest::skins2D() {
    CORRADE_SKIP("No plugin supports 2D skin conversion");
}

void AnySceneConverterTest::skins3D() {
    CORRADE_SKIP("No plugin supports 3D skin conversion");
}

void AnySceneConverterTest::meshes() {
    PluginManager::Manager<AbstractSceneConverter> manager{MAGNUM_PLUGINS_SCENECONVERTER_INSTALL_DIR};
    #ifdef ANYSCENECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("GltfSceneConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("GltfSceneConverter plugin can't be loaded.");

    Containers::Pointer<AbstractSceneConverter> converter = manager.instantiate("AnySceneConverter");

    Containers::String filename = Utility::Path::join(ANYSCENECONVERTER_TEST_OUTPUT_DIR, "mesh.gltf");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    const struct Vertex {
        Vector3 position;
        Float factor;
    } vertices[3]{
        {{}, 0.0f},
        {{}, 0.0f},
        {{}, 0.0f},
    }; /* GCC 4.8 ICEs if I do just a {} here */
    auto view = Containers::stridedArrayView(vertices);

    CORRADE_VERIFY(converter->beginFile(filename));
    converter->setMeshAttributeName(meshAttributeCustom(667), "veryNiceFactor");
    CORRADE_VERIFY(converter->add(MeshData{MeshPrimitive::Triangles, {}, vertices, {
        MeshAttributeData{MeshAttribute::Position, view.slice(&Vertex::position)},
        MeshAttributeData{meshAttributeCustom(667), view.slice(&Vertex::factor)},
    }}, "Very nice mesh"));
    CORRADE_VERIFY(converter->endFile());

    /* Load the file and check that it contains mesh name and the custom
       attribute */
    Containers::Optional<Containers::String> loaded = Utility::Path::readString(filename);
    CORRADE_VERIFY(loaded);
    CORRADE_COMPARE_AS(*loaded,
        "\"name\": \"Very nice mesh\"",
        TestSuite::Compare::StringContains);
    CORRADE_COMPARE_AS(*loaded,
        "\"veryNiceFactor\": ", /* some accessor number after */
        TestSuite::Compare::StringContains);
}

void AnySceneConverterTest::meshLevels() {
    CORRADE_SKIP("No plugin supports mesh level conversion");
}

void AnySceneConverterTest::materials() {
    PluginManager::Manager<AbstractSceneConverter> manager{MAGNUM_PLUGINS_SCENECONVERTER_INSTALL_DIR};
    #ifdef ANYSCENECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("GltfSceneConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("GltfSceneConverter plugin can't be loaded.");

    Containers::Pointer<AbstractSceneConverter> converter = manager.instantiate("AnySceneConverter");

    Containers::String filename = Utility::Path::join(ANYSCENECONVERTER_TEST_OUTPUT_DIR, "material.gltf");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    CORRADE_VERIFY(converter->beginFile(filename));
    CORRADE_VERIFY(converter->add(MaterialData{{}, {}}, "Very nice material"));
    CORRADE_VERIFY(converter->endFile());

    /* Load the file and check that it contains the material name. That alone
       is enough to verify this works */
    Containers::Optional<Containers::String> loaded = Utility::Path::readString(filename);
    CORRADE_VERIFY(loaded);
    CORRADE_COMPARE_AS(*loaded,
        "\"name\": \"Very nice material\"",
        TestSuite::Compare::StringContains);
}

void AnySceneConverterTest::textures() {
    PluginManager::Manager<AbstractImageConverter> imageManager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    PluginManager::Manager<AbstractSceneConverter> manager{MAGNUM_PLUGINS_SCENECONVERTER_INSTALL_DIR};
    manager.registerExternalManager(imageManager);
    #ifdef ANYSCENECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("GltfSceneConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("GltfSceneConverter plugin can't be loaded.");
    if(!(imageManager.load("PngImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("PngImageConverter plugin can't be loaded.");

    Containers::Pointer<AbstractSceneConverter> converter = manager.instantiate("AnySceneConverter");

    Containers::String filename = Utility::Path::join(ANYSCENECONVERTER_TEST_OUTPUT_DIR, "texture.gltf");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    CORRADE_VERIFY(converter->beginFile(filename));
    CORRADE_VERIFY(converter->add(ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, "hey"}));
    CORRADE_VERIFY(converter->add(TextureData{
        TextureType::Texture2D,
        SamplerFilter::Nearest,
        SamplerFilter::Nearest,
        SamplerMipmap::Nearest,
        SamplerWrapping::ClampToEdge,
        0
    }, "Very nice texture"));
    CORRADE_VERIFY(converter->endFile());

    /* Load the file and check that it contains the texture name. That alone is
       enough to verify this works */
    Containers::Optional<Containers::String> loaded = Utility::Path::readString(filename);
    CORRADE_VERIFY(loaded);
    CORRADE_COMPARE_AS(*loaded,
        "\"name\": \"Very nice texture\"",
        TestSuite::Compare::StringContains);
}

void AnySceneConverterTest::images1D() {
    CORRADE_SKIP("No plugin supports 1D image conversion");
}

void AnySceneConverterTest::images2D() {
    PluginManager::Manager<AbstractImageConverter> imageManager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    PluginManager::Manager<AbstractSceneConverter> manager{MAGNUM_PLUGINS_SCENECONVERTER_INSTALL_DIR};
    manager.registerExternalManager(imageManager);
    #ifdef ANYSCENECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("GltfSceneConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("GltfSceneConverter plugin can't be loaded.");
    if(!(imageManager.load("PngImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("PngImageConverter plugin can't be loaded.");

    Containers::Pointer<AbstractSceneConverter> converter = manager.instantiate("AnySceneConverter");

    Containers::String filename = Utility::Path::join(ANYSCENECONVERTER_TEST_OUTPUT_DIR, "image2d.gltf");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    CORRADE_VERIFY(converter->beginFile(filename));
    CORRADE_VERIFY(converter->add(ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, "hey"}, "Very nice image"));
    CORRADE_VERIFY(converter->endFile());

    /* Load the file and check that it contains the image name. That alone is
       enough to verify this works */
    Containers::Optional<Containers::String> loaded = Utility::Path::readString(filename);
    CORRADE_VERIFY(loaded);
    CORRADE_COMPARE_AS(*loaded,
        "\"name\": \"Very nice image\"",
        TestSuite::Compare::StringContains);
}

void AnySceneConverterTest::images3D() {
    PluginManager::Manager<AbstractImageConverter> imageManager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    PluginManager::Manager<AbstractSceneConverter> manager{MAGNUM_PLUGINS_SCENECONVERTER_INSTALL_DIR};
    manager.registerExternalManager(imageManager);
    #ifdef ANYSCENECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSCENECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("GltfSceneConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("GltfSceneConverter plugin can't be loaded.");
    if(!(imageManager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::Pointer<AbstractSceneConverter> converter = manager.instantiate("AnySceneConverter");
    converter->configuration().setValue("imageConverter", "KtxImageConverter");
    converter->configuration().setValue("experimentalKhrTextureKtx", true);

    Containers::String filename = Utility::Path::join(ANYSCENECONVERTER_TEST_OUTPUT_DIR, "image3d.gltf");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    CORRADE_VERIFY(converter->beginFile(filename));
    CORRADE_VERIFY(converter->add(ImageView3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, "hey", ImageFlag3D::Array}, "Very nice image"));
    CORRADE_VERIFY(converter->endFile());

    /* Load the file and check that it contains the image name. That alone is
       enough to verify this works */
    Containers::Optional<Containers::String> loaded = Utility::Path::readString(filename);
    CORRADE_VERIFY(loaded);
    CORRADE_COMPARE_AS(*loaded,
        "\"name\": \"Very nice image\"",
        TestSuite::Compare::StringContains);
}

void AnySceneConverterTest::imageLevels1D() {
    CORRADE_SKIP("No plugin supports 1D image level conversion");
}

void AnySceneConverterTest::imageLevels2D() {
    CORRADE_SKIP("No plugin supports 2D image level conversion");
}

void AnySceneConverterTest::imageLevels3D() {
    CORRADE_SKIP("No plugin supports 3D image level conversion");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AnySceneConverterTest)
