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

#include <sstream>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/FileCallback.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/AnimationData.h"
#include "Magnum/Trade/ArrayAllocator.h"
#include "Magnum/Trade/CameraData.h"
#include "Magnum/Trade/ImageData.h"
#include "Magnum/Trade/LightData.h"
#include "Magnum/Trade/MeshData.h"
#include "Magnum/Trade/PhongMaterialData.h"
#include "Magnum/Trade/SceneData.h"
#include "Magnum/Trade/SkinData.h"
#include "Magnum/Trade/TextureData.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Containers/ArrayTuple.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/Algorithms.h>

#define _MAGNUM_NO_DEPRECATED_MESHDATA /* So it doesn't yell here */
#define _MAGNUM_NO_DEPRECATED_OBJECTDATA /* So it doesn't yell here */

#include "Magnum/Trade/MeshData2D.h"
#include "Magnum/Trade/MeshData3D.h"
#include "Magnum/Trade/MeshObjectData2D.h"
#include "Magnum/Trade/MeshObjectData3D.h"
#endif

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct AbstractImporterTest: TestSuite::Tester {
    explicit AbstractImporterTest();

    void construct();
    void constructWithPluginManagerReference();

    void setFlags();
    void setFlagsFileOpened();
    void setFlagsFeatureNotSupported();
    void setFlagsNotImplemented();

    void openData();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void openDataDeprecatedFallback();
    #endif
    void openMemory();
    void openFileAsData();
    void openFileAsDataNotFound();

    void openFileNotImplemented();
    void openDataNotSupported();
    void openDataNotImplemented();
    void openStateNotSupported();
    void openStateNotImplemented();

    void setFileCallback();
    void setFileCallbackTemplate();
    void setFileCallbackTemplateNull();
    void setFileCallbackTemplateConst();
    void setFileCallbackFileOpened();
    void setFileCallbackNotImplemented();
    void setFileCallbackNotSupported();
    void setFileCallbackOpenFileDirectly();
    void setFileCallbackOpenFileThroughBaseImplementation();
    void setFileCallbackOpenFileThroughBaseImplementationFailed();
    void setFileCallbackOpenFileAsData();
    void setFileCallbackOpenFileAsDataFailed();

    void thingCountNotImplemented();
    void thingCountNoFile();
    void thingForNameNotImplemented();
    void thingForNameNoFile();
    void thingByNameNotFound();
    void thingNameNoFile();
    void thingNoFile();

    void defaultScene();
    void defaultSceneNotImplemented();
    void defaultSceneOutOfRange();

    void scene();
    void object();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void sceneDeprecatedFallback2D();
    void sceneDeprecatedFallback3D();
    void sceneDeprecatedFallbackParentless2D();
    void sceneDeprecatedFallbackParentless3D();
    void sceneDeprecatedFallbackTransformless2D();
    void sceneDeprecatedFallbackTransformless3D();
    void sceneDeprecatedFallbackMultiFunctionObjects2D();
    void sceneDeprecatedFallbackMultiFunctionObjects3D();
    void sceneDeprecatedFallbackObjectCountNoScenes();
    void sceneDeprecatedFallbackObjectCountAllSceneImportFailed();
    void sceneDeprecatedFallbackBoth2DAnd3DScene();
    #endif
    void sceneNameNotImplemented();
    void objectNameNotImplemented();
    void sceneForNameOutOfRange();
    void objectForNameOutOfRange();
    void sceneNameOutOfRange();
    void objectNameOutOfRange();
    void sceneNotImplemented();
    void sceneOutOfRange();
    void sceneNonOwningDeleters();
    void sceneCustomDataDeleter();
    void sceneCustomFieldDataDeleter();

    void sceneFieldName();
    void sceneFieldNameNotImplemented();
    void sceneFieldNameNotCustom();

    void animation();
    void animationNameNotImplemented();
    void animationForNameOutOfRange();
    void animationNameOutOfRange();
    void animationNotImplemented();
    void animationOutOfRange();
    void animationNonOwningDeleters();
    void animationGrowableDeleters();
    void animationCustomDataDeleter();
    void animationCustomTrackDeleter();

    void light();
    void lightNameNotImplemented();
    void lightForNameOutOfRange();
    void lightNameOutOfRange();
    void lightNotImplemented();
    void lightOutOfRange();

    void camera();
    void cameraNameNotImplemented();
    void cameraForNameOutOfRange();
    void cameraNameOutOfRange();
    void cameraNotImplemented();
    void cameraOutOfRange();

    #ifdef MAGNUM_BUILD_DEPRECATED
    void object2D();
    void object2DCountNotImplemented();
    void object2DCountNoFile();
    void object2DForNameNotImplemented();
    void object2DForNameNoFile();
    void object2DForNameOutOfRange();
    void object2DByNameNotFound();
    void object2DNameNotImplemented();
    void object2DNameNoFile();
    void object2DNameOutOfRange();
    void object2DNotImplemented();
    void object2DNoFile();
    void object2DOutOfRange();

    void object3D();
    void object3DCountNotImplemented();
    void object3DCountNoFile();
    void object3DForNameNotImplemented();
    void object3DForNameNoFile();
    void object3DForNameOutOfRange();
    void object3DByNameNotFound();
    void object3DNameNotImplemented();
    void object3DNameNoFile();
    void object3DNameOutOfRange();
    void object3DNotImplemented();
    void object3DNoFile();
    void object3DOutOfRange();
    #endif

    void skin2D();
    void skin2DNameNotImplemented();
    void skin2DForNameOutOfRange();
    void skin2DNameOutOfRange();
    void skin2DNotImplemented();
    void skin2DOutOfRange();
    void skin2DNonOwningDeleters();
    void skin2DCustomJointDataDeleter();
    void skin2DCustomInverseBindMatrixDataDeleter();

    void skin3D();
    void skin3DNameNotImplemented();
    void skin3DForNameOutOfRange();
    void skin3DNameOutOfRange();
    void skin3DNotImplemented();
    void skin3DOutOfRange();
    void skin3DNonOwningDeleters();
    void skin3DCustomJointDataDeleter();
    void skin3DCustomInverseBindMatrixDataDeleter();

    void mesh();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void meshDeprecatedFallback();
    #endif
    void meshLevelCountNotImplemented();
    void meshLevelCountOutOfRange();
    void meshLevelCountZero();
    void meshForNameOutOfRange();
    void meshNameNotImplemented();
    void meshNameOutOfRange();
    void meshNotImplemented();
    void meshOutOfRange();
    void meshLevelOutOfRange();
    void meshNonOwningDeleters();
    void meshGrowableDeleters();
    void meshCustomIndexDataDeleter();
    void meshCustomVertexDataDeleter();
    void meshCustomAttributesDeleter();

    void meshAttributeName();
    void meshAttributeNameNotImplemented();
    void meshAttributeNameNotCustom();

    #ifdef MAGNUM_BUILD_DEPRECATED
    void mesh2D();
    void mesh2DCountNotImplemented();
    void mesh2DCountNoFile();
    void mesh2DForNameNotImplemented();
    void mesh2DForNameNoFile();
    void mesh2DNameNotImplemented();
    void mesh2DNameNoFile();
    void mesh2DNameOutOfRange();
    void mesh2DNotImplemented();
    void mesh2DNoFile();
    void mesh2DOutOfRange();

    void mesh3D();
    void mesh3DCountNotImplemented();
    void mesh3DCountNoFile();
    void mesh3DForNameNotImplemented();
    void mesh3DForNameNoFile();
    void mesh3DNameNotImplemented();
    void mesh3DNameNoFile();
    void mesh3DNameOutOfRange();
    void mesh3DNotImplemented();
    void mesh3DNoFile();
    void mesh3DOutOfRange();
    #endif

    void material();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void materialDeprecatedFallback();
    #endif
    void materialForNameOutOfRange();
    void materialNameNotImplemented();
    void materialNameOutOfRange();
    void materialNotImplemented();
    void materialOutOfRange();
    void materialNonOwningDeleters();
    void materialCustomAttributeDataDeleter();
    void materialCustomLayerDataDeleter();

    void texture();
    void textureForNameOutOfRange();
    void textureNameNotImplemented();
    void textureNameOutOfRange();
    void textureNotImplemented();
    void textureOutOfRange();

    void image1D();
    void image1DLevelCountNotImplemented();
    void image1DLevelCountOutOfRange();
    void image1DLevelCountZero();
    void image1DForNameOutOfRange();
    void image1DNameNotImplemented();
    void image1DNameOutOfRange();
    void image1DNotImplemented();
    void image1DOutOfRange();
    void image1DLevelOutOfRange();
    void image1DNonOwningDeleter();
    void image1DGrowableDeleter();
    void image1DCustomDeleter();

    void image2D();
    void image2DLevelCountNotImplemented();
    void image2DLevelCountOutOfRange();
    void image2DLevelCountZero();
    void image2DForNameOutOfRange();
    void image2DNameNotImplemented();
    void image2DNameOutOfRange();
    void image2DNotImplemented();
    void image2DOutOfRange();
    void image2DLevelOutOfRange();
    void image2DNonOwningDeleter();
    void image2DGrowableDeleter();
    void image2DCustomDeleter();

    void image3D();
    void image3DLevelCountNotImplemented();
    void image3DLevelCountOutOfRange();
    void image3DLevelCountZero();
    void image3DForNameOutOfRange();
    void image3DNameNotImplemented();
    void image3DNameOutOfRange();
    void image3DNotImplemented();
    void image3DOutOfRange();
    void image3DLevelOutOfRange();
    void image3DNonOwningDeleter();
    void image3DGrowableDeleter();
    void image3DCustomDeleter();

    void importerState();
    void importerStateNotImplemented();
    void importerStateNoFile();

    void debugFeature();
    void debugFeatures();
    void debugFlag();
    void debugFlags();
};

const struct {
    const char* message;
    ImporterFlag flag;
    ImporterFeatures features;
} SetFlagsFeatureNotSupportedData[] {
    #define _c(name, enumName) {"zero-copy " name, ImporterFlag::ForceZeroCopy ## enumName, ~ImporterFeature::ZeroCopy ## enumName}
    _c("animations", Animations),
    _c("images", Images),
    _c("material attributes", MaterialAttributes),
    _c("material layers", MaterialLayers),
    _c("mesh indices", MeshIndices),
    _c("mesh vertices", MeshVertices),
    _c("skin joints", SkinJoints),
    _c("skin inverse bind matrices", SkinInverseBindMatrices)
    #undef _c
};

constexpr struct {
    const char* name;
    bool checkMessage;
} ThingByNameData[] {
    {"check it's not an assert", false},
    {"verify the message", true}
};

using namespace Math::Literals;

AbstractImporterTest::AbstractImporterTest() {
    addTests({&AbstractImporterTest::construct,
              &AbstractImporterTest::constructWithPluginManagerReference,

              &AbstractImporterTest::setFlags,
              &AbstractImporterTest::setFlagsFileOpened});

    addInstancedTests({&AbstractImporterTest::setFlagsFeatureNotSupported},
        Containers::arraySize(SetFlagsFeatureNotSupportedData));

    addTests({&AbstractImporterTest::setFlagsNotImplemented,

              &AbstractImporterTest::openData,
              #ifdef MAGNUM_BUILD_DEPRECATED
              &AbstractImporterTest::openDataDeprecatedFallback,
              #endif
              &AbstractImporterTest::openMemory,
              &AbstractImporterTest::openFileAsData,
              &AbstractImporterTest::openFileAsDataNotFound,

              &AbstractImporterTest::openFileNotImplemented,
              &AbstractImporterTest::openDataNotSupported,
              &AbstractImporterTest::openDataNotImplemented,
              &AbstractImporterTest::openStateNotSupported,
              &AbstractImporterTest::openStateNotImplemented,

              &AbstractImporterTest::setFileCallback,
              &AbstractImporterTest::setFileCallbackTemplate,
              &AbstractImporterTest::setFileCallbackTemplateNull,
              &AbstractImporterTest::setFileCallbackTemplateConst,
              &AbstractImporterTest::setFileCallbackFileOpened,
              &AbstractImporterTest::setFileCallbackNotImplemented,
              &AbstractImporterTest::setFileCallbackNotSupported,
              &AbstractImporterTest::setFileCallbackOpenFileDirectly,
              &AbstractImporterTest::setFileCallbackOpenFileThroughBaseImplementation,
              &AbstractImporterTest::setFileCallbackOpenFileThroughBaseImplementationFailed,
              &AbstractImporterTest::setFileCallbackOpenFileAsData,
              &AbstractImporterTest::setFileCallbackOpenFileAsDataFailed,

              &AbstractImporterTest::thingCountNotImplemented,
              &AbstractImporterTest::thingCountNoFile,
              &AbstractImporterTest::thingForNameNotImplemented,
              &AbstractImporterTest::thingForNameNoFile});

    addInstancedTests({&AbstractImporterTest::thingByNameNotFound},
        Containers::arraySize(ThingByNameData));

    addTests({&AbstractImporterTest::thingNameNoFile,
              &AbstractImporterTest::thingNoFile,

              &AbstractImporterTest::defaultScene,
              &AbstractImporterTest::defaultSceneOutOfRange,
              &AbstractImporterTest::defaultSceneNotImplemented,

              &AbstractImporterTest::scene,
              &AbstractImporterTest::object,
              #ifdef MAGNUM_BUILD_DEPRECATED
              &AbstractImporterTest::sceneDeprecatedFallback2D,
              &AbstractImporterTest::sceneDeprecatedFallback3D,
              &AbstractImporterTest::sceneDeprecatedFallbackParentless2D,
              &AbstractImporterTest::sceneDeprecatedFallbackParentless3D,
              &AbstractImporterTest::sceneDeprecatedFallbackTransformless2D,
              &AbstractImporterTest::sceneDeprecatedFallbackTransformless3D,
              &AbstractImporterTest::sceneDeprecatedFallbackMultiFunctionObjects2D,
              &AbstractImporterTest::sceneDeprecatedFallbackMultiFunctionObjects3D,
              &AbstractImporterTest::sceneDeprecatedFallbackObjectCountNoScenes,
              &AbstractImporterTest::sceneDeprecatedFallbackObjectCountAllSceneImportFailed,
              &AbstractImporterTest::sceneDeprecatedFallbackBoth2DAnd3DScene,
              #endif
              &AbstractImporterTest::sceneForNameOutOfRange,
              &AbstractImporterTest::objectForNameOutOfRange,
              &AbstractImporterTest::sceneNameNotImplemented,
              &AbstractImporterTest::objectNameNotImplemented,
              &AbstractImporterTest::sceneNameOutOfRange,
              &AbstractImporterTest::objectNameOutOfRange,
              &AbstractImporterTest::sceneNotImplemented,
              &AbstractImporterTest::sceneOutOfRange,
              &AbstractImporterTest::sceneNonOwningDeleters,
              &AbstractImporterTest::sceneCustomDataDeleter,
              &AbstractImporterTest::sceneCustomFieldDataDeleter,

              &AbstractImporterTest::sceneFieldName,
              &AbstractImporterTest::sceneFieldNameNotImplemented,
              &AbstractImporterTest::sceneFieldNameNotCustom,

              &AbstractImporterTest::animation,
              &AbstractImporterTest::animationForNameOutOfRange,
              &AbstractImporterTest::animationNameNotImplemented,
              &AbstractImporterTest::animationNameOutOfRange,
              &AbstractImporterTest::animationNotImplemented,
              &AbstractImporterTest::animationOutOfRange,
              &AbstractImporterTest::animationNonOwningDeleters,
              &AbstractImporterTest::animationGrowableDeleters,
              &AbstractImporterTest::animationCustomDataDeleter,
              &AbstractImporterTest::animationCustomTrackDeleter,

              &AbstractImporterTest::light,
              &AbstractImporterTest::lightForNameOutOfRange,
              &AbstractImporterTest::lightNameNotImplemented,
              &AbstractImporterTest::lightNameOutOfRange,
              &AbstractImporterTest::lightNotImplemented,
              &AbstractImporterTest::lightOutOfRange,

              &AbstractImporterTest::camera,
              &AbstractImporterTest::cameraForNameOutOfRange,
              &AbstractImporterTest::cameraNameNotImplemented,
              &AbstractImporterTest::cameraNameOutOfRange,
              &AbstractImporterTest::cameraNotImplemented,
              &AbstractImporterTest::cameraOutOfRange});

    #ifdef MAGNUM_BUILD_DEPRECATED
    addTests({&AbstractImporterTest::object2D,
              &AbstractImporterTest::object2DCountNotImplemented,
              &AbstractImporterTest::object2DCountNoFile,
              &AbstractImporterTest::object2DForNameNotImplemented,
              &AbstractImporterTest::object2DForNameNoFile,
              &AbstractImporterTest::object2DForNameOutOfRange});

    addInstancedTests({&AbstractImporterTest::object2DByNameNotFound},
        Containers::arraySize(ThingByNameData));

    addTests({&AbstractImporterTest::object2DNameNotImplemented,
              &AbstractImporterTest::object2DNameNoFile,
              &AbstractImporterTest::object2DNameOutOfRange,
              &AbstractImporterTest::object2DNotImplemented,
              &AbstractImporterTest::object2DNoFile,
              &AbstractImporterTest::object2DOutOfRange,

              &AbstractImporterTest::object3D,
              &AbstractImporterTest::object3DCountNotImplemented,
              &AbstractImporterTest::object3DCountNoFile,
              &AbstractImporterTest::object3DForNameNotImplemented,
              &AbstractImporterTest::object3DForNameNoFile,
              &AbstractImporterTest::object3DForNameOutOfRange});

    addInstancedTests({&AbstractImporterTest::object3DByNameNotFound},
        Containers::arraySize(ThingByNameData));

    addTests({&AbstractImporterTest::object3DNameNotImplemented,
              &AbstractImporterTest::object3DNameNoFile,
              &AbstractImporterTest::object3DNameOutOfRange,
              &AbstractImporterTest::object3DNotImplemented,
              &AbstractImporterTest::object3DNoFile,
              &AbstractImporterTest::object3DOutOfRange});
    #endif

    addTests({&AbstractImporterTest::skin2D,
              &AbstractImporterTest::skin2DForNameOutOfRange,
              &AbstractImporterTest::skin2DNameNotImplemented,
              &AbstractImporterTest::skin2DNameOutOfRange,
              &AbstractImporterTest::skin2DNotImplemented,
              &AbstractImporterTest::skin2DOutOfRange,
              &AbstractImporterTest::skin2DNonOwningDeleters,
              &AbstractImporterTest::skin2DCustomJointDataDeleter,
              &AbstractImporterTest::skin2DCustomInverseBindMatrixDataDeleter,

              &AbstractImporterTest::skin3D,
              &AbstractImporterTest::skin3DForNameOutOfRange,
              &AbstractImporterTest::skin3DNameNotImplemented,
              &AbstractImporterTest::skin3DNameOutOfRange,
              &AbstractImporterTest::skin3DNotImplemented,
              &AbstractImporterTest::skin3DOutOfRange,
              &AbstractImporterTest::skin3DNonOwningDeleters,
              &AbstractImporterTest::skin3DCustomJointDataDeleter,
              &AbstractImporterTest::skin3DCustomInverseBindMatrixDataDeleter,

              &AbstractImporterTest::mesh,
              #ifdef MAGNUM_BUILD_DEPRECATED
              &AbstractImporterTest::meshDeprecatedFallback,
              #endif
              &AbstractImporterTest::meshLevelCountNotImplemented,
              &AbstractImporterTest::meshLevelCountOutOfRange,
              &AbstractImporterTest::meshLevelCountZero,
              &AbstractImporterTest::meshForNameOutOfRange,
              &AbstractImporterTest::meshNameNotImplemented,
              &AbstractImporterTest::meshNameOutOfRange,
              &AbstractImporterTest::meshNotImplemented,
              &AbstractImporterTest::meshOutOfRange,
              &AbstractImporterTest::meshLevelOutOfRange,
              &AbstractImporterTest::meshNonOwningDeleters,
              &AbstractImporterTest::meshGrowableDeleters,
              &AbstractImporterTest::meshCustomIndexDataDeleter,
              &AbstractImporterTest::meshCustomVertexDataDeleter,
              &AbstractImporterTest::meshCustomAttributesDeleter,

              &AbstractImporterTest::meshAttributeName,
              &AbstractImporterTest::meshAttributeNameNotImplemented,
              &AbstractImporterTest::meshAttributeNameNotCustom,

              #ifdef MAGNUM_BUILD_DEPRECATED
              &AbstractImporterTest::mesh2D,
              &AbstractImporterTest::mesh2DCountNotImplemented,
              &AbstractImporterTest::mesh2DCountNoFile,
              &AbstractImporterTest::mesh2DForNameNotImplemented,
              &AbstractImporterTest::mesh2DForNameNoFile,
              &AbstractImporterTest::mesh2DNameNotImplemented,
              &AbstractImporterTest::mesh2DNameNoFile,
              &AbstractImporterTest::mesh2DNameOutOfRange,
              &AbstractImporterTest::mesh2DNotImplemented,
              &AbstractImporterTest::mesh2DNoFile,
              &AbstractImporterTest::mesh2DOutOfRange,

              &AbstractImporterTest::mesh3D,
              &AbstractImporterTest::mesh3DCountNotImplemented,
              &AbstractImporterTest::mesh3DCountNoFile,
              &AbstractImporterTest::mesh3DForNameNotImplemented,
              &AbstractImporterTest::mesh3DForNameNoFile,
              &AbstractImporterTest::mesh3DNameNotImplemented,
              &AbstractImporterTest::mesh3DNameNoFile,
              &AbstractImporterTest::mesh3DNameOutOfRange,
              &AbstractImporterTest::mesh3DNotImplemented,
              &AbstractImporterTest::mesh3DNoFile,
              &AbstractImporterTest::mesh3DOutOfRange,
              #endif

              &AbstractImporterTest::material,
              #ifdef MAGNUM_BUILD_DEPRECATED
              &AbstractImporterTest::materialDeprecatedFallback,
              #endif
              &AbstractImporterTest::materialForNameOutOfRange,
              &AbstractImporterTest::materialNameNotImplemented,
              &AbstractImporterTest::materialNameOutOfRange,
              &AbstractImporterTest::materialNotImplemented,
              &AbstractImporterTest::materialOutOfRange,
              &AbstractImporterTest::materialNonOwningDeleters,
              &AbstractImporterTest::materialCustomAttributeDataDeleter,
              &AbstractImporterTest::materialCustomLayerDataDeleter,

              &AbstractImporterTest::texture,
              &AbstractImporterTest::textureForNameOutOfRange,
              &AbstractImporterTest::textureNameNotImplemented,
              &AbstractImporterTest::textureNameOutOfRange,
              &AbstractImporterTest::textureNotImplemented,
              &AbstractImporterTest::textureOutOfRange,

              &AbstractImporterTest::image1D,
              &AbstractImporterTest::image1DLevelCountNotImplemented,
              &AbstractImporterTest::image1DLevelCountOutOfRange,
              &AbstractImporterTest::image1DLevelCountZero,
              &AbstractImporterTest::image1DForNameOutOfRange,
              &AbstractImporterTest::image1DNameNotImplemented,
              &AbstractImporterTest::image1DNameOutOfRange,
              &AbstractImporterTest::image1DNotImplemented,
              &AbstractImporterTest::image1DOutOfRange,
              &AbstractImporterTest::image1DLevelOutOfRange,
              &AbstractImporterTest::image1DNonOwningDeleter,
              &AbstractImporterTest::image1DGrowableDeleter,
              &AbstractImporterTest::image1DCustomDeleter,

              &AbstractImporterTest::image2D,
              &AbstractImporterTest::image2DLevelCountNotImplemented,
              &AbstractImporterTest::image2DLevelCountOutOfRange,
              &AbstractImporterTest::image2DLevelCountZero,
              &AbstractImporterTest::image2DForNameOutOfRange,
              &AbstractImporterTest::image2DNameNotImplemented,
              &AbstractImporterTest::image2DNameOutOfRange,
              &AbstractImporterTest::image2DNotImplemented,
              &AbstractImporterTest::image2DOutOfRange,
              &AbstractImporterTest::image2DLevelOutOfRange,
              &AbstractImporterTest::image2DNonOwningDeleter,
              &AbstractImporterTest::image2DGrowableDeleter,
              &AbstractImporterTest::image2DCustomDeleter,

              &AbstractImporterTest::image3D,
              &AbstractImporterTest::image3DLevelCountNotImplemented,
              &AbstractImporterTest::image3DLevelCountOutOfRange,
              &AbstractImporterTest::image3DLevelCountZero,
              &AbstractImporterTest::image3DForNameOutOfRange,
              &AbstractImporterTest::image3DNameNotImplemented,
              &AbstractImporterTest::image3DNameOutOfRange,
              &AbstractImporterTest::image3DNotImplemented,
              &AbstractImporterTest::image3DOutOfRange,
              &AbstractImporterTest::image3DLevelOutOfRange,
              &AbstractImporterTest::image3DNonOwningDeleter,
              &AbstractImporterTest::image3DGrowableDeleter,
              &AbstractImporterTest::image3DCustomDeleter,

              &AbstractImporterTest::importerState,
              &AbstractImporterTest::importerStateNotImplemented,
              &AbstractImporterTest::importerStateNoFile,

              &AbstractImporterTest::debugFeature,
              &AbstractImporterTest::debugFeatures,
              &AbstractImporterTest::debugFlag,
              &AbstractImporterTest::debugFlags});
}

