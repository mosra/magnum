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
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringStl.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/ShaderTools/AbstractConverter.h"
#include "Magnum/ShaderTools/Stage.h"

#include "configure.h"

namespace Magnum { namespace ShaderTools { namespace Test { namespace {

struct AnyConverterTest: TestSuite::Tester {
    explicit AnyConverterTest();

    void validateFile();
    void validateFilePluginLoadFailed();
    void validateFileUnknown();
    void validateFileNotSupported();
    void validateFilePreprocessNotSupported();
    void validateFilePropagateFlags();
    void validateFilePropagateInputVersion();
    void validateFilePropagateOutputVersion();
    void validateFilePropagatePreprocess();

    void validateData();
    void validateDataPluginLoadFailed();
    void validateDataNoFormatSet();
    void validateDataNotSupported();
    void validateDataPreprocessNotSupported();
    void validateDataPropagateFlags();
    void validateDataPropagateInputVersion();
    void validateDataPropagateOutputVersion();
    void validateDataPropagatePreprocess();

    void convertFileToFile();
    void convertFileToFilePluginLoadFailed();
    void convertFileToFileUnknownInput();
    void convertFileToFileUnknownOutput();
    void convertFileToFileNotSupported();
    void convertFileToFilePreprocessNotSupported();
    void convertFileToFileDebugInfoNotSupported();
    void convertFileToFileOptimizationNotSupported();
    void convertFileToFilePropagateFlags();
    void convertFileToFilePropagateInputVersion();
    void convertFileToFilePropagateOutputVersion();
    void convertFileToFilePropagatePreprocess();
    void convertFileToFilePropagateDebugInfo();
    void convertFileToFilePropagateOptimization();

    void convertFileToData();
    void convertFileToDataPluginLoadFailed();
    void convertFileToDataUnknown();
    void convertFileToDataNoFormatSet();
    void convertFileToDataNotSupported();
    void convertFileToDataPreprocessNotSupported();
    void convertFileToDataDebugInfoNotSupported();
    void convertFileToDataOptimizationNotSupported();
    void convertFileToDataPropagateFlags();
    void convertFileToDataPropagateInputVersion();
    void convertFileToDataPropagateOutputVersion();
    void convertFileToDataPropagatePreprocess();
    void convertFileToDataPropagateDebugInfo();
    void convertFileToDataPropagateOptimization();

    void convertDataToData();
    void convertDataToDataPluginLoadFailed();
    void convertDataToDataNoInputFormatSet();
    void convertDataToDataNoOutputFormatSet();
    void convertDataToDataNotSupported();
    void convertDataToDataPreprocessNotSupported();
    void convertDataToDataDebugInfoNotSupported();
    void convertDataToDataOptimizationNotSupported();
    void convertDataToDataPropagateFlags();
    void convertDataToDataPropagateInputVersion();
    void convertDataToDataPropagateOutputVersion();
    void convertDataToDataPropagatePreprocess();
    void convertDataToDataPropagateDebugInfo();
    void convertDataToDataPropagateOptimization();

    void detectValidate();
    void detectValidateExplicitFormat();
    void detectConvert();
    void detectConvertExplicitFormat();

    /* Explicitly forbid system-wide plugin dependencies. Tests that need those
       have their own manager. */
    PluginManager::Manager<AbstractConverter> _manager{"nonexistent"};
};

constexpr struct {
    const char* name;
    const char* filename;
    const char* plugin;
} DetectValidateData[]{
    {"SPIR-V", "flat.spv", "SpirvShaderConverter"},
    {"SPIR-V assembly uppercase", "DOOM.SPVASM", "SpirvAssemblyShaderConverter"},
    {"SPIR-V assembly weird", "test.asm.rahit", "SpirvAssemblyShaderConverter"},
    {"GLSL explicit", "phong.glsl", "GlslShaderConverter"},
    {"GLSL implicit", "phong.frag", "GlslShaderConverter"},
};

constexpr struct {
    const char* name;
    const char* from;
    const char* to;
    const char* plugin;
} DetectConvertData[]{
    {"SPIR-V to SPIR-V", "flat.spv", "optimized.spv", "SpirvShaderConverter"},
    {"SPIR-V assembly to SPIR-V", "a.spvasm", "b.spv", "SpirvAssemblyToSpirvShaderConverter"},
    {"SPIR-V to GLSL", "phong.frag.spv", "phong.glsl", "SpirvToGlslShaderConverter"}
};

AnyConverterTest::AnyConverterTest() {
    addTests({&AnyConverterTest::validateFile,
              &AnyConverterTest::validateFilePluginLoadFailed,
              &AnyConverterTest::validateFileUnknown,
              &AnyConverterTest::validateFileNotSupported,
              &AnyConverterTest::validateFilePreprocessNotSupported,
              &AnyConverterTest::validateFilePropagateFlags,
              &AnyConverterTest::validateFilePropagateInputVersion,
              &AnyConverterTest::validateFilePropagateOutputVersion,
              &AnyConverterTest::validateFilePropagatePreprocess,

              &AnyConverterTest::validateData,
              &AnyConverterTest::validateDataPluginLoadFailed,
              &AnyConverterTest::validateDataNoFormatSet,
              &AnyConverterTest::validateDataNotSupported,
              &AnyConverterTest::validateDataPreprocessNotSupported,
              &AnyConverterTest::validateDataPropagateFlags,
              &AnyConverterTest::validateDataPropagateInputVersion,
              &AnyConverterTest::validateDataPropagateOutputVersion,
              &AnyConverterTest::validateDataPropagatePreprocess,

              &AnyConverterTest::convertFileToFile,
              &AnyConverterTest::convertFileToFilePluginLoadFailed,
              &AnyConverterTest::convertFileToFileUnknownInput,
              &AnyConverterTest::convertFileToFileUnknownOutput,
              &AnyConverterTest::convertFileToFileNotSupported,
              &AnyConverterTest::convertFileToFilePreprocessNotSupported,
              &AnyConverterTest::convertFileToFileDebugInfoNotSupported,
              &AnyConverterTest::convertFileToFileOptimizationNotSupported,
              &AnyConverterTest::convertFileToFilePropagateFlags,
              &AnyConverterTest::convertFileToFilePropagateInputVersion,
              &AnyConverterTest::convertFileToFilePropagateOutputVersion,
              &AnyConverterTest::convertFileToFilePropagatePreprocess,
              &AnyConverterTest::convertFileToFilePropagateDebugInfo,
              &AnyConverterTest::convertFileToFilePropagateOptimization,

              &AnyConverterTest::convertFileToData,
              &AnyConverterTest::convertFileToDataPluginLoadFailed,
              &AnyConverterTest::convertFileToDataUnknown,
              &AnyConverterTest::convertFileToDataNoFormatSet,
              &AnyConverterTest::convertFileToDataNotSupported,
              &AnyConverterTest::convertFileToDataPreprocessNotSupported,
              &AnyConverterTest::convertFileToDataDebugInfoNotSupported,
              &AnyConverterTest::convertFileToDataOptimizationNotSupported,
              &AnyConverterTest::convertFileToDataPropagateFlags,
              &AnyConverterTest::convertFileToDataPropagateInputVersion,
              &AnyConverterTest::convertFileToDataPropagateOutputVersion,
              &AnyConverterTest::convertFileToDataPropagatePreprocess,
              &AnyConverterTest::convertFileToDataPropagateDebugInfo,
              &AnyConverterTest::convertFileToDataPropagateOptimization,

              &AnyConverterTest::convertDataToData,
              &AnyConverterTest::convertDataToDataPluginLoadFailed,
              &AnyConverterTest::convertDataToDataNoInputFormatSet,
              &AnyConverterTest::convertDataToDataNoOutputFormatSet,
              &AnyConverterTest::convertDataToDataNotSupported,
              &AnyConverterTest::convertDataToDataPreprocessNotSupported,
              &AnyConverterTest::convertDataToDataDebugInfoNotSupported,
              &AnyConverterTest::convertDataToDataOptimizationNotSupported,
              &AnyConverterTest::convertDataToDataPropagateFlags,
              &AnyConverterTest::convertDataToDataPropagateInputVersion,
              &AnyConverterTest::convertDataToDataPropagateOutputVersion,
              &AnyConverterTest::convertDataToDataPropagatePreprocess,
              &AnyConverterTest::convertDataToDataPropagateDebugInfo,
              &AnyConverterTest::convertDataToDataPropagateOptimization});

    addInstancedTests({&AnyConverterTest::detectValidate},
        Containers::arraySize(DetectValidateData));

    addTests({&AnyConverterTest::detectValidateExplicitFormat});

    addInstancedTests({&AnyConverterTest::detectConvert},
        Containers::arraySize(DetectConvertData));

    addTests({&AnyConverterTest::detectConvertExplicitFormat});

    /* Load the plugin directly from the build tree. Otherwise it's static and
       already loaded. */
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Create the output directory if it doesn't exist yet */
    CORRADE_INTERNAL_ASSERT_OUTPUT(Utility::Directory::mkpath(ANYSHADERCONVERTER_TEST_OUTPUT_DIR));
}

void AnyConverterTest::validateFile() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    const std::string filename = Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl");

