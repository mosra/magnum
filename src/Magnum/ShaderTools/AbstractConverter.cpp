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

#include "AbstractConverter.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringStl.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/FileCallback.h"

#ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
#include "Magnum/ShaderTools/configure.h"
#endif

namespace Magnum { namespace ShaderTools {

std::string AbstractConverter::pluginInterface() {
    return
/* [interface] */
"cz.mosra.magnum.ShaderTools.AbstractConverter/0.1"
/* [interface] */
    ;
}

#ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
std::vector<std::string> AbstractConverter::pluginSearchPaths() {
    return PluginManager::implicitPluginSearchPaths(
        #ifndef MAGNUM_BUILD_STATIC
        Utility::Directory::libraryLocation(&pluginInterface),
        #else
        {},
        #endif
        #ifdef CORRADE_IS_DEBUG_BUILD
        MAGNUM_PLUGINS_SHADERCONVERTER_DEBUG_DIR,
        #else
        MAGNUM_PLUGINS_SHADERCONVERTER_DIR,
        #endif
        #ifdef CORRADE_IS_DEBUG_BUILD
        "magnum-d/"
        #else
        "magnum/"
        #endif
        "shaderconverters");
}
#endif

AbstractConverter::AbstractConverter() = default;

AbstractConverter::AbstractConverter(PluginManager::Manager<AbstractConverter>& manager): PluginManager::AbstractManagingPlugin<AbstractConverter>{manager} {}

AbstractConverter::AbstractConverter(PluginManager::AbstractManager& manager, const std::string& plugin): PluginManager::AbstractManagingPlugin<AbstractConverter>{manager, plugin} {}

ConverterFeatures AbstractConverter::features() const {
    const ConverterFeatures features = doFeatures();
    CORRADE_ASSERT(features & ~(ConverterFeature::InputFileCallback|ConverterFeature::Preprocess|ConverterFeature::Optimize|ConverterFeature::DebugInfo),
        "ShaderTools::AbstractConverter::features(): implementation reported no features", {});
    return features;
}

void AbstractConverter::setFlags(const ConverterFlags flags) {
    CORRADE_ASSERT(!(flags >= (ConverterFlag::Quiet|ConverterFlag::Verbose)),
        "ShaderTools::AbstractConverter::setFlags(): can't have both Quiet and Verbose set", );
    CORRADE_ASSERT((features() & ConverterFeature::Preprocess) || !(flags & ConverterFlag::PreprocessOnly),
        "ShaderTools::AbstractConverter::setFlags(): PreprocessOnly not supported by the implementation", );
    _flags = flags;
    doSetFlags(flags);
}

void AbstractConverter::doSetFlags(ConverterFlags) {}

void AbstractConverter::setInputFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*callback)(const std::string&, InputFileCallbackPolicy, void*), void* const userData) {
    /* Clearing the *File bits as those are present in *Data as well and thus
       this would pass even if only file conversion/validation is supported,
       which is wrong */
    CORRADE_ASSERT(features() & (ConverterFeature::InputFileCallback|ConverterFeature::ValidateData|ConverterFeature::ConvertData|ConverterFeature::LinkData) & ~(ConverterFeature::ValidateFile|ConverterFeature::ConvertFile|ConverterFeature::LinkFile),
        "ShaderTools::AbstractConverter::setInputFileCallback(): converter supports neither loading from data nor via callbacks, callbacks can't be used", );

    _inputFileCallback = callback;
    _inputFileCallbackUserData = userData;
    doSetInputFileCallback(callback, userData);
}

void AbstractConverter::doSetInputFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, void*), void*) {}

void AbstractConverter::setInputFormat(const Format format, const Containers::StringView version) {
    return doSetInputFormat(format, version);
}

void AbstractConverter::setInputFormat(const Format format) {
    return setInputFormat(format, {});
}

void AbstractConverter::setOutputFormat(const Format format, const Containers::StringView version) {
    return doSetOutputFormat(format, version);
}

void AbstractConverter::setOutputFormat(const Format format) {
    return setOutputFormat(format, {});
}

