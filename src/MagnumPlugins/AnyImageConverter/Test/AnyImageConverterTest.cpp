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

#include <thread> /* std::thread::hardware_concurrency(), sigh */
#include <Corrade/Containers/Optional.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/PluginManager/PluginMetadata.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/File.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/ConfigurationGroup.h>
#include <Corrade/Utility/Format.h>
#include <Corrade/Utility/Path.h>

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
} Detect1DData[]{
    {"KTX2", "file.ktx2", "KtxImageConverter"},
    /* Have at least one test case with uppercase */
    {"KTX2 uppercase", "FIL~1.KTX2", "KtxImageConverter"}
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
    /* Have at least one test case with uppercase */
    {"JPEG uppercase", "output.JPG", "JpegImageConverter"},
    {"KTX2", "foo.ktx2", "KtxImageConverter"},
    {"PNG", "file.png", "PngImageConverter"},
    {"WebP", "file.webp", "WebPImageConverter"}
};

constexpr struct {
    const char* name;
    const char* filename;
    const char* plugin;
} Detect3DData[]{
    {"Basis Universal", "file.basis", "BasisImageConverter"},
    {"EXR", "file.exr", "OpenExrImageConverter"},
    {"KTX2", "file.ktx2", "KtxImageConverter"},
    {"OpenVDB", "volume.vdb", "OpenVdbImageConverter"},
    /* Have at least one test case with uppercase */
    {"EXR uppercase", "FIL~1.EXR", "OpenExrImageConverter"}
};

constexpr struct {
    const char* name;
    const char* filename;
    const char* plugin;
} DetectLevels1DData[]{
    {"KTX2", "file.ktx2", "KtxImageConverter"},
    /* Have at least one test case with uppercase */
    {"KTX2 uppercase", "FIL~1.KTX2", "KtxImageConverter"}
};

constexpr struct {
    const char* name;
    const char* filename;
    const char* plugin;
} DetectLevels2DData[]{
    {"Basis Universal", "file.basis", "BasisImageConverter"},
    {"EXR", "file.exr", "OpenExrImageConverter"},
    {"KTX2", "file.ktx2", "KtxImageConverter"},
    /* Have at least one test case with uppercase */
    {"EXR uppercase", "FIL~1.EXR", "OpenExrImageConverter"}
};

constexpr struct {
    const char* name;
    const char* filename;
    const char* plugin;
} DetectLevels3DData[]{
    {"Basis Universal", "file.basis", "BasisImageConverter"},
    {"EXR", "file.exr", "OpenExrImageConverter"},
    {"KTX2", "file.ktx2", "KtxImageConverter"},
    /* Have at least one test case with uppercase */
    {"EXR uppercase", "FIL~1.EXR", "OpenExrImageConverter"}
};

/* It's all duplicated, yes, but I expect at least the 2D compressed cases to
   grow a bit */

constexpr struct {
    const char* name;
    const char* filename;
    const char* plugin;
} DetectCompressed1DData[]{
    {"KTX2", "file.ktx2", "KtxImageConverter"},
    /* Have at least one test case with uppercase */
    {"KTX2 uppercase", "FIL~1.KTX2", "KtxImageConverter"}
};

constexpr struct {
    const char* name;
    const char* filename;
    const char* plugin;
} DetectCompressed2DData[]{
    {"KTX2", "file.ktx2", "KtxImageConverter"},
    /* Have at least one test case with uppercase */
    {"KTX2 uppercase", "FIL~1.KTX2", "KtxImageConverter"}
};

constexpr struct {
    const char* name;
    const char* filename;
    const char* plugin;
} DetectCompressed3DData[]{
    {"KTX2", "file.ktx2", "KtxImageConverter"},
    /* Have at least one test case with uppercase */
    {"KTX2 uppercase", "FIL~1.KTX2", "KtxImageConverter"}
};

constexpr struct {
    const char* name;
    const char* filename;
    const char* plugin;
} DetectCompressedLevels1DData[]{
    {"KTX2", "file.ktx2", "KtxImageConverter"},
    /* Have at least one test case with uppercase */
    {"KTX2 uppercase", "FIL~1.KTX2", "KtxImageConverter"}
};

constexpr struct {
    const char* name;
    const char* filename;
    const char* plugin;
} DetectCompressedLevels2DData[]{
    {"KTX2", "file.ktx2", "KtxImageConverter"},
    /* Have at least one test case with uppercase */
    {"KTX2 uppercase", "FIL~1.KTX2", "KtxImageConverter"}
};

constexpr struct {
    const char* name;
    const char* filename;
    const char* plugin;
} DetectCompressedLevels3DData[]{
    {"KTX2", "file.ktx2", "KtxImageConverter"},
    /* Have at least one test case with uppercase */
    {"KTX2 uppercase", "FIL~1.KTX2", "KtxImageConverter"}
};

