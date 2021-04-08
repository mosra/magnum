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
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/Containers/StringStl.h>
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

    void convert2D();
    void convert2DNotImplemented();
    void convert2DCustomDeleter();

    void convertCompressed2D();
    void convertCompressed2DNotImplemented();
    void convertCompressed2DCustomDeleter();

    void convertImageData2D();

    void convert2DToData();
    void convert2DToDataNotImplemented();
    void convert2DToDataCustomDeleter();

    void convertCompressed2DToData();
    void convertCompressed2DToDataNotImplemented();
    void convertCompressed2DToDataCustomDeleter();

    void convertImageData2DToData();

    void convert2DToFile();
    void convert2DToFileThroughData();
    void convert2DToFileThroughDataFailed();
    void convert2DToFileThroughDataNotWritable();
    void convert2DToFileNotImplemented();

    void convertCompressed2DToFile();
    void convertCompressed2DToFileThroughData();
    void convertCompressed2DToFileThroughDataFailed();
    void convertCompressed2DToFileThroughDataNotWritable();
    void convertCompressed2DToFileNotImplemented();

    void convertImageData2DToFile();

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

              &AbstractImageConverterTest::convert2D,
              &AbstractImageConverterTest::convert2DNotImplemented,
              &AbstractImageConverterTest::convert2DCustomDeleter,

              &AbstractImageConverterTest::convertCompressed2D,
              &AbstractImageConverterTest::convertCompressed2DNotImplemented,
              &AbstractImageConverterTest::convertCompressed2DCustomDeleter,

              &AbstractImageConverterTest::convertImageData2D,

              &AbstractImageConverterTest::convert2DToData,
              &AbstractImageConverterTest::convert2DToDataNotImplemented,
              &AbstractImageConverterTest::convert2DToDataCustomDeleter,

              &AbstractImageConverterTest::convertCompressed2DToData,
              &AbstractImageConverterTest::convertCompressed2DToDataNotImplemented,
              &AbstractImageConverterTest::convertCompressed2DToDataCustomDeleter,

              &AbstractImageConverterTest::convertImageData2DToData,

              &AbstractImageConverterTest::convert2DToFile,
              &AbstractImageConverterTest::convert2DToFileThroughData,
              &AbstractImageConverterTest::convert2DToFileThroughDataFailed,
              &AbstractImageConverterTest::convert2DToFileThroughDataNotWritable,
              &AbstractImageConverterTest::convert2DToFileNotImplemented,

              &AbstractImageConverterTest::convertCompressed2DToFile,
              &AbstractImageConverterTest::convertCompressed2DToFileThroughData,
              &AbstractImageConverterTest::convertCompressed2DToFileThroughDataFailed,
              &AbstractImageConverterTest::convertCompressed2DToFileThroughDataNotWritable,
              &AbstractImageConverterTest::convertCompressed2DToFileNotImplemented,

              &AbstractImageConverterTest::convertImageData2DToFile,

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

    /** @todo use a real flag when we have more than one */
    converter.addFlags(ImageConverterFlag(4));
    CORRADE_COMPARE(converter.flags(), ImageConverterFlag::Verbose|ImageConverterFlag(4));
    CORRADE_COMPARE(converter._flags, ImageConverterFlag::Verbose|ImageConverterFlag(4));

    converter.clearFlags(ImageConverterFlag::Verbose);
    CORRADE_COMPARE(converter.flags(), ImageConverterFlag(4));
    CORRADE_COMPARE(converter._flags, ImageConverterFlag(4));
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
    converter.convert(ImageView2D{PixelFormat::R8Unorm, {4, 6}, Containers::ArrayView<char>{nullptr, 24}});
    converter.convert(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 8}, {nullptr, 64}});
    converter.convertToData(ImageView2D{PixelFormat::RGBA8Unorm, {4, 6}, Containers::ArrayView<char>{nullptr, 96}});
    converter.convertToData(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 8}, Containers::ArrayView<char>{nullptr, 64}});
    converter.convertToFile(ImageView2D{PixelFormat::RGBA8Unorm, {4, 6}, {nullptr, 96}}, Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    converter.convertToFile(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 8}, {nullptr, 64}}, Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImageConverter::convert(): 2D image conversion not supported\n"
        "Trade::AbstractImageConverter::convert(): compressed 2D image conversion not supported\n"
        "Trade::AbstractImageConverter::convertToData(): 2D image conversion not supported\n"
        "Trade::AbstractImageConverter::convertToData(): compressed 2D image conversion not supported\n"
        "Trade::AbstractImageConverter::convertToFile(): 2D image conversion not supported\n"
        "Trade::AbstractImageConverter::convertToFile(): compressed 2D image conversion not supported\n");
}

