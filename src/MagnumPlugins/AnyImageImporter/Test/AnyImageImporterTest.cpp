/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
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

#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/ConfigurationGroup.h>
#include <Corrade/Utility/Format.h>
#include <Corrade/Utility/Path.h>

#include "Magnum/ImageView.h"
#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ImageData.h"

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct AnyImageImporterTest: TestSuite::Tester {
    explicit AnyImageImporterTest();

    void load();
    void detect();

    void ktxBasisFallbackFile();
    void ktxBasisFallbackData();

    void unknownExtension();
    void unknownSignature();
    void emptyData();

    void propagateFlags();
    void propagateConfiguration();
    void propagateConfigurationUnknown();
    /* configuration propagation fully tested in AnySceneImporter, as there the
       plugins have configuration subgroups as well */
    void propagateFileCallback();

    void images1D();
    void images2D();
    void images3D();
    void imageLevels1D();
    void imageLevels2D();
    void imageLevels3D();

    /* Explicitly forbid system-wide plugin dependencies */
    PluginManager::Manager<AbstractImporter> _manager{"nonexistent"};
};

constexpr struct {
    const char* name;
    const char* filename;
    bool asData;
    const char* messageFunctionName;
} LoadData[]{
    {"TGA", "rgb.tga", false, "openFile"},
    {"TGA data", "rgb.tga", true, "openData"}
};

constexpr struct {
    const char* name;
    const char* filename;
    bool asData;
    const char* plugin;
} DetectData[]{
    /* Try to keep the order the same as in the documentation, and use all
       variants if there are */
    {"ASTC", "8x8.astc", false, "AstcImporter"},
    {"ASTC data", "8x8.astc", true, "AstcImporter"},
    {"Basis", "rgb.basis", false, "BasisImporter"},
    {"Basis data", "rgb.basis", true, "BasisImporter"},
    {"BMP", "rgb.bmp", false, "BmpImporter"},
    {"BMP data", "rgb.bmp", true, "BmpImporter"},
    {"DDS", "rgba_dxt1.dds", false, "DdsImporter"},
    {"DDS data", "rgba_dxt1.dds", true, "DdsImporter"},
    {"GIF", "image.gif", false, "GifImporter"},
    {"OpenEXR", "skybox.exr", false, "OpenExrImporter"},
    {"HDR", "rgb.hdr", false, "HdrImporter"},
    {"HDR data", "rgb.hdr", true, "HdrImporter"},
    {"HDR data, different signature", "rgb.2.hdr", true, "HdrImporter"},
    {"ICO", "pngs.ico", false, "IcoImporter"},
    {"JPEG", "gray.jpg", false, "JpegImporter"},
    {"JPEG data", "gray.jpg", true, "JpegImporter"},
    {"JPEG uppercase", "uppercase.JPG", false, "JpegImporter"},
    {"JPEG2000", "image.jp2", false, "Jpeg2000Importer"},
    /* KTX2, including data, tested sufficiently elsewhere */
    {"MNG", "obsolete.mng", false, "MngImporter"},
    {"Portable Bitmap", "text.pbm", false, "PbmImporter"},
    {"Portable Graymap", "text.pgm", false, "PgmImporter"},
    {"Portable Anymap", "text.pnm", false, "PnmImporter"},
    {"Portable Pixmap", "text.ppm", false, "PpmImporter"},
    {"ZSoft PCX", "image.pcx", false, "PcxImporter"},
    {"Softimage PIC", "image.pic", false, "PicImporter"},
    {"PNG", "rgb.png", false, "PngImporter"},
    {"PNG data", "rgb.png", true, "PngImporter"},
    {"PSD", "image.psd", false, "PsdImporter"},
    {"Sillicon Graphics SGI", "pixar.sgi", false, "SgiImporter"},
    {"Sillicon Graphics BW", "pixar.bw", false, "SgiImporter"},
    {"Sillicon Graphics RGB", "pixar.rgb", false, "SgiImporter"},
    {"Sillicon Graphics RGBA", "pixar.rgba", false, "SgiImporter"},
    {"TIFF", "image.tiff", false, "TiffImporter"},
    {"TIFF, 3-character extension", "image.tif", false, "TiffImporter"},
    {"TIFF data", "image.tiff", true, "TiffImporter"},
    /* TGA, including data, tested sufficiently elsewhere. The extension
       variants however cannot be tested because the plugin is available. */
    {"OpenVDB", "volume.vdb", false, "OpenVdbImporter"},
    {"WebP", "rgb-lossless.webp", false, "WebPImporter"},
    {"WebP data", "rgb-lossless.webp", true, "WebPImporter"}
};

