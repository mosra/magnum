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

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/FileToString.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/Path.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Trade/AbstractImageConverter.h"
#include "Magnum/Trade/ImageData.h"

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct AbstractImageConverterTest: TestSuite::Tester {
    explicit AbstractImageConverterTest();

    void construct();
    void constructWithPluginManagerReference();

    void setFlags();
    void setFlagsNotImplemented();

    void thingNotSupported();

    void extensionMimeType();
    void extensionMimeTypeNotImplemented();
    void extensionMimeTypeCustomDeleter();

    void convert1D();
    void convert2D();
    void convert3D();
    void convert1DFailed();
    void convert2DFailed();
    void convert3DFailed();
    void convert1DNotImplemented();
    void convert2DNotImplemented();
    void convert3DNotImplemented();
    void convert1DCustomDeleter();
    void convert2DCustomDeleter();
    void convert3DCustomDeleter();

    void convertCompressed1D();
    void convertCompressed2D();
    void convertCompressed3D();
    void convertCompressed1DFailed();
    void convertCompressed2DFailed();
    void convertCompressed3DFailed();
    void convertCompressed1DNotImplemented();
    void convertCompressed2DNotImplemented();
    void convertCompressed3DNotImplemented();
    void convertCompressed1DCustomDeleter();
    void convertCompressed2DCustomDeleter();
    void convertCompressed3DCustomDeleter();

    void convertImageData1D();
    void convertImageData2D();
    void convertImageData3D();

    void convert1DToData();
    void convert2DToData();
    void convert3DToData();
    void convert1DToDataFailed();
    void convert2DToDataFailed();
    void convert3DToDataFailed();
    /* 1D/2D/3D share the same image validity check function, so only verify
       one dimension thoroughly and for others just that the check is used */
    void convert1DToDataInvalidImage();
    void convert2DToDataZeroSize();
    void convert2DToDataNullptr();
    void convert3DToDataInvalidImage();
    void convert1DToDataNotImplemented();
    void convert2DToDataNotImplemented();
    void convert3DToDataNotImplemented();
    void convert1DToDataCustomDeleter();
    void convert2DToDataCustomDeleter();
    void convert3DToDataCustomDeleter();

    void convertCompressed1DToData();
    void convertCompressed2DToData();
    void convertCompressed3DToData();
    void convertCompressed1DToDataFailed();
    void convertCompressed2DToDataFailed();
    void convertCompressed3DToDataFailed();
    /* Compressed share the same image validity check function with
       uncompressed, so only verify it's used */
    void convertCompressed1DToDataInvalidImage();
    void convertCompressed2DToDataInvalidImage();
    void convertCompressed3DToDataInvalidImage();
    void convertCompressed1DToDataNotImplemented();
    void convertCompressed2DToDataNotImplemented();
    void convertCompressed3DToDataNotImplemented();
    void convertCompressed1DToDataCustomDeleter();
    void convertCompressed2DToDataCustomDeleter();
    void convertCompressed3DToDataCustomDeleter();

    void convertImageData1DToData();
    void convertImageData2DToData();
    void convertImageData3DToData();

    void convertLevels1DToData();
    void convertLevels2DToData();
    void convertLevels3DToData();
    void convertLevels1DToDataFailed();
    void convertLevels2DToDataFailed();
    void convertLevels3DToDataFailed();
    /* 1D/2D/3D share the same image list validity check function, so only
       verify one dimension thoroughly and for others just that the check is
       used */
    void convertLevels1DToDataInvalidImage();
    void convertLevels2DToDataNoLevels();
    void convertLevels2DToDataZeroSize();
    void convertLevels2DToDataNullptr();
    void convertLevels2DToDataInconsistentFormat();
    void convertLevels2DToDataInconsistentFormatExtra();
    void convertLevels2DToDataInconsistentFlags();
    void convertLevels3DToDataInvalidImage();
    void convertLevels1DToDataNotImplemented();
    void convertLevels2DToDataNotImplemented();
    void convertLevels3DToDataNotImplemented();
    void convertLevels1DToDataCustomDeleter();
    void convertLevels2DToDataCustomDeleter();
    void convertLevels3DToDataCustomDeleter();

    void convertCompressedLevels1DToData();
    void convertCompressedLevels2DToData();
    void convertCompressedLevels3DToData();
    void convertCompressedLevels1DToDataFailed();
    void convertCompressedLevels2DToDataFailed();
    void convertCompressedLevels3DToDataFailed();
    /* Compressed have different format checks for uncompressed so it's tested
       again; 1D/2D/3D share the same image list validity check function, so
       only verify one dimension thoroughly and for others just that the check
       is used */
    void convertCompressedLevels1DToDataInvalidImage();
    void convertCompressedLevels2DToDataNoLevels();
    void convertCompressedLevels2DToDataZeroSize();
    void convertCompressedLevels2DToDataNullptr();
    void convertCompressedLevels2DToDataInconsistentFormat();
    void convertCompressedLevels2DToDataInconsistentFlags();
    void convertCompressedLevels3DToDataInvalidImage();
    void convertCompressedLevels1DToDataNotImplemented();
    void convertCompressedLevels2DToDataNotImplemented();
    void convertCompressedLevels3DToDataNotImplemented();
    void convertCompressedLevels1DToDataCustomDeleter();
    void convertCompressedLevels2DToDataCustomDeleter();
    void convertCompressedLevels3DToDataCustomDeleter();

    void convert1DToDataThroughLevels();
    void convert2DToDataThroughLevels();
    void convert3DToDataThroughLevels();

    void convertCompressed1DToDataThroughLevels();
    void convertCompressed2DToDataThroughLevels();
    void convertCompressed3DToDataThroughLevels();

    void convert1DToFile();
    void convert2DToFile();
    void convert3DToFile();
    void convert1DToFileFailed();
    void convert2DToFileFailed();
    void convert3DToFileFailed();
    void convert1DToFileThroughData();
    void convert2DToFileThroughData();
    void convert3DToFileThroughData();
    void convert1DToFileThroughDataFailed();
    void convert2DToFileThroughDataFailed();
    void convert3DToFileThroughDataFailed();
    void convert1DToFileThroughDataNotWritable();
    void convert2DToFileThroughDataNotWritable();
    void convert3DToFileThroughDataNotWritable();
    /* ToFile share the same check function with ToData, so only verify it's
       used */
    void convert1DToFileInvalidImage();
    void convert2DToFileInvalidImage();
    void convert3DToFileInvalidImage();
    void convert1DToFileNotImplemented();
    void convert2DToFileNotImplemented();
    void convert3DToFileNotImplemented();

    void convertCompressed1DToFile();
    void convertCompressed2DToFile();
    void convertCompressed3DToFile();
    void convertCompressed1DToFileFailed();
    void convertCompressed2DToFileFailed();
    void convertCompressed3DToFileFailed();
    void convertCompressed1DToFileThroughData();
    void convertCompressed2DToFileThroughData();
    void convertCompressed3DToFileThroughData();
    void convertCompressed1DToFileThroughDataFailed();
    void convertCompressed2DToFileThroughDataFailed();
    void convertCompressed3DToFileThroughDataFailed();
    void convertCompressed1DToFileThroughDataNotWritable();
    void convertCompressed2DToFileThroughDataNotWritable();
    void convertCompressed3DToFileThroughDataNotWritable();
    /* ToFile share the same check function with ToData, so only verify it's
       used */
    void convertCompressed1DToFileInvalidImage();
    void convertCompressed2DToFileInvalidImage();
    void convertCompressed3DToFileInvalidImage();
    void convertCompressed1DToFileNotImplemented();
    void convertCompressed2DToFileNotImplemented();
    void convertCompressed3DToFileNotImplemented();

    void convertImageData1DToFile();
    void convertImageData2DToFile();
    void convertImageData3DToFile();

    void convertLevels1DToFile();
    void convertLevels2DToFile();
    void convertLevels3DToFile();
    void convertLevels1DToFileFailed();
    void convertLevels2DToFileFailed();
    void convertLevels3DToFileFailed();
    void convertLevels1DToFileThroughData();
    void convertLevels2DToFileThroughData();
    void convertLevels3DToFileThroughData();
    void convertLevels1DToFileThroughDataFailed();
    void convertLevels2DToFileThroughDataFailed();
    void convertLevels3DToFileThroughDataFailed();
    void convertLevels1DToFileThroughDataNotWritable();
    void convertLevels2DToFileThroughDataNotWritable();
    void convertLevels3DToFileThroughDataNotWritable();
    /* ToFile share the same check function with ToData, so only verify it's
       used */
    void convertLevels1DToFileInvalidImage();
    void convertLevels2DToFileInvalidImage();
    void convertLevels3DToFileInvalidImage();
    void convertLevels1DToFileNotImplemented();
    void convertLevels2DToFileNotImplemented();
    void convertLevels3DToFileNotImplemented();

    void convertCompressedLevels1DToFile();
    void convertCompressedLevels2DToFile();
    void convertCompressedLevels3DToFile();
    void convertCompressedLevels1DToFileFailed();
    void convertCompressedLevels2DToFileFailed();
    void convertCompressedLevels3DToFileFailed();
    void convertCompressedLevels1DToFileThroughData();
    void convertCompressedLevels2DToFileThroughData();
    void convertCompressedLevels3DToFileThroughData();
    void convertCompressedLevels1DToFileThroughDataFailed();
    void convertCompressedLevels2DToFileThroughDataFailed();
    void convertCompressedLevels3DToFileThroughDataFailed();
    void convertCompressedLevels1DToFileThroughDataNotWritable();
    void convertCompressedLevels2DToFileThroughDataNotWritable();
    void convertCompressedLevels3DToFileThroughDataNotWritable();
    /* ToFile share the same check function with ToData, so only verify it's
       used */
    void convertCompressedLevels1DToFileInvalidImage();
    void convertCompressedLevels2DToFileInvalidImage();
    void convertCompressedLevels3DToFileInvalidImage();
    void convertCompressedLevels1DToFileNotImplemented();
    void convertCompressedLevels2DToFileNotImplemented();
    void convertCompressedLevels3DToFileNotImplemented();

    void convert1DToFileThroughLevels();
    void convert2DToFileThroughLevels();
    void convert3DToFileThroughLevels();

    void convertCompressed1DToFileThroughLevels();
    void convertCompressed2DToFileThroughLevels();
    void convertCompressed3DToFileThroughLevels();

    void debugFeature();
    void debugFeaturePacked();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void debugFeatureDeprecated();
    void debugFeatureDeprecatedPacked();
    #endif
    void debugFeatures();
    void debugFeaturesPacked();
    void debugFeaturesSupersets();
    void debugFlag();
    void debugFlags();
};

