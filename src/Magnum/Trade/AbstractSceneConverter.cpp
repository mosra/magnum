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

#include <Corrade/Containers/AnyReference.h>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/String.h>
#include <Corrade/PluginManager/Manager.hpp>
#include <Corrade/Utility/Path.h>

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ArrayAllocator.h"
#include "Magnum/Trade/ImageData.h"
#include "Magnum/Trade/MeshData.h"
#include "Magnum/Trade/SceneData.h"

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

/* Gets allocated in begin*() and deallocated in end*() or abort(). The direct
   conversion functions such as convert(const MeshData&) don't need this
   state. */
struct AbstractSceneConverter::State {
    enum class Type {
        Convert,
        ConvertToData,
        ConvertToFile
    };

    explicit State(Type type): type{type} {
        if(type == Type::Convert)
            new(&converted.mesh) Containers::Optional<MeshData>{};
        else if(type == Type::ConvertToData)
            new(&converted.meshToData) Containers::Optional<Containers::Array<char>>{};
        else if(type == Type::ConvertToFile)
            new(&converted.meshToFile) bool{};
        else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    }

    ~State() {
        if(type == Type::Convert)
            converted.mesh.~Optional();
        else if(type == Type::ConvertToData)
            converted.meshToData.~Optional();
        else if(type == Type::ConvertToFile)
            ;
        else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    }

    Type type;

    UnsignedInt sceneCount = 0;
    UnsignedInt animationCount = 0;
    UnsignedInt lightCount = 0;
    UnsignedInt cameraCount = 0;
    UnsignedInt skin2DCount = 0;
    UnsignedInt skin3DCount = 0;
    UnsignedInt meshCount = 0;
    UnsignedInt materialCount = 0;
    UnsignedInt textureCount = 0;
    UnsignedInt image1DCount = 0;
    UnsignedInt image2DCount = 0;
    UnsignedInt image3DCount = 0;

    /* Used if type == Type::ConvertToFile. Could theoretically be in the same
       allocation as State (ArrayTuple?), or at least reusing the space in
       `converted`, but I don't think a single allocation matters that much. */
    Containers::String filename;

    union Converted {
        /* C++, FUCK OFF, what's the point of requiring me to create an
           explicit constructor and destructor if I have no way to store or
           know the information about the active field at this point?! */
        Converted() noexcept {}
        ~Converted() {}

        Containers::Optional<MeshData> mesh;
        Containers::Optional<Containers::Array<char>> meshToData;
        bool meshToFile;
    } converted;
};

