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

#include "AbstractImporter.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/FileCallback.h"
#include "Magnum/Trade/AnimationData.h"
#include "Magnum/Trade/ArrayAllocator.h"
#include "Magnum/Trade/CameraData.h"
#include "Magnum/Trade/ImageData.h"
#include "Magnum/Trade/LightData.h"
#include "Magnum/Trade/MaterialData.h"
#include "Magnum/Trade/MeshData.h"
#include "Magnum/Trade/ObjectData2D.h"
#include "Magnum/Trade/ObjectData3D.h"
#include "Magnum/Trade/SceneData.h"
#include "Magnum/Trade/SkinData.h"
#include "Magnum/Trade/TextureData.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#define _MAGNUM_NO_DEPRECATED_MESHDATA /* So it doesn't yell here */

#include "Magnum/Trade/MeshData2D.h"
#include "Magnum/Trade/MeshData3D.h"
#endif

#ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
#include "Magnum/Trade/configure.h"
#endif

namespace Magnum { namespace Trade {

std::string AbstractImporter::pluginInterface() {
    return
/* [interface] */
"cz.mosra.magnum.Trade.AbstractImporter/0.3.3"
/* [interface] */
    ;
}

#ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
std::vector<std::string> AbstractImporter::pluginSearchPaths() {
    return PluginManager::implicitPluginSearchPaths(
        #ifndef MAGNUM_BUILD_STATIC
        Utility::Directory::libraryLocation(&pluginInterface),
        #else
        {},
        #endif
        #ifdef CORRADE_IS_DEBUG_BUILD
        MAGNUM_PLUGINS_IMPORTER_DEBUG_DIR,
        #else
        MAGNUM_PLUGINS_IMPORTER_DIR,
        #endif
        #ifdef CORRADE_IS_DEBUG_BUILD
        "magnum-d/"
        #else
        "magnum/"
        #endif
        "importers");
}
#endif

AbstractImporter::AbstractImporter() = default;

AbstractImporter::AbstractImporter(PluginManager::Manager<AbstractImporter>& manager): PluginManager::AbstractManagingPlugin<AbstractImporter>{manager} {}

AbstractImporter::AbstractImporter(PluginManager::AbstractManager& manager, const std::string& plugin): PluginManager::AbstractManagingPlugin<AbstractImporter>{manager, plugin} {}

void AbstractImporter::setFlags(ImporterFlags flags) {
    CORRADE_ASSERT(!isOpened(),
        "Trade::AbstractImporter::setFlags(): can't be set while a file is opened", );
    _flags = flags;
    doSetFlags(flags);
}

void AbstractImporter::doSetFlags(ImporterFlags) {}

void AbstractImporter::setFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*callback)(const std::string&, InputFileCallbackPolicy, void*), void* const userData) {
    CORRADE_ASSERT(!isOpened(), "Trade::AbstractImporter::setFileCallback(): can't be set while a file is opened", );
    CORRADE_ASSERT(features() & (ImporterFeature::FileCallback|ImporterFeature::OpenData), "Trade::AbstractImporter::setFileCallback(): importer supports neither loading from data nor via callbacks, callbacks can't be used", );

    _fileCallback = callback;
    _fileCallbackUserData = userData;
    doSetFileCallback(callback, userData);
}

void AbstractImporter::doSetFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, void*), void*) {}

bool AbstractImporter::openData(Containers::ArrayView<const char> data) {
    CORRADE_ASSERT(features() & ImporterFeature::OpenData,
        "Trade::AbstractImporter::openData(): feature not supported", {});

    /* We accept empty data here (instead of checking for them and failing so
       the check doesn't be done on the plugin side) because for some file
       formats it could be valid (e.g. OBJ or JSON-based formats). */
    close();
    doOpenData(data);
    return isOpened();
}

void AbstractImporter::doOpenData(Containers::ArrayView<const char>) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImporter::openData(): feature advertised but not implemented", );
}

bool AbstractImporter::openState(const void* state, const std::string& filePath) {
    CORRADE_ASSERT(features() & ImporterFeature::OpenState,
        "Trade::AbstractImporter::openState(): feature not supported", {});

    close();
    doOpenState(state, filePath);
    return isOpened();
}

void AbstractImporter::doOpenState(const void*, const std::string&) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImporter::openState(): feature advertised but not implemented", );
}