void AbstractImageConverterTest::convert2D() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert2D; }
        Containers::Optional<ImageData2D> doConvert(const ImageView2D& image) override {
            return ImageData2D{PixelFormat::RGBA8Unorm, image.size(), Containers::Array<char>{96}};
        }
    } converter;

    Containers::Optional<ImageData2D> actual = converter.convert(ImageView2D{PixelFormat::R8Unorm, {4, 6}, Containers::ArrayView<char>{nullptr, 24}});
    CORRADE_VERIFY(actual);
    CORRADE_VERIFY(!actual->isCompressed());
    CORRADE_COMPARE(actual->data().size(), 96);
    CORRADE_COMPARE(actual->size(), (Vector2i{4, 6}));
}

void AbstractImageConverterTest::convert2DNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert2D; }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convert(ImageView2D{PixelFormat::R8Unorm, {4, 6}, Containers::ArrayView<char>{nullptr, 128}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::convert(): 2D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convert2DCustomDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert2D; }
        Containers::Optional<ImageData2D> doConvert(const ImageView2D&) override {
            return ImageData2D{PixelFormat::RGBA8Unorm, {}, Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}}};
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convert(ImageView2D{PixelFormat::R8Unorm, {}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::convert(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImageConverterTest::convertCompressed2D() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed2D; }
        Containers::Optional<ImageData2D> doConvert(const CompressedImageView2D& image) override {
            return ImageData2D{image.format(), image.size(), Containers::Array<char>{64}};
        }
    } converter;

    Containers::Optional<ImageData2D> actual = converter.convert(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 8}, Containers::ArrayView<char>{nullptr, 128}});
    CORRADE_VERIFY(actual);
    CORRADE_VERIFY(actual->isCompressed());
    CORRADE_COMPARE(actual->data().size(), 64);
    CORRADE_COMPARE(actual->size(), (Vector2i{16, 8}));
}

void AbstractImageConverterTest::convertCompressed2DNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed2D; }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convert(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 8}, Containers::ArrayView<char>{nullptr, 128}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::convert(): compressed 2D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convertCompressed2DCustomDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed2D; }
        Containers::Optional<ImageData2D> doConvert(const CompressedImageView2D&) override {
            return ImageData2D{CompressedPixelFormat::Bc1RGBAUnorm, {}, Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}}};
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convert(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::convert(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImageConverterTest::convertImageData2D() {
    struct: Trade::AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert2D|ImageConverterFeature::ConvertCompressed2D; }

        Containers::Optional<ImageData2D> doConvert(const ImageView2D&) override {
            return ImageData2D{PixelFormat::R8Unorm, {}, Containers::array({'B'})};
        };

        Containers::Optional<ImageData2D> doConvert(const CompressedImageView2D&) override {
            return ImageData2D{PixelFormat::R8Unorm, {}, Containers::array({'C'})};
        };
    } converter;

    {
        /* Should get "B" when converting uncompressed */
        ImageData2D image{PixelFormat::RGBA8Unorm, {}, nullptr};
        Containers::Optional<ImageData2D> out = converter.convert(image);
        CORRADE_VERIFY(out);
        CORRADE_COMPARE_AS(out->data(),
            Containers::arrayView<char>({'B'}),
            TestSuite::Compare::Container);
    } {
        /* Should get "C" when converting compressed */
        ImageData2D image{CompressedPixelFormat::Bc1RGBUnorm, {}, nullptr};
        Containers::Optional<ImageData2D> out = converter.convert(image);
        CORRADE_VERIFY(out);
        CORRADE_COMPARE_AS(out->data(),
            Containers::arrayView<char>({'C'}),
            TestSuite::Compare::Container);
    }
}

void AbstractImageConverterTest::convert2DToData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert2DToData; }
        Containers::Array<char> doConvertToData(const ImageView2D& image) override {
            return Containers::Array<char>{nullptr, std::size_t(image.size().product())};
        }
    } converter;

    Containers::Array<char> actual = converter.convertToData(ImageView2D{PixelFormat::RGBA8Unorm, {4, 6}, Containers::ArrayView<char>{nullptr, 96}});
    CORRADE_COMPARE(actual.size(), 24);
}