void AbstractImporterTest::construct() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    CORRADE_COMPARE(importer.features(), ImporterFeatures{});
    CORRADE_VERIFY(!importer.isOpened());

    importer.close();
    CORRADE_VERIFY(!importer.isOpened());
}

void AbstractImporterTest::constructWithPluginManagerReference() {
    PluginManager::Manager<AbstractImporter> manager;

    struct Importer: AbstractImporter {
        explicit Importer(PluginManager::Manager<AbstractImporter>& manager): AbstractImporter{manager} {}

        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer{manager};

    CORRADE_VERIFY(!importer.isOpened());
}

void AbstractImporterTest::setFlags() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        void doSetFlags(ImporterFlags flags) override {
            _flags = flags;
        }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        ImporterFlags _flags;
    } importer;
    CORRADE_COMPARE(importer.flags(), ImporterFlags{});
    CORRADE_COMPARE(importer._flags, ImporterFlags{});

    importer.setFlags(ImporterFlag::Verbose);
    CORRADE_COMPARE(importer.flags(), ImporterFlag::Verbose);
    CORRADE_COMPARE(importer._flags, ImporterFlag::Verbose);

    importer.addFlags(ImporterFlag::ZeroCopy);
    CORRADE_COMPARE(importer.flags(), ImporterFlag::Verbose|ImporterFlag::ZeroCopy);
    CORRADE_COMPARE(importer._flags, ImporterFlag::Verbose|ImporterFlag::ZeroCopy);

    importer.clearFlags(ImporterFlag::Verbose);
    CORRADE_COMPARE(importer.flags(), ImporterFlag::ZeroCopy);
    CORRADE_COMPARE(importer._flags, ImporterFlag::ZeroCopy);
}

void AbstractImporterTest::setFlagsFileOpened() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    importer.setFlags(ImporterFlag::Verbose);
    importer.addFlags(ImporterFlag::Verbose);
    importer.clearFlags(ImporterFlag::Verbose);
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::setFlags(): can't be set while a file is opened\n"
        /* These all call into setFlags(), so the same assert is reused */
        "Trade::AbstractImporter::setFlags(): can't be set while a file is opened\n"
        "Trade::AbstractImporter::setFlags(): can't be set while a file is opened\n");
}

void AbstractImporterTest::setFlagsFeatureNotSupported() {
    auto&& data = SetFlagsFeatureNotSupportedData[testCaseInstanceId()];
    setTestCaseDescription(data.message);

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct Importer: AbstractImporter {
        explicit Importer(ImporterFeatures features): _features{features} {}

        ImporterFeatures doFeatures() const override { return _features; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        ImporterFeatures _features;
    } importer{data.features};

    std::ostringstream out;
    Error redirectError{&out};
    importer.setFlags(data.flag);
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "Trade::AbstractImporter::setFlags(): importer doesn't support {}\n",
        data.message));
}

void AbstractImporterTest::setFlagsNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    CORRADE_COMPARE(importer.flags(), ImporterFlags{});
    importer.setFlags(ImporterFlag::Verbose);
    CORRADE_COMPARE(importer.flags(), ImporterFlag::Verbose);
    /* Should just work, no need to implement the function */
}

void AbstractImporterTest::openData() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return ImporterFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        void doOpenData(Containers::Array<char>&& data, DataFlags dataFlags) override {
            CORRADE_COMPARE_AS(data,
                Containers::arrayView({'\xa5'}),
                TestSuite::Compare::Container);
            CORRADE_COMPARE(dataFlags, DataFlags{});
            /* The array should have a custom no-op deleter */
            CORRADE_VERIFY(data.deleter());
            _opened = true;
        }

        bool _opened = false;
    } importer;

    CORRADE_VERIFY(!importer.isOpened());
    const char a5 = '\xa5';
    CORRADE_VERIFY(importer.openData({&a5, 1}));
    CORRADE_VERIFY(importer.isOpened());

    importer.close();
    CORRADE_VERIFY(!importer.isOpened());
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractImporterTest::openDataDeprecatedFallback() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return ImporterFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        void doOpenData(Containers::ArrayView<const char> data) override {
            CORRADE_COMPARE_AS(data,
                Containers::arrayView({'\xa5'}),
                TestSuite::Compare::Container);
            _opened = true;
        }

        bool _opened = false;
    } importer;

    CORRADE_VERIFY(!importer.isOpened());
    const char a5 = '\xa5';
    CORRADE_VERIFY(importer.openData({&a5, 1}));
    CORRADE_VERIFY(importer.isOpened());

    importer.close();
    CORRADE_VERIFY(!importer.isOpened());
}
#endif

void AbstractImporterTest::openMemory() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return ImporterFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        void doOpenData(Containers::Array<char>&& data, DataFlags dataFlags) override {
            CORRADE_COMPARE_AS(data,
                Containers::arrayView({'\xa5'}),
                TestSuite::Compare::Container);
            CORRADE_COMPARE(dataFlags, DataFlag::ExternallyOwned);
            /* The array should have a custom no-op deleter */
            CORRADE_VERIFY(data.deleter());
            _opened = true;
        }

        bool _opened = false;
    } importer;

    CORRADE_VERIFY(!importer.isOpened());
    const char a5 = '\xa5';
    CORRADE_VERIFY(importer.openMemory({&a5, 1}));
    CORRADE_VERIFY(importer.isOpened());

    importer.close();
    CORRADE_VERIFY(!importer.isOpened());
}

void AbstractImporterTest::openFileAsData() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return ImporterFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        void doOpenData(Containers::Array<char>&& data, DataFlags dataFlags) override {
            CORRADE_COMPARE_AS(data,
                Containers::arrayView({'\xa5'}),
                TestSuite::Compare::Container);
            CORRADE_COMPARE(dataFlags, DataFlag::Owned|DataFlag::Mutable);
            /* I.e., we can take over the array, it's not just a view */
            CORRADE_VERIFY(!data.deleter());
            _opened = true;
        }

        bool _opened = false;
    } importer;

    /* doOpenFile() should call doOpenData() */
    CORRADE_VERIFY(!importer.isOpened());
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(TRADE_TEST_DIR, "file.bin")));
    CORRADE_VERIFY(importer.isOpened());

    importer.close();
    CORRADE_VERIFY(!importer.isOpened());
}

void AbstractImporterTest::openFileAsDataNotFound() {
    struct Importer: AbstractImporter {
        ImporterFeatures doFeatures() const override { return ImporterFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        void doOpenData(Containers::Array<char>&&, DataFlags) override {
            _opened = true;
        }

        bool _opened = false;
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_VERIFY(!importer.openFile("nonexistent.bin"));
    CORRADE_VERIFY(!importer.isOpened());
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::openFile(): cannot open file nonexistent.bin\n");
}

void AbstractImporterTest::openFileNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct Importer: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_VERIFY(!importer.openFile("file.dat"));
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::openFile(): not implemented\n");
}

void AbstractImporterTest::openDataNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_VERIFY(!importer.openData(nullptr));
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::openData(): feature not supported\n");
}

void AbstractImporterTest::openDataNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return ImporterFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_VERIFY(!importer.openData(nullptr));
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::openData(): feature advertised but not implemented\n");
}

void AbstractImporterTest::openStateNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_VERIFY(!importer.openState(nullptr));
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::openState(): feature not supported\n");
}

void AbstractImporterTest::openStateNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return ImporterFeature::OpenState; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_VERIFY(!importer.openState(nullptr));
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::openState(): feature advertised but not implemented\n");
}

void AbstractImporterTest::setFileCallback() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return ImporterFeature::OpenData|ImporterFeature::FileCallback; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
        void doSetFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, void*), void* userData) override {
            *static_cast<int*>(userData) = 1337;
        }
    } importer;

    int a = 0;
    auto lambda = [](const std::string&, InputFileCallbackPolicy, void*) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    };
    importer.setFileCallback(lambda, &a);
    CORRADE_COMPARE(importer.fileCallback(), lambda);
    CORRADE_COMPARE(importer.fileCallbackUserData(), &a);
    CORRADE_COMPARE(a, 1337);
}

void AbstractImporterTest::setFileCallbackTemplate() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return ImporterFeature::OpenData|ImporterFeature::FileCallback; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
        void doSetFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, void*), void*) override {
            called = true;
        }

        bool called = false;
    } importer;

    int a = 0;
    auto lambda = [](const std::string&, InputFileCallbackPolicy, int&) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    };
    importer.setFileCallback(lambda, a);
    CORRADE_VERIFY(importer.fileCallback());
    CORRADE_VERIFY(importer.fileCallbackUserData());
    CORRADE_VERIFY(importer.called);

    /* The data pointers should be wrapped, thus not the same */
    CORRADE_VERIFY(reinterpret_cast<void(*)()>(importer.fileCallback()) != reinterpret_cast<void(*)()>(static_cast<Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, int&)>(lambda)));
    CORRADE_VERIFY(importer.fileCallbackUserData() != &a);
}

void AbstractImporterTest::setFileCallbackTemplateNull() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return ImporterFeature::OpenData|ImporterFeature::FileCallback; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
        void doSetFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*callback)(const std::string&, InputFileCallbackPolicy, void*), void* userData) override {
            CORRADE_VERIFY(!callback);
            CORRADE_VERIFY(!userData);
            called = true;
        }

        bool called = false;
    } importer;

    CORRADE_VERIFY(true); /* to record correct function name */

    int a = 0;
    importer.setFileCallback(static_cast<Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, int&)>(nullptr), a);
    CORRADE_VERIFY(!importer.fileCallback());
    CORRADE_VERIFY(!importer.fileCallbackUserData());
    CORRADE_VERIFY(importer.called);
}

void AbstractImporterTest::setFileCallbackTemplateConst() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return ImporterFeature::OpenData|ImporterFeature::FileCallback; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
        void doSetFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, void*), void*) override {
            called = true;
        }

        bool called = false;
    } importer;

    /* Just verify we can have const parameters */
    const int a = 0;
    auto lambda = [](const std::string&, InputFileCallbackPolicy, const int&) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    };
    importer.setFileCallback(lambda, a);
    CORRADE_VERIFY(importer.fileCallback());
    CORRADE_VERIFY(importer.fileCallbackUserData());
    CORRADE_VERIFY(importer.called);
}

void AbstractImporterTest::setFileCallbackFileOpened() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    importer.setFileCallback([](const std::string&, InputFileCallbackPolicy, void*) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    });
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::setFileCallback(): can't be set while a file is opened\n");
}

void AbstractImporterTest::setFileCallbackNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return ImporterFeature::FileCallback; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    int a;
    auto lambda = [](const std::string&, InputFileCallbackPolicy, void*) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    };
    importer.setFileCallback(lambda, &a);
    CORRADE_COMPARE(importer.fileCallback(), lambda);
    CORRADE_COMPARE(importer.fileCallbackUserData(), &a);
    /* Should just work, no need to implement the function */
}

void AbstractImporterTest::setFileCallbackNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    int a;
    importer.setFileCallback([](const std::string&, InputFileCallbackPolicy, void*) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    }, &a);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::setFileCallback(): importer supports neither loading from data nor via callbacks, callbacks can't be used\n");
}

void AbstractImporterTest::setFileCallbackOpenFileDirectly() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return ImporterFeature::FileCallback|ImporterFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        void doOpenFile(const std::string& filename) override {
            /* Called because FileCallback is supported */
            CORRADE_COMPARE(filename, "file.dat");
            CORRADE_VERIFY(fileCallback());
            CORRADE_VERIFY(fileCallbackUserData());
            _opened = true;
        }

        void doOpenData(Containers::Array<char>&&, DataFlags) override {
            /* Shouldn't be called because FileCallback is supported */
            openDataCalledNotSureWhy = true;
        }

        bool _opened = false;
        bool openDataCalledNotSureWhy = false;
    } importer;

    bool calledNotSureWhy = false;
    importer.setFileCallback([](const std::string&, InputFileCallbackPolicy, bool& calledNotSureWhy) -> Containers::Optional<Containers::ArrayView<const char>> {
        calledNotSureWhy = true;
        return {};
    }, calledNotSureWhy);

    CORRADE_VERIFY(importer.openFile("file.dat"));
    CORRADE_VERIFY(!calledNotSureWhy);
    CORRADE_VERIFY(!importer.openDataCalledNotSureWhy);
}

void AbstractImporterTest::setFileCallbackOpenFileThroughBaseImplementation() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return ImporterFeature::FileCallback|ImporterFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        void doOpenFile(const std::string& filename) override {
            CORRADE_COMPARE(filename, "file.dat");
            CORRADE_VERIFY(fileCallback());
            CORRADE_VERIFY(fileCallbackUserData());
            openFileCalled = true;
            AbstractImporter::doOpenFile(filename);
        }

        void doOpenData(Containers::Array<char>&& data, DataFlags dataFlags) override {
            CORRADE_COMPARE_AS(data,
                Containers::arrayView({'\xb0'}),
                TestSuite::Compare::Container);
            CORRADE_COMPARE(dataFlags, DataFlags{});
            _opened = true;
        }

        bool _opened = false;
        bool openFileCalled = false;
    } importer;

    struct State {
        const char data = '\xb0';
        bool loaded = false;
        bool closed = false;
        bool calledNotSureWhy = false;
    } state;

    importer.setFileCallback([](const std::string& filename, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        if(filename == "file.dat" && policy == InputFileCallbackPolicy::LoadTemporary) {
            state.loaded = true;
            return Containers::arrayView(&state.data, 1);
        }

        if(filename == "file.dat" && policy == InputFileCallbackPolicy::Close) {
            state.closed = true;
            return {};
        }

        state.calledNotSureWhy = true;
        return {};
    }, state);

    CORRADE_VERIFY(importer.openFile("file.dat"));
    CORRADE_VERIFY(importer.openFileCalled);
    CORRADE_VERIFY(state.loaded);
    CORRADE_VERIFY(state.closed);
    CORRADE_VERIFY(!state.calledNotSureWhy);
}

void AbstractImporterTest::setFileCallbackOpenFileThroughBaseImplementationFailed() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return ImporterFeature::FileCallback|ImporterFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        void doOpenFile(const std::string& filename) override {
            openFileCalled = true;
            AbstractImporter::doOpenFile(filename);
        }

        bool openFileCalled = false;
    } importer;

    importer.setFileCallback([](const std::string&, InputFileCallbackPolicy, void*) -> Containers::Optional<Containers::ArrayView<const char>> {
        return {};
    });

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_VERIFY(!importer.openFile("file.dat"));
    CORRADE_VERIFY(importer.openFileCalled);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::openFile(): cannot open file file.dat\n");
}

void AbstractImporterTest::setFileCallbackOpenFileAsData() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return ImporterFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        void doOpenFile(const std::string&) override {
            openFileCalled = true;
        }

        void doOpenData(Containers::Array<char>&& data, DataFlags dataFlags) override {
            CORRADE_COMPARE_AS(data,
                Containers::arrayView({'\xb0'}),
                TestSuite::Compare::Container);
            CORRADE_COMPARE(dataFlags, DataFlags{});
            _opened = true;
        }

        bool _opened = false;
        bool openFileCalled = false;
    } importer;

    struct State {
        const char data = '\xb0';
        bool loaded = false;
        bool closed = false;
        bool calledNotSureWhy = false;
    } state;

    importer.setFileCallback([](const std::string& filename, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        if(filename == "file.dat" && policy == InputFileCallbackPolicy::LoadTemporary) {
            state.loaded = true;
            return Containers::arrayView(&state.data, 1);
        }

        if(filename == "file.dat" && policy == InputFileCallbackPolicy::Close) {
            state.closed = true;
            return {};
        }

        state.calledNotSureWhy = true;
        return {};
    }, state);

    CORRADE_VERIFY(importer.openFile("file.dat"));
    CORRADE_VERIFY(!importer.openFileCalled);
    CORRADE_VERIFY(state.loaded);
    CORRADE_VERIFY(state.closed);
    CORRADE_VERIFY(!state.calledNotSureWhy);
}

void AbstractImporterTest::setFileCallbackOpenFileAsDataFailed() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return ImporterFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        void doOpenFile(const std::string&) override {
            openFileCalled = true;
        }

        bool openFileCalled = false;
    } importer;

    importer.setFileCallback([](const std::string&, InputFileCallbackPolicy, void*) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    });

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_VERIFY(!importer.openFile("file.dat"));
    CORRADE_VERIFY(!importer.openFileCalled);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::openFile(): cannot open file file.dat\n");
}

void AbstractImporterTest::thingCountNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    } importer;

    CORRADE_COMPARE(importer.sceneCount(), 0);
    CORRADE_COMPARE(importer.objectCount(), 0);
    CORRADE_COMPARE(importer.animationCount(), 0);
    CORRADE_COMPARE(importer.lightCount(), 0);
    CORRADE_COMPARE(importer.cameraCount(), 0);

    CORRADE_COMPARE(importer.skin2DCount(), 0);
    CORRADE_COMPARE(importer.skin3DCount(), 0);

    CORRADE_COMPARE(importer.meshCount(), 0);
    CORRADE_COMPARE(importer.materialCount(), 0);
    CORRADE_COMPARE(importer.textureCount(), 0);

    CORRADE_COMPARE(importer.image1DCount(), 0);
    CORRADE_COMPARE(importer.image2DCount(), 0);
    CORRADE_COMPARE(importer.image3DCount(), 0);
}

void AbstractImporterTest::thingCountNoFile() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.sceneCount();
    importer.objectCount();
    importer.animationCount();
    importer.lightCount();
    importer.cameraCount();

    importer.skin2DCount();
    importer.skin3DCount();

    importer.meshCount();
    importer.meshLevelCount(7);
    importer.materialCount();
    importer.textureCount();

    importer.image1DCount();
    importer.image1DLevelCount(7);
    importer.image2DCount();
    importer.image2DLevelCount(7);
    importer.image3DCount();
    importer.image3DLevelCount(7);

    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::sceneCount(): no file opened\n"
        "Trade::AbstractImporter::objectCount(): no file opened\n"
        "Trade::AbstractImporter::animationCount(): no file opened\n"
        "Trade::AbstractImporter::lightCount(): no file opened\n"
        "Trade::AbstractImporter::cameraCount(): no file opened\n"

        "Trade::AbstractImporter::skin2DCount(): no file opened\n"
        "Trade::AbstractImporter::skin3DCount(): no file opened\n"

        "Trade::AbstractImporter::meshCount(): no file opened\n"
        "Trade::AbstractImporter::meshLevelCount(): no file opened\n"
        "Trade::AbstractImporter::materialCount(): no file opened\n"
        "Trade::AbstractImporter::textureCount(): no file opened\n"

        "Trade::AbstractImporter::image1DCount(): no file opened\n"
        "Trade::AbstractImporter::image1DLevelCount(): no file opened\n"
        "Trade::AbstractImporter::image2DCount(): no file opened\n"
        "Trade::AbstractImporter::image2DLevelCount(): no file opened\n"
        "Trade::AbstractImporter::image3DCount(): no file opened\n"
        "Trade::AbstractImporter::image3DLevelCount(): no file opened\n");
}

