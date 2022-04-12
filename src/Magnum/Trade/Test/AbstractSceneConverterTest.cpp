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
#include <Corrade/Containers/AnyReference.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/Containers/StringStl.h> /** @todo remove once Debug is stream-free */
#include <Corrade/Containers/Optional.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/FileToString.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Path.h>

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Trade/ArrayAllocator.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/AbstractSceneConverter.h"
#include "Magnum/Trade/AnimationData.h"
#include "Magnum/Trade/CameraData.h"
#include "Magnum/Trade/ImageData.h"
#include "Magnum/Trade/LightData.h"
#include "Magnum/Trade/MaterialData.h"
#include "Magnum/Trade/MeshData.h"
#include "Magnum/Trade/SceneData.h"
#include "Magnum/Trade/SkinData.h"
#include "Magnum/Trade/TextureData.h"

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct AbstractSceneConverterTest: TestSuite::Tester {
    explicit AbstractSceneConverterTest();

    void featuresNone();

    void setFlags();
    void setFlagsNotImplemented();

    void thingNotSupported();
    /* Certain features need a combination of flags, test them explicitly */
    void thingLevelsNotSupported();

    void convertMesh();
    void convertMeshFailed();
    void convertMeshNotImplemented();
    void convertMeshNonOwningDeleters();
    void convertMeshGrowableDeleters();
    void convertMeshCustomIndexDataDeleter();
    void convertMeshCustomVertexDataDeleter();
    void convertMeshCustomAttributeDataDeleter();

    void convertMeshInPlace();
    void convertMeshInPlaceFailed();
    void convertMeshInPlaceNotImplemented();

    void convertMeshToData();
    void convertMeshToDataFailed();
    void convertMeshToDataThroughBatch();
    void convertMeshToDataThroughBatchAddFailed();
    void convertMeshToDataThroughBatchEndFailed();
    void convertMeshToDataNotImplemented();
    void convertMeshToDataNonOwningDeleter();
    void convertMeshToDataGrowableDeleter();
    void convertMeshToDataCustomDeleter();

    void convertMeshToFile();
    void convertMeshToFileFailed();
    void convertMeshToFileThroughData();
    void convertMeshToFileThroughDataFailed();
    void convertMeshToFileThroughDataNotWritable();
    void convertMeshToFileThroughBatch();
    void convertMeshToFileThroughBatchAddFailed();
    void convertMeshToFileThroughBatchEndFailed();
    void convertMeshToFileNotImplemented();

    void beginEnd();
    void beginEndFailed();
    void beginNotImplemented();
    void endNotImplemented();

    void beginEndData();
    void beginEndDataFailed();
    void beginDataNotImplemented();
    void endDataNotImplemented();
    void beginEndDataCustomDeleter();

    void beginEndFile();
    void beginEndFileFailed();
    void beginEndFileThroughData();
    void beginEndFileThroughDataFailed();
    void beginEndFileThroughDataNotWritable();
    void beginFileNotImplemented();
    void endFileNotImplemented();

    void abort();
    void abortNotImplemented();

    void thingNoBegin();
    void endMismatchedBegin();
    void endDataMismatchedBegin();
    void endFileMismatchedBegin();

    void addScene();
    void addSceneFailed();
    void addSceneNotImplemented();

    void setSceneFieldName();
    void setSceneFieldNameNotImplemented();
    void setSceneFieldNameNotCustom();

    void setObjectName();
    void setObjectNameNotImplemented();

    void setDefaultScene();
    void setDefaultSceneNotImplemented();
    void setDefaultSceneOutOfRange();

    void addAnimation();
    void addAnimationFailed();
    void addAnimationNotImplemented();

    void addLight();
    void addLightFailed();
    void addLightNotImplemented();

    void addCamera();
    void addCameraFailed();
    void addCameraNotImplemented();

    void addSkin2D();
    void addSkin2DFailed();
    void addSkin2DNotImplemented();

    void addSkin3D();
    void addSkin3DFailed();
    void addSkin3DNotImplemented();

    void addMesh();
    void addMeshFailed();
    void addMeshThroughConvertMesh();
    void addMeshThroughConvertMeshFailed();
    void addMeshThroughConvertMeshZeroMeshes();
    void addMeshThroughConvertMeshTwoMeshes();
    void addMeshThroughConvertMeshToData();
    void addMeshThroughConvertMeshToDataFailed();
    void addMeshThroughConvertMeshToDataZeroMeshes();
    void addMeshThroughConvertMeshToDataTwoMeshes();
    void addMeshThroughConvertMeshToFile();
    void addMeshThroughConvertMeshToFileThroughData();
    void addMeshThroughConvertMeshToFileFailed();
    void addMeshThroughConvertMeshToFileZeroMeshes();
    void addMeshThroughConvertMeshToFileTwoMeshes();
    void addMeshNotImplemented();

    void addMeshLevels();
    void addMeshLevelsFailed();
    void addMeshLevelsNoLevels();
    void addMeshLevelsNotImplemented();

    void addMeshThroughLevels();

    void setMeshAttributeName();
    void setMeshAttributeNameNotImplemented();
    void setMeshAttributeNameNotCustom();

    void addMaterial();
    void addMaterialFailed();
    void addMaterialNotImplemented();

    void addTexture();
    void addTextureFailed();
    void addTextureNotImplemented();

    void addImage1D();
    void addImage1DView();
    void addImage1DCompressedView();
    void addImage1DFailed();
    /* 1D/2D/3D share the same image validity check function, so it's verified
       only for 2D thoroughly and for others just that the check is used */
    void addImage1DInvalidImage();
    void addImage1DNotImplemented();

    void addImage2D();
    void addImage2DView();
    void addImage2DCompressedView();
    void addImage2DFailed();
    void addImage2DZeroSize();
    void addImage2DNullptr();
    void addImage2DNotImplemented();

    void addImage3D();
    void addImage3DView();
    void addImage3DCompressedView();
    void addImage3DFailed();
    /* 1D/2D/3D share the same image validity check function, so it's verified
       only for 2D thoroughly and for others just that the check is used */
    void addImage3DInvalidImage();
    void addImage3DNotImplemented();

    void addImageLevels1D();
    void addImageLevels1DView();
    void addImageLevels1DCompressedView();
    void addImageLevels1DFailed();
    /* 1D/2D/3D share the same image list validity check function, so it's
       verified only for 2D thoroughly and for others just that the check is
       used */
    void addImageLevels1DInvalidImage();
    void addImageLevels1DNotImplemented();

    void addImageLevels2D();
    void addImageLevels2DView();
    void addImageLevels2DCompressedView();
    void addImageLevels2DFailed();
    void addImageLevels2DNoLevels();
    void addImageLevels2DZeroSize();
    void addImageLevels2DNullptr();
    void addImageLevels2DInconsistentCompressed();
    void addImageLevels2DInconsistentFormat();
    void addImageLevels2DInconsistentFormatExtra();
    void addImageLevels2DInconsistentCompressedFormat();
    void addImageLevels2DNotImplemented();

    void addImageLevels3D();
    void addImageLevels3DView();
    void addImageLevels3DCompressedView();
    void addImageLevels3DFailed();
    /* 1D/2D/3D share the same image list validity check function, so it's
       verified only for 2D thoroughly and for others just that the check is
       used */
    void addImageLevels3DInvalidImage();
    void addImageLevels3DNotImplemented();

    void addImage1DThroughLevels();
    void addImage2DThroughLevels();
    void addImage3DThroughLevels();

    void debugFeature();
    void debugFeatures();
    void debugFeaturesSupersets();
    void debugFlag();
    void debugFlags();
};

AbstractSceneConverterTest::AbstractSceneConverterTest() {
    addTests({&AbstractSceneConverterTest::featuresNone,

              &AbstractSceneConverterTest::setFlags,
              &AbstractSceneConverterTest::setFlagsNotImplemented,

              &AbstractSceneConverterTest::thingNotSupported,
              &AbstractSceneConverterTest::thingLevelsNotSupported,

              &AbstractSceneConverterTest::convertMesh,
              &AbstractSceneConverterTest::convertMeshFailed,
              &AbstractSceneConverterTest::convertMeshNotImplemented,
              &AbstractSceneConverterTest::convertMeshNonOwningDeleters,
              &AbstractSceneConverterTest::convertMeshGrowableDeleters,
              &AbstractSceneConverterTest::convertMeshCustomIndexDataDeleter,
              &AbstractSceneConverterTest::convertMeshCustomVertexDataDeleter,
              &AbstractSceneConverterTest::convertMeshCustomAttributeDataDeleter,

              &AbstractSceneConverterTest::convertMeshInPlace,
              &AbstractSceneConverterTest::convertMeshInPlaceFailed,
              &AbstractSceneConverterTest::convertMeshInPlaceNotImplemented,

              &AbstractSceneConverterTest::convertMeshToData,
              &AbstractSceneConverterTest::convertMeshToDataFailed,
              &AbstractSceneConverterTest::convertMeshToDataThroughBatch,
              &AbstractSceneConverterTest::convertMeshToDataThroughBatchAddFailed,
              &AbstractSceneConverterTest::convertMeshToDataThroughBatchEndFailed,
              &AbstractSceneConverterTest::convertMeshToDataNotImplemented,
              &AbstractSceneConverterTest::convertMeshToDataNonOwningDeleter,
              &AbstractSceneConverterTest::convertMeshToDataGrowableDeleter,
              &AbstractSceneConverterTest::convertMeshToDataCustomDeleter,

              &AbstractSceneConverterTest::convertMeshToFile,
              &AbstractSceneConverterTest::convertMeshToFileFailed,
              &AbstractSceneConverterTest::convertMeshToFileThroughData,
              &AbstractSceneConverterTest::convertMeshToFileThroughDataFailed,
              &AbstractSceneConverterTest::convertMeshToFileThroughDataNotWritable,
              &AbstractSceneConverterTest::convertMeshToFileThroughBatch,
              &AbstractSceneConverterTest::convertMeshToFileThroughBatchAddFailed,
              &AbstractSceneConverterTest::convertMeshToFileThroughBatchEndFailed,
              &AbstractSceneConverterTest::convertMeshToFileNotImplemented,

              &AbstractSceneConverterTest::beginEnd,
              &AbstractSceneConverterTest::beginEndFailed,
              &AbstractSceneConverterTest::beginNotImplemented,
              &AbstractSceneConverterTest::endNotImplemented,

              &AbstractSceneConverterTest::beginEndData,
              &AbstractSceneConverterTest::beginEndDataFailed,
              &AbstractSceneConverterTest::beginDataNotImplemented,
              &AbstractSceneConverterTest::endDataNotImplemented,
              &AbstractSceneConverterTest::beginEndDataCustomDeleter,

              &AbstractSceneConverterTest::beginEndFile,
              &AbstractSceneConverterTest::beginEndFileFailed,
              &AbstractSceneConverterTest::beginEndFileThroughData,
              &AbstractSceneConverterTest::beginEndFileThroughDataFailed,
              &AbstractSceneConverterTest::beginEndFileThroughDataNotWritable,
              &AbstractSceneConverterTest::beginFileNotImplemented,
              &AbstractSceneConverterTest::endFileNotImplemented,

              &AbstractSceneConverterTest::abort,
              &AbstractSceneConverterTest::abortNotImplemented,

              &AbstractSceneConverterTest::thingNoBegin,
              &AbstractSceneConverterTest::endMismatchedBegin,
              &AbstractSceneConverterTest::endDataMismatchedBegin,
              &AbstractSceneConverterTest::endFileMismatchedBegin,

              &AbstractSceneConverterTest::addScene,
              &AbstractSceneConverterTest::addSceneFailed,
              &AbstractSceneConverterTest::addSceneNotImplemented,

              &AbstractSceneConverterTest::setSceneFieldName,
              &AbstractSceneConverterTest::setSceneFieldNameNotImplemented,
              &AbstractSceneConverterTest::setSceneFieldNameNotCustom,

              &AbstractSceneConverterTest::setObjectName,
              &AbstractSceneConverterTest::setObjectNameNotImplemented,

              &AbstractSceneConverterTest::setDefaultScene,
              &AbstractSceneConverterTest::setDefaultSceneNotImplemented,
              &AbstractSceneConverterTest::setDefaultSceneOutOfRange,

              &AbstractSceneConverterTest::addAnimation,
              &AbstractSceneConverterTest::addAnimationFailed,
              &AbstractSceneConverterTest::addAnimationNotImplemented,

              &AbstractSceneConverterTest::addLight,
              &AbstractSceneConverterTest::addLightFailed,
              &AbstractSceneConverterTest::addLightNotImplemented,

              &AbstractSceneConverterTest::addCamera,
              &AbstractSceneConverterTest::addCameraFailed,
              &AbstractSceneConverterTest::addCameraNotImplemented,

              &AbstractSceneConverterTest::addSkin2D,
              &AbstractSceneConverterTest::addSkin2DFailed,
              &AbstractSceneConverterTest::addSkin2DNotImplemented,

              &AbstractSceneConverterTest::addSkin3D,
              &AbstractSceneConverterTest::addSkin3DFailed,
              &AbstractSceneConverterTest::addSkin3DNotImplemented,

              &AbstractSceneConverterTest::addMesh,
              &AbstractSceneConverterTest::addMeshFailed,
              &AbstractSceneConverterTest::addMeshThroughConvertMesh,
              &AbstractSceneConverterTest::addMeshThroughConvertMeshFailed,
              &AbstractSceneConverterTest::addMeshThroughConvertMeshZeroMeshes,
              &AbstractSceneConverterTest::addMeshThroughConvertMeshTwoMeshes,
              &AbstractSceneConverterTest::addMeshThroughConvertMeshToData,
              &AbstractSceneConverterTest::addMeshThroughConvertMeshToDataFailed,
              &AbstractSceneConverterTest::addMeshThroughConvertMeshToDataZeroMeshes,
              &AbstractSceneConverterTest::addMeshThroughConvertMeshToDataTwoMeshes,
              &AbstractSceneConverterTest::addMeshThroughConvertMeshToFile,
              &AbstractSceneConverterTest::addMeshThroughConvertMeshToFileThroughData,
              &AbstractSceneConverterTest::addMeshThroughConvertMeshToFileFailed,
              &AbstractSceneConverterTest::addMeshThroughConvertMeshToFileZeroMeshes,
              &AbstractSceneConverterTest::addMeshThroughConvertMeshToFileTwoMeshes,
              &AbstractSceneConverterTest::addMeshNotImplemented,

              &AbstractSceneConverterTest::addMeshLevels,
              &AbstractSceneConverterTest::addMeshLevelsFailed,
              &AbstractSceneConverterTest::addMeshLevelsNoLevels,
              &AbstractSceneConverterTest::addMeshLevelsNotImplemented,

              &AbstractSceneConverterTest::addMeshThroughLevels,

              &AbstractSceneConverterTest::setMeshAttributeName,
              &AbstractSceneConverterTest::setMeshAttributeNameNotImplemented,
              &AbstractSceneConverterTest::setMeshAttributeNameNotCustom,

              &AbstractSceneConverterTest::addMaterial,
              &AbstractSceneConverterTest::addMaterialFailed,
              &AbstractSceneConverterTest::addMaterialNotImplemented,

              &AbstractSceneConverterTest::addTexture,
              &AbstractSceneConverterTest::addTextureFailed,
              &AbstractSceneConverterTest::addTextureNotImplemented,

              &AbstractSceneConverterTest::addImage1D,
              &AbstractSceneConverterTest::addImage1DView,
              &AbstractSceneConverterTest::addImage1DCompressedView,
              &AbstractSceneConverterTest::addImage1DFailed,
              &AbstractSceneConverterTest::addImage1DInvalidImage,
              &AbstractSceneConverterTest::addImage1DNotImplemented,

              &AbstractSceneConverterTest::addImage2D,
              &AbstractSceneConverterTest::addImage2DView,
              &AbstractSceneConverterTest::addImage2DCompressedView,
              &AbstractSceneConverterTest::addImage2DFailed,
              &AbstractSceneConverterTest::addImage2DZeroSize,
              &AbstractSceneConverterTest::addImage2DNullptr,
              &AbstractSceneConverterTest::addImage2DNotImplemented,

              &AbstractSceneConverterTest::addImage3D,
              &AbstractSceneConverterTest::addImage3DView,
              &AbstractSceneConverterTest::addImage3DCompressedView,
              &AbstractSceneConverterTest::addImage3DFailed,
              &AbstractSceneConverterTest::addImage3DInvalidImage,
              &AbstractSceneConverterTest::addImage3DNotImplemented,

              &AbstractSceneConverterTest::addImageLevels1D,
              &AbstractSceneConverterTest::addImageLevels1DView,
              &AbstractSceneConverterTest::addImageLevels1DCompressedView,
              &AbstractSceneConverterTest::addImageLevels1DFailed,
              &AbstractSceneConverterTest::addImageLevels1DInvalidImage,
              &AbstractSceneConverterTest::addImageLevels1DNotImplemented,

              &AbstractSceneConverterTest::addImageLevels2D,
              &AbstractSceneConverterTest::addImageLevels2DView,
              &AbstractSceneConverterTest::addImageLevels2DCompressedView,
              &AbstractSceneConverterTest::addImageLevels2DFailed,
              &AbstractSceneConverterTest::addImageLevels2DNoLevels,
              &AbstractSceneConverterTest::addImageLevels2DZeroSize,
              &AbstractSceneConverterTest::addImageLevels2DNullptr,
              &AbstractSceneConverterTest::addImageLevels2DInconsistentCompressed,
              &AbstractSceneConverterTest::addImageLevels2DInconsistentFormat,
              &AbstractSceneConverterTest::addImageLevels2DInconsistentFormatExtra,
              &AbstractSceneConverterTest::addImageLevels2DInconsistentCompressedFormat,
              &AbstractSceneConverterTest::addImageLevels2DNotImplemented,

              &AbstractSceneConverterTest::addImageLevels3D,
              &AbstractSceneConverterTest::addImageLevels3DView,
              &AbstractSceneConverterTest::addImageLevels3DCompressedView,
              &AbstractSceneConverterTest::addImageLevels3DFailed,
              &AbstractSceneConverterTest::addImageLevels3DInvalidImage,
              &AbstractSceneConverterTest::addImageLevels3DNotImplemented,

              &AbstractSceneConverterTest::addImage1DThroughLevels,
              &AbstractSceneConverterTest::addImage2DThroughLevels,
              &AbstractSceneConverterTest::addImage3DThroughLevels,

              &AbstractSceneConverterTest::debugFeature,
              &AbstractSceneConverterTest::debugFeatures,
              &AbstractSceneConverterTest::debugFeaturesSupersets,
              &AbstractSceneConverterTest::debugFlag,
              &AbstractSceneConverterTest::debugFlags});

    /* Create testing dir */
    Utility::Path::make(TRADE_TEST_OUTPUT_DIR);
}