void AbstractImageConverterTest::convert2DToDataNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert2DToData; }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convertToData(ImageView2D{PixelFormat::RGBA8Unorm, {4, 6}, Containers::ArrayView<char>{nullptr, 96}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::convertToData(): 2D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convert2DToDataCustomDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert2DToData; }
        Containers::Array<char> doConvertToData(const ImageView2D&) override {
            return Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}};
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convertToData(ImageView2D{PixelFormat::RGBA8Unorm, {}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImageConverterTest::convertCompressed2DToData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed2DToData; }
        Containers::Array<char> doConvertToData(const CompressedImageView2D& image) override {
            return Containers::Array<char>{nullptr, std::size_t(image.size().product())};
        }
    } converter;

    Containers::Array<char> actual = converter.convertToData(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 8}, Containers::ArrayView<char>{nullptr, 64}});
    CORRADE_COMPARE(actual.size(), 128);
}

void AbstractImageConverterTest::convertCompressed2DToDataNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed2DToData; }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convertToData(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 8}, Containers::ArrayView<char>{nullptr, 64}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::convertToData(): compressed 2D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convertCompressed2DToDataCustomDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed2DToData; }
        Containers::Array<char> doConvertToData(const CompressedImageView2D&) override {
            return Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}};
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convertToData(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter\n");
}

/* Used by convertImageDataToData() and convertImageDataToFile() */
class ImageData2DConverter: public Trade::AbstractImageConverter {
    private:
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert2DToData|ImageConverterFeature::ConvertCompressed2DToData; }

        Containers::Array<char> doConvertToData(const ImageView2D&) override {
            return Containers::array({'B'});
        };

        Containers::Array<char> doConvertToData(const CompressedImageView2D&) override {
            return Containers::array({'C'});
        };
};

void AbstractImageConverterTest::convertImageData2DToData() {
    ImageData2DConverter converter;

    /* Should get "B" when converting uncompressed */
    CORRADE_COMPARE_AS(converter.convertToData(ImageData2D{PixelFormat::RGBA8Unorm, {}, nullptr}),
        Containers::arrayView({'B'}),
        TestSuite::Compare::Container);

    /* Should get "C" when converting compressed */
    CORRADE_COMPARE_AS(converter.convertToData(ImageData2D{CompressedPixelFormat::Bc1RGBUnorm, {}, nullptr}),
        Containers::arrayView({'C'}),
        TestSuite::Compare::Container);
}

void AbstractImageConverterTest::convert2DToFile() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert2DToFile; }
        bool doConvertToFile(const ImageView2D& image, Containers::StringView filename) override {
            return Utility::Directory::write(filename, Containers::arrayView(
                {char(image.size().x()), char(image.size().y())}));
        }
    } converter;

    const std::string filename = Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out");

    /* Remove previous file, if any */
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));

    CORRADE_VERIFY(converter.convertToFile(ImageView2D{PixelFormat::RGBA8Unorm, {0xf0, 0x0d}, {nullptr, 0xf0*0x0d*4}}, filename));
    CORRADE_COMPARE_AS(filename,
        "\xf0\x0d", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convert2DToFileThroughData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert2DToData; }

        Containers::Array<char> doConvertToData(const ImageView2D& image) override {
            return Containers::array({char(image.size().x()), char(image.size().y())});
        };
    } converter;

    const std::string filename = Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out");

    /* Remove previous file, if any */
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));

    /* doConvertToFile() should call doConvertToData() */
    CORRADE_VERIFY(converter.convertToFile(ImageView2D(PixelFormat::RGBA8Unorm, {0xfe, 0xed}, {nullptr, 0xfe*0xed*4}), filename));
    CORRADE_COMPARE_AS(filename,
        "\xfe\xed", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convert2DToFileThroughDataFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert2DToData; }

        Containers::Array<char> doConvertToData(const ImageView2D&) override {
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
    CORRADE_VERIFY(!converter.convertToFile(ImageView2D(PixelFormat::RGBA8Unorm, {0xfe, 0xed}, {nullptr, 0xfe*0xed*4}), filename));
    CORRADE_VERIFY(!Utility::Directory::exists(filename));
    CORRADE_COMPARE(out.str(), "");
}

