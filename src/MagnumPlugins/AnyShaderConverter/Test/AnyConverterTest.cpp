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
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringStl.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/ShaderTools/AbstractConverter.h"

#include "configure.h"

namespace Magnum { namespace ShaderTools { namespace Test { namespace {

struct AnyConverterTest: TestSuite::Tester {
    explicit AnyConverterTest();

    void validate();
    void validateNotSupported();
    void validatePreprocessNotSupported();
    void validatePropagateFlags();
    void validatePropagateInputVersion();
    void validatePropagateOutputVersion();
    void validatePropagatePreprocess();

    void convert();
    void convertNotSupported();
    void convertPreprocessNotSupported();
    void convertDebugInfoNotSupported();
    void convertOptimizationNotSupported();
    void convertPropagateFlags();
    void convertPropagateInputVersion();
    void convertPropagateOutputVersion();
    void convertPropagatePreprocess();
    void convertPropagateDebugInfo();
    void convertPropagateOptimization();

    void detectValidate();
    void detectConvert();

    void unknown();

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
    addTests({&AnyConverterTest::validate,
              &AnyConverterTest::validateNotSupported,
              &AnyConverterTest::validatePreprocessNotSupported,
              &AnyConverterTest::validatePropagateFlags,
              &AnyConverterTest::validatePropagateInputVersion,
              &AnyConverterTest::validatePropagateOutputVersion,
              &AnyConverterTest::validatePropagatePreprocess,

              &AnyConverterTest::convert,
              &AnyConverterTest::convertNotSupported,
              &AnyConverterTest::convertPreprocessNotSupported,
              &AnyConverterTest::convertDebugInfoNotSupported,
              &AnyConverterTest::convertOptimizationNotSupported,
              &AnyConverterTest::convertPropagateFlags,
              &AnyConverterTest::convertPropagateInputVersion,
              &AnyConverterTest::convertPropagateOutputVersion,
              &AnyConverterTest::convertPropagatePreprocess,
              &AnyConverterTest::convertPropagateDebugInfo,
              &AnyConverterTest::convertPropagateOptimization});

    addInstancedTests({&AnyConverterTest::detectValidate},
        Containers::arraySize(DetectValidateData));

    addInstancedTests({&AnyConverterTest::detectConvert},
        Containers::arraySize(DetectConvertData));

    addTests({&AnyConverterTest::unknown});

    /* Load the plugin directly from the build tree. Otherwise it's static and
       already loaded. */
    #ifdef ANYSHADERCONVERTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(ANYSHADERCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Create the output directory if it doesn't exist yet */
    CORRADE_INTERNAL_ASSERT_OUTPUT(Utility::Directory::mkpath(ANYSHADERCONVERTER_TEST_OUTPUT_DIR));
}

void AnyConverterTest::validate() {
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

void AnyConverterTest::validateNotSupported() {
    CORRADE_SKIP("No plugin that would support just validation exists.");
}

void AnyConverterTest::validatePreprocessNotSupported() {
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

void AnyConverterTest::validatePropagateFlags() {
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

void AnyConverterTest::validatePropagateInputVersion() {
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

void AnyConverterTest::validatePropagateOutputVersion() {
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

void AnyConverterTest::validatePropagatePreprocess() {
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

void AnyConverterTest::convert() {
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

void AnyConverterTest::convertNotSupported() {
    CORRADE_SKIP("No plugin that would support just validation exists.");
}

void AnyConverterTest::convertPreprocessNotSupported() {
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

void AnyConverterTest::convertDebugInfoNotSupported() {
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

void AnyConverterTest::convertOptimizationNotSupported() {
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

void AnyConverterTest::convertPropagateFlags() {
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
    CORRADE_VERIFY(!converter->convertFileToFile(Stage::Fragment, Utility::Directory::join(ANYSHADERCONVERTER_TEST_DIR, "file.glsl"), Utility::Directory::join(ANYSHADERCONVERTER_TEST_OUTPUT_DIR, "file.spv")));
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "ShaderTools::AnyConverter::convertFileToFile(): using GlslToSpirvShaderConverter (provided by GlslangShaderConverter)\n"
        "ShaderTools::GlslangConverter::convertDataToData(): compilation failed:\n"
        "WARNING: {}:10: 'reserved__identifier' : identifiers containing consecutive underscores (\"__\") are reserved\n", filename));
}

void AnyConverterTest::convertPropagateInputVersion() {
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

void AnyConverterTest::convertPropagateOutputVersion() {
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

void AnyConverterTest::convertPropagatePreprocess() {
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

void AnyConverterTest::convertPropagateDebugInfo() {
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

void AnyConverterTest::convertPropagateOptimization() {
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

void AnyConverterTest::unknown() {
    std::ostringstream output;
    Error redirectError{&output};

    Containers::Pointer<AbstractConverter> converter = _manager.instantiate("AnyShaderConverter");
    CORRADE_COMPARE(converter->validateFile({}, "dead.cg"),
        std::make_pair(false, ""));
    CORRADE_COMPARE(output.str(), "ShaderTools::AnyConverter::validateFile(): cannot determine the format of dead.cg\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::ShaderTools::Test::AnyConverterTest)
