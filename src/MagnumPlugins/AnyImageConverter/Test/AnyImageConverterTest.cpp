/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/Containers/StringStl.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/File.h>
#include <Corrade/Utility/ConfigurationGroup.h>
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

    void convert2D();
    void convertCompressed2D();
    void detect2D();
    void detectCompressed2D();

    void unknown2D();
    void unknownCompressed2D();

    void propagateFlags2D();
    void propagateFlagsCompressed2D();
    void propagateConfiguration2D();
    void propagateConfigurationUnknown2D();
    void propagateConfigurationCompressed2D();
    void propagateConfigurationCompressedUnknown2D();
    /* configuration propagation fully tested in AnySceneImporter, as there the
       plugins have configuration subgroups as well */

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
    addInstancedTests({&AnyImageConverterTest::convert2D},
        Containers::arraySize(ConvertData));

    addTests({&AnyImageConverterTest::convertCompressed2D});

    addInstancedTests({&AnyImageConverterTest::detect2D},
        Containers::arraySize(DetectData));

    addTests({&AnyImageConverterTest::detectCompressed2D,

              &AnyImageConverterTest::unknown2D,
              &AnyImageConverterTest::unknownCompressed2D,

              &AnyImageConverterTest::propagateFlags2D,
              &AnyImageConverterTest::propagateFlagsCompressed2D,
              &AnyImageConverterTest::propagateConfiguration2D,
              &AnyImageConverterTest::propagateConfigurationUnknown2D,
              &AnyImageConverterTest::propagateConfigurationCompressed2D,
              &AnyImageConverterTest::propagateConfigurationCompressedUnknown2D});

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
    CORRADE_INTERNAL_ASSERT_OUTPUT(Utility::Directory::mkpath(ANYIMAGECONVERTER_TEST_OUTPUT_DIR));
}

constexpr const char Data[] = {
    1, 2, 3, 2, 3, 4, 0, 0,
    3, 4, 5, 4, 5, 6, 0, 0,
    5, 6, 7, 6, 7, 8, 0, 0
};

const ImageView2D Image{PixelFormat::RGB8Unorm, {2, 3}, Data};

void AnyImageConverterTest::convert2D() {
    auto&& data = ConvertData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!(_manager.loadState("TgaImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImageConverter plugin not enabled, cannot test");

    const std::string filename = Utility::Directory::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, data.filename);

    if(Utility::Directory::exists(filename))
        CORRADE_VERIFY(Utility::Directory::rm(filename));

    /* Just test that the exported file exists */
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");
    CORRADE_VERIFY(converter->convertToFile(Image, filename));
    CORRADE_VERIFY(Utility::Directory::exists(filename));
}

void AnyImageConverterTest::convertCompressed2D() {
    CORRADE_SKIP("No file formats to store compressed data yet.");
}

void AnyImageConverterTest::detect2D() {
    auto&& data = DetectData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile(Image, data.filename));
    /* Can't use raw string literals in macros on GCC 4.8 */
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out.str(), Utility::formatString(
"PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\nTrade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n", data.plugin));
    #else
    CORRADE_COMPARE(out.str(), Utility::formatString(
"PluginManager::Manager::load(): plugin {0} was not found\nTrade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n", data.plugin));
    #endif
}

void AnyImageConverterTest::detectCompressed2D() {
    CORRADE_SKIP("No file formats to store compressed data yet.");
}

void AnyImageConverterTest::unknown2D() {
    std::ostringstream output;
    Error redirectError{&output};

    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");
    CORRADE_VERIFY(!converter->convertToFile(Image, "image.xcf"));

    CORRADE_COMPARE(output.str(), "Trade::AnyImageConverter::convertToFile(): cannot determine the format of image.xcf\n");
}

void AnyImageConverterTest::unknownCompressed2D() {
    CORRADE_SKIP("No file formats to store compressed data yet.");
}

void AnyImageConverterTest::propagateFlags2D() {
    if(!(_manager.loadState("TgaImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImageConverter plugin not enabled, cannot test");

    const std::string filename = Utility::Directory::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "output.tga");

    if(Utility::Directory::exists(filename))
        CORRADE_VERIFY(Utility::Directory::rm(filename));

    /* Just test that the exported file exists */
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");
    converter->setFlags(ImageConverterFlag::Verbose);
    std::ostringstream out;
    {
        Debug redirectOutput{&out};
        CORRADE_VERIFY(converter->convertToFile(Image, filename));
    }
    CORRADE_VERIFY(Utility::Directory::exists(filename));
    CORRADE_COMPARE(out.str(),
        "Trade::AnyImageConverter::convertToFile(): using TgaImageConverter\n"
        "Trade::TgaImageConverter::convertToData(): converting from RGB to BGR\n");
}

void AnyImageConverterTest::propagateFlagsCompressed2D() {
    CORRADE_SKIP("No file formats to store compressed data yet.");
}

void AnyImageConverterTest::propagateConfiguration2D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.loadState("OpenExrImageConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("OpenExrImageConverter plugin can't be loaded.");

    const std::string filename = Utility::Directory::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "depth32f-custom-channels.exr");

    if(Utility::Directory::exists(filename))
        CORRADE_VERIFY(Utility::Directory::rm(filename));

    const Float Depth32fData[] = {
        0.125f, 0.250f, 0.375f,
        0.500f, 0.625f, 0.750f
    };

    const ImageView2D Depth32f{PixelFormat::Depth32F, {3, 2}, Depth32fData};

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("layer", "left");
    converter->configuration().setValue("depth", "height");
    CORRADE_VERIFY(converter->convertToFile(Depth32f, filename));
    /* Compare to an expected output to ensure the custom channels names were
       used */
    CORRADE_COMPARE_AS(filename, EXR_FILE, TestSuite::Compare::File);
}

void AnyImageConverterTest::propagateConfigurationUnknown2D() {
    if(!(_manager.loadState("TgaImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImageConverter plugin not enabled, cannot test");

    /* Just test that the exported file exists */
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("noSuchOption", "isHere");

    std::ostringstream out;
    Warning redirectWarning{&out};
    CORRADE_VERIFY(converter->convertToFile(Image, Utility::Directory::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "output.tga")));
    CORRADE_COMPARE(out.str(), "Trade::AnyImageConverter::convertToFile(): option noSuchOption not recognized by TgaImageConverter\n");
}

void AnyImageConverterTest::propagateConfigurationCompressed2D() {
    CORRADE_SKIP("No file formats to store compressed data yet.");
}

void AnyImageConverterTest::propagateConfigurationCompressedUnknown2D() {
    CORRADE_SKIP("No file formats to store compressed data yet.");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AnyImageConverterTest)
