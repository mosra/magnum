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
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/FileToString.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Trade/AbstractImageConverter.h"
#include "Magnum/Trade/ImageData.h"

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct AbstractImageConverterTest: TestSuite::Tester {
    explicit AbstractImageConverterTest();

    void construct();
    void constructWithPluginManagerReference();

    void setFlags();
    void setFlagsNotImplemented();

    void thingNotSupported();

    void exportToImage();
    void exportToImageNotImplemented();
    void exportToImageCustomDeleter();

    void exportToCompressedImage();
    void exportToCompressedImageNotImplemented();
    void exportToCompressedImageCustomDeleter();

    void exportToData();
    void exportToDataNotImplemented();
    void exportToDataCustomDeleter();

    void exportCompressedToData();
    void exportCompressedToDataNotImplemented();
    void exportCompressedToDataCustomDeleter();

    void exportImageDataToData();

    void exportToFile();
    void exportToFileThroughData();
    void exportToFileThroughDataFailed();
    void exportToFileThroughDataNotWritable();
    void exportToFileNotImplemented();

    void exportCompressedToFile();
    void exportCompressedToFileThroughData();
    void exportCompressedToFileThroughDataFailed();
    void exportCompressedToFileThroughDataNotWritable();
    void exportCompressedToFileNotImplemented();

    void exportImageDataToFile();

    void debugFeature();
    void debugFeatures();
    void debugFlag();
    void debugFlags();
};

AbstractImageConverterTest::AbstractImageConverterTest() {
    addTests({&AbstractImageConverterTest::construct,
              &AbstractImageConverterTest::constructWithPluginManagerReference,

              &AbstractImageConverterTest::setFlags,
              &AbstractImageConverterTest::setFlagsNotImplemented,

              &AbstractImageConverterTest::thingNotSupported,

              &AbstractImageConverterTest::exportToImage,
              &AbstractImageConverterTest::exportToImageNotImplemented,
              &AbstractImageConverterTest::exportToImageCustomDeleter,

              &AbstractImageConverterTest::exportToCompressedImage,
              &AbstractImageConverterTest::exportToCompressedImageNotImplemented,
              &AbstractImageConverterTest::exportToCompressedImageCustomDeleter,

              &AbstractImageConverterTest::exportToData,
              &AbstractImageConverterTest::exportToDataNotImplemented,
              &AbstractImageConverterTest::exportToDataCustomDeleter,

              &AbstractImageConverterTest::exportCompressedToData,
              &AbstractImageConverterTest::exportCompressedToDataNotImplemented,
              &AbstractImageConverterTest::exportCompressedToDataCustomDeleter,

              &AbstractImageConverterTest::exportImageDataToData,

              &AbstractImageConverterTest::exportToFile,
              &AbstractImageConverterTest::exportToFileThroughData,
              &AbstractImageConverterTest::exportToFileThroughDataFailed,
              &AbstractImageConverterTest::exportToFileThroughDataNotWritable,
              &AbstractImageConverterTest::exportToFileNotImplemented,

              &AbstractImageConverterTest::exportCompressedToFile,
              &AbstractImageConverterTest::exportCompressedToFileThroughData,
              &AbstractImageConverterTest::exportCompressedToFileThroughDataFailed,
              &AbstractImageConverterTest::exportCompressedToFileThroughDataNotWritable,
              &AbstractImageConverterTest::exportCompressedToFileNotImplemented,

              &AbstractImageConverterTest::exportImageDataToFile,

              &AbstractImageConverterTest::debugFeature,
              &AbstractImageConverterTest::debugFeatures,
              &AbstractImageConverterTest::debugFlag,
              &AbstractImageConverterTest::debugFlags});

    /* Create testing dir */
    Utility::Directory::mkpath(TRADE_TEST_OUTPUT_DIR);
}

void AbstractImageConverterTest::construct() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return {}; }
    } converter;

    CORRADE_COMPARE(converter.features(), ImageConverterFeatures{});
}