const struct {
    const char* name;
    ImageConverterFlags flags;
    bool quiet;
} PropagateConfigurationUnknownData[]{
    {"", {}, false},
    {"quiet", ImageConverterFlag::Quiet, true}
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
              &AnyImageConverterTest::convertCompressedLevels3D});

    addInstancedTests({&AnyImageConverterTest::detect1D},
        Containers::arraySize(Detect1DData));

    addInstancedTests({&AnyImageConverterTest::detect2D},
        Containers::arraySize(Detect2DData));

    addInstancedTests({&AnyImageConverterTest::detect3D},
        Containers::arraySize(Detect3DData));

    addInstancedTests({&AnyImageConverterTest::detectCompressed1D},
        Containers::arraySize(DetectCompressed1DData));

    addInstancedTests({&AnyImageConverterTest::detectCompressed2D},
        Containers::arraySize(DetectCompressed2DData));

    addInstancedTests({&AnyImageConverterTest::detectCompressed3D},
        Containers::arraySize(DetectCompressed3DData));

    addInstancedTests({&AnyImageConverterTest::detectLevels1D},
        Containers::arraySize(DetectLevels1DData));

    addInstancedTests({&AnyImageConverterTest::detectLevels2D},
        Containers::arraySize(DetectLevels2DData));

    addInstancedTests({&AnyImageConverterTest::detectLevels3D},
        Containers::arraySize(DetectLevels3DData));

    addInstancedTests({&AnyImageConverterTest::detectCompressedLevels1D},
        Containers::arraySize(DetectCompressedLevels1DData));

    addInstancedTests({&AnyImageConverterTest::detectCompressedLevels2D},
        Containers::arraySize(DetectCompressedLevels2DData));

    addInstancedTests({&AnyImageConverterTest::detectCompressedLevels3D},
        Containers::arraySize(DetectCompressedLevels3DData));

    addTests({&AnyImageConverterTest::unknown1D,
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
              &AnyImageConverterTest::propagateConfiguration3D});

    addInstancedTests({
        &AnyImageConverterTest::propagateConfigurationUnknown1D,
        &AnyImageConverterTest::propagateConfigurationUnknown2D,
        &AnyImageConverterTest::propagateConfigurationUnknown3D},
        Containers::arraySize(PropagateConfigurationUnknownData));

    addTests({&AnyImageConverterTest::propagateConfigurationCompressed1D,
              &AnyImageConverterTest::propagateConfigurationCompressed2D,
              &AnyImageConverterTest::propagateConfigurationCompressed3D});

    addInstancedTests({
        &AnyImageConverterTest::propagateConfigurationCompressedUnknown1D,
        &AnyImageConverterTest::propagateConfigurationCompressedUnknown2D,
        &AnyImageConverterTest::propagateConfigurationCompressedUnknown3D},
        Containers::arraySize(PropagateConfigurationUnknownData));

    addTests({&AnyImageConverterTest::propagateConfigurationLevels1D,
              &AnyImageConverterTest::propagateConfigurationLevels2D,
              &AnyImageConverterTest::propagateConfigurationLevels3D});

    addInstancedTests({
        &AnyImageConverterTest::propagateConfigurationUnknownLevels1D,
        &AnyImageConverterTest::propagateConfigurationUnknownLevels2D,
        &AnyImageConverterTest::propagateConfigurationUnknownLevels3D},
        Containers::arraySize(PropagateConfigurationUnknownData));

    addTests({&AnyImageConverterTest::propagateConfigurationCompressedLevels1D,
              &AnyImageConverterTest::propagateConfigurationCompressedLevels2D,
              &AnyImageConverterTest::propagateConfigurationCompressedLevels3D});

    addInstancedTests({
        &AnyImageConverterTest::propagateConfigurationCompressedUnknownLevels1D,
        &AnyImageConverterTest::propagateConfigurationCompressedUnknownLevels2D,
        &AnyImageConverterTest::propagateConfigurationCompressedUnknownLevels3D},
        Containers::arraySize(PropagateConfigurationUnknownData));

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
    CORRADE_INTERNAL_ASSERT_OUTPUT(Utility::Path::make(ANYIMAGECONVERTER_TEST_OUTPUT_DIR));
}

using namespace Containers::Literals;

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
const ImageView3D ImageCube{PixelFormat::Depth32F, {1, 1, 6}, CubeData, ImageFlag3D::CubeMap};
const CompressedImageView1D CompressedImage1D{CompressedPixelFormat::Bc1RGBAUnorm, 3, Data};
const CompressedImageView2D CompressedImage2D{CompressedPixelFormat::Bc1RGBAUnorm, {1, 3}, Data};
const CompressedImageView3D CompressedImage3D{CompressedPixelFormat::Bc1RGBAUnorm, {1, 1, 3}, Data};

void AnyImageConverterTest::convert1D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    /* Drop version info from KtxImageConverter generator name */
    manager.metadata("KtxImageConverter")->configuration().setValue("generator", "Magnum KtxImageConverter");

    Containers::String filename = Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "1d.ktx2");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    CORRADE_VERIFY(converter->convertToFile(Image1D, filename));
    /* Compare to an expected output to ensure we actually saved the file
       including the metadata */
    CORRADE_COMPARE_AS(filename, Utility::Path::join(ANYIMAGECONVERTER_TEST_DIR, "1d.ktx2"), TestSuite::Compare::File);
}

