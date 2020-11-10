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

#include <new>
#include <sstream>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Vk/Image.h"
#include "Magnum/Vk/Integration.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct ImageTest: TestSuite::Tester {
    explicit ImageTest();

    void createInfoConstruct();
    void createInfoConstruct1D();
    void createInfoConstruct2D();
    void createInfoConstruct3D();
    void createInfoConstruct1DArray();
    void createInfoConstruct2DArray();
    void createInfoConstructCubeMap();
    void createInfoConstructCubeMapArray();
    void createInfoConstructNoInit();
    void createInfoConstructFromVk();

    void constructNoCreate();
    void constructCopy();

    void dedicatedMemoryNotDedicated();
};

ImageTest::ImageTest() {
    addTests({&ImageTest::createInfoConstruct,
              &ImageTest::createInfoConstruct1D,
              &ImageTest::createInfoConstruct2D,
              &ImageTest::createInfoConstruct3D,
              &ImageTest::createInfoConstruct1DArray,
              &ImageTest::createInfoConstruct2DArray,
              &ImageTest::createInfoConstructCubeMap,
              &ImageTest::createInfoConstructCubeMapArray,
              &ImageTest::createInfoConstructNoInit,
              &ImageTest::createInfoConstructFromVk,

              &ImageTest::constructNoCreate,
              &ImageTest::constructCopy,

              &ImageTest::dedicatedMemoryNotDedicated});
}

void ImageTest::createInfoConstruct() {
    ImageCreateInfo info{VK_IMAGE_TYPE_2D, ImageUsage::Sampled, VK_FORMAT_R8G8B8A8_UNORM, {256, 128, 1}, 6, 8, 16, ImageCreateInfo::Flag::CubeCompatible};
    CORRADE_COMPARE(info->flags, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);
    CORRADE_COMPARE(info->imageType, VK_IMAGE_TYPE_2D);
    CORRADE_COMPARE(info->format, VK_FORMAT_R8G8B8A8_UNORM);
    CORRADE_COMPARE(Vector3i(info->extent), (Vector3i{256, 128, 1}));
    CORRADE_COMPARE(info->mipLevels, 8);
    CORRADE_COMPARE(info->arrayLayers, 6);
    CORRADE_COMPARE(info->samples, VK_SAMPLE_COUNT_16_BIT);
    CORRADE_COMPARE(info->usage, VK_IMAGE_USAGE_SAMPLED_BIT);
    CORRADE_COMPARE(info->tiling, VK_IMAGE_TILING_OPTIMAL);
    CORRADE_COMPARE(info->sharingMode, VK_SHARING_MODE_EXCLUSIVE);
    CORRADE_COMPARE(info->initialLayout, VK_IMAGE_LAYOUT_UNDEFINED);
}

void ImageTest::createInfoConstruct1D() {
    ImageCreateInfo1D info{ImageUsage::Storage, VK_FORMAT_R8G8B8A8_UNORM, 256, 8, 16, ImageCreateInfo::Flag::MutableFormat};
    CORRADE_COMPARE(info->flags, VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT);
    CORRADE_COMPARE(info->imageType, VK_IMAGE_TYPE_1D);
    CORRADE_COMPARE(info->format, VK_FORMAT_R8G8B8A8_UNORM);
    CORRADE_COMPARE(Vector3i(info->extent), (Vector3i{256, 1, 1}));
    CORRADE_COMPARE(info->mipLevels, 8);
    CORRADE_COMPARE(info->arrayLayers, 1);
    CORRADE_COMPARE(info->samples, VK_SAMPLE_COUNT_16_BIT);
    CORRADE_COMPARE(info->usage, VK_IMAGE_USAGE_STORAGE_BIT);
}

void ImageTest::createInfoConstruct2D() {
    ImageCreateInfo2D info{ImageUsage::TransferDestination, VK_FORMAT_R8G8B8A8_UNORM, {256, 64}, 8, 16, ImageCreateInfo::Flag::MutableFormat};
    CORRADE_COMPARE(info->flags, VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT);
    CORRADE_COMPARE(info->imageType, VK_IMAGE_TYPE_2D);
    CORRADE_COMPARE(info->format, VK_FORMAT_R8G8B8A8_UNORM);
    CORRADE_COMPARE(Vector3i(info->extent), (Vector3i{256, 64, 1}));
    CORRADE_COMPARE(info->mipLevels, 8);
    CORRADE_COMPARE(info->arrayLayers, 1);
    CORRADE_COMPARE(info->samples, VK_SAMPLE_COUNT_16_BIT);
    CORRADE_COMPARE(info->usage, VK_IMAGE_USAGE_TRANSFER_DST_BIT);
}