Containers::StringView AbstractSceneConverter::pluginInterface() {
    return
/* [interface] */
"cz.mosra.magnum.Trade.AbstractSceneConverter/0.2"_s
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

Containers::Optional<Containers::Array<char>> AbstractSceneConverter::doConvertToData(const MeshData& data) {
    if(features() >= (SceneConverterFeature::ConvertMultipleToData|SceneConverterFeature::AddMeshes)) {
        beginData();
        Containers::Optional<Containers::Array<char>> out;
        if(add(data) && (out = endData())) return out;
        return {};
    }

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
    // TODO delegate to batch
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

bool AbstractSceneConverter::isConverting() const {
    return !!_state;
}

void AbstractSceneConverter::abort() {
    if(!_state) return;

    doAbort();
    _state = {};
}

void AbstractSceneConverter::doAbort() {}

void AbstractSceneConverter::begin() {
    CORRADE_ASSERT(features() & (SceneConverterFeature::ConvertMultiple|SceneConverterFeature::ConvertMesh),
        "Trade::AbstractSceneConverter::begin(): feature not supported", );

    if(_state) abort();

    _state.emplace(State::Type::Convert);
    doBegin();
}

void AbstractSceneConverter::doBegin() {
    /* Actual operation performed in doAdd(const MeshData&) */
    if(features() & SceneConverterFeature::ConvertMesh) return;

    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::begin(): feature advertised but not implemented", );
}

Containers::Pointer<AbstractImporter> AbstractSceneConverter::end() {
    CORRADE_ASSERT(_state && _state->type == State::Type::Convert,
        "Trade::AbstractSceneConverter::end(): no conversion in progress", {});
    Containers::Pointer<AbstractImporter> out = doEnd();
    _state = {};
    return out;
}

Containers::Pointer<AbstractImporter> AbstractSceneConverter::doEnd() {
    if(features() & SceneConverterFeature::ConvertMesh) {
        if(_state->meshCount != 1) {
            Error{} << "Trade::AbstractSceneConverter::end(): the converter requires exactly one mesh";
            return {};
        }

        struct SingleMeshImporter: AbstractImporter {
            explicit SingleMeshImporter(Containers::Optional<Trade::MeshData>&& mesh) noexcept: _mesh{std::move(mesh)} {}

            ImporterFeatures doFeatures() const override { return {}; }
            bool doIsOpened() const override { return !!_mesh; }
            void doClose() override { _mesh = {}; }

            UnsignedInt doMeshCount() const override { return 1; }
            Containers::Optional<MeshData> doMesh(UnsignedInt, UnsignedInt) override {
                /* After retrieving the mesh the importer behaves as if
                   closed */
                /** @todo better behavior that doesn't require dependency on
                    MeshTools::reference() or owned()? */
                Containers::Optional<Trade::MeshData> out = std::move(_mesh);
                _mesh = {};
                return out;
            }

            private:
                Containers::Optional<Trade::MeshData> _mesh;
        };

        return Containers::Pointer<AbstractImporter>(new SingleMeshImporter{std::move(_state->converted.mesh)});
    }

    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::end(): feature advertised but not implemented", {});
}

void AbstractSceneConverter::beginData() {
    CORRADE_ASSERT(
        features() >= SceneConverterFeature::ConvertMultipleToData ||
        features() >= SceneConverterFeature::ConvertMeshToData,
        "Trade::AbstractSceneConverter::beginData(): feature not supported", );

    if(_state) abort();

    _state.emplace(State::Type::ConvertToData);
    doBeginData();
}

void AbstractSceneConverter::doBeginData() {
    /* Actual operation performed in doAdd(const MeshData&) */
    if(features() >= SceneConverterFeature::ConvertMeshToData) return;

    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::beginData(): feature advertised but not implemented", );
}

Containers::Optional<Containers::Array<char>> AbstractSceneConverter::endData() {
    CORRADE_ASSERT(_state && _state->type == State::Type::ConvertToData,
        "Trade::AbstractSceneConverter::endData(): no data conversion in progress", {});

    Containers::Optional<Containers::Array<char>> out = doEndData();
    CORRADE_ASSERT(!out || !out->deleter() || out->deleter() == static_cast<void(*)(char*, std::size_t)>(Implementation::nonOwnedArrayDeleter) || out->deleter() == ArrayAllocator<char>::deleter,
        "Trade::AbstractSceneConverter::endData(): implementation is not allowed to use a custom Array deleter", {});
    _state = {};
    return out;
}

Containers::Optional<Containers::Array<char>> AbstractSceneConverter::doEndData() {
    if(features() >= SceneConverterFeature::ConvertMeshToData) {
        if(_state->meshCount != 1) {
            Error{} << "Trade::AbstractSceneConverter::endData(): the converter requires exactly one mesh";
            return {};
        }

        return std::move(_state->converted.meshToData);
    }

    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::endData(): feature advertised but not implemented", {});
}

void AbstractSceneConverter::beginFile(const Containers::StringView filename) {
    CORRADE_ASSERT(features() & (SceneConverterFeature::ConvertMultipleToFile|SceneConverterFeature::ConvertMeshToFile),
        "Trade::AbstractSceneConverter::beginFile(): feature not supported", );

    if(_state) abort();

    _state.emplace(State::Type::ConvertToFile);
    _state->filename = Containers::String::nullTerminatedGlobalView(filename);
    doBeginFile(_state->filename);
}

void AbstractSceneConverter::doBeginFile(Containers::StringView) {
    /* Actual operation performed in doAdd(const MeshData&) */
    if(features() & SceneConverterFeature::ConvertMeshToFile) return;

    CORRADE_ASSERT(features() >= SceneConverterFeature::ConvertMultipleToData,
        "Trade::AbstractSceneConverter::beginFile(): feature advertised but not implemented", );

    doBeginData();
}

bool AbstractSceneConverter::endFile() {
    CORRADE_ASSERT(_state && _state->type == State::Type::ConvertToFile,
        "Trade::AbstractSceneConverter::endFile(): no file conversion in progress", {});

    const bool out = doEndFile(_state->filename);
    _state = {};
    return out;
}

bool AbstractSceneConverter::doEndFile(const Containers::StringView filename) {
    if(features() & SceneConverterFeature::ConvertMeshToFile) {
        if(_state->meshCount != 1) {
            Error{} << "Trade::AbstractSceneConverter::endFile(): the converter requires exactly one mesh";
            return {};
        }

        return _state->converted.meshToFile;
    }

    CORRADE_ASSERT(features() >= SceneConverterFeature::ConvertMultipleToData,
        "Trade::AbstractSceneConverter::endFile(): feature advertised but not implemented", {});

    const Containers::Optional<Containers::Array<char>> data = doEndData();
    /* No deleter checks as it doesn't matter here */
    if(!data) return false;

    if(!Utility::Path::write(filename, *data)) {
        Error{} << "Trade::AbstractSceneConverter::endFile(): cannot write to file" << filename;
        return false;
    }

    return true;
}

UnsignedInt AbstractSceneConverter::sceneCount() const {
    CORRADE_ASSERT(_state, "Trade::AbstractSceneConverter::sceneCount(): no conversion in progress", {});
    return _state->sceneCount;
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const SceneData& scene, const Containers::StringView name) {
    CORRADE_ASSERT(features() & SceneConverterFeature::AddScenes,
        "Trade::AbstractSceneConverter::add(): scene conversion not supported", {});
    CORRADE_ASSERT(_state,
        "Trade::AbstractSceneConverter::add(): no conversion in progress", {});

    if(doAdd(_state->sceneCount, scene, name))
        return _state->sceneCount++;
    return {};
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const SceneData& scene) {
    return add(scene, {});
}

bool AbstractSceneConverter::doAdd(UnsignedInt, const SceneData&, Containers::StringView) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::add(): scene conversion advertised but not implemented", {});
}

void AbstractSceneConverter::setSceneFieldName(const SceneField field, const Containers::StringView name) {
    CORRADE_ASSERT(features() & SceneConverterFeature::AddScenes,
        "Trade::AbstractSceneConverter::setSceneFieldName(): feature not supported", );
    CORRADE_ASSERT(_state,
        "Trade::AbstractSceneConverter::setSceneFieldName(): no conversion in progress", );
    CORRADE_ASSERT(isSceneFieldCustom(field),
        "Trade::AbstractSceneConverter::setSceneFieldName():" << field << "is not custom", );

    doSetSceneFieldName(sceneFieldCustom(field), name);
}

void AbstractSceneConverter::doSetSceneFieldName(UnsignedInt, Containers::StringView) {}

void AbstractSceneConverter::setObjectName(const UnsignedLong object, const Containers::StringView name) {
    CORRADE_ASSERT(features() & SceneConverterFeature::AddScenes,
        "Trade::AbstractSceneConverter::setObjectName(): feature not supported", );
    CORRADE_ASSERT(_state,
        "Trade::AbstractSceneConverter::setObjectName(): no conversion in progress", );

    doSetObjectName(object, name);
}

void AbstractSceneConverter::doSetObjectName(UnsignedLong, Containers::StringView) {}

void AbstractSceneConverter::setDefaultScene(const UnsignedInt id) {
    CORRADE_ASSERT(features() & SceneConverterFeature::AddScenes,
        "Trade::AbstractSceneConverter::setDefaultScene(): feature not supported", );
    CORRADE_ASSERT(_state,
        "Trade::AbstractSceneConverter::setDefaultScene(): no conversion in progress", );
    CORRADE_ASSERT(id < _state->sceneCount,
        "Trade::AbstractSceneConverter::setDefaultScene(): index" << id << "out of range for" << _state->sceneCount << "scenes", );

    doSetDefaultScene(id);
}

void AbstractSceneConverter::doSetDefaultScene(UnsignedInt) {}

UnsignedInt AbstractSceneConverter::animationCount() const {
    CORRADE_ASSERT(_state, "Trade::AbstractSceneConverter::animationCount(): no conversion in progress", {});
    return _state->animationCount;
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const AnimationData& animation, const Containers::StringView name) {
    CORRADE_ASSERT(features() & SceneConverterFeature::AddAnimations,
        "Trade::AbstractSceneConverter::add(): animation conversion not supported", {});
    CORRADE_ASSERT(_state,
        "Trade::AbstractSceneConverter::add(): no conversion in progress", {});

    if(doAdd(_state->animationCount, animation, name))
        return _state->animationCount++;
    return {};
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const AnimationData& animation) {
    return add(animation, {});
}

bool AbstractSceneConverter::doAdd(UnsignedInt, const AnimationData&, Containers::StringView) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::add(): animation conversion advertised but not implemented", {});
}