AbstractImageConverterTest::AbstractImageConverterTest() {
    addTests({&AbstractImageConverterTest::construct,
              &AbstractImageConverterTest::constructWithPluginManagerReference,

              &AbstractImageConverterTest::setFlags,
              &AbstractImageConverterTest::setFlagsNotImplemented,

              &AbstractImageConverterTest::thingNotSupported,

              &AbstractImageConverterTest::extensionMimeType,
              &AbstractImageConverterTest::extensionMimeTypeNotImplemented,
              &AbstractImageConverterTest::extensionMimeTypeCustomDeleter,

              &AbstractImageConverterTest::convert1D,
              &AbstractImageConverterTest::convert2D,
              &AbstractImageConverterTest::convert3D,
              &AbstractImageConverterTest::convert1DFailed,
              &AbstractImageConverterTest::convert2DFailed,
              &AbstractImageConverterTest::convert3DFailed,
              &AbstractImageConverterTest::convert1DNotImplemented,
              &AbstractImageConverterTest::convert2DNotImplemented,
              &AbstractImageConverterTest::convert3DNotImplemented,
              &AbstractImageConverterTest::convert1DCustomDeleter,
              &AbstractImageConverterTest::convert2DCustomDeleter,
              &AbstractImageConverterTest::convert3DCustomDeleter,

              &AbstractImageConverterTest::convertCompressed1D,
              &AbstractImageConverterTest::convertCompressed2D,
              &AbstractImageConverterTest::convertCompressed3D,
              &AbstractImageConverterTest::convertCompressed1DFailed,
              &AbstractImageConverterTest::convertCompressed2DFailed,
              &AbstractImageConverterTest::convertCompressed3DFailed,
              &AbstractImageConverterTest::convertCompressed1DNotImplemented,
              &AbstractImageConverterTest::convertCompressed2DNotImplemented,
              &AbstractImageConverterTest::convertCompressed3DNotImplemented,
              &AbstractImageConverterTest::convertCompressed1DCustomDeleter,
              &AbstractImageConverterTest::convertCompressed2DCustomDeleter,
              &AbstractImageConverterTest::convertCompressed3DCustomDeleter,

              &AbstractImageConverterTest::convertImageData1D,
              &AbstractImageConverterTest::convertImageData2D,
              &AbstractImageConverterTest::convertImageData3D,

              &AbstractImageConverterTest::convert1DToData,
              &AbstractImageConverterTest::convert2DToData,
              &AbstractImageConverterTest::convert3DToData,
              &AbstractImageConverterTest::convert1DToDataFailed,
              &AbstractImageConverterTest::convert2DToDataFailed,
              &AbstractImageConverterTest::convert3DToDataFailed,
              &AbstractImageConverterTest::convert1DToDataInvalidImage,
              &AbstractImageConverterTest::convert2DToDataZeroSize,
              &AbstractImageConverterTest::convert2DToDataNullptr,
              &AbstractImageConverterTest::convert3DToDataInvalidImage,
              &AbstractImageConverterTest::convert1DToDataNotImplemented,
              &AbstractImageConverterTest::convert2DToDataNotImplemented,
              &AbstractImageConverterTest::convert3DToDataNotImplemented,
              &AbstractImageConverterTest::convert1DToDataCustomDeleter,
              &AbstractImageConverterTest::convert2DToDataCustomDeleter,
              &AbstractImageConverterTest::convert3DToDataCustomDeleter,

              &AbstractImageConverterTest::convertCompressed1DToData,
              &AbstractImageConverterTest::convertCompressed2DToData,
              &AbstractImageConverterTest::convertCompressed3DToData,
              &AbstractImageConverterTest::convertCompressed1DToDataFailed,
              &AbstractImageConverterTest::convertCompressed2DToDataFailed,
              &AbstractImageConverterTest::convertCompressed3DToDataFailed,
              &AbstractImageConverterTest::convertCompressed1DToDataInvalidImage,
              &AbstractImageConverterTest::convertCompressed2DToDataInvalidImage,
              &AbstractImageConverterTest::convertCompressed3DToDataInvalidImage,
              &AbstractImageConverterTest::convertCompressed1DToDataNotImplemented,
              &AbstractImageConverterTest::convertCompressed2DToDataNotImplemented,
              &AbstractImageConverterTest::convertCompressed3DToDataNotImplemented,
              &AbstractImageConverterTest::convertCompressed1DToDataCustomDeleter,
              &AbstractImageConverterTest::convertCompressed2DToDataCustomDeleter,
              &AbstractImageConverterTest::convertCompressed3DToDataCustomDeleter,

              &AbstractImageConverterTest::convertImageData1DToData,
              &AbstractImageConverterTest::convertImageData2DToData,
              &AbstractImageConverterTest::convertImageData3DToData,

              &AbstractImageConverterTest::convertLevels1DToData,
              &AbstractImageConverterTest::convertLevels2DToData,
              &AbstractImageConverterTest::convertLevels3DToData,
              &AbstractImageConverterTest::convertLevels1DToDataFailed,
              &AbstractImageConverterTest::convertLevels2DToDataFailed,
              &AbstractImageConverterTest::convertLevels3DToDataFailed,
              &AbstractImageConverterTest::convertLevels1DToDataInvalidImage,
              &AbstractImageConverterTest::convertLevels2DToDataNoLevels,
              &AbstractImageConverterTest::convertLevels2DToDataZeroSize,
              &AbstractImageConverterTest::convertLevels2DToDataNullptr,
              &AbstractImageConverterTest::convertLevels2DToDataInconsistentFormat,
              &AbstractImageConverterTest::convertLevels2DToDataInconsistentFormatExtra,
              &AbstractImageConverterTest::convertLevels2DToDataInconsistentFlags,
              &AbstractImageConverterTest::convertLevels3DToDataInvalidImage,
              &AbstractImageConverterTest::convertLevels1DToDataNotImplemented,
              &AbstractImageConverterTest::convertLevels2DToDataNotImplemented,
              &AbstractImageConverterTest::convertLevels3DToDataNotImplemented,
              &AbstractImageConverterTest::convertLevels1DToDataCustomDeleter,
              &AbstractImageConverterTest::convertLevels2DToDataCustomDeleter,
              &AbstractImageConverterTest::convertLevels3DToDataCustomDeleter,

              &AbstractImageConverterTest::convertCompressedLevels1DToData,
              &AbstractImageConverterTest::convertCompressedLevels2DToData,
              &AbstractImageConverterTest::convertCompressedLevels3DToData,
              &AbstractImageConverterTest::convertCompressedLevels1DToDataFailed,
              &AbstractImageConverterTest::convertCompressedLevels2DToDataFailed,
              &AbstractImageConverterTest::convertCompressedLevels3DToDataFailed,
              &AbstractImageConverterTest::convertCompressedLevels1DToDataInvalidImage,
              &AbstractImageConverterTest::convertCompressedLevels2DToDataNoLevels,
              &AbstractImageConverterTest::convertCompressedLevels2DToDataZeroSize,
              &AbstractImageConverterTest::convertCompressedLevels2DToDataNullptr,
              &AbstractImageConverterTest::convertCompressedLevels2DToDataInconsistentFormat,
              &AbstractImageConverterTest::convertCompressedLevels2DToDataInconsistentFlags,
              &AbstractImageConverterTest::convertCompressedLevels3DToDataInvalidImage,
              &AbstractImageConverterTest::convertCompressedLevels1DToDataNotImplemented,
              &AbstractImageConverterTest::convertCompressedLevels2DToDataNotImplemented,
              &AbstractImageConverterTest::convertCompressedLevels3DToDataNotImplemented,
              &AbstractImageConverterTest::convertCompressedLevels1DToDataCustomDeleter,
              &AbstractImageConverterTest::convertCompressedLevels2DToDataCustomDeleter,
              &AbstractImageConverterTest::convertCompressedLevels3DToDataCustomDeleter,

              &AbstractImageConverterTest::convert1DToDataThroughLevels,
              &AbstractImageConverterTest::convert2DToDataThroughLevels,
              &AbstractImageConverterTest::convert3DToDataThroughLevels,

              &AbstractImageConverterTest::convertCompressed1DToDataThroughLevels,
              &AbstractImageConverterTest::convertCompressed2DToDataThroughLevels,
              &AbstractImageConverterTest::convertCompressed3DToDataThroughLevels,

              &AbstractImageConverterTest::convert1DToFile,
              &AbstractImageConverterTest::convert2DToFile,
              &AbstractImageConverterTest::convert3DToFile,
              &AbstractImageConverterTest::convert1DToFileFailed,
              &AbstractImageConverterTest::convert2DToFileFailed,
              &AbstractImageConverterTest::convert3DToFileFailed,
              &AbstractImageConverterTest::convert1DToFileThroughData,
              &AbstractImageConverterTest::convert2DToFileThroughData,
              &AbstractImageConverterTest::convert3DToFileThroughData,
              &AbstractImageConverterTest::convert1DToFileThroughDataFailed,
              &AbstractImageConverterTest::convert2DToFileThroughDataFailed,
              &AbstractImageConverterTest::convert3DToFileThroughDataFailed,
              &AbstractImageConverterTest::convert1DToFileThroughDataNotWritable,
              &AbstractImageConverterTest::convert2DToFileThroughDataNotWritable,
              &AbstractImageConverterTest::convert3DToFileThroughDataNotWritable,
              &AbstractImageConverterTest::convert1DToFileInvalidImage,
              &AbstractImageConverterTest::convert2DToFileInvalidImage,
              &AbstractImageConverterTest::convert3DToFileInvalidImage,
              &AbstractImageConverterTest::convert1DToFileNotImplemented,
              &AbstractImageConverterTest::convert2DToFileNotImplemented,
              &AbstractImageConverterTest::convert3DToFileNotImplemented,

              &AbstractImageConverterTest::convertCompressed1DToFile,
              &AbstractImageConverterTest::convertCompressed2DToFile,
              &AbstractImageConverterTest::convertCompressed3DToFile,
              &AbstractImageConverterTest::convertCompressed1DToFileFailed,
              &AbstractImageConverterTest::convertCompressed2DToFileFailed,
              &AbstractImageConverterTest::convertCompressed3DToFileFailed,
              &AbstractImageConverterTest::convertCompressed1DToFileThroughData,
              &AbstractImageConverterTest::convertCompressed2DToFileThroughData,
              &AbstractImageConverterTest::convertCompressed3DToFileThroughData,
              &AbstractImageConverterTest::convertCompressed1DToFileThroughDataFailed,
              &AbstractImageConverterTest::convertCompressed2DToFileThroughDataFailed,
              &AbstractImageConverterTest::convertCompressed3DToFileThroughDataFailed,
              &AbstractImageConverterTest::convertCompressed1DToFileThroughDataNotWritable,
              &AbstractImageConverterTest::convertCompressed2DToFileThroughDataNotWritable,
              &AbstractImageConverterTest::convertCompressed3DToFileThroughDataNotWritable,
              &AbstractImageConverterTest::convertCompressed1DToFileInvalidImage,
              &AbstractImageConverterTest::convertCompressed2DToFileInvalidImage,
              &AbstractImageConverterTest::convertCompressed3DToFileInvalidImage,
              &AbstractImageConverterTest::convertCompressed1DToFileNotImplemented,
              &AbstractImageConverterTest::convertCompressed2DToFileNotImplemented,
              &AbstractImageConverterTest::convertCompressed3DToFileNotImplemented,

              &AbstractImageConverterTest::convertImageData1DToFile,
              &AbstractImageConverterTest::convertImageData2DToFile,
              &AbstractImageConverterTest::convertImageData3DToFile,

              &AbstractImageConverterTest::convertLevels1DToFile,
              &AbstractImageConverterTest::convertLevels2DToFile,
              &AbstractImageConverterTest::convertLevels3DToFile,
              &AbstractImageConverterTest::convertLevels1DToFileFailed,
              &AbstractImageConverterTest::convertLevels2DToFileFailed,
              &AbstractImageConverterTest::convertLevels3DToFileFailed,
              &AbstractImageConverterTest::convertLevels1DToFileThroughData,
              &AbstractImageConverterTest::convertLevels2DToFileThroughData,
              &AbstractImageConverterTest::convertLevels3DToFileThroughData,
              &AbstractImageConverterTest::convertLevels1DToFileThroughDataFailed,
              &AbstractImageConverterTest::convertLevels2DToFileThroughDataFailed,
              &AbstractImageConverterTest::convertLevels3DToFileThroughDataFailed,
              &AbstractImageConverterTest::convertLevels1DToFileThroughDataNotWritable,
              &AbstractImageConverterTest::convertLevels2DToFileThroughDataNotWritable,
              &AbstractImageConverterTest::convertLevels3DToFileThroughDataNotWritable,
              &AbstractImageConverterTest::convertLevels1DToFileInvalidImage,
              &AbstractImageConverterTest::convertLevels2DToFileInvalidImage,
              &AbstractImageConverterTest::convertLevels3DToFileInvalidImage,
              &AbstractImageConverterTest::convertLevels1DToFileNotImplemented,
              &AbstractImageConverterTest::convertLevels2DToFileNotImplemented,
              &AbstractImageConverterTest::convertLevels3DToFileNotImplemented,

              &AbstractImageConverterTest::convertCompressedLevels1DToFile,
              &AbstractImageConverterTest::convertCompressedLevels2DToFile,
              &AbstractImageConverterTest::convertCompressedLevels3DToFile,
              &AbstractImageConverterTest::convertCompressedLevels1DToFileFailed,
              &AbstractImageConverterTest::convertCompressedLevels2DToFileFailed,
              &AbstractImageConverterTest::convertCompressedLevels3DToFileFailed,
              &AbstractImageConverterTest::convertCompressedLevels1DToFileThroughData,
              &AbstractImageConverterTest::convertCompressedLevels2DToFileThroughData,
              &AbstractImageConverterTest::convertCompressedLevels3DToFileThroughData,
              &AbstractImageConverterTest::convertCompressedLevels1DToFileThroughDataFailed,
              &AbstractImageConverterTest::convertCompressedLevels2DToFileThroughDataFailed,
              &AbstractImageConverterTest::convertCompressedLevels3DToFileThroughDataFailed,
              &AbstractImageConverterTest::convertCompressedLevels1DToFileThroughDataNotWritable,
              &AbstractImageConverterTest::convertCompressedLevels2DToFileThroughDataNotWritable,
              &AbstractImageConverterTest::convertCompressedLevels3DToFileThroughDataNotWritable,
              &AbstractImageConverterTest::convertCompressedLevels1DToFileInvalidImage,
              &AbstractImageConverterTest::convertCompressedLevels2DToFileInvalidImage,
              &AbstractImageConverterTest::convertCompressedLevels3DToFileInvalidImage,
              &AbstractImageConverterTest::convertCompressedLevels1DToFileNotImplemented,
              &AbstractImageConverterTest::convertCompressedLevels2DToFileNotImplemented,
              &AbstractImageConverterTest::convertCompressedLevels3DToFileNotImplemented,

              &AbstractImageConverterTest::convert1DToFileThroughLevels,
              &AbstractImageConverterTest::convert2DToFileThroughLevels,
              &AbstractImageConverterTest::convert3DToFileThroughLevels,

              &AbstractImageConverterTest::convertCompressed1DToFileThroughLevels,
              &AbstractImageConverterTest::convertCompressed2DToFileThroughLevels,
              &AbstractImageConverterTest::convertCompressed3DToFileThroughLevels,

              &AbstractImageConverterTest::debugFeature,
              &AbstractImageConverterTest::debugFeaturePacked,
              #ifdef MAGNUM_BUILD_DEPRECATED
              &AbstractImageConverterTest::debugFeatureDeprecated,
              &AbstractImageConverterTest::debugFeatureDeprecatedPacked,
              #endif
              &AbstractImageConverterTest::debugFeatures,
              &AbstractImageConverterTest::debugFeaturesPacked,
              &AbstractImageConverterTest::debugFeaturesSupersets,
              &AbstractImageConverterTest::debugFlag,
              &AbstractImageConverterTest::debugFlags});

    /* Create testing dir */
    Utility::Path::make(TRADE_TEST_OUTPUT_DIR);
}

void AbstractImageConverterTest::construct() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return {}; }
    } converter;

    CORRADE_COMPARE(converter.features(), ImageConverterFeatures{});
}

void AbstractImageConverterTest::constructWithPluginManagerReference() {
    PluginManager::Manager<AbstractImageConverter> manager;

    struct Converter: AbstractImageConverter {
        explicit Converter(PluginManager::Manager<AbstractImageConverter>& manager): AbstractImageConverter{manager} {}

        ImageConverterFeatures doFeatures() const override { return {}; }
    } converter{manager};

    CORRADE_COMPARE(converter.features(), ImageConverterFeatures{});
}

void AbstractImageConverterTest::setFlags() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return {}; }
        void doSetFlags(ImageConverterFlags flags) override {
            _flags = flags;
        }

        ImageConverterFlags _flags;
    } converter;
    CORRADE_COMPARE(converter.flags(), ImageConverterFlags{});
    CORRADE_COMPARE(converter._flags, ImageConverterFlags{});

    converter.setFlags(ImageConverterFlag::Verbose);
    CORRADE_COMPARE(converter.flags(), ImageConverterFlag::Verbose);
    CORRADE_COMPARE(converter._flags, ImageConverterFlag::Verbose);

    /** @todo use a real flag when we have more than one */
    converter.addFlags(ImageConverterFlag(4));
    CORRADE_COMPARE(converter.flags(), ImageConverterFlag::Verbose|ImageConverterFlag(4));
    CORRADE_COMPARE(converter._flags, ImageConverterFlag::Verbose|ImageConverterFlag(4));

    converter.clearFlags(ImageConverterFlag::Verbose);
    CORRADE_COMPARE(converter.flags(), ImageConverterFlag(4));
    CORRADE_COMPARE(converter._flags, ImageConverterFlag(4));
}

void AbstractImageConverterTest::setFlagsNotImplemented() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return {}; }
    } converter;

    CORRADE_COMPARE(converter.flags(), ImageConverterFlags{});
    converter.setFlags(ImageConverterFlag::Verbose);
    CORRADE_COMPARE(converter.flags(), ImageConverterFlag::Verbose);
    /* Should just work, no need to implement the function */
}

