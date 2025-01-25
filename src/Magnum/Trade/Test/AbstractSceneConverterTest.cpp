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

#include <Corrade/Containers/Iterable.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/FileToString.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/Format.h>
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

    void sceneContentsForImporterNone();
    void sceneContentsForImporterAll();
    void sceneContentsForImporterNotOpened();

    void sceneContentsForConverterNone();
    void sceneContentsForConverterSingleMesh();
    void sceneContentsForConverterAll();

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
    void beginFailed();
    void endFailed();
    void beginNotImplemented();
    void endNotImplemented();

    void beginEndData();
    void beginDataFailed();
    void endDataFailed();
    void beginDataNotImplemented();
    void endDataNotImplemented();
    void beginEndDataCustomDeleter();

    void beginEndFile();
    void beginFileFailed();
    void endFileFailed();
    void beginEndFileThroughData();
    void beginEndFileThroughDataFailed();
    void beginEndFileThroughDataNotWritable();
    void beginFileNotImplemented();
    void endFileNotImplemented();

    void abort();
    void abortNotImplemented();
    void abortImplicitlyConvertMesh();
    void abortImplicitlyConvertMeshInPlace();
    void abortImplicitlyConvertMeshToData();
    void abortImplicitlyConvertMeshToFile();
    void abortImplicitlyBegin();
    void abortImplicitlyBeginData();
    void abortImplicitlyBeginFile();

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

    void setAnimationTrackTargetName();
    void setAnimationTrackTargetNameNotImplemented();
    void setAnimationTrackTargetNameNotCustom();

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
    void addImageLevels2DInconsistentFlags();
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

    void addImporterContents();
    void addImporterContentsCustomSceneFields();
    void addImporterContentsCustomMeshAttributes();
    void addImporterContentsImportFail();
    void addImporterContentsConversionFail();
    void addImporterContentsNotConverting();
    void addImporterContentsNotOpened();
    void addImporterContentsNotSupported();
    void addImporterContentsNotSupportedLevels();
    void addImporterContentsNotSupportedUncompressedImage();
    void addImporterContentsNotSupportedCompressedImage();

    void addSupportedImporterContents();
    void addSupportedImporterContentsLevels();
    void addSupportedImporterContentsNotOpened();

    void debugFeature();
    void debugFeaturePacked();
    void debugFeatures();
    void debugFeaturesPacked();
    void debugFeaturesSupersets();
    void debugFlag();
    void debugFlags();
    void debugContent();
    void debugContentPacked();
    void debugContents();
    void debugContentsPacked();
};

using namespace Containers::Literals;

const struct {
    const char* name;
    SceneConverterFeatures features;
} SetMeshAttributeData[]{
    {"batch conversion supported", SceneConverterFeature::ConvertMultiple|SceneConverterFeature::AddMeshes},
    {"single conversion supported", SceneConverterFeature::ConvertMesh},
    {"single to file conversion supported", SceneConverterFeature::ConvertMeshToFile},
    {"in-place conversion supported", SceneConverterFeature::ConvertMeshInPlace},
};

const struct {
    TestSuite::TestCaseDescriptionSourceLocation name;
    SceneContents contents;
    SceneConverterFlags flags;
    const char* expected;
} AddImporterContentsData[]{
    {"scenes", SceneContent::Scenes, {},
        "Adding scene 0 named  with 0x5ce00000\n"
        "Adding scene 1 named  with 0x5ce00001\n"
        "Setting default scene to 1\n"},
    {"scenes, verbose", SceneContent::Scenes, SceneConverterFlag::Verbose,
        "Trade::AbstractSceneConverter::addImporterContents(): adding scene 0 out of 2\n"
        "Adding scene 0 named  with 0x5ce00000\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding scene 1 out of 2\n"
        "Adding scene 1 named  with 0x5ce00001\n"
        "Setting default scene to 1\n"},
    {"scenes + names", SceneContent::Scenes|SceneContent::Names, {},
        "Setting object 0 name to Object 0\n"
        "Setting object 1 name to Object 1\n"
        "Setting object 2 name to Object 2\n"
        "Adding scene 0 named Scene 0 with 0x5ce00000\n"
        "Adding scene 1 named Scene 1 with 0x5ce00001\n"
        "Setting default scene to 1\n"},
    {"animations", SceneContent::Animations, {},
        "Adding animation 0 named  with 0x40100000\n"
        "Adding animation 1 named  with 0x40100001\n"},
    {"animations, verbose", SceneContent::Animations, SceneConverterFlag::Verbose,
        "Trade::AbstractSceneConverter::addImporterContents(): adding animation 0 out of 2\n"
        "Adding animation 0 named  with 0x40100000\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding animation 1 out of 2\n"
        "Adding animation 1 named  with 0x40100001\n"},
    {"animations + names", SceneContent::Animations|SceneContent::Names, {},
        "Adding animation 0 named Animation 0 with 0x40100000\n"
        "Adding animation 1 named Animation 1 with 0x40100001\n"},
    {"lights", SceneContent::Lights, {},
        "Adding light 0 named  with 0x11600000\n"
        "Adding light 1 named  with 0x11600001\n"},
    {"lights, verbose", SceneContent::Lights, SceneConverterFlag::Verbose,
        "Trade::AbstractSceneConverter::addImporterContents(): adding light 0 out of 2\n"
        "Adding light 0 named  with 0x11600000\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding light 1 out of 2\n"
        "Adding light 1 named  with 0x11600001\n"},
    {"lights + names", SceneContent::Lights|SceneContent::Names, {},
        "Adding light 0 named Light 0 with 0x11600000\n"
        "Adding light 1 named Light 1 with 0x11600001\n"},
    {"cameras", SceneContent::Cameras, {},
        "Adding camera 0 named  with 0xca0e0000\n"
        "Adding camera 1 named  with 0xca0e0001\n"},
    {"cameras, verbose", SceneContent::Cameras, SceneConverterFlag::Verbose,
        "Trade::AbstractSceneConverter::addImporterContents(): adding camera 0 out of 2\n"
        "Adding camera 0 named  with 0xca0e0000\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding camera 1 out of 2\n"
        "Adding camera 1 named  with 0xca0e0001\n"},
    {"cameras + names", SceneContent::Cameras|SceneContent::Names, {},
        "Adding camera 0 named Camera 0 with 0xca0e0000\n"
        "Adding camera 1 named Camera 1 with 0xca0e0001\n"},
    {"2D skins", SceneContent::Skins2D, {},
        "Adding 2D skin 0 named  with 0x50102d00\n"
        "Adding 2D skin 1 named  with 0x50102d01\n"},
    {"2D skins, verbose", SceneContent::Skins2D, SceneConverterFlag::Verbose,
        "Trade::AbstractSceneConverter::addImporterContents(): adding 2D skin 0 out of 2\n"
        "Adding 2D skin 0 named  with 0x50102d00\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding 2D skin 1 out of 2\n"
        "Adding 2D skin 1 named  with 0x50102d01\n"},
    {"2D skins + names", SceneContent::Skins2D|SceneContent::Names, {},
        "Adding 2D skin 0 named 2D skin 0 with 0x50102d00\n"
        "Adding 2D skin 1 named 2D skin 1 with 0x50102d01\n"},
    {"3D skins", SceneContent::Skins3D, {},
        "Adding 3D skin 0 named  with 0x50103d00\n"
        "Adding 3D skin 1 named  with 0x50103d01\n"},
    {"3D skins, verbose", SceneContent::Skins3D, SceneConverterFlag::Verbose,
        "Trade::AbstractSceneConverter::addImporterContents(): adding 3D skin 0 out of 2\n"
        "Adding 3D skin 0 named  with 0x50103d00\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding 3D skin 1 out of 2\n"
        "Adding 3D skin 1 named  with 0x50103d01\n"},
    {"3D skins + names", SceneContent::Skins3D|SceneContent::Names, {},
        "Adding 3D skin 0 named 3D skin 0 with 0x50103d00\n"
        "Adding 3D skin 1 named 3D skin 1 with 0x50103d01\n"},
    {"meshes", SceneContent::Meshes, {},
        "Adding mesh 0 named  with 0xe500000\n"
        "Adding mesh 1 named  with 0xe500001\n"},
    {"meshes, verbose", SceneContent::Meshes, SceneConverterFlag::Verbose,
        "Trade::AbstractSceneConverter::addImporterContents(): adding mesh 0 out of 2\n"
        "Adding mesh 0 named  with 0xe500000\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding mesh 1 out of 2\n"
        "Adding mesh 1 named  with 0xe500001\n"},
    {"meshes + names", SceneContent::Meshes|SceneContent::Names, {},
        "Adding mesh 0 named Mesh 0 with 0xe500000\n"
        "Adding mesh 1 named Mesh 1 with 0xe500001\n"},
    {"meshes + levels", SceneContent::Meshes|SceneContent::MeshLevels, {},
        "Adding mesh 0 named  with 0xe500000\n"
        "Adding mesh 1 level 0 named  with 0xe500001\n"
        "Adding mesh 1 level 1 named  with 0xe500011\n"
        "Adding mesh 1 level 2 named  with 0xe500021\n"},
    {"meshes + levels, verbose", SceneContent::Meshes|SceneContent::MeshLevels, SceneConverterFlag::Verbose,
        "Trade::AbstractSceneConverter::addImporterContents(): adding mesh 0 out of 2\n"
        "Adding mesh 0 named  with 0xe500000\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding mesh 1 out of 2\n"
        "Trade::AbstractSceneConverter::addImporterContents(): importing mesh 1 level 0 out of 3\n"
        "Trade::AbstractSceneConverter::addImporterContents(): importing mesh 1 level 1 out of 3\n"
        "Trade::AbstractSceneConverter::addImporterContents(): importing mesh 1 level 2 out of 3\n"
        "Adding mesh 1 level 0 named  with 0xe500001\n"
        "Adding mesh 1 level 1 named  with 0xe500011\n"
        "Adding mesh 1 level 2 named  with 0xe500021\n"},
    {"meshes + levels + names", SceneContent::Meshes|SceneContent::MeshLevels|SceneContent::Names, {},
        "Adding mesh 0 named Mesh 0 with 0xe500000\n"
        "Adding mesh 1 level 0 named Mesh 1 with 0xe500001\n"
        "Adding mesh 1 level 1 named Mesh 1 with 0xe500011\n"
        "Adding mesh 1 level 2 named Mesh 1 with 0xe500021\n"},
    {"materials", SceneContent::Materials, {},
        "Adding material 0 named  with 0xa7e0000\n"
        "Adding material 1 named  with 0xa7e0001\n"},
    {"materials, verbose", SceneContent::Materials, SceneConverterFlag::Verbose,
        "Trade::AbstractSceneConverter::addImporterContents(): adding material 0 out of 2\n"
        "Adding material 0 named  with 0xa7e0000\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding material 1 out of 2\n"
        "Adding material 1 named  with 0xa7e0001\n"},
    {"materials + names", SceneContent::Materials|SceneContent::Names, {},
        "Adding material 0 named Material 0 with 0xa7e0000\n"
        "Adding material 1 named Material 1 with 0xa7e0001\n"},
    {"textures", SceneContent::Textures, {},
        "Adding texture 0 named  with 0x7e070000\n"
        "Adding texture 1 named  with 0x7e070001\n"},
    {"textures, verbose", SceneContent::Textures, SceneConverterFlag::Verbose,
        "Trade::AbstractSceneConverter::addImporterContents(): adding texture 0 out of 2\n"
        "Adding texture 0 named  with 0x7e070000\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding texture 1 out of 2\n"
        "Adding texture 1 named  with 0x7e070001\n"},
    {"textures + names", SceneContent::Textures|SceneContent::Names, {},
        "Adding texture 0 named Texture 0 with 0x7e070000\n"
        "Adding texture 1 named Texture 1 with 0x7e070001\n"},
    {"1D images", SceneContent::Images1D, {},
        "Adding 1D image 0 named  with 0x10a91d00\n"
        "Adding 1D image 1 named  with 0x10a91d01\n"},
    {"1D images, verbose", SceneContent::Images1D, SceneConverterFlag::Verbose,
        "Trade::AbstractSceneConverter::addImporterContents(): adding 1D image 0 out of 2\n"
        "Adding 1D image 0 named  with 0x10a91d00\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding 1D image 1 out of 2\n"
        "Adding 1D image 1 named  with 0x10a91d01\n"},
    {"1D images + names", SceneContent::Images1D|SceneContent::Names, {},
        "Adding 1D image 0 named 1D image 0 with 0x10a91d00\n"
        "Adding 1D image 1 named 1D image 1 with 0x10a91d01\n"},
    {"1D images + levels", SceneContent::Images1D|SceneContent::ImageLevels, {},
        "Adding 1D image 0 named  with 0x10a91d00\n"
        "Adding 1D image 1 level 0 named  with 0x10a91d01\n"
        "Adding 1D image 1 level 1 named  with 0x10a91d11\n"
        "Adding 1D image 1 level 2 named  with 0x10a91d21\n"
        "Adding 1D image 1 level 3 named  with 0x10a91d31\n"},
    {"1D images + levels, verbose", SceneContent::Images1D|SceneContent::ImageLevels, SceneConverterFlag::Verbose,
        "Trade::AbstractSceneConverter::addImporterContents(): adding 1D image 0 out of 2\n"
        "Adding 1D image 0 named  with 0x10a91d00\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding 1D image 1 out of 2\n"
        "Trade::AbstractSceneConverter::addImporterContents(): importing 1D image 1 level 0 out of 4\n"
        "Trade::AbstractSceneConverter::addImporterContents(): importing 1D image 1 level 1 out of 4\n"
        "Trade::AbstractSceneConverter::addImporterContents(): importing 1D image 1 level 2 out of 4\n"
        "Trade::AbstractSceneConverter::addImporterContents(): importing 1D image 1 level 3 out of 4\n"
        "Adding 1D image 1 level 0 named  with 0x10a91d01\n"
        "Adding 1D image 1 level 1 named  with 0x10a91d11\n"
        "Adding 1D image 1 level 2 named  with 0x10a91d21\n"
        "Adding 1D image 1 level 3 named  with 0x10a91d31\n"},
    {"1D images + levels + names", SceneContent::Images1D|SceneContent::ImageLevels|SceneContent::Names, {},
        "Adding 1D image 0 named 1D image 0 with 0x10a91d00\n"
        "Adding 1D image 1 level 0 named 1D image 1 with 0x10a91d01\n"
        "Adding 1D image 1 level 1 named 1D image 1 with 0x10a91d11\n"
        "Adding 1D image 1 level 2 named 1D image 1 with 0x10a91d21\n"
        "Adding 1D image 1 level 3 named 1D image 1 with 0x10a91d31\n"},
    {"2D images", SceneContent::Images2D, {},
        "Adding 2D image 0 named  with 0x10a92d00\n"
        "Adding 2D image 1 named  with 0x10a92d01\n"},
    {"2D images, verbose", SceneContent::Images2D, SceneConverterFlag::Verbose,
        "Trade::AbstractSceneConverter::addImporterContents(): adding 2D image 0 out of 2\n"
        "Adding 2D image 0 named  with 0x10a92d00\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding 2D image 1 out of 2\n"
        "Adding 2D image 1 named  with 0x10a92d01\n"},
    {"2D images + names", SceneContent::Images2D|SceneContent::Names, {},
        "Adding 2D image 0 named 2D image 0 with 0x10a92d00\n"
        "Adding 2D image 1 named 2D image 1 with 0x10a92d01\n"},
    {"2D images + levels", SceneContent::Images2D|SceneContent::ImageLevels, {},
        "Adding 2D image 0 level 0 named  with 0x10a92d00\n"
        "Adding 2D image 0 level 1 named  with 0x10a92d10\n"
        "Adding 2D image 0 level 2 named  with 0x10a92d20\n"
        "Adding 2D image 1 named  with 0x10a92d01\n"},
    {"2D images + levels, verbose", SceneContent::Images2D|SceneContent::ImageLevels, SceneConverterFlag::Verbose,
        "Trade::AbstractSceneConverter::addImporterContents(): adding 2D image 0 out of 2\n"
        "Trade::AbstractSceneConverter::addImporterContents(): importing 2D image 0 level 0 out of 3\n"
        "Trade::AbstractSceneConverter::addImporterContents(): importing 2D image 0 level 1 out of 3\n"
        "Trade::AbstractSceneConverter::addImporterContents(): importing 2D image 0 level 2 out of 3\n"
        "Adding 2D image 0 level 0 named  with 0x10a92d00\n"
        "Adding 2D image 0 level 1 named  with 0x10a92d10\n"
        "Adding 2D image 0 level 2 named  with 0x10a92d20\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding 2D image 1 out of 2\n"
        "Adding 2D image 1 named  with 0x10a92d01\n"},
    {"2D images + names", SceneContent::Images2D|SceneContent::ImageLevels|SceneContent::Names, {},
        "Adding 2D image 0 level 0 named 2D image 0 with 0x10a92d00\n"
        "Adding 2D image 0 level 1 named 2D image 0 with 0x10a92d10\n"
        "Adding 2D image 0 level 2 named 2D image 0 with 0x10a92d20\n"
        "Adding 2D image 1 named 2D image 1 with 0x10a92d01\n"},
    {"3D images", SceneContent::Images3D, {},
        "Adding 3D image 0 named  with 0x10a93d00\n"
        "Adding 3D image 1 named  with 0x10a93d01\n"},
    {"3D images, verbose", SceneContent::Images3D, SceneConverterFlag::Verbose,
        "Trade::AbstractSceneConverter::addImporterContents(): adding 3D image 0 out of 2\n"
        "Adding 3D image 0 named  with 0x10a93d00\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding 3D image 1 out of 2\n"
        "Adding 3D image 1 named  with 0x10a93d01\n"},
    {"3D images + names", SceneContent::Images3D|SceneContent::Names, {},
        "Adding 3D image 0 named 3D image 0 with 0x10a93d00\n"
        "Adding 3D image 1 named 3D image 1 with 0x10a93d01\n"},
    {"3D images + levels", SceneContent::Images3D|SceneContent::ImageLevels, {},
        "Adding 3D image 0 named  with 0x10a93d00\n"
        "Adding 3D image 1 level 0 named  with 0x10a93d01\n"
        "Adding 3D image 1 level 1 named  with 0x10a93d11\n"},
    {"3D images + levels, verbose", SceneContent::Images3D|SceneContent::ImageLevels, SceneConverterFlag::Verbose,
        "Trade::AbstractSceneConverter::addImporterContents(): adding 3D image 0 out of 2\n"
        "Adding 3D image 0 named  with 0x10a93d00\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding 3D image 1 out of 2\n"
        "Trade::AbstractSceneConverter::addImporterContents(): importing 3D image 1 level 0 out of 2\n"
        "Trade::AbstractSceneConverter::addImporterContents(): importing 3D image 1 level 1 out of 2\n"
        "Adding 3D image 1 level 0 named  with 0x10a93d01\n"
        "Adding 3D image 1 level 1 named  with 0x10a93d11\n"},
    {"3D images + names", SceneContent::Images3D|SceneContent::ImageLevels|SceneContent::Names, {},
        "Adding 3D image 0 named 3D image 0 with 0x10a93d00\n"
        "Adding 3D image 1 level 0 named 3D image 1 with 0x10a93d01\n"
        "Adding 3D image 1 level 1 named 3D image 1 with 0x10a93d11\n"},
    {"names only", SceneContent::Names, {},
        "" /* Nothing */},
    {"names only, verbose", SceneContent::Names, SceneConverterFlag::Verbose,
        "" /* Nothing */},
};

const struct {
    const char* name;
    SceneContents contents;
} AddImporterContentsFailData[]{
    {"scene", SceneContent::Scenes},
    {"animation", SceneContent::Animations},
    {"light", SceneContent::Lights},
    {"camera", SceneContent::Cameras},
    {"2D skin", SceneContent::Skins2D},
    {"3D skin", SceneContent::Skins3D},
    {"mesh", SceneContent::Meshes},
    {"mesh levels", SceneContent::Meshes|SceneContent::MeshLevels},
    {"material", SceneContent::Materials},
    {"texture", SceneContent::Textures},
    {"1D image", SceneContent::Images1D},
    {"1D image levels", SceneContent::Images1D|SceneContent::ImageLevels},
    {"2D image", SceneContent::Images2D},
    {"2D image levels", SceneContent::Images2D|SceneContent::ImageLevels},
    {"3D image", SceneContent::Images3D},
    {"3D image levels", SceneContent::Images3D|SceneContent::ImageLevels},
};

const struct {
    const char* name;
    const char* except;
    SceneConverterFeatures exceptFeatures;
    SceneContents exceptContents;
    SceneContents wantExceptContents;
} AddSupportedImporterContentsData[]{
    {"except scenes", "2 scenes",
        SceneConverterFeature::AddScenes, SceneContent::Scenes, {}},
    {"except animations", "3 animations",
        SceneConverterFeature::AddAnimations, SceneContent::Animations, {}},
    {"except lights", "4 lights",
        SceneConverterFeature::AddLights, SceneContent::Lights, {}},
    {"except cameras", "5 cameras",
        SceneConverterFeature::AddCameras, SceneContent::Cameras, {}},
    {"except 2D skins", "6 2D skins",
        SceneConverterFeature::AddSkins2D, SceneContent::Skins2D, {}},
    {"except 3D skins", "7 3D skins",
        SceneConverterFeature::AddSkins3D, SceneContent::Skins3D, {}},
    {"except meshes", "8 meshes",
        SceneConverterFeature::AddMeshes, SceneContent::Meshes, {}},
    {"except materials", "9 materials",
        SceneConverterFeature::AddMaterials, SceneContent::Materials, {}},
    {"except textures", "10 textures",
        SceneConverterFeature::AddTextures, SceneContent::Textures, {}},
    {"except 1D images", "11 1D images",
        SceneConverterFeature::AddImages1D, SceneContent::Images1D, {}},
    {"except 2D images", "12 2D images",
        SceneConverterFeature::AddImages2D, SceneContent::Images2D, {}},
    {"except 3D images", "13 3D images",
        SceneConverterFeature::AddImages3D, SceneContent::Images3D, {}},
    /* Should only warn about materials not supported by the converter, not
       meshes because we don't want them anyway */
    {"except materials and meshes, without meshes", "9 materials",
        SceneConverterFeature::AddMaterials|SceneConverterFeature::AddMeshes,
        SceneContent::Materials|SceneContent::Meshes,
        SceneContent::Meshes},
    /* Should only warn about materials not supported by the converter, nothing
       about meshes (which are available in the importer always but not
       passed to the converter) */
    {"except materials, without meshes", "9 materials",
        SceneConverterFeature::AddMaterials, SceneContent::Materials,
        SceneContent::Meshes},
};