void AbstractImageConverterTest::convert2DToFileThroughDataNotWritable() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert2DToData; }

        Containers::Array<char> doConvertToData(const ImageView2D& image) override {
            return Containers::array({char(image.size().x()), char(image.size().y())});
        };
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile(ImageView2D{PixelFormat::RGBA8Unorm, {0xfe, 0xed}, {nullptr, 0xfe*0xed*4}}, "/some/path/that/does/not/exist"));
    CORRADE_COMPARE(out.str(),
        "Utility::Directory::write(): can't open /some/path/that/does/not/exist\n"
        "Trade::AbstractImageConverter::convertToFile(): cannot write to file /some/path/that/does/not/exist\n");
}

void AbstractImageConverterTest::convert2DToFileNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert2DToFile; }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convertToFile(ImageView2D{PixelFormat::RGBA8Unorm, {4, 6}, {nullptr, 96}}, Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::convertToFile(): 2D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convertCompressed2DToFile() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed2DToFile; }
        bool doConvertToFile(const CompressedImageView2D& image, Containers::StringView filename) override {
            return Utility::Directory::write(filename, Containers::arrayView(
                {char(image.size().x()), char(image.size().y())}));
        }
    } converter;

    const std::string filename = Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out");

    /* Remove previous file, if any */
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));

    CORRADE_VERIFY(converter.convertToFile(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {0xd0, 0x0d}, {nullptr, 64}}, filename));
    CORRADE_COMPARE_AS(filename,
        "\xd0\x0d", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convertCompressed2DToFileThroughData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed2DToData; }

        Containers::Array<char> doConvertToData(const CompressedImageView2D& image) override {
            return Containers::array({char(image.size().x()), char(image.size().y())});
        };
    } converter;

    const std::string filename = Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out");

    /* Remove previous file, if any */
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));

    /* doConvertToFile() should call doConvertToData() */
    CORRADE_VERIFY(converter.convertToFile(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {0xb0, 0xd9}, {nullptr, 64}}, filename));
    CORRADE_COMPARE_AS(filename,
        "\xb0\xd9", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convertCompressed2DToFileThroughDataFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed2DToData; }

        Containers::Array<char> doConvertToData(const CompressedImageView2D&) override {
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
    CORRADE_VERIFY(!converter.convertToFile(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {0xb0, 0xd9}, {nullptr, 64}}, filename));
    CORRADE_VERIFY(!Utility::Directory::exists(filename));
    CORRADE_COMPARE(out.str(), "");
}

void AbstractImageConverterTest::convertCompressed2DToFileThroughDataNotWritable() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed2DToData; }

        Containers::Array<char> doConvertToData(const CompressedImageView2D& image) override {
            return Containers::array({char(image.size().x()), char(image.size().y())});
        };
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convertToFile(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 8}, {nullptr, 64}}, "/some/path/that/does/not/exist");
    CORRADE_COMPARE(out.str(),
        "Utility::Directory::write(): can't open /some/path/that/does/not/exist\n"
        "Trade::AbstractImageConverter::convertToFile(): cannot write to file /some/path/that/does/not/exist\n");
}

void AbstractImageConverterTest::convertCompressed2DToFileNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed2DToFile; }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convertToFile(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 8}, {nullptr, 64}}, Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::convertToFile(): compressed 2D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convertImageData2DToFile() {
    ImageData2DConverter converter;

    /* Should get "B" when converting uncompressed */
    CORRADE_VERIFY(converter.convertToFile(ImageData2D{PixelFormat::RGBA16F, {}, nullptr}, Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out")));
    CORRADE_COMPARE_AS(Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out"),
        "B", TestSuite::Compare::FileToString);

    /* Should get "C" when converting compressed */
    CORRADE_VERIFY(converter.convertToFile(ImageData2D{CompressedPixelFormat::Bc2RGBAUnorm, {}, nullptr}, Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out")));
    CORRADE_COMPARE_AS(Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out"),
        "C", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::debugFeature() {
    std::ostringstream out;

    Debug{&out} << ImageConverterFeature::ConvertCompressed2D << ImageConverterFeature(0xdeadbeef);
    CORRADE_COMPARE(out.str(), "Trade::ImageConverterFeature::ConvertCompressed2D Trade::ImageConverterFeature(0xdeadbeef)\n");
}

void AbstractImageConverterTest::debugFeatures() {
    std::ostringstream out;

    Debug{&out} << (ImageConverterFeature::Convert2DToData|ImageConverterFeature::ConvertCompressed2DToFile) << ImageConverterFeatures{};
    CORRADE_COMPARE(out.str(), "Trade::ImageConverterFeature::Convert2DToData|Trade::ImageConverterFeature::ConvertCompressed2DToFile Trade::ImageConverterFeatures{}\n");
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
