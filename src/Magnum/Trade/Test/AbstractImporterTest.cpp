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

#include <sstream>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/FileCallback.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/AnimationData.h"
#include "Magnum/Trade/ArrayAllocator.h"
#include "Magnum/Trade/CameraData.h"
#include "Magnum/Trade/ImageData.h"
#include "Magnum/Trade/LightData.h"
#include "Magnum/Trade/MeshData.h"
#include "Magnum/Trade/MeshObjectData2D.h"
#include "Magnum/Trade/MeshObjectData3D.h"
#include "Magnum/Trade/PhongMaterialData.h"
#include "Magnum/Trade/SceneData.h"
#include "Magnum/Trade/SkinData.h"
#include "Magnum/Trade/TextureData.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#define _MAGNUM_NO_DEPRECATED_MESHDATA /* So it doesn't yell here */

#include "Magnum/Trade/MeshData2D.h"
#include "Magnum/Trade/MeshData3D.h"
#endif

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct AbstractImporterTest: TestSuite::Tester {
    explicit AbstractImporterTest();

    void construct();
    void constructWithPluginManagerReference();

    void setFlags();
    void setFlagsFileOpened();
    void setFlagsNotImplemented();

    void openData();
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

    void scene();
    void sceneNameNotImplemented();
    void sceneNameOutOfRange();
    void sceneNotImplemented();
    void sceneOutOfRange();

    void animation();
    void animationNameNotImplemented();
    void animationNameOutOfRange();
    void animationNotImplemented();
    void animationOutOfRange();
    void animationNonOwningDeleters();
    void animationGrowableDeleters();
    void animationCustomDataDeleter();
    void animationCustomTrackDeleter();

    void light();
    void lightNameNotImplemented();
    void lightNameOutOfRange();
    void lightNotImplemented();
    void lightOutOfRange();

    void camera();
    void cameraNameNotImplemented();
    void cameraNameOutOfRange();
    void cameraNotImplemented();
    void cameraOutOfRange();

    void object2D();
    void object2DNameNotImplemented();
    void object2DNameOutOfRange();
    void object2DNotImplemented();
    void object2DOutOfRange();

    void object3D();
    void object3DNameNotImplemented();
    void object3DNameOutOfRange();
    void object3DNotImplemented();
    void object3DOutOfRange();

    void skin2D();
    void skin2DNameNotImplemented();
    void skin2DNameOutOfRange();
    void skin2DNotImplemented();
    void skin2DOutOfRange();
    void skin2DNonOwningDeleters();
    void skin2DCustomJointDataDeleter();
    void skin2DCustomInverseBindMatrixDataDeleter();

    void skin3D();
    void skin3DNameNotImplemented();
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
    void materialNameNotImplemented();
    void materialNameOutOfRange();
    void materialNotImplemented();
    void materialOutOfRange();
    void materialNonOwningDeleters();
    void materialCustomAttributeDataDeleter();
    void materialCustomLayerDataDeleter();

    void texture();
    void textureNameNotImplemented();
    void textureNameOutOfRange();
    void textureNotImplemented();
    void textureOutOfRange();

    void image1D();
    void image1DLevelCountNotImplemented();
    void image1DLevelCountOutOfRange();
    void image1DLevelCountZero();
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

constexpr struct {
    const char* name;
    bool checkMessage;
} ThingByNameData[] {
    {"check it's not an assert", false},
    {"verify the message", true}
};

AbstractImporterTest::AbstractImporterTest() {
    addTests({&AbstractImporterTest::construct,
              &AbstractImporterTest::constructWithPluginManagerReference,

              &AbstractImporterTest::setFlags,
              &AbstractImporterTest::setFlagsFileOpened,
              &AbstractImporterTest::setFlagsNotImplemented,

              &AbstractImporterTest::openData,
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
              &AbstractImporterTest::defaultSceneNotImplemented,

              &AbstractImporterTest::scene,
              &AbstractImporterTest::sceneNameNotImplemented,
              &AbstractImporterTest::sceneNameOutOfRange,
              &AbstractImporterTest::sceneNotImplemented,
              &AbstractImporterTest::sceneOutOfRange,

              &AbstractImporterTest::animation,
              &AbstractImporterTest::animationNameNotImplemented,
              &AbstractImporterTest::animationNameOutOfRange,
              &AbstractImporterTest::animationNotImplemented,
              &AbstractImporterTest::animationOutOfRange,
              &AbstractImporterTest::animationNonOwningDeleters,
              &AbstractImporterTest::animationGrowableDeleters,
              &AbstractImporterTest::animationCustomDataDeleter,
              &AbstractImporterTest::animationCustomTrackDeleter,

              &AbstractImporterTest::light,
              &AbstractImporterTest::lightNameNotImplemented,
              &AbstractImporterTest::lightNameOutOfRange,
              &AbstractImporterTest::lightNotImplemented,
              &AbstractImporterTest::lightOutOfRange,

              &AbstractImporterTest::camera,
              &AbstractImporterTest::cameraNameNotImplemented,
              &AbstractImporterTest::cameraNameOutOfRange,
              &AbstractImporterTest::cameraNotImplemented,
              &AbstractImporterTest::cameraOutOfRange,

              &AbstractImporterTest::object2D,
              &AbstractImporterTest::object2DNameNotImplemented,
              &AbstractImporterTest::object2DNameOutOfRange,
              &AbstractImporterTest::object2DNotImplemented,
              &AbstractImporterTest::object2DOutOfRange,

              &AbstractImporterTest::object3D,
              &AbstractImporterTest::object3DNameNotImplemented,
              &AbstractImporterTest::object3DNameOutOfRange,
              &AbstractImporterTest::object3DNotImplemented,
              &AbstractImporterTest::object3DOutOfRange,

              &AbstractImporterTest::skin2D,
              &AbstractImporterTest::skin2DNameNotImplemented,
              &AbstractImporterTest::skin2DNameOutOfRange,
              &AbstractImporterTest::skin2DNotImplemented,
              &AbstractImporterTest::skin2DOutOfRange,
              &AbstractImporterTest::skin2DNonOwningDeleters,
              &AbstractImporterTest::skin2DCustomJointDataDeleter,
              &AbstractImporterTest::skin2DCustomInverseBindMatrixDataDeleter,

              &AbstractImporterTest::skin3D,
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
              &AbstractImporterTest::materialNameNotImplemented,
              &AbstractImporterTest::materialNameOutOfRange,
              &AbstractImporterTest::materialNotImplemented,
              &AbstractImporterTest::materialOutOfRange,
              &AbstractImporterTest::materialNonOwningDeleters,
              &AbstractImporterTest::materialCustomAttributeDataDeleter,
              &AbstractImporterTest::materialCustomLayerDataDeleter,

              &AbstractImporterTest::texture,
              &AbstractImporterTest::textureNameNotImplemented,
              &AbstractImporterTest::textureNameOutOfRange,
              &AbstractImporterTest::textureNotImplemented,
              &AbstractImporterTest::textureOutOfRange,

              &AbstractImporterTest::image1D,
              &AbstractImporterTest::image1DLevelCountNotImplemented,
              &AbstractImporterTest::image1DLevelCountOutOfRange,
              &AbstractImporterTest::image1DLevelCountZero,
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
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::setFlags(): can't be set while a file is opened\n");
}

void AbstractImporterTest::openData() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return ImporterFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        void doOpenData(Containers::ArrayView<const char> data) override {
            _opened = (data.size() == 1 && data[0] == '\xa5');
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

void AbstractImporterTest::openFileAsData() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return ImporterFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        void doOpenData(Containers::ArrayView<const char> data) override {
            _opened = (data.size() == 1 && data[0] == '\xa5');
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

        void doOpenData(Containers::ArrayView<const char>) override {
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
            called = !callback && !userData;
        }

        bool called = false;
    } importer;

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
            _opened = filename == "file.dat" && fileCallback() && fileCallbackUserData();
        }

        void doOpenData(Containers::ArrayView<const char>) override {
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
            openFileCalled = filename == "file.dat" && fileCallback() && fileCallbackUserData();
            AbstractImporter::doOpenFile(filename);
        }

        void doOpenData(Containers::ArrayView<const char> data) override {
            _opened = (data.size() == 1 && data[0] == '\xb0');
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

        void doOpenData(Containers::ArrayView<const char> data) override {
            _opened = (data.size() == 1 && data[0] == '\xb0');
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
    CORRADE_COMPARE(importer.animationCount(), 0);
    CORRADE_COMPARE(importer.lightCount(), 0);
    CORRADE_COMPARE(importer.cameraCount(), 0);

    CORRADE_COMPARE(importer.object2DCount(), 0);
    CORRADE_COMPARE(importer.object3DCount(), 0);

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
    importer.animationCount();
    importer.lightCount();
    importer.cameraCount();

    importer.object2DCount();
    importer.object3DCount();

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
        "Trade::AbstractImporter::animationCount(): no file opened\n"
        "Trade::AbstractImporter::lightCount(): no file opened\n"
        "Trade::AbstractImporter::cameraCount(): no file opened\n"

        "Trade::AbstractImporter::object2DCount(): no file opened\n"
        "Trade::AbstractImporter::object3DCount(): no file opened\n"

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
    CORRADE_COMPARE(importer.animationForName(""), -1);
    CORRADE_COMPARE(importer.lightForName(""), -1);
    CORRADE_COMPARE(importer.cameraForName(""), -1);

    CORRADE_COMPARE(importer.object2DForName(""), -1);
    CORRADE_COMPARE(importer.object3DForName(""), -1);

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
    importer.animationForName("");
    importer.lightForName("");
    importer.cameraForName("");

    importer.object2DForName("");
    importer.object3DForName("");

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
        "Trade::AbstractImporter::animationForName(): no file opened\n"
        "Trade::AbstractImporter::lightForName(): no file opened\n"
        "Trade::AbstractImporter::cameraForName(): no file opened\n"

        "Trade::AbstractImporter::object2DForName(): no file opened\n"
        "Trade::AbstractImporter::object3DForName(): no file opened\n"

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
    } importer;

    std::ostringstream out;
    {
        Containers::Optional<Error> redirectError;
        if(data.checkMessage) redirectError.emplace(&out);

        CORRADE_VERIFY(!importer.scene("foobar"));
        CORRADE_VERIFY(!importer.animation("foobar"));
        CORRADE_VERIFY(!importer.light("foobar"));
        CORRADE_VERIFY(!importer.camera("foobar"));

        CORRADE_VERIFY(!importer.object2D("foobar"));
        CORRADE_VERIFY(!importer.object3D("foobar"));

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
            "Trade::AbstractImporter::scene(): scene foobar not found\n"
            "Trade::AbstractImporter::animation(): animation foobar not found\n"
            "Trade::AbstractImporter::light(): light foobar not found\n"
            "Trade::AbstractImporter::camera(): camera foobar not found\n"

            "Trade::AbstractImporter::object2D(): object foobar not found\n"
            "Trade::AbstractImporter::object3D(): object foobar not found\n"

            "Trade::AbstractImporter::skin2D(): skin foobar not found\n"
            "Trade::AbstractImporter::skin3D(): skin foobar not found\n"

            "Trade::AbstractImporter::mesh(): mesh foobar not found\n"
            "Trade::AbstractImporter::material(): material foobar not found\n"
            "Trade::AbstractImporter::texture(): texture foobar not found\n"

            "Trade::AbstractImporter::image1D(): image foobar not found\n"
            "Trade::AbstractImporter::image2D(): image foobar not found\n"
            "Trade::AbstractImporter::image3D(): image foobar not found\n");
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

    importer.object2DName(42);
    importer.object3DName(42);

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

        "Trade::AbstractImporter::object2DName(): no file opened\n"
        "Trade::AbstractImporter::object3DName(): no file opened\n"

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

    importer.object2D(42);
    importer.object2D("foo");
    importer.object3D(42);
    importer.object3D("foo");

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

        "Trade::AbstractImporter::object2D(): no file opened\n"
        "Trade::AbstractImporter::object2D(): no file opened\n"
        "Trade::AbstractImporter::object3D(): no file opened\n"
        "Trade::AbstractImporter::object3D(): no file opened\n"

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
            if(id == 7) return SceneData{{}, {}, &state};
            return SceneData{{}, {}};
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

void AbstractImporterTest::sceneNameNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 8; }
    } importer;

    CORRADE_COMPARE(importer.sceneName(7), "");
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
                reinterpret_cast<void(*)(AnimationTrackData*, std::size_t)>(Implementation::nonOwnedArrayDeleter)}};
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
        Containers::Pointer<ObjectData2D> doObject2D(UnsignedInt id) override {
            if(id == 7) return Containers::pointer(new ObjectData2D{{}, {}, &state});
            return {};
        }
    } importer;

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
}