AbstractSceneConverterTest::AbstractSceneConverterTest() {
    addTests({&AbstractSceneConverterTest::sceneContentsForImporterNone,
              &AbstractSceneConverterTest::sceneContentsForImporterAll,
              &AbstractSceneConverterTest::sceneContentsForImporterNotOpened,

              &AbstractSceneConverterTest::sceneContentsForConverterNone,
              &AbstractSceneConverterTest::sceneContentsForConverterSingleMesh,
              &AbstractSceneConverterTest::sceneContentsForConverterAll,

              &AbstractSceneConverterTest::featuresNone,

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
              &AbstractSceneConverterTest::beginFailed,
              &AbstractSceneConverterTest::endFailed,
              &AbstractSceneConverterTest::beginNotImplemented,
              &AbstractSceneConverterTest::endNotImplemented,

              &AbstractSceneConverterTest::beginEndData,
              &AbstractSceneConverterTest::beginDataFailed,
              &AbstractSceneConverterTest::endDataFailed,
              &AbstractSceneConverterTest::beginDataNotImplemented,
              &AbstractSceneConverterTest::endDataNotImplemented,
              &AbstractSceneConverterTest::beginEndDataCustomDeleter,

              &AbstractSceneConverterTest::beginEndFile,
              &AbstractSceneConverterTest::beginFileFailed,
              &AbstractSceneConverterTest::endFileFailed,
              &AbstractSceneConverterTest::beginEndFileThroughData,
              &AbstractSceneConverterTest::beginEndFileThroughDataFailed,
              &AbstractSceneConverterTest::beginEndFileThroughDataNotWritable,
              &AbstractSceneConverterTest::beginFileNotImplemented,
              &AbstractSceneConverterTest::endFileNotImplemented,

              &AbstractSceneConverterTest::abort,
              &AbstractSceneConverterTest::abortNotImplemented,
              &AbstractSceneConverterTest::abortImplicitlyConvertMesh,
              &AbstractSceneConverterTest::abortImplicitlyConvertMeshInPlace,
              &AbstractSceneConverterTest::abortImplicitlyConvertMeshToData,
              &AbstractSceneConverterTest::abortImplicitlyConvertMeshToFile,
              &AbstractSceneConverterTest::abortImplicitlyBegin,
              &AbstractSceneConverterTest::abortImplicitlyBeginData,
              &AbstractSceneConverterTest::abortImplicitlyBeginFile,

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

              &AbstractSceneConverterTest::setAnimationTrackTargetName,
              &AbstractSceneConverterTest::setAnimationTrackTargetNameNotImplemented,
              &AbstractSceneConverterTest::setAnimationTrackTargetNameNotCustom,

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

              &AbstractSceneConverterTest::addMeshThroughLevels});

    addInstancedTests({&AbstractSceneConverterTest::setMeshAttributeName},
        Containers::arraySize(SetMeshAttributeData));

    addTests({&AbstractSceneConverterTest::setMeshAttributeNameNotImplemented,
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
              &AbstractSceneConverterTest::addImageLevels2DInconsistentFlags,
              &AbstractSceneConverterTest::addImageLevels2DNotImplemented,

              &AbstractSceneConverterTest::addImageLevels3D,
              &AbstractSceneConverterTest::addImageLevels3DView,
              &AbstractSceneConverterTest::addImageLevels3DCompressedView,
              &AbstractSceneConverterTest::addImageLevels3DFailed,
              &AbstractSceneConverterTest::addImageLevels3DInvalidImage,
              &AbstractSceneConverterTest::addImageLevels3DNotImplemented,

              &AbstractSceneConverterTest::addImage1DThroughLevels,
              &AbstractSceneConverterTest::addImage2DThroughLevels,
              &AbstractSceneConverterTest::addImage3DThroughLevels});

    addInstancedTests({&AbstractSceneConverterTest::addImporterContents},
        Containers::arraySize(AddImporterContentsData));

    addTests({&AbstractSceneConverterTest::addImporterContentsCustomSceneFields,
              &AbstractSceneConverterTest::addImporterContentsCustomMeshAttributes});

    addInstancedTests({&AbstractSceneConverterTest::addImporterContentsImportFail},
        Containers::arraySize(AddImporterContentsFailData));

    addInstancedTests({&AbstractSceneConverterTest::addImporterContentsConversionFail},
        Containers::arraySize(AddImporterContentsFailData));

    addTests({&AbstractSceneConverterTest::addImporterContentsNotConverting,
              &AbstractSceneConverterTest::addImporterContentsNotOpened,
              &AbstractSceneConverterTest::addImporterContentsNotSupported,              &AbstractSceneConverterTest::addImporterContentsNotSupportedLevels,
              &AbstractSceneConverterTest::addImporterContentsNotSupportedUncompressedImage,
              &AbstractSceneConverterTest::addImporterContentsNotSupportedCompressedImage});

    addInstancedTests({&AbstractSceneConverterTest::addSupportedImporterContents},
        Containers::arraySize(AddSupportedImporterContentsData));

    addTests({&AbstractSceneConverterTest::addSupportedImporterContentsNotOpened,
              &AbstractSceneConverterTest::addSupportedImporterContentsLevels,

              &AbstractSceneConverterTest::debugFeature,
              &AbstractSceneConverterTest::debugFeaturePacked,
              &AbstractSceneConverterTest::debugFeatures,
              &AbstractSceneConverterTest::debugFeaturesPacked,
              &AbstractSceneConverterTest::debugFeaturesSupersets,
              &AbstractSceneConverterTest::debugFlag,
              &AbstractSceneConverterTest::debugFlags,
              &AbstractSceneConverterTest::debugContent,
              &AbstractSceneConverterTest::debugContentPacked,
              &AbstractSceneConverterTest::debugContents,
              &AbstractSceneConverterTest::debugContentsPacked});

    /* Create testing dir */
    Utility::Path::make(TRADE_TEST_OUTPUT_DIR);
}

void AbstractSceneConverterTest::sceneContentsForImporterNone() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    } importer;

    CORRADE_COMPARE(sceneContentsFor(importer), SceneContent::Names);
}

void AbstractSceneConverterTest::sceneContentsForImporterAll() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 1; }
        UnsignedInt doAnimationCount() const override { return 1; }
        UnsignedInt doLightCount() const override { return 1; }
        UnsignedInt doCameraCount() const override { return 1; }
        UnsignedInt doSkin2DCount() const override { return 1; }
        UnsignedInt doSkin3DCount() const override { return 1; }
        UnsignedInt doMeshCount() const override { return 1; }
        UnsignedInt doMeshLevelCount(UnsignedInt) override { return 17; }
        UnsignedInt doMaterialCount() const override { return 1; }
        UnsignedInt doTextureCount() const override { return 1; }
        UnsignedInt doImage1DCount() const override { return 1; }
        UnsignedInt doImage1DLevelCount(UnsignedInt) override { return 17; }
        UnsignedInt doImage2DCount() const override { return 1; }
        UnsignedInt doImage2DLevelCount(UnsignedInt) override { return 17; }
        UnsignedInt doImage3DCount() const override { return 1; }
        UnsignedInt doImage3DLevelCount(UnsignedInt) override { return 17; }
    } importer;

    CORRADE_COMPARE(sceneContentsFor(importer),
        SceneContent::Scenes|
        SceneContent::Animations|
        SceneContent::Lights|
        SceneContent::Cameras|
        SceneContent::Skins2D|
        SceneContent::Skins3D|
        SceneContent::Meshes|
        SceneContent::Materials|
        SceneContent::Textures|
        SceneContent::Images1D|
        SceneContent::Images2D|
        SceneContent::Images3D|
        /* No mesh or image levels, even though reported */
        SceneContent::Names);
}

void AbstractSceneConverterTest::sceneContentsForImporterNotOpened() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    Containers::String out;
    Error redirectError{&out};
    sceneContentsFor(importer);
    CORRADE_COMPARE(out, "Trade::sceneContentsFor(): the importer is not opened\n");
}

void AbstractSceneConverterTest::sceneContentsForConverterNone() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMeshInPlace;
        }
    } converter;

    CORRADE_COMPARE(sceneContentsFor(converter), SceneContent::Names);
}

void AbstractSceneConverterTest::sceneContentsForConverterSingleMesh() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMeshToFile;
        }
    } converter;

    CORRADE_COMPARE(sceneContentsFor(converter), SceneContent::Meshes|SceneContent::Names);
}

void AbstractSceneConverterTest::sceneContentsForConverterAll() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return
                SceneConverterFeature::AddScenes|
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
                SceneConverterFeature::MeshLevels|
                SceneConverterFeature::ImageLevels;
        }
    } converter;

    CORRADE_COMPARE(sceneContentsFor(converter),
        SceneContent::Scenes|
        SceneContent::Animations|
        SceneContent::Lights|
        SceneContent::Cameras|
        SceneContent::Skins2D|
        SceneContent::Skins3D|
        SceneContent::Meshes|
        SceneContent::Materials|
        SceneContent::Textures|
        SceneContent::Images1D|
        SceneContent::Images2D|
        SceneContent::Images3D|
        SceneContent::MeshLevels|
        SceneContent::ImageLevels|
        SceneContent::Names);
}

void AbstractSceneConverterTest::featuresNone() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return {}; }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.features();
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::features(): implementation reported no features\n");
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
    CORRADE_SKIP_IF_NO_ASSERT();

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

    Containers::String out;
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
    converter.setAnimationTrackTargetName({}, {});

    converter.add(LightData{LightType::Point, {}, 0.0f});
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

    CORRADE_COMPARE_AS(out,
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
        "Trade::AbstractSceneConverter::setAnimationTrackTargetName(): feature not supported\n"

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
        "Trade::AbstractSceneConverter::add(): multi-level compressed 3D image conversion not supported\n",
        TestSuite::Compare::String);
}

void AbstractSceneConverterTest::thingLevelsNotSupported() {
    CORRADE_SKIP_IF_NO_ASSERT();

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

    Containers::String out;
    Error redirectError{&out};
    converter.add({mesh, mesh});
    converter.add({image1D, image1D});
    converter.add({compressedImage1D, compressedImage1D});
    converter.add({image2D, image2D});
    converter.add({compressedImage2D, compressedImage2D});
    converter.add({image3D, image3D});
    converter.add({compressedImage3D, compressedImage3D});
    CORRADE_COMPARE(out,
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
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convert(MeshData{MeshPrimitive::Triangles, 0}));
    CORRADE_COMPARE(out, "");
}

void AbstractSceneConverterTest::convertMeshNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMesh; }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convert(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::convert(): mesh conversion advertised but not implemented\n");
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
                Utility::move(indexData), indices,
                Containers::arrayAllocatorCast<char, ArrayAllocator>(Utility::move(vertexData)), {positions}};
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
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMesh; }

        Containers::Optional<MeshData> doConvert(const MeshData&) override {
            return MeshData{MeshPrimitive::Triangles, Containers::Array<char>{data, 1, [](char*, std::size_t) {}}, MeshIndexData{MeshIndexType::UnsignedByte, data}, 1};
        }

        char data[1];
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convert(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_COMPARE(out,
        "Trade::AbstractSceneConverter::convert(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractSceneConverterTest::convertMeshCustomVertexDataDeleter() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMesh; }

        Containers::Optional<MeshData> doConvert(const MeshData&) override {
            return MeshData{MeshPrimitive::Triangles, Containers::Array<char>{data, 1, [](char*, std::size_t) {}}, MeshIndexData{MeshIndexType::UnsignedByte, data}, 1};
        }

        char data[1];
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convert(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_COMPARE(out,
        "Trade::AbstractSceneConverter::convert(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractSceneConverterTest::convertMeshCustomAttributeDataDeleter() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMesh; }

        Containers::Optional<MeshData> doConvert(const MeshData&) override {
            return MeshData{MeshPrimitive::Triangles, Containers::Array<char>{data, 1, [](char*, std::size_t) {}}, MeshIndexData{MeshIndexType::UnsignedByte, data}, 1};
        }

        char data[1];
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convert(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_COMPARE(out,
        "Trade::AbstractSceneConverter::convert(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractSceneConverterTest::convertMeshInPlace() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMeshInPlace; }

        bool doConvertInPlace(MeshData& mesh) override {
            auto indices = mesh.mutableIndices<UnsignedInt>();
            for(std::size_t i = 0; i != indices.size()/2; ++i)
                Utility::swap(indices[i], indices[indices.size() - i -1]);
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
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertInPlace(mesh));
    CORRADE_COMPARE(out, "");
}

void AbstractSceneConverterTest::convertMeshInPlaceNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMeshInPlace; }
    } converter;

    MeshData mesh{MeshPrimitive::Triangles, 3};

    Containers::String out;
    Error redirectError{&out};
    converter.convertInPlace(mesh);
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::convertInPlace(): mesh conversion advertised but not implemented\n");
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
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToData(MeshData{MeshPrimitive::Triangles, 0}));
    CORRADE_COMPARE(out, "");
}

void AbstractSceneConverterTest::convertMeshToDataThroughBatch() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToData|
                   SceneConverterFeature::AddMeshes;
        }

        Containers::Optional<Containers::Array<char>> doConvertToData(const Magnum::Trade::MeshData&) override {
            CORRADE_FAIL("doConvertToData() should not be called");
            return {};
        }

        bool doBeginData() override {
            _vertexCount = 42;
            return true;
        }

        bool doAdd(UnsignedInt id, const MeshData& mesh, Containers::StringView) override {
            CORRADE_COMPARE(id, 0);
            CORRADE_COMPARE(mesh.primitive(), MeshPrimitive::Triangles);
            _vertexCount *= mesh.vertexCount();
            return true;
        }

        Containers::Optional<Containers::Array<char>> doEndData() override {
            return Containers::Array<char>{nullptr, _vertexCount};
        }

        std::size_t _vertexCount = 0;
    } converter;

    Containers::Optional<Containers::Array<char>> data = converter.convertToData(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_VERIFY(!converter.isConverting());
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->size(), 42*6);
}

void AbstractSceneConverterTest::convertMeshToDataThroughBatchAddFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToData|
                   SceneConverterFeature::AddMeshes;
        }

        bool doBeginData() override { return true; }

        bool doAdd(UnsignedInt, const MeshData&, Containers::StringView) override {
            return false;
        }

        Containers::Optional<Containers::Array<char>> doEndData() override {
            CORRADE_FAIL("doEndData() shouldn't be called");
            return {};
        }
    } converter;

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToData(MeshData{MeshPrimitive::Triangles, 6}));
    CORRADE_VERIFY(!converter.isConverting());
    CORRADE_COMPARE(out, "");
}

void AbstractSceneConverterTest::convertMeshToDataThroughBatchEndFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToData|
                   SceneConverterFeature::AddMeshes;
        }

        bool doBeginData() override { return true; }

        bool doAdd(UnsignedInt, const MeshData&, Containers::StringView) override {
            return true;
        }

        Containers::Optional<Containers::Array<char>> doEndData() override {
            return {};
        }
    } converter;

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToData(MeshData{MeshPrimitive::Triangles, 6}));
    CORRADE_VERIFY(!converter.isConverting());
    CORRADE_COMPARE(out, "");
}

void AbstractSceneConverterTest::convertMeshToDataNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMeshToData; }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convertToData(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::convertToData(): mesh conversion advertised but not implemented\n");
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

            /* GCC 4.8 needs extra help here */
            return Containers::optional(Utility::move(out));
        }
    } converter;

    Containers::Optional<Containers::Array<char>> data = converter.convertToData(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_VERIFY(data);
    CORRADE_COMPARE_AS(*data,
        Containers::arrayView({'h', 'e', 'l', 'l', 'o'}),
        TestSuite::Compare::Container);
}

void AbstractSceneConverterTest::convertMeshToDataCustomDeleter() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMeshToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const MeshData&) override {
            return Containers::Array<char>{data, 1, [](char*, std::size_t) {}};
        }

        char data[1];
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convertToData(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::convertToData(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractSceneConverterTest::convertMeshToFile() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMeshToFile; }

        bool doConvertToFile(const MeshData& mesh, Containers::StringView filename) override {
            return Utility::Path::write(filename, Containers::arrayView({char(mesh.vertexCount())}));
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
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile(MeshData{MeshPrimitive::Triangles, 0}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "mesh.out")));
    CORRADE_COMPARE(out, "");
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
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile(MeshData{MeshPrimitive::Triangles, 0xef}, filename));
    CORRADE_VERIFY(!Utility::Path::exists(filename));
    CORRADE_COMPARE(out, "");
}

void AbstractSceneConverterTest::convertMeshToFileThroughDataNotWritable() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMeshToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const MeshData& mesh) override {
            return Containers::array({char(mesh.vertexCount())});
        }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile(MeshData{MeshPrimitive::Triangles, 0xef}, "/some/path/that/does/not/exist"));
    /* There's an error from Path::write() before */
    CORRADE_COMPARE_AS(out,
        "\nTrade::AbstractSceneConverter::convertToFile(): cannot write to file /some/path/that/does/not/exist\n",
        TestSuite::Compare::StringHasSuffix);
}

void AbstractSceneConverterTest::convertMeshToFileThroughBatch() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToFile|
                   SceneConverterFeature::AddMeshes;
        }

        bool doConvertToFile(const Trade::MeshData&, Containers::StringView) override {
            CORRADE_FAIL("doConvertToFile() should not be called");
            return {};
        }

        bool doBeginFile(Containers::StringView filename) override {
            _filename = filename;
            return true;
        }

        bool doAdd(UnsignedInt id, const MeshData& mesh, Containers::StringView) override {
            CORRADE_COMPARE(id, 0);
            CORRADE_COMPARE(mesh.primitive(), MeshPrimitive::Triangles);
            _vertexCount = mesh.vertexCount();
            return true;
        }

        bool doEndFile(Containers::StringView filename) override {
            CORRADE_COMPARE(filename, _filename);
            return Utility::Path::write(filename, Containers::arrayView({char(_vertexCount)}));
        }

        std::size_t _vertexCount = 0;
        Containers::StringView _filename;
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "mesh.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    CORRADE_VERIFY(converter.convertToFile(MeshData{MeshPrimitive::Triangles, 0xfc}, filename));
    CORRADE_VERIFY(!converter.isConverting());
    CORRADE_COMPARE_AS(filename,
        "\xfc", TestSuite::Compare::FileToString);
}

void AbstractSceneConverterTest::convertMeshToFileThroughBatchAddFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToFile|
                   SceneConverterFeature::AddMeshes;
        }

        bool doBeginFile(Containers::StringView) override { return true; }

        bool doAdd(UnsignedInt, const MeshData&, Containers::StringView) override {
            return false;
        }

        bool doEndFile(Containers::StringView) override {
            CORRADE_FAIL("doEndFile() shouldn't be called");
            return {};
        }
    } converter;

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile(MeshData{MeshPrimitive::Triangles, 0xfc}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "mesh.out")));
    CORRADE_VERIFY(!converter.isConverting());
    CORRADE_COMPARE(out, "");
}

void AbstractSceneConverterTest::convertMeshToFileThroughBatchEndFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToFile|
                   SceneConverterFeature::AddMeshes;
        }

        bool doBeginFile(Containers::StringView) override { return true; }

        bool doAdd(UnsignedInt, const MeshData&, Containers::StringView) override {
            return true;
        }

        bool doEndFile(Containers::StringView) override {
            return {};
        }
    } converter;

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile(MeshData{MeshPrimitive::Triangles, 0xfc}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "mesh.out")));
    CORRADE_VERIFY(!converter.isConverting());
    CORRADE_COMPARE(out, "");
}

void AbstractSceneConverterTest::convertMeshToFileNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMeshToFile; }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile(MeshData{MeshPrimitive::Triangles, 6}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "mesh.out"));
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::convertToFile(): mesh conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::beginEnd() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple;
        }

        bool doBegin() override {
            CORRADE_VERIFY(!beginCalled);
            beginCalled = true;
            return true;
        }

        Containers::Pointer<AbstractImporter> doEnd() override {
            CORRADE_VERIFY(!endCalled);
            endCalled = true;

            struct Importer: AbstractImporter {
                ImporterFeatures doFeatures() const override { return {}; }
                void doClose() override {}
                bool doIsOpened() const override { return true; }

                const void* doImporterState() const override {
                    return reinterpret_cast<const void*>(std::size_t{0xdeadbeef});
                }
            };

            return Containers::Pointer<AbstractImporter>{new Importer};
        }

        bool beginCalled = false, endCalled = false;
    } converter;

    CORRADE_VERIFY(!converter.isConverting());
    CORRADE_VERIFY(converter.begin());
    CORRADE_VERIFY(converter.beginCalled);
    CORRADE_VERIFY(!converter.endCalled);
    CORRADE_VERIFY(converter.isConverting());

    Containers::Pointer<AbstractImporter> out = converter.end();
    CORRADE_VERIFY(out);
    CORRADE_COMPARE(out->importerState(), reinterpret_cast<const void*>(std::size_t{0xdeadbeef}));
    CORRADE_VERIFY(converter.endCalled);
    CORRADE_VERIFY(!converter.isConverting());
}

void AbstractSceneConverterTest::beginFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple;
        }

        bool doBegin() override { return false; }
    } converter;

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.begin());
    CORRADE_COMPARE(out, "");

    CORRADE_VERIFY(!converter.isConverting());
}

void AbstractSceneConverterTest::endFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple;
        }

        bool doBegin() override { return true; }

        Containers::Pointer<AbstractImporter> doEnd() override {
            return nullptr;
        }
    } converter;

    CORRADE_VERIFY(converter.begin());

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.end());
    CORRADE_COMPARE(out, "");

    CORRADE_VERIFY(!converter.isConverting());
}

void AbstractSceneConverterTest::beginNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple;
        }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.begin();
    CORRADE_COMPARE(out,
        "Trade::AbstractSceneConverter::begin(): feature advertised but not implemented\n");
}

void AbstractSceneConverterTest::endNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple;
        }

        bool doBegin() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.end();
    CORRADE_COMPARE(out,
        "Trade::AbstractSceneConverter::end(): feature advertised but not implemented\n");
}

void AbstractSceneConverterTest::beginEndData() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToData;
        }

        bool doBeginData() override {
            CORRADE_VERIFY(!beginCalled);
            beginCalled = true;
            return true;
        }

        Containers::Optional<Containers::Array<char>> doEndData() override {
            CORRADE_VERIFY(!endCalled);
            endCalled = true;
            return Containers::array({'h', 'e', 'l', 'l', 'o'});
        }

        bool beginCalled = false, endCalled = false;
    } converter;

    CORRADE_VERIFY(!converter.isConverting());
    CORRADE_VERIFY(converter.beginData());
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

void AbstractSceneConverterTest::beginDataFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToData;
        }

        bool doBeginData() override { return false; }
    } converter;

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.beginData());
    CORRADE_COMPARE(out, "");

    CORRADE_VERIFY(!converter.isConverting());
}