void AbstractImageConverterTest::thingNotSupported() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return {}; }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.extension();
    converter.mimeType();
    converter.convert(ImageView1D{PixelFormat::R8Unorm, 0, nullptr});
    converter.convert(ImageView2D{PixelFormat::R8Unorm, {}, nullptr});
    converter.convert(ImageView3D{PixelFormat::R8Unorm, {}, nullptr});
    converter.convert(CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 0, nullptr});
    converter.convert(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {}, nullptr});
    converter.convert(CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {}, nullptr});
    converter.convertToData(ImageView1D{PixelFormat::RGBA8Unorm, 0, nullptr});
    converter.convertToData(ImageView2D{PixelFormat::RGBA8Unorm, {}, nullptr});
    converter.convertToData(ImageView3D{PixelFormat::RGBA8Unorm, {}, nullptr});
    converter.convertToData({ImageView1D{PixelFormat::RGBA8Unorm, 0, nullptr}});
    converter.convertToData({ImageView2D{PixelFormat::RGBA8Unorm, {}, nullptr}});
    converter.convertToData({ImageView3D{PixelFormat::RGBA8Unorm, {}, nullptr}});
    converter.convertToData(CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 0, nullptr});
    converter.convertToData(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {}, nullptr});
    converter.convertToData(CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {}, nullptr});
    converter.convertToData({CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 0, nullptr}});
    converter.convertToData({CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {}, nullptr}});
    converter.convertToData({CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {}, nullptr}});
    converter.convertToFile(ImageView1D{PixelFormat::RGBA8Unorm, 0, nullptr}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    converter.convertToFile(ImageView2D{PixelFormat::RGBA8Unorm, {}, nullptr}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    converter.convertToFile(ImageView3D{PixelFormat::RGBA8Unorm, {}, nullptr}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    converter.convertToFile({ImageView1D{PixelFormat::RGBA8Unorm, 0, nullptr}}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    converter.convertToFile({ImageView2D{PixelFormat::RGBA8Unorm, {}, nullptr}}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    converter.convertToFile({ImageView3D{PixelFormat::RGBA8Unorm, {}, nullptr}}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    converter.convertToFile(CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 0, nullptr}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    converter.convertToFile(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {}, nullptr}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    converter.convertToFile(CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {}, nullptr}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    converter.convertToFile({CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 0, nullptr}}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    converter.convertToFile({CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {}, nullptr}}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    converter.convertToFile({CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {}, nullptr}}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out,
        "Trade::AbstractImageConverter::extension(): file conversion not supported\n"
        "Trade::AbstractImageConverter::mimeType(): file conversion not supported\n"
        "Trade::AbstractImageConverter::convert(): 1D image conversion not supported\n"
        "Trade::AbstractImageConverter::convert(): 2D image conversion not supported\n"
        "Trade::AbstractImageConverter::convert(): 3D image conversion not supported\n"
        "Trade::AbstractImageConverter::convert(): compressed 1D image conversion not supported\n"
        "Trade::AbstractImageConverter::convert(): compressed 2D image conversion not supported\n"
        "Trade::AbstractImageConverter::convert(): compressed 3D image conversion not supported\n"
        "Trade::AbstractImageConverter::convertToData(): 1D image conversion not supported\n"
        "Trade::AbstractImageConverter::convertToData(): 2D image conversion not supported\n"
        "Trade::AbstractImageConverter::convertToData(): 3D image conversion not supported\n"
        "Trade::AbstractImageConverter::convertToData(): multi-level 1D image conversion not supported\n"
        "Trade::AbstractImageConverter::convertToData(): multi-level 2D image conversion not supported\n"
        "Trade::AbstractImageConverter::convertToData(): multi-level 3D image conversion not supported\n"
        "Trade::AbstractImageConverter::convertToData(): compressed 1D image conversion not supported\n"
        "Trade::AbstractImageConverter::convertToData(): compressed 2D image conversion not supported\n"
        "Trade::AbstractImageConverter::convertToData(): compressed 3D image conversion not supported\n"
        "Trade::AbstractImageConverter::convertToData(): multi-level compressed 1D image conversion not supported\n"
        "Trade::AbstractImageConverter::convertToData(): multi-level compressed 2D image conversion not supported\n"
        "Trade::AbstractImageConverter::convertToData(): multi-level compressed 3D image conversion not supported\n"
        "Trade::AbstractImageConverter::convertToFile(): 1D image conversion not supported\n"
        "Trade::AbstractImageConverter::convertToFile(): 2D image conversion not supported\n"
        "Trade::AbstractImageConverter::convertToFile(): 3D image conversion not supported\n"
        "Trade::AbstractImageConverter::convertToFile(): multi-level 1D image conversion not supported\n"
        "Trade::AbstractImageConverter::convertToFile(): multi-level 2D image conversion not supported\n"
        "Trade::AbstractImageConverter::convertToFile(): multi-level 3D image conversion not supported\n"
        "Trade::AbstractImageConverter::convertToFile(): compressed 1D image conversion not supported\n"
        "Trade::AbstractImageConverter::convertToFile(): compressed 2D image conversion not supported\n"
        "Trade::AbstractImageConverter::convertToFile(): compressed 3D image conversion not supported\n"
        "Trade::AbstractImageConverter::convertToFile(): multi-level compressed 1D image conversion not supported\n"
        "Trade::AbstractImageConverter::convertToFile(): multi-level compressed 2D image conversion not supported\n"
        "Trade::AbstractImageConverter::convertToFile(): multi-level compressed 3D image conversion not supported\n");
}

void AbstractImageConverterTest::extensionMimeType() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed3DToData;
        }
        Containers::String doExtension() const override { return "yello"; }
        Containers::String doMimeType() const override { return "yel/low"; }
    } converter;

    CORRADE_COMPARE(converter.extension(), "yello");
    CORRADE_COMPARE(converter.mimeType(), "yel/low");
}

void AbstractImageConverterTest::extensionMimeTypeNotImplemented() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert1DToFile;
        }
    } converter;

    CORRADE_COMPARE(converter.extension(), "");
    CORRADE_COMPARE(converter.mimeType(), "");
}

void AbstractImageConverterTest::extensionMimeTypeCustomDeleter() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed1DToData;
        }
        Containers::String doExtension() const override {
            return Containers::String{"yello", 5, [](char*, std::size_t) {}};
        }
        Containers::String doMimeType() const override {
            return Containers::String{"yel/low", 7, [](char*, std::size_t) {}};
        }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.extension();
    converter.mimeType();
    CORRADE_COMPARE(out,
        "Trade::AbstractImageConverter::extension(): implementation is not allowed to use a custom String deleter\n"
        "Trade::AbstractImageConverter::mimeType(): implementation is not allowed to use a custom String deleter\n");
}

void AbstractImageConverterTest::convert1D() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert1D; }
        Containers::Optional<ImageData1D> doConvert(const ImageView1D& image) override {
            return ImageData1D{PixelFormat::RGBA8Unorm, image.size(), Containers::Array<char>{16}};
        }
    } converter;

    Containers::Optional<ImageData1D> actual = converter.convert(ImageView1D{PixelFormat::R8Unorm, 4, Containers::ArrayView<char>{nullptr, 24}});
    CORRADE_VERIFY(actual);
    CORRADE_VERIFY(!actual->isCompressed());
    CORRADE_COMPARE(actual->data().size(), 16);
    CORRADE_COMPARE(actual->size(), 4);
}

void AbstractImageConverterTest::convert2D() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert2D; }
        Containers::Optional<ImageData2D> doConvert(const ImageView2D& image) override {
            return ImageData2D{PixelFormat::RGBA8Unorm, image.size(), Containers::Array<char>{96}};
        }
    } converter;

    Containers::Optional<ImageData2D> actual = converter.convert(ImageView2D{PixelFormat::R8Unorm, {4, 6}, Containers::ArrayView<char>{nullptr, 24}});
    CORRADE_VERIFY(actual);
    CORRADE_VERIFY(!actual->isCompressed());
    CORRADE_COMPARE(actual->data().size(), 96);
    CORRADE_COMPARE(actual->size(), (Vector2i{4, 6}));
}

void AbstractImageConverterTest::convert3D() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert3D; }
        Containers::Optional<ImageData3D> doConvert(const ImageView3D& image) override {
            return ImageData3D{PixelFormat::RGBA8Unorm, image.size(), Containers::Array<char>{96}};
        }
    } converter;

    Containers::Optional<ImageData3D> actual = converter.convert(ImageView3D{PixelFormat::R8Unorm, {4, 6, 1}, Containers::ArrayView<char>{nullptr, 24}});
    CORRADE_VERIFY(actual);
    CORRADE_VERIFY(!actual->isCompressed());
    CORRADE_COMPARE(actual->data().size(), 96);
    CORRADE_COMPARE(actual->size(), (Vector3i{4, 6, 1}));
}

void AbstractImageConverterTest::convert1DFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert1D;
        }
        Containers::Optional<ImageData1D> doConvert(const ImageView1D&) override {
            return {};
        }
    } converter;

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convert(ImageView1D{PixelFormat::RGBA8Unorm, 1, {nullptr, 4}}));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convert2DFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert2D;
        }
        Containers::Optional<ImageData2D> doConvert(const ImageView2D&) override {
            return {};
        }
    } converter;

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convert(ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, {nullptr, 4}}));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convert3DFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert3D;
        }
        Containers::Optional<ImageData3D> doConvert(const ImageView3D&) override {
            return {};
        }
    } converter;

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convert(ImageView3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, {nullptr, 4}}));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convert1DNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert1D; }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convert(ImageView1D{PixelFormat::R8Unorm, 0, nullptr});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convert(): 1D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convert2DNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert2D; }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convert(ImageView2D{PixelFormat::R8Unorm, {}, nullptr});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convert(): 2D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convert3DNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert3D; }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convert(ImageView3D{PixelFormat::R8Unorm, {}, nullptr});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convert(): 3D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convert1DCustomDeleter() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert1D; }
        Containers::Optional<ImageData1D> doConvert(const ImageView1D&) override {
            return ImageData1D{PixelFormat::RGBA8Unorm, {}, Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}}};
        }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convert(ImageView1D{PixelFormat::R8Unorm, {}, nullptr});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convert(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImageConverterTest::convert2DCustomDeleter() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert2D; }
        Containers::Optional<ImageData2D> doConvert(const ImageView2D&) override {
            return ImageData2D{PixelFormat::RGBA8Unorm, {}, Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}}};
        }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convert(ImageView2D{PixelFormat::R8Unorm, {}, nullptr});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convert(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImageConverterTest::convert3DCustomDeleter() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert3D; }
        Containers::Optional<ImageData3D> doConvert(const ImageView3D&) override {
            return ImageData3D{PixelFormat::RGBA8Unorm, {}, Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}}};
        }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convert(ImageView3D{PixelFormat::R8Unorm, {}, nullptr});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convert(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImageConverterTest::convertCompressed1D() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed1D; }
        Containers::Optional<ImageData1D> doConvert(const CompressedImageView1D& image) override {
            return ImageData1D{image.format(), image.size(), Containers::Array<char>{64}};
        }
    } converter;

    Containers::Optional<ImageData1D> actual = converter.convert(CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 16, Containers::ArrayView<char>{nullptr, 128}});
    CORRADE_VERIFY(actual);
    CORRADE_VERIFY(actual->isCompressed());
    CORRADE_COMPARE(actual->data().size(), 64);
    CORRADE_COMPARE(actual->size(), 16);
}

void AbstractImageConverterTest::convertCompressed2D() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed2D; }
        Containers::Optional<ImageData2D> doConvert(const CompressedImageView2D& image) override {
            return ImageData2D{image.format(), image.size(), Containers::Array<char>{64}};
        }
    } converter;

    Containers::Optional<ImageData2D> actual = converter.convert(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 8}, Containers::ArrayView<char>{nullptr, 128}});
    CORRADE_VERIFY(actual);
    CORRADE_VERIFY(actual->isCompressed());
    CORRADE_COMPARE(actual->data().size(), 64);
    CORRADE_COMPARE(actual->size(), (Vector2i{16, 8}));
}

void AbstractImageConverterTest::convertCompressed3D() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed3D; }
        Containers::Optional<ImageData3D> doConvert(const CompressedImageView3D& image) override {
            return ImageData3D{image.format(), image.size(), Containers::Array<char>{64}};
        }
    } converter;

    Containers::Optional<ImageData3D> actual = converter.convert(CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 8, 1}, Containers::ArrayView<char>{nullptr, 128}});
    CORRADE_VERIFY(actual);
    CORRADE_VERIFY(actual->isCompressed());
    CORRADE_COMPARE(actual->data().size(), 64);
    CORRADE_COMPARE(actual->size(), (Vector3i{16, 8, 1}));
}

void AbstractImageConverterTest::convertCompressed1DFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed1D;
        }
        Containers::Optional<ImageData1D> doConvert(const CompressedImageView1D&) override {
            return {};
        }
    } converter;

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convert(CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 1, {nullptr, 4*4}}));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertCompressed2DFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed2D;
        }
        Containers::Optional<ImageData2D> doConvert(const CompressedImageView2D&) override {
            return {};
        }
    } converter;

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convert(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {1, 1}, {nullptr, 4*4}}));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertCompressed3DFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed3D;
        }
        Containers::Optional<ImageData3D> doConvert(const CompressedImageView3D&) override {
            return {};
        }
    } converter;

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convert(CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {1, 1, 1}, {nullptr, 4*4}}));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertCompressed1DNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed1D; }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convert(CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 0, nullptr});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convert(): compressed 1D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convertCompressed2DNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed2D; }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convert(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {}, nullptr});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convert(): compressed 2D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convertCompressed3DNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed3D; }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convert(CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {}, nullptr});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convert(): compressed 3D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convertCompressed1DCustomDeleter() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed1D; }
        Containers::Optional<ImageData1D> doConvert(const CompressedImageView1D&) override {
            return ImageData1D{CompressedPixelFormat::Bc1RGBAUnorm, {}, Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}}};
        }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convert(CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, {}, nullptr});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convert(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImageConverterTest::convertCompressed2DCustomDeleter() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed2D; }
        Containers::Optional<ImageData2D> doConvert(const CompressedImageView2D&) override {
            return ImageData2D{CompressedPixelFormat::Bc1RGBAUnorm, {}, Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}}};
        }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convert(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {}, nullptr});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convert(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImageConverterTest::convertCompressed3DCustomDeleter() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed3D; }
        Containers::Optional<ImageData3D> doConvert(const CompressedImageView3D&) override {
            return ImageData3D{CompressedPixelFormat::Bc1RGBAUnorm, {}, Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}}};
        }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convert(CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {}, nullptr});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convert(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImageConverterTest::convertImageData1D() {
    struct: Trade::AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert1D|ImageConverterFeature::ConvertCompressed1D; }

        Containers::Optional<ImageData1D> doConvert(const ImageView1D&) override {
            return ImageData1D{PixelFormat::R8Unorm, {}, Containers::array({'B'})};
        };

        Containers::Optional<ImageData1D> doConvert(const CompressedImageView1D&) override {
            return ImageData1D{PixelFormat::R8Unorm, {}, Containers::array({'C'})};
        };
    } converter;

    {
        /* Should get "B" when converting uncompressed */
        ImageData1D image{PixelFormat::RGBA8Unorm, {}, nullptr};
        Containers::Optional<ImageData1D> out = converter.convert(image);
        CORRADE_VERIFY(out);
        CORRADE_COMPARE_AS(out->data(),
            Containers::arrayView<char>({'B'}),
            TestSuite::Compare::Container);
    } {
        /* Should get "C" when converting compressed */
        ImageData1D image{CompressedPixelFormat::Bc1RGBUnorm, {}, nullptr};
        Containers::Optional<ImageData1D> out = converter.convert(image);
        CORRADE_VERIFY(out);
        CORRADE_COMPARE_AS(out->data(),
            Containers::arrayView<char>({'C'}),
            TestSuite::Compare::Container);
    }
}

