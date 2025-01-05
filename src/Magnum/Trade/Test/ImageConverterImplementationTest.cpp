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

#include <sstream> /** @todo remove once Configuration is stream-free */
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/StringToFile.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/Configuration.h>
#include <Corrade/Utility/Path.h>

#include "Magnum/Trade/Implementation/converterUtilities.h"

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct ImageConverterImplementationTest: TestSuite::Tester {
    explicit ImageConverterImplementationTest();

    void pluginInfo();
    void pluginInfoAliases();
    void pluginConfigurationInfoEmpty();
    void pluginConfigurationInfo();
    void pluginConfigurationInfoDoxygenDelimiter();
    void importerInfo();
    void converterInfo();
    void converterInfoExtensionMimeType();
    void converterInfoExtensionMimeTypeNoFileConversion();

    void info();
    void infoError();

    /* Explicitly forbid system-wide plugin dependencies */
    PluginManager::Manager<Trade::AbstractImporter> _importerManager{"nonexistent"};
    PluginManager::Manager<Trade::AbstractImageConverter> _converterManager{"nonexistent"};
};

ImageConverterImplementationTest::ImageConverterImplementationTest() {
    addTests({&ImageConverterImplementationTest::pluginInfo,
              &ImageConverterImplementationTest::pluginInfoAliases,
              &ImageConverterImplementationTest::pluginConfigurationInfoEmpty,
              &ImageConverterImplementationTest::pluginConfigurationInfo,
              &ImageConverterImplementationTest::pluginConfigurationInfoDoxygenDelimiter,
              &ImageConverterImplementationTest::importerInfo,
              &ImageConverterImplementationTest::converterInfo,
              &ImageConverterImplementationTest::converterInfoExtensionMimeType,
              &ImageConverterImplementationTest::converterInfoExtensionMimeTypeNoFileConversion,

              &ImageConverterImplementationTest::info,
              &ImageConverterImplementationTest::infoError});

    /* Load the plugin directly from the build tree. Otherwise it's static and
       already loaded. */
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_importerManager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_converterManager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    #ifdef TGAIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_converterManager.load(TGAIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
}

void ImageConverterImplementationTest::pluginInfo() {
    /* Check if the required plugin can be loaded. Catches also ABI and
       interface mismatch errors. */
    if(!(_converterManager.load("AnyImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageConverter plugin can't be loaded.");

    Containers::Pointer<Trade::AbstractImageConverter> converter = _converterManager.instantiate("AnyImageConverter");

    /* Print to visually verify coloring */
    {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printPluginInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, *converter);
        Debug{} << "======================== visual color verification end =========================";
    }

    Containers::String out;
    Debug redirectOutput{&out};
    Implementation::printPluginInfo(Debug::Flag::DisableColors, *converter);
    CORRADE_COMPARE(out,
        "Plugin name: AnyImageConverter\n"
        "Features:\n"
        "  Convert1DToFile\n"
        "  Convert2DToFile\n"
        "  Convert3DToFile\n"
        "  ConvertCompressed1DToFile\n"
        "  ConvertCompressed2DToFile\n"
        "  ConvertCompressed3DToFile\n"
        "  Levels\n");
}

void ImageConverterImplementationTest::pluginInfoAliases() {
    PluginManager::Manager<Trade::AbstractImporter> importerManager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};

    /* Check if the required plugin can be loaded. Catches also ABI and
       interface mismatch errors. */
    if(!(importerManager.load("StbImageImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("StbImageImporter plugin can't be loaded.");

    /* Loading under an alias to verify that it's highlighted. Make
       StbImageImporter *the* plugin to load PPMs, so it's not replaced by e.g.
       DevIlImageImporter. */
    importerManager.setPreferredPlugins("PpmImporter", {"StbImageImporter"});
    Containers::Pointer<Trade::AbstractImporter> importer = importerManager.instantiate("PpmImporter");

    /* Print to visually verify coloring */
    {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printPluginInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, *importer);
        Debug{} << "======================== visual color verification end =========================";
    }

    Containers::String out;
    Debug redirectOutput{&out};
    Implementation::printPluginInfo(Debug::Flag::DisableColors, *importer);
    CORRADE_COMPARE(out,
        "Plugin name: StbImageImporter\n"
        "Aliases:\n"
        "  BmpImporter\n"
        "  GifImporter\n"
        "  HdrImporter\n"
        "  JpegImporter\n"
        "  PgmImporter\n"
        "  PicImporter\n"
        "  PngImporter\n"
        "  PpmImporter\n"
        "  PsdImporter\n"
        "  TgaImporter\n"
        "Features:\n"
        "  OpenData\n");
}

void ImageConverterImplementationTest::pluginConfigurationInfoEmpty() {
    struct: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override {
            return Trade::ImporterFeature::FileCallback|
                   Trade::ImporterFeature::OpenState;
        }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    Containers::String out;
    Debug redirectOutput{&out};
    Implementation::printPluginConfigurationInfo(Debug::Flag::DisableColors, importer);
    CORRADE_COMPARE(out, "");
}

void ImageConverterImplementationTest::pluginConfigurationInfo() {
    struct: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    /** @todo UGH, fix the insane Configuration API already */
    std::stringstream in;
    in << R"([configuration]
# A comment
; Another
value=yes
another=42
# Empty lines should not have trailing whitespace

[configuration/group]
spaces="  YES  "
newlines="""
A
 L
  S
   O
"""

[configuration/group/subgroup]
subvalue=35

# Another instance of the same group
[configuration/group]
true=false
)";
    importer.configuration() = Utility::ConfigurationGroup{*Utility::Configuration{in}.group("configuration")};

    /* Print to visually verify coloring */
    {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printPluginConfigurationInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, importer);
        Debug{} << "======================== visual color verification end =========================";
    }

    Containers::String out;
    Debug redirectOutput{&out};
    Implementation::printPluginConfigurationInfo(Debug::Flag::DisableColors, importer);
    CORRADE_COMPARE(out,
        "Configuration:\n"
        "  # A comment\n"
        "  ; Another\n"
        "  value=yes\n"
        "  another=42\n"
        "  # Empty lines should not have trailing whitespace\n"
        "\n"
        "  [group]\n"
        "  spaces=\"  YES  \"\n"
        "  newlines=\"\"\"\n"
        "  A\n"
        "   L\n"
        "    S\n"
        "     O\n"
        "  \"\"\"\n"
        "\n"
        "  [group/subgroup]\n"
        "  subvalue=35\n"
        "\n"
        "  # Another instance of the same group\n"
        "  [group]\n"
        "  true=false\n");
}

