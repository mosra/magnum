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
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringStl.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/FileToString.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/FileCallback.h"
#include "Magnum/ShaderTools/AbstractConverter.h"

#include "configure.h"

namespace Magnum { namespace ShaderTools { namespace Test { namespace {

struct AbstractConverterTest: TestSuite::Tester {
    explicit AbstractConverterTest();

    void featuresNone();

    void setFlags();
    void setFlagsBothQuietAndVerbose();
    void setFlagsPreprocessNotSupported();
    void setFlagsPreprocessOnlyNotAllowed();
    void setFlagsNotImplemented();

    void setInputOutputFormat();

    void setDefinitions();
    void setDefinitionsNotSupported();
    void setDefinitionsNotImplemented();

    void setOptimizationLevel();
    void setOptimizationLevelNotSupported();
    void setOptimizationLevelNotImplemented();

    void setDebugInfoLevel();
    void setDebugInfoLevelNotSupported();
    void setDebugInfoLevelNotImplemented();

    void validateData();
    void validateDataNotSupported();
    void validateDataNotImplemented();
    void validateDataPreprocessOnly();
    void validateDataCustomStringDeleter();

    void validateFile();
    void validateFileAsData();
    void validateFileAsDataNotFound();
    void validateFileNotSupported();
    void validateFileNotImplemented();
    void validateFilePreprocessOnly();
    void validateFileCustomStringDeleter();

    void convertDataToData();
    void convertDataToDataNotSupported();
    void convertDataToDataNotImplemented();
    void convertDataToDataCustomDeleter();
    void convertDataToFileThroughData();
    void convertDataToFileThroughDataFailed();
    void convertDataToFileThroughDataNotWritable();
    void convertDataToFileNotSupported();
    void convertDataToFileNotImplemented();

    void convertFileToFile();
    void convertFileToFileThroughData();
    void convertFileToFileThroughDataNotFound();
    void convertFileToFileThroughDataFailed();
    void convertFileToFileThroughDataNotWritable();
    void convertFileToFileNotSupported();
    void convertFileToFileNotImplemented();
    void convertFileToData();
    void convertFileToDataAsData();
    void convertFileToDataAsDataNotFound();
    void convertFileToDataNotSupported();
    void convertFileToDataNotImplemented();
    void convertFileToDataCustomDeleter();

    void linkDataToData();
    void linkDataToDataNotSupported();
    void linkDataToDataNotImplemented();
    void linkDataToDataPreprocessOnly();
    void linkDataToDataNoData();
    void linkDataToDataCustomDeleter();
    void linkDataToFileThroughData();
    void linkDataToFileThroughDataFailed();
    void linkDataToFileThroughDataNotWritable();
    void linkDataToFileNotSupported();
    void linkDataToFileNotImplemented();
    void linkDataToFilePreprocessOnly();
    void linkDataToFileNoData();

    void linkFilesToFile();
    void linkFilesToFileThroughData();
    void linkFilesToFileThroughDataNotFound();
    void linkFilesToFileThroughDataFailed();
    void linkFilesToFileThroughDataNotWritable();
    void linkFilesToFileNotSupported();
    void linkFilesToFileNotImplemented();
    void linkFilesToFilePreprocessOnly();
    void linkFilesToFileNoFile();
    void linkFilesToData();
    void linkFilesToDataAsData();
    void linkFilesToDataAsDataNotFound();
    void linkFilesToDataNotSupported();
    void linkFilesToDataNotImplemented();
    void linkFilesToDataPreprocessOnly();
    void linkFilesToDataNoFile();
    void linkFilesToDataCustomDeleter();

    void setInputFileCallback();
    void setInputFileCallbackTemplate();
    void setInputFileCallbackTemplateNull();
    void setInputFileCallbackTemplateConst();
    void setInputFileCallbackNotImplemented();
    void setInputFileCallbackNotSupported();

    void setInputFileCallbackValidateFileDirectly();
    void setInputFileCallbackValidateFileThroughBaseImplementation();
    void setInputFileCallbackValidateFileThroughBaseImplementationFailed();
    void setInputFileCallbackValidateFileAsData();
    void setInputFileCallbackValidateFileAsDataFailed();

    void setInputFileCallbackConvertFileToFileDirectly();
    void setInputFileCallbackConvertFileToFileThroughBaseImplementation();
    void setInputFileCallbackConvertFileToFileThroughBaseImplementationFailed();
    void setInputFileCallbackConvertFileToFileAsData();
    void setInputFileCallbackConvertFileToFileAsDataFailed();
    void setInputFileCallbackConvertFileToFileAsDataNotWritable();

    void setInputFileCallbackConvertFileToDataDirectly();
    void setInputFileCallbackConvertFileToDataThroughBaseImplementation();
    void setInputFileCallbackConvertFileToDataThroughBaseImplementationFailed();
    void setInputFileCallbackConvertFileToDataAsData();
    void setInputFileCallbackConvertFileToDataAsDataFailed();

    void setInputFileCallbackLinkFilesToFileDirectly();
    void setInputFileCallbackLinkFilesToFileThroughBaseImplementation();
    void setInputFileCallbackLinkFilesToFileThroughBaseImplementationFailed();
    void setInputFileCallbackLinkFilesToFileAsData();
    void setInputFileCallbackLinkFilesToFileAsDataFailed();
    void setInputFileCallbackLinkFilesToFileAsDataNotWritable();

    void setInputFileCallbackLinkFilesToDataDirectly();
    void setInputFileCallbackLinkFilesToDataThroughBaseImplementation();
    void setInputFileCallbackLinkFilesToDataThroughBaseImplementationFailed();
    void setInputFileCallbackLinkFilesToDataAsData();
    void setInputFileCallbackLinkFilesToDataAsDataFailed();

    void debugFeature();
    void debugFeatures();
    void debugFlag();
    void debugFlags();
    void debugFormat();
    void debugStage();
};

AbstractConverterTest::AbstractConverterTest() {
    addTests({&AbstractConverterTest::featuresNone,

              &AbstractConverterTest::setFlags,
              &AbstractConverterTest::setFlagsBothQuietAndVerbose,
              &AbstractConverterTest::setFlagsPreprocessNotSupported,
              &AbstractConverterTest::setFlagsPreprocessOnlyNotAllowed,
              &AbstractConverterTest::setFlagsNotImplemented,

              &AbstractConverterTest::setInputOutputFormat,

              &AbstractConverterTest::setDefinitions,
              &AbstractConverterTest::setDefinitionsNotSupported,
              &AbstractConverterTest::setDefinitionsNotImplemented,

              &AbstractConverterTest::setOptimizationLevel,
              &AbstractConverterTest::setOptimizationLevelNotSupported,
              &AbstractConverterTest::setOptimizationLevelNotImplemented,

              &AbstractConverterTest::setDebugInfoLevel,
              &AbstractConverterTest::setDebugInfoLevelNotSupported,
              &AbstractConverterTest::setDebugInfoLevelNotImplemented,

              &AbstractConverterTest::validateData,
              &AbstractConverterTest::validateDataNotSupported,
              &AbstractConverterTest::validateDataNotImplemented,
              &AbstractConverterTest::validateDataPreprocessOnly,
              &AbstractConverterTest::validateDataCustomStringDeleter,

              &AbstractConverterTest::validateFile,
              &AbstractConverterTest::validateFileAsData,
              &AbstractConverterTest::validateFileAsDataNotFound,
              &AbstractConverterTest::validateFileNotSupported,
              &AbstractConverterTest::validateFileNotImplemented,
              &AbstractConverterTest::validateFilePreprocessOnly,
              &AbstractConverterTest::validateFileCustomStringDeleter,

              &AbstractConverterTest::convertDataToData,
              &AbstractConverterTest::convertDataToDataNotSupported,
              &AbstractConverterTest::convertDataToDataNotImplemented,
              &AbstractConverterTest::convertDataToDataCustomDeleter,
              &AbstractConverterTest::convertDataToFileThroughData,
              &AbstractConverterTest::convertDataToFileThroughDataFailed,
              &AbstractConverterTest::convertDataToFileThroughDataNotWritable,
              &AbstractConverterTest::convertDataToFileNotSupported,
              &AbstractConverterTest::convertDataToFileNotImplemented,

              &AbstractConverterTest::convertFileToFile,
              &AbstractConverterTest::convertFileToFileThroughData,
              &AbstractConverterTest::convertFileToFileThroughDataNotFound,
              &AbstractConverterTest::convertFileToFileThroughDataFailed,
              &AbstractConverterTest::convertFileToFileThroughDataNotWritable,
              &AbstractConverterTest::convertFileToFileNotSupported,
              &AbstractConverterTest::convertFileToFileNotImplemented,
              &AbstractConverterTest::convertFileToData,
              &AbstractConverterTest::convertFileToDataAsData,
              &AbstractConverterTest::convertFileToDataAsDataNotFound,
              &AbstractConverterTest::convertFileToDataNotSupported,
              &AbstractConverterTest::convertFileToDataNotImplemented,
              &AbstractConverterTest::convertFileToDataCustomDeleter,

              &AbstractConverterTest::linkDataToData,
              &AbstractConverterTest::linkDataToDataNotSupported,
              &AbstractConverterTest::linkDataToDataNotImplemented,
              &AbstractConverterTest::linkDataToDataPreprocessOnly,
              &AbstractConverterTest::linkDataToDataNoData,
              &AbstractConverterTest::linkDataToDataCustomDeleter,
              &AbstractConverterTest::linkDataToFileThroughData,
              &AbstractConverterTest::linkDataToFileThroughDataFailed,
              &AbstractConverterTest::linkDataToFileThroughDataNotWritable,
              &AbstractConverterTest::linkDataToFileNotSupported,
              &AbstractConverterTest::linkDataToFileNotImplemented,
              &AbstractConverterTest::linkDataToFilePreprocessOnly,
              &AbstractConverterTest::linkDataToFileNoData,

              &AbstractConverterTest::linkFilesToFile,
              &AbstractConverterTest::linkFilesToFileThroughData,
              &AbstractConverterTest::linkFilesToFileThroughDataNotFound,
              &AbstractConverterTest::linkFilesToFileThroughDataFailed,
              &AbstractConverterTest::linkFilesToFileThroughDataNotWritable,
              &AbstractConverterTest::linkFilesToFileNotSupported,
              &AbstractConverterTest::linkFilesToFileNotImplemented,
              &AbstractConverterTest::linkFilesToFilePreprocessOnly,
              &AbstractConverterTest::linkFilesToFileNoFile,
              &AbstractConverterTest::linkFilesToData,
              &AbstractConverterTest::linkFilesToDataAsData,
              &AbstractConverterTest::linkFilesToDataAsDataNotFound,
              &AbstractConverterTest::linkFilesToDataNotSupported,
              &AbstractConverterTest::linkFilesToDataNotImplemented,
              &AbstractConverterTest::linkFilesToDataPreprocessOnly,
              &AbstractConverterTest::linkFilesToDataNoFile,
              &AbstractConverterTest::linkFilesToDataCustomDeleter,

              &AbstractConverterTest::setInputFileCallback,
              &AbstractConverterTest::setInputFileCallbackTemplate,
              &AbstractConverterTest::setInputFileCallbackTemplateNull,
              &AbstractConverterTest::setInputFileCallbackTemplateConst,
              &AbstractConverterTest::setInputFileCallbackNotImplemented,
              &AbstractConverterTest::setInputFileCallbackNotSupported,

              &AbstractConverterTest::setInputFileCallbackValidateFileDirectly,
              &AbstractConverterTest::setInputFileCallbackValidateFileThroughBaseImplementation,
              &AbstractConverterTest::setInputFileCallbackValidateFileThroughBaseImplementationFailed,
              &AbstractConverterTest::setInputFileCallbackValidateFileAsData,
              &AbstractConverterTest::setInputFileCallbackValidateFileAsDataFailed,

              &AbstractConverterTest::setInputFileCallbackConvertFileToFileDirectly,
              &AbstractConverterTest::setInputFileCallbackConvertFileToFileThroughBaseImplementation,
              &AbstractConverterTest::setInputFileCallbackConvertFileToFileThroughBaseImplementationFailed,
              &AbstractConverterTest::setInputFileCallbackConvertFileToFileAsData,
              &AbstractConverterTest::setInputFileCallbackConvertFileToFileAsDataFailed,
              &AbstractConverterTest::setInputFileCallbackConvertFileToFileAsDataNotWritable,

              &AbstractConverterTest::setInputFileCallbackConvertFileToDataDirectly,
              &AbstractConverterTest::setInputFileCallbackConvertFileToDataThroughBaseImplementation,
              &AbstractConverterTest::setInputFileCallbackConvertFileToDataThroughBaseImplementationFailed,
              &AbstractConverterTest::setInputFileCallbackConvertFileToDataAsData,
              &AbstractConverterTest::setInputFileCallbackConvertFileToDataAsDataFailed,

              &AbstractConverterTest::setInputFileCallbackLinkFilesToFileDirectly,
              &AbstractConverterTest::setInputFileCallbackLinkFilesToFileThroughBaseImplementation,
              &AbstractConverterTest::setInputFileCallbackLinkFilesToFileThroughBaseImplementationFailed,
              &AbstractConverterTest::setInputFileCallbackLinkFilesToFileAsData,
              &AbstractConverterTest::setInputFileCallbackLinkFilesToFileAsDataFailed,
              &AbstractConverterTest::setInputFileCallbackLinkFilesToFileAsDataNotWritable,

              &AbstractConverterTest::setInputFileCallbackLinkFilesToDataDirectly,
              &AbstractConverterTest::setInputFileCallbackLinkFilesToDataThroughBaseImplementation,
              &AbstractConverterTest::setInputFileCallbackLinkFilesToDataThroughBaseImplementationFailed,
              &AbstractConverterTest::setInputFileCallbackLinkFilesToDataAsData,
              &AbstractConverterTest::setInputFileCallbackLinkFilesToDataAsDataFailed,

              &AbstractConverterTest::debugFeature,
              &AbstractConverterTest::debugFeatures,
              &AbstractConverterTest::debugFlag,
              &AbstractConverterTest::debugFlags,
              &AbstractConverterTest::debugFormat,
              &AbstractConverterTest::debugStage});

    /* Create testing dir */
    Utility::Directory::mkpath(SHADERTOOLS_TEST_OUTPUT_DIR);
}

void AbstractConverterTest::featuresNone() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            /* These aren't real features, so it should still complain */
            return ConverterFeature::InputFileCallback|ConverterFeature::Preprocess|ConverterFeature::Optimize|ConverterFeature::DebugInfo;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.features();
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::features(): implementation reported no features\n");
}

void AbstractConverterTest::setFlags() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            /* Assuming this bit is unused */
            return ConverterFeature(1 << 15);
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
        void doSetFlags(ConverterFlags flags) override {
            _flags = flags;
        }