void AbstractImageConverterTest::constructWithPluginManagerReference() {
    PluginManager::Manager<AbstractImageConverter> manager;

    struct Converter: AbstractImageConverter {
        explicit Converter(PluginManager::Manager<AbstractImageConverter>& manager): AbstractImageConverter{manager} {}

        ImageConverterFeatures doFeatures() const override { return {}; }
    } converter{manager};

    CORRADE_COMPARE(converter.features(), ImageConverterFeatures{});
}

void AbstractImageConverterTest::setFlags() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return {}; }
        void doSetFlags(ImageConverterFlags flags) override {
            _flags = flags;
        }

        ImageConverterFlags _flags;
    } converter;

    CORRADE_COMPARE(converter.flags(), ImageConverterFlags{});
    CORRADE_COMPARE(converter._flags, ImageConverterFlags{});
    converter.setFlags(ImageConverterFlag::Verbose);
    CORRADE_COMPARE(converter.flags(), ImageConverterFlag::Verbose);
    CORRADE_COMPARE(converter._flags, ImageConverterFlag::Verbose);
}

void AbstractImageConverterTest::setFlagsNotImplemented() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return {}; }
    } converter;

    CORRADE_COMPARE(converter.flags(), ImageConverterFlags{});
    converter.setFlags(ImageConverterFlag::Verbose);
    CORRADE_COMPARE(converter.flags(), ImageConverterFlag::Verbose);
    /* Should just work, no need to implement the function */
}

void AbstractImageConverterTest::thingNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return {}; }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.exportToImage(ImageView2D{PixelFormat::R8Unorm, {4, 6}, Containers::ArrayView<char>{nullptr, 24}});
    converter.exportToCompressedImage(ImageView2D{PixelFormat::R8Unorm, {16, 8}, Containers::ArrayView<char>{nullptr, 128}});
    converter.exportToData(ImageView2D{PixelFormat::RGBA8Unorm, {4, 6}, Containers::ArrayView<char>{nullptr, 96}});
    converter.exportToData(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 8}, Containers::ArrayView<char>{nullptr, 64}});
    converter.exportToFile(ImageView2D{PixelFormat::RGBA8Unorm, {4, 6}, {nullptr, 96}}, Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    converter.exportToFile(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 8}, {nullptr, 64}}, Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImageConverter::exportToImage(): feature not supported\n"
        "Trade::AbstractImageConverter::exportToCompressedImage(): feature not supported\n"
        "Trade::AbstractImageConverter::exportToData(): feature not supported\n"
        "Trade::AbstractImageConverter::exportToData(): feature not supported\n"
        "Trade::AbstractImageConverter::exportToFile(): feature not supported\n"
        "Trade::AbstractImageConverter::exportToFile(): feature not supported\n");
}

void AbstractImageConverterTest::exportToImage() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertImage; }
        Containers::Optional<Image2D> doExportToImage(const ImageView2D& image) override {
            return Image2D{PixelFormat::RGBA8Unorm, image.size(), Containers::Array<char>{96}};
        }
    } converter;

    Containers::Optional<Image2D> actual = converter.exportToImage(ImageView2D{PixelFormat::R8Unorm, {4, 6}, Containers::ArrayView<char>{nullptr, 24}});
    CORRADE_VERIFY(actual);
    CORRADE_COMPARE(actual->data().size(), 96);
    CORRADE_COMPARE(actual->size(), (Vector2i{4, 6}));
}

void AbstractImageConverterTest::exportToImageNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertImage; }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.exportToImage(ImageView2D{PixelFormat::R8Unorm, {4, 6}, Containers::ArrayView<char>{nullptr, 128}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::exportToImage(): feature advertised but not implemented\n");
}

void AbstractImageConverterTest::exportToImageCustomDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertImage; }
        Containers::Optional<Image2D> doExportToImage(const ImageView2D&) override {
            return Image2D{PixelFormat::RGBA8Unorm, {}, Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}}};
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.exportToImage(ImageView2D{PixelFormat::R8Unorm, {}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::exportToImage(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImageConverterTest::exportToCompressedImage() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressedImage; }
        Containers::Optional<CompressedImage2D> doExportToCompressedImage(const ImageView2D& image) override {
            return CompressedImage2D{CompressedPixelFormat::Bc1RGBAUnorm, image.size(), Containers::Array<char>{64}};
        }
    } converter;

    Containers::Optional<CompressedImage2D> actual = converter.exportToCompressedImage(ImageView2D{PixelFormat::R8Unorm, {16, 8}, Containers::ArrayView<char>{nullptr, 128}});
    CORRADE_VERIFY(actual);
    CORRADE_COMPARE(actual->data().size(), 64);
    CORRADE_COMPARE(actual->size(), (Vector2i{16, 8}));
}