void AbstractSceneConverterTest::endDataFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToData;
        }

        bool doBeginData() override { return true; }

        Containers::Optional<Containers::Array<char>> doEndData() override {
            return {};
        }
    } converter;

    CORRADE_VERIFY(converter.beginData());

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.endData());
    CORRADE_COMPARE(out, "");

    CORRADE_VERIFY(!converter.isConverting());
}

void AbstractSceneConverterTest::beginDataNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToData;
        }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.beginData();
    CORRADE_COMPARE(out,
        "Trade::AbstractSceneConverter::beginData(): feature advertised but not implemented\n");
}

void AbstractSceneConverterTest::endDataNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToData;
        }

        bool doBeginData() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.beginData());

    Containers::String out;
    Error redirectError{&out};
    converter.endData();
    CORRADE_COMPARE(out,
        "Trade::AbstractSceneConverter::endData(): feature advertised but not implemented\n");
}

void AbstractSceneConverterTest::beginEndDataCustomDeleter() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToData;
        }

        bool doBeginData() override { return true; }

        Containers::Optional<Containers::Array<char>> doEndData() override {
            return Containers::Array<char>{data, 1, [](char*, std::size_t) {}};
        }

        char data[1];
    } converter;

    CORRADE_VERIFY(converter.beginData());

    Containers::String out;
    Error redirectError{&out};
    converter.endData();
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::endData(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractSceneConverterTest::beginEndFile() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToFile;
        }

        bool doBeginFile(Containers::StringView filename) override {
            CORRADE_VERIFY(!beginCalled);
            beginCalled = true;
            CORRADE_COMPARE(filename, "file.gltf");
            filenameDataPointer = filename.data();
            return true;
        }

        bool doEndFile(Containers::StringView filename) override {
            CORRADE_VERIFY(!endCalled);
            endCalled = true;
            CORRADE_COMPARE(filename, "file.gltf");

            /* The filename should stay in scope and be the same pointer */
            CORRADE_COMPARE(filename.data(), filenameDataPointer);
            return true;
        }

        bool beginCalled = false, endCalled = false;
        const void* filenameDataPointer;
    } converter;

    CORRADE_VERIFY(!converter.isConverting());
    CORRADE_VERIFY(converter.beginFile("file.gltf!"_s.exceptSuffix(1)));
    CORRADE_VERIFY(converter.beginCalled);
    CORRADE_VERIFY(!converter.endCalled);
    CORRADE_VERIFY(converter.isConverting());

    CORRADE_VERIFY(converter.endFile());
    CORRADE_VERIFY(converter.endCalled);
    CORRADE_VERIFY(!converter.isConverting());
}

void AbstractSceneConverterTest::beginFileFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToFile;
        }

        bool doBeginFile(Containers::StringView) override { return false; }
    } converter;

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.beginFile("file.gltf"));
    CORRADE_COMPARE(out, "");

    CORRADE_VERIFY(!converter.isConverting());
}

void AbstractSceneConverterTest::endFileFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToFile;
        }

        bool doBeginFile(Containers::StringView) override { return true; }

        bool doEndFile(Containers::StringView) override { return false; }
    } converter;

    CORRADE_VERIFY(converter.beginFile("file.gltf"));

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.endFile());
    CORRADE_COMPARE(out, "");

    CORRADE_VERIFY(!converter.isConverting());
}

void AbstractSceneConverterTest::beginEndFileThroughData() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToData;
        }

        bool doBeginData() override { return true; }

        Containers::Optional<Containers::Array<char>> doEndData() override {
            return Containers::array({'h', 'e', 'l', 'l', 'o'});
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "mesh.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    CORRADE_VERIFY(converter.beginFile(filename));

    /* doEndFile() should call doEndData() */
    CORRADE_VERIFY(converter.endFile());
    CORRADE_VERIFY(!converter.isConverting());
    CORRADE_COMPARE_AS(filename, "hello", TestSuite::Compare::FileToString);
}

void AbstractSceneConverterTest::beginEndFileThroughDataFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToData;
        }

        bool doBeginData() override { return true; }

        Containers::Optional<Containers::Array<char>> doEndData() override {
            return {};
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "mesh.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    CORRADE_VERIFY(converter.beginFile(filename));

    /* Function should fail, no file should get written and no error output
       should be printed (the base implementation assumes the plugin does it) */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.endFile());
    CORRADE_VERIFY(!converter.isConverting());
    CORRADE_VERIFY(!Utility::Path::exists(filename));
    CORRADE_COMPARE(out, "");
}

void AbstractSceneConverterTest::beginEndFileThroughDataNotWritable() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToData;
        }

        bool doBeginData() override { return true; }

        Containers::Optional<Containers::Array<char>> doEndData() override {
            return Containers::array({'h', 'e', 'l', 'l', 'o'});
        }
    } converter;

    CORRADE_VERIFY(converter.beginFile("/some/path/that/does/not/exist"));

    /* Function should fail, no file should get written and no error output
       should be printed (the base implementation assumes the plugin does it) */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.endFile());
    CORRADE_VERIFY(!converter.isConverting());
    /* There's an error from Path::write() before */
    CORRADE_COMPARE_AS(out,
        "\nTrade::AbstractSceneConverter::endFile(): cannot write to file /some/path/that/does/not/exist\n",
        TestSuite::Compare::StringHasSuffix);
}

void AbstractSceneConverterTest::beginFileNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToFile;
        }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.beginFile("file.gltf");
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::beginFile(): feature advertised but not implemented\n");
}

void AbstractSceneConverterTest::endFileNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToFile;
        }

        bool doBeginFile(Containers::StringView) override { return true; }
    } converter;

    CORRADE_VERIFY(converter.beginFile("file.gltf"));

    Containers::String out;
    Error redirectError{&out};
    converter.endFile();
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::endFile(): feature advertised but not implemented\n");
}

void AbstractSceneConverterTest::abort() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple;
        }

        bool doBegin() override { return true; }

        void doAbort() override {
            CORRADE_VERIFY(!abortCalled);
            abortCalled = true;
        }

        bool abortCalled = false;
    } converter;

    CORRADE_VERIFY(!converter.abortCalled);
    CORRADE_VERIFY(converter.begin());
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

        bool doBegin() override { return true; }
    } converter;

    /* This should work, there's no need for a plugin to implement this */
    CORRADE_VERIFY(converter.begin());
    CORRADE_VERIFY(converter.isConverting());
    converter.abort();
    CORRADE_VERIFY(!converter.isConverting());
}

void AbstractSceneConverterTest::abortImplicitlyConvertMesh() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMesh|
                   SceneConverterFeature::ConvertMultiple;
        }

        Containers::Optional<MeshData> doConvert(const MeshData&) override {
            return MeshData{MeshPrimitive::Lines, 2};
        }

        void doAbort() override {
            CORRADE_VERIFY(!abortCalled);
            abortCalled = true;
        }

        bool doBegin() override { return true; }

        bool abortCalled = false;
    } converter;

    /* Shouldn't be called if there's no previous conversion happening */
    CORRADE_VERIFY(!converter.abortCalled);
    CORRADE_VERIFY(converter.begin());
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_VERIFY(!converter.abortCalled);

    /* Should be called if there's a batch conversion happening and the
       immediate APIs are used */
    CORRADE_VERIFY(converter.convert(MeshData{MeshPrimitive::Triangles, 6}));
    CORRADE_VERIFY(!converter.isConverting());
    CORRADE_VERIFY(converter.abortCalled);
}

void AbstractSceneConverterTest::abortImplicitlyConvertMeshInPlace() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMeshInPlace|
                   SceneConverterFeature::ConvertMultiple;
        }

        bool doConvertInPlace(MeshData&) override {
            return true;
        }

        void doAbort() override {
            CORRADE_VERIFY(!abortCalled);
            abortCalled = true;
        }

        bool doBegin() override { return true; }

        bool abortCalled = false;
    } converter;

    /* Shouldn't be called if there's no previous conversion happening */
    CORRADE_VERIFY(!converter.abortCalled);
    CORRADE_VERIFY(converter.begin());
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_VERIFY(!converter.abortCalled);

    /* Should be called if there's a batch conversion happening and the
       immediate APIs are used */
    MeshData mesh{MeshPrimitive::Triangles, 6};
    CORRADE_VERIFY(converter.convertInPlace(mesh));
    CORRADE_VERIFY(!converter.isConverting());
    CORRADE_VERIFY(converter.abortCalled);
}

void AbstractSceneConverterTest::abortImplicitlyConvertMeshToData() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMeshToData|
                   SceneConverterFeature::ConvertMultiple;
        }

        Containers::Optional<Containers::Array<char>> doConvertToData(const MeshData&) override {
            return Containers::Array<char>{};
        }

        void doAbort() override {
            CORRADE_VERIFY(!abortCalled);
            abortCalled = true;
        }

        bool doBegin() override { return true; }

        bool abortCalled = false;
    } converter;

    /* Shouldn't be called if there's no previous conversion happening */
    CORRADE_VERIFY(!converter.abortCalled);
    CORRADE_VERIFY(converter.begin());
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_VERIFY(!converter.abortCalled);

    /* Should be called if there's a batch conversion happening and the
       immediate APIs are used */
    CORRADE_VERIFY(converter.convertToData(MeshData{MeshPrimitive::Triangles, 6}));
    CORRADE_VERIFY(!converter.isConverting());
    CORRADE_VERIFY(converter.abortCalled);
}

void AbstractSceneConverterTest::abortImplicitlyConvertMeshToFile() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMeshToFile|
                   SceneConverterFeature::ConvertMultiple;
        }

        bool doConvertToFile(const MeshData&, Containers::StringView) override {
            return true;
        }

        void doAbort() override {
            CORRADE_VERIFY(!abortCalled);
            abortCalled = true;
        }

        bool doBegin() override { return true; }

        bool abortCalled = false;
    } converter;

    /* Shouldn't be called if there's no previous conversion happening */
    CORRADE_VERIFY(!converter.abortCalled);
    CORRADE_VERIFY(converter.begin());
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_VERIFY(!converter.abortCalled);

    /* Should be called if there's a batch conversion happening and the
       immediate APIs are used */
    CORRADE_VERIFY(converter.convertToFile(MeshData{MeshPrimitive::Triangles, 6}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "mesh.out")));
    CORRADE_VERIFY(!converter.isConverting());
    CORRADE_VERIFY(converter.abortCalled);
}

void AbstractSceneConverterTest::abortImplicitlyBegin() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple;
        }

        void doAbort() override {
            CORRADE_VERIFY(!abortCalled);
            abortCalled = true;
        }

        bool doBegin() override { return true; }

        bool abortCalled = false;
    } converter;

    /* Shouldn't be called if there's no previous conversion happening */
    CORRADE_VERIFY(!converter.abortCalled);
    CORRADE_VERIFY(converter.begin());
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_VERIFY(!converter.abortCalled);

    /* Should be called if calling begin() while another conversion is already
       happening. Then, what is happening is the new conversion. */
    CORRADE_VERIFY(converter.begin());
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_VERIFY(converter.abortCalled);
}

void AbstractSceneConverterTest::abortImplicitlyBeginData() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::ConvertMultipleToData;
        }

        void doAbort() override {
            CORRADE_VERIFY(!abortCalled);
            abortCalled = true;
        }

        bool doBegin() override { return true; }

        bool doBeginData() override { return true; }

        bool abortCalled = false;
    } converter;

    /* Shouldn't be called if there's no previous conversion happening */
    CORRADE_VERIFY(!converter.abortCalled);
    CORRADE_VERIFY(converter.begin());
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_VERIFY(!converter.abortCalled);

    /* Should be called if calling beginData() while another conversion is
       already happening. Then, what is happening is the new conversion. */
    CORRADE_VERIFY(converter.beginData());
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_VERIFY(converter.abortCalled);
}

void AbstractSceneConverterTest::abortImplicitlyBeginFile() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::ConvertMultipleToFile;
        }

        void doAbort() override {
            CORRADE_VERIFY(!abortCalled);
            abortCalled = true;
        }

        bool doBegin() override { return true; }

        bool doBeginFile(Containers::StringView) override { return true; }

        bool abortCalled = false;
    } converter;

    /* Shouldn't be called if there's no previous conversion happening */
    CORRADE_VERIFY(!converter.abortCalled);
    CORRADE_VERIFY(converter.begin());
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_VERIFY(!converter.abortCalled);

    /* Should be called if calling beginData() while another conversion is
       already happening. Then, what is happening is the new conversion. */
    CORRADE_VERIFY(converter.beginFile(Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "mesh.out")));
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_VERIFY(converter.abortCalled);
}

void AbstractSceneConverterTest::thingNoBegin() {
    CORRADE_SKIP_IF_NO_ASSERT();

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

        bool doBeginData() override { return true; }
    } converter;

    Containers::String out;
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
    converter.setAnimationTrackTargetName({}, {});

    converter.lightCount();
    converter.add(LightData{LightType::Point, {}, 0.0f});

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

    CORRADE_COMPARE(out,
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
        "Trade::AbstractSceneConverter::setAnimationTrackTargetName(): no conversion in progress\n"

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
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToData;
        }

        bool doBeginData() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.beginData());

    Containers::String out;
    Error redirectError{&out};
    converter.end();
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::end(): no conversion in progress\n");
}

void AbstractSceneConverterTest::endDataMismatchedBegin() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple;
        }

        bool doBegin() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.endData();
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::endData(): no data conversion in progress\n");
}

void AbstractSceneConverterTest::endFileMismatchedBegin() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple;
        }

        bool doBegin() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.endFile();
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::endFile(): no file conversion in progress\n");
}

void AbstractSceneConverterTest::addScene() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddScenes;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt id, const SceneData& scene, Containers::StringView name) override {
            /* Scene count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, sceneCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(scene.importerState(), reinterpret_cast<const void*>(std::size_t{0xdeadbeef}));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.sceneCount(), 0);
    CORRADE_COMPARE(converter.add(SceneData{SceneMappingType::UnsignedInt, 0, nullptr, nullptr, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.sceneCount(), 1);
    CORRADE_COMPARE(converter.add(SceneData{SceneMappingType::UnsignedInt, 0, nullptr, nullptr, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})}, "hello"), 1);
    CORRADE_COMPARE(converter.sceneCount(), 2);
}

void AbstractSceneConverterTest::addSceneFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddScenes;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const SceneData&, Containers::StringView) override {
            return false;
        }
    } converter;

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.sceneCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        Containers::String out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add(SceneData{SceneMappingType::UnsignedInt, 0, nullptr, nullptr}));
        CORRADE_COMPARE(out, "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.sceneCount(), 0);
}

void AbstractSceneConverterTest::addSceneNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddScenes;
        }

        bool doBegin() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.add(SceneData{SceneMappingType::UnsignedInt, 0, nullptr, nullptr});
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): scene conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::setSceneFieldName() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddScenes;
        }

        bool doBegin() override { return true; }

        void doSetSceneFieldName(SceneField field, Containers::StringView name) override {
            CORRADE_COMPARE(field, sceneFieldCustom(1337));
            CORRADE_COMPARE(name, "hello!");
            setSceneFieldNameCalled = true;
        }

        bool setSceneFieldNameCalled = false;
    } converter;

    CORRADE_VERIFY(true); /* capture correct function name */

    CORRADE_VERIFY(converter.begin());
    converter.setSceneFieldName(sceneFieldCustom(1337), "hello!");
    CORRADE_VERIFY(converter.setSceneFieldNameCalled);
}

void AbstractSceneConverterTest::setSceneFieldNameNotImplemented() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddScenes;
        }

        bool doBegin() override { return true; }
    } converter;

    /* This should work, there's no need for a plugin to implement this */
    CORRADE_VERIFY(converter.begin());
    converter.setSceneFieldName(sceneFieldCustom(1337), "hello!");
    CORRADE_VERIFY(true);
}

void AbstractSceneConverterTest::setSceneFieldNameNotCustom() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddScenes;
        }

        bool doBegin() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.setSceneFieldName(SceneField::Transformation, "hello!");
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::setSceneFieldName(): Trade::SceneField::Transformation is not custom\n");
}

void AbstractSceneConverterTest::setObjectName() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddScenes;
        }

        bool doBegin() override { return true; }

        void doSetObjectName(UnsignedLong object, Containers::StringView name) override {
            CORRADE_COMPARE(object, 1337);
            CORRADE_COMPARE(name, "hey!");
            setObjectNameCalled = true;
        }

        bool setObjectNameCalled = false;
    } converter;

    CORRADE_VERIFY(true); /* capture correct function name */

    CORRADE_VERIFY(converter.begin());
    converter.setObjectName(1337, "hey!");
    CORRADE_VERIFY(converter.setObjectNameCalled);
}

void AbstractSceneConverterTest::setObjectNameNotImplemented() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddScenes;
        }

        bool doBegin() override { return true; }
    } converter;

    /* This should work, there's no need for a plugin to implement this */
    CORRADE_VERIFY(converter.begin());
    converter.setObjectName(1337, "hey!");
    CORRADE_VERIFY(true);
}

void AbstractSceneConverterTest::setDefaultScene() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddScenes;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const SceneData&, Containers::StringView) override {
            return true;
        }

        void doSetDefaultScene(UnsignedInt id) override {
            CORRADE_COMPARE(id, 2);
            setDefaultSceneCalled = true;
        }

        bool setDefaultSceneCalled = false;
    } converter;

    CORRADE_VERIFY(converter.begin());
    CORRADE_VERIFY(converter.add(SceneData{SceneMappingType::UnsignedInt, 0, nullptr, nullptr}));
    CORRADE_VERIFY(converter.add(SceneData{SceneMappingType::UnsignedInt, 0, nullptr, nullptr}));
    CORRADE_COMPARE(converter.add(SceneData{SceneMappingType::UnsignedInt, 0, nullptr, nullptr}), 2);

    converter.setDefaultScene(2);
    CORRADE_VERIFY(converter.setDefaultSceneCalled);
}

void AbstractSceneConverterTest::setDefaultSceneNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddScenes;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const SceneData&, Containers::StringView) override {
            return true;
        }
    } converter;

    CORRADE_VERIFY(converter.begin());
    CORRADE_VERIFY(converter.add(SceneData{SceneMappingType::UnsignedInt, 0, nullptr, nullptr}));
    CORRADE_COMPARE(converter.sceneCount(), 1);

    /* This should work, there's no need for a plugin to implement this */
    converter.setDefaultScene(0);
}

void AbstractSceneConverterTest::setDefaultSceneOutOfRange() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddScenes;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const SceneData&, Containers::StringView) override {
            return true;
        }
    } converter;

    CORRADE_VERIFY(converter.begin());
    CORRADE_VERIFY(converter.add(SceneData{SceneMappingType::UnsignedInt, 0, nullptr, nullptr}));
    CORRADE_VERIFY(converter.add(SceneData{SceneMappingType::UnsignedInt, 0, nullptr, nullptr}));
    CORRADE_COMPARE(converter.sceneCount(), 2);

    Containers::String out;
    Error redirectError{&out};
    converter.setDefaultScene(2);
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::setDefaultScene(): index 2 out of range for 2 scenes\n");
}

void AbstractSceneConverterTest::addAnimation() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddAnimations;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt id, const AnimationData& animation, Containers::StringView name) override {
            /* Animation count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, animationCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(animation.importerState(), reinterpret_cast<const void*>(std::size_t{0xdeadbeef}));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.animationCount(), 0);
    CORRADE_COMPARE(converter.add(AnimationData{nullptr, nullptr, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.animationCount(), 1);
    CORRADE_COMPARE(converter.add(AnimationData{nullptr, nullptr, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})}, "hello"), 1);
    CORRADE_COMPARE(converter.animationCount(), 2);
}

void AbstractSceneConverterTest::addAnimationFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddAnimations;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const AnimationData&, Containers::StringView) override {
            return false;
        }
    } converter;

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.animationCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        Containers::String out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add(AnimationData{nullptr, nullptr}));
        CORRADE_COMPARE(out, "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.animationCount(), 0);
}

void AbstractSceneConverterTest::addAnimationNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddAnimations;
        }

        bool doBegin() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.add(AnimationData{nullptr, nullptr});
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): animation conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::setAnimationTrackTargetName() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddAnimations;
        }

        bool doBegin() override { return true; }

        void doSetAnimationTrackTargetName(AnimationTrackTarget target, Containers::StringView name) override {
            CORRADE_COMPARE(target, animationTrackTargetCustom(1337));
            CORRADE_COMPARE(name, "hello!");
            setAnimationTrackTargetNameCalled = true;
        }

        bool setAnimationTrackTargetNameCalled = false;
    } converter;

    CORRADE_VERIFY(true); /* capture correct function name */

    CORRADE_VERIFY(converter.begin());
    converter.setAnimationTrackTargetName(animationTrackTargetCustom(1337), "hello!");
    CORRADE_VERIFY(converter.setAnimationTrackTargetNameCalled);
}

void AbstractSceneConverterTest::setAnimationTrackTargetNameNotImplemented() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddAnimations;
        }

        bool doBegin() override { return true; }
    } converter;

    /* This should work, there's no need for a plugin to implement this */
    CORRADE_VERIFY(converter.begin());
    converter.setAnimationTrackTargetName(animationTrackTargetCustom(1337), "hello!");
    CORRADE_VERIFY(true);
}

void AbstractSceneConverterTest::setAnimationTrackTargetNameNotCustom() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddAnimations;
        }

        bool doBegin() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.setAnimationTrackTargetName(AnimationTrackTarget::Scaling2D, "hello!");
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::setAnimationTrackTargetName(): Trade::AnimationTrackTarget::Scaling2D is not custom\n");
}

