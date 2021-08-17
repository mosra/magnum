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
#include <thread> /* std::thread::hardware_concurrency(), sigh */
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

    void convert1D();
    void convert2D();
    void convert3D();
    void convertCompressed1D();
    void convertCompressed2D();
    void convertCompressed3D();

    void convertLevels1D();
    void convertLevels2D();
    void convertLevels3D();
    void convertCompressedLevels1D();
    void convertCompressedLevels2D();
    void convertCompressedLevels3D();

    void detect1D();
    void detect2D();
    void detect3D();
    void detectCompressed1D();
    void detectCompressed2D();
    void detectCompressed3D();

    void detectLevels1D();
    void detectLevels2D();
    void detectLevels3D();
    void detectCompressedLevels1D();
    void detectCompressedLevels2D();
    void detectCompressedLevels3D();

    void unknown1D();
    void unknown2D();
    void unknown3D();
    void unknownCompressed1D();
    void unknownCompressed2D();
    void unknownCompressed3D();

    void unknownLevels1D();
    void unknownLevels2D();
    void unknownLevels3D();
    void unknownCompressedLevels1D();
    void unknownCompressedLevels2D();
    void unknownCompressedLevels3D();

    void propagateFlags1D();
    void propagateFlags2D();
    void propagateFlags3D();
    void propagateFlagsCompressed1D();
    void propagateFlagsCompressed2D();
    void propagateFlagsCompressed3D();

    void propagateFlagsLevels1D();
    void propagateFlagsLevels2D();
    void propagateFlagsLevels3D();
    void propagateFlagsCompressedLevels1D();
    void propagateFlagsCompressedLevels2D();
    void propagateFlagsCompressedLevels3D();

    void propagateConfiguration1D();
    void propagateConfiguration2D();
    void propagateConfiguration3D();
    void propagateConfigurationUnknown1D();
    void propagateConfigurationUnknown2D();
    void propagateConfigurationUnknown3D();
    void propagateConfigurationCompressed1D();
    void propagateConfigurationCompressed2D();
    void propagateConfigurationCompressed3D();
    void propagateConfigurationCompressedUnknown1D();
    void propagateConfigurationCompressedUnknown2D();
    void propagateConfigurationCompressedUnknown3D();

    void propagateConfigurationLevels1D();
    void propagateConfigurationLevels2D();
    void propagateConfigurationLevels3D();
    void propagateConfigurationUnknownLevels1D();
    void propagateConfigurationUnknownLevels2D();
    void propagateConfigurationUnknownLevels3D();
    void propagateConfigurationCompressedLevels1D();
    void propagateConfigurationCompressedLevels2D();
    void propagateConfigurationCompressedLevels3D();
    void propagateConfigurationCompressedUnknownLevels1D();
    void propagateConfigurationCompressedUnknownLevels2D();
    void propagateConfigurationCompressedUnknownLevels3D();

    /* configuration propagation fully tested in AnySceneImporter, as there the
       plugins have configuration subgroups as well */

    /* Explicitly forbid system-wide plugin dependencies */
    PluginManager::Manager<AbstractImageConverter> _manager{"nonexistent"};
};

constexpr struct {
    const char* name;
    const char* filename;
    const char* plugin;
} Detect2DData[]{
    {"BMP", "file.bmp", "BmpImageConverter"},
    {"Basis Universal", "file.basis", "BasisImageConverter"},
    {"EXR", "file.exr", "OpenExrImageConverter"},
    {"HDR", "file.hdr", "HdrImageConverter"},
    {"JPEG", "file.jpg", "JpegImageConverter"},
    {"JPEG weird extension", "file.jpe", "JpegImageConverter"},
    {"JPEG uppercase", "output.JPG", "JpegImageConverter"},
    {"KTX2", "foo.ktx2", "KtxImageConverter"},
    {"PNG", "file.png", "PngImageConverter"}
};

constexpr struct {
    const char* name;
    const char* filename;
    const char* plugin;
} Detect3DData[]{
    {"EXR", "file.exr", "OpenExrImageConverter"},
    /* Have at least one test case with uppercase */
    {"EXR uppercase", "FIL~1.EXR", "OpenExrImageConverter"}
};

constexpr struct {
    const char* name;
    const char* filename;
    const char* plugin;
} DetectLevels2DData[]{
    {"EXR", "file.exr", "OpenExrImageConverter"},
    /* Have at least one test case with uppercase */
    {"EXR uppercase", "FIL~1.EXR", "OpenExrImageConverter"}
};