void AbstractImporterTest::thingForNameNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    } importer;

    CORRADE_COMPARE(importer.sceneForName(""), -1);
    CORRADE_COMPARE(importer.objectForName(""), -1);
    CORRADE_COMPARE(importer.animationForName(""), -1);
    CORRADE_COMPARE(importer.lightForName(""), -1);
    CORRADE_COMPARE(importer.cameraForName(""), -1);

    CORRADE_COMPARE(importer.skin2DForName(""), -1);
    CORRADE_COMPARE(importer.skin3DForName(""), -1);

    CORRADE_COMPARE(importer.meshForName(""), -1);
    CORRADE_COMPARE(importer.materialForName(""), -1);
    CORRADE_COMPARE(importer.textureForName(""), -1);

    CORRADE_COMPARE(importer.image1DForName(""), -1);
    CORRADE_COMPARE(importer.image2DForName(""), -1);
    CORRADE_COMPARE(importer.image3DForName(""), -1);
}

void AbstractImporterTest::thingForNameNoFile() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.sceneForName("");
    importer.objectForName("");
    importer.animationForName("");
    importer.lightForName("");
    importer.cameraForName("");

    importer.skin2DForName("");
    importer.skin3DForName("");

    importer.meshForName("");
    importer.materialForName("");
    importer.textureForName("");

    importer.image1DForName("");
    importer.image2DForName("");
    importer.image3DForName("");

    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::sceneForName(): no file opened\n"
        "Trade::AbstractImporter::objectForName(): no file opened\n"
        "Trade::AbstractImporter::animationForName(): no file opened\n"
        "Trade::AbstractImporter::lightForName(): no file opened\n"
        "Trade::AbstractImporter::cameraForName(): no file opened\n"

        "Trade::AbstractImporter::skin2DForName(): no file opened\n"
        "Trade::AbstractImporter::skin3DForName(): no file opened\n"

        "Trade::AbstractImporter::meshForName(): no file opened\n"
        "Trade::AbstractImporter::materialForName(): no file opened\n"
        "Trade::AbstractImporter::textureForName(): no file opened\n"

        "Trade::AbstractImporter::image1DForName(): no file opened\n"
        "Trade::AbstractImporter::image2DForName(): no file opened\n"
        "Trade::AbstractImporter::image3DForName(): no file opened\n");
}

void AbstractImporterTest::thingByNameNotFound() {
    auto&& data = ThingByNameData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 1; }
        UnsignedInt doAnimationCount() const override { return 2; }
        UnsignedInt doLightCount() const override { return 3; }
        UnsignedInt doCameraCount() const override { return 4; }

        UnsignedInt doSkin2DCount() const override { return 5; }
        UnsignedInt doSkin3DCount() const override { return 6; }

        UnsignedInt doMeshCount() const override { return 7; }
        UnsignedInt doMaterialCount() const override { return 8; }
        UnsignedInt doTextureCount() const override { return 9; }

        UnsignedInt doImage1DCount() const override { return 10; }
        UnsignedInt doImage2DCount() const override { return 11; }
        UnsignedInt doImage3DCount() const override { return 12; }
    } importer;

    std::ostringstream out;
    {
        Containers::Optional<Error> redirectError;
        if(data.checkMessage) redirectError.emplace(&out);

        CORRADE_VERIFY(!importer.scene("foobar"));
        CORRADE_VERIFY(!importer.animation("foobar"));
        CORRADE_VERIFY(!importer.light("foobar"));
        CORRADE_VERIFY(!importer.camera("foobar"));

        CORRADE_VERIFY(!importer.skin2D("foobar"));
        CORRADE_VERIFY(!importer.skin3D("foobar"));

        CORRADE_VERIFY(!importer.mesh("foobar"));
        CORRADE_VERIFY(!importer.material("foobar"));
        CORRADE_VERIFY(!importer.texture("foobar"));

        CORRADE_VERIFY(!importer.image1D("foobar"));
        CORRADE_VERIFY(!importer.image2D("foobar"));
        CORRADE_VERIFY(!importer.image3D("foobar"));
    }

    if(data.checkMessage) {
        CORRADE_COMPARE(out.str(),
            "Trade::AbstractImporter::scene(): scene foobar not found among 1 entries\n"
            "Trade::AbstractImporter::animation(): animation foobar not found among 2 entries\n"
            "Trade::AbstractImporter::light(): light foobar not found among 3 entries\n"
            "Trade::AbstractImporter::camera(): camera foobar not found among 4 entries\n"

            "Trade::AbstractImporter::skin2D(): skin foobar not found among 5 entries\n"
            "Trade::AbstractImporter::skin3D(): skin foobar not found among 6 entries\n"

            "Trade::AbstractImporter::mesh(): mesh foobar not found among 7 entries\n"
            "Trade::AbstractImporter::material(): material foobar not found among 8 entries\n"
            "Trade::AbstractImporter::texture(): texture foobar not found among 9 entries\n"

            "Trade::AbstractImporter::image1D(): image foobar not found among 10 entries\n"
            "Trade::AbstractImporter::image2D(): image foobar not found among 11 entries\n"
            "Trade::AbstractImporter::image3D(): image foobar not found among 12 entries\n");
    }
}

void AbstractImporterTest::thingNameNoFile() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.sceneName(42);
    importer.animationName(42);
    importer.lightName(42);
    importer.cameraName(42);

    importer.skin2DName(42);
    importer.skin3DName(42);

    importer.meshName(42);
    importer.materialName(42);
    importer.textureName(42);

    importer.image1DName(42);
    importer.image2DName(42);
    importer.image3DName(42);

    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::sceneName(): no file opened\n"
        "Trade::AbstractImporter::animationName(): no file opened\n"
        "Trade::AbstractImporter::lightName(): no file opened\n"
        "Trade::AbstractImporter::cameraName(): no file opened\n"

        "Trade::AbstractImporter::skin2DName(): no file opened\n"
        "Trade::AbstractImporter::skin3DName(): no file opened\n"

        "Trade::AbstractImporter::meshName(): no file opened\n"
        "Trade::AbstractImporter::materialName(): no file opened\n"
        "Trade::AbstractImporter::textureName(): no file opened\n"

        "Trade::AbstractImporter::image1DName(): no file opened\n"
        "Trade::AbstractImporter::image2DName(): no file opened\n"
        "Trade::AbstractImporter::image3DName(): no file opened\n");
}

void AbstractImporterTest::thingNoFile() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.defaultScene();
    importer.scene(42);
    importer.scene("foo");
    importer.animation(42);
    importer.animation("foo");
    importer.light(42);
    importer.light("foo");
    importer.camera(42);
    importer.camera("foo");

    importer.skin2D(42);
    importer.skin2D("foo");
    importer.skin3D(42);
    importer.skin3D("foo");

    importer.mesh(42);
    importer.mesh("foo");
    importer.material(42);
    importer.material("foo");
    importer.texture(42);
    importer.texture("foo");

    importer.image1D(42);
    importer.image1D("foo");
    importer.image2D(42);
    importer.image2D("foo");
    importer.image3D(42);
    importer.image3D("foo");

    importer.importerState();

    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::defaultScene(): no file opened\n"
        "Trade::AbstractImporter::scene(): no file opened\n"
        "Trade::AbstractImporter::scene(): no file opened\n"
        "Trade::AbstractImporter::animation(): no file opened\n"
        "Trade::AbstractImporter::animation(): no file opened\n"
        "Trade::AbstractImporter::light(): no file opened\n"
        "Trade::AbstractImporter::light(): no file opened\n"
        "Trade::AbstractImporter::camera(): no file opened\n"
        "Trade::AbstractImporter::camera(): no file opened\n"

        "Trade::AbstractImporter::skin2D(): no file opened\n"
        "Trade::AbstractImporter::skin2D(): no file opened\n"
        "Trade::AbstractImporter::skin3D(): no file opened\n"
        "Trade::AbstractImporter::skin3D(): no file opened\n"

        "Trade::AbstractImporter::mesh(): no file opened\n"
        "Trade::AbstractImporter::mesh(): no file opened\n"
        "Trade::AbstractImporter::material(): no file opened\n"
        "Trade::AbstractImporter::material(): no file opened\n"
        "Trade::AbstractImporter::texture(): no file opened\n"
        "Trade::AbstractImporter::texture(): no file opened\n"

        "Trade::AbstractImporter::image1D(): no file opened\n"
        "Trade::AbstractImporter::image1D(): no file opened\n"
        "Trade::AbstractImporter::image2D(): no file opened\n"
        "Trade::AbstractImporter::image2D(): no file opened\n"
        "Trade::AbstractImporter::image3D(): no file opened\n"
        "Trade::AbstractImporter::image3D(): no file opened\n"

        "Trade::AbstractImporter::importerState(): no file opened\n");
}

void AbstractImporterTest::defaultScene() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 43; }
        Int doDefaultScene() const override { return 42; }
    } importer;

    CORRADE_COMPARE(importer.defaultScene(), 42);
}

void AbstractImporterTest::defaultSceneNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    } importer;

    CORRADE_COMPARE(importer.defaultScene(), -1);
}

void AbstractImporterTest::defaultSceneOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 8; }
        Int doDefaultScene() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    importer.defaultScene();
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::defaultScene(): implementation-returned index 8 out of range for 8 entries\n");
}

int state;

void AbstractImporterTest::scene() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 8; }
        Int doSceneForName(const std::string& name) override {
            if(name == "eighth") return 7;
            return -1;
        }
        std::string doSceneName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            return {};
        }
        Containers::Optional<SceneData> doScene(UnsignedInt id) override {
            if(id == 7)
                return SceneData{SceneMappingType::UnsignedByte, 0, nullptr, {}, &state};
            return SceneData{SceneMappingType::UnsignedByte, 0, nullptr, {}};
        }
    } importer;

    CORRADE_COMPARE(importer.sceneCount(), 8);
    CORRADE_COMPARE(importer.sceneForName("eighth"), 7);
    CORRADE_COMPARE(importer.sceneName(7), "eighth");

    {
        auto data = importer.scene(7);
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data->importerState(), &state);
    } {
        auto data = importer.scene("eighth");
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data->importerState(), &state);
    }
}

void AbstractImporterTest::object() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedLong doObjectCount() const override { return 8; }
        Long doObjectForName(const std::string& name) override {
            if(name == "eighth") return 7;
            return -1;
        }
        std::string doObjectName(UnsignedLong id) override {
            if(id == 7) return "eighth";
            return {};
        }
    } importer;

    CORRADE_COMPARE(importer.objectCount(), 8);
    CORRADE_COMPARE(importer.objectForName("eighth"), 7);
    CORRADE_COMPARE(importer.objectName(7), "eighth");
}

void AbstractImporterTest::sceneForNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 8; }
        Int doSceneForName(const std::string&) override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    importer.sceneForName("");
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::sceneForName(): implementation-returned index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::objectForNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedLong doObjectCount() const override { return 8; }
        Long doObjectForName(const std::string&) override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    importer.objectForName("");
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::objectForName(): implementation-returned index 8 out of range for 8 entries\n");
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractImporterTest::sceneDeprecatedFallback2D() {
    /* Need to test the following combinations:

        - few objects in the root
        - an object with one child, with more than one, with none
        - an object with a mesh and a material
        - an object with a mesh and without a material
        - an object with a mesh and a skin
        - an object with a skin but no mesh
        - an object with a camera
        - an object with TRS transformation
        - an object with TRS transformation and a mesh
        - an object with nothing except parent / transformation
    */

    struct Transform {
        UnsignedInt object;
        Int parent;
        Matrix3 transformation;
    };
    struct Trs {
        UnsignedInt object;
        Vector2 translation;
        Complex rotation;
        Vector2 scaling;
    };
    struct Mesh {
        UnsignedInt object;
        UnsignedShort mesh;
        Short meshMaterial;
    };
    struct Index {
        UnsignedInt object;
        UnsignedInt id;
    };
    struct ImporterState {
        UnsignedInt object;
        const void* importerState;
    };
    Containers::StridedArrayView1D<Transform> transformations;
    Containers::StridedArrayView1D<Trs> trs;
    Containers::StridedArrayView1D<Mesh> meshes;
    Containers::StridedArrayView1D<Index> cameras;
    Containers::StridedArrayView1D<Index> skins;
    Containers::StridedArrayView1D<ImporterState> importerState;
    Containers::Array<char> data = Containers::ArrayTuple{
        {NoInit, 6, transformations},
        {NoInit, 2, trs},
        {NoInit, 2, meshes},
        {NoInit, 1, cameras},
        {NoInit, 2, skins},
        {NoInit, 3, importerState}
    };

    int a, b, c;

    /* Object 3 is in the root, has a camera attached, TRS and children 5 + 4.
       Because of the TRS, the actual transformation gets ignored. Has importer
       state. */
    transformations[0] = {3, -1, Matrix3::rotation(75.0_degf)};
    trs[0] = {3, {0.0f, 3.0f}, Complex::rotation(15.0_degf), Vector2{1.0f}};
    cameras[0] = {3, 15};
    importerState[0] = {3, &a};

    /* Object 5 is a child of object 3, has a skin (which gets ignored by the
       legacy interface) */
    transformations[1] = {5, 3, Matrix3::rotation(-15.0_degf)};
    skins[0] = {5, 226};

    /* Object 1 is a child of object 2 */
    transformations[2] = {1, 2, Matrix3::translation({1.0f, 0.5f})*Matrix3::rotation(15.0_degf)};

    /* Object 2 is in the root, has object 1 as a child but nothing else */
    transformations[3] = {2, -1, {}};

    /* Object 0 is in the root, has a mesh without a material and no children */
    transformations[4] = {0, -1, Matrix3::rotation(30.0_degf)};
    meshes[0] = {0, 33, -1};

    /* Object 4 has TRS also, a mesh with a material and a skin and is a child
       of object 3. The transformation gets ignored again. Has importer
       state. */
    transformations[5] = {4, 3, Matrix3::translation(Vector2::xAxis(5.0f))};
    trs[1] = {4, {}, {}, {1.5f, -0.5f}};
    meshes[1] = {4, 27, 46};
    skins[1] = {4, 72};
    importerState[1] = {4, &b};

    /* Object 6 has neither a transformation nor a parent, only an importer
       state. It should get ignored. */
    importerState[2] = {6, &c};

    struct Importer: AbstractImporter {
        explicit Importer(SceneData&& data): _data{std::move(data)} {}

        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 3; }
        UnsignedLong doObjectCount() const override { return 7; }
        Long doObjectForName(const std::string& name) override {
            if(name == "sixth") return 5;
            return -1;
        }
        std::string doObjectName(UnsignedLong id) override {
            if(id == 5) return "sixth";
            return {};
        }
        Containers::Optional<SceneData> doScene(UnsignedInt id) override {
            /* This one has seven objects, but no fields for them so it should
               get skipped */
            if(id == 0)
                return SceneData{SceneMappingType::UnsignedByte, 7, nullptr, {}};
            /* This one has no objects, so it should get skipped as well
               without even querying any fieldFor() API (as those would
               assert) */
            if(id == 1)
                return SceneData{SceneMappingType::UnsignedShort, 0, nullptr, {}};
            /* This one is the one */
            if(id == 2)
                return SceneData{SceneMappingType::UnsignedInt, 7, {}, _data.data(), sceneFieldDataNonOwningArray(_data.fieldData())};
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        private:
            SceneData _data;
    } importer{SceneData{SceneMappingType::UnsignedInt, 7, std::move(data), {
        SceneFieldData{SceneField::Parent,
            transformations.slice(&Transform::object),
            transformations.slice(&Transform::parent)},
        SceneFieldData{SceneField::Transformation,
            transformations.slice(&Transform::object),
            transformations.slice(&Transform::transformation)},
        SceneFieldData{SceneField::Translation,
            trs.slice(&Trs::object),
            trs.slice(&Trs::translation)},
        SceneFieldData{SceneField::Rotation,
            trs.slice(&Trs::object),
            trs.slice(&Trs::rotation)},
        SceneFieldData{SceneField::Scaling,
            trs.slice(&Trs::object),
            trs.slice(&Trs::scaling)},
        SceneFieldData{SceneField::Mesh,
            meshes.slice(&Mesh::object),
            meshes.slice(&Mesh::mesh)},
        SceneFieldData{SceneField::MeshMaterial,
            meshes.slice(&Mesh::object),
            meshes.slice(&Mesh::meshMaterial)},
        SceneFieldData{SceneField::Camera,
            cameras.slice(&Index::object),
            cameras.slice(&Index::id)},
        SceneFieldData{SceneField::Skin,
            skins.slice(&Index::object),
            skins.slice(&Index::id)},
        SceneFieldData{SceneField::ImporterState,
            importerState.slice(&ImporterState::object), importerState.slice(&ImporterState::importerState)}
    }}};

    CORRADE_COMPARE(importer.sceneCount(), 3);

    Containers::Optional<SceneData> scene = importer.scene(2);
    CORRADE_VERIFY(scene);

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE_AS(scene->children2D(),
        (std::vector<UnsignedInt>{3, 2, 0}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene->children3D(),
        (std::vector<UnsignedInt>{}),
        TestSuite::Compare::Container);

    CORRADE_COMPARE(importer.object2DCount(), 7);
    CORRADE_COMPARE(importer.object2DForName("sixth"), 5);
    CORRADE_COMPARE(importer.object2DName(5), "sixth");

    CORRADE_COMPARE(importer.object3DCount(), 0);
    CORRADE_COMPARE(importer.object3DForName("sixth"), -1);

    {
        Containers::Pointer<ObjectData2D> o = importer.object2D(0);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->importerState(), nullptr);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType2D::Mesh);
        CORRADE_COMPARE(o->instance(), 33);
        CORRADE_COMPARE(o->flags(), ObjectFlags2D{});
        CORRADE_COMPARE(o->transformation(), Matrix3::rotation(30.0_degf));
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{},
            TestSuite::Compare::Container);
        MeshObjectData2D& mo = static_cast<MeshObjectData2D&>(*o);
        CORRADE_COMPARE(mo.material(), -1);
        CORRADE_COMPARE(mo.skin(), -1);
    } {
        Containers::Pointer<ObjectData2D> o = importer.object2D(1);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->importerState(), nullptr);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType2D::Empty);
        CORRADE_COMPARE(o->instance(), -1);
        CORRADE_COMPARE(o->flags(), ObjectFlags2D{});
        CORRADE_COMPARE(o->transformation(), Matrix3::translation({1.0f, 0.5f})*Matrix3::rotation(15.0_degf));
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{},
            TestSuite::Compare::Container);
    } {
        Containers::Pointer<ObjectData2D> o = importer.object2D(2);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->importerState(), nullptr);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType2D::Empty);
        CORRADE_COMPARE(o->instance(), -1);
        CORRADE_COMPARE(o->flags(), ObjectFlags2D{});
        CORRADE_COMPARE(o->transformation(), Matrix3{});
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{1},
            TestSuite::Compare::Container);
    } {
        Containers::Pointer<ObjectData2D> o = importer.object2D(3);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->importerState(), &a);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType2D::Camera);
        CORRADE_COMPARE(o->instance(), 15);
        CORRADE_COMPARE(o->flags(), ObjectFlag2D::HasTranslationRotationScaling);
        CORRADE_COMPARE(o->transformation(), Matrix3::translation({0.0f, 3.0f})*Matrix3::rotation(15.0_degf));
        CORRADE_COMPARE(o->translation(), (Vector2{0.0f, 3.0f}));
        CORRADE_COMPARE(o->rotation(), Complex::rotation(15.0_degf));
        CORRADE_COMPARE(o->scaling(), (Vector2{1.0f}));
        CORRADE_COMPARE_AS(o->children(),
            (std::vector<UnsignedInt>{5, 4}),
            TestSuite::Compare::Container);
    } {
        Containers::Pointer<ObjectData2D> o = importer.object2D(4);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->importerState(), &b);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType2D::Mesh);
        CORRADE_COMPARE(o->instance(), 27);
        CORRADE_COMPARE(o->flags(), ObjectFlag2D::HasTranslationRotationScaling);
        CORRADE_COMPARE(o->transformation(), Matrix3::scaling({1.5f, -0.5f}));
        CORRADE_COMPARE(o->translation(), Vector2{});
        CORRADE_COMPARE(o->rotation(), Complex{});
        CORRADE_COMPARE(o->scaling(), (Vector2{1.5, -0.5f}));
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{},
            TestSuite::Compare::Container);
        MeshObjectData2D& mo = static_cast<MeshObjectData2D&>(*o);
        CORRADE_COMPARE(mo.material(), 46);
        CORRADE_COMPARE(mo.skin(), 72);
    } {
        Containers::Pointer<ObjectData2D> o = importer.object2D("sixth");
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->importerState(), nullptr);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType2D::Empty);
        CORRADE_COMPARE(o->instance(), -1);
        CORRADE_COMPARE(o->flags(), ObjectFlags2D{});
        CORRADE_COMPARE(o->transformation(), Matrix3::rotation(-15.0_degf));
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{},
            TestSuite::Compare::Container);
    } {
        /* This one is not contained in any parent hierarchy, so it fails to
           import */
        std::ostringstream out;
        Error redirectError{&out};
        CORRADE_VERIFY(!importer.object2D(6));
        CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object2D(): object 6 not found in any 2D scene hierarchy\n");
    }
    CORRADE_IGNORE_DEPRECATED_POP
}

