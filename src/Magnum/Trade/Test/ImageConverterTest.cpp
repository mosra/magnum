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

#include <cstdlib>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringIterable.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/StringToFile.h>
#include <Corrade/Utility/Format.h>
#include <Corrade/Utility/Path.h>

#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/AbstractImageConverter.h"

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct ImageConverterTest: TestSuite::Tester {
    explicit ImageConverterTest();

    void info();
};

using namespace Containers::Literals;

const struct {
    TestSuite::TestCaseDescriptionSourceLocation name;
    Containers::Array<Containers::String> args;
    const char* requiresImporter;
    const char* requiresConverter;
    const char* expected;
} InfoData[]{
    {"importer", {InPlaceInit, {
            "--info-importer", "-i", "someOption=yes"
        }},
        "AnyImageImporter", nullptr,
        "info-importer.txt"},
    {"converter", {InPlaceInit, {
            "-C", "AnyImageConverter", "--info-converter", "-c", "someOption=yes"
        }},
        nullptr, "AnyImageConverter",
        "info-converter.txt"},
    {"converter, implicit", {InPlaceInit, {
            "--info-converter", "-c", "someOption=yes"
        }},
        nullptr, "AnyImageConverter",
        "info-converter.txt"},
    {"importer, ignored input and output", {InPlaceInit, {
            "--info-importer", "a.png", "b.png", "out.jpg"
        }},
        "AnySceneImporter", nullptr,
        "info-importer-ignored-input-output.txt"},
    {"data", {InPlaceInit, {
            "-I", "TgaImporter", "--info", Utility::Path::join(TRADE_TEST_DIR, "ImageConverterTestFiles/file.tga")
        }},
        "TgaImporter", nullptr,
        "info-data.txt"},
    {"data, map", {InPlaceInit, {
            "--map", "-I", "TgaImporter", "--info", Utility::Path::join(TRADE_TEST_DIR, "ImageConverterTestFiles/file.tga")
        }},
        "TgaImporter", nullptr,
        /** @todo change to something else once we have a plugin that can
            zero-copy pass the imported data */
        "info-data.txt"},
    {"data, ignored output file", {InPlaceInit, {
            "-I", "TgaImporter", "--info", Utility::Path::join(TRADE_TEST_DIR, "ImageConverterTestFiles/file.tga"), "whatever.png"
        }},
        "TgaImporter", nullptr,
        "info-data-ignored-output.txt"}
};

ImageConverterTest::ImageConverterTest() {
    addInstancedTests({&ImageConverterTest::info},
        Containers::arraySize(InfoData));

    /* Create output dir, if doesn't already exist */
    Utility::Path::make(Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "ImageConverterTestFiles"));
}

namespace {

#ifdef IMAGECONVERTER_EXECUTABLE_FILENAME
Containers::Pair<bool, Containers::String> call(const Containers::StringIterable& arguments) {
    const Containers::String outputFilename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "ImageConverterTestFiles/output.txt");
    /** @todo clean up once Utility::System::execute() with output redirection
        exists */
    /* Implicitly pass the plugin directory override */
    const bool success = std::system(Utility::format("{} --plugin-dir {} {} > {} 2>&1",
        IMAGECONVERTER_EXECUTABLE_FILENAME,
        MAGNUM_PLUGINS_INSTALL_DIR,
        " "_s.join(arguments), /** @todo handle space escaping here? */
        outputFilename
    ).data()) == 0;

    const Containers::Optional<Containers::String> output = Utility::Path::readString(outputFilename);
    CORRADE_VERIFY(output);

    return {success, Utility::move(*output)};
}
#endif

}

void ImageConverterTest::info() {
    auto&& data = InfoData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef IMAGECONVERTER_EXECUTABLE_FILENAME
    CORRADE_SKIP("magnum-imageconverter not built, can't test");
    #else
    /* Check if required plugins can be loaded. Catches also ABI and interface
       mismatch errors. */
    PluginManager::Manager<Trade::AbstractImporter> importerManager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    PluginManager::Manager<Trade::AbstractImageConverter> converterManager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    if(data.requiresImporter && !(importerManager.load(data.requiresImporter) & PluginManager::LoadState::Loaded))
        CORRADE_SKIP(data.requiresImporter << "plugin can't be loaded.");
    if(data.requiresConverter && !(converterManager.load(data.requiresConverter) & PluginManager::LoadState::Loaded))
        CORRADE_SKIP(data.requiresConverter << "plugin can't be loaded.");

    CORRADE_VERIFY(true); /* capture correct function name */

    Containers::Pair<bool, Containers::String> output = call(data.args);
    CORRADE_COMPARE_AS(output.second(),
        Utility::Path::join({TRADE_TEST_DIR, "ImageConverterTestFiles", data.expected}),
        TestSuite::Compare::StringToFile);
    CORRADE_VERIFY(output.first());
    #endif
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::ImageConverterTest)