        ConverterFlags _flags;
    } converter;

    CORRADE_COMPARE(converter.flags(), ConverterFlags{});
    CORRADE_COMPARE(converter._flags, ConverterFlags{});
    converter.setFlags(ConverterFlag::Verbose);
    CORRADE_COMPARE(converter.flags(), ConverterFlag::Verbose);
    CORRADE_COMPARE(converter._flags, ConverterFlag::Verbose);
}

void AbstractConverterTest::setFlagsBothQuietAndVerbose() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ValidateData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.setFlags(ConverterFlag::Quiet|ConverterFlag::Verbose);
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::setFlags(): can't have both Quiet and Verbose set\n");
}

void AbstractConverterTest::setFlagsPreprocessNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ValidateData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.setFlags(ConverterFlag::PreprocessOnly);
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::setFlags(): PreprocessOnly not supported by the implementation\n");
}

void AbstractConverterTest::setFlagsPreprocessOnlyNotAllowed() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            /** @todo should Validate/Convert be enforced when Preprocess is
                present? */
            return ConverterFeature::Preprocess|ConverterFeature::LinkData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    converter.setFlags(ConverterFlag::PreprocessOnly);

    std::ostringstream out;
    Error redirectError{&out};
    converter.linkDataToData({});
    converter.linkDataToFile({}, {});
    converter.linkFilesToFile({}, {});
    converter.linkFilesToData({});
    CORRADE_COMPARE(out.str(),
        "ShaderTools::AbstractConverter::linkDataToData(): PreprocessOnly is not allowed in combination with linking\n"
        "ShaderTools::AbstractConverter::linkDataToFile(): PreprocessOnly is not allowed in combination with linking\n"
        "ShaderTools::AbstractConverter::linkFilesToFile(): PreprocessOnly is not allowed in combination with linking\n"
        "ShaderTools::AbstractConverter::linkFilesToData(): PreprocessOnly is not allowed in combination with linking\n");
}

void AbstractConverterTest::setFlagsNotImplemented() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            /* Assuming this bit is unused */
            return ConverterFeature(1 << 15);
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    CORRADE_COMPARE(converter.flags(), ConverterFlags{});
    converter.setFlags(ConverterFlag::Verbose);
    CORRADE_COMPARE(converter.flags(), ConverterFlag::Verbose);
    /* Should just work, no need to implement the function */
}

void AbstractConverterTest::setInputOutputFormat() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData;
        }
        void doSetInputFormat(Format format, Containers::StringView version) override {
            inputFormat = format;
            inputVersion = version;
        }
        void doSetOutputFormat(Format format, Containers::StringView version) override {
            outputFormat = format;
            outputVersion = version;
        }

        Format inputFormat, outputFormat;
        Containers::StringView inputVersion, outputVersion;
    } converter;

    converter.setInputFormat(Format::Glsl, "4.5");
    converter.setOutputFormat(Format::SpirvAssembly, "1.5");
    CORRADE_COMPARE(converter.inputFormat, Format::Glsl);
    CORRADE_COMPARE(converter.inputVersion, "4.5");
    CORRADE_COMPARE(converter.outputFormat, Format::SpirvAssembly);
    CORRADE_COMPARE(converter.outputVersion, "1.5");

    converter.setInputFormat(Format::Msl);
    converter.setOutputFormat(Format::Dxil);
    CORRADE_COMPARE(converter.inputFormat, Format::Msl);
    CORRADE_COMPARE(converter.inputVersion, "");
    CORRADE_COMPARE(converter.outputFormat, Format::Dxil);
    CORRADE_COMPARE(converter.outputVersion, "");
}

void AbstractConverterTest::setDefinitions() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::Preprocess|ConverterFeature::ValidateData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        void doSetDefinitions(Containers::ArrayView<const std::pair<Containers::StringView, Containers::StringView>> definitions) override {
            howManyIsThere = definitions.size();
        }

        std::size_t howManyIsThere = 0;
    } converter;

    converter.setDefinitions({
        {"VULKAN", ""},
        {"LIGHT_COUNT", "3"},
        {"GL_ES", nullptr}
    });
    CORRADE_COMPARE(converter.howManyIsThere, 3);
}

void AbstractConverterTest::setDefinitionsNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ValidateData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.setDefinitions({});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::setDefinitions(): feature not supported\n");
}

void AbstractConverterTest::setDefinitionsNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::Preprocess|ConverterFeature::ValidateData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.setDefinitions({});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::setDefinitions(): feature advertised but not implemented\n");
}

void AbstractConverterTest::setOptimizationLevel() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::Optimize|ConverterFeature::ValidateData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        void doSetOptimizationLevel(Containers::StringView level) override {
            optimization = level;
        }

        Containers::StringView optimization;
    } converter;

    converter.setOptimizationLevel("2");
    CORRADE_COMPARE(converter.optimization, "2");
}

void AbstractConverterTest::setOptimizationLevelNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ValidateData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.setOptimizationLevel({});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::setOptimizationLevel(): feature not supported\n");
}

void AbstractConverterTest::setOptimizationLevelNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::Optimize|ConverterFeature::ValidateData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.setOptimizationLevel({});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::setOptimizationLevel(): feature advertised but not implemented\n");
}

void AbstractConverterTest::setDebugInfoLevel() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::DebugInfo|ConverterFeature::ValidateData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        void doSetDebugInfoLevel(Containers::StringView level) override {
            debugInfo = level;
        }

        Containers::StringView debugInfo;
    } converter;

    converter.setDebugInfoLevel("0");
    CORRADE_COMPARE(converter.debugInfo, "0");
}

void AbstractConverterTest::setDebugInfoLevelNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ValidateData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.setDebugInfoLevel({});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::setDebugInfoLevel(): feature not supported\n");
}

void AbstractConverterTest::setDebugInfoLevelNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::DebugInfo|ConverterFeature::ValidateData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.setDebugInfoLevel({});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::setDebugInfoLevel(): feature advertised but not implemented\n");
}

void AbstractConverterTest::validateData() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ValidateData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        std::pair<bool, Containers::String> doValidateData(const Stage stage, const Containers::ArrayView<const char> data) override {
            return {data.size() == 5*4 && stage == Stage::MeshTask, "Yes, this is valid"};
        }
    } converter;

    std::pair<bool, Containers::String> out = converter.validateData(Stage::MeshTask, Containers::arrayView<UnsignedInt>({0x07230203, 99, 0xcafebabeu, 50, 0}));
    CORRADE_VERIFY(out.first);
    CORRADE_COMPARE(out.second, "Yes, this is valid");
}

void AbstractConverterTest::validateDataNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.validateData({}, {});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::validateData(): feature not supported\n");
}

void AbstractConverterTest::validateDataNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ValidateData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.validateData({}, {});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::validateData(): feature advertised but not implemented\n");
}

void AbstractConverterTest::validateDataPreprocessOnly() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ValidateData|ConverterFeature::Preprocess;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.setFlags(ConverterFlag::PreprocessOnly);
    converter.validateData({}, {});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::validateData(): PreprocessOnly is not allowed in combination with validation\n");
}