constexpr struct {
    const char* name;
    const char* filename;
    const char* plugin;
} DetectLevels3DData[]{
    {"EXR", "file.exr", "OpenExrImageConverter"},
    /* Have at least one test case with uppercase */
    {"EXR uppercase", "FIL~1.EXR", "OpenExrImageConverter"}
};

AnyImageConverterTest::AnyImageConverterTest() {
    addTests({&AnyImageConverterTest::convert1D,
              &AnyImageConverterTest::convert2D,
              &AnyImageConverterTest::convert3D,
              &AnyImageConverterTest::convertCompressed1D,
              &AnyImageConverterTest::convertCompressed2D,
              &AnyImageConverterTest::convertCompressed3D,

              &AnyImageConverterTest::convertLevels1D,
              &AnyImageConverterTest::convertLevels2D,
              &AnyImageConverterTest::convertLevels3D,
              &AnyImageConverterTest::convertCompressedLevels1D,
              &AnyImageConverterTest::convertCompressedLevels2D,
              &AnyImageConverterTest::convertCompressedLevels3D,

              &AnyImageConverterTest::detect1D});

    addInstancedTests({&AnyImageConverterTest::detect2D},
        Containers::arraySize(Detect2DData));

    addInstancedTests({&AnyImageConverterTest::detect3D},
        Containers::arraySize(Detect3DData));

    addTests({&AnyImageConverterTest::detectCompressed1D,
              &AnyImageConverterTest::detectCompressed2D,
              &AnyImageConverterTest::detectCompressed3D,

              &AnyImageConverterTest::detectLevels1D});

    addInstancedTests({&AnyImageConverterTest::detectLevels2D},
        Containers::arraySize(DetectLevels2DData));

    addInstancedTests({&AnyImageConverterTest::detectLevels3D},
        Containers::arraySize(DetectLevels3DData));

    addTests({&AnyImageConverterTest::detectCompressedLevels1D,
              &AnyImageConverterTest::detectCompressedLevels2D,
              &AnyImageConverterTest::detectCompressedLevels3D,

              &AnyImageConverterTest::unknown1D,
              &AnyImageConverterTest::unknown2D,
              &AnyImageConverterTest::unknown3D,
              &AnyImageConverterTest::unknownCompressed1D,
              &AnyImageConverterTest::unknownCompressed2D,
              &AnyImageConverterTest::unknownCompressed3D,

              &AnyImageConverterTest::unknownLevels1D,
              &AnyImageConverterTest::unknownLevels2D,
              &AnyImageConverterTest::unknownLevels3D,
              &AnyImageConverterTest::unknownCompressedLevels1D,
              &AnyImageConverterTest::unknownCompressedLevels2D,
              &AnyImageConverterTest::unknownCompressedLevels3D,

              &AnyImageConverterTest::propagateFlags1D,
              &AnyImageConverterTest::propagateFlags2D,
              &AnyImageConverterTest::propagateFlags3D,
              &AnyImageConverterTest::propagateFlagsCompressed1D,
              &AnyImageConverterTest::propagateFlagsCompressed2D,
              &AnyImageConverterTest::propagateFlagsCompressed3D,

              &AnyImageConverterTest::propagateFlagsLevels1D,
              &AnyImageConverterTest::propagateFlagsLevels2D,
              &AnyImageConverterTest::propagateFlagsLevels3D,
              &AnyImageConverterTest::propagateFlagsCompressedLevels1D,
              &AnyImageConverterTest::propagateFlagsCompressedLevels2D,
              &AnyImageConverterTest::propagateFlagsCompressedLevels3D,

              &AnyImageConverterTest::propagateConfiguration1D,
              &AnyImageConverterTest::propagateConfiguration2D,
              &AnyImageConverterTest::propagateConfiguration3D,
              &AnyImageConverterTest::propagateConfigurationUnknown1D,
              &AnyImageConverterTest::propagateConfigurationUnknown2D,
              &AnyImageConverterTest::propagateConfigurationUnknown3D,
              &AnyImageConverterTest::propagateConfigurationCompressed1D,
              &AnyImageConverterTest::propagateConfigurationCompressed2D,
              &AnyImageConverterTest::propagateConfigurationCompressed3D,
              &AnyImageConverterTest::propagateConfigurationCompressedUnknown1D,
              &AnyImageConverterTest::propagateConfigurationCompressedUnknown2D,
              &AnyImageConverterTest::propagateConfigurationCompressedUnknown3D,

              &AnyImageConverterTest::propagateConfigurationLevels1D,
              &AnyImageConverterTest::propagateConfigurationLevels2D,
              &AnyImageConverterTest::propagateConfigurationLevels3D,
              &AnyImageConverterTest::propagateConfigurationUnknownLevels1D,
              &AnyImageConverterTest::propagateConfigurationUnknownLevels2D,
              &AnyImageConverterTest::propagateConfigurationUnknownLevels3D,
              &AnyImageConverterTest::propagateConfigurationCompressedLevels1D,
              &AnyImageConverterTest::propagateConfigurationCompressedLevels2D,
              &AnyImageConverterTest::propagateConfigurationCompressedLevels3D,
              &AnyImageConverterTest::propagateConfigurationCompressedUnknownLevels1D,
              &AnyImageConverterTest::propagateConfigurationCompressedUnknownLevels2D,
              &AnyImageConverterTest::propagateConfigurationCompressedUnknownLevels3D});

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

/* 2*3*2 RGB pixels with four-byte row padding, or 3 16-byte blocks */
constexpr const char Data[] = {
    1, 2, 3, 2, 3, 4, 0, 0,
    3, 4, 5, 4, 5, 6, 0, 0,
    5, 6, 7, 6, 7, 8, 0, 0,
    7, 8, 9, 8, 9, 0, 0, 0,
    9, 0, 1, 0, 1, 2, 0, 0,
    1, 2, 3, 2, 3, 4, 0, 0
};

constexpr Float CubeData[] = {
    0.125f,
    0.250f,
    0.375f,
    0.500f,
    0.625f,
    0.750f
};

constexpr Float FloatData[] = {
    0.125f, 0.250f, 0.375f,
    0.500f, 0.625f, 0.750f
};

const ImageView1D Image1D{PixelFormat::RGB8Unorm, 2, Data};
const ImageView2D Image2D{PixelFormat::RGB8Unorm, {2, 3}, Data};
const ImageView2D Image2DFloat{PixelFormat::Depth32F, {3, 2}, FloatData};
const ImageView3D Image3D{PixelFormat::RGB8Unorm, {2, 3, 2}, Data};
const ImageView3D ImageCube{PixelFormat::Depth32F, {1, 1, 6}, CubeData};
const CompressedImageView1D CompressedImage1D{CompressedPixelFormat::Bc1RGBAUnorm, 3, Data};
const CompressedImageView2D CompressedImage2D{CompressedPixelFormat::Bc1RGBAUnorm, {1, 3}, Data};
const CompressedImageView3D CompressedImage3D{CompressedPixelFormat::Bc1RGBAUnorm, {1, 1, 3}, Data};

void AnyImageConverterTest::convert1D() {
    CORRADE_SKIP("No file formats to store 1D data yet.");
}

void AnyImageConverterTest::convert2D() {
    if(!(_manager.loadState("TgaImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImageConverter plugin not enabled, cannot test");

    const std::string filename = Utility::Directory::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "output.tga");

    if(Utility::Directory::exists(filename))
        CORRADE_VERIFY(Utility::Directory::rm(filename));

    /* Just test that the exported file exists */
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");
    CORRADE_VERIFY(converter->convertToFile(Image2D, filename));
    CORRADE_VERIFY(Utility::Directory::exists(filename));
}

void AnyImageConverterTest::convert3D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.loadState("OpenExrImageConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("OpenExrImageConverter plugin can't be loaded.");

    const std::string filename = Utility::Directory::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "cube.exr");

    if(Utility::Directory::exists(filename))
        CORRADE_VERIFY(Utility::Directory::rm(filename));

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    /* Well, this is in fact the same as propagateConfiguration3D() but we
       can't really do much else. */
    converter->configuration().setValue("envmap", "cube");
    CORRADE_VERIFY(converter->convertToFile(ImageCube, filename));
    CORRADE_VERIFY(Utility::Directory::exists(filename));
}

void AnyImageConverterTest::convertCompressed1D() {
    CORRADE_SKIP("No file formats to store compressed 1D data yet.");
}

void AnyImageConverterTest::convertCompressed2D() {
    CORRADE_SKIP("No file formats to store compressed 2D data yet.");
}

void AnyImageConverterTest::convertCompressed3D() {
    CORRADE_SKIP("No file formats to store compressed 3D data yet.");
}

void AnyImageConverterTest::convertLevels1D() {
    CORRADE_SKIP("No file formats to store multi-level 1D data yet.");
}

void AnyImageConverterTest::convertLevels2D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.loadState("OpenExrImageConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("OpenExrImageConverter plugin can't be loaded.");

    const std::string filename = Utility::Directory::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "output.exr");

    if(Utility::Directory::exists(filename))
        CORRADE_VERIFY(Utility::Directory::rm(filename));

    /* Just test that the exported file exists */
    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(converter->convertToFile({Image2DFloat}, filename));
    CORRADE_VERIFY(Utility::Directory::exists(filename));
}

