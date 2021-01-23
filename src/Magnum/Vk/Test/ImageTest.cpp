/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Magnum/PixelFormat.h"
#include "Magnum/Vk/ImageCreateInfo.h"
#include "Magnum/Vk/Integration.h"
#include "Magnum/Vk/PixelFormat.h"

#include "Magnum/Vk/Test/pixelFormatTraits.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct ImageTest: TestSuite::Tester {
    explicit ImageTest();

    template<class T> void createInfoConstruct();
    template<class T> void createInfoConstruct1D();
    template<class T> void createInfoConstruct2D();
    template<class T> void createInfoConstruct3D();
    template<class T> void createInfoConstruct1DArray();
    template<class T> void createInfoConstruct2DArray();
    template<class T> void createInfoConstructCubeMap();
    template<class T> void createInfoConstructCubeMapArray();
    void createInfoConstructNoInit();
    void createInfoConstructFromVk();

    void aspectsFor();
    void aspectsForInvalidFormat();
    void aspectsForGenericFormat();

    void constructNoCreate();
    void constructCopy();

    void dedicatedMemoryNotDedicated();

    void debugAspect();
    void debugAspects();
};

ImageTest::ImageTest() {
    addTests({&ImageTest::createInfoConstruct<PixelFormat>,
              &ImageTest::createInfoConstruct<Magnum::PixelFormat>,
              &ImageTest::createInfoConstruct<Magnum::CompressedPixelFormat>,
              &ImageTest::createInfoConstruct1D<PixelFormat>,
              &ImageTest::createInfoConstruct1D<Magnum::PixelFormat>,
              &ImageTest::createInfoConstruct1D<Magnum::CompressedPixelFormat>,
              &ImageTest::createInfoConstruct2D<PixelFormat>,
              &ImageTest::createInfoConstruct2D<Magnum::PixelFormat>,
              &ImageTest::createInfoConstruct2D<Magnum::CompressedPixelFormat>,
              &ImageTest::createInfoConstruct3D<PixelFormat>,
              &ImageTest::createInfoConstruct3D<Magnum::PixelFormat>,
              &ImageTest::createInfoConstruct3D<Magnum::CompressedPixelFormat>,
              &ImageTest::createInfoConstruct1DArray<PixelFormat>,
              &ImageTest::createInfoConstruct1DArray<Magnum::PixelFormat>,
              &ImageTest::createInfoConstruct1DArray<Magnum::CompressedPixelFormat>,
              &ImageTest::createInfoConstruct2DArray<PixelFormat>,
              &ImageTest::createInfoConstruct2DArray<Magnum::PixelFormat>,
              &ImageTest::createInfoConstruct2DArray<Magnum::CompressedPixelFormat>,
              &ImageTest::createInfoConstructCubeMap<PixelFormat>,
              &ImageTest::createInfoConstructCubeMap<Magnum::PixelFormat>,
              &ImageTest::createInfoConstructCubeMap<Magnum::CompressedPixelFormat>,
              &ImageTest::createInfoConstructCubeMapArray<PixelFormat>,
              &ImageTest::createInfoConstructCubeMapArray<Magnum::PixelFormat>,
              &ImageTest::createInfoConstructCubeMapArray<Magnum::CompressedPixelFormat>,
              &ImageTest::createInfoConstructNoInit,
              &ImageTest::createInfoConstructFromVk,

              &ImageTest::aspectsFor,
              &ImageTest::aspectsForInvalidFormat,
              &ImageTest::aspectsForGenericFormat,

              &ImageTest::constructNoCreate,
              &ImageTest::constructCopy,

              &ImageTest::dedicatedMemoryNotDedicated,

              &ImageTest::debugAspect,
              &ImageTest::debugAspects});
}

template<class T> void ImageTest::createInfoConstruct() {
    setTestCaseTemplateName(PixelFormatTraits<T>::name());

    ImageCreateInfo info{VK_IMAGE_TYPE_2D, ImageUsage::Sampled, PixelFormatTraits<T>::format(), {256, 128, 1}, 6, 8, 16, ImageLayout::Undefined, ImageCreateInfo::Flag::CubeCompatible};
    CORRADE_COMPARE(info->flags, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);
    CORRADE_COMPARE(info->imageType, VK_IMAGE_TYPE_2D);
    CORRADE_COMPARE(info->format, PixelFormatTraits<T>::expected());
    CORRADE_COMPARE(Vector3i(info->extent), (Vector3i{256, 128, 1}));
    CORRADE_COMPARE(info->mipLevels, 8);
    CORRADE_COMPARE(info->arrayLayers, 6);
    CORRADE_COMPARE(info->samples, VK_SAMPLE_COUNT_16_BIT);
    CORRADE_COMPARE(info->usage, VK_IMAGE_USAGE_SAMPLED_BIT);
    CORRADE_COMPARE(info->tiling, VK_IMAGE_TILING_OPTIMAL);
    CORRADE_COMPARE(info->sharingMode, VK_SHARING_MODE_EXCLUSIVE);
    CORRADE_COMPARE(info->initialLayout, VK_IMAGE_LAYOUT_UNDEFINED);
}