void AbstractSceneConverterTest::addLight() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddLights;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt id, const LightData& light, Containers::StringView name) override {
            /* Light count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, lightCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(light.importerState(), reinterpret_cast<const void*>(std::size_t{0xdeadbeef}));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.lightCount(), 0);
    CORRADE_COMPARE(converter.add(LightData{LightType::Point, {}, 0.0f, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.lightCount(), 1);
    CORRADE_COMPARE(converter.add(LightData{LightType::Point, {}, 0.0f, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})}, "hello"), 1);
    CORRADE_COMPARE(converter.lightCount(), 2);
}

void AbstractSceneConverterTest::addLightFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddLights;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const LightData&, Containers::StringView) override {
            return false;
        }
    } converter;

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.lightCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        Containers::String out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add(LightData{LightType::Point, {}, 0.0f}));
        CORRADE_COMPARE(out, "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.lightCount(), 0);
}

void AbstractSceneConverterTest::addLightNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddLights;
        }

        bool doBegin() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.add(LightData{LightType::Point, {}, 0.0f});
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): light conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::addCamera() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddCameras;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt id, const CameraData& camera, Containers::StringView name) override {
            /* Camera count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, cameraCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(camera.importerState(), reinterpret_cast<const void*>(std::size_t{0xdeadbeef}));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.cameraCount(), 0);
    CORRADE_COMPARE(converter.add(CameraData{CameraType::Orthographic3D, {}, 0.0f, 1.0f, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.cameraCount(), 1);
    CORRADE_COMPARE(converter.add(CameraData{CameraType::Orthographic3D, {}, 0.0f, 1.0f, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})}, "hello"), 1);
    CORRADE_COMPARE(converter.cameraCount(), 2);
}

void AbstractSceneConverterTest::addCameraFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddCameras;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const CameraData&, Containers::StringView) override {
            return false;
        }
    } converter;

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.cameraCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        Containers::String out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add(CameraData{CameraType::Orthographic3D, {}, 0.0f, 1.0f}));
        CORRADE_COMPARE(out, "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.cameraCount(), 0);
}

void AbstractSceneConverterTest::addCameraNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddCameras;
        }

        bool doBegin() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.add(CameraData{CameraType::Orthographic3D, {}, 0.0f, 1.0f});
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): camera conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::addSkin2D() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddSkins2D;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt id, const SkinData2D& skin, Containers::StringView name) override {
            /* Camera count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, skin2DCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(skin.importerState(), reinterpret_cast<const void*>(std::size_t{0xdeadbeef}));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.skin2DCount(), 0);
    CORRADE_COMPARE(converter.add(SkinData2D{nullptr, nullptr, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.skin2DCount(), 1);
    CORRADE_COMPARE(converter.add(SkinData2D{nullptr, nullptr, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})}, "hello"), 1);
    CORRADE_COMPARE(converter.skin2DCount(), 2);
}

void AbstractSceneConverterTest::addSkin2DFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddSkins2D;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const SkinData2D&, Containers::StringView) override {
            return false;
        }
    } converter;

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.skin2DCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        Containers::String out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add(SkinData2D{nullptr, nullptr}));
        CORRADE_COMPARE(out, "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.skin2DCount(), 0);
}

void AbstractSceneConverterTest::addSkin2DNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddSkins2D;
        }

        bool doBegin() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.add(SkinData2D{nullptr, nullptr});
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): 2D skin conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::addSkin3D() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddSkins3D;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt id, const SkinData3D& skin, Containers::StringView name) override {
            /* Camera count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, skin3DCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(skin.importerState(), reinterpret_cast<const void*>(std::size_t{0xdeadbeef}));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.skin3DCount(), 0);
    CORRADE_COMPARE(converter.add(SkinData3D{nullptr, nullptr, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.skin3DCount(), 1);
    CORRADE_COMPARE(converter.add(SkinData3D{nullptr, nullptr, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})}, "hello"), 1);
    CORRADE_COMPARE(converter.skin3DCount(), 2);
}

void AbstractSceneConverterTest::addSkin3DFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddSkins3D;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const SkinData3D&, Containers::StringView) override {
            return false;
        }
    } converter;

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.skin3DCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        Containers::String out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add(SkinData3D{nullptr, nullptr}));
        CORRADE_COMPARE(out, "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.skin3DCount(), 0);
}

void AbstractSceneConverterTest::addSkin3DNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddSkins3D;
        }

        bool doBegin() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.add(SkinData3D{nullptr, nullptr});
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): 3D skin conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::addMesh() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddMeshes;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt id, const MeshData& mesh, Containers::StringView name) override {
            /* Camera count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, meshCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(mesh.importerState(), reinterpret_cast<const void*>(std::size_t{0xdeadbeef}));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.meshCount(), 0);
    CORRADE_COMPARE(converter.add(MeshData{MeshPrimitive::Triangles, 0, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.meshCount(), 1);
    CORRADE_COMPARE(converter.add(MeshData{MeshPrimitive::Triangles, 0, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})}, "hello"), 1);
    CORRADE_COMPARE(converter.meshCount(), 2);
}

void AbstractSceneConverterTest::addMeshFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddMeshes;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const MeshData&, Containers::StringView) override {
            return false;
        }
    } converter;

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.meshCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        Containers::String out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add(MeshData{MeshPrimitive::Triangles, 0}));
        CORRADE_COMPARE(out, "");
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

        bool doBegin() override {
            CORRADE_FAIL("doBegin() should not be called");
            return {};
        }

        Containers::Pointer<AbstractImporter> doEnd() override {
            CORRADE_FAIL("doEnd() should not be called");
            return {};
        }

        bool doAdd(UnsignedInt, const Trade::MeshData&, Containers::StringView) override {
            CORRADE_FAIL("doAdd() should not be called");
            return {};
        }
    } converter;

    CORRADE_VERIFY(true); /* capture correct function name */

    CORRADE_VERIFY(converter.begin());

    CORRADE_COMPARE(converter.meshCount(), 0);
    CORRADE_COMPARE(converter.add(MeshData{MeshPrimitive::Triangles, 6}), 0);
    CORRADE_COMPARE(converter.meshCount(), 1);

    Containers::Pointer<AbstractImporter> importer = converter.end();
    CORRADE_VERIFY(!converter.isConverting());
    CORRADE_VERIFY(importer);
    CORRADE_VERIFY(importer->isOpened());
    CORRADE_COMPARE(importer->meshCount(), 1);

    Containers::Optional<MeshData> mesh = importer->mesh(0);
    CORRADE_VERIFY(mesh);
    CORRADE_COMPARE(mesh->primitive(), MeshPrimitive::Lines);
    CORRADE_COMPARE(mesh->vertexCount(), 12);

    /* The mesh is returned only once, second time it will fail (but just an
       error, not an assert */
    {
        Containers::String out;
        Error redirectError{&out};
        CORRADE_VERIFY(!importer->mesh(0));
        CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::end(): mesh can be retrieved only once from a converter with just Trade::SceneConverterFeature::ConvertMesh\n");
    }

    /* Verify that it's also possible to close the importer without hitting
       some nasty assert */
    importer->close();
    CORRADE_VERIFY(!importer->isOpened());
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

    CORRADE_VERIFY(converter.begin());

    CORRADE_VERIFY(!converter.add(MeshData{MeshPrimitive::Lines, 6}));
    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.meshCount(), 0);

    /* But the observable behavior is as if no mesh was added */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.end());
    CORRADE_VERIFY(!converter.isConverting());
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::end(): the converter requires exactly one mesh, got 0\n");
}

void AbstractSceneConverterTest::addMeshThroughConvertMeshZeroMeshes() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMesh;
        }
    } converter;

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.end());
    CORRADE_VERIFY(!converter.isConverting());
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::end(): the converter requires exactly one mesh, got 0\n");
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

    CORRADE_VERIFY(converter.begin());

    CORRADE_COMPARE(converter.meshCount(), 0);
    CORRADE_COMPARE(converter.add(MeshData{MeshPrimitive::Triangles, 6}), 0);
    CORRADE_COMPARE(converter.meshCount(), 1);

    {
        Containers::String out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add(MeshData{MeshPrimitive::Triangles, 7}));
        /* It shouldn't abort the whole process */
        CORRADE_VERIFY(converter.isConverting());
        CORRADE_COMPARE(converter.meshCount(), 1);
        CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): the converter requires exactly one mesh, got 2\n");
    }

    /* Getting the result should still work */
    Containers::Pointer<AbstractImporter> out = converter.end();
    CORRADE_VERIFY(!converter.isConverting());
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

        bool doBeginData() override {
            CORRADE_FAIL("doBeginData() should not be called");
            return {};
        }

        Containers::Optional<Containers::Array<char>> doEndData() override {
            CORRADE_FAIL("doEndData() should not be called");
            return {};
        }

        bool doAdd(UnsignedInt, const Trade::MeshData&, Containers::StringView) override {
            CORRADE_FAIL("doAdd() should not be called");
            return {};
        }
    } converter;

    CORRADE_VERIFY(converter.beginData());

    CORRADE_COMPARE(converter.meshCount(), 0);
    CORRADE_COMPARE(converter.add(MeshData{MeshPrimitive::Triangles, 6}), 0);
    CORRADE_COMPARE(converter.meshCount(), 1);

    Containers::Optional<Containers::Array<char>> data = converter.endData();
    CORRADE_VERIFY(!converter.isConverting());
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

    CORRADE_VERIFY(converter.beginData());

    CORRADE_VERIFY(!converter.add(MeshData{MeshPrimitive::Lines, 6}));
    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.meshCount(), 0);

    /* But the observable behavior is as if no mesh was added */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.endData());
    CORRADE_VERIFY(!converter.isConverting());
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::endData(): the converter requires exactly one mesh, got 0\n");
}

void AbstractSceneConverterTest::addMeshThroughConvertMeshToDataZeroMeshes() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMeshToData;
        }
    } converter;

    CORRADE_VERIFY(converter.beginData());

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.endData());
    CORRADE_VERIFY(!converter.isConverting());
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::endData(): the converter requires exactly one mesh, got 0\n");
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

    CORRADE_VERIFY(converter.beginData());

    CORRADE_COMPARE(converter.meshCount(), 0);
    CORRADE_COMPARE(converter.add(MeshData{MeshPrimitive::Triangles, 6}), 0);
    CORRADE_COMPARE(converter.meshCount(), 1);

    {
        Containers::String out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add(MeshData{MeshPrimitive::Triangles, 7}));
        /* It shouldn't abort the whole process */
        CORRADE_VERIFY(converter.isConverting());
        CORRADE_COMPARE(converter.meshCount(), 1);
        CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): the converter requires exactly one mesh, got 2\n");
    }

    /* Getting the result should still work */
    Containers::Optional<Containers::Array<char>> data = converter.endData();
    CORRADE_VERIFY(!converter.isConverting());
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->size(), 6);
}

void AbstractSceneConverterTest::addMeshThroughConvertMeshToFile() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMeshToFile;
        }

        bool doConvertToFile(const MeshData& mesh, Containers::StringView filename) override {
            return Utility::Path::write(filename, Containers::arrayView({char(mesh.vertexCount())}));
        }

        bool doBeginFile(Containers::StringView) override {
            CORRADE_FAIL("doBeginFile() should not be called");
            return {};
        }

        bool doEndFile(Containers::StringView) override {
            CORRADE_FAIL("doEndFile() should not be called");
            return {};
        }

        bool doAdd(UnsignedInt, const Trade::MeshData&, Containers::StringView) override {
            CORRADE_FAIL("doAdd() should not be called");
            return {};
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "mesh.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    CORRADE_VERIFY(converter.beginFile(filename));
    CORRADE_VERIFY(!Utility::Path::exists(filename));

    CORRADE_COMPARE(converter.meshCount(), 0);
    CORRADE_COMPARE(converter.add(MeshData{MeshPrimitive::Triangles, 0xef}), 0);
    CORRADE_COMPARE(converter.meshCount(), 1);

    /* It's easier to just perform the operation right during add() than to
       make a copy of the passed MeshData and do it at endFile(), so the file
       exists at this point already */
    CORRADE_VERIFY(Utility::Path::exists(filename));

    CORRADE_VERIFY(converter.endFile());
    CORRADE_VERIFY(!converter.isConverting());
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

        bool doBeginData() override {
            CORRADE_FAIL("doBeginData() should not be called");
            return {};
        }

        Containers::Optional<Containers::Array<char>> doEndData() override {
            CORRADE_FAIL("doEndData() should not be called");
            return {};
        }

        bool doBeginFile(Containers::StringView) override {
            CORRADE_FAIL("doBeginFile() should not be called");
            return {};
        }

        bool doEndFile(Containers::StringView) override {
            CORRADE_FAIL("doEndFile() should not be called");
            return {};
        }

        bool doAdd(UnsignedInt, const Trade::MeshData&, Containers::StringView) override {
            CORRADE_FAIL("doAdd() should not be called");
            return {};
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "mesh.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    CORRADE_VERIFY(converter.beginFile(filename));
    CORRADE_VERIFY(!Utility::Path::exists(filename));

    CORRADE_COMPARE(converter.meshCount(), 0);
    CORRADE_COMPARE(converter.add(MeshData{MeshPrimitive::Triangles, 0xef}), 0);
    CORRADE_COMPARE(converter.meshCount(), 1);

    /* It's easier to just perform the operation right during add() than to
       make a copy of the passed MeshData and do it at the end(), so the file
       exists at this point already */
    CORRADE_VERIFY(Utility::Path::exists(filename));

    CORRADE_VERIFY(converter.endFile());
    CORRADE_VERIFY(!converter.isConverting());
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

    CORRADE_VERIFY(converter.beginFile(filename));
    CORRADE_VERIFY(!Utility::Path::exists(filename));

    CORRADE_VERIFY(!converter.add(MeshData{MeshPrimitive::Lines, 6}));
    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.meshCount(), 0);

    /* But the observable behavior is as if no mesh was added */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.endFile());
    CORRADE_VERIFY(!converter.isConverting());
    CORRADE_VERIFY(!Utility::Path::exists(filename));
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::endFile(): the converter requires exactly one mesh, got 0\n");
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

    CORRADE_VERIFY(converter.beginFile(filename));
    CORRADE_VERIFY(!Utility::Path::exists(filename));

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.endFile());
    CORRADE_VERIFY(!converter.isConverting());
    CORRADE_VERIFY(!Utility::Path::exists(filename));
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::endFile(): the converter requires exactly one mesh, got 0\n");
}

void AbstractSceneConverterTest::addMeshThroughConvertMeshToFileTwoMeshes() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMeshToFile;
        }

        bool doConvertToFile(const MeshData& mesh, Containers::StringView filename) override {
            return Utility::Path::write(filename, Containers::arrayView({char(mesh.vertexCount())}));
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "mesh.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    CORRADE_VERIFY(converter.beginFile(filename));
    CORRADE_VERIFY(!Utility::Path::exists(filename));

    CORRADE_COMPARE(converter.meshCount(), 0);
    CORRADE_COMPARE(converter.add(MeshData{MeshPrimitive::Triangles, 0xef}), 0);
    CORRADE_COMPARE(converter.meshCount(), 1);

    /* It's easier to just perform the operation right during add() than to
       make a copy of the passed MeshData and do it at the endFile(), so the
       file exists at this point already */
    CORRADE_VERIFY(Utility::Path::exists(filename));

    {
        Containers::String out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add(MeshData{MeshPrimitive::Triangles, 0xb0}));
        /* It shouldn't abort the whole process */
        CORRADE_VERIFY(converter.isConverting());
        CORRADE_COMPARE(converter.meshCount(), 1);
        CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): the converter requires exactly one mesh, got 2\n");
    }

    /* Getting the result should still work */
    CORRADE_VERIFY(converter.endFile());
    CORRADE_VERIFY(!converter.isConverting());
    CORRADE_VERIFY(Utility::Path::exists(filename));
    CORRADE_COMPARE_AS(filename,
        "\xef", TestSuite::Compare::FileToString);
}

void AbstractSceneConverterTest::addMeshNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddMeshes;
        }

        bool doBegin() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.add(MeshData{MeshPrimitive::Triangles, 0});
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): mesh conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::addMeshLevels() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddMeshes|
                   SceneConverterFeature::MeshLevels;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt id, const Containers::Iterable<const MeshData>& meshLevels, Containers::StringView name) override {
            /* Camera count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, meshCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(meshLevels.size(), 3);
            CORRADE_COMPARE(meshLevels[1].importerState(), reinterpret_cast<const void*>(std::size_t{0xdeadbeef}));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.meshCount(), 0);
    CORRADE_COMPARE(converter.add({
        MeshData{MeshPrimitive::Lines, 0},
        MeshData{MeshPrimitive::Triangles, 3, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})},
        MeshData{MeshPrimitive::Faces, 0},
    }, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.meshCount(), 1);
    CORRADE_COMPARE(converter.add({
        MeshData{MeshPrimitive::Faces, 2},
        MeshData{MeshPrimitive::Meshlets, 1, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})},
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

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const Containers::Iterable<const MeshData>&, Containers::StringView) override {
            return false;
        }
    } converter;

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.meshCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        Containers::String out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add({
            MeshData{MeshPrimitive::Triangles, 0},
            MeshData{MeshPrimitive::Triangles, 0}
        }));
        CORRADE_COMPARE(out, "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.meshCount(), 0);
}

void AbstractSceneConverterTest::addMeshLevelsNoLevels() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddMeshes|
                   SceneConverterFeature::MeshLevels;
        }

        bool doBegin() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.add(Containers::Iterable<const MeshData>{});
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): at least one mesh level has to be specified\n");
}

void AbstractSceneConverterTest::addMeshLevelsNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddMeshes|
                   SceneConverterFeature::MeshLevels;
        }

        bool doBegin() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.add({
        MeshData{MeshPrimitive::Triangles, 0},
        MeshData{MeshPrimitive::Triangles, 0}
    });
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): multi-level mesh conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::addMeshThroughLevels() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddMeshes|
                   SceneConverterFeature::MeshLevels;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const Containers::Iterable<const MeshData>& meshLevels, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(meshLevels.size(), 1);
            CORRADE_COMPARE(meshLevels[0].importerState(), reinterpret_cast<const void*>(std::size_t{0xdeadbeef}));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.add(MeshData{MeshPrimitive::Triangles, 0, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.meshCount(), 1);
}

void AbstractSceneConverterTest::setMeshAttributeName() {
    auto&& data = SetMeshAttributeData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct Converter: AbstractSceneConverter {
        explicit Converter(SceneConverterFeatures features): features{features} {}

        SceneConverterFeatures doFeatures() const override {
            return features;
        }

        bool doBegin() override { return true; }

        void doSetMeshAttributeName(MeshAttribute field, Containers::StringView name) override {
            CORRADE_COMPARE(field, meshAttributeCustom(1337));
            CORRADE_COMPARE(name, "hello!");
            setMeshAttributeNameCalled = true;
        }

        SceneConverterFeatures features;
        bool setMeshAttributeNameCalled = false;
    } converter{data.features};

    CORRADE_VERIFY(true); /* capture correct function name */

    /* Only single-mesh conversion can call the API alone */
    if(data.features & SceneConverterFeature::ConvertMultiple)
        CORRADE_VERIFY(converter.begin());

    converter.setMeshAttributeName(meshAttributeCustom(1337), "hello!");
    CORRADE_VERIFY(converter.setMeshAttributeNameCalled);
}

void AbstractSceneConverterTest::setMeshAttributeNameNotImplemented() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddMeshes;
        }

        bool doBegin() override { return true; }
    } converter;

    /* This should work, there's no need for a plugin to implement this */
    CORRADE_VERIFY(converter.begin());
    converter.setMeshAttributeName(meshAttributeCustom(1337), "hello!");
    CORRADE_VERIFY(true);
}

void AbstractSceneConverterTest::setMeshAttributeNameNotCustom() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddMeshes;
        }

        bool doBegin() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.setMeshAttributeName(MeshAttribute::ObjectId, "hello!");
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::setMeshAttributeName(): Trade::MeshAttribute::ObjectId is not custom\n");
}

void AbstractSceneConverterTest::addMaterial() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddMaterials;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt id, const MaterialData& material, Containers::StringView name) override {
            /* Camera count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, materialCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(material.importerState(), reinterpret_cast<const void*>(std::size_t{0xdeadbeef}));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.materialCount(), 0);
    CORRADE_COMPARE(converter.add(MaterialData{{}, nullptr, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.materialCount(), 1);
    CORRADE_COMPARE(converter.add(MaterialData{{}, nullptr, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})}, "hello"), 1);
    CORRADE_COMPARE(converter.materialCount(), 2);
}

void AbstractSceneConverterTest::addMaterialFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddMaterials;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const MaterialData&, Containers::StringView) override {
            return false;
        }
    } converter;

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.materialCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        Containers::String out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add(MaterialData{{}, nullptr}));
        CORRADE_COMPARE(out, "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.materialCount(), 0);
}

void AbstractSceneConverterTest::addMaterialNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddMaterials;
        }

        bool doBegin() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.add(MaterialData{{}, nullptr});
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): material conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::addTexture() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddTextures;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt id, const TextureData& texture, Containers::StringView name) override {
            /* Camera count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, textureCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(texture.importerState(), reinterpret_cast<const void*>(std::size_t{0xdeadbeef}));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.textureCount(), 0);
    CORRADE_COMPARE(converter.add(TextureData{{}, {}, {}, {}, {}, 0, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.textureCount(), 1);
    CORRADE_COMPARE(converter.add(TextureData{{}, {}, {}, {}, {}, 0, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})}, "hello"), 1);
    CORRADE_COMPARE(converter.textureCount(), 2);
}

void AbstractSceneConverterTest::addTextureFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddTextures;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const TextureData&, Containers::StringView) override {
            return false;
        }
    } converter;

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.textureCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        Containers::String out;
        Error redirectError{&out};
        CORRADE_VERIFY(!converter.add(TextureData{{}, {}, {}, {}, {}, 0}));
        CORRADE_COMPARE(out, "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.textureCount(), 0);
}

void AbstractSceneConverterTest::addTextureNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddTextures;
        }

        bool doBegin() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.add(TextureData{{}, {}, {}, {}, {}, 0});
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): texture conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::addImage1D() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages1D;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt id, const ImageData1D& image, Containers::StringView name) override {
            /* Camera count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, image1DCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(image.importerState(), reinterpret_cast<const void*>(std::size_t{0xdeadbeef}));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[4]{};

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.image1DCount(), 0);
    CORRADE_COMPARE(converter.add(ImageData1D{PixelFormat::RGBA8Unorm, 1, {}, imageData, ImageFlags1D{}, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image1DCount(), 1);
    CORRADE_COMPARE(converter.add(ImageData1D{PixelFormat::RGBA8Unorm, 1, {}, imageData, ImageFlags1D{}, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})}, "hello"), 1);
    CORRADE_COMPARE(converter.image1DCount(), 2);
}

void AbstractSceneConverterTest::addImage1DView() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages1D;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const ImageData1D& image, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_VERIFY(!image.isCompressed());
            CORRADE_COMPARE(image.storage().alignment(), 2);
            CORRADE_COMPARE(image.format(), PixelFormat::RG8Snorm);
            CORRADE_COMPARE(image.size(), 3);
            CORRADE_COMPARE(image.flags(), ImageFlags1D{});
            CORRADE_VERIFY(image.data());

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[6]{};

    CORRADE_VERIFY(converter.begin());
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

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const ImageData1D& image, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_VERIFY(image.isCompressed());
            CORRADE_COMPARE(image.compressedStorage().compressedBlockSize(), Vector3i{3});
            CORRADE_COMPARE(image.compressedFormat(), CompressedPixelFormat::Astc3x3x3RGBASrgb);
            CORRADE_COMPARE(image.size(), 3);
            CORRADE_COMPARE(image.flags(), ImageFlags1D{});
            CORRADE_VERIFY(image.data());

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[4*4]{};

    CORRADE_VERIFY(converter.begin());
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
                   SceneConverterFeature::AddImages1D|
                   SceneConverterFeature::AddCompressedImages1D;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const ImageData1D&, Containers::StringView) override {
            return false;
        }
    } converter;

    const char imageData[16]{};

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.image1DCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        Containers::String out;
        Error redirectError{&out};
        /* Testing all three variants to "fake" coverage for the name-less
           overloads as well */
        CORRADE_VERIFY(!converter.add(ImageData1D{PixelFormat::RGBA8Unorm, 1, DataFlags{}, imageData}));
        CORRADE_VERIFY(!converter.add(ImageView1D{PixelFormat::RGBA8Unorm, 1, imageData}));
        CORRADE_VERIFY(!converter.add(CompressedImageView1D{CompressedPixelFormat::Astc4x4RGBAUnorm, 1, imageData}));
        CORRADE_COMPARE(out, "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.image1DCount(), 0);
}

void AbstractSceneConverterTest::addImage1DInvalidImage() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages1D;
        }

        bool doBegin() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.add(ImageData1D{PixelFormat::RGBA8Unorm, {}, nullptr});
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): can't add image with a zero size: Vector(0)\n");
}

