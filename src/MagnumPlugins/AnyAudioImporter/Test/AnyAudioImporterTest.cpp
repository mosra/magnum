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
#include <Corrade/Containers/Array.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/Audio/AbstractImporter.h"

#include "configure.h"

namespace Magnum { namespace Audio { namespace Test { namespace {

struct AnyImporterTest: TestSuite::Tester {
    explicit AnyImporterTest();

    void load();
    void detect();

    void unknown();

    /* Explicitly forbid system-wide plugin dependencies */
    PluginManager::Manager<AbstractImporter> _manager{"nonexistent"};
};

constexpr struct {
    const char* name;
    const char* filename;
} LoadData[]{
    {"WAV", WAV_FILE}
};

constexpr struct {
    const char* name;
    const char* filename;
    const char* plugin;
} DetectData[]{
    {"OGG", "thunder.ogg", "VorbisAudioImporter"},
    {"OGG uppercase", "YELL.OGG", "VorbisAudioImporter"},
    {"FLAC", "symphony.flac", "FlacAudioImporter"}
};

AnyImporterTest::AnyImporterTest() {
    addInstancedTests({&AnyImporterTest::load},
        Containers::arraySize(LoadData));

    addInstancedTests({&AnyImporterTest::detect},
        Containers::arraySize(DetectData));

    addTests({&AnyImporterTest::unknown});

    /* Load the plugin directly from the build tree. Otherwise it's static and
       already loaded. */
    #ifdef ANYAUDIOIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(ANYAUDIOIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    /* Optional plugins that don't have to be here */
    #ifdef WAVAUDIOIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(WAVAUDIOIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
}

void AnyImporterTest::load() {
    auto&& data = LoadData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!(_manager.loadState("WavAudioImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("WavAudioImporter plugin not enabled, cannot test");

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnyAudioImporter");
    CORRADE_VERIFY(importer->openFile(data.filename));

    /* Check only roughly, as it is good enough proof that it is working */
    CORRADE_COMPARE(importer->format(), BufferFormat::Stereo8);
    CORRADE_COMPARE(importer->frequency(), 96000);
    CORRADE_COMPARE(importer->data().size(), 4);

    importer->close();
    CORRADE_VERIFY(!importer->isOpened());
}

void AnyImporterTest::detect() {
    auto&& data = DetectData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnyAudioImporter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->openFile(data.filename));
    /* Can't use raw string literals in macros on GCC 4.8 */
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out.str(), Utility::formatString(
"PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\nAudio::AnyImporter::openFile(): cannot load the {0} plugin\n", data.plugin));
    #else
    CORRADE_COMPARE(out.str(), Utility::formatString(
"PluginManager::Manager::load(): plugin {0} was not found\nAudio::AnyImporter::openFile(): cannot load the {0} plugin\n", data.plugin));
    #endif
}

void AnyImporterTest::unknown() {
    std::ostringstream output;
    Error redirectError{&output};

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnyAudioImporter");
    CORRADE_VERIFY(!importer->openFile("sound.mid"));

    CORRADE_COMPARE(output.str(), "Audio::AnyImporter::openFile(): cannot determine the format of sound.mid\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Audio::Test::AnyImporterTest)