void ImageTest::createInfoConstruct3D() {
    ImageCreateInfo3D info{ImageUsage::InputAttachment, VK_FORMAT_R8G8B8A8_UNORM, {256, 64, 32}, 8, 16, ImageCreateInfo::Flag::MutableFormat};
    CORRADE_COMPARE(info->flags, VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT);
    CORRADE_COMPARE(info->imageType, VK_IMAGE_TYPE_3D);
    CORRADE_COMPARE(info->format, VK_FORMAT_R8G8B8A8_UNORM);
    CORRADE_COMPARE(Vector3i(info->extent), (Vector3i{256, 64, 32}));
    CORRADE_COMPARE(info->mipLevels, 8);
    CORRADE_COMPARE(info->arrayLayers, 1);
    CORRADE_COMPARE(info->samples, VK_SAMPLE_COUNT_16_BIT);
    CORRADE_COMPARE(info->usage, VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
}

void ImageTest::createInfoConstruct1DArray() {
    ImageCreateInfo1DArray info{ImageUsage::TransferDestination, VK_FORMAT_R8G8B8A8_UNORM, {256, 64}, 8, 16, ImageCreateInfo::Flag::MutableFormat};
    CORRADE_COMPARE(info->flags, VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT);
    CORRADE_COMPARE(info->imageType, VK_IMAGE_TYPE_1D);
    CORRADE_COMPARE(info->format, VK_FORMAT_R8G8B8A8_UNORM);
    CORRADE_COMPARE(Vector3i(info->extent), (Vector3i{256, 1, 1}));
    CORRADE_COMPARE(info->mipLevels, 8);
    CORRADE_COMPARE(info->arrayLayers, 64);
    CORRADE_COMPARE(info->samples, VK_SAMPLE_COUNT_16_BIT);
    CORRADE_COMPARE(info->usage, VK_IMAGE_USAGE_TRANSFER_DST_BIT);
}

void ImageTest::createInfoConstruct2DArray() {
    ImageCreateInfo2DArray info{ImageUsage::TransferDestination, VK_FORMAT_R8G8B8A8_UNORM, {256, 64, 32}, 8, 16, ImageCreateInfo::Flag::MutableFormat};
    CORRADE_COMPARE(info->flags, VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT);
    CORRADE_COMPARE(info->imageType, VK_IMAGE_TYPE_2D);
    CORRADE_COMPARE(info->format, VK_FORMAT_R8G8B8A8_UNORM);
    CORRADE_COMPARE(Vector3i(info->extent), (Vector3i{256, 64, 1}));
    CORRADE_COMPARE(info->mipLevels, 8);
    CORRADE_COMPARE(info->arrayLayers, 32);
    CORRADE_COMPARE(info->samples, VK_SAMPLE_COUNT_16_BIT);
    CORRADE_COMPARE(info->usage, VK_IMAGE_USAGE_TRANSFER_DST_BIT);
}

void ImageTest::createInfoConstructCubeMap() {
    ImageCreateInfoCubeMap info{ImageUsage::TransferDestination, VK_FORMAT_R8G8B8A8_UNORM, {256, 256}, 8, 16, ImageCreateInfo::Flag::MutableFormat};
    CORRADE_COMPARE(info->flags, VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT|VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);
    CORRADE_COMPARE(info->imageType, VK_IMAGE_TYPE_2D);
    CORRADE_COMPARE(info->format, VK_FORMAT_R8G8B8A8_UNORM);
    CORRADE_COMPARE(Vector3i(info->extent), (Vector3i{256, 256, 1}));
    CORRADE_COMPARE(info->mipLevels, 8);
    CORRADE_COMPARE(info->arrayLayers, 6);
    CORRADE_COMPARE(info->samples, VK_SAMPLE_COUNT_16_BIT);
    CORRADE_COMPARE(info->usage, VK_IMAGE_USAGE_TRANSFER_DST_BIT);
}

void ImageTest::createInfoConstructCubeMapArray() {
    ImageCreateInfoCubeMapArray info{ImageUsage::TransferDestination, VK_FORMAT_R8G8B8A8_UNORM, {256, 256, 36}, 8, 16, ImageCreateInfo::Flag::MutableFormat};
    CORRADE_COMPARE(info->flags, VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT|VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);
    CORRADE_COMPARE(info->imageType, VK_IMAGE_TYPE_2D);
    CORRADE_COMPARE(info->format, VK_FORMAT_R8G8B8A8_UNORM);
    CORRADE_COMPARE(Vector3i(info->extent), (Vector3i{256, 256, 1}));
    CORRADE_COMPARE(info->mipLevels, 8);
    CORRADE_COMPARE(info->arrayLayers, 36);
    CORRADE_COMPARE(info->samples, VK_SAMPLE_COUNT_16_BIT);
    CORRADE_COMPARE(info->usage, VK_IMAGE_USAGE_TRANSFER_DST_BIT);
}

void ImageTest::createInfoConstructNoInit() {
    ImageCreateInfo info{NoInit};
    info->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    new(&info) ImageCreateInfo{NoInit};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);

    CORRADE_VERIFY((std::is_nothrow_constructible<ImageCreateInfo, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, ImageCreateInfo>::value));
}

void ImageTest::createInfoConstructFromVk() {
    VkImageCreateInfo vkInfo;
    vkInfo.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;

    ImageCreateInfo info{vkInfo};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
}

void ImageTest::constructNoCreate() {
    {
        Image image{NoCreate};
        CORRADE_VERIFY(!image.handle());
    }

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoCreateT, Image>::value));
}

void ImageTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<Image, const Image&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Image, const Image&>{}));
}

void ImageTest::dedicatedMemoryNotDedicated() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Image image{NoCreate};
    CORRADE_VERIFY(!image.hasDedicatedMemory());

    std::ostringstream out;
    Error redirectError{&out};
    image.dedicatedMemory();
    CORRADE_COMPARE(out.str(), "Vk::Image::dedicatedMemory(): image doesn't have a dedicated memory\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::ImageTest)