void AnyImageConverterTest::convert2D() {
    if(!(_manager.loadState("TgaImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImageConverter plugin not enabled, cannot test");

    Containers::String filename = Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "2d.tga");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    /* Just test that the exported file exists */
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");
    CORRADE_VERIFY(converter->convertToFile(Image2D, filename));
    CORRADE_VERIFY(Utility::Path::exists(filename));
}

void AnyImageConverterTest::convert3D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    /* Drop version info from KtxImageConverter generator name */
    manager.metadata("KtxImageConverter")->configuration().setValue("generator", "Magnum KtxImageConverter");

    Containers::String filename = Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "3d.ktx2");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    CORRADE_VERIFY(converter->convertToFile(Image3D, filename));
    /* Compare to an expected output to ensure we actually saved the file
       including the metadata */
    CORRADE_COMPARE_AS(filename, Utility::Path::join(ANYIMAGECONVERTER_TEST_DIR, "3d.ktx2"), TestSuite::Compare::File);
}

void AnyImageConverterTest::convertCompressed1D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "compressed-1d.ktx2");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    /* Just test that the exported file exists */
    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    CORRADE_VERIFY(converter->convertToFile(CompressedImage1D, filename));
    CORRADE_VERIFY(Utility::Path::exists(filename));
}

void AnyImageConverterTest::convertCompressed2D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "compressed-2d.ktx2");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    /* Just test that the exported file exists */
    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    CORRADE_VERIFY(converter->convertToFile(CompressedImage2D, filename));
    CORRADE_VERIFY(Utility::Path::exists(filename));
}

void AnyImageConverterTest::convertCompressed3D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "compressed-3d.ktx2");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    /* Just test that the exported file exists */
    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    CORRADE_VERIFY(converter->convertToFile(CompressedImage3D, filename));
    CORRADE_VERIFY(Utility::Path::exists(filename));
}

void AnyImageConverterTest::convertLevels1D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "levels-1d.ktx2");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    /* Just test that the exported file exists */
    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(converter->convertToFile({Image1D}, filename));
    CORRADE_VERIFY(Utility::Path::exists(filename));
}

void AnyImageConverterTest::convertLevels2D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "levels-2d.ktx2");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    /* Just test that the exported file exists */
    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(converter->convertToFile({Image2D}, filename));
    CORRADE_VERIFY(Utility::Path::exists(filename));
}

void AnyImageConverterTest::convertLevels3D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "levels-3d.ktx2");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    /* Just test that the exported file exists */
    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(converter->convertToFile({Image3D}, filename));
    CORRADE_VERIFY(Utility::Path::exists(filename));
}

void AnyImageConverterTest::convertCompressedLevels1D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "compressed-levels-1d.ktx2");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    /* Just test that the exported file exists */
    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(converter->convertToFile({CompressedImage1D}, filename));
    CORRADE_VERIFY(Utility::Path::exists(filename));
}

void AnyImageConverterTest::convertCompressedLevels2D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "compressed-levels-2d.ktx2");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    /* Just test that the exported file exists */
    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(converter->convertToFile({CompressedImage2D}, filename));
    CORRADE_VERIFY(Utility::Path::exists(filename));
}

void AnyImageConverterTest::convertCompressedLevels3D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "compressed-levels-3d.ktx2");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    /* Just test that the exported file exists */
    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(converter->convertToFile({CompressedImage3D}, filename));
    CORRADE_VERIFY(Utility::Path::exists(filename));
}

void AnyImageConverterTest::detect1D() {
    auto&& data = Detect1DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile(Image1D, data.filename));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n",
        data.plugin));
    #else
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} was not found\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n", data.plugin));
    #endif
}

void AnyImageConverterTest::detect2D() {
    auto&& data = Detect2DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile(Image2D, data.filename));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n",
        data.plugin));
    #else
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} was not found\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n", data.plugin));
    #endif
}

void AnyImageConverterTest::detect3D() {
    auto&& data = Detect3DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile(Image3D, data.filename));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n",
        data.plugin));
    #else
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} was not found\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n",
        data.plugin));
    #endif
}

void AnyImageConverterTest::detectCompressed1D() {
    auto&& data = DetectCompressed1DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile(CompressedImage1D, data.filename));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n",
        data.plugin));
    #else
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} was not found\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n", data.plugin));
    #endif
}

void AnyImageConverterTest::detectCompressed2D() {
    auto&& data = DetectCompressed1DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile(CompressedImage2D, data.filename));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n",
        data.plugin));
    #else
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} was not found\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n", data.plugin));
    #endif
}

void AnyImageConverterTest::detectCompressed3D() {
    auto&& data = DetectCompressed1DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile(CompressedImage3D, data.filename));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n",
        data.plugin));
    #else
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} was not found\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n", data.plugin));
    #endif
}