void AbstractSceneConverterTest::addImage1DNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages1D;
        }

        bool doBegin() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.begin());

    const char imageData[4]{};

    Containers::String out;
    Error redirectError{&out};
    converter.add(ImageData1D{PixelFormat::RGBA8Unorm, 1, DataFlags{}, imageData});
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): 1D image conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::addImage2D() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt id, const ImageData2D& image, Containers::StringView name) override {
            /* Camera count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, image2DCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(image.importerState(), reinterpret_cast<const void*>(std::size_t{0xdeadbeef}));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[4]{};

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.image2DCount(), 0);
    CORRADE_COMPARE(converter.add(ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, {}, imageData, ImageFlags2D{}, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image2DCount(), 1);
    CORRADE_COMPARE(converter.add(ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, {}, imageData, ImageFlags2D{}, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})}, "hello"), 1);
    CORRADE_COMPARE(converter.image2DCount(), 2);
}

void AbstractSceneConverterTest::addImage2DView() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const ImageData2D& image, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_VERIFY(!image.isCompressed());
            CORRADE_COMPARE(image.storage().alignment(), 2);
            CORRADE_COMPARE(image.format(), PixelFormat::RG8Snorm);
            CORRADE_COMPARE(image.size(), (Vector2i{3, 1}));
            CORRADE_COMPARE(image.flags(), ImageFlag2D::Array);
            CORRADE_VERIFY(image.data());

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[6]{};

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.image2DCount(), 0);
    CORRADE_COMPARE(converter.add(ImageView2D{
        PixelStorage{}.setAlignment(2),
        PixelFormat::RG8Snorm, {3, 1}, imageData, ImageFlag2D::Array},
        "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image2DCount(), 1);
}

void AbstractSceneConverterTest::addImage2DCompressedView() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddCompressedImages2D;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const ImageData2D& image, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_VERIFY(image.isCompressed());
            CORRADE_COMPARE(image.compressedStorage().compressedBlockSize(), Vector3i{3});
            CORRADE_COMPARE(image.compressedFormat(), CompressedPixelFormat::Astc3x3x3RGBASrgb);
            CORRADE_COMPARE(image.size(), (Vector2i{3, 2}));
            CORRADE_COMPARE(image.flags(), ImageFlag2D::Array);
            CORRADE_VERIFY(image.data());

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[4*4]{};

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.image2DCount(), 0);
    CORRADE_COMPARE(converter.add(CompressedImageView2D{
        CompressedPixelStorage{}.setCompressedBlockSize({3, 3, 3}),
        CompressedPixelFormat::Astc3x3x3RGBASrgb, {3, 2}, imageData,
        ImageFlag2D::Array}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image2DCount(), 1);
}

void AbstractSceneConverterTest::addImage2DFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D|
                   SceneConverterFeature::AddCompressedImages2D;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const ImageData2D&, Containers::StringView) override {
            return false;
        }
    } converter;

    const char imageData[16]{};

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.image2DCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        Containers::String out;
        Error redirectError{&out};
        /* Testing all three variants to "fake" coverage for the name-less
           overloads as well */
        CORRADE_VERIFY(!converter.add(ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, DataFlags{}, imageData}));
        CORRADE_VERIFY(!converter.add(ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, imageData}));
        CORRADE_VERIFY(!converter.add(CompressedImageView2D{CompressedPixelFormat::Astc4x4RGBAUnorm, {1, 1}, imageData}));
        CORRADE_COMPARE(out, "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.image2DCount(), 0);
}

void AbstractSceneConverterTest::addImage2DZeroSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D;
        }

        bool doBegin() override { return true; }
    } converter;

    const char imageData[16]{};

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.add(ImageData2D{PixelFormat::RGBA8Unorm, {4, 0}, DataFlags{}, imageData});
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): can't add image with a zero size: Vector(4, 0)\n");
}

void AbstractSceneConverterTest::addImage2DNullptr() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D;
        }

        bool doBegin() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.add(ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, DataFlags{}, {nullptr, 4}});
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): can't add image with a nullptr view\n");
}

void AbstractSceneConverterTest::addImage2DNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D;
        }

        bool doBegin() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.begin());

    const char imageData[4]{};

    Containers::String out;
    Error redirectError{&out};
    converter.add(ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, DataFlags{}, imageData});
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): 2D image conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::addImage3D() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages3D;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt id, const ImageData3D& image, Containers::StringView name) override {
            /* Camera count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, image3DCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(image.importerState(), reinterpret_cast<const void*>(std::size_t{0xdeadbeef}));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[4]{};

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.image3DCount(), 0);
    CORRADE_COMPARE(converter.add(ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, {}, imageData, ImageFlags3D{}, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image3DCount(), 1);
    CORRADE_COMPARE(converter.add(ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, {}, imageData, ImageFlags3D{}, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})}, "hello"), 1);
    CORRADE_COMPARE(converter.image3DCount(), 2);
}

void AbstractSceneConverterTest::addImage3DView() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages3D;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const ImageData3D& image, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_VERIFY(!image.isCompressed());
            CORRADE_COMPARE(image.storage().alignment(), 2);
            CORRADE_COMPARE(image.format(), PixelFormat::RG8Snorm);
            CORRADE_COMPARE(image.size(), (Vector3i{1, 3, 1}));
            CORRADE_COMPARE(image.flags(), ImageFlag3D::Array);
            CORRADE_VERIFY(image.data());

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[6]{};

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.image3DCount(), 0);
    CORRADE_COMPARE(converter.add(ImageView3D{
        PixelStorage{}.setAlignment(2),
        PixelFormat::RG8Snorm, {1, 3, 1}, imageData, ImageFlag3D::Array},
        "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image3DCount(), 1);
}

void AbstractSceneConverterTest::addImage3DCompressedView() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddCompressedImages3D;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const ImageData3D& image, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_VERIFY(image.isCompressed());
            CORRADE_COMPARE(image.compressedStorage().compressedBlockSize(), Vector3i{3});
            CORRADE_COMPARE(image.compressedFormat(), CompressedPixelFormat::Astc3x3x3RGBASrgb);
            CORRADE_COMPARE(image.size(), (Vector3i{1, 3, 1}));
            CORRADE_COMPARE(image.flags(), ImageFlag3D::Array);
            CORRADE_VERIFY(image.data());

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[4*4]{};

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.image3DCount(), 0);
    CORRADE_COMPARE(converter.add(CompressedImageView3D{
        CompressedPixelStorage{}.setCompressedBlockSize({3, 3, 3}),
        CompressedPixelFormat::Astc3x3x3RGBASrgb, {1, 3, 1}, imageData,
        ImageFlag3D::Array}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image3DCount(), 1);
}

void AbstractSceneConverterTest::addImage3DFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages3D|
                   SceneConverterFeature::AddCompressedImages3D;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const ImageData3D&, Containers::StringView) override {
            return false;
        }
    } converter;

    const char imageData[16]{};

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.image3DCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        Containers::String out;
        Error redirectError{&out};
        /* Testing all three variants to "fake" coverage for the name-less
           overloads as well */
        CORRADE_VERIFY(!converter.add(ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, DataFlags{}, imageData}));
        CORRADE_VERIFY(!converter.add(ImageView3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, imageData}));
        CORRADE_VERIFY(!converter.add(CompressedImageView3D{CompressedPixelFormat::Astc4x4RGBAUnorm, {1, 1, 1}, imageData}));
        CORRADE_COMPARE(out, "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.image3DCount(), 0);
}

void AbstractSceneConverterTest::addImage3DInvalidImage() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages3D;
        }

        bool doBegin() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.add(ImageData3D{PixelFormat::RGBA8Unorm, {}, nullptr});
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): can't add image with a zero size: Vector(0, 0, 0)\n");
}

void AbstractSceneConverterTest::addImage3DNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages3D;
        }

        bool doBegin() override { return true; }
    } converter;

    const char imageData[4]{};

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.add(ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, DataFlags{}, imageData});
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): 3D image conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::addImageLevels1D() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages1D|
                   SceneConverterFeature::ImageLevels;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt id, const Containers::Iterable<const ImageData1D>& imageLevels, Containers::StringView name) override {
            /* Camera count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, image1DCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(imageLevels.size(), 3);
            CORRADE_COMPARE(imageLevels[1].importerState(), reinterpret_cast<const void*>(std::size_t{0xdeadbeef}));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[4*4]{};

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.image1DCount(), 0);
    CORRADE_COMPARE(converter.add({
        /* Arbitrary dimensions should be fine */
        ImageData1D{PixelFormat::RGBA8Unorm, 4, DataFlags{}, imageData},
        ImageData1D{PixelFormat::RGBA8Unorm, 2, DataFlags{}, imageData, ImageFlags1D{}, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})},
        ImageData1D{PixelFormat::RGBA8Unorm, 3, DataFlags{}, imageData}
    }, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image1DCount(), 1);
    CORRADE_COMPARE(converter.add({
        ImageData1D{PixelFormat::RGBA8Unorm, 2, DataFlags{}, imageData},
        ImageData1D{PixelFormat::RGBA8Unorm, 1, DataFlags{}, imageData, ImageFlags1D{}, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})},
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

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const Containers::Iterable<const ImageData1D>& imageLevels, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(imageLevels.size(), 2);
            CORRADE_VERIFY(!imageLevels[1].isCompressed());
            CORRADE_COMPARE(imageLevels[1].storage().alignment(), 2);
            CORRADE_COMPARE(imageLevels[1].format(), PixelFormat::RG8Snorm);
            CORRADE_COMPARE(imageLevels[1].size(), 3);
            CORRADE_COMPARE(imageLevels[1].flags(), ImageFlags1D{});
            CORRADE_VERIFY(imageLevels[1].data());

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[6]{};

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.image1DCount(), 0);
    CORRADE_COMPARE(converter.add({
        ImageView1D{PixelFormat::RG8Snorm, 1, imageData},
        ImageView1D{PixelStorage{}.setAlignment(2), PixelFormat::RG8Snorm, 3, imageData}
    }, "hello"), 0);
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

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const Containers::Iterable<const ImageData1D>& imageLevels, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(imageLevels.size(), 2);
            CORRADE_VERIFY(imageLevels[1].isCompressed());
            CORRADE_COMPARE(imageLevels[1].compressedStorage().rowLength(), 6);
            CORRADE_COMPARE(imageLevels[1].compressedFormat(), CompressedPixelFormat::Astc3x3x3RGBASrgb);
            CORRADE_COMPARE(imageLevels[1].size(), 3);
            CORRADE_COMPARE(imageLevels[1].flags(), ImageFlags1D{});
            CORRADE_VERIFY(imageLevels[1].data());

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[2*16]{};

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.image1DCount(), 0);
    CORRADE_COMPARE(converter.add({
        CompressedImageView1D{CompressedPixelFormat::Astc3x3x3RGBASrgb, 1, imageData},
        CompressedImageView1D{CompressedPixelStorage{}.setRowLength(6), CompressedPixelFormat::Astc3x3x3RGBASrgb, 3, imageData}
    }, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image1DCount(), 1);
}

void AbstractSceneConverterTest::addImageLevels1DFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages1D|
                   SceneConverterFeature::AddCompressedImages1D|
                   SceneConverterFeature::ImageLevels;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const Containers::Iterable<const ImageData1D>&, Containers::StringView) override {
            return false;
        }
    } converter;

    const char imageData[16]{};

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.image1DCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        Containers::String out;
        Error redirectError{&out};
        /* Testing all three variants to "fake" coverage for the name-less
           overloads as well */
        CORRADE_VERIFY(!converter.add({
            ImageData1D{PixelFormat::RGBA8Unorm, 1, DataFlags{}, imageData},
            ImageData1D{PixelFormat::RGBA8Unorm, 1, DataFlags{}, imageData}
        }));
        CORRADE_VERIFY(!converter.add({
            ImageView1D{PixelFormat::RGBA8Unorm, 1, imageData},
            ImageView1D{PixelFormat::RGBA8Unorm, 1, imageData},
        }));
        CORRADE_VERIFY(!converter.add({
            CompressedImageView1D{CompressedPixelFormat::Astc4x4RGBAUnorm, 1, imageData},
            CompressedImageView1D{CompressedPixelFormat::Astc4x4RGBAUnorm, 1, imageData},
        }));
        CORRADE_COMPARE(out, "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.image1DCount(), 0);
}

void AbstractSceneConverterTest::addImageLevels1DInvalidImage() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages1D|
                   SceneConverterFeature::ImageLevels;
        }

        bool doBegin() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.add(std::initializer_list<Containers::AnyReference<const ImageData1D>>{});
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): at least one image level has to be specified\n");
}

void AbstractSceneConverterTest::addImageLevels1DNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages1D|
                   SceneConverterFeature::ImageLevels;
        }

        bool doBegin() override { return true; }
    } converter;

    const char imageData[4]{};

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.add({
        ImageData1D{PixelFormat::RGBA8Unorm, 1, DataFlags{}, imageData},
        ImageData1D{PixelFormat::RGBA8Unorm, 1, DataFlags{}, imageData}
    });
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): multi-level 1D image conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::addImageLevels2D() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D|
                   SceneConverterFeature::ImageLevels;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt id, const Containers::Iterable<const ImageData2D>& imageLevels, Containers::StringView name) override {
            /* Camera count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, image2DCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(imageLevels.size(), 3);
            CORRADE_COMPARE(imageLevels[1].importerState(), reinterpret_cast<const void*>(std::size_t{0xdeadbeef}));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[4*4]{};

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.image2DCount(), 0);
    CORRADE_COMPARE(converter.add({
        /* Arbitrary dimensions should be fine */
        ImageData2D{PixelFormat::RGBA8Unorm, {4, 1}, DataFlags{}, imageData},
        ImageData2D{PixelFormat::RGBA8Unorm, {2, 2}, DataFlags{}, imageData, ImageFlags2D{}, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})},
        ImageData2D{PixelFormat::RGBA8Unorm, {1, 3}, DataFlags{}, imageData}
    }, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image2DCount(), 1);
    CORRADE_COMPARE(converter.add({
        ImageData2D{PixelFormat::RGBA8Unorm, {2, 2}, DataFlags{}, imageData},
        ImageData2D{PixelFormat::RGBA8Unorm, {1, 3}, DataFlags{}, imageData, ImageFlags2D{}, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})},
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

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const Containers::Iterable<const ImageData2D>& imageLevels, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(imageLevels.size(), 2);
            CORRADE_VERIFY(!imageLevels[1].isCompressed());
            CORRADE_COMPARE(imageLevels[1].storage().alignment(), 2);
            CORRADE_COMPARE(imageLevels[1].format(), PixelFormat::RG8Snorm);
            CORRADE_COMPARE(imageLevels[1].size(), (Vector2i{1, 3}));
            CORRADE_COMPARE(imageLevels[1].flags(), ImageFlag2D::Array);
            CORRADE_VERIFY(imageLevels[1].data());

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[6]{};

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.image2DCount(), 0);
    CORRADE_COMPARE(converter.add({
        ImageView2D{PixelFormat::RG8Snorm, {1, 1}, imageData, ImageFlag2D::Array},
        ImageView2D{PixelStorage{}.setAlignment(2), PixelFormat::RG8Snorm, {1, 3}, imageData, ImageFlag2D::Array}
    }, "hello"), 0);
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

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const Containers::Iterable<const ImageData2D>& imageLevels, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(imageLevels.size(), 2);
            CORRADE_VERIFY(imageLevels[1].isCompressed());
            CORRADE_COMPARE(imageLevels[1].compressedStorage().rowLength(), 6);
            CORRADE_COMPARE(imageLevels[1].compressedFormat(), CompressedPixelFormat::Astc3x3x3RGBASrgb);
            CORRADE_COMPARE(imageLevels[1].size(), (Vector2i{3, 1}));
            CORRADE_COMPARE(imageLevels[1].flags(), ImageFlag2D::Array);
            CORRADE_VERIFY(imageLevels[1].data());

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[2*16]{};

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.image2DCount(), 0);
    CORRADE_COMPARE(converter.add({
        CompressedImageView2D{CompressedPixelFormat::Astc3x3x3RGBASrgb, {1, 1}, imageData, ImageFlag2D::Array},
        CompressedImageView2D{CompressedPixelStorage{}.setRowLength(6), CompressedPixelFormat::Astc3x3x3RGBASrgb, {3, 1}, imageData, ImageFlag2D::Array}
    }, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image2DCount(), 1);
}

void AbstractSceneConverterTest::addImageLevels2DFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D|
                   SceneConverterFeature::AddCompressedImages2D|
                   SceneConverterFeature::ImageLevels;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const Containers::Iterable<const ImageData2D>&, Containers::StringView) override {
            return false;
        }
    } converter;

    const char imageData[16]{};

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.image2DCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        Containers::String out;
        Error redirectError{&out};
        /* Testing all three variants to "fake" coverage for the name-less
           overloads as well */
        CORRADE_VERIFY(!converter.add({
            ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, DataFlags{}, imageData},
            ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, DataFlags{}, imageData}
        }));
        CORRADE_VERIFY(!converter.add({
            ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, imageData},
            ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, imageData}
        }));
        CORRADE_VERIFY(!converter.add({
            CompressedImageView2D{CompressedPixelFormat::Astc4x4RGBAUnorm, {1, 1}, imageData},
            CompressedImageView2D{CompressedPixelFormat::Astc4x4RGBAUnorm, {1, 1}, imageData}
        }));
        CORRADE_COMPARE(out, "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.image2DCount(), 0);
}

void AbstractSceneConverterTest::addImageLevels2DNoLevels() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D|
                   SceneConverterFeature::ImageLevels;
        }

        bool doBegin() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.add(std::initializer_list<Containers::AnyReference<const ImageData2D>>{});
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): at least one image level has to be specified\n");
}

void AbstractSceneConverterTest::addImageLevels2DZeroSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D|
                   SceneConverterFeature::ImageLevels;
        }

        bool doBegin() override { return true; }
    } converter;

    const char imageData[4*4]{};

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.add({
        ImageData2D{PixelFormat::RGBA8Unorm, {2, 2}, DataFlags{}, imageData},
        ImageData2D{PixelFormat::RGBA8Unorm, {4, 0}, DataFlags{}, imageData}
    });
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): can't add image level 1 with a zero size: Vector(4, 0)\n");
}

void AbstractSceneConverterTest::addImageLevels2DNullptr() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D|
                   SceneConverterFeature::ImageLevels;
        }

        bool doBegin() override { return true; }
    } converter;

    const char imageData[4*4]{};

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.add({
        ImageData2D{PixelFormat::RGBA8Unorm, {2, 2}, DataFlags{}, imageData},
        ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, DataFlags{}, {nullptr, 4}}
    });
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): can't add image level 1 with a nullptr view\n");
}

void AbstractSceneConverterTest::addImageLevels2DInconsistentCompressed() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D|
                   SceneConverterFeature::ImageLevels;
        }

        bool doBegin() override { return true; }
    } converter;

    const char imageData[4*4]{};

    CORRADE_VERIFY(converter.begin());

    ImageData2D a{PixelFormat::RGBA8Unorm, {2, 2}, DataFlags{}, imageData};
    ImageData2D b{CompressedPixelFormat::Astc10x10RGBAF, {1, 1}, DataFlags{}, imageData};

    Containers::String out;
    Error redirectError{&out};
    converter.add({a, b});
    converter.add({b, b, a});
    CORRADE_COMPARE(out,
        "Trade::AbstractSceneConverter::add(): image level 1 is compressed but previous aren't\n"
        "Trade::AbstractSceneConverter::add(): image level 2 is not compressed but previous are\n");
}

void AbstractSceneConverterTest::addImageLevels2DInconsistentFormat() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D|
                   SceneConverterFeature::ImageLevels;
        }

        bool doBegin() override { return true; }
    } converter;

    const char imageData[4*4]{};

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.add({
        ImageData2D{PixelFormat::RGBA8Unorm, {2, 2}, DataFlags{}, imageData},
        ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, DataFlags{}, imageData},
        ImageData2D{PixelFormat::RGBA8Srgb, {4, 1}, DataFlags{}, imageData}
    });
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): image levels don't have the same format, expected PixelFormat::RGBA8Unorm but got PixelFormat::RGBA8Srgb for level 2\n");
}

void AbstractSceneConverterTest::addImageLevels2DInconsistentFormatExtra() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D|
                   SceneConverterFeature::ImageLevels;
        }

        bool doBegin() override { return true; }
    } converter;

    const char imageData[4*4]{};

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.add({
        ImageData2D{PixelStorage{}, 252, 1037, 4, {2, 2}, DataFlags{}, imageData},
        ImageData2D{PixelStorage{}, 252, 1037, 4, {1, 1}, DataFlags{}, imageData},
        ImageData2D{PixelStorage{}, 252, 4467, 4, {4, 1}, DataFlags{}, imageData}
    });
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): image levels don't have the same extra format field, expected 1037 but got 4467 for level 2\n");
}

void AbstractSceneConverterTest::addImageLevels2DInconsistentCompressedFormat() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D|
                   SceneConverterFeature::ImageLevels;
        }

        bool doBegin() override { return true; }
    } converter;

    const char imageData[4*4]{};

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.add({
        ImageData2D{CompressedPixelFormat::Bc1RGBAUnorm, {2, 2}, DataFlags{}, imageData},
        ImageData2D{CompressedPixelFormat::Bc1RGBAUnorm, {1, 1}, DataFlags{}, imageData},
        ImageData2D{CompressedPixelFormat::Bc1RGBASrgb, {4, 1}, DataFlags{}, imageData}
    });
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): image levels don't have the same format, expected CompressedPixelFormat::Bc1RGBAUnorm but got CompressedPixelFormat::Bc1RGBASrgb for level 2\n");
}