template<class T> void ImageTest::createInfoConstruct1D() {
    setTestCaseTemplateName(PixelFormatTraits<T>::name());

    ImageCreateInfo1D info{ImageUsage::Storage, PixelFormatTraits<T>::format(), 256, 8, 16, ImageCreateInfo::Flag::MutableFormat};
    CORRADE_COMPARE(info->flags, VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT);
    CORRADE_COMPARE(info->imageType, VK_IMAGE_TYPE_1D);
    CORRADE_COMPARE(info->format, PixelFormatTraits<T>::expected());
    CORRADE_COMPARE(Vector3i(info->extent), (Vector3i{256, 1, 1}));
    CORRADE_COMPARE(info->mipLevels, 8);
    CORRADE_COMPARE(info->arrayLayers, 1);
    CORRADE_COMPARE(info->samples, VK_SAMPLE_COUNT_16_BIT);
    CORRADE_COMPARE(info->usage, VK_IMAGE_USAGE_STORAGE_BIT);
}

template<class T> void ImageTest::createInfoConstruct2D() {
    setTestCaseTemplateName(PixelFormatTraits<T>::name());

    ImageCreateInfo2D info{ImageUsage::TransferDestination, PixelFormatTraits<T>::format(), {256, 64}, 8, 16, ImageCreateInfo::Flag::MutableFormat};
    CORRADE_COMPARE(info->flags, VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT);
    CORRADE_COMPARE(info->imageType, VK_IMAGE_TYPE_2D);
    CORRADE_COMPARE(info->format, PixelFormatTraits<T>::expected());
    CORRADE_COMPARE(Vector3i(info->extent), (Vector3i{256, 64, 1}));
    CORRADE_COMPARE(info->mipLevels, 8);
    CORRADE_COMPARE(info->arrayLayers, 1);
    CORRADE_COMPARE(info->samples, VK_SAMPLE_COUNT_16_BIT);
    CORRADE_COMPARE(info->usage, VK_IMAGE_USAGE_TRANSFER_DST_BIT);
}