bool AbstractImporter::openFile(const std::string& filename) {
    close();

    /* If file loading callbacks are not set or the importer supports handling
       them directly, call into the implementation */
    if(!_fileCallback || (doFeatures() & ImporterFeature::FileCallback)) {
        doOpenFile(filename);

    /* Otherwise, if loading from data is supported, use the callback and pass
       the data through to openData(). Mark the file as ready to be closed once
       opening is finished. */
    } else if(doFeatures() & ImporterFeature::OpenData) {
        /* This needs to be duplicated here and in the doOpenFile()
           implementation in order to support both following cases:
            - plugins that don't support FileCallback but have their own
              doOpenFile() implementation (callback needs to be used here,
              because the base doOpenFile() implementation might never get
              called)
            - plugins that support FileCallback but want to delegate the actual
              file loading to the default implementation (callback used in the
              base doOpenFile() implementation, because this branch is never
              taken in that case) */
        const Containers::Optional<Containers::ArrayView<const char>> data = _fileCallback(filename, InputFileCallbackPolicy::LoadTemporary, _fileCallbackUserData);
        if(!data) {
            Error() << "Trade::AbstractImporter::openFile(): cannot open file" << filename;
            return isOpened();
        }
        doOpenData(*data);
        _fileCallback(filename, InputFileCallbackPolicy::Close, _fileCallbackUserData);

    /* Shouldn't get here, the assert is fired already in setFileCallback() */
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

    return isOpened();
}

void AbstractImporter::doOpenFile(const std::string& filename) {
    CORRADE_ASSERT(features() & ImporterFeature::OpenData, "Trade::AbstractImporter::openFile(): not implemented", );

    /* If callbacks are set, use them. This is the same implementation as in
       openFile(), see the comment there for details. */
    if(_fileCallback) {
        const Containers::Optional<Containers::ArrayView<const char>> data = _fileCallback(filename, InputFileCallbackPolicy::LoadTemporary, _fileCallbackUserData);
        if(!data) {
            Error() << "Trade::AbstractImporter::openFile(): cannot open file" << filename;
            return;
        }
        doOpenData(*data);
        _fileCallback(filename, InputFileCallbackPolicy::Close, _fileCallbackUserData);

    /* Otherwise open the file directly */
    } else {
        if(!Utility::Directory::exists(filename)) {
            Error() << "Trade::AbstractImporter::openFile(): cannot open file" << filename;
            return;
        }

        doOpenData(Utility::Directory::read(filename));
    }
}

void AbstractImporter::close() {
    if(isOpened()) {
        doClose();
        CORRADE_INTERNAL_ASSERT(!isOpened());
    }
}

Int AbstractImporter::defaultScene() const {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::defaultScene(): no file opened", -1);
    return doDefaultScene();
}

Int AbstractImporter::doDefaultScene() const { return -1; }

UnsignedInt AbstractImporter::sceneCount() const {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::sceneCount(): no file opened", 0);
    return doSceneCount();
}

UnsignedInt AbstractImporter::doSceneCount() const { return 0; }

Int AbstractImporter::sceneForName(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::sceneForName(): no file opened", -1);
    return doSceneForName(name);
}

Int AbstractImporter::doSceneForName(const std::string&) { return -1; }

std::string AbstractImporter::sceneName(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::sceneName(): no file opened", {});
    CORRADE_ASSERT(id < doSceneCount(), "Trade::AbstractImporter::sceneName(): index" << id << "out of range for" << doSceneCount() << "entries", {});
    return doSceneName(id);
}

std::string AbstractImporter::doSceneName(UnsignedInt) { return {}; }

Containers::Optional<SceneData> AbstractImporter::scene(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::scene(): no file opened", {});
    CORRADE_ASSERT(id < doSceneCount(), "Trade::AbstractImporter::scene(): index" << id << "out of range for" << doSceneCount() << "entries", {});
    return doScene(id);
}

Containers::Optional<SceneData> AbstractImporter::doScene(UnsignedInt) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImporter::scene(): not implemented", {});
}

Containers::Optional<SceneData> AbstractImporter::scene(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::scene(): no file opened", {});
    const Int id = doSceneForName(name);
    if(id == -1) {
        Error{} << "Trade::AbstractImporter::scene(): scene" << name << "not found";
        return {};
    }
    return scene(id); /* not doScene(), so we get the range checks also */
}

UnsignedInt AbstractImporter::animationCount() const {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::animationCount(): no file opened", {});
    return doAnimationCount();
}

UnsignedInt AbstractImporter::doAnimationCount() const { return 0; }

Int AbstractImporter::animationForName(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::animationForName(): no file opened", {});
    return doAnimationForName(name);
}

Int AbstractImporter::doAnimationForName(const std::string&) { return -1; }

std::string AbstractImporter::animationName(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::animationName(): no file opened", {});
    CORRADE_ASSERT(id < doAnimationCount(), "Trade::AbstractImporter::animationName(): index" << id << "out of range for" << doAnimationCount() << "entries", {});
    return doAnimationName(id);
}

std::string AbstractImporter::doAnimationName(UnsignedInt) { return {}; }

Containers::Optional<AnimationData> AbstractImporter::animation(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::animation(): no file opened", {});
    CORRADE_ASSERT(id < doAnimationCount(), "Trade::AbstractImporter::animation(): index" << id << "out of range for" << doAnimationCount() << "entries", {});
    Containers::Optional<AnimationData> animation = doAnimation(id);
    CORRADE_ASSERT(!animation ||
        ((!animation->_data.deleter() || animation->_data.deleter() == Implementation::nonOwnedArrayDeleter || animation->_data.deleter() == ArrayAllocator<char>::deleter) &&
        (!animation->_tracks.deleter() || animation->_tracks.deleter() == reinterpret_cast<void(*)(AnimationTrackData*, std::size_t)>(Implementation::nonOwnedArrayDeleter))),
        "Trade::AbstractImporter::animation(): implementation is not allowed to use a custom Array deleter", {});
    return animation;
}

Containers::Optional<AnimationData> AbstractImporter::doAnimation(UnsignedInt) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImporter::animation(): not implemented", {});
}

Containers::Optional<AnimationData> AbstractImporter::animation(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::animation(): no file opened", {});
    const Int id = doAnimationForName(name);
    if(id == -1) {
        Error{} << "Trade::AbstractImporter::animation(): animation" << name << "not found";
        return {};
    }
    return animation(id); /* not doAnimation(), so we get the checks also */
}

UnsignedInt AbstractImporter::lightCount() const {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::lightCount(): no file opened", {});
    return doLightCount();
}

UnsignedInt AbstractImporter::doLightCount() const { return 0; }

Int AbstractImporter::lightForName(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::lightForName(): no file opened", {});
    return doLightForName(name);
}

Int AbstractImporter::doLightForName(const std::string&) { return -1; }

std::string AbstractImporter::lightName(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::lightName(): no file opened", {});
    CORRADE_ASSERT(id < doLightCount(), "Trade::AbstractImporter::lightName(): index" << id << "out of range for" << doLightCount() << "entries", {});
    return doLightName(id);
}