UnsignedInt AbstractSceneConverter::lightCount() const {
    CORRADE_ASSERT(_state, "Trade::AbstractSceneConverter::lightCount(): no conversion in progress", {});
    return _state->lightCount;
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const LightData& light, const Containers::StringView name) {
    CORRADE_ASSERT(features() & SceneConverterFeature::AddLights,
        "Trade::AbstractSceneConverter::add(): light conversion not supported", {});
    CORRADE_ASSERT(_state,
        "Trade::AbstractSceneConverter::add(): no conversion in progress", {});

    if(doAdd(_state->lightCount, light, name))
        return _state->lightCount++;
    return {};
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const LightData& light) {
    return add(light, {});
}

bool AbstractSceneConverter::doAdd(UnsignedInt, const LightData&, Containers::StringView) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::add(): light conversion advertised but not implemented", {});
}

UnsignedInt AbstractSceneConverter::cameraCount() const {
    CORRADE_ASSERT(_state, "Trade::AbstractSceneConverter::cameraCount(): no conversion in progress", {});
    return _state->cameraCount;
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const CameraData& camera, const Containers::StringView name) {
    CORRADE_ASSERT(features() & SceneConverterFeature::AddCameras,
        "Trade::AbstractSceneConverter::add(): camera conversion not supported", {});
    CORRADE_ASSERT(_state,
        "Trade::AbstractSceneConverter::add(): no conversion in progress", {});

    if(doAdd(_state->cameraCount, camera, name))
        return _state->cameraCount++;
    return {};
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const CameraData& camera) {
    return add(camera, {});
}

bool AbstractSceneConverter::doAdd(UnsignedInt, const CameraData&, Containers::StringView) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::add(): camera conversion advertised but not implemented", {});
}

UnsignedInt AbstractSceneConverter::skin2DCount() const {
    CORRADE_ASSERT(_state, "Trade::AbstractSceneConverter::skin2DCount(): no conversion in progress", {});
    return _state->skin2DCount;
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const SkinData2D& skin, const Containers::StringView name) {
    CORRADE_ASSERT(features() & SceneConverterFeature::AddSkins2D,
        "Trade::AbstractSceneConverter::add(): 2D skin conversion not supported", {});
    CORRADE_ASSERT(_state,
        "Trade::AbstractSceneConverter::add(): no conversion in progress", {});

    if(doAdd(_state->skin2DCount, skin, name))
        return _state->skin2DCount++;
    return {};
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const SkinData2D& skin) {
    return add(skin, {});
}

bool AbstractSceneConverter::doAdd(UnsignedInt, const SkinData2D&, Containers::StringView) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::add(): 2D skin conversion advertised but not implemented", {});
}

UnsignedInt AbstractSceneConverter::skin3DCount() const {
    CORRADE_ASSERT(_state, "Trade::AbstractSceneConverter::skin3DCount(): no conversion in progress", {});
    return _state->skin3DCount;
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const SkinData3D& skin, const Containers::StringView name) {
    CORRADE_ASSERT(features() & SceneConverterFeature::AddSkins3D,
        "Trade::AbstractSceneConverter::add(): 3D skin conversion not supported", {});
    CORRADE_ASSERT(_state,
        "Trade::AbstractSceneConverter::add(): no conversion in progress", {});

    if(doAdd(_state->skin3DCount, skin, name))
        return _state->skin3DCount++;
    return {};
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const SkinData3D& skin) {
    return add(skin, {});
}

bool AbstractSceneConverter::doAdd(UnsignedInt, const SkinData3D&, Containers::StringView) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::add(): 3D skin conversion advertised but not implemented", {});
}