void AbstractImageConverterTest::exportToCompressedImageNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressedImage; }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.exportToCompressedImage(ImageView2D{PixelFormat::R8Unorm, {16, 8}, Containers::ArrayView<char>{nullptr, 128}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::exportToCompressedImage(): feature advertised but not implemented\n");
}

void AbstractImageConverterTest::exportToCompressedImageCustomDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressedImage; }
        Containers::Optional<CompressedImage2D> doExportToCompressedImage(const ImageView2D&) override {
            return CompressedImage2D{CompressedPixelFormat::Bc1RGBAUnorm, {}, Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}}};
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.exportToCompressedImage(ImageView2D{PixelFormat::R8Unorm, {}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::exportToCompressedImage(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImageConverterTest::exportToData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertData; }
        Containers::Array<char> doExportToData(const ImageView2D& image) override {
            return Containers::Array<char>{nullptr, std::size_t(image.size().product())};
        }
    } converter;

    Containers::Array<char> actual = converter.exportToData(ImageView2D{PixelFormat::RGBA8Unorm, {4, 6}, Containers::ArrayView<char>{nullptr, 96}});
    CORRADE_COMPARE(actual.size(), 24);
}

void AbstractImageConverterTest::exportToDataNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertData; }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.exportToData(ImageView2D{PixelFormat::RGBA8Unorm, {4, 6}, Containers::ArrayView<char>{nullptr, 96}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::exportToData(): feature advertised but not implemented\n");
}

void AbstractImageConverterTest::exportToDataCustomDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertData; }
        Containers::Array<char> doExportToData(const ImageView2D&) override {
            return Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}};
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.exportToData(ImageView2D{PixelFormat::RGBA8Unorm, {}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::exportToData(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImageConverterTest::exportCompressedToData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressedData; }
        Containers::Array<char> doExportToData(const CompressedImageView2D& image) override {
            return Containers::Array<char>{nullptr, std::size_t(image.size().product())};
        }
    } converter;

    Containers::Array<char> actual = converter.exportToData(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 8}, Containers::ArrayView<char>{nullptr, 64}});
    CORRADE_COMPARE(actual.size(), 128);
}

void AbstractImageConverterTest::exportCompressedToDataNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertData; }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.exportToData(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 8}, Containers::ArrayView<char>{nullptr, 64}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::exportToData(): feature advertised but not implemented\n");
}

void AbstractImageConverterTest::exportCompressedToDataCustomDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressedData; }
        Containers::Array<char> doExportToData(const CompressedImageView2D&) override {
            return Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}};
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.exportToData(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::exportToData(): implementation is not allowed to use a custom Array deleter\n");
}

class ImageDataConverter: public Trade::AbstractImageConverter {
    private:
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertData|ImageConverterFeature::ConvertCompressedData; }

        Containers::Array<char> doExportToData(const ImageView2D&) override {
            Containers::Array<char> out{Containers::InPlaceInit, {'B'}};
            return out;
        };

        Containers::Array<char> doExportToData(const CompressedImageView2D&) override {
            Containers::Array<char> out{Containers::InPlaceInit, {'C'}};
            return out;
        };
};