    /* Make it print a warning so we know it's doing something */
    CORRADE_COMPARE(converter->validateFile(Stage::Fragment, filename),
        std::make_pair(true, Utility::formatString("WARNING: {}:10: 'reserved__identifier' : identifiers containing consecutive underscores (\"__\") are reserved", filename)));
}

void AnyConverterTest::validateFilePluginLoadFailed() {
    Containers::Pointer<AbstractConverter> converter = _manager.instantiate("AnyShaderConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_COMPARE(converter->validateFile({}, "file.glsl"),
        std::make_pair(false, ""));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out.str(),
        "PluginManager::Manager::load(): plugin GlslShaderConverter is not static and was not found in nonexistent\n"
        "ShaderTools::AnyConverter::validateFile(): cannot load the GlslShaderConverter plugin\n");
    #else
    CORRADE_COMPARE(out.str(),
        "PluginManager::Manager::load(): plugin GlslShaderConverter was not found\n"
        "ShaderTools::AnyConverter::validateFile(): cannot load the GlslShaderConverter plugin\n");
    #endif
}

void AnyConverterTest::validateFileUnknown() {
    Containers::Pointer<AbstractConverter> converter = _manager.instantiate("AnyShaderConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_COMPARE(converter->validateFile({}, "dead.cg"),
        std::make_pair(false, ""));
    CORRADE_COMPARE(out.str(), "ShaderTools::AnyConverter::validateFile(): cannot determine the format of dead.cg\n");
}

void AnyConverterTest::validateFileNotSupported() {
    CORRADE_SKIP("No plugin that would support just validation exists.");
}

void AnyConverterTest::validateFilePreprocessNotSupported() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("SpirvToolsShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("SpirvToolsShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    converter->setDefinitions({
        {"DEFINE", "hahahahah"}
    });

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_COMPARE(converter->validateFile({}, Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.spv")),
        std::make_pair(false, ""));
    CORRADE_COMPARE(out.str(),
        "ShaderTools::AnyConverter::validateFile(): SpirvToolsShaderConverter does not support preprocessing\n");
}

void AnyConverterTest::validateFilePropagateFlags() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    const std::string filename = Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl");

    /* With this, the warning should turn into an error. The converter should
       also print the verbose info. */
    converter->setFlags(ConverterFlag::Verbose|ConverterFlag::WarningAsError);

    std::ostringstream out;
    Debug redirectDebug{&out};
    CORRADE_COMPARE(converter->validateFile(Stage::Fragment, filename),
        std::make_pair(false, Utility::formatString("WARNING: {}:10: 'reserved__identifier' : identifiers containing consecutive underscores (\"__\") are reserved", filename)));
    CORRADE_COMPARE(out.str(),
        "ShaderTools::AnyConverter::validateFile(): using GlslShaderConverter (provided by GlslangShaderConverter)\n");
}

void AnyConverterTest::validateFilePropagateInputVersion() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    /* This is an invalid version. We have to supply a valid file path because
       the version gets checked in doValidateData(), called from
       AbstractConverter::doValidateFile() with the file contents. */
    converter->setInputFormat(Format::Glsl, "100");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_COMPARE(converter->validateFile(Stage::Fragment, Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl")),
        std::make_pair(false, ""));
    CORRADE_COMPARE(out.str(),
        "ShaderTools::GlslangConverter::validateData(): input format version should be one of supported GLSL #version strings but got 100\n");
}

void AnyConverterTest::validateFilePropagateOutputVersion() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    /* This is an invalid version. We have to supply a valid file path because
       the version gets checked in doValidateData(), called from
       AbstractConverter::doValidateFile() with the file contents. */
    converter->setOutputFormat(Format::Glsl, "opengl4.0");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_COMPARE(converter->validateFile(Stage::Fragment, Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl")),
        std::make_pair(false, ""));
    CORRADE_COMPARE(out.str(),
        "ShaderTools::GlslangConverter::validateData(): output format should be Spirv or Unspecified but got ShaderTools::Format::Glsl\n");
}

void AnyConverterTest::validateFilePropagatePreprocess() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    const std::string filename = Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl");