std::string AbstractImporter::doLightName(UnsignedInt) { return {}; }

Containers::Optional<LightData> AbstractImporter::light(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::light(): no file opened", {});
    CORRADE_ASSERT(id < doLightCount(), "Trade::AbstractImporter::light(): index" << id << "out of range for" << doLightCount() << "entries", {});
    return doLight(id);
}

Containers::Optional<LightData> AbstractImporter::doLight(UnsignedInt) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImporter::light(): not implemented", {});
}

Containers::Optional<LightData> AbstractImporter::light(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::light(): no file opened", {});
    const Int id = doLightForName(name);
    if(id == -1) {
        Error{} << "Trade::AbstractImporter::light(): light" << name << "not found";
        return {};
    }
    return light(id); /* not doLight(), so we get the range checks also */
}

UnsignedInt AbstractImporter::cameraCount() const {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::cameraCount(): no file opened", {});
    return doCameraCount();
}

UnsignedInt AbstractImporter::doCameraCount() const { return 0; }

Int AbstractImporter::cameraForName(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::cameraForName(): no file opened", {});
    return doCameraForName(name);
}

Int AbstractImporter::doCameraForName(const std::string&) { return -1; }

std::string AbstractImporter::cameraName(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::cameraName(): no file opened", {});
    CORRADE_ASSERT(id < doCameraCount(), "Trade::AbstractImporter::cameraName(): index" << id << "out of range for" << doCameraCount() << "entries", {});
    return doCameraName(id);
}

std::string AbstractImporter::doCameraName(UnsignedInt) { return {}; }

Containers::Optional<CameraData> AbstractImporter::camera(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::camera(): no file opened", {});
    CORRADE_ASSERT(id < doCameraCount(), "Trade::AbstractImporter::camera(): index" << id << "out of range for" << doCameraCount() << "entries", {});
    return doCamera(id);
}

Containers::Optional<CameraData> AbstractImporter::doCamera(UnsignedInt) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImporter::camera(): not implemented", {});
}

Containers::Optional<CameraData> AbstractImporter::camera(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::camera(): no file opened", {});
    const Int id = doCameraForName(name);
    if(id == -1) {
        Error{} << "Trade::AbstractImporter::camera(): camera" << name << "not found";
        return {};
    }
    return camera(id); /* not doCamera(), so we get the range checks also */
}

UnsignedInt AbstractImporter::object2DCount() const {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::object2DCount(): no file opened", {});
    return doObject2DCount();
}

UnsignedInt AbstractImporter::doObject2DCount() const { return 0; }

Int AbstractImporter::object2DForName(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::object2DForName(): no file opened", {});
    return doObject2DForName(name);
}

Int AbstractImporter::doObject2DForName(const std::string&) { return -1; }

std::string AbstractImporter::object2DName(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::object2DName(): no file opened", {});
    CORRADE_ASSERT(id < doObject2DCount(), "Trade::AbstractImporter::object2DName(): index" << id << "out of range for" << doObject2DCount() << "entries", {});
    return doObject2DName(id);
}

std::string AbstractImporter::doObject2DName(UnsignedInt) { return {}; }

Containers::Pointer<ObjectData2D> AbstractImporter::object2D(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::object2D(): no file opened", {});
    CORRADE_ASSERT(id < doObject2DCount(), "Trade::AbstractImporter::object2D(): index" << id << "out of range for" << doObject2DCount() << "entries", {});
    return doObject2D(id);
}

Containers::Pointer<ObjectData2D> AbstractImporter::doObject2D(UnsignedInt) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImporter::object2D(): not implemented", {});
}

Containers::Pointer<ObjectData2D> AbstractImporter::object2D(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::object2D(): no file opened", {});
    const Int id = doObject2DForName(name);
    if(id == -1) {
        Error{} << "Trade::AbstractImporter::object2D(): object" << name << "not found";
        return {};
    }
    return object2D(id); /* not doObject2D(), so we get the range checks also */
}

UnsignedInt AbstractImporter::object3DCount() const {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::object3DCount(): no file opened", {});
    return doObject3DCount();
}

UnsignedInt AbstractImporter::doObject3DCount() const { return 0; }

Int AbstractImporter::object3DForName(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::object3DForName(): no file opened", {});
    return doObject3DForName(name);
}

Int AbstractImporter::doObject3DForName(const std::string&) { return -1; }

std::string AbstractImporter::object3DName(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::object3DName(): no file opened", {});
    CORRADE_ASSERT(id < doObject3DCount(), "Trade::AbstractImporter::object3DName(): index" << id << "out of range for" << doObject3DCount() << "entries", {});
    return doObject3DName(id);
}

std::string AbstractImporter::doObject3DName(UnsignedInt) { return {}; }

Containers::Pointer<ObjectData3D> AbstractImporter::object3D(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::object3D(): no file opened", {});
    CORRADE_ASSERT(id < doObject3DCount(), "Trade::AbstractImporter::object3D(): index" << id << "out of range for" << doObject3DCount() << "entries", {});
    return doObject3D(id);
}

Containers::Pointer<ObjectData3D> AbstractImporter::doObject3D(UnsignedInt) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImporter::object3D(): not implemented", {});
}

Containers::Pointer<ObjectData3D> AbstractImporter::object3D(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::object3D(): no file opened", {});
    const Int id = doObject3DForName(name);
    if(id == -1) {
        Error{} << "Trade::AbstractImporter::object3D(): object" << name << "not found";
        return {};
    }
    return object3D(id); /* not doObject3D(), so we get the range checks also */
}

UnsignedInt AbstractImporter::skin2DCount() const {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::skin2DCount(): no file opened", {});
    return doSkin2DCount();
}

