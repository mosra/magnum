/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include "AbstractImporter.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/Trade/AbstractMaterialData.h"
#include "Magnum/Trade/CameraData.h"
#include "Magnum/Trade/ImageData.h"
#include "Magnum/Trade/LightData.h"
#include "Magnum/Trade/MeshData2D.h"
#include "Magnum/Trade/MeshData3D.h"
#include "Magnum/Trade/ObjectData2D.h"
#include "Magnum/Trade/ObjectData3D.h"
#include "Magnum/Trade/SceneData.h"
#include "Magnum/Trade/TextureData.h"

namespace Magnum { namespace Trade {

AbstractImporter::AbstractImporter() = default;

AbstractImporter::AbstractImporter(PluginManager::Manager<AbstractImporter>& manager): PluginManager::AbstractManagingPlugin<AbstractImporter>{manager} {}

AbstractImporter::AbstractImporter(PluginManager::AbstractManager& manager, const std::string& plugin): PluginManager::AbstractManagingPlugin<AbstractImporter>{manager, plugin} {}

bool AbstractImporter::openData(Containers::ArrayView<const char> data) {
    CORRADE_ASSERT(features() & Feature::OpenData,
        "Trade::AbstractImporter::openData(): feature not supported", {});

    close();
    doOpenData(data);
    return isOpened();
}

void AbstractImporter::doOpenData(Containers::ArrayView<const char>) {
    CORRADE_ASSERT(false, "Trade::AbstractImporter::openData(): feature advertised but not implemented", );
}

bool AbstractImporter::openFile(const std::string& filename) {
    close();
    doOpenFile(filename);
    return isOpened();
}

void AbstractImporter::doOpenFile(const std::string& filename) {
    CORRADE_ASSERT(features() & Feature::OpenData, "Trade::AbstractImporter::openFile(): not implemented", );

    /* Open file */
    if(!Utility::Directory::fileExists(filename)) {
        Error() << "Trade::AbstractImporter::openFile(): cannot open file" << filename;
        return;
    }

    doOpenData(Utility::Directory::read(filename));
}

void AbstractImporter::close() {
    if(isOpened()) {
        doClose();
        CORRADE_INTERNAL_ASSERT(!isOpened());
    }
}

Int AbstractImporter::defaultScene() {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::defaultScene(): no file opened", -1);
    return doDefaultScene();
}

Int AbstractImporter::doDefaultScene() { return -1; }

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
    CORRADE_ASSERT(id < doSceneCount(), "Trade::AbstractImporter::sceneName(): index out of range", {});
    return doSceneName(id);
}

std::string AbstractImporter::doSceneName(UnsignedInt) { return {}; }

std::optional<SceneData> AbstractImporter::scene(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::scene(): no file opened", {});
    CORRADE_ASSERT(id < doSceneCount(), "Trade::AbstractImporter::scene(): index out of range", {});
    return doScene(id);
}

std::optional<SceneData> AbstractImporter::doScene(UnsignedInt) { return std::nullopt; }

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
    CORRADE_ASSERT(id < doLightCount(), "Trade::AbstractImporter::lightName(): index out of range", {});
    return doLightName(id);
}

std::string AbstractImporter::doLightName(UnsignedInt) { return {}; }

std::optional<LightData> AbstractImporter::light(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::light(): no file opened", {});
    CORRADE_ASSERT(id < doLightCount(), "Trade::AbstractImporter::light(): index out of range", {});
    return doLight(id);
}

std::optional<LightData> AbstractImporter::doLight(UnsignedInt) { return std::nullopt; }

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
    CORRADE_ASSERT(id < doCameraCount(), "Trade::AbstractImporter::cameraName(): index out of range", {});
    return doCameraName(id);
}

std::string AbstractImporter::doCameraName(UnsignedInt) { return {}; }

std::optional<CameraData> AbstractImporter::camera(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::camera(): no file opened", {});
    CORRADE_ASSERT(id < doCameraCount(), "Trade::AbstractImporter::camera(): index out of range", {});
    return doCamera(id);
}

std::optional<CameraData> AbstractImporter::doCamera(UnsignedInt) { return std::nullopt; }

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
    CORRADE_ASSERT(id < doObject2DCount(), "Trade::AbstractImporter::object2DName(): index out of range", {});
    return doObject2DName(id);
}