void AbstractConverterTest::validateDataCustomStringDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ValidateData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        std::pair<bool, Containers::String> doValidateData(Stage, const Containers::ArrayView<const char>) override {
            /* libc++ and MSVC STL is STUPID and doing
                `return {{}, Containers::String{...}};`
               will do a COPY, even though not needed. That then of course
               discards the custom deleter and makes the test fail. Oh C++, do
               I really have to reimplement even std::pair, FFS?! */
            std::pair<bool, Containers::String> out;
            out.second = Containers::String{"", 0, [](char*, std::size_t){}};
            return out;
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.validateData({}, {});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::validateData(): implementation is not allowed to use a custom String deleter\n");
}

void AbstractConverterTest::validateFile() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ValidateFile;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        std::pair<bool, Containers::String> doValidateFile(const Stage stage, const Containers::StringView filename) override {
            return {stage == Stage::Vertex && filename.size() == 8, "Yes, this is valid"};
        }
    } converter;

    std::pair<bool, Containers::String> out = converter.validateFile(Stage::Vertex, "file.spv");
    CORRADE_VERIFY(out.first);
    CORRADE_COMPARE(out.second, "Yes, this is valid");
}

void AbstractConverterTest::validateFileAsData() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ValidateData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        std::pair<bool, Containers::String> doValidateData(const Stage stage, const Containers::ArrayView<const char> data) override {
            return {stage == Stage::Compute && data.size() == 5, "Yes, this is valid"};
        }
    } converter;

    std::pair<bool, Containers::String> out = converter.validateFile(Stage::Compute, Utility::Directory::join(SHADERTOOLS_TEST_DIR, "file.dat"));
    CORRADE_VERIFY(out.first);
    CORRADE_COMPARE(out.second, "Yes, this is valid");
}

void AbstractConverterTest::validateFileAsDataNotFound() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ValidateData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        std::pair<bool, Containers::String> doValidateData(Stage, Containers::ArrayView<const char>) override {
            CORRADE_VERIFY(!"this shouldn't be reached");
            return {};
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    std::pair<bool, Containers::String> out2 = converter.validateFile({}, "nonexistent.bin");
    CORRADE_VERIFY(!out2.first);
    CORRADE_COMPARE(out2.second, "");
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::validateFile(): cannot open file nonexistent.bin\n");
}

void AbstractConverterTest::validateFileNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.validateFile({}, {});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::validateFile(): feature not supported\n");
}

void AbstractConverterTest::validateFileNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ValidateFile;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.validateFile({}, {});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::validateFile(): feature advertised but not implemented\n");
}

void AbstractConverterTest::validateFilePreprocessOnly() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ValidateFile|ConverterFeature::Preprocess;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.setFlags(ConverterFlag::PreprocessOnly);
    converter.validateFile({}, {});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::validateFile(): PreprocessOnly is not allowed in combination with validation\n");
}

void AbstractConverterTest::validateFileCustomStringDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ValidateData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        std::pair<bool, Containers::String> doValidateFile(Stage, Containers::StringView) override {
            /* libc++ and MSVC STL is STUPID and doing
                `return {{}, Containers::String{...}};`
               will do a COPY, even though not needed. That then of course
               discards the custom deleter and makes the test fail. Oh C++, do
               I really have to reimplement even std::pair, FFS?! */
            std::pair<bool, Containers::String> out;
            out.second = Containers::String{"", 0, [](char*, std::size_t){}};
            return out;
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.validateFile({}, {});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::validateFile(): implementation is not allowed to use a custom String deleter\n");
}

void AbstractConverterTest::convertDataToData() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doConvertDataToData(Stage, Containers::ArrayView<const char> data) override {
            return Containers::array({data.back(), data.front()});
        }
    } converter;

    const char data[] = {'S', 'P', 'I', 'R', 'V'};
    Containers::Array<char> out = converter.convertDataToData({}, data);
    CORRADE_COMPARE_AS(out, Containers::arrayView({'V', 'S'}),
        TestSuite::Compare::Container);
}

void AbstractConverterTest::convertDataToDataNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertFile;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convertDataToData({}, {});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::convertDataToData(): feature not supported\n");
}

void AbstractConverterTest::convertDataToDataNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convertDataToData({}, {});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::convertDataToData(): feature advertised but not implemented\n");
}

void AbstractConverterTest::convertDataToDataCustomDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doConvertDataToData(Stage, Containers::ArrayView<const char>) override {
            return Containers::Array<char>{nullptr, 0, [](char*, std::size_t){}};
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convertDataToData({}, {});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::convertDataToData(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractConverterTest::convertDataToFileThroughData() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doConvertDataToData(Stage, Containers::ArrayView<const char> data) override {
            return Containers::array({data.back(), data.front()});
        }
    } converter;

    const std::string filename = Utility::Directory::join(SHADERTOOLS_TEST_OUTPUT_DIR, "file.dat");

    /* Remove previous file, if any */
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));

    const char data[] = {'S', 'P', 'I', 'R', 'V'};
    CORRADE_VERIFY(converter.convertDataToFile({}, data, filename));
    CORRADE_COMPARE_AS(filename, "VS",
        TestSuite::Compare::FileToString);
}

void AbstractConverterTest::convertDataToFileThroughDataFailed() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doConvertDataToData(Stage, Containers::ArrayView<const char>) override {
            return {};
        }
    } converter;

    const std::string filename = Utility::Directory::join(SHADERTOOLS_TEST_OUTPUT_DIR, "file.dat");

    /* Remove previous file, if any */
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));

    /* Function should fail, no file should get written and no error output
       should be printed (the base implementation assumes the plugin does it) */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertDataToFile({}, {}, filename));
    CORRADE_VERIFY(!Utility::Directory::exists(filename));
    CORRADE_COMPARE(out.str(), "");
}

void AbstractConverterTest::convertDataToFileThroughDataNotWritable() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doConvertDataToData(Stage, Containers::ArrayView<const char>) override {
            return Containers::Array<char>{1};
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertDataToFile({}, nullptr, "/some/path/that/does/not/exist"));
    CORRADE_COMPARE(out.str(),
        "Utility::Directory::write(): can't open /some/path/that/does/not/exist\n"
        "ShaderTools::AbstractConverter::convertDataToFile(): cannot write to file /some/path/that/does/not/exist\n");
}

void AbstractConverterTest::convertDataToFileNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertFile;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convertDataToFile({}, {}, {});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::convertDataToFile(): feature not supported\n");
}

void AbstractConverterTest::convertDataToFileNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convertDataToFile({}, {}, {});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::convertDataToData(): feature advertised but not implemented\n");
}

void AbstractConverterTest::convertFileToFile() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertFile;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        bool doConvertFileToFile(Stage, const Containers::StringView from, const Containers::StringView to) override {
            Containers::Array<char> data = Utility::Directory::read(from);
            return Utility::Directory::write(to, Containers::array({data.back(), data.front()}));
        }
    } converter;

    const std::string filename = Utility::Directory::join(SHADERTOOLS_TEST_OUTPUT_DIR, "file.dat");

    /* Remove previous file, if any */
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));

    CORRADE_VERIFY(converter.convertFileToFile({}, Utility::Directory::join(SHADERTOOLS_TEST_DIR, "file.dat"), filename));
    CORRADE_COMPARE_AS(filename, "VS",
        TestSuite::Compare::FileToString);
}

void AbstractConverterTest::convertFileToFileThroughData() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doConvertDataToData(Stage, Containers::ArrayView<const char> data) override {
            return Containers::array({data.back(), data.front()});
        }
    } converter;

    const std::string filename = Utility::Directory::join(SHADERTOOLS_TEST_OUTPUT_DIR, "file.dat");

    /* Remove previous file, if any */
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));

    CORRADE_VERIFY(converter.convertFileToFile({}, Utility::Directory::join(SHADERTOOLS_TEST_DIR, "file.dat"), filename));
    CORRADE_COMPARE_AS(filename, "VS",
        TestSuite::Compare::FileToString);
}

void AbstractConverterTest::convertFileToFileThroughDataNotFound() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doConvertDataToData(Stage, Containers::ArrayView<const char>) override {
            CORRADE_VERIFY(!"this shouldn't be reached");
            return {};
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertFileToFile({}, "nonexistent.bin", "file.dat"));
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::convertFileToFile(): cannot open file nonexistent.bin\n");
}

void AbstractConverterTest::convertFileToFileThroughDataFailed() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doConvertDataToData(Stage, Containers::ArrayView<const char>) override {
            return {};
        }
    } converter;

    const std::string filename = Utility::Directory::join(SHADERTOOLS_TEST_OUTPUT_DIR, "file.dat");

    /* Remove previous file, if any */
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));

    /* Function should fail, no file should get written and no error output
       should be printed (the base implementation assumes the plugin does it) */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertFileToFile({}, Utility::Directory::join(SHADERTOOLS_TEST_DIR, "file.dat"), filename));
    CORRADE_VERIFY(!Utility::Directory::exists(filename));
    CORRADE_COMPARE(out.str(), "");
}

void AbstractConverterTest::convertFileToFileThroughDataNotWritable() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doConvertDataToData(Stage, Containers::ArrayView<const char>) override {
            return Containers::Array<char>{1};
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertFileToFile({}, Utility::Directory::join(SHADERTOOLS_TEST_DIR, "file.dat"), "/some/path/that/does/not/exist"));
    CORRADE_COMPARE(out.str(),
        "Utility::Directory::write(): can't open /some/path/that/does/not/exist\n"
        "ShaderTools::AbstractConverter::convertFileToFile(): cannot write to file /some/path/that/does/not/exist\n");
}

void AbstractConverterTest::convertFileToFileNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ValidateData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convertFileToFile({}, {}, {});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::convertFileToFile(): feature not supported\n");
}

void AbstractConverterTest::convertFileToFileNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertFile;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convertFileToFile({}, {}, {});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::convertFileToFile(): feature advertised but not implemented\n");
}

void AbstractConverterTest::convertFileToData() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doConvertFileToData(Stage, const Containers::StringView from) override {
            Containers::Array<char> data = Utility::Directory::read(from);
            return Containers::array({data.back(), data.front()});
        }
    } converter;

    Containers::Array<char> out = converter.convertFileToData({}, Utility::Directory::join(SHADERTOOLS_TEST_DIR, "file.dat"));
    CORRADE_COMPARE_AS(out, Containers::arrayView({'V', 'S'}),
        TestSuite::Compare::Container);
}

void AbstractConverterTest::convertFileToDataAsData() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doConvertDataToData(Stage, Containers::ArrayView<const char> data) override {
            return Containers::array({data.back(), data.front()});
        }
    } converter;

    Containers::Array<char> out = converter.convertFileToData({}, Utility::Directory::join(SHADERTOOLS_TEST_DIR, "file.dat"));
    CORRADE_COMPARE_AS(out, Containers::arrayView({'V', 'S'}),
        TestSuite::Compare::Container);
}

void AbstractConverterTest::convertFileToDataAsDataNotFound() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doConvertDataToData(Stage, Containers::ArrayView<const char>) override {
            CORRADE_VERIFY(!"this shouldn't be reached");
            return {};
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertFileToData({}, "nonexistent.bin"));
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::convertFileToData(): cannot open file nonexistent.bin\n");
}