void AbstractImporterTest::object2DNameNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doObject2DCount() const override { return 8; }
    } importer;

    CORRADE_COMPARE(importer.object2DName(7), "");
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

    importer.object2DName(8);
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

        UnsignedInt doObject2DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.object2D(7);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object2D(): not implemented\n");
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

    importer.object2D(8);
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
        Containers::Pointer<ObjectData3D> doObject3D(UnsignedInt id) override {
            if(id == 7) return Containers::pointer(new ObjectData3D{{}, {}, &state});
            return {};
        }
    } importer;

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
}

void AbstractImporterTest::object3DNameNotImplemented() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doObject3DCount() const override { return 8; }
    } importer;

    CORRADE_COMPARE(importer.object3DName(7), "");
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

    importer.object3DName(8);
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

        UnsignedInt doObject3DCount() const override { return 8; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.object3D(7);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object3D(): not implemented\n");
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

    importer.object3D(8);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object3D(): index 8 out of range for 8 entries\n");
}

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

    Debug{&out} << ImporterFeature::OpenData << ImporterFeature(0xf0);
    CORRADE_COMPARE(out.str(), "Trade::ImporterFeature::OpenData Trade::ImporterFeature(0xf0)\n");
}

void AbstractImporterTest::debugFeatures() {
    std::ostringstream out;

    Debug{&out} << (ImporterFeature::OpenData|ImporterFeature::OpenState) << ImporterFeatures{};
    CORRADE_COMPARE(out.str(), "Trade::ImporterFeature::OpenData|Trade::ImporterFeature::OpenState Trade::ImporterFeatures{}\n");
}

void AbstractImporterTest::debugFlag() {
    std::ostringstream out;

    Debug{&out} << ImporterFlag::Verbose << ImporterFlag(0xf0);
    CORRADE_COMPARE(out.str(), "Trade::ImporterFlag::Verbose Trade::ImporterFlag(0xf0)\n");
}

void AbstractImporterTest::debugFlags() {
    std::ostringstream out;

    Debug{&out} << (ImporterFlag::Verbose|ImporterFlag(0xf0)) << ImporterFlags{};
    CORRADE_COMPARE(out.str(), "Trade::ImporterFlag::Verbose|Trade::ImporterFlag(0xf0) Trade::ImporterFlags{}\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AbstractImporterTest)