const struct {
    const char* name;
    bool ktxImporterPresent;
    bool basisImporterPresent;
    bool verbose;
    const char* expectedMessage;
} KtxBasisFallbackData[]{
    {"both KtxImporter and BasisImporter present", true, true, true,
        "Trade::AnyImageImporter::{}(): using KtxImporter\n"},
    {"only KtxImporter present", true, false, true,
        "Trade::AnyImageImporter::{}(): using KtxImporter\n"},
    {"only BasisImporter present", false, true, true,
        "Trade::AnyImageImporter::{0}(): KtxImporter not found, trying a fallback\n"
        "Trade::AnyImageImporter::{0}(): using BasisImporter\n"},
    {"only BasisImporter present, verbose output disabled", false, true, false,
        nullptr},
    {"neither present", false, false, true,
        #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
        "PluginManager::Manager::load(): plugin KtxImporter is not static and was not found in nonexistent\n"
        #else
        "PluginManager::Manager::load(): plugin KtxImporter was not found\n"
        #endif
        "Trade::AnyImageImporter::{}(): cannot load the KtxImporter plugin"}
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
    {"HDR, but without the trailing newline", "#?RADIANCE."_s, "233f5241"},
    {"TIFF, but too short", "II\x2a"_s, "49492a"},
    {"TIFF, but no zero byte", "MM\xff\x2a"_s, "4d4dff2a"},
    {"KTX, but wrong version", "\xabKTX 30\xbb\r\n\x1a\n"_s, "ab4b5458"},
    {"RIFF (for WebP), but only 4 bytes", "RIFF"_s, "52494646"},
    {"WebP, but only 11 bytes", "RIFF    WEB"_s, "52494646"}
};

constexpr struct {
    const char* name;
    const char* filename;
    bool asData;
} PropagateConfigurationData[]{
    {"EXR", "depth32f-custom-channels.exr", false},
    {"EXR data", "depth32f-custom-channels.exr", true}
};

constexpr struct {
    const char* name;
    const char* filename;
    bool asData;
    const char* messageFunctionName;
    ImporterFlags flags;
    bool quiet;
} PropagateConfigurationUnknownData[]{
    {"", "rgb.tga", false, "openFile", {}, false},
    {"quiet", "rgb.tga", false, "", ImporterFlag::Quiet, true},
    {"data", "rgb.tga", true, "openData", {}, false},
    {"data, quiet", "rgb.tga", true, "openData", ImporterFlag::Quiet, true}
};