void AbstractSceneConverterTest::featuresNone() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return {}; }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.features();
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::features(): implementation reported no features\n");
}

void AbstractSceneConverterTest::setFlags() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            /* Assuming this bit is unused */
            return SceneConverterFeature(1 << 7);
        }
        void doSetFlags(SceneConverterFlags flags) override {
            _flags = flags;
        }

        SceneConverterFlags _flags;
    } converter;
    CORRADE_COMPARE(converter.flags(), SceneConverterFlags{});
    CORRADE_COMPARE(converter._flags, SceneConverterFlags{});

    converter.setFlags(SceneConverterFlag::Verbose);
    CORRADE_COMPARE(converter.flags(), SceneConverterFlag::Verbose);
    CORRADE_COMPARE(converter._flags, SceneConverterFlag::Verbose);

    /** @todo use a real flag when we have more than one */
    converter.addFlags(SceneConverterFlag(4));
    CORRADE_COMPARE(converter.flags(), SceneConverterFlag::Verbose|SceneConverterFlag(4));
    CORRADE_COMPARE(converter._flags, SceneConverterFlag::Verbose|SceneConverterFlag(4));

    converter.clearFlags(SceneConverterFlag::Verbose);
    CORRADE_COMPARE(converter.flags(), SceneConverterFlag(4));
    CORRADE_COMPARE(converter._flags, SceneConverterFlag(4));
}

void AbstractSceneConverterTest::setFlagsNotImplemented() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            /* Assuming this bit is unused */
            return SceneConverterFeature(1 << 7);
        }
    } converter;

    CORRADE_COMPARE(converter.flags(), SceneConverterFlags{});
    converter.setFlags(SceneConverterFlag::Verbose);
    CORRADE_COMPARE(converter.flags(), SceneConverterFlag::Verbose);
    /* Should just work, no need to implement the function */
}

void AbstractSceneConverterTest::thingNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            /* Assuming this bit is unused */
            return SceneConverterFeature(1u << 31);
        }
    } converter;

    MeshData mesh{MeshPrimitive::Triangles, 0};

    const char imageData[4*4]{};
    ImageData1D image1D{PixelFormat::RGBA8Unorm, 1, DataFlags{}, imageData};
    ImageData1D compressedImage1D{CompressedPixelFormat::Astc4x4RGBAF, 1, DataFlags{}, imageData};
    ImageData2D image2D{PixelFormat::RGBA8Unorm, {1, 1}, DataFlags{}, imageData};
    ImageData2D compressedImage2D{CompressedPixelFormat::Astc4x4RGBAF, {1, 1}, DataFlags{}, imageData};
    ImageData3D image3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, DataFlags{}, imageData};
    ImageData3D compressedImage3D{CompressedPixelFormat::Astc4x4RGBAF, {1, 1, 1}, DataFlags{}, imageData};

    std::ostringstream out;
    Error redirectError{&out};
    converter.convert(mesh);
    converter.convertInPlace(mesh);
    converter.convertToData(mesh);
    converter.convertToFile(mesh, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "mesh.out"));

    converter.begin();
    converter.beginData();
    converter.beginFile(Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "mesh.out"));

    converter.add(SceneData{SceneMappingType::UnsignedInt, 0, nullptr, nullptr});
    converter.setSceneFieldName({}, {});
    converter.setObjectName(0, {});
    converter.setDefaultScene(0);

    converter.add(AnimationData{nullptr, nullptr});
    converter.add(LightData{LightData::Type::Point, {}, 0.0f});
    converter.add(CameraData{CameraType::Orthographic3D, {}, 0.0f, 1.0f});
    converter.add(SkinData2D{nullptr, nullptr});
    converter.add(SkinData3D{nullptr, nullptr});

    converter.add(mesh);
    converter.add({mesh, mesh});
    converter.setMeshAttributeName({}, {});

    converter.add(MaterialData{{}, nullptr});
    converter.add(TextureData{{}, {}, {}, {}, {}, 0});

    converter.add(image1D);
    converter.add(compressedImage1D);
    converter.add({image1D, image1D});
    converter.add({compressedImage1D, compressedImage1D});

    converter.add(image2D);
    converter.add(compressedImage2D);
    converter.add({image2D, image2D});
    converter.add({compressedImage2D, compressedImage2D});

    converter.add(image3D);
    converter.add(compressedImage3D);
    converter.add({image3D, image3D});
    converter.add({compressedImage3D, compressedImage3D});

    CORRADE_COMPARE(out.str(),
        "Trade::AbstractSceneConverter::convert(): mesh conversion not supported\n"
        "Trade::AbstractSceneConverter::convertInPlace(): mesh conversion not supported\n"
        "Trade::AbstractSceneConverter::convertToData(): mesh conversion not supported\n"
        "Trade::AbstractSceneConverter::convertToFile(): mesh conversion not supported\n"

        "Trade::AbstractSceneConverter::begin(): feature not supported\n"
        "Trade::AbstractSceneConverter::beginData(): feature not supported\n"
        "Trade::AbstractSceneConverter::beginFile(): feature not supported\n"

        "Trade::AbstractSceneConverter::add(): scene conversion not supported\n"
        "Trade::AbstractSceneConverter::setSceneFieldName(): feature not supported\n"
        "Trade::AbstractSceneConverter::setObjectName(): feature not supported\n"
        "Trade::AbstractSceneConverter::setDefaultScene(): feature not supported\n"

        "Trade::AbstractSceneConverter::add(): animation conversion not supported\n"
        "Trade::AbstractSceneConverter::add(): light conversion not supported\n"
        "Trade::AbstractSceneConverter::add(): camera conversion not supported\n"
        "Trade::AbstractSceneConverter::add(): 2D skin conversion not supported\n"
        "Trade::AbstractSceneConverter::add(): 3D skin conversion not supported\n"

        "Trade::AbstractSceneConverter::add(): mesh conversion not supported\n"
        "Trade::AbstractSceneConverter::add(): multi-level mesh conversion not supported\n"
        "Trade::AbstractSceneConverter::setMeshAttributeName(): feature not supported\n"

        "Trade::AbstractSceneConverter::add(): material conversion not supported\n"
        "Trade::AbstractSceneConverter::add(): texture conversion not supported\n"

        "Trade::AbstractSceneConverter::add(): 1D image conversion not supported\n"
        "Trade::AbstractSceneConverter::add(): compressed 1D image conversion not supported\n"
        "Trade::AbstractSceneConverter::add(): multi-level 1D image conversion not supported\n"
        "Trade::AbstractSceneConverter::add(): multi-level compressed 1D image conversion not supported\n"

        "Trade::AbstractSceneConverter::add(): 2D image conversion not supported\n"
        "Trade::AbstractSceneConverter::add(): compressed 2D image conversion not supported\n"
        "Trade::AbstractSceneConverter::add(): multi-level 2D image conversion not supported\n"
        "Trade::AbstractSceneConverter::add(): multi-level compressed 2D image conversion not supported\n"

        "Trade::AbstractSceneConverter::add(): 3D image conversion not supported\n"
        "Trade::AbstractSceneConverter::add(): compressed 3D image conversion not supported\n"
        "Trade::AbstractSceneConverter::add(): multi-level 3D image conversion not supported\n"
        "Trade::AbstractSceneConverter::add(): multi-level compressed 3D image conversion not supported\n");
}

void AbstractSceneConverterTest::thingLevelsNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddMeshes|
                   SceneConverterFeature::AddImages1D|
                   SceneConverterFeature::AddImages2D|
                   SceneConverterFeature::AddImages3D|
                   SceneConverterFeature::AddCompressedImages1D|
                   SceneConverterFeature::AddCompressedImages2D|
                   SceneConverterFeature::AddCompressedImages3D;
        }
    } converter;

    MeshData mesh{MeshPrimitive::Triangles, 3};

    const char imageData[4*4]{};
    ImageData1D image1D{PixelFormat::RGBA8Unorm, 1, DataFlags{}, imageData};
    ImageData1D compressedImage1D{CompressedPixelFormat::Astc4x4RGBAF, 1, DataFlags{}, imageData};
    ImageData2D image2D{PixelFormat::RGBA8Unorm, {1, 1}, DataFlags{}, imageData};
    ImageData2D compressedImage2D{CompressedPixelFormat::Astc4x4RGBAF, {1, 1}, DataFlags{}, imageData};
    ImageData3D image3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, DataFlags{}, imageData};
    ImageData3D compressedImage3D{CompressedPixelFormat::Astc4x4RGBAF, {1, 1, 1}, DataFlags{}, imageData};

    std::ostringstream out;
    Error redirectError{&out};
    converter.add({mesh, mesh});
    converter.add({image1D, image1D});
    converter.add({compressedImage1D, compressedImage1D});
    converter.add({image2D, image2D});
    converter.add({compressedImage2D, compressedImage2D});
    converter.add({image3D, image3D});
    converter.add({compressedImage3D, compressedImage3D});
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractSceneConverter::add(): multi-level mesh conversion not supported\n"
        "Trade::AbstractSceneConverter::add(): multi-level 1D image conversion not supported\n"
        "Trade::AbstractSceneConverter::add(): multi-level compressed 1D image conversion not supported\n"
        "Trade::AbstractSceneConverter::add(): multi-level 2D image conversion not supported\n"
        "Trade::AbstractSceneConverter::add(): multi-level compressed 2D image conversion not supported\n"
        "Trade::AbstractSceneConverter::add(): multi-level 3D image conversion not supported\n"
        "Trade::AbstractSceneConverter::add(): multi-level compressed 3D image conversion not supported\n");
}

void AbstractSceneConverterTest::convertMesh() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMesh; }

        Containers::Optional<MeshData> doConvert(const MeshData& mesh) override {
            CORRADE_COMPARE(mesh.primitive(), MeshPrimitive::Triangles);
            return MeshData{MeshPrimitive::Lines, mesh.vertexCount()*2};
        }
    } converter;

    CORRADE_VERIFY(true); /* capture correct function name */

    Containers::Optional<MeshData> out = converter.convert(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_VERIFY(out);
    CORRADE_COMPARE(out->primitive(), MeshPrimitive::Lines);
    CORRADE_COMPARE(out->vertexCount(), 12);
}

void AbstractSceneConverterTest::convertMeshFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMesh;
        }

        Containers::Optional<MeshData> doConvert(const MeshData&) override {
            return {};
        }
    } converter;

    /* The implementation is expected to print an error message on its own */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convert(MeshData{MeshPrimitive::Triangles, 0}));
    CORRADE_COMPARE(out.str(), "");
}

void AbstractSceneConverterTest::convertMeshNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMesh; }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convert(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::convert(): mesh conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::convertMeshNonOwningDeleters() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMesh; }

        Containers::Optional<MeshData> doConvert(const MeshData&) override {
            return MeshData{MeshPrimitive::Triangles,
                Containers::Array<char>{indexData, 1, Implementation::nonOwnedArrayDeleter}, MeshIndexData{MeshIndexType::UnsignedByte, indexData},
                Containers::Array<char>{nullptr, 0, Implementation::nonOwnedArrayDeleter},
                meshAttributeDataNonOwningArray(attributes)};
        }

        char indexData[1];
        MeshAttributeData attributes[1]{
            MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector3, nullptr}
        };
    } converter;

    Containers::Optional<MeshData> out = converter.convert(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_VERIFY(out);
    CORRADE_COMPARE(static_cast<const void*>(out->indexData()), converter.indexData);
}