void AbstractImporterTest::sceneDeprecatedFallback3D() {
    /* Need to test the following combinations:

        - few objects in the root
        - an object with one child, with more than one, with none
        - an object with a mesh and a material
        - an object with a mesh and without a material
        - an object with a mesh and a skin
        - an object with a skin but no mesh
        - an object with a camera
        - an object with a light
        - an object with TRS transformation
        - an object with TRS transformation and a mesh
        - an object with nothing except parent / transformation
    */

    struct Transform {
        UnsignedInt object;
        Int parent;
        Matrix4 transformation;
    };
    struct Trs {
        UnsignedInt object;
        Vector3 translation;
        Quaternion rotation;
        Vector3 scaling;
    };
    struct Mesh {
        UnsignedInt object;
        UnsignedShort mesh;
        Short meshMaterial;
    };
    struct Index {
        UnsignedInt object;
        UnsignedInt id;
    };
    struct ImporterState {
        UnsignedInt object;
        const void* importerState;
    };
    Containers::StridedArrayView1D<Transform> transformations;
    Containers::StridedArrayView1D<Trs> trs;
    Containers::StridedArrayView1D<Mesh> meshes;
    Containers::StridedArrayView1D<Index> cameras;
    Containers::StridedArrayView1D<Index> lights;
    Containers::StridedArrayView1D<Index> skins;
    Containers::StridedArrayView1D<ImporterState> importerState;
    Containers::Array<char> data = Containers::ArrayTuple{
        {NoInit, 6, transformations},
        {NoInit, 2, trs},
        {NoInit, 2, meshes},
        {NoInit, 1, cameras},
        {NoInit, 1, lights},
        {NoInit, 2, skins},
        {NoInit, 3, importerState}
    };

    int a, b, c;

    /* Object 3 is in the root, has a camera attached, TRS and children 5 + 4.
       Because of the TRS, the actual transformation gets ignored. Has importer
       state. */
    transformations[0] = {3, -1, Matrix4::rotationX(75.0_degf)};
    trs[0] = {3, {0.0f, 0.0f, 3.0f}, Quaternion::rotation(15.0_degf, Vector3::xAxis()), Vector3{1.0f}};
    cameras[0] = {3, 15};
    importerState[0] = {3, &a};

    /* Object 5 is a child of object 3, has a skin (which gets ignored by the
       legacy interface) */
    transformations[1] = {5, 3, Matrix4::rotationY(-15.0_degf)};
    skins[0] = {5, 226};

    /* Object 1 is a child of object 2, has a light. */
    transformations[2] = {1, 2, Matrix4::translation({1.0f, 0.0f, 1.0f})*Matrix4::rotationZ(15.0_degf)};
    lights[0] = {1, 113};

    /* Object 2 is in the root, has object 1 as a child but nothing else */
    transformations[3] = {2, -1, {}};

    /* Object 0 is in the root, has a mesh without a material and no children */
    transformations[4] = {0, -1, Matrix4::rotationX(30.0_degf)};
    meshes[0] = {0, 33, -1};

    /* Object 4 has TRS also, a mesh with a material and a skin and is a child
       of object 3. The transformation gets ignored again. Has importer
       state. */
    transformations[5] = {4, 3, Matrix4::translation(Vector3::xAxis(5.0f))};
    trs[1] = {4, {}, {}, {1.5f, 3.0f, -0.5f}};
    meshes[1] = {4, 27, 46};
    skins[1] = {4, 72};
    importerState[1] = {4, &b};

    /* Object 6 has neither a transformation nor a parent, only an importer
       state. It should get ignored. */
    importerState[2] = {6, &c};

    struct Importer: AbstractImporter {
        explicit Importer(SceneData&& data): _data{std::move(data)} {}

        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 3; }
        UnsignedLong doObjectCount() const override { return 7; }
        Long doObjectForName(const std::string& name) override {
            if(name == "sixth") return 5;
            return -1;
        }
        std::string doObjectName(UnsignedLong id) override {
            if(id == 5) return "sixth";
            return {};
        }
        Containers::Optional<SceneData> doScene(UnsignedInt id) override {
            /* This one has seven objects, but no fields for them so it should
               get skipped */
            if(id == 0)
                return SceneData{SceneMappingType::UnsignedByte, 7, nullptr, {}};
            /* This one has no objects, so it should get skipped as well
               without even querying any fieldFor() API (as those would
               assert) */
            if(id == 1)
                return SceneData{SceneMappingType::UnsignedShort, 0, nullptr, {}};
            /* This one is the one */
            if(id == 2)
                return SceneData{SceneMappingType::UnsignedInt, 7, {}, _data.data(), sceneFieldDataNonOwningArray(_data.fieldData())};
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        private:
            SceneData _data;
    } importer{SceneData{SceneMappingType::UnsignedInt, 7, std::move(data), {
        SceneFieldData{SceneField::Parent,
            transformations.slice(&Transform::object),
            transformations.slice(&Transform::parent)},
        SceneFieldData{SceneField::Transformation,
            transformations.slice(&Transform::object),
            transformations.slice(&Transform::transformation)},
        SceneFieldData{SceneField::Translation,
            trs.slice(&Trs::object),
            trs.slice(&Trs::translation)},
        SceneFieldData{SceneField::Rotation,
            trs.slice(&Trs::object),
            trs.slice(&Trs::rotation)},
        SceneFieldData{SceneField::Scaling,
            trs.slice(&Trs::object),
            trs.slice(&Trs::scaling)},
        SceneFieldData{SceneField::Mesh,
            meshes.slice(&Mesh::object),
            meshes.slice(&Mesh::mesh)},
        SceneFieldData{SceneField::MeshMaterial,
            meshes.slice(&Mesh::object),
            meshes.slice(&Mesh::meshMaterial)},
        SceneFieldData{SceneField::Camera,
            cameras.slice(&Index::object),
            cameras.slice(&Index::id)},
        SceneFieldData{SceneField::Light,
            lights.slice(&Index::object),
            lights.slice(&Index::id)},
        SceneFieldData{SceneField::Skin,
            skins.slice(&Index::object),
            skins.slice(&Index::id)},
        SceneFieldData{SceneField::ImporterState,
            importerState.slice(&ImporterState::object), importerState.slice(&ImporterState::importerState)}
    }}};

    CORRADE_COMPARE(importer.sceneCount(), 3);

    Containers::Optional<SceneData> scene = importer.scene(2);
    CORRADE_VERIFY(scene);

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE_AS(scene->children2D(),
        std::vector<UnsignedInt>{},
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene->children3D(),
        (std::vector<UnsignedInt>{3, 2, 0}),
        TestSuite::Compare::Container);

    CORRADE_COMPARE(importer.object2DCount(), 0);
    CORRADE_COMPARE(importer.object2DForName("sixth"), -1);

    CORRADE_COMPARE(importer.object3DCount(), 7);
    CORRADE_COMPARE(importer.object3DForName("sixth"), 5);
    CORRADE_COMPARE(importer.object3DName(5), "sixth");

    {
        Containers::Pointer<ObjectData3D> o = importer.object3D(0);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->importerState(), nullptr);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType3D::Mesh);
        CORRADE_COMPARE(o->instance(), 33);
        CORRADE_COMPARE(o->flags(), ObjectFlags3D{});
        CORRADE_COMPARE(o->transformation(), Matrix4::rotationX(30.0_degf));
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{},
            TestSuite::Compare::Container);
        MeshObjectData3D& mo = static_cast<MeshObjectData3D&>(*o);
        CORRADE_COMPARE(mo.material(), -1);
        CORRADE_COMPARE(mo.skin(), -1);
    } {
        Containers::Pointer<ObjectData3D> o = importer.object3D(1);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->importerState(), nullptr);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType3D::Light);
        CORRADE_COMPARE(o->instance(), 113);
        CORRADE_COMPARE(o->flags(), ObjectFlags3D{});
        CORRADE_COMPARE(o->transformation(), Matrix4::translation({1.0f, 0.0f, 1.0f})*Matrix4::rotationZ(15.0_degf));
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{},
            TestSuite::Compare::Container);
    } {
        Containers::Pointer<ObjectData3D> o = importer.object3D(2);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->importerState(), nullptr);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType3D::Empty);
        CORRADE_COMPARE(o->instance(), -1);
        CORRADE_COMPARE(o->flags(), ObjectFlags3D{});
        CORRADE_COMPARE(o->transformation(), Matrix4{});
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{1},
            TestSuite::Compare::Container);
    } {
        Containers::Pointer<ObjectData3D> o = importer.object3D(3);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->importerState(), &a);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType3D::Camera);
        CORRADE_COMPARE(o->instance(), 15);
        CORRADE_COMPARE(o->flags(), ObjectFlag3D::HasTranslationRotationScaling);
        CORRADE_COMPARE(o->transformation(), Matrix4::translation({0.0f, 0.0f, 3.0f})*Matrix4::rotationX(15.0_degf));
        CORRADE_COMPARE(o->translation(), (Vector3{0.0f, 0.0f, 3.0f}));
        CORRADE_COMPARE(o->rotation(), Quaternion::rotation(15.0_degf, Vector3::xAxis()));
        CORRADE_COMPARE(o->scaling(), (Vector3{1.0f}));
        CORRADE_COMPARE_AS(o->children(),
            (std::vector<UnsignedInt>{5, 4}),
            TestSuite::Compare::Container);
    } {
        Containers::Pointer<ObjectData3D> o = importer.object3D(4);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->importerState(), &b);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType3D::Mesh);
        CORRADE_COMPARE(o->instance(), 27);
        CORRADE_COMPARE(o->flags(), ObjectFlag3D::HasTranslationRotationScaling);
        CORRADE_COMPARE(o->transformation(), Matrix4::scaling({1.5f, 3.0f, -0.5f}));
        CORRADE_COMPARE(o->translation(), Vector3{});
        CORRADE_COMPARE(o->rotation(), Quaternion{});
        CORRADE_COMPARE(o->scaling(), (Vector3{1.5, 3.0f, -0.5f}));
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{},
            TestSuite::Compare::Container);
        MeshObjectData3D& mo = static_cast<MeshObjectData3D&>(*o);
        CORRADE_COMPARE(mo.material(), 46);
        CORRADE_COMPARE(mo.skin(), 72);
    } {
        Containers::Pointer<ObjectData3D> o = importer.object3D("sixth");
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->importerState(), nullptr);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType3D::Empty);
        CORRADE_COMPARE(o->instance(), -1);
        CORRADE_COMPARE(o->flags(), ObjectFlags3D{});
        CORRADE_COMPARE(o->transformation(), Matrix4::rotationY(-15.0_degf));
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{},
            TestSuite::Compare::Container);
    } {
        /* This one is not contained in any parent hierarchy, so it fails to
           import */
        std::ostringstream out;
        Error redirectError{&out};
        CORRADE_VERIFY(!importer.object3D(6));
        CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object3D(): object 6 not found in any 3D scene hierarchy\n");
    }
    CORRADE_IGNORE_DEPRECATED_POP
}

void AbstractImporterTest::sceneDeprecatedFallbackParentless2D() {
    /* As the Parent field is currently used to distinguish which objects
       belong to which scene, its absence means the objects are advertised, but
       aren't listed as children of any scene, and retrieving them will fail */
    /** @todo adapt when there's a dedicated way to distinguish which objects
        belong to which scene */

    struct Field {
        UnsignedInt object;
        Matrix3 transformation;
    } fields[]{
        {5, Matrix3{}},
        {2, Matrix3{}},
    };

    Containers::StridedArrayView1D<Field> view = fields;

    struct Importer: AbstractImporter {
        explicit Importer(SceneData&& data): _data{std::move(data)} {}

        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 1; }
        UnsignedLong doObjectCount() const override { return 6; }
        Containers::Optional<SceneData> doScene(UnsignedInt) override {
            return SceneData{SceneMappingType::UnsignedInt, 6, {}, _data.data(), sceneFieldDataNonOwningArray(_data.fieldData())};
        }

        private:
            SceneData _data;
    } importer{SceneData{SceneMappingType::UnsignedInt, 6, {}, fields, {
        SceneFieldData{SceneField::Transformation,
            view.slice(&Field::object),
            view.slice(&Field::transformation)}
    }}};

    CORRADE_COMPARE(importer.sceneCount(), 1);
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(importer.object2DCount(), 6);
    CORRADE_COMPARE(importer.object3DCount(), 0);
    CORRADE_IGNORE_DEPRECATED_POP

    Containers::Optional<SceneData> scene = importer.scene(0);
    CORRADE_VERIFY(scene);

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE_AS(scene->children2D(),
        std::vector<UnsignedInt>{},
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene->children3D(),
        (std::vector<UnsignedInt>{}),
        TestSuite::Compare::Container);
    CORRADE_IGNORE_DEPRECATED_POP

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_VERIFY(!importer.object2D(0));
    CORRADE_VERIFY(!importer.object2D(1));
    CORRADE_VERIFY(!importer.object2D(2));
    CORRADE_VERIFY(!importer.object2D(3));
    CORRADE_VERIFY(!importer.object2D(4));
    CORRADE_VERIFY(!importer.object2D(5));
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::object2D(): object 0 not found in any 2D scene hierarchy\n"
        "Trade::AbstractImporter::object2D(): object 1 not found in any 2D scene hierarchy\n"
        "Trade::AbstractImporter::object2D(): object 2 not found in any 2D scene hierarchy\n"
        "Trade::AbstractImporter::object2D(): object 3 not found in any 2D scene hierarchy\n"
        "Trade::AbstractImporter::object2D(): object 4 not found in any 2D scene hierarchy\n"
        "Trade::AbstractImporter::object2D(): object 5 not found in any 2D scene hierarchy\n");
}

void AbstractImporterTest::sceneDeprecatedFallbackParentless3D() {
    /* As the Parent field is currently used to distinguish which objects
       belong to which scene, its absence means the objects are advertised, but
       aren't listed as children of any scene, and retrieving them will fail */
    /** @todo adapt when there's a dedicated way to distinguish which objects
        belong to which scene */

    struct Field {
        UnsignedInt object;
        Matrix4 transformation;
    } fields[]{
        {5, Matrix4{}},
        {2, Matrix4{}},
    };

    Containers::StridedArrayView1D<Field> view = fields;

    struct Importer: AbstractImporter {
        explicit Importer(SceneData&& data): _data{std::move(data)} {}

        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 1; }
        UnsignedLong doObjectCount() const override { return 6; }
        Containers::Optional<SceneData> doScene(UnsignedInt) override {
            return SceneData{SceneMappingType::UnsignedInt, 6, {}, _data.data(), sceneFieldDataNonOwningArray(_data.fieldData())};
        }

        private:
            SceneData _data;
    } importer{SceneData{SceneMappingType::UnsignedInt, 6, {}, fields, {
        SceneFieldData{SceneField::Transformation,
            view.slice(&Field::object),
            view.slice(&Field::transformation)}
    }}};

    CORRADE_COMPARE(importer.sceneCount(), 1);
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(importer.object2DCount(), 0);
    CORRADE_COMPARE(importer.object3DCount(), 6);
    CORRADE_IGNORE_DEPRECATED_POP

    Containers::Optional<SceneData> scene = importer.scene(0);
    CORRADE_VERIFY(scene);

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE_AS(scene->children2D(),
        std::vector<UnsignedInt>{},
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene->children3D(),
        (std::vector<UnsignedInt>{}),
        TestSuite::Compare::Container);
    CORRADE_IGNORE_DEPRECATED_POP

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_VERIFY(!importer.object3D(0));
    CORRADE_VERIFY(!importer.object3D(1));
    CORRADE_VERIFY(!importer.object3D(2));
    CORRADE_VERIFY(!importer.object3D(3));
    CORRADE_VERIFY(!importer.object3D(4));
    CORRADE_VERIFY(!importer.object3D(5));
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::object3D(): object 0 not found in any 3D scene hierarchy\n"
        "Trade::AbstractImporter::object3D(): object 1 not found in any 3D scene hierarchy\n"
        "Trade::AbstractImporter::object3D(): object 2 not found in any 3D scene hierarchy\n"
        "Trade::AbstractImporter::object3D(): object 3 not found in any 3D scene hierarchy\n"
        "Trade::AbstractImporter::object3D(): object 4 not found in any 3D scene hierarchy\n"
        "Trade::AbstractImporter::object3D(): object 5 not found in any 3D scene hierarchy\n");
}

void AbstractImporterTest::sceneDeprecatedFallbackTransformless2D() {
    /* If no transformation field is present, for backwards compatibility we
       assume the objects are 3D -- the only plugin that has a 2D scene is
       PrimitiveImporter and it has the transformation field. */

    struct Field {
        UnsignedInt object;
        Int parent;
    } fields[]{
        {5, -1},
        {2, 5},
        {3, 5},
        {1, -1}
    };

    Containers::StridedArrayView1D<Field> view = fields;

    struct Importer: AbstractImporter {
        explicit Importer(SceneData&& data): _data{std::move(data)} {}

        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 1; }
        UnsignedLong doObjectCount() const override { return 6; }
        Containers::Optional<SceneData> doScene(UnsignedInt) override {
            return SceneData{SceneMappingType::UnsignedInt, 6, {}, _data.data(), sceneFieldDataNonOwningArray(_data.fieldData())};
        }

        private:
            SceneData _data;
    } importer{SceneData{SceneMappingType::UnsignedInt, 6, {}, fields, {
        SceneFieldData{SceneField::Parent,
            view.slice(&Field::object),
            view.slice(&Field::parent)},
        /* Required in order to have the scene recognized as 2D */
        SceneFieldData{SceneField::Transformation, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Matrix3x3, nullptr}
    }}};

    CORRADE_COMPARE(importer.sceneCount(), 1);
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(importer.object2DCount(), 6);
    CORRADE_COMPARE(importer.object3DCount(), 0);
    CORRADE_IGNORE_DEPRECATED_POP

    Containers::Optional<SceneData> scene = importer.scene(0);
    CORRADE_VERIFY(scene);

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE_AS(scene->children2D(),
        (std::vector<UnsignedInt>{5, 1}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene->children3D(),
        std::vector<UnsignedInt>{},
        TestSuite::Compare::Container);

    /* If we have neither a matrix nor a TRS, having an identity TRS is better
       as it's more flexible compared to a matrix */
    {
        Containers::Pointer<ObjectData2D> o = importer.object2D(5);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->importerState(), nullptr);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType2D::Empty);
        CORRADE_COMPARE(o->instance(), -1);
        CORRADE_COMPARE(o->flags(), ObjectFlag2D::HasTranslationRotationScaling);
        CORRADE_COMPARE(o->transformation(), Matrix3{});
        CORRADE_COMPARE_AS(o->children(),
            (std::vector<UnsignedInt>{2, 3}),
            TestSuite::Compare::Container);
    } {
        Containers::Pointer<ObjectData2D> o = importer.object2D(2);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->importerState(), nullptr);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType2D::Empty);
        CORRADE_COMPARE(o->instance(), -1);
        CORRADE_COMPARE(o->flags(), ObjectFlag2D::HasTranslationRotationScaling);
        CORRADE_COMPARE(o->transformation(), Matrix3{});
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{},
            TestSuite::Compare::Container);
    } {
        Containers::Pointer<ObjectData2D> o = importer.object2D(3);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->importerState(), nullptr);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType2D::Empty);
        CORRADE_COMPARE(o->instance(), -1);
        CORRADE_COMPARE(o->flags(), ObjectFlag2D::HasTranslationRotationScaling);
        CORRADE_COMPARE(o->transformation(), Matrix3{});
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{},
            TestSuite::Compare::Container);
    } {
        Containers::Pointer<ObjectData2D> o = importer.object2D(1);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->importerState(), nullptr);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType2D::Empty);
        CORRADE_COMPARE(o->instance(), -1);
        CORRADE_COMPARE(o->flags(), ObjectFlag2D::HasTranslationRotationScaling);
        CORRADE_COMPARE(o->transformation(), Matrix3{});
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{},
            TestSuite::Compare::Container);
    }
    CORRADE_IGNORE_DEPRECATED_POP
}
void AbstractImporterTest::sceneDeprecatedFallbackTransformless3D() {
    /* If no transformation field is present, for backwards compatibility we
       assume the objects are 3D -- the only plugin that has a 2D scene is
       PrimitiveImporter and it has the transformation field. */

    struct Field {
        UnsignedInt object;
        Int parent;
    } fields[]{
        {5, -1},
        {2, 5},
        {3, 5},
        {1, -1}
    };

    Containers::StridedArrayView1D<Field> view = fields;

    struct Importer: AbstractImporter {
        explicit Importer(SceneData&& data): _data{std::move(data)} {}

        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 1; }
        UnsignedLong doObjectCount() const override { return 6; }
        Containers::Optional<SceneData> doScene(UnsignedInt) override {
            return SceneData{SceneMappingType::UnsignedInt, 6, {}, _data.data(), sceneFieldDataNonOwningArray(_data.fieldData())};
        }

        private:
            SceneData _data;
    } importer{SceneData{SceneMappingType::UnsignedInt, 6, {}, fields, {
        SceneFieldData{SceneField::Parent,
            view.slice(&Field::object),
            view.slice(&Field::parent)},
        /* Required in order to have the scene recognized as 3D */
        SceneFieldData{SceneField::Transformation, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Matrix4x4, nullptr}
    }}};

    CORRADE_COMPARE(importer.sceneCount(), 1);
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(importer.object2DCount(), 0);
    CORRADE_COMPARE(importer.object3DCount(), 6);
    CORRADE_IGNORE_DEPRECATED_POP

    Containers::Optional<SceneData> scene = importer.scene(0);
    CORRADE_VERIFY(scene);

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE_AS(scene->children2D(),
        std::vector<UnsignedInt>{},
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene->children3D(),
        (std::vector<UnsignedInt>{5, 1}),
        TestSuite::Compare::Container);

    /* If we have neither a matrix nor a TRS, having an identity TRS is better
       as it's more flexible compared to a matrix */
    {
        Containers::Pointer<ObjectData3D> o = importer.object3D(5);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->importerState(), nullptr);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType3D::Empty);
        CORRADE_COMPARE(o->instance(), -1);
        CORRADE_COMPARE(o->flags(), ObjectFlag3D::HasTranslationRotationScaling);
        CORRADE_COMPARE(o->transformation(), Matrix4{});
        CORRADE_COMPARE_AS(o->children(),
            (std::vector<UnsignedInt>{2, 3}),
            TestSuite::Compare::Container);
    } {
        Containers::Pointer<ObjectData3D> o = importer.object3D(2);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->importerState(), nullptr);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType3D::Empty);
        CORRADE_COMPARE(o->instance(), -1);
        CORRADE_COMPARE(o->flags(), ObjectFlag3D::HasTranslationRotationScaling);
        CORRADE_COMPARE(o->transformation(), Matrix4{});
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{},
            TestSuite::Compare::Container);
    } {
        Containers::Pointer<ObjectData3D> o = importer.object3D(3);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->importerState(), nullptr);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType3D::Empty);
        CORRADE_COMPARE(o->instance(), -1);
        CORRADE_COMPARE(o->flags(), ObjectFlag3D::HasTranslationRotationScaling);
        CORRADE_COMPARE(o->transformation(), Matrix4{});
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{},
            TestSuite::Compare::Container);
    } {
        Containers::Pointer<ObjectData3D> o = importer.object3D(1);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->importerState(), nullptr);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType3D::Empty);
        CORRADE_COMPARE(o->instance(), -1);
        CORRADE_COMPARE(o->flags(), ObjectFlag3D::HasTranslationRotationScaling);
        CORRADE_COMPARE(o->transformation(), Matrix4{});
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{},
            TestSuite::Compare::Container);
    }
    CORRADE_IGNORE_DEPRECATED_POP
}