void AbstractConverterTest::convertFileToDataNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertFile;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convertFileToData({}, {});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::convertFileToData(): feature not supported\n");
}

void AbstractConverterTest::convertFileToDataNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convertFileToData({}, Utility::Directory::join(SHADERTOOLS_TEST_DIR, "file.dat"));
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::convertDataToData(): feature advertised but not implemented\n");
}

void AbstractConverterTest::convertFileToDataCustomDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doConvertFileToData(Stage, const Containers::StringView) override {
            return Containers::Array<char>{nullptr, 0, [](char*, std::size_t){}};
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convertFileToData({}, {});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::convertFileToData(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractConverterTest::linkDataToData() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doLinkDataToData(Containers::ArrayView<const std::pair<Stage, Containers::ArrayView<const char>>> data) override {
            CORRADE_COMPARE(data.size(), 2);
            return Containers::array({
                data[0].first == Stage::Vertex ? data[0].second[0] : ' ',
                data[1].first == Stage::Fragment ? data[1].second[0] : ' '
            });
        }
    } converter;

    CORRADE_VERIFY(true); /* so it picks up correct test case name */

    Containers::Array<char> out = converter.linkDataToData({
        {Stage::Vertex, Containers::arrayView({'V', 'E'})},
        {Stage::Fragment, Containers::arrayView({'S', 'A'})}
    });
    CORRADE_COMPARE_AS(out, Containers::arrayView({'V', 'S'}),
        TestSuite::Compare::Container);
}

void AbstractConverterTest::linkDataToDataNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkFile;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.linkDataToData({});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::linkDataToData(): feature not supported\n");
}

void AbstractConverterTest::linkDataToDataNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.linkDataToData({{}});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::linkDataToData(): feature advertised but not implemented\n");
}

void AbstractConverterTest::linkDataToDataPreprocessOnly() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData|ConverterFeature::Preprocess;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.setFlags(ConverterFlag::PreprocessOnly);
    converter.linkDataToData({});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::linkDataToData(): PreprocessOnly is not allowed in combination with linking\n");
}

void AbstractConverterTest::linkDataToDataNoData() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.linkDataToData({});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::linkDataToData(): no data passed\n");
}

void AbstractConverterTest::linkDataToDataCustomDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doLinkDataToData(Containers::ArrayView<const std::pair<Stage, Containers::ArrayView<const char>>>) override {
            return Containers::Array<char>{nullptr, 0, [](char*, std::size_t){}};
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.linkDataToData({{}});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::linkDataToData(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractConverterTest::linkDataToFileThroughData() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doLinkDataToData(Containers::ArrayView<const std::pair<Stage, Containers::ArrayView<const char>>> data) override {
            CORRADE_COMPARE(data.size(), 2);
            return Containers::array({
                data[0].first == Stage::Vertex ? data[0].second[0] : ' ',
                data[1].first == Stage::Fragment ? data[1].second[0] : ' '
            });
        }
    } converter;

    const std::string filename = Utility::Directory::join(SHADERTOOLS_TEST_OUTPUT_DIR, "file.dat");

    /* Remove previous file, if any */
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));

    CORRADE_VERIFY(converter.linkDataToFile({
        {Stage::Vertex, Containers::arrayView({'V', 'E'})},
        {Stage::Fragment, Containers::arrayView({'S', 'A'})}
    }, filename));
    CORRADE_COMPARE_AS(filename, "VS",
        TestSuite::Compare::FileToString);
}

void AbstractConverterTest::linkDataToFileThroughDataFailed() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doLinkDataToData(Containers::ArrayView<const std::pair<Stage, Containers::ArrayView<const char>>>) override {
            return {};
        }
    } converter;

    const std::string filename = Utility::Directory::join(SHADERTOOLS_TEST_OUTPUT_DIR, "file.dat");

    /* Remove previous file, if any */
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));

    /* Function should fail, no file should get written and no error output
       should be printed (the base implementation assumes the plugin does it) */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.linkDataToFile({{}}, filename));
    CORRADE_VERIFY(!Utility::Directory::exists(filename));
    CORRADE_COMPARE(out.str(), "");
}

void AbstractConverterTest::linkDataToFileThroughDataNotWritable() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doLinkDataToData(Containers::ArrayView<const std::pair<Stage, Containers::ArrayView<const char>>>) override {
            return Containers::Array<char>{1};
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.linkDataToFile({{}}, "/some/path/that/does/not/exist"));
    CORRADE_COMPARE(out.str(),
        "Utility::Directory::write(): can't open /some/path/that/does/not/exist\n"
        "ShaderTools::AbstractConverter::linkDataToFile(): cannot write to file /some/path/that/does/not/exist\n");
}

void AbstractConverterTest::linkDataToFileNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkFile;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.linkDataToFile({}, "file.dat");
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::linkDataToFile(): feature not supported\n");
}

void AbstractConverterTest::linkDataToFileNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.linkDataToFile({{}}, "file.dat");
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::linkDataToData(): feature advertised but not implemented\n");
}

void AbstractConverterTest::linkDataToFilePreprocessOnly() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData|ConverterFeature::Preprocess;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.setFlags(ConverterFlag::PreprocessOnly);
    converter.linkDataToFile({}, {});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::linkDataToFile(): PreprocessOnly is not allowed in combination with linking\n");
}

void AbstractConverterTest::linkDataToFileNoData() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.linkDataToFile({}, {});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::linkDataToFile(): no data passed\n");
}

void AbstractConverterTest::linkFilesToFile() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkFile;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        bool doLinkFilesToFile(Containers::ArrayView<const std::pair<Stage, Containers::StringView>> from, Containers::StringView to) override {
            CORRADE_COMPARE(from.size(), 2);
            Containers::Array<char> first = Utility::Directory::read(from[0].second);
            Containers::Array<char> second = Utility::Directory::read(from[1].second);
            CORRADE_VERIFY(first);
            CORRADE_VERIFY(second);
            return Utility::Directory::write(to, Containers::array({
                from[0].first == Stage::Vertex ? first[0] : ' ',
                from[1].first == Stage::Fragment ? second[0] : ' '
            }));
        }
    } converter;

    const std::string filename = Utility::Directory::join(SHADERTOOLS_TEST_OUTPUT_DIR, "file.dat");

    /* Remove previous file, if any */
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));

    CORRADE_VERIFY(converter.linkFilesToFile({
        {Stage::Vertex, Utility::Directory::join(SHADERTOOLS_TEST_DIR, "another.dat")},
        {Stage::Fragment, Utility::Directory::join(SHADERTOOLS_TEST_DIR, "file.dat")}
    }, filename));
    CORRADE_COMPARE_AS(filename, "VS",
        TestSuite::Compare::FileToString);
}

void AbstractConverterTest::linkFilesToFileThroughData() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doLinkDataToData(Containers::ArrayView<const std::pair<Stage, Containers::ArrayView<const char>>> data) override {
            CORRADE_COMPARE(data.size(), 2);
            return Containers::array({
                data[0].first == Stage::Vertex ? data[0].second[0] : ' ',
                data[1].first == Stage::Fragment ? data[1].second[0] : ' '
            });
        }
    } converter;

    const std::string filename = Utility::Directory::join(SHADERTOOLS_TEST_OUTPUT_DIR, "file.dat");

    /* Remove previous file, if any */
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));

    CORRADE_VERIFY(converter.linkFilesToFile({
        {Stage::Vertex, Utility::Directory::join(SHADERTOOLS_TEST_DIR, "another.dat")},
        {Stage::Fragment, Utility::Directory::join(SHADERTOOLS_TEST_DIR, "file.dat")}
    }, filename));
    CORRADE_COMPARE_AS(filename, "VS",
        TestSuite::Compare::FileToString);
}

void AbstractConverterTest::linkFilesToFileThroughDataNotFound() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doLinkDataToData(Containers::ArrayView<const std::pair<Stage, Containers::ArrayView<const char>>>) override {
            CORRADE_VERIFY(!"this shouldn't be reached");
            return {};
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.linkFilesToFile({
        {{}, Utility::Directory::join(SHADERTOOLS_TEST_DIR, "another.dat")},
        {{}, "nonexistent.bin"}
    }, "file.dat"));
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::linkFilesToFile(): cannot open file nonexistent.bin\n");
}

void AbstractConverterTest::linkFilesToFileThroughDataFailed() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doLinkDataToData(Containers::ArrayView<const std::pair<Stage, Containers::ArrayView<const char>>>) override {
            return {};
        }
    } converter;

    const std::string filename = Utility::Directory::join(SHADERTOOLS_TEST_OUTPUT_DIR, "file.dat");

    /* Remove previous file, if any */
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));

    /* Function should fail, no file should get written and no error output
       should be printed (the base implementation assumes the plugin does it) */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.linkFilesToFile({
        {{}, Utility::Directory::join(SHADERTOOLS_TEST_DIR, "file.dat")}
    }, filename));
    CORRADE_VERIFY(!Utility::Directory::exists(filename));
    CORRADE_COMPARE(out.str(), "");
}

void AbstractConverterTest::linkFilesToFileThroughDataNotWritable() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doLinkDataToData(Containers::ArrayView<const std::pair<Stage, Containers::ArrayView<const char>>>) override {
            return Containers::Array<char>{1};
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.linkFilesToFile({
        {{}, Utility::Directory::join(SHADERTOOLS_TEST_DIR, "file.dat")}
    }, "/some/path/that/does/not/exist"));
    CORRADE_COMPARE(out.str(),
        "Utility::Directory::write(): can't open /some/path/that/does/not/exist\n"
        "ShaderTools::AbstractConverter::linkFilesToFile(): cannot write to file /some/path/that/does/not/exist\n");
}

void AbstractConverterTest::linkFilesToFileNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ValidateData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.linkFilesToFile({}, {});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::linkFilesToFile(): feature not supported\n");
}

void AbstractConverterTest::linkFilesToFileNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkFile;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.linkFilesToFile({{}}, {});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::linkFilesToFile(): feature advertised but not implemented\n");
}

void AbstractConverterTest::linkFilesToFilePreprocessOnly() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkFile|ConverterFeature::Preprocess;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.setFlags(ConverterFlag::PreprocessOnly);
    converter.linkFilesToFile({}, {});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::linkFilesToFile(): PreprocessOnly is not allowed in combination with linking\n");
}

void AbstractConverterTest::linkFilesToFileNoFile() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkFile;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.linkFilesToFile({}, {});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::linkFilesToFile(): no files passed\n");
}

void AbstractConverterTest::linkFilesToData() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doLinkFilesToData(Containers::ArrayView<const std::pair<Stage, Containers::StringView>> from) override {
            CORRADE_COMPARE(from.size(), 2);
            Containers::Array<char> first = Utility::Directory::read(from[0].second);
            Containers::Array<char> second = Utility::Directory::read(from[1].second);
            CORRADE_VERIFY(first);
            CORRADE_VERIFY(second);
            return Containers::array({
                from[0].first == Stage::Vertex ? first[0] : ' ',
                from[1].first == Stage::Fragment ? second[0] : ' '
            });
        }
    } converter;

    CORRADE_VERIFY(true); /* so it picks up correct test case name */

    Containers::Array<char> out = converter.linkFilesToData({
        {Stage::Vertex, Utility::Directory::join(SHADERTOOLS_TEST_DIR, "another.dat")},
        {Stage::Fragment, Utility::Directory::join(SHADERTOOLS_TEST_DIR, "file.dat")}
    });
    CORRADE_COMPARE_AS(out, Containers::arrayView({'V', 'S'}),
        TestSuite::Compare::Container);
}