UnsignedInt AbstractImporter::doSkin2DCount() const { return 0; }

Int AbstractImporter::skin2DForName(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::skin2DForName(): no file opened", {});
    return doSkin2DForName(name);
}

Int AbstractImporter::doSkin2DForName(const std::string&) { return -1; }

std::string AbstractImporter::skin2DName(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::skin2DName(): no file opened", {});
    CORRADE_ASSERT(id < doSkin2DCount(), "Trade::AbstractImporter::skin2DName(): index" << id << "out of range for" << doSkin2DCount() << "entries", {});
    return doSkin2DName(id);
}

std::string AbstractImporter::doSkin2DName(UnsignedInt) { return {}; }

Containers::Optional<SkinData2D> AbstractImporter::skin2D(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::skin2D(): no file opened", {});
    CORRADE_ASSERT(id < doSkin2DCount(), "Trade::AbstractImporter::skin2D(): index" << id << "out of range for" << doSkin2DCount() << "entries", {});
    Containers::Optional<SkinData2D> skin = doSkin2D(id);
    CORRADE_ASSERT(!skin || (
        (!skin->_jointData.deleter() || skin->_jointData.deleter() == reinterpret_cast<void(*)(UnsignedInt*, std::size_t)>(Implementation::nonOwnedArrayDeleter)) &&
        (!skin->_inverseBindMatrixData.deleter() || skin->_inverseBindMatrixData.deleter() == reinterpret_cast<void(*)(Matrix3*, std::size_t)>(Implementation::nonOwnedArrayDeleter))),
        "Trade::AbstractImporter::skin2D(): implementation is not allowed to use a custom Array deleter", {});
    return skin;
}

Containers::Optional<SkinData2D> AbstractImporter::doSkin2D(UnsignedInt) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImporter::skin2D(): not implemented", {});
}

Containers::Optional<SkinData2D> AbstractImporter::skin2D(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::skin2D(): no file opened", {});
    const Int id = doSkin2DForName(name);
    if(id == -1) {
        Error{} << "Trade::AbstractImporter::skin2D(): skin" << name << "not found";
        return {};
    }
    return skin2D(id); /* not doSkin2D(), so we get the range checks also */
}

UnsignedInt AbstractImporter::skin3DCount() const {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::skin3DCount(): no file opened", {});
    return doSkin3DCount();
}

UnsignedInt AbstractImporter::doSkin3DCount() const { return 0; }

Int AbstractImporter::skin3DForName(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::skin3DForName(): no file opened", {});
    return doSkin3DForName(name);
}

Int AbstractImporter::doSkin3DForName(const std::string&) { return -1; }

std::string AbstractImporter::skin3DName(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::skin3DName(): no file opened", {});
    CORRADE_ASSERT(id < doSkin3DCount(), "Trade::AbstractImporter::skin3DName(): index" << id << "out of range for" << doSkin3DCount() << "entries", {});
    return doSkin3DName(id);
}

std::string AbstractImporter::doSkin3DName(UnsignedInt) { return {}; }

Containers::Optional<SkinData3D> AbstractImporter::skin3D(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::skin3D(): no file opened", {});
    CORRADE_ASSERT(id < doSkin3DCount(), "Trade::AbstractImporter::skin3D(): index" << id << "out of range for" << doSkin3DCount() << "entries", {});
    Containers::Optional<SkinData3D> skin = doSkin3D(id);
    CORRADE_ASSERT(!skin || (
        (!skin->_jointData.deleter() || skin->_jointData.deleter() == reinterpret_cast<void(*)(UnsignedInt*, std::size_t)>(Implementation::nonOwnedArrayDeleter)) &&
        (!skin->_inverseBindMatrixData.deleter() || skin->_inverseBindMatrixData.deleter() == reinterpret_cast<void(*)(Matrix4*, std::size_t)>(Implementation::nonOwnedArrayDeleter))),
        "Trade::AbstractImporter::skin3D(): implementation is not allowed to use a custom Array deleter", {});
    return skin;
}

Containers::Optional<SkinData3D> AbstractImporter::doSkin3D(UnsignedInt) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImporter::skin3D(): not implemented", {});
}

Containers::Optional<SkinData3D> AbstractImporter::skin3D(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::skin3D(): no file opened", {});
    const Int id = doSkin3DForName(name);
    if(id == -1) {
        Error{} << "Trade::AbstractImporter::skin3D(): skin" << name << "not found";
        return {};
    }
    return skin3D(id); /* not doSkin3D(), so we get the range checks also */
}

UnsignedInt AbstractImporter::meshCount() const {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::meshCount(): no file opened", {});
    return doMeshCount();
}

UnsignedInt AbstractImporter::doMeshCount() const { return 0; }

UnsignedInt AbstractImporter::meshLevelCount(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::meshLevelCount(): no file opened", {});
    CORRADE_ASSERT(id < doMeshCount(), "Trade::AbstractImporter::meshLevelCount(): index" << id << "out of range for" << doMeshCount() << "entries", {});
    const UnsignedInt out = doMeshLevelCount(id);
    CORRADE_ASSERT(out, "Trade::AbstractImporter::meshLevelCount(): implementation reported zero levels", {});
    return out;
}

UnsignedInt AbstractImporter::doMeshLevelCount(UnsignedInt) { return 1; }

Int AbstractImporter::meshForName(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::meshForName(): no file opened", {});
    return doMeshForName(name);
}

Int AbstractImporter::doMeshForName(const std::string&) { return -1; }

std::string AbstractImporter::meshName(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::meshName(): no file opened", {});
    CORRADE_ASSERT(id < doMeshCount(), "Trade::AbstractImporter::meshName(): index" << id << "out of range for" << doMeshCount() << "entries", {});
    return doMeshName(id);
}

