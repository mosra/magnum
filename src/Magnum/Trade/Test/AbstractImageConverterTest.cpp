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

class AbstractImageConverterTest: public TestSuite::Tester {
    public:
        explicit AbstractImageConverterTest();

        void construct();
        void constructWithPluginManagerReference();

        void exportToImage();
        void exportToImageNotSupported();
        void exportToImageNotImplemented();
        void exportToImageCustomDeleter();

        void exportToCompressedImage();
        void exportToCompressedImageNotSupported();
        void exportToCompressedImageNotImplemented();
        void exportToCompressedImageCustomDeleter();

        void exportToData();
        void exportToDataNotSupported();
        void exportToDataNotImplemented();
        void exportToDataCustomDeleter();

        void exportCompressedToData();
        void exportCompressedToDataNotSupported();
        void exportCompressedToDataNotImplemented();
        void exportCompressedToDataCustomDeleter();

        void exportImageDataToData();

        void exportToFile();
        void exportToFileThroughData();
        void exportToFileThroughDataNotWritable();
        void exportToFileNotSupported();
        void exportToFileNotImplemented();

        void exportCompressedToFile();
        void exportCompressedToFileThroughData();
        void exportCompressedToFileThroughDataNotWritable();
        void exportCompressedToFileNotSupported();
        void exportCompressedToFileNotImplemented();

        void exportImageDataToFile();

        void debugFeature();
        void debugFeatures();
};

AbstractImageConverterTest::AbstractImageConverterTest() {
    addTests({&AbstractImageConverterTest::construct,
              &AbstractImageConverterTest::constructWithPluginManagerReference,

              &AbstractImageConverterTest::exportToImage,
              &AbstractImageConverterTest::exportToImageNotSupported,
              &AbstractImageConverterTest::exportToImageNotImplemented,
              &AbstractImageConverterTest::exportToImageCustomDeleter,

              &AbstractImageConverterTest::exportToCompressedImage,
              &AbstractImageConverterTest::exportToCompressedImageNotSupported,
              &AbstractImageConverterTest::exportToCompressedImageNotImplemented,
              &AbstractImageConverterTest::exportToCompressedImageCustomDeleter,

              &AbstractImageConverterTest::exportToData,
              &AbstractImageConverterTest::exportToDataNotSupported,
              &AbstractImageConverterTest::exportToDataNotImplemented,
              &AbstractImageConverterTest::exportToDataCustomDeleter,

              &AbstractImageConverterTest::exportCompressedToData,
              &AbstractImageConverterTest::exportCompressedToDataNotSupported,
              &AbstractImageConverterTest::exportCompressedToDataNotImplemented,
              &AbstractImageConverterTest::exportCompressedToDataCustomDeleter,

              &AbstractImageConverterTest::exportImageDataToData,

              &AbstractImageConverterTest::exportToFile,
              &AbstractImageConverterTest::exportToFileThroughData,
              &AbstractImageConverterTest::exportToFileThroughDataNotWritable,
              &AbstractImageConverterTest::exportToFileNotSupported,
              &AbstractImageConverterTest::exportToFileNotImplemented,

              &AbstractImageConverterTest::exportCompressedToFile,
              &AbstractImageConverterTest::exportCompressedToFileThroughData,
              &AbstractImageConverterTest::exportCompressedToFileThroughDataNotWritable,
              &AbstractImageConverterTest::exportCompressedToFileNotSupported,
              &AbstractImageConverterTest::exportCompressedToFileNotImplemented,

              &AbstractImageConverterTest::exportImageDataToFile,

              &AbstractImageConverterTest::debugFeature,
              &AbstractImageConverterTest::debugFeatures});

    /* Create testing dir */
    Utility::Directory::mkpath(TRADE_TEST_OUTPUT_DIR);
}

void AbstractImageConverterTest::construct() {
    class Converter: public AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return {}; }
    };

    Converter converter;
    CORRADE_COMPARE(converter.features(), ImageConverterFeatures{});
}

void AbstractImageConverterTest::constructWithPluginManagerReference() {
    class Converter: public AbstractImageConverter {
        public:
            explicit Converter(PluginManager::Manager<AbstractImageConverter>& manager): AbstractImageConverter{manager} {}

        private:
            ImageConverterFeatures doFeatures() const override { return {}; }
    };

    PluginManager::Manager<AbstractImageConverter> manager;
    Converter converter{manager};
    CORRADE_COMPARE(converter.features(), ImageConverterFeatures{});
}

