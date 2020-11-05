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

#include "AnyConverter.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringStl.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/PluginManager/PluginMetadata.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/FormatStl.h>
#include <Corrade/Utility/String.h>

namespace Magnum { namespace ShaderTools {

struct AnyConverter::State {
    Format inputFormat, outputFormat;
    Containers::String inputVersion, outputVersion;

    Containers::Array<std::pair<Containers::String, Containers::String>> definitions;
    Containers::Array<std::pair<Containers::StringView, Containers::StringView>> definitionViews;

    Containers::String debugInfoLevel, optimizationLevel;
};

AnyConverter::AnyConverter(PluginManager::Manager<AbstractConverter>& manager): AbstractConverter{manager} {}

AnyConverter::AnyConverter(PluginManager::AbstractManager& manager, const std::string& plugin): AbstractConverter{manager, plugin}, _state{Containers::InPlaceInit} {}

AnyConverter::~AnyConverter() = default;

ConverterFeatures AnyConverter::doFeatures() const {
    return ConverterFeature::ValidateFile|ConverterFeature::ConvertFile|ConverterFeature::Preprocess|ConverterFeature::DebugInfo|ConverterFeature::Optimize;
}

void AnyConverter::doSetInputFormat(const Format format, const Containers::StringView version) {
    _state->inputFormat = format;
    _state->inputVersion = Containers::String::nullTerminatedGlobalView(version);
}

void AnyConverter::doSetOutputFormat(Format format, Containers::StringView version) {
    _state->outputFormat = format;
    _state->outputVersion = Containers::String::nullTerminatedGlobalView(version);
}

void AnyConverter::doSetDefinitions(const Containers::ArrayView<const std::pair<Containers::StringView, Containers::StringView>> definitions) {
    /* We have to make a local copy, unfortunately, and then a view on that
       local copy */
    _state->definitions = Containers::Array<std::pair<Containers::String, Containers::String>>{definitions.size()};
    _state->definitionViews = Containers::Array<std::pair<Containers::StringView, Containers::StringView>>{definitions.size()};
    for(std::size_t i = 0; i != definitions.size(); ++i) {
        /* Avoid a copy if the input is a global string literal */
        _state->definitions[i] = {
            Containers::String::nullTerminatedGlobalView(definitions[i].first),
            Containers::String::nullTerminatedGlobalView(definitions[i].second)
        };
        /* Preserve the distinction between empty defines ("") and undefines
           (nullptr or default constructor) */
        _state->definitionViews[i] = {
            _state->definitions[i].first,
            definitions[i].second.data() ?
                Containers::StringView{_state->definitions[i].second} :
                Containers::StringView{}
        };
    }
}

void AnyConverter::doSetDebugInfoLevel(const Containers::StringView level) {
    _state->debugInfoLevel = Containers::String::nullTerminatedGlobalView(level);
}

void AnyConverter::doSetOptimizationLevel(const Containers::StringView level) {
    _state->optimizationLevel = Containers::String::nullTerminatedGlobalView(level);
}

namespace {

using namespace Containers::Literals;

Containers::StringView formatForExtension(const char* prefix, const Containers::StringView filename) {
    /** @todo lowercase only the extension, once Directory::split() is done */
    const std::string normalized = Utility::String::lowercase(filename);

    /* https://github.com/KhronosGroup/SPIRV-Tools/blob/a715b1b4053519ad0f2bdb2d22ace35d35867cff/README.md#command-line-tools
       "It's a convention to name SPIR-V assembly and binary files with suffix
       .spvasm and .spv, respectively." IT'S GREAT THAT I HAD TO SEARCH HALF
       THE INTERNET TO FIND THIS CONVENTION. Especially when tests in the
       SPIRV-Cross repo use `.asm.bla` instead, FFS. */
    if(Utility::String::endsWith(normalized, ".spvasm") ||
       /* Not official, used by https://github.com/KhronosGroup/SPIRV-Cross */
       Utility::String::endsWith(normalized, ".asm.vert") ||
       Utility::String::endsWith(normalized, ".asm.frag") ||
       Utility::String::endsWith(normalized, ".asm.geom") ||
       Utility::String::endsWith(normalized, ".asm.comp") ||
       Utility::String::endsWith(normalized, ".asm.tesc") ||
       Utility::String::endsWith(normalized, ".asm.tese") ||
       Utility::String::endsWith(normalized, ".asm.rgen") ||
       Utility::String::endsWith(normalized, ".asm.rint") ||
       Utility::String::endsWith(normalized, ".asm.rahit") ||
       Utility::String::endsWith(normalized, ".asm.rchit") ||
       Utility::String::endsWith(normalized, ".asm.rmiss") ||
       Utility::String::endsWith(normalized, ".asm.rcall") ||
       Utility::String::endsWith(normalized, ".asm.mesh") ||
       Utility::String::endsWith(normalized, ".asm.task"))
        return "SpirvAssembly"_s;
    /* https://github.com/KhronosGroup/glslang/blob/3ce148638bdc3807316e358dee4a5c9583189ae7/StandAlone/StandAlone.cpp#L260-L274 */
    else if(Utility::String::endsWith(normalized, ".glsl") ||
            Utility::String::endsWith(normalized, ".vert") ||
            Utility::String::endsWith(normalized, ".frag") ||
            Utility::String::endsWith(normalized, ".geom") ||
            Utility::String::endsWith(normalized, ".comp") ||
            Utility::String::endsWith(normalized, ".tesc") ||
            Utility::String::endsWith(normalized, ".tese") ||
            Utility::String::endsWith(normalized, ".rgen") ||
            Utility::String::endsWith(normalized, ".rint") ||
            Utility::String::endsWith(normalized, ".rahit") ||
            Utility::String::endsWith(normalized, ".rchit") ||
            Utility::String::endsWith(normalized, ".rmiss") ||
            Utility::String::endsWith(normalized, ".rcall") ||
            Utility::String::endsWith(normalized, ".mesh") ||
            Utility::String::endsWith(normalized, ".task"))
        return "Glsl"_s;
    else if(Utility::String::endsWith(normalized, ".spv"))
        return "Spirv"_s;

    Error{} << prefix << "cannot determine the format of" << filename;
    return {};
}

}

std::pair<bool, Containers::String> AnyConverter::doValidateFile(const Stage stage, const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /* Decide on a plugin name based on the extension */
    const Containers::StringView format = formatForExtension("ShaderTools::AnyConverter::validateFile():", filename);
    if(format.isEmpty()) return {};
    const std::string plugin = Utility::formatString("{}ShaderConverter", format);

    /* Try to load the plugin */
    if(!(manager()->load(plugin) & PluginManager::LoadState::Loaded)) {
        Error{} << "ShaderTools::AnyConverter::validateFile(): cannot load the" << plugin << "plugin";
        return {};
    }
    PluginManager::PluginMetadata* metadata = manager()->metadata(plugin);
    if(flags() & ConverterFlag::Verbose) {
        Debug d;
        d << "ShaderTools::AnyConverter::validateFile(): using" << plugin;
        CORRADE_INTERNAL_ASSERT(metadata);
        if(plugin != metadata->name())
            d << "(provided by" << metadata->name() << Debug::nospace << ")";
    }

    /* Instantiate the plugin */
    Containers::Pointer<AbstractConverter> converter = static_cast<PluginManager::Manager<AbstractConverter>*>(manager())->instantiate(plugin);

    /* Check that it can actually validate */
    if(!(converter->features() & ConverterFeature::ValidateFile)) {
        Error{} << "ShaderTools::AnyConverter::validateFile():" << metadata->name() << "does not support validation";
        return {};
    }

    /* Check that it can preprocess, in case we were asked to preprocess */
    if((!_state->definitionViews.empty() || (flags() & ConverterFlag::PreprocessOnly)) && !(converter->features() & ConverterFeature::Preprocess)) {
        Error{} << "ShaderTools::AnyConverter::validateFile():" << metadata->name() << "does not support preprocessing";
        return {};
    }

    /* Propagate input/output version and flags */
    converter->setFlags(flags());
    converter->setInputFormat(_state->inputFormat, _state->inputVersion);
    converter->setOutputFormat(_state->outputFormat, _state->outputVersion);

    /* Propagate definitions, if any */
    if(!_state->definitionViews.empty())
        converter->setDefinitions(_state->definitionViews);

    /* Try to validate the file (error output should be printed by the plugin
       itself) */
    return converter->validateFile(stage, filename);
}

bool AnyConverter::doConvertFileToFile(const Stage stage, const Containers::StringView from, const Containers::StringView to) {
    CORRADE_INTERNAL_ASSERT(manager());

    /* Decide on a plugin name based on the input and output extension. This
       might result in invalid combinations such as SpirvToGlslShaderConverter
       which can't be really handled yet but I think that's okay for now */
    const Containers::StringView formatFrom = formatForExtension("ShaderTools::AnyConverter::convertFileToFile():", from);
    const Containers::StringView formatTo = formatForExtension("ShaderTools::AnyConverter::convertFileToFile():", to);
    if(formatFrom.isEmpty() || formatTo.isEmpty()) return {};
    const std::string plugin = Utility::formatString(
        formatFrom == formatTo ? "{}ShaderConverter" : "{}To{}ShaderConverter",
        formatFrom, formatTo);

    /* Try to load the plugin */
    if(!(manager()->load(plugin) & PluginManager::LoadState::Loaded)) {
        Error{} << "ShaderTools::AnyConverter::convertFileToFile(): cannot load the" << plugin << "plugin";
        return {};
    }
    PluginManager::PluginMetadata* metadata = manager()->metadata(plugin);
    if(flags() & ConverterFlag::Verbose) {
        Debug d;
        d << "ShaderTools::AnyConverter::convertFileToFile(): using" << plugin;
        CORRADE_INTERNAL_ASSERT(metadata);
        if(plugin != metadata->name())
            d << "(provided by" << metadata->name() << Debug::nospace << ")";
    }

    /* Instantiate the plugin */
    Containers::Pointer<AbstractConverter> converter = static_cast<PluginManager::Manager<AbstractConverter>*>(manager())->instantiate(plugin);

    /* Check that it can actually convert */
    if(!(converter->features() & ConverterFeature::ConvertFile)) {
        Error{} << "ShaderTools::AnyConverter::convertFileToFile():" << metadata->name() << "does not support conversion";
        return {};
    }

    /* Check that it can preprocess, in case we were asked to preprocess */
    if((!_state->definitionViews.empty() || (flags() & ConverterFlag::PreprocessOnly)) && !(converter->features() & ConverterFeature::Preprocess)) {
        Error{} << "ShaderTools::AnyConverter::convertFileToFile():" << metadata->name() << "does not support preprocessing";
        return {};
    }

    /* Check that it can output debug info, in case we were asked to */
    if(!_state->debugInfoLevel.isEmpty() && !(converter->features() & ConverterFeature::DebugInfo)) {
        Error{} << "ShaderTools::AnyConverter::convertFileToFile():" << metadata->name() << "does not support controlling debug info output";
        return {};
    }

    /* Check that it can optimize, in case we were asked to */
    if(!_state->optimizationLevel.isEmpty() && !(converter->features() & ConverterFeature::Optimize)) {
        Error{} << "ShaderTools::AnyConverter::convertFileToFile():" << metadata->name() << "does not support optimization";
        return {};
    }

    /* Propagate input/output version and flags */
    converter->setFlags(flags());
    converter->setInputFormat(_state->inputFormat, _state->inputVersion);
    converter->setOutputFormat(_state->outputFormat, _state->outputVersion);

    /* Propagate definitions and debug info, if any */
    if(!_state->definitionViews.empty())
        converter->setDefinitions(_state->definitionViews);
    if(!_state->debugInfoLevel.isEmpty())
        converter->setDebugInfoLevel(_state->debugInfoLevel);
    if(!_state->optimizationLevel.isEmpty())
        converter->setOptimizationLevel(_state->optimizationLevel);

    /* Try to convert the file (error output should be printed by the plugin
       itself) */
    return converter->convertFileToFile(stage, from, to);
}

}}

CORRADE_PLUGIN_REGISTER(AnyShaderConverter, Magnum::ShaderTools::AnyConverter,
    "cz.mosra.magnum.ShaderTools.AbstractConverter/0.1")
