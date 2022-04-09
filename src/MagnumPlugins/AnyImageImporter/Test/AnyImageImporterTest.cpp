/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2021 Pablo Escobar <mail@rvrs.in>

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
#include <Corrade/Utility/ConfigurationGroup.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/FormatStl.h>
#include <Corrade/Utility/Path.h>

#include "Magnum/ImageView.h"
#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ImageData.h"

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct AnyImageImporterTest: TestSuite::Tester {
    explicit AnyImageImporterTest();

    void load1D();
    void load2D();
    void load3D();
    void detect();

    void unknownExtension();
    void unknownSignature();
    void emptyData();

    void propagateFlags();
    void propagateConfiguration();
    void propagateConfigurationUnknown();
    /* configuration propagation fully tested in AnySceneImporter, as there the
       plugins have configuration subgroups as well */
    void propagateFileCallback();

    /* Explicitly forbid system-wide plugin dependencies */
    PluginManager::Manager<AbstractImporter> _manager{"nonexistent"};
};

constexpr struct {
    const char* name;
    const char* filename;
    bool asData;
    const char* messageFunctionName;
} Load1DData[]{
    {"KTX2", KTX_1D_FILE, false, "KtxImporter"},
    {"KTX2 data", KTX_1D_FILE, true, "KtxImporter"},
};

constexpr struct {
    const char* name;
    const char* filename;
    bool asData;
    const char* messageFunctionName;
} Load2DData[]{
    {"TGA", TGA_FILE, false, "openFile"},
    {"TGA data", TGA_FILE, true, "openData"}
};

constexpr struct {
    const char* name;
    const char* filename;
    bool asData;
    const char* messageFunctionName;
} Load3DData[]{
    {"KTX2", KTX_3D_FILE, false, "KtxImporter"},
    {"KTX2 data", KTX_3D_FILE, true, "KtxImporter"},
};

constexpr struct {
    const char* name;
    const char* filename;
    bool asData;
    const char* plugin;
} DetectData[]{
    {"PNG", "rgb.png", false, "PngImporter"},
    {"PNG data", "rgb.png", true, "PngImporter"},
    {"JPEG", "gray.jpg", false, "JpegImporter"},
    {"JPEG data", "gray.jpg", true, "JpegImporter"},
    {"JPEG uppercase", "uppercase.JPG", false, "JpegImporter"},
    {"JPEG2000", "image.jp2", false, "Jpeg2000Importer"},
    {"KTX2", "image.ktx2", false, "KtxImporter"},
    /** @todo KTX2 data once we have some */
    {"HDR", "rgb.hdr", false, "HdrImporter"},
    {"HDR data", "rgb.hdr", true, "HdrImporter"},
    {"ICO", "pngs.ico", false, "IcoImporter"},
    {"DDS", "rgba_dxt1.dds", false, "DdsImporter"},
    {"DDS data", "rgba_dxt1.dds", true, "DdsImporter"},
    {"BMP", "rgb.bmp", false, "BmpImporter"},
    {"BMP data", "rgb.bmp", true, "BmpImporter"},
    {"GIF", "image.gif", false, "GifImporter"},
    {"PSD", "image.psd", false, "PsdImporter"},
    {"TIFF", "image.tiff", false, "TiffImporter"},
    {"TIFF data", "image.tiff", true, "TiffImporter"},
    {"Basis", "rgb.basis", false, "BasisImporter"},
    {"Basis data", "rgb.basis", true, "BasisImporter"},
    {"OpenVDB", "volume.vdb", false, "OpenVdbImporter"}
    /* Not testing everything, just the most important ones */
};

using namespace Containers::Literals;

const struct {
    const char* name;
    Containers::StringView data;
    const char* signature;
} DetectUnknownData[]{
    {"something random", "\x25\x3a\x00\x56 blablabla"_s, "253a0056"},
    /* There was a bug where the error message shifted a signed value,
       poisoning the output. It also was throwing away leading zero bytes. */
    {"leading zeros, negative char", "\x00\xff\x00\xff"_s, "00ff00ff"},
    {"lots zero bytes", "\x00\x00\x00\x00"_s, "00000000"},
    {"just one byte", "\x33"_s, "33"},
    {"just one zero byte", "\x00"_s, "00"},
    {"DDS, but no space", "DDS!"_s, "44445321"},
    {"TIFF, but too short", "II\x2a"_s, "49492a"},
    {"TIFF, but no zero byte", "MM\xff\x2a"_s, "4d4dff2a"},
    {"KTX, but wrong version", "\xabKTX 30\xbb\r\n\x1a\n"_s, "ab4b5458"}
};

constexpr struct {
    const char* name;
    const char* filename;
    bool asData;
} PropagateConfigurationData[]{
    {"EXR", EXR_FILE, false},
    {"EXR data", EXR_FILE, true}
};