void AbstractImageConverterTest::convertImageData2D() {
    struct: Trade::AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert2D|ImageConverterFeature::ConvertCompressed2D; }

        Containers::Optional<ImageData2D> doConvert(const ImageView2D&) override {
            return ImageData2D{PixelFormat::R8Unorm, {}, Containers::array({'B'})};
        };

        Containers::Optional<ImageData2D> doConvert(const CompressedImageView2D&) override {
            return ImageData2D{PixelFormat::R8Unorm, {}, Containers::array({'C'})};
        };
    } converter;

    {
        /* Should get "B" when converting uncompressed */
        ImageData2D image{PixelFormat::RGBA8Unorm, {}, nullptr};
        Containers::Optional<ImageData2D> out = converter.convert(image);
        CORRADE_VERIFY(out);
        CORRADE_COMPARE_AS(out->data(),
            Containers::arrayView<char>({'B'}),
            TestSuite::Compare::Container);
    } {
        /* Should get "C" when converting compressed */
        ImageData2D image{CompressedPixelFormat::Bc1RGBUnorm, {}, nullptr};
        Containers::Optional<ImageData2D> out = converter.convert(image);
        CORRADE_VERIFY(out);
        CORRADE_COMPARE_AS(out->data(),
            Containers::arrayView<char>({'C'}),
            TestSuite::Compare::Container);
    }
}

void AbstractImageConverterTest::convertImageData3D() {
    struct: Trade::AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert3D|ImageConverterFeature::ConvertCompressed3D; }

        Containers::Optional<ImageData3D> doConvert(const ImageView3D&) override {
            return ImageData3D{PixelFormat::R8Unorm, {}, Containers::array({'B'})};
        };

        Containers::Optional<ImageData3D> doConvert(const CompressedImageView3D&) override {
            return ImageData3D{PixelFormat::R8Unorm, {}, Containers::array({'C'})};
        };
    } converter;

    {
        /* Should get "B" when converting uncompressed */
        ImageData3D image{PixelFormat::RGBA8Unorm, {}, nullptr};
        Containers::Optional<ImageData3D> out = converter.convert(image);
        CORRADE_VERIFY(out);
        CORRADE_COMPARE_AS(out->data(),
            Containers::arrayView<char>({'B'}),
            TestSuite::Compare::Container);
    } {
        /* Should get "C" when converting compressed */
        ImageData3D image{CompressedPixelFormat::Bc1RGBUnorm, {}, nullptr};
        Containers::Optional<ImageData3D> out = converter.convert(image);
        CORRADE_VERIFY(out);
        CORRADE_COMPARE_AS(out->data(),
            Containers::arrayView<char>({'C'}),
            TestSuite::Compare::Container);
    }
}

void AbstractImageConverterTest::convert1DToData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert1DToData; }
        Containers::Optional<Containers::Array<char>> doConvertToData(const ImageView1D& image) override {
            return Containers::Array<char>{nullptr, std::size_t(image.size().product())};
        }
    } converter;

    const char data[16]{};
    Containers::Optional<Containers::Array<char>> actual = converter.convertToData(ImageView1D{PixelFormat::RGBA8Unorm, 4, data});
    CORRADE_VERIFY(actual);
    CORRADE_COMPARE(actual->size(), 4);
}

void AbstractImageConverterTest::convert2DToData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert2DToData; }
        Containers::Optional<Containers::Array<char>> doConvertToData(const ImageView2D& image) override {
            return Containers::Array<char>{nullptr, std::size_t(image.size().product())};
        }
    } converter;

    const char data[96]{};
    Containers::Optional<Containers::Array<char>> actual = converter.convertToData(ImageView2D{PixelFormat::RGBA8Unorm, {4, 6}, data});
    CORRADE_VERIFY(actual);
    CORRADE_COMPARE(actual->size(), 24);
}

void AbstractImageConverterTest::convert3DToData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert3DToData; }
        Containers::Optional<Containers::Array<char>> doConvertToData(const ImageView3D& image) override {
            return Containers::Array<char>{nullptr, std::size_t(image.size().product())};
        }
    } converter;

    const char data[192]{};
    Containers::Optional<Containers::Array<char>> actual = converter.convertToData(ImageView3D{PixelFormat::RGBA8Unorm, {4, 6, 2}, data});
    CORRADE_VERIFY(actual);
    CORRADE_COMPARE(actual->size(), 48);
}

void AbstractImageConverterTest::convert1DToDataFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert1DToData;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(const ImageView1D&) override {
            return {};
        }
    } converter;

    const char imageData[4]{};

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToData(ImageView1D{PixelFormat::RGBA8Unorm, 1, imageData}));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convert2DToDataFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert2DToData;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(const ImageView2D&) override {
            return {};
        }
    } converter;

    const char imageData[4]{};

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToData(ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, imageData}));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convert3DToDataFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert3DToData;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(const ImageView3D&) override {
            return {};
        }
    } converter;

    const char imageData[4]{};

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToData(ImageView3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, imageData}));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convert1DToDataInvalidImage() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert1DToData; }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convertToData(ImageView1D{PixelFormat::RGBA8Unorm, 0, nullptr});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): can't convert image with a zero size: Vector(0)\n");
}

void AbstractImageConverterTest::convert2DToDataZeroSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert2DToData; }
    } converter;

    const char data[16]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData(ImageView2D{PixelFormat::RGBA8Unorm, {4, 0}, data});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): can't convert image with a zero size: Vector(4, 0)\n");
}

void AbstractImageConverterTest::convert2DToDataNullptr() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert2DToData; }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convertToData(ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, {nullptr, 4}});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): can't convert image with a nullptr view\n");
}

void AbstractImageConverterTest::convert3DToDataInvalidImage() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert3DToData; }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convertToData(ImageView3D{PixelFormat::RGBA8Unorm, {}, nullptr});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): can't convert image with a zero size: Vector(0, 0, 0)\n");
}

void AbstractImageConverterTest::convert1DToDataNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert1DToData; }
    } converter;

    const char data[4]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData(ImageView1D{PixelFormat::RGBA8Unorm, 1, data});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): 1D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convert2DToDataNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert2DToData; }
    } converter;

    const char data[4]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData(ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, data});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): 2D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convert3DToDataNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert3DToData; }
    } converter;

    const char data[4]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData(ImageView3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, data});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): 3D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convert1DToDataCustomDeleter() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert1DToData; }
        Containers::Optional<Containers::Array<char>> doConvertToData(const ImageView1D&) override {
            return Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}};
        }
    } converter;

    const char data[4]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData(ImageView1D{PixelFormat::RGBA8Unorm, 1, data});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImageConverterTest::convert2DToDataCustomDeleter() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert2DToData; }
        Containers::Optional<Containers::Array<char>> doConvertToData(const ImageView2D&) override {
            return Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}};
        }
    } converter;

    const char data[4]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData(ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, data});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImageConverterTest::convert3DToDataCustomDeleter() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert3DToData; }
        Containers::Optional<Containers::Array<char>> doConvertToData(const ImageView3D&) override {
            return Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}};
        }
    } converter;

    const char data[4]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData(ImageView3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, data});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImageConverterTest::convertCompressed1DToData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed1DToData; }
        Containers::Optional<Containers::Array<char>> doConvertToData(const CompressedImageView1D& image) override {
            return Containers::Array<char>{nullptr, std::size_t(image.size().product())};
        }
    } converter;

    const char data[32]{};
    Containers::Optional<Containers::Array<char>> actual = converter.convertToData(CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 16, data});
    CORRADE_VERIFY(actual);
    CORRADE_COMPARE(actual->size(), 16);
}

void AbstractImageConverterTest::convertCompressed2DToData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed2DToData; }
        Containers::Optional<Containers::Array<char>> doConvertToData(const CompressedImageView2D& image) override {
            return Containers::Array<char>{nullptr, std::size_t(image.size().product())};
        }
    } converter;

    const char data[64]{};
    Containers::Optional<Containers::Array<char>> actual = converter.convertToData(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 8}, data});
    CORRADE_VERIFY(actual);
    CORRADE_COMPARE(actual->size(), 128);
}

void AbstractImageConverterTest::convertCompressed3DToData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed3DToData; }
        Containers::Optional<Containers::Array<char>> doConvertToData(const CompressedImageView3D& image) override {
            return Containers::Array<char>{nullptr, std::size_t(image.size().product())};
        }
    } converter;

    const char data[128]{};
    Containers::Optional<Containers::Array<char>> actual = converter.convertToData(CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 8, 2}, data});
    CORRADE_VERIFY(actual);
    CORRADE_COMPARE(actual->size(), 256);
}

void AbstractImageConverterTest::convertCompressed1DToDataFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed1DToData;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(const CompressedImageView1D&) override {
            return {};
        }
    } converter;

    const char imageData[4*4]{};

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToData(CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 1, imageData}));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertCompressed2DToDataFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed2DToData;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(const CompressedImageView2D&) override {
            return {};
        }
    } converter;

    const char imageData[4*4]{};

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToData(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {1, 1}, imageData}));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertCompressed3DToDataFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed3DToData;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(const CompressedImageView3D&) override {
            return {};
        }
    } converter;

    const char imageData[4*4]{};

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToData(CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {1, 1, 1}, imageData}));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertCompressed1DToDataInvalidImage() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed1DToData; }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convertToData(CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 0, nullptr});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): can't convert image with a zero size: Vector(0)\n");
}

void AbstractImageConverterTest::convertCompressed2DToDataInvalidImage() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed2DToData; }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convertToData(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {}, nullptr});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): can't convert image with a zero size: Vector(0, 0)\n");
}

void AbstractImageConverterTest::convertCompressed3DToDataInvalidImage() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed3DToData; }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convertToData(CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {}, nullptr});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): can't convert image with a zero size: Vector(0, 0, 0)\n");
}

void AbstractImageConverterTest::convertCompressed1DToDataNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed1DToData; }
    } converter;

    const char data[8]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData(CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 4, data});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): compressed 1D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convertCompressed2DToDataNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed2DToData; }
    } converter;

    const char data[8]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4}, data});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): compressed 2D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convertCompressed3DToDataNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed3DToData; }
    } converter;

    const char data[8]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData(CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4, 1}, data});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): compressed 3D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convertCompressed1DToDataCustomDeleter() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed1DToData; }
        Containers::Optional<Containers::Array<char>> doConvertToData(const CompressedImageView1D&) override {
            return Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}};
        }
    } converter;

    const char data[8]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData(CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 4, data});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImageConverterTest::convertCompressed2DToDataCustomDeleter() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed2DToData; }
        Containers::Optional<Containers::Array<char>> doConvertToData(const CompressedImageView2D&) override {
            return Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}};
        }
    } converter;

    const char data[8]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4}, data});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImageConverterTest::convertCompressed3DToDataCustomDeleter() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed3DToData; }
        Containers::Optional<Containers::Array<char>> doConvertToData(const CompressedImageView3D&) override {
            return Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}};
        }
    } converter;

    const char data[8]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData(CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4, 1}, data});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter\n");
}

/* Used by convertImageDataToData() and convertImageDataToFile() */
class ImageData1DConverter: public Trade::AbstractImageConverter {
    private:
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert1DToData|ImageConverterFeature::ConvertCompressed1DToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const ImageView1D&) override {
            return Containers::array({'B'});
        };

        Containers::Optional<Containers::Array<char>> doConvertToData(const CompressedImageView1D&) override {
            return Containers::array({'C'});
        };
};
class ImageData2DConverter: public Trade::AbstractImageConverter {
    private:
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert2DToData|ImageConverterFeature::ConvertCompressed2DToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const ImageView2D&) override {
            return Containers::array({'B'});
        };

        Containers::Optional<Containers::Array<char>> doConvertToData(const CompressedImageView2D&) override {
            return Containers::array({'C'});
        };
};
class ImageData3DConverter: public Trade::AbstractImageConverter {
    private:
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert3DToData|ImageConverterFeature::ConvertCompressed3DToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const ImageView3D&) override {
            return Containers::array({'B'});
        };

        Containers::Optional<Containers::Array<char>> doConvertToData(const CompressedImageView3D&) override {
            return Containers::array({'C'});
        };
};

void AbstractImageConverterTest::convertImageData1DToData() {
    ImageData1DConverter converter;

    /* Should get "B" when converting uncompressed */
    {
        Containers::Optional<Containers::Array<char>> out = converter.convertToData(ImageData1D{PixelFormat::RGBA8Unorm, 1, Containers::Array<char>{4}});
        CORRADE_VERIFY(out);
        CORRADE_COMPARE_AS(*out,
            Containers::arrayView({'B'}),
            TestSuite::Compare::Container);
    }

    /* Should get "C" when converting compressed */
    {
        Containers::Optional<Containers::Array<char>> out = converter.convertToData(ImageData1D{CompressedPixelFormat::Bc1RGBUnorm, 4, Containers::Array<char>{8}});
        CORRADE_VERIFY(out);
        CORRADE_COMPARE_AS(*out,
            Containers::arrayView({'C'}),
            TestSuite::Compare::Container);
    }
}

void AbstractImageConverterTest::convertImageData2DToData() {
    ImageData2DConverter converter;

    /* Should get "B" when converting uncompressed */
    {
        Containers::Optional<Containers::Array<char>> out = converter.convertToData(ImageData2D{PixelFormat::RGBA8Unorm, {1, 1}, Containers::Array<char>{4}});
        CORRADE_VERIFY(out);
        CORRADE_COMPARE_AS(*out,
            Containers::arrayView({'B'}),
            TestSuite::Compare::Container);
    }

    /* Should get "C" when converting compressed */
    {
        Containers::Optional<Containers::Array<char>> out = converter.convertToData(ImageData2D{CompressedPixelFormat::Bc1RGBUnorm, {4, 4}, Containers::Array<char>{NoInit, 8}});
        CORRADE_VERIFY(out);
        CORRADE_COMPARE_AS(*out,
            Containers::arrayView({'C'}),
            TestSuite::Compare::Container);
    }
}

void AbstractImageConverterTest::convertImageData3DToData() {
    ImageData3DConverter converter;

    /* Should get "B" when converting uncompressed */
    {
        Containers::Optional<Containers::Array<char>> out = converter.convertToData(ImageData3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, Containers::Array<char>{4}});
        CORRADE_VERIFY(out);
        CORRADE_COMPARE_AS(*out,
            Containers::arrayView({'B'}),
            TestSuite::Compare::Container);
    }

    /* Should get "C" when converting compressed */
    {
        Containers::Optional<Containers::Array<char>> out = converter.convertToData(ImageData3D{CompressedPixelFormat::Bc1RGBUnorm, {4, 4, 1}, Containers::Array<char>{NoInit, 8}});
        CORRADE_VERIFY(out);
        CORRADE_COMPARE_AS(*out,
            Containers::arrayView({'C'}),
            TestSuite::Compare::Container);
    }
}

void AbstractImageConverterTest::convertLevels1DToData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert1DToData|
                   ImageConverterFeature::Levels;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const ImageView1D> imageLevels) override {
            return Containers::Array<char>{nullptr, std::size_t(imageLevels[0].size().product()*imageLevels.size())};
        }
    } converter;

    const char data[96]{};
    Containers::Optional<Containers::Array<char>> actual = converter.convertToData({
        /* Arbitrary dimensions should be fine */
        ImageView1D{PixelFormat::RGBA8Unorm, 4, data},
        ImageView1D{PixelFormat::RGBA8Unorm, 2, data},
        ImageView1D{PixelFormat::RGBA8Unorm, 3, data},
    });
    CORRADE_VERIFY(actual);
    CORRADE_COMPARE(actual->size(), 4*3);
}