void AnyImageConverterTest::convertLevels3D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.loadState("OpenExrImageConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("OpenExrImageConverter plugin can't be loaded.");

    const std::string filename = Utility::Directory::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "cube.exr");

    if(Utility::Directory::exists(filename))
        CORRADE_VERIFY(Utility::Directory::rm(filename));

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    /* Well, this is in fact the same as propagateConfigurationLevels3D() but
       we can't really do much else. */
    converter->configuration().setValue("envmap", "cube");
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(converter->convertToFile({ImageCube}, filename));
    CORRADE_VERIFY(Utility::Directory::exists(filename));
}

void AnyImageConverterTest::convertCompressedLevels1D() {
    CORRADE_SKIP("No file formats to store multi-level compressed 1D data yet.");
}

void AnyImageConverterTest::convertCompressedLevels2D() {
    CORRADE_SKIP("No file formats to store multi-level compressed 2D data yet.");
}

void AnyImageConverterTest::convertCompressedLevels3D() {
    CORRADE_SKIP("No file formats to store multi-level compressed 3D data yet.");
}

void AnyImageConverterTest::detect1D() {
    CORRADE_SKIP("No file formats to store 1D data yet.");
}

void AnyImageConverterTest::detect2D() {
    auto&& data = Detect2DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile(Image2D, data.filename));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n",
        data.plugin));
    #else
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "PluginManager::Manager::load(): plugin {0} was not found\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n", data.plugin));
    #endif
}

