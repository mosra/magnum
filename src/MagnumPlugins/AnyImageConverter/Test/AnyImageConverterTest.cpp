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

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Trade/AbstractImageConverter.h"
#include "Magnum/Trade/ImageData.h"

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct AnyImageConverterTest: TestSuite::Tester {
    explicit AnyImageConverterTest();

    void convert();
    void detect();

    void unknown();

    void verbose();

    /* Explicitly forbid system-wide plugin dependencies */
    PluginManager::Manager<AbstractImageConverter> _manager{"nonexistent"};
};

constexpr struct {
    const char* name;
    const char* filename;
} ConvertData[]{
    {"TGA", "output.tga"}
};

constexpr struct {
    const char* name;
    const char* filename;
    const char* plugin;
} DetectData[]{
    {"BMP", "file.bmp", "BmpImageConverter"},
    {"EXR", "file.exr", "OpenExrImageConverter"},
    {"HDR", "file.hdr", "HdrImageConverter"},
    {"JPEG", "file.jpg", "JpegImageConverter"},
    {"JPEG weird extension", "file.jpe", "JpegImageConverter"},
    {"JPEG uppercase", "output.JPG", "JpegImageConverter"},
    {"PNG", "file.png", "PngImageConverter"}
};

AnyImageConverterTest::AnyImageConverterTest() {
    addInstancedTests({&AnyImageConverterTest::convert},
        Containers::arraySize(ConvertData));

    addInstancedTests({&AnyImageConverterTest::detect},
        Containers::arraySize(DetectData));

    addTests({&AnyImageConverterTest::unknown,

              &AnyImageConverterTest::verbose});

    /* Load the plugin directly from the build tree. Otherwise it's static and
       already loaded. */
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    /* Optional plugins that don't have to be here */
    #ifdef TGAIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(TGAIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Create the output directory if it doesn't exist yet */
    CORRADE_INTERNAL_ASSERT_OUTPUT(Utility::Directory::mkpath(ANYIMAGECONVERTER_TEST_DIR));
}

constexpr const char Data[] = {
    1, 2, 3, 2, 3, 4, 0, 0,
    3, 4, 5, 4, 5, 6, 0, 0,
    5, 6, 7, 6, 7, 8, 0, 0
};

const ImageView2D Image{PixelFormat::RGB8Unorm, {2, 3}, Data};

void AnyImageConverterTest::convert() {
    auto&& data = ConvertData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!(_manager.loadState("TgaImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImageConverter plugin not enabled, cannot test");

    const std::string filename = Utility::Directory::join(ANYIMAGECONVERTER_TEST_DIR, data.filename);

    if(Utility::Directory::exists(filename))
        CORRADE_VERIFY(Utility::Directory::rm(filename));

    /* Just test that the exported file exists */
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");
    CORRADE_VERIFY(converter->exportToFile(Image, filename));
    CORRADE_VERIFY(Utility::Directory::exists(filename));
}

void AnyImageConverterTest::detect() {
    auto&& data = DetectData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->exportToFile(Image, data.filename));
    /* Can't use raw string literals in macros on GCC 4.8 */
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out.str(), Utility::formatString(
"PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\nTrade::AnyImageConverter::exportToFile(): cannot load the {0} plugin\n", data.plugin));
    #else
    CORRADE_COMPARE(out.str(), Utility::formatString(
"PluginManager::Manager::load(): plugin {0} was not found\nTrade::AnyImageConverter::exportToFile(): cannot load the {0} plugin\n", data.plugin));
    #endif
}

void AnyImageConverterTest::unknown() {
    std::ostringstream output;
    Error redirectError{&output};

    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");
    CORRADE_VERIFY(!converter->exportToFile(Image, "image.xcf"));

    CORRADE_COMPARE(output.str(), "Trade::AnyImageConverter::exportToFile(): cannot determine the format of image.xcf\n");
}

void AnyImageConverterTest::verbose() {
    if(!(_manager.loadState("TgaImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImageConverter plugin not enabled, cannot test");

    const std::string filename = Utility::Directory::join(ANYIMAGECONVERTER_TEST_DIR, "output.tga");

    if(Utility::Directory::exists(filename))
        CORRADE_VERIFY(Utility::Directory::rm(filename));

    /* Just test that the exported file exists */
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");
    converter->setFlags(ImageConverterFlag::Verbose);
    std::ostringstream out;
    {
        Debug redirectOutput{&out};
        CORRADE_VERIFY(converter->exportToFile(Image, filename));
    }
    CORRADE_VERIFY(Utility::Directory::exists(filename));
    CORRADE_COMPARE(out.str(),
        "Trade::AnyImageConverter::exportToFile(): using TgaImageConverter\n"
        "Trade::TgaImageConverter::exportToData(): converting from RGB to BGR\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AnyImageConverterTest)
