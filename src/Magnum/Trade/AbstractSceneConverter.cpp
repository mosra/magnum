/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

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

#include "AbstractSceneConverter.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/String.h>
#include <Corrade/PluginManager/Manager.hpp>
#include <Corrade/Utility/Path.h>

#include "Magnum/Trade/ArrayAllocator.h"
#include "Magnum/Trade/MeshData.h"

#ifdef MAGNUM_BUILD_DEPRECATED
/* needed by deprecated convertToFile() that takes a std::string */
#include <Corrade/Containers/StringStl.h>
#endif

#ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
#include "Magnum/Trade/configure.h"
#endif

namespace Corrade { namespace PluginManager {

/* On non-MinGW Windows the instantiations are already marked with extern
   template. However Clang-CL doesn't propagate the export from the extern
   template, it seems. */
#if !defined(CORRADE_TARGET_WINDOWS) || defined(CORRADE_TARGET_MINGW) || defined(CORRADE_TARGET_CLANG_CL)
#define MAGNUM_TRADE_EXPORT_HPP MAGNUM_TRADE_EXPORT
#else
#define MAGNUM_TRADE_EXPORT_HPP
#endif
template class MAGNUM_TRADE_EXPORT_HPP Manager<Magnum::Trade::AbstractSceneConverter>;

}}

namespace Magnum { namespace Trade {

using namespace Containers::Literals;

Containers::StringView AbstractSceneConverter::pluginInterface() {
    return
/* [interface] */
"cz.mosra.magnum.Trade.AbstractSceneConverter/0.1.2"_s
/* [interface] */
    ;
}

#ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
Containers::Array<Containers::String> AbstractSceneConverter::pluginSearchPaths() {
    const Containers::Optional<Containers::String> libraryLocation = Utility::Path::libraryLocation(&pluginInterface);
    return PluginManager::implicitPluginSearchPaths(
        #ifndef MAGNUM_BUILD_STATIC
        libraryLocation ? *libraryLocation : Containers::String{},
        #else
        {},
        #endif
        #ifdef CORRADE_IS_DEBUG_BUILD
        MAGNUM_PLUGINS_SCENECONVERTER_DEBUG_DIR,
        #else
        MAGNUM_PLUGINS_SCENECONVERTER_DIR,
        #endif
        #ifdef CORRADE_IS_DEBUG_BUILD
        "magnum-d/"
        #else
        "magnum/"
        #endif
        "sceneconverters"_s);
}
#endif

AbstractSceneConverter::AbstractSceneConverter() = default;

AbstractSceneConverter::AbstractSceneConverter(PluginManager::Manager<AbstractSceneConverter>& manager): PluginManager::AbstractManagingPlugin<AbstractSceneConverter>{manager} {}

AbstractSceneConverter::AbstractSceneConverter(PluginManager::AbstractManager& manager, const Containers::StringView& plugin): PluginManager::AbstractManagingPlugin<AbstractSceneConverter>{manager, plugin} {}

AbstractSceneConverter::~AbstractSceneConverter() = default;

SceneConverterFeatures AbstractSceneConverter::features() const {
    const SceneConverterFeatures features = doFeatures();
    CORRADE_ASSERT(features, "Trade::AbstractSceneConverter::features(): implementation reported no features", {});
    return features;
}

void AbstractSceneConverter::setFlags(SceneConverterFlags flags) {
    _flags = flags;
    doSetFlags(flags);
}

void AbstractSceneConverter::doSetFlags(SceneConverterFlags) {}

void AbstractSceneConverter::addFlags(SceneConverterFlags flags) {
    setFlags(_flags|flags);
}

void AbstractSceneConverter::clearFlags(SceneConverterFlags flags) {
    setFlags(_flags & ~flags);
}

Containers::Optional<MeshData> AbstractSceneConverter::convert(const MeshData& mesh) {
    CORRADE_ASSERT(features() & SceneConverterFeature::ConvertMesh,
        "Trade::AbstractSceneConverter::convert(): mesh conversion not supported", {});

    Containers::Optional<MeshData> out = doConvert(mesh);
    CORRADE_ASSERT(!out || (
        (!out->_indexData.deleter() || out->_indexData.deleter() == static_cast<void(*)(char*, std::size_t)>(Implementation::nonOwnedArrayDeleter) || out->_indexData.deleter() == ArrayAllocator<char>::deleter) &&
        (!out->_vertexData.deleter() || out->_vertexData.deleter() == static_cast<void(*)(char*, std::size_t)>(Implementation::nonOwnedArrayDeleter) || out->_vertexData.deleter() == ArrayAllocator<char>::deleter) &&
        (!out->_attributes.deleter() || out->_attributes.deleter() == static_cast<void(*)(MeshAttributeData*, std::size_t)>(Implementation::nonOwnedArrayDeleter))),
        "Trade::AbstractSceneConverter::convert(): implementation is not allowed to use a custom Array deleter", {});
    return out;
}

Containers::Optional<MeshData> AbstractSceneConverter::doConvert(const MeshData&) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::convert(): mesh conversion advertised but not implemented", {});
}

bool AbstractSceneConverter::convertInPlace(MeshData& mesh) {
    CORRADE_ASSERT(features() & SceneConverterFeature::ConvertMeshInPlace,
        "Trade::AbstractSceneConverter::convertInPlace(): mesh conversion not supported", {});

    return doConvertInPlace(mesh);
}

bool AbstractSceneConverter::doConvertInPlace(MeshData&) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::convertInPlace(): mesh conversion advertised but not implemented", {});
}

