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

#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/Path.h>

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/Math/ColorBatch.h"
#include "Magnum/Math/Vector4.h"
#include "Magnum/Trade/AbstractImageConverter.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ImageData.h"

#include "configure.h"

namespace Magnum { namespace Math { namespace Test { namespace {

struct ColorBatchTest: TestSuite::Tester {
    explicit ColorBatchTest();

    void yFlip();
    void yFlip3D();

    void yFlipInvalidLastDimension();

    PluginManager::Manager<Trade::AbstractImageConverter> _converterManager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    PluginManager::Manager<Trade::AbstractImporter> _importerManager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
};

/* The expected arrays are formatted from the test failure output with
    print(', '.join(["'\\x{:02x}'".format(i + 255 if i < 0 else i) for i in vals]))
   with vals being an array printed by the test */
/** @todo make something more convenient for binary data comparison in
    TestSuite already, FFS */

/* Used by both yFlip() and yFlip3D() */
const char CheckerboardBC1[]{
    /* ./format-block-data.py checkerboard.in.png checkerboard.png */
    '\x5c', '\xa6', '\x54', '\x74', '\x00', '\x00', '\x40', '\x40',
    '\x3c', '\xa6', '\x95', '\x7c', '\x00', '\x00', '\x02', '\x01',
    '\x8c', '\x3e', '\x69', '\x33', '\x00', '\x00', '\x40', '\x40',
    '\x8c', '\x3e', '\x29', '\x33', '\x00', '\x00', '\x01', '\x01',
    '\x3c', '\xa6', '\x6f', '\x5b', '\x00', '\x40', '\x00', '\x00',
    '\x3c', '\xa6', '\xcc', '\x4a', '\x03', '\x01', '\x00', '\x00',
    '\x8c', '\x3e', '\xc9', '\x32', '\x40', '\xc0', '\x00', '\x00',
    '\x8c', '\x3e', '\x89', '\x33', '\x01', '\x01', '\x00', '\x00',
    '\x6c', '\x3e', '\xe9', '\x32', '\x00', '\x00', '\xc0', '\x40',
    '\x6c', '\x3e', '\x69', '\x33', '\x00', '\x00', '\x01', '\x01',
    '\x66', '\xc6', '\x67', '\x84', '\x00', '\x00', '\x40', '\x40',
    '\x66', '\xc6', '\xa6', '\x8c', '\x00', '\x00', '\x02', '\x01',
    '\x8c', '\x3e', '\xc9', '\x32', '\x40', '\xc0', '\x00', '\x00',
    '\x8c', '\x3e', '\x8a', '\x33', '\x03', '\x01', '\x00', '\x00',
    '\x66', '\xc6', '\x87', '\x63', '\x00', '\x40', '\x00', '\x00',
    '\x66', '\xc6', '\xc7', '\x52', '\x03', '\x01', '\x00', '\x00',
    '\x66', '\xc6', '\x87', '\x6b', '\x00', '\x00', '\x40', '\x00',
    '\x66', '\xc6', '\x67', '\x63', '\x00', '\x00', '\x01', '\x09',
    '\xa6', '\xc9', '\xc7', '\x51', '\x00', '\x00', '\xc0', '\x40',
    '\xa6', '\xc9', '\xa7', '\x61', '\x00', '\x00', '\x01', '\x01',
    '\x66', '\xc6', '\x67', '\x42', '\x40', '\xc0', '\x00', '\x00',
    '\x66', '\xc6', '\xc7', '\x73', '\x01', '\x01', '\x00', '\x00',
    '\xa6', '\xc9', '\xa7', '\x51', '\x40', '\xc0', '\x00', '\x00',
    '\xa6', '\xc9', '\xa7', '\x69', '\x03', '\x01', '\x00', '\x00',
};
const char CheckerboardBC1Flipped[]{
    '\x66', '\xc6', '\x67', '\x42', '\x00', '\x00', '\xc0', '\x40',
    '\x66', '\xc6', '\xc7', '\x73', '\x00', '\x00', '\x01', '\x01',
    '\xa6', '\xc9', '\xa7', '\x51', '\x00', '\x00', '\xc0', '\x40',
    '\xa6', '\xc9', '\xa7', '\x69', '\x00', '\x00', '\x01', '\x03',
    '\x66', '\xc6', '\x87', '\x6b', '\x00', '\x40', '\x00', '\x00',
    '\x66', '\xc6', '\x67', '\x63', '\x09', '\x01', '\x00', '\x00',
    '\xa6', '\xc9', '\xc7', '\x51', '\x40', '\xc0', '\x00', '\x00',
    '\xa6', '\xc9', '\xa7', '\x61', '\x01', '\x01', '\x00', '\x00',
    '\x8c', '\x3e', '\xc9', '\x32', '\x00', '\x00', '\xc0', '\x40',
    '\x8c', '\x3e', '\x8a', '\x33', '\x00', '\x00', '\x01', '\x03',
    '\x66', '\xc6', '\x87', '\x63', '\x00', '\x00', '\x40', '\x00',
    '\x66', '\xc6', '\xc7', '\x52', '\x00', '\x00', '\x01', '\x03',
    '\x6c', '\x3e', '\xe9', '\x32', '\x40', '\xc0', '\x00', '\x00',
    '\x6c', '\x3e', '\x69', '\x33', '\x01', '\x01', '\x00', '\x00',
    '\x66', '\xc6', '\x67', '\x84', '\x40', '\x40', '\x00', '\x00',
    '\x66', '\xc6', '\xa6', '\x8c', '\x01', '\x02', '\x00', '\x00',
    '\x3c', '\xa6', '\x6f', '\x5b', '\x00', '\x00', '\x40', '\x00',
    '\x3c', '\xa6', '\xcc', '\x4a', '\x00', '\x00', '\x01', '\x03',
    '\x8c', '\x3e', '\xc9', '\x32', '\x00', '\x00', '\xc0', '\x40',
    '\x8c', '\x3e', '\x89', '\x33', '\x00', '\x00', '\x01', '\x01',
    '\x5c', '\xa6', '\x54', '\x74', '\x40', '\x40', '\x00', '\x00',
    '\x3c', '\xa6', '\x95', '\x7c', '\x01', '\x02', '\x00', '\x00',
    '\x8c', '\x3e', '\x69', '\x33', '\x40', '\x40', '\x00', '\x00',
    '\x8c', '\x3e', '\x29', '\x33', '\x01', '\x01', '\x00', '\x00'
};

const struct {
    const char* name;
    CompressedPixelFormat format;
    Vector2i blockCount;
    Containers::Array<char> input;
    void(*function)(const Containers::StridedArrayView4D<char>&);
    const char* file;
    Containers::Array<char> expected;
} YFlipData[]{
    /* The multi-block behavior is tested just for one format as it's internally
       a shared template for all */
    {"BC1, even block count", CompressedPixelFormat::Bc1RGBAUnorm, {4, 6},
        Containers::Array<char>{const_cast<char*>(CheckerboardBC1), Containers::arraySize(CheckerboardBC1), [](char*, std::size_t){}},
        yFlipBc1InPlace, "checkerboard.png",
        Containers::Array<char>{const_cast<char*>(CheckerboardBC1Flipped), Containers::arraySize(CheckerboardBC1Flipped), [](char*, std::size_t){}}},
    {"BC1, odd block count", CompressedPixelFormat::Bc1RGBAUnorm, {6, 3}, {InPlaceInit, {
        /* ./format-block-data.py checkerboard-odd.in.png checkerboard-odd.png */
        '\x8c', '\x3e', '\x8a', '\x33', '\x00', '\xa0', '\x50', '\x90',
        '\x66', '\xc6', '\x8c', '\x3e', '\x05', '\x05', '\x05', '\x05',
        '\x66', '\xc6', '\xe7', '\x73', '\x00', '\x02', '\x0b', '\x06',
        '\xa6', '\xc9', '\xa7', '\x71', '\x00', '\xa0', '\x50', '\x50',
        '\xa6', '\xc9', '\x19', '\x34', '\x50', '\x50', '\x50', '\x50',
        '\x19', '\x34', '\x32', '\x33', '\x00', '\x0f', '\x07', '\x05',
        '\x45', '\xc6', '\x2c', '\x3e', '\x55', '\x55', '\x00', '\xa0',
        '\x25', '\xe2', '\x8a', '\x77', '\xf5', '\xf5', '\x0f', '\x0f',
        '\x66', '\xbe', '\xa6', '\xb9', '\x0a', '\x00', '\x55', '\x55',
        '\xa6', '\xc1', '\x18', '\x34', '\x00', '\x00', '\x55', '\x55',
        '\xc5', '\xc9', '\x38', '\x34', '\x50', '\x50', '\xf5', '\xf5',
        '\x8e', '\x73', '\x19', '\x34', '\x57', '\x55', '\x00', '\x00',
        '\x66', '\xc6', '\x67', '\x63', '\x60', '\xd0', '\xa0', '\x00',
        '\xa6', '\xc9', '\x66', '\xc6', '\x05', '\x05', '\x05', '\x05',
        '\xa6', '\xc9', '\xa7', '\x71', '\x05', '\x0d', '\x0a', '\x00',
        '\x19', '\x34', '\xd0', '\x32', '\xb0', '\x60', '\xf0', '\x00',
        '\xae', '\x73', '\x19', '\x34', '\x05', '\x05', '\x05', '\x05',
        '\xae', '\x73', '\x8a', '\x4a', '\x05', '\x05', '\x0a', '\x00',
    }}, yFlipBc1InPlace, "checkerboard-odd.png", {InPlaceInit, {
        '\x66', '\xc6', '\x67', '\x63', '\x00', '\xa0', '\xd0', '\x60',
        '\xa6', '\xc9', '\x66', '\xc6', '\x05', '\x05', '\x05', '\x05',
        '\xa6', '\xc9', '\xa7', '\x71', '\x00', '\x0a', '\x0d', '\x05',
        '\x19', '\x34', '\xd0', '\x32', '\x00', '\xf0', '\x60', '\xb0',
        '\xae', '\x73', '\x19', '\x34', '\x05', '\x05', '\x05', '\x05',
        '\xae', '\x73', '\x8a', '\x4a', '\x00', '\x0a', '\x05', '\x05',
        '\x45', '\xc6', '\x2c', '\x3e', '\xa0', '\x00', '\x55', '\x55',
        '\x25', '\xe2', '\x8a', '\x77', '\x0f', '\x0f', '\xf5', '\xf5',
        '\x66', '\xbe', '\xa6', '\xb9', '\x55', '\x55', '\x00', '\x0a',
        '\xa6', '\xc1', '\x18', '\x34', '\x55', '\x55', '\x00', '\x00',
        '\xc5', '\xc9', '\x38', '\x34', '\xf5', '\xf5', '\x50', '\x50',
        '\x8e', '\x73', '\x19', '\x34', '\x00', '\x00', '\x55', '\x57',
        '\x8c', '\x3e', '\x8a', '\x33', '\x90', '\x50', '\xa0', '\x00',
        '\x66', '\xc6', '\x8c', '\x3e', '\x05', '\x05', '\x05', '\x05',
        '\x66', '\xc6', '\xe7', '\x73', '\x06', '\x0b', '\x02', '\x00',
        '\xa6', '\xc9', '\xa7', '\x71', '\x50', '\x50', '\xa0', '\x00',
        '\xa6', '\xc9', '\x19', '\x34', '\x50', '\x50', '\x50', '\x50',
        '\x19', '\x34', '\x32', '\x33', '\x05', '\x07', '\x0f', '\x00'
    }}},
    {"BC1", CompressedPixelFormat::Bc1RGBUnorm, {1, 4}, {InPlaceInit, {
        /* ./extract-interesting-blocks.py kodim23_bc1.dds bc1.png --offset 139
           (image taken from the bcdec repository test files) */
        /* [50, 53], 1.792 */
        '\x79', '\xd6', '\xa7', '\x39', '\x5c', '\x55', '\xd5', '\x35',
        /* [32, 46], 1.784 */
        '\xdd', '\xff', '\xa8', '\x6b', '\x55', '\x95', '\x25', '\x09',
        /* [48, 61], 1.780 */
        '\xba', '\xe6', '\x07', '\x52', '\x00', '\x00', '\x2a', '\xd5',
        /* [132, 47], 1.780 */
        '\x7a', '\xfe', '\x46', '\x81', '\xe0', '\x78', '\xd7', '\x2d',
    }}, yFlipBc1InPlace, "bc1.png", {InPlaceInit, {
        '\x7a', '\xfe', '\x46', '\x81', '\x2d', '\xd7', '\x78', '\xe0',
        '\xba', '\xe6', '\x07', '\x52', '\xd5', '\x2a', '\x00', '\x00',
        '\xdd', '\xff', '\xa8', '\x6b', '\x09', '\x25', '\x95', '\x55',
        '\x79', '\xd6', '\xa7', '\x39', '\x35', '\xd5', '\x55', '\x5c'
    }}},
    {"BC2", CompressedPixelFormat::Bc2RGBAUnorm, {1, 4}, {InPlaceInit, {
        /* ./extract-interesting-blocks.py testcard_bc2.dds bc2.png --offset 88
           (image taken from the bcdec repository test files) */
        /* [7, 52], 2.427 */
        '\xf8', '\xff', '\x91', '\xff', '\x20', '\xfa', '\x00', '\xc4',
        '\xff', '\xff', '\x0f', '\xb3', '\x55', '\x55', '\x54', '\x50',
        /* [6, 51], 2.427 */
        '\x90', '\xff', '\x20', '\xfb', '\x00', '\xd4', '\x00', '\x60',
        '\xff', '\xff', '\x0f', '\xb3', '\x54', '\x54', '\x50', '\x40',
        /* [53, 8], 2.412 */
        '\x6d', '\x00', '\xdf', '\x06', '\xff', '\x7e', '\xff', '\xff',
        '\xff', '\xff', '\xf6', '\x63', '\x05', '\x15', '\x55', '\x55',
        /* [10, 8], 2.412 */
        '\x00', '\xd6', '\x60', '\xfd', '\xe7', '\xff', '\xff', '\xff',
        '\xff', '\xff', '\xf6', '\x63', '\x50', '\x54', '\x55', '\x55',
    }}, yFlipBc2InPlace, "bc2.png", {InPlaceInit, {
        '\xff', '\xff', '\xe7', '\xff', '\x60', '\xfd', '\x00', '\xd6',
        '\xff', '\xff', '\xf6', '\x63', '\x55', '\x55', '\x54', '\x50',
        '\xff', '\xff', '\xff', '\x7e', '\xdf', '\x06', '\x6d', '\x00',
        '\xff', '\xff', '\xf6', '\x63', '\x55', '\x55', '\x15', '\x05',
        '\x00', '\x60', '\x00', '\xd4', '\x20', '\xfb', '\x90', '\xff',
        '\xff', '\xff', '\x0f', '\xb3', '\x40', '\x50', '\x54', '\x54',
        '\x00', '\xc4', '\x20', '\xfa', '\x91', '\xff', '\xf8', '\xff',
        '\xff', '\xff', '\x0f', '\xb3', '\x50', '\x54', '\x55', '\x55'
    }}},
    {"BC3", CompressedPixelFormat::Bc3RGBAUnorm, {1, 4}, {InPlaceInit, {
        /* ./extract-interesting-blocks.py dice_bc3.dds bc3.png --offset 148
           (image taken from the bcdec repository test files) */
        /* [105, 42], 2.392 */
        '\x26', '\x98', '\xb6', '\x0d', '\x00', '\x23', '\x99', '\x24',
        '\x8e', '\xfb', '\x00', '\x18', '\x55', '\x00', '\x02', '\xaa',
        /* [121, 122], 2.388 */
        '\x0d', '\x5f', '\x29', '\x57', '\x4e', '\x9c', '\x30', '\xc1',
        '\x8b', '\xd6', '\x00', '\x00', '\x00', '\x00', '\x00', '\x40',
        /* [160, 22], 2.388 */
        '\x02', '\x4a', '\x1c', '\x5c', '\xca', '\xe5', '\x90', '\x52',
        '\x72', '\x97', '\x00', '\x00', '\x40', '\x40', '\x00', '\x00',
        /* [96, 131], 2.376 */
        '\x03', '\x32', '\x49', '\xba', '\x6d', '\xb6', '\x6d', '\xdb',
        '\x0c', '\xe7', '\x00', '\x00', '\x00', '\x00', '\x51', '\x55',
    }}, yFlipBc3InPlace, "bc3.png", {InPlaceInit, {
        '\x03', '\x32', '\xb6', '\x6d', '\xdb', '\xdb', '\x96', '\xa4',
        '\x0c', '\xe7', '\x00', '\x00', '\x55', '\x51', '\x00', '\x00',
        '\x02', '\x4a', '\x29', '\x55', '\x0e', '\xa5', '\xcc', '\xc1',
        '\x72', '\x97', '\x00', '\x00', '\x00', '\x00', '\x40', '\x40',
        '\x0d', '\x5f', '\x13', '\xcc', '\x09', '\xe5', '\x94', '\x72',
        '\x8b', '\xd6', '\x00', '\x00', '\x40', '\x00', '\x00', '\x00',
        '\x26', '\x98', '\x49', '\x32', '\x92', '\x00', '\x60', '\xdb',
        '\x8e', '\xfb', '\x00', '\x18', '\xaa', '\x02', '\x00', '\x55'
    }}},
    {"BC4", CompressedPixelFormat::Bc4RUnorm, {1, 4}, {InPlaceInit, {
        /* ./extract-interesting-blocks.py dice_bc4.dds bc4.png
           (image taken from the bcdec repository test files) */
        /* [88, 130], 1.000 */
        '\xec', '\xed', '\x3e', '\x62', '\xdb', '\xb6', '\x6d', '\xdb',
        /* [87, 129], 1.000 */
        '\xd9', '\xec', '\xa3', '\xd0', '\x70', '\x7e', '\x62', '\xfb',
        /* [82, 125], 1.000 */
        '\xdd', '\xfc', '\xa7', '\xe0', '\x4c', '\x36', '\x67', '\x9b',
        /* [81, 124], 1.000 */
        '\xe9', '\xf5', '\x76', '\x60', '\x7f', '\xb6', '\x67', '\xfb',
    }}, yFlipBc4InPlace, "bc4.png", {InPlaceInit, {
        '\xe9', '\xf5', '\xb6', '\x6f', '\x7b', '\xf6', '\x67', '\x07',
        '\xdd', '\xfc', '\xb6', '\x69', '\x73', '\xce', '\x74', '\x0a',
        '\xd9', '\xec', '\xb6', '\xef', '\x27', '\x0d', '\x37', '\x0a',
        '\xec', '\xed', '\xb6', '\x6d', '\xdb', '\xb6', '\xed', '\x23'
    }}},
    {"BC5", CompressedPixelFormat::Bc5RGUnorm, {1, 4}, {InPlaceInit, {
        /* ./extract-interesting-blocks.py dice_bc5.dds bc5.png --offset 26
           (image taken from the bcdec repository test files) */
        /* [120, 124], 2.000 */
        '\xd3', '\xdf', '\x58', '\xbf', '\xda', '\xb1', '\x7d', '\xdb',
        '\xd3', '\xdf', '\x58', '\xbf', '\xda', '\xb1', '\x7d', '\xdb',
        /* [81, 124], 2.000 */
        '\xeb', '\xf6', '\x76', '\x60', '\x7f', '\xb6', '\x67', '\xfb',
        '\xeb', '\xf6', '\x76', '\x60', '\x7f', '\xb6', '\x67', '\xfb',
        /* [121, 123], 2.000 */
        '\xd4', '\xe2', '\xc8', '\x1d', '\xdb', '\xb3', '\x6d', '\xdb',
        '\xd4', '\xe2', '\xc8', '\x1d', '\xdb', '\xb3', '\x6d', '\xdb',
        /* [81, 123], 2.000 */
        '\xd7', '\xf3', '\x9d', '\x10', '\x4e', '\x2f', '\xe7', '\x77',
        '\xd7', '\xf3', '\x9d', '\x10', '\x4e', '\x2f', '\xe7', '\x77',
    }}, yFlipBc5InPlace, "bc5.png", {InPlaceInit, {
        '\xd7', '\xf3', '\x7e', '\xf7', '\x72', '\xe1', '\xd4', '\x09',
        '\xd7', '\xf3', '\x7e', '\xf7', '\x72', '\xe1', '\xd4', '\x09',
        '\xd4', '\xe2', '\xb6', '\x3d', '\xdb', '\xb1', '\x8d', '\xdc',
        '\xd4', '\xe2', '\xb6', '\x3d', '\xdb', '\xb1', '\x8d', '\xdc',
        '\xeb', '\xf6', '\xb6', '\x6f', '\x7b', '\xf6', '\x67', '\x07',
        '\xeb', '\xf6', '\xb6', '\x6f', '\x7b', '\xf6', '\x67', '\x07',
        '\xd3', '\xdf', '\xb7', '\x1d', '\xdb', '\xab', '\x8d', '\xf5',
        '\xd3', '\xdf', '\xb7', '\x1d', '\xdb', '\xab', '\x8d', '\xf5'
    }}},
};

ColorBatchTest::ColorBatchTest() {
    addInstancedTests({&ColorBatchTest::yFlip},
        Containers::arraySize(YFlipData));

    addTests({&ColorBatchTest::yFlip3D,

              &ColorBatchTest::yFlipInvalidLastDimension});
}

void ColorBatchTest::yFlip() {
    auto&& data = YFlipData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Copy to a mutable array first to operate in-place */
    Containers::Array<char> blocks{Magnum::NoInit, data.input.size()};
    Utility::copy(data.input, blocks);
    /* Using expanded() instead of the constructor as it catches issues where
       the size would be smaller than the actual data  */
    data.function(stridedArrayView(blocks).expanded<0>(Containers::Size3D{
        std::size_t(data.blockCount.y()),
        std::size_t(data.blockCount.x()),
        compressedPixelFormatBlockDataSize(data.format)}));
    CORRADE_COMPARE_AS(blocks,
        data.expected,
        TestSuite::Compare::Container);

    /* Catch also ABI and interface mismatch errors */
    if(!(_converterManager.load("BcDecImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("BcDecImageConverter plugin can't be loaded, cannot test decoded image equality.");
    if(!(_importerManager.load("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_importerManager.load("PngImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / PngImporter plugin can't be loaded, cannot test decoded image equality.");

    /* The flipped output should be exactly the same after decoding as an
       Y-flipped decoded input */
    Containers::Pointer<Trade::AbstractImageConverter> decoder = _converterManager.loadAndInstantiate("BcDecImageConverter");
    Containers::Optional<Trade::ImageData2D> decoded = decoder->convert(CompressedImageView2D{data.format, data.blockCount*compressedPixelFormatBlockSize(data.format).xy(), blocks});
    CORRADE_VERIFY(decoded);

    if(decoded->format() == PixelFormat::RGBA8Unorm)
        CORRADE_COMPARE_WITH(decoded->pixels<Vector4ub>().flipped<0>(),
            Utility::Path::join(COLORBATCH_TEST_DIR, data.file),
            (DebugTools::CompareImageToFile{_importerManager, _converterManager}));
    else if(decoded->format() == PixelFormat::RG8Unorm)
        CORRADE_COMPARE_WITH(decoded->pixels<Vector2ub>().flipped<0>(),
            Utility::Path::join(COLORBATCH_TEST_DIR, data.file),
            (DebugTools::CompareImageToFile{_importerManager, _converterManager}));
    else if(decoded->format() == PixelFormat::R8Unorm)
        CORRADE_COMPARE_WITH(decoded->pixels<UnsignedByte>().flipped<0>(),
            Utility::Path::join(COLORBATCH_TEST_DIR, data.file),
            (DebugTools::CompareImageToFile{_importerManager, _converterManager}));
    else CORRADE_FAIL("Unexpected format" << decoded->format());
}

void ColorBatchTest::yFlip3D() {
    /* Copy to a mutable array first to operate in-place */
    Containers::Array<char> blocks{Magnum::NoInit, Containers::arraySize(CheckerboardBC1)};
    Utility::copy(CheckerboardBC1, blocks);

    /* The 2D 4x6 blocks image is turned into 4 slices of 1x6 blocks each.
       Y flipping should result in the exact same result as in the 2D case. */
    Containers::StridedArrayView4D<char> view{blocks,
        {4, 6, 1, 8},
        {1*8, /* Each slice is 1 block wide */
         4*8, /* Each row is still 4 blocks */
         8,   /* Each block is 8 bytes */
         1}
    };
    yFlipBc1InPlace(view);

    CORRADE_COMPARE_AS(blocks,
        Containers::arrayView(CheckerboardBC1Flipped),
        TestSuite::Compare::Container);
}

void ColorBatchTest::yFlipInvalidLastDimension() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* All formats delegate to the same template code with the assertions so
       it's enough to test just some */
    char data[32];

    Containers::String out;
    Error redirectError{&out};
    yFlipBc3InPlace(Containers::stridedArrayView(data).expanded<0>(Containers::Size3D{1, 4, 8}));
    yFlipBc1InPlace(Containers::stridedArrayView(data).expanded<0>(Containers::Size3D{1, 2, 16}).every({1, 1, 2}));
    CORRADE_COMPARE(out,
        "Math::yFlipBc3InPlace(): expected last dimension to be 16 bytes but got 8\n"
        "Math::yFlipBc1InPlace(): last dimension is not contiguous\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::ColorBatchTest)