void AbstractImporterTest::sceneDeprecatedFallbackMultiFunctionObjects2D() {
    /* Mostly just a copy of SceneToolsTest::convertToSingleFunctionObjects()
       except that here we can't use the convenience combining tool so it's
       done by hand */

    struct Parent {
        UnsignedInt object;
        Int parent;
    };
    struct Mesh {
        UnsignedInt object;
        UnsignedInt mesh;
        Int meshMaterial;
    };
    struct Camera {
        UnsignedInt object;
        UnsignedInt camera;
    };
    struct Skin {
        UnsignedInt object;
        UnsignedInt skin;
    };
    Containers::StridedArrayView1D<Parent> parents;
    Containers::StridedArrayView1D<Mesh> meshes;
    Containers::StridedArrayView1D<Camera> cameras;
    Containers::StridedArrayView1D<Skin> skins;
    Containers::Array<char> dataData = Containers::ArrayTuple{
        {NoInit, 5, parents},
        {NoInit, 7, meshes},
        {NoInit, 2, cameras},
        {NoInit, 2, skins},
    };
    Utility::copy({{15, -1}, {21, -1}, {22, 21}, {23, 22}, {1, -1}}, parents);
    Utility::copy({
        {15, 6, 4},
        {23, 1, 0},
        {23, 2, 3},
        {23, 4, 2},
        {1, 7, 2},
        {15, 3, 1},
        {21, 5, -1}
    }, meshes);
    Utility::copy({{22, 1}, {1, 5}}, cameras);
    Utility::copy({{15, 9}, {21, 10}}, skins);

    /* Second scene that also has a duplicate, to verify the newly added object
       IDs don't conflict with each other. A potential downside is that
       multi-primitive nodes shared by multiple scenes get duplicated, but
       that's a smaller problem than two unrelated nodes sharing the same ID
       (and thus having a wrong name, etc). */
    Containers::StridedArrayView1D<Parent> parentsSecondary;
    Containers::StridedArrayView1D<Mesh> meshesSecondary;
    Containers::Array<char> dataDataSecondary = Containers::ArrayTuple{
        {NoInit, 1, parentsSecondary},
        {NoInit, 2, meshesSecondary}
    };
    Utility::copy({{30, -1}}, parentsSecondary);
    Utility::copy({
        {30, 6, 2},
        {30, 1, -1}
    }, meshesSecondary);

    SceneData data{SceneMappingType::UnsignedInt, 32, std::move(dataData), {
        SceneFieldData{SceneField::Parent, parents.slice(&Parent::object), parents.slice(&Parent::parent)},
        SceneFieldData{SceneField::Mesh, meshes.slice(&Mesh::object), meshes.slice(&Mesh::mesh)},
        SceneFieldData{SceneField::MeshMaterial, meshes.slice(&Mesh::object), meshes.slice(&Mesh::meshMaterial)},
        SceneFieldData{SceneField::Camera, cameras.slice(&Camera::object), cameras.slice(&Camera::camera)},
        SceneFieldData{SceneField::Skin, skins.slice(&Skin::object), skins.slice(&Skin::skin)},
        /* Just to disambiguate this as a 2D scene */
        SceneFieldData{SceneField::Transformation, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Matrix3x3, nullptr},
    }};
    SceneData dataSecondary{SceneMappingType::UnsignedInt, 31, std::move(dataDataSecondary), {
        SceneFieldData{SceneField::Parent, parentsSecondary.slice(&Parent::object), parentsSecondary.slice(&Parent::parent)},
        SceneFieldData{SceneField::Mesh, meshesSecondary.slice(&Mesh::object), meshesSecondary.slice(&Mesh::mesh)},
        SceneFieldData{SceneField::MeshMaterial, meshesSecondary.slice(&Mesh::object), meshesSecondary.slice(&Mesh::meshMaterial)},
        /* Just to disambiguate this as a 2D scene */
        SceneFieldData{SceneField::Transformation, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Matrix3x3, nullptr},
    }};

    struct Importer: AbstractImporter {
        explicit Importer(SceneData&& data, SceneData&& dataSecondary): _data{std::move(data)}, _dataSecondary{std::move(dataSecondary)} {}

        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 4; }
        UnsignedLong doObjectCount() const override { return 63; }
        std::string doObjectName(UnsignedLong id) override {
            if(id == 1) return "object 1";
            if(id == 15) return "object 15";
            if(id == 23) return "object 23";
            if(id == 30) return "object 30 from secondary scene";
            if(id == 62) return "last";
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }
        Containers::Optional<SceneData> doScene(UnsignedInt id) override {
            /* This scene should get skipped when querying names as it's not
               2D */
            if(id == 0)
                return SceneData{SceneMappingType::UnsignedByte, 32, nullptr, {}};
            /* This scene should get skipped when querying names as it has too
               little objects */
            if(id == 1)
                return SceneData{SceneMappingType::UnsignedByte, 32, nullptr, {
                    SceneFieldData{SceneField::Transformation, SceneMappingType::UnsignedByte, nullptr, SceneFieldType::Matrix3x3, nullptr}
                }};
            if(id == 2)
                return SceneData{SceneMappingType::UnsignedInt, 32, {}, _data.data(), sceneFieldDataNonOwningArray(_data.fieldData())};
            /* A secondary scene, which should have non-overlapping IDs for the
               newly added objects */
            if(id == 3)
                return SceneData{SceneMappingType::UnsignedInt, 31, {}, _dataSecondary.data(), sceneFieldDataNonOwningArray(_dataSecondary.fieldData())};
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        private:
            SceneData _data, _dataSecondary;
    } importer{std::move(data), std::move(dataSecondary)};

    CORRADE_COMPARE(importer.sceneCount(), 4);

    CORRADE_IGNORE_DEPRECATED_PUSH
    /* Total object count reported by the importer plus four new added for the
       first and one for the second scene */
    CORRADE_COMPARE(importer.object2DCount(), 63 + 4 + 1);
    CORRADE_COMPARE(importer.object3DCount(), 0);

    /* Object name should return parent names for the additional objects */
    CORRADE_COMPARE(importer.object2DName(62), "last");
    CORRADE_COMPARE(importer.object2DName(63), "object 23");
    CORRADE_COMPARE(importer.object2DName(64), "object 23");
    CORRADE_COMPARE(importer.object2DName(65), "object 15");
    CORRADE_COMPARE(importer.object2DName(66), "object 1");
    CORRADE_COMPARE(importer.object2DName(67), "object 30 from secondary scene");
    CORRADE_IGNORE_DEPRECATED_POP

    Containers::Optional<SceneData> scene = importer.scene(2);
    CORRADE_VERIFY(scene);

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE_AS(scene->children2D(),
        (std::vector<UnsignedInt>{15, 21, 1}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene->children3D(),
        std::vector<UnsignedInt>{},
        TestSuite::Compare::Container);

    /* Only 9 objects should exist in total, go in order. Usually the object
       IDs will be contiguous so no such mess as this happens. */
    {
        Containers::Pointer<ObjectData2D> o = importer.object2D(1);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType2D::Mesh);
        CORRADE_COMPARE(o->instance(), 7);
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{66},
            TestSuite::Compare::Container);
        MeshObjectData2D& mo = static_cast<MeshObjectData2D&>(*o);
        CORRADE_COMPARE(mo.material(), 2);
        CORRADE_COMPARE(mo.skin(), -1);
    } {
        Containers::Pointer<ObjectData2D> o = importer.object2D(15);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType2D::Mesh);
        CORRADE_COMPARE(o->instance(), 6);
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{65},
            TestSuite::Compare::Container);
        MeshObjectData2D& mo = static_cast<MeshObjectData2D&>(*o);
        CORRADE_COMPARE(mo.material(), 4);
        CORRADE_COMPARE(mo.skin(), 9);
    } {
        Containers::Pointer<ObjectData2D> o = importer.object2D(21);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType2D::Mesh);
        CORRADE_COMPARE(o->instance(), 5);
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{22},
            TestSuite::Compare::Container);
        MeshObjectData2D& mo = static_cast<MeshObjectData2D&>(*o);
        CORRADE_COMPARE(mo.material(), -1);
        CORRADE_COMPARE(mo.skin(), 10);
    } {
        Containers::Pointer<ObjectData2D> o = importer.object2D(22);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType2D::Camera);
        CORRADE_COMPARE(o->instance(), 1);
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{23},
            TestSuite::Compare::Container);
    } {
        Containers::Pointer<ObjectData2D> o = importer.object2D(23);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType2D::Mesh);
        CORRADE_COMPARE(o->instance(), 1);
        CORRADE_COMPARE_AS(o->children(),
            (std::vector<UnsignedInt>{63, 64}),
            TestSuite::Compare::Container);
        MeshObjectData2D& mo = static_cast<MeshObjectData2D&>(*o);
        CORRADE_COMPARE(mo.material(), 0);
        CORRADE_COMPARE(mo.skin(), -1);
    } {
        Containers::Pointer<ObjectData2D> o = importer.object2D(63);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType2D::Mesh);
        CORRADE_COMPARE(o->instance(), 2);
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{},
            TestSuite::Compare::Container);
        MeshObjectData2D& mo = static_cast<MeshObjectData2D&>(*o);
        CORRADE_COMPARE(mo.material(), 3);
        CORRADE_COMPARE(mo.skin(), -1);
    } {
        Containers::Pointer<ObjectData2D> o = importer.object2D(64);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType2D::Mesh);
        CORRADE_COMPARE(o->instance(), 4);
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{},
            TestSuite::Compare::Container);
        MeshObjectData2D& mo = static_cast<MeshObjectData2D&>(*o);
        CORRADE_COMPARE(mo.material(), 2);
        CORRADE_COMPARE(mo.skin(), -1);
    } {
        Containers::Pointer<ObjectData2D> o = importer.object2D(65);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType2D::Mesh);
        CORRADE_COMPARE(o->instance(), 3);
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{},
            TestSuite::Compare::Container);
        MeshObjectData2D& mo = static_cast<MeshObjectData2D&>(*o);
        CORRADE_COMPARE(mo.material(), 1);
        CORRADE_COMPARE(mo.skin(), 9);
    } {
        Containers::Pointer<ObjectData2D> o = importer.object2D(66);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType2D::Camera);
        CORRADE_COMPARE(o->instance(), 5);
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{},
            TestSuite::Compare::Container);
    }
    CORRADE_IGNORE_DEPRECATED_POP

    Containers::Optional<SceneData> sceneSecondary = importer.scene(3);
    CORRADE_VERIFY(sceneSecondary);

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE_AS(sceneSecondary->children2D(),
        std::vector<UnsignedInt>{30},
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(sceneSecondary->children3D(),
        (std::vector<UnsignedInt>{}),
        TestSuite::Compare::Container);

    /* One additional duplicated object here */
    {
        Containers::Pointer<ObjectData2D> o = importer.object2D(30);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType2D::Mesh);
        CORRADE_COMPARE(o->instance(), 6);
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{67},
            TestSuite::Compare::Container);
        MeshObjectData2D& mo = static_cast<MeshObjectData2D&>(*o);
        CORRADE_COMPARE(mo.material(), 2);
    } {
        Containers::Pointer<ObjectData2D> o = importer.object2D(67);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType2D::Mesh);
        CORRADE_COMPARE(o->instance(), 1);
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{},
            TestSuite::Compare::Container);
        MeshObjectData2D& mo = static_cast<MeshObjectData2D&>(*o);
        CORRADE_COMPARE(mo.material(), -1);
    }
    CORRADE_IGNORE_DEPRECATED_POP
}

void AbstractImporterTest::sceneDeprecatedFallbackMultiFunctionObjects3D() {
    /* Mostly just a copy of SceneToolsTest::convertToSingleFunctionObjects()
       except that here we can't use the convenience combining tool so it's
       done by hand */

    struct Parent {
        UnsignedInt object;
        Int parent;
    };
    struct Mesh {
        UnsignedInt object;
        UnsignedInt mesh;
        Int meshMaterial;
    };
    struct Camera {
        UnsignedInt object;
        UnsignedInt camera;
    };
    struct Skin {
        UnsignedInt object;
        UnsignedInt skin;
    };
    Containers::StridedArrayView1D<Parent> parents;
    Containers::StridedArrayView1D<Mesh> meshes;
    Containers::StridedArrayView1D<Camera> cameras;
    Containers::StridedArrayView1D<Skin> skins;
    Containers::Array<char> dataData = Containers::ArrayTuple{
        {NoInit, 5, parents},
        {NoInit, 7, meshes},
        {NoInit, 2, cameras},
        {NoInit, 2, skins},
    };
    Utility::copy({{15, -1}, {21, -1}, {22, 21}, {23, 22}, {1, -1}}, parents);
    Utility::copy({
        {15, 6, 4},
        {23, 1, 0},
        {23, 2, 3},
        {23, 4, 2},
        {1, 7, 2},
        {15, 3, 1},
        {21, 5, -1}
    }, meshes);
    Utility::copy({{22, 1}, {1, 5}}, cameras);
    Utility::copy({{15, 9}, {21, 10}}, skins);

    /* Second scene that also has a duplicate, to verify the newly added object
       IDs don't conflict with each other. A potential downside is that
       multi-primitive nodes shared by multiple scenes get duplicated, but
       that's a smaller problem than two unrelated nodes sharing the same ID
       (and thus having a wrong name, etc). */
    Containers::StridedArrayView1D<Parent> parentsSecondary;
    Containers::StridedArrayView1D<Mesh> meshesSecondary;
    Containers::Array<char> dataDataSecondary = Containers::ArrayTuple{
        {NoInit, 1, parentsSecondary},
        {NoInit, 2, meshesSecondary}
    };
    Utility::copy({{30, -1}}, parentsSecondary);
    Utility::copy({
        {30, 6, 2},
        {30, 1, -1}
    }, meshesSecondary);

    SceneData data{SceneMappingType::UnsignedInt, 32, std::move(dataData), {
        SceneFieldData{SceneField::Parent, parents.slice(&Parent::object), parents.slice(&Parent::parent)},
        SceneFieldData{SceneField::Mesh, meshes.slice(&Mesh::object), meshes.slice(&Mesh::mesh)},
        SceneFieldData{SceneField::MeshMaterial, meshes.slice(&Mesh::object), meshes.slice(&Mesh::meshMaterial)},
        SceneFieldData{SceneField::Camera, cameras.slice(&Camera::object), cameras.slice(&Camera::camera)},
        SceneFieldData{SceneField::Skin, skins.slice(&Skin::object), skins.slice(&Skin::skin)},
        /* Just to disambiguate this as a 3D scene */
        SceneFieldData{SceneField::Transformation, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Matrix4x4, nullptr},
    }};
    SceneData dataSecondary{SceneMappingType::UnsignedInt, 31, std::move(dataDataSecondary), {
        SceneFieldData{SceneField::Parent, parentsSecondary.slice(&Parent::object), parentsSecondary.slice(&Parent::parent)},
        SceneFieldData{SceneField::Mesh, meshesSecondary.slice(&Mesh::object), meshesSecondary.slice(&Mesh::mesh)},
        SceneFieldData{SceneField::MeshMaterial, meshesSecondary.slice(&Mesh::object), meshesSecondary.slice(&Mesh::meshMaterial)},
        /* Just to disambiguate this as a 3D scene */
        SceneFieldData{SceneField::Transformation, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Matrix4x4, nullptr},
    }};

    struct Importer: AbstractImporter {
        explicit Importer(SceneData&& data, SceneData&& dataSecondary): _data{std::move(data)}, _dataSecondary{std::move(dataSecondary)} {}

        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 4; }
        UnsignedLong doObjectCount() const override { return 63; }
        std::string doObjectName(UnsignedLong id) override {
            if(id == 1) return "object 1";
            if(id == 15) return "object 15";
            if(id == 23) return "object 23";
            if(id == 30) return "object 30 from secondary scene";
            if(id == 62) return "last";
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }
        Containers::Optional<SceneData> doScene(UnsignedInt id) override {
            /* This scene should get skipped when querying names as it's not
               2D */
            if(id == 0)
                return SceneData{SceneMappingType::UnsignedByte, 32, nullptr, {}};
            /* This scene should get skipped when querying names as it has too
               little objects */
            if(id == 1)
                return SceneData{SceneMappingType::UnsignedByte, 32, nullptr, {
                    SceneFieldData{SceneField::Transformation, SceneMappingType::UnsignedByte, nullptr, SceneFieldType::Matrix4x4, nullptr}
                }};
            if(id == 2)
                return SceneData{SceneMappingType::UnsignedInt, 32, {}, _data.data(), sceneFieldDataNonOwningArray(_data.fieldData())};
            /* A secondary scene, which should have non-overlapping IDs for the
               newly added objects */
            if(id == 3)
                return SceneData{SceneMappingType::UnsignedInt, 31, {}, _dataSecondary.data(), sceneFieldDataNonOwningArray(_dataSecondary.fieldData())};
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        private:
            SceneData _data, _dataSecondary;
    } importer{std::move(data), std::move(dataSecondary)};

    CORRADE_COMPARE(importer.sceneCount(), 4);

    CORRADE_IGNORE_DEPRECATED_PUSH
    /* Total object count reported by the importer plus four new added for the
       first and one for the second scene */
    CORRADE_COMPARE(importer.object2DCount(), 0);
    CORRADE_COMPARE(importer.object3DCount(), 63 + 4 + 1);

    /* Object name should return parent names for the additional objects */
    CORRADE_COMPARE(importer.object3DName(62), "last");
    CORRADE_COMPARE(importer.object3DName(63), "object 23");
    CORRADE_COMPARE(importer.object3DName(64), "object 23");
    CORRADE_COMPARE(importer.object3DName(65), "object 15");
    CORRADE_COMPARE(importer.object3DName(66), "object 1");
    CORRADE_COMPARE(importer.object3DName(67), "object 30 from secondary scene");
    CORRADE_IGNORE_DEPRECATED_POP

    Containers::Optional<SceneData> scene = importer.scene(2);
    CORRADE_VERIFY(scene);

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE_AS(scene->children2D(),
        std::vector<UnsignedInt>{},
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene->children3D(),
        (std::vector<UnsignedInt>{15, 21, 1}),
        TestSuite::Compare::Container);

    /* Only 9 objects should exist in total, go in order. Usually the object
       IDs will be contiguous so no such mess as this happens. */
    {
        Containers::Pointer<ObjectData3D> o = importer.object3D(1);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType3D::Mesh);
        CORRADE_COMPARE(o->instance(), 7);
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{66},
            TestSuite::Compare::Container);
        MeshObjectData3D& mo = static_cast<MeshObjectData3D&>(*o);
        CORRADE_COMPARE(mo.material(), 2);
        CORRADE_COMPARE(mo.skin(), -1);
    } {
        Containers::Pointer<ObjectData3D> o = importer.object3D(15);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType3D::Mesh);
        CORRADE_COMPARE(o->instance(), 6);
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{65},
            TestSuite::Compare::Container);
        MeshObjectData3D& mo = static_cast<MeshObjectData3D&>(*o);
        CORRADE_COMPARE(mo.material(), 4);
        CORRADE_COMPARE(mo.skin(), 9);
    } {
        Containers::Pointer<ObjectData3D> o = importer.object3D(21);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType3D::Mesh);
        CORRADE_COMPARE(o->instance(), 5);
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{22},
            TestSuite::Compare::Container);
        MeshObjectData3D& mo = static_cast<MeshObjectData3D&>(*o);
        CORRADE_COMPARE(mo.material(), -1);
        CORRADE_COMPARE(mo.skin(), 10);
    } {
        Containers::Pointer<ObjectData3D> o = importer.object3D(22);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType3D::Camera);
        CORRADE_COMPARE(o->instance(), 1);
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{23},
            TestSuite::Compare::Container);
    } {
        Containers::Pointer<ObjectData3D> o = importer.object3D(23);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType3D::Mesh);
        CORRADE_COMPARE(o->instance(), 1);
        CORRADE_COMPARE_AS(o->children(),
            (std::vector<UnsignedInt>{63, 64}),
            TestSuite::Compare::Container);
        MeshObjectData3D& mo = static_cast<MeshObjectData3D&>(*o);
        CORRADE_COMPARE(mo.material(), 0);
        CORRADE_COMPARE(mo.skin(), -1);
    } {
        Containers::Pointer<ObjectData3D> o = importer.object3D(63);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType3D::Mesh);
        CORRADE_COMPARE(o->instance(), 2);
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{},
            TestSuite::Compare::Container);
        MeshObjectData3D& mo = static_cast<MeshObjectData3D&>(*o);
        CORRADE_COMPARE(mo.material(), 3);
        CORRADE_COMPARE(mo.skin(), -1);
    } {
        Containers::Pointer<ObjectData3D> o = importer.object3D(64);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType3D::Mesh);
        CORRADE_COMPARE(o->instance(), 4);
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{},
            TestSuite::Compare::Container);
        MeshObjectData3D& mo = static_cast<MeshObjectData3D&>(*o);
        CORRADE_COMPARE(mo.material(), 2);
        CORRADE_COMPARE(mo.skin(), -1);
    } {
        Containers::Pointer<ObjectData3D> o = importer.object3D(65);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType3D::Mesh);
        CORRADE_COMPARE(o->instance(), 3);
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{},
            TestSuite::Compare::Container);
        MeshObjectData3D& mo = static_cast<MeshObjectData3D&>(*o);
        CORRADE_COMPARE(mo.material(), 1);
        CORRADE_COMPARE(mo.skin(), 9);
    } {
        Containers::Pointer<ObjectData3D> o = importer.object3D(66);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType3D::Camera);
        CORRADE_COMPARE(o->instance(), 5);
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{},
            TestSuite::Compare::Container);
    }
    CORRADE_IGNORE_DEPRECATED_POP

    Containers::Optional<SceneData> sceneSecondary = importer.scene(3);
    CORRADE_VERIFY(sceneSecondary);

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE_AS(sceneSecondary->children2D(),
        std::vector<UnsignedInt>{},
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(sceneSecondary->children3D(),
        (std::vector<UnsignedInt>{30}),
        TestSuite::Compare::Container);

    /* One additional duplicated object here */
    {
        Containers::Pointer<ObjectData3D> o = importer.object3D(30);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType3D::Mesh);
        CORRADE_COMPARE(o->instance(), 6);
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{67},
            TestSuite::Compare::Container);
        MeshObjectData3D& mo = static_cast<MeshObjectData3D&>(*o);
        CORRADE_COMPARE(mo.material(), 2);
    } {
        Containers::Pointer<ObjectData3D> o = importer.object3D(67);
        CORRADE_VERIFY(o);
        CORRADE_COMPARE(o->instanceType(), ObjectInstanceType3D::Mesh);
        CORRADE_COMPARE(o->instance(), 1);
        CORRADE_COMPARE_AS(o->children(),
            std::vector<UnsignedInt>{},
            TestSuite::Compare::Container);
        MeshObjectData3D& mo = static_cast<MeshObjectData3D&>(*o);
        CORRADE_COMPARE(mo.material(), -1);
    }
    CORRADE_IGNORE_DEPRECATED_POP
}