void AnyImageConverterTest::detect3D() {
    auto&& data = Detect3DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile(Image2D, data.filename));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n",
        data.plugin));
    #else
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "PluginManager::Manager::load(): plugin {0} was not found\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n",
        data.plugin));
    #endif
}

void AnyImageConverterTest::detectCompressed1D() {
    CORRADE_SKIP("No file formats to store compressed 1D data yet.");
}

void AnyImageConverterTest::detectCompressed2D() {
    CORRADE_SKIP("No file formats to store compressed 2D data yet.");
}

void AnyImageConverterTest::detectCompressed3D() {
    CORRADE_SKIP("No file formats to store compressed 3D data yet.");
}

void AnyImageConverterTest::detectLevels1D() {
    CORRADE_SKIP("No file formats to store multi-level 1D data yet.");
}

void AnyImageConverterTest::detectLevels2D() {
    auto&& data = DetectLevels2DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    std::ostringstream out;
    Error redirectError{&out};
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(!converter->convertToFile({Image2D}, data.filename));
    /* Can't use raw string literals in macros on GCC 4.8 */
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n",
        data.plugin));
    #else
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "PluginManager::Manager::load(): plugin {0} was not found\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n",
        data.plugin));
    #endif
}

void AnyImageConverterTest::detectLevels3D() {
    auto&& data = DetectLevels3DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    std::ostringstream out;
    Error redirectError{&out};
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(!converter->convertToFile({Image3D}, data.filename));
    /* Can't use raw string literals in macros on GCC 4.8 */
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n",
        data.plugin));
    #else
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "PluginManager::Manager::load(): plugin {0} was not found\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n",
        data.plugin));
    #endif
}

void AnyImageConverterTest::detectCompressedLevels1D() {
    CORRADE_SKIP("No file formats to store multi-level compressed 1D data yet.");
}

void AnyImageConverterTest::detectCompressedLevels2D() {
    CORRADE_SKIP("No file formats to store multi-level compressed 2D data yet.");
}

void AnyImageConverterTest::detectCompressedLevels3D() {
    CORRADE_SKIP("No file formats to store multi-level compressed 3D data yet.");
}