void AbstractImageConverterTest::exportToImage() {
    class Converter: public AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertImage; }
        Containers::Optional<Image2D> doExportToImage(const ImageView2D& image) override {
            return Image2D{PixelFormat::RGBA8Unorm, image.size(), Containers::Array<char>{96}};
        }
    };

    Converter converter;
    Containers::Optional<Image2D> actual = converter.exportToImage(ImageView2D{PixelFormat::R8Unorm, {4, 6}, Containers::ArrayView<char>{nullptr, 24}});
    CORRADE_VERIFY(actual);
    CORRADE_COMPARE(actual->data().size(), 96);
    CORRADE_COMPARE(actual->size(), (Vector2i{4, 6}));
}

void AbstractImageConverterTest::exportToImageNotSupported() {
    class Converter: public AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return {}; }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Converter converter;
    converter.exportToImage(ImageView2D{PixelFormat::R8Unorm, {4, 6}, Containers::ArrayView<char>{nullptr, 24}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::exportToImage(): feature not supported\n");
}

void AbstractImageConverterTest::exportToImageNotImplemented() {
    class Converter: public AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertImage; }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Converter converter;
    converter.exportToImage(ImageView2D{PixelFormat::R8Unorm, {4, 6}, Containers::ArrayView<char>{nullptr, 128}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::exportToImage(): feature advertised but not implemented\n");
}

void AbstractImageConverterTest::exportToImageCustomDeleter() {
    class Converter: public AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertImage; }
        Containers::Optional<Image2D> doExportToImage(const ImageView2D&) override {
            return Image2D{PixelFormat::RGBA8Unorm, {}, Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}}};
        }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Converter converter;
    converter.exportToImage(ImageView2D{PixelFormat::R8Unorm, {}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::exportToImage(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImageConverterTest::exportToCompressedImage() {
    class Converter: public AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressedImage; }
        Containers::Optional<CompressedImage2D> doExportToCompressedImage(const ImageView2D& image) override {
            return CompressedImage2D{CompressedPixelFormat::Bc1RGBAUnorm, image.size(), Containers::Array<char>{64}};
        }
    };

    Converter converter;
    Containers::Optional<CompressedImage2D> actual = converter.exportToCompressedImage(ImageView2D{PixelFormat::R8Unorm, {16, 8}, Containers::ArrayView<char>{nullptr, 128}});
    CORRADE_VERIFY(actual);
    CORRADE_COMPARE(actual->data().size(), 64);
    CORRADE_COMPARE(actual->size(), (Vector2i{16, 8}));
}

void AbstractImageConverterTest::exportToCompressedImageNotSupported() {
    class Converter: public AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return {}; }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Converter converter;
    converter.exportToCompressedImage(ImageView2D{PixelFormat::R8Unorm, {16, 8}, Containers::ArrayView<char>{nullptr, 128}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::exportToCompressedImage(): feature not supported\n");
}

void AbstractImageConverterTest::exportToCompressedImageNotImplemented() {
    class Converter: public AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressedImage; }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Converter converter;
    converter.exportToCompressedImage(ImageView2D{PixelFormat::R8Unorm, {16, 8}, Containers::ArrayView<char>{nullptr, 128}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::exportToCompressedImage(): feature advertised but not implemented\n");
}

void AbstractImageConverterTest::exportToCompressedImageCustomDeleter() {
    class Converter: public AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressedImage; }
        Containers::Optional<CompressedImage2D> doExportToCompressedImage(const ImageView2D&) override {
            return CompressedImage2D{CompressedPixelFormat::Bc1RGBAUnorm, {}, Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}}};
        }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Converter converter;
    converter.exportToCompressedImage(ImageView2D{PixelFormat::R8Unorm, {}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::exportToCompressedImage(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImageConverterTest::exportToData() {
    class Converter: public AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertData; }
        Containers::Array<char> doExportToData(const ImageView2D& image) override {
            return Containers::Array<char>{nullptr, std::size_t(image.size().product())};
        }
    };

    Converter converter;
    Containers::Array<char> actual = converter.exportToData(ImageView2D{PixelFormat::RGBA8Unorm, {4, 6}, Containers::ArrayView<char>{nullptr, 96}});
    CORRADE_COMPARE(actual.size(), 24);
}

void AbstractImageConverterTest::exportToDataNotSupported() {
    class Converter: public AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return {}; }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Converter converter;
    converter.exportToData(ImageView2D{PixelFormat::RGBA8Unorm, {4, 6}, Containers::ArrayView<char>{nullptr, 96}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::exportToData(): feature not supported\n");
}

void AbstractImageConverterTest::exportToDataNotImplemented() {
    class Converter: public AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertData; }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Converter converter;
    converter.exportToData(ImageView2D{PixelFormat::RGBA8Unorm, {4, 6}, Containers::ArrayView<char>{nullptr, 96}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::exportToData(): feature advertised but not implemented\n");
}

void AbstractImageConverterTest::exportToDataCustomDeleter() {
    class Converter: public AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertData; }
        Containers::Array<char> doExportToData(const ImageView2D&) override {
            return Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}};
        }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Converter converter;
    converter.exportToData(ImageView2D{PixelFormat::RGBA8Unorm, {}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::exportToData(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImageConverterTest::exportCompressedToData() {
    class Converter: public AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressedData; }
        Containers::Array<char> doExportToData(const CompressedImageView2D& image) override {
            return Containers::Array<char>{nullptr, std::size_t(image.size().product())};
        }
    };

    Converter converter;
    Containers::Array<char> actual = converter.exportToData(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 8}, Containers::ArrayView<char>{nullptr, 64}});
    CORRADE_COMPARE(actual.size(), 128);
}

void AbstractImageConverterTest::exportCompressedToDataNotSupported() {
    class Converter: public AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return {}; }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Converter converter;
    converter.exportToData(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 8}, Containers::ArrayView<char>{nullptr, 64}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::exportToData(): feature not supported\n");
}