void AbstractImageConverterTest::exportImageDataToData() {
    ImageDataConverter converter;

    {
        /* Should get "B" when converting uncompressed */
        ImageData2D image{PixelFormat::RGBA8Unorm, {}, nullptr};
        CORRADE_COMPARE_AS(converter.exportToData(image),
            (Containers::Array<char>{Containers::InPlaceInit, {'B'}}),
            TestSuite::Compare::Container);
    } {
        /* Should get "C" when converting compressed */
        ImageData2D image{CompressedPixelFormat::Bc1RGBUnorm, {}, nullptr};
        CORRADE_COMPARE_AS(converter.exportToData(image),
            (Containers::Array<char>{Containers::InPlaceInit, {'C'}}),
            TestSuite::Compare::Container);
    }
}

void AbstractImageConverterTest::exportToFile() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertData; }
        bool doExportToFile(const ImageView2D& image, const std::string& filename) override {
            return Utility::Directory::write(filename, Containers::arrayView(
                {char(image.size().x()), char(image.size().y())}));
        }
    } converter;

    const std::string filename = Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out");

    /* Remove previous file, if any */
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));

    CORRADE_VERIFY(converter.exportToFile(ImageView2D{PixelFormat::RGBA8Unorm, {0xf0, 0x0d}, {nullptr, 0xf0*0x0d*4}}, filename));
    CORRADE_COMPARE_AS(filename,
        "\xf0\x0d", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::exportToFileThroughData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertData; }

        Containers::Array<char> doExportToData(const ImageView2D& image) override {
            return Containers::array({char(image.size().x()), char(image.size().y())});
        };
    } converter;

    const std::string filename = Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out");

    /* Remove previous file, if any */
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));

    /* doExportToFile() should call doExportToData() */
    CORRADE_VERIFY(converter.exportToFile(ImageView2D(PixelFormat::RGBA8Unorm, {0xfe, 0xed}, {nullptr, 0xfe*0xed*4}), filename));
    CORRADE_COMPARE_AS(filename,
        "\xfe\xed", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::exportToFileThroughDataFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertData; }

        Containers::Array<char> doExportToData(const ImageView2D&) override {
            return {};
        };
    } converter;

    const std::string filename = Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out");

    /* Remove previous file, if any */
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));

    /* Function should fail, no file should get written and no error output
       should be printed (the base implementation assumes the plugin does it) */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.exportToFile(ImageView2D(PixelFormat::RGBA8Unorm, {0xfe, 0xed}, {nullptr, 0xfe*0xed*4}), filename));
    CORRADE_VERIFY(!Utility::Directory::exists(filename));
    CORRADE_COMPARE(out.str(), "");
}

void AbstractImageConverterTest::exportToFileThroughDataNotWritable() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertData; }

        Containers::Array<char> doExportToData(const ImageView2D& image) override {
            return Containers::array({char(image.size().x()), char(image.size().y())});
        };
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.exportToFile(ImageView2D{PixelFormat::RGBA8Unorm, {0xfe, 0xed}, {nullptr, 0xfe*0xed*4}}, "/some/path/that/does/not/exist"));
    CORRADE_COMPARE(out.str(),
        "Utility::Directory::write(): can't open /some/path/that/does/not/exist\n"
        "Trade::AbstractImageConverter::exportToFile(): cannot write to file /some/path/that/does/not/exist\n");
}

void AbstractImageConverterTest::exportToFileNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertFile; }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.exportToFile(ImageView2D{PixelFormat::RGBA8Unorm, {4, 6}, {nullptr, 96}}, Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::exportToFile(): feature advertised but not implemented\n");
}