void AbstractSceneConverterTest::convertMeshGrowableDeleters() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMesh; }

        Containers::Optional<MeshData> doConvert(const MeshData&) override {
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

        char indexData[1];
        MeshAttributeData attributes[1]{
            MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector3, nullptr}
        };
    } converter;

    Containers::Optional<MeshData> out = converter.convert(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_VERIFY(out);
    CORRADE_COMPARE(out->indexData()[0], '\xab');
    CORRADE_COMPARE(out->vertexData().size(), 12);
}

void AbstractSceneConverterTest::convertMeshCustomIndexDataDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMesh; }

        Containers::Optional<MeshData> doConvert(const MeshData&) override {
            return MeshData{MeshPrimitive::Triangles, Containers::Array<char>{data, 1, [](char*, std::size_t) {}}, MeshIndexData{MeshIndexType::UnsignedByte, data}, 1};
        }

        char data[1];
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convert(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractSceneConverter::convert(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractSceneConverterTest::convertMeshCustomVertexDataDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMesh; }

        Containers::Optional<MeshData> doConvert(const MeshData&) override {
            return MeshData{MeshPrimitive::Triangles, Containers::Array<char>{data, 1, [](char*, std::size_t) {}}, MeshIndexData{MeshIndexType::UnsignedByte, data}, 1};
        }

        char data[1];
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convert(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractSceneConverter::convert(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractSceneConverterTest::convertMeshCustomAttributeDataDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMesh; }

        Containers::Optional<MeshData> doConvert(const MeshData&) override {
            return MeshData{MeshPrimitive::Triangles, Containers::Array<char>{data, 1, [](char*, std::size_t) {}}, MeshIndexData{MeshIndexType::UnsignedByte, data}, 1};
        }

        char data[1];
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convert(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractSceneConverter::convert(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractSceneConverterTest::convertMeshInPlace() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMeshInPlace; }

        bool doConvertInPlace(MeshData& mesh) override {
            auto indices = mesh.mutableIndices<UnsignedInt>();
            for(std::size_t i = 0; i != indices.size()/2; ++i)
                std::swap(indices[i], indices[indices.size() - i -1]);
            return true;
        }
    } converter;

    UnsignedInt indices[]{1, 2, 3, 4, 2, 0};
    MeshData mesh{MeshPrimitive::Triangles,
        DataFlag::Mutable, indices, MeshIndexData{indices}, 5};
    CORRADE_VERIFY(converter.convertInPlace(mesh));
    CORRADE_COMPARE_AS(mesh.indices<UnsignedInt>(),
        Containers::arrayView<UnsignedInt>({0, 2, 4, 3, 2, 1}),
        TestSuite::Compare::Container);
}

void AbstractSceneConverterTest::convertMeshInPlaceFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMeshInPlace;
        }

        bool doConvertInPlace(MeshData&) override {
            return {};
        }
    } converter;

    MeshData mesh{MeshPrimitive::Triangles, 0};

    /* The implementation is expected to print an error message on its own */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertInPlace(mesh));
    CORRADE_COMPARE(out.str(), "");
}

void AbstractSceneConverterTest::convertMeshInPlaceNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMeshInPlace; }
    } converter;

    MeshData mesh{MeshPrimitive::Triangles, 3};

    std::ostringstream out;
    Error redirectError{&out};
    converter.convertInPlace(mesh);
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::convertInPlace(): mesh conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::convertMeshToData() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMeshToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const MeshData& mesh) override {
            return Containers::Array<char>{nullptr, mesh.vertexCount()};
        }
    } converter;

    Containers::Optional<Containers::Array<char>> data = converter.convertToData(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->size(), 6);
}

void AbstractSceneConverterTest::convertMeshToDataFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMeshToData;
        }

        Containers::Optional<Containers::Array<char>> doConvertToData(const MeshData&) override {
            return {};
        }
    } converter;

    /* The implementation is expected to print an error message on its own */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToData(MeshData{MeshPrimitive::Triangles, 0}));
    CORRADE_COMPARE(out.str(), "");
}

void AbstractSceneConverterTest::convertMeshToDataThroughBatch() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddMeshes;
        }

        void doBeginData() override {}

        bool doAdd(UnsignedInt id, const MeshData& mesh, Containers::StringView) override {
            CORRADE_COMPARE(id, 0);
            CORRADE_COMPARE(mesh.primitive(), MeshPrimitive::Triangles);
            _vertexCount = mesh.vertexCount();
            return true;
        }

        Containers::Optional<Containers::Array<char>> doEndData() override {
            return Containers::Array<char>{nullptr, _vertexCount};
        }

        std::size_t _vertexCount = 0;
    } converter;

    Containers::Optional<Containers::Array<char>> data = converter.convertToData(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->size(), 6);
}

void AbstractSceneConverterTest::convertMeshToDataThroughBatchAddFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddMeshes;
        }

        void doBeginData() override {}

        bool doAdd(UnsignedInt, const MeshData&, Containers::StringView) override {
            return false;
        }

        Containers::Optional<Containers::Array<char>> doEndData() override {
            CORRADE_FAIL_IF(true, "doEndData() shouldn't be called");
            return {};
        }
    } converter;

    /* The implementation is expected to print an error message on its own */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToData(MeshData{MeshPrimitive::Triangles, 6}));
    CORRADE_COMPARE(out.str(), "");
}

void AbstractSceneConverterTest::convertMeshToDataThroughBatchEndFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddMeshes;
        }

        void doBeginData() override {}

        bool doAdd(UnsignedInt, const MeshData&, Containers::StringView) override {
            return true;
        }

        Containers::Optional<Containers::Array<char>> doEndData() override {
            return {};
        }
    } converter;

    /* The implementation is expected to print an error message on its own */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToData(MeshData{MeshPrimitive::Triangles, 6}));
    CORRADE_COMPARE(out.str(), "");
}

void AbstractSceneConverterTest::convertMeshToDataNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMeshToData; }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convertToData(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::convertToData(): mesh conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::convertMeshToDataNonOwningDeleter() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMeshToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const MeshData&) override {
            return Containers::Array<char>{data, 5, Implementation::nonOwnedArrayDeleter};
        }

        char data[5]{'h', 'e', 'l', 'l', 'o'};
    } converter;

    Containers::Optional<Containers::Array<char>> data = converter.convertToData(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_VERIFY(data);
    CORRADE_COMPARE_AS(*data,
        Containers::arrayView({'h', 'e', 'l', 'l', 'o'}),
        TestSuite::Compare::Container);
}

void AbstractSceneConverterTest::convertMeshToDataGrowableDeleter() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMeshToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const MeshData&) override {
            Containers::Array<char> out;
            Containers::arrayAppend<ArrayAllocator>(out, {'h', 'e', 'l', 'l', 'o'});

            /* GCC 4.8 and Clang 3.8 need extra help here */
            return Containers::optional(std::move(out));
        }
    } converter;

    Containers::Optional<Containers::Array<char>> data = converter.convertToData(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_VERIFY(data);
    CORRADE_COMPARE_AS(*data,
        Containers::arrayView({'h', 'e', 'l', 'l', 'o'}),
        TestSuite::Compare::Container);
}

void AbstractSceneConverterTest::convertMeshToDataCustomDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMeshToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const MeshData&) override {
            return Containers::Array<char>{data, 1, [](char*, std::size_t) {}};
        }

        char data[1];
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convertToData(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::convertToData(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractSceneConverterTest::convertMeshToFile() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMeshToFile; }

        bool doConvertToFile(const MeshData& mesh, Containers::StringView filename) override {
            return Utility::Path::write(filename, Containers::arrayView(               {char(mesh.vertexCount())}));
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "mesh.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    CORRADE_VERIFY(converter.convertToFile(MeshData{MeshPrimitive::Triangles, 0xef}, filename));
    CORRADE_COMPARE_AS(filename,
        "\xef", TestSuite::Compare::FileToString);
}

void AbstractSceneConverterTest::convertMeshToFileFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMeshToFile;
        }

        bool doConvertToFile(const MeshData&, Containers::StringView) override {
            return false;
        }
    } converter;

    /* The implementation is expected to print an error message on its own */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile(MeshData{MeshPrimitive::Triangles, 0}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "mesh.out")));
    CORRADE_COMPARE(out.str(), "");
}

void AbstractSceneConverterTest::convertMeshToFileThroughData() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMeshToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const MeshData& mesh) override {
            return Containers::array({char(mesh.vertexCount())});
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "mesh.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    /* doConvertToFile() should call doConvertToData() */
    CORRADE_VERIFY(converter.convertToFile(MeshData{MeshPrimitive::Triangles, 0xef}, filename));
    CORRADE_COMPARE_AS(filename,
        "\xef", TestSuite::Compare::FileToString);
}

void AbstractSceneConverterTest::convertMeshToFileThroughDataFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMeshToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const MeshData&) override {
            return {};
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "mesh.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    /* Function should fail, no file should get written and no error output
       should be printed (the base implementation assumes the plugin does it) */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile(MeshData{MeshPrimitive::Triangles, 0xef}, filename));
    CORRADE_VERIFY(!Utility::Path::exists(filename));
    CORRADE_COMPARE(out.str(), "");
}

void AbstractSceneConverterTest::convertMeshToFileThroughDataNotWritable() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMeshToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const MeshData& mesh) override {
            return Containers::array({char(mesh.vertexCount())});
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile(MeshData{MeshPrimitive::Triangles, 0xef}, "/some/path/that/does/not/exist"));
    /* There's an error from Path::write() before */
    CORRADE_COMPARE_AS(out.str(),
        "\nTrade::AbstractSceneConverter::convertToFile(): cannot write to file /some/path/that/does/not/exist\n",
        TestSuite::Compare::StringHasSuffix);
}

void AbstractSceneConverterTest::convertMeshToFileThroughBatch() {
}

void AbstractSceneConverterTest::convertMeshToFileThroughBatchAddFailed() {
}

void AbstractSceneConverterTest::convertMeshToFileThroughBatchEndFailed() {
}

void AbstractSceneConverterTest::convertMeshToFileNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMeshToFile; }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convertToFile(MeshData{MeshPrimitive::Triangles, 6}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "mesh.out"));
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::convertToFile(): mesh conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::beginEnd() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple;
        }

        void doBegin() override {
            CORRADE_VERIFY(!beginCalled);
            beginCalled = true;
        }

        Containers::Pointer<AbstractImporter> doEnd() override {
            CORRADE_VERIFY(!endCalled);
            endCalled = true;

            struct Importer: AbstractImporter {
                ImporterFeatures doFeatures() const override { return {}; }
                void doClose() override {}
                bool doIsOpened() const override { return true; }

                const void* doImporterState() const override {
                    return reinterpret_cast<const void*>(0xdeadbeef);
                }
            };

            return Containers::Pointer<AbstractImporter>{new Importer};
        }

        bool beginCalled = false, endCalled = false;
    } converter;

    CORRADE_VERIFY(!converter.isConverting());
    converter.begin();
    CORRADE_VERIFY(converter.beginCalled);
    CORRADE_VERIFY(!converter.endCalled);
    CORRADE_VERIFY(converter.isConverting());

    Containers::Pointer<AbstractImporter> out = converter.end();
    CORRADE_VERIFY(out);
    CORRADE_COMPARE(out->importerState(), reinterpret_cast<const void*>(0xdeadbeef));
    CORRADE_VERIFY(converter.endCalled);
    CORRADE_VERIFY(!converter.isConverting());
}

void AbstractSceneConverterTest::beginEndFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple;
        }

        void doBegin() override {}

        Containers::Pointer<AbstractImporter> doEnd() override {
            return nullptr;
        }
    } converter;

    converter.begin();

    /* The implementation is expected to print an error message on its own */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.end());
    CORRADE_COMPARE(out.str(), "");

    CORRADE_VERIFY(!converter.isConverting());
}

void AbstractSceneConverterTest::beginNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple;
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.begin();
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractSceneConverter::begin(): feature advertised but not implemented\n");
}

void AbstractSceneConverterTest::endNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple;
        }

        void doBegin() override {}
    } converter;

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.end();
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractSceneConverter::end(): feature advertised but not implemented\n");
}

void AbstractSceneConverterTest::beginEndData() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToData;
        }

        void doBeginData() override {
            CORRADE_VERIFY(!beginCalled);
            beginCalled = true;
        }

        Containers::Optional<Containers::Array<char>> doEndData() override {
            CORRADE_VERIFY(!endCalled);
            endCalled = true;
            return Containers::array({'h', 'e', 'l', 'l', 'o'});
        }

        bool beginCalled = false, endCalled = false;
    } converter;

    CORRADE_VERIFY(!converter.isConverting());
    converter.beginData();
    CORRADE_VERIFY(converter.beginCalled);
    CORRADE_VERIFY(!converter.endCalled);
    CORRADE_VERIFY(converter.isConverting());

    Containers::Optional<Containers::Array<char>> out = converter.endData();
    CORRADE_VERIFY(out);
    CORRADE_COMPARE_AS(*out,
        Containers::arrayView({'h', 'e', 'l', 'l', 'o'}),
        TestSuite::Compare::Container);
    CORRADE_VERIFY(converter.endCalled);
    CORRADE_VERIFY(!converter.isConverting());
}

void AbstractSceneConverterTest::beginEndDataFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToData;
        }

        void doBeginData() override {}

        Containers::Optional<Containers::Array<char>> doEndData() override {
            return {};
        }
    } converter;

    converter.beginData();

    /* The implementation is expected to print an error message on its own */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.endData());
    CORRADE_COMPARE(out.str(), "");

    CORRADE_VERIFY(!converter.isConverting());
}

void AbstractSceneConverterTest::beginDataNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToData;
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.beginData();
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractSceneConverter::beginData(): feature advertised but not implemented\n");
}

void AbstractSceneConverterTest::endDataNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToData;
        }

        void doBeginData() override {}
    } converter;

    converter.beginData();

    std::ostringstream out;
    Error redirectError{&out};
    converter.endData();
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractSceneConverter::endData(): feature advertised but not implemented\n");
}