void AbstractSceneConverterTest::addImageLevels2DInconsistentFlags() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D|
                   SceneConverterFeature::ImageLevels;
        }

        bool doBegin() override { return true; }
    } converter;

    const char imageData[4*4]{};

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.add({
        ImageData2D{PixelFormat::RGBA8Unorm, {2, 2}, DataFlags{}, imageData, ImageFlag2D::Array},
        ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, DataFlags{}, imageData, ImageFlag2D::Array},
        ImageData2D{PixelFormat::RGBA8Unorm, {4, 1}, DataFlags{}, imageData}
    });
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): image levels don't have the same flags, expected ImageFlag2D::Array but got ImageFlags2D{} for level 2\n");
}

void AbstractSceneConverterTest::addImageLevels2DNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages2D|
                   SceneConverterFeature::ImageLevels;
        }

        bool doBegin() override { return true; }
    } converter;

    const char imageData[4]{};

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.add({
        ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, DataFlags{}, imageData},
        ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, DataFlags{}, imageData}
    });
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): multi-level 2D image conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::addImageLevels3D() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages3D|
                   SceneConverterFeature::ImageLevels;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt id, const Containers::Iterable<const ImageData3D>& imageLevels, Containers::StringView name) override {
            /* Camera count should not be increased before the function
               returns */
            CORRADE_COMPARE(id, image3DCount());

            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(imageLevels.size(), 3);
            CORRADE_COMPARE(imageLevels[1].importerState(), reinterpret_cast<const void*>(std::size_t{0xdeadbeef}));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[4*4]{};

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.image3DCount(), 0);
    CORRADE_COMPARE(converter.add({
        /* Arbitrary dimensions should be fine */
        ImageData3D{PixelFormat::RGBA8Unorm, {4, 1, 1}, DataFlags{}, imageData},
        ImageData3D{PixelFormat::RGBA8Unorm, {2, 2, 1}, DataFlags{}, imageData, ImageFlags3D{}, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})},
        ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 3}, DataFlags{}, imageData}
    }, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image3DCount(), 1);
    CORRADE_COMPARE(converter.add({
        ImageData3D{PixelFormat::RGBA8Unorm, {2, 1, 2}, DataFlags{}, imageData},
        ImageData3D{PixelFormat::RGBA8Unorm, {1, 3, 1}, DataFlags{}, imageData, ImageFlags3D{}, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})},
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

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const Containers::Iterable<const ImageData3D>& imageLevels, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(imageLevels.size(), 2);
            CORRADE_VERIFY(!imageLevels[1].isCompressed());
            CORRADE_COMPARE(imageLevels[1].storage().alignment(), 2);
            CORRADE_COMPARE(imageLevels[1].format(), PixelFormat::RG8Snorm);
            CORRADE_COMPARE(imageLevels[1].size(), (Vector3i{1, 3, 1}));
            CORRADE_COMPARE(imageLevels[1].flags(), ImageFlag3D::Array);
            CORRADE_VERIFY(imageLevels[1].data());

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[6]{};

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.image3DCount(), 0);
    CORRADE_COMPARE(converter.add({
        ImageView3D{PixelFormat::RG8Snorm, {1, 1, 1}, imageData, ImageFlag3D::Array},
        ImageView3D{PixelStorage{}.setAlignment(2), PixelFormat::RG8Snorm, {1, 3, 1}, imageData, ImageFlag3D::Array}
    }, "hello"), 0);
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

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const Containers::Iterable<const ImageData3D>& imageLevels, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(imageLevels.size(), 2);
            CORRADE_VERIFY(imageLevels[1].isCompressed());
            CORRADE_COMPARE(imageLevels[1].compressedStorage().rowLength(), 6);
            CORRADE_COMPARE(imageLevels[1].compressedFormat(), CompressedPixelFormat::Astc3x3x3RGBASrgb);
            CORRADE_COMPARE(imageLevels[1].size(), (Vector3i{3, 1, 1}));
            CORRADE_COMPARE(imageLevels[1].flags(), ImageFlag3D::Array);
            CORRADE_VERIFY(imageLevels[1].data());

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[2*16]{};

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.image3DCount(), 0);
    CORRADE_COMPARE(converter.add({
        CompressedImageView3D{CompressedPixelFormat::Astc3x3x3RGBASrgb, {1, 1, 1}, imageData, ImageFlag3D::Array},
        CompressedImageView3D{CompressedPixelStorage{}.setRowLength(6), CompressedPixelFormat::Astc3x3x3RGBASrgb, {3, 1, 1}, imageData, ImageFlag3D::Array}
    }, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image3DCount(), 1);
}

void AbstractSceneConverterTest::addImageLevels3DFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages3D|
                   SceneConverterFeature::AddCompressedImages3D|
                   SceneConverterFeature::ImageLevels;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const Containers::Iterable<const ImageData3D>&, Containers::StringView) override {
            return false;
        }
    } converter;

    const char imageData[16]{};

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.image3DCount(), 0);

    /* The implementation is expected to print an error message on its own */
    {
        Containers::String out;
        Error redirectError{&out};
        /* Testing all three variants to "fake" coverage for the name-less
           overloads as well */
        CORRADE_VERIFY(!converter.add({
            ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, DataFlags{}, imageData},
            ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, DataFlags{}, imageData}
        }));
        CORRADE_VERIFY(!converter.add({
            ImageView3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, imageData},
            ImageView3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, imageData}
        }));
        CORRADE_VERIFY(!converter.add({
            CompressedImageView3D{CompressedPixelFormat::Astc4x4RGBAUnorm, {1, 1, 1}, imageData},
            CompressedImageView3D{CompressedPixelFormat::Astc4x4RGBAUnorm, {1, 1, 1}, imageData}
        }));
        CORRADE_COMPARE(out, "");
    }

    /* It shouldn't abort the whole process */
    CORRADE_VERIFY(converter.isConverting());
    CORRADE_COMPARE(converter.image3DCount(), 0);
}

void AbstractSceneConverterTest::addImageLevels3DInvalidImage() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages3D|
                   SceneConverterFeature::ImageLevels;
        }

        bool doBegin() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.add(std::initializer_list<Containers::AnyReference<const ImageData3D>>{});
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): at least one image level has to be specified\n");
}

void AbstractSceneConverterTest::addImageLevels3DNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages3D|
                   SceneConverterFeature::ImageLevels;
        }

        bool doBegin() override { return true; }
    } converter;

    const char imageData[4]{};

    CORRADE_VERIFY(converter.begin());

    Containers::String out;
    Error redirectError{&out};
    converter.add({
        ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, DataFlags{}, imageData},
        ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, DataFlags{}, imageData}
    });
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::add(): multi-level 3D image conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::addImage1DThroughLevels() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultiple|
                   SceneConverterFeature::AddImages1D|
                   SceneConverterFeature::ImageLevels;
        }

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const Containers::Iterable<const ImageData1D>& imageLevels, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(imageLevels.size(), 1);
            CORRADE_COMPARE(imageLevels[0].importerState(), reinterpret_cast<const void*>(std::size_t{0xdeadbeef}));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[4]{};

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.add(ImageData1D{PixelFormat::RGBA8Unorm, 1, {}, imageData, ImageFlags1D{}, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})}, "hello"), 0);
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

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const Containers::Iterable<const ImageData2D>& imageLevels, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(imageLevels.size(), 1);
            CORRADE_COMPARE(imageLevels[0].importerState(), reinterpret_cast<const void*>(std::size_t{0xdeadbeef}));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[4]{};

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.add(ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, {}, imageData, ImageFlags2D{}, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})}, "hello"), 0);
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

        bool doBegin() override { return true; }

        bool doAdd(UnsignedInt, const Containers::Iterable<const ImageData3D>& imageLevels, Containers::StringView name) override {
            CORRADE_COMPARE(name, "hello");
            CORRADE_COMPARE(imageLevels.size(), 1);
            CORRADE_COMPARE(imageLevels[0].importerState(), reinterpret_cast<const void*>(std::size_t{0xdeadbeef}));

            addCalled = true;
            return true;
        }

        bool addCalled = false;
    } converter;

    const char imageData[4]{};

    CORRADE_VERIFY(converter.begin());
    CORRADE_COMPARE(converter.add(ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, {}, imageData, ImageFlags3D{}, reinterpret_cast<const void*>(std::size_t{0xdeadbeef})}, "hello"), 0);
    CORRADE_VERIFY(converter.addCalled);
    CORRADE_COMPARE(converter.image3DCount(), 1);
}

void AbstractSceneConverterTest::addImporterContents() {
    auto&& data = AddImporterContentsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct Importer: AbstractImporter {
        explicit Importer(SceneContents contents): contents{contents} {}

        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedLong doObjectCount() const override {
            /* Counts are queried unconditionally in an assert, so can't
               really check anything here. Returning 0 would be
               counterproductive, the addImporterContentsImportFail() case
               tests that the right *Count() gets called. */
            return 3;
        }
        Containers::String doObjectName(UnsignedLong id) override {
            CORRADE_VERIFY(contents & SceneContent::Names);
            return Utility::format("Object {}", id);
        }

        UnsignedInt doSceneCount() const override {
            /* Counts are queried unconditionally in an assert, so can't
               really check anything here. Returning 0 would be
               counterproductive, the addImporterContentsImportFail() case
               tests that the right *Count() gets called. */
            return 2;
        }
        Int doDefaultScene() const override {
            CORRADE_VERIFY(contents & SceneContent::Scenes);
            return 1;
        }
        Containers::String doSceneName(UnsignedInt id) override {
            CORRADE_VERIFY(contents & SceneContent::Names);
            return Utility::format("Scene {}", id);
        }
        Containers::Optional<SceneData> doScene(UnsignedInt id) override {
            return SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {}, reinterpret_cast<const void*>(std::size_t{0x5ce00000 + id})};
        }

        UnsignedInt doAnimationCount() const override {
            /* Counts are queried unconditionally in an assert, so can't
               really check anything here. Returning 0 would be
               counterproductive, the addImporterContentsImportFail() case
               tests that the right *Count() gets called. */
            return 2;
        }
        Containers::String doAnimationName(UnsignedInt id) override {
            CORRADE_VERIFY(contents & SceneContent::Names);
            return Utility::format("Animation {}", id);
        }
        Containers::Optional<AnimationData> doAnimation(UnsignedInt id) override {
            return AnimationData{nullptr, {}, reinterpret_cast<const void*>(std::size_t{0x40100000 + id})};
        }

        UnsignedInt doLightCount() const override {
            /* Counts are queried unconditionally in an assert, so can't
               really check anything here. Returning 0 would be
               counterproductive, the addImporterContentsImportFail() case
               tests that the right *Count() gets called. */
            return 2;
        }
        Containers::String doLightName(UnsignedInt id) override {
            CORRADE_VERIFY(contents & SceneContent::Names);
            return Utility::format("Light {}", id);
        }
        Containers::Optional<LightData> doLight(UnsignedInt id) override {
            return LightData{LightType::Point, {}, {}, reinterpret_cast<const void*>(std::size_t{0x11600000 + id})};
        }

        UnsignedInt doCameraCount() const override {
            /* Counts are queried unconditionally in an assert, so can't
               really check anything here. Returning 0 would be
               counterproductive, the addImporterContentsImportFail() case
               tests that the right *Count() gets called. */
            return 2;
        }
        Containers::String doCameraName(UnsignedInt id) override {
            CORRADE_VERIFY(contents & SceneContent::Names);
            return Utility::format("Camera {}", id);
        }
        Containers::Optional<CameraData> doCamera(UnsignedInt id) override {
            return CameraData{CameraType::Orthographic2D, {}, 0.0f, 0.0f, reinterpret_cast<const void*>(std::size_t{0xca0e0000 + id})};
        }

        UnsignedInt doSkin2DCount() const override {
            /* Counts are queried unconditionally in an assert, so can't
               really check anything here. Returning 0 would be
               counterproductive, the addImporterContentsImportFail() case
               tests that the right *Count() gets called. */
            return 2;
        }
        Containers::String doSkin2DName(UnsignedInt id) override {
            CORRADE_VERIFY(contents & SceneContent::Names);
            return Utility::format("2D skin {}", id);
        }
        Containers::Optional<SkinData2D> doSkin2D(UnsignedInt id) override {
            return SkinData2D{{}, {}, reinterpret_cast<const void*>(std::size_t{0x50102d00 + id})};
        }

        UnsignedInt doSkin3DCount() const override {
            /* Counts are queried unconditionally in an assert, so can't
               really check anything here. Returning 0 would be
               counterproductive, the addImporterContentsImportFail() case
               tests that the right *Count() gets called. */
            return 2;
        }
        Containers::String doSkin3DName(UnsignedInt id) override {
            CORRADE_VERIFY(contents & SceneContent::Names);
            return Utility::format("3D skin {}", id);
        }
        Containers::Optional<SkinData3D> doSkin3D(UnsignedInt id) override {
            return SkinData3D{{}, {}, reinterpret_cast<const void*>(std::size_t{0x50103d00 + id})};
        }

        UnsignedInt doMeshCount() const override {
            /* Counts are queried unconditionally in an assert, so can't
               really check anything here. Returning 0 would be
               counterproductive, the addImporterContentsImportFail() case
               tests that the right *Count() gets called. */
            return 2;
        }
        UnsignedInt doMeshLevelCount(UnsignedInt id) override {
            CORRADE_VERIFY(contents & SceneContent::MeshLevels);
            return id == 1 ? 3 : 1;
        }
        Containers::String doMeshName(UnsignedInt id) override {
            CORRADE_VERIFY(contents & SceneContent::Names);
            return Utility::format("Mesh {}", id);
        }
        Containers::Optional<MeshData> doMesh(UnsignedInt id, UnsignedInt level) override {
            return MeshData{{}, 0, reinterpret_cast<const void*>(std::size_t{0x0e500000 + id + level*16})};
        }

        UnsignedInt doMaterialCount() const override {
            /* Counts are queried unconditionally in an assert, so can't
               really check anything here. Returning 0 would be
               counterproductive, the addImporterContentsImportFail() case
               tests that the right *Count() gets called. */
            return 2;
        }
        Containers::String doMaterialName(UnsignedInt id) override {
            CORRADE_VERIFY(contents & SceneContent::Names);
            return Utility::format("Material {}", id);
        }
        Containers::Optional<MaterialData> doMaterial(UnsignedInt id) override {
            return MaterialData{{}, {}, reinterpret_cast<const void*>(std::size_t{0x0a7e0000 + id})};
        }

        UnsignedInt doTextureCount() const override {
            /* Counts are queried unconditionally in an assert, so can't
               really check anything here. Returning 0 would be
               counterproductive, the addImporterContentsImportFail() case
               tests that the right *Count() gets called. */
            return 2;
        }
        Containers::String doTextureName(UnsignedInt id) override {
            CORRADE_VERIFY(contents & SceneContent::Names);
            return Utility::format("Texture {}", id);
        }
        Containers::Optional<TextureData> doTexture(UnsignedInt id) override {
            return TextureData{TextureType::Texture1D, SamplerFilter::Nearest, SamplerFilter::Nearest, SamplerMipmap::Nearest, SamplerWrapping::ClampToEdge, 0, reinterpret_cast<const void*>(std::size_t{0x7e070000 + id})};
        }

        UnsignedInt doImage1DCount() const override {
            /* Counts are queried unconditionally in an assert, so can't
               really check anything here. Returning 0 would be
               counterproductive, the addImporterContentsImportFail() case
               tests that the right *Count() gets called. */
            return 2;
        }
        UnsignedInt doImage1DLevelCount(UnsignedInt id) override {
            CORRADE_VERIFY(contents & SceneContent::ImageLevels);
            return id == 1 ? 4 : 1;
        }
        Containers::String doImage1DName(UnsignedInt id) override {
            CORRADE_VERIFY(contents & SceneContent::Names);
            return Utility::format("1D image {}", id);
        }
        Containers::Optional<ImageData1D> doImage1D(UnsignedInt id, UnsignedInt level) override {
            return ImageData1D{PixelFormat::RGBA8Unorm, 1, {}, "yes", {}, reinterpret_cast<const void*>(std::size_t{0x10a91d00 + id + level*16})};
        }

        UnsignedInt doImage2DCount() const override {
            /* Counts are queried unconditionally in an assert, so can't
               really check anything here. Returning 0 would be
               counterproductive, the addImporterContentsImportFail() case
               tests that the right *Count() gets called. */
            return 2;
        }
        UnsignedInt doImage2DLevelCount(UnsignedInt id) override {
            CORRADE_VERIFY(contents & SceneContent::ImageLevels);
            return id == 0 ? 3 : 1;
        }
        Containers::String doImage2DName(UnsignedInt id) override {
            CORRADE_VERIFY(contents & SceneContent::Names);
            return Utility::format("2D image {}", id);
        }
        Containers::Optional<ImageData2D> doImage2D(UnsignedInt id, UnsignedInt level) override {
            return ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, {}, "yes", {}, reinterpret_cast<const void*>(std::size_t{0x10a92d00 + id + level*16})};
        }

        UnsignedInt doImage3DCount() const override {
            /* Counts are queried unconditionally in an assert, so can't
               really check anything here. Returning 0 would be
               counterproductive, the addImporterContentsImportFail() case
               tests that the right *Count() gets called. */
            return 2;
        }
        UnsignedInt doImage3DLevelCount(UnsignedInt id) override {
            CORRADE_VERIFY(contents & SceneContent::ImageLevels);
            return id == 1 ? 2 : 1;
        }
        Containers::String doImage3DName(UnsignedInt id) override {
            CORRADE_VERIFY(contents & SceneContent::Names);
            return Utility::format("3D image {}", id);
        }
        Containers::Optional<ImageData3D> doImage3D(UnsignedInt id, UnsignedInt level) override {
            return ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, {}, "yes", {}, reinterpret_cast<const void*>(std::size_t{0x10a93d00 + id + level*16})};
        }

        SceneContents contents;
    } importer{data.contents};

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return
                SceneConverterFeature::ConvertMultipleToData|
                SceneConverterFeature::AddScenes|
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
                SceneConverterFeature::MeshLevels|
                SceneConverterFeature::ImageLevels;
        }

        bool doBeginData() override { return true; }

        void doSetObjectName(UnsignedLong id, Containers::StringView name) override {
            Debug{} << "Setting object" << id << "name to" << name;
        }
        bool doAdd(UnsignedInt id, const SceneData& data, Containers::StringView name) override {
            Debug{} << "Adding scene" << id << "named" << name << "with" << data.importerState();
            return true;
        }
        void doSetDefaultScene(UnsignedInt id) override {
            Debug{} << "Setting default scene to" << id;
        }

        bool doAdd(UnsignedInt id, const AnimationData& data, Containers::StringView name) override {
            Debug{} << "Adding animation" << id << "named" << name << "with" << data.importerState();
            return true;
        }
        bool doAdd(UnsignedInt id, const LightData& data, Containers::StringView name) override {
            Debug{} << "Adding light" << id << "named" << name << "with" << data.importerState();
            return true;
        }
        bool doAdd(UnsignedInt id, const CameraData& data, Containers::StringView name) override {
            Debug{} << "Adding camera" << id << "named" << name << "with" << data.importerState();
            return true;
        }
        bool doAdd(UnsignedInt id, const SkinData2D& data, Containers::StringView name) override {
            Debug{} << "Adding 2D skin" << id << "named" << name << "with" << data.importerState();
            return true;
        }
        bool doAdd(UnsignedInt id, const SkinData3D& data, Containers::StringView name) override {
            Debug{} << "Adding 3D skin" << id << "named" << name << "with" << data.importerState();
            return true;
        }

        bool doAdd(UnsignedInt id, const MeshData& data, Containers::StringView name) override {
            Debug{} << "Adding mesh" << id << "named" << name << "with" << data.importerState();
            return true;
        }
        bool doAdd(UnsignedInt id, const Containers::Iterable<const MeshData>& levels, Containers::StringView name) override {
            for(std::size_t i = 0; i != levels.size(); ++i)
                Debug{} << "Adding mesh" << id << "level" << i << "named" << name << "with" << levels[i].importerState();
            return true;
        }

        bool doAdd(UnsignedInt id, const MaterialData& data, Containers::StringView name) override {
            Debug{} << "Adding material" << id << "named" << name << "with" << data.importerState();
            return true;
        }
        bool doAdd(UnsignedInt id, const TextureData& data, Containers::StringView name) override {
            Debug{} << "Adding texture" << id << "named" << name << "with" << data.importerState();
            return true;
        }

        bool doAdd(UnsignedInt id, const ImageData1D& data, Containers::StringView name) override {
            Debug{} << "Adding 1D image" << id << "named" << name << "with" << data.importerState();
            return true;
        }
        bool doAdd(UnsignedInt id, const Containers::Iterable<const ImageData1D>& levels, Containers::StringView name) override {
            for(std::size_t i = 0; i != levels.size(); ++i)
                Debug{} << "Adding 1D image" << id << "level" << i << "named" << name << "with" << levels[i].importerState();
            return true;
        }

        bool doAdd(UnsignedInt id, const ImageData2D& data, Containers::StringView name) override {
            Debug{} << "Adding 2D image" << id << "named" << name << "with" << data.importerState();
            return true;
        }
        bool doAdd(UnsignedInt id, const Containers::Iterable<const ImageData2D>& levels, Containers::StringView name) override {
            for(std::size_t i = 0; i != levels.size(); ++i)
                Debug{} << "Adding 2D image" << id << "level" << i << "named" << name << "with" << levels[i].importerState();
            return true;
        }

        bool doAdd(UnsignedInt id, const ImageData3D& data, Containers::StringView name) override {
            Debug{} << "Adding 3D image" << id << "named" << name << "with" << data.importerState();
            return true;
        }
        bool doAdd(UnsignedInt id, const Containers::Iterable<const ImageData3D>& levels, Containers::StringView name) override {
            for(std::size_t i = 0; i != levels.size(); ++i)
                Debug{} << "Adding 3D image" << id << "level" << i << "named" << name << "with" << levels[i].importerState();
            return true;
        }
    } converter;

    converter.addFlags(data.flags);
    CORRADE_VERIFY(converter.beginData());

    Containers::String out;
    Debug redirectOutput{&out};
    CORRADE_VERIFY(converter.addImporterContents(importer, data.contents));
    CORRADE_COMPARE(out, data.expected);
}