    /* Check that undefining works properly -- if it stays defined, the source
       won't compile */
    converter->setDefinitions({
        {"SHOULD_BE_UNDEFINED", "really"},
        {"SHOULD_BE_UNDEFINED", nullptr},
        {"reserved__identifier", "different__but_also_wrong"}
    });

    CORRADE_COMPARE(converter->validateFile(Stage::Fragment, filename),
        std::make_pair(true, Utility::formatString("WARNING: {}:10: 'different__but_also_wrong' : identifiers containing consecutive underscores (\"__\") are reserved", filename)));
}

void AnyConverterTest::validateData() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    converter->setInputFormat(Format::Glsl);

    /* Make it print a warning so we know it's doing something */
    CORRADE_COMPARE(converter->validateData(Stage::Fragment, Utility::Directory::read(Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl"))),
        std::make_pair(true, "WARNING: 0:10: 'reserved__identifier' : identifiers containing consecutive underscores (\"__\") are reserved"));
}

void AnyConverterTest::validateDataPluginLoadFailed() {
    Containers::Pointer<AbstractConverter> converter = _manager.instantiate("AnyShaderConverter");

    converter->setInputFormat(Format::Glsl);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_COMPARE(converter->validateData({}, {}),
        std::make_pair(false, ""));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out.str(),
        "PluginManager::Manager::load(): plugin GlslShaderConverter is not static and was not found in nonexistent\n"
        "ShaderTools::AnyConverter::validateData(): cannot load the GlslShaderConverter plugin\n");
    #else
    CORRADE_COMPARE(out.str(),
        "PluginManager::Manager::load(): plugin GlslShaderConverter was not found\n"
        "ShaderTools::AnyConverter::validateData(): cannot load the GlslShaderConverter plugin\n");
    #endif
}

void AnyConverterTest::validateDataNoFormatSet() {
    Containers::Pointer<AbstractConverter> converter = _manager.instantiate("AnyShaderConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_COMPARE(converter->validateData({}, "dead.cg"),
        std::make_pair(false, ""));
    CORRADE_COMPARE(out.str(), "ShaderTools::AnyConverter::validateData(): no input format specified\n");
}

void AnyConverterTest::validateDataNotSupported() {
    CORRADE_SKIP("No plugin that would support just validation exists.");
}

void AnyConverterTest::validateDataPreprocessNotSupported() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("SpirvToolsShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("SpirvToolsShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    converter->setInputFormat(Format::Spirv);

    converter->setDefinitions({
        {"DEFINE", "hahahahah"}
    });

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_COMPARE(converter->validateData({}, Utility::Directory::read(Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.spv"))),
        std::make_pair(false, ""));
    CORRADE_COMPARE(out.str(),
        "ShaderTools::AnyConverter::validateData(): SpirvToolsShaderConverter does not support preprocessing\n");
}

void AnyConverterTest::validateDataPropagateFlags() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    converter->setInputFormat(Format::Glsl);

    /* With this, the warning should turn into an error. The converter should
       also print the verbose info. */
    converter->setFlags(ConverterFlag::Verbose|ConverterFlag::WarningAsError);

    std::ostringstream out;
    Debug redirectDebug{&out};
    CORRADE_COMPARE(converter->validateData(Stage::Fragment, Utility::Directory::read(Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl"))),
        std::make_pair(false, "WARNING: 0:10: 'reserved__identifier' : identifiers containing consecutive underscores (\"__\") are reserved"));
    CORRADE_COMPARE(out.str(),
        "ShaderTools::AnyConverter::validateData(): using GlslShaderConverter (provided by GlslangShaderConverter)\n");
}

void AnyConverterTest::validateDataPropagateInputVersion() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    converter->setInputFormat(Format::Glsl);

    /* This is an invalid version. We have to supply a valid file path because
       the version gets checked in doValidateData(), called from
       AbstractConverter::doValidateFile() with the file contents. */
    converter->setInputFormat(Format::Glsl, "100");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_COMPARE(converter->validateData(Stage::Fragment, Utility::Directory::read(Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl"))),
        std::make_pair(false, ""));
    CORRADE_COMPARE(out.str(),
        "ShaderTools::GlslangConverter::validateData(): input format version should be one of supported GLSL #version strings but got 100\n");
}

void AnyConverterTest::validateDataPropagateOutputVersion() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    converter->setInputFormat(Format::Glsl);

    /* This is an invalid version. We have to supply a valid file path because
       the version gets checked in doValidateData(), called from
       AbstractConverter::doValidateFile() with the file contents. */
    converter->setOutputFormat(Format::Glsl, "opengl4.0");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_COMPARE(converter->validateData(Stage::Fragment, Utility::Directory::read(Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl"))),
        std::make_pair(false, ""));
    CORRADE_COMPARE(out.str(),
        "ShaderTools::GlslangConverter::validateData(): output format should be Spirv or Unspecified but got ShaderTools::Format::Glsl\n");
}

void AnyConverterTest::validateDataPropagatePreprocess() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    converter->setInputFormat(Format::Glsl);

    /* Check that undefining works properly -- if it stays defined, the source
       won't compile */
    converter->setDefinitions({
        {"SHOULD_BE_UNDEFINED", "really"},
        {"SHOULD_BE_UNDEFINED", nullptr},
        {"reserved__identifier", "different__but_also_wrong"}
    });

    CORRADE_COMPARE(converter->validateData(Stage::Fragment, Utility::Directory::read(Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl"))),
        std::make_pair(true, "WARNING: 0:10: 'different__but_also_wrong' : identifiers containing consecutive underscores (\"__\") are reserved"));
}

void AnyConverterTest::convertFileToFile() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    const std::string inputFilename = Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl");
    const std::string outputFilename = Utility::Directory::join(ANYSHADERCONVERTER_TEST_OUTPUT_DIR, "file.spv");
    Utility::Directory::rm(outputFilename);
    CORRADE_VERIFY(!Utility::Directory::exists(outputFilename));

    /* Make it print a warning so we know it's doing something */
    std::ostringstream out;
    Warning redirectWarning{&out};
    CORRADE_VERIFY(converter->convertFileToFile(Stage::Fragment, inputFilename, outputFilename));
    CORRADE_VERIFY(Utility::Directory::exists(outputFilename));
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "ShaderTools::GlslangConverter::convertDataToData(): compilation succeeded with the following message:\n"
        "WARNING: {}:10: 'reserved__identifier' : identifiers containing consecutive underscores (\"__\") are reserved\n", inputFilename));
}

void AnyConverterTest::convertFileToFilePluginLoadFailed() {
    Containers::Pointer<AbstractConverter> converter = _manager.instantiate("AnyShaderConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertFileToFile({}, "file.spv", "file.glsl"));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out.str(),
        "PluginManager::Manager::load(): plugin SpirvToGlslShaderConverter is not static and was not found in nonexistent\n"
        "ShaderTools::AnyConverter::convertFileToFile(): cannot load the SpirvToGlslShaderConverter plugin\n");
    #else
    CORRADE_COMPARE(out.str(),
        "PluginManager::Manager::load(): plugin SpirvToGlslShaderConverter was not found\n"
        "ShaderTools::AnyConverter::convertFileToFile(): cannot load the SpirvToGlslShaderConverter plugin\n");
    #endif
}

void AnyConverterTest::convertFileToFileUnknownInput() {
    Containers::Pointer<AbstractConverter> converter = _manager.instantiate("AnyShaderConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertFileToFile({}, "dead.cg", "whatever.osl"));
    CORRADE_COMPARE(out.str(), "ShaderTools::AnyConverter::convertFileToFile(): cannot determine the format of dead.cg\n");
}

void AnyConverterTest::convertFileToFileUnknownOutput() {
    Containers::Pointer<AbstractConverter> converter = _manager.instantiate("AnyShaderConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertFileToFile({}, "file.spv", "whatever.osl"));
    CORRADE_COMPARE(out.str(), "ShaderTools::AnyConverter::convertFileToFile(): cannot determine the format of whatever.osl\n");
}

void AnyConverterTest::convertFileToFileNotSupported() {
    CORRADE_SKIP("No plugin that would support just conversion exists.");
}

void AnyConverterTest::convertFileToFilePreprocessNotSupported() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("SpirvToolsShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("SpirvToolsShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    converter->setDefinitions({
        {"DEFINE", "hahahahah"}
    });

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertFileToFile({}, Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.spv"),
    Utility::Directory::join(ANYSHADERCONVERTER_TEST_OUTPUT_DIR, "file.spvasm")));
    CORRADE_COMPARE(out.str(),
        "ShaderTools::AnyConverter::convertFileToFile(): SpirvToolsShaderConverter does not support preprocessing\n");

    /* It should fail for the flag as well */
    out.str({});
    converter->setDefinitions({});
    converter->setFlags(ConverterFlag::PreprocessOnly);
    CORRADE_VERIFY(!converter->convertFileToFile({}, Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.spv"),
    Utility::Directory::join(ANYSHADERCONVERTER_TEST_OUTPUT_DIR, "file.spvasm")));
    CORRADE_COMPARE(out.str(),
        "ShaderTools::AnyConverter::convertFileToFile(): SpirvToolsShaderConverter does not support preprocessing\n");
}

void AnyConverterTest::convertFileToFileDebugInfoNotSupported() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("SpirvToolsShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("SpirvToolsShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    converter->setDebugInfoLevel("1");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertFileToFile({}, Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.spv"),
    Utility::Directory::join(ANYSHADERCONVERTER_TEST_OUTPUT_DIR, "file.spvasm")));
    /** @todo it once may support that, in which case we need to find another
        victim */
    CORRADE_COMPARE(out.str(),
        "ShaderTools::AnyConverter::convertFileToFile(): SpirvToolsShaderConverter does not support controlling debug info output\n");
}

void AnyConverterTest::convertFileToFileOptimizationNotSupported() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    converter->setOptimizationLevel("1");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertFileToFile({}, Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl"),
    Utility::Directory::join(ANYSHADERCONVERTER_TEST_OUTPUT_DIR, "file.spv")));
    /** @todo it once may support that, in which case we need to find another
        victim */
    CORRADE_COMPARE(out.str(),
        "ShaderTools::AnyConverter::convertFileToFile(): GlslangShaderConverter does not support optimization\n");
}

void AnyConverterTest::convertFileToFilePropagateFlags() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    const std::string filename = Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl");

    /* With this, the warning should turn into an error. The converter should
       also print the verbose info. */
    converter->setFlags(ConverterFlag::Verbose|ConverterFlag::WarningAsError);

    /* We have to supply a valid file path because the version gets checked in
       doConvertDataToData(), called from AbstractConverter::doConvertFileToFile()
       with the file contents. */
    std::ostringstream out;
    Debug redirectDebug{&out};
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertFileToFile(Stage::Fragment, filename, Utility::Directory::join(ANYSHADERCONVERTER_TEST_OUTPUT_DIR, "file.spv")));
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "ShaderTools::AnyConverter::convertFileToFile(): using GlslToSpirvShaderConverter (provided by GlslangShaderConverter)\n"
        "ShaderTools::GlslangConverter::convertDataToData(): compilation failed:\n"
        "WARNING: {}:10: 'reserved__identifier' : identifiers containing consecutive underscores (\"__\") are reserved\n", filename));
}