void AbstractImporterTest::sceneDeprecatedFallbackObjectCountNoScenes() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 0; }
        UnsignedLong doObjectCount() const override { return 27; }
    } importer;

    /* There's no scenes to get data or hierarchy from, so there are no
       2D/3D objects reported even though objectCount() says 27 */
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(importer.object2DCount(), 0);
    CORRADE_COMPARE(importer.object3DCount(), 0);
    CORRADE_IGNORE_DEPRECATED_POP
}

void AbstractImporterTest::sceneDeprecatedFallbackObjectCountAllSceneImportFailed() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 1; }
        UnsignedLong doObjectCount() const override { return 27; }
        Containers::Optional<SceneData> doScene(UnsignedInt) override {
            return {};
        }
    } importer;

    /* There's a scene but it failed to import, assume it was 3D and proxy the
       objectCount() */
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(importer.object2DCount(), 0);
    CORRADE_COMPARE(importer.object3DCount(), 27);
    CORRADE_IGNORE_DEPRECATED_POP
}

void AbstractImporterTest::sceneDeprecatedFallbackBoth2DAnd3DScene() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 2; }
        UnsignedLong doObjectCount() const override { return 7; }
        Long doObjectForName(const std::string& name) override {
            if(name == "sixth") return 5;
            return -1;
        }
        std::string doObjectName(UnsignedLong id) override {
            if(id == 5) return "sixth";
            return {};
        }
        Containers::Optional<SceneData> doScene(UnsignedInt id) override {
            if(id == 0) return SceneData{SceneMappingType::UnsignedInt, 7, nullptr, {
                SceneFieldData{SceneField::Parent, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
                SceneFieldData{SceneField::Translation, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Vector2, nullptr},
            }};
            if(id == 1) return SceneData{SceneMappingType::UnsignedInt, 7, nullptr, {
                SceneFieldData{SceneField::Parent, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
                SceneFieldData{SceneField::Translation, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Vector3, nullptr},
            }};
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }
    } importer;

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(importer.object2DCount(), 7);
    CORRADE_COMPARE(importer.object3DCount(), 7);
    CORRADE_IGNORE_DEPRECATED_POP

    {
        CORRADE_EXPECT_FAIL("No check for whether given object is 2D or 3D is done, so the names are reported for both 2D and 3D objects.");
        CORRADE_IGNORE_DEPRECATED_PUSH
        CORRADE_COMPARE(importer.object2DForName("sixth"), -1);
        CORRADE_COMPARE(importer.object2DName(5), "");
        CORRADE_COMPARE(importer.object3DForName("sixth"), -1);
        CORRADE_COMPARE(importer.object3DName(5), "");
        CORRADE_IGNORE_DEPRECATED_POP
    } {
        /* Just to be sure, verify that the names get really reported for both
           instead of some other weird shit happening */
        CORRADE_IGNORE_DEPRECATED_PUSH
        CORRADE_COMPARE(importer.object2DForName("sixth"), 5);
        CORRADE_COMPARE(importer.object2DName(5), "sixth");
        CORRADE_COMPARE(importer.object3DForName("sixth"), 5);
        CORRADE_COMPARE(importer.object3DName(5), "sixth");
        CORRADE_IGNORE_DEPRECATED_POP
    }
}
#endif

void AbstractImporterTest::sceneNameNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 8; }
    } importer;

    CORRADE_COMPARE(importer.sceneName(7), "");
}

void AbstractImporterTest::objectNameNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedLong doObjectCount() const override { return 8; }
    } importer;

    CORRADE_COMPARE(importer.objectName(7), "");
}

void AbstractImporterTest::sceneNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.sceneName(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::sceneName(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::objectNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedLong doObjectCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.objectName(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::objectName(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::sceneNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.scene(7);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::scene(): not implemented\n");
}

void AbstractImporterTest::sceneOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.scene(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::scene(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::sceneNonOwningDeleters() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 1; }
        Containers::Optional<SceneData> doScene(UnsignedInt) override {
            return SceneData{SceneMappingType::UnsignedInt, 0,
                Containers::Array<char>{data, 1, Implementation::nonOwnedArrayDeleter},
                sceneFieldDataNonOwningArray(fields)};
        }

        char data[1];
        SceneFieldData fields[1]{
            SceneFieldData{SceneField::Parent, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr}
        };
    } importer;

    auto data = importer.scene(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(static_cast<const void*>(data->data()), importer.data);
    CORRADE_COMPARE(static_cast<const void*>(data->fieldData()), importer.fields);
}

void AbstractImporterTest::sceneCustomDataDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 1; }
        Int doSceneForName(const std::string&) override { return 0; }
        Containers::Optional<SceneData> doScene(UnsignedInt) override {
            return SceneData{SceneMappingType::UnsignedInt, 0,
                Containers::Array<char>{data, 1, [](char*, std::size_t) {}},
                {}};
        }

        char data[1];
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.scene(0);
    importer.scene("");
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::scene(): implementation is not allowed to use a custom Array deleter\n"
        "Trade::AbstractImporter::scene(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImporterTest::sceneCustomFieldDataDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 1; }
        Int doSceneForName(const std::string&) override { return 0; }
        Containers::Optional<SceneData> doScene(UnsignedInt) override {
            return SceneData{SceneMappingType::UnsignedInt, 0, nullptr, Containers::Array<SceneFieldData>{&parents, 1, [](SceneFieldData*, std::size_t) {}}};
        }

        SceneFieldData parents{SceneField::Parent, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr};
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.scene(0);
    importer.scene("");
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::scene(): implementation is not allowed to use a custom Array deleter\n"
        "Trade::AbstractImporter::scene(): implementation is not allowed to use a custom Array deleter\n"
    );
}

void AbstractImporterTest::sceneFieldName() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        SceneField doSceneFieldForName(const std::string& name) override {
            if(name == "OctreeCell") return sceneFieldCustom(100037);
            return SceneField{};
        }

        std::string doSceneFieldName(UnsignedInt id) override {
            if(id == 100037) return "OctreeCell";
            return "";
        }
    } importer;

    CORRADE_COMPARE(importer.sceneFieldForName("OctreeCell"), sceneFieldCustom(100037));
    CORRADE_COMPARE(importer.sceneFieldName(sceneFieldCustom(100037)), "OctreeCell");
}

void AbstractImporterTest::sceneFieldNameNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    CORRADE_COMPARE(importer.sceneFieldForName(""), SceneField{});
    CORRADE_COMPARE(importer.sceneFieldName(sceneFieldCustom(100037)), "");
}

void AbstractImporterTest::sceneFieldNameNotCustom() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        SceneField doSceneFieldForName(const std::string&) override {
            return SceneField::Translation;
        }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    importer.sceneFieldForName("OctreeCell");
    importer.sceneFieldName(SceneField::Translation);
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::sceneFieldForName(): implementation-returned Trade::SceneField::Translation is neither custom nor invalid\n"
        "Trade::AbstractImporter::sceneFieldName(): Trade::SceneField::Translation is not custom\n");
}

void AbstractImporterTest::animation() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doAnimationCount() const override { return 8; }
        Int doAnimationForName(const std::string& name) override {
            if(name == "eighth") return 7;
            return -1;
        }
        std::string doAnimationName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            return {};
        }
        Containers::Optional<AnimationData> doAnimation(UnsignedInt id) override {
            /* Verify that initializer list is converted to an array with
               the default deleter and not something disallowed */
            if(id == 7) return AnimationData{nullptr, {
                AnimationTrackData{AnimationTrackType::Vector3,
                    AnimationTrackTargetType::Scaling3D, 0, {}}
                }, &state};
            return AnimationData{{}, {}};
        }
    } importer;

    CORRADE_COMPARE(importer.animationCount(), 8);
    CORRADE_COMPARE(importer.animationForName("eighth"), 7);
    CORRADE_COMPARE(importer.animationName(7), "eighth");

    {
        auto data = importer.animation(7);
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data->importerState(), &state);
    } {
        auto data = importer.animation("eighth");
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data->importerState(), &state);
    }
}

void AbstractImporterTest::animationForNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doAnimationCount() const override { return 8; }
        Int doAnimationForName(const std::string&) override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    importer.animationForName("");
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::animationForName(): implementation-returned index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::animationNameNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doAnimationCount() const override { return 8; }
    } importer;

    CORRADE_COMPARE(importer.animationName(7), "");
}

void AbstractImporterTest::animationNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doAnimationCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.animationName(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::animationName(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::animationNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doAnimationCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.animation(7);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::animation(): not implemented\n");
}

void AbstractImporterTest::animationOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doAnimationCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.animation(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::animation(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::animationNonOwningDeleters() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doAnimationCount() const override { return 1; }
        Containers::Optional<AnimationData> doAnimation(UnsignedInt) override {
            return AnimationData{Containers::Array<char>{data, 1, Implementation::nonOwnedArrayDeleter},
                Containers::Array<AnimationTrackData>{&track, 1,
                Implementation::nonOwnedArrayDeleter}};
        }

        char data[1];
        AnimationTrackData track;
    } importer;

    auto data = importer.animation(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(static_cast<const void*>(data->data()), importer.data);
}

void AbstractImporterTest::animationGrowableDeleters() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doAnimationCount() const override { return 1; }
        Containers::Optional<AnimationData> doAnimation(UnsignedInt) override {
            Containers::Array<char> data;
            Containers::arrayAppend<ArrayAllocator>(data, '\x37');
            return AnimationData{std::move(data), {AnimationTrackData{}}};
        }
    } importer;

    auto data = importer.animation(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->data()[0], '\x37');
}

void AbstractImporterTest::animationCustomDataDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doAnimationCount() const override { return 1; }
        Int doAnimationForName(const std::string&) override { return 0; }
        Containers::Optional<AnimationData> doAnimation(UnsignedInt) override {
            return AnimationData{Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}}, nullptr};
        }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.animation(0);
    importer.animation("");
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::animation(): implementation is not allowed to use a custom Array deleter\n"
        "Trade::AbstractImporter::animation(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImporterTest::animationCustomTrackDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doAnimationCount() const override { return 1; }
        Int doAnimationForName(const std::string&) override { return 0; }
        Containers::Optional<AnimationData> doAnimation(UnsignedInt) override {
            return AnimationData{nullptr, Containers::Array<AnimationTrackData>{nullptr, 0, [](AnimationTrackData*, std::size_t) {}}};
        }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.animation(0);
    importer.animation("");
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::animation(): implementation is not allowed to use a custom Array deleter\n"
        "Trade::AbstractImporter::animation(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImporterTest::light() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doLightCount() const override { return 8; }
        Int doLightForName(const std::string& name) override {
            if(name == "eighth") return 7;
            return -1;
        }
        std::string doLightName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            return {};
        }
        Containers::Optional<LightData> doLight(UnsignedInt id) override {
            if(id == 7) return LightData{{}, {}, {}, &state};
            return {};
        }
    } importer;

    CORRADE_COMPARE(importer.lightCount(), 8);
    CORRADE_COMPARE(importer.lightForName("eighth"), 7);
    CORRADE_COMPARE(importer.lightName(7), "eighth");

    {
        auto data = importer.light(7);
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data->importerState(), &state);
    } {
        auto data = importer.light("eighth");
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data->importerState(), &state);
    }
}

void AbstractImporterTest::lightForNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doLightCount() const override { return 8; }
        Int doLightForName(const std::string&) override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    importer.lightForName("");
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::lightForName(): implementation-returned index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::lightNameNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doLightCount() const override { return 8; }
    } importer;

    CORRADE_COMPARE(importer.lightName(7), "");
}

void AbstractImporterTest::lightNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doLightCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.lightName(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::lightName(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::lightNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doLightCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.light(7);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::light(): not implemented\n");
}

void AbstractImporterTest::lightOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doLightCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.light(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::light(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::camera() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doCameraCount() const override { return 8; }
        Int doCameraForName(const std::string& name) override {
            if(name == "eighth") return 7;
            return -1;
        }
        std::string doCameraName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            return {};
        }
        Containers::Optional<CameraData> doCamera(UnsignedInt id) override {
            if(id == 7) return CameraData{{}, Vector2{}, {}, {}, &state};
            return {};
        }
    } importer;

    CORRADE_COMPARE(importer.cameraCount(), 8);
    CORRADE_COMPARE(importer.cameraForName("eighth"), 7);
    CORRADE_COMPARE(importer.cameraName(7), "eighth");

    {
        auto data = importer.camera(7);
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data->importerState(), &state);
    } {
        auto data = importer.camera("eighth");
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data->importerState(), &state);
    }
}

void AbstractImporterTest::cameraForNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doCameraCount() const override { return 8; }
        Int doCameraForName(const std::string&) override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    importer.cameraForName("");
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::cameraForName(): implementation-returned index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::cameraNameNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doCameraCount() const override { return 8; }
    } importer;

    CORRADE_COMPARE(importer.cameraName(7), "");
}

void AbstractImporterTest::cameraNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doCameraCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.cameraName(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::cameraName(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::cameraNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doCameraCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.camera(7);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::camera(): not implemented\n");
}

void AbstractImporterTest::cameraOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doCameraCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.camera(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::camera(): index 8 out of range for 8 entries\n");
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractImporterTest::object2D() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doObject2DCount() const override { return 8; }
        Int doObject2DForName(const std::string& name) override {
            if(name == "eighth") return 7;
            return -1;
        }
        std::string doObject2DName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            return {};
        }
        CORRADE_IGNORE_DEPRECATED_PUSH
        Containers::Pointer<ObjectData2D> doObject2D(UnsignedInt id) override {
            if(id == 7) return Containers::pointer(new ObjectData2D{{}, {}, &state});
            return {};
        }
        CORRADE_IGNORE_DEPRECATED_POP
    } importer;

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(importer.object2DCount(), 8);
    CORRADE_COMPARE(importer.object2DForName("eighth"), 7);
    CORRADE_COMPARE(importer.object2DName(7), "eighth");

    {
        auto data = importer.object2D(7);
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data->importerState(), &state);
    } {
        auto data = importer.object2D("eighth");
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data->importerState(), &state);
    }
    CORRADE_IGNORE_DEPRECATED_POP
}

void AbstractImporterTest::object2DCountNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    } importer;

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(importer.object2DCount(), 0);
    CORRADE_IGNORE_DEPRECATED_POP
}

void AbstractImporterTest::object2DCountNoFile() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.object2DCount();
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object2DCount(): no file opened\n");
}

void AbstractImporterTest::object2DForNameNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    } importer;

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(importer.object2DForName(""), -1);
    CORRADE_IGNORE_DEPRECATED_POP
}

void AbstractImporterTest::object2DForNameNoFile() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.object2DForName("");
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object2DForName(): no file opened\n");
}

void AbstractImporterTest::object2DByNameNotFound() {
    auto&& data = ThingByNameData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doObject2DCount() const override { return 5; }
    } importer;

    std::ostringstream out;
    {
        Containers::Optional<Error> redirectError;
        if(data.checkMessage) redirectError.emplace(&out);

        CORRADE_IGNORE_DEPRECATED_PUSH
        CORRADE_VERIFY(!importer.object2D("foobar"));
        CORRADE_IGNORE_DEPRECATED_POP
    }

    if(data.checkMessage) CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::object2D(): object foobar not found among 5 entries\n");
}

void AbstractImporterTest::object2DForNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doObject2DCount() const override { return 8; }
        Int doObject2DForName(const std::string&) override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.object2DForName("");
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object2DForName(): implementation-returned index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::object2DNameNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doObject2DCount() const override { return 8; }
    } importer;

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(importer.object2DName(7), "");
    CORRADE_IGNORE_DEPRECATED_POP
}

void AbstractImporterTest::object2DNameNoFile() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.object2DName(42);
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object2DName(): no file opened\n");
}

void AbstractImporterTest::object2DNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doObject2DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.object2DName(8);
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object2DName(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::object2DNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 1; }
        UnsignedInt doObject2DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.object2D(7);
    CORRADE_IGNORE_DEPRECATED_POP
    /* It delegates to scene(), but since the assert is graceful and returns a
       null optional, it errors out immediately after */
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::scene(): not implemented\n"
        "Trade::AbstractImporter::object2D(): object 7 not found in any 2D scene hierarchy\n");
}

void AbstractImporterTest::object2DNoFile() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.object2D(42);
    importer.object2D("foo");
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::object2D(): no file opened\n"
        "Trade::AbstractImporter::object2D(): no file opened\n");
}

void AbstractImporterTest::object2DOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doObject2DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.object2D(8);
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object2D(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::object3D() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doObject3DCount() const override { return 8; }
        Int doObject3DForName(const std::string& name) override {
            if(name == "eighth") return 7;
            return -1;
        }
        std::string doObject3DName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            return {};
        }
        CORRADE_IGNORE_DEPRECATED_PUSH
        Containers::Pointer<ObjectData3D> doObject3D(UnsignedInt id) override {
            if(id == 7) return Containers::pointer(new ObjectData3D{{}, {}, &state});
            return {};
        }
        CORRADE_IGNORE_DEPRECATED_POP
    } importer;

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(importer.object3DCount(), 8);
    CORRADE_COMPARE(importer.object3DForName("eighth"), 7);
    CORRADE_COMPARE(importer.object3DName(7), "eighth");

    {
        auto data = importer.object3D(7);
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data->importerState(), &state);
    } {
        auto data = importer.object3D("eighth");
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data->importerState(), &state);
    }
    CORRADE_IGNORE_DEPRECATED_POP
}

void AbstractImporterTest::object3DCountNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    } importer;

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(importer.object3DCount(), 0);
    CORRADE_IGNORE_DEPRECATED_POP
}

void AbstractImporterTest::object3DCountNoFile() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.object3DCount();
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object3DCount(): no file opened\n");
}

void AbstractImporterTest::object3DForNameNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    } importer;

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(importer.object3DForName(""), -1);
    CORRADE_IGNORE_DEPRECATED_POP
}

void AbstractImporterTest::object3DForNameNoFile() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.object2DForName("");
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object2DForName(): no file opened\n");
}

void AbstractImporterTest::object3DByNameNotFound() {
    auto&& data = ThingByNameData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doObject3DCount() const override { return 6; }
    } importer;

    std::ostringstream out;
    {
        Containers::Optional<Error> redirectError;
        if(data.checkMessage) redirectError.emplace(&out);

        CORRADE_IGNORE_DEPRECATED_PUSH
        CORRADE_VERIFY(!importer.object3D("foobar"));
        CORRADE_IGNORE_DEPRECATED_POP
    }

    if(data.checkMessage) CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::object3D(): object foobar not found among 6 entries\n");
}