AnyImageImporterTest::AnyImageImporterTest() {
    addInstancedTests({&AnyImageImporterTest::load1D},
        Containers::arraySize(Load1DData));

    addInstancedTests({&AnyImageImporterTest::load2D},
        Containers::arraySize(Load2DData));

    addInstancedTests({&AnyImageImporterTest::load3D},
        Containers::arraySize(Load3DData));

    addInstancedTests({&AnyImageImporterTest::detect},
        Containers::arraySize(DetectData));

    addTests({&AnyImageImporterTest::unknownExtension});

    addInstancedTests({&AnyImageImporterTest::unknownSignature},
        Containers::arraySize(DetectUnknownData));

    addTests({&AnyImageImporterTest::emptyData});

    addInstancedTests({&AnyImageImporterTest::propagateFlags},
        Containers::arraySize(Load2DData));

    addInstancedTests({&AnyImageImporterTest::propagateConfiguration},
        Containers::arraySize(PropagateConfigurationData));

    addInstancedTests({&AnyImageImporterTest::propagateConfigurationUnknown},
        Containers::arraySize(Load2DData));

    addTests({&AnyImageImporterTest::propagateFileCallback});

    /* Load the plugin directly from the build tree. Otherwise it's static and
       already loaded. */
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    /* Optional plugins that don't have to be here */
    #ifdef TGAIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
}

void AnyImageImporterTest::load1D() {
    auto&& data = Load1DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImporter plugin can't be loaded.");

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnyImageImporter");

    if(data.asData) {
        Containers::Optional<Containers::Array<char>> read = Utility::Path::read(data.filename);
        CORRADE_VERIFY(read);
        CORRADE_VERIFY(importer->openData(*read));
    } else CORRADE_VERIFY(importer->openFile(data.filename));
    CORRADE_COMPARE(importer->image1DCount(), 1);

    /* Check only size, as it is good enough proof that it is working */
    Containers::Optional<ImageData1D> image = importer->image1D(0);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->size(), 2);
}

void AnyImageImporterTest::load2D() {
    auto&& data = Load2DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImporter plugin not enabled, cannot test");

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnyImageImporter");

    if(data.asData) {
        Containers::Optional<Containers::Array<char>> read = Utility::Path::read(data.filename);
        CORRADE_VERIFY(read);
        CORRADE_VERIFY(importer->openData(*read));
    } else CORRADE_VERIFY(importer->openFile(data.filename));
    CORRADE_COMPARE(importer->image2DCount(), 1);

    /* Check only size, as it is good enough proof that it is working */
    Containers::Optional<ImageData2D> image = importer->image2D(0);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->size(), Vector2i(3, 2));

    importer->close();
    CORRADE_VERIFY(!importer->isOpened());
}

void AnyImageImporterTest::load3D() {
    auto&& data = Load3DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImporter plugin can't be loaded.");

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnyImageImporter");

    if(data.asData) {
        Containers::Optional<Containers::Array<char>> read = Utility::Path::read(data.filename);
        CORRADE_VERIFY(read);
        CORRADE_VERIFY(importer->openData(*read));
    } else CORRADE_VERIFY(importer->openFile(data.filename));
    CORRADE_COMPARE(importer->image3DCount(), 1);

    /* Check only size, as it is good enough proof that it is working */
    Containers::Optional<ImageData3D> image = importer->image3D(0);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->size(), (Vector3i{2, 3, 2}));
}

void AnyImageImporterTest::detect() {
    auto&& data = DetectData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnyImageImporter");
    Containers::String filename = Utility::Path::join(TEST_FILE_DIR, data.filename);

    std::ostringstream out;
    Error redirectError{&out};
    if(data.asData) {
        Containers::Optional<Containers::Array<char>> read = Utility::Path::read(filename);
        CORRADE_VERIFY(read);
        CORRADE_VERIFY(!importer->openData(*read));
    } else CORRADE_VERIFY(!importer->openFile(filename));
    /* Can't use raw string literals in macros on GCC 4.8 */
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out.str(), Utility::formatString(
"PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\nTrade::AnyImageImporter::{1}(): cannot load the {0} plugin\n", data.plugin, data.asData ? "openData" : "openFile"));
    #else
    CORRADE_COMPARE(out.str(), Utility::formatString(
"PluginManager::Manager::load(): plugin {0} was not found\nTrade::AnyImageImporter::{1}(): cannot load the {0} plugin\n", data.plugin, data.asData ? "openData" : "openFile"));
    #endif
}