void AnyConverterTest::convertFileToFilePropagateInputVersion() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    /* This is an invalid version */
    converter->setInputFormat(Format::Glsl, "100");

    /* We have to supply a valid file path because the version gets checked in
       doConvertDataToData(), called from AbstractConverter::doConvertFileToFile()
       with the file contents. */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertFileToFile(Stage::Fragment, Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl"), Utility::Directory::join(ANYSHADERCONVERTER_TEST_OUTPUT_DIR, "file.spv")));
    CORRADE_COMPARE(out.str(),
        "ShaderTools::GlslangConverter::convertDataToData(): input format version should be one of supported GLSL #version strings but got 100\n");
}

void AnyConverterTest::convertFileToFilePropagateOutputVersion() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    /* This is an invalid version */
    converter->setOutputFormat(Format::Spirv, "opengl4.0");

    /* We have to supply a valid file path because the version gets checked in
       doConvertDataToData(), called from AbstractConverter::doConvertFileToFile()
       with the file contents. */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertFileToFile(Stage::Fragment, Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl"), Utility::Directory::join(ANYSHADERCONVERTER_TEST_OUTPUT_DIR, "file.spv")));
    CORRADE_COMPARE(out.str(),
        "ShaderTools::GlslangConverter::convertDataToData(): output format version target should be opengl4.5 or vulkanX.Y but got opengl4.0\n");
}