std::string AbstractImporter::doMeshName(UnsignedInt) { return {}; }

Containers::Optional<MeshData> AbstractImporter::mesh(const UnsignedInt id, const UnsignedInt level) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::mesh(): no file opened", {});
    CORRADE_ASSERT(id < doMeshCount(), "Trade::AbstractImporter::mesh(): index" << id << "out of range for" << doMeshCount() << "entries", {});
    #ifndef CORRADE_NO_ASSERT
    /* Check for the range only if requested level is nonzero, as
       meshLevelCount() is expected to return >= 1. This is done to prevent
       random assertions and messages from a doMeshLevelCount() to be printed
       (which are unlikely, but let's be consistent with what image*D() does). */
    if(level) {
        const UnsignedInt levelCount = doMeshLevelCount(id);
        CORRADE_ASSERT(levelCount, "Trade::AbstractImporter::mesh(): implementation reported zero levels", {});
        CORRADE_ASSERT(level < levelCount, "Trade::AbstractImporter::mesh(): level" << level << "out of range for" << levelCount << "entries", {});
    }
    #endif
    Containers::Optional<MeshData> mesh = doMesh(id, level);
    CORRADE_ASSERT(!mesh || (
        (!mesh->_indexData.deleter() || mesh->_indexData.deleter() == Implementation::nonOwnedArrayDeleter || mesh->_indexData.deleter() == ArrayAllocator<char>::deleter) &&
        (!mesh->_vertexData.deleter() || mesh->_vertexData.deleter() == Implementation::nonOwnedArrayDeleter || mesh->_vertexData.deleter() == ArrayAllocator<char>::deleter) &&
        (!mesh->_attributes.deleter() || mesh->_attributes.deleter() == reinterpret_cast<void(*)(MeshAttributeData*, std::size_t)>(Implementation::nonOwnedArrayDeleter))),
        "Trade::AbstractImporter::mesh(): implementation is not allowed to use a custom Array deleter", {});
    return mesh;
}

Containers::Optional<MeshData> AbstractImporter::doMesh(UnsignedInt, UnsignedInt) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImporter::mesh(): not implemented", {});
}

Containers::Optional<MeshData> AbstractImporter::mesh(const std::string& name, const UnsignedInt level) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::mesh(): no file opened", {});
    const Int id = doMeshForName(name);
    if(id == -1) {
        Error{} << "Trade::AbstractImporter::mesh(): mesh" << name << "not found";
        return {};
    }
    return mesh(id, level); /* not doMesh(), so we get the checks also */
}

MeshAttribute AbstractImporter::meshAttributeForName(const std::string& name) {
    const MeshAttribute out = doMeshAttributeForName(name);
    CORRADE_ASSERT(out == MeshAttribute{} || isMeshAttributeCustom(out),
        "Trade::AbstractImporter::meshAttributeForName(): implementation-returned" << out << "is neither custom nor invalid", {});
    return out;
}

MeshAttribute AbstractImporter::doMeshAttributeForName(const std::string&) {
    return {};
}

std::string AbstractImporter::meshAttributeName(MeshAttribute name) {
    CORRADE_ASSERT(isMeshAttributeCustom(name),
        "Trade::AbstractImporter::meshAttributeName():" << name << "is not custom", {});
    return doMeshAttributeName(meshAttributeCustom(name));
}

std::string AbstractImporter::doMeshAttributeName(UnsignedShort) { return {}; }

#ifdef MAGNUM_BUILD_DEPRECATED
UnsignedInt AbstractImporter::mesh2DCount() const {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::mesh2DCount(): no file opened", {});
    CORRADE_IGNORE_DEPRECATED_PUSH
    return doMesh2DCount();
    CORRADE_IGNORE_DEPRECATED_POP
}

UnsignedInt AbstractImporter::doMesh2DCount() const { return 0; }

Int AbstractImporter::mesh2DForName(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::mesh2DForName(): no file opened", {});
    CORRADE_IGNORE_DEPRECATED_PUSH
    return doMesh2DForName(name);
    CORRADE_IGNORE_DEPRECATED_POP
}

Int AbstractImporter::doMesh2DForName(const std::string&) { return -1; }

std::string AbstractImporter::mesh2DName(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::mesh2DName(): no file opened", {});
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_ASSERT(id < doMesh2DCount(), "Trade::AbstractImporter::mesh2DName(): index" << id << "out of range for" << doMesh2DCount() << "entries", {});
    return doMesh2DName(id);
    CORRADE_IGNORE_DEPRECATED_POP
}

std::string AbstractImporter::doMesh2DName(UnsignedInt) { return {}; }

CORRADE_IGNORE_DEPRECATED_PUSH
Containers::Optional<MeshData2D> AbstractImporter::mesh2D(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::mesh2D(): no file opened", {});
    CORRADE_ASSERT(id < doMesh2DCount(), "Trade::AbstractImporter::mesh2D(): index" << id << "out of range for" << doMesh2DCount() << "entries", {});
    return doMesh2D(id);
}

Containers::Optional<MeshData2D> AbstractImporter::doMesh2D(UnsignedInt) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImporter::mesh2D(): not implemented", {});
}
CORRADE_IGNORE_DEPRECATED_POP

UnsignedInt AbstractImporter::mesh3DCount() const {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::mesh3DCount(): no file opened", {});
    CORRADE_IGNORE_DEPRECATED_PUSH
    return doMesh3DCount();
    CORRADE_IGNORE_DEPRECATED_POP
}

UnsignedInt AbstractImporter::doMesh3DCount() const {
    return doMeshCount();
}