void AnyImageImporterTest::unknownExtension() {
    std::ostringstream output;
    Error redirectError{&output};

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnyImageImporter");
    CORRADE_VERIFY(!importer->openFile("image.xcf"));

    CORRADE_COMPARE(output.str(), "Trade::AnyImageImporter::openFile(): cannot determine the format of image.xcf\n");
}

void AnyImageImporterTest::unknownSignature() {
    auto&& data = DetectUnknownData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    std::ostringstream output;
    Error redirectError{&output};

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnyImageImporter");
    CORRADE_VERIFY(!importer->openData(data.data));

    CORRADE_COMPARE(output.str(), Utility::formatString("Trade::AnyImageImporter::openData(): cannot determine the format from signature 0x{}\n", data.signature));
}

void AnyImageImporterTest::emptyData() {
    std::ostringstream output;
    Error redirectError{&output};

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnyImageImporter");
    CORRADE_VERIFY(!importer->openData(nullptr));

    CORRADE_COMPARE(output.str(), "Trade::AnyImageImporter::openData(): file is empty\n");
}

void AnyImageImporterTest::propagateFlags() {
    auto&& data = Load2DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImporter plugin not enabled, cannot test");

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnyImageImporter");
    importer->setFlags(ImporterFlag::Verbose);

    std::ostringstream out;
    {
        Debug redirectOutput{&out};
        if(data.asData) {
            Containers::Optional<Containers::Array<char>> read = Utility::Path::read(data.filename);
            CORRADE_VERIFY(read);
            CORRADE_VERIFY(importer->openData(*read));
        } else CORRADE_VERIFY(importer->openFile(data.filename));
        CORRADE_VERIFY(importer->image2D(0));
    }
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "Trade::AnyImageImporter::{}(): using TgaImporter\n"
        "Trade::TgaImporter::image2D(): converting from BGR to RGB\n",
        data.messageFunctionName));
}

void AnyImageImporterTest::propagateConfiguration() {
    auto&& data = PropagateConfigurationData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("OpenExrImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("OpenExrImporter plugin can't be loaded.");

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnyImageImporter");
    importer->configuration().setValue("layer", "left");
    importer->configuration().setValue("depth", "height");

    if(data.asData) {
        Containers::Optional<Containers::Array<char>> read = Utility::Path::read(data.filename);
        CORRADE_VERIFY(read);
        CORRADE_VERIFY(importer->openData(*read));
    } else CORRADE_VERIFY(importer->openFile(data.filename));
    Containers::Optional<Trade::ImageData2D> image = importer->image2D(0);
    CORRADE_VERIFY(image);

    /* Comparing image contents to verify the custom channels were used */
    const Float Depth32fData[] = {
        0.125f, 0.250f, 0.375f,
        0.500f, 0.625f, 0.750f
    };
    const ImageView2D Depth32f{PixelFormat::Depth32F, {3, 2}, Depth32fData};
    CORRADE_COMPARE_AS(*image, Depth32f,
        DebugTools::CompareImage);
}

void AnyImageImporterTest::propagateConfigurationUnknown() {
    auto&& data = Load2DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImporter plugin not enabled, cannot test");

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnyImageImporter");
    importer->configuration().setValue("noSuchOption", "isHere");

    std::ostringstream out;
    Warning redirectWarning{&out};
    if(data.asData) {
        Containers::Optional<Containers::Array<char>> read = Utility::Path::read(data.filename);
        CORRADE_VERIFY(read);
        CORRADE_VERIFY(importer->openData(*read));
    } else CORRADE_VERIFY(importer->openFile(data.filename));
    CORRADE_COMPARE(out.str(), Utility::formatString("Trade::AnyImageImporter::{}(): option noSuchOption not recognized by TgaImporter\n", data.messageFunctionName));
}

void AnyImageImporterTest::propagateFileCallback() {
    if(!(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImporter plugin not enabled, cannot test");

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnyImageImporter");

    Containers::Array<char> storage;
    importer->setFileCallback([](const std::string&, InputFileCallbackPolicy, Containers::Array<char>& storage) -> Containers::Optional<Containers::ArrayView<const char>> {
        Containers::Optional<Containers::Array<char>> data = Utility::Path::read(TGA_FILE);
        CORRADE_VERIFY(data);
        storage = *std::move(data);
        return Containers::ArrayView<const char>{storage};
    }, storage);

    CORRADE_VERIFY(true); /* Capture correct function name first */

    CORRADE_VERIFY(importer->openFile("you-know-where-the-file-is.tga"));
    CORRADE_COMPARE(importer->image2DCount(), 1);

    /* Check only size, as it is good enough proof that it is working */
    Containers::Optional<ImageData2D> image = importer->image2D(0);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->size(), Vector2i(3, 2));

    importer->close();
    CORRADE_VERIFY(!importer->isOpened());
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AnyImageImporterTest)