void AnyConverterTest::convertFileToFilePropagatePreprocess() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    /* Check that undefining works properly -- if it stays defined, the source
       won't compile */
    converter->setDefinitions({
        {"SHOULD_BE_UNDEFINED", "really"},
        {"SHOULD_BE_UNDEFINED", nullptr},
        {"reserved__identifier", "different__but_also_wrong"}
    });

    const std::string inputFilename = Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl");
    const std::string outputFilename = Utility::Directory::join(ANYSHADERCONVERTER_TEST_OUTPUT_DIR, "file.spv");
    Utility::Directory::rm(outputFilename);
    CORRADE_VERIFY(!Utility::Directory::exists(outputFilename));

    /* Make it print a warning so we know it's doing something */
    std::ostringstream out;
    Warning redirectWarning{&out};
    CORRADE_VERIFY(converter->convertFileToFile(Stage::Fragment, inputFilename, outputFilename));
    CORRADE_VERIFY(Utility::Directory::exists(outputFilename));
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "ShaderTools::GlslangConverter::convertDataToData(): compilation succeeded with the following message:\n"
        "WARNING: {}:10: 'different__but_also_wrong' : identifiers containing consecutive underscores (\"__\") are reserved\n", inputFilename));
}

void AnyConverterTest::convertFileToFilePropagateDebugInfo() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    /* This is an invalid level */
    converter->setDebugInfoLevel("2");

    /* We have to supply a valid file path because the version gets checked in
       doConvertDataToData(), called from AbstractConverter::doConvertFileToFile()
       with the file contents. */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertFileToFile(Stage::Fragment, Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl"), Utility::Directory::join(ANYSHADERCONVERTER_TEST_OUTPUT_DIR, "file.spv")));
    CORRADE_COMPARE(out.str(),
        "ShaderTools::GlslangConverter::convertDataToData(): debug info level should be 0, 1 or empty but got 2\n");
}

void AnyConverterTest::convertFileToFilePropagateOptimization() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("SpirvToolsShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("SpirvToolsShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    /* This is an invalid level */
    converter->setOptimizationLevel("2");

    /* We have to supply a valid file path because the version gets checked in
       doConvertDataToData(), called from AbstractConverter::doConvertFileToFile()
       with the file contents. */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertFileToFile(Stage::Fragment, Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.spv"), Utility::Directory::join(ANYSHADERCONVERTER_TEST_OUTPUT_DIR, "file.spv")));
    CORRADE_COMPARE(out.str(),
        "ShaderTools::SpirvToolsConverter::convertDataToData(): optimization level should be 0, 1, s, legalizeHlsl, vulkanToWebGpu, webGpuToVulkan or empty but got 2\n");
}

void AnyConverterTest::convertFileToData() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    converter->setOutputFormat(Format::Spirv);

    const std::string inputFilename = Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl");

    /* Make it print a warning so we know it's doing something */
    std::ostringstream out;
    Warning redirectWarning{&out};
    CORRADE_VERIFY(converter->convertFileToData(Stage::Fragment, inputFilename));
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "ShaderTools::GlslangConverter::convertDataToData(): compilation succeeded with the following message:\n"
        "WARNING: {}:10: 'reserved__identifier' : identifiers containing consecutive underscores (\"__\") are reserved\n", inputFilename));
}

void AnyConverterTest::convertFileToDataPluginLoadFailed() {
    Containers::Pointer<AbstractConverter> converter = _manager.instantiate("AnyShaderConverter");

    converter->setOutputFormat(Format::Wgsl);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertFileToData({}, "file.spv"));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out.str(),
        "PluginManager::Manager::load(): plugin SpirvToWgslShaderConverter is not static and was not found in nonexistent\n"
        "ShaderTools::AnyConverter::convertFileToData(): cannot load the SpirvToWgslShaderConverter plugin\n");
    #else
    CORRADE_COMPARE(out.str(),
        "PluginManager::Manager::load(): plugin SpirvToWgslShaderConverter was not found\n"
        "ShaderTools::AnyConverter::convertFileToData(): cannot load the SpirvToWgslShaderConverter plugin\n");
    #endif
}