void AbstractImageConverterTest::exportCompressedToDataNotImplemented() {
    class Converter: public AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertData; }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Converter converter;
    converter.exportToData(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 8}, Containers::ArrayView<char>{nullptr, 64}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::exportToData(): feature advertised but not implemented\n");
}

void AbstractImageConverterTest::exportCompressedToDataCustomDeleter() {
    class Converter: public AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressedData; }
        Containers::Array<char> doExportToData(const CompressedImageView2D&) override {
            return Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}};
        }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Converter converter;
    converter.exportToData(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::exportToData(): implementation is not allowed to use a custom Array deleter\n");
}

class ImageDataExporter: public Trade::AbstractImageConverter {
    private:
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertData|ImageConverterFeature::ConvertCompressedData; }

        Containers::Array<char> doExportToData(const ImageView2D&) override {
            /* DirectInit / InPlaceInit is unfortunately causing a custom
               deleter right now */
            /** @todo clean up when fixed */
            Containers::Array<char> out{1};
            out[0] = 'B';
            return out;
        };

        Containers::Array<char> doExportToData(const CompressedImageView2D&) override {
            /* DirectInit / InPlaceInit is unfortunately causing a custom
               deleter right now */
            /** @todo clean up when fixed */
            Containers::Array<char> out{1};
            out[0] = 'C';
            return out;
        };
};

void AbstractImageConverterTest::exportImageDataToData() {
    ImageDataExporter exporter;

    {
        /* Should get "B" when converting uncompressed */
        ImageData2D image{PixelFormat::RGBA8Unorm, {}, nullptr};
        CORRADE_COMPARE_AS(exporter.exportToData(image),
            (Containers::Array<char>{Containers::InPlaceInit, {'B'}}),
            TestSuite::Compare::Container);
    } {
        /* Should get "C" when converting compressed */
        ImageData2D image{CompressedPixelFormat::Bc1RGBUnorm, {}, nullptr};
        CORRADE_COMPARE_AS(exporter.exportToData(image),
            (Containers::Array<char>{Containers::InPlaceInit, {'C'}}),
            TestSuite::Compare::Container);
    }
}