void ImageConverterImplementationTest::pluginConfigurationInfoDoxygenDelimiter() {
    struct: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    /** @todo UGH, fix the insane Configuration API already */
    std::stringstream in;
    in << R"(# [configuration_]
[configuration]
# A comment
value=yes
# [configuration_]

newlyAddedValue=42
)";
    Utility::Configuration conf{in};

    importer.configuration() = Utility::ConfigurationGroup{*conf.group("configuration")};

    Containers::String out;
    Debug redirectOutput{&out};
    Implementation::printPluginConfigurationInfo(Debug::Flag::DisableColors, importer);
    CORRADE_COMPARE(out,
        "Configuration:\n"
        "  # A comment\n"
        "  value=yes\n"
        "\n"
        "  newlyAddedValue=42\n");
}

void ImageConverterImplementationTest::importerInfo() {
    /* Check if the required plugin can be loaded. Catches also ABI and
       interface mismatch errors. */
    if(!(_importerManager.load("AnyImageImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter plugin can't be loaded.");

    Containers::Pointer<Trade::AbstractImporter> importer = _importerManager.instantiate("AnyImageImporter");
    /** @todo pick a plugin that has some actual configuration */
    importer->configuration().setValue("something", "is there");

    /* Print to visually verify coloring */
    {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printImporterInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, *importer);
        Debug{} << "======================== visual color verification end =========================";
    }

    Containers::String out;
    Debug redirectOutput{&out};
    Implementation::printImporterInfo(Debug::Flag::DisableColors, *importer);
    CORRADE_COMPARE(out,
        "Plugin name: AnyImageImporter\n"
        "Features:\n"
        "  OpenData\n"
        "  FileCallback\n"
        "Configuration:\n"
        "  something=is there\n");
}

void ImageConverterImplementationTest::converterInfo() {
    /* Check if the required plugin can be loaded. Catches also ABI and
       interface mismatch errors. */
    if(!(_converterManager.load("AnyImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageConverter plugin can't be loaded.");

    Containers::Pointer<Trade::AbstractImageConverter> converter = _converterManager.instantiate("AnyImageConverter");
    /** @todo pick a plugin that has some actual configuration */
    converter->configuration().setValue("something", "is there");

    /* Print to visually verify coloring */
    {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printImageConverterInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, *converter);
        Debug{} << "======================== visual color verification end =========================";
    }

    Containers::String out;
    Debug redirectOutput{&out};
    Implementation::printImageConverterInfo(Debug::Flag::DisableColors, *converter);
    CORRADE_COMPARE(out,
        "Plugin name: AnyImageConverter\n"
        "Features:\n"
        "  Convert1DToFile\n"
        "  Convert2DToFile\n"
        "  Convert3DToFile\n"
        "  ConvertCompressed1DToFile\n"
        "  ConvertCompressed2DToFile\n"
        "  ConvertCompressed3DToFile\n"
        "  Levels\n"
        "Configuration:\n"
        "  something=is there\n");
}

void ImageConverterImplementationTest::converterInfoExtensionMimeType() {
    /* Check if the required plugin can be loaded. Catches also ABI and
       interface mismatch errors. */
    if(!(_converterManager.load("TgaImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImageConverter plugin can't be loaded.");

    Containers::Pointer<Trade::AbstractImageConverter> converter = _converterManager.instantiate("TgaImageConverter");
    converter->configuration().setValue("rle", "yes hello");

    /* Print to visually verify coloring */
    {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printImageConverterInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, *converter);
        Debug{} << "======================== visual color verification end =========================";
    }

    Containers::String out;
    Debug redirectOutput{&out};
    Implementation::printImageConverterInfo(Debug::Flag::DisableColors, *converter);
    CORRADE_COMPARE_AS(out,
        "Plugin name: TgaImageConverter\n"
        "Features:\n"
        "  Convert2DToData\n"
        "File extension: tga\n"
        "MIME type: image/x-tga\n"
        "Configuration:\n"
        "  # Run-length encode the data for smaller file size\n"
        "  rle=yes hello\n",
        TestSuite::Compare::StringHasPrefix);
}

void ImageConverterImplementationTest::converterInfoExtensionMimeTypeNoFileConversion() {
    PluginManager::Manager<Trade::AbstractImageConverter> converterManager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};

    /* Check if the required plugin can be loaded. Catches also ABI and
       interface mismatch errors. */
    if(!(converterManager.load("StbResizeImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("StbResizeImageConverter plugin can't be loaded.");

    Containers::Pointer<Trade::AbstractImageConverter> converter = converterManager.instantiate("StbResizeImageConverter");

    /* Print to visually verify coloring */
    {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printImageConverterInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, *converter);
        Debug{} << "======================== visual color verification end =========================";
    }

    Containers::String out;
    Debug redirectOutput{&out};
    Implementation::printImageConverterInfo(Debug::Flag::DisableColors, *converter);
    CORRADE_COMPARE_AS(out,
        "Plugin name: StbResizeImageConverter\n"
        "Features:\n"
        "  Convert2D\n"
        "  Convert3D\n"
        "Configuration:\n"
        "  # Target width and height",
        TestSuite::Compare::StringHasPrefix);
}

void ImageConverterImplementationTest::info() {
    struct Importer: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        /* Three 1D images, one with two levels and named, one compressed,
           one just to not have two of everything */
        UnsignedInt doImage1DCount() const override { return 3; }
        UnsignedInt doImage1DLevelCount(UnsignedInt id) override {
            return id == 1 ? 2 : 1;
        }
        Containers::String doImage1DName(UnsignedInt id) override {
            return id == 2 ? "Third 1D image just so there aren't two" : "";
        }
        Containers::Optional<Trade::ImageData1D> doImage1D(UnsignedInt id, UnsignedInt level) override {
            if(id == 0 && level == 0)
                return Trade::ImageData1D{CompressedPixelFormat::Astc10x10RGBAF, 1024, Containers::Array<char>{NoInit, 4096}};
            if(id == 1 && level == 0)
                return Trade::ImageData1D{PixelFormat::RGBA8Snorm, 16, Containers::Array<char>{NoInit, 64}};
            if(id == 1 && level == 1)
                return Trade::ImageData1D{PixelFormat::RGBA8Snorm, 8, Containers::Array<char>{NoInit, 32}};
            if(id == 2 && level == 0)
                return Trade::ImageData1D{PixelFormat::Depth16Unorm, 4, Containers::Array<char>{NoInit, 8}};
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        /* Two 2D images, one with three levels and named, the other compressed
           and array */
        UnsignedInt doImage2DCount() const override { return 2; }
        UnsignedInt doImage2DLevelCount(UnsignedInt id) override {
            return id == 0 ? 3 : 1;
        }
        Containers::String doImage2DName(UnsignedInt id) override {
            return id == 0 ? "A very nice mipmapped 2D image" : "";
        }
        Containers::Optional<Trade::ImageData2D> doImage2D(UnsignedInt id, UnsignedInt level) override {
            if(id == 0 && level == 0)
                return Trade::ImageData2D{PixelFormat::RG16F, {256, 128}, Containers::Array<char>{NoInit, 131072}};
            if(id == 0 && level == 1)
                return Trade::ImageData2D{PixelFormat::RG16F, {128, 64}, Containers::Array<char>{NoInit, 32768}};
            if(id == 0 && level == 2)
                return Trade::ImageData2D{PixelFormat::RG16F, {64, 32}, Containers::Array<char>{NoInit, 8192}};
            if(id == 1)
                return Trade::ImageData2D{CompressedPixelFormat::PvrtcRGB2bppUnorm, {4, 8}, Containers::Array<char>{NoInit, 32}, ImageFlag2D::Array};
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        /* One 2D cube map array image, one 3D mipmapped & named and two 2D
           array; with one externally owned */
        UnsignedInt doImage3DCount() const override { return 4; }
        UnsignedInt doImage3DLevelCount(UnsignedInt id) override {
            return id == 1 ? 2 : 1;
        }
        Containers::String doImage3DName(UnsignedInt id) override {
            return id == 1 ? "Volume kills!" : "";
        }
        Containers::Optional<Trade::ImageData3D> doImage3D(UnsignedInt id, UnsignedInt level) override {
            if(id == 0 && level == 0)
                return Trade::ImageData3D{PixelFormat::R8Unorm, {16, 16, 12}, Containers::Array<char>{NoInit, 3072}, ImageFlag3D::CubeMap|ImageFlag3D::Array};
            if(id == 1 && level == 0)
                return Trade::ImageData3D{PixelFormat::R8Unorm, {16, 16, 16}, Containers::Array<char>{NoInit, 4096}};
            if(id == 1 && level == 1)
                return Trade::ImageData3D{PixelFormat::R8Unorm, {8, 8, 6}, Containers::Array<char>{NoInit, 2048}};
            if(id == 2 && level == 0)
                return Trade::ImageData3D{CompressedPixelFormat::Bc1RGBSrgb, {4, 1, 1}, Containers::Array<char>{NoInit, 16}, ImageFlag3D::Array};
            if(id == 3 && level == 0)
                return Trade::ImageData3D{PixelFormat::R32F, {1, 4, 1}, Trade::DataFlag::ExternallyOwned|Trade::DataFlag::Mutable, data, ImageFlag3D::Array};
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        char data[16];
    } importer;

    bool error = false;
    std::chrono::high_resolution_clock::duration time;
    Containers::Array<Implementation::ImageInfo> infos = Implementation::imageInfo(importer, error, time);
    CORRADE_VERIFY(!error);
    CORRADE_COMPARE(infos.size(), 13);

    /* Print to visually verify coloring */
    {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printImageInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, infos, nullptr, nullptr, nullptr);
        Debug{} << "======================== visual color verification end =========================";
    }

    Containers::String out;
    Debug redirectOutput{&out};
    Implementation::printImageInfo(Debug::Flag::DisableColors, infos, nullptr, nullptr, nullptr);
    CORRADE_COMPARE_AS(out,
        Utility::Path::join(TRADE_TEST_DIR, "ImageConverterImplementationTestFiles/info.txt"),
        TestSuite::Compare::StringToFile);
}

void ImageConverterImplementationTest::infoError() {
    struct Importer: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage1DCount() const override { return 2; }
        Containers::Optional<Trade::ImageData1D> doImage1D(UnsignedInt id, UnsignedInt) override {
            Error{} << "1D image" << id << "error!";
            return {};
        }

        UnsignedInt doImage2DCount() const override { return 2; }
        Containers::Optional<Trade::ImageData2D> doImage2D(UnsignedInt id, UnsignedInt) override {
            Error{} << "2D image" << id << "error!";
            return {};
        }

        UnsignedInt doImage3DCount() const override { return 2; }
        Containers::Optional<Trade::ImageData3D> doImage3D(UnsignedInt id, UnsignedInt) override {
            Error{} << "3D image" << id << "error!";
            return {};
        }
    } importer;

    bool error = false;
    std::chrono::high_resolution_clock::duration time;
    Containers::String out;
    Debug redirectOutput{&out};
    Error redirectError{&out};
    Containers::Array<Implementation::ImageInfo> infos = Implementation::imageInfo(importer, error, time);
    /* It should return a failure and no output */
    CORRADE_VERIFY(error);
    CORRADE_VERIFY(infos.isEmpty());
    /* But it should not exit after first error */
    CORRADE_COMPARE(out,
        "1D image 0 error!\n"
        "Can't import 1D image 0 level 0\n"
        "1D image 1 error!\n"
        "Can't import 1D image 1 level 0\n"
        "2D image 0 error!\n"
        "Can't import 2D image 0 level 0\n"
        "2D image 1 error!\n"
        "Can't import 2D image 1 level 0\n"
        "3D image 0 error!\n"
        "Can't import 3D image 0 level 0\n"
        "3D image 1 error!\n"
        "Can't import 3D image 1 level 0\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::ImageConverterImplementationTest)