void AbstractSceneConverterTest::beginEndDataCustomDeleter() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToData;
        }

        void doBeginData() override {}

        Containers::Optional<Containers::Array<char>> doEndData() override {
            return Containers::Array<char>{data, 1, [](char*, std::size_t) {}};
        }

        char data[1];
    } converter;

    converter.beginData();

    std::ostringstream out;
    Error redirectError{&out};
    converter.endData();
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::endData(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractSceneConverterTest::beginEndFile() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToFile;
        }

        void doBeginFile(Containers::StringView filename) override {
            CORRADE_VERIFY(!beginCalled);
            beginCalled = true;
            CORRADE_COMPARE(filename, "file.gltf");
        }

        bool doEndFile(Containers::StringView filename) override {
            CORRADE_VERIFY(!endCalled);
            endCalled = true;
            CORRADE_COMPARE(filename, "file.gltf");
            return true;
        }

        bool beginCalled = false, endCalled = false;
    } converter;

    CORRADE_VERIFY(!converter.isConverting());
    converter.beginFile("file.gltf");
    CORRADE_VERIFY(converter.beginCalled);
    CORRADE_VERIFY(!converter.endCalled);
    CORRADE_VERIFY(converter.isConverting());

    CORRADE_VERIFY(converter.endFile());
    CORRADE_VERIFY(converter.endCalled);
    CORRADE_VERIFY(!converter.isConverting());
}

void AbstractSceneConverterTest::beginEndFileFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToFile;
        }

        void doBeginFile(Containers::StringView) override {}

        bool doEndFile(Containers::StringView) override {
            return false;
        }
    } converter;

    converter.beginFile("file.gltf");

    /* The implementation is expected to print an error message on its own */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.endFile());
    CORRADE_COMPARE(out.str(), "");

    CORRADE_VERIFY(!converter.isConverting());
}

void AbstractSceneConverterTest::beginEndFileThroughData() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToData;
        }

        void doBeginData() override {}

        Containers::Optional<Containers::Array<char>> doEndData() override {
            return Containers::array({'h', 'e', 'l', 'l', 'o'});
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "mesh.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    converter.beginFile(filename);

    /* doEndFile() should call doEndData() */
    CORRADE_VERIFY(converter.endFile());
    CORRADE_COMPARE_AS(filename, "hello", TestSuite::Compare::FileToString);
}

void AbstractSceneConverterTest::beginEndFileThroughDataFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToData;
        }

        void doBeginData() override {}

        Containers::Optional<Containers::Array<char>> doEndData() override {
            return {};
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "mesh.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    converter.beginFile(filename);

    /* Function should fail, no file should get written and no error output
       should be printed (the base implementation assumes the plugin does it) */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.endFile());
    CORRADE_VERIFY(!Utility::Path::exists(filename));
    CORRADE_COMPARE(out.str(), "");
}

void AbstractSceneConverterTest::beginEndFileThroughDataNotWritable() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToData;
        }

        void doBeginData() override {}

        Containers::Optional<Containers::Array<char>> doEndData() override {
            return Containers::array({'h', 'e', 'l', 'l', 'o'});
        }
    } converter;

    converter.beginFile("/some/path/that/does/not/exist");

    /* Function should fail, no file should get written and no error output
       should be printed (the base implementation assumes the plugin does it) */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.endFile());
    /* There's an error from Path::write() before */
    CORRADE_COMPARE_AS(out.str(),
        "\nTrade::AbstractSceneConverter::endFile(): cannot write to file /some/path/that/does/not/exist\n",
        TestSuite::Compare::StringHasSuffix);
}

void AbstractSceneConverterTest::beginFileNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToFile;
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.beginFile("file.gltf");
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::beginFile(): feature advertised but not implemented\n");
}

void AbstractSceneConverterTest::endFileNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToFile;
        }

        void doBeginFile(Containers::StringView) override {}
    } converter;

    converter.beginFile("file.gltf");

    std::ostringstream out;
    Error redirectError{&out};
    converter.endFile();
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::endFile(): feature advertised but not implemented\n");
}

void AbstractSceneConverterTest::abort() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple;
        }

        void doBegin() override {}

        void doAbort() override {
            CORRADE_VERIFY(!abortCalled);
            abortCalled = true;
        }

        bool abortCalled = false;
    } converter;

    CORRADE_VERIFY(!converter.abortCalled);
    converter.begin();
    CORRADE_VERIFY(!converter.abortCalled);
    CORRADE_VERIFY(converter.isConverting());
    converter.abort();
    CORRADE_VERIFY(converter.abortCalled);
    CORRADE_VERIFY(!converter.isConverting());
}

void AbstractSceneConverterTest::abortNotImplemented() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple;
        }

        void doBegin() override {}
    } converter;

    /* This should work, there's no need for a plugin to implement this */
    converter.begin();
    CORRADE_VERIFY(converter.isConverting());
    converter.abort();
    CORRADE_VERIFY(!converter.isConverting());
}

void AbstractSceneConverterTest::thingNoBegin() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::AddScenes|
                   SceneConverterFeature::AddAnimations|
                   SceneConverterFeature::AddLights|
                   SceneConverterFeature::AddCameras|
                   SceneConverterFeature::AddSkins2D|
                   SceneConverterFeature::AddSkins3D|
                   SceneConverterFeature::AddMeshes|
                   SceneConverterFeature::AddMaterials|
                   SceneConverterFeature::AddTextures|
                   SceneConverterFeature::AddImages1D|
                   SceneConverterFeature::AddImages2D|
                   SceneConverterFeature::AddImages3D|
                   SceneConverterFeature::AddCompressedImages1D|
                   SceneConverterFeature::AddCompressedImages2D|
                   SceneConverterFeature::AddCompressedImages3D|
                   SceneConverterFeature::MeshLevels|
                   SceneConverterFeature::ImageLevels;
        }

        void doBeginData() override {}
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.end();
    converter.endData();
    converter.endFile();

    converter.sceneCount();
    converter.add(SceneData{SceneMappingType::UnsignedInt, 0, nullptr, nullptr});
    converter.setSceneFieldName({}, {});
    converter.setObjectName(0, {});
    converter.setDefaultScene(0);

    converter.animationCount();
    converter.add(AnimationData{nullptr, nullptr});

    converter.lightCount();
    converter.add(LightData{LightData::Type::Point, {}, 0.0f});

    converter.cameraCount();
    converter.add(CameraData{CameraType::Orthographic3D, {}, 0.0f, 1.0f});

    converter.skin2DCount();
    converter.add(SkinData2D{nullptr, nullptr});

    converter.skin3DCount();
    converter.add(SkinData3D{nullptr, nullptr});

    converter.meshCount();
    converter.add(MeshData{MeshPrimitive::Triangles, 0});
    converter.add({MeshData{MeshPrimitive::Triangles, 0},
                   MeshData{MeshPrimitive::Triangles, 0}});
    converter.setMeshAttributeName({}, {});

    converter.materialCount();
    converter.add(MaterialData{{}, nullptr});

    converter.textureCount();
    converter.add(TextureData{{}, {}, {}, {}, {}, 0});

    const char imageData[4]{};

    converter.image1DCount();
    converter.add(ImageData1D{PixelFormat::RGBA8Unorm, 1, DataFlags{}, imageData});
    converter.add({ImageData1D{PixelFormat::RGBA8Unorm, 1, DataFlags{}, imageData},
                   ImageData1D{PixelFormat::RGBA8Unorm, 1, DataFlags{}, imageData}});

    converter.image2DCount();
    converter.add(ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, DataFlags{}, imageData});
    converter.add({ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, DataFlags{}, imageData},
                   ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, DataFlags{}, imageData}});

    converter.image3DCount();
    converter.add(ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, DataFlags{}, imageData});
    converter.add({ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, DataFlags{}, imageData},
                   ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, DataFlags{}, imageData}});

    CORRADE_COMPARE(out.str(),
        "Trade::AbstractSceneConverter::end(): no conversion in progress\n"
        "Trade::AbstractSceneConverter::endData(): no data conversion in progress\n"
        "Trade::AbstractSceneConverter::endFile(): no file conversion in progress\n"

        "Trade::AbstractSceneConverter::sceneCount(): no conversion in progress\n"
        "Trade::AbstractSceneConverter::add(): no conversion in progress\n"
        "Trade::AbstractSceneConverter::setSceneFieldName(): no conversion in progress\n"
        "Trade::AbstractSceneConverter::setObjectName(): no conversion in progress\n"
        "Trade::AbstractSceneConverter::setDefaultScene(): no conversion in progress\n"

        "Trade::AbstractSceneConverter::animationCount(): no conversion in progress\n"
        "Trade::AbstractSceneConverter::add(): no conversion in progress\n"

        "Trade::AbstractSceneConverter::lightCount(): no conversion in progress\n"
        "Trade::AbstractSceneConverter::add(): no conversion in progress\n"

        "Trade::AbstractSceneConverter::cameraCount(): no conversion in progress\n"
        "Trade::AbstractSceneConverter::add(): no conversion in progress\n"

        "Trade::AbstractSceneConverter::skin2DCount(): no conversion in progress\n"
        "Trade::AbstractSceneConverter::add(): no conversion in progress\n"

        "Trade::AbstractSceneConverter::skin3DCount(): no conversion in progress\n"
        "Trade::AbstractSceneConverter::add(): no conversion in progress\n"

        "Trade::AbstractSceneConverter::meshCount(): no conversion in progress\n"
        "Trade::AbstractSceneConverter::add(): no conversion in progress\n"
        "Trade::AbstractSceneConverter::add(): no conversion in progress\n"
        "Trade::AbstractSceneConverter::setMeshAttributeName(): no conversion in progress\n"

        "Trade::AbstractSceneConverter::materialCount(): no conversion in progress\n"
        "Trade::AbstractSceneConverter::add(): no conversion in progress\n"

        "Trade::AbstractSceneConverter::textureCount(): no conversion in progress\n"
        "Trade::AbstractSceneConverter::add(): no conversion in progress\n"

        "Trade::AbstractSceneConverter::image1DCount(): no conversion in progress\n"
        "Trade::AbstractSceneConverter::add(): no conversion in progress\n"
        "Trade::AbstractSceneConverter::add(): no conversion in progress\n"

        "Trade::AbstractSceneConverter::image2DCount(): no conversion in progress\n"
        "Trade::AbstractSceneConverter::add(): no conversion in progress\n"
        "Trade::AbstractSceneConverter::add(): no conversion in progress\n"

        "Trade::AbstractSceneConverter::image3DCount(): no conversion in progress\n"
        "Trade::AbstractSceneConverter::add(): no conversion in progress\n"
        "Trade::AbstractSceneConverter::add(): no conversion in progress\n");
}

void AbstractSceneConverterTest::endMismatchedBegin() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToData;
        }

        void doBeginData() override {}
    } converter;

    converter.beginData();

    std::ostringstream out;
    Error redirectError{&out};
    converter.end();
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::end(): no conversion in progress\n");
}

void AbstractSceneConverterTest::endDataMismatchedBegin() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple;
        }

        void doBegin() override {}
    } converter;

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.endData();
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::endData(): no data conversion in progress\n");
}

void AbstractSceneConverterTest::endFileMismatchedBegin() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple;
        }

        void doBegin() override {}
    } converter;

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.endFile();
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::endFile(): no file conversion in progress\n");
}

void AbstractSceneConverterTest::addScene() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddScenes;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt id, const SceneData& scene, Containers::StringView name) override {
            /* Scene count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, sceneCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(scene.importerState(), reinterpret_cast<const void*>(0xdeadbeef));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    converter.begin();
    CORRADE_COMPARE(converter.sceneCount(), 0);
    CORRADE_COMPARE(converter.add(SceneData{SceneMappingType::UnsignedInt, 0, nullptr, nullptr, reinterpret_cast<const void*>(0xdeadbeef)}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.sceneCount(), 1);
    CORRADE_COMPARE(converter.add(SceneData{SceneMappingType::UnsignedInt, 0, nullptr, nullptr, reinterpret_cast<const void*>(0xdeadbeef)}, "hello"), 1);
    CORRADE_COMPARE(converter.sceneCount(), 2);
}

void AbstractSceneConverterTest::addSceneFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddScenes;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, const SceneData&, Containers::StringView) override {
            return false;
        }
    } converter;

    converter.begin();
    CORRADE_COMPARE(converter.sceneCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        std::ostringstream out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add(SceneData{SceneMappingType::UnsignedInt, 0, nullptr, nullptr}));
        CORRADE_COMPARE(out.str(), "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.sceneCount(), 0);
}

void AbstractSceneConverterTest::addSceneNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddScenes;
        }

        void doBegin() override {}
    } converter;

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.add(SceneData{SceneMappingType::UnsignedInt, 0, nullptr, nullptr});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): scene conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::setSceneFieldName() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddScenes;
        }

        void doBegin() override {}

        void doSetSceneFieldName(UnsignedInt field, Containers::StringView name) override {
            CORRADE_COMPARE(field, 1337);
            CORRADE_COMPARE(name, "hello!");
            setSceneFieldNameCalled = true;
        }

        bool setSceneFieldNameCalled = false;
    } converter;

    CORRADE_VERIFY(true); /* capture correct function name */

    converter.begin();
    converter.setSceneFieldName(sceneFieldCustom(1337), "hello!");
    CORRADE_VERIFY(converter.setSceneFieldNameCalled);
}

void AbstractSceneConverterTest::setSceneFieldNameNotImplemented() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddScenes;
        }

        void doBegin() override {}
    } converter;

    /* This should work, there's no need for a plugin to implement this */
    converter.begin();
    converter.setSceneFieldName(sceneFieldCustom(1337), "hello!");
    CORRADE_VERIFY(true);
}

void AbstractSceneConverterTest::setSceneFieldNameNotCustom() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddScenes;
        }

        void doBegin() override {}
    } converter;

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.setSceneFieldName(SceneField::Transformation, "hello!");
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::setSceneFieldName(): Trade::SceneField::Transformation is not custom\n");
}

void AbstractSceneConverterTest::setObjectName() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddScenes;
        }

        void doBegin() override {}

        void doSetObjectName(UnsignedLong object, Containers::StringView name) override {
            CORRADE_COMPARE(object, 1337);
            CORRADE_COMPARE(name, "hey!");
            setObjectNameCalled = true;
        }

        bool setObjectNameCalled = false;
    } converter;

    CORRADE_VERIFY(true); /* capture correct function name */

    converter.begin();
    converter.setObjectName(1337, "hey!");
    CORRADE_VERIFY(converter.setObjectNameCalled);
}

void AbstractSceneConverterTest::setObjectNameNotImplemented() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddScenes;
        }

        void doBegin() override {}
    } converter;

    /* This should work, there's no need for a plugin to implement this */
    converter.begin();
    converter.setObjectName(1337, "hey!");
    CORRADE_VERIFY(true);
}

void AbstractSceneConverterTest::setDefaultScene() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddScenes;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, const SceneData&, Containers::StringView) override {
            return true;
        }

        void doSetDefaultScene(UnsignedInt id) override {
            CORRADE_COMPARE(id, 2);
            setDefaultSceneCalled = true;
        }

        bool setDefaultSceneCalled = false;
    } converter;

    converter.begin();
    CORRADE_VERIFY(converter.add(SceneData{SceneMappingType::UnsignedInt, 0, nullptr, nullptr}));
    CORRADE_VERIFY(converter.add(SceneData{SceneMappingType::UnsignedInt, 0, nullptr, nullptr}));
    CORRADE_COMPARE(converter.add(SceneData{SceneMappingType::UnsignedInt, 0, nullptr, nullptr}), 2);

    converter.setDefaultScene(2);
    CORRADE_VERIFY(converter.setDefaultSceneCalled);
}