void AnyConverterTest::convertFileToDataUnknown() {
    Containers::Pointer<AbstractConverter> converter = _manager.instantiate("AnyShaderConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertFileToData({}, "dead.cg"));
    CORRADE_COMPARE(out.str(), "ShaderTools::AnyConverter::convertFileToData(): cannot determine the format of dead.cg\n");
}

void AnyConverterTest::convertFileToDataNoFormatSet() {
    Containers::Pointer<AbstractConverter> converter = _manager.instantiate("AnyShaderConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertFileToData({}, "file.spv"));
    CORRADE_COMPARE(out.str(), "ShaderTools::AnyConverter::convertFileToData(): no output format specified\n");
}

void AnyConverterTest::convertFileToDataNotSupported() {
    CORRADE_SKIP("No plugin that would support just conversion exists.");
}

void AnyConverterTest::convertFileToDataPreprocessNotSupported() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("SpirvToolsShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("SpirvToolsShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    converter->setOutputFormat(Format::SpirvAssembly);

    converter->setDefinitions({
        {"DEFINE", "hahahahah"}
    });

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertFileToData({}, Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.spv")));
    CORRADE_COMPARE(out.str(),
        "ShaderTools::AnyConverter::convertFileToData(): SpirvToolsShaderConverter does not support preprocessing\n");

    /* It should fail for the flag as well */
    out.str({});
    converter->setDefinitions({});
    converter->setFlags(ConverterFlag::PreprocessOnly);
    CORRADE_VERIFY(!converter->convertFileToData({}, Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.spv")));
    CORRADE_COMPARE(out.str(),
        "ShaderTools::AnyConverter::convertFileToData(): SpirvToolsShaderConverter does not support preprocessing\n");
}

void AnyConverterTest::convertFileToDataDebugInfoNotSupported() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("SpirvToolsShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("SpirvToolsShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    converter->setOutputFormat(Format::SpirvAssembly);

    converter->setDebugInfoLevel("1");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertFileToData({}, Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.spv")));
    /** @todo it once may support that, in which case we need to find another
        victim */
    CORRADE_COMPARE(out.str(),
        "ShaderTools::AnyConverter::convertFileToData(): SpirvToolsShaderConverter does not support controlling debug info output\n");
}

void AnyConverterTest::convertFileToDataOptimizationNotSupported() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    converter->setOutputFormat(Format::Spirv);

    converter->setOptimizationLevel("1");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertFileToData({}, Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl")));
    /** @todo it once may support that, in which case we need to find another
        victim */
    CORRADE_COMPARE(out.str(),
        "ShaderTools::AnyConverter::convertFileToData(): GlslangShaderConverter does not support optimization\n");
}

void AnyConverterTest::convertFileToDataPropagateFlags() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    const std::string filename = Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl");

    converter->setOutputFormat(Format::Spirv);

    /* With this, the warning should turn into an error. The converter should
       also print the verbose info. */
    converter->setFlags(ConverterFlag::Verbose|ConverterFlag::WarningAsError);

    /* We have to supply a valid file path because the version gets checked in
       doConvertDataToData(), called from AbstractConverter::doConvertFileToFile()
       with the file contents. */
    std::ostringstream out;
    Debug redirectDebug{&out};
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertFileToData(Stage::Fragment, filename));
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "ShaderTools::AnyConverter::convertFileToData(): using GlslToSpirvShaderConverter (provided by GlslangShaderConverter)\n"
        "ShaderTools::GlslangConverter::convertDataToData(): compilation failed:\n"
        "WARNING: {}:10: 'reserved__identifier' : identifiers containing consecutive underscores (\"__\") are reserved\n", filename));
}

void AnyConverterTest::convertFileToDataPropagateInputVersion() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    /* This is an invalid version */
    converter->setInputFormat(Format::Glsl, "100");

    converter->setOutputFormat(Format::Spirv);

    /* We have to supply a valid file path because the version gets checked in
       doConvertDataToData(), called from AbstractConverter::doConvertFileToFile()
       with the file contents. */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertFileToData(Stage::Fragment, Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl")));
    CORRADE_COMPARE(out.str(),
        "ShaderTools::GlslangConverter::convertDataToData(): input format version should be one of supported GLSL #version strings but got 100\n");
}

void AnyConverterTest::convertFileToDataPropagateOutputVersion() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    /* This is an invalid version */
    converter->setOutputFormat(Format::Spirv, "opengl4.0");

    /* We have to supply a valid file path because the version gets checked in
       doConvertDataToData(), called from AbstractConverter::doConvertFileToFile()
       with the file contents. */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertFileToData(Stage::Fragment, Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl")));
    CORRADE_COMPARE(out.str(),
        "ShaderTools::GlslangConverter::convertDataToData(): output format version target should be opengl4.5 or vulkanX.Y but got opengl4.0\n");
}

void AnyConverterTest::convertFileToDataPropagatePreprocess() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    converter->setOutputFormat(Format::Spirv);

    /* Check that undefining works properly -- if it stays defined, the source
       won't compile */
    converter->setDefinitions({
        {"SHOULD_BE_UNDEFINED", "really"},
        {"SHOULD_BE_UNDEFINED", nullptr},
        {"reserved__identifier", "different__but_also_wrong"}
    });

    const std::string inputFilename = Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl");

    /* Make it print a warning so we know it's doing something */
    std::ostringstream out;
    Warning redirectWarning{&out};
    CORRADE_VERIFY(converter->convertFileToData(Stage::Fragment, inputFilename));
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "ShaderTools::GlslangConverter::convertDataToData(): compilation succeeded with the following message:\n"
        "WARNING: {}:10: 'different__but_also_wrong' : identifiers containing consecutive underscores (\"__\") are reserved\n", inputFilename));
}

void AnyConverterTest::convertFileToDataPropagateDebugInfo() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    converter->setOutputFormat(Format::Spirv);

    /* This is an invalid level */
    converter->setDebugInfoLevel("2");

    /* We have to supply a valid file path because the version gets checked in
       doConvertDataToData(), called from AbstractConverter::doConvertFileToFile()
       with the file contents. */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertFileToData(Stage::Fragment, Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl")));
    CORRADE_COMPARE(out.str(),
        "ShaderTools::GlslangConverter::convertDataToData(): debug info level should be 0, 1 or empty but got 2\n");
}