void AbstractSceneConverterTest::addImporterContentsCustomSceneFields() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override {
            return 3;
        }
        Containers::Optional<SceneData> doScene(UnsignedInt id) override {
            if(id == 1 || id == 2)
                return SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {
                    SceneFieldData{SceneField::Translation, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Vector3, nullptr},
                    SceneFieldData{sceneFieldCustom(34977), SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Vector2b, nullptr},
                    SceneFieldData{SceneField::Scaling, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Vector3, nullptr},
                    SceneFieldData{sceneFieldCustom(5266), SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Pointer, nullptr},
                }};
            return SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {}};
        }
        Containers::String doSceneFieldName(SceneField name) override {
            if(name == sceneFieldCustom(34977)) return "offsetSmall";
            if(name == sceneFieldCustom(5266)) return "valueData";
            CORRADE_FAIL("This should not be reached");
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }
    } importer;

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return
                SceneConverterFeature::ConvertMultipleToData|
                SceneConverterFeature::AddScenes;
        }

        bool doBeginData() override { return true; }

        bool doAdd(UnsignedInt, const SceneData&, Containers::StringView) override {
            Debug{} << "Adding scene";
            return true;
        }
        void doSetSceneFieldName(SceneField field, Containers::StringView name) override {
            Debug{} << "Setting field" << sceneFieldCustom(field) << "name to" << name;
        }
    } converter;

    CORRADE_VERIFY(converter.beginData());

    Containers::String out;
    Debug redirectOutput{&out};
    CORRADE_VERIFY(converter.addImporterContents(importer));
    /* No error message, the importer is expected to print that on its own */
    CORRADE_COMPARE(out,
        "Adding scene\n"
        /** @todo cache the names to avoid querying repeatedly */
        "Setting field 34977 name to offsetSmall\n"
        "Setting field 5266 name to valueData\n"
        "Adding scene\n"
        "Setting field 34977 name to offsetSmall\n"
        "Setting field 5266 name to valueData\n"
        "Adding scene\n");
}

void AbstractSceneConverterTest::addImporterContentsCustomMeshAttributes() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMeshCount() const override {
            return 2;
        }
        UnsignedInt doMeshLevelCount(UnsignedInt) override {
            return 5;
        }
        Containers::Optional<MeshData> doMesh(UnsignedInt id, UnsignedInt level) override {
            if(id == 1 && (level == 2 || level == 3))
                return MeshData{MeshPrimitive::Triangles, nullptr, {
                    MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector3, nullptr},
                    MeshAttributeData{meshAttributeCustom(31977), VertexFormat::Vector2b, nullptr},
                    MeshAttributeData{MeshAttribute::Normal, VertexFormat::Vector3, nullptr},
                    MeshAttributeData{meshAttributeCustom(5266), VertexFormat::ByteNormalized, nullptr},
                }};
            return MeshData{MeshPrimitive::Points, 0};
        }
        Containers::String doMeshAttributeName(MeshAttribute name) override {
            if(name == meshAttributeCustom(31977)) return "offsetSmall";
            if(name == meshAttributeCustom(5266)) return "valueData";
            CORRADE_FAIL("This should not be reached");
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }
    } importer;

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return
                SceneConverterFeature::ConvertMultipleToData|
                SceneConverterFeature::AddMeshes|
                SceneConverterFeature::MeshLevels;
        }

        bool doBeginData() override { return true; }

        bool doAdd(UnsignedInt, const Containers::Iterable<const MeshData>&, Containers::StringView) override {
            Debug{} << "Adding mesh levels";
            return true;
        }
        void doSetMeshAttributeName(MeshAttribute attribute, Containers::StringView name) override {
            Debug{} << "Setting attribute" << meshAttributeCustom(attribute) << "name to" << name;
        }
    } converter;

    CORRADE_VERIFY(converter.beginData());

    Containers::String out;
    Debug redirectOutput{&out};
    CORRADE_VERIFY(converter.addImporterContents(importer));
    /* No error message, the importer is expected to print that on its own */
    CORRADE_COMPARE(out,
        "Adding mesh levels\n"
        /** @todo cache the names to avoid querying repeatedly */
        "Setting attribute 31977 name to offsetSmall\n"
        "Setting attribute 5266 name to valueData\n"
        "Setting attribute 31977 name to offsetSmall\n"
        "Setting attribute 5266 name to valueData\n"
        "Adding mesh levels\n");
}

void AbstractSceneConverterTest::addImporterContentsImportFail() {
    auto&& data = AddImporterContentsFailData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct Importer: AbstractImporter {
        explicit Importer(SceneContents contents): contents{contents} {}

        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override {
            return contents & SceneContent::Scenes ? 4 : 0;
        }
        Containers::Optional<SceneData> doScene(UnsignedInt id) override {
            if(id == 2) return {};
            return SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {}};
        }

        UnsignedInt doAnimationCount() const override {
            return contents & SceneContent::Animations ? 4 : 0;
        }
        Containers::Optional<AnimationData> doAnimation(UnsignedInt id) override {
            if(id == 2) return {};
            return AnimationData{nullptr, {}};
        }

        UnsignedInt doLightCount() const override {
            return contents & SceneContent::Lights ? 4 : 0;
        }
        Containers::Optional<LightData> doLight(UnsignedInt id) override {
            if(id == 2) return {};
            return LightData{LightType::Point, {}, {}};
        }

        UnsignedInt doCameraCount() const override {
            return contents & SceneContent::Cameras ? 4 : 0;
        }
        Containers::Optional<CameraData> doCamera(UnsignedInt id) override {
            if(id == 2) return {};
            return CameraData{CameraType::Orthographic2D, {}, 0.0f, 0.0f};
        }

        UnsignedInt doSkin2DCount() const override {
            return contents & SceneContent::Skins2D ? 4 : 0;
        }
        Containers::Optional<SkinData2D> doSkin2D(UnsignedInt id) override {
            if(id == 2) return {};
            return SkinData2D{{}, {}};
        }

        UnsignedInt doSkin3DCount() const override {
            return contents & SceneContent::Skins3D ? 4 : 0;
        }
        Containers::Optional<SkinData3D> doSkin3D(UnsignedInt id) override {
            if(id == 2) return {};
            return SkinData3D{{}, {}};
        }

        UnsignedInt doMeshCount() const override {
            return contents & SceneContent::Meshes ? 4 : 0;
        }
        UnsignedInt doMeshLevelCount(UnsignedInt) override {
            return contents & SceneContent::MeshLevels ? 5 : 1;
        }
        Containers::Optional<MeshData> doMesh(UnsignedInt id, UnsignedInt level) override {
            if(id == 2) {
                if(contents & SceneContent::MeshLevels) {
                    if(level == 3) return {};
                } else return {};
            }

            return MeshData{{}, 0};
        }

        UnsignedInt doMaterialCount() const override {
            return contents & SceneContent::Materials ? 4 : 0;
        }
        Containers::Optional<MaterialData> doMaterial(UnsignedInt id) override {
            if(id == 2) return {};
            return MaterialData{{}, {}};
        }

        UnsignedInt doTextureCount() const override {
            return contents & SceneContent::Textures ? 4 : 0;
        }
        Containers::Optional<TextureData> doTexture(UnsignedInt id) override {
            if(id == 2) return {};
            return TextureData{TextureType::Texture1D, SamplerFilter::Nearest, SamplerFilter::Nearest, SamplerMipmap::Nearest, SamplerWrapping::ClampToEdge, 0};
        }

        UnsignedInt doImage1DCount() const override {
            return contents & SceneContent::Images1D ? 4 : 0;
        }
        UnsignedInt doImage1DLevelCount(UnsignedInt) override {
            return contents & SceneContent::ImageLevels ? 5 : 1;
        }
        Containers::Optional<ImageData1D> doImage1D(UnsignedInt id, UnsignedInt level) override {
            if(id == 2) {
                if(contents & SceneContent::ImageLevels) {
                    if(level == 3) return {};
                } else return {};
            }

            return ImageData1D{PixelFormat::RGBA8Unorm, 1, DataFlags{}, "yes"};
        }

        UnsignedInt doImage2DCount() const override {
            return contents & SceneContent::Images2D ? 4 : 0;
        }
        UnsignedInt doImage2DLevelCount(UnsignedInt) override {
            return contents & SceneContent::ImageLevels ? 5 : 1;
        }
        Containers::Optional<ImageData2D> doImage2D(UnsignedInt id, UnsignedInt level) override {
            if(id == 2) {
                if(contents & SceneContent::ImageLevels) {
                    if(level == 3) return {};
                } else return {};
            }

            return ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, DataFlags{}, "yes"};
        }

        UnsignedInt doImage3DCount() const override {
            return contents & SceneContent::Images3D ? 4 : 0;
        }
        UnsignedInt doImage3DLevelCount(UnsignedInt) override {
            return contents & SceneContent::ImageLevels ? 5 : 1;
        }
        Containers::Optional<ImageData3D> doImage3D(UnsignedInt id, UnsignedInt level) override {
            if(id == 2) {
                if(contents & SceneContent::ImageLevels) {
                    if(level == 3) return {};
                } else return {};
            }

            return ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, DataFlags{}, "yes"};
        }

        SceneContents contents;
    } importer{data.contents};

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return
                SceneConverterFeature::ConvertMultipleToData|
                SceneConverterFeature::AddScenes|
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
                SceneConverterFeature::MeshLevels|
                SceneConverterFeature::ImageLevels;
        }

        bool doBeginData() override { return true; }

        bool doAdd(UnsignedInt, const SceneData&, Containers::StringView) override {
            Debug{} << "Adding scene";
            return true;
        }

        bool doAdd(UnsignedInt, const AnimationData&, Containers::StringView) override {
            Debug{} << "Adding animation";
            return true;
        }
        bool doAdd(UnsignedInt, const LightData&, Containers::StringView) override {
            Debug{} << "Adding light";
            return true;
        }
        bool doAdd(UnsignedInt, const CameraData&, Containers::StringView) override {
            Debug{} << "Adding camera";
            return true;
        }
        bool doAdd(UnsignedInt, const SkinData2D&, Containers::StringView) override {
            Debug{} << "Adding 2D skin";
            return true;
        }
        bool doAdd(UnsignedInt, const SkinData3D&, Containers::StringView) override {
            Debug{} << "Adding 3D skin";
            return true;
        }

        bool doAdd(UnsignedInt, const MeshData&, Containers::StringView) override {
            Debug{} << "Adding mesh";
            return true;
        }
        bool doAdd(UnsignedInt, const Containers::Iterable<const MeshData>&, Containers::StringView) override {
            Debug{} << "Adding mesh levels";
            return true;
        }

        bool doAdd(UnsignedInt, const MaterialData&, Containers::StringView) override {
            Debug{} << "Adding material";
            return true;
        }
        bool doAdd(UnsignedInt, const TextureData&, Containers::StringView) override {
            Debug{} << "Adding texture";
            return true;
        }

        bool doAdd(UnsignedInt, const ImageData1D&, Containers::StringView) override {
            Debug{} << "Adding 1D image";
            return true;
        }
        bool doAdd(UnsignedInt, const Containers::Iterable<const ImageData1D>&, Containers::StringView) override {
            Debug{} << "Adding 1D image levels";
            return true;
        }

        bool doAdd(UnsignedInt, const ImageData2D&, Containers::StringView) override {
            Debug{} << "Adding 2D image";
            return true;
        }
        bool doAdd(UnsignedInt, const Containers::Iterable<const ImageData2D>&, Containers::StringView) override {
            Debug{} << "Adding 2D image levels";
            return true;
        }
        bool doAdd(UnsignedInt, const ImageData3D&, Containers::StringView) override {
            Debug{} << "Adding 3D image";
            return true;
        }
        bool doAdd(UnsignedInt, const Containers::Iterable<const ImageData3D>&, Containers::StringView) override {
            Debug{} << "Adding 3D image levels";
            return true;
        }
    } converter;

    CORRADE_VERIFY(converter.beginData());

    Containers::String out;
    Debug redirectOutput{&out};
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.addImporterContents(importer, data.contents));
    /* No error message, the importer is expected to print that on its own */
    CORRADE_COMPARE(out, Utility::format(
        "Adding {0}\n"
        "Adding {0}\n", data.name));
}

void AbstractSceneConverterTest::addImporterContentsConversionFail() {
    auto&& data = AddImporterContentsFailData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override {
            return 4;
        }
        Containers::Optional<SceneData> doScene(UnsignedInt) override {
            return SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {}};
        }

        UnsignedInt doAnimationCount() const override {
            return 4;
        }
        Containers::Optional<AnimationData> doAnimation(UnsignedInt) override {
            return AnimationData{nullptr, {}};
        }

        UnsignedInt doLightCount() const override {
            return 4;
        }
        Containers::Optional<LightData> doLight(UnsignedInt) override {
            return LightData{LightType::Point, {}, {}};
        }

        UnsignedInt doCameraCount() const override {
            return 4;
        }
        Containers::Optional<CameraData> doCamera(UnsignedInt) override {
            return CameraData{CameraType::Orthographic2D, {}, 0.0f, 0.0f};
        }

        UnsignedInt doSkin2DCount() const override {
            return 4;
        }
        Containers::Optional<SkinData2D> doSkin2D(UnsignedInt) override {
            return SkinData2D{{}, {}};
        }

        UnsignedInt doSkin3DCount() const override {
            return 4;
        }
        Containers::Optional<SkinData3D> doSkin3D(UnsignedInt) override {
            return SkinData3D{{}, {}};
        }

        UnsignedInt doMeshCount() const override {
            return 4;
        }
        UnsignedInt doMeshLevelCount(UnsignedInt) override {
            return 5;
        }
        Containers::Optional<MeshData> doMesh(UnsignedInt, UnsignedInt) override {
            return MeshData{{}, 0};
        }

        UnsignedInt doMaterialCount() const override {
            return 4;
        }
        Containers::Optional<MaterialData> doMaterial(UnsignedInt) override {
            return MaterialData{{}, {}};
        }

        UnsignedInt doTextureCount() const override {
            return 4;
        }
        Containers::Optional<TextureData> doTexture(UnsignedInt) override {
            return TextureData{TextureType::Texture1D, SamplerFilter::Nearest, SamplerFilter::Nearest, SamplerMipmap::Nearest, SamplerWrapping::ClampToEdge, 0};
        }

        UnsignedInt doImage1DCount() const override {
            return 4;
        }
        UnsignedInt doImage1DLevelCount(UnsignedInt) override {
            return 5;
        }
        Containers::Optional<ImageData1D> doImage1D(UnsignedInt, UnsignedInt) override {
            return ImageData1D{PixelFormat::RGBA8Unorm, 1, DataFlags{}, "yes"};
        }

        UnsignedInt doImage2DCount() const override {
            return 4;
        }
        UnsignedInt doImage2DLevelCount(UnsignedInt) override {
            return 5;
        }
        Containers::Optional<ImageData2D> doImage2D(UnsignedInt, UnsignedInt) override {
            return ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, DataFlags{}, "yes"};
        }

        UnsignedInt doImage3DCount() const override {
            return 4;
        }
        UnsignedInt doImage3DLevelCount(UnsignedInt) override {
            return 5;
        }
        Containers::Optional<ImageData3D> doImage3D(UnsignedInt, UnsignedInt) override {
            return ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, DataFlags{}, "yes"};
        }
    } importer;

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return
                SceneConverterFeature::ConvertMultipleToData|
                SceneConverterFeature::AddScenes|
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
                SceneConverterFeature::MeshLevels|
                SceneConverterFeature::ImageLevels;
        }

        bool doBeginData() override { return true; }

        bool doAdd(UnsignedInt id, const SceneData&, Containers::StringView) override {
            if(id == 2) return false;

            Debug{} << "Adding scene";
            return true;
        }

        bool doAdd(UnsignedInt id, const AnimationData&, Containers::StringView) override {
            if(id == 2) return false;

            Debug{} << "Adding animation";
            return true;
        }
        bool doAdd(UnsignedInt id, const LightData&, Containers::StringView) override {
            if(id == 2) return false;

            Debug{} << "Adding light";
            return true;
        }
        bool doAdd(UnsignedInt id, const CameraData&, Containers::StringView) override {
            if(id == 2) return false;

            Debug{} << "Adding camera";
            return true;
        }
        bool doAdd(UnsignedInt id, const SkinData2D&, Containers::StringView) override {
            if(id == 2) return false;

            Debug{} << "Adding 2D skin";
            return true;
        }
        bool doAdd(UnsignedInt id, const SkinData3D&, Containers::StringView) override {
            if(id == 2) return false;

            Debug{} << "Adding 3D skin";
            return true;
        }

        bool doAdd(UnsignedInt id, const MeshData&, Containers::StringView) override {
            if(id == 2) return false;

            Debug{} << "Adding mesh";
            return true;
        }
        bool doAdd(UnsignedInt id, const Containers::Iterable<const MeshData>&, Containers::StringView) override {
            if(id == 2) return false;

            Debug{} << "Adding mesh levels";
            return true;
        }

        bool doAdd(UnsignedInt id, const MaterialData&, Containers::StringView) override {
            if(id == 2) return false;

            Debug{} << "Adding material";
            return true;
        }
        bool doAdd(UnsignedInt id, const TextureData&, Containers::StringView) override {
            if(id == 2) return false;

            Debug{} << "Adding texture";
            return true;
        }

        bool doAdd(UnsignedInt id, const ImageData1D&, Containers::StringView) override {
            if(id == 2) return false;

            Debug{} << "Adding 1D image";
            return true;
        }
        bool doAdd(UnsignedInt id, const Containers::Iterable<const ImageData1D>&, Containers::StringView) override {
            if(id == 2) return false;

            Debug{} << "Adding 1D image levels";
            return true;
        }

        bool doAdd(UnsignedInt id, const ImageData2D&, Containers::StringView) override {
            if(id == 2) return false;

            Debug{} << "Adding 2D image";
            return true;
        }
        bool doAdd(UnsignedInt id, const Containers::Iterable<const ImageData2D>&, Containers::StringView) override {
            if(id == 2) return false;

            Debug{} << "Adding 2D image levels";
            return true;
        }
        bool doAdd(UnsignedInt id, const ImageData3D&, Containers::StringView) override {
            if(id == 2) return false;

            Debug{} << "Adding 3D image";
            return true;
        }
        bool doAdd(UnsignedInt id, const Containers::Iterable<const ImageData3D>&, Containers::StringView) override {
            if(id == 2) return false;

            Debug{} << "Adding 3D image levels";
            return true;
        }
    } converter;

    CORRADE_VERIFY(converter.beginData());

    Containers::String out;
    Debug redirectOutput{&out};
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.addImporterContents(importer, data.contents));
    /* No error message, the importer is expected to print that on its own */
    CORRADE_COMPARE(out, Utility::format(
        "Adding {0}\n"
        "Adding {0}\n", data.name));
}

void AbstractSceneConverterTest::addImporterContentsNotConverting() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    } importer;

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToData;
        }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.addImporterContents(importer, {}));
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::addImporterContents(): no conversion in progress\n");
}

void AbstractSceneConverterTest::addImporterContentsNotOpened() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToData;
        }
        bool doBeginData() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.beginData());

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.addImporterContents(importer));
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::addImporterContents(): the importer is not opened\n");
}

void AbstractSceneConverterTest::addImporterContentsNotSupported() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doCameraCount() const override { return 2; }
        UnsignedInt doLightCount() const override { return 4; }
        UnsignedInt doMeshCount() const override { return 3; }
        UnsignedInt doMaterialCount() const override { return 3; }
    } importer;

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToData|
                SceneConverterFeature::AddCameras|
                SceneConverterFeature::AddMaterials;
        }
        bool doBeginData() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.beginData());

    Containers::String out;
    Error redirectError{&out};
    /* Scenes (which are not present in the input) should not be part of the
       error, materials are in the input and supported, meshes and lights are
       in the input but not supported so these should be printed */
    CORRADE_VERIFY(!converter.addImporterContents(importer, SceneContent::Scenes|SceneContent::Cameras|SceneContent::Meshes|SceneContent::Lights));
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::addImporterContents(): unsupported contents Lights|Meshes\n");
}

void AbstractSceneConverterTest::addImporterContentsNotSupportedLevels() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMeshCount() const override { return 4; }
        UnsignedInt doMeshLevelCount(UnsignedInt id) override {
            return id == 2 ? 5 : 1;
        }
        Containers::Optional<MeshData> doMesh(UnsignedInt, UnsignedInt) override {
            return MeshData{{}, 0};
        }

        UnsignedInt doImage1DCount() const override { return 4; }
        UnsignedInt doImage1DLevelCount(UnsignedInt id) override {
            return id == 2 ? 2 : 1;
        }
        Containers::Optional<ImageData1D> doImage1D(UnsignedInt, UnsignedInt) override {
            return ImageData1D{PixelFormat::RGBA8Unorm, 1, DataFlags{}, "yes"};
        }

        UnsignedInt doImage2DCount() const override { return 4; }
        UnsignedInt doImage2DLevelCount(UnsignedInt id) override {
            return id == 2 ? 3 : 1;
        }
        Containers::Optional<ImageData2D> doImage2D(UnsignedInt, UnsignedInt) override {
            return ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, DataFlags{}, "yes"};
        }

        UnsignedInt doImage3DCount() const override { return 4; }
        UnsignedInt doImage3DLevelCount(UnsignedInt id) override {
            return id == 2 ? 4 : 1;
        }
        Containers::Optional<ImageData3D> doImage3D(UnsignedInt, UnsignedInt) override {
            return ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, DataFlags{}, "yes"};
        }
    } importer;

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToData|
                SceneConverterFeature::AddMeshes|
                SceneConverterFeature::AddImages1D|
                SceneConverterFeature::AddImages2D|
                SceneConverterFeature::AddImages3D;
        }
        bool doBeginData() override { return true; }

        bool doAdd(UnsignedInt, const MeshData&, Containers::StringView) override {
            Debug{} << "Adding mesh";
            return true;
        }
        bool doAdd(UnsignedInt, const ImageData1D&, Containers::StringView) override {
            Debug{} << "Adding 1D image";
            return true;
        }
        bool doAdd(UnsignedInt, const ImageData2D&, Containers::StringView) override {
            Debug{} << "Adding 2D image";
            return true;
        }
        bool doAdd(UnsignedInt, const ImageData3D&, Containers::StringView) override {
            Debug{} << "Adding 3D image";
            return true;
        }
    } converter;

    CORRADE_VERIFY(converter.beginData());

    Containers::String out;
    Debug redirectOutput{&out};
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.addImporterContents(importer, SceneContent::Meshes|SceneContent::MeshLevels));
    CORRADE_VERIFY(!converter.addImporterContents(importer, SceneContent::Images1D|SceneContent::ImageLevels));
    CORRADE_VERIFY(!converter.addImporterContents(importer, SceneContent::Images2D|SceneContent::ImageLevels));
    CORRADE_VERIFY(!converter.addImporterContents(importer, SceneContent::Images3D|SceneContent::ImageLevels));
    CORRADE_COMPARE(out,
        "Adding mesh\n"
        "Adding mesh\n"
        "Trade::AbstractSceneConverter::addImporterContents(): mesh 2 contains 5 levels but the converter doesn't support Trade::SceneConverterFeature::MeshLevels\n"
        "Adding 1D image\n"
        "Adding 1D image\n"
        "Trade::AbstractSceneConverter::addImporterContents(): 1D image 2 contains 2 levels but the converter doesn't support Trade::SceneConverterFeature::ImageLevels\n"
        "Adding 2D image\n"
        "Adding 2D image\n"
        "Trade::AbstractSceneConverter::addImporterContents(): 2D image 2 contains 3 levels but the converter doesn't support Trade::SceneConverterFeature::ImageLevels\n"
        "Adding 3D image\n"
        "Adding 3D image\n"
        "Trade::AbstractSceneConverter::addImporterContents(): 3D image 2 contains 4 levels but the converter doesn't support Trade::SceneConverterFeature::ImageLevels\n");
}

