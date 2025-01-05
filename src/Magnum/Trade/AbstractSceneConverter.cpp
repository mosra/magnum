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

#include "AbstractSceneConverter.h"

#include <Corrade/Containers/AnyReference.h>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/Iterable.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/ScopeGuard.h>
#include <Corrade/Containers/String.h>
#include <Corrade/PluginManager/Manager.hpp>
#include <Corrade/Utility/Path.h>

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ArrayAllocator.h"
#include "Magnum/Trade/AnimationData.h"
#include "Magnum/Trade/CameraData.h"
#include "Magnum/Trade/ImageData.h"
#include "Magnum/Trade/LightData.h"
#include "Magnum/Trade/MeshData.h"
#include "Magnum/Trade/MaterialData.h"
#include "Magnum/Trade/SceneData.h"
#include "Magnum/Trade/SkinData.h"
#include "Magnum/Trade/TextureData.h"

#ifdef MAGNUM_BUILD_DEPRECATED
/* needed by deprecated convertToFile() that takes a std::string */
#include <Corrade/Containers/StringStl.h>
#endif

#ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
#include "Magnum/Trade/configure.h"
#endif

namespace Corrade { namespace PluginManager {

template class MAGNUM_TRADE_EXPORT Manager<Magnum::Trade::AbstractSceneConverter>;

}}

namespace Magnum { namespace Trade {

using namespace Containers::Literals;

SceneContents sceneContentsFor(const AbstractImporter& importer) {
    CORRADE_ASSERT(importer.isOpened(),
        "Trade::sceneContentsFor(): the importer is not opened", {});

    SceneContents contents = SceneContent::Names;
    if(importer.sceneCount())
        contents |= SceneContent::Scenes;
    if(importer.animationCount())
        contents |= SceneContent::Animations;
    if(importer.lightCount())
        contents |= SceneContent::Lights;
    if(importer.cameraCount())
        contents |= SceneContent::Cameras;
    if(importer.skin2DCount())
        contents |= SceneContent::Skins2D;
    if(importer.skin3DCount())
        contents |= SceneContent::Skins3D;
    if(importer.meshCount())
        contents |= SceneContent::Meshes;
    if(importer.materialCount())
        contents |= SceneContent::Materials;
    if(importer.textureCount())
        contents |= SceneContent::Textures;
    if(importer.image1DCount())
        contents |= SceneContent::Images1D;
    if(importer.image2DCount())
        contents |= SceneContent::Images2D;
    if(importer.image3DCount())
        contents |= SceneContent::Images3D;
    return contents;
}

SceneContents sceneContentsFor(const AbstractSceneConverter& converter) {
    const SceneConverterFeatures features = converter.features();
    SceneContents contents = SceneContent::Names;
    if(features & SceneConverterFeature::AddScenes)
        contents |= SceneContent::Scenes;
    if(features & SceneConverterFeature::AddAnimations)
        contents |= SceneContent::Animations;
    if(features & SceneConverterFeature::AddLights)
        contents |= SceneContent::Lights;
    if(features & SceneConverterFeature::AddCameras)
        contents |= SceneContent::Cameras;
    if(features & SceneConverterFeature::AddSkins2D)
        contents |= SceneContent::Skins2D;
    if(features & SceneConverterFeature::AddSkins3D)
        contents |= SceneContent::Skins3D;
    if(features & (SceneConverterFeature::AddMeshes|
                   SceneConverterFeature::ConvertMesh|
                   SceneConverterFeature::ConvertMeshToFile|
                   SceneConverterFeature::ConvertMeshToData))
        contents |= SceneContent::Meshes;
    if(features & SceneConverterFeature::AddMaterials)
        contents |= SceneContent::Materials;
    if(features & SceneConverterFeature::AddTextures)
        contents |= SceneContent::Textures;
    if(features & (SceneConverterFeature::AddImages1D|
                   SceneConverterFeature::AddCompressedImages1D))
        contents |= SceneContent::Images1D;
    if(features & (SceneConverterFeature::AddImages2D|
                   SceneConverterFeature::AddCompressedImages2D))
        contents |= SceneContent::Images2D;
    if(features & (SceneConverterFeature::AddImages3D|
                   SceneConverterFeature::AddCompressedImages3D))
        contents |= SceneContent::Images3D;
    if(features & SceneConverterFeature::MeshLevels)
        contents |= SceneContent::MeshLevels;
    if(features & SceneConverterFeature::ImageLevels)
        contents |= SceneContent::ImageLevels;

    return contents;
}

/* Gets allocated in begin*() and deallocated in end*() or abort(). The direct
   conversion functions such as convert(const MeshData&) don't directly need
   this state, but can indirectly delegate to it, such as when
   convert(const MeshData&) is emulated with a sequence of begin(),
   add(const MeshData&) and end(). */
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
    return MAGNUM_TRADE_ABSTRACTSCENECONVERTER_PLUGIN_INTERFACE ""_s;
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
    abort();

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
    abort();

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
    abort();

    if(features() >= SceneConverterFeature::ConvertMeshToData) {
        Containers::Optional<Containers::Array<char>> out = doConvertToData(mesh);
        CORRADE_ASSERT(!out || !out->deleter() || out->deleter() == static_cast<void(*)(char*, std::size_t)>(Implementation::nonOwnedArrayDeleter) || out->deleter() == ArrayAllocator<char>::deleter,
            "Trade::AbstractSceneConverter::convertToData(): implementation is not allowed to use a custom Array deleter", {});

        /* GCC 4.8 needs an explicit conversion here */
        #ifdef MAGNUM_BUILD_DEPRECATED
        return Implementation::SceneConverterOptionalButAlsoArray<char>{Utility::move(out)};
        #else
        return out;
        #endif

    } else if(features() >= (SceneConverterFeature::ConvertMultipleToData|SceneConverterFeature::AddMeshes)) {
        beginData();

        if(add(mesh)) return endData();

        /* Finish the conversion even if add() fails -- this API shouldn't
           leave it in an in-progress state */
        abort();
        return {};

    } else CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::convertToData(): mesh conversion not supported", {});
}

Containers::Optional<Containers::Array<char>> AbstractSceneConverter::doConvertToData(const MeshData&) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::convertToData(): mesh conversion advertised but not implemented", {});
}

bool AbstractSceneConverter::convertToFile(const MeshData& mesh, const Containers::StringView filename) {
    abort();

    if(features() >= SceneConverterFeature::ConvertMeshToFile) {
        return doConvertToFile(mesh, filename);

    } else if(features() & (SceneConverterFeature::ConvertMultipleToFile|SceneConverterFeature::AddMeshes)) {
        beginFile(filename);

        if(add(mesh)) return endFile();

        /* Finish the conversion even if add() fails -- this API shouldn't
           leave it in an in-progress state */
        abort();
        return false;

    } else CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::convertToFile(): mesh conversion not supported", {});
}