void AbstractConverterTest::linkFilesToDataAsData() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doLinkDataToData(Containers::ArrayView<const std::pair<Stage, Containers::ArrayView<const char>>> data) override {
            CORRADE_COMPARE(data.size(), 2);
            return Containers::array({
                data[0].first == Stage::Vertex ? data[0].second[0] : ' ',
                data[1].first == Stage::Fragment ? data[1].second[0] : ' '
            });
        }
    } converter;

    Containers::Array<char> out = converter.linkFilesToData({
        {Stage::Vertex, Utility::Directory::join(SHADERTOOLS_TEST_DIR, "another.dat")},
        {Stage::Fragment, Utility::Directory::join(SHADERTOOLS_TEST_DIR, "file.dat")}
    });
    CORRADE_COMPARE_AS(out, Containers::arrayView({'V', 'S'}),
        TestSuite::Compare::Container);
}

void AbstractConverterTest::linkFilesToDataAsDataNotFound() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doLinkDataToData(Containers::ArrayView<const std::pair<Stage, Containers::ArrayView<const char>>>) override {
            CORRADE_VERIFY(!"this shouldn't be reached");
            return {};
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.linkFilesToData({
        {{}, "nonexistent.bin"}
    }));
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::linkFilesToData(): cannot open file nonexistent.bin\n");
}

void AbstractConverterTest::linkFilesToDataNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkFile;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.linkFilesToData({});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::linkFilesToData(): feature not supported\n");
}

void AbstractConverterTest::linkFilesToDataNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.linkFilesToData({
        {{}, Utility::Directory::join(SHADERTOOLS_TEST_DIR, "file.dat")}
    });
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::linkDataToData(): feature advertised but not implemented\n");
}

void AbstractConverterTest::linkFilesToDataPreprocessOnly() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData|ConverterFeature::Preprocess;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.setFlags(ConverterFlag::PreprocessOnly);
    converter.linkFilesToData({});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::linkFilesToData(): PreprocessOnly is not allowed in combination with linking\n");
}

void AbstractConverterTest::linkFilesToDataNoFile() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.linkFilesToData({});
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::linkFilesToData(): no files passed\n");
}

void AbstractConverterTest::linkFilesToDataCustomDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doLinkFilesToData(Containers::ArrayView<const std::pair<Stage, Containers::StringView>>) override {
            return Containers::Array<char>{nullptr, 0, [](char*, std::size_t){}};
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.linkFilesToData({
        {{}, "file.dat"}
    });
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::linkFilesToData(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractConverterTest::setInputFileCallback() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        void doSetInputFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, void*), void* userData) override {
            *static_cast<int*>(userData) = 1337;
        }
    } converter;

    int a = 0;
    auto lambda = [](const std::string&, InputFileCallbackPolicy, void*) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    };
    converter.setInputFileCallback(lambda, &a);
    CORRADE_COMPARE(converter.inputFileCallback(), lambda);
    CORRADE_COMPARE(converter.inputFileCallbackUserData(), &a);
    CORRADE_COMPARE(a, 1337);
}

void AbstractConverterTest::setInputFileCallbackTemplate() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        void doSetInputFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, void*), void*) override {
            called = true;
        }

        bool called = false;
    } converter;

    int a = 0;
    auto lambda = [](const std::string&, InputFileCallbackPolicy, int&) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    };
    converter.setInputFileCallback(lambda, a);
    CORRADE_VERIFY(converter.inputFileCallback());
    CORRADE_VERIFY(converter.inputFileCallbackUserData());
    CORRADE_VERIFY(converter.called);

    /* The data pointers should be wrapped, thus not the same */
    CORRADE_VERIFY(reinterpret_cast<void(*)()>(converter.inputFileCallback()) != reinterpret_cast<void(*)()>(static_cast<Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, int&)>(lambda)));
    CORRADE_VERIFY(converter.inputFileCallbackUserData() != &a);
}

void AbstractConverterTest::setInputFileCallbackTemplateNull() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        void doSetInputFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*callback)(const std::string&, InputFileCallbackPolicy, void*), void* userData) override {
            called = !callback && !userData;
        }

        bool called = false;
    } converter;

    int a = 0;
    converter.setInputFileCallback(static_cast<Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, int&)>(nullptr), a);
    CORRADE_VERIFY(!converter.inputFileCallback());
    CORRADE_VERIFY(!converter.inputFileCallbackUserData());
    CORRADE_VERIFY(converter.called);
}

void AbstractConverterTest::setInputFileCallbackTemplateConst() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        void doSetInputFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, void*), void*) override {
            called = true;
        }

        bool called = false;
    } converter;

    /* Just verify we can have const parameters */
    const int a = 0;
    auto lambda = [](const std::string&, InputFileCallbackPolicy, const int&) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    };
    converter.setInputFileCallback(lambda, a);
    CORRADE_VERIFY(converter.inputFileCallback());
    CORRADE_VERIFY(converter.inputFileCallbackUserData());
    CORRADE_VERIFY(converter.called);
}

void AbstractConverterTest::setInputFileCallbackNotImplemented() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    int a;
    auto lambda = [](const std::string&, InputFileCallbackPolicy, void*) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    };
    converter.setInputFileCallback(lambda, &a);
    CORRADE_COMPARE(converter.inputFileCallback(), lambda);
    CORRADE_COMPARE(converter.inputFileCallbackUserData(), &a);
    /* Should just work, no need to implement the function */
}

void AbstractConverterTest::setInputFileCallbackNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertFile;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};

    int a;
    converter.setInputFileCallback([](const std::string&, InputFileCallbackPolicy, void*) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    }, &a);
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::setInputFileCallback(): converter supports neither loading from data nor via callbacks, callbacks can't be used\n");
}

void AbstractConverterTest::setInputFileCallbackValidateFileDirectly() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ValidateFile|ConverterFeature::InputFileCallback;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        std::pair<bool, Containers::String> doValidateFile(Stage, const Containers::StringView filename) override {
            return {filename == "file.dat" && inputFileCallback() && inputFileCallbackUserData(), "it's what it is!"};
        }

        std::pair<bool, Containers::String> doValidateData(Stage, Containers::ArrayView<const char>) override {
            CORRADE_VERIFY(!"this should not be reached");
            return {};
        }
    } converter;

    int a{};
    converter.setInputFileCallback([](const std::string&, InputFileCallbackPolicy, void*) {
        CORRADE_VERIFY(!"this should not be reached");
        return Containers::Optional<Containers::ArrayView<const char>>{};
    }, &a);

    CORRADE_COMPARE(converter.validateFile({}, "file.dat"), std::make_pair(true, "it's what it is!"));
}

void AbstractConverterTest::setInputFileCallbackValidateFileThroughBaseImplementation() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ValidateData|ConverterFeature::InputFileCallback;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        std::pair<bool, Containers::String> doValidateFile(Stage stage, const Containers::StringView filename) override {
            validateFileCalled = true;

            if(filename != "file.dat" || !inputFileCallback() || !inputFileCallbackUserData())
                return {};

            return AbstractConverter::doValidateFile(stage, filename);
        }

        std::pair<bool, Containers::String> doValidateData(Stage stage, Containers::ArrayView<const char> data) override {
            return {stage == Stage::RayCallable && data.size() == 1 && data[0] == '\xb0', "yep!!"};
        }

        bool validateFileCalled = false;
    } converter;

    struct State {
        const char data = '\xb0';
        bool loaded = false;
        bool closed = false;
    } state;

    converter.setInputFileCallback([](const std::string& filename, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        if(filename == "file.dat" && policy == InputFileCallbackPolicy::LoadTemporary) {
            state.loaded = true;
            return Containers::arrayView(&state.data, 1);
        }

        if(filename == "file.dat" && policy == InputFileCallbackPolicy::Close) {
            state.closed = true;
            return {};
        }

        CORRADE_VERIFY(!"this shouldn't be reached");
        return {};
    }, state);

    CORRADE_COMPARE(converter.validateFile(Stage::RayCallable, "file.dat"), std::make_pair(true, "yep!!"));
    CORRADE_VERIFY(converter.validateFileCalled);
    CORRADE_VERIFY(state.loaded);
    CORRADE_VERIFY(state.closed);
}

void AbstractConverterTest::setInputFileCallbackValidateFileThroughBaseImplementationFailed() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ValidateData|ConverterFeature::InputFileCallback;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        std::pair<bool, Containers::String> doValidateFile(Stage stage, const Containers::StringView filename) override {
            validateFileCalled = true;
            return AbstractConverter::doValidateFile(stage, filename);
        }

        bool validateFileCalled = false;
    } converter;

    converter.setInputFileCallback([](const std::string&, InputFileCallbackPolicy, void*) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    });

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_COMPARE(converter.validateFile({}, "file.dat"), std::make_pair(false, ""));
    CORRADE_VERIFY(converter.validateFileCalled);
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::validateFile(): cannot open file file.dat\n");
}

void AbstractConverterTest::setInputFileCallbackValidateFileAsData() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ValidateData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        std::pair<bool, Containers::String> doValidateFile(Stage, const Containers::StringView) override {
            CORRADE_VERIFY(!"this shouldn't be reached");
            return {};
        }

        std::pair<bool, Containers::String> doValidateData(Stage stage, Containers::ArrayView<const char> data) override {
            return {stage == Stage::Fragment && data.size() == 1 && data[0] == '\xb0', "yep!!"};
        }
    } converter;

    struct State {
        const char data = '\xb0';
        bool loaded = false;
        bool closed = false;
    } state;

    converter.setInputFileCallback([](const std::string& filename, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        if(filename == "file.dat" && policy == InputFileCallbackPolicy::LoadTemporary) {
            state.loaded = true;
            return Containers::arrayView(&state.data, 1);
        }

        if(filename == "file.dat" && policy == InputFileCallbackPolicy::Close) {
            state.closed = true;
            return {};
        }

        CORRADE_VERIFY(!"this shouldn't be reached");
        return {};
    }, state);

    CORRADE_COMPARE(converter.validateFile(Stage::Fragment, "file.dat"), std::make_pair(true, "yep!!"));
    CORRADE_VERIFY(state.loaded);
    CORRADE_VERIFY(state.closed);
}

void AbstractConverterTest::setInputFileCallbackValidateFileAsDataFailed() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ValidateData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        std::pair<bool, Containers::String> doValidateFile(Stage, const Containers::StringView) override {
            CORRADE_VERIFY(!"this shouldn't be reached");
            return {};
        }
    } converter;

    converter.setInputFileCallback([](const std::string&, InputFileCallbackPolicy, void*) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    });

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_COMPARE(converter.validateFile({}, "file.dat"), std::make_pair(false, ""));
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::validateFile(): cannot open file file.dat\n");
}