void AbstractSceneConverterTest::setDefaultSceneNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddScenes;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, const SceneData&, Containers::StringView) override {
            return true;
        }
    } converter;

    converter.begin();
    CORRADE_VERIFY(converter.add(SceneData{SceneMappingType::UnsignedInt, 0, nullptr, nullptr}));
    CORRADE_COMPARE(converter.sceneCount(), 1);

    /* This should work, there's no need for a plugin to implement this */
    converter.setDefaultScene(0);
}

void AbstractSceneConverterTest::setDefaultSceneOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddScenes;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, const SceneData&, Containers::StringView) override {
            return true;
        }
    } converter;

    converter.begin();
    CORRADE_VERIFY(converter.add(SceneData{SceneMappingType::UnsignedInt, 0, nullptr, nullptr}));
    CORRADE_VERIFY(converter.add(SceneData{SceneMappingType::UnsignedInt, 0, nullptr, nullptr}));
    CORRADE_COMPARE(converter.sceneCount(), 2);

    std::ostringstream out;
    Error redirectError{&out};
    converter.setDefaultScene(2);
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::setDefaultScene(): index 2 out of range for 2 scenes\n");
}

void AbstractSceneConverterTest::addAnimation() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddAnimations;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt id, const AnimationData& animation, Containers::StringView name) override {
            /* Animation count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, animationCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(animation.importerState(), reinterpret_cast<const void*>(0xdeadbeef));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    converter.begin();
    CORRADE_COMPARE(converter.animationCount(), 0);
    CORRADE_COMPARE(converter.add(AnimationData{nullptr, nullptr, reinterpret_cast<const void*>(0xdeadbeef)}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.animationCount(), 1);
    CORRADE_COMPARE(converter.add(AnimationData{nullptr, nullptr, reinterpret_cast<const void*>(0xdeadbeef)}, "hello"), 1);
    CORRADE_COMPARE(converter.animationCount(), 2);
}

void AbstractSceneConverterTest::addAnimationFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddAnimations;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, const AnimationData&, Containers::StringView) override {
            return false;
        }
    } converter;

    converter.begin();
    CORRADE_COMPARE(converter.animationCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        std::ostringstream out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add(AnimationData{nullptr, nullptr}));
        CORRADE_COMPARE(out.str(), "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.animationCount(), 0);
}

void AbstractSceneConverterTest::addAnimationNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddAnimations;
        }

        void doBegin() override {}
    } converter;

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.add(AnimationData{nullptr, nullptr});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): animation conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::addLight() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddLights;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt id, const LightData& light, Containers::StringView name) override {
            /* Light count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, lightCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(light.importerState(), reinterpret_cast<const void*>(0xdeadbeef));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    converter.begin();
    CORRADE_COMPARE(converter.lightCount(), 0);
    CORRADE_COMPARE(converter.add(LightData{LightData::Type::Point, {}, 0.0f, reinterpret_cast<const void*>(0xdeadbeef)}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.lightCount(), 1);
    CORRADE_COMPARE(converter.add(LightData{LightData::Type::Point, {}, 0.0f, reinterpret_cast<const void*>(0xdeadbeef)}, "hello"), 1);
    CORRADE_COMPARE(converter.lightCount(), 2);
}

void AbstractSceneConverterTest::addLightFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddLights;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, const LightData&, Containers::StringView) override {
            return false;
        }
    } converter;

    converter.begin();
    CORRADE_COMPARE(converter.lightCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        std::ostringstream out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add(LightData{LightData::Type::Point, {}, 0.0f}));
        CORRADE_COMPARE(out.str(), "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.lightCount(), 0);
}

void AbstractSceneConverterTest::addLightNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddLights;
        }

        void doBegin() override {}
    } converter;

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.add(LightData{LightData::Type::Point, {}, 0.0f});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): light conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::addCamera() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddCameras;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt id, const CameraData& camera, Containers::StringView name) override {
            /* Camera count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, cameraCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(camera.importerState(), reinterpret_cast<const void*>(0xdeadbeef));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    converter.begin();
    CORRADE_COMPARE(converter.cameraCount(), 0);
    CORRADE_COMPARE(converter.add(CameraData{CameraType::Orthographic3D, {}, 0.0f, 1.0f, reinterpret_cast<const void*>(0xdeadbeef)}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.cameraCount(), 1);
    CORRADE_COMPARE(converter.add(CameraData{CameraType::Orthographic3D, {}, 0.0f, 1.0f, reinterpret_cast<const void*>(0xdeadbeef)}, "hello"), 1);
    CORRADE_COMPARE(converter.cameraCount(), 2);
}

void AbstractSceneConverterTest::addCameraFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddCameras;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, const CameraData&, Containers::StringView) override {
            return false;
        }
    } converter;

    converter.begin();
    CORRADE_COMPARE(converter.cameraCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        std::ostringstream out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add(CameraData{CameraType::Orthographic3D, {}, 0.0f, 1.0f}));
        CORRADE_COMPARE(out.str(), "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.cameraCount(), 0);
}

void AbstractSceneConverterTest::addCameraNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddCameras;
        }

        void doBegin() override {}
    } converter;

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.add(CameraData{CameraType::Orthographic3D, {}, 0.0f, 1.0f});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): camera conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::addSkin2D() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddSkins2D;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt id, const SkinData2D& skin, Containers::StringView name) override {
            /* Camera count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, skin2DCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(skin.importerState(), reinterpret_cast<const void*>(0xdeadbeef));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    converter.begin();
    CORRADE_COMPARE(converter.skin2DCount(), 0);
    CORRADE_COMPARE(converter.add(SkinData2D{nullptr, nullptr, reinterpret_cast<const void*>(0xdeadbeef)}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.skin2DCount(), 1);
    CORRADE_COMPARE(converter.add(SkinData2D{nullptr, nullptr, reinterpret_cast<const void*>(0xdeadbeef)}, "hello"), 1);
    CORRADE_COMPARE(converter.skin2DCount(), 2);
}

void AbstractSceneConverterTest::addSkin2DFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddSkins2D;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, const SkinData2D&, Containers::StringView) override {
            return false;
        }
    } converter;

    converter.begin();
    CORRADE_COMPARE(converter.skin2DCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        std::ostringstream out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add(SkinData2D{nullptr, nullptr}));
        CORRADE_COMPARE(out.str(), "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.skin2DCount(), 0);
}

void AbstractSceneConverterTest::addSkin2DNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddSkins2D;
        }

        void doBegin() override {}
    } converter;

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.add(SkinData2D{nullptr, nullptr});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): 2D skin conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::addSkin3D() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddSkins3D;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt id, const SkinData3D& skin, Containers::StringView name) override {
            /* Camera count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, skin3DCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(skin.importerState(), reinterpret_cast<const void*>(0xdeadbeef));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    converter.begin();
    CORRADE_COMPARE(converter.skin3DCount(), 0);
    CORRADE_COMPARE(converter.add(SkinData3D{nullptr, nullptr, reinterpret_cast<const void*>(0xdeadbeef)}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.skin3DCount(), 1);
    CORRADE_COMPARE(converter.add(SkinData3D{nullptr, nullptr, reinterpret_cast<const void*>(0xdeadbeef)}, "hello"), 1);
    CORRADE_COMPARE(converter.skin3DCount(), 2);
}

void AbstractSceneConverterTest::addSkin3DFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddSkins3D;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, const SkinData3D&, Containers::StringView) override {
            return false;
        }
    } converter;

    converter.begin();
    CORRADE_COMPARE(converter.skin3DCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        std::ostringstream out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add(SkinData3D{nullptr, nullptr}));
        CORRADE_COMPARE(out.str(), "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.skin3DCount(), 0);
}

void AbstractSceneConverterTest::addSkin3DNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddSkins3D;
        }

        void doBegin() override {}
    } converter;

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.add(SkinData3D{nullptr, nullptr});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): 3D skin conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::addMesh() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddMeshes;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt id, const MeshData& mesh, Containers::StringView name) override {
            /* Camera count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, meshCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(mesh.importerState(), reinterpret_cast<const void*>(0xdeadbeef));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    converter.begin();
    CORRADE_COMPARE(converter.meshCount(), 0);
    CORRADE_COMPARE(converter.add(MeshData{MeshPrimitive::Triangles, 0, reinterpret_cast<const void*>(0xdeadbeef)}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.meshCount(), 1);
    CORRADE_COMPARE(converter.add(MeshData{MeshPrimitive::Triangles, 0, reinterpret_cast<const void*>(0xdeadbeef)}, "hello"), 1);
    CORRADE_COMPARE(converter.meshCount(), 2);
}

void AbstractSceneConverterTest::addMeshFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddMeshes;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, const MeshData&, Containers::StringView) override {
            return false;
        }
    } converter;

    converter.begin();
    CORRADE_COMPARE(converter.meshCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        std::ostringstream out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add(MeshData{MeshPrimitive::Triangles, 0}));
        CORRADE_COMPARE(out.str(), "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.meshCount(), 0);
}

void AbstractSceneConverterTest::addMeshThroughConvertMesh() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMesh;
        }

        Containers::Optional<MeshData> doConvert(const MeshData& mesh) override {
            CORRADE_COMPARE(mesh.primitive(), MeshPrimitive::Triangles);
            return MeshData{MeshPrimitive::Lines, mesh.vertexCount()*2};
        }
    } converter;

    CORRADE_VERIFY(true); /* capture correct function name */

    converter.begin();

    CORRADE_COMPARE(converter.meshCount(), 0);
    CORRADE_COMPARE(converter.add(MeshData{MeshPrimitive::Triangles, 6}), 0);
    CORRADE_COMPARE(converter.meshCount(), 1);

    Containers::Pointer<AbstractImporter> out = converter.end();
    CORRADE_VERIFY(out);
    CORRADE_COMPARE(out->meshCount(), 1);

    Containers::Optional<MeshData> mesh = out->mesh(0);
    CORRADE_VERIFY(mesh);
    CORRADE_COMPARE(mesh->primitive(), MeshPrimitive::Lines);
    CORRADE_COMPARE(mesh->vertexCount(), 12);
}

void AbstractSceneConverterTest::addMeshThroughConvertMeshFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMesh;
        }

        Containers::Optional<MeshData> doConvert(const MeshData&) override {
            return {};
        }
    } converter;

    converter.begin();

    CORRADE_VERIFY(!converter.add(MeshData{MeshPrimitive::Lines, 6}));
    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.meshCount(), 0);

    /* But the observable behavior is as if no mesh was added */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.end());
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::end(): the converter requires exactly one mesh\n");
}

void AbstractSceneConverterTest::addMeshThroughConvertMeshZeroMeshes() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMesh;
        }
    } converter;

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.end());
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::end(): the converter requires exactly one mesh\n");
}

void AbstractSceneConverterTest::addMeshThroughConvertMeshTwoMeshes() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMesh;
        }

        Containers::Optional<MeshData> doConvert(const MeshData& mesh) override {
            return MeshData{MeshPrimitive::Lines, mesh.vertexCount()*2};
        }
    } converter;

    converter.begin();

    CORRADE_COMPARE(converter.meshCount(), 0);
    CORRADE_COMPARE(converter.add(MeshData{MeshPrimitive::Triangles, 6}), 0);
    CORRADE_COMPARE(converter.meshCount(), 1);

    {
        std::ostringstream out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add(MeshData{MeshPrimitive::Triangles, 7}));
        /* It shouldn't abort the whole process */
        CORRADE_VERIFY(converter.isConverting());
        CORRADE_COMPARE(converter.meshCount(), 1);
        CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): the converter requires exactly one mesh\n");
    }

    /* Getting the result should still work */
    Containers::Pointer<AbstractImporter> out = converter.end();
    CORRADE_VERIFY(out);
    CORRADE_COMPARE(out->meshCount(), 1);

    Containers::Optional<MeshData> mesh = out->mesh(0);
    CORRADE_VERIFY(mesh);
    CORRADE_COMPARE(mesh->primitive(), MeshPrimitive::Lines);
    CORRADE_COMPARE(mesh->vertexCount(), 12);
}

void AbstractSceneConverterTest::addMeshThroughConvertMeshToData() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMeshToData;
        }

        Containers::Optional<Containers::Array<char>> doConvertToData(const MeshData& mesh) override {
            return Containers::Array<char>{nullptr, mesh.vertexCount()};
        }
    } converter;

    converter.beginData();

    CORRADE_COMPARE(converter.meshCount(), 0);
    CORRADE_COMPARE(converter.add(MeshData{MeshPrimitive::Triangles, 6}), 0);
    CORRADE_COMPARE(converter.meshCount(), 1);

    Containers::Optional<Containers::Array<char>> data = converter.endData();
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->size(), 6);
}

void AbstractSceneConverterTest::addMeshThroughConvertMeshToDataFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMeshToData;
        }

        Containers::Optional<Containers::Array<char>> doConvertToData(const MeshData&) override {
            return {};
        }
    } converter;

    converter.beginData();

    CORRADE_VERIFY(!converter.add(MeshData{MeshPrimitive::Lines, 6}));
    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.meshCount(), 0);

    /* But the observable behavior is as if no mesh was added */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.endData());
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::endData(): the converter requires exactly one mesh\n");
}

void AbstractSceneConverterTest::addMeshThroughConvertMeshToDataZeroMeshes() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMeshToData;
        }
    } converter;

    converter.beginData();

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.endData());
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::endData(): the converter requires exactly one mesh\n");
}

void AbstractSceneConverterTest::addMeshThroughConvertMeshToDataTwoMeshes() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMeshToData;
        }

        Containers::Optional<Containers::Array<char>> doConvertToData(const MeshData& mesh) override {
            return Containers::Array<char>{nullptr, mesh.vertexCount()};
        }
    } converter;

    converter.beginData();

    CORRADE_COMPARE(converter.meshCount(), 0);
    CORRADE_COMPARE(converter.add(MeshData{MeshPrimitive::Triangles, 6}), 0);
    CORRADE_COMPARE(converter.meshCount(), 1);

    {
        std::ostringstream out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add(MeshData{MeshPrimitive::Triangles, 7}));
        /* It shouldn't abort the whole process */
        CORRADE_VERIFY(converter.isConverting());
        CORRADE_COMPARE(converter.meshCount(), 1);
        CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): the converter requires exactly one mesh\n");
    }

    /* Getting the result should still work */
    Containers::Optional<Containers::Array<char>> data = converter.endData();
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->size(), 6);
}