UnsignedInt AbstractSceneConverter::meshCount() const {
    CORRADE_ASSERT(_state, "Trade::AbstractSceneConverter::meshCount(): no conversion in progress", {});
    return _state->meshCount;
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const MeshData& mesh, const Containers::StringView name) {
    CORRADE_ASSERT(features() & (SceneConverterFeature::AddMeshes|
                                 SceneConverterFeature::ConvertMesh|
                                 SceneConverterFeature::ConvertMeshToData|
                                 SceneConverterFeature::ConvertMeshToFile),
        "Trade::AbstractSceneConverter::add(): mesh conversion not supported", {});
    CORRADE_ASSERT(_state,
        "Trade::AbstractSceneConverter::add(): no conversion in progress", {});

    if(doAdd(_state->meshCount, mesh, name))
        return _state->meshCount++;
    return {};
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const MeshData& mesh) {
    return add(mesh, {});
}

bool AbstractSceneConverter::doAdd(const UnsignedInt id, const MeshData& mesh, const Containers::StringView name) {
    if(features() >= SceneConverterFeature::MeshLevels) {
        return doAdd(id, Containers::arrayView<Containers::AnyReference<const MeshData>>({mesh}), name);
    }

    if(features() & (SceneConverterFeature::ConvertMesh|
                     SceneConverterFeature::ConvertMeshToData|
                     SceneConverterFeature::ConvertMeshToFile)) {
        if(id != 0) {
            Error{} << "Trade::AbstractSceneConverter::add(): the converter requires exactly one mesh";
            return false;
        }

        if(_state->type == State::Type::Convert) {
            CORRADE_INTERNAL_ASSERT(features() & SceneConverterFeature::ConvertMesh);
            if(!(_state->converted.mesh = doConvert(mesh)))
                return false;
        } else if(_state->type == State::Type::ConvertToData) {
            CORRADE_INTERNAL_ASSERT(features() >= SceneConverterFeature::ConvertMeshToData);
            if(!(_state->converted.meshToData = doConvertToData(mesh)))
                return false;
        } else if(_state->type == State::Type::ConvertToFile) {
            CORRADE_INTERNAL_ASSERT(features() & SceneConverterFeature::ConvertMeshToFile);
            if(!(_state->converted.meshToFile = doConvertToFile(mesh, _state->filename)))
                return false;
        } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

        return true;
    }

    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::add(): mesh conversion advertised but not implemented", {});
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::ArrayView<const Containers::AnyReference<const MeshData>> meshLevels, const Containers::StringView name) {
    CORRADE_ASSERT(features() >= (SceneConverterFeature::AddMeshes|SceneConverterFeature::MeshLevels),
        "Trade::AbstractSceneConverter::add(): multi-level mesh conversion not supported", {});
    CORRADE_ASSERT(_state,
        "Trade::AbstractSceneConverter::add(): no conversion in progress", {});
    CORRADE_ASSERT(!meshLevels.isEmpty(),
        "Trade::AbstractSceneConverter::add(): at least one mesh level has to be specified", false);

    if(doAdd(_state->meshCount, meshLevels, name))
        return _state->meshCount++;
    return {};
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const std::initializer_list<Containers::AnyReference<const MeshData>> meshLevels, const Containers::StringView name) {
    return add(Containers::arrayView(meshLevels), name);
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::ArrayView<const Containers::AnyReference<const MeshData>> meshLevels) {
    return add(meshLevels, {});
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const std::initializer_list<Containers::AnyReference<const MeshData>> meshLevels) {
    return add(Containers::arrayView(meshLevels), {});
}

bool AbstractSceneConverter::doAdd(UnsignedInt, Containers::ArrayView<const Containers::AnyReference<const MeshData>>, Containers::StringView) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::add(): multi-level mesh conversion advertised but not implemented", {});
}

void AbstractSceneConverter::setMeshAttributeName(const MeshAttribute attribute, const Containers::StringView name) {
    CORRADE_ASSERT(features() & SceneConverterFeature::AddMeshes,
        "Trade::AbstractSceneConverter::setMeshAttributeName(): feature not supported", );
    CORRADE_ASSERT(_state,
        "Trade::AbstractSceneConverter::setMeshAttributeName(): no conversion in progress", );
    CORRADE_ASSERT(isMeshAttributeCustom(attribute),
        "Trade::AbstractSceneConverter::setMeshAttributeName():" << attribute << "is not custom", );

    doSetMeshAttributeName(meshAttributeCustom(attribute), name);
}

void AbstractSceneConverter::doSetMeshAttributeName(UnsignedShort, Containers::StringView) {}

UnsignedInt AbstractSceneConverter::materialCount() const {
    CORRADE_ASSERT(_state, "Trade::AbstractSceneConverter::materialCount(): no conversion in progress", {});
    return _state->materialCount;
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const MaterialData& material, const Containers::StringView name) {
    CORRADE_ASSERT(features() & SceneConverterFeature::AddMaterials,
        "Trade::AbstractSceneConverter::add(): material conversion not supported", {});
    CORRADE_ASSERT(_state,
        "Trade::AbstractSceneConverter::add(): no conversion in progress", {});

    if(doAdd(_state->materialCount, material, name))
        return _state->materialCount++;
    return {};
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const MaterialData& material) {
    return add(material, {});
}

bool AbstractSceneConverter::doAdd(UnsignedInt, const MaterialData&, Containers::StringView) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::add(): material conversion advertised but not implemented", {});
}

UnsignedInt AbstractSceneConverter::textureCount() const {
    CORRADE_ASSERT(_state, "Trade::AbstractSceneConverter::textureCount(): no conversion in progress", {});
    return _state->textureCount;
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const TextureData& texture, const Containers::StringView name) {
    CORRADE_ASSERT(features() & SceneConverterFeature::AddTextures,
        "Trade::AbstractSceneConverter::add(): texture conversion not supported", {});
    CORRADE_ASSERT(_state,
        "Trade::AbstractSceneConverter::add(): no conversion in progress", {});

    if(doAdd(_state->textureCount, texture, name))
        return _state->textureCount++;
    return {};
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const TextureData& texture) {
    return add(texture, {});
}