void AbstractConverterTest::setInputFileCallbackConvertFileToFileDirectly() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertFile|ConverterFeature::InputFileCallback;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        bool doConvertFileToFile(Stage stage, const Containers::StringView from, const Containers::StringView to) override {
            return stage == Stage::Mesh && from == "file.dat" && to == "file.out" && inputFileCallback() && inputFileCallbackUserData();
        }

        Containers::Array<char> doConvertDataToData(Stage, Containers::ArrayView<const char>) override {
            CORRADE_VERIFY(!"this should not be reached");
            return {};
        }
    } converter;

    int a{};
    converter.setInputFileCallback([](const std::string&, InputFileCallbackPolicy, void*) {
        CORRADE_VERIFY(!"this should not be reached");
        return Containers::Optional<Containers::ArrayView<const char>>{};
    }, &a);

    CORRADE_VERIFY(converter.convertFileToFile(Stage::Mesh, "file.dat", "file.out"));
}

void AbstractConverterTest::setInputFileCallbackConvertFileToFileThroughBaseImplementation() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData|ConverterFeature::InputFileCallback;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        bool doConvertFileToFile(Stage stage, const Containers::StringView from, const Containers::StringView to) override {
            convertFileToFileCalled = true;

            if(stage != Stage::Geometry || from != "file.dat" || !to.hasSuffix("file.out") || !inputFileCallback() || !inputFileCallbackUserData())
                return {};

            return AbstractConverter::doConvertFileToFile(stage, from, to);
        }

        Containers::Array<char> doConvertDataToData(Stage stage, Containers::ArrayView<const char> data) override {
            if(stage == Stage::Geometry && data.size() == 1 && data[0] == '\xb0')
                return Containers::array({'y', 'e', 'p'});
            return {};
        }

        bool convertFileToFileCalled = false;
    } converter;

    struct State {
        const char data = '\xb0';
        bool loaded = false;
        bool closed = false;
    } state;

    converter.setInputFileCallback([](const std::string& filename, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        if(filename == "file.dat" && policy == InputFileCallbackPolicy::LoadTemporary) {
            state.loaded = true;
            return Containers::arrayView(&state.data, 1);
        }

        if(filename == "file.dat" && policy == InputFileCallbackPolicy::Close) {
            state.closed = true;
            return {};
        }

        CORRADE_VERIFY(!"this shouldn't be reached");
        return {};
    }, state);

    /* Remove previous file, if any */
    const std::string filename = Utility::Directory::join(SHADERTOOLS_TEST_OUTPUT_DIR, "file.out");
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));

    CORRADE_VERIFY(converter.convertFileToFile(Stage::Geometry, "file.dat", filename));
    CORRADE_VERIFY(converter.convertFileToFileCalled);
    CORRADE_VERIFY(state.loaded);
    CORRADE_VERIFY(state.closed);
    CORRADE_COMPARE_AS(filename, "yep", TestSuite::Compare::FileToString);
}

void AbstractConverterTest::setInputFileCallbackConvertFileToFileThroughBaseImplementationFailed() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData|ConverterFeature::InputFileCallback;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        bool doConvertFileToFile(Stage stage, const Containers::StringView from, const Containers::StringView to) override {
            convertFileToFileCalled = true;
            return AbstractConverter::doConvertFileToFile(stage, from, to);
        }

        bool convertFileToFileCalled = false;
    } converter;

    converter.setInputFileCallback([](const std::string&, InputFileCallbackPolicy, void*) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    });

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_VERIFY(!converter.convertFileToFile({}, "file.dat", "/some/path/that/does/not/exist"));
    CORRADE_VERIFY(converter.convertFileToFileCalled);
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::convertFileToFile(): cannot open file file.dat\n");
}

void AbstractConverterTest::setInputFileCallbackConvertFileToFileAsData() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        bool doConvertFileToFile(Stage, Containers::StringView, Containers::StringView) override {
            CORRADE_VERIFY(!"this shouldn't be reached");
            return {};
        }

        Containers::Array<char> doConvertDataToData(Stage stage, Containers::ArrayView<const char> data) override {
            if(stage == Stage::RayAnyHit && data.size() == 1 && data[0] == '\xb0')
                return Containers::array({'y', 'e', 'p'});
            return {};
        }
    } converter;

    struct State {
        const char data = '\xb0';
        bool loaded = false;
        bool closed = false;
    } state;

    converter.setInputFileCallback([](const std::string& filename, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        if(filename == "file.dat" && policy == InputFileCallbackPolicy::LoadTemporary) {
            state.loaded = true;
            return Containers::arrayView(&state.data, 1);
        }

        if(filename == "file.dat" && policy == InputFileCallbackPolicy::Close) {
            state.closed = true;
            return {};
        }

        CORRADE_VERIFY(!"this shouldn't be reached");
        return {};
    }, state);

    /* Remove previous file, if any */
    const std::string filename = Utility::Directory::join(SHADERTOOLS_TEST_OUTPUT_DIR, "file.out");
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));

    CORRADE_VERIFY(converter.convertFileToFile(Stage::RayAnyHit, "file.dat", filename));
    CORRADE_VERIFY(state.loaded);
    CORRADE_VERIFY(state.closed);
    CORRADE_COMPARE_AS(filename, "yep", TestSuite::Compare::FileToString);
}

void AbstractConverterTest::setInputFileCallbackConvertFileToFileAsDataFailed() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        bool doConvertFileToFile(Stage, Containers::StringView, Containers::StringView) override {
            CORRADE_VERIFY(!"this shouldn't be reached");
            return {};
        }
    } converter;

    converter.setInputFileCallback([](const std::string&, InputFileCallbackPolicy, void*) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    });

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_VERIFY(!converter.convertFileToFile({}, "file.dat", "/some/path/that/does/not/exist"));
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::convertFileToFile(): cannot open file file.dat\n");
}

void AbstractConverterTest::setInputFileCallbackConvertFileToFileAsDataNotWritable() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        bool doConvertFileToFile(Stage, Containers::StringView, Containers::StringView) override {
            CORRADE_VERIFY(!"this shouldn't be reached");
            return {};
        }

        Containers::Array<char> doConvertDataToData(Stage, Containers::ArrayView<const char>) override {
            return Containers::Array<char>{1};
        }
    } converter;

    struct State {
        const char data = '\xb0';
        bool loaded = false;
        bool closed = false;
    } state;

    converter.setInputFileCallback([](const std::string& filename, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        if(filename == "file.dat" && policy == InputFileCallbackPolicy::LoadTemporary) {
            state.loaded = true;
            return Containers::arrayView(&state.data, 1);
        }

        if(filename == "file.dat" && policy == InputFileCallbackPolicy::Close) {
            state.closed = true;
            return {};
        }

        CORRADE_VERIFY(!"this shouldn't be reached");
        return {};
    }, state);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertFileToFile({}, "file.dat", "/some/path/that/does/not/exist"));
    CORRADE_VERIFY(state.loaded);
    CORRADE_VERIFY(state.closed);
    CORRADE_COMPARE(out.str(),
        "Utility::Directory::write(): can't open /some/path/that/does/not/exist\n"
        "ShaderTools::AbstractConverter::convertFileToFile(): cannot write to file /some/path/that/does/not/exist\n");
}

void AbstractConverterTest::setInputFileCallbackConvertFileToDataDirectly() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData|ConverterFeature::InputFileCallback;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doConvertFileToData(Stage stage, Containers::StringView from) override {
            if(stage == Stage::Compute && from == "file.dat" && inputFileCallback() && inputFileCallbackUserData())
                return Containers::array({'y', 'e', 'p'});
            return {};
        }

        Containers::Array<char> doConvertDataToData(Stage, Containers::ArrayView<const char>) override {
            CORRADE_VERIFY(!"this should not be reached");
            return {};
        }
    } converter;

    int a{};
    converter.setInputFileCallback([](const std::string&, InputFileCallbackPolicy, void*) {
        CORRADE_VERIFY(!"this should not be reached");
        return Containers::Optional<Containers::ArrayView<const char>>{};
    }, &a);

    CORRADE_COMPARE_AS(converter.convertFileToData(Stage::Compute, "file.dat"),
        Containers::arrayView({'y', 'e', 'p'}),
        TestSuite::Compare::Container);
}

void AbstractConverterTest::setInputFileCallbackConvertFileToDataThroughBaseImplementation() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData|ConverterFeature::InputFileCallback;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doConvertFileToData(Stage stage, Containers::StringView from) override {
            convertFileToDataCalled = true;

            if(stage != Stage::TessellationEvaluation || from != "file.dat" || !inputFileCallback() || !inputFileCallbackUserData())
                return {};

            return AbstractConverter::doConvertFileToData(stage, from);
        }

        Containers::Array<char> doConvertDataToData(Stage stage, Containers::ArrayView<const char> data) override {
            if(stage == Stage::TessellationEvaluation && data.size() == 1 && data[0] == '\xb0')
                return Containers::array({'y', 'e', 'p'});
            return {};
        }

        bool convertFileToDataCalled = false;
    } converter;

    struct State {
        const char data = '\xb0';
        bool loaded = false;
        bool closed = false;
    } state;

    converter.setInputFileCallback([](const std::string& filename, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        if(filename == "file.dat" && policy == InputFileCallbackPolicy::LoadTemporary) {
            state.loaded = true;
            return Containers::arrayView(&state.data, 1);
        }

        if(filename == "file.dat" && policy == InputFileCallbackPolicy::Close) {
            state.closed = true;
            return {};
        }

        CORRADE_VERIFY(!"this shouldn't be reached");
        return {};
    }, state);

    CORRADE_COMPARE_AS(converter.convertFileToData(Stage::TessellationEvaluation, "file.dat"),
        Containers::arrayView({'y', 'e', 'p'}),
        TestSuite::Compare::Container);
    CORRADE_VERIFY(converter.convertFileToDataCalled);
    CORRADE_VERIFY(state.loaded);
    CORRADE_VERIFY(state.closed);
}

void AbstractConverterTest::setInputFileCallbackConvertFileToDataThroughBaseImplementationFailed() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData|ConverterFeature::InputFileCallback;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doConvertFileToData(Stage stage, Containers::StringView from) override {
            convertFileToDataCalled = true;
            return AbstractConverter::doConvertFileToData(stage, from);
        }

        bool convertFileToDataCalled = false;
    } converter;

    converter.setInputFileCallback([](const std::string&, InputFileCallbackPolicy, void*) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    });

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_VERIFY(!converter.convertFileToData({}, "file.dat"));
    CORRADE_VERIFY(converter.convertFileToDataCalled);
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::convertFileToData(): cannot open file file.dat\n");
}