void AnyImageConverterTest::detectLevels1D() {
    auto&& data = DetectLevels1DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    Containers::String out;
    Error redirectError{&out};
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(!converter->convertToFile({Image1D}, data.filename));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n",
        data.plugin));
    #else
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} was not found\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n", data.plugin));
    #endif
}

void AnyImageConverterTest::detectLevels2D() {
    auto&& data = DetectLevels2DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    Containers::String out;
    Error redirectError{&out};
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(!converter->convertToFile({Image2D}, data.filename));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n",
        data.plugin));
    #else
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} was not found\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n",
        data.plugin));
    #endif
}

void AnyImageConverterTest::detectLevels3D() {
    auto&& data = DetectLevels3DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    Containers::String out;
    Error redirectError{&out};
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(!converter->convertToFile({Image3D}, data.filename));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n",
        data.plugin));
    #else
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} was not found\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n",
        data.plugin));
    #endif
}

void AnyImageConverterTest::detectCompressedLevels1D() {
    auto&& data = DetectCompressedLevels1DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    Containers::String out;
    Error redirectError{&out};
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(!converter->convertToFile({CompressedImage1D}, data.filename));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n",
        data.plugin));
    #else
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} was not found\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n",
        data.plugin));
    #endif
}

void AnyImageConverterTest::detectCompressedLevels2D() {
    auto&& data = DetectCompressedLevels2DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    Containers::String out;
    Error redirectError{&out};
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(!converter->convertToFile({CompressedImage2D}, data.filename));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n",
        data.plugin));
    #else
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} was not found\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n",
        data.plugin));
    #endif
}

void AnyImageConverterTest::detectCompressedLevels3D() {
    auto&& data = DetectCompressedLevels3DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    Containers::String out;
    Error redirectError{&out};
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(!converter->convertToFile({CompressedImage3D}, data.filename));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n",
        data.plugin));
    #else
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} was not found\n"
        "Trade::AnyImageConverter::convertToFile(): cannot load the {0} plugin\n",
        data.plugin));
    #endif
}

void AnyImageConverterTest::unknown1D() {
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile(Image1D, "image.jpg"));
    CORRADE_COMPARE(out, "Trade::AnyImageConverter::convertToFile(): cannot determine the format of image.jpg for a 1D image\n");
}

void AnyImageConverterTest::unknown2D() {
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile(Image2D, "image.xcf"));
    CORRADE_COMPARE(out, "Trade::AnyImageConverter::convertToFile(): cannot determine the format of image.xcf for a 2D image\n");
}

void AnyImageConverterTest::unknown3D() {
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile(Image3D, "image.dds"));
    CORRADE_COMPARE(out, "Trade::AnyImageConverter::convertToFile(): cannot determine the format of image.dds for a 3D image\n");
}

void AnyImageConverterTest::unknownCompressed1D() {
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile(CompressedImage1D, "image.exr"));
    CORRADE_COMPARE(out, "Trade::AnyImageConverter::convertToFile(): cannot determine the format of image.exr for a compressed 1D image\n");
}

void AnyImageConverterTest::unknownCompressed2D() {
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile(CompressedImage2D, "image.png"));
    CORRADE_COMPARE(out, "Trade::AnyImageConverter::convertToFile(): cannot determine the format of image.png for a compressed 2D image\n");
}

void AnyImageConverterTest::unknownCompressed3D() {
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile(CompressedImage3D, "image.exr"));
    CORRADE_COMPARE(out, "Trade::AnyImageConverter::convertToFile(): cannot determine the format of image.exr for a compressed 3D image\n");
}

void AnyImageConverterTest::unknownLevels1D() {
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile({Image1D}, "image.tga"));
    CORRADE_COMPARE(out, "Trade::AnyImageConverter::convertToFile(): cannot determine the format of image.tga for a multi-level 1D image\n");
}

void AnyImageConverterTest::unknownLevels2D() {
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile({Image2D}, "image.png"));
    CORRADE_COMPARE(out, "Trade::AnyImageConverter::convertToFile(): cannot determine the format of image.png for a multi-level 2D image\n");
}

void AnyImageConverterTest::unknownLevels3D() {
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile({Image3D}, "image.jpg"));
    CORRADE_COMPARE(out, "Trade::AnyImageConverter::convertToFile(): cannot determine the format of image.jpg for a multi-level 3D image\n");
}

void AnyImageConverterTest::unknownCompressedLevels1D() {
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile({CompressedImage1D}, "image.bmp"));
    CORRADE_COMPARE(out, "Trade::AnyImageConverter::convertToFile(): cannot determine the format of image.bmp for a multi-level compressed 1D image\n");
}

void AnyImageConverterTest::unknownCompressedLevels2D() {
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile({CompressedImage2D}, "image.exr"));
    CORRADE_COMPARE(out, "Trade::AnyImageConverter::convertToFile(): cannot determine the format of image.exr for a multi-level compressed 2D image\n");
}