AnyImageImporterTest::AnyImageImporterTest() {
    addInstancedTests({&AnyImageImporterTest::load},
        Containers::arraySize(LoadData));

    addInstancedTests({&AnyImageImporterTest::detect},
        Containers::arraySize(DetectData));

    addInstancedTests({&AnyImageImporterTest::ktxBasisFallbackFile,
                       &AnyImageImporterTest::ktxBasisFallbackData},
        Containers::arraySize(KtxBasisFallbackData));

    addTests({&AnyImageImporterTest::unknownExtension});

    addInstancedTests({&AnyImageImporterTest::unknownSignature},
        Containers::arraySize(DetectUnknownData));

    addTests({&AnyImageImporterTest::emptyData});

    addInstancedTests({&AnyImageImporterTest::propagateFlags},
        Containers::arraySize(LoadData));

    addInstancedTests({&AnyImageImporterTest::propagateConfiguration},
        Containers::arraySize(PropagateConfigurationData));

    addInstancedTests({&AnyImageImporterTest::propagateConfigurationUnknown},
        Containers::arraySize(PropagateConfigurationUnknownData));

    addTests({&AnyImageImporterTest::propagateFileCallback,

              &AnyImageImporterTest::images1D,
              &AnyImageImporterTest::images2D,
              &AnyImageImporterTest::images3D,
              &AnyImageImporterTest::imageLevels1D,
              &AnyImageImporterTest::imageLevels2D,
              &AnyImageImporterTest::imageLevels3D});

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

    if(data.asData) {
        Containers::Optional<Containers::Array<char>> read = Utility::Path::read(Utility::Path::join(ANYIMAGEIMPORTER_TEST_DIR, data.filename));
        CORRADE_VERIFY(read);
        CORRADE_VERIFY(importer->openData(*read));
    } else CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYIMAGEIMPORTER_TEST_DIR, data.filename)));
    CORRADE_COMPARE(importer->image2DCount(), 1);

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
    Containers::String filename = Utility::Path::join(ANYIMAGEIMPORTER_TEST_DIR, data.filename);

    Containers::String out;
    Error redirectError{&out};
    if(data.asData) {
        Containers::Optional<Containers::Array<char>> read = Utility::Path::read(filename);
        CORRADE_VERIFY(read);
        CORRADE_VERIFY(!importer->openData(*read));
    } else CORRADE_VERIFY(!importer->openFile(filename));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\n"
        "Trade::AnyImageImporter::{1}(): cannot load the {0} plugin\n",
        data.plugin, data.asData ? "openData" : "openFile"));
    #else
    CORRADE_COMPARE(out, Utility::format(
        "PluginManager::Manager::load(): plugin {0} was not found\n"
        "Trade::AnyImageImporter::{1}(): cannot load the {0} plugin\n",
        data.plugin, data.asData ? "openData" : "openFile"));
    #endif
}