void AbstractImageConverterTest::convertLevels2DToData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert2DToData|
                   ImageConverterFeature::Levels;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const ImageView2D> imageLevels) override {
            return Containers::Array<char>{nullptr, std::size_t(imageLevels[0].size().product()*imageLevels.size())};
        }
    } converter;

    const char data[96]{};
    Containers::Optional<Containers::Array<char>> actual = converter.convertToData({
        /* Arbitrary dimensions should be fine */
        ImageView2D{PixelFormat::RGBA8Unorm, {4, 6}, data},
        ImageView2D{PixelFormat::RGBA8Unorm, {1, 3}, data},
        ImageView2D{PixelFormat::RGBA8Unorm, {4, 2}, data}
    });
    CORRADE_VERIFY(actual);
    CORRADE_COMPARE(actual->size(), 24*3);
}

void AbstractImageConverterTest::convertLevels3DToData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert3DToData|
                   ImageConverterFeature::Levels;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const ImageView3D> imageLevels) override {
            return Containers::Array<char>{nullptr, std::size_t(imageLevels[0].size().product()*imageLevels.size())};
        }
    } converter;

    const char data[192]{};
    Containers::Optional<Containers::Array<char>> actual = converter.convertToData({
        /* Arbitrary dimensions should be fine */
        ImageView3D{PixelFormat::RGBA8Unorm, {4, 6, 2}, data},
        ImageView3D{PixelFormat::RGBA8Unorm, {2, 3, 5}, data},
        ImageView3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, data}
    });
    CORRADE_VERIFY(actual);
    CORRADE_COMPARE(actual->size(), 48*3);
}

void AbstractImageConverterTest::convertLevels1DToDataFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert1DToData|
                   ImageConverterFeature::Levels;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const ImageView1D>) override {
            return {};
        }
    } converter;

    const char imageData[4]{};

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToData({ImageView1D{PixelFormat::RGBA8Unorm, 1, imageData}}));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertLevels2DToDataFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert2DToData|
                   ImageConverterFeature::Levels;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const ImageView2D>) override {
            return {};
        }
    } converter;

    const char imageData[4]{};

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToData({ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, imageData}}));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertLevels3DToDataFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert3DToData|
                   ImageConverterFeature::Levels;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const ImageView3D>) override {
            return {};
        }
    } converter;

    const char imageData[4]{};

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToData({ImageView3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, imageData}}));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertLevels1DToDataInvalidImage() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert1DToData|
                   ImageConverterFeature::Levels;
        }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convertToData(std::initializer_list<ImageView1D>{});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): at least one image has to be specified\n");
}

void AbstractImageConverterTest::convertLevels2DToDataNoLevels() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert2DToData|
                   ImageConverterFeature::Levels;
        }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convertToData(std::initializer_list<ImageView2D>{});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): at least one image has to be specified\n");
}

void AbstractImageConverterTest::convertLevels2DToDataZeroSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert2DToData|
                   ImageConverterFeature::Levels;
        }
    } converter;

    const char data[16]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData({
        ImageView2D{PixelFormat::RGBA8Unorm, {2, 2}, data},
        ImageView2D{PixelFormat::RGBA8Unorm, {4, 0}, data}
    });
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): can't convert image 1 with a zero size: Vector(4, 0)\n");
}

void AbstractImageConverterTest::convertLevels2DToDataNullptr() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert2DToData|
                   ImageConverterFeature::Levels;
        }
    } converter;

    const char data[16]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData({
        ImageView2D{PixelFormat::RGBA8Unorm, {2, 2}, data},
        ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, {nullptr, 4}}
    });
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): can't convert image 1 with a nullptr view\n");
}

void AbstractImageConverterTest::convertLevels2DToDataInconsistentFormat() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert2DToData|
                   ImageConverterFeature::Levels;
        }
    } converter;

    const char data[16]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData({
        ImageView2D{PixelFormat::RGBA8Unorm, {2, 2}, data},
        ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, data},
        ImageView2D{PixelFormat::RGBA8Srgb, {4, 1}, data}
    });
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): levels don't have the same format, expected PixelFormat::RGBA8Unorm but got PixelFormat::RGBA8Srgb for image 2\n");
}

void AbstractImageConverterTest::convertLevels2DToDataInconsistentFormatExtra() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert2DToData|
                   ImageConverterFeature::Levels;
        }
    } converter;

    const char data[16]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData({
        ImageView2D{PixelStorage{}, 252, 1037, 4, {2, 2}, data},
        ImageView2D{PixelStorage{}, 252, 1037, 4, {1, 1}, data},
        ImageView2D{PixelStorage{}, 252, 4467, 4, {4, 1}, data}
    });
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): levels don't have the same extra format field, expected 1037 but got 4467 for image 2\n");
}

void AbstractImageConverterTest::convertLevels2DToDataInconsistentFlags() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert2DToData|
                   ImageConverterFeature::Levels;
        }
    } converter;

    const char data[16]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData({
        ImageView2D{PixelFormat::RGBA8Unorm, {2, 2}, data, ImageFlag2D::Array},
        ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, data, ImageFlag2D::Array},
        ImageView2D{PixelFormat::RGBA8Unorm, {4, 1}, data}
    });
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): levels don't have the same flags, expected ImageFlag2D::Array but got ImageFlags2D{} for image 2\n");
}

void AbstractImageConverterTest::convertLevels3DToDataInvalidImage() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert3DToData|
                   ImageConverterFeature::Levels;
        }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convertToData(std::initializer_list<ImageView3D>{});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): at least one image has to be specified\n");
}

void AbstractImageConverterTest::convertLevels1DToDataNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert1DToData|
                   ImageConverterFeature::Levels;
        }
    } converter;

    const char data[4]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData({ImageView1D{PixelFormat::RGBA8Unorm, 1, data}});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): multi-level 1D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convertLevels2DToDataNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert2DToData|
                   ImageConverterFeature::Levels;
        }
    } converter;

    const char data[4]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData({ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, data}});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): multi-level 2D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convertLevels3DToDataNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert3DToData|
                   ImageConverterFeature::Levels;
        }
    } converter;

    const char data[4]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData({ImageView3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, data}});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): multi-level 3D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convertLevels1DToDataCustomDeleter() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert1DToData|
                   ImageConverterFeature::Levels;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const ImageView1D>) override {
            return Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}};
        }
    } converter;

    const char data[4]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData({ImageView1D{PixelFormat::RGBA8Unorm, 1, data}});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImageConverterTest::convertLevels2DToDataCustomDeleter() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert2DToData|
                   ImageConverterFeature::Levels;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const ImageView2D>) override {
            return Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}};
        }
    } converter;

    const char data[4]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData({ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, data}});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImageConverterTest::convertLevels3DToDataCustomDeleter() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert3DToData|
                   ImageConverterFeature::Levels;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const ImageView3D>) override {
            return Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}};
        }
    } converter;

    const char data[4]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData({ImageView3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, data}});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImageConverterTest::convertCompressedLevels1DToData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed1DToData|
                   ImageConverterFeature::Levels;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const CompressedImageView1D> imageLevels) override {
            return Containers::Array<char>{nullptr, std::size_t(imageLevels[0].size().product()*imageLevels.size())};
        }
    } converter;

    const char data[32]{};
    Containers::Optional<Containers::Array<char>> actual = converter.convertToData({
        /* Arbitrary dimensions should be fine */
        CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 16, data},
        CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 4, data},
        CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 12, data}
    });
    CORRADE_VERIFY(actual);
    CORRADE_COMPARE(actual->size(), 16*3);
}

void AbstractImageConverterTest::convertCompressedLevels2DToData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed2DToData|
                   ImageConverterFeature::Levels;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const CompressedImageView2D> imageLevels) override {
            return Containers::Array<char>{nullptr, std::size_t(imageLevels[0].size().product()*imageLevels.size())};
        }
    } converter;

    const char data[64]{};
    Containers::Optional<Containers::Array<char>> actual = converter.convertToData({
        /* Arbitrary dimensions should be fine */
        CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 8}, data},
        CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 12}, data},
        CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4}, data}
    });
    CORRADE_VERIFY(actual);
    CORRADE_COMPARE(actual->size(), 128*3);
}

void AbstractImageConverterTest::convertCompressedLevels3DToData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed3DToData|
                   ImageConverterFeature::Levels;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const CompressedImageView3D> imageLevels) override {
            return Containers::Array<char>{nullptr, std::size_t(imageLevels[0].size().product()*imageLevels.size())};
        }
    } converter;

    const char data[128]{};
    Containers::Optional<Containers::Array<char>> actual = converter.convertToData({
        /* Arbitrary dimensions should be fine */
        CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 8, 2}, data},
        CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 16, 1}, data},
        CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 4, 1}, data}
    });
    CORRADE_VERIFY(actual);
    CORRADE_COMPARE(actual->size(), 256*3);
}

void AbstractImageConverterTest::convertCompressedLevels1DToDataFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed1DToData|
                   ImageConverterFeature::Levels;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const CompressedImageView1D>) override {
            return {};
        }
    } converter;

    const char imageData[4*4]{};

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToData({CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 1, imageData}}));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertCompressedLevels2DToDataFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed2DToData|
                   ImageConverterFeature::Levels;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const CompressedImageView2D>) override {
            return {};
        }
    } converter;

    const char imageData[4*4]{};

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToData({CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {1, 1}, imageData}}));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertCompressedLevels3DToDataFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed3DToData|
                   ImageConverterFeature::Levels;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const CompressedImageView3D>) override {
            return {};
        }
    } converter;

    const char imageData[4*4]{};

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToData({CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {1, 1, 1}, imageData}}));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertCompressedLevels1DToDataInvalidImage() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed1DToData|
                   ImageConverterFeature::Levels;
        }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convertToData(std::initializer_list<CompressedImageView1D>{});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): at least one image has to be specified\n");
}

void AbstractImageConverterTest::convertCompressedLevels2DToDataNoLevels() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed2DToData|
                   ImageConverterFeature::Levels;
        }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convertToData(std::initializer_list<CompressedImageView2D>{});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): at least one image has to be specified\n");
}

void AbstractImageConverterTest::convertCompressedLevels2DToDataZeroSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed2DToData|
                   ImageConverterFeature::Levels;
        }
    } converter;

    const char data[16]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData({
        CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 8}, data},
        CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 0}, data},
    });
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): can't convert image 1 with a zero size: Vector(4, 0)\n");
}

void AbstractImageConverterTest::convertCompressedLevels2DToDataNullptr() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed2DToData|
                   ImageConverterFeature::Levels;
        }
    } converter;

    const char data[16]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData({
        CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 8}, data},
        CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {1, 1}, {nullptr, 8}},
    });
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): can't convert image 1 with a nullptr view\n");
}

void AbstractImageConverterTest::convertCompressedLevels2DToDataInconsistentFormat() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed2DToData|
                   ImageConverterFeature::Levels;
        }
    } converter;

    const char data[16]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData({
        CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {8, 4}, data},
        CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4}, data},
        CompressedImageView2D{CompressedPixelFormat::Bc1RGBASrgb, {4, 4}, data},
    });
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): levels don't have the same format, expected CompressedPixelFormat::Bc1RGBAUnorm but got CompressedPixelFormat::Bc1RGBASrgb for image 2\n");
}

void AbstractImageConverterTest::convertCompressedLevels2DToDataInconsistentFlags() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed2DToData|
                   ImageConverterFeature::Levels;
        }
    } converter;

    const char data[16]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData({
        CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {8, 4}, data, ImageFlag2D::Array},
        CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4}, data, ImageFlag2D::Array},
        CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4}, data},
    });
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): levels don't have the same flags, expected ImageFlag2D::Array but got ImageFlags2D{} for image 2\n");
}

void AbstractImageConverterTest::convertCompressedLevels3DToDataInvalidImage() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed3DToData|
                   ImageConverterFeature::Levels;
        }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convertToData(std::initializer_list<CompressedImageView3D>{});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): at least one image has to be specified\n");
}

void AbstractImageConverterTest::convertCompressedLevels1DToDataNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed1DToData|
                   ImageConverterFeature::Levels;
        }
    } converter;

    const char data[8]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData({CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 4, data}});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): multi-level compressed 1D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convertCompressedLevels2DToDataNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed2DToData|
                   ImageConverterFeature::Levels;
        }
    } converter;

    const char data[8]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData({CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4}, data}});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): multi-level compressed 2D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convertCompressedLevels3DToDataNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed3DToData|
                   ImageConverterFeature::Levels;
        }
    } converter;

    const char data[8]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData({CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4, 1}, data}});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): multi-level compressed 3D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convertCompressedLevels1DToDataCustomDeleter() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed1DToData|
                   ImageConverterFeature::Levels;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const CompressedImageView1D>) override {
            return Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}};
        }
    } converter;

    const char data[8]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData({CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 4, data}});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImageConverterTest::convertCompressedLevels2DToDataCustomDeleter() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed2DToData|
                   ImageConverterFeature::Levels;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const CompressedImageView2D>) override {
            return Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}};
        }
    } converter;

    const char data[8]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData({CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4}, data}});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImageConverterTest::convertCompressedLevels3DToDataCustomDeleter() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed3DToData|
                   ImageConverterFeature::Levels;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const CompressedImageView3D>) override {
            return Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}};
        }
    } converter;

    const char data[8]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToData({CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4, 1}, data}});
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToData(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImageConverterTest::convert1DToDataThroughLevels() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert1DToData|
                   ImageConverterFeature::Levels;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const ImageView1D> imageLevels) override {
            return Containers::Array<char>{nullptr, std::size_t(imageLevels[0].size().product()*imageLevels.size())};
        }
    } converter;

    const char data[16]{};
    Containers::Optional<Containers::Array<char>> actual = converter.convertToData(ImageView1D{PixelFormat::RGBA8Unorm, 4, data});
    CORRADE_VERIFY(actual);
    CORRADE_COMPARE(actual->size(), 4);
}

void AbstractImageConverterTest::convert2DToDataThroughLevels() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert2DToData|
                   ImageConverterFeature::Levels;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const ImageView2D> imageLevels) override {
            return Containers::Array<char>{nullptr, std::size_t(imageLevels[0].size().product()*imageLevels.size())};
        }
    } converter;

    const char data[96]{};
    Containers::Optional<Containers::Array<char>> actual = converter.convertToData(ImageView2D{PixelFormat::RGBA8Unorm, {4, 6}, data});
    CORRADE_VERIFY(actual);
    CORRADE_COMPARE(actual->size(), 24);
}

void AbstractImageConverterTest::convert3DToDataThroughLevels() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert3DToData|
                   ImageConverterFeature::Levels;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const ImageView3D> imageLevels) override {
            return Containers::Array<char>{nullptr, std::size_t(imageLevels[0].size().product()*imageLevels.size())};
        }
    } converter;

    const char data[192]{};
    Containers::Optional<Containers::Array<char>> actual = converter.convertToData(ImageView3D{PixelFormat::RGBA8Unorm, {4, 6, 2}, data});
    CORRADE_VERIFY(actual);
    CORRADE_COMPARE(actual->size(), 48);
}