#ifdef MAGNUM_BUILD_DEPRECATED
bool AbstractSceneConverter::convertToFile(const std::string& filename, const MeshData& mesh) {
    return convertToFile(mesh, filename);
}
#endif

bool AbstractSceneConverter::doConvertToFile(const MeshData& mesh, const Containers::StringView filename) {
    CORRADE_ASSERT(features() >= SceneConverterFeature::ConvertMeshToData, "Trade::AbstractSceneConverter::convertToFile(): mesh conversion advertised but not implemented", false);

    const Containers::Optional<Containers::Array<char>> out = doConvertToData(mesh);
    /* No deleter checks as it doesn't matter here */
    if(!out) return false;

    if(!Utility::Path::write(filename, *out)) {
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

bool AbstractSceneConverter::begin() {
    abort();

    _state.emplace(State::Type::Convert);

    if(features() >= SceneConverterFeature::ConvertMultiple) {
        if(!doBegin()) {
            _state = {};
            return false;
        }

        return true;

    } else if(features() & SceneConverterFeature::ConvertMesh) {
        /* Actual operation performed in doAdd(const MeshData&) */
        return true;

    } else CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::begin(): feature not supported", {});
}

bool AbstractSceneConverter::doBegin() {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::begin(): feature advertised but not implemented", {});
}

Containers::Pointer<AbstractImporter> AbstractSceneConverter::end() {
    CORRADE_ASSERT(_state && _state->type == State::Type::Convert,
        "Trade::AbstractSceneConverter::end(): no conversion in progress", {});

    Containers::ScopeGuard deleteState{this, [](AbstractSceneConverter* self) {
        self->_state = {};
    }};

    if(features() & SceneConverterFeature::ConvertMesh) {
        if(_state->meshCount != 1) {
            Error{} << "Trade::AbstractSceneConverter::end(): the converter requires exactly one mesh, got" << _state->meshCount;
            return {};
        }

        struct SingleMeshImporter: AbstractImporter {
            explicit SingleMeshImporter(Containers::Optional<Trade::MeshData>&& mesh) noexcept: _mesh{Utility::move(mesh)} {}

            ImporterFeatures doFeatures() const override { return {}; }
            bool doIsOpened() const override { return _opened; }
            void doClose() override {
                _opened = false;
                _mesh = {};
            }

            UnsignedInt doMeshCount() const override { return 1; }
            Containers::Optional<MeshData> doMesh(UnsignedInt, UnsignedInt) override {
                /* To avoid complicated logic (such as returning non-owned
                   data and then having to specify the lifetime guarantees),
                   the mesh can be retrieved only once. Second time it's an
                   error. Another option would be to behave like if the
                   importer is closed afterwards, but that would result in
                   assertions which isn't nice. */
                if(!_mesh) {
                    Error{} << "Trade::AbstractSceneConverter::end(): mesh can be retrieved only once from a converter with just Trade::SceneConverterFeature::ConvertMesh";
                    return {};
                }

                Containers::Optional<Trade::MeshData> out = Utility::move(_mesh);
                _mesh = {};
                return out;
            }

            private:
                bool _opened = true;
                Containers::Optional<Trade::MeshData> _mesh;
        };

        return Containers::Pointer<AbstractImporter>(new SingleMeshImporter{Utility::move(_state->converted.mesh)});

    } else if(features() & SceneConverterFeature::ConvertMultiple) {
        return doEnd();

    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

Containers::Pointer<AbstractImporter> AbstractSceneConverter::doEnd() {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::end(): feature advertised but not implemented", {});
}

bool AbstractSceneConverter::beginData() {
    abort();

    _state.emplace(State::Type::ConvertToData);

    if(features() >= SceneConverterFeature::ConvertMultipleToData) {
        if(!doBeginData()) {
            _state = {};
            return false;
        }

        return true;

    } else if(features() >= SceneConverterFeature::ConvertMeshToData) {
        /* Actual operation performed in doAdd(const MeshData&) */
        return true;

    } else CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::beginData(): feature not supported", {});
}

bool AbstractSceneConverter::doBeginData() {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::beginData(): feature advertised but not implemented", {});
}

Containers::Optional<Containers::Array<char>> AbstractSceneConverter::endData() {
    CORRADE_ASSERT(_state && _state->type == State::Type::ConvertToData,
        "Trade::AbstractSceneConverter::endData(): no data conversion in progress", {});

    Containers::ScopeGuard deleteState{this, [](AbstractSceneConverter* self) {
        self->_state = {};
    }};

    if(features() >= SceneConverterFeature::ConvertMultipleToData) {
        Containers::Optional<Containers::Array<char>> out = doEndData();
        CORRADE_ASSERT(!out || !out->deleter() || out->deleter() == static_cast<void(*)(char*, std::size_t)>(Implementation::nonOwnedArrayDeleter) || out->deleter() == ArrayAllocator<char>::deleter,
            "Trade::AbstractSceneConverter::endData(): implementation is not allowed to use a custom Array deleter", {});

        return out;

    } else if(features() >= SceneConverterFeature::ConvertMeshToData) {
        if(_state->meshCount != 1) {
            Error{} << "Trade::AbstractSceneConverter::endData(): the converter requires exactly one mesh, got" << _state->meshCount;
            return {};
        }

        /* No deleter validity checks here, those were performed in
           convertToData(const MeshData&) already */

        return Utility::move(_state->converted.meshToData);

    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

Containers::Optional<Containers::Array<char>> AbstractSceneConverter::doEndData() {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::endData(): feature advertised but not implemented", {});
}

bool AbstractSceneConverter::beginFile(const Containers::StringView filename) {
    abort();

    _state.emplace(State::Type::ConvertToFile);
    _state->filename = Containers::String::nullTerminatedGlobalView(filename);

    if(features() >= SceneConverterFeature::ConvertMultipleToFile) {
        if(!doBeginFile(_state->filename)) {
            _state = {};
            return false;
        }

        return true;

    } else if(features() >= SceneConverterFeature::ConvertMeshToFile) {
        /* Actual operation performed in doAdd(const MeshData&) */
        return true;

    } else CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::beginFile(): feature not supported", {});
}

bool AbstractSceneConverter::doBeginFile(Containers::StringView) {
    CORRADE_ASSERT(features() >= SceneConverterFeature::ConvertMultipleToData,
        "Trade::AbstractSceneConverter::beginFile(): feature advertised but not implemented", {});

    return doBeginData();
}

bool AbstractSceneConverter::endFile() {
    CORRADE_ASSERT(_state && _state->type == State::Type::ConvertToFile,
        "Trade::AbstractSceneConverter::endFile(): no file conversion in progress", {});

    Containers::ScopeGuard deleteState{this, [](AbstractSceneConverter* self) {
        self->_state = {};
    }};

    if(features() >= SceneConverterFeature::ConvertMultipleToFile) {
        return doEndFile(_state->filename);

    } else if(features() & SceneConverterFeature::ConvertMeshToFile) {
        if(_state->meshCount != 1) {
            Error{} << "Trade::AbstractSceneConverter::endFile(): the converter requires exactly one mesh, got" << _state->meshCount;
            return {};
        }

        return _state->converted.meshToFile;

    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

bool AbstractSceneConverter::doEndFile(const Containers::StringView filename) {
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

    doSetSceneFieldName(field, name);
}

void AbstractSceneConverter::doSetSceneFieldName(SceneField, Containers::StringView) {}

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

void AbstractSceneConverter::setAnimationTrackTargetName(const AnimationTrackTarget target, const Containers::StringView name) {
    CORRADE_ASSERT(features() & SceneConverterFeature::AddAnimations,
        "Trade::AbstractSceneConverter::setAnimationTrackTargetName(): feature not supported", );
    CORRADE_ASSERT(_state,
        "Trade::AbstractSceneConverter::setAnimationTrackTargetName(): no conversion in progress", );
    CORRADE_ASSERT(isAnimationTrackTargetCustom(target),
        "Trade::AbstractSceneConverter::setAnimationTrackTargetName():" << target << "is not custom", );

    doSetAnimationTrackTargetName(target, name);
}

void AbstractSceneConverter::doSetAnimationTrackTargetName(AnimationTrackTarget, Containers::StringView) {}

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
    CORRADE_ASSERT(_state,
        "Trade::AbstractSceneConverter::add(): no conversion in progress", {});

    if(features() >= SceneConverterFeature::AddMeshes) {
        if(!doAdd(_state->meshCount, mesh, name)) return {};

    } else if(features() & (SceneConverterFeature::ConvertMesh|
                            SceneConverterFeature::ConvertMeshToData|
                            SceneConverterFeature::ConvertMeshToFile)) {
        if(_state->meshCount != 0) {
            Error{} << "Trade::AbstractSceneConverter::add(): the converter requires exactly one mesh, got" << _state->meshCount + 1;
            return {};
        }

        if(_state->type == State::Type::Convert) {
            CORRADE_INTERNAL_ASSERT(features() & SceneConverterFeature::ConvertMesh);
            if(!(_state->converted.mesh = doConvert(mesh)))
                return {};
        } else if(_state->type == State::Type::ConvertToData) {
            CORRADE_INTERNAL_ASSERT(features() >= SceneConverterFeature::ConvertMeshToData);
            if(!(_state->converted.meshToData = doConvertToData(mesh)))
                return {};
        } else if(_state->type == State::Type::ConvertToFile) {
            CORRADE_INTERNAL_ASSERT(features() & SceneConverterFeature::ConvertMeshToFile);
            if(!(_state->converted.meshToFile = doConvertToFile(mesh, _state->filename)))
                return {};
        } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

    } else CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::add(): mesh conversion not supported", {});

    return _state->meshCount++;
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const MeshData& mesh) {
    return add(mesh, {});
}

bool AbstractSceneConverter::doAdd(const UnsignedInt id, const MeshData& mesh, const Containers::StringView name) {
    CORRADE_ASSERT(features() >= SceneConverterFeature::MeshLevels, "Trade::AbstractSceneConverter::add(): mesh conversion advertised but not implemented", {});

    return doAdd(id, Containers::Iterable<const MeshData>{mesh}, name);
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::Iterable<const MeshData>& meshLevels, const Containers::StringView name) {
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

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::Iterable<const MeshData>& meshLevels) {
    return add(meshLevels, {});
}

bool AbstractSceneConverter::doAdd(UnsignedInt, const Containers::Iterable<const MeshData>&, Containers::StringView) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::add(): multi-level mesh conversion advertised but not implemented", {});
}

void AbstractSceneConverter::setMeshAttributeName(const MeshAttribute attribute, const Containers::StringView name) {
    CORRADE_ASSERT(features() & (SceneConverterFeature::AddMeshes|
                                 SceneConverterFeature::ConvertMesh|
                                 SceneConverterFeature::ConvertMeshInPlace|
                                 SceneConverterFeature::ConvertMeshToData|
                                 SceneConverterFeature::ConvertMeshToFile),
        "Trade::AbstractSceneConverter::setMeshAttributeName(): feature not supported", );
    /* Unless single mesh conversion is supported, allow this function to be
       called only if begin*() was called before */
    CORRADE_ASSERT(features() & (SceneConverterFeature::ConvertMesh|
                                 SceneConverterFeature::ConvertMeshInPlace|
                                 SceneConverterFeature::ConvertMeshToData|
                                 SceneConverterFeature::ConvertMeshToFile) || _state,
        "Trade::AbstractSceneConverter::setMeshAttributeName(): no conversion in progress", );
    CORRADE_ASSERT(isMeshAttributeCustom(attribute),
        "Trade::AbstractSceneConverter::setMeshAttributeName():" << attribute << "is not custom", );

    doSetMeshAttributeName(attribute, name);
}

void AbstractSceneConverter::doSetMeshAttributeName(MeshAttribute, Containers::StringView) {}

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

template<UnsignedInt dimensions> bool checkImageValidity(const char*
    #ifndef CORRADE_STANDARD_ASSERT
    const messagePrefix
    #endif
    , const ImageData<dimensions>& image)
{
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

template<UnsignedInt dimensions> bool checkImageValidity(const char*
    #ifndef CORRADE_STANDARD_ASSERT
    const messagePrefix
    #endif
    , const Containers::Iterable<const ImageData<dimensions>>& imageLevels)
{
    CORRADE_ASSERT(!imageLevels.isEmpty(),
        messagePrefix << "at least one image level has to be specified", false);

    const bool isCompressed = imageLevels[0].isCompressed();
    const PixelFormat format = isCompressed ? PixelFormat{} : imageLevels[0].format();
    const UnsignedInt formatExtra = isCompressed ? 0 : imageLevels[0].formatExtra();
    const CompressedPixelFormat compressedFormat = isCompressed ? imageLevels[0].compressedFormat() : CompressedPixelFormat{};
    const ImageFlags<dimensions> flags = imageLevels[0].flags();
    /* Going through *all* levels although the format assertion is never fired
       in the first iteration in order to properly check also the first one for
       zero size / nullptr. */
    for(std::size_t i = 0; i != imageLevels.size(); ++i) {
        CORRADE_ASSERT(imageLevels[i].size().product(),
            messagePrefix << "can't add image level" << i << "with a zero size:" << imageLevels[i].size(), false);
        CORRADE_ASSERT(imageLevels[i].data(),
            messagePrefix << "can't add image level" << i << "with a nullptr view", false);
        CORRADE_ASSERT(imageLevels[i].isCompressed() == isCompressed,
            messagePrefix << "image level" << i << (isCompressed ? "is not" : "is") << "compressed but previous" << (isCompressed ? "are" : "aren't"), false);
        if(!isCompressed) {
            CORRADE_ASSERT(imageLevels[i].format() == format,
                messagePrefix << "image levels don't have the same format, expected" << format << "but got" << imageLevels[i].format() << "for level" << i, false);
            CORRADE_ASSERT(imageLevels[i].formatExtra() == formatExtra,
                messagePrefix << "image levels don't have the same extra format field, expected" << formatExtra << "but got" << imageLevels[i].formatExtra() << "for level" << i, false);
        } else {
            CORRADE_ASSERT(imageLevels[i].compressedFormat() == compressedFormat,
                messagePrefix << "image levels don't have the same format, expected" << compressedFormat << "but got" << imageLevels[i].compressedFormat() << "for level" << i, false);
        }
        CORRADE_ASSERT(imageLevels[i].flags() == flags,
            messagePrefix << "image levels don't have the same flags, expected" << flags << "but got" << imageLevels[i].flags() << "for level" << i, false);
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

    return doAdd(id, Containers::Iterable<const ImageData1D>{image}, name);
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const ImageView1D& image, const Containers::StringView name) {
    return add(ImageData1D{image.storage(), image.format(), image.formatExtra(), image.pixelSize(), image.size(), DataFlags{}, image.data(), image.flags()}, name);
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const ImageView1D& image) {
    return add(image, {});
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const CompressedImageView1D& image, const Containers::StringView name) {
    return add(ImageData1D{image.storage(), image.format(), image.size(), DataFlags{}, image.data(), image.flags()}, name);
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const CompressedImageView1D& image) {
    return add(image, {});
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::Iterable<const ImageData1D>& imageLevels, const Containers::StringView name) {
    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds.
       Has to be first so we can safely ask for the first item in asserts
       below. */
    if(!checkImageValidity("Trade::AbstractSceneConverter::add():", imageLevels))
        return {};
    #endif
    CORRADE_ASSERT(features() >= ((imageLevels.front().isCompressed() ? SceneConverterFeature::AddCompressedImages1D : SceneConverterFeature::AddImages1D)|SceneConverterFeature::ImageLevels),
        "Trade::AbstractSceneConverter::add(): multi-level" << (imageLevels.front().isCompressed() ? "compressed 1D" : "1D") << "image conversion not supported", {});
    CORRADE_ASSERT(_state,
        "Trade::AbstractSceneConverter::add(): no conversion in progress", {});

    if(doAdd(_state->image1DCount, imageLevels, name))
        return _state->image1DCount++;
    return {};
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::Iterable<const ImageData1D>& imageLevels) {
    return add(imageLevels, {});
}

bool AbstractSceneConverter::doAdd(UnsignedInt, const Containers::Iterable<const ImageData1D>&, Containers::StringView) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::add(): multi-level 1D image conversion advertised but not implemented", {});
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::Iterable<const ImageView1D>& imageLevels, const Containers::StringView name) {
    Containers::Array<ImageData1D> data{NoInit, imageLevels.size()};
    for(std::size_t i = 0; i != imageLevels.size(); ++i) {
        const ImageView1D& image = imageLevels[i];
        new(&data[i]) ImageData1D{image.storage(), image.format(), image.formatExtra(), image.pixelSize(), image.size(), DataFlags{}, image.data(), image.flags()};
    }

    return add(data, name);
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::Iterable<const ImageView1D>& imageLevels) {
    return add(imageLevels, {});
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::Iterable<const CompressedImageView1D>& imageLevels, const Containers::StringView name) {
    Containers::Array<ImageData1D> data{NoInit, imageLevels.size()};
    for(std::size_t i = 0; i != imageLevels.size(); ++i) {
        const CompressedImageView1D& image = imageLevels[i];
        new(&data[i]) ImageData1D{image.storage(), image.format(), image.size(), DataFlags{}, image.data(), image.flags()};
    }

    return add(data, name);
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::Iterable<const CompressedImageView1D>& imageLevels) {
    return add(imageLevels, {});
}

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

    return doAdd(id, Containers::Iterable<const ImageData2D>{image}, name);
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const ImageView2D& image, const Containers::StringView name) {
    return add(ImageData2D{image.storage(), image.format(), image.formatExtra(), image.pixelSize(), image.size(), DataFlags{}, image.data(), image.flags()}, name);
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const ImageView2D& image) {
    return add(image, {});
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const CompressedImageView2D& image, const Containers::StringView name) {
    return add(ImageData2D{image.storage(), image.format(), image.size(), DataFlags{}, image.data(), image.flags()}, name);
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const CompressedImageView2D& image) {
    return add(image, {});
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::Iterable<const ImageData2D>& imageLevels, const Containers::StringView name) {
    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds.
       Has to be first so we can safely ask for the first item in asserts
       below. */
    if(!checkImageValidity("Trade::AbstractSceneConverter::add():", imageLevels))
        return {};
    #endif
    CORRADE_ASSERT(features() >= ((imageLevels.front().isCompressed() ? SceneConverterFeature::AddCompressedImages2D : SceneConverterFeature::AddImages2D)|SceneConverterFeature::ImageLevels),
        "Trade::AbstractSceneConverter::add(): multi-level" << (imageLevels.front().isCompressed() ? "compressed 2D" : "2D") << "image conversion not supported", {});
    CORRADE_ASSERT(_state,
        "Trade::AbstractSceneConverter::add(): no conversion in progress", {});

    if(doAdd(_state->image2DCount, imageLevels, name))
        return _state->image2DCount++;
    return {};
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::Iterable<const ImageData2D>& imageLevels) {
    return add(imageLevels, {});
}

bool AbstractSceneConverter::doAdd(UnsignedInt, const Containers::Iterable<const ImageData2D>&, Containers::StringView) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::add(): multi-level 2D image conversion advertised but not implemented", {});
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::Iterable<const ImageView2D>& imageLevels, const Containers::StringView name) {
    Containers::Array<ImageData2D> data{NoInit, imageLevels.size()};
    for(std::size_t i = 0; i != imageLevels.size(); ++i) {
        const ImageView2D& image = imageLevels[i];
        new(&data[i]) ImageData2D{image.storage(), image.format(), image.formatExtra(), image.pixelSize(), image.size(), DataFlags{}, image.data(), image.flags()};
    }

    return add(data, name);
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::Iterable<const ImageView2D>& imageLevels) {
    return add(imageLevels, {});
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::Iterable<const CompressedImageView2D>& imageLevels, const Containers::StringView name) {
    Containers::Array<ImageData2D> data{NoInit, imageLevels.size()};
    for(std::size_t i = 0; i != imageLevels.size(); ++i) {
        const CompressedImageView2D& image = imageLevels[i];
        new(&data[i]) ImageData2D{image.storage(), image.format(), image.size(), DataFlags{}, image.data(), image.flags()};
    }

    return add(data, name);
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::Iterable<const CompressedImageView2D>& imageLevels) {
    return add(imageLevels, {});
}

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

    return doAdd(id, Containers::Iterable<const ImageData3D>{image}, name);
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const ImageView3D& image, const Containers::StringView name) {
    return add(ImageData3D{image.storage(), image.format(), image.formatExtra(), image.pixelSize(), image.size(), DataFlags{}, image.data(), image.flags()}, name);
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const ImageView3D& image) {
    return add(image, {});
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const CompressedImageView3D& image, const Containers::StringView name) {
    return add(ImageData3D{image.storage(), image.format(), image.size(), DataFlags{}, image.data(), image.flags()}, name);
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const CompressedImageView3D& image) {
    return add(image, {});
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::Iterable<const ImageData3D>& imageLevels, const Containers::StringView name) {
    #ifndef CORRADE_NO_ASSERT
    /* Explicitly return if checks fail for CORRADE_GRACEFUL_ASSERT builds.
       Has to be first so we can safely ask for the first item in asserts
       below. */
    if(!checkImageValidity("Trade::AbstractSceneConverter::add():", imageLevels))
        return {};
    #endif
    CORRADE_ASSERT(features() >= ((imageLevels.front().isCompressed() ? SceneConverterFeature::AddCompressedImages3D : SceneConverterFeature::AddImages3D)|SceneConverterFeature::ImageLevels),
        "Trade::AbstractSceneConverter::add(): multi-level" << (imageLevels.front().isCompressed() ? "compressed 3D" : "3D") << "image conversion not supported", {});
    CORRADE_ASSERT(_state,
        "Trade::AbstractSceneConverter::add(): no conversion in progress", {});

    if(doAdd(_state->image3DCount, imageLevels, name))
        return _state->image3DCount++;
    return {};
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::Iterable<const ImageData3D>& imageLevels) {
    return add(imageLevels, {});
}

bool AbstractSceneConverter::doAdd(UnsignedInt, const Containers::Iterable<const ImageData3D>&, Containers::StringView) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractSceneConverter::add(): multi-level 3D image conversion advertised but not implemented", {});
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::Iterable<const ImageView3D>& imageLevels, const Containers::StringView name) {
    Containers::Array<ImageData3D> data{NoInit, imageLevels.size()};
    for(std::size_t i = 0; i != imageLevels.size(); ++i) {
        const ImageView3D& image = imageLevels[i];
        new(&data[i]) ImageData3D{image.storage(), image.format(), image.formatExtra(), image.pixelSize(), image.size(), DataFlags{}, image.data(), image.flags()};
    }

    return add(data, name);
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::Iterable<const ImageView3D>& imageLevels) {
    return add(imageLevels, {});
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::Iterable<const CompressedImageView3D>& imageLevels, const Containers::StringView name) {
    Containers::Array<ImageData3D> data{NoInit, imageLevels.size()};
    for(std::size_t i = 0; i != imageLevels.size(); ++i) {
        const CompressedImageView3D& image = imageLevels[i];
        new(&data[i]) ImageData3D{image.storage(), image.format(), image.size(), DataFlags{}, image.data(), image.flags()};
    }

    return add(data, name);
}

Containers::Optional<UnsignedInt> AbstractSceneConverter::add(const Containers::Iterable<const CompressedImageView3D>& imageLevels) {
    return add(imageLevels, {});
}

bool AbstractSceneConverter::addImporterContentsInternal(AbstractImporter& importer, const SceneContents contents, const bool noLevelsIfUnsupported) {
    CORRADE_ASSERT(isConverting(),
        "Trade::AbstractSceneConverter::addImporterContents(): no conversion in progress", {});
    const SceneContents contentsSupported = sceneContentsFor(*this);
    #ifndef CORRADE_NO_ASSERT
    const SceneContents contentsPresentExceptLevels = contents & sceneContentsFor(importer);
    #endif
    CORRADE_ASSERT(!(contentsPresentExceptLevels & ~contentsSupported),
        "Trade::AbstractSceneConverter::addImporterContents(): unsupported contents" << Debug::packed << (contentsPresentExceptLevels & ~contentsSupported), {});

    /* These are in dependency order -- i.e., images between textures that
       reference them or scenes before animations that reference them. The
       actual bound checks (if any) are left on concrete implementations. */

    if(contents & SceneContent::Meshes) {
        Containers::Array<Trade::MeshData> levels;
        for(UnsignedInt i = 0, iMax = importer.meshCount(); i != iMax; ++i) {
            if(flags() & SceneConverterFlag::Verbose)
                Debug{} << "Trade::AbstractSceneConverter::addImporterContents(): adding mesh" << i << "out of" << iMax;

            UnsignedInt levelCount = contents & SceneContent::MeshLevels ? importer.meshLevelCount(i) : 1;
            if(levelCount != 1 && !(contentsSupported & SceneContent::MeshLevels)) {
                if(noLevelsIfUnsupported) {
                    Warning{} << "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring extra" << levelCount - 1 << "levels of mesh" << i << "not supported by the converter";
                    levelCount = 1;
                /* Not an assert because having to check this in advance could
                   be prohibitively expensive */
                } else {
                    Error{} << "Trade::AbstractSceneConverter::addImporterContents(): mesh" << i << "contains" << levelCount << "levels but the converter doesn't support" << SceneConverterFeature::MeshLevels;
                    return false;
                }
            }

            arrayReserve(levels, levelCount);
            arrayResize(levels, NoInit, 0); /** @todo arrayClear() */
            for(UnsignedInt j = 0; j != levelCount; ++j) {
                if((flags() & SceneConverterFlag::Verbose) && levelCount != 1)
                    Debug{} << "Trade::AbstractSceneConverter::addImporterContents(): importing mesh" << i << "level" << j << "out of" << levelCount;

                Containers::Optional<Trade::MeshData> mesh = importer.mesh(i, j);
                if(!mesh) return false;

                /* Propagate custom attribute names, skip ones that are empty.
                   Compared to data names this is done always to avoid
                   information loss. */
                for(UnsignedInt k = 0; k != mesh->attributeCount(); ++k) {
                    /** @todo have some kind of a map to not have to query the
                        same custom attribute again for each mesh */
                    const Trade::MeshAttribute name = mesh->attributeName(k);
                    if(!isMeshAttributeCustom(name)) continue;
                    if(const Containers::String nameString = importer.meshAttributeName(name)) {
                        setMeshAttributeName(name, nameString);
                    }
                }

                arrayAppend(levels, *Utility::move(mesh));
            }

            const Containers::String name = contents & SceneContent::Names ? importer.meshName(i) : Containers::String{};
            if(levelCount != 1) {
                if(!add(levels, name))
                    return false;
            } else {
                if(!add(levels[0], name))
                    return false;
            }
        }
    }

    if(contents & SceneContent::Images1D) {
        Containers::Array<Trade::ImageData1D> levels;
        for(UnsignedInt i = 0, iMax = importer.image1DCount(); i != iMax; ++i) {
            if(flags() & SceneConverterFlag::Verbose)
                Debug{} << "Trade::AbstractSceneConverter::addImporterContents(): adding 1D image" << i << "out of" << iMax;

            UnsignedInt levelCount = contents & SceneContent::ImageLevels ? importer.image1DLevelCount(i) : 1;
            if(levelCount != 1 && !(contentsSupported & SceneContent::ImageLevels)) {
                if(noLevelsIfUnsupported) {
                    Warning{} << "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring extra" << levelCount - 1 << "levels of 1D image" << i << "not supported by the converter";
                    levelCount = 1;
                /* Not an assert because having to check this in advance could
                   be prohibitively expensive (decoding an arbitrary amount of
                   images twice...) */
                } else {
                    Error{} << "Trade::AbstractSceneConverter::addImporterContents(): 1D image" << i << "contains" << levelCount << "levels but the converter doesn't support" << SceneConverterFeature::ImageLevels;
                    return false;
                }
            }

            arrayReserve(levels, levelCount);
            arrayResize(levels, NoInit, 0); /** @todo arrayClear() */
            for(UnsignedInt j = 0; j != levelCount; ++j) {
                if((flags() & SceneConverterFlag::Verbose) && levelCount != 1)
                    Debug{} << "Trade::AbstractSceneConverter::addImporterContents(): importing 1D image" << i << "level" << j << "out of" << levelCount;

                Containers::Optional<Trade::ImageData1D> image = importer.image1D(i, j);
                if(!image) return false;

                if(image->isCompressed() && !(features() & SceneConverterFeature::AddCompressedImages1D)) {
                    Error{} << "Trade::AbstractSceneConverter::addImporterContents(): 1D image" << i << "level" << j << "is compressed but the converter doesn't support" << SceneConverterFeature::AddCompressedImages1D;
                    return false;
                }

                if(!image->isCompressed() && !(features() & SceneConverterFeature::AddImages1D)) {
                    Error{} << "Trade::AbstractSceneConverter::addImporterContents(): 1D image" << i << "level" << j << "is uncompressed but the converter doesn't support" << SceneConverterFeature::AddImages1D;
                    return false;
                }

                arrayAppend(levels, *Utility::move(image));
            }

            const Containers::String name = contents & SceneContent::Names ? importer.image1DName(i) : Containers::String{};
            if(levelCount != 1) {
                if(!add(levels, name))
                    return false;
            } else {
                if(!add(levels[0], name))
                    return false;
            }
        }
    }

    if(contents & SceneContent::Images2D) {
        Containers::Array<Trade::ImageData2D> levels;
        for(UnsignedInt i = 0, iMax = importer.image2DCount(); i != iMax; ++i) {
            if(flags() & SceneConverterFlag::Verbose)
                Debug{} << "Trade::AbstractSceneConverter::addImporterContents(): adding 2D image" << i << "out of" << iMax;

            UnsignedInt levelCount = contents & SceneContent::ImageLevels ? importer.image2DLevelCount(i) : 1;
            if(levelCount != 1 && !(contentsSupported & SceneContent::ImageLevels)) {
                if(noLevelsIfUnsupported) {
                    Warning{} << "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring extra" << levelCount - 1 << "levels of 2D image" << i << "not supported by the converter";
                    levelCount = 1;
                /* Not an assert because having to check this in advance could
                   be prohibitively expensive (decoding an arbitrary amount of
                   images twice...) */
                } else {
                    Error{} << "Trade::AbstractSceneConverter::addImporterContents(): 2D image" << i << "contains" << levelCount << "levels but the converter doesn't support" << SceneConverterFeature::ImageLevels;
                    return false;
                }
            }

            arrayReserve(levels, levelCount);
            arrayResize(levels, NoInit, 0); /** @todo arrayClear() */
            for(UnsignedInt j = 0; j != levelCount; ++j) {
                if((flags() & SceneConverterFlag::Verbose) && levelCount != 1)
                    Debug{} << "Trade::AbstractSceneConverter::addImporterContents(): importing 2D image" << i << "level" << j << "out of" << levelCount;

                Containers::Optional<Trade::ImageData2D> image = importer.image2D(i, j);
                if(!image) return false;

                if(image->isCompressed() && !(features() & SceneConverterFeature::AddCompressedImages2D)) {
                    Error{} << "Trade::AbstractSceneConverter::addImporterContents(): 2D image" << i << "level" << j << "is compressed but the converter doesn't support" << SceneConverterFeature::AddCompressedImages2D;
                    return false;
                }

                if(!image->isCompressed() && !(features() & SceneConverterFeature::AddImages2D)) {
                    Error{} << "Trade::AbstractSceneConverter::addImporterContents(): 2D image" << i << "level" << j << "is uncompressed but the converter doesn't support" << SceneConverterFeature::AddImages2D;
                    return false;
                }

                arrayAppend(levels, *Utility::move(image));
            }

            const Containers::String name = contents & SceneContent::Names ? importer.image2DName(i) : Containers::String{};
            if(levelCount != 1) {
                if(!add(levels, name))
                    return false;
            } else {
                if(!add(levels[0], name))
                    return false;
            }
        }
    }

    if(contents & SceneContent::Images3D) {
        Containers::Array<Trade::ImageData3D> levels;
        for(UnsignedInt i = 0, iMax = importer.image3DCount(); i != iMax; ++i) {
            if(flags() & SceneConverterFlag::Verbose)
                Debug{} << "Trade::AbstractSceneConverter::addImporterContents(): adding 3D image" << i << "out of" << iMax;

            UnsignedInt levelCount = contents & SceneContent::ImageLevels ? importer.image3DLevelCount(i) : 1;
            if(levelCount != 1 && !(contentsSupported & SceneContent::ImageLevels)) {
                if(noLevelsIfUnsupported) {
                    Warning{} << "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring extra" << levelCount - 1 << "levels of 3D image" << i << "not supported by the converter";
                    levelCount = 1;
                /* Not an assert because having to check this in advance could
                   be prohibitively expensive (decoding an arbitrary amount of
                   images twice...) */
                } else {
                    Error{} << "Trade::AbstractSceneConverter::addImporterContents(): 3D image" << i << "contains" << levelCount << "levels but the converter doesn't support" << SceneConverterFeature::ImageLevels;
                    return false;
                }
            }

            arrayReserve(levels, levelCount);
            arrayResize(levels, NoInit, 0); /** @todo arrayClear() */
            for(UnsignedInt j = 0; j != levelCount; ++j) {
                if((flags() & SceneConverterFlag::Verbose) && levelCount != 1)
                    Debug{} << "Trade::AbstractSceneConverter::addImporterContents(): importing 3D image" << i << "level" << j << "out of" << levelCount;

                Containers::Optional<Trade::ImageData3D> image = importer.image3D(i, j);
                if(!image) return false;

                if(image->isCompressed() && !(features() & SceneConverterFeature::AddCompressedImages3D)) {
                    Error{} << "Trade::AbstractSceneConverter::addImporterContents(): 3D image" << i << "level" << j << "is compressed but the converter doesn't support" << SceneConverterFeature::AddCompressedImages3D;
                    return false;
                }

                if(!image->isCompressed() && !(features() & SceneConverterFeature::AddImages3D)) {
                    Error{} << "Trade::AbstractSceneConverter::addImporterContents(): 3D image" << i << "level" << j << "is uncompressed but the converter doesn't support" << SceneConverterFeature::AddImages3D;
                    return false;
                }

                arrayAppend(levels, *Utility::move(image));
            }

            const Containers::String name = contents & SceneContent::Names ? importer.image3DName(i) : Containers::String{};
            if(levelCount != 1) {
                if(!add(levels, name))
                    return false;
            } else {
                if(!add(levels[0], name))
                    return false;
            }
        }
    }

    if(contents & SceneContent::Textures) {
        for(UnsignedInt i = 0, iMax = importer.textureCount(); i != iMax; ++i) {
            if(flags() & SceneConverterFlag::Verbose)
                Debug{} << "Trade::AbstractSceneConverter::addImporterContents(): adding texture" << i << "out of" << iMax;

            const Containers::Optional<Trade::TextureData> texture = importer.texture(i);
            if(!texture || !add(*texture, contents & SceneContent::Names ? importer.textureName(i) : Containers::String{}))
                return false;
        }
    }

    if(contents & SceneContent::Materials) {
        for(UnsignedInt i = 0, iMax = importer.materialCount(); i != iMax; ++i) {
            if(flags() & SceneConverterFlag::Verbose)
                Debug{} << "Trade::AbstractSceneConverter::addImporterContents(): adding material" << i << "out of" << iMax;

            const Containers::Optional<Trade::MaterialData> material = importer.material(i);
            if(!material || !add(*material, contents & SceneContent::Names ? importer.materialName(i) : Containers::String{}))
                return false;
        }
    }

    if(contents & SceneContent::Lights) {
        for(UnsignedInt i = 0, iMax = importer.lightCount(); i != iMax; ++i) {
            if(flags() & SceneConverterFlag::Verbose)
                Debug{} << "Trade::AbstractSceneConverter::addImporterContents(): adding light" << i << "out of" << iMax;

            const Containers::Optional<Trade::LightData> light = importer.light(i);
            if(!light || !add(*light, contents & SceneContent::Names ? importer.lightName(i) : Containers::String{}))
                return false;
        }
    }

    if(contents & SceneContent::Cameras) {
        for(UnsignedInt i = 0, iMax = importer.cameraCount(); i != iMax; ++i) {
            if(flags() & SceneConverterFlag::Verbose)
                Debug{} << "Trade::AbstractSceneConverter::addImporterContents(): adding camera" << i << "out of" << iMax;

            const Containers::Optional<Trade::CameraData> camera = importer.camera(i);
            if(!camera || !add(*camera, contents & SceneContent::Names ? importer.cameraName(i) : Containers::String{}))
                return false;
        }
    }

    if(contents & SceneContent::Scenes) {
        /* Propagate object names, skip ones that are empty */
        if(contents & SceneContent::Names) for(UnsignedLong i = 0, iMax = importer.objectCount(); i != iMax; ++i) {
            if(const Containers::String name = importer.objectName(i))
                setObjectName(i, name);
        }

        for(UnsignedInt i = 0, iMax = importer.sceneCount(); i != iMax; ++i) {
            if(flags() & SceneConverterFlag::Verbose)
                Debug{} << "Trade::AbstractSceneConverter::addImporterContents(): adding scene" << i << "out of" << iMax;

            Containers::Optional<Trade::SceneData> scene = importer.scene(i);
            if(!scene) return false;

            /* Propagate custom field names, skip ones that are empty. Compared
               to data names this is done always to avoid information loss. */
            for(UnsignedInt j = 0; j != scene->fieldCount(); ++j) {
                /** @todo have some kind of a map to not have to query the same
                    field again for each scene */
                const Trade::SceneField name = scene->fieldName(j);
                if(!isSceneFieldCustom(name)) continue;
                if(const Containers::String nameString = importer.sceneFieldName(name)) {
                    setSceneFieldName(name, nameString);
                }
            }

            if(!scene || !add(*scene, contents & SceneContent::Names ? importer.sceneName(i) : Containers::String{}))
                return false;
        }

        const Int defaultScene = importer.defaultScene();
        if(defaultScene != -1)
            setDefaultScene(defaultScene);
    }

    if(contents & SceneContent::Skins2D) {
        for(UnsignedInt i = 0, iMax = importer.skin2DCount(); i != iMax; ++i) {
            if(flags() & SceneConverterFlag::Verbose)
                Debug{} << "Trade::AbstractSceneConverter::addImporterContents(): adding 2D skin" << i << "out of" << iMax;

            const Containers::Optional<Trade::SkinData2D> skin = importer.skin2D(i);
            if(!skin || !add(*skin, contents & SceneContent::Names ? importer.skin2DName(i) : Containers::String{}))
                return false;
        }
    }

    if(contents & SceneContent::Skins3D) {
        for(UnsignedInt i = 0, iMax = importer.skin3DCount(); i != iMax; ++i) {
            if(flags() & SceneConverterFlag::Verbose)
                Debug{} << "Trade::AbstractSceneConverter::addImporterContents(): adding 3D skin" << i << "out of" << iMax;

            const Containers::Optional<Trade::SkinData3D> skin = importer.skin3D(i);
            if(!skin || !add(*skin, contents & SceneContent::Names ? importer.skin3DName(i) : Containers::String{}))
                return false;
        }
    }

    if(contents & SceneContent::Animations) {
        for(UnsignedInt i = 0, iMax = importer.animationCount(); i != iMax; ++i) {
            if(flags() & SceneConverterFlag::Verbose)
                Debug{} << "Trade::AbstractSceneConverter::addImporterContents(): adding animation" << i << "out of" << iMax;

            const Containers::Optional<Trade::AnimationData> animation = importer.animation(i);
            if(!animation || !add(*animation, contents & SceneContent::Names ? importer.animationName(i) : Containers::String{}))
                return false;
        }
    }

    return true;
}


bool AbstractSceneConverter::addImporterContents(AbstractImporter& importer, const SceneContents contents) {
    CORRADE_ASSERT(importer.isOpened(),
        "Trade::AbstractSceneConverter::addImporterContents(): the importer is not opened", {});
    return addImporterContentsInternal(importer, contents, false);
}

bool AbstractSceneConverter::addSupportedImporterContents(AbstractImporter& importer, const SceneContents contents) {
    CORRADE_ASSERT(importer.isOpened(),
        "Trade::AbstractSceneConverter::addSupportedImporterContents(): the importer is not opened", {});

    /* To avoid accidental differences in handling SceneConverterFeatures in
       sceneContentsFor(const AbstractSceneConverter&) and here, this branches
       on SceneContents instead of SceneConverterFeatures */
    const SceneContents wantedButUnsupported = contents & ~sceneContentsFor(*this);

    /* To avoid needlessly querying fooCount() several times (which might be
       expensive in certain unfortunate cases), this basically unwraps the
       contents of sceneContentsFor(const AbstractImporter&) and adds warnings
       there.

       The data kinds are in dependency order, matching the order in
       addImporterContentsInternal(), so when warnings are printed they're not
       in an order different from the verbose output for no reason. */
    SceneContents used = contents;
    UnsignedInt count;
    if((wantedButUnsupported & SceneContent::Meshes) && (count = importer.meshCount())) {
        Warning{} << "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring" << count << "meshes not supported by the converter";
        used &= ~SceneContent::Meshes;
    }
    if((wantedButUnsupported & SceneContent::Images1D) && (count = importer.image1DCount())) {
        Warning{} << "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring" << count << "1D images not supported by the converter";
        used &= ~SceneContent::Images1D;
    }
    if((wantedButUnsupported & SceneContent::Images2D) && (count = importer.image2DCount())) {
        Warning{} << "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring" << count << "2D images not supported by the converter";
        used &= ~SceneContent::Images2D;
    }
    if((wantedButUnsupported & SceneContent::Images3D) && (count = importer.image3DCount())) {
        Warning{} << "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring" << count << "3D images not supported by the converter";
        used &= ~SceneContent::Images3D;
    }
    if((wantedButUnsupported & SceneContent::Textures) && (count = importer.textureCount())) {
        Warning{} << "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring" << count << "textures not supported by the converter";
        used &= ~SceneContent::Textures;
    }
    if((wantedButUnsupported & SceneContent::Materials) && (count = importer.materialCount())) {
        Warning{} << "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring" << count << "materials not supported by the converter";
        used &= ~SceneContent::Materials;
    }
    if((wantedButUnsupported & SceneContent::Lights) && (count = importer.lightCount())) {
        Warning{} << "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring" << count << "lights not supported by the converter";
        used &= ~SceneContent::Lights;
    }
    if((wantedButUnsupported & SceneContent::Cameras) && (count = importer.cameraCount())) {
        Warning{} << "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring" << count << "cameras not supported by the converter";
        used &= ~SceneContent::Cameras;
    }
    if((wantedButUnsupported & SceneContent::Scenes) && (count = importer.sceneCount())) {
        Warning{} << "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring" << count << "scenes not supported by the converter";
        used &= ~SceneContent::Scenes;
    }
    if((wantedButUnsupported & SceneContent::Skins2D) && (count = importer.skin2DCount())) {
        Warning{} << "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring" << count << "2D skins not supported by the converter";
        used &= ~SceneContent::Skins2D;
    }
    if((wantedButUnsupported & SceneContent::Skins3D) && (count = importer.skin3DCount())) {
        Warning{} << "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring" << count << "3D skins not supported by the converter";
        used &= ~SceneContent::Skins3D;
    }
    if((wantedButUnsupported & SceneContent::Animations) && (count = importer.animationCount())) {
        Warning{} << "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring" << count << "animations not supported by the converter";
        used &= ~SceneContent::Animations;
    }

    /* MeshLevels and ImageLevels handled inside */
    return addImporterContentsInternal(importer, used, true);
}

Debug& operator<<(Debug& debug, const SceneConverterFeature value) {
    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;

    if(!packed)
        debug << "Trade::SceneConverterFeature" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case SceneConverterFeature::v: return debug << (packed ? "" : "::") << Debug::nospace << #v;
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

    return debug << (packed ? "" : "(") << Debug::nospace << Debug::hex << UnsignedInt(value) << Debug::nospace << (packed ? "" : ")");
}

Debug& operator<<(Debug& debug, const SceneConverterFeatures value) {
    return Containers::enumSetDebugOutput(debug, value, debug.immediateFlags() >= Debug::Flag::Packed ? "{}" : "Trade::SceneConverterFeatures{}", {
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
        _c(Quiet)
        _c(Verbose)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << Debug::hex << UnsignedByte(value) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const SceneConverterFlags value) {
    return Containers::enumSetDebugOutput(debug, value, "Trade::SceneConverterFlags{}", {
        SceneConverterFlag::Quiet,
        SceneConverterFlag::Verbose});
}

Debug& operator<<(Debug& debug, const SceneContent value) {
    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;

    if(!packed)
        debug << "Trade::SceneContent" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case SceneContent::v: return debug << (packed ? "" : "::") << Debug::nospace << #v;
        _c(Scenes)
        _c(Animations)
        _c(Lights)
        _c(Cameras)
        _c(Skins2D)
        _c(Skins3D)
        _c(Meshes)
        _c(Materials)
        _c(Textures)
        _c(Images1D)
        _c(Images2D)
        _c(Images3D)
        _c(MeshLevels)
        _c(ImageLevels)
        _c(Names)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << (packed ? "" : "(") << Debug::nospace << Debug::hex << UnsignedInt(value) << Debug::nospace << (packed ? "" : ")");
}

Debug& operator<<(Debug& debug, const SceneContents value) {
    return Containers::enumSetDebugOutput(debug, value, debug.immediateFlags() >= Debug::Flag::Packed ? "{}" : "Trade::SceneContents{}", {
        SceneContent::Scenes,
        SceneContent::Animations,
        SceneContent::Lights,
        SceneContent::Cameras,
        SceneContent::Skins2D,
        SceneContent::Skins3D,
        SceneContent::Meshes,
        SceneContent::Materials,
        SceneContent::Textures,
        SceneContent::Images1D,
        SceneContent::Images2D,
        SceneContent::Images3D,
        SceneContent::MeshLevels,
        SceneContent::ImageLevels,
        SceneContent::Names});
}

}}
