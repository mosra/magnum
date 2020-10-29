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
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/Trade/AbstractSceneConverter.h"
#include "Magnum/Trade/MeshData.h"

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct AnySceneConverterTest: TestSuite::Tester {
    explicit AnySceneConverterTest();

    void load();
    void detect();

    void unknown();

    void verbose();

    /* Explicitly forbid system-wide plugin dependencies */
    PluginManager::Manager<AbstractSceneConverter> _manager{"nonexistent"};
};

constexpr struct {
    const char* name;
    const char* filename;
    const char* plugin;
} DetectData[]{
    {"Stanford PLY", "bunny.ply", "StanfordSceneConverter"},
    {"Stanford PLY uppercase", "ARMADI~1.PLY", "StanfordSceneConverter"}
};

AnySceneConverterTest::AnySceneConverterTest() {
    addTests({&AnySceneConverterTest::load});

    addInstancedTests({&AnySceneConverterTest::detect},
        Containers::arraySize(DetectData));

    addTests({&AnySceneConverterTest::unknown,

              &AnySceneConverterTest::verbose});

    /* Load the plugin directly from the build tree. Otherwise it's static and
       already loaded. */
    #ifdef ANYSCENECONVERTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(ANYSCENECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Create the output directory if it doesn't exist yet */
    CORRADE_INTERNAL_ASSERT_OUTPUT(Utility::Directory::mkpath(ANYSCENECONVERTER_TEST_OUTPUT_DIR));
}

void AnySceneConverterTest::load() {
    CORRADE_SKIP("No scene converter plugin available to test.");
}

void AnySceneConverterTest::detect() {
    auto&& data = DetectData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractSceneConverter> converter = _manager.instantiate("AnySceneConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile(data.filename, MeshData{MeshPrimitive::Triangles, 0}));
    /* Can't use raw string literals in macros on GCC 4.8 */
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out.str(), Utility::formatString(
"PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\nTrade::AnySceneConverter::convertToFile(): cannot load the {0} plugin\n", data.plugin));
    #else
    CORRADE_COMPARE(out.str(), Utility::formatString(
"PluginManager::Manager::load(): plugin {0} was not found\nTrade::AnySceneConverter::convertToFile(): cannot load the {0} plugin\n", data.plugin));
    #endif
}

void AnySceneConverterTest::unknown() {
    std::ostringstream output;
    Error redirectError{&output};

    Containers::Pointer<AbstractSceneConverter> converter = _manager.instantiate("AnySceneConverter");
    CORRADE_VERIFY(!converter->convertToFile("mesh.obj", MeshData{MeshPrimitive::Triangles, 0}));

    CORRADE_COMPARE(output.str(), "Trade::AnySceneConverter::convertToFile(): cannot determine the format of mesh.obj\n");
}

void AnySceneConverterTest::verbose() {
    CORRADE_SKIP("No plugin available to test.");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AnySceneConverterTest)