void AnyImageConverterTest::unknown1D() {
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile(Image1D, "image.ktx2"));
    CORRADE_COMPARE(out.str(), "Trade::AnyImageConverter::convertToFile(): cannot determine the format of image.ktx2 for a 1D image\n");
}

void AnyImageConverterTest::unknown2D() {
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile(Image2D, "image.xcf"));
    CORRADE_COMPARE(out.str(), "Trade::AnyImageConverter::convertToFile(): cannot determine the format of image.xcf for a 2D image\n");
}

void AnyImageConverterTest::unknown3D() {
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile(Image3D, "image.ktx2"));
    CORRADE_COMPARE(out.str(), "Trade::AnyImageConverter::convertToFile(): cannot determine the format of image.ktx2 for a 3D image\n");
}

void AnyImageConverterTest::unknownCompressed1D() {
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile(CompressedImage1D, "image.ktx2"));
    CORRADE_COMPARE(out.str(), "Trade::AnyImageConverter::convertToFile(): cannot determine the format of image.ktx2 for a compressed 1D image\n");
}

void AnyImageConverterTest::unknownCompressed2D() {
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile(CompressedImage2D, "image.ktx2"));
    CORRADE_COMPARE(out.str(), "Trade::AnyImageConverter::convertToFile(): cannot determine the format of image.ktx2 for a compressed 2D image\n");
}

void AnyImageConverterTest::unknownCompressed3D() {
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile(CompressedImage3D, "image.ktx2"));
    CORRADE_COMPARE(out.str(), "Trade::AnyImageConverter::convertToFile(): cannot determine the format of image.ktx2 for a compressed 3D image\n");
}

void AnyImageConverterTest::unknownLevels1D() {
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile({Image1D}, "image.ktx2"));
    CORRADE_COMPARE(out.str(), "Trade::AnyImageConverter::convertToFile(): cannot determine the format of image.ktx2 for a multi-level 1D image\n");
}

void AnyImageConverterTest::unknownLevels2D() {
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile({Image2D}, "image.ktx2"));
    CORRADE_COMPARE(out.str(), "Trade::AnyImageConverter::convertToFile(): cannot determine the format of image.ktx2 for a multi-level 2D image\n");
}

void AnyImageConverterTest::unknownLevels3D() {
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile({Image3D}, "image.ktx2"));
    CORRADE_COMPARE(out.str(), "Trade::AnyImageConverter::convertToFile(): cannot determine the format of image.ktx2 for a multi-level 3D image\n");
}

void AnyImageConverterTest::unknownCompressedLevels1D() {
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile({CompressedImage1D}, "image.ktx2"));
    CORRADE_COMPARE(out.str(), "Trade::AnyImageConverter::convertToFile(): cannot determine the format of image.ktx2 for a multi-level compressed 1D image\n");
}

void AnyImageConverterTest::unknownCompressedLevels2D() {
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile({CompressedImage2D}, "image.ktx2"));
    CORRADE_COMPARE(out.str(), "Trade::AnyImageConverter::convertToFile(): cannot determine the format of image.ktx2 for a multi-level compressed 2D image\n");
}

void AnyImageConverterTest::unknownCompressedLevels3D() {
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile({CompressedImage3D}, "image.ktx2"));
    CORRADE_COMPARE(out.str(), "Trade::AnyImageConverter::convertToFile(): cannot determine the format of image.ktx2 for a multi-level compressed 3D image\n");
}

void AnyImageConverterTest::propagateFlags1D() {
    CORRADE_SKIP("No file formats to store 1D data yet.");
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
        CORRADE_VERIFY(converter->convertToFile(Image2D, filename));
    }
    CORRADE_VERIFY(Utility::Directory::exists(filename));
    CORRADE_COMPARE(out.str(),
        "Trade::AnyImageConverter::convertToFile(): using TgaImageConverter\n"
        "Trade::TgaImageConverter::convertToData(): converting from RGB to BGR\n");
}