void AbstractImageConverterTest::exportCompressedToFile() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressedFile; }
        bool doExportToFile(const CompressedImageView2D& image, const std::string& filename) override {
            return Utility::Directory::write(filename, Containers::arrayView(
                {char(image.size().x()), char(image.size().y())}));
        }
    } converter;

    const std::string filename = Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out");

    /* Remove previous file, if any */
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));

    CORRADE_VERIFY(converter.exportToFile(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {0xd0, 0x0d}, {nullptr, 64}}, filename));
    CORRADE_COMPARE_AS(filename,
        "\xd0\x0d", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::exportCompressedToFileThroughData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressedData; }

        Containers::Array<char> doExportToData(const CompressedImageView2D& image) override {
            return Containers::array({char(image.size().x()), char(image.size().y())});
        };
    } converter;

    const std::string filename = Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out");

    /* Remove previous file, if any */
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));

    /* doExportToFile() should call doExportToData() */
    CORRADE_VERIFY(converter.exportToFile(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {0xb0, 0xd9}, {nullptr, 64}}, filename));
    CORRADE_COMPARE_AS(filename,
        "\xb0\xd9", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::exportCompressedToFileThroughDataFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressedData; }

        Containers::Array<char> doExportToData(const CompressedImageView2D&) override {
            return {};
        };
    } converter;

    const std::string filename = Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out");

    /* Remove previous file, if any */
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));

    /* Function should fail, no file should get written and no error output
       should be printed (the base implementation assumes the plugin does it) */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.exportToFile(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {0xb0, 0xd9}, {nullptr, 64}}, filename));
    CORRADE_VERIFY(!Utility::Directory::exists(filename));
    CORRADE_COMPARE(out.str(), "");
}

void AbstractImageConverterTest::exportCompressedToFileThroughDataNotWritable() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressedData; }

        Containers::Array<char> doExportToData(const CompressedImageView2D& image) override {
            return Containers::array({char(image.size().x()), char(image.size().y())});
        };
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.exportToFile(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 8}, {nullptr, 64}}, "/some/path/that/does/not/exist");
    CORRADE_COMPARE(out.str(),
        "Utility::Directory::write(): can't open /some/path/that/does/not/exist\n"
        "Trade::AbstractImageConverter::exportToFile(): cannot write to file /some/path/that/does/not/exist\n");
}

void AbstractImageConverterTest::exportCompressedToFileNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressedFile; }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.exportToFile(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 8}, {nullptr, 64}}, Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::exportToFile(): feature advertised but not implemented\n");
}

void AbstractImageConverterTest::exportImageDataToFile() {
    ImageDataConverter converter;

    {
        /* Should get "B" when converting uncompressed */
        ImageData2D image{PixelFormat::RGBA16F, {}, nullptr};
        CORRADE_VERIFY(converter.exportToFile(image, Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out")));
        CORRADE_COMPARE_AS(Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out"),
            "B", TestSuite::Compare::FileToString);
    } {
        /* Should get "C" when converting compressed */
        ImageData2D image{CompressedPixelFormat::Bc2RGBAUnorm, {}, nullptr};
        CORRADE_VERIFY(converter.exportToFile(image, Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out")));
        CORRADE_COMPARE_AS(Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out"),
            "C", TestSuite::Compare::FileToString);
    }
}

void AbstractImageConverterTest::debugFeature() {
    std::ostringstream out;

    Debug{&out} << ImageConverterFeature::ConvertCompressedImage << ImageConverterFeature(0xf0);
    CORRADE_COMPARE(out.str(), "Trade::ImageConverterFeature::ConvertCompressedImage Trade::ImageConverterFeature(0xf0)\n");
}

void AbstractImageConverterTest::debugFeatures() {
    std::ostringstream out;

    Debug{&out} << (ImageConverterFeature::ConvertData|ImageConverterFeature::ConvertCompressedFile) << ImageConverterFeatures{};
    CORRADE_COMPARE(out.str(), "Trade::ImageConverterFeature::ConvertData|Trade::ImageConverterFeature::ConvertCompressedFile Trade::ImageConverterFeatures{}\n");
}

void AbstractImageConverterTest::debugFlag() {
    std::ostringstream out;

    Debug{&out} << ImageConverterFlag::Verbose << ImageConverterFlag(0xf0);
    CORRADE_COMPARE(out.str(), "Trade::ImageConverterFlag::Verbose Trade::ImageConverterFlag(0xf0)\n");
}

void AbstractImageConverterTest::debugFlags() {
    std::ostringstream out;

    Debug{&out} << (ImageConverterFlag::Verbose|ImageConverterFlag(0xf0)) << ImageConverterFlags{};
    CORRADE_COMPARE(out.str(), "Trade::ImageConverterFlag::Verbose|Trade::ImageConverterFlag(0xf0) Trade::ImageConverterFlags{}\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AbstractImageConverterTest)