bool AbstractSceneConverter::doAdd(UnsignedInt, const TextureData&, Containers::StringView) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::add(): texture conversion advertised but not implemented", {});
}

#ifndef CORRADE_NO_ASSERT
namespace {

template<UnsignedInt dimensions> bool checkImageValidity(const char* const messagePrefix, const ImageData<dimensions>& image) {
    /* At some point there might be a file format that allows zero-sized
       images, but so far I don't know about any. When such format appears,
       this check will get moved to plugin implementations that can't work with
       zero-sized images.

       Also note that this check isn't done for the Image->Image conversion
       above, there zero-sized images and nullptr *could* make sense. */
    CORRADE_ASSERT(image.size().product(),
        messagePrefix << "can't add image with a zero size:" << image.size(), false);
    CORRADE_ASSERT(image.data(),
        messagePrefix << "can't add image with a nullptr view", false);
    return true;
}

template<UnsignedInt dimensions> bool checkImageValidity(const char* const messagePrefix, const Containers::ArrayView<const Containers::AnyReference<const ImageData<dimensions>>> imageLevels) {
    CORRADE_ASSERT(!imageLevels.isEmpty(),
        messagePrefix << "at least one image level has to be specified", false);

    const bool isCompressed = imageLevels[0]->isCompressed();
    const PixelFormat format = isCompressed ? PixelFormat{} : imageLevels[0]->format();
    const UnsignedInt formatExtra = isCompressed ? 0 : imageLevels[0]->formatExtra();
    const CompressedPixelFormat compressedFormat = isCompressed ? imageLevels[0]->compressedFormat() : CompressedPixelFormat{};
    /* Going through *all* levels although the format assertion is never fired
       in the first iteration in order to properly check also the first one for
       zero size / nullptr. */
    for(std::size_t i = 0; i != imageLevels.size(); ++i) {
        CORRADE_ASSERT(imageLevels[i]->size().product(),
            messagePrefix << "can't add image level" << i << "with a zero size:" << imageLevels[i]->size(), false);
        CORRADE_ASSERT(imageLevels[i]->data(),
            messagePrefix << "can't add image level" << i << "with a nullptr view", false);
        CORRADE_ASSERT(imageLevels[i]->isCompressed() == isCompressed,
            messagePrefix << "image level" << i << (isCompressed ? "is not" : "is") << "compressed but previous" << (isCompressed ? "are" : "aren't"), false);
        if(!isCompressed) {
            CORRADE_ASSERT(imageLevels[i]->format() == format,
                messagePrefix << "image levels don't have the same format, expected" << format << "but got" << imageLevels[i]->format() << "for level" << i, false);
            CORRADE_ASSERT(imageLevels[i]->formatExtra() == formatExtra,
                messagePrefix << "image levels don't have the same extra format field, expected" << formatExtra << "but got" << imageLevels[i]->formatExtra() << "for level" << i, false);
        } else {
            CORRADE_ASSERT(imageLevels[i]->compressedFormat() == compressedFormat,
                messagePrefix << "image levels don't have the same format, expected" << compressedFormat << "but got" << imageLevels[i]->compressedFormat() << "for level" << i, false);
        }
    }

    return true;
}

}
#endif

UnsignedInt AbstractSceneConverter::image1DCount() const {
    CORRADE_ASSERT(_state, "Trade::AbstractSceneConverter::image1DCount(): no conversion in progress", {});
    return _state->image1DCount;
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const ImageData1D& image, const Containers::StringView name) {
    CORRADE_ASSERT(features() & (image.isCompressed() ? SceneConverterFeature::AddCompressedImages1D : SceneConverterFeature::AddImages1D),
        "Trade::AbstractSceneConverter::add():" << (image.isCompressed() ? "compressed 1D" : "1D") << "image conversion not supported", {});
    CORRADE_ASSERT(_state,
        "Trade::AbstractSceneConverter::add(): no conversion in progress", {});
    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds */
    if(!checkImageValidity("Trade::AbstractSceneConverter::add():", image))
        return {};
    #endif

    if(doAdd(_state->image1DCount, image, name))
        return _state->image1DCount++;
    return {};
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const ImageData1D& image) {
    return add(image, {});
}

bool AbstractSceneConverter::doAdd(const UnsignedInt id, const ImageData1D& image, const Containers::StringView name) {
    CORRADE_ASSERT(features() >= SceneConverterFeature::ImageLevels, "Trade::AbstractSceneConverter::add(): 1D image conversion advertised but not implemented", {});

    return doAdd(id, Containers::arrayView<Containers::AnyReference<const ImageData1D>>({image}), name);
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const ImageView1D& image, const Containers::StringView name) {
    return add(ImageData1D{image.storage(), image.format(), image.formatExtra(), image.pixelSize(), image.size(), DataFlags{}, image.data()}, name);
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const ImageView1D& image) {
    return add(image, {});
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const CompressedImageView1D& image, const Containers::StringView name) {
    return add(ImageData1D{image.storage(), image.format(), image.size(), DataFlags{}, image.data()}, name);
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const CompressedImageView1D& image) {
    return add(image, {});
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::ArrayView<const Containers::AnyReference<const ImageData1D>> imageLevels, const Containers::StringView name) {
    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds.
       Has to be first so we can safely ask for the first item in asserts
       below. */
    if(!checkImageValidity("Trade::AbstractSceneConverter::add():", imageLevels))
        return {};
    #endif
    CORRADE_ASSERT(features() >= ((imageLevels.front()->isCompressed() ? SceneConverterFeature::AddCompressedImages1D : SceneConverterFeature::AddImages1D)|SceneConverterFeature::ImageLevels),
        "Trade::AbstractSceneConverter::add(): multi-level" << (imageLevels.front()->isCompressed() ? "compressed 1D" : "1D") << "image conversion not supported", {});
    CORRADE_ASSERT(_state,
        "Trade::AbstractSceneConverter::add(): no conversion in progress", {});

    if(doAdd(_state->image1DCount, imageLevels, name))
        return _state->image1DCount++;
    return {};
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const std::initializer_list<Containers::AnyReference<const ImageData1D>> imageLevels, const Containers::StringView name) {
    return add(Containers::arrayView(imageLevels), name);
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::ArrayView<const Containers::AnyReference<const ImageData1D>> imageLevels) {
    return add(imageLevels, {});
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const std::initializer_list<Containers::AnyReference<const ImageData1D>> imageLevels) {
    return add(Containers::arrayView(imageLevels), {});
}