void AbstractImageConverterTest::convertCompressed1DToDataThroughLevels() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed1DToData|
                   ImageConverterFeature::Levels;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const CompressedImageView1D> imageLevels) override {
            return Containers::Array<char>{nullptr, std::size_t(imageLevels[0].size().product()*imageLevels.size())};
        }
    } converter;

    const char data[32]{};
    Containers::Optional<Containers::Array<char>> actual = converter.convertToData(CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 16, data});
    CORRADE_VERIFY(actual);
    CORRADE_COMPARE(actual->size(), 16);
}

void AbstractImageConverterTest::convertCompressed2DToDataThroughLevels() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed2DToData|
                   ImageConverterFeature::Levels;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const CompressedImageView2D> imageLevels) override {
            return Containers::Array<char>{nullptr, std::size_t(imageLevels[0].size().product()*imageLevels.size())};
        }
    } converter;

    const char data[64]{};
    Containers::Optional<Containers::Array<char>> actual = converter.convertToData(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 8}, data});
    CORRADE_VERIFY(actual);
    CORRADE_COMPARE(actual->size(), 128);
}

void AbstractImageConverterTest::convertCompressed3DToDataThroughLevels() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed3DToData|
                   ImageConverterFeature::Levels;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const CompressedImageView3D> imageLevels) override {
            return Containers::Array<char>{nullptr, std::size_t(imageLevels[0].size().product()*imageLevels.size())};
        }
    } converter;

    const char data[128]{};
    Containers::Optional<Containers::Array<char>> actual = converter.convertToData(CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {16, 8, 2}, data});
    CORRADE_VERIFY(actual);
    CORRADE_COMPARE(actual->size(), 256);
}

void AbstractImageConverterTest::convert1DToFile() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert1DToFile; }
        bool doConvertToFile(const ImageView1D& image, Containers::StringView filename) override {
            return Utility::Path::write(filename, Containers::arrayView(
                {char(image.size()[0])}));
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    CORRADE_VERIFY(converter.convertToFile(ImageView1D{PixelFormat::RGBA8Unorm, 0x0f, Containers::Array<char>{0x0f*4}}, filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convert2DToFile() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert2DToFile; }
        bool doConvertToFile(const ImageView2D& image, Containers::StringView filename) override {
            return Utility::Path::write(filename, Containers::arrayView(
                {char(image.size().x()), char(image.size().y())}));
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    CORRADE_VERIFY(converter.convertToFile(ImageView2D{PixelFormat::RGBA8Unorm, {0x0f, 0x0d}, Containers::Array<char>{0x0f*0x0d*4}}, filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f\x0d", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convert3DToFile() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert3DToFile; }
        bool doConvertToFile(const ImageView3D& image, Containers::StringView filename) override {
            return Utility::Path::write(filename, Containers::arrayView(
                {char(image.size().x()), char(image.size().y()), char(image.size().z())}));
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    CORRADE_VERIFY(converter.convertToFile(ImageView3D{PixelFormat::RGBA8Unorm, {0x0f, 0x0d, 0x02}, Containers::Array<char>{0x0f*0x0d*0x02*4}}, filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f\x0d\x02", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convert1DToFileFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert1DToFile;
        }
        bool doConvertToFile(const ImageView1D&, Containers::StringView) override {
            return {};
        }
    } converter;

    const char imageData[4]{};

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile(ImageView1D{PixelFormat::RGBA8Unorm, 1, imageData}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out")));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convert2DToFileFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert2DToFile;
        }
        bool doConvertToFile(const ImageView2D&, Containers::StringView) override {
            return {};
        }
    } converter;

    const char imageData[4]{};

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile(ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, imageData}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out")));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convert3DToFileFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert3DToFile;
        }
        bool doConvertToFile(const ImageView3D&, Containers::StringView) override {
            return {};
        }
    } converter;

    const char imageData[4]{};

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile(ImageView3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, imageData}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out")));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convert1DToFileThroughData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert1DToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const ImageView1D& image) override {
            return Containers::array({char(image.size()[0])});
        };
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    /* doConvertToFile() should call doConvertToData() */
    CORRADE_VERIFY(converter.convertToFile(ImageView1D(PixelFormat::RGBA8Unorm, 0x0f, Containers::Array<char>{0x0f*4}), filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convert2DToFileThroughData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert2DToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const ImageView2D& image) override {
            return Containers::array({char(image.size().x()), char(image.size().y())});
        };
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    /* doConvertToFile() should call doConvertToData() */
    CORRADE_VERIFY(converter.convertToFile(ImageView2D(PixelFormat::RGBA8Unorm, {0x0f, 0x0d}, Containers::Array<char>{0x0f*0x0d*4}), filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f\x0d", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convert3DToFileThroughData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert3DToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const ImageView3D& image) override {
            return Containers::array({char(image.size().x()), char(image.size().y()), char(image.size().z())});
        };
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    /* doConvertToFile() should call doConvertToData() */
    CORRADE_VERIFY(converter.convertToFile(ImageView3D(PixelFormat::RGBA8Unorm, {0x0f, 0x0d, 0x02}, Containers::Array<char>{0x0f*0x0d*0x02*4}), filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f\x0d\x02", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convert1DToFileThroughDataFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert1DToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const ImageView1D&) override {
            return {};
        };
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    const char data[4]{};

    /* Function should fail, no file should get written and no error output
       should be printed (the base implementation assumes the plugin does it) */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile(ImageView1D{PixelFormat::RGBA8Unorm, 1, data}, filename));
    CORRADE_VERIFY(!Utility::Path::exists(filename));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convert2DToFileThroughDataFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert2DToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const ImageView2D&) override {
            return {};
        };
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    const char data[4]{};

    /* Function should fail, no file should get written and no error output
       should be printed (the base implementation assumes the plugin does it) */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile(ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, data}, filename));
    CORRADE_VERIFY(!Utility::Path::exists(filename));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convert3DToFileThroughDataFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert3DToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const ImageView3D&) override {
            return {};
        };
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    const char data[4]{};

    /* Function should fail, no file should get written and no error output
       should be printed (the base implementation assumes the plugin does it) */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile(ImageView3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, data}, filename));
    CORRADE_VERIFY(!Utility::Path::exists(filename));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convert1DToFileThroughDataNotWritable() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert1DToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const ImageView1D&) override {
            return Containers::array({'\x00'});
        };
    } converter;

    const char data[4]{};
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile(ImageView1D{PixelFormat::RGBA8Unorm, 1, data}, "/some/path/that/does/not/exist"));
    CORRADE_COMPARE_AS(out,
        "Trade::AbstractImageConverter::convertToFile(): cannot write to file /some/path/that/does/not/exist\n",
        TestSuite::Compare::StringHasSuffix);
}

void AbstractImageConverterTest::convert2DToFileThroughDataNotWritable() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert2DToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const ImageView2D&) override {
            return Containers::array({'\x00'});
        };
    } converter;

    const char data[4]{};
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile(ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, data}, "/some/path/that/does/not/exist"));
    CORRADE_COMPARE_AS(out,
        "Trade::AbstractImageConverter::convertToFile(): cannot write to file /some/path/that/does/not/exist\n",
        TestSuite::Compare::StringHasSuffix);
}

void AbstractImageConverterTest::convert3DToFileThroughDataNotWritable() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert3DToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const ImageView3D&) override {
            return Containers::array({'\x00'});
        };
    } converter;

    const char data[4]{};
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile(ImageView3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, data}, "/some/path/that/does/not/exist"));
    CORRADE_COMPARE_AS(out,
        "Trade::AbstractImageConverter::convertToFile(): cannot write to file /some/path/that/does/not/exist\n",
        TestSuite::Compare::StringHasSuffix);
}

void AbstractImageConverterTest::convert1DToFileInvalidImage() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert1DToFile; }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile(ImageView1D{PixelFormat::RGBA8Unorm, 0, nullptr}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToFile(): can't convert image with a zero size: Vector(0)\n");
}

void AbstractImageConverterTest::convert2DToFileInvalidImage() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert2DToFile; }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile(ImageView2D{PixelFormat::RGBA8Unorm, {}, nullptr}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToFile(): can't convert image with a zero size: Vector(0, 0)\n");
}

void AbstractImageConverterTest::convert3DToFileInvalidImage() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert3DToFile; }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile(ImageView3D{PixelFormat::RGBA8Unorm, {}, nullptr}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToFile(): can't convert image with a zero size: Vector(0, 0, 0)\n");
}

void AbstractImageConverterTest::convert1DToFileNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert1DToFile; }
    } converter;

    const char data[4]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile(ImageView1D{PixelFormat::RGBA8Unorm, 1, data}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToFile(): 1D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convert2DToFileNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert2DToFile; }
    } converter;

    const char data[4]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile(ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, data}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToFile(): 2D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convert3DToFileNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::Convert3DToFile; }
    } converter;

    const char data[4]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile(ImageView3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, data}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToFile(): 3D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convertCompressed1DToFile() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed1DToFile; }
        bool doConvertToFile(const CompressedImageView1D& image, Containers::StringView filename) override {
            return Utility::Path::write(filename, Containers::arrayView(
                {char(image.size()[0])}));
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    CORRADE_VERIFY(converter.convertToFile(CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 0x0f, Containers::Array<char>{64}}, filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convertCompressed2DToFile() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed2DToFile; }
        bool doConvertToFile(const CompressedImageView2D& image, Containers::StringView filename) override {
            return Utility::Path::write(filename, Containers::arrayView(
                {char(image.size().x()), char(image.size().y())}));
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    CORRADE_VERIFY(converter.convertToFile(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {0x0f, 0x0d}, Containers::Array<char>{NoInit, 128}}, filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f\x0d", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convertCompressed3DToFile() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed3DToFile; }
        bool doConvertToFile(const CompressedImageView3D& image, Containers::StringView filename) override {
            return Utility::Path::write(filename, Containers::arrayView(
                {char(image.size().x()), char(image.size().y()), char(image.size().z())}));
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    CORRADE_VERIFY(converter.convertToFile(CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {0x0f, 0x0d, 0x02}, Containers::Array<char>{NoInit, 256}}, filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f\x0d\x02", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convertCompressed1DToFileFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed1DToFile;
        }
        bool doConvertToFile(const CompressedImageView1D&, Containers::StringView) override {
            return {};
        }
    } converter;

    const char imageData[4*4]{};

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile(CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 1, imageData}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out")));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertCompressed2DToFileFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed2DToFile;
        }
        bool doConvertToFile(const CompressedImageView2D&, Containers::StringView) override {
            return {};
        }
    } converter;

    const char imageData[4*4]{};

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {1, 1}, imageData}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out")));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertCompressed3DToFileFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed3DToFile;
        }
        bool doConvertToFile(const CompressedImageView3D&, Containers::StringView) override {
            return {};
        }
    } converter;

    const char imageData[4*4]{};

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile(CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {1, 1, 1}, imageData}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out")));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertCompressed1DToFileThroughData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed1DToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const CompressedImageView1D& image) override {
            return Containers::array({char(image.size()[0])});
        };
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    /* doConvertToFile() should call doConvertToData() */
    CORRADE_VERIFY(converter.convertToFile(CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 0x0f, Containers::Array<char>{64}}, filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convertCompressed2DToFileThroughData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed2DToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const CompressedImageView2D& image) override {
            return Containers::array({char(image.size().x()), char(image.size().y())});
        };
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    /* doConvertToFile() should call doConvertToData() */
    CORRADE_VERIFY(converter.convertToFile(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {0x0f, 0x0d}, Containers::Array<char>{NoInit, 128}}, filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f\x0d", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convertCompressed3DToFileThroughData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed3DToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const CompressedImageView3D& image) override {
            return Containers::array({char(image.size().x()), char(image.size().y()), char(image.size().z())});
        };
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    /* doConvertToFile() should call doConvertToData() */
    CORRADE_VERIFY(converter.convertToFile(CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {0x0f, 0x0d, 0x02}, Containers::Array<char>{NoInit, 256}}, filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f\x0d\x02", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convertCompressed1DToFileThroughDataFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed1DToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const CompressedImageView1D&) override {
            return {};
        };
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    const char data[8]{};

    /* Function should fail, no file should get written and no error output
       should be printed (the base implementation assumes the plugin does it) */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile(CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 4, data}, filename));
    CORRADE_VERIFY(!Utility::Path::exists(filename));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertCompressed2DToFileThroughDataFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed2DToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const CompressedImageView2D&) override {
            return {};
        };
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    const char data[8]{};

    /* Function should fail, no file should get written and no error output
       should be printed (the base implementation assumes the plugin does it) */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4}, data}, filename));
    CORRADE_VERIFY(!Utility::Path::exists(filename));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertCompressed3DToFileThroughDataFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed3DToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const CompressedImageView3D&) override {
            return {};
        };
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    const char data[8]{};

    /* Function should fail, no file should get written and no error output
       should be printed (the base implementation assumes the plugin does it) */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile(CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4, 1}, data}, filename));
    CORRADE_VERIFY(!Utility::Path::exists(filename));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertCompressed1DToFileThroughDataNotWritable() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed1DToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const CompressedImageView1D&) override {
            return Containers::array({'\x00'});
        };
    } converter;

    const char data[8]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile(CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 4, data}, "/some/path/that/does/not/exist");
    CORRADE_COMPARE_AS(out,
        "Trade::AbstractImageConverter::convertToFile(): cannot write to file /some/path/that/does/not/exist\n",
        TestSuite::Compare::StringHasSuffix);
}

void AbstractImageConverterTest::convertCompressed2DToFileThroughDataNotWritable() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed2DToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const CompressedImageView2D&) override {
            return Containers::array({'\x00'});
        };
    } converter;

    const char data[8]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4}, data}, "/some/path/that/does/not/exist");
    CORRADE_COMPARE_AS(out,
        "Trade::AbstractImageConverter::convertToFile(): cannot write to file /some/path/that/does/not/exist\n",
        TestSuite::Compare::StringHasSuffix);
}

void AbstractImageConverterTest::convertCompressed3DToFileThroughDataNotWritable() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed3DToData; }

        Containers::Optional<Containers::Array<char>> doConvertToData(const CompressedImageView3D&) override {
            return Containers::array({'\x00'});
        };
    } converter;

    const char data[8]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile(CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4, 1}, data}, "/some/path/that/does/not/exist");
    CORRADE_COMPARE_AS(out,
        "Trade::AbstractImageConverter::convertToFile(): cannot write to file /some/path/that/does/not/exist\n",
        TestSuite::Compare::StringHasSuffix);
}

void AbstractImageConverterTest::convertCompressed1DToFileInvalidImage() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed1DToFile; }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile(CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 0, nullptr}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToFile(): can't convert image with a zero size: Vector(0)\n");
}

void AbstractImageConverterTest::convertCompressed2DToFileInvalidImage() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed2DToFile; }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {}, nullptr}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToFile(): can't convert image with a zero size: Vector(0, 0)\n");
}

void AbstractImageConverterTest::convertCompressed3DToFileInvalidImage() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed3DToFile; }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile(CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {}, nullptr}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToFile(): can't convert image with a zero size: Vector(0, 0, 0)\n");
}