void AnyConverterTest::convertFileToDataPropagateOptimization() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("SpirvToolsShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("SpirvToolsShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    converter->setOutputFormat(Format::Spirv);

    /* This is an invalid level */
    converter->setOptimizationLevel("2");

    /* We have to supply a valid file path because the version gets checked in
       doConvertDataToData(), called from AbstractConverter::doConvertFileToFile()
       with the file contents. */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertFileToData(Stage::Fragment, Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.spv")));
    CORRADE_COMPARE(out.str(),
        "ShaderTools::SpirvToolsConverter::convertDataToData(): optimization level should be 0, 1, s, legalizeHlsl, vulkanToWebGpu, webGpuToVulkan or empty but got 2\n");
}

void AnyConverterTest::convertDataToData() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    converter->setInputFormat(Format::Glsl);
    converter->setOutputFormat(Format::Spirv);

    /* Make it print a warning so we know it's doing something */
    std::ostringstream out;
    Warning redirectWarning{&out};
    CORRADE_VERIFY(converter->convertDataToData(Stage::Fragment, Utility::Directory::read(Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl"))));
    CORRADE_COMPARE(out.str(),
        "ShaderTools::GlslangConverter::convertDataToData(): compilation succeeded with the following message:\n"
        "WARNING: 0:10: 'reserved__identifier' : identifiers containing consecutive underscores (\"__\") are reserved\n");
}

void AnyConverterTest::convertDataToDataPluginLoadFailed() {
    Containers::Pointer<AbstractConverter> converter = _manager.instantiate("AnyShaderConverter");

    converter->setInputFormat(Format::Hlsl);
    converter->setOutputFormat(Format::Wgsl);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertDataToData({}, {}));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out.str(),
        "PluginManager::Manager::load(): plugin HlslToWgslShaderConverter is not static and was not found in nonexistent\n"
        "ShaderTools::AnyConverter::convertDataToData(): cannot load the HlslToWgslShaderConverter plugin\n");
    #else
    CORRADE_COMPARE(out.str(),
        "PluginManager::Manager::load(): plugin HlslToWgslShaderConverter was not found\n"
        "ShaderTools::AnyConverter::convertDataToData(): cannot load the HlslToWgslShaderConverter plugin\n");
    #endif
}

void AnyConverterTest::convertDataToDataNoInputFormatSet() {
    Containers::Pointer<AbstractConverter> converter = _manager.instantiate("AnyShaderConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertDataToData({}, {}));
    CORRADE_COMPARE(out.str(), "ShaderTools::AnyConverter::convertDataToData(): no input format specified\n");
}

void AnyConverterTest::convertDataToDataNoOutputFormatSet() {
    Containers::Pointer<AbstractConverter> converter = _manager.instantiate("AnyShaderConverter");

    converter->setInputFormat(Format::Spirv);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertDataToData({}, {}));
    CORRADE_COMPARE(out.str(), "ShaderTools::AnyConverter::convertDataToData(): no output format specified\n");
}

void AnyConverterTest::convertDataToDataNotSupported() {
    CORRADE_SKIP("No plugin that would support just conversion exists.");
}

void AnyConverterTest::convertDataToDataPreprocessNotSupported() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("SpirvToolsShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("SpirvToolsShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    converter->setInputFormat(Format::Spirv);
    converter->setOutputFormat(Format::SpirvAssembly);

    converter->setDefinitions({
        {"DEFINE", "hahahahah"}
    });

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertDataToData({}, Utility::Directory::read(Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.spv"))));
    CORRADE_COMPARE(out.str(),
        "ShaderTools::AnyConverter::convertDataToData(): SpirvToolsShaderConverter does not support preprocessing\n");

    /* It should fail for the flag as well */
    out.str({});
    converter->setDefinitions({});
    converter->setFlags(ConverterFlag::PreprocessOnly);
    CORRADE_VERIFY(!converter->convertDataToData({}, Utility::Directory::read(Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.spv"))));
    CORRADE_COMPARE(out.str(),
        "ShaderTools::AnyConverter::convertDataToData(): SpirvToolsShaderConverter does not support preprocessing\n");
}

void AnyConverterTest::convertDataToDataDebugInfoNotSupported() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("SpirvToolsShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("SpirvToolsShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    converter->setInputFormat(Format::Spirv);
    converter->setOutputFormat(Format::SpirvAssembly);

    converter->setDebugInfoLevel("1");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertDataToData({}, Utility::Directory::read(Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.spv"))));
    /** @todo it once may support that, in which case we need to find another
        victim */
    CORRADE_COMPARE(out.str(),
        "ShaderTools::AnyConverter::convertDataToData(): SpirvToolsShaderConverter does not support controlling debug info output\n");
}

void AnyConverterTest::convertDataToDataOptimizationNotSupported() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    converter->setInputFormat(Format::Glsl);
    converter->setOutputFormat(Format::Spirv);

    converter->setOptimizationLevel("1");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertDataToData({}, Utility::Directory::read(Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl"))));
    /** @todo it once may support that, in which case we need to find another
        victim */
    CORRADE_COMPARE(out.str(),
        "ShaderTools::AnyConverter::convertDataToData(): GlslangShaderConverter does not support optimization\n");
}

void AnyConverterTest::convertDataToDataPropagateFlags() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    converter->setInputFormat(Format::Glsl);
    converter->setOutputFormat(Format::Spirv);

    /* With this, the warning should turn into an error. The converter should
       also print the verbose info. */
    converter->setFlags(ConverterFlag::Verbose|ConverterFlag::WarningAsError);

    /* We have to supply a valid file path because the version gets checked in
       doConvertDataToData(), called from AbstractConverter::doConvertFileToFile()
       with the file contents. */
    std::ostringstream out;
    Debug redirectDebug{&out};
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertDataToData(Stage::Fragment, Utility::Directory::read(Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl"))));
    CORRADE_COMPARE(out.str(),
        "ShaderTools::AnyConverter::convertDataToData(): using GlslToSpirvShaderConverter (provided by GlslangShaderConverter)\n"
        "ShaderTools::GlslangConverter::convertDataToData(): compilation failed:\n"
        "WARNING: 0:10: 'reserved__identifier' : identifiers containing consecutive underscores (\"__\") are reserved\n");
}

void AnyConverterTest::convertDataToDataPropagateInputVersion() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    /* This is an invalid version */
    converter->setInputFormat(Format::Glsl, "100");

    converter->setOutputFormat(Format::Spirv);

    /* We have to supply a valid file path because the version gets checked in
       doConvertDataToData(), called from AbstractConverter::doConvertFileToFile()
       with the file contents. */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertDataToData(Stage::Fragment, Utility::Directory::read(Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl"))));
    CORRADE_COMPARE(out.str(),
        "ShaderTools::GlslangConverter::convertDataToData(): input format version should be one of supported GLSL #version strings but got 100\n");
}

void AnyConverterTest::convertDataToDataPropagateOutputVersion() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    converter->setInputFormat(Format::Glsl);

    /* This is an invalid version */
    converter->setOutputFormat(Format::Spirv, "opengl4.0");

    /* We have to supply a valid file path because the version gets checked in
       doConvertDataToData(), called from AbstractConverter::doConvertFileToFile()
       with the file contents. */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertDataToData(Stage::Fragment, Utility::Directory::read(Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl"))));
    CORRADE_COMPARE(out.str(),
        "ShaderTools::GlslangConverter::convertDataToData(): output format version target should be opengl4.5 or vulkanX.Y but got opengl4.0\n");
}

void AnyConverterTest::convertDataToDataPropagatePreprocess() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    converter->setInputFormat(Format::Glsl);
    converter->setOutputFormat(Format::Spirv);

    /* Check that undefining works properly -- if it stays defined, the source
       won't compile */
    converter->setDefinitions({
        {"SHOULD_BE_UNDEFINED", "really"},
        {"SHOULD_BE_UNDEFINED", nullptr},
        {"reserved__identifier", "different__but_also_wrong"}
    });

    /* Make it print a warning so we know it's doing something */
    std::ostringstream out;
    Warning redirectWarning{&out};
    CORRADE_VERIFY(converter->convertDataToData(Stage::Fragment, Utility::Directory::read(Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl"))));
    CORRADE_COMPARE(out.str(),
        "ShaderTools::GlslangConverter::convertDataToData(): compilation succeeded with the following message:\n"
        "WARNING: 0:10: 'different__but_also_wrong' : identifiers containing consecutive underscores (\"__\") are reserved\n");
}

void AnyConverterTest::convertDataToDataPropagateDebugInfo() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("GlslangShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("GlslangShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    converter->setInputFormat(Format::Glsl);
    converter->setOutputFormat(Format::Spirv);

    /* This is an invalid level */
    converter->setDebugInfoLevel("2");

    /* We have to supply a valid file path because the version gets checked in
       doConvertDataToData(), called from AbstractConverter::doConvertFileToFile()
       with the file contents. */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertDataToData(Stage::Fragment, Utility::Directory::read(Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl"))));
    CORRADE_COMPARE(out.str(),
        "ShaderTools::GlslangConverter::convertDataToData(): debug info level should be 0, 1 or empty but got 2\n");
}

void AnyConverterTest::convertDataToDataPropagateOptimization() {
    PluginManager::Manager<AbstractConverter> manager{MAGNUM_PLUGINS_SHADERCONVERTER_INSTALL_DIR};
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_VERIFY(manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    if(manager.load("SpirvToolsShaderConverter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("SpirvToolsShaderConverter plugin can't be loaded.");

    Containers::Pointer<AbstractConverter> converter = manager.instantiate("AnyShaderConverter");

    converter->setInputFormat(Format::Spirv);
    converter->setOutputFormat(Format::Spirv);

    /* This is an invalid level */
    converter->setOptimizationLevel("2");

    /* We have to supply a valid file path because the version gets checked in
       doConvertDataToData(), called from AbstractConverter::doConvertFileToFile()
       with the file contents. */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertDataToData(Stage::Fragment, Utility::Directory::read(Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.spv"))));
    CORRADE_COMPARE(out.str(),
        "ShaderTools::SpirvToolsConverter::convertDataToData(): optimization level should be 0, 1, s, legalizeHlsl, vulkanToWebGpu, webGpuToVulkan or empty but got 2\n");
}

void AnyConverterTest::detectValidate() {
    auto&& data = DetectValidateData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractConverter> converter = _manager.instantiate("AnyShaderConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_COMPARE(converter->validateFile({}, data.filename),
        std::make_pair(false, ""));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\n"
        "ShaderTools::AnyConverter::validateFile(): cannot load the {0} plugin\n", data.plugin));
    #else
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "PluginManager::Manager::load(): plugin {0} was not found\n"
        "ShaderTools::AnyConverter::validateFile(): cannot load the {0} plugin\n", data.plugin));
    #endif
}

void AnyConverterTest::detectValidateExplicitFormat() {
    Containers::Pointer<AbstractConverter> converter = _manager.instantiate("AnyShaderConverter");

    /* It should pick up this format and not bother with the extension */
    converter->setInputFormat(Format::Hlsl);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_COMPARE(converter->validateFile({}, "file.spv"),
        std::make_pair(false, ""));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out.str(),
        "PluginManager::Manager::load(): plugin HlslShaderConverter is not static and was not found in nonexistent\n"
        "ShaderTools::AnyConverter::validateFile(): cannot load the HlslShaderConverter plugin\n");
    #else
    CORRADE_COMPARE(out.str(),
        "PluginManager::Manager::load(): plugin HlslShaderConverter was not found\n"
        "ShaderTools::AnyConverter::validateFile(): cannot load the HlslShaderConverter plugin\n");
    #endif
}

void AnyConverterTest::detectConvert() {
    auto&& data = DetectConvertData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractConverter> converter = _manager.instantiate("AnyShaderConverter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertFileToFile({}, data.from, Utility::Directory::join(ANYSHADERCONVERTER_TEST_OUTPUT_DIR, data.to)));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "PluginManager::Manager::load(): plugin {0} is not static and was not found in nonexistent\n"
        "ShaderTools::AnyConverter::convertFileToFile(): cannot load the {0} plugin\n", data.plugin));
    #else
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "PluginManager::Manager::load(): plugin {0} was not found\n"
        "ShaderTools::AnyConverter::convertFileToFile(): cannot load the {0} plugin\n", data.plugin));
    #endif
}

void AnyConverterTest::detectConvertExplicitFormat() {
    Containers::Pointer<AbstractConverter> converter = _manager.instantiate("AnyShaderConverter");

    /* It should pick up this format and not bother with the extension */
    converter->setInputFormat(Format::Hlsl);
    converter->setOutputFormat(Format::Wgsl);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertFileToFile({}, "file.spv", Utility::Directory::join(ANYSHADERCONVERTER_TEST_OUTPUT_DIR, "file.glsl")));
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out.str(),
        "PluginManager::Manager::load(): plugin HlslToWgslShaderConverter is not static and was not found in nonexistent\n"
        "ShaderTools::AnyConverter::convertFileToFile(): cannot load the HlslToWgslShaderConverter plugin\n");
    #else
    CORRADE_COMPARE(out.str(),
        "PluginManager::Manager::load(): plugin HlslToWgslShaderConverter was not found\n"
        "ShaderTools::AnyConverter::convertFileToFile(): cannot load the HlslToWgslShaderConverter plugin\n");
    #endif
}

}}}}

CORRADE_TEST_MAIN(Magnum::ShaderTools::Test::AnyConverterTest)