bool AbstractSceneConverter::doAdd(UnsignedInt, Containers::ArrayView<const Containers::AnyReference<const ImageData1D>>, Containers::StringView) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::add(): multi-level 1D image conversion advertised but not implemented", {});
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::ArrayView<const ImageView1D> imageLevels, const Containers::StringView name) {
    /** @todo ArrayTuple or StaticAllocator? */
    Containers::Array<ImageData1D> data{NoInit, imageLevels.size()};
    Containers::Array<Containers::AnyReference<const ImageData1D>> references{NoInit, imageLevels.size()};
    for(std::size_t i = 0; i != imageLevels.size(); ++i) {
        const ImageView1D& image = imageLevels[i];
        new(&data[i]) ImageData1D{image.storage(), image.format(), image.formatExtra(), image.pixelSize(), image.size(), DataFlags{}, image.data()};
        new(&references[i]) Containers::AnyReference<const ImageData1D>{data[i]};
    }

    return add(references, name);
}

// Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const std::initializer_list<ImageView1D> imageLevels, const Containers::StringView name) {
//     return add(Containers::arrayView(imageLevels), name);
// }

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::ArrayView<const ImageView1D> imageLevels) {
    return add(imageLevels, {});
}

// Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const std::initializer_list<ImageView1D> imageLevels) {
//     return add(Containers::arrayView(imageLevels), {});
// }

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::ArrayView<const CompressedImageView1D> imageLevels, const Containers::StringView name) {
    /** @todo ArrayTuple or StaticAllocator? */
    Containers::Array<ImageData1D> data{NoInit, imageLevels.size()};
    Containers::Array<Containers::AnyReference<const ImageData1D>> references{NoInit, imageLevels.size()};
    for(std::size_t i = 0; i != imageLevels.size(); ++i) {
        const CompressedImageView1D& image = imageLevels[i];
        new(&data[i]) ImageData1D{image.storage(), image.format(), image.size(), DataFlags{}, image.data()};
        new(&references[i]) Containers::AnyReference<const ImageData1D>{data[i]};
    }

    return add(references, name);
}

// Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const std::initializer_list<CompressedImageView1D> imageLevels, const Containers::StringView name) {
//     return add(Containers::arrayView(imageLevels), name);
// }

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::ArrayView<const CompressedImageView1D> imageLevels) {
    return add(imageLevels, {});
}
/*
Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const std::initializer_list<CompressedImageView1D> imageLevels) {
    return add(Containers::arrayView(imageLevels), {});
}*/

UnsignedInt AbstractSceneConverter::image2DCount() const {
    CORRADE_ASSERT(_state, "Trade::AbstractSceneConverter::image2DCount(): no conversion in progress", {});
    return _state->image2DCount;
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const ImageData2D& image, const Containers::StringView name) {
    CORRADE_ASSERT(features() & (image.isCompressed() ? SceneConverterFeature::AddCompressedImages2D : SceneConverterFeature::AddImages2D),
        "Trade::AbstractSceneConverter::add():" << (image.isCompressed() ? "compressed 2D" : "2D") << "image conversion not supported", {});
    CORRADE_ASSERT(_state,
        "Trade::AbstractSceneConverter::add(): no conversion in progress", {});
    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds */
    if(!checkImageValidity("Trade::AbstractSceneConverter::add():", image))
        return {};
    #endif

    if(doAdd(_state->image2DCount, image, name))
        return _state->image2DCount++;
    return {};
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const ImageData2D& image) {
    return add(image, {});
}

bool AbstractSceneConverter::doAdd(const UnsignedInt id, const ImageData2D& image, const Containers::StringView name) {
    CORRADE_ASSERT(features() >= SceneConverterFeature::ImageLevels, "Trade::AbstractSceneConverter::add(): 2D image conversion advertised but not implemented", {});

    return doAdd(id, Containers::arrayView<Containers::AnyReference<const ImageData2D>>({image}), name);
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const ImageView2D& image, const Containers::StringView name) {
    return add(ImageData2D{image.storage(), image.format(), image.formatExtra(), image.pixelSize(), image.size(), DataFlags{}, image.data()}, name);
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const ImageView2D& image) {
    return add(image, {});
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const CompressedImageView2D& image, const Containers::StringView name) {
    return add(ImageData2D{image.storage(), image.format(), image.size(), DataFlags{}, image.data()}, name);
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const CompressedImageView2D& image) {
    return add(image, {});
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::ArrayView<const Containers::AnyReference<const ImageData2D>> imageLevels, const Containers::StringView name) {
    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds.
       Has to be first so we can safely ask for the first item in asserts
       below. */
    if(!checkImageValidity("Trade::AbstractSceneConverter::add():", imageLevels))
        return {};
    #endif
    CORRADE_ASSERT(features() >= ((imageLevels.front()->isCompressed() ? SceneConverterFeature::AddCompressedImages2D : SceneConverterFeature::AddImages2D)|SceneConverterFeature::ImageLevels),
        "Trade::AbstractSceneConverter::add(): multi-level" << (imageLevels.front()->isCompressed() ? "compressed 2D" : "2D") << "image conversion not supported", {});
    CORRADE_ASSERT(_state,
        "Trade::AbstractSceneConverter::add(): no conversion in progress", {});

    if(doAdd(_state->image2DCount, imageLevels, name))
        return _state->image2DCount++;
    return {};
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const std::initializer_list<Containers::AnyReference<const ImageData2D>> imageLevels, const Containers::StringView name) {
    return add(Containers::arrayView(imageLevels), name);
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::ArrayView<const Containers::AnyReference<const ImageData2D>> imageLevels) {
    return add(imageLevels, {});
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const std::initializer_list<Containers::AnyReference<const ImageData2D>> imageLevels) {
    return add(Containers::arrayView(imageLevels), {});
}

