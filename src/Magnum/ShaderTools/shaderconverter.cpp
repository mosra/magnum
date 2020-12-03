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

#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/Utility/String.h>

#include "Magnum/Implementation/converterUtilities.h"
#include "Magnum/Math/Functions.h"
#include "Magnum/ShaderTools/AbstractConverter.h"

namespace Magnum {

/** @page magnum-shaderconverter Shader conversion utility
@brief Converts, compiles, optimizes and links shaders of different formats
@m_since_latest

@m_footernavigation
@m_keywords{magnum-shaderconverter shaderconverter}

This utility is built if both `WITH_SHADERTOOLS` and `WITH_SHADERCONVERTER` is
enabled when building Magnum. To use this utility with CMake, you need to
request the `shaderconverter` component of the `Magnum` package and use the
`Magnum::shaderconverter` target for example in a custom command:

@code{.cmake}
find_package(Magnum REQUIRED shaderconverter)

add_custom_command(OUTPUT ... COMMAND Magnum::shaderconverter ...)
@endcode

See @ref building, @ref cmake and the @ref ShaderTools namespace for more
information.

@section magnum-shaderconverter-usage Usage

@code{.sh}
magnum-shaderconverter [-h|--help] [--validate] [--link]
    [-C|--converter NAME]... [--plugin-dir DIR]
    [-c|--converter-options key=val,key2=val2,…]... [-q|--quiet] [-v|--verbose]
    [--warning-as-error] [-E|--preprocess-only] [-D|--define name=value]...
    [-U|--undefine name]... [-O|--optimize LEVEL] [-g|--debug-info LEVEL]
    [--input-format glsl|spv|spvasm|hlsl|metal]...
    [--output-format glsl|spv|spvasm|hlsl|metal]...
    [--input-version VERSION]... [--output-version VERSION]...
    [--] input... output
@endcode

Arguments:

-   `input` --- input file(s)
-   `output` --- output file, ignored if `--validate` is present. If neither
    `--validate` nor `--link` is present, corresponds to the
    @ref ShaderTools::AbstractConverter::convertFileToFile() function.
-   `-h`, `--help` --- display this help message and exit
-   `--validate` --- validate input. Corresponds to the
    @ref ShaderTools::AbstractConverter::validateFile() function.
-   `--link` --- link multiple input files together. Corresponds to the
    @ref ShaderTools::AbstractConverter::linkFilesToFile() function.
-   `-C`, `--converter CONVERTER` --- shader converter plugin(s)
-   `--plugin-dir DIR` --- override base plugin dir
-   `-c`, `--converter-options key=val,key2=val2,…` --- configuration options
    to pass to the converter(s)
-   `-q`, `--quiet` --- quiet output from converter plugin(s). Corresponds to
    the @ref ShaderTools::ConverterFlag::Quiet flag.
-   `-v`, `--verbose` --- verbose output from converter plugin(s). Corresponds
    to the @ref ShaderTools::ConverterFlag::Verbose flag.
-   `--warning-as-error` --- treat warnings as errors. Corresponds to the
    @ref ShaderTools::ConverterFlag::WarningAsError flag.
-   `-E`, `--preprocess-only` --- preprocess the input file and exit.
    Corresponds to the @ref ShaderTools::ConverterFlag::PreprocessOnly flag.
-   `-D`, `--define name=value` --- define a preprocessor macro. Corresponds to
    the @ref ShaderTools::AbstractConverter::setDefinitions() function.
-   `-U`, `--undefine name` --- undefine a preprocessor macro. Corresponds to
    the @ref ShaderTools::AbstractConverter::setDefinitions() function.
-   `-O`, `--optimize LEVEL` --- optimization level to use. Corresponds to the
    @ref ShaderTools::AbstractConverter::setOptimizationLevel() function.
-   `-g`, `--debug-info LEVEL` --- debug info level to use. Corresponds to the
    @ref ShaderTools::AbstractConverter::setDebugInfoLevel() function.
-   `--input-format glsl|spv|spvasm|hlsl|metal` --- input format for each
    converter
-   `--output-format glsl|spv|spvasm|hlsl|metal` --- output format for each
    converter
-   `--input-version VERSION` --- input format version for each converter
-   `--output-version VERSION` --- output format version for each converter

If `--validate` is given, the utility will validate the `input` file using
passed `--converter` (or @ref ShaderTools::AnyConverter "AnyShaderConverter" if
none is specified), print the validation log on output and exit with a non-zero
code if the validation fails. If `--link` is given, the utility will link all
files together using passed `--converter` (or
@ref ShaderTools::AnyConverter "AnyShaderConverter" if none is specified) and
save it to `output`. If neither is specified, the utility will convert the
`input` file using (one or more) passed `--converter` (or
@ref ShaderTools::AnyConverter "AnyShaderConverter" if none is specified) and
save it to `output`.

The `-c` / `--converter-options` argument accept a comma-separated list of
key/value pairs to set in the converter plugin configuration. If the `=`
character is omitted, it's equivalent to saying `key=true`; configuration
subgroups are delimited with `/`. It's possible to specify the `-C` /
`--converter` option (and correspondingly also `-c` / `--converter-options`,
`--input-format`, `--output-format`, `--input-version` and `--output-version`)
multiple times in order to chain more converters together. All converters in
the chain have to support the @ref ShaderTools::ConverterFeature::ConvertData
feature, if there's just one converter it's enough for it to support
@ref ShaderTools::ConverterFeature::ConvertFile. If no `-C` / `--converter` is
specified, @ref ShaderTools::AnyConverter "AnyShaderConverter" is used.

The `-D` / `--define`, `-U` / `--undefine`, `-O` / `--optimize`, `-g` /
`--debug-info`, `-E` / `--preprocess-only` arguments apply only to the first
converter. Split the conversion to multiple passes if you need to pass those to
converters later in the chain.

Values accepted by `-O` / `--optimize`, `-g` / `--debug-info`, `--input-format`,
`--output-format`, `--input-version` and `--output-version` are
converter-specific, see documentation of a particular converter for more
information.

@section magnum-shaderconverter-example Example usage

Validate a SPIR-V file for a Vulkan 1.1 target, using
@ref ShaderTools::SpirvToolsConverter "SpirvToolsShaderConverter" picked by
@ref ShaderTools::AnyConverter "AnyShaderConverter":

@code{.sh}
magnum-shaderconverter --validate --output-version vulkan1.1 shader.spv
@endcode

Converting a GLSL 4.10 file to a SPIR-V, supplying various preprocessor
definitions, treating warnings as errors and targeting OpenGL instead of the
(default) Vulkan, using @ref ShaderTools::GlslangConverter "GlslangShaderConverter"
picked again by @ref ShaderTools::AnyConverter "AnyShaderConverter":

@m_class{m-console-wrap}

@code{.sh}
magnum-shaderconverter phong.frag -DDIFFUSE_TEXTURE -DNORMAL_TEXTURE --input-version "410 core" --output-version opengl4.5 --warning-as-error phong.frag.spv
@endcode
*/

}