void AbstractImageConverterTest::convertCompressed1DToFileNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed1DToFile; }
    } converter;

    const char data[8]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile(CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 4, data}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToFile(): compressed 1D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convertCompressed2DToFileNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed2DToFile; }
    } converter;

    const char data[8]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4}, data}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToFile(): compressed 2D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convertCompressed3DToFileNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override { return ImageConverterFeature::ConvertCompressed3DToFile; }
    } converter;

    const char data[8]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile(CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4, 1}, data}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToFile(): compressed 3D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convertImageData1DToFile() {
    ImageData1DConverter converter;

    /* Should get "B" when converting uncompressed */
    CORRADE_VERIFY(converter.convertToFile(ImageData1D{PixelFormat::RGBA16F, 1, Containers::Array<char>{8}}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out")));
    CORRADE_COMPARE_AS(Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"),
        "B", TestSuite::Compare::FileToString);

    /* Should get "C" when converting compressed */
    CORRADE_VERIFY(converter.convertToFile(ImageData1D{CompressedPixelFormat::Bc2RGBAUnorm, 4, Containers::Array<char>{NoInit, 16}}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out")));
    CORRADE_COMPARE_AS(Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"),
        "C", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convertImageData2DToFile() {
    ImageData2DConverter converter;

    /* Should get "B" when converting uncompressed */
    CORRADE_VERIFY(converter.convertToFile(ImageData2D{PixelFormat::RGBA16F, {1, 1}, Containers::Array<char>{8}}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out")));
    CORRADE_COMPARE_AS(Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"),
        "B", TestSuite::Compare::FileToString);

    /* Should get "C" when converting compressed */
    CORRADE_VERIFY(converter.convertToFile(ImageData2D{CompressedPixelFormat::Bc2RGBAUnorm, {4, 4}, Containers::Array<char>{NoInit, 16}}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out")));
    CORRADE_COMPARE_AS(Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"),
        "C", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convertImageData3DToFile() {
    ImageData3DConverter converter;

    /* Should get "B" when converting uncompressed */
    CORRADE_VERIFY(converter.convertToFile(ImageData3D{PixelFormat::RGBA16F, {1, 1, 1}, Containers::Array<char>{8}}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out")));
    CORRADE_COMPARE_AS(Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"),
        "B", TestSuite::Compare::FileToString);

    /* Should get "C" when converting compressed */
    CORRADE_VERIFY(converter.convertToFile(ImageData3D{CompressedPixelFormat::Bc2RGBAUnorm, {4, 4, 1}, Containers::Array<char>{NoInit, 16}}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out")));
    CORRADE_COMPARE_AS(Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"),
        "C", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convertLevels1DToFile() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert1DToFile|
                   ImageConverterFeature::Levels;
        }
        bool doConvertToFile(Containers::ArrayView<const ImageView1D> imageLevels, Containers::StringView filename) override {
            return Utility::Path::write(filename, Containers::arrayView(
                {char(imageLevels[0].size()[0]), char(imageLevels.size())}));
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    const char data[4]{};
    CORRADE_VERIFY(converter.convertToFile({
        /* Arbitrary dimensions should be fine */
        ImageView1D{PixelFormat::RGBA8Unorm, 0x0f, Containers::Array<char>{0x0f*4}},
        ImageView1D{PixelFormat::RGBA8Unorm, 1, data}
    }, filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f\x02", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convertLevels2DToFile() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert2DToFile|
                   ImageConverterFeature::Levels;
        }
        bool doConvertToFile(Containers::ArrayView<const ImageView2D> imageLevels, Containers::StringView filename) override {
            return Utility::Path::write(filename, Containers::arrayView(
                {char(imageLevels[0].size().x()), char(imageLevels[0].size().y()), char(imageLevels.size())}));
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    const char data[4]{};
    CORRADE_VERIFY(converter.convertToFile({
        /* Arbitrary dimensions should be fine */
        ImageView2D{PixelFormat::RGBA8Unorm, {0x0f, 0x0d}, Containers::Array<char>{0x0f*0x0d*4}},
        ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, data}
    }, filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f\x0d\x02", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convertLevels3DToFile() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert3DToFile|
                   ImageConverterFeature::Levels;
        }
        bool doConvertToFile(Containers::ArrayView<const ImageView3D> imageLevels, Containers::StringView filename) override {
            return Utility::Path::write(filename, Containers::arrayView(
                {char(imageLevels[0].size().x()), char(imageLevels[0].size().y()), char(imageLevels[0].size().z()), char(imageLevels.size())}));
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    const char data[4]{};
    CORRADE_VERIFY(converter.convertToFile({
        /* Arbitrary dimensions should be fine */
        ImageView3D{PixelFormat::RGBA8Unorm, {0x0f, 0x0d, 0x0e}, Containers::Array<char>{0x0f*0x0d*0x0e*4}},
        ImageView3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, data}
    }, filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f\x0d\x0e\x02", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convertLevels1DToFileFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert1DToFile|
                   ImageConverterFeature::Levels;
        }
        bool doConvertToFile(Containers::ArrayView<const ImageView1D>, Containers::StringView) override {
            return {};
        }
    } converter;

    const char imageData[4]{};

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile({ImageView1D{PixelFormat::RGBA8Unorm, 1, imageData}}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out")));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertLevels2DToFileFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert2DToFile|
                   ImageConverterFeature::Levels;
        }
        bool doConvertToFile(Containers::ArrayView<const ImageView2D>, Containers::StringView) override {
            return {};
        }
    } converter;

    const char imageData[4]{};

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile({ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, imageData}}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out")));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertLevels3DToFileFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert3DToFile|
                   ImageConverterFeature::Levels;
        }
        bool doConvertToFile(Containers::ArrayView<const ImageView3D>, Containers::StringView) override {
            return {};
        }
    } converter;

    const char imageData[4]{};

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile({ImageView3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, imageData}}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out")));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertLevels1DToFileThroughData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert1DToData|
                   ImageConverterFeature::Levels;
        }

        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const ImageView1D> imageLevels) override {
            return Containers::array({char(imageLevels[0].size()[0]), char(imageLevels.size())});
        };
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    const char data[4]{};

    /* doConvertToFile() should call doConvertToData() */
    CORRADE_VERIFY(converter.convertToFile({
        /* Arbitrary dimensions should be fine */
        ImageView1D{PixelFormat::RGBA8Unorm, 0x0f, Containers::Array<char>{0x0f*4}},
        ImageView1D{PixelFormat::RGBA8Unorm, 1, data}
    }, filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f\x02", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convertLevels2DToFileThroughData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert2DToData|
                   ImageConverterFeature::Levels;
        }

        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const ImageView2D> imageLevels) override {
            return Containers::array({char(imageLevels[0].size().x()), char(imageLevels[0].size().y()), char(imageLevels.size())});
        };
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    const char data[4]{};

    /* doConvertToFile() should call doConvertToData() */
    CORRADE_VERIFY(converter.convertToFile({
        ImageView2D(PixelFormat::RGBA8Unorm, {0x0f, 0x0d}, Containers::Array<char>{0x0f*0x0d*4}),
        ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, data}
    }, filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f\x0d\x02", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convertLevels3DToFileThroughData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert3DToData|
                   ImageConverterFeature::Levels;
        }

        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const ImageView3D> imageLevels) override {
            return Containers::array({char(imageLevels[0].size().x()), char(imageLevels[0].size().y()), char(imageLevels[0].size().z()), char(imageLevels.size())});
        };
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    const char data[4]{};

    /* doConvertToFile() should call doConvertToData() */
    CORRADE_VERIFY(converter.convertToFile({
        ImageView3D{PixelFormat::RGBA8Unorm, {0x0f, 0x0d, 0x0e}, Containers::Array<char>{0x0f*0x0d*0x0e*4}},
        ImageView3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, data}
    }, filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f\x0d\x0e\x02", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convertLevels1DToFileThroughDataFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert1DToData|
                   ImageConverterFeature::Levels;
        }

        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const ImageView1D>) override {
            return {};
        };
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    const char data[4]{};

    /* Function should fail, no file should get written and no error output
       should be printed (the base implementation assumes the plugin does it) */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile({ImageView1D{PixelFormat::RGBA8Unorm, 1, data}}, filename));
    CORRADE_VERIFY(!Utility::Path::exists(filename));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertLevels2DToFileThroughDataFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert2DToData|
                   ImageConverterFeature::Levels;
        }

        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const ImageView2D>) override {
            return {};
        };
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    const char data[4]{};

    /* Function should fail, no file should get written and no error output
       should be printed (the base implementation assumes the plugin does it) */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile({ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, data}}, filename));
    CORRADE_VERIFY(!Utility::Path::exists(filename));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertLevels3DToFileThroughDataFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert3DToData|
                   ImageConverterFeature::Levels;
        }

        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const ImageView3D>) override {
            return {};
        };
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    const char data[4]{};

    /* Function should fail, no file should get written and no error output
       should be printed (the base implementation assumes the plugin does it) */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile({ImageView3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, data}}, filename));
    CORRADE_VERIFY(!Utility::Path::exists(filename));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertLevels1DToFileThroughDataNotWritable() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert1DToData|
                   ImageConverterFeature::Levels;
        }
        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const ImageView1D>) override {
            return Containers::array({'\x00'});
        };
    } converter;

    const char data[4]{};
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile({ImageView1D{PixelFormat::RGBA8Unorm, 1, data}}, "/some/path/that/does/not/exist"));
    CORRADE_COMPARE_AS(out,
        "Trade::AbstractImageConverter::convertToFile(): cannot write to file /some/path/that/does/not/exist\n",
        TestSuite::Compare::StringHasSuffix);
}

void AbstractImageConverterTest::convertLevels2DToFileThroughDataNotWritable() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert2DToData|
                   ImageConverterFeature::Levels;
        }

        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const ImageView2D>) override {
            return Containers::array({'\x00'});
        };
    } converter;

    const char data[4]{};
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile({ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, data}}, "/some/path/that/does/not/exist"));
    CORRADE_COMPARE_AS(out,
        "Trade::AbstractImageConverter::convertToFile(): cannot write to file /some/path/that/does/not/exist\n",
        TestSuite::Compare::StringHasSuffix);
}

void AbstractImageConverterTest::convertLevels3DToFileThroughDataNotWritable() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert3DToData|
                   ImageConverterFeature::Levels;
        }

        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const ImageView3D>) override {
            return Containers::array({'\x00'});
        };
    } converter;

    const char data[4]{};
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile({ImageView3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, data}}, "/some/path/that/does/not/exist"));
    CORRADE_COMPARE_AS(out,
        "Trade::AbstractImageConverter::convertToFile(): cannot write to file /some/path/that/does/not/exist\n",
        TestSuite::Compare::StringHasSuffix);
}

void AbstractImageConverterTest::convertLevels1DToFileInvalidImage() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert1DToFile|
                   ImageConverterFeature::Levels;
        }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile(std::initializer_list<ImageView1D>{}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToFile(): at least one image has to be specified\n");
}

void AbstractImageConverterTest::convertLevels2DToFileInvalidImage() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert2DToFile|
                   ImageConverterFeature::Levels;
        }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile(std::initializer_list<ImageView2D>{}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToFile(): at least one image has to be specified\n");
}

void AbstractImageConverterTest::convertLevels3DToFileInvalidImage() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert3DToFile|
                   ImageConverterFeature::Levels;
        }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile(std::initializer_list<ImageView3D>{}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToFile(): at least one image has to be specified\n");
}

void AbstractImageConverterTest::convertLevels1DToFileNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert1DToFile|
                   ImageConverterFeature::Levels;
        }
    } converter;

    const char data[4]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile({ImageView1D{PixelFormat::RGBA8Unorm, 1, data}}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToFile(): multi-level 1D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convertLevels2DToFileNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert2DToFile|
                   ImageConverterFeature::Levels;
        }
    } converter;

    const char data[4]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile({ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, data}}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToFile(): multi-level 2D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convertLevels3DToFileNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert3DToFile|
                   ImageConverterFeature::Levels;
        }
    } converter;

    const char data[4]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile({ImageView3D{PixelFormat::RGBA8Unorm, {1, 1, 1}, data}}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToFile(): multi-level 3D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convertCompressedLevels1DToFile() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed1DToFile|
                   ImageConverterFeature::Levels;
        }
        bool doConvertToFile(Containers::ArrayView<const CompressedImageView1D> imageLevels, Containers::StringView filename) override {
            return Utility::Path::write(filename, Containers::arrayView(
                {char(imageLevels[0].size()[0]), char(imageLevels.size())}));
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    const char data[8]{};
    CORRADE_VERIFY(converter.convertToFile({
        CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 0x0f, Containers::Array<char>{64}},
        CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 4, data}
    }, filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f\x02", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convertCompressedLevels2DToFile() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed2DToFile|
                   ImageConverterFeature::Levels;
        }
        bool doConvertToFile(Containers::ArrayView<const CompressedImageView2D> imageLevels, Containers::StringView filename) override {
            return Utility::Path::write(filename, Containers::arrayView(
                {char(imageLevels[0].size().x()), char(imageLevels[0].size().y()), char(imageLevels.size())}));
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    const char data[8]{};
    CORRADE_VERIFY(converter.convertToFile({
        CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {0x0f, 0x0d}, Containers::Array<char>{NoInit, 128}},
        CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4}, data}
    }, filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f\x0d\x02", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convertCompressedLevels3DToFile() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed3DToFile|
                   ImageConverterFeature::Levels;
        }
        bool doConvertToFile(Containers::ArrayView<const CompressedImageView3D> imageLevels, Containers::StringView filename) override {
            return Utility::Path::write(filename, Containers::arrayView(
                {char(imageLevels[0].size().x()), char(imageLevels[0].size().y()), char(imageLevels[0].size().z()), char(imageLevels.size())}));
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    const char data[8]{};
    CORRADE_VERIFY(converter.convertToFile({
        CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {0x0f, 0x0d, 0x0e}, Containers::Array<char>{NoInit, 1792}},
        CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4, 1}, data}
    }, filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f\x0d\x0e\x02", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convertCompressedLevels1DToFileFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed1DToFile|
                   ImageConverterFeature::Levels;
        }
        bool doConvertToFile(Containers::ArrayView<const CompressedImageView1D>, Containers::StringView) override {
            return {};
        }
    } converter;

    const char imageData[4*4]{};

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile({CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 1, imageData}}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out")));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertCompressedLevels2DToFileFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed2DToFile|
                   ImageConverterFeature::Levels;
        }
        bool doConvertToFile(Containers::ArrayView<const CompressedImageView2D>, Containers::StringView) override {
            return {};
        }
    } converter;

    const char imageData[4*4]{};

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile({CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {1, 1}, imageData}}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out")));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertCompressedLevels3DToFileFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed3DToFile|
                   ImageConverterFeature::Levels;
        }
        bool doConvertToFile(Containers::ArrayView<const CompressedImageView3D>, Containers::StringView) override {
            return {};
        }
    } converter;

    const char imageData[8]{};

    /* The implementation is expected to print an error message on its own */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile({CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {1, 1, 1}, imageData}}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out")));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertCompressedLevels1DToFileThroughData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed1DToData|
                   ImageConverterFeature::Levels;
        }

        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const CompressedImageView1D> imageLevels) override {
            return Containers::array({char(imageLevels[0].size()[0]), char(imageLevels.size())});
        };
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    const char data[8]{};

    /* doConvertToFile() should call doConvertToData() */
    CORRADE_VERIFY(converter.convertToFile({
        CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 0x0f, Containers::Array<char>{64}},
        CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 4, data}
    }, filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f\x02", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convertCompressedLevels2DToFileThroughData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed2DToData|
                   ImageConverterFeature::Levels;
        }

        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const CompressedImageView2D> imageLevels) override {
            return Containers::array({char(imageLevels[0].size().x()), char(imageLevels[0].size().y()), char(imageLevels.size())});
        };
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    const char data[8]{};

    /* doConvertToFile() should call doConvertToData() */
    CORRADE_VERIFY(converter.convertToFile({
        CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {0x0f, 0x0d}, Containers::Array<char>{NoInit, 128}},
        CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4}, data}
    }, filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f\x0d\x02", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convertCompressedLevels3DToFileThroughData() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed3DToData|
                   ImageConverterFeature::Levels;
        }

        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const CompressedImageView3D> imageLevels) override {
            return Containers::array({char(imageLevels[0].size().x()), char(imageLevels[0].size().y()), char(imageLevels[0].size().z()), char(imageLevels.size())});
        };
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    const char data[8]{};

    /* doConvertToFile() should call doConvertToData() */
    CORRADE_VERIFY(converter.convertToFile({
        CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {0x0f, 0x0d, 0x0e}, Containers::Array<char>{NoInit, 1792}},
        CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4, 1}, data}
    }, filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f\x0d\x0e\x02", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convertCompressedLevels1DToFileThroughDataFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed1DToData|
                   ImageConverterFeature::Levels;
        }

        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const CompressedImageView1D>) override {
            return {};
        };
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    const char data[8]{};

    /* Function should fail, no file should get written and no error output
       should be printed (the base implementation assumes the plugin does it) */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile({CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 4, data}}, filename));
    CORRADE_VERIFY(!Utility::Path::exists(filename));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertCompressedLevels2DToFileThroughDataFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed2DToData|
                   ImageConverterFeature::Levels;
        }

        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const CompressedImageView2D>) override {
            return {};
        };
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    const char data[8]{};

    /* Function should fail, no file should get written and no error output
       should be printed (the base implementation assumes the plugin does it) */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile({CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4}, data}}, filename));
    CORRADE_VERIFY(!Utility::Path::exists(filename));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertCompressedLevels3DToFileThroughDataFailed() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed3DToData|
                   ImageConverterFeature::Levels;
        }

        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const CompressedImageView3D>) override {
            return {};
        };
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    const char data[8]{};

    /* Function should fail, no file should get written and no error output
       should be printed (the base implementation assumes the plugin does it) */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile({CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4, 1}, data}}, filename));
    CORRADE_VERIFY(!Utility::Path::exists(filename));
    CORRADE_COMPARE(out, "");
}

