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

#include <sstream>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Audio/AbstractImporter.h"

#include "configure.h"

namespace Magnum { namespace Audio { namespace Test { namespace {

struct AnyImporterTest: TestSuite::Tester {
    explicit AnyImporterTest();

    void wav();

    void unknown();

    /* Explicitly forbid system-wide plugin dependencies */
    PluginManager::Manager<AbstractImporter> _manager{"nonexistent"};
};

AnyImporterTest::AnyImporterTest() {
    addTests({&AnyImporterTest::wav,

              &AnyImporterTest::unknown});

    /* Load the plugin directly from the build tree. Otherwise it's static and
       already loaded. */
    #ifdef ANYAUDIOIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT(_manager.load(ANYAUDIOIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    /* Optional plugins that don't have to be here */
    #ifdef WAVAUDIOIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT(_manager.load(WAVAUDIOIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
}

void AnyImporterTest::wav() {
    if(!(_manager.loadState("WavAudioImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("WavAudioImporter plugin not enabled, cannot test");

    std::unique_ptr<AbstractImporter> importer = _manager.instantiate("AnyAudioImporter");
    CORRADE_VERIFY(importer->openFile(WAV_FILE));

    /* Check only parameters, as it is good enough proof that it is working */
    CORRADE_COMPARE(importer->format(), BufferFormat::Stereo8);
    CORRADE_COMPARE(importer->frequency(), 96000);
}

void AnyImporterTest::unknown() {
    std::ostringstream output;
    Error redirectError{&output};

    std::unique_ptr<AbstractImporter> importer = _manager.instantiate("AnyAudioImporter");
    CORRADE_VERIFY(!importer->openFile("sound.mid"));

    CORRADE_COMPARE(output.str(), "Audio::AnyImporter::openFile(): cannot determine type of file sound.mid\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Audio::Test::AnyImporterTest)