using namespace Corrade::Containers::Literals;
using namespace Magnum;

int main(int argc, char** argv) {
    Utility::Arguments args;
    args.addArrayArgument("input").setHelp("input", "input file(s)")
        .addArgument("output").setHelp("output", "output file, ignored if --validate is present")
        .addBooleanOption("validate").setHelp("validate", "validate input")
        .addBooleanOption("link").setHelp("link", "link multiple input files together")
        .addArrayOption('C', "converter").setHelp("converter", "shader converter plugin(s)")
        .addOption("plugin-dir").setHelp("plugin-dir", "override base plugin dir", "DIR")
        .addArrayOption('c', "converter-options").setHelp("converter-options", "configuration options to pass to the converter(s)", "key=val,key2=val2,…")
        .addBooleanOption('q', "quiet").setHelp("quiet", "quiet output from converter plugin(s)")
        .addBooleanOption('v', "verbose").setHelp("verbose", "verbose output from converter plugin(s)")
        .addBooleanOption("warning-as-error").setHelp("warning-as-error", "treat warnings as errors")
        .addBooleanOption('E', "preprocess-only").setHelp("preprocess-only", "preprocess the input file and exit")
        .addArrayOption('D', "define").setHelp("define", "define a preprocessor macro", "name=value")
        .addArrayOption('U', "undefine").setHelp("undefine", "undefine a preprocessor macro", "name")
        .addOption('O', "optimize").setHelp("optimize", "optimization level to use", "LEVEL")
        .addOption('g', "debug-info").setHelp("debug-info", "debug info level to use", "LEVEL")
        /** @todo what the heck is the extension for wgsl and dxil?! */
        .addArrayOption("input-format").setHelp("input-format", "input format for each converter", "glsl|spv|spvasm|hlsl|metal")
        .addArrayOption("output-format").setHelp("output-format", "output format for each converter", "glsl|spv|spvasm|hlsl|metal")
        .addArrayOption("input-version").setHelp("input-version", "input format version for each converter", "VERSION")
        .addArrayOption("output-version").setHelp("output-version", "output format version for each converter", "VERSION")
        .setParseErrorCallback([](const Utility::Arguments& args, Utility::Arguments::ParseError error, const std::string& key) {
            /* If --validate is passed, we don't need the output argument */
            if(error == Utility::Arguments::ParseError::MissingArgument &&
                key == "output" && args.isSet("validate")) return true;

            /* Handle all other errors as usual */
            return false;
        })
        .setGlobalHelp(R"(Converts, compiles, optimizes and links shaders of different formats.

If --validate is given, the utility will validate the input file using passed
--converter (or AnyShaderConverter if none is specified), print the validation
log on output and exit with a non-zero code if the validation fails. If --link
is given, the utility will link all files together using passed --converter (or
AnyShaderConverter if none is specified) and save it to output. If neither is
specified, the utility will convert the input file using (one or more) passed
--converter and save it to output.

The -c / --converter-options argument accept a comma-separated list of
key/value pairs to set in the converter plugin configuration. If the =
character is omitted, it's equivalent to saying key=true; configuration
subgroups are delimited with /. It's possible to specify the -C / --converter
option (and correspondingly also -c / --converter-options, --input-format,
--output-format, --input-version and --output-version) multiple times in order
to chain more converters together. All converters in the chain have to support
the ConvertData feature, if there's just one converter it's enough for it to
support ConvertFile. If no -C / --converter is specified, AnyShaderConverter is
used.

The -D / --define, -U / --undefine, -O / --optimize, -g / --debug-info, -E /
--preprocess-only arguments apply only to the first converter. Split the
conversion to multiple passes if you need to pass those to converters later in
the chain.

Values accepted by -O / --optimize, -g / --debug-info, --input-format,
--output-format, --input-version and --output-version are converter-specific,
see documentation of a particular converter for more information.)")
        .parse(argc, argv);

    /* Generic checks */
    if(args.isSet("validate")) {
        if(!args.value<Containers::StringView>("output").isEmpty()) {
            Error{} << "Output file shouldn't be set for --validate";
            return 1;
        }
    }
    if(!args.isSet("link"))  {
        if(args.arrayValueCount("input") != 1) {
            Error{} << "Multiple input files are allowed only for --link";
            return 3;
        }
    }
    if(args.isSet("validate") || args.isSet("link")) {
        if(args.isSet("preprocess-only")) {
            Error{} << "The --preprocess-only option isn't allowed for --validate or --link";
            return 4;
        }

        if(args.arrayValueCount("converter") > 1) {
            Error{} << "Cannot use multiple converters with --validate or --link";
            return 5;
        }
    }
    if(args.isSet("quiet") && args.isSet("verbose")) {
        Error{} << "Can't set both --quiet and --verbose";
        return 6;
    }
    if(args.isSet("quiet") && args.isSet("warning-as-error")) {
        Error{} << "Can't set both --quiet and --warning-as-error";
        return 6;
    }

    /* Set up a converter manager */
    PluginManager::Manager<ShaderTools::AbstractConverter> converterManager{
        args.value("plugin-dir").empty() ? std::string{} :
        Utility::Directory::join(args.value("plugin-dir"), ShaderTools::AbstractConverter::pluginSearchPaths()[0])};

    /* Data passed from one converter to another in case there's more than one */
    Containers::Array<char> data;

    /* If there's no converters, it'll be just one AnyShaderConverter. */
    for(std::size_t i = 0, converterCount = args.arrayValueCount("converter"); i < Math::max(converterCount, std::size_t{1}); ++i) {
        const std::string converterName = converterCount ?
            args.arrayValue("converter", i) : "AnyShaderConverter";
        Containers::Pointer<ShaderTools::AbstractConverter> converter = converterManager.loadAndInstantiate(converterName);
        if(!converter) {
            Debug{} << "Available converter plugins:" << Utility::String::join(converterManager.aliasList(), ", ");
            return 7;
        }

        /* Set options if passed */
        if(i < args.arrayValueCount("converter-options"))
            Implementation::setOptions(*converter, args.arrayValue("converter-options", i));

        /* Parse format, if passed */
        ShaderTools::Format inputFormat{}, outputFormat{};
        auto parseFormat = [](Containers::StringView format) -> Containers::Optional<ShaderTools::Format> {
            if(format == ""_s) return ShaderTools::Format::Unspecified;
            if(format == "glsl"_s) return ShaderTools::Format::Glsl;
            if(format == "spv"_s) return ShaderTools::Format::Spirv;
            if(format == "spvasm"_s) return ShaderTools::Format::SpirvAssembly;
            if(format == "hlsl"_s) return ShaderTools::Format::Hlsl;
            if(format == "metal"_s) return ShaderTools::Format::Msl;
            /** @todo wgsl and dxil once i figure out the extensions */

            Error{} << "Unrecognized format" << format << Debug::nospace << ", expected glsl, spv, spvasm, hlsl or metal";
            return {};
        };
        if(i < args.arrayValueCount("input-format")) {
            if(const Containers::Optional<ShaderTools::Format> format = parseFormat(args.arrayValue<Containers::StringView>("input-format", i)))
                inputFormat = *format;
            else return 8;
        }
        if(i < args.arrayValueCount("output-format")) {
            if(const Containers::Optional<ShaderTools::Format> format = parseFormat(args.arrayValue<Containers::StringView>("output-format", i)))
                outputFormat = *format;
            else return 9;
        }

        /* Get version, if passed */
        Containers::StringView inputVersion{}, outputVersion{};
        if(i < args.arrayValueCount("input-version"))
            inputVersion = args.arrayValue<Containers::StringView>("input-version", i);
        if(i < args.arrayValueCount("output-version"))
            outputVersion = args.arrayValue<Containers::StringView>("output-version", i);

        /* If not passed, these are set to Unspecified and "", which is the
           default */
        converter->setInputFormat(inputFormat, inputVersion);
        converter->setOutputFormat(outputFormat, outputVersion);

        ShaderTools::ConverterFlags flags;

        /* Global flags, applied for all converters */
        if(args.isSet("quiet")) flags |= ShaderTools::ConverterFlag::Quiet;
        if(args.isSet("verbose")) flags |= ShaderTools::ConverterFlag::Verbose;
        if(args.isSet("warning-as-error")) flags |= ShaderTools::ConverterFlag::WarningAsError;

        /* Options and flags applied just for the first converter; setting up
           file list for linking  */
        Containers::Array<std::pair<ShaderTools::Stage, Containers::StringView>> linkInputs;
        if(i == 0) {
            if((args.isSet("preprocess-only") || args.arrayValueCount("define") || args.arrayValueCount("undefine"))) {
                if(!(converter->features() & ShaderTools::ConverterFeature::Preprocess)) {
                    Error{} << "The -E / -D / -U options are set, but" << converterName << "doesn't support preprocessing";
                    return 10;
                }

                if(args.isSet("preprocess-only"))
                    flags |= ShaderTools::ConverterFlag::PreprocessOnly;

                Containers::Array<std::pair<Containers::StringView, Containers::StringView>> definitions;
                arrayReserve(definitions, args.arrayValueCount("define") + args.arrayValueCount("undefine"));
                for(std::size_t i = 0; i != args.arrayValueCount("define"); ++i) {
                    const Containers::Array3<Containers::StringView> define =
                    args.arrayValue<Containers::StringView>("define", i).partition('=');
                    arrayAppend(definitions, Containers::InPlaceInit,
                        define[0], define[2]);
                }
                for(std::size_t i = 0; i != args.arrayValueCount("undefine"); ++i) {
                    arrayAppend(definitions, Containers::InPlaceInit,
                        args.arrayValue<Containers::StringView>("undefine", i), nullptr);
                }

                converter->setDefinitions(definitions);
            }

            if(!args.value<Containers::StringView>("optimize").isEmpty()) {
                if(!(converter->features() & ShaderTools::ConverterFeature::Optimize)) {
                    Error{} << "The -O option is set, but" << converterName << "doesn't support optimization";
                    return 11;
                }

                converter->setOptimizationLevel(args.value<Containers::StringView>("optimize"));
            }

            if(!args.value<Containers::StringView>("debug-info").isEmpty()) {
                if(!(converter->features() & ShaderTools::ConverterFeature::DebugInfo)) {
                    Error{} << "The -g option is set, but" << converterName << "doesn't support debug info";
                    return 12;
                }

                converter->setDebugInfoLevel(args.value<Containers::StringView>("debug-info"));
            }

            if(args.isSet("link")) {
                arrayReserve(linkInputs, args.arrayValueCount("input"));
                for(std::size_t i = 0; i != args.arrayValueCount("input"); ++i)
                    arrayAppend(linkInputs, Containers::InPlaceInit,
                        ShaderTools::Stage::Unspecified, args.arrayValue<Containers::StringView>("input", i));
            }
        }

        converter->setFlags(flags);

        /* If validating, do it just with the first passed converter and then
           exit */
        if(args.isSet("validate")) {
            /* The validation exits right after, so this branch shouldn't get
               re-entered again */
            CORRADE_INTERNAL_ASSERT(i == 0);

            if(!(converter->features() & ShaderTools::ConverterFeature::ValidateFile)) {
                Error{} << converterName << "doesn't support file validation";
                return 13;
            }

            std::pair<bool, Containers::String> out = converter->validateFile(ShaderTools::Stage::Unspecified, args.arrayValue<Containers::StringView>("input", 0));
            if(!out.first) {
                if(args.isSet("verbose"))
                    Error{} << "Validation failed:";
                if(!out.second.isEmpty()) Error{} << out.second;
            } else if(!out.second.isEmpty()) {
                if(args.isSet("verbose"))
                    Warning{} << "Validation succeeded with warnings:";
                if(!out.second.isEmpty()) Warning{} << out.second;
            } else if(args.isSet("verbose"))
                Debug{} << "Validation passed";
            return out.first ? 0 : 14;
        }

        /** @todo ability to specify the stage (need a configurationvalue parser for this) */

        /* This is the first *and* last --converter, go from a file to a file */
        if(i == 0 && converterCount <= 1) {
            if(!(converter->features() & ShaderTools::ConverterFeature::ConvertFile)) {
                Error{} << converterName << "doesn't support file conversion";
                return 15;
            }

            /* No verbose output for just one converter */

            /* Linking */
            if(args.isSet("link")) {
                if(!converter->linkFilesToFile(linkInputs, args.value<Containers::StringView>("output"))) {
                    Error{} << "Cannot link" << args.arrayValue<Containers::StringView>("input", 0) << "and others to" << args.value<Containers::StringView>("output");
                    return 16;
                }

            /* Converting */
            } else {
                if(!converter->convertFileToFile(ShaderTools::Stage::Unspecified, args.arrayValue<Containers::StringView>("input", 0), args.value<Containers::StringView>("output"))) {
                    Error{} << "Cannot convert" << args.arrayValue<Containers::StringView>("input", 0) << "to" << args.value<Containers::StringView>("output");
                    return 17;
                }
            }

        /* Otherwise we need to go through data */
        } else {
            if(!(converter->features() & ShaderTools::ConverterFeature::ConvertData)) {
                Error{} << converterName << "doesn't support data conversion";
                return 18;
            }

            /* This is the first --converter and there are more, go from a file
               to data */
            if(i == 0 && converterCount > 1) {
                if(args.isSet("verbose"))
                    Debug{} << "Processing (" << Debug::nospace << (i+1) << Debug::nospace << "/" << Debug::nospace << converterCount << Debug::nospace << ") with" << converterName << Debug::nospace << "...";

                /* Linking */
                if(args.isSet("link")) {
                    if(!(data = converter->linkFilesToData(linkInputs))) {
                        Error{} << "Cannot link" << args.arrayValue<Containers::StringView>("input", 0) << "and others to" << args.value<Containers::StringView>("output");
                        return 19;
                    }

                /* Converting */
                } else {
                    if(!(data = converter->convertFileToData(ShaderTools::Stage::Unspecified, args.arrayValue<Containers::StringView>("input", 0)))) {
                        Error{} << "Cannot convert" << args.arrayValue<Containers::StringView>("input", 0);
                        return 20;
                    }
                }

            /* This is neither first nor last --converter, go from data to
               data */
            } else if(i > 0 && i + 1 < converterCount) {
                if(args.isSet("verbose"))
                    Debug{} << "Processing (" << Debug::nospace << (i+1) << Debug::nospace << "/" << Debug::nospace << converterCount << Debug::nospace << ") with" << converterName << Debug::nospace << "...";

                CORRADE_INTERNAL_ASSERT(data);

                /* Subsequent operations are always a conversion, not link */
                if(!(data = converter->convertDataToData(ShaderTools::Stage::Unspecified, data))) {
                    Error{} << "Cannot convert shader data";
                    return 21;
                }

            /* This is the last --converter, output to a file and exit the
               loop */
            } else if(i + 1 >= converterCount) {
                if(args.isSet("verbose"))
                    Debug{} << "Saving output with" << converterName << Debug::nospace << "...";

                CORRADE_INTERNAL_ASSERT(data);

                /* Subsequent operations are always a conversion, not link */
                if(!converter->convertDataToFile(ShaderTools::Stage::Unspecified, data, args.value<Containers::StringView>("output"))) {
                    Error{} << "Cannot save file" << args.value<Containers::StringView>("output");
                    return 22;
                }

            } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }
    }
}