void AbstractConverterTest::setInputFileCallbackConvertFileToDataAsData() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doConvertFileToData(Stage, Containers::StringView) override {
            CORRADE_VERIFY(!"this shouldn't be reached");
            return {};
        }

        Containers::Array<char> doConvertDataToData(Stage stage, Containers::ArrayView<const char> data) override {
            if(stage == Stage::RayGeneration && data.size() == 1 && data[0] == '\xb0')
                return Containers::array({'y', 'e', 'p'});
            return {};
        }
    } converter;

    struct State {
        const char data = '\xb0';
        bool loaded = false;
        bool closed = false;
    } state;

    converter.setInputFileCallback([](const std::string& filename, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        if(filename == "file.dat" && policy == InputFileCallbackPolicy::LoadTemporary) {
            state.loaded = true;
            return Containers::arrayView(&state.data, 1);
        }

        if(filename == "file.dat" && policy == InputFileCallbackPolicy::Close) {
            state.closed = true;
            return {};
        }

        CORRADE_VERIFY(!"this shouldn't be reached");
        return {};
    }, state);

    CORRADE_COMPARE_AS(converter.convertFileToData(Stage::RayGeneration, "file.dat"),
        Containers::arrayView({'y', 'e', 'p'}),
        TestSuite::Compare::Container);
    CORRADE_VERIFY(state.loaded);
    CORRADE_VERIFY(state.closed);
}

void AbstractConverterTest::setInputFileCallbackConvertFileToDataAsDataFailed() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::ConvertData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doConvertFileToData(Stage, Containers::StringView) override {
            CORRADE_VERIFY(!"this shouldn't be reached");
            return {};
        }
    } converter;

    converter.setInputFileCallback([](const std::string&, InputFileCallbackPolicy, void*) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    });

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_VERIFY(!converter.convertFileToData({}, "file.dat"));
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::convertFileToData(): cannot open file file.dat\n");
}

void AbstractConverterTest::setInputFileCallbackLinkFilesToFileDirectly() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkFile|ConverterFeature::InputFileCallback;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        bool doLinkFilesToFile(Containers::ArrayView<const std::pair<Stage, Containers::StringView>> from, Containers::StringView to) override {
            return from.size() == 2 && from[0].first == Stage::Vertex && from[0].second == "another.dat" && from[1].first == Stage::Fragment && from[1].second == "file.dat" && to == "file.out" && inputFileCallback() && inputFileCallbackUserData();
        }

        Containers::Array<char> doConvertDataToData(Stage, Containers::ArrayView<const char>) override {
            CORRADE_VERIFY(!"this should not be reached");
            return {};
        }
    } converter;

    int a{};
    converter.setInputFileCallback([](const std::string&, InputFileCallbackPolicy, void*) {
        CORRADE_VERIFY(!"this should not be reached");
        return Containers::Optional<Containers::ArrayView<const char>>{};
    }, &a);

    CORRADE_VERIFY(converter.linkFilesToFile({
        {Stage::Vertex, "another.dat"},
        {Stage::Fragment, "file.dat"}
    }, "file.out"));
}

void AbstractConverterTest::setInputFileCallbackLinkFilesToFileThroughBaseImplementation() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData|ConverterFeature::InputFileCallback;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        bool doLinkFilesToFile(Containers::ArrayView<const std::pair<Stage, Containers::StringView>> from, Containers::StringView to) override {
            linkFilesToFileCalled = true;

            if(from.size() != 2 || from[0].first != Stage::Vertex || from[0].second != "another.dat" || from[1].first != Stage::Fragment || from[1].second != "file.dat" || !to.hasSuffix("file.out") || !inputFileCallback() || !inputFileCallbackUserData())
                return {};

            return AbstractConverter::doLinkFilesToFile(from, to);
        }

        Containers::Array<char> doLinkDataToData(Containers::ArrayView<const std::pair<Stage, Containers::ArrayView<const char>>> data) override {
            CORRADE_COMPARE(data.size(), 2);
            return Containers::array({
                data[0].first == Stage::Vertex ? data[0].second[0] : ' ',
                data[1].first == Stage::Fragment ? data[1].second[0] : ' '
            });
        }

        bool linkFilesToFileCalled = false;
    } converter;

    struct State {
        const char first[2]{'V', 'E'};
        const char second[2]{'S', 'A'};
        std::string operations;
    } state;

    converter.setInputFileCallback([](const std::string& filename, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        if(policy == InputFileCallbackPolicy::LoadTemporary) {
            state.operations += "loaded " + filename + "\n";
            if(filename == "another.dat")
                return Containers::arrayView(state.first);
            if(filename == "file.dat")
                return Containers::arrayView(state.second);
        }

        if(policy == InputFileCallbackPolicy::Close) {
            state.operations += "closed " + filename + "\n";
            return {};
        }

        CORRADE_VERIFY(!"this shouldn't be reached");
        return {};
    }, state);

    /* Remove previous file, if any */
    const std::string filename = Utility::Directory::join(SHADERTOOLS_TEST_OUTPUT_DIR, "file.out");
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));

    CORRADE_VERIFY(converter.linkFilesToFile({
        {Stage::Vertex, "another.dat"},
        {Stage::Fragment, "file.dat"}
    }, filename));
    CORRADE_VERIFY(converter.linkFilesToFileCalled);
    CORRADE_COMPARE(state.operations,
        "loaded another.dat\n"
        "loaded file.dat\n"
        "closed another.dat\n"
        "closed file.dat\n");
    CORRADE_COMPARE_AS(filename, "VS", TestSuite::Compare::FileToString);
}

void AbstractConverterTest::setInputFileCallbackLinkFilesToFileThroughBaseImplementationFailed() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData|ConverterFeature::InputFileCallback;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        bool doLinkFilesToFile(Containers::ArrayView<const std::pair<Stage, Containers::StringView>> from, Containers::StringView to) override {
            linkFilesToFileCalled = true;
            return AbstractConverter::doLinkFilesToFile(from, to);
        }

        Containers::Array<char> doLinkDataToData(Containers::ArrayView<const std::pair<Stage, Containers::ArrayView<const char>>>) override {
            CORRADE_VERIFY(!"this shouldn't be called");
            return {};
        }

        bool linkFilesToFileCalled = false;
    } converter;

    struct State {
        const char data[1]{};
        std::string operations;
    } state;

    converter.setInputFileCallback([](const std::string& filename, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        if(policy == InputFileCallbackPolicy::LoadTemporary) {
            state.operations += "loaded " + filename + "\n";
            if(filename == "another.dat")
                return Containers::arrayView(state.data);
            /* This deliberately fails */
            if(filename == "file.dat") return {};
        }

        if(policy == InputFileCallbackPolicy::Close) {
            state.operations += "closed " + filename + "\n";
            return {};
        }

        CORRADE_VERIFY(!"this shouldn't be reached");
        return {};
    }, state);

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_VERIFY(!converter.linkFilesToFile({
        {Stage::Vertex, "another.dat"},
        {Stage::Fragment, "file.dat"}
    }, "/some/path/that/does/not/exist"));
    CORRADE_VERIFY(converter.linkFilesToFileCalled);
    CORRADE_COMPARE(state.operations,
        "loaded another.dat\n"
        "loaded file.dat\n" /* this fails */
        "closed another.dat\n");
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::linkFilesToFile(): cannot open file file.dat\n");
}

void AbstractConverterTest::setInputFileCallbackLinkFilesToFileAsData() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        bool doLinkFilesToFile(Containers::ArrayView<const std::pair<Stage, Containers::StringView>>, Containers::StringView) override {
            CORRADE_VERIFY(!"this shouldn't be reached");
            return {};
        }

        Containers::Array<char> doLinkDataToData(Containers::ArrayView<const std::pair<Stage, Containers::ArrayView<const char>>> data) override {
            CORRADE_COMPARE(data.size(), 2);
            return Containers::array({
                data[0].first == Stage::Vertex ? data[0].second[0] : ' ',
                data[1].first == Stage::Fragment ? data[1].second[0] : ' '
            });
        }
    } converter;

    struct State {
        const char first[2]{'V', 'E'};
        const char second[2]{'S', 'A'};
        std::string operations;
    } state;

    converter.setInputFileCallback([](const std::string& filename, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        if(policy == InputFileCallbackPolicy::LoadTemporary) {
            state.operations += "loaded " + filename + "\n";
            if(filename == "another.dat")
                return Containers::arrayView(state.first);
            if(filename == "file.dat")
                return Containers::arrayView(state.second);
        }

        if(policy == InputFileCallbackPolicy::Close) {
            state.operations += "closed " + filename + "\n";
            return {};
        }

        CORRADE_VERIFY(!"this shouldn't be reached");
        return {};
    }, state);

    /* Remove previous file, if any */
    const std::string filename = Utility::Directory::join(SHADERTOOLS_TEST_OUTPUT_DIR, "file.out");
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));

    CORRADE_VERIFY(converter.linkFilesToFile({
        {Stage::Vertex, "another.dat"},
        {Stage::Fragment, "file.dat"}
    }, filename));
    CORRADE_COMPARE(state.operations,
        "loaded another.dat\n"
        "loaded file.dat\n"
        "closed another.dat\n"
        "closed file.dat\n");
    CORRADE_COMPARE_AS(filename, "VS", TestSuite::Compare::FileToString);
}

void AbstractConverterTest::setInputFileCallbackLinkFilesToFileAsDataFailed() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        bool doLinkFilesToFile(Containers::ArrayView<const std::pair<Stage, Containers::StringView>>, Containers::StringView) override {
            CORRADE_VERIFY(!"this shouldn't be reached");
            return {};
        }
    } converter;

    struct State {
        const char data[1]{};
        std::string operations;
    } state;

    converter.setInputFileCallback([](const std::string& filename, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        if(policy == InputFileCallbackPolicy::LoadTemporary) {
            state.operations += "loaded " + filename + "\n";
            if(filename == "another.dat")
                return Containers::arrayView(state.data);
            /* This deliberately fails */
            if(filename == "file.dat") return {};
        }

        if(policy == InputFileCallbackPolicy::Close) {
            state.operations += "closed " + filename + "\n";
            return {};
        }

        CORRADE_VERIFY(!"this shouldn't be reached");
        return {};
    }, state);

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_VERIFY(!converter.linkFilesToFile({
        {Stage::Vertex, "another.dat"},
        {Stage::Fragment, "file.dat"}
    }, "/some/path/that/does/not/exist"));
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::linkFilesToFile(): cannot open file file.dat\n");
}

void AbstractConverterTest::setInputFileCallbackLinkFilesToFileAsDataNotWritable() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        bool doLinkFilesToFile(Containers::ArrayView<const std::pair<Stage, Containers::StringView>>, Containers::StringView) override {
            CORRADE_VERIFY(!"this shouldn't be reached");
            return {};
        }

        Containers::Array<char> doLinkDataToData(Containers::ArrayView<const std::pair<Stage, Containers::ArrayView<const char>>>) override {
            return Containers::Array<char>{1};
        }
    } converter;

    struct State {
        const char first[2]{'V', 'E'};
        const char second[2]{'S', 'A'};
        std::string operations;
    } state;

    converter.setInputFileCallback([](const std::string& filename, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        if(policy == InputFileCallbackPolicy::LoadTemporary) {
            state.operations += "loaded " + filename + "\n";
            if(filename == "another.dat")
                return Containers::arrayView(state.first);
            if(filename == "file.dat")
                return Containers::arrayView(state.second);
        }

        if(policy == InputFileCallbackPolicy::Close) {
            state.operations += "closed " + filename + "\n";
            return {};
        }

        CORRADE_VERIFY(!"this shouldn't be reached");
        return {};
    }, state);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.linkFilesToFile({
        {Stage::Vertex, "another.dat"},
        {Stage::Fragment, "file.dat"}
    }, "/some/path/that/does/not/exist"));
    CORRADE_COMPARE(state.operations,
        "loaded another.dat\n"
        "loaded file.dat\n"
        "closed another.dat\n"
        "closed file.dat\n");
    CORRADE_COMPARE(out.str(),
        "Utility::Directory::write(): can't open /some/path/that/does/not/exist\n"
        "ShaderTools::AbstractConverter::linkFilesToFile(): cannot write to file /some/path/that/does/not/exist\n");
}