void AbstractImageConverterTest::convertCompressedLevels1DToFileThroughDataNotWritable() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed1DToData|
                   ImageConverterFeature::Levels;
        }

        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const CompressedImageView1D>) override {
            return Containers::array({'\x00'});
        };
    } converter;

    const char data[8]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile({CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 4, data}}, "/some/path/that/does/not/exist");
    CORRADE_COMPARE_AS(out,
        "Trade::AbstractImageConverter::convertToFile(): cannot write to file /some/path/that/does/not/exist\n",
        TestSuite::Compare::StringHasSuffix);
}

void AbstractImageConverterTest::convertCompressedLevels2DToFileThroughDataNotWritable() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed2DToData|
                   ImageConverterFeature::Levels;
        }

        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const CompressedImageView2D>) override {
            return Containers::array({'\x00'});
        };
    } converter;

    const char data[8]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile({CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4}, data}}, "/some/path/that/does/not/exist");
    CORRADE_COMPARE_AS(out,
        "Trade::AbstractImageConverter::convertToFile(): cannot write to file /some/path/that/does/not/exist\n",
        TestSuite::Compare::StringHasSuffix);
}

void AbstractImageConverterTest::convertCompressedLevels3DToFileThroughDataNotWritable() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed3DToData|
                   ImageConverterFeature::Levels;
        }

        Containers::Optional<Containers::Array<char>> doConvertToData(Containers::ArrayView<const CompressedImageView3D>) override {
            return Containers::array({'\x00'});
        };
    } converter;

    const char data[8]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile({CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4, 1}, data}}, "/some/path/that/does/not/exist");
    CORRADE_COMPARE_AS(out,
        "Trade::AbstractImageConverter::convertToFile(): cannot write to file /some/path/that/does/not/exist\n",
        TestSuite::Compare::StringHasSuffix);
}

void AbstractImageConverterTest::convertCompressedLevels1DToFileInvalidImage() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed1DToFile|
                   ImageConverterFeature::Levels;
        }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile(std::initializer_list<CompressedImageView1D>{}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToFile(): at least one image has to be specified\n");
}

void AbstractImageConverterTest::convertCompressedLevels2DToFileInvalidImage() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed2DToFile|
                   ImageConverterFeature::Levels;
        }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile(std::initializer_list<CompressedImageView2D>{}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToFile(): at least one image has to be specified\n");
}

void AbstractImageConverterTest::convertCompressedLevels3DToFileInvalidImage() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed3DToFile|
                   ImageConverterFeature::Levels;
        }
    } converter;

    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile(std::initializer_list<CompressedImageView3D>{}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToFile(): at least one image has to be specified\n");
}

void AbstractImageConverterTest::convertCompressedLevels1DToFileNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed1DToFile|
                   ImageConverterFeature::Levels;
        }
    } converter;

    const char data[8]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile({CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 4, data}}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToFile(): multi-level compressed 1D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convertCompressedLevels2DToFileNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed2DToFile|
                   ImageConverterFeature::Levels;
        }
    } converter;

    const char data[8]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile({CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4}, data}}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToFile(): multi-level compressed 2D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convertCompressedLevels3DToFileNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed3DToFile|
                   ImageConverterFeature::Levels;
        }
    } converter;

    const char data[8]{};
    Containers::String out;
    Error redirectError{&out};
    converter.convertToFile({CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4, 1}, data}}, Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out"));
    CORRADE_COMPARE(out, "Trade::AbstractImageConverter::convertToFile(): multi-level compressed 3D image conversion advertised but not implemented\n");
}

void AbstractImageConverterTest::convert1DToFileThroughLevels() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert1DToFile|
                   ImageConverterFeature::Levels;
        }
        bool doConvertToFile(Containers::ArrayView<const ImageView1D> imageLevels, Containers::StringView filename) override {
            return Utility::Path::write(filename, Containers::arrayView(
                {char(imageLevels[0].size()[0]), char(imageLevels.size())}));
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    CORRADE_VERIFY(converter.convertToFile(ImageView1D{PixelFormat::RGBA8Unorm, 0x0f, Containers::Array<char>{0x0f*4}}, filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f\x01", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convert2DToFileThroughLevels() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert2DToFile|
                   ImageConverterFeature::Levels;
        }
        bool doConvertToFile(Containers::ArrayView<const ImageView2D> imageLevels, Containers::StringView filename) override {
            return Utility::Path::write(filename, Containers::arrayView(
                {char(imageLevels[0].size().x()), char(imageLevels[0].size().y()), char(imageLevels.size())}));
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    CORRADE_VERIFY(converter.convertToFile(ImageView2D{PixelFormat::RGBA8Unorm, {0x0f, 0x0d}, Containers::Array<char>{0x0f*0x0d*4}}, filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f\x0d\x01", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convert3DToFileThroughLevels() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::Convert3DToFile|
                   ImageConverterFeature::Levels;
        }
        bool doConvertToFile(Containers::ArrayView<const ImageView3D> imageLevels, Containers::StringView filename) override {
            return Utility::Path::write(filename, Containers::arrayView(
                {char(imageLevels[0].size().x()), char(imageLevels[0].size().y()), char(imageLevels[0].size().z()), char(imageLevels.size())}));
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    CORRADE_VERIFY(converter.convertToFile(ImageView3D{PixelFormat::RGBA8Unorm, {0x0f, 0x0d, 0x0e}, Containers::Array<char>{0x0f*0x0d*0x0e*4}}, filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f\x0d\x0e\x01", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convertCompressed1DToFileThroughLevels() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed1DToFile|
                   ImageConverterFeature::Levels;
        }
        bool doConvertToFile(Containers::ArrayView<const CompressedImageView1D> imageLevels, Containers::StringView filename) override {
            return Utility::Path::write(filename, Containers::arrayView(
                {char(imageLevels[0].size()[0]), char(imageLevels.size())}));
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    CORRADE_VERIFY(converter.convertToFile(CompressedImageView1D{CompressedPixelFormat::Bc1RGBAUnorm, 0x0f, Containers::Array<char>{64}}, filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f\x01", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convertCompressed2DToFileThroughLevels() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed2DToFile|
                   ImageConverterFeature::Levels;
        }
        bool doConvertToFile(Containers::ArrayView<const CompressedImageView2D> imageLevels, Containers::StringView filename) override {
            return Utility::Path::write(filename, Containers::arrayView(
                {char(imageLevels[0].size().x()), char(imageLevels[0].size().y()), char(imageLevels.size())}));
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    CORRADE_VERIFY(converter.convertToFile(CompressedImageView2D{CompressedPixelFormat::Bc1RGBAUnorm, {0x0f, 0x0d}, Containers::Array<char>{NoInit, 128}}, filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f\x0d\x01", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::convertCompressed3DToFileThroughLevels() {
    struct: AbstractImageConverter {
        ImageConverterFeatures doFeatures() const override {
            return ImageConverterFeature::ConvertCompressed3DToFile|
                   ImageConverterFeature::Levels;
        }
        bool doConvertToFile(Containers::ArrayView<const CompressedImageView3D> imageLevels, Containers::StringView filename) override {
            return Utility::Path::write(filename, Containers::arrayView(
                {char(imageLevels[0].size().x()), char(imageLevels[0].size().y()), char(imageLevels[0].size().z()), char(imageLevels.size())}));
        }
    } converter;

    /* Remove previous file, if any */
    Containers::String filename = Utility::Path::join(TRADE_TEST_OUTPUT_DIR, "image.out");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    CORRADE_VERIFY(converter.convertToFile(CompressedImageView3D{CompressedPixelFormat::Bc1RGBAUnorm, {0x0f, 0x0d, 0x0e}, Containers::Array<char>{NoInit, 1792}}, filename));
    CORRADE_COMPARE_AS(filename,
        "\x0f\x0d\x0e\x01", TestSuite::Compare::FileToString);
}

void AbstractImageConverterTest::debugFeature() {
    Containers::String out;

    Debug{&out} << ImageConverterFeature::ConvertCompressed2D << ImageConverterFeature(0xdeadbeef);
    CORRADE_COMPARE(out, "Trade::ImageConverterFeature::ConvertCompressed2D Trade::ImageConverterFeature(0xdeadbeef)\n");
}

void AbstractImageConverterTest::debugFeaturePacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << ImageConverterFeature::ConvertCompressed2D << Debug::packed << ImageConverterFeature(0xdeadbeef) << ImageConverterFeature::Convert3D;
    CORRADE_COMPARE(out, "ConvertCompressed2D 0xdeadbeef Trade::ImageConverterFeature::Convert3D\n");
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AbstractImageConverterTest::debugFeatureDeprecated() {
    Containers::String out;

    CORRADE_IGNORE_DEPRECATED_PUSH
    Debug{&out} << ImageConverterFeature::ConvertCompressedLevels1DToData << ImageConverterFeature::ConvertLevels3DToFile;
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out, "Trade::ImageConverterFeature::ConvertCompressed1DToData|Trade::ImageConverterFeature::Levels Trade::ImageConverterFeature::Convert3DToFile|Trade::ImageConverterFeature::Levels\n");
}

void AbstractImageConverterTest::debugFeatureDeprecatedPacked() {
    Containers::String out;

    CORRADE_IGNORE_DEPRECATED_PUSH
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << ImageConverterFeature::ConvertCompressedLevels1DToData << Debug::packed << ImageConverterFeature::ConvertLevels3DToFile << ImageConverterFeature::Convert1D;
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out, "ConvertCompressed1DToData|Levels Convert3DToFile|Levels Trade::ImageConverterFeature::Convert1D\n");
}
#endif

void AbstractImageConverterTest::debugFeatures() {
    Containers::String out;

    Debug{&out} << (ImageConverterFeature::Convert2DToData|ImageConverterFeature::ConvertCompressed2DToFile) << ImageConverterFeatures{};
    CORRADE_COMPARE(out, "Trade::ImageConverterFeature::Convert2DToData|Trade::ImageConverterFeature::ConvertCompressed2DToFile Trade::ImageConverterFeatures{}\n");
}

void AbstractImageConverterTest::debugFeaturesPacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << (ImageConverterFeature::Convert2DToData|ImageConverterFeature::ConvertCompressed2DToFile) << Debug::packed << ImageConverterFeatures{} << ImageConverterFeature::Convert1D;
    CORRADE_COMPARE(out, "Convert2DToData|ConvertCompressed2DToFile {} Trade::ImageConverterFeature::Convert1D\n");
}

void AbstractImageConverterTest::debugFeaturesSupersets() {
    /* Convert*DToData is a superset of Convert*DToFile, so only one should be
       printed */
    {
        Containers::String out;
        Debug{&out} << (ImageConverterFeature::Convert2DToData|ImageConverterFeature::Convert2DToFile);
        CORRADE_COMPARE(out, "Trade::ImageConverterFeature::Convert2DToData\n");

    /* ConvertCompressed*DToData is a superset of ConvertCompressed*DToFile, so
       only one should be printed */
    } {
        Containers::String out;
        Debug{&out} << (ImageConverterFeature::ConvertCompressed1DToData|ImageConverterFeature::ConvertCompressed1DToFile);
        CORRADE_COMPARE(out, "Trade::ImageConverterFeature::ConvertCompressed1DToData\n");
    }
}

void AbstractImageConverterTest::debugFlag() {
    Containers::String out;

    Debug{&out} << ImageConverterFlag::Verbose << ImageConverterFlag(0xf0);
    CORRADE_COMPARE(out, "Trade::ImageConverterFlag::Verbose Trade::ImageConverterFlag(0xf0)\n");
}

void AbstractImageConverterTest::debugFlags() {
    Containers::String out;

    Debug{&out} << (ImageConverterFlag::Verbose|ImageConverterFlag(0xf0)) << ImageConverterFlags{};
    CORRADE_COMPARE(out, "Trade::ImageConverterFlag::Verbose|Trade::ImageConverterFlag(0xf0) Trade::ImageConverterFlags{}\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AbstractImageConverterTest)