Int AbstractImporter::mesh3DForName(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::mesh3DForName(): no file opened", {});
    CORRADE_IGNORE_DEPRECATED_PUSH
    return doMesh3DForName(name);
    CORRADE_IGNORE_DEPRECATED_POP
}

Int AbstractImporter::doMesh3DForName(const std::string& name) {
    return doMeshForName(name);
}

std::string AbstractImporter::mesh3DName(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::mesh3DName(): no file opened", {});
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_ASSERT(id < doMesh3DCount(), "Trade::AbstractImporter::mesh3DName(): index" << id << "out of range for" << doMesh3DCount() << "entries", {});
    return doMesh3DName(id);
    CORRADE_IGNORE_DEPRECATED_POP
}

std::string AbstractImporter::doMesh3DName(const UnsignedInt id) {
    return doMeshName(id);
}

CORRADE_IGNORE_DEPRECATED_PUSH
Containers::Optional<MeshData3D> AbstractImporter::mesh3D(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::mesh3D(): no file opened", {});
    CORRADE_ASSERT(id < doMesh3DCount(), "Trade::AbstractImporter::mesh3D(): index" << id << "out of range for" << doMesh3DCount() << "entries", {});
    return doMesh3D(id);
}

Containers::Optional<MeshData3D> AbstractImporter::doMesh3D(const UnsignedInt id) {
    Containers::Optional<MeshData> out = doMesh(id, 0);
    if(out) return MeshData3D{*out};
    return Containers::NullOpt;
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

UnsignedInt AbstractImporter::materialCount() const {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::materialCount(): no file opened", {});
    return doMaterialCount();
}

UnsignedInt AbstractImporter::doMaterialCount() const { return 0; }

Int AbstractImporter::materialForName(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::materialForName(): no file opened", {});
    return doMaterialForName(name);
}

Int AbstractImporter::doMaterialForName(const std::string&) { return -1; }

std::string AbstractImporter::materialName(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::materialName(): no file opened", {});
    CORRADE_ASSERT(id < doMaterialCount(), "Trade::AbstractImporter::materialName(): index" << id << "out of range for" << doMaterialCount() << "entries", {});
    return doMaterialName(id);
}

std::string AbstractImporter::doMaterialName(UnsignedInt) { return {}; }

#if !defined(MAGNUM_BUILD_DEPRECATED) || defined(DOXYGEN_GENERATING_OUTPUT)
Containers::Optional<MaterialData>
#else
OptionalButAlsoPointer<MaterialData>
#endif
AbstractImporter::material(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::material(): no file opened", {});
    CORRADE_ASSERT(id < doMaterialCount(), "Trade::AbstractImporter::material(): index" << id << "out of range for" << doMaterialCount() << "entries", {});

    Containers::Optional<MaterialData> material = doMaterial(id);
    CORRADE_ASSERT(!material || (
        (!material->_data.deleter() || material->_data.deleter() == reinterpret_cast<void(*)(MaterialAttributeData*, std::size_t)>(Implementation::nonOwnedArrayDeleter)) &&
        (!material->_layerOffsets.deleter() || material->_layerOffsets.deleter() == reinterpret_cast<void(*)(UnsignedInt*, std::size_t)>(Implementation::nonOwnedArrayDeleter))),
        "Trade::AbstractImporter::material(): implementation is not allowed to use a custom Array deleter", {});

    /* GCC 4.8 and clang-cl needs an explicit conversion here */
    #ifdef MAGNUM_BUILD_DEPRECATED
    return OptionalButAlsoPointer<MaterialData>{std::move(material)};
    #else
    return material;
    #endif
}

Containers::Optional<MaterialData> AbstractImporter::doMaterial(UnsignedInt) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImporter::material(): not implemented", {});
}

#if !defined(MAGNUM_BUILD_DEPRECATED) || defined(DOXYGEN_GENERATING_OUTPUT)
Containers::Optional<MaterialData>
#else
OptionalButAlsoPointer<MaterialData>
#endif
AbstractImporter::material(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::material(): no file opened", {});
    const Int id = doMaterialForName(name);
    if(id == -1) {
        Error{} << "Trade::AbstractImporter::material(): material" << name << "not found";
        return {};
    }
    return material(id); /* not doMaterial(), so we get the range checks also */
}

UnsignedInt AbstractImporter::textureCount() const {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::textureCount(): no file opened", {});
    return doTextureCount();
}

UnsignedInt AbstractImporter::doTextureCount() const { return 0; }

Int AbstractImporter::textureForName(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::textureForName(): no file opened", {});
    return doTextureForName(name);
}

Int AbstractImporter::doTextureForName(const std::string&) { return -1; }

std::string AbstractImporter::textureName(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::textureName(): no file opened", {});
    CORRADE_ASSERT(id < doTextureCount(), "Trade::AbstractImporter::textureName(): index" << id << "out of range for" << doTextureCount() << "entries", {});
    return doTextureName(id);
}

std::string AbstractImporter::doTextureName(UnsignedInt) { return {}; }

Containers::Optional<TextureData> AbstractImporter::texture(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::texture(): no file opened", {});
    CORRADE_ASSERT(id < doTextureCount(), "Trade::AbstractImporter::texture(): index" << id << "out of range for" << doTextureCount() << "entries", {});
    return doTexture(id);
}

Containers::Optional<TextureData> AbstractImporter::doTexture(UnsignedInt) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImporter::texture(): not implemented", {});
}

Containers::Optional<TextureData> AbstractImporter::texture(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::texture(): no file opened", {});
    const Int id = doTextureForName(name);
    if(id == -1) {
        Error{} << "Trade::AbstractImporter::texture(): texture" << name << "not found";
        return {};
    }
    return texture(id); /* not doTexture(), so we get the range checks also */
}