void AnyImageConverterTest::unknownCompressedLevels3D() {
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToFile({CompressedImage3D}, "image.exr"));
    CORRADE_COMPARE(out, "Trade::AnyImageConverter::convertToFile(): cannot determine the format of image.exr for a multi-level compressed 3D image\n");
}

void AnyImageConverterTest::propagateFlags1D() {
    CORRADE_SKIP("No plugins with flag-dependent behavior for 1D data yet.");
}

void AnyImageConverterTest::propagateFlags2D() {
    if(!(_manager.loadState("TgaImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImageConverter plugin not enabled, cannot test");

    Containers::String filename = Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "output.tga");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    /* Just test that the exported file exists */
    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");
    converter->setFlags(ImageConverterFlag::Verbose);
    Containers::String out;
    {
        Debug redirectOutput{&out};
        CORRADE_VERIFY(converter->convertToFile(Image2D, filename));
    }
    CORRADE_VERIFY(Utility::Path::exists(filename));
    CORRADE_COMPARE(out,
        "Trade::AnyImageConverter::convertToFile(): using TgaImageConverter\n"
        "Trade::TgaImageConverter::convertToData(): converting from RGB to BGR\n"
        "Trade::TgaImageConverter::convertToData(): RLE output 3 bytes larger than uncompressed, falling back to uncompressed\n");
}

void AnyImageConverterTest::propagateFlags3D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("OpenExrImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("OpenExrImageConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "cube.exr");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    /* This will make the verbose output print the detected hardware thread
       count, but also the info about updating global thread count for the
       first time. Thus run it once w/o a verbose flag and then again with to
       filter out the other message. */
    /** @todo switch to testing something else when there is */
    converter->configuration().setValue("threads", 0);
    CORRADE_VERIFY(converter->convertToFile(ImageCube, filename));

    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    converter->setFlags(ImageConverterFlag::Verbose);
    Containers::String out;
    {
        Debug redirectOutput{&out};
        CORRADE_VERIFY(converter->convertToFile(ImageCube, filename));
    }
    CORRADE_VERIFY(Utility::Path::exists(filename));
    CORRADE_COMPARE(out, Utility::format(
        "Trade::AnyImageConverter::convertToFile(): using OpenExrImageConverter\n"
        "Trade::OpenExrImageConverter::convertToData(): autodetected hardware concurrency to {} threads\n",
        std::thread::hardware_concurrency()));
}

void AnyImageConverterTest::propagateFlagsCompressed1D() {
    CORRADE_SKIP("No plugins with flag-dependent behavior for compressed 1D data yet.");
}

void AnyImageConverterTest::propagateFlagsCompressed2D() {
    CORRADE_SKIP("No plugins with flag-dependent behavior for compressed 2D data yet.");
}

void AnyImageConverterTest::propagateFlagsCompressed3D() {
    CORRADE_SKIP("No plugins with flag-dependent behavior for compressed 3D data yet.");
}

void AnyImageConverterTest::propagateFlagsLevels1D() {
    CORRADE_SKIP("No plugins with flag-dependent behavior for multi-level 1D data yet.");
}

void AnyImageConverterTest::propagateFlagsLevels2D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("OpenExrImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("OpenExrImageConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "output.exr");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    /* This will make the verbose output print the detected hardware thread
       count, but also the info about updating global thread count for the
       first time. Thus run it once w/o a verbose flag and then again with to
       filter out the other message. */
    /** @todo switch to testing something else when there is */
    converter->configuration().setValue("threads", 0);
    CORRADE_VERIFY(converter->convertToFile({Image2DFloat}, filename));

    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    converter->setFlags(ImageConverterFlag::Verbose);
    Containers::String out;
    {
        Debug redirectOutput{&out};
        /* Using the list API even though there's just one image, which should
           still trigger the correct code path for AnyImageConverter. */
        CORRADE_VERIFY(converter->convertToFile({Image2DFloat}, filename));
    }
    CORRADE_VERIFY(Utility::Path::exists(filename));
    CORRADE_COMPARE(out, Utility::format(
        "Trade::AnyImageConverter::convertToFile(): using OpenExrImageConverter\n"
        "Trade::OpenExrImageConverter::convertToData(): autodetected hardware concurrency to {} threads\n",
        std::thread::hardware_concurrency()));
}

void AnyImageConverterTest::propagateFlagsLevels3D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("OpenExrImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("OpenExrImageConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "cube.exr");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    /* This will make the verbose output print the detected hardware thread
       count, but also the info about updating global thread count for the
       first time. Thus run it once w/o a verbose flag and then again with to
       filter out the other message. */
    /** @todo switch to testing something else when there is */
    converter->configuration().setValue("threads", 0);
    CORRADE_VERIFY(converter->convertToFile({ImageCube}, filename));

    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    converter->setFlags(ImageConverterFlag::Verbose);
    Containers::String out;
    {
        Debug redirectOutput{&out};
        /* Using the list API even though there's just one image, which should
           still trigger the correct code path for AnyImageConverter. */
        CORRADE_VERIFY(converter->convertToFile({ImageCube}, filename));
    }
    CORRADE_VERIFY(Utility::Path::exists(filename));
    CORRADE_COMPARE(out, Utility::format(
        "Trade::AnyImageConverter::convertToFile(): using OpenExrImageConverter\n"
        "Trade::OpenExrImageConverter::convertToData(): autodetected hardware concurrency to {} threads\n",
        std::thread::hardware_concurrency()));
}