void AbstractImageConverterTest::exportToFile() {
    class Converter: public AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertData; }
        bool doExportToFile(const ImageView2D& image, const std::string& filename) override {
            return Utility::Directory::write(filename, Containers::Array<char>{Containers::InPlaceInit,
                {char(image.size().x()), char(image.size().y())}});
        }
    };

    /* Remove previous file */
    Utility::Directory::rm(Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out"));

    Converter converter;
    CORRADE_VERIFY(converter.exportToFile(ImageView2D{PixelFormat::RGBA8Unorm, {0xf0, 0x0d}, {nullptr, 0xf0*0x0d*4}}, Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out")));
    CORRADE_COMPARE_AS(Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out"),
        "\xf0\x0d", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::exportToFileThroughData() {
    class DataExporter: public AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertData; }

        Containers::Array<char> doExportToData(const ImageView2D& image) override {
            return Containers::Array<char>{Containers::InPlaceInit,
                {char(image.size().x()), char(image.size().y())}};
        };
    };

    /* Remove previous file */
    Utility::Directory::rm(Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out"));

    /* doExportToFile() should call doExportToData() */
    DataExporter exporter;
    ImageView2D image(PixelFormat::RGBA8Unorm, {0xfe, 0xed}, {nullptr, 0xfe*0xed*4});
    CORRADE_VERIFY(exporter.exportToFile(image, Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out")));
    CORRADE_COMPARE_AS(Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out"),
        "\xfe\xed", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::exportToFileThroughDataNotWritable() {
    class Converter: public AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertData; }

        Containers::Array<char> doExportToData(const ImageView2D& image) override {
            return Containers::Array<char>{Containers::InPlaceInit,
                {char(image.size().x()), char(image.size().y())}};
        };
    };

    std::ostringstream out;
    Error redirectError{&out};

    Converter converter;
    converter.exportToFile(ImageView2D{PixelFormat::RGBA8Unorm, {0xfe, 0xed}, {nullptr, 0xfe*0xed*4}}, "/some/path/that/does/not/exist");
    CORRADE_COMPARE(out.str(),
        "Utility::Directory::write(): can't open /some/path/that/does/not/exist\n"
        "Trade::AbstractImageConverter::exportToFile(): cannot write to file /some/path/that/does/not/exist\n");
}

void AbstractImageConverterTest::exportToFileNotSupported() {
    class Converter: public AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return {}; }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Converter converter;
    converter.exportToFile(ImageView2D{PixelFormat::RGBA8Unorm, {4, 6}, {nullptr, 96}}, Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::exportToFile(): feature not supported\n");
}

void AbstractImageConverterTest::exportToFileNotImplemented() {
    class Converter: public AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertFile; }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Converter converter;
    converter.exportToFile(ImageView2D{PixelFormat::RGBA8Unorm, {4, 6}, {nullptr, 96}}, Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::exportToFile(): feature advertised but not implemented\n");
}

void AbstractImageConverterTest::exportCompressedToFile() {
    class Converter: public AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressedFile; }
        bool doExportToFile(const CompressedImageView2D& image, const std::string& filename) override {
            return Utility::Directory::write(filename, Containers::Array<char>{Containers::InPlaceInit,
                {char(image.size().x()), char(image.size().y())}});
        }
    };

    Converter converter;
    CORRADE_VERIFY(converter.exportToFile(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {0xd0, 0x0d}, {nullptr, 64}}, Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out")));
    CORRADE_COMPARE_AS(Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out"),
        "\xd0\x0d", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::exportCompressedToFileThroughData() {
    class DataExporter: public AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressedData; }

        Containers::Array<char> doExportToData(const CompressedImageView2D& image) override {
            return Containers::Array<char>{Containers::InPlaceInit,
                {char(image.size().x()), char(image.size().y())}};
        };
    };

    /* Remove previous file */
    Utility::Directory::rm(Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out"));

    /* doExportToFile() should call doExportToData() */
    DataExporter exporter;
    CORRADE_VERIFY(exporter.exportToFile(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {0xb0, 0xd9}, {nullptr, 64}}, Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out")));
    CORRADE_COMPARE_AS(Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out"),
        "\xb0\xd9", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::exportCompressedToFileThroughDataNotWritable() {
    class Converter: public AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressedData; }

        Containers::Array<char> doExportToData(const CompressedImageView2D& image) override {
            return Containers::Array<char>{Containers::InPlaceInit,
                {char(image.size().x()), char(image.size().y())}};
        };
    };

    std::ostringstream out;
    Error redirectError{&out};

    Converter converter;
    converter.exportToFile(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 8}, {nullptr, 64}}, "/some/path/that/does/not/exist");
    CORRADE_COMPARE(out.str(),
        "Utility::Directory::write(): can't open /some/path/that/does/not/exist\n"
        "Trade::AbstractImageConverter::exportToFile(): cannot write to file /some/path/that/does/not/exist\n");
}

void AbstractImageConverterTest::exportCompressedToFileNotSupported() {
    class Converter: public AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return {}; }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Converter converter;
    converter.exportToFile(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 8}, {nullptr, 64}}, Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::exportToFile(): feature not supported\n");
}

void AbstractImageConverterTest::exportCompressedToFileNotImplemented() {
    class Converter: public AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressedFile; }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Converter converter;
    converter.exportToFile(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 8}, {nullptr, 64}}, Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out.str(), "Trade::AbstractImageConverter::exportToFile(): feature advertised but not implemented\n");
}

void AbstractImageConverterTest::exportImageDataToFile() {
    ImageDataExporter exporter;

    {
        /* Should get "B" when converting uncompressed */
        ImageData2D image{PixelFormat::RGBA16F, {}, nullptr};
        CORRADE_VERIFY(exporter.exportToFile(image, Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out")));
        CORRADE_COMPARE_AS(Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out"),
            "B", TestSuite::Compare::FileToString);
    } {
        /* Should get "C" when converting compressed */
        ImageData2D image{CompressedPixelFormat::Bc2RGBAUnorm, {}, nullptr};
        CORRADE_VERIFY(exporter.exportToFile(image, Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out")));
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

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AbstractImageConverterTest)