std::string AbstractImporter::doObject2DName(UnsignedInt) { return {}; }

std::unique_ptr<ObjectData2D> AbstractImporter::object2D(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::object2D(): no file opened", {});
    CORRADE_ASSERT(id < doObject2DCount(), "Trade::AbstractImporter::object2D(): index out of range", {});
    return doObject2D(id);
}

std::unique_ptr<ObjectData2D> AbstractImporter::doObject2D(UnsignedInt) { return nullptr; }

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
    CORRADE_ASSERT(id < doObject3DCount(), "Trade::AbstractImporter::object3DName(): index out of range", {});
    return doObject3DName(id);
}

std::string AbstractImporter::doObject3DName(UnsignedInt) { return {}; }

std::unique_ptr<ObjectData3D> AbstractImporter::object3D(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::object3D(): no file opened", {});
    CORRADE_ASSERT(id < doObject3DCount(), "Trade::AbstractImporter::object3D(): index out of range", {});
    return doObject3D(id);
}

std::unique_ptr<ObjectData3D> AbstractImporter::doObject3D(UnsignedInt) { return nullptr; }

UnsignedInt AbstractImporter::mesh2DCount() const {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::mesh2DCount(): no file opened", {});
    return doMesh2DCount();
}

UnsignedInt AbstractImporter::doMesh2DCount() const { return 0; }

Int AbstractImporter::mesh2DForName(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::mesh2DForName(): no file opened", {});
    return doMesh2DForName(name);
}

Int AbstractImporter::doMesh2DForName(const std::string&) { return -1; }

std::string AbstractImporter::mesh2DName(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::mesh2DName(): no file opened", {});
    CORRADE_ASSERT(id < doMesh2DCount(), "Trade::AbstractImporter::object2DName(): index out of range", {});
    return doMesh2DName(id);
}

std::string AbstractImporter::doMesh2DName(UnsignedInt) { return {}; }

std::optional<MeshData2D> AbstractImporter::mesh2D(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::mesh2D(): no file opened", {});
    CORRADE_ASSERT(id < doMesh2DCount(), "Trade::AbstractImporter::object2D(): index out of range", {});
    return doMesh2D(id);
}

std::optional<MeshData2D> AbstractImporter::doMesh2D(UnsignedInt) { return std::nullopt; }

UnsignedInt AbstractImporter::mesh3DCount() const {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::mesh3DCount(): no file opened", {});
    return doMesh3DCount();
}

UnsignedInt AbstractImporter::doMesh3DCount() const { return 0; }

Int AbstractImporter::mesh3DForName(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::mesh3DForName(): no file opened", {});
    return doMesh3DForName(name);
}

Int AbstractImporter::doMesh3DForName(const std::string&) { return -1; }

std::string AbstractImporter::mesh3DName(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::mesh3DName(): no file opened", {});
    CORRADE_ASSERT(id < doMesh3DCount(), "Trade::AbstractImporter::object3DName(): index out of range", {});
    return doMesh3DName(id);
}

std::string AbstractImporter::doMesh3DName(UnsignedInt) { return {}; }

std::optional<MeshData3D> AbstractImporter::mesh3D(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::mesh3D(): no file opened", {});
    CORRADE_ASSERT(id < doMesh3DCount(), "Trade::AbstractImporter::mesh3D(): index out of range", {});
    return doMesh3D(id);
}

std::optional<MeshData3D> AbstractImporter::doMesh3D(UnsignedInt) { return std::nullopt; }

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
    CORRADE_ASSERT(id < doMaterialCount(), "Trade::AbstractImporter::materialName(): index out of range", {});
    return doMaterialName(id);
}

std::string AbstractImporter::doMaterialName(UnsignedInt) { return {}; }

std::unique_ptr<AbstractMaterialData> AbstractImporter::material(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::material(): no file opened", {});
    CORRADE_ASSERT(id < doMaterialCount(), "Trade::AbstractImporter::material(): index out of range", {});
    return doMaterial(id);
}