void AnyImageConverterTest::propagateFlagsCompressedLevels1D() {
    CORRADE_SKIP("No plugins with flag-dependent behavior for multi-level compressed 1D data yet.");
}

void AnyImageConverterTest::propagateFlagsCompressedLevels2D() {
    CORRADE_SKIP("No plugins with flag-dependent behavior for multi-level compressed 2D data yet.");
}

void AnyImageConverterTest::propagateFlagsCompressedLevels3D() {
    CORRADE_SKIP("No plugins with flag-dependent behavior for multi-level compressed 3D data yet.");
}

void AnyImageConverterTest::propagateConfiguration1D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "custom-writer-1d.ktx2");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("generator", "Yello this did Magnum!");
    CORRADE_VERIFY(converter->convertToFile(Image1D, filename));

    Containers::Optional<Containers::String> output = Utility::Path::readString(filename);
    CORRADE_VERIFY(output);
    CORRADE_COMPARE_AS(*output,
        "KTXwriter\0Yello this did Magnum!"_s,
        TestSuite::Compare::StringContains);
}

void AnyImageConverterTest::propagateConfiguration2D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("OpenExrImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("OpenExrImageConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "depth32f-custom-channels.exr");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("layer", "left");
    converter->configuration().setValue("depth", "height");
    CORRADE_VERIFY(converter->convertToFile(Image2DFloat, filename));
    /* Compare to an expected output to ensure the custom channels names were
       used. This also doubles as a generator for the
       depth32f-custom-channels.exr file that AnyImageImporterTest uses. */
    CORRADE_COMPARE_AS(filename, Utility::Path::join(ANYIMAGEIMPORTER_TEST_DIR, "depth32f-custom-channels.exr"), TestSuite::Compare::File);
}

void AnyImageConverterTest::propagateConfiguration3D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "custom-writer-3d.ktx2");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("generator", "Yello this did Magnum!");
    CORRADE_VERIFY(converter->convertToFile(Image3D, filename));

    Containers::Optional<Containers::String> output = Utility::Path::readString(filename);
    CORRADE_VERIFY(output);
    CORRADE_COMPARE_AS(*output,
        "KTXwriter\0Yello this did Magnum!"_s,
        TestSuite::Compare::StringContains);
}

void AnyImageConverterTest::propagateConfigurationUnknown1D() {
    auto&& data = PropagateConfigurationUnknownData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("noSuchOption", "isHere");
    converter->setFlags(data.flags);

    Containers::String out;
    Warning redirectWarning{&out};
    CORRADE_VERIFY(converter->convertToFile(Image1D, Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "1d.ktx2")));
    if(data.quiet)
        CORRADE_COMPARE(out, "");
    else
        CORRADE_COMPARE(out, "Trade::AnyImageConverter::convertToFile(): option noSuchOption not recognized by KtxImageConverter\n");
}

void AnyImageConverterTest::propagateConfigurationUnknown2D() {
    auto&& data = PropagateConfigurationUnknownData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!(_manager.loadState("TgaImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImageConverter plugin not enabled, cannot test");

    Containers::Pointer<AbstractImageConverter> converter = _manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("noSuchOption", "isHere");
    converter->setFlags(data.flags);

    Containers::String out;
    Warning redirectWarning{&out};
    CORRADE_VERIFY(converter->convertToFile(Image2D, Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "2d.tga")));
    if(data.quiet)
        CORRADE_COMPARE(out, "");
    else
        CORRADE_COMPARE(out, "Trade::AnyImageConverter::convertToFile(): option noSuchOption not recognized by TgaImageConverter\n");
}

void AnyImageConverterTest::propagateConfigurationUnknown3D() {
    auto&& data = PropagateConfigurationUnknownData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("noSuchOption", "isHere");
    converter->setFlags(data.flags);

    Containers::String out;
    Warning redirectWarning{&out};
    CORRADE_VERIFY(converter->convertToFile(Image3D, Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "3d.ktx2")));
    if(data.quiet)
        CORRADE_COMPARE(out, "");
    else
        CORRADE_COMPARE(out, "Trade::AnyImageConverter::convertToFile(): option noSuchOption not recognized by KtxImageConverter\n");
}

void AnyImageConverterTest::propagateConfigurationCompressed1D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "compressed-custom-writer-1d.ktx2");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("generator", "Yello this did Magnum!");
    CORRADE_VERIFY(converter->convertToFile(CompressedImage1D, filename));

    Containers::Optional<Containers::String> output = Utility::Path::readString(filename);
    CORRADE_VERIFY(output);
    CORRADE_COMPARE_AS(*output,
        "KTXwriter\0Yello this did Magnum!"_s,
        TestSuite::Compare::StringContains);
}