#ifndef MAGNUM_BUILD_DEPRECATED
Containers::Optional<Containers::Array<char>>
#else
Implementation::SceneConverterOptionalButAlsoArray<char>
#endif
AbstractSceneConverter::convertToData(const MeshData& mesh) {
    CORRADE_ASSERT(features() >= SceneConverterFeature::ConvertMeshToData,
        "Trade::AbstractSceneConverter::convertToData(): mesh conversion not supported", {});

    Containers::Optional<Containers::Array<char>> out = doConvertToData(mesh);
    CORRADE_ASSERT(!out || !out->deleter() || out->deleter() == static_cast<void(*)(char*, std::size_t)>(Implementation::nonOwnedArrayDeleter) || out->deleter() == ArrayAllocator<char>::deleter,
        "Trade::AbstractSceneConverter::convertToData(): implementation is not allowed to use a custom Array deleter", {});

    /* GCC 4.8 and Clang 3.8 need an explicit conversion here */
    #ifdef MAGNUM_BUILD_DEPRECATED
    return Implementation::SceneConverterOptionalButAlsoArray<char>{std::move(out)};
    #else
    return out;
    #endif
}

Containers::Optional<Containers::Array<char>> AbstractSceneConverter::doConvertToData(const MeshData&) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::convertToData(): mesh conversion advertised but not implemented", {});
}

bool AbstractSceneConverter::convertToFile(const MeshData& mesh, const Containers::StringView filename) {
    CORRADE_ASSERT(features() >= SceneConverterFeature::ConvertMeshToFile,
        "Trade::AbstractSceneConverter::convertToFile(): mesh conversion not supported", {});

    return doConvertToFile(mesh, filename);
}

#ifdef MAGNUM_BUILD_DEPRECATED
bool AbstractSceneConverter::convertToFile(const std::string& filename, const MeshData& mesh) {
    return convertToFile(mesh, filename);
}
#endif

bool AbstractSceneConverter::doConvertToFile(const MeshData& mesh, const Containers::StringView filename) {
    CORRADE_ASSERT(features() >= SceneConverterFeature::ConvertMeshToData, "Trade::AbstractSceneConverter::convertToFile(): mesh conversion advertised but not implemented", false);

    const Containers::Optional<Containers::Array<char>> data = doConvertToData(mesh);
    /* No deleter checks as it doesn't matter here */
    if(!data) return false;

    if(!Utility::Path::write(filename, *data)) {
        Error() << "Trade::AbstractSceneConverter::convertToFile(): cannot write to file" << filename;
        return false;
    }

    return true;
}

Debug& operator<<(Debug& debug, const SceneConverterFeature value) {
    debug << "Trade::SceneConverterFeature" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case SceneConverterFeature::v: return debug << "::" #v;
        _c(ConvertMesh)
        _c(ConvertMeshInPlace)
        _c(ConvertMeshToData)
        _c(ConvertMeshToFile)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const SceneConverterFeatures value) {
    return Containers::enumSetDebugOutput(debug, value, "Trade::SceneConverterFeatures{}", {
        SceneConverterFeature::ConvertMesh,
        SceneConverterFeature::ConvertMeshInPlace,
        SceneConverterFeature::ConvertMeshToData,
        /* Implied by ConvertMeshToData, has to be after */
        SceneConverterFeature::ConvertMeshToFile});
}

Debug& operator<<(Debug& debug, const SceneConverterFlag value) {
    debug << "Trade::SceneConverterFlag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case SceneConverterFlag::v: return debug << "::" #v;
        _c(Verbose)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const SceneConverterFlags value) {
    return Containers::enumSetDebugOutput(debug, value, "Trade::SceneConverterFlags{}", {
        SceneConverterFlag::Verbose});
}

}}