std::unique_ptr<AbstractMaterialData> AbstractImporter::doMaterial(UnsignedInt) { return nullptr; }

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
    CORRADE_ASSERT(id < doTextureCount(), "Trade::AbstractImporter::textureName(): index out of range", {});
    return doTextureName(id);
}

std::string AbstractImporter::doTextureName(UnsignedInt) { return {}; }

std::optional<TextureData> AbstractImporter::texture(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::texture(): no file opened", {});
    CORRADE_ASSERT(id < doTextureCount(), "Trade::AbstractImporter::texture(): index out of range", {});
    return doTexture(id);
}

std::optional<TextureData> AbstractImporter::doTexture(UnsignedInt) { return std::nullopt; }

UnsignedInt AbstractImporter::image1DCount() const {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image1DCount(): no file opened", {});
    return doImage1DCount();
}

UnsignedInt AbstractImporter::doImage1DCount() const { return 0; }

Int AbstractImporter::image1DForName(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image1DForName(): no file opened", {});
    return doImage1DForName(name);
}

Int AbstractImporter::doImage1DForName(const std::string&) { return -1; }

std::string AbstractImporter::image1DName(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image1DName(): no file opened", {});
    CORRADE_ASSERT(id < doImage1DCount(), "Trade::AbstractImporter::image1DName(): index out of range", {});
    return doImage1DName(id);
}

std::string AbstractImporter::doImage1DName(UnsignedInt) { return {}; }

std::optional<ImageData1D> AbstractImporter::image1D(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image1D(): no file opened", {});
    CORRADE_ASSERT(id < doImage1DCount(), "Trade::AbstractImporter::image1D(): index out of range", {});
    return doImage1D(id);
}

std::optional<ImageData1D> AbstractImporter::doImage1D(UnsignedInt) { return std::nullopt; }

UnsignedInt AbstractImporter::image2DCount() const {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image2DCount(): no file opened", {});
    return doImage2DCount();
}

UnsignedInt AbstractImporter::doImage2DCount() const { return 0; }

Int AbstractImporter::image2DForName(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image1DForName(): no file opened", {});
    return doImage2DForName(name);
}

Int AbstractImporter::doImage2DForName(const std::string&) { return -1; }

std::string AbstractImporter::image2DName(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image2DName(): no file opened", {});
    CORRADE_ASSERT(id < doImage2DCount(), "Trade::AbstractImporter::image2DName(): index out of range", {});
    return doImage2DName(id);
}

std::string AbstractImporter::doImage2DName(UnsignedInt) { return {}; }

std::optional<ImageData2D> AbstractImporter::image2D(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image2D(): no file opened", {});
    CORRADE_ASSERT(id < doImage2DCount(), "Trade::AbstractImporter::image2D(): index out of range", {});
    return doImage2D(id);
}

std::optional<ImageData2D> AbstractImporter::doImage2D(UnsignedInt) { return std::nullopt; }

UnsignedInt AbstractImporter::image3DCount() const {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image3DCount(): no file opened", {});
    return doImage3DCount();
}

UnsignedInt AbstractImporter::doImage3DCount() const { return 0; }

Int AbstractImporter::image3DForName(const std::string& name) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image3DForName(): no file opened", {});
    return doImage3DForName(name);
}

Int AbstractImporter::doImage3DForName(const std::string&) { return -1; }

std::string AbstractImporter::image3DName(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image3DName(): no file opened", {});
    CORRADE_ASSERT(id < image3DCount(), "Trade::AbstractImporter::image3DName(): index out of range", {});
    return doImage3DName(id);
}

std::string AbstractImporter::doImage3DName(UnsignedInt) { return {}; }

std::optional<ImageData3D> AbstractImporter::image3D(const UnsignedInt id) {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::image3D(): no file opened", {});
    CORRADE_ASSERT(id < doImage3DCount(), "Trade::AbstractImporter::image3D(): index out of range", {});
    return doImage3D(id);
}

std::optional<ImageData3D> AbstractImporter::doImage3D(UnsignedInt) { return std::nullopt; }

const void* AbstractImporter::importerState() const {
    CORRADE_ASSERT(isOpened(), "Trade::AbstractImporter::importerState(): no file opened", {});
    return doImporterState();
}

const void* AbstractImporter::doImporterState() const { return nullptr; }

}}