UnsignedInt AbstractImporter::image1DCount() const {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image1DCount(): no file opened", {});
    return doImage1DCount();
}

UnsignedInt AbstractImporter::doImage1DCount() const { return 0; }

UnsignedInt AbstractImporter::image1DLevelCount(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image1DLevelCount(): no file opened", {});
    CORRADE_ASSERT(id < doImage1DCount(), "Trade::AbstractImporter::image1DLevelCount(): index" << id << "out of range for" << doImage1DCount() << "entries", {});
    const UnsignedInt out = doImage1DLevelCount(id);
    CORRADE_ASSERT(out, "Trade::AbstractImporter::image1DLevelCount(): implementation reported zero levels", {});
    return out;
}

UnsignedInt AbstractImporter::doImage1DLevelCount(UnsignedInt) { return 1; }

Int AbstractImporter::image1DForName(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image1DForName(): no file opened", {});
    return doImage1DForName(name);
}

Int AbstractImporter::doImage1DForName(const std::string&) { return -1; }

std::string AbstractImporter::image1DName(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image1DName(): no file opened", {});
    CORRADE_ASSERT(id < doImage1DCount(), "Trade::AbstractImporter::image1DName(): index" << id << "out of range for" << doImage1DCount() << "entries", {});
    return doImage1DName(id);
}

std::string AbstractImporter::doImage1DName(UnsignedInt) { return {}; }

Containers::Optional<ImageData1D> AbstractImporter::image1D(const UnsignedInt id, const UnsignedInt level) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image1D(): no file opened", {});
    CORRADE_ASSERT(id < doImage1DCount(), "Trade::AbstractImporter::image1D(): index" << id << "out of range for" << doImage1DCount() << "entries", {});
    #ifndef CORRADE_NO_ASSERT
    /* Check for the range only if requested level is nonzero, as
       image*DLevelCount() is expected to return >= 1. This is done to prevent
       random assertions and messages from a doImage*DLevelCount() to be
       printed (e.g., many plugins delegate image loading and assert an access
       to the manager for that), which may be confusing */
    if(level) {
        const UnsignedInt levelCount = doImage1DLevelCount(id);
        CORRADE_ASSERT(levelCount, "Trade::AbstractImporter::image1D(): implementation reported zero levels", {});
        CORRADE_ASSERT(level < levelCount, "Trade::AbstractImporter::image1D(): level" << level << "out of range for" << levelCount << "entries", {});
    }
    #endif
    Containers::Optional<ImageData1D> image = doImage1D(id, level);
    CORRADE_ASSERT(!image || !image->_data.deleter() || image->_data.deleter() == Implementation::nonOwnedArrayDeleter || image->_data.deleter() == ArrayAllocator<char>::deleter, "Trade::AbstractImporter::image1D(): implementation is not allowed to use a custom Array deleter", {});
    return image;
}

Containers::Optional<ImageData1D> AbstractImporter::doImage1D(UnsignedInt, UnsignedInt) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImporter::image1D(): not implemented", {});
}

Containers::Optional<ImageData1D> AbstractImporter::image1D(const std::string& name, const UnsignedInt level) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image1D(): no file opened", {});
    const Int id = doImage1DForName(name);
    if(id == -1) {
        Error{} << "Trade::AbstractImporter::image1D(): image" << name << "not found";
        return {};
    }
    /* not doImage1D(), so we get the range checks also */
    return image1D(id, level);
}

UnsignedInt AbstractImporter::image2DCount() const {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image2DCount(): no file opened", {});
    return doImage2DCount();
}

UnsignedInt AbstractImporter::doImage2DCount() const { return 0; }

UnsignedInt AbstractImporter::image2DLevelCount(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image2DLevelCount(): no file opened", {});
    CORRADE_ASSERT(id < doImage2DCount(), "Trade::AbstractImporter::image2DLevelCount(): index" << id << "out of range for" << doImage2DCount() << "entries", {});
    const UnsignedInt out = doImage2DLevelCount(id);
    CORRADE_ASSERT(out, "Trade::AbstractImporter::image2DLevelCount(): implementation reported zero levels", {});
    return out;
}

UnsignedInt AbstractImporter::doImage2DLevelCount(UnsignedInt) { return 1; }

Int AbstractImporter::image2DForName(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image2DForName(): no file opened", {});
    return doImage2DForName(name);
}

Int AbstractImporter::doImage2DForName(const std::string&) { return -1; }

std::string AbstractImporter::image2DName(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image2DName(): no file opened", {});
    CORRADE_ASSERT(id < doImage2DCount(), "Trade::AbstractImporter::image2DName(): index" << id << "out of range for" << doImage2DCount() << "entries", {});
    return doImage2DName(id);
}

std::string AbstractImporter::doImage2DName(UnsignedInt) { return {}; }

Containers::Optional<ImageData2D> AbstractImporter::image2D(const UnsignedInt id, const UnsignedInt level) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image2D(): no file opened", {});
    CORRADE_ASSERT(id < doImage2DCount(), "Trade::AbstractImporter::image2D(): index" << id << "out of range for" << doImage2DCount() << "entries", {});
    #ifndef CORRADE_NO_ASSERT
    /* Check for the range only if requested level is nonzero, as
       image*DLevelCount() is expected to return >= 1. This is done to prevent
       random assertions and messages from a doImage*DLevelCount() to be
       printed (e.g., many plugins delegate image loading and assert an access
       to the manager for that), which may be confusing */
    if(level) {
        const UnsignedInt levelCount = doImage2DLevelCount(id);
        CORRADE_ASSERT(levelCount, "Trade::AbstractImporter::image2D(): implementation reported zero levels", {});
        CORRADE_ASSERT(level < levelCount, "Trade::AbstractImporter::image2D(): level" << level << "out of range for" << levelCount << "entries", {});
    }
    #endif
    Containers::Optional<ImageData2D> image = doImage2D(id, level);
    CORRADE_ASSERT(!image || !image->_data.deleter() || image->_data.deleter() == Implementation::nonOwnedArrayDeleter || image->_data.deleter() == ArrayAllocator<char>::deleter, "Trade::AbstractImporter::image2D(): implementation is not allowed to use a custom Array deleter", {});
    return image;
}