void AbstractConverter::setDefinitions(const Containers::ArrayView<const std::pair<Containers::StringView, Containers::StringView>> definitions) {
    CORRADE_ASSERT(features() & ConverterFeature::Preprocess,
        "ShaderTools::AbstractConverter::setDefinitions(): feature not supported", );
    doSetDefinitions(definitions);
}

void AbstractConverter::setDefinitions(std::initializer_list<std::pair<Containers::StringView, Containers::StringView>> definitions) {
    return setDefinitions(Containers::arrayView(definitions));
}

void AbstractConverter::doSetDefinitions(Containers::ArrayView<const std::pair<Containers::StringView, Containers::StringView>>) {
    CORRADE_ASSERT_UNREACHABLE("ShaderTools::AbstractConverter::setDefinitions(): feature advertised but not implemented", );
}

void AbstractConverter::setOptimizationLevel(const Containers::StringView level) {
    CORRADE_ASSERT(features() & ConverterFeature::Optimize,
        "ShaderTools::AbstractConverter::setOptimizationLevel(): feature not supported", );
    doSetOptimizationLevel(level);
}

void AbstractConverter::doSetOptimizationLevel(Containers::StringView) {
    CORRADE_ASSERT_UNREACHABLE("ShaderTools::AbstractConverter::setOptimizationLevel(): feature advertised but not implemented", );
}

void AbstractConverter::setDebugInfoLevel(const Containers::StringView level) {
    CORRADE_ASSERT(features() & ConverterFeature::DebugInfo,
        "ShaderTools::AbstractConverter::setDebugInfoLevel(): feature not supported", );
    doSetDebugInfoLevel(level);
}

void AbstractConverter::doSetDebugInfoLevel(Containers::StringView) {
    CORRADE_ASSERT_UNREACHABLE("ShaderTools::AbstractConverter::setDebugInfoLevel(): feature advertised but not implemented", );
}

std::pair<bool, Containers::String> AbstractConverter::validateData(const Stage stage, const Containers::ArrayView<const void> data) {
    CORRADE_ASSERT(features() & ConverterFeature::ValidateData,
        "ShaderTools::AbstractConverter::validateData(): feature not supported", {});
    CORRADE_ASSERT(!(_flags & ConverterFlag::PreprocessOnly),
        "ShaderTools::AbstractConverter::validateData(): PreprocessOnly is not allowed in combination with validation", {});

    /* Cast to a non-void type for more convenience */
    std::pair<bool, Containers::String> out = doValidateData(stage, Containers::arrayCast<const char>(data));
    CORRADE_ASSERT(out.second.isSmall() || !out.second.deleter(),
        "ShaderTools::AbstractConverter::validateData(): implementation is not allowed to use a custom String deleter", {});
    return out;
}

std::pair<bool, Containers::String> AbstractConverter::doValidateData(Stage, Containers::ArrayView<const char>) {
    CORRADE_ASSERT_UNREACHABLE("ShaderTools::AbstractConverter::validateData(): feature advertised but not implemented", {});
}