void AbstractImporterTest::object3DForNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doObject3DCount() const override { return 8; }
        Int doObject3DForName(const std::string&) override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.object3DForName("");
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object3DForName(): implementation-returned index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::object3DNameNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doObject3DCount() const override { return 8; }
    } importer;

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(importer.object3DName(7), "");
    CORRADE_IGNORE_DEPRECATED_POP
}

void AbstractImporterTest::object3DNameNoFile() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.object3DName(42);
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object3DName(): no file opened\n");
}

void AbstractImporterTest::object3DNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doObject3DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.object3DName(8);
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object3DName(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::object3DNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 1; }
        UnsignedInt doObject3DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.object3D(7);
    CORRADE_IGNORE_DEPRECATED_POP
    /* It delegates to scene(), but since the assert is graceful and returns a
       null optional, it errors out immediately after */
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::scene(): not implemented\n"
        "Trade::AbstractImporter::object3D(): object 7 not found in any 3D scene hierarchy\n");
}

void AbstractImporterTest::object3DNoFile() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.object3D(42);
    importer.object3D("foo");
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::object3D(): no file opened\n"
        "Trade::AbstractImporter::object3D(): no file opened\n");
}

void AbstractImporterTest::object3DOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doObject3DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.object3D(8);
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object3D(): index 8 out of range for 8 entries\n");
}
#endif

void AbstractImporterTest::skin2D() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSkin2DCount() const override { return 8; }
        Int doSkin2DForName(const std::string& name) override {
            if(name == "eighth") return 7;
            return -1;
        }
        std::string doSkin2DName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            return {};
        }
        Containers::Optional<SkinData2D> doSkin2D(UnsignedInt id) override {
            /* Verify that initializer list is converted to an array with
               the default deleter and not something disallowed */
            if(id == 7) return SkinData2D{{1}, {{}}, &state};
            return {};
        }
    } importer;

    CORRADE_COMPARE(importer.skin2DCount(), 8);
    CORRADE_COMPARE(importer.skin2DForName("eighth"), 7);
    CORRADE_COMPARE(importer.skin2DName(7), "eighth");

    {
        auto data = importer.skin2D(7);
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data->importerState(), &state);
    } {
        auto data = importer.skin2D("eighth");
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data->importerState(), &state);
    }
}

void AbstractImporterTest::skin2DForNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSkin2DCount() const override { return 8; }
        Int doSkin2DForName(const std::string&) override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    importer.skin2DForName("");
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::skin2DForName(): implementation-returned index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::skin2DNameNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSkin2DCount() const override { return 8; }
    } importer;

    CORRADE_COMPARE(importer.skin2DName(7), "");
}

void AbstractImporterTest::skin2DNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSkin2DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.skin2DName(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::skin2DName(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::skin2DNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSkin2DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.skin2D(7);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::skin2D(): not implemented\n");
}

void AbstractImporterTest::skin2DOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSkin2DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.skin2D(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::skin2D(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::skin2DNonOwningDeleters() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSkin2DCount() const override { return 1; }
        Int doSkin2DForName(const std::string&) override { return 0; }
        Containers::Optional<SkinData2D> doSkin2D(UnsignedInt) override {
            return SkinData2D{{}, jointData, {}, inverseBindMatrixData};
        }

        UnsignedInt jointData[1]{};
        Matrix3 inverseBindMatrixData[1]{};
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    auto data = importer.skin2D(0);
    CORRADE_COMPARE(data->joints(), importer.jointData);
    CORRADE_COMPARE(data->inverseBindMatrices(), importer.inverseBindMatrixData);
}

void AbstractImporterTest::skin2DCustomJointDataDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSkin2DCount() const override { return 1; }
        Int doSkin2DForName(const std::string&) override { return 0; }
        Containers::Optional<SkinData2D> doSkin2D(UnsignedInt) override {
            return SkinData2D{Containers::Array<UnsignedInt>{jointData, 1, [](UnsignedInt*, std::size_t){}}, Containers::Array<Matrix3>{1}};
        }

        UnsignedInt jointData[1]{};
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.skin2D(0);
    importer.skin2D("");
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::skin2D(): implementation is not allowed to use a custom Array deleter\n"
        "Trade::AbstractImporter::skin2D(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImporterTest::skin2DCustomInverseBindMatrixDataDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSkin2DCount() const override { return 1; }
        Int doSkin2DForName(const std::string&) override { return 0; }
        Containers::Optional<SkinData2D> doSkin2D(UnsignedInt) override {
            return SkinData2D{Containers::Array<UnsignedInt>{1}, Containers::Array<Matrix3>{inverseBindMatrixData, 1, [](Matrix3*, std::size_t){}}};
        }

        Matrix3 inverseBindMatrixData[1]{};
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.skin2D(0);
    importer.skin2D("");
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::skin2D(): implementation is not allowed to use a custom Array deleter\n"
        "Trade::AbstractImporter::skin2D(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImporterTest::skin3D() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSkin3DCount() const override { return 8; }
        Int doSkin3DForName(const std::string& name) override {
            if(name == "eighth") return 7;
            return -1;
        }
        std::string doSkin3DName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            return {};
        }
        Containers::Optional<SkinData3D> doSkin3D(UnsignedInt id) override {
            /* Verify that initializer list is converted to an array with
               the default deleter and not something disallowed */
            if(id == 7) return SkinData3D{{1}, {{}}, &state};
            return {};
        }
    } importer;

    CORRADE_COMPARE(importer.skin3DCount(), 8);
    CORRADE_COMPARE(importer.skin3DForName("eighth"), 7);
    CORRADE_COMPARE(importer.skin3DName(7), "eighth");

    {
        auto data = importer.skin3D(7);
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data->importerState(), &state);
    } {
        auto data = importer.skin3D("eighth");
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data->importerState(), &state);
    }
}

void AbstractImporterTest::skin3DForNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSkin3DCount() const override { return 8; }
        Int doSkin3DForName(const std::string&) override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.skin3DForName("");
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::skin3DForName(): implementation-returned index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::skin3DNameNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSkin3DCount() const override { return 8; }
    } importer;

    CORRADE_COMPARE(importer.skin3DName(7), "");
}

void AbstractImporterTest::skin3DNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSkin3DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.skin3DName(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::skin3DName(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::skin3DNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSkin3DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.skin3D(7);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::skin3D(): not implemented\n");
}

void AbstractImporterTest::skin3DOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSkin3DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.skin3D(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::skin3D(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::skin3DNonOwningDeleters() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSkin3DCount() const override { return 1; }
        Int doSkin3DForName(const std::string&) override { return 0; }
        Containers::Optional<SkinData3D> doSkin3D(UnsignedInt) override {
            return SkinData3D{{}, jointData, {}, inverseBindMatrixData};
        }

        UnsignedInt jointData[1]{};
        Matrix4 inverseBindMatrixData[1]{};
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    auto data = importer.skin3D(0);
    CORRADE_COMPARE(data->joints(), importer.jointData);
    CORRADE_COMPARE(data->inverseBindMatrices(), importer.inverseBindMatrixData);
}

void AbstractImporterTest::skin3DCustomJointDataDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSkin3DCount() const override { return 1; }
        Int doSkin3DForName(const std::string&) override { return 0; }
        Containers::Optional<SkinData3D> doSkin3D(UnsignedInt) override {
            return SkinData3D{Containers::Array<UnsignedInt>{jointData, 1, [](UnsignedInt*, std::size_t){}}, Containers::Array<Matrix4>{1}};
        }

        UnsignedInt jointData[1]{};
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.skin3D(0);
    importer.skin3D("");
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::skin3D(): implementation is not allowed to use a custom Array deleter\n"
        "Trade::AbstractImporter::skin3D(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImporterTest::skin3DCustomInverseBindMatrixDataDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSkin3DCount() const override { return 1; }
        Int doSkin3DForName(const std::string&) override { return 0; }
        Containers::Optional<SkinData3D> doSkin3D(UnsignedInt) override {
            return SkinData3D{Containers::Array<UnsignedInt>{1}, Containers::Array<Matrix4>{inverseBindMatrixData, 1, [](Matrix4*, std::size_t){}}};
        }

        Matrix4 inverseBindMatrixData[1]{};
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.skin3D(0);
    importer.skin3D("");
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::skin3D(): implementation is not allowed to use a custom Array deleter\n"
        "Trade::AbstractImporter::skin3D(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImporterTest::mesh() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMeshCount() const override { return 8; }
        UnsignedInt doMeshLevelCount(UnsignedInt id) override {
            if(id == 7) return 3;
            return {};
        }
        Int doMeshForName(const std::string& name) override {
            if(name == "eighth") return 7;
            return -1;
        }
        std::string doMeshName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            return {};
        }
        Containers::Optional<MeshData> doMesh(UnsignedInt id, UnsignedInt level) override {
            /* Verify that initializer list is converted to an array with
               the default deleter and not something disallowed */
            if(id == 7 && level == 2) return MeshData{MeshPrimitive::Points, nullptr, {MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector3, nullptr}}, MeshData::ImplicitVertexCount, &state};
            return {};
        }
    } importer;

    CORRADE_COMPARE(importer.meshCount(), 8);
    CORRADE_COMPARE(importer.meshForName("eighth"), 7);
    CORRADE_COMPARE(importer.meshName(7), "eighth");

    {
        auto data = importer.mesh(7, 2);
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data->importerState(), &state);
    } {
        auto data = importer.mesh("eighth", 2);
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data->importerState(), &state);
    }
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractImporterTest::meshDeprecatedFallback() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMeshCount() const override { return 8; }
        Int doMeshForName(const std::string& name) override {
            if(name == "eighth") return 7;
            return -1;
        }
        std::string doMeshName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            return {};
        }
        Containers::Optional<MeshData> doMesh(UnsignedInt id, UnsignedInt level) override {
            if(id == 7 && level == 0) return MeshData{MeshPrimitive::Points, nullptr, {MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector3, nullptr}}, MeshData::ImplicitVertexCount, &state};
            return {};
        }
    } importer;

    CORRADE_IGNORE_DEPRECATED_PUSH
    /* Nothing done for 2D as there were no known importers for these */
    CORRADE_COMPARE(importer.mesh2DCount(), 0);
    CORRADE_COMPARE(importer.mesh2DForName("eighth"), -1);

    /* For 3D it's called through */
    CORRADE_COMPARE(importer.mesh3DCount(), 8);
    CORRADE_COMPARE(importer.mesh3DForName("eighth"), 7);
    CORRADE_COMPARE(importer.mesh3DName(7), "eighth");

    auto data = importer.mesh3D(7);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->importerState(), &state);
    CORRADE_IGNORE_DEPRECATED_POP
}
#endif

void AbstractImporterTest::meshLevelCountNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMeshCount() const override { return 8; }
    } importer;

    CORRADE_COMPARE(importer.meshLevelCount(7), 1);
}

void AbstractImporterTest::meshLevelCountOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMeshCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    importer.meshLevelCount(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::meshLevelCount(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::meshLevelCountZero() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMeshCount() const override { return 8; }
        Int doMeshForName(const std::string&) override { return 0; }
        UnsignedInt doMeshLevelCount(UnsignedInt) override { return 0; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    importer.meshLevelCount(7);
    /* This should print a similar message instead of a confusing
       "level 1 out of range for 0 entries" */
    importer.mesh(7, 1);
    importer.mesh("", 1);
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::meshLevelCount(): implementation reported zero levels\n"
        "Trade::AbstractImporter::mesh(): implementation reported zero levels\n"
        "Trade::AbstractImporter::mesh(): implementation reported zero levels\n");
}

void AbstractImporterTest::meshForNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMeshCount() const override { return 8; }
        Int doMeshForName(const std::string&) override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    importer.meshForName("");
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::meshForName(): implementation-returned index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::meshNameNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMeshCount() const override { return 8; }
    } importer;

    CORRADE_COMPARE(importer.meshName(7), "");
}

void AbstractImporterTest::meshNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMeshCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.meshName(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::meshName(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::meshNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMeshCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.mesh(7);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh(): not implemented\n");
}

void AbstractImporterTest::meshOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMeshCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.mesh(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::meshLevelOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMeshCount() const override { return 8; }
        Int doMeshForName(const std::string&) override { return 0; }
        UnsignedInt doMeshLevelCount(UnsignedInt) override { return 3; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    importer.mesh(7, 3);
    importer.mesh("", 3);
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::mesh(): level 3 out of range for 3 entries\n"
        "Trade::AbstractImporter::mesh(): level 3 out of range for 3 entries\n");
}

void AbstractImporterTest::meshNonOwningDeleters() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMeshCount() const override { return 1; }
        Containers::Optional<MeshData> doMesh(UnsignedInt, UnsignedInt) override {
            return MeshData{MeshPrimitive::Triangles,
                Containers::Array<char>{indexData, 1, Implementation::nonOwnedArrayDeleter}, MeshIndexData{MeshIndexType::UnsignedByte, indexData},
                Containers::Array<char>{nullptr, 0, Implementation::nonOwnedArrayDeleter},
                meshAttributeDataNonOwningArray(attributes)};
        }

        char indexData[1];
        MeshAttributeData attributes[1]{
            MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector3, nullptr}
        };
    } importer;

    auto data = importer.mesh(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(static_cast<const void*>(data->indexData()), importer.indexData);
}

void AbstractImporterTest::meshGrowableDeleters() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMeshCount() const override { return 1; }
        Containers::Optional<MeshData> doMesh(UnsignedInt, UnsignedInt) override {
            Containers::Array<char> indexData;
            Containers::arrayAppend<ArrayAllocator>(indexData, '\xab');
            Containers::Array<Vector3> vertexData;
            Containers::arrayAppend<ArrayAllocator>(vertexData, Vector3{});
            MeshIndexData indices{MeshIndexType::UnsignedByte, indexData};
            MeshAttributeData positions{MeshAttribute::Position, Containers::arrayView(vertexData)};

            return MeshData{MeshPrimitive::Triangles,
                std::move(indexData), indices,
                Containers::arrayAllocatorCast<char, ArrayAllocator>(std::move(vertexData)), {positions}};
        }
    } importer;

    auto data = importer.mesh(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->indexData()[0], '\xab');
    CORRADE_COMPARE(data->vertexData().size(), 12);
}

void AbstractImporterTest::meshCustomIndexDataDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMeshCount() const override { return 1; }
        Int doMeshForName(const std::string&) override { return 0; }
        Containers::Optional<MeshData> doMesh(UnsignedInt, UnsignedInt) override {
            return MeshData{MeshPrimitive::Triangles, Containers::Array<char>{data, 1, [](char*, std::size_t) {}}, MeshIndexData{MeshIndexType::UnsignedByte, data}, 1};
        }

        char data[1];
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.mesh(0);
    importer.mesh("");
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::mesh(): implementation is not allowed to use a custom Array deleter\n"
        "Trade::AbstractImporter::mesh(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImporterTest::meshCustomVertexDataDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMeshCount() const override { return 1; }
        Int doMeshForName(const std::string&) override { return 0; }
        Containers::Optional<MeshData> doMesh(UnsignedInt, UnsignedInt) override {
            return MeshData{MeshPrimitive::Triangles, Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}}, {MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector3, nullptr}}};
        }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.mesh(0);
    importer.mesh("");
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::mesh(): implementation is not allowed to use a custom Array deleter\n"
        "Trade::AbstractImporter::mesh(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImporterTest::meshCustomAttributesDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMeshCount() const override { return 1; }
        Int doMeshForName(const std::string&) override { return 0; }
        Containers::Optional<MeshData> doMesh(UnsignedInt, UnsignedInt) override {
            return MeshData{MeshPrimitive::Triangles, nullptr, Containers::Array<MeshAttributeData>{&positions, 1, [](MeshAttributeData*, std::size_t) {}}};
        }

        MeshAttributeData positions{MeshAttribute::Position, VertexFormat::Vector3, nullptr};
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.mesh(0);
    importer.mesh("");
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::mesh(): implementation is not allowed to use a custom Array deleter\n"
        "Trade::AbstractImporter::mesh(): implementation is not allowed to use a custom Array deleter\n"
    );
}

void AbstractImporterTest::meshAttributeName() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        MeshAttribute doMeshAttributeForName(const std::string& name) override {
            if(name == "SMOOTH_GROUP_ID") return meshAttributeCustom(37);
            return MeshAttribute{};
        }

        std::string doMeshAttributeName(UnsignedShort id) override {
            if(id == 37) return "SMOOTH_GROUP_ID";
            return "";
        }
    } importer;

    CORRADE_COMPARE(importer.meshAttributeForName("SMOOTH_GROUP_ID"), meshAttributeCustom(37));
    CORRADE_COMPARE(importer.meshAttributeName(meshAttributeCustom(37)), "SMOOTH_GROUP_ID");
}

void AbstractImporterTest::meshAttributeNameNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    CORRADE_COMPARE(importer.meshAttributeForName(""), MeshAttribute{});
    CORRADE_COMPARE(importer.meshAttributeName(meshAttributeCustom(37)), "");
}

void AbstractImporterTest::meshAttributeNameNotCustom() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        MeshAttribute doMeshAttributeForName(const std::string&) override {
            return MeshAttribute::Position;
        }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    importer.meshAttributeForName("SMOOTH_GROUP_ID");
    importer.meshAttributeName(MeshAttribute::Position);
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::meshAttributeForName(): implementation-returned Trade::MeshAttribute::Position is neither custom nor invalid\n"
        "Trade::AbstractImporter::meshAttributeName(): Trade::MeshAttribute::Position is not custom\n");
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractImporterTest::mesh2D() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMesh2DCount() const override { return 8; }
        Int doMesh2DForName(const std::string& name) override {
            if(name == "eighth") return 7;
            return -1;
        }
        std::string doMesh2DName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            return {};
        }
        CORRADE_IGNORE_DEPRECATED_PUSH
        Containers::Optional<MeshData2D> doMesh2D(UnsignedInt id) override {
            if(id == 7) return MeshData2D{{}, {}, {{}}, {}, {}, &state};
            return {};
        }
        CORRADE_IGNORE_DEPRECATED_POP
    } importer;

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(importer.mesh2DCount(), 8);
    CORRADE_COMPARE(importer.mesh2DForName("eighth"), 7);
    CORRADE_COMPARE(importer.mesh2DName(7), "eighth");

    auto data = importer.mesh2D(7);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->importerState(), &state);
    CORRADE_IGNORE_DEPRECATED_POP
}

void AbstractImporterTest::mesh2DCountNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    } importer;

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(importer.mesh2DCount(), 0);
    CORRADE_IGNORE_DEPRECATED_POP
}

void AbstractImporterTest::mesh2DCountNoFile() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.mesh2DCount();
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh2DCount(): no file opened\n");
}

void AbstractImporterTest::mesh2DForNameNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    } importer;

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(importer.mesh2DForName(""), -1);
    CORRADE_IGNORE_DEPRECATED_POP
}

void AbstractImporterTest::mesh2DForNameNoFile() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.mesh2DForName("");
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh2DForName(): no file opened\n");
}

void AbstractImporterTest::mesh2DNameNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMesh2DCount() const override { return 8; }
    } importer;

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(importer.mesh2DName(7), "");
    CORRADE_IGNORE_DEPRECATED_POP
}

void AbstractImporterTest::mesh2DNameNoFile() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.mesh2DName(42);
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh2DName(): no file opened\n");
}

void AbstractImporterTest::mesh2DNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMesh2DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.mesh2DName(8);
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh2DName(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::mesh2DNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMesh2DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.mesh2D(7);
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh2D(): not implemented\n");
}

void AbstractImporterTest::mesh2DNoFile() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.mesh2D(42);
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh2D(): no file opened\n");
}

void AbstractImporterTest::mesh2DOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMesh2DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.mesh2D(8);
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh2D(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::mesh3D() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMesh3DCount() const override { return 8; }
        Int doMesh3DForName(const std::string& name) override {
            if(name == "eighth") return 7;
            return -1;
        }
        std::string doMesh3DName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            return {};
        }
        CORRADE_IGNORE_DEPRECATED_PUSH
        Containers::Optional<MeshData3D> doMesh3D(UnsignedInt id) override {
            if(id == 7) return MeshData3D{{}, {}, {{}}, {}, {}, {}, &state};
            return {};
        }
        CORRADE_IGNORE_DEPRECATED_POP
    } importer;

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(importer.mesh3DCount(), 8);
    CORRADE_COMPARE(importer.mesh3DForName("eighth"), 7);
    CORRADE_COMPARE(importer.mesh3DName(7), "eighth");

    auto data = importer.mesh3D(7);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->importerState(), &state);
    CORRADE_IGNORE_DEPRECATED_POP
}

void AbstractImporterTest::mesh3DCountNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    } importer;

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(importer.mesh3DCount(), 0);
    CORRADE_IGNORE_DEPRECATED_POP
}

void AbstractImporterTest::mesh3DCountNoFile() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.mesh3DCount();
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh3DCount(): no file opened\n");
}

void AbstractImporterTest::mesh3DForNameNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    } importer;

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(importer.mesh3DForName(""), -1);
    CORRADE_IGNORE_DEPRECATED_POP
}

void AbstractImporterTest::mesh3DForNameNoFile() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.mesh3DForName("");
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh3DForName(): no file opened\n");
}

void AbstractImporterTest::mesh3DNameNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMesh3DCount() const override { return 8; }
    } importer;

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(importer.mesh3DName(7), "");
    CORRADE_IGNORE_DEPRECATED_POP
}

void AbstractImporterTest::mesh3DNameNoFile() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.mesh3DName(42);
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh3DName(): no file opened\n");
}