void AnyImageImporterTest::ktxBasisFallbackFile() {
    auto&& data = KtxBasisFallbackData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(data.ktxImporterPresent && !(manager.load("KtxImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImporter plugin can't be loaded.");
    if(data.basisImporterPresent && !(manager.load("BasisImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("BasisImporter plugin can't be loaded.");

    /* Set invalid plugin directory to ensure the remaining plugins don't get
       loaded after this point */
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    manager.setPluginDirectory("nonexistent");
    #endif

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnyImageImporter");
    if(data.verbose)
        importer->setFlags(ImporterFlag::Verbose);

    Containers::String out;
    Debug redirectOutput{&out};
    Error redirectError{&out};
    /* We don't care if the file opens (it won't if BasisImporter isn't
       present), just verifying if correct plugin got picked by checking the
       message. */
    importer->openFile(Utility::Path::join(ANYIMAGEIMPORTER_TEST_DIR, "basis.ktx2"));
    if(data.expectedMessage) CORRADE_COMPARE_AS(out,
        Utility::format(data.expectedMessage, "openFile"),
        TestSuite::Compare::StringHasPrefix);
    else CORRADE_COMPARE(out, "");
}

void AnyImageImporterTest::ktxBasisFallbackData() {
    auto&& data = KtxBasisFallbackData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(data.ktxImporterPresent && !(manager.load("KtxImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImporter plugin can't be loaded.");
    if(data.basisImporterPresent && !(manager.load("BasisImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("BasisImporter plugin can't be loaded.");

    /* Set invalid plugin directory to ensure the remaining plugins don't get
       loaded after this point */
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    manager.setPluginDirectory("nonexistent");
    #endif

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnyImageImporter");
    if(data.verbose)
        importer->setFlags(ImporterFlag::Verbose);

    Containers::Optional<Containers::Array<char>> read = Utility::Path::read(Utility::Path::join(ANYIMAGEIMPORTER_TEST_DIR, "basis.ktx2"));
    CORRADE_VERIFY(read);

    Containers::String out;
    Debug redirectOutput{&out};
    Error redirectError{&out};
    /* We don't care if the file opens (it won't if BasisImporter isn't
       present), just verifying if correct plugin got picked by checking the
       message. */
    importer->openData(*read);
    if(data.expectedMessage) CORRADE_COMPARE_AS(out,
        Utility::format(data.expectedMessage, "openData"),
        TestSuite::Compare::StringHasPrefix);
    else CORRADE_COMPARE(out, "");
}

void AnyImageImporterTest::unknownExtension() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnyImageImporter");

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->openFile("image.xcf"));
    CORRADE_COMPARE(out, "Trade::AnyImageImporter::openFile(): cannot determine the format of image.xcf\n");
}

void AnyImageImporterTest::unknownSignature() {
    auto&& data = DetectUnknownData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnyImageImporter");

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->openData(data.data));
    CORRADE_COMPARE(out, Utility::format("Trade::AnyImageImporter::openData(): cannot determine the format from signature 0x{}\n", data.signature));
}

void AnyImageImporterTest::emptyData() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnyImageImporter");

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->openData(nullptr));
    CORRADE_COMPARE(out, "Trade::AnyImageImporter::openData(): file is empty\n");
}

void AnyImageImporterTest::propagateFlags() {
    auto&& data = LoadData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImporter plugin not enabled, cannot test");

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnyImageImporter");
    importer->setFlags(ImporterFlag::Verbose);

    Containers::String out;
    {
        Debug redirectOutput{&out};
        if(data.asData) {
            Containers::Optional<Containers::Array<char>> read = Utility::Path::read(Utility::Path::join(ANYIMAGEIMPORTER_TEST_DIR, data.filename));
            CORRADE_VERIFY(read);
            CORRADE_VERIFY(importer->openData(*read));
        } else CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYIMAGEIMPORTER_TEST_DIR, data.filename)));
        CORRADE_VERIFY(importer->image2D(0));
    }
    CORRADE_COMPARE(out, Utility::format(
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
        Containers::Optional<Containers::Array<char>> read = Utility::Path::read(Utility::Path::join(ANYIMAGEIMPORTER_TEST_DIR, data.filename));
        CORRADE_VERIFY(read);
        CORRADE_VERIFY(importer->openData(*read));
    } else CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYIMAGEIMPORTER_TEST_DIR, data.filename)));
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
    auto&& data = PropagateConfigurationUnknownData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImporter plugin not enabled, cannot test");

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnyImageImporter");
    importer->configuration().setValue("noSuchOption", "isHere");
    importer->setFlags(data.flags);

    Containers::String out;
    Warning redirectWarning{&out};
    if(data.asData) {
        Containers::Optional<Containers::Array<char>> read = Utility::Path::read(Utility::Path::join(ANYIMAGEIMPORTER_TEST_DIR, data.filename));
        CORRADE_VERIFY(read);
        CORRADE_VERIFY(importer->openData(*read));
    } else CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYIMAGEIMPORTER_TEST_DIR, data.filename)));
    if(data.quiet)
        CORRADE_COMPARE(out, "");
    else
        CORRADE_COMPARE(out, Utility::format("Trade::AnyImageImporter::{}(): option noSuchOption not recognized by TgaImporter\n", data.messageFunctionName));
}