void AbstractSceneConverterTest::addMeshThroughConvertMeshToFile() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMeshToFile;
        }

        bool doConvertToFile(const MeshData& mesh, Containers::StringView filename) override {
            return Utility::Path::write(filename, Containers::arrayView(               {char(mesh.vertexCount())}));
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "mesh.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    converter.beginFile(filename);
    CORRADE_VERIFY(!Utility::Path::exists(filename));

    CORRADE_COMPARE(converter.meshCount(), 0);
    CORRADE_COMPARE(converter.add(MeshData{MeshPrimitive::Triangles, 0xef}), 0);
    CORRADE_COMPARE(converter.meshCount(), 1);

    /* It's easier to just perform the operation right during add() than to
       make a copy of the passed MeshData and do it at endFile(), so the file
       exists at this point already */
    CORRADE_VERIFY(Utility::Path::exists(filename));

    CORRADE_VERIFY(converter.endFile());
    CORRADE_VERIFY(Utility::Path::exists(filename));
    CORRADE_COMPARE_AS(filename,
        "\xef", TestSuite::Compare::FileToString);
}

void AbstractSceneConverterTest::addMeshThroughConvertMeshToFileThroughData() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMeshToData;
        }

        Containers::Optional<Containers::Array<char>> doConvertToData(const MeshData& mesh) override {
            return Containers::array({char(mesh.vertexCount())});
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "mesh.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    converter.beginFile(filename);
    CORRADE_VERIFY(!Utility::Path::exists(filename));

    CORRADE_COMPARE(converter.meshCount(), 0);
    CORRADE_COMPARE(converter.add(MeshData{MeshPrimitive::Triangles, 0xef}), 0);
    CORRADE_COMPARE(converter.meshCount(), 1);

    /* It's easier to just perform the operation right during add() than to
       make a copy of the passed MeshData and do it at the end(), so the file
       exists at this point already */
    CORRADE_VERIFY(Utility::Path::exists(filename));

    CORRADE_VERIFY(converter.endFile());
    CORRADE_VERIFY(Utility::Path::exists(filename));
    CORRADE_COMPARE_AS(filename,
        "\xef", TestSuite::Compare::FileToString);
}

void AbstractSceneConverterTest::addMeshThroughConvertMeshToFileFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMeshToFile;
        }

        bool doConvertToFile(const MeshData&, Containers::StringView) override {
            return false;
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "mesh.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    converter.beginFile(filename);
    CORRADE_VERIFY(!Utility::Path::exists(filename));

    CORRADE_VERIFY(!converter.add(MeshData{MeshPrimitive::Lines, 6}));
    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.meshCount(), 0);

    /* But the observable behavior is as if no mesh was added */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.endFile());
    CORRADE_VERIFY(!Utility::Path::exists(filename));
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::endFile(): the converter requires exactly one mesh\n");
}

void AbstractSceneConverterTest::addMeshThroughConvertMeshToFileZeroMeshes() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMeshToFile;
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "mesh.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    converter.beginFile(filename);
    CORRADE_VERIFY(!Utility::Path::exists(filename));

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.endFile());
    CORRADE_VERIFY(!Utility::Path::exists(filename));
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::endFile(): the converter requires exactly one mesh\n");
}

void AbstractSceneConverterTest::addMeshThroughConvertMeshToFileTwoMeshes() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMeshToFile;
        }

        bool doConvertToFile(const MeshData& mesh, Containers::StringView filename) override {
            return Utility::Path::write(filename, Containers::arrayView(               {char(mesh.vertexCount())}));
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "mesh.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    converter.beginFile(filename);
    CORRADE_VERIFY(!Utility::Path::exists(filename));

    CORRADE_COMPARE(converter.meshCount(), 0);
    CORRADE_COMPARE(converter.add(MeshData{MeshPrimitive::Triangles, 0xef}), 0);
    CORRADE_COMPARE(converter.meshCount(), 1);

    /* It's easier to just perform the operation right during add() than to
       make a copy of the passed MeshData and do it at the endFile(), so the
       file exists at this point already */
    CORRADE_VERIFY(Utility::Path::exists(filename));

    {
        std::ostringstream out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add(MeshData{MeshPrimitive::Triangles, 0xb0}));
        /* It shouldn't abort the whole process */
        CORRADE_VERIFY(converter.isConverting());
        CORRADE_COMPARE(converter.meshCount(), 1);
        CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): the converter requires exactly one mesh\n");
    }

    /* Getting the result should still work */
    CORRADE_VERIFY(converter.endFile());
    CORRADE_VERIFY(Utility::Path::exists(filename));
    CORRADE_COMPARE_AS(filename,
        "\xef", TestSuite::Compare::FileToString);
}

void AbstractSceneConverterTest::addMeshNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddMeshes;
        }

        void doBegin() override {}
    } converter;

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.add(MeshData{MeshPrimitive::Triangles, 0});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): mesh conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::addMeshLevels() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddMeshes|
                   SceneConverterFeature::MeshLevels;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt id, Containers::ArrayView<const Containers::AnyReference<const MeshData>> meshLevels, Containers::StringView name) override {
            /* Camera count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, meshCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(meshLevels.size(), 3);
            CORRADE_COMPARE(meshLevels[1]->importerState(), reinterpret_cast<const void*>(0xdeadbeef));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    converter.begin();
    CORRADE_COMPARE(converter.meshCount(), 0);
    CORRADE_COMPARE(converter.add({
        MeshData{MeshPrimitive::Lines, 0},
        MeshData{MeshPrimitive::Triangles, 3, reinterpret_cast<const void*>(0xdeadbeef)},
        MeshData{MeshPrimitive::Faces, 0},
    }, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.meshCount(), 1);
    CORRADE_COMPARE(converter.add({
        MeshData{MeshPrimitive::Faces, 2},
        MeshData{MeshPrimitive::Meshlets, 1, reinterpret_cast<const void*>(0xdeadbeef)},
        MeshData{MeshPrimitive::Points, 0},
    }, "hello"), 1);
    CORRADE_COMPARE(converter.meshCount(), 2);
}

void AbstractSceneConverterTest::addMeshLevelsFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddMeshes|
                   SceneConverterFeature::MeshLevels;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, Containers::ArrayView<const Containers::AnyReference<const MeshData>>, Containers::StringView) override {
            return false;
        }
    } converter;

    converter.begin();
    CORRADE_COMPARE(converter.meshCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        std::ostringstream out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add({MeshData{MeshPrimitive::Triangles, 0}}));
        CORRADE_COMPARE(out.str(), "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.meshCount(), 0);
}

void AbstractSceneConverterTest::addMeshLevelsNoLevels() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages1D|
                   SceneConverterFeature::MeshLevels;
        }

        void doBegin() override {}
    } converter;

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.add(std::initializer_list<Containers::AnyReference<const ImageData1D>>{});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): at least one image level has to be specified\n");
}

void AbstractSceneConverterTest::addMeshLevelsNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddMeshes|
                   SceneConverterFeature::MeshLevels;
        }

        void doBegin() override {}
    } converter;

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.add({MeshData{MeshPrimitive::Triangles, 0}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): multi-level mesh conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::addMeshThroughLevels() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddMeshes|
                   SceneConverterFeature::MeshLevels;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, Containers::ArrayView<const Containers::AnyReference<const MeshData>> meshLevels, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(meshLevels.size(), 1);
            CORRADE_COMPARE(meshLevels[0]->importerState(), reinterpret_cast<const void*>(0xdeadbeef));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    converter.begin();
    CORRADE_COMPARE(converter.add(MeshData{MeshPrimitive::Triangles, 0, reinterpret_cast<const void*>(0xdeadbeef)}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.meshCount(), 1);
}

void AbstractSceneConverterTest::setMeshAttributeName() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddMeshes;
        }

        void doBegin() override {}

        void doSetMeshAttributeName(UnsignedShort field, Containers::StringView name) override {
            CORRADE_COMPARE(field, 1337);
            CORRADE_COMPARE(name, "hello!");
            setMeshAttributeNameCalled = true;
        }

        bool setMeshAttributeNameCalled = false;
    } converter;

    CORRADE_VERIFY(true); /* capture correct function name */

    converter.begin();
    converter.setMeshAttributeName(meshAttributeCustom(1337), "hello!");
    CORRADE_VERIFY(converter.setMeshAttributeNameCalled);
}

void AbstractSceneConverterTest::setMeshAttributeNameNotImplemented() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddMeshes;
        }

        void doBegin() override {}
    } converter;

    /* This should work, there's no need for a plugin to implement this */
    converter.begin();
    converter.setMeshAttributeName(meshAttributeCustom(1337), "hello!");
    CORRADE_VERIFY(true);
}

void AbstractSceneConverterTest::setMeshAttributeNameNotCustom() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddMeshes;
        }

        void doBegin() override {}
    } converter;

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.setMeshAttributeName(MeshAttribute::ObjectId, "hello!");
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::setMeshAttributeName(): Trade::MeshAttribute::ObjectId is not custom\n");
}