void AnyImageConverterTest::propagateConfigurationCompressed2D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "compressed-custom-writer-2d.ktx2");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("generator", "Yello this did Magnum!");
    CORRADE_VERIFY(converter->convertToFile(CompressedImage2D, filename));

    Containers::Optional<Containers::String> output = Utility::Path::readString(filename);
    CORRADE_VERIFY(output);
    CORRADE_COMPARE_AS(*output,
        "KTXwriter\0Yello this did Magnum!"_s,
        TestSuite::Compare::StringContains);
}

void AnyImageConverterTest::propagateConfigurationCompressed3D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "compressed-custom-writer-3d.ktx2");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("generator", "Yello this did Magnum!");
    CORRADE_VERIFY(converter->convertToFile(CompressedImage3D, filename));

    Containers::Optional<Containers::String> output = Utility::Path::readString(filename);
    CORRADE_VERIFY(output);
    CORRADE_COMPARE_AS(*output,
        "KTXwriter\0Yello this did Magnum!"_s,
        TestSuite::Compare::StringContains);
}

void AnyImageConverterTest::propagateConfigurationCompressedUnknown1D() {
    auto&& data = PropagateConfigurationUnknownData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("noSuchOption", "isHere");
    converter->setFlags(data.flags);

    Containers::String out;
    Warning redirectWarning{&out};
    CORRADE_VERIFY(converter->convertToFile(CompressedImage1D, Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "compressed-1d.ktx2")));
    if(data.quiet)
        CORRADE_COMPARE(out, "");
    else
        CORRADE_COMPARE(out, "Trade::AnyImageConverter::convertToFile(): option noSuchOption not recognized by KtxImageConverter\n");
}

void AnyImageConverterTest::propagateConfigurationCompressedUnknown2D() {
    auto&& data = PropagateConfigurationUnknownData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("noSuchOption", "isHere");
    converter->setFlags(data.flags);

    Containers::String out;
    Warning redirectWarning{&out};
    CORRADE_VERIFY(converter->convertToFile(CompressedImage2D, Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "compressed-2d.ktx2")));
    if(data.quiet)
        CORRADE_COMPARE(out, "");
    else
        CORRADE_COMPARE(out, "Trade::AnyImageConverter::convertToFile(): option noSuchOption not recognized by KtxImageConverter\n");
}

void AnyImageConverterTest::propagateConfigurationCompressedUnknown3D() {
    auto&& data = PropagateConfigurationUnknownData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("noSuchOption", "isHere");
    converter->setFlags(data.flags);

    Containers::String out;
    Warning redirectWarning{&out};
    CORRADE_VERIFY(converter->convertToFile(CompressedImage3D, Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "compressed-3d.ktx2")));
    if(data.quiet)
        CORRADE_COMPARE(out, "");
    else
        CORRADE_COMPARE(out, "Trade::AnyImageConverter::convertToFile(): option noSuchOption not recognized by KtxImageConverter\n");
}

void AnyImageConverterTest::propagateConfigurationLevels1D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "custom-writer-1d.ktx2");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("generator", "Yello this did Magnum!");
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(converter->convertToFile({Image1D}, filename));

    Containers::Optional<Containers::String> output = Utility::Path::readString(filename);
    CORRADE_VERIFY(output);
    CORRADE_COMPARE_AS(*output,
        "KTXwriter\0Yello this did Magnum!"_s,
        TestSuite::Compare::StringContains);
}

void AnyImageConverterTest::propagateConfigurationLevels2D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "custom-writer-2d.ktx2");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("generator", "Yello this did Magnum!");
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(converter->convertToFile({Image2D}, filename));

    Containers::Optional<Containers::String> output = Utility::Path::readString(filename);
    CORRADE_VERIFY(output);
    CORRADE_COMPARE_AS(*output,
        "KTXwriter\0Yello this did Magnum!"_s,
        TestSuite::Compare::StringContains);
}

void AnyImageConverterTest::propagateConfigurationLevels3D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "custom-writer-3d.ktx2");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("generator", "Yello this did Magnum!");
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(converter->convertToFile({Image3D}, filename));

    Containers::Optional<Containers::String> output = Utility::Path::readString(filename);
    CORRADE_VERIFY(output);
    CORRADE_COMPARE_AS(*output,
        "KTXwriter\0Yello this did Magnum!"_s,
        TestSuite::Compare::StringContains);
}

void AnyImageConverterTest::propagateConfigurationUnknownLevels1D() {
    auto&& data = PropagateConfigurationUnknownData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("noSuchOption", "isHere");
    converter->setFlags(data.flags);

    Containers::String out;
    Warning redirectWarning{&out};
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(converter->convertToFile({Image1D}, Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "1d.ktx2")));
    if(data.quiet)
        CORRADE_COMPARE(out, "");
    else
        CORRADE_COMPARE(out, "Trade::AnyImageConverter::convertToFile(): option noSuchOption not recognized by KtxImageConverter\n");
}