bool AbstractSceneConverter::doAdd(UnsignedInt, Containers::ArrayView<const Containers::AnyReference<const ImageData2D>>, Containers::StringView) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::add(): multi-level 2D image conversion advertised but not implemented", {});
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::ArrayView<const ImageView2D> imageLevels, const Containers::StringView name) {
    /** @todo ArrayTuple or StaticAllocator? */
    Containers::Array<ImageData2D> data{NoInit, imageLevels.size()};
    Containers::Array<Containers::AnyReference<const ImageData2D>> references{NoInit, imageLevels.size()};
    for(std::size_t i = 0; i != imageLevels.size(); ++i) {
        const ImageView2D& image = imageLevels[i];
        new(&data[i]) ImageData2D{image.storage(), image.format(), image.formatExtra(), image.pixelSize(), image.size(), DataFlags{}, image.data()};
        new(&references[i]) Containers::AnyReference<const ImageData2D>{data[i]};
    }

    return add(references, name);
}

// Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const std::initializer_list<ImageView2D> imageLevels, const Containers::StringView name) {
//     return add(Containers::arrayView(imageLevels), name);
// }

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::ArrayView<const ImageView2D> imageLevels) {
    return add(imageLevels, {});
}

// Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const std::initializer_list<ImageView2D> imageLevels) {
//     return add(Containers::arrayView(imageLevels), {});
// }

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::ArrayView<const CompressedImageView2D> imageLevels, const Containers::StringView name) {
    /** @todo ArrayTuple or StaticAllocator? */
    Containers::Array<ImageData2D> data{NoInit, imageLevels.size()};
    Containers::Array<Containers::AnyReference<const ImageData2D>> references{NoInit, imageLevels.size()};
    for(std::size_t i = 0; i != imageLevels.size(); ++i) {
        const CompressedImageView2D& image = imageLevels[i];
        new(&data[i]) ImageData2D{image.storage(), image.format(), image.size(), DataFlags{}, image.data()};
        new(&references[i]) Containers::AnyReference<const ImageData2D>{data[i]};
    }

    return add(references, name);
}

// Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const std::initializer_list<CompressedImageView2D> imageLevels, const Containers::StringView name) {
//     return add(Containers::arrayView(imageLevels), name);
// }

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::ArrayView<const CompressedImageView2D> imageLevels) {
    return add(imageLevels, {});
}

// Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const std::initializer_list<CompressedImageView2D> imageLevels) {
//     return add(Containers::arrayView(imageLevels), {});
// }

UnsignedInt AbstractSceneConverter::image3DCount() const {
    CORRADE_ASSERT(_state, "Trade::AbstractSceneConverter::image3DCount(): no conversion in progress", {});
    return _state->image3DCount;
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const ImageData3D& image, const Containers::StringView name) {
    CORRADE_ASSERT(features() & (image.isCompressed() ? SceneConverterFeature::AddCompressedImages3D : SceneConverterFeature::AddImages3D),
        "Trade::AbstractSceneConverter::add():" << (image.isCompressed() ? "compressed 3D" : "3D") << "image conversion not supported", {});
    CORRADE_ASSERT(_state,
        "Trade::AbstractSceneConverter::add(): no conversion in progress", {});
    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds */
    if(!checkImageValidity("Trade::AbstractSceneConverter::add():", image))
        return {};
    #endif

    if(doAdd(_state->image3DCount, image, name))
        return _state->image3DCount++;
    return {};
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const ImageData3D& image) {
    return add(image, {});
}

bool AbstractSceneConverter::doAdd(const UnsignedInt id, const ImageData3D& image, const Containers::StringView name) {
    CORRADE_ASSERT(features() >= SceneConverterFeature::ImageLevels, "Trade::AbstractSceneConverter::add(): 3D image conversion advertised but not implemented", {});

    return doAdd(id, Containers::arrayView<Containers::AnyReference<const ImageData3D>>({image}), name);
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const ImageView3D& image, const Containers::StringView name) {
    return add(ImageData3D{image.storage(), image.format(), image.formatExtra(), image.pixelSize(), image.size(), DataFlags{}, image.data()}, name);
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const ImageView3D& image) {
    return add(image, {});
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const CompressedImageView3D& image, const Containers::StringView name) {
    return add(ImageData3D{image.storage(), image.format(), image.size(), DataFlags{}, image.data()}, name);
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const CompressedImageView3D& image) {
    return add(image, {});
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::ArrayView<const Containers::AnyReference<const ImageData3D>> imageLevels, const Containers::StringView name) {
    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds.
       Has to be first so we can safely ask for the first item in asserts
       below. */
    if(!checkImageValidity("Trade::AbstractSceneConverter::add():", imageLevels))
        return {};
    #endif
    CORRADE_ASSERT(features() >= ((imageLevels.front()->isCompressed() ? SceneConverterFeature::AddCompressedImages3D : SceneConverterFeature::AddImages3D)|SceneConverterFeature::ImageLevels),
        "Trade::AbstractSceneConverter::add(): multi-level" << (imageLevels.front()->isCompressed() ? "compressed 3D" : "3D") << "image conversion not supported", {});
    CORRADE_ASSERT(_state,
        "Trade::AbstractSceneConverter::add(): no conversion in progress", {});

    if(doAdd(_state->image3DCount, imageLevels, name))
        return _state->image3DCount++;
    return {};
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const std::initializer_list<Containers::AnyReference<const ImageData3D>> imageLevels, const Containers::StringView name) {
    return add(Containers::arrayView(imageLevels), name);
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::ArrayView<const Containers::AnyReference<const ImageData3D>> imageLevels) {
    return add(imageLevels, {});
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const std::initializer_list<Containers::AnyReference<const ImageData3D>> imageLevels) {
    return add(Containers::arrayView(imageLevels), {});
}