void AbstractSceneConverterTest::addMaterial() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddMaterials;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt id, const MaterialData& material, Containers::StringView name) override {
            /* Camera count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, materialCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(material.importerState(), reinterpret_cast<const void*>(0xdeadbeef));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    converter.begin();
    CORRADE_COMPARE(converter.materialCount(), 0);
    CORRADE_COMPARE(converter.add(MaterialData{{}, nullptr, reinterpret_cast<const void*>(0xdeadbeef)}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.materialCount(), 1);
    CORRADE_COMPARE(converter.add(MaterialData{{}, nullptr, reinterpret_cast<const void*>(0xdeadbeef)}, "hello"), 1);
    CORRADE_COMPARE(converter.materialCount(), 2);
}

void AbstractSceneConverterTest::addMaterialFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddMaterials;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, const MaterialData&, Containers::StringView) override {
            return false;
        }
    } converter;

    converter.begin();
    CORRADE_COMPARE(converter.materialCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        std::ostringstream out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add(MaterialData{{}, nullptr}));
        CORRADE_COMPARE(out.str(), "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.materialCount(), 0);
}

void AbstractSceneConverterTest::addMaterialNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddMaterials;
        }

        void doBegin() override {}
    } converter;

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.add(MaterialData{{}, nullptr});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): material conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::addTexture() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddTextures;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt id, const TextureData& texture, Containers::StringView name) override {
            /* Camera count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, textureCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(texture.importerState(), reinterpret_cast<const void*>(0xdeadbeef));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    converter.begin();
    CORRADE_COMPARE(converter.textureCount(), 0);
    CORRADE_COMPARE(converter.add(TextureData{{}, {}, {}, {}, {}, 0, reinterpret_cast<const void*>(0xdeadbeef)}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.textureCount(), 1);
    CORRADE_COMPARE(converter.add(TextureData{{}, {}, {}, {}, {}, 0, reinterpret_cast<const void*>(0xdeadbeef)}, "hello"), 1);
    CORRADE_COMPARE(converter.textureCount(), 2);
}

void AbstractSceneConverterTest::addTextureFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddTextures;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, const TextureData&, Containers::StringView) override {
            return false;
        }
    } converter;

    converter.begin();
    CORRADE_COMPARE(converter.textureCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        std::ostringstream out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add(TextureData{{}, {}, {}, {}, {}, 0}));
        CORRADE_COMPARE(out.str(), "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.textureCount(), 0);
}

void AbstractSceneConverterTest::addTextureNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddTextures;
        }

        void doBegin() override {}
    } converter;

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.add(TextureData{{}, {}, {}, {}, {}, 0});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): texture conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::addImage1D() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages1D;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt id, const ImageData1D& image, Containers::StringView name) override {
            /* Camera count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, image1DCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(image.importerState(), reinterpret_cast<const void*>(0xdeadbeef));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[4]{};

    converter.begin();
    CORRADE_COMPARE(converter.image1DCount(), 0);
    CORRADE_COMPARE(converter.add(ImageData1D{PixelFormat::RGBA8Unorm, 1, {}, imageData, reinterpret_cast<const void*>(0xdeadbeef)}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image1DCount(), 1);
    CORRADE_COMPARE(converter.add(ImageData1D{PixelFormat::RGBA8Unorm, 1, {}, imageData, reinterpret_cast<const void*>(0xdeadbeef)}, "hello"), 1);
    CORRADE_COMPARE(converter.image1DCount(), 2);
}

void AbstractSceneConverterTest::addImage1DView() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages1D;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, const ImageData1D& image, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_VERIFY(!image.isCompressed());
            CORRADE_COMPARE(image.storage().alignment(), 2);
            CORRADE_COMPARE(image.format(), PixelFormat::RG8Snorm);
            CORRADE_COMPARE(image.size(), 3);
            CORRADE_VERIFY(image.data());

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[6]{};

    converter.begin();
    CORRADE_COMPARE(converter.image1DCount(), 0);
    CORRADE_COMPARE(converter.add(ImageView1D{
        PixelStorage{}.setAlignment(2),
        PixelFormat::RG8Snorm, 3, imageData}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image1DCount(), 1);
}

void AbstractSceneConverterTest::addImage1DCompressedView() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddCompressedImages1D;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, const ImageData1D& image, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_VERIFY(image.isCompressed());
            CORRADE_COMPARE(image.compressedStorage().compressedBlockSize(), Vector3i{3});
            CORRADE_COMPARE(image.compressedFormat(), CompressedPixelFormat::Astc3x3x3RGBASrgb);
            CORRADE_COMPARE(image.size(), 3);
            CORRADE_VERIFY(image.data());

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[4*4]{};

    converter.begin();
    CORRADE_COMPARE(converter.image1DCount(), 0);
    CORRADE_COMPARE(converter.add(CompressedImageView1D{
        CompressedPixelStorage{}.setCompressedBlockSize({3, 3, 3}),
        CompressedPixelFormat::Astc3x3x3RGBASrgb, 3, imageData}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image1DCount(), 1);
}

void AbstractSceneConverterTest::addImage1DFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages1D;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, const ImageData1D&, Containers::StringView) override {
            return false;
        }
    } converter;

    const char imageData[4]{};

    converter.begin();
    CORRADE_COMPARE(converter.image1DCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        std::ostringstream out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add(ImageData1D{PixelFormat::RGBA8Unorm, 1, DataFlags{}, imageData}));
        CORRADE_COMPARE(out.str(), "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.image1DCount(), 0);
}

void AbstractSceneConverterTest::addImage1DInvalidImage() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages1D;
        }

        void doBegin() override {}
    } converter;

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.add(ImageData1D{PixelFormat::RGBA8Unorm, {}, nullptr});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): can't add image with a zero size: Vector(0)\n");
}

void AbstractSceneConverterTest::addImage1DNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages1D;
        }

        void doBegin() override {}
    } converter;

    converter.begin();

    const char imageData[4]{};

    std::ostringstream out;
    Error redirectError{&out};
    converter.add(ImageData1D{PixelFormat::RGBA8Unorm, 1, DataFlags{}, imageData});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): 1D image conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::addImage2D() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt id, const ImageData2D& image, Containers::StringView name) override {
            /* Camera count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, image2DCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(image.importerState(), reinterpret_cast<const void*>(0xdeadbeef));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[4]{};

    converter.begin();
    CORRADE_COMPARE(converter.image2DCount(), 0);
    CORRADE_COMPARE(converter.add(ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, {}, imageData, reinterpret_cast<const void*>(0xdeadbeef)}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image2DCount(), 1);
    CORRADE_COMPARE(converter.add(ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, {}, imageData, reinterpret_cast<const void*>(0xdeadbeef)}, "hello"), 1);
    CORRADE_COMPARE(converter.image2DCount(), 2);
}

void AbstractSceneConverterTest::addImage2DView() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, const ImageData2D& image, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_VERIFY(!image.isCompressed());
            CORRADE_COMPARE(image.storage().alignment(), 2);
            CORRADE_COMPARE(image.format(), PixelFormat::RG8Snorm);
            CORRADE_COMPARE(image.size(), (Vector2i{3, 1}));
            CORRADE_VERIFY(image.data());

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[6]{};

    converter.begin();
    CORRADE_COMPARE(converter.image2DCount(), 0);
    CORRADE_COMPARE(converter.add(ImageView2D{
        PixelStorage{}.setAlignment(2),
        PixelFormat::RG8Snorm, {3, 1}, imageData}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image2DCount(), 1);
}

void AbstractSceneConverterTest::addImage2DCompressedView() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddCompressedImages2D;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, const ImageData2D& image, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_VERIFY(image.isCompressed());
            CORRADE_COMPARE(image.compressedStorage().compressedBlockSize(), Vector3i{3});
            CORRADE_COMPARE(image.compressedFormat(), CompressedPixelFormat::Astc3x3x3RGBASrgb);
            CORRADE_COMPARE(image.size(), (Vector2i{3, 2}));
            CORRADE_VERIFY(image.data());

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[4*4]{};

    converter.begin();
    CORRADE_COMPARE(converter.image2DCount(), 0);
    CORRADE_COMPARE(converter.add(CompressedImageView2D{
        CompressedPixelStorage{}.setCompressedBlockSize({3, 3, 3}),
        CompressedPixelFormat::Astc3x3x3RGBASrgb, {3, 2}, imageData}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image2DCount(), 1);
}

void AbstractSceneConverterTest::addImage2DFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, const ImageData2D&, Containers::StringView) override {
            return false;
        }
    } converter;

    const char imageData[4]{};

    converter.begin();
    CORRADE_COMPARE(converter.image2DCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        std::ostringstream out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add(ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, DataFlags{}, imageData}));
        CORRADE_COMPARE(out.str(), "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.image2DCount(), 0);
}

void AbstractSceneConverterTest::addImage2DZeroSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D;
        }

        void doBegin() override {}
    } converter;

    const char imageData[16]{};

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.add(ImageData2D{PixelFormat::RGBA8Unorm, {4, 0}, DataFlags{}, imageData});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): can't add image with a zero size: Vector(4, 0)\n");
}

void AbstractSceneConverterTest::addImage2DNullptr() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D;
        }

        void doBegin() override {}
    } converter;

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.add(ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, DataFlags{}, {nullptr, 4}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): can't add image with a nullptr view\n");
}

void AbstractSceneConverterTest::addImage2DNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D;
        }

        void doBegin() override {}
    } converter;

    converter.begin();

    const char imageData[4]{};

    std::ostringstream out;
    Error redirectError{&out};
    converter.add(ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, DataFlags{}, imageData});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): 2D image conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::addImage3D() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages3D;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt id, const ImageData3D& image, Containers::StringView name) override {
            /* Camera count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, image3DCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(image.importerState(), reinterpret_cast<const void*>(0xdeadbeef));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[4]{};

    converter.begin();
    CORRADE_COMPARE(converter.image3DCount(), 0);
    CORRADE_COMPARE(converter.add(ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, {}, imageData, reinterpret_cast<const void*>(0xdeadbeef)}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image3DCount(), 1);
    CORRADE_COMPARE(converter.add(ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, {}, imageData, reinterpret_cast<const void*>(0xdeadbeef)}, "hello"), 1);
    CORRADE_COMPARE(converter.image3DCount(), 2);
}

void AbstractSceneConverterTest::addImage3DView() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages3D;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, const ImageData3D& image, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_VERIFY(!image.isCompressed());
            CORRADE_COMPARE(image.storage().alignment(), 2);
            CORRADE_COMPARE(image.format(), PixelFormat::RG8Snorm);
            CORRADE_COMPARE(image.size(), (Vector3i{1, 3, 1}));
            CORRADE_VERIFY(image.data());

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[6]{};

    converter.begin();
    CORRADE_COMPARE(converter.image3DCount(), 0);
    CORRADE_COMPARE(converter.add(ImageView3D{
        PixelStorage{}.setAlignment(2),
        PixelFormat::RG8Snorm, {1, 3, 1}, imageData}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image3DCount(), 1);
}

void AbstractSceneConverterTest::addImage3DCompressedView() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddCompressedImages3D;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, const ImageData3D& image, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_VERIFY(image.isCompressed());
            CORRADE_COMPARE(image.compressedStorage().compressedBlockSize(), Vector3i{3});
            CORRADE_COMPARE(image.compressedFormat(), CompressedPixelFormat::Astc3x3x3RGBASrgb);
            CORRADE_COMPARE(image.size(), (Vector3i{1, 3, 1}));
            CORRADE_VERIFY(image.data());

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[4*4]{};

    converter.begin();
    CORRADE_COMPARE(converter.image3DCount(), 0);
    CORRADE_COMPARE(converter.add(CompressedImageView3D{
        CompressedPixelStorage{}.setCompressedBlockSize({3, 3, 3}),
        CompressedPixelFormat::Astc3x3x3RGBASrgb, {1, 3, 1}, imageData}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image3DCount(), 1);
}

void AbstractSceneConverterTest::addImage3DFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages3D;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, const ImageData3D&, Containers::StringView) override {
            return false;
        }
    } converter;

    const char imageData[4]{};

    converter.begin();
    CORRADE_COMPARE(converter.image3DCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        std::ostringstream out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add(ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, DataFlags{}, imageData}));
        CORRADE_COMPARE(out.str(), "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.image3DCount(), 0);
}

void AbstractSceneConverterTest::addImage3DInvalidImage() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages3D;
        }

        void doBegin() override {}
    } converter;

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.add(ImageData3D{PixelFormat::RGBA8Unorm, {}, nullptr});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): can't add image with a zero size: Vector(0, 0, 0)\n");
}

void AbstractSceneConverterTest::addImage3DNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages3D;
        }

        void doBegin() override {}
    } converter;

    const char imageData[4]{};

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.add(ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, DataFlags{}, imageData});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): 3D image conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::addImageLevels1D() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages1D|
                   SceneConverterFeature::ImageLevels;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt id, Containers::ArrayView<const Containers::AnyReference<const ImageData1D>> imageLevels, Containers::StringView name) override {
            /* Camera count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, image1DCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(imageLevels.size(), 3);
            CORRADE_COMPARE(imageLevels[1]->importerState(), reinterpret_cast<const void*>(0xdeadbeef));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[4*4]{};

    converter.begin();
    CORRADE_COMPARE(converter.image1DCount(), 0);
    CORRADE_COMPARE(converter.add({
        /* Arbitrary dimensions should be fine */
        ImageData1D{PixelFormat::RGBA8Unorm, 4, DataFlags{}, imageData},
        ImageData1D{PixelFormat::RGBA8Unorm, 2, DataFlags{}, imageData, reinterpret_cast<const void*>(0xdeadbeef)},
        ImageData1D{PixelFormat::RGBA8Unorm, 3, DataFlags{}, imageData}
    }, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image1DCount(), 1);
    CORRADE_COMPARE(converter.add({
        ImageData1D{PixelFormat::RGBA8Unorm, 2, DataFlags{}, imageData},
        ImageData1D{PixelFormat::RGBA8Unorm, 1, DataFlags{}, imageData, reinterpret_cast<const void*>(0xdeadbeef)},
        ImageData1D{PixelFormat::RGBA8Unorm, 4, DataFlags{}, imageData}
    }, "hello"), 1);
    CORRADE_COMPARE(converter.image1DCount(), 2);
}

void AbstractSceneConverterTest::addImageLevels1DView() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages1D|
                   SceneConverterFeature::ImageLevels;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, Containers::ArrayView<const Containers::AnyReference<const ImageData1D>> imageLevels, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(imageLevels.size(), 2);
            CORRADE_VERIFY(!imageLevels[1]->isCompressed());
            CORRADE_COMPARE(imageLevels[1]->storage().alignment(), 2);
            CORRADE_COMPARE(imageLevels[1]->format(), PixelFormat::RG8Snorm);
            CORRADE_COMPARE(imageLevels[1]->size(), 3);
            CORRADE_VERIFY(imageLevels[1]->data());

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[6]{};

    converter.begin();
    CORRADE_COMPARE(converter.image1DCount(), 0);
    /** @todo remove the arrayView() once overload ambiguity is fixed */
    CORRADE_COMPARE(converter.add(Containers::arrayView({
        ImageView1D{PixelFormat::RG8Snorm, 1, imageData},
        ImageView1D{PixelStorage{}.setAlignment(2), PixelFormat::RG8Snorm, 3, imageData}
    }), "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image1DCount(), 1);
}

void AbstractSceneConverterTest::addImageLevels1DCompressedView() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddCompressedImages1D|
                   SceneConverterFeature::ImageLevels;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, Containers::ArrayView<const Containers::AnyReference<const ImageData1D>> imageLevels, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(imageLevels.size(), 2);
            CORRADE_VERIFY(imageLevels[1]->isCompressed());
            CORRADE_COMPARE(imageLevels[1]->compressedStorage().compressedBlockSize(), Vector3i{3});
            CORRADE_COMPARE(imageLevels[1]->compressedFormat(), CompressedPixelFormat::Astc3x3x3RGBASrgb);
            CORRADE_COMPARE(imageLevels[1]->size(), 3);
            CORRADE_VERIFY(imageLevels[1]->data());

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[6]{};

    converter.begin();
    CORRADE_COMPARE(converter.image1DCount(), 0);
    /** @todo remove the arrayView() once overload ambiguity is fixed */
    CORRADE_COMPARE(converter.add(Containers::arrayView({
        CompressedImageView1D{CompressedPixelFormat::Astc3x3x3RGBASrgb, 1, imageData},
        CompressedImageView1D{CompressedPixelStorage{}.setCompressedBlockSize({3, 3, 3}), CompressedPixelFormat::Astc3x3x3RGBASrgb, 3, imageData}
    }), "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image1DCount(), 1);
}

void AbstractSceneConverterTest::addImageLevels1DFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages1D|
                   SceneConverterFeature::ImageLevels;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, Containers::ArrayView<const Containers::AnyReference<const ImageData1D>>, Containers::StringView) override {
            return false;
        }
    } converter;

    const char imageData[4]{};

    converter.begin();
    CORRADE_COMPARE(converter.image1DCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        std::ostringstream out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add({ImageData1D{PixelFormat::RGBA8Unorm, 1, DataFlags{}, imageData}}));
        CORRADE_COMPARE(out.str(), "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.image1DCount(), 0);
}

void AbstractSceneConverterTest::addImageLevels1DInvalidImage() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages1D|
                   SceneConverterFeature::ImageLevels;
        }

        void doBegin() override {}
    } converter;

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.add(std::initializer_list<Containers::AnyReference<const ImageData1D>>{});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): at least one image level has to be specified\n");
}

void AbstractSceneConverterTest::addImageLevels1DNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages1D|
                   SceneConverterFeature::ImageLevels;
        }

        void doBegin() override {}
    } converter;

    const char imageData[4]{};

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.add({ImageData1D{PixelFormat::RGBA8Unorm, 1, DataFlags{}, imageData}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): multi-level 1D image conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::addImageLevels2D() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D|
                   SceneConverterFeature::ImageLevels;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt id, Containers::ArrayView<const Containers::AnyReference<const ImageData2D>> imageLevels, Containers::StringView name) override {
            /* Camera count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, image2DCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(imageLevels.size(), 3);
            CORRADE_COMPARE(imageLevels[1]->importerState(), reinterpret_cast<const void*>(0xdeadbeef));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[4*4]{};

    converter.begin();
    CORRADE_COMPARE(converter.image2DCount(), 0);
    CORRADE_COMPARE(converter.add({
        /* Arbitrary dimensions should be fine */
        ImageData2D{PixelFormat::RGBA8Unorm, {4, 1}, DataFlags{}, imageData},
        ImageData2D{PixelFormat::RGBA8Unorm, {2, 2}, DataFlags{}, imageData, reinterpret_cast<const void*>(0xdeadbeef)},
        ImageData2D{PixelFormat::RGBA8Unorm, {1, 3}, DataFlags{}, imageData}
    }, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image2DCount(), 1);
    CORRADE_COMPARE(converter.add({
        ImageData2D{PixelFormat::RGBA8Unorm, {2, 2}, DataFlags{}, imageData},
        ImageData2D{PixelFormat::RGBA8Unorm, {1, 3}, DataFlags{}, imageData, reinterpret_cast<const void*>(0xdeadbeef)},
        ImageData2D{PixelFormat::RGBA8Unorm, {4, 1}, DataFlags{}, imageData}
    }, "hello"), 1);
    CORRADE_COMPARE(converter.image2DCount(), 2);
}

void AbstractSceneConverterTest::addImageLevels2DView() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D|
                   SceneConverterFeature::ImageLevels;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, Containers::ArrayView<const Containers::AnyReference<const ImageData2D>> imageLevels, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(imageLevels.size(), 2);
            CORRADE_VERIFY(!imageLevels[1]->isCompressed());
            CORRADE_COMPARE(imageLevels[1]->storage().alignment(), 2);
            CORRADE_COMPARE(imageLevels[1]->format(), PixelFormat::RG8Snorm);
            CORRADE_COMPARE(imageLevels[1]->size(), (Vector2i{1, 3}));
            CORRADE_VERIFY(imageLevels[1]->data());

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[6]{};

    converter.begin();
    CORRADE_COMPARE(converter.image2DCount(), 0);
    /** @todo remove the arrayView() once overload ambiguity is fixed */
    CORRADE_COMPARE(converter.add(Containers::arrayView({
        ImageView2D{PixelFormat::RG8Snorm, {1, 1}, imageData},
        ImageView2D{PixelStorage{}.setAlignment(2), PixelFormat::RG8Snorm, {1, 3}, imageData}
    }), "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image2DCount(), 1);
}

void AbstractSceneConverterTest::addImageLevels2DCompressedView() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddCompressedImages2D|
                   SceneConverterFeature::ImageLevels;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, Containers::ArrayView<const Containers::AnyReference<const ImageData2D>> imageLevels, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(imageLevels.size(), 2);
            CORRADE_VERIFY(imageLevels[1]->isCompressed());
            CORRADE_COMPARE(imageLevels[1]->compressedStorage().compressedBlockSize(), Vector3i{3});
            CORRADE_COMPARE(imageLevels[1]->compressedFormat(), CompressedPixelFormat::Astc3x3x3RGBASrgb);
            CORRADE_COMPARE(imageLevels[1]->size(), (Vector2i{3, 1}));
            CORRADE_VERIFY(imageLevels[1]->data());

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[6]{};

    converter.begin();
    CORRADE_COMPARE(converter.image2DCount(), 0);
    /** @todo remove the arrayView() once overload ambiguity is fixed */
    CORRADE_COMPARE(converter.add(Containers::arrayView({
        CompressedImageView2D{CompressedPixelFormat::Astc3x3x3RGBASrgb, {1, 1}, imageData},
        CompressedImageView2D{CompressedPixelStorage{}.setCompressedBlockSize({3, 3, 3}), CompressedPixelFormat::Astc3x3x3RGBASrgb, {3, 1}, imageData}
    }), "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image2DCount(), 1);
}

void AbstractSceneConverterTest::addImageLevels2DFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D|
                   SceneConverterFeature::ImageLevels;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, Containers::ArrayView<const Containers::AnyReference<const ImageData2D>>, Containers::StringView) override {
            return false;
        }
    } converter;

    const char imageData[4]{};

    converter.begin();
    CORRADE_COMPARE(converter.image2DCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        std::ostringstream out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add({ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, DataFlags{}, imageData}}));
        CORRADE_COMPARE(out.str(), "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.image2DCount(), 0);
}