void AbstractConverterTest::setInputFileCallbackLinkFilesToDataDirectly() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData|ConverterFeature::InputFileCallback;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doLinkFilesToData(Containers::ArrayView<const std::pair<Stage, Containers::StringView>> from) override {
            if(from.size() == 2 && from[0].first == Stage::Vertex && from[0].second == "another.dat" && from[1].first == Stage::Fragment && from[1].second == "file.dat" && inputFileCallback() && inputFileCallbackUserData())
                return Containers::array({'y', 'e', 'p'});
            return {};
        }

        Containers::Array<char> doLinkDataToData(Containers::ArrayView<const std::pair<Stage, Containers::ArrayView<const char>>>) override {
            CORRADE_VERIFY(!"this should not be reached");
            return {};
        }
    } converter;

    int a{};
    converter.setInputFileCallback([](const std::string&, InputFileCallbackPolicy, void*) {
        CORRADE_VERIFY(!"this should not be reached");
        return Containers::Optional<Containers::ArrayView<const char>>{};
    }, &a);

    CORRADE_COMPARE_AS(converter.linkFilesToData({
            {Stage::Vertex, "another.dat"},
            {Stage::Fragment, "file.dat"}
        }), Containers::arrayView({'y', 'e', 'p'}),
        TestSuite::Compare::Container);
}

void AbstractConverterTest::setInputFileCallbackLinkFilesToDataThroughBaseImplementation() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData|ConverterFeature::InputFileCallback;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doLinkFilesToData(Containers::ArrayView<const std::pair<Stage, Containers::StringView>> from) override {
            linkFilesToDataCalled = true;

            if(from.size() != 2 || from[0].first != Stage::Vertex || from[0].second != "another.dat" || from[1].first != Stage::Fragment || from[1].second != "file.dat" || !inputFileCallback() || !inputFileCallbackUserData())
                return {};

            return AbstractConverter::doLinkFilesToData(from);
        }

        Containers::Array<char> doLinkDataToData(Containers::ArrayView<const std::pair<Stage, Containers::ArrayView<const char>>> data) override {
            CORRADE_COMPARE(data.size(), 2);
            return Containers::array({
                data[0].first == Stage::Vertex ? data[0].second[0] : ' ',
                data[1].first == Stage::Fragment ? data[1].second[0] : ' '
            });
        }

        bool linkFilesToDataCalled = false;
    } converter;

    struct State {
        const char first[2]{'V', 'E'};
        const char second[2]{'S', 'A'};
        std::string operations;
    } state;

    converter.setInputFileCallback([](const std::string& filename, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        if(policy == InputFileCallbackPolicy::LoadTemporary) {
            state.operations += "loaded " + filename + "\n";
            if(filename == "another.dat")
                return Containers::arrayView(state.first);
            if(filename == "file.dat")
                return Containers::arrayView(state.second);
        }

        if(policy == InputFileCallbackPolicy::Close) {
            state.operations += "closed " + filename + "\n";
            return {};
        }

        CORRADE_VERIFY(!"this shouldn't be reached");
        return {};
    }, state);

    CORRADE_COMPARE_AS(converter.linkFilesToData({
            {Stage::Vertex, "another.dat"},
            {Stage::Fragment, "file.dat"}
        }), Containers::arrayView({'V', 'S'}),
        TestSuite::Compare::Container);
    CORRADE_VERIFY(converter.linkFilesToDataCalled);
    CORRADE_COMPARE(state.operations,
        "loaded another.dat\n"
        "loaded file.dat\n"
        "closed another.dat\n"
        "closed file.dat\n");
}

void AbstractConverterTest::setInputFileCallbackLinkFilesToDataThroughBaseImplementationFailed() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData|ConverterFeature::InputFileCallback;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doLinkFilesToData(Containers::ArrayView<const std::pair<Stage, Containers::StringView>> from) override {
            linkFilesToDataCalled = true;
            return AbstractConverter::doLinkFilesToData(from);
        }

        bool linkFilesToDataCalled = false;
    } converter;

    struct State {
        const char data[1]{};
        std::string operations;
    } state;

    converter.setInputFileCallback([](const std::string& filename, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        if(policy == InputFileCallbackPolicy::LoadTemporary) {
            state.operations += "loaded " + filename + "\n";
            if(filename == "another.dat")
                return Containers::arrayView(state.data);
            /* This deliberately fails */
            if(filename == "file.dat") return {};
        }

        if(policy == InputFileCallbackPolicy::Close) {
            state.operations += "closed " + filename + "\n";
            return {};
        }

        CORRADE_VERIFY(!"this shouldn't be reached");
        return {};
    }, state);

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_VERIFY(!converter.linkFilesToData({
        {Stage::Vertex, "another.dat"},
        {Stage::Fragment, "file.dat"}
    }));
    CORRADE_VERIFY(converter.linkFilesToDataCalled);
    CORRADE_COMPARE(state.operations,
        "loaded another.dat\n"
        "loaded file.dat\n" /* this fails */
        "closed another.dat\n");
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::linkFilesToData(): cannot open file file.dat\n");
}

void AbstractConverterTest::setInputFileCallbackLinkFilesToDataAsData() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doLinkFilesToData(Containers::ArrayView<const std::pair<Stage, Containers::StringView>>) override {
            CORRADE_VERIFY(!"this shouldn't be reached");
            return {};
        }

        Containers::Array<char> doLinkDataToData(Containers::ArrayView<const std::pair<Stage, Containers::ArrayView<const char>>> data) override {
            CORRADE_COMPARE(data.size(), 2);
            return Containers::array({
                data[0].first == Stage::Vertex ? data[0].second[0] : ' ',
                data[1].first == Stage::Fragment ? data[1].second[0] : ' '
            });
        }
    } converter;

    struct State {
        const char first[2]{'V', 'E'};
        const char second[2]{'S', 'A'};
        std::string operations;
    } state;

    converter.setInputFileCallback([](const std::string& filename, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        if(policy == InputFileCallbackPolicy::LoadTemporary) {
            state.operations += "loaded " + filename + "\n";
            if(filename == "another.dat")
                return Containers::arrayView(state.first);
            if(filename == "file.dat")
                return Containers::arrayView(state.second);
        }

        if(policy == InputFileCallbackPolicy::Close) {
            state.operations += "closed " + filename + "\n";
            return {};
        }

        CORRADE_VERIFY(!"this shouldn't be reached");
        return {};
    }, state);

    CORRADE_COMPARE_AS(converter.linkFilesToData({
            {Stage::Vertex, "another.dat"},
            {Stage::Fragment, "file.dat"}
        }), Containers::arrayView({'V', 'S'}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(state.operations,
        "loaded another.dat\n"
        "loaded file.dat\n"
        "closed another.dat\n"
        "closed file.dat\n");
}

void AbstractConverterTest::setInputFileCallbackLinkFilesToDataAsDataFailed() {
    struct: AbstractConverter {
        ConverterFeatures doFeatures() const override {
            return ConverterFeature::LinkData;
        }
        void doSetInputFormat(Format, Containers::StringView) override {}
        void doSetOutputFormat(Format, Containers::StringView) override {}

        Containers::Array<char> doLinkFilesToData(Containers::ArrayView<const std::pair<Stage, Containers::StringView>>) override {
            CORRADE_VERIFY(!"this shouldn't be reached");
            return {};
        }
    } converter;

    struct State {
        const char data[1]{};
        std::string operations;
    } state;

    converter.setInputFileCallback([](const std::string& filename, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        if(policy == InputFileCallbackPolicy::LoadTemporary) {
            state.operations += "loaded " + filename + "\n";
            if(filename == "another.dat")
                return Containers::arrayView(state.data);
            /* This deliberately fails */
            if(filename == "file.dat") return {};
        }

        if(policy == InputFileCallbackPolicy::Close) {
            state.operations += "closed " + filename + "\n";
            return {};
        }

        CORRADE_VERIFY(!"this shouldn't be reached");
        return {};
    }, state);

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_VERIFY(!converter.linkFilesToData({
        {Stage::Vertex, "another.dat"},
        {Stage::Fragment, "file.dat"}
    }));
    CORRADE_COMPARE(out.str(), "ShaderTools::AbstractConverter::linkFilesToData(): cannot open file file.dat\n");
}

void AbstractConverterTest::debugFeature() {
    std::ostringstream out;

    Debug{&out} << ConverterFeature::ConvertData << ConverterFeature(0xf0);
    CORRADE_COMPARE(out.str(), "ShaderTools::ConverterFeature::ConvertData ShaderTools::ConverterFeature(0xf0)\n");
}

void AbstractConverterTest::debugFeatures() {
    std::ostringstream out;

    Debug{&out} << (ConverterFeature::ValidateData|ConverterFeature::ConvertFile) << ConverterFeatures{};
    CORRADE_COMPARE(out.str(), "ShaderTools::ConverterFeature::ValidateData|ShaderTools::ConverterFeature::ConvertFile ShaderTools::ConverterFeatures{}\n");
}

void AbstractConverterTest::debugFlag() {
    std::ostringstream out;

    Debug{&out} << ConverterFlag::Verbose << ConverterFlag(0xf0);
    CORRADE_COMPARE(out.str(), "ShaderTools::ConverterFlag::Verbose ShaderTools::ConverterFlag(0xf0)\n");
}

void AbstractConverterTest::debugFlags() {
    std::ostringstream out;

    Debug{&out} << (ConverterFlag::Verbose|ConverterFlag(0xf0)) << ConverterFlags{};
    CORRADE_COMPARE(out.str(), "ShaderTools::ConverterFlag::Verbose|ShaderTools::ConverterFlag(0xf0) ShaderTools::ConverterFlags{}\n");
}

void AbstractConverterTest::debugFormat() {
    std::ostringstream out;

    Debug{&out} << Format::Glsl << Format(0xf0);
    CORRADE_COMPARE(out.str(), "ShaderTools::Format::Glsl ShaderTools::Format(0xf0)\n");
}

void AbstractConverterTest::debugStage() {
    std::ostringstream out;

    Debug{&out} << Stage::RayMiss << Stage(0xf0);
    CORRADE_COMPARE(out.str(), "ShaderTools::Stage::RayMiss ShaderTools::Stage(0xf0)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::ShaderTools::Test::AbstractConverterTest)