Containers::Optional<ImageData2D> AbstractImporter::doImage2D(UnsignedInt, UnsignedInt) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImporter::image2D(): not implemented", {});
}

Containers::Optional<ImageData2D> AbstractImporter::image2D(const std::string& name, const UnsignedInt level) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image2D(): no file opened", {});
    const Int id = doImage2DForName(name);
    if(id == -1) {
        Error{} << "Trade::AbstractImporter::image2D(): image" << name << "not found";
        return {};
    }
    /* not doImage2D(), so we get the range checks also */
    return image2D(id, level);
}

UnsignedInt AbstractImporter::image3DCount() const {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image3DCount(): no file opened", {});
    return doImage3DCount();
}

UnsignedInt AbstractImporter::doImage3DCount() const { return 0; }

UnsignedInt AbstractImporter::image3DLevelCount(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image3DLevelCount(): no file opened", {});
    CORRADE_ASSERT(id < doImage3DCount(), "Trade::AbstractImporter::image3DLevelCount(): index" << id << "out of range for" << doImage3DCount() << "entries", {});
    const UnsignedInt out = doImage3DLevelCount(id);
    CORRADE_ASSERT(out, "Trade::AbstractImporter::image3DLevelCount(): implementation reported zero levels", {});
    return out;
}

UnsignedInt AbstractImporter::doImage3DLevelCount(UnsignedInt) { return 1; }

Int AbstractImporter::image3DForName(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image3DForName(): no file opened", {});
    return doImage3DForName(name);
}

Int AbstractImporter::doImage3DForName(const std::string&) { return -1; }

std::string AbstractImporter::image3DName(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image3DName(): no file opened", {});
    CORRADE_ASSERT(id < doImage3DCount(), "Trade::AbstractImporter::image3DName(): index" << id << "out of range for" << doImage3DCount() << "entries", {});
    return doImage3DName(id);
}

std::string AbstractImporter::doImage3DName(UnsignedInt) { return {}; }

Containers::Optional<ImageData3D> AbstractImporter::image3D(const UnsignedInt id, const UnsignedInt level) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image3D(): no file opened", {});
    CORRADE_ASSERT(id < doImage3DCount(), "Trade::AbstractImporter::image3D(): index" << id << "out of range for" << doImage3DCount() << "entries", {});
    #ifndef CORRADE_NO_ASSERT
    /* Check for the range only if requested level is nonzero, as
       image*DLevelCount() is expected to return >= 1. This is done to prevent
       random assertions and messages from a doImage*DLevelCount() to be
       printed (e.g., many plugins delegate image loading and assert an access
       to the manager for that), which may be confusing */
    if(level) {
        const UnsignedInt levelCount = doImage3DLevelCount(id);
        CORRADE_ASSERT(levelCount, "Trade::AbstractImporter::image3D(): implementation reported zero levels", {});
        CORRADE_ASSERT(level < levelCount, "Trade::AbstractImporter::image3D(): level" << level << "out of range for" << levelCount << "entries", {});
    }
    #endif
    Containers::Optional<ImageData3D> image = doImage3D(id, level);
    CORRADE_ASSERT(!image || !image->_data.deleter() || image->_data.deleter() == Implementation::nonOwnedArrayDeleter || image->_data.deleter() == ArrayAllocator<char>::deleter, "Trade::AbstractImporter::image3D(): implementation is not allowed to use a custom Array deleter", {});
    return image;
}

Containers::Optional<ImageData3D> AbstractImporter::doImage3D(UnsignedInt, UnsignedInt) {
    CORRADE_ASSERT_UNREACHABLE("Trade::AbstractImporter::image3D(): not implemented", {});
}

Containers::Optional<ImageData3D> AbstractImporter::image3D(const std::string& name, const UnsignedInt level) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image3D(): no file opened", {});
    const Int id = doImage3DForName(name);
    if(id == -1) {
        Error{} << "Trade::AbstractImporter::image3D(): image" << name << "not found";
        return {};
    }
    /* not doImage3D(), so we get the range checks also */
    return image3D(id, level);
}

const void* AbstractImporter::importerState() const {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::importerState(): no file opened", {});
    return doImporterState();
}

const void* AbstractImporter::doImporterState() const { return nullptr; }

Debug& operator<<(Debug& debug, const ImporterFeature value) {
    debug << "Trade::ImporterFeature" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case ImporterFeature::v: return debug << "::" #v;
        _c(OpenData)
        _c(OpenState)
        _c(FileCallback)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const ImporterFeatures value) {
    return Containers::enumSetDebugOutput(debug, value, "Trade::ImporterFeatures{}", {
        ImporterFeature::OpenData,
        ImporterFeature::OpenState,
        ImporterFeature::FileCallback});
}

Debug& operator<<(Debug& debug, const ImporterFlag value) {
    debug << "Trade::ImporterFlag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case ImporterFlag::v: return debug << "::" #v;
        _c(Verbose)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const ImporterFlags value) {
    return Containers::enumSetDebugOutput(debug, value, "Trade::ImporterFlags{}", {
        ImporterFlag::Verbose});
}

}}