void AbstractImporterTest::mesh3DNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMesh3DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.mesh3DName(8);
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh3DName(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::mesh3DNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMesh3DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.mesh3D(7);
    CORRADE_IGNORE_DEPRECATED_POP
    /* Not mesh3D() because this one delegates into mesh() for backwards
       compatibility */
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh(): not implemented\n");
}

void AbstractImporterTest::mesh3DNoFile() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.mesh3D(42);
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh3D(): no file opened\n");
}

void AbstractImporterTest::mesh3DOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMesh3DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_IGNORE_DEPRECATED_PUSH
    importer.mesh3D(8);
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh3D(): index 8 out of range for 8 entries\n");
}
#endif

void AbstractImporterTest::material() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMaterialCount() const override { return 8; }
        Int doMaterialForName(const std::string& name) override {
            if(name == "eighth") return 7;
            return -1;
        }
        std::string doMaterialName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            return {};
        }
        Containers::Optional<MaterialData> doMaterial(UnsignedInt id) override {
            if(id == 7) return Containers::optional<MaterialData>(MaterialTypes{}, nullptr, &state);
            return {};
        }
    } importer;

    CORRADE_COMPARE(importer.materialCount(), 8);
    CORRADE_COMPARE(importer.materialForName("eighth"), 7);
    CORRADE_COMPARE(importer.materialName(7), "eighth");

    {
        auto data = importer.material(7);
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data->importerState(), &state);
    } {
        auto data = importer.material("eighth");
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data->importerState(), &state);
    }
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractImporterTest::materialDeprecatedFallback() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMaterialCount() const override { return 8; }
        Int doMaterialForName(const std::string&) override { return 0; }
        /* Using a deprecated PhongMaterialData constructor to verify that
           propagating such instance works as well (array deleters etc.) */
        Containers::Optional<MaterialData> doMaterial(UnsignedInt) override {
            CORRADE_IGNORE_DEPRECATED_PUSH
            return Containers::Optional<MaterialData>{std::move(PhongMaterialData{{},
                {}, {},
                {}, {},
                {}, {},
                {},
                {}, {}, {}, {}, &state
            })};
            CORRADE_IGNORE_DEPRECATED_POP
        }
    } importer;

    {
        CORRADE_IGNORE_DEPRECATED_PUSH
        Containers::Pointer<MaterialData> data = importer.material(0);
        CORRADE_IGNORE_DEPRECATED_POP
        CORRADE_VERIFY(data);
        CORRADE_COMPARE_AS(data->attributeCount(), 0, TestSuite::Compare::Greater);
        CORRADE_COMPARE(data->importerState(), &state);
    } {
        CORRADE_IGNORE_DEPRECATED_PUSH
        Containers::Pointer<MaterialData> data = importer.material("");
        CORRADE_IGNORE_DEPRECATED_POP
        CORRADE_VERIFY(data);
        CORRADE_COMPARE_AS(data->attributeCount(), 0, TestSuite::Compare::Greater);
        CORRADE_COMPARE(data->importerState(), &state);
    }
}
#endif

void AbstractImporterTest::materialForNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMaterialCount() const override { return 8; }
        Int doMaterialForName(const std::string&) override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    importer.materialForName("");
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::materialForName(): implementation-returned index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::materialNameNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMaterialCount() const override { return 8; }
    } importer;

    CORRADE_COMPARE(importer.materialName(7), "");
}

void AbstractImporterTest::materialNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMaterialCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.materialName(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::materialName(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::materialNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMaterialCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.material(7);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::material(): not implemented\n");
}

void AbstractImporterTest::materialOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMaterialCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.material(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::material(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::materialNonOwningDeleters() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMaterialCount() const override { return 1; }
        Containers::Optional<MaterialData> doMaterial(UnsignedInt) override {
            return MaterialData{{}, {}, attributeData, {}, layerData};
        }

        UnsignedInt layerData[1]{};
        MaterialAttributeData attributeData[1]{
            {MaterialAttribute::DiffuseColor, Color4{}}
        };
    } importer;

    auto data = importer.material(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(static_cast<const void*>(data->attributeData()), importer.attributeData);
    CORRADE_COMPARE(static_cast<const void*>(data->layerData()), importer.layerData);
}

void AbstractImporterTest::materialCustomAttributeDataDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMaterialCount() const override { return 1; }
        Int doMaterialForName(const std::string&) override { return 0; }
        Containers::Optional<MaterialData> doMaterial(UnsignedInt) override {
            return MaterialData{{}, Containers::Array<MaterialAttributeData>{attributeData, 1, [](MaterialAttributeData*, std::size_t) {}}};
        }

        MaterialAttributeData attributeData[1]{
            {MaterialAttribute::DiffuseColor, Color4{}}
        };
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.material(0);
    importer.material("");
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::material(): implementation is not allowed to use a custom Array deleter\n"
        "Trade::AbstractImporter::material(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImporterTest::materialCustomLayerDataDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMaterialCount() const override { return 1; }
        Int doMaterialForName(const std::string&) override { return 0; }
        Containers::Optional<MaterialData> doMaterial(UnsignedInt) override {
            return MaterialData{{}, nullptr, Containers::Array<UnsignedInt>{layerData, 1, [](UnsignedInt*, std::size_t) {}}};
        }

        UnsignedInt layerData[1]{};
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.material(0);
    importer.material("");
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::material(): implementation is not allowed to use a custom Array deleter\n"
        "Trade::AbstractImporter::material(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImporterTest::texture() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doTextureCount() const override { return 8; }
        Int doTextureForName(const std::string& name) override {
            if(name == "eighth") return 7;
            return -1;
        }
        std::string doTextureName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            return {};
        }
        Containers::Optional<TextureData> doTexture(UnsignedInt id) override {
            if(id == 7) return TextureData{{}, {}, {}, {}, {}, {}, &state};
            return {};
        }
    } importer;

    CORRADE_COMPARE(importer.textureCount(), 8);
    CORRADE_COMPARE(importer.textureForName("eighth"), 7);
    CORRADE_COMPARE(importer.textureName(7), "eighth");

    {
        auto data = importer.texture(7);
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data->importerState(), &state);
    } {
        auto data = importer.texture("eighth");
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data->importerState(), &state);
    }
}

void AbstractImporterTest::textureForNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doTextureCount() const override { return 8; }
        Int doTextureForName(const std::string&) override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    importer.textureForName("");
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::textureForName(): implementation-returned index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::textureNameNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doTextureCount() const override { return 8; }
    } importer;

    CORRADE_COMPARE(importer.textureName(7), "");
}

void AbstractImporterTest::textureNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doTextureCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.textureName(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::textureName(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::textureNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doTextureCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.texture(7);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::texture(): not implemented\n");
}

void AbstractImporterTest::textureOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doTextureCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.texture(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::texture(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::image1D() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage1DCount() const override { return 8; }
        UnsignedInt doImage1DLevelCount(UnsignedInt id) override {
            if(id == 7) return 3;
            return {};
        }
        Int doImage1DForName(const std::string& name) override {
            if(name == "eighth") return 7;
            return -1;
        }
        std::string doImage1DName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            return {};
        }
        Containers::Optional<ImageData1D> doImage1D(UnsignedInt id, UnsignedInt level) override {
            if(id == 7 && level == 2) return ImageData1D{PixelFormat::RGBA8Unorm, {}, {}, &state};
            return {};
        }
    } importer;

    CORRADE_COMPARE(importer.image1DCount(), 8);
    CORRADE_COMPARE(importer.image1DLevelCount(7), 3);
    CORRADE_COMPARE(importer.image1DForName("eighth"), 7);
    CORRADE_COMPARE(importer.image1DName(7), "eighth");

    {
        auto data = importer.image1D(7, 2);
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data->importerState(), &state);
    } {
        auto data = importer.image1D("eighth", 2);
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data->importerState(), &state);
    }
}

void AbstractImporterTest::image1DLevelCountNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage1DCount() const override { return 8; }
    } importer;

    CORRADE_COMPARE(importer.image1DLevelCount(7), 1);
}

void AbstractImporterTest::image1DLevelCountOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage1DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    importer.image1DLevelCount(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image1DLevelCount(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::image1DLevelCountZero() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage1DCount() const override { return 8; }
        Int doImage1DForName(const std::string&) override { return 0; }
        UnsignedInt doImage1DLevelCount(UnsignedInt) override { return 0; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    importer.image1DLevelCount(7);
    /* This should print a similar message instead of a confusing
       "level 1 out of range for 0 entries" */
    importer.image1D(7, 1);
    importer.image1D("", 1);
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::image1DLevelCount(): implementation reported zero levels\n"
        "Trade::AbstractImporter::image1D(): implementation reported zero levels\n"
        "Trade::AbstractImporter::image1D(): implementation reported zero levels\n");
}

void AbstractImporterTest::image1DForNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage1DCount() const override { return 8; }
        Int doImage1DForName(const std::string&) override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    importer.image1DForName("");
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image1DForName(): implementation-returned index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::image1DNameNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage1DCount() const override { return 8; }
    } importer;

    CORRADE_COMPARE(importer.image1DName(7), "");
}

void AbstractImporterTest::image1DNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage1DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.image1DName(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image1DName(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::image1DNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage1DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.image1D(7);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image1D(): not implemented\n");
}

void AbstractImporterTest::image1DOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage1DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.image1D(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image1D(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::image1DLevelOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage1DCount() const override { return 8; }
        Int doImage1DForName(const std::string&) override { return 0; }
        UnsignedInt doImage1DLevelCount(UnsignedInt) override { return 3; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    importer.image1D(7, 3);
    importer.image1D("", 3);
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::image1D(): level 3 out of range for 3 entries\n"
        "Trade::AbstractImporter::image1D(): level 3 out of range for 3 entries\n");
}

void AbstractImporterTest::image1DNonOwningDeleter() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage1DCount() const override { return 1; }
        Containers::Optional<ImageData1D> doImage1D(UnsignedInt, UnsignedInt) override {
            return ImageData1D{PixelFormat::RGBA8Unorm, {}, Containers::Array<char>{data, 1, Implementation::nonOwnedArrayDeleter}};
        }

        char data[1];
    } importer;

    auto data = importer.image1D(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(static_cast<const void*>(data->data()), importer.data);
}

void AbstractImporterTest::image1DGrowableDeleter() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage1DCount() const override { return 1; }
        Containers::Optional<ImageData1D> doImage1D(UnsignedInt, UnsignedInt) override {
            Containers::Array<char> data;
            Containers::arrayAppend<ArrayAllocator>(data, '\xff');
            return ImageData1D{PixelFormat::RGBA8Unorm, {}, std::move(data)};
        }
    } importer;

    auto data = importer.image1D(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->data()[0], '\xff');
}

void AbstractImporterTest::image1DCustomDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage1DCount() const override { return 1; }
        Int doImage1DForName(const std::string&) override { return 0; }
        Containers::Optional<ImageData1D> doImage1D(UnsignedInt, UnsignedInt) override {
            return ImageData1D{PixelFormat::RGBA8Unorm, {}, Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}}};
        }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.image1D(0);
    importer.image1D("");
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::image1D(): implementation is not allowed to use a custom Array deleter\n"
        "Trade::AbstractImporter::image1D(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImporterTest::image2D() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage2DCount() const override { return 8; }
        UnsignedInt doImage2DLevelCount(UnsignedInt id) override {
            if(id == 7) return 3;
            return {};
        }
        Int doImage2DForName(const std::string& name) override {
            if(name == "eighth") return 7;
            return -1;
        }
        std::string doImage2DName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            return {};
        }
        Containers::Optional<ImageData2D> doImage2D(UnsignedInt id, UnsignedInt level) override {
            if(id == 7 && level == 2) return ImageData2D{PixelFormat::RGBA8Unorm, {}, {}, &state};
            return {};
        }
    } importer;

    CORRADE_COMPARE(importer.image2DCount(), 8);
    CORRADE_COMPARE(importer.image2DLevelCount(7), 3);
    CORRADE_COMPARE(importer.image2DForName("eighth"), 7);
    CORRADE_COMPARE(importer.image2DName(7), "eighth");

    {
        auto data = importer.image2D(7, 2);
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data->importerState(), &state);
    } {
        auto data = importer.image2D("eighth", 2);
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data->importerState(), &state);
    }
}

void AbstractImporterTest::image2DLevelCountNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage2DCount() const override { return 8; }
    } importer;

    CORRADE_COMPARE(importer.image2DLevelCount(7), 1);
}

void AbstractImporterTest::image2DLevelCountOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage2DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    importer.image2DLevelCount(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image2DLevelCount(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::image2DLevelCountZero() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage2DCount() const override { return 8; }
        Int doImage2DForName(const std::string&) override { return 0; }
        UnsignedInt doImage2DLevelCount(UnsignedInt) override { return 0; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    importer.image2DLevelCount(7);
    /* This should print a similar message instead of a confusing
       "level 1 out of range for 0 entries" */
    importer.image2D(7, 1);
    importer.image2D(7, 1);
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::image2DLevelCount(): implementation reported zero levels\n"
        "Trade::AbstractImporter::image2D(): implementation reported zero levels\n"
        "Trade::AbstractImporter::image2D(): implementation reported zero levels\n");
}

void AbstractImporterTest::image2DForNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage2DCount() const override { return 8; }
        Int doImage2DForName(const std::string&) override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    importer.image2DForName("");
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image2DForName(): implementation-returned index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::image2DNameNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage2DCount() const override { return 8; }
    } importer;

    CORRADE_COMPARE(importer.image2DName(7), "");
}

void AbstractImporterTest::image2DNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage2DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.image2DName(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image2DName(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::image2DNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage2DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.image2D(7);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image2D(): not implemented\n");
}

void AbstractImporterTest::image2DOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage2DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.image2D(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image2D(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::image2DLevelOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage2DCount() const override { return 8; }
        Int doImage2DForName(const std::string&) override { return 0; }
        UnsignedInt doImage2DLevelCount(UnsignedInt) override { return 3; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    importer.image2D(7, 3);
    importer.image2D("", 3);
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::image2D(): level 3 out of range for 3 entries\n"
        "Trade::AbstractImporter::image2D(): level 3 out of range for 3 entries\n");
}

void AbstractImporterTest::image2DNonOwningDeleter() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage2DCount() const override { return 1; }
        Containers::Optional<ImageData2D> doImage2D(UnsignedInt, UnsignedInt) override {
            return ImageData2D{PixelFormat::RGBA8Unorm, {}, Containers::Array<char>{data, 1, Implementation::nonOwnedArrayDeleter}};
        }

        char data[1];
    } importer;

    auto data = importer.image2D(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(static_cast<const void*>(data->data()), importer.data);
}

void AbstractImporterTest::image2DGrowableDeleter() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage2DCount() const override { return 1; }
        Containers::Optional<ImageData2D> doImage2D(UnsignedInt, UnsignedInt) override {
            Containers::Array<char> data;
            Containers::arrayAppend<ArrayAllocator>(data, '\xff');
            return ImageData2D{PixelFormat::RGBA8Unorm, {}, std::move(data)};
        }
    } importer;

    auto data = importer.image2D(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->data()[0], '\xff');
}

void AbstractImporterTest::image2DCustomDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage2DCount() const override { return 1; }
        Int doImage2DForName(const std::string&) override { return 0; }
        Containers::Optional<ImageData2D> doImage2D(UnsignedInt, UnsignedInt) override {
            return ImageData2D{PixelFormat::RGBA8Unorm, {}, Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}}};
        }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.image2D(0);
    importer.image2D("");
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::image2D(): implementation is not allowed to use a custom Array deleter\n"
        "Trade::AbstractImporter::image2D(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImporterTest::image3D() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage3DCount() const override { return 8; }
        UnsignedInt doImage3DLevelCount(UnsignedInt id) override {
            if(id == 7) return 3;
            return {};
        }
        Int doImage3DForName(const std::string& name) override {
            if(name == "eighth") return 7;
            return -1;
        }
        std::string doImage3DName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            return {};
        }
        Containers::Optional<ImageData3D> doImage3D(UnsignedInt id, UnsignedInt level) override {
            if(id == 7 && level == 2) return ImageData3D{PixelFormat::RGBA8Unorm, {}, {}, &state};
            return {};
        }
    } importer;

    CORRADE_COMPARE(importer.image3DCount(), 8);
    CORRADE_COMPARE(importer.image3DLevelCount(7), 3);
    CORRADE_COMPARE(importer.image3DForName("eighth"), 7);
    CORRADE_COMPARE(importer.image3DName(7), "eighth");

    {
        auto data = importer.image3D(7, 2);
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data->importerState(), &state);
    } {
        auto data = importer.image3D("eighth", 2);
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data->importerState(), &state);
    }
}

void AbstractImporterTest::image3DForNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage3DCount() const override { return 8; }
        Int doImage3DForName(const std::string&) override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    importer.image3DForName("");
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image3DForName(): implementation-returned index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::image3DLevelCountNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage3DCount() const override { return 8; }
    } importer;

    CORRADE_COMPARE(importer.image3DLevelCount(7), 1);
}

void AbstractImporterTest::image3DLevelCountOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage3DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    importer.image3DLevelCount(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image3DLevelCount(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::image3DLevelCountZero() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage3DCount() const override { return 8; }
        Int doImage3DForName(const std::string&) override { return 0; }
        UnsignedInt doImage3DLevelCount(UnsignedInt) override { return 0; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    importer.image3DLevelCount(7);
    /* This should print a similar message instead of a confusing
       "level 1 out of range for 0 entries" */
    importer.image3D(7, 1);
    importer.image3D("", 1);
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::image3DLevelCount(): implementation reported zero levels\n"
        "Trade::AbstractImporter::image3D(): implementation reported zero levels\n"
        "Trade::AbstractImporter::image3D(): implementation reported zero levels\n");
}

void AbstractImporterTest::image3DNameNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage3DCount() const override { return 8; }
    } importer;

    CORRADE_COMPARE(importer.image3DName(7), "");
}

void AbstractImporterTest::image3DNameOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage3DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.image3DName(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image3DName(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::image3DNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage3DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.image3D(7);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image3D(): not implemented\n");
}

void AbstractImporterTest::image3DOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage3DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.image3D(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image3D(): index 8 out of range for 8 entries\n");
}

void AbstractImporterTest::image3DLevelOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage3DCount() const override { return 8; }
        Int doImage3DForName(const std::string&) override { return 0; }
        UnsignedInt doImage3DLevelCount(UnsignedInt) override { return 3; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};
    importer.image3D(7, 3);
    importer.image3D("", 3);
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::image3D(): level 3 out of range for 3 entries\n"
        "Trade::AbstractImporter::image3D(): level 3 out of range for 3 entries\n");
}

void AbstractImporterTest::image3DNonOwningDeleter() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage3DCount() const override { return 1; }
        Containers::Optional<ImageData3D> doImage3D(UnsignedInt, UnsignedInt) override {
            return ImageData3D{PixelFormat::RGBA8Unorm, {}, Containers::Array<char>{data, 1, Implementation::nonOwnedArrayDeleter}};
        }

        char data[1];
    } importer;

    auto data = importer.image3D(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(static_cast<const void*>(data->data()), importer.data);
}

void AbstractImporterTest::image3DGrowableDeleter() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage3DCount() const override { return 1; }
        Containers::Optional<ImageData3D> doImage3D(UnsignedInt, UnsignedInt) override {
            Containers::Array<char> data;
            Containers::arrayAppend<ArrayAllocator>(data, '\xff');
            return ImageData3D{PixelFormat::RGBA8Unorm, {}, std::move(data)};
        }
    } importer;

    auto data = importer.image3D(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->data()[0], '\xff');
}

void AbstractImporterTest::image3DCustomDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage3DCount() const override { return 1; }
        Int doImage3DForName(const std::string&) override { return 0; }
        Containers::Optional<ImageData3D> doImage3D(UnsignedInt, UnsignedInt) override {
            return ImageData3D{PixelFormat::RGBA8Unorm, {}, Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}}};
        }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.image3D(0);
    importer.image3D("");
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractImporter::image3D(): implementation is not allowed to use a custom Array deleter\n"
        "Trade::AbstractImporter::image3D(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImporterTest::importerState() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        const void* doImporterState() const override { return &state; }
    } importer;

    CORRADE_COMPARE(importer.importerState(), &state);
}

void AbstractImporterTest::importerStateNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    } importer;

    CORRADE_COMPARE(importer.importerState(), nullptr);
}

void AbstractImporterTest::importerStateNoFile() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.importerState();
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::importerState(): no file opened\n");
}

void AbstractImporterTest::debugFeature() {
    std::ostringstream out;

    Debug{&out} << ImporterFeature::OpenData << ImporterFeature(0xfefe);
    CORRADE_COMPARE(out.str(), "Trade::ImporterFeature::OpenData Trade::ImporterFeature(0xfefe)\n");
}

void AbstractImporterTest::debugFeatures() {
    std::ostringstream out;

    Debug{&out} << (ImporterFeature::OpenData|ImporterFeature::OpenState) << ImporterFeatures{};
    CORRADE_COMPARE(out.str(), "Trade::ImporterFeature::OpenData|Trade::ImporterFeature::OpenState Trade::ImporterFeatures{}\n");
}

void AbstractImporterTest::debugFlag() {
    std::ostringstream out;

    Debug{&out} << ImporterFlag::Verbose << ImporterFlag(0xbaba);
    CORRADE_COMPARE(out.str(), "Trade::ImporterFlag::Verbose Trade::ImporterFlag(0xbaba)\n");
}

void AbstractImporterTest::debugFlags() {
    std::ostringstream out;

    Debug{&out} << (ImporterFlag::Verbose|ImporterFlag(0xf000)) << ImporterFlags{};
    CORRADE_COMPARE(out.str(), "Trade::ImporterFlag::Verbose|Trade::ImporterFlag(0xf000) Trade::ImporterFlags{}\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AbstractImporterTest)