void AbstractSceneConverterTest::addImageLevels2DNoLevels() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D|
                   SceneConverterFeature::ImageLevels;
        }

        void doBegin() override {}
    } converter;

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.add(std::initializer_list<Containers::AnyReference<const ImageData2D>>{});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): at least one image level has to be specified\n");
}

void AbstractSceneConverterTest::addImageLevels2DZeroSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D|
                   SceneConverterFeature::ImageLevels;
        }

        void doBegin() override {}
    } converter;

    const char imageData[4*4]{};

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.add({
        ImageData2D{PixelFormat::RGBA8Unorm, {2, 2}, DataFlags{}, imageData},
        ImageData2D{PixelFormat::RGBA8Unorm, {4, 0}, DataFlags{}, imageData}
    });
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): can't add image level 1 with a zero size: Vector(4, 0)\n");
}

void AbstractSceneConverterTest::addImageLevels2DNullptr() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D|
                   SceneConverterFeature::ImageLevels;
        }

        void doBegin() override {}
    } converter;

    const char imageData[4*4]{};

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.add({
        ImageData2D{PixelFormat::RGBA8Unorm, {2, 2}, DataFlags{}, imageData},
        ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, DataFlags{}, {nullptr, 4}}
    });
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): can't add image level 1 with a nullptr view\n");
}

void AbstractSceneConverterTest::addImageLevels2DInconsistentCompressed() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D|
                   SceneConverterFeature::ImageLevels;
        }

        void doBegin() override {}
    } converter;

    const char imageData[4*4]{};

    converter.begin();

    ImageData2D a{PixelFormat::RGBA8Unorm, {2, 2}, DataFlags{}, imageData};
    ImageData2D b{CompressedPixelFormat::Astc10x10RGBAF, {1, 1}, DataFlags{}, imageData};

    std::ostringstream out;
    Error redirectError{&out};
    converter.add({a, b});
    converter.add({b, b, a});
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractSceneConverter::add(): image level 1 is compressed but previous aren't\n"
        "Trade::AbstractSceneConverter::add(): image level 2 is not compressed but previous are\n");
}

void AbstractSceneConverterTest::addImageLevels2DInconsistentFormat() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D|
                   SceneConverterFeature::ImageLevels;
        }

        void doBegin() override {}
    } converter;

    const char imageData[4*4]{};

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.add({
        ImageData2D{PixelFormat::RGBA8Unorm, {2, 2}, DataFlags{}, imageData},
        ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, DataFlags{}, imageData},
        ImageData2D{PixelFormat::RGBA8Srgb, {4, 1}, DataFlags{}, imageData}
    });
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): image levels don't have the same format, expected PixelFormat::RGBA8Unorm but got PixelFormat::RGBA8Srgb for level 2\n");
}

void AbstractSceneConverterTest::addImageLevels2DInconsistentFormatExtra() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D|
                   SceneConverterFeature::ImageLevels;
        }

        void doBegin() override {}
    } converter;

    const char imageData[4*4]{};

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.add({
        ImageData2D{PixelStorage{}, 252, 1037, 4, {2, 2}, DataFlags{}, imageData},
        ImageData2D{PixelStorage{}, 252, 1037, 4, {1, 1}, DataFlags{}, imageData},
        ImageData2D{PixelStorage{}, 252, 4467, 4, {4, 1}, DataFlags{}, imageData}
    });
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): image levels don't have the same extra format field, expected 1037 but got 4467 for level 2\n");
}

void AbstractSceneConverterTest::addImageLevels2DInconsistentCompressedFormat() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D|
                   SceneConverterFeature::ImageLevels;
        }

        void doBegin() override {}
    } converter;

    const char imageData[4*4]{};

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.add({
        ImageData2D{CompressedPixelFormat::Bc1RGBAUnorm, {2, 2}, DataFlags{}, imageData},
        ImageData2D{CompressedPixelFormat::Bc1RGBAUnorm, {1, 1}, DataFlags{}, imageData},
        ImageData2D{CompressedPixelFormat::Bc1RGBASrgb, {4, 1}, DataFlags{}, imageData}
    });
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): image levels don't have the same format, expected CompressedPixelFormat::Bc1RGBAUnorm but got CompressedPixelFormat::Bc1RGBASrgb for level 2\n");
}

void AbstractSceneConverterTest::addImageLevels2DNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D|
                   SceneConverterFeature::ImageLevels;
        }

        void doBegin() override {}
    } converter;

    const char imageData[4]{};

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.add({ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, DataFlags{}, imageData}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): multi-level 2D image conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::addImageLevels3D() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages3D|
                   SceneConverterFeature::ImageLevels;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt id, Containers::ArrayView<const Containers::AnyReference<const ImageData3D>> imageLevels, Containers::StringView name) override {
            /* Camera count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, image3DCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(imageLevels.size(), 3);
            CORRADE_COMPARE(imageLevels[1]->importerState(), reinterpret_cast<const void*>(0xdeadbeef));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[4*4]{};

    converter.begin();
    CORRADE_COMPARE(converter.image3DCount(), 0);
    CORRADE_COMPARE(converter.add({
        /* Arbitrary dimensions should be fine */
        ImageData3D{PixelFormat::RGBA8Unorm, {4, 1, 1}, DataFlags{}, imageData},
        ImageData3D{PixelFormat::RGBA8Unorm, {2, 2, 1}, DataFlags{}, imageData, reinterpret_cast<const void*>(0xdeadbeef)},
        ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 3}, DataFlags{}, imageData}
    }, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image3DCount(), 1);
    CORRADE_COMPARE(converter.add({
        ImageData3D{PixelFormat::RGBA8Unorm, {2, 1, 2}, DataFlags{}, imageData},
        ImageData3D{PixelFormat::RGBA8Unorm, {1, 3, 1}, DataFlags{}, imageData, reinterpret_cast<const void*>(0xdeadbeef)},
        ImageData3D{PixelFormat::RGBA8Unorm, {1, 4, 1}, DataFlags{}, imageData}
    }, "hello"), 1);
    CORRADE_COMPARE(converter.image3DCount(), 2);
}

void AbstractSceneConverterTest::addImageLevels3DView() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages3D|
                   SceneConverterFeature::ImageLevels;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, Containers::ArrayView<const Containers::AnyReference<const ImageData3D>> imageLevels, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(imageLevels.size(), 2);
            CORRADE_VERIFY(!imageLevels[1]->isCompressed());
            CORRADE_COMPARE(imageLevels[1]->storage().alignment(), 2);
            CORRADE_COMPARE(imageLevels[1]->format(), PixelFormat::RG8Snorm);
            CORRADE_COMPARE(imageLevels[1]->size(), (Vector3i{1, 3, 1}));
            CORRADE_VERIFY(imageLevels[1]->data());

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[6]{};

    converter.begin();
    CORRADE_COMPARE(converter.image3DCount(), 0);
    /** @todo remove the arrayView() once overload ambiguity is fixed */
    CORRADE_COMPARE(converter.add(Containers::arrayView({
        ImageView3D{PixelFormat::RG8Snorm, {1, 1, 1}, imageData},
        ImageView3D{PixelStorage{}.setAlignment(2), PixelFormat::RG8Snorm, {1, 3, 1}, imageData}
    }), "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image3DCount(), 1);
}

void AbstractSceneConverterTest::addImageLevels3DCompressedView() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddCompressedImages3D|
                   SceneConverterFeature::ImageLevels;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, Containers::ArrayView<const Containers::AnyReference<const ImageData3D>> imageLevels, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(imageLevels.size(), 2);
            CORRADE_VERIFY(imageLevels[1]->isCompressed());
            CORRADE_COMPARE(imageLevels[1]->compressedStorage().compressedBlockSize(), Vector3i{3});
            CORRADE_COMPARE(imageLevels[1]->compressedFormat(), CompressedPixelFormat::Astc3x3x3RGBASrgb);
            CORRADE_COMPARE(imageLevels[1]->size(), (Vector3i{3, 1, 1}));
            CORRADE_VERIFY(imageLevels[1]->data());

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[6]{};

    converter.begin();
    CORRADE_COMPARE(converter.image3DCount(), 0);
    /** @todo remove the arrayView() once overload ambiguity is fixed */
    CORRADE_COMPARE(converter.add(Containers::arrayView({
        CompressedImageView3D{CompressedPixelFormat::Astc3x3x3RGBASrgb, {1, 1, 1}, imageData},
        CompressedImageView3D{CompressedPixelStorage{}.setCompressedBlockSize({3, 3, 3}), CompressedPixelFormat::Astc3x3x3RGBASrgb, {3, 1, 1}, imageData}
    }), "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image3DCount(), 1);
}

void AbstractSceneConverterTest::addImageLevels3DFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages3D|
                   SceneConverterFeature::ImageLevels;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, Containers::ArrayView<const Containers::AnyReference<const ImageData3D>>, Containers::StringView) override {
            return false;
        }
    } converter;

    const char imageData[4]{};

    converter.begin();
    CORRADE_COMPARE(converter.image3DCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        std::ostringstream out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add({ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, DataFlags{}, imageData}}));
        CORRADE_COMPARE(out.str(), "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.image3DCount(), 0);
}

void AbstractSceneConverterTest::addImageLevels3DInvalidImage() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages3D|
                   SceneConverterFeature::ImageLevels;
        }

        void doBegin() override {}
    } converter;

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.add(std::initializer_list<Containers::AnyReference<const ImageData3D>>{});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): at least one image level has to be specified\n");
}

void AbstractSceneConverterTest::addImageLevels3DNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages3D|
                   SceneConverterFeature::ImageLevels;
        }

        void doBegin() override {}
    } converter;

    const char imageData[4]{};

    converter.begin();

    std::ostringstream out;
    Error redirectError{&out};
    converter.add({ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, DataFlags{}, imageData}});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::add(): multi-level 3D image conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::addImage1DThroughLevels() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages1D|
                   SceneConverterFeature::ImageLevels;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, Containers::ArrayView<const Containers::AnyReference<const ImageData1D>> imageLevels, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(imageLevels.size(), 1);
            CORRADE_COMPARE(imageLevels[0]->importerState(), reinterpret_cast<const void*>(0xdeadbeef));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[4]{};

    converter.begin();
    CORRADE_COMPARE(converter.add(ImageData1D{PixelFormat::RGBA8Unorm, 1, {}, imageData, reinterpret_cast<const void*>(0xdeadbeef)}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image1DCount(), 1);
}

void AbstractSceneConverterTest::addImage2DThroughLevels() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D|
                   SceneConverterFeature::ImageLevels;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, Containers::ArrayView<const Containers::AnyReference<const ImageData2D>> imageLevels, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(imageLevels.size(), 1);
            CORRADE_COMPARE(imageLevels[0]->importerState(), reinterpret_cast<const void*>(0xdeadbeef));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[4]{};

    converter.begin();
    CORRADE_COMPARE(converter.add(ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, {}, imageData, reinterpret_cast<const void*>(0xdeadbeef)}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image2DCount(), 1);
}

void AbstractSceneConverterTest::addImage3DThroughLevels() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages3D|
                   SceneConverterFeature::ImageLevels;
        }

        void doBegin() override {}

        bool doAdd(UnsignedInt, Containers::ArrayView<const Containers::AnyReference<const ImageData3D>> imageLevels, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(imageLevels.size(), 1);
            CORRADE_COMPARE(imageLevels[0]->importerState(), reinterpret_cast<const void*>(0xdeadbeef));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[4]{};

    converter.begin();
    CORRADE_COMPARE(converter.add(ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, {}, imageData, reinterpret_cast<const void*>(0xdeadbeef)}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image3DCount(), 1);
}

void AbstractSceneConverterTest::debugFeature() {
    std::ostringstream out;

    Debug{&out} << SceneConverterFeature::ConvertMeshInPlace << SceneConverterFeature(0xdeaddead);
    CORRADE_COMPARE(out.str(), "Trade::SceneConverterFeature::ConvertMeshInPlace Trade::SceneConverterFeature(0xdeaddead)\n");
}

void AbstractSceneConverterTest::debugFeatures() {
    std::ostringstream out;

    Debug{&out} << (SceneConverterFeature::ConvertMesh|SceneConverterFeature::ConvertMeshToFile) << SceneConverterFeatures{};
    CORRADE_COMPARE(out.str(), "Trade::SceneConverterFeature::ConvertMesh|Trade::SceneConverterFeature::ConvertMeshToFile Trade::SceneConverterFeatures{}\n");
}

void AbstractSceneConverterTest::debugFeaturesSupersets() {
    /* ConvertMeshToData is a superset of ConvertMeshToFile, so only one should
       be printed */
    {
        std::ostringstream out;
        Debug{&out} << (SceneConverterFeature::ConvertMeshToData|SceneConverterFeature::ConvertMeshToFile);
        CORRADE_COMPARE(out.str(), "Trade::SceneConverterFeature::ConvertMeshToData\n");

    /* ConvertMultipleToData is a superset of ConvertMultipleToFile, so only
       one should be printed */
    } {
        std::ostringstream out;
        Debug{&out} << (SceneConverterFeature::ConvertMultipleToData|SceneConverterFeature::ConvertMultipleToFile);
        CORRADE_COMPARE(out.str(), "Trade::SceneConverterFeature::ConvertMultipleToData\n");
    }
}

void AbstractSceneConverterTest::debugFlag() {
    std::ostringstream out;

    Debug{&out} << SceneConverterFlag::Verbose << SceneConverterFlag(0xf0);
    CORRADE_COMPARE(out.str(), "Trade::SceneConverterFlag::Verbose Trade::SceneConverterFlag(0xf0)\n");
}

void AbstractSceneConverterTest::debugFlags() {
    std::ostringstream out;

    Debug{&out} << (SceneConverterFlag::Verbose|SceneConverterFlag(0xf0)) << SceneConverterFlags{};
    CORRADE_COMPARE(out.str(), "Trade::SceneConverterFlag::Verbose|Trade::SceneConverterFlag(0xf0) Trade::SceneConverterFlags{}\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AbstractSceneConverterTest)
