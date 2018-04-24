/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/PluginManager/Manager.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/Trade/AbstractImageConverter.h"
#include "Magnum/Trade/ImageData.h"

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test {

struct AnyImageConverterTest: TestSuite::Tester {
    explicit AnyImageConverterTest();

    void tga();

    void unknown();

    /* Explicitly forbid system-wide plugin dependencies */
    PluginManager::Manager<AbstractImageConverter> _manager{"nonexistent"};
};

AnyImageConverterTest::AnyImageConverterTest() {
    addTests({&AnyImageConverterTest::tga,

              &AnyImageConverterTest::unknown});

    /* Load the plugin directly from the build tree. Otherwise it's static and
       already loaded. */
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT(_manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    /* Optional plugins that don't have to be here */
    #ifdef TGAIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT(_manager.load(TGAIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Create the output directory if it doesn't exist yet */
    CORRADE_INTERNAL_ASSERT(Utility::Directory::mkpath(ANYIMAGECONVERTER_TEST_DIR));
}

namespace {
    constexpr const char Data[] = {
        1, 2, 3, 2, 3, 4, 0, 0,
        3, 4, 5, 4, 5, 6, 0, 0,
        5, 6, 7, 6, 7, 8, 0, 0
    };

    const ImageView2D Image{PixelFormat::RGB8Unorm, {2, 3}, Data};
}

void AnyImageConverterTest::tga() {
    if(!(_manager.loadState("TgaImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImageConverter plugin not enabled, cannot test");

    const std::string filename = Utility::Directory::join(ANYIMAGECONVERTER_TEST_DIR, "output.tga");

    if(Utility::Directory::fileExists(filename))
        CORRADE_VERIFY(Utility::Directory::rm(filename));

    /* Just test that the exported file exists */
    std::unique_ptr<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");
    CORRADE_VERIFY(converter->exportToFile(Image, filename));
    CORRADE_VERIFY(Utility::Directory::fileExists(filename));
}

void AnyImageConverterTest::unknown() {
    std::ostringstream output;
    Error redirectError{&output};

    std::unique_ptr<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");
    CORRADE_VERIFY(!converter->exportToFile(Image, "image.xcf"));

    CORRADE_COMPARE(output.str(), "Trade::AnyImageConverter::exportToFile(): cannot determine type of file image.xcf\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AnyImageConverterTest)