void AnyImageConverterTest::propagateFlags3D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.loadState("OpenExrImageConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("OpenExrImageConverter plugin can't be loaded.");

    const std::string filename = Utility::Directory::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "cube.exr");

    if(Utility::Directory::exists(filename))
        CORRADE_VERIFY(Utility::Directory::rm(filename));

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("envmap", "cube");
    /* This will make the verbose output print the detected hardware thread
       count, but also the info about updating global thread count for the
       first time. Thus run it once w/o a verbose flag and then again with to
       filter out the other message. */
    /** @todo switch to testing something else when there is */
    converter->configuration().setValue("threads", 0);
    CORRADE_VERIFY(converter->convertToFile(ImageCube, filename));

    if(Utility::Directory::exists(filename))
        CORRADE_VERIFY(Utility::Directory::rm(filename));

    converter->setFlags(ImageConverterFlag::Verbose);
    std::ostringstream out;
    {
        Debug redirectOutput{&out};
        CORRADE_VERIFY(converter->convertToFile(ImageCube, filename));
    }
    CORRADE_VERIFY(Utility::Directory::exists(filename));
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "Trade::AnyImageConverter::convertToFile(): using OpenExrImageConverter\n"
        "Trade::OpenExrImageConverter::convertToData(): autodetected hardware concurrency to {} threads\n",
        std::thread::hardware_concurrency()));
}

void AnyImageConverterTest::propagateFlagsCompressed1D() {
    CORRADE_SKIP("No file formats to store compressed 1D data yet.");
}

void AnyImageConverterTest::propagateFlagsCompressed2D() {
    CORRADE_SKIP("No file formats to store compressed 2D data yet.");
}

void AnyImageConverterTest::propagateFlagsCompressed3D() {
    CORRADE_SKIP("No file formats to store compressed 3D data yet.");
}

void AnyImageConverterTest::propagateFlagsLevels1D() {
    CORRADE_SKIP("No file formats to store multi-level 1D data yet.");
}

void AnyImageConverterTest::propagateFlagsLevels2D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.loadState("OpenExrImageConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("OpenExrImageConverter plugin can't be loaded.");

    const std::string filename = Utility::Directory::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "output.exr");

    if(Utility::Directory::exists(filename))
        CORRADE_VERIFY(Utility::Directory::rm(filename));

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    /* This will make the verbose output print the detected hardware thread
       count, but also the info about updating global thread count for the
       first time. Thus run it once w/o a verbose flag and then again with to
       filter out the other message. */
    /** @todo switch to testing something else when there is */
    converter->configuration().setValue("threads", 0);
    CORRADE_VERIFY(converter->convertToFile({Image2DFloat}, filename));

    if(Utility::Directory::exists(filename))
        CORRADE_VERIFY(Utility::Directory::rm(filename));

    converter->setFlags(ImageConverterFlag::Verbose);
    std::ostringstream out;
    {
        Debug redirectOutput{&out};
        /* Using the list API even though there's just one image, which should
           still trigger the correct code path for AnyImageConverter. */
        CORRADE_VERIFY(converter->convertToFile({Image2DFloat}, filename));
    }
    CORRADE_VERIFY(Utility::Directory::exists(filename));
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "Trade::AnyImageConverter::convertToFile(): using OpenExrImageConverter\n"
        "Trade::OpenExrImageConverter::convertToData(): autodetected hardware concurrency to {} threads\n",
        std::thread::hardware_concurrency()));
}

void AnyImageConverterTest::propagateFlagsLevels3D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.loadState("OpenExrImageConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("OpenExrImageConverter plugin can't be loaded.");

    const std::string filename = Utility::Directory::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "cube.exr");

    if(Utility::Directory::exists(filename))
        CORRADE_VERIFY(Utility::Directory::rm(filename));

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("envmap", "cube");
    /* This will make the verbose output print the detected hardware thread
       count, but also the info about updating global thread count for the
       first time. Thus run it once w/o a verbose flag and then again with to
       filter out the other message. */
    /** @todo switch to testing something else when there is */
    converter->configuration().setValue("threads", 0);
    CORRADE_VERIFY(converter->convertToFile({ImageCube}, filename));

    if(Utility::Directory::exists(filename))
        CORRADE_VERIFY(Utility::Directory::rm(filename));

    converter->setFlags(ImageConverterFlag::Verbose);
    std::ostringstream out;
    {
        Debug redirectOutput{&out};
        /* Using the list API even though there's just one image, which should
           still trigger the correct code path for AnyImageConverter. */
        CORRADE_VERIFY(converter->convertToFile({ImageCube}, filename));
    }
    CORRADE_VERIFY(Utility::Directory::exists(filename));
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "Trade::AnyImageConverter::convertToFile(): using OpenExrImageConverter\n"
        "Trade::OpenExrImageConverter::convertToData(): autodetected hardware concurrency to {} threads\n",
        std::thread::hardware_concurrency()));
}