void AbstractSceneConverterTest::addImporterContentsNotSupportedUncompressedImage() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage1DCount() const override { return 4; }
        UnsignedInt doImage1DLevelCount(UnsignedInt) override { return 3; }
        Containers::Optional<ImageData1D> doImage1D(UnsignedInt id, UnsignedInt level) override {
            return id == 2 && level == 1 ?
                ImageData1D{PixelFormat::RGBA8Unorm, 1, DataFlags{}, "yes"} :
                ImageData1D{CompressedPixelFormat::Bc1RGBUnorm, 1, DataFlags{}, "hello!!"};
        }

        UnsignedInt doImage2DCount() const override { return 5; }
        UnsignedInt doImage2DLevelCount(UnsignedInt) override { return 4; }
        Containers::Optional<ImageData2D> doImage2D(UnsignedInt id, UnsignedInt level) override {
            return id == 3 && level == 2 ?
                ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, DataFlags{}, "yes"} :
                ImageData2D{CompressedPixelFormat::Bc1RGBUnorm, {1, 1}, DataFlags{}, "hello!!"};
        }

        UnsignedInt doImage3DCount() const override { return 6; }
        UnsignedInt doImage3DLevelCount(UnsignedInt) override { return 5; }
        Containers::Optional<ImageData3D> doImage3D(UnsignedInt id, UnsignedInt level) override {
            return id == 4 && level == 3 ?
                ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, DataFlags{}, "yes"} :
                ImageData3D{CompressedPixelFormat::Bc1RGBUnorm, {1, 1, 1}, DataFlags{}, "hello!!"};
        }
    } importer;

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToData|
                SceneConverterFeature::AddCompressedImages1D|
                SceneConverterFeature::AddCompressedImages2D|
                SceneConverterFeature::AddCompressedImages3D|
                SceneConverterFeature::ImageLevels;
        }
        bool doBeginData() override { return true; }

        bool doAdd(UnsignedInt, const Containers::Iterable<const Trade::ImageData1D>&, Containers::StringView) override {
            Debug{} << "Added 1D image";
            return true;
        }
        bool doAdd(UnsignedInt, const Containers::Iterable<const Trade::ImageData2D>&, Containers::StringView) override {
            Debug{} << "Added 2D image";
            return true;
        }
        bool doAdd(UnsignedInt, const Containers::Iterable<const Trade::ImageData3D>&, Containers::StringView) override {
            Debug{} << "Added 3D image";
            return true;
        }
    } converter;

    CORRADE_VERIFY(converter.beginData());

    Containers::String out;
    Debug redirectOutput{&out};
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.addImporterContents(importer, SceneContent::Images1D|SceneContent::ImageLevels));
    CORRADE_VERIFY(!converter.addImporterContents(importer, SceneContent::Images2D|SceneContent::ImageLevels));
    CORRADE_VERIFY(!converter.addImporterContents(importer, SceneContent::Images3D|SceneContent::ImageLevels));
    CORRADE_COMPARE(out,
        "Added 1D image\n"
        "Added 1D image\n"
        "Trade::AbstractSceneConverter::addImporterContents(): 1D image 2 level 1 is uncompressed but the converter doesn't support Trade::SceneConverterFeature::AddImages1D\n"
        "Added 2D image\n"
        "Added 2D image\n"
        "Added 2D image\n"
        "Trade::AbstractSceneConverter::addImporterContents(): 2D image 3 level 2 is uncompressed but the converter doesn't support Trade::SceneConverterFeature::AddImages2D\n"
        "Added 3D image\n"
        "Added 3D image\n"
        "Added 3D image\n"
        "Added 3D image\n"
        "Trade::AbstractSceneConverter::addImporterContents(): 3D image 4 level 3 is uncompressed but the converter doesn't support Trade::SceneConverterFeature::AddImages3D\n");
}

void AbstractSceneConverterTest::addImporterContentsNotSupportedCompressedImage() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage1DCount() const override { return 4; }
        UnsignedInt doImage1DLevelCount(UnsignedInt) override { return 3; }
        Containers::Optional<ImageData1D> doImage1D(UnsignedInt id, UnsignedInt level) override {
            return id == 2 && level == 1 ?
                ImageData1D{CompressedPixelFormat::Bc1RGBUnorm, 1, DataFlags{}, "hello!!"} :
                ImageData1D{PixelFormat::RGBA8Unorm, 1, DataFlags{}, "yes"};
        }

        UnsignedInt doImage2DCount() const override { return 5; }
        UnsignedInt doImage2DLevelCount(UnsignedInt) override { return 4; }
        Containers::Optional<ImageData2D> doImage2D(UnsignedInt id, UnsignedInt level) override {
            return id == 3 && level == 2 ?
                ImageData2D{CompressedPixelFormat::Bc1RGBUnorm, {1, 1}, DataFlags{}, "hello!!"} :
                ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, DataFlags{}, "yes"};
        }

        UnsignedInt doImage3DCount() const override { return 6; }
        UnsignedInt doImage3DLevelCount(UnsignedInt) override { return 5; }
        Containers::Optional<ImageData3D> doImage3D(UnsignedInt id, UnsignedInt level) override {
            return id == 4 && level == 3 ?
                ImageData3D{CompressedPixelFormat::Bc1RGBUnorm, {1, 1, 1}, DataFlags{}, "hello!!"} :
                ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, DataFlags{}, "yes"};
        }
    } importer;

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToData|
                SceneConverterFeature::AddImages1D|
                SceneConverterFeature::AddImages2D|
                SceneConverterFeature::AddImages3D|
                SceneConverterFeature::ImageLevels;
        }
        bool doBeginData() override { return true; }

        bool doAdd(UnsignedInt, const Containers::Iterable<const Trade::ImageData1D>&, Containers::StringView) override {
            Debug{} << "Added 1D image";
            return true;
        }
        bool doAdd(UnsignedInt, const Containers::Iterable<const Trade::ImageData2D>&, Containers::StringView) override {
            Debug{} << "Added 2D image";
            return true;
        }
        bool doAdd(UnsignedInt, const Containers::Iterable<const Trade::ImageData3D>&, Containers::StringView) override {
            Debug{} << "Added 3D image";
            return true;
        }
    } converter;

    CORRADE_VERIFY(converter.beginData());

    Containers::String out;
    Debug redirectOutput{&out};
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.addImporterContents(importer, SceneContent::Images1D|SceneContent::ImageLevels));
    CORRADE_VERIFY(!converter.addImporterContents(importer, SceneContent::Images2D|SceneContent::ImageLevels));
    CORRADE_VERIFY(!converter.addImporterContents(importer, SceneContent::Images3D|SceneContent::ImageLevels));
    CORRADE_COMPARE(out,
        "Added 1D image\n"
        "Added 1D image\n"
        "Trade::AbstractSceneConverter::addImporterContents(): 1D image 2 level 1 is compressed but the converter doesn't support Trade::SceneConverterFeature::AddCompressedImages1D\n"
        "Added 2D image\n"
        "Added 2D image\n"
        "Added 2D image\n"
        "Trade::AbstractSceneConverter::addImporterContents(): 2D image 3 level 2 is compressed but the converter doesn't support Trade::SceneConverterFeature::AddCompressedImages2D\n"
        "Added 3D image\n"
        "Added 3D image\n"
        "Added 3D image\n"
        "Added 3D image\n"
        "Trade::AbstractSceneConverter::addImporterContents(): 3D image 4 level 3 is compressed but the converter doesn't support Trade::SceneConverterFeature::AddCompressedImages3D\n");
}

void AbstractSceneConverterTest::addSupportedImporterContents() {
    auto&& data = AddSupportedImporterContentsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 2; }
        Containers::Optional<SceneData> doScene(UnsignedInt) override {
            return SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {}};
        }

        UnsignedInt doAnimationCount() const override { return 3; }
        Containers::Optional<AnimationData> doAnimation(UnsignedInt) override {
            return AnimationData{nullptr, {}};
        }

        UnsignedInt doLightCount() const override { return 4; }
        Containers::Optional<LightData> doLight(UnsignedInt) override {
            return LightData{LightType::Point, {}, {}};
        }

        UnsignedInt doCameraCount() const override { return 5; }
        Containers::Optional<CameraData> doCamera(UnsignedInt) override {
            return CameraData{CameraType::Orthographic2D, {}, 0.0f, 0.0f};
        }

        UnsignedInt doSkin2DCount() const override { return 6; }
        Containers::Optional<SkinData2D> doSkin2D(UnsignedInt) override {
            return SkinData2D{{}, {}};
        }

        UnsignedInt doSkin3DCount() const override { return 7; }
        Containers::Optional<SkinData3D> doSkin3D(UnsignedInt) override {
            return SkinData3D{{}, {}};
        }

        UnsignedInt doMeshCount() const override { return 8; }
        UnsignedInt doMeshLevelCount(UnsignedInt) override {
            return 5;
        }
        Containers::Optional<MeshData> doMesh(UnsignedInt, UnsignedInt) override {
            return MeshData{{}, 0};
        }

        UnsignedInt doMaterialCount() const override { return 9; }
        Containers::Optional<MaterialData> doMaterial(UnsignedInt) override {
            return MaterialData{{}, {}};
        }

        UnsignedInt doTextureCount() const override { return 10; }
        Containers::Optional<TextureData> doTexture(UnsignedInt) override {
            return TextureData{TextureType::Texture1D, SamplerFilter::Nearest, SamplerFilter::Nearest, SamplerMipmap::Nearest, SamplerWrapping::ClampToEdge, 0};
        }

        UnsignedInt doImage1DCount() const override { return 11; }
        UnsignedInt doImage1DLevelCount(UnsignedInt) override {
            return 5;
        }
        Containers::Optional<ImageData1D> doImage1D(UnsignedInt, UnsignedInt) override {
            return ImageData1D{PixelFormat::RGBA8Unorm, 1, DataFlags{}, "yes"};
        }

        UnsignedInt doImage2DCount() const override { return 12; }
        UnsignedInt doImage2DLevelCount(UnsignedInt) override {
            return 5;
        }
        Containers::Optional<ImageData2D> doImage2D(UnsignedInt, UnsignedInt) override {
            return ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, DataFlags{}, "yes"};
        }

        UnsignedInt doImage3DCount() const override { return 13; }
        UnsignedInt doImage3DLevelCount(UnsignedInt) override {
            return 5;
        }
        Containers::Optional<ImageData3D> doImage3D(UnsignedInt, UnsignedInt) override {
            return ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, DataFlags{}, "yes"};
        }
    } importer;

    struct Converter: AbstractSceneConverter {
        explicit Converter(SceneConverterFeatures except): except{except} {}

        SceneConverterFeatures doFeatures() const override {
            return ~except &
               (SceneConverterFeature::ConvertMultipleToData|
                SceneConverterFeature::AddScenes|
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
                SceneConverterFeature::MeshLevels|
                SceneConverterFeature::ImageLevels);
        }

        bool doBeginData() override { return true; }

        bool doAdd(UnsignedInt, const SceneData&, Containers::StringView) override {
            return true;
        }

        bool doAdd(UnsignedInt, const AnimationData&, Containers::StringView) override {
            return true;
        }
        bool doAdd(UnsignedInt, const LightData&, Containers::StringView) override {
            return true;
        }
        bool doAdd(UnsignedInt, const CameraData&, Containers::StringView) override {
            return true;
        }
        bool doAdd(UnsignedInt, const SkinData2D&, Containers::StringView) override {
            return true;
        }
        bool doAdd(UnsignedInt, const SkinData3D&, Containers::StringView) override {
            return true;
        }
        bool doAdd(UnsignedInt, const Containers::Iterable<const MeshData>&, Containers::StringView) override {
            return true;
        }
        bool doAdd(UnsignedInt, const MaterialData&, Containers::StringView) override {
            return true;
        }
        bool doAdd(UnsignedInt, const TextureData&, Containers::StringView) override {
            return true;
        }
        bool doAdd(UnsignedInt, const Containers::Iterable<const ImageData1D>&, Containers::StringView) override {
            return true;
        }
        bool doAdd(UnsignedInt, const Containers::Iterable<const ImageData2D>&, Containers::StringView) override {
            return true;
        }
        bool doAdd(UnsignedInt, const Containers::Iterable<const ImageData3D>&, Containers::StringView) override {
            return true;
        }

        SceneConverterFeatures except;
    } converter{data.exceptFeatures};

    CORRADE_VERIFY(converter.beginData());

    Containers::String out;
    Warning redirectWarning{&out};
    CORRADE_VERIFY(converter.addSupportedImporterContents(importer, ~data.wantExceptContents));
    CORRADE_COMPARE(out, Utility::format(
        "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring {} not supported by the converter\n", data.except));

    /* All data except the one unsupported should be added */
    const SceneContents expectedConvertedExceptContents = data.exceptContents| data.wantExceptContents;
    CORRADE_COMPARE(converter.sceneCount(),
        expectedConvertedExceptContents & SceneContent::Scenes ? 0 : importer.sceneCount());
    CORRADE_COMPARE(converter.animationCount(),
        expectedConvertedExceptContents & SceneContent::Animations ? 0 : importer.animationCount());
    CORRADE_COMPARE(converter.lightCount(),
        expectedConvertedExceptContents & SceneContent::Lights ? 0 : importer.lightCount());
    CORRADE_COMPARE(converter.cameraCount(),
        expectedConvertedExceptContents & SceneContent::Cameras ? 0 : importer.cameraCount());
    CORRADE_COMPARE(converter.skin2DCount(),
        expectedConvertedExceptContents & SceneContent::Skins2D ? 0 : importer.skin2DCount());
    CORRADE_COMPARE(converter.skin3DCount(),
        expectedConvertedExceptContents & SceneContent::Skins3D ? 0 : importer.skin3DCount());
    CORRADE_COMPARE(converter.meshCount(),
        expectedConvertedExceptContents & SceneContent::Meshes ? 0 : importer.meshCount());
    CORRADE_COMPARE(converter.materialCount(),
        expectedConvertedExceptContents & SceneContent::Materials ? 0 : importer.materialCount());
    CORRADE_COMPARE(converter.textureCount(),
        expectedConvertedExceptContents & SceneContent::Textures ? 0 : importer.textureCount());
    CORRADE_COMPARE(converter.image1DCount(),
        expectedConvertedExceptContents & SceneContent::Images1D ? 0 : importer.image1DCount());
    CORRADE_COMPARE(converter.image2DCount(),
        expectedConvertedExceptContents & SceneContent::Images2D ? 0 : importer.image2DCount());
    CORRADE_COMPARE(converter.image3DCount(),
        expectedConvertedExceptContents & SceneContent::Images3D ? 0 : importer.image3DCount());
}

void AbstractSceneConverterTest::addSupportedImporterContentsLevels() {
    /* Similar to addImporterContentsNotSupportedLevels(), but not failing */

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMeshCount() const override { return 4; }
        UnsignedInt doMeshLevelCount(UnsignedInt id) override {
            return id == 2 ? 5 : 1;
        }
        Containers::Optional<MeshData> doMesh(UnsignedInt, UnsignedInt) override {
            return MeshData{{}, 0};
        }

        UnsignedInt doImage1DCount() const override { return 5; }
        UnsignedInt doImage1DLevelCount(UnsignedInt id) override {
            return id == 3 ? 6 : 1;
        }
        Containers::Optional<ImageData1D> doImage1D(UnsignedInt, UnsignedInt) override {
            return ImageData1D{PixelFormat::RGBA8Unorm, 1, DataFlags{}, "yes"};
        }

        UnsignedInt doImage2DCount() const override { return 6; }
        UnsignedInt doImage2DLevelCount(UnsignedInt id) override {
            return id == 4 ? 7 : 1;
        }
        Containers::Optional<ImageData2D> doImage2D(UnsignedInt, UnsignedInt) override {
            return ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, DataFlags{}, "yes"};
        }

        UnsignedInt doImage3DCount() const override { return 7; }
        UnsignedInt doImage3DLevelCount(UnsignedInt id) override {
            return id == 5 ? 8 : 1;
        }
        Containers::Optional<ImageData3D> doImage3D(UnsignedInt, UnsignedInt) override {
            return ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, DataFlags{}, "yes"};
        }
    } importer;

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return
                SceneConverterFeature::ConvertMultipleToData|
                SceneConverterFeature::AddMeshes|
                SceneConverterFeature::AddImages1D|
                SceneConverterFeature::AddImages2D|
                SceneConverterFeature::AddImages3D;
        }

        bool doBeginData() override { return true; }

        bool doAdd(UnsignedInt, const MeshData&, Containers::StringView) override {
            return true;
        }
        bool doAdd(UnsignedInt, const ImageData1D&, Containers::StringView) override {
            return true;
        }
        bool doAdd(UnsignedInt, const ImageData2D&, Containers::StringView) override {
            return true;
        }
        bool doAdd(UnsignedInt, const ImageData3D&, Containers::StringView) override {
            return true;
        }

        SceneConverterFeatures except;
    } converter;

    CORRADE_VERIFY(converter.beginData());

    Containers::String out;
    Warning redirectWarning{&out};
    CORRADE_VERIFY(converter.addSupportedImporterContents(importer));
    CORRADE_COMPARE(out,
        "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring extra 4 levels of mesh 2 not supported by the converter\n"
        "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring extra 5 levels of 1D image 3 not supported by the converter\n"
        "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring extra 6 levels of 2D image 4 not supported by the converter\n"
        "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring extra 7 levels of 3D image 5 not supported by the converter\n");

    /* All data should be added, just not the extra levels */
    CORRADE_COMPARE(converter.meshCount(), importer.meshCount());
    CORRADE_COMPARE(converter.image1DCount(), importer.image1DCount());
    CORRADE_COMPARE(converter.image2DCount(), importer.image2DCount());
    CORRADE_COMPARE(converter.image3DCount(), importer.image3DCount());
}

void AbstractSceneConverterTest::addSupportedImporterContentsNotOpened() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    } importer;

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            return SceneConverterFeature::ConvertMultipleToData;
        }
        bool doBeginData() override { return true; }
    } converter;

    CORRADE_VERIFY(converter.beginData());

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.addSupportedImporterContents(importer));
    CORRADE_COMPARE(out, "Trade::AbstractSceneConverter::addSupportedImporterContents(): the importer is not opened\n");
}

void AbstractSceneConverterTest::debugFeature() {
    Containers::String out;

    Debug{&out} << SceneConverterFeature::ConvertMeshInPlace << SceneConverterFeature(0xdeaddead);
    CORRADE_COMPARE(out, "Trade::SceneConverterFeature::ConvertMeshInPlace Trade::SceneConverterFeature(0xdeaddead)\n");
}

void AbstractSceneConverterTest::debugFeaturePacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << SceneConverterFeature::ConvertMeshInPlace << Debug::packed << SceneConverterFeature(0xdeaddead) << SceneConverterFeature::AddCameras;
    CORRADE_COMPARE(out, "ConvertMeshInPlace 0xdeaddead Trade::SceneConverterFeature::AddCameras\n");
}

void AbstractSceneConverterTest::debugFeatures() {
    Containers::String out;

    Debug{&out} << (SceneConverterFeature::ConvertMesh|SceneConverterFeature::ConvertMeshToFile) << SceneConverterFeatures{};
    CORRADE_COMPARE(out, "Trade::SceneConverterFeature::ConvertMesh|Trade::SceneConverterFeature::ConvertMeshToFile Trade::SceneConverterFeatures{}\n");
}

void AbstractSceneConverterTest::debugFeaturesPacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << (SceneConverterFeature::ConvertMesh|SceneConverterFeature::ConvertMeshToFile) << Debug::packed << SceneConverterFeatures{} << SceneConverterFeature::AddLights;
    CORRADE_COMPARE(out, "ConvertMesh|ConvertMeshToFile {} Trade::SceneConverterFeature::AddLights\n");
}

void AbstractSceneConverterTest::debugFeaturesSupersets() {
    /* ConvertMeshToData is a superset of ConvertMeshToFile, so only one should
       be printed */
    {
        Containers::String out;
        Debug{&out} << (SceneConverterFeature::ConvertMeshToData|SceneConverterFeature::ConvertMeshToFile);
        CORRADE_COMPARE(out, "Trade::SceneConverterFeature::ConvertMeshToData\n");

    /* ConvertMultipleToData is a superset of ConvertMultipleToFile, so only
       one should be printed */
    } {
        Containers::String out;
        Debug{&out} << (SceneConverterFeature::ConvertMultipleToData|SceneConverterFeature::ConvertMultipleToFile);
        CORRADE_COMPARE(out, "Trade::SceneConverterFeature::ConvertMultipleToData\n");
    }
}

void AbstractSceneConverterTest::debugFlag() {
    Containers::String out;

    Debug{&out} << SceneConverterFlag::Verbose << SceneConverterFlag(0xf0);
    CORRADE_COMPARE(out, "Trade::SceneConverterFlag::Verbose Trade::SceneConverterFlag(0xf0)\n");
}

void AbstractSceneConverterTest::debugFlags() {
    Containers::String out;

    Debug{&out} << (SceneConverterFlag::Verbose|SceneConverterFlag(0xf0)) << SceneConverterFlags{};
    CORRADE_COMPARE(out, "Trade::SceneConverterFlag::Verbose|Trade::SceneConverterFlag(0xf0) Trade::SceneConverterFlags{}\n");
}

void AbstractSceneConverterTest::debugContent() {
    Containers::String out;

    Debug{&out} << SceneContent::Skins3D << SceneContent(0xdeaddead);
    CORRADE_COMPARE(out, "Trade::SceneContent::Skins3D Trade::SceneContent(0xdeaddead)\n");
}

void AbstractSceneConverterTest::debugContentPacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << SceneContent::Animations << Debug::packed << SceneContent(0xdeaddead) << SceneContent::Cameras;
    CORRADE_COMPARE(out, "Animations 0xdeaddead Trade::SceneContent::Cameras\n");
}

void AbstractSceneConverterTest::debugContents() {
    Containers::String out;

    Debug{&out} << (SceneContent::Animations|SceneContent::MeshLevels) << SceneConverterFeatures{};
    CORRADE_COMPARE(out, "Trade::SceneContent::Animations|Trade::SceneContent::MeshLevels Trade::SceneConverterFeatures{}\n");
}

void AbstractSceneConverterTest::debugContentsPacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << (SceneContent::Animations|SceneContent::MeshLevels) << Debug::packed << SceneConverterFeatures{} << SceneContent::Lights;
    CORRADE_COMPARE(out, "Animations|MeshLevels {} Trade::SceneContent::Lights\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AbstractSceneConverterTest)