template<class T> void ImageTest::createInfoConstruct3D() {
    setTestCaseTemplateName(PixelFormatTraits<T>::name());

    ImageCreateInfo3D info{ImageUsage::InputAttachment, PixelFormatTraits<T>::format(), {256, 64, 32}, 8, 16, ImageCreateInfo::Flag::MutableFormat};
    CORRADE_COMPARE(info->flags, VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT);
    CORRADE_COMPARE(info->imageType, VK_IMAGE_TYPE_3D);
    CORRADE_COMPARE(info->format, PixelFormatTraits<T>::expected());
    CORRADE_COMPARE(Vector3i(info->extent), (Vector3i{256, 64, 32}));
    CORRADE_COMPARE(info->mipLevels, 8);
    CORRADE_COMPARE(info->arrayLayers, 1);
    CORRADE_COMPARE(info->samples, VK_SAMPLE_COUNT_16_BIT);
    CORRADE_COMPARE(info->usage, VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
}

template<class T> void ImageTest::createInfoConstruct1DArray() {
    setTestCaseTemplateName(PixelFormatTraits<T>::name());

    ImageCreateInfo1DArray info{ImageUsage::TransferDestination, PixelFormatTraits<T>::format(), {256, 64}, 8, 16, ImageCreateInfo::Flag::MutableFormat};
    CORRADE_COMPARE(info->flags, VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT);
    CORRADE_COMPARE(info->imageType, VK_IMAGE_TYPE_1D);
    CORRADE_COMPARE(info->format, PixelFormatTraits<T>::expected());
    CORRADE_COMPARE(Vector3i(info->extent), (Vector3i{256, 1, 1}));
    CORRADE_COMPARE(info->mipLevels, 8);
    CORRADE_COMPARE(info->arrayLayers, 64);
    CORRADE_COMPARE(info->samples, VK_SAMPLE_COUNT_16_BIT);
    CORRADE_COMPARE(info->usage, VK_IMAGE_USAGE_TRANSFER_DST_BIT);
}

template<class T> void ImageTest::createInfoConstruct2DArray() {
    setTestCaseTemplateName(PixelFormatTraits<T>::name());

    ImageCreateInfo2DArray info{ImageUsage::TransferDestination, PixelFormatTraits<T>::format(), {256, 64, 32}, 8, 16, ImageCreateInfo::Flag::MutableFormat};
    CORRADE_COMPARE(info->flags, VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT);
    CORRADE_COMPARE(info->imageType, VK_IMAGE_TYPE_2D);
    CORRADE_COMPARE(info->format, PixelFormatTraits<T>::expected());
    CORRADE_COMPARE(Vector3i(info->extent), (Vector3i{256, 64, 1}));
    CORRADE_COMPARE(info->mipLevels, 8);
    CORRADE_COMPARE(info->arrayLayers, 32);
    CORRADE_COMPARE(info->samples, VK_SAMPLE_COUNT_16_BIT);
    CORRADE_COMPARE(info->usage, VK_IMAGE_USAGE_TRANSFER_DST_BIT);
}

template<class T> void ImageTest::createInfoConstructCubeMap() {
    setTestCaseTemplateName(PixelFormatTraits<T>::name());

    ImageCreateInfoCubeMap info{ImageUsage::TransferDestination, PixelFormatTraits<T>::format(), {256, 256}, 8, 16, ImageCreateInfo::Flag::MutableFormat};
    CORRADE_COMPARE(info->flags, VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT|VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);
    CORRADE_COMPARE(info->imageType, VK_IMAGE_TYPE_2D);
    CORRADE_COMPARE(info->format, PixelFormatTraits<T>::expected());
    CORRADE_COMPARE(Vector3i(info->extent), (Vector3i{256, 256, 1}));
    CORRADE_COMPARE(info->mipLevels, 8);
    CORRADE_COMPARE(info->arrayLayers, 6);
    CORRADE_COMPARE(info->samples, VK_SAMPLE_COUNT_16_BIT);
    CORRADE_COMPARE(info->usage, VK_IMAGE_USAGE_TRANSFER_DST_BIT);
}

template<class T> void ImageTest::createInfoConstructCubeMapArray() {
    setTestCaseTemplateName(PixelFormatTraits<T>::name());

    ImageCreateInfoCubeMapArray info{ImageUsage::TransferDestination, PixelFormatTraits<T>::format(), {256, 256, 36}, 8, 16, ImageCreateInfo::Flag::MutableFormat};
    CORRADE_COMPARE(info->flags, VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT|VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);
    CORRADE_COMPARE(info->imageType, VK_IMAGE_TYPE_2D);
    CORRADE_COMPARE(info->format, PixelFormatTraits<T>::expected());
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

void ImageTest::aspectsFor() {
    CORRADE_COMPARE(imageAspectsFor(PixelFormat::RGBA8Unorm), ImageAspect::Color);
    CORRADE_COMPARE(imageAspectsFor(PixelFormat::Depth32FStencil8UI), ImageAspect::Depth|ImageAspect::Stencil);
    CORRADE_COMPARE(imageAspectsFor(PixelFormat::Depth16Unorm), ImageAspect::Depth);
    CORRADE_COMPARE(imageAspectsFor(PixelFormat::Stencil8UI), ImageAspect::Stencil);
}

void ImageTest::aspectsForInvalidFormat() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    imageAspectsFor(PixelFormat{});
    CORRADE_COMPARE(out.str(), "Vk::imageAspectsFor(): can't get an aspect for Vk::PixelFormat(0)\n");
}

void ImageTest::aspectsForGenericFormat() {
    /* No generic depth/stencil formats yet, can't test */
    CORRADE_COMPARE(imageAspectsFor(Magnum::PixelFormat::R16I), ImageAspect::Color);
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
    CORRADE_VERIFY(!std::is_copy_constructible<Image>{});
    CORRADE_VERIFY(!std::is_copy_assignable<Image>{});
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

void ImageTest::debugAspect() {
    std::ostringstream out;
    Debug{&out} << ImageAspect::Depth << ImageAspect(0xdeadcafe);
    CORRADE_COMPARE(out.str(), "Vk::ImageAspect::Depth Vk::ImageAspect(0xdeadcafe)\n");
}

void ImageTest::debugAspects() {
    std::ostringstream out;
    Debug{&out} << (ImageAspect::Stencil|ImageAspect(0xf0)) << ImageAspects{};
    CORRADE_COMPARE(out.str(), "Vk::ImageAspect::Stencil|Vk::ImageAspect(0xf0) Vk::ImageAspects{}\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::ImageTest)