void AnyImageImporterTest::propagateFileCallback() {
    if(!(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImporter plugin not enabled, cannot test");

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnyImageImporter");

    Containers::Array<char> storage;
    importer->setFileCallback([](const std::string&, InputFileCallbackPolicy, Containers::Array<char>& storage) -> Containers::Optional<Containers::ArrayView<const char>> {
        Containers::Optional<Containers::Array<char>> data = Utility::Path::read(Utility::Path::join(ANYIMAGEIMPORTER_TEST_DIR, "rgb.tga"));
        CORRADE_VERIFY(data);
        storage = *Utility::move(data);
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

void AnyImageImporterTest::images1D() {
    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImporter plugin can't be loaded.");

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnyImageImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYIMAGEIMPORTER_TEST_DIR, "1d-mipmaps.ktx2")));
    CORRADE_COMPARE(importer->image1DCount(), 1);
    CORRADE_COMPARE(importer->image2DCount(), 0);
    CORRADE_COMPARE(importer->image3DCount(), 0);

    /* Check only size, as it is good enough proof that it is working */
    Containers::Optional<ImageData1D> image = importer->image1D(0);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->size(), 4);
}

void AnyImageImporterTest::images2D() {
    if(!(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImporter plugin not enabled, cannot test");

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("AnyImageImporter");

    CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYIMAGEIMPORTER_TEST_DIR, "rgb.tga")));
    CORRADE_COMPARE(importer->image1DCount(), 0);
    CORRADE_COMPARE(importer->image2DCount(), 1);
    CORRADE_COMPARE(importer->image3DCount(), 0);

    /* Check only size, as it is good enough proof that it is working */
    Containers::Optional<ImageData2D> image = importer->image2D(0);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->size(), (Vector2i{3, 2}));
}

void AnyImageImporterTest::images3D() {
    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImporter plugin can't be loaded.");

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnyImageImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYIMAGEIMPORTER_TEST_DIR, "2d-mipmaps-and-layers.ktx2")));
    CORRADE_COMPARE(importer->image1DCount(), 0);
    CORRADE_COMPARE(importer->image2DCount(), 0);
    CORRADE_COMPARE(importer->image3DCount(), 1);

    /* Check only size, as it is good enough proof that it is working */
    Containers::Optional<ImageData3D> image = importer->image3D(0);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->size(), (Vector3i{4, 3, 3}));
}

void AnyImageImporterTest::imageLevels1D() {
    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImporter plugin can't be loaded.");

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnyImageImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYIMAGEIMPORTER_TEST_DIR, "1d-mipmaps.ktx2")));
    CORRADE_COMPARE(importer->image1DCount(), 1);
    CORRADE_COMPARE(importer->image1DLevelCount(0), 3);

    /* Check only size, as it is good enough proof that it is working */
    Containers::Optional<ImageData1D> image = importer->image1D(0, 1);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->size(), 2);
}

void AnyImageImporterTest::imageLevels2D() {
    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImporter plugin can't be loaded.");

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnyImageImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYIMAGEIMPORTER_TEST_DIR, "2d-mipmaps.ktx2")));
    CORRADE_COMPARE(importer->image2DCount(), 1);
    CORRADE_COMPARE(importer->image2DLevelCount(0), 3);

    /* Check only size, as it is good enough proof that it is working */
    Containers::Optional<ImageData2D> image = importer->image2D(0, 1);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->size(), (Vector2i{2, 1}));
}

void AnyImageImporterTest::imageLevels3D() {
    PluginManager::Manager<AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Catch also ABI and interface mismatch errors */
    if(!(manager.load("KtxImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("KtxImporter plugin can't be loaded.");

    Containers::Pointer<AbstractImporter> importer = manager.instantiate("AnyImageImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(ANYIMAGEIMPORTER_TEST_DIR, "2d-mipmaps-and-layers.ktx2")));
    CORRADE_COMPARE(importer->image3DCount(), 1);
    CORRADE_COMPARE(importer->image3DLevelCount(0), 3);

    /* Check only size, as it is good enough proof that it is working */
    Containers::Optional<ImageData3D> image = importer->image3D(0, 1);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->size(), (Vector3i{2, 1, 3}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AnyImageImporterTest)