bool AbstractSceneConverter::doAdd(UnsignedInt, Containers::ArrayView<const Containers::AnyReference<const ImageData3D>>, Containers::StringView) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::add(): multi-level 3D image conversion advertised but not implemented", {});
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::ArrayView<const ImageView3D> imageLevels, const Containers::StringView name) {
    /** @todo ArrayTuple or StaticAllocator? */
    Containers::Array<ImageData3D> data{NoInit, imageLevels.size()};
    Containers::Array<Containers::AnyReference<const ImageData3D>> references{NoInit, imageLevels.size()};
    for(std::size_t i = 0; i != imageLevels.size(); ++i) {
        const ImageView3D& image = imageLevels[i];
        new(&data[i]) ImageData3D{image.storage(), image.format(), image.formatExtra(), image.pixelSize(), image.size(), DataFlags{}, image.data()};
        new(&references[i]) Containers::AnyReference<const ImageData3D>{data[i]};
    }

    return add(references, name);
}

// Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const std::initializer_list<ImageView3D> imageLevels, const Containers::StringView name) {
//     return add(Containers::arrayView(imageLevels), name);
// }

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::ArrayView<const ImageView3D> imageLevels) {
    return add(imageLevels, {});
}

// Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const std::initializer_list<ImageView3D> imageLevels) {
//     return add(Containers::arrayView(imageLevels), {});
// }

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::ArrayView<const CompressedImageView3D> imageLevels, const Containers::StringView name) {
    /** @todo ArrayTuple or StaticAllocator? */
    Containers::Array<ImageData3D> data{NoInit, imageLevels.size()};
    Containers::Array<Containers::AnyReference<const ImageData3D>> references{NoInit, imageLevels.size()};
    for(std::size_t i = 0; i != imageLevels.size(); ++i) {
        const CompressedImageView3D& image = imageLevels[i];
        new(&data[i]) ImageData3D{image.storage(), image.format(), image.size(), DataFlags{}, image.data()};
        new(&references[i]) Containers::AnyReference<const ImageData3D>{data[i]};
    }

    return add(references, name);
}

// Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const std::initializer_list<CompressedImageView3D> imageLevels, const Containers::StringView name) {
//     return add(Containers::arrayView(imageLevels), name);
// }

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::ArrayView<const CompressedImageView3D> imageLevels) {
    return add(imageLevels, {});
}

// Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const std::initializer_list<CompressedImageView3D> imageLevels) {
//     return add(Containers::arrayView(imageLevels), {});
// }

Debug& operator<<(Debug& debug, const SceneConverterFeature value) {
    debug << "Trade::SceneConverterFeature" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case SceneConverterFeature::v: return debug << "::" #v;
        _c(ConvertMesh)
        _c(ConvertMeshInPlace)
        _c(ConvertMeshToData)
        _c(ConvertMeshToFile)
        _c(ConvertMultiple)
        _c(ConvertMultipleToData)
        _c(ConvertMultipleToFile)
        _c(AddScenes)
        _c(AddAnimations)
        _c(AddLights)
        _c(AddCameras)
        _c(AddSkins2D)
        _c(AddSkins3D)
        _c(AddMeshes)
        _c(AddMaterials)
        _c(AddTextures)
        _c(AddImages1D)
        _c(AddImages2D)
        _c(AddImages3D)
        _c(AddCompressedImages1D)
        _c(AddCompressedImages2D)
        _c(AddCompressedImages3D)
        _c(MeshLevels)
        _c(ImageLevels)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedInt(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const SceneConverterFeatures value) {
    return Containers::enumSetDebugOutput(debug, value, "Trade::SceneConverterFeatures{}", {
        SceneConverterFeature::ConvertMesh,
        SceneConverterFeature::ConvertMeshInPlace,
        SceneConverterFeature::ConvertMeshToData,
        /* Implied by ConvertMeshToData, has to be after */
        SceneConverterFeature::ConvertMeshToFile,
        SceneConverterFeature::ConvertMultiple,
        SceneConverterFeature::ConvertMultipleToData,
        /* Implied by ConvertMultipleToData, has to be after */
        SceneConverterFeature::ConvertMultipleToFile,
        SceneConverterFeature::AddScenes,
        SceneConverterFeature::AddAnimations,
        SceneConverterFeature::AddLights,
        SceneConverterFeature::AddCameras,
        SceneConverterFeature::AddSkins2D,
        SceneConverterFeature::AddSkins3D,
        SceneConverterFeature::AddMeshes,
        SceneConverterFeature::AddMaterials,
        SceneConverterFeature::AddTextures,
        SceneConverterFeature::AddImages1D,
        SceneConverterFeature::AddImages2D,
        SceneConverterFeature::AddImages3D,
        SceneConverterFeature::AddCompressedImages1D,
        SceneConverterFeature::AddCompressedImages2D,
        SceneConverterFeature::AddCompressedImages3D,
        SceneConverterFeature::MeshLevels,
        SceneConverterFeature::ImageLevels});
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