std::pair<bool, Containers::String> AbstractConverter::validateFile(const Stage stage, const Containers::StringView filename) {
    CORRADE_ASSERT(features() & (ConverterFeature::ValidateFile|ConverterFeature::ValidateData),
        "ShaderTools::AbstractConverter::validateFile(): feature not supported", {});
    CORRADE_ASSERT(!(_flags & ConverterFlag::PreprocessOnly),
        "ShaderTools::AbstractConverter::validateFile(): PreprocessOnly is not allowed in combination with validation", {});

    std::pair<bool, Containers::String> out;

    /* If input file callbacks are not set or the converter supports handling
       them directly, call into the implementation */
    if(!_inputFileCallback || (doFeatures() & ConverterFeature::InputFileCallback)) {
        out = doValidateFile(stage, filename);

    /* Otherwise, if validating data is supported, use the callback and pass
       the data through to validateData(). Mark the file as ready to be closed
       once validating is finished. */
    } else if(doFeatures() & ConverterFeature::ValidateData) {
        /* This needs to be duplicated here and in the doValidateFile()
           implementation in order to support both following cases:
            - plugins that don't support InputFileCallback but have their own
              doValidateFile() implementation (callback needs to be used here,
              because the base doValidateFile() implementation might never get
              called)
            - plugins that support InputFileCallback but want to delegate the
              actual file loading to the default implementation (callback used
              in the base doValidateFile() implementation, because this branch
              is never taken in that case) */
        const Containers::Optional<Containers::ArrayView<const char>> data = _inputFileCallback(filename, InputFileCallbackPolicy::LoadTemporary, _inputFileCallbackUserData);
        if(!data) {
            Error{} << "ShaderTools::AbstractConverter::validateFile(): cannot open file" << filename;
            return {};
        }
        out = doValidateData(stage, *data);
        _inputFileCallback(filename, InputFileCallbackPolicy::Close, _inputFileCallbackUserData);

    /* Shouldn't get here, the assert is fired already in setFileCallback() */
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

    CORRADE_ASSERT(out.second.isSmall() || !out.second.deleter(),
        "ShaderTools::AbstractConverter::validateFile(): implementation is not allowed to use a custom String deleter", {});
    return out;
}

std::pair<bool, Containers::String> AbstractConverter::doValidateFile(const Stage stage, const Containers::StringView filename) {
    CORRADE_ASSERT(features() >= ConverterFeature::ValidateData, "ShaderTools::AbstractConverter::validateFile(): feature advertised but not implemented", {});

    /* If callbacks are set, use them. This is the same implementation as in
       validateFile(), see the comment there for details. */
    if(_inputFileCallback) {
        const Containers::Optional<Containers::ArrayView<const char>> data = _inputFileCallback(filename, InputFileCallbackPolicy::LoadTemporary, _inputFileCallbackUserData);
        if(!data) {
            Error{} << "ShaderTools::AbstractConverter::validateFile(): cannot open file" << filename;
            return {};
        }
        std::pair<bool, Containers::String> out = doValidateData(stage, *data);
        _inputFileCallback(filename, InputFileCallbackPolicy::Close, _inputFileCallbackUserData);
        return out;

    /* Otherwise open the file directly */
    } else {
        if(!Utility::Directory::exists(filename)) {
            Error() << "ShaderTools::AbstractConverter::validateFile(): cannot open file" << filename;
            return {};
        }

        return doValidateData(stage, Utility::Directory::read(filename));
    }
}

Containers::Array<char> AbstractConverter::convertDataToData(const Stage stage, const Containers::ArrayView<const void> data) {
    CORRADE_ASSERT(features() >= ConverterFeature::ConvertData,
        "ShaderTools::AbstractConverter::convertDataToData(): feature not supported", {});

    /* Cast to a non-void type for more convenience */
    Containers::Array<char> out = doConvertDataToData(stage, Containers::arrayCast<const char>(data));
    CORRADE_ASSERT(!out.deleter(),
        "ShaderTools::AbstractConverter::convertDataToData(): implementation is not allowed to use a custom Array deleter", {});
    return out;
}

Containers::Array<char> AbstractConverter::doConvertDataToData(Stage, Containers::ArrayView<const char>) {
    CORRADE_ASSERT_UNREACHABLE("ShaderTools::AbstractConverter::convertDataToData(): feature advertised but not implemented", {});
}

bool AbstractConverter::convertDataToFile(const Stage stage, const Containers::ArrayView<const void> data, const Containers::StringView to) {
    CORRADE_ASSERT(features() >= ConverterFeature::ConvertData,
        "ShaderTools::AbstractConverter::convertDataToFile(): feature not supported", {});

    /** @todo this needs expansion once output callbacks are supported as well */

    /* Cast to a non-void type for more convenience */
    Containers::Array<char> out = doConvertDataToData(stage, Containers::arrayCast<const char>(data));
    if(!out) return false;

    if(!Utility::Directory::write(to, out)) {
        Error{} << "ShaderTools::AbstractConverter::convertDataToFile(): cannot write to file" << to;
        return false;
    }

    return true;
}

Containers::Array<char> AbstractConverter::convertDataToDataUsingInputFileCallbacks(const char* const prefix, const Stage stage, const Containers::StringView from) {
    const Containers::Optional<Containers::ArrayView<const char>> data = _inputFileCallback(from, InputFileCallbackPolicy::LoadTemporary, _inputFileCallbackUserData);
    if(!data) {
        Error{} << prefix << "cannot open file" << from;
        return {};
    }
    Containers::Array<char> out = doConvertDataToData(stage, *data);
    _inputFileCallback(from, InputFileCallbackPolicy::Close, _inputFileCallbackUserData);
    return out;
}

bool AbstractConverter::convertFileToFile(const Stage stage, const Containers::StringView from, const Containers::StringView to) {
    CORRADE_ASSERT(features() & (ConverterFeature::ConvertFile|ConverterFeature::ConvertData),
        "ShaderTools::AbstractConverter::convertFileToFile(): feature not supported", {});

    /** @todo this needs expansion once output callbacks are supported as well */

    /* If input file callbacks are not set or the converter supports handling
       them directly, call into the implementation */
    if(!_inputFileCallback || (doFeatures() & ConverterFeature::InputFileCallback)) {
        return doConvertFileToFile(stage, from, to);

    /* Otherwise, if converting data is supported, use the callback and pass
       the data through to convertDataToData(). Mark the file as ready to be
       closed once conversion is finished. */
    } else if(doFeatures() & ConverterFeature::ConvertData) {
        /* This needs to be duplicated here and in the doConvertFileToFile()
           implementation in order to support both following cases:
            - plugins that don't support InputFileCallback but have their own
              doConvertFileToFile() implementation (callback needs to be used
              here, because the base doConvertFileToFile() implementation might
              never get called)
            - plugins that support InputFileCallback but want to delegate the
              actual file loading to the default implementation (callback used
              in the base doConvertFileToFile() implementation, because this
              branch is never taken in that case) */
        Containers::Array<char> out = convertDataToDataUsingInputFileCallbacks("ShaderTools::AbstractConverter::convertFileToFile():", stage, from);
        if(!out) return false;

        if(!Utility::Directory::write(to, out)) {
            Error{} << "ShaderTools::AbstractConverter::convertFileToFile(): cannot write to file" << to;
            return false;
        }

        return true;

    /* Shouldn't get here, the assert is fired already in setFileCallback() */
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

bool AbstractConverter::doConvertFileToFile(const Stage stage, const Containers::StringView from, const Containers::StringView to) {
    CORRADE_ASSERT(features() >= ConverterFeature::ConvertData, "ShaderTools::AbstractConverter::convertFileToFile(): feature advertised but not implemented", {});

    /** @todo this needs expansion once output callbacks are supported as well */
    Containers::Array<char> out;

    /* If callbacks are set, use them. This is the same implementation as in
       convertFileToFile(), see the comment there for details. */
    if(_inputFileCallback) {
        out = convertDataToDataUsingInputFileCallbacks("ShaderTools::AbstractConverter::convertFileToFile():", stage, from);

    /* Otherwise open the file directly */
    } else {
        if(!Utility::Directory::exists(from)) {
            Error() << "ShaderTools::AbstractConverter::convertFileToFile(): cannot open file" << from;
            return {};
        }

        out = doConvertDataToData(stage, Utility::Directory::read(from));
    }

    if(!out) return false;

    if(!Utility::Directory::write(to, out)) {
        Error{} << "ShaderTools::AbstractConverter::convertFileToFile(): cannot write to file" << to;
        return false;
    }

    return true;
}

Containers::Array<char> AbstractConverter::convertFileToData(const Stage stage, const Containers::StringView from) {
    CORRADE_ASSERT(features() >= ConverterFeature::ConvertData,
        "ShaderTools::AbstractConverter::convertFileToData(): feature not supported", {});

    Containers::Array<char> out;

    /* If input file callbacks are not set or the converter supports handling
       them directly, call into the implementation */
    if(!_inputFileCallback || (doFeatures() & ConverterFeature::InputFileCallback)) {
        out = doConvertFileToData(stage, from);

    /* Otherwise use the callback and pass the data through to
       convertDataToData(). Mark the file as ready to be closed once conversion
       is finished. */
    } else {
        /* This needs to be duplicated here and in the doConvertFileToData()
           implementation in order to support both following cases:
            - plugins that don't support InputFileCallback but have their own
              doConvertFileToData() implementation (callback needs to be used
              here, because the base doConvertFileToData() implementation might
              never get called)
            - plugins that support InputFileCallback but want to delegate the
              actual file loading to the default implementation (callback used
              in the base doConvertFileToData() implementation, because this
              branch is never taken in that case) */
        out = convertDataToDataUsingInputFileCallbacks("ShaderTools::AbstractConverter::convertFileToData():", stage, from);
    }

    CORRADE_ASSERT(!out.deleter(),
        "ShaderTools::AbstractConverter::convertFileToData(): implementation is not allowed to use a custom Array deleter", {});
    return out;
}

Containers::Array<char> AbstractConverter::doConvertFileToData(const Stage stage, const Containers::StringView from) {
    /* If callbacks are set, use them. This is the same implementation as in
       convertFileToFile(), see the comment there for details. */
    if(_inputFileCallback) {
        return convertDataToDataUsingInputFileCallbacks("ShaderTools::AbstractConverter::convertFileToData():", stage, from);

    /* Otherwise open the file directly */
    } else {
        if(!Utility::Directory::exists(from)) {
            Error() << "ShaderTools::AbstractConverter::convertFileToData(): cannot open file" << from;
            return {};
        }

        return doConvertDataToData(stage, Utility::Directory::read(from));
    }
}

Containers::Array<char> AbstractConverter::linkDataToData(const Containers::ArrayView<const std::pair<Stage, Containers::ArrayView<const void>>> data) {
    CORRADE_ASSERT(features() >= ConverterFeature::LinkData,
        "ShaderTools::AbstractConverter::linkDataToData(): feature not supported", {});
    CORRADE_ASSERT(!(_flags & ConverterFlag::PreprocessOnly),
        "ShaderTools::AbstractConverter::linkDataToData(): PreprocessOnly is not allowed in combination with linking", {});
    CORRADE_ASSERT(!data.empty(),
        "ShaderTools::AbstractConverter::linkDataToData(): no data passed", {});

    /* Cast to a non-void type for more convenience */
    Containers::Array<char> out = doLinkDataToData(Containers::arrayCast<const std::pair<Stage, Containers::ArrayView<const char>>>(data));
    CORRADE_ASSERT(!out.deleter(),
        "ShaderTools::AbstractConverter::linkDataToData(): implementation is not allowed to use a custom Array deleter", {});
    return out;
}

Containers::Array<char> AbstractConverter::linkDataToData(const std::initializer_list<std::pair<Stage, Containers::ArrayView<const void>>> data) {
    return linkDataToData(Containers::arrayView(data));
}

Containers::Array<char> AbstractConverter::doLinkDataToData(Containers::ArrayView<const std::pair<Stage, Containers::ArrayView<const char>>>) {
    CORRADE_ASSERT_UNREACHABLE("ShaderTools::AbstractConverter::linkDataToData(): feature advertised but not implemented", {});
}

bool AbstractConverter::linkDataToFile(const Containers::ArrayView<const std::pair<Stage, Containers::ArrayView<const void>>> data, const Containers::StringView to) {
    CORRADE_ASSERT(features() >= ConverterFeature::LinkData,
        "ShaderTools::AbstractConverter::linkDataToFile(): feature not supported", {});
    CORRADE_ASSERT(!(_flags & ConverterFlag::PreprocessOnly),
        "ShaderTools::AbstractConverter::linkDataToFile(): PreprocessOnly is not allowed in combination with linking", {});
    CORRADE_ASSERT(!data.empty(),
        "ShaderTools::AbstractConverter::linkDataToFile(): no data passed", {});

    /** @todo this needs expansion once output callbacks are supported as well */

    /* Cast to a non-void type for more convenience */
    Containers::Array<char> out = doLinkDataToData(Containers::arrayCast<const std::pair<Stage, Containers::ArrayView<const char>>>(data));
    if(!out) return false;

    if(!Utility::Directory::write(to, out)) {
        Error{} << "ShaderTools::AbstractConverter::linkDataToFile(): cannot write to file" << to;
        return false;
    }

    return true;
}

bool AbstractConverter::linkDataToFile(const std::initializer_list<std::pair<Stage, Containers::ArrayView<const void>>> data, const Containers::StringView to) {
    return linkDataToFile(Containers::arrayView(data), to);
}

Containers::Array<char> AbstractConverter::linkDataToDataUsingInputFileCallbacks(const char* const prefix, const Containers::ArrayView<const std::pair<Stage, Containers::StringView>> from) {
    Containers::Array<std::pair<Stage, Containers::ArrayView<const char>>> data{Containers::NoInit, from.size()};

    /* First load all files. Remember how many of these succeeded so we can
       close them again after */
    std::size_t i;
    for(i = 0; i != from.size(); ++i) {
        const Containers::Optional<Containers::ArrayView<const char>> contents = _inputFileCallback(from[i].second, InputFileCallbackPolicy::LoadTemporary, _inputFileCallbackUserData);
        if(!contents) break;

        data[i].first = from[i].first;
        data[i].second = *contents;
    }

    /* If all input files loaded successfully, process */
    Containers::Array<char> out;
    if(i == from.size()) out = doLinkDataToData(data);

    /* Close again all input files that loaded successfully */
    for(std::size_t ii = 0; ii != i; ++ii)
        _inputFileCallback(from[ii].second, InputFileCallbackPolicy::Close, _inputFileCallbackUserData);

    /* Now that we have cleaned up correctly, it's time print the error message
       if something didn't go well. IN this case doLinkDataToData() was not
       called at all. */
    if(i != from.size()) {
        Error{} << prefix << "cannot open file" << from[i].second;
        return {};
    }

    /* Return the data. This could have failed too, but the error message got
       printed by the implementation already */
    return out;
}

bool AbstractConverter::linkFilesToFile(const Containers::ArrayView<const std::pair<Stage, Containers::StringView>> from, const Containers::StringView to) {
    CORRADE_ASSERT(features() & (ConverterFeature::LinkFile|ConverterFeature::LinkData),
        "ShaderTools::AbstractConverter::linkFilesToFile(): feature not supported", {});
    CORRADE_ASSERT(!(_flags & ConverterFlag::PreprocessOnly),
        "ShaderTools::AbstractConverter::linkFilesToFile(): PreprocessOnly is not allowed in combination with linking", {});
    CORRADE_ASSERT(!from.empty(),
        "ShaderTools::AbstractConverter::linkFilesToFile(): no files passed", {});

    /** @todo this needs expansion once output callbacks are supported as well */

    /* If input file callbacks are not set or the converter supports handling
       them directly, call into the implementation */
    if(!_inputFileCallback || (doFeatures() & ConverterFeature::InputFileCallback)) {
        return doLinkFilesToFile(from, to);

    /* Otherwise, if converting data is supported, use the callback and pass
       the data through to convertDataToData(). Mark the file as ready to be
       closed once conversion is finished. */
    } else if(doFeatures() & ConverterFeature::LinkData) {
        /* This needs to be duplicated here and in the doLinkFilesToFile()
           implementation in order to support both following cases:
            - plugins that don't support InputFileCallback but have their own
              doLinkFilesToFile() implementation (callback needs to be used
              here, because the base doLinkFilesToFile() implementation might
              never get called)
            - plugins that support InputFileCallback but want to delegate the
              actual file loading to the default implementation (callback used
              in the base doLinkFilesToFile() implementation, because this
              branch is never taken in that case) */
        Containers::Array<char> out = linkDataToDataUsingInputFileCallbacks("ShaderTools::AbstractConverter::linkFilesToFile():", from);
        if(!out) return false;

        if(!Utility::Directory::write(to, out)) {
            Error{} << "ShaderTools::AbstractConverter::linkFilesToFile(): cannot write to file" << to;
            return false;
        }

        return true;

    /* Shouldn't get here, the assert is fired already in setFileCallback() */
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

bool AbstractConverter::linkFilesToFile(const std::initializer_list<std::pair<Stage, Containers::StringView>> from, const Containers::StringView to) {
    return linkFilesToFile(Containers::arrayView(from), to);
}

bool AbstractConverter::doLinkFilesToFile(const Containers::ArrayView<const std::pair<Stage, Containers::StringView>> from, const Containers::StringView to) {
    CORRADE_ASSERT(features() >= ConverterFeature::LinkData, "ShaderTools::AbstractConverter::linkFilesToFile(): feature advertised but not implemented", {});

    /** @todo this needs expansion once output callbacks are supported as well */
    Containers::Array<char> out;

    /* If callbacks are set, use them. This is the same implementation as in
       convertFileToFile(), see the comment there for details. */
    if(_inputFileCallback) {
        out = linkDataToDataUsingInputFileCallbacks("ShaderTools::AbstractConverter::linkFilesToFile():", from);

    /* Otherwise open the files directly */
    } else {
        Containers::Array<Containers::Array<char>> fileData{from.size()};
        for(std::size_t i = 0; i != from.size(); ++i) {
            if(!Utility::Directory::exists(from[i].second)) {
                Error() << "ShaderTools::AbstractConverter::linkFilesToFile(): cannot open file" << from[i].second;
                return {};
            }

            fileData[i] = Utility::Directory::read(from[i].second);
        }

        /** @todo merge the allocations once we have an ArrayTuple */
        Containers::Array<std::pair<Stage, Containers::ArrayView<const char>>> data{Containers::NoInit, from.size()};
        for(std::size_t i = 0; i != from.size(); ++i)  {
            data[i].first = from[i].first;
            data[i].second = fileData[i];
        }

        out = doLinkDataToData(data);
    }

    if(!out) return false;

    if(!Utility::Directory::write(to, out)) {
        Error{} << "ShaderTools::AbstractConverter::linkFilesToFile(): cannot write to file" << to;
        return false;
    }

    return true;
}

Containers::Array<char> AbstractConverter::linkFilesToData(const Containers::ArrayView<const std::pair<Stage, Containers::StringView>> from) {
    CORRADE_ASSERT(features() >= ConverterFeature::LinkData,
        "ShaderTools::AbstractConverter::linkFilesToData(): feature not supported", {});
    CORRADE_ASSERT(!(_flags & ConverterFlag::PreprocessOnly),
        "ShaderTools::AbstractConverter::linkFilesToData(): PreprocessOnly is not allowed in combination with linking", {});
    CORRADE_ASSERT(!from.empty(),
        "ShaderTools::AbstractConverter::linkFilesToData(): no files passed", {});

    Containers::Array<char> out;

    /* If input file callbacks are not set or the converter supports handling
       them directly, call into the implementation */
    if(!_inputFileCallback || (doFeatures() & ConverterFeature::InputFileCallback)) {
        out = doLinkFilesToData(from);

    /* Otherwise use the callback and pass the data through to
       convertDataToData(). Mark the file as ready to be closed once conversion
       is finished. */
    } else {
        /* This needs to be duplicated here and in the doConvertFileToData()
           implementation in order to support both following cases:
            - plugins that don't support InputFileCallback but have their own
              doLinkFilesToData() implementation (callback needs to be used
              here, because the base doLinkFilesToData() implementation might
              never get called)
            - plugins that support InputFileCallback but want to delegate the
              actual file loading to the default implementation (callback used
              in the base doLinkFilesToData() implementation, because this
              branch is never taken in that case) */
        out = linkDataToDataUsingInputFileCallbacks("ShaderTools::AbstractConverter::linkFilesToData():", from);
    }

    CORRADE_ASSERT(!out.deleter(),
        "ShaderTools::AbstractConverter::linkFilesToData(): implementation is not allowed to use a custom Array deleter", {});
    return out;
}

Containers::Array<char> AbstractConverter::linkFilesToData(const std::initializer_list<std::pair<Stage, Containers::StringView>> from) {
    return linkFilesToData(Containers::arrayView(from));
}

Containers::Array<char> AbstractConverter::doLinkFilesToData(const Containers::ArrayView<const std::pair<Stage, Containers::StringView>> from) {
    /* If callbacks are set, use them. This is the same implementation as in
       linkFilesToFile(), see the comment there for details. */
    if(_inputFileCallback) {
        return linkDataToDataUsingInputFileCallbacks("ShaderTools::AbstractConverter::linkFilesToData():", from);

    /* Otherwise open the files directly */
    } else {
        Containers::Array<Containers::Array<char>> fileData{from.size()};
        for(std::size_t i = 0; i != from.size(); ++i) {
            if(!Utility::Directory::exists(from[i].second)) {
                Error() << "ShaderTools::AbstractConverter::linkFilesToData(): cannot open file" << from[i].second;
                return {};
            }

            fileData[i] = Utility::Directory::read(from[i].second);
        }

        /** @todo merge the allocations once we have an ArrayTuple */
        Containers::Array<std::pair<Stage, Containers::ArrayView<const char>>> data{Containers::NoInit, from.size()};
        for(std::size_t i = 0; i != from.size(); ++i)  {
            data[i].first = from[i].first;
            data[i].second = fileData[i];
        }

        return doLinkDataToData(data);
    }
}

Debug& operator<<(Debug& debug, const ConverterFeature value) {
    debug << "ShaderTools::ConverterFeature" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case ConverterFeature::v: return debug << "::" #v;
        _c(ValidateData)
        _c(ValidateFile)
        _c(ConvertData)
        _c(ConvertFile)
        _c(LinkData)
        _c(LinkFile)
        _c(InputFileCallback)
        _c(Preprocess)
        _c(Optimize)
        _c(DebugInfo)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const ConverterFeatures value) {
    return Containers::enumSetDebugOutput(debug, value, "ShaderTools::ConverterFeatures{}", {
        ConverterFeature::ValidateData,
        /* Implied by ValidateData, has to be after */
        ConverterFeature::ValidateFile,
        ConverterFeature::ConvertData,
        /* Implied by ConvertData, has to be after */
        ConverterFeature::ConvertFile,
        ConverterFeature::LinkData,
        /* Implied by LinkData, has to be after */
        ConverterFeature::LinkFile,
        ConverterFeature::InputFileCallback,
        ConverterFeature::Preprocess,
        ConverterFeature::Optimize,
        ConverterFeature::DebugInfo
    });
}

Debug& operator<<(Debug& debug, const ConverterFlag value) {
    debug << "ShaderTools::ConverterFlag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case ConverterFlag::v: return debug << "::" #v;
        _c(Quiet)
        _c(Verbose)
        _c(WarningAsError)
        _c(PreprocessOnly)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const ConverterFlags value) {
    return Containers::enumSetDebugOutput(debug, value, "ShaderTools::ConverterFlags{}", {
        ConverterFlag::Quiet,
        ConverterFlag::Verbose,
        ConverterFlag::WarningAsError,
        ConverterFlag::PreprocessOnly
    });
}

Debug& operator<<(Debug& debug, const Format value) {
    debug << "ShaderTools::Format" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case Format::v: return debug << "::" #v;
        _c(Unspecified)
        _c(Glsl)
        _c(Spirv)
        _c(SpirvAssembly)
        _c(Hlsl)
        _c(Msl)
        _c(Wgsl)
        _c(Dxil)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const Stage value) {
    debug << "ShaderTools::Stage" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case Stage::v: return debug << "::" #v;
        _c(Unspecified)
        _c(Vertex)
        _c(Fragment)
        _c(Geometry)
        _c(TessellationControl)
        _c(TessellationEvaluation)
        _c(Compute)
        _c(RayGeneration)
        _c(RayAnyHit)
        _c(RayClosestHit)
        _c(RayMiss)
        _c(RayIntersection)
        _c(RayCallable)
        _c(MeshTask)
        _c(Mesh)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

}}
