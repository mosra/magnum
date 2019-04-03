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
#include <Corrade/Containers/Optional.h>
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
} LoadData[]{
    {"TGA", TGA_FILE, nullptr},
    {"TGA data", TGA_FILE, fileCallback}
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
    {"DDS", "rgba_dxt1.dds", nullptr, "DdsImporter"},
    {"DDS data", "rgba_dxt1.dds", fileCallback, "DdsImporter"},
    {"BMP", "image.bmp", nullptr, "BmpImporter"},
    {"GIF", "image.gif", nullptr, "GifImporter"},
    {"PSD", "image.psd", nullptr, "PsdImporter"},
    {"TIFF", "image.tiff", nullptr, "TiffImporter"}
    /* Not testing everything, just the most important ones */
};

AnyImageImporterTest::AnyImageImporterTest() {
    addInstancedTests({&AnyImageImporterTest::load},
        Containers::arraySize(LoadData));

    addInstancedTests({&AnyImageImporterTest::detect},
        Containers::arraySize(DetectData));

    addTests({&AnyImageImporterTest::unknownExtension,
              &AnyImageImporterTest::unknownSignature,
              &AnyImageImporterTest::emptyData});

    /* Load the plugin directly from the build tree. Otherwise it's static and
       already loaded. */
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT(_manager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    /* Optional plugins that don't have to be here */
    #ifdef TGAIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT(_manager.load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
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
    CORRADE_COMPARE(image->size(), Vector2i(2, 3));

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
"PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\nTrade::AnyImageImporter::{1}(): cannot load {0} plugin\n", data.plugin, data.callback ? "openData" : "openFile"));
    #else
    CORRADE_COMPARE(out.str(), Utility::formatString(
"PluginManager::Manager::load(): plugin {0} was not found\nTrade::AnyImageImporter::{1}(): cannot load {0} plugin\n", data.plugin, data.callback ? "openData" : "openFile"));
    #endif
}

void AnyImageImporterTest::unknownExtension() {
    std::ostringstream output;
    Error redirectError{&output};

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnyImageImporter");
    CORRADE_VERIFY(!importer->openFile("image.xcf"));

    CORRADE_COMPARE(output.str(), "Trade::AnyImageImporter::openFile(): cannot determine type of file image.xcf\n");
}

void AnyImageImporterTest::unknownSignature() {
    std::ostringstream output;
    Error redirectError{&output};

    constexpr const char data[]{ 0x25, 0x3a };

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnyImageImporter");
    CORRADE_VERIFY(!importer->openData(data));

    CORRADE_COMPARE(output.str(), "Trade::AnyImageImporter::openData(): cannot determine type from signature 0x253a0000\n");
}

void AnyImageImporterTest::emptyData() {
    std::ostringstream output;
    Error redirectError{&output};

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnyImageImporter");
    CORRADE_VERIFY(!importer->openData(nullptr));

    CORRADE_COMPARE(output.str(), "Trade::AnyImageImporter::openData(): file is empty\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AnyImageImporterTest)