void AnyImageConverterTest::propagateFlagsCompressedLevels1D() {
    CORRADE_SKIP("No file formats to store multi-level compressed 1D data yet.");
}

void AnyImageConverterTest::propagateFlagsCompressedLevels2D() {
    CORRADE_SKIP("No file formats to store multi-level compressed 2D data yet.");
}

void AnyImageConverterTest::propagateFlagsCompressedLevels3D() {
    CORRADE_SKIP("No file formats to store multi-level compressed 3D data yet.");
}

void AnyImageConverterTest::propagateConfiguration1D() {
    CORRADE_SKIP("No file formats to store 1D data yet.");
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

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("layer", "left");
    converter->configuration().setValue("depth", "height");
    CORRADE_VERIFY(converter->convertToFile(Image2DFloat, filename));
    /* Compare to an expected output to ensure the custom channels names were
       used */
    CORRADE_COMPARE_AS(filename, EXR_FILE, TestSuite::Compare::File);
}

void AnyImageConverterTest::propagateConfiguration3D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.loadState("OpenExrImageConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("OpenExrImageConverter plugin can't be loaded.");

    const std::string filename = Utility::Directory::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "cube.exr");

    if(Utility::Directory::exists(filename))
        CORRADE_VERIFY(Utility::Directory::rm(filename));

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    /* This should be enough to test -- 3D images can be saved only if this
       option is set */
    converter->configuration().setValue("envmap", "cube");
    CORRADE_VERIFY(converter->convertToFile(ImageCube, filename));
    /* Compare to an expected output to ensure we actually saved the file
       including the metadata. This also doubles as a generator for the
       cube.exr file that AnyImageImporterTest uses. */
    CORRADE_COMPARE_AS(filename, EXR_CUBE_FILE, TestSuite::Compare::File);
}

void AnyImageConverterTest::propagateConfigurationUnknown1D() {
    CORRADE_SKIP("No file formats to store 1D data yet.");
}

void AnyImageConverterTest::propagateConfigurationUnknown2D() {
    if(!(_manager.loadState("TgaImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImageConverter plugin not enabled, cannot test");

    /* Just test that the exported file exists */
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("noSuchOption", "isHere");

    std::ostringstream out;
    Warning redirectWarning{&out};
    CORRADE_VERIFY(converter->convertToFile(Image2D, Utility::Directory::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "output.tga")));
    CORRADE_COMPARE(out.str(), "Trade::AnyImageConverter::convertToFile(): option noSuchOption not recognized by TgaImageConverter\n");
}

void AnyImageConverterTest::propagateConfigurationUnknown3D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.loadState("OpenExrImageConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("OpenExrImageConverter plugin can't be loaded.");

    /* Just test that the exported file exists */
    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("envmap", "cube");
    converter->configuration().setValue("noSuchOption", "isHere");

    std::ostringstream out;
    Warning redirectWarning{&out};
    CORRADE_VERIFY(converter->convertToFile(ImageCube, Utility::Directory::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "output.exr")));
    CORRADE_COMPARE(out.str(), "Trade::AnyImageConverter::convertToFile(): option noSuchOption not recognized by OpenExrImageConverter\n");
}

void AnyImageConverterTest::propagateConfigurationCompressed1D() {
    CORRADE_SKIP("No file formats to store compressed 1D data yet.");
}

void AnyImageConverterTest::propagateConfigurationCompressed2D() {
    CORRADE_SKIP("No file formats to store compressed 2D data yet.");
}

void AnyImageConverterTest::propagateConfigurationCompressed3D() {
    CORRADE_SKIP("No file formats to store compressed 3D data yet.");
}

void AnyImageConverterTest::propagateConfigurationCompressedUnknown1D() {
    CORRADE_SKIP("No file formats to store compressed 1D data yet.");
}

void AnyImageConverterTest::propagateConfigurationCompressedUnknown2D() {
    CORRADE_SKIP("No file formats to store compressed 2D data yet.");
}

void AnyImageConverterTest::propagateConfigurationCompressedUnknown3D() {
    CORRADE_SKIP("No file formats to store compressed 3D data yet.");
}

