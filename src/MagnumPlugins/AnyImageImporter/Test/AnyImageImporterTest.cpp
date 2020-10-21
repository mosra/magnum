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
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ImageData.h"

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct AnyImageImporterTest: TestSuite::Tester {
    explicit AnyImageImporterTest();

    void load();
    void detect();

    void unknownExtension();
    void unknownSignature();
    void emptyData();

    void verbose();

    /* Explicitly forbid system-wide plugin dependencies */
    PluginManager::Manager<AbstractImporter> _manager{"nonexistent"};
};

Containers::Optional<Containers::ArrayView<const char>> fileCallback(const std::string& filename, InputFileCallbackPolicy, Containers::Array<char>& storage) {
    storage = Utility::Directory::read(filename);
    return Containers::ArrayView<const char>{storage};
}

constexpr struct {
    const char* name;
    const char* filename;
    Containers::Optional<Containers::ArrayView<const char>>(*callback)(const std::string&, InputFileCallbackPolicy, Containers::Array<char>&);
    const char* verboseFunctionName;
} LoadData[]{
    {"TGA", TGA_FILE, nullptr, "openFile"},
    {"TGA data", TGA_FILE, fileCallback, "openData"}
};

constexpr struct {
    const char* name;
    const char* filename;
    Containers::Optional<Containers::ArrayView<const char>>(*callback)(const std::string&, InputFileCallbackPolicy, Containers::Array<char>&);
    const char* plugin;
} DetectData[]{
    {"PNG", "rgb.png", nullptr, "PngImporter"},
    {"PNG data", "rgb.png", fileCallback, "PngImporter"},
    {"JPEG", "gray.jpg", nullptr, "JpegImporter"},
    {"JPEG data", "gray.jpg", fileCallback, "JpegImporter"},
    {"JPEG uppercase", "uppercase.JPG", nullptr, "JpegImporter"},
    {"JPEG2000", "image.jp2", nullptr, "Jpeg2000Importer"},
    {"EXR", "image.exr", nullptr, "OpenExrImporter"},
    {"EXR data", "image.exr", fileCallback, "OpenExrImporter"},
    {"HDR", "rgb.hdr", nullptr, "HdrImporter"},
    {"HDR data", "rgb.hdr", fileCallback, "HdrImporter"},
    {"ICO", "pngs.ico", nullptr, "IcoImporter"},
    {"DDS", "rgba_dxt1.dds", nullptr, "DdsImporter"},
    {"DDS data", "rgba_dxt1.dds", fileCallback, "DdsImporter"},
    {"BMP", "image.bmp", nullptr, "BmpImporter"},
    {"GIF", "image.gif", nullptr, "GifImporter"},
    {"PSD", "image.psd", nullptr, "PsdImporter"},
    {"TIFF", "image.tiff", nullptr, "TiffImporter"},
    {"TIFF data", "image.tiff", fileCallback, "TiffImporter"},
    {"Basis", "rgb.basis", nullptr, "BasisImporter"}
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
    {"just one byte", "\x33"_s, "33"},
    {"just one zero byte", "\x00"_s, "00"},
    {"DDS, but no space", "DDS!"_s, "44445321"},
    {"TIFF, but too short", "II\x2a"_s, "49492a"},
    {"TIFF, but no zero byte", "MM\xff\x2a"_s, "4d4dff2a"}
};

AnyImageImporterTest::AnyImageImporterTest() {
    addInstancedTests({&AnyImageImporterTest::load},
        Containers::arraySize(LoadData));

    addInstancedTests({&AnyImageImporterTest::detect},
        Containers::arraySize(DetectData));

    addTests({&AnyImageImporterTest::unknownExtension});

    addInstancedTests({&AnyImageImporterTest::unknownSignature},
        Containers::arraySize(DetectUnknownData));

    addTests({&AnyImageImporterTest::emptyData});

    addInstancedTests({&AnyImageImporterTest::verbose},
        Containers::arraySize(LoadData));

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

void AnyImageImporterTest::load() {
    auto&& data = LoadData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImporter plugin not enabled, cannot test");

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnyImageImporter");

    Containers::Array<char> storage;
    importer->setFileCallback(data.callback, storage);

    CORRADE_VERIFY(importer->openFile(data.filename));

    /* Check only size, as it is good enough proof that it is working */
    Containers::Optional<ImageData2D> image = importer->image2D(0);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->size(), Vector2i(3, 2));

    importer->close();
    CORRADE_VERIFY(!importer->isOpened());
}

void AnyImageImporterTest::detect() {
    auto&& data = DetectData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnyImageImporter");

    Containers::Array<char> storage;
    importer->setFileCallback(data.callback, storage);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->openFile(Utility::Directory::join(TEST_FILE_DIR, data.filename)));
    /* Can't use raw string literals in macros on GCC 4.8 */
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out.str(), Utility::formatString(
"PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\nTrade::AnyImageImporter::{1}(): cannot load the {0} plugin\n", data.plugin, data.callback ? "openData" : "openFile"));
    #else
    CORRADE_COMPARE(out.str(), Utility::formatString(
"PluginManager::Manager::load(): plugin {0} was not found\nTrade::AnyImageImporter::{1}(): cannot load the {0} plugin\n", data.plugin, data.callback ? "openData" : "openFile"));
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

void AnyImageImporterTest::verbose() {
    auto&& data = LoadData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImporter plugin not enabled, cannot test");

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnyImageImporter");
    importer->setFlags(ImporterFlag::Verbose);

    Containers::Array<char> storage;
    importer->setFileCallback(data.callback, storage);

    std::ostringstream out;
    {
        Debug redirectOutput{&out};
        CORRADE_VERIFY(importer->openFile(data.filename));
        CORRADE_VERIFY(importer->image2D(0));
    }
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "Trade::AnyImageImporter::{}(): using TgaImporter\n"
        "Trade::TgaImporter::image2D(): converting from BGR to RGB\n",
        data.verboseFunctionName));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AnyImageImporterTest)