void AnyImageConverterTest::propagateConfigurationUnknownLevels2D() {
    auto&& data = PropagateConfigurationUnknownData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("noSuchOption", "isHere");
    converter->setFlags(data.flags);

    Containers::String out;
    Warning redirectWarning{&out};
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(converter->convertToFile({Image2D}, Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "2d.ktx2")));
    if(data.quiet)
        CORRADE_COMPARE(out, "");
    else
        CORRADE_COMPARE(out, "Trade::AnyImageConverter::convertToFile(): option noSuchOption not recognized by KtxImageConverter\n");
}

void AnyImageConverterTest::propagateConfigurationUnknownLevels3D() {
    auto&& data = PropagateConfigurationUnknownData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("noSuchOption", "isHere");
    converter->setFlags(data.flags);

    Containers::String out;
    Warning redirectWarning{&out};
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(converter->convertToFile({Image3D}, Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "3d.ktx2")));
    if(data.quiet)
        CORRADE_COMPARE(out, "");
    else
        CORRADE_COMPARE(out, "Trade::AnyImageConverter::convertToFile(): option noSuchOption not recognized by KtxImageConverter\n");
}

void AnyImageConverterTest::propagateConfigurationCompressedLevels1D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "compressed-custom-writer-1d.ktx2");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("generator", "Yello this did Magnum!");
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(converter->convertToFile({CompressedImage1D}, filename));

    Containers::Optional<Containers::String> output = Utility::Path::readString(filename);
    CORRADE_VERIFY(output);
    CORRADE_COMPARE_AS(*output,
        "KTXwriter\0Yello this did Magnum!"_s,
        TestSuite::Compare::StringContains);
}

void AnyImageConverterTest::propagateConfigurationCompressedLevels2D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "compressed-custom-writer-2d.ktx2");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("generator", "Yello this did Magnum!");
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(converter->convertToFile({CompressedImage2D}, filename));

    Containers::Optional<Containers::String> output = Utility::Path::readString(filename);
    CORRADE_VERIFY(output);
    CORRADE_COMPARE_AS(*output,
        "KTXwriter\0Yello this did Magnum!"_s,
        TestSuite::Compare::StringContains);
}

void AnyImageConverterTest::propagateConfigurationCompressedLevels3D() {
    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::String filename = Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "compresed-custom-writer-3d.ktx2");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("generator", "Yello this did Magnum!");
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(converter->convertToFile({CompressedImage3D}, filename));

    Containers::Optional<Containers::String> output = Utility::Path::readString(filename);
    CORRADE_VERIFY(output);
    CORRADE_COMPARE_AS(*output,
        "KTXwriter\0Yello this did Magnum!"_s,
        TestSuite::Compare::StringContains);
}

void AnyImageConverterTest::propagateConfigurationCompressedUnknownLevels1D() {
    auto&& data = PropagateConfigurationUnknownData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("noSuchOption", "isHere");
    converter->setFlags(data.flags);

    Containers::String out;
    Warning redirectWarning{&out};
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(converter->convertToFile({CompressedImage1D}, Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "compressed-1d.ktx2")));
    if(data.quiet)
        CORRADE_COMPARE(out, "");
    else
        CORRADE_COMPARE(out, "Trade::AnyImageConverter::convertToFile(): option noSuchOption not recognized by KtxImageConverter\n");
}

void AnyImageConverterTest::propagateConfigurationCompressedUnknownLevels2D() {
    auto&& data = PropagateConfigurationUnknownData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("noSuchOption", "isHere");
    converter->setFlags(data.flags);

    Containers::String out;
    Warning redirectWarning{&out};
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(converter->convertToFile({CompressedImage2D}, Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "compressed-2d.ktx2")));
    if(data.quiet)
        CORRADE_COMPARE(out, "");
    else
        CORRADE_COMPARE(out, "Trade::AnyImageConverter::convertToFile(): option noSuchOption not recognized by KtxImageConverter\n");
}

void AnyImageConverterTest::propagateConfigurationCompressedUnknownLevels3D() {
    auto&& data = PropagateConfigurationUnknownData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    PluginManager::Manager<AbstractImageConverter> manager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImageConverter plugin can't be loaded.");

    Containers::Pointer<AbstractImageConverter> converter = manager.instantiate("AnyImageConverter");
    converter->configuration().setValue("noSuchOption", "isHere");
    converter->setFlags(data.flags);

    Containers::String out;
    Warning redirectWarning{&out};
    /* Using the list API even though there's just one image, which should
       still trigger the correct code path for AnyImageConverter. */
    CORRADE_VERIFY(converter->convertToFile({CompressedImage3D}, Utility::Path::join(ANYIMAGECONVERTER_TEST_OUTPUT_DIR, "compressed-3d.ktx2")));
    if(data.quiet)
        CORRADE_COMPARE(out, "");
    else
        CORRADE_COMPARE(out, "Trade::AnyImageConverter::convertToFile(): option noSuchOption not recognized by KtxImageConverter\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AnyImageConverterTest)