void AnyImageConverterTest::propagateConfigurationLevels1D() {
    CORRADE_SKIP("No file formats to store multi-level 1D data yet.");
}

void AnyImageConverterTest::propagateConfigurationLevels2D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.loadState("OpenExrImageConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("OpenExrImageConverter plugin can't be loaded.");

    const std::string filename = Utility::Directory::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "depth32f-custom-channels.exr");

    if(Utility::Directory::exists(filename))
        CORRADE_VERIFY(Utility::Directory::rm(filename));

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("layer", "left");
    converter->configuration().setValue("depth", "height");
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. For
       OpenExrImageConverter both single and list are the same code path so we
       can reuse the same expected file. */
    CORRADE_VERIFY(converter->convertToFile({Image2DFloat}, filename));
    /* Compare to an expected output to ensure the custom channels names were
       used */
    CORRADE_COMPARE_AS(filename, EXR_FILE, TestSuite::Compare::File);
}

void AnyImageConverterTest::propagateConfigurationLevels3D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.loadState("OpenExrImageConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("OpenExrImageConverter plugin can't be loaded.");

    const std::string filename = Utility::Directory::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "cube.exr");

    if(Utility::Directory::exists(filename))
        CORRADE_VERIFY(Utility::Directory::rm(filename));

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    /* This should be enough to test -- 3D images can be saved only if this
       option is set */
    converter->configuration().setValue("envmap", "cube");
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. For
       OpenExrImageConverter both single and list are the same code path so we
       can reuse the same expected file. */
    CORRADE_VERIFY(converter->convertToFile({ImageCube}, filename));
    /* Compare to an expected output to ensure we actually saved the file */
    CORRADE_COMPARE_AS(filename, EXR_CUBE_FILE, TestSuite::Compare::File);
}

void AnyImageConverterTest::propagateConfigurationUnknownLevels1D() {
    CORRADE_SKIP("No file formats to store multi-level 1D data yet.");
}

void AnyImageConverterTest::propagateConfigurationUnknownLevels2D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.loadState("OpenExrImageConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("OpenExrImageConverter plugin can't be loaded.");

    const std::string filename = Utility::Directory::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "depth32f-custom-channels.exr");

    if(Utility::Directory::exists(filename))
        CORRADE_VERIFY(Utility::Directory::rm(filename));

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("noSuchOption", "isHere");

    std::ostringstream out;
    Warning redirectWarning{&out};
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(converter->convertToFile({Image2DFloat}, Utility::Directory::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "output.exr")));
    CORRADE_COMPARE(out.str(), "Trade::AnyImageConverter::convertToFile(): option noSuchOption not recognized by OpenExrImageConverter\n");
}

void AnyImageConverterTest::propagateConfigurationUnknownLevels3D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.loadState("OpenExrImageConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("OpenExrImageConverter plugin can't be loaded.");

    /* Just test that the exported file exists */
    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("envmap", "cube");
    converter->configuration().setValue("noSuchOption", "isHere");

    std::ostringstream out;
    Warning redirectWarning{&out};
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(converter->convertToFile({ImageCube}, Utility::Directory::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "output.exr")));
    CORRADE_COMPARE(out.str(), "Trade::AnyImageConverter::convertToFile(): option noSuchOption not recognized by OpenExrImageConverter\n");
}

void AnyImageConverterTest::propagateConfigurationCompressedLevels1D() {
    CORRADE_SKIP("No file formats to store multi-level compressed 1D data yet.");
}

void AnyImageConverterTest::propagateConfigurationCompressedLevels2D() {
    CORRADE_SKIP("No file formats to store multi-level compressed 2D data yet.");
}

void AnyImageConverterTest::propagateConfigurationCompressedLevels3D() {
    CORRADE_SKIP("No file formats to store multi-level compressed 3D data yet.");
}

void AnyImageConverterTest::propagateConfigurationCompressedUnknownLevels1D() {
    CORRADE_SKIP("No file formats to store multi-level compressed 1D data yet.");
}

void AnyImageConverterTest::propagateConfigurationCompressedUnknownLevels2D() {
    CORRADE_SKIP("No file formats to store multi-level compressed 2D data yet.");
}

void AnyImageConverterTest::propagateConfigurationCompressedUnknownLevels3D() {
    CORRADE_SKIP("No file formats to store multi-level compressed 3D data yet.");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AnyImageConverterTest)
