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
#include <Corrade/Containers/Array.h>
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

    /* While *ConstructFromVk() tests that going from VkFromThing -> Vk::Thing
       -> VkToThing doesn't result in information loss, the *ConvertToVk()
       tests additionally check that all calls both on our APIs and by editing
       the contained structure are correctly propagated to the resulting
       structures. */

    void imageCopyConstruct();
    void imageCopyConstructNoInit();
    template<class From, class To> void imageCopyConstructFromVk();
    template<class T> void imageCopyConvertToVk();
    void imageCopyConvertDisallowed();

    void copyImageInfoConstruct();
    void copyImageInfoConstructNoInit();
    void copyImageInfoConstructFromVk();
    void copyImageInfoConvertToVk();

    void bufferImageCopyConstruct();
    void bufferImageCopyConstruct1D();
    void bufferImageCopyConstruct2D();
    void bufferImageCopyConstruct3D();
    void bufferImageCopyConstruct1DArray();
    void bufferImageCopyConstruct2DArray();
    void bufferImageCopyConstructCubeMap();
    void bufferImageCopyConstructCubeMapArray();
    void bufferImageCopyConstructNoInit();
    template<class From, class To> void bufferImageCopyConstructFromVk();
    template<class T> void bufferImageCopyConvertToVk();
    void bufferImageCopyConvertDisallowed();

    void copyBufferToImageInfoConstruct();
    void copyBufferToImageInfoConstructNoInit();
    void copyBufferToImageInfoConstructFromVk();
    void copyBufferToImageInfoConvertToVk();

    void copyImageToBufferInfoConstruct();
    void copyImageToBufferInfoConstructNoInit();
    void copyImageToBufferInfoConstructFromVk();
    void copyImageToBufferInfoConvertToVk();

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

              &ImageTest::imageCopyConstruct,
              &ImageTest::imageCopyConstructNoInit,
              &ImageTest::imageCopyConstructFromVk<VkImageCopy2KHR, VkImageCopy2KHR>,
              &ImageTest::imageCopyConstructFromVk<VkImageCopy, VkImageCopy2KHR>,
              &ImageTest::imageCopyConstructFromVk<VkImageCopy2KHR, VkImageCopy>,
              &ImageTest::imageCopyConstructFromVk<VkImageCopy, VkImageCopy>,
              &ImageTest::imageCopyConvertToVk<VkImageCopy2KHR>,
              &ImageTest::imageCopyConvertToVk<VkImageCopy>,
              &ImageTest::imageCopyConvertDisallowed,

              &ImageTest::copyImageInfoConstruct,
              &ImageTest::copyImageInfoConstructNoInit,
              &ImageTest::copyImageInfoConstructFromVk,
              &ImageTest::copyImageInfoConvertToVk,

              &ImageTest::bufferImageCopyConstruct,
              &ImageTest::bufferImageCopyConstruct1D,
              &ImageTest::bufferImageCopyConstruct2D,
              &ImageTest::bufferImageCopyConstruct3D,
              &ImageTest::bufferImageCopyConstruct1DArray,
              &ImageTest::bufferImageCopyConstruct2DArray,
              &ImageTest::bufferImageCopyConstructCubeMap,
              &ImageTest::bufferImageCopyConstructCubeMapArray,
              &ImageTest::bufferImageCopyConstructNoInit,
              &ImageTest::bufferImageCopyConstructFromVk<VkBufferImageCopy2KHR, VkBufferImageCopy2KHR>,
              &ImageTest::bufferImageCopyConstructFromVk<VkBufferImageCopy, VkBufferImageCopy2KHR>,
              &ImageTest::bufferImageCopyConstructFromVk<VkBufferImageCopy2KHR, VkBufferImageCopy>,
              &ImageTest::bufferImageCopyConstructFromVk<VkBufferImageCopy, VkBufferImageCopy>,
              &ImageTest::bufferImageCopyConvertToVk<VkBufferImageCopy2KHR>,
              &ImageTest::bufferImageCopyConvertToVk<VkBufferImageCopy>,
              &ImageTest::bufferImageCopyConvertDisallowed,

              &ImageTest::copyBufferToImageInfoConstruct,
              &ImageTest::copyBufferToImageInfoConstructNoInit,
              &ImageTest::copyBufferToImageInfoConstructFromVk,
              &ImageTest::copyBufferToImageInfoConvertToVk,

              &ImageTest::copyImageToBufferInfoConstruct,
              &ImageTest::copyImageToBufferInfoConstructNoInit,
              &ImageTest::copyImageToBufferInfoConstructFromVk,
              &ImageTest::copyImageToBufferInfoConvertToVk,

              &ImageTest::debugAspect,
              &ImageTest::debugAspects});
}

template<class> struct Traits;
#define _c(type)                                                            \
    template<> struct Traits<Vk ## type> {                                  \
        static const char* name() { return #type; }                         \
        static Vk ## type convert(const type& instance) {                   \
            return instance.vk ## type ();                                  \
        }                                                                   \
    };                                                                      \
    template<> struct Traits<Vk ## type ## 2KHR> {                          \
        static const char* name() { return #type "2KHR"; }                  \
        static Vk ## type ## 2KHR convert(const type& instance) {           \
            return instance;                                                \
        }                                                                   \
    };
_c(ImageCopy)
_c(BufferImageCopy)
#undef _c

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

void ImageTest::imageCopyConstruct() {
    ImageCopy copy{ImageAspect::Color|ImageAspect::Depth, 3, 5, 7, {9, 11, 13}, 4, 6, 8, {10, 12, 14}, {1, 2, 15}};
    CORRADE_COMPARE(copy->srcSubresource.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT|VK_IMAGE_ASPECT_DEPTH_BIT);
    CORRADE_COMPARE(copy->srcSubresource.mipLevel, 3);
    CORRADE_COMPARE(copy->srcSubresource.baseArrayLayer, 5);
    CORRADE_COMPARE(copy->srcSubresource.layerCount, 7);
    CORRADE_COMPARE(Vector3i{copy->srcOffset}, (Vector3i{9, 11, 13}));
    CORRADE_COMPARE(copy->dstSubresource.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT|VK_IMAGE_ASPECT_DEPTH_BIT);
    CORRADE_COMPARE(copy->dstSubresource.mipLevel, 4);
    CORRADE_COMPARE(copy->dstSubresource.baseArrayLayer, 6);
    CORRADE_COMPARE(copy->dstSubresource.layerCount, 8);
    CORRADE_COMPARE(Vector3i{copy->dstOffset}, (Vector3i{10, 12, 14}));
    CORRADE_COMPARE(Vector3i{copy->extent}, (Vector3i{1, 2, 15}));
}

void ImageTest::imageCopyConstructNoInit() {
    ImageCopy copy{NoInit};
    copy->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    new(&copy) ImageCopy{NoInit};
    CORRADE_COMPARE(copy->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);

    CORRADE_VERIFY((std::is_nothrow_constructible<ImageCopy, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, ImageCopy>::value));
}

template<class From, class To> void ImageTest::imageCopyConstructFromVk() {
    setTestCaseTemplateName({Traits<From>::name(), Traits<To>::name()});

    From from{};
    from.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    from.srcSubresource.mipLevel = 3;
    from.srcSubresource.baseArrayLayer = 5;
    from.srcSubresource.layerCount = 7;
    from.srcOffset = {9, 11, 13};
    /* Deliberately using a different src/dst aspect to verify it's not
       conflated */
    from.dstSubresource.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    from.dstSubresource.mipLevel = 4;
    from.dstSubresource.baseArrayLayer = 6;
    from.dstSubresource.layerCount = 8;
    from.dstOffset = {10, 12, 14};
    from.extent = {1, 2, 15};

    ImageCopy copy{from};
    To to = Traits<To>::convert(copy);
    CORRADE_COMPARE(to.srcSubresource.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT);
    CORRADE_COMPARE(to.srcSubresource.mipLevel, 3);
    CORRADE_COMPARE(to.srcSubresource.baseArrayLayer, 5);
    CORRADE_COMPARE(to.srcSubresource.layerCount, 7);
    CORRADE_COMPARE(Vector3i{to.srcOffset}, (Vector3i{9, 11, 13}));
    CORRADE_COMPARE(to.dstSubresource.aspectMask, VK_IMAGE_ASPECT_DEPTH_BIT);
    CORRADE_COMPARE(to.dstSubresource.mipLevel, 4);
    CORRADE_COMPARE(to.dstSubresource.baseArrayLayer, 6);
    CORRADE_COMPARE(to.dstSubresource.layerCount, 8);
    CORRADE_COMPARE(Vector3i{to.dstOffset}, (Vector3i{10, 12, 14}));
    CORRADE_COMPARE(Vector3i{to.extent}, (Vector3i{1, 2, 15}));
}

template<class T> void ImageTest::imageCopyConvertToVk() {
    ImageCopy copy{ImageAspect::Color|ImageAspect::Depth, 3, 5, 7, {9, 11, 13}, 4, 6, 8, {10, 12, 14}, {1, 2, 15}};

    T out = Traits<T>::convert(copy);
    CORRADE_COMPARE(out.srcSubresource.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT|VK_IMAGE_ASPECT_DEPTH_BIT);
    CORRADE_COMPARE(out.srcSubresource.mipLevel, 3);
    CORRADE_COMPARE(out.srcSubresource.baseArrayLayer, 5);
    CORRADE_COMPARE(out.srcSubresource.layerCount, 7);
    CORRADE_COMPARE(Vector3i{out.srcOffset}, (Vector3i{9, 11, 13}));
    CORRADE_COMPARE(out.dstSubresource.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT|VK_IMAGE_ASPECT_DEPTH_BIT);
    CORRADE_COMPARE(out.dstSubresource.mipLevel, 4);
    CORRADE_COMPARE(out.dstSubresource.baseArrayLayer, 6);
    CORRADE_COMPARE(out.dstSubresource.layerCount, 8);
    CORRADE_COMPARE(Vector3i{out.dstOffset}, (Vector3i{10, 12, 14}));
    CORRADE_COMPARE(Vector3i{out.extent}, (Vector3i{1, 2, 15}));
}

void ImageTest::imageCopyConvertDisallowed() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    ImageCopy copy{ImageAspect{}, 0, 0, 0, {}, 0, 0, 0, {}, {}};
    copy->pNext = &copy;

    std::ostringstream out;
    Error redirectError{&out};
    copy.vkImageCopy();
    CORRADE_COMPARE(out.str(), "Vk::ImageCopy: disallowing conversion to VkImageCopy with non-empty pNext to prevent information loss\n");
}

void ImageTest::copyImageInfoConstruct() {
    auto a = reinterpret_cast<VkImage>(0xdead);
    auto b = reinterpret_cast<VkImage>(0xcafe);

    CopyImageInfo info{a, ImageLayout::Preinitialized, b, ImageLayout::General, {
        {ImageAspect::Color, 3, 0, 0, {}, 0, 0, 0, {}, {}},
        {ImageAspect::Depth, 0, 5, 0, {}, 0, 0, 0, {}, {}}
    }};
    CORRADE_COMPARE(info->srcImage, a);
    CORRADE_COMPARE(info->srcImageLayout, VK_IMAGE_LAYOUT_PREINITIALIZED);
    CORRADE_COMPARE(info->dstImage, b);
    CORRADE_COMPARE(info->dstImageLayout, VK_IMAGE_LAYOUT_GENERAL);
    CORRADE_COMPARE(info->regionCount, 2);
    CORRADE_VERIFY(info->pRegions);
    CORRADE_COMPARE(info->pRegions[0].srcSubresource.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT);
    CORRADE_COMPARE(info->pRegions[0].srcSubresource.mipLevel, 3);
    CORRADE_COMPARE(info->pRegions[1].dstSubresource.aspectMask, VK_IMAGE_ASPECT_DEPTH_BIT);
    CORRADE_COMPARE(info->pRegions[1].srcSubresource.baseArrayLayer, 5);
}

void ImageTest::copyImageInfoConstructNoInit() {
    CopyImageInfo info{NoInit};
    info->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    new(&info) CopyImageInfo{NoInit};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);

    CORRADE_VERIFY((std::is_nothrow_constructible<CopyImageInfo, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, CopyImageInfo>::value));
}

void ImageTest::copyImageInfoConstructFromVk() {
    VkCopyImageInfo2KHR vkInfo;
    vkInfo.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;

    CopyImageInfo info{vkInfo};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
}

void ImageTest::copyImageInfoConvertToVk() {
    CopyImageInfo info{VkImage{}, ImageLayout{}, VkImage{}, ImageLayout{}, {
        {ImageAspect::Color, 3, 0, 0, {}, 0, 0, 0, {}, {}},
        {ImageAspect::Depth, 0, 5, 0, {}, 0, 0, 0, {}, {}}
    }};

    Containers::Array<VkImageCopy> copies = info.vkImageCopies();
    CORRADE_COMPARE(copies.size(), 2);
    CORRADE_COMPARE(copies[0].srcSubresource.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT);
    CORRADE_COMPARE(copies[0].srcSubresource.mipLevel, 3);
    CORRADE_COMPARE(copies[1].dstSubresource.aspectMask, VK_IMAGE_ASPECT_DEPTH_BIT);
    CORRADE_COMPARE(copies[1].srcSubresource.baseArrayLayer, 5);
}

void ImageTest::bufferImageCopyConstruct() {
    /* It's min/max, not offset + size in the default Range constructor */
    BufferImageCopy copy{3, 5, 7, ImageAspect::Stencil, 9, 11, 13, {{2, 4, 6}, {10, 14, 18}}};
    CORRADE_COMPARE(copy->bufferOffset, 3);
    CORRADE_COMPARE(copy->bufferRowLength, 5);
    CORRADE_COMPARE(copy->bufferImageHeight, 7);
    CORRADE_COMPARE(copy->imageSubresource.aspectMask, VK_IMAGE_ASPECT_STENCIL_BIT);
    CORRADE_COMPARE(copy->imageSubresource.mipLevel, 9);
    CORRADE_COMPARE(copy->imageSubresource.baseArrayLayer, 11);
    CORRADE_COMPARE(copy->imageSubresource.layerCount, 13);
    CORRADE_COMPARE(Vector3i{copy->imageOffset}, (Vector3i{2, 4, 6}));
    CORRADE_COMPARE(Vector3i{copy->imageExtent}, (Vector3i{8, 10, 12}));
}

void ImageTest::bufferImageCopyConstruct1D() {
    /* It's min/max, not offset + size in the default Range constructor */
    BufferImageCopy1D copy{3, ImageAspect::Stencil, 9, {2, 10}};
    CORRADE_COMPARE(copy->bufferOffset, 3);
    CORRADE_COMPARE(copy->bufferRowLength, 0);
    CORRADE_COMPARE(copy->bufferImageHeight, 0);
    CORRADE_COMPARE(copy->imageSubresource.aspectMask, VK_IMAGE_ASPECT_STENCIL_BIT);
    CORRADE_COMPARE(copy->imageSubresource.mipLevel, 9);
    CORRADE_COMPARE(copy->imageSubresource.baseArrayLayer, 0);
    CORRADE_COMPARE(copy->imageSubresource.layerCount, 1);
    CORRADE_COMPARE(Vector3i{copy->imageOffset}, (Vector3i{2, 0, 0}));
    CORRADE_COMPARE(Vector3i{copy->imageExtent}, (Vector3i{8, 1, 1}));
}

void ImageTest::bufferImageCopyConstruct2D() {
    /* It's min/max, not offset + size in the default Range constructor */
    BufferImageCopy2D copy{3, 5, ImageAspect::Stencil, 9, {{2, 4}, {10, 14}}};
    CORRADE_COMPARE(copy->bufferOffset, 3);
    CORRADE_COMPARE(copy->bufferRowLength, 5);
    CORRADE_COMPARE(copy->bufferImageHeight, 0);
    CORRADE_COMPARE(copy->imageSubresource.aspectMask, VK_IMAGE_ASPECT_STENCIL_BIT);
    CORRADE_COMPARE(copy->imageSubresource.mipLevel, 9);
    CORRADE_COMPARE(copy->imageSubresource.baseArrayLayer, 0);
    CORRADE_COMPARE(copy->imageSubresource.layerCount, 1);
    CORRADE_COMPARE(Vector3i{copy->imageOffset}, (Vector3i{2, 4, 0}));
    CORRADE_COMPARE(Vector3i{copy->imageExtent}, (Vector3i{8, 10, 1}));
}

void ImageTest::bufferImageCopyConstruct3D() {
    /* It's min/max, not offset + size in the default Range constructor */
    BufferImageCopy3D copy{3, 5, 7, ImageAspect::Stencil, 9, {{2, 4, 6}, {10, 14, 18}}};
    CORRADE_COMPARE(copy->bufferOffset, 3);
    CORRADE_COMPARE(copy->bufferRowLength, 5);
    CORRADE_COMPARE(copy->bufferImageHeight, 7);
    CORRADE_COMPARE(copy->imageSubresource.aspectMask, VK_IMAGE_ASPECT_STENCIL_BIT);
    CORRADE_COMPARE(copy->imageSubresource.mipLevel, 9);
    CORRADE_COMPARE(copy->imageSubresource.baseArrayLayer, 0);
    CORRADE_COMPARE(copy->imageSubresource.layerCount, 1);
    CORRADE_COMPARE(Vector3i{copy->imageOffset}, (Vector3i{2, 4, 6}));
    CORRADE_COMPARE(Vector3i{copy->imageExtent}, (Vector3i{8, 10, 12}));
}

void ImageTest::bufferImageCopyConstruct1DArray() {
    /* It's min/max, not offset + size in the default Range constructor */
    BufferImageCopy1DArray copy{3, 5, ImageAspect::Stencil, 9, {{2, 11}, {10, 24}}};
    CORRADE_COMPARE(copy->bufferOffset, 3);
    CORRADE_COMPARE(copy->bufferRowLength, 5);
    CORRADE_COMPARE(copy->bufferImageHeight, 0);
    CORRADE_COMPARE(copy->imageSubresource.aspectMask, VK_IMAGE_ASPECT_STENCIL_BIT);
    CORRADE_COMPARE(copy->imageSubresource.mipLevel, 9);
    CORRADE_COMPARE(copy->imageSubresource.baseArrayLayer, 11);
    CORRADE_COMPARE(copy->imageSubresource.layerCount, 13);
    CORRADE_COMPARE(Vector3i{copy->imageOffset}, (Vector3i{2, 0, 0}));
    CORRADE_COMPARE(Vector3i{copy->imageExtent}, (Vector3i{8, 1, 1}));
}

void ImageTest::bufferImageCopyConstruct2DArray() {
    /* It's min/max, not offset + size in the default Range constructor */
    BufferImageCopy2DArray copy{3, 5, 7, ImageAspect::Stencil, 9, {{2, 4, 11}, {10, 14, 24}}};
    CORRADE_COMPARE(copy->bufferOffset, 3);
    CORRADE_COMPARE(copy->bufferRowLength, 5);
    CORRADE_COMPARE(copy->bufferImageHeight, 7);
    CORRADE_COMPARE(copy->imageSubresource.aspectMask, VK_IMAGE_ASPECT_STENCIL_BIT);
    CORRADE_COMPARE(copy->imageSubresource.mipLevel, 9);
    CORRADE_COMPARE(copy->imageSubresource.baseArrayLayer, 11);
    CORRADE_COMPARE(copy->imageSubresource.layerCount, 13);
    CORRADE_COMPARE(Vector3i{copy->imageOffset}, (Vector3i{2, 4, 0}));
    CORRADE_COMPARE(Vector3i{copy->imageExtent}, (Vector3i{8, 10, 1}));
}

void ImageTest::bufferImageCopyConstructCubeMap() {
    /* It's min/max, not offset + size in the default Range constructor */
    BufferImageCopyCubeMap copy{3, 5, 7, ImageAspect::Stencil, 9, {{2, 4}, {10, 14}}};
    CORRADE_COMPARE(copy->bufferOffset, 3);
    CORRADE_COMPARE(copy->bufferRowLength, 5);
    CORRADE_COMPARE(copy->bufferImageHeight, 7);
    CORRADE_COMPARE(copy->imageSubresource.aspectMask, VK_IMAGE_ASPECT_STENCIL_BIT);
    CORRADE_COMPARE(copy->imageSubresource.mipLevel, 9);
    CORRADE_COMPARE(copy->imageSubresource.baseArrayLayer, 0);
    CORRADE_COMPARE(copy->imageSubresource.layerCount, 6);
    CORRADE_COMPARE(Vector3i{copy->imageOffset}, (Vector3i{2, 4, 0}));
    CORRADE_COMPARE(Vector3i{copy->imageExtent}, (Vector3i{8, 10, 1}));
}

void ImageTest::bufferImageCopyConstructCubeMapArray() {
    /* It's min/max, not offset + size in the default Range constructor */
    BufferImageCopyCubeMapArray copy{3, 5, 7, ImageAspect::Stencil, 9, {{2, 4, 11}, {10, 14, 24}}};
    CORRADE_COMPARE(copy->bufferOffset, 3);
    CORRADE_COMPARE(copy->bufferRowLength, 5);
    CORRADE_COMPARE(copy->bufferImageHeight, 7);
    CORRADE_COMPARE(copy->imageSubresource.aspectMask, VK_IMAGE_ASPECT_STENCIL_BIT);
    CORRADE_COMPARE(copy->imageSubresource.mipLevel, 9);
    CORRADE_COMPARE(copy->imageSubresource.baseArrayLayer, 11);
    CORRADE_COMPARE(copy->imageSubresource.layerCount, 13);
    CORRADE_COMPARE(Vector3i{copy->imageOffset}, (Vector3i{2, 4, 0}));
    CORRADE_COMPARE(Vector3i{copy->imageExtent}, (Vector3i{8, 10, 1}));
}

void ImageTest::bufferImageCopyConstructNoInit() {
    BufferImageCopy copy{NoInit};
    copy->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    new(&copy) BufferImageCopy{NoInit};
    CORRADE_COMPARE(copy->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);

    CORRADE_VERIFY((std::is_nothrow_constructible<BufferImageCopy, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, BufferImageCopy>::value));
}

template<class From, class To> void ImageTest::bufferImageCopyConstructFromVk() {
    setTestCaseTemplateName({Traits<From>::name(), Traits<To>::name()});

    From from{};
    from.bufferOffset = 3;
    from.bufferRowLength = 5;
    from.bufferImageHeight = 7;
    from.imageSubresource.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
    from.imageSubresource.mipLevel = 9;
    from.imageSubresource.baseArrayLayer = 11;
    from.imageSubresource.layerCount = 13;
    from.imageOffset = {2, 4, 6};
    from.imageExtent = {8, 10, 12};

    BufferImageCopy copy{from};
    To to = Traits<To>::convert(copy);
    CORRADE_COMPARE(to.bufferOffset, 3);
    CORRADE_COMPARE(to.bufferRowLength, 5);
    CORRADE_COMPARE(to.bufferImageHeight, 7);
    CORRADE_COMPARE(to.imageSubresource.aspectMask, VK_IMAGE_ASPECT_STENCIL_BIT);
    CORRADE_COMPARE(to.imageSubresource.mipLevel, 9);
    CORRADE_COMPARE(to.imageSubresource.baseArrayLayer, 11);
    CORRADE_COMPARE(to.imageSubresource.layerCount, 13);
    CORRADE_COMPARE(Vector3i{to.imageOffset}, (Vector3i{2, 4, 6}));
    CORRADE_COMPARE(Vector3i{to.imageExtent}, (Vector3i{8, 10, 12}));
}

template<class T> void ImageTest::bufferImageCopyConvertToVk() {
    /* It's min/max, not offset + size in the default Range constructor */
    BufferImageCopy copy{3, 5, 7, ImageAspect::Stencil, 9, 11, 13, {{2, 4, 6}, {10, 14, 18}}};

    T out = Traits<T>::convert(copy);
    CORRADE_COMPARE(out.bufferOffset, 3);
    CORRADE_COMPARE(out.bufferRowLength, 5);
    CORRADE_COMPARE(out.bufferImageHeight, 7);
    CORRADE_COMPARE(out.imageSubresource.aspectMask, VK_IMAGE_ASPECT_STENCIL_BIT);
    CORRADE_COMPARE(out.imageSubresource.mipLevel, 9);
    CORRADE_COMPARE(out.imageSubresource.baseArrayLayer, 11);
    CORRADE_COMPARE(out.imageSubresource.layerCount, 13);
    CORRADE_COMPARE(Vector3i{out.imageOffset}, (Vector3i{2, 4, 6}));
    CORRADE_COMPARE(Vector3i{out.imageExtent}, (Vector3i{8, 10, 12}));
}

void ImageTest::bufferImageCopyConvertDisallowed() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    BufferImageCopy copy{0, 0, 0, ImageAspect{}, 0, 0, 0, {}};
    copy->pNext = &copy;

    std::ostringstream out;
    Error redirectError{&out};
    copy.vkBufferImageCopy();
    CORRADE_COMPARE(out.str(), "Vk::BufferImageCopy: disallowing conversion to VkBufferImageCopy with non-empty pNext to prevent information loss\n");
}

void ImageTest::copyBufferToImageInfoConstruct() {
    auto a = reinterpret_cast<VkBuffer>(0xdead);
    auto b = reinterpret_cast<VkImage>(0xcafe);

    CopyBufferToImageInfo info{a, b, ImageLayout::TransferDestination, {
        BufferImageCopy1D{5, ImageAspect::Color, 0, {}},
        BufferImageCopy1D{0, ImageAspect::Stencil, 3, {}}
    }};
    CORRADE_COMPARE(info->srcBuffer, a);
    CORRADE_COMPARE(info->dstImage, b);
    CORRADE_COMPARE(info->dstImageLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    CORRADE_COMPARE(info->regionCount, 2);
    CORRADE_VERIFY(info->pRegions);
    CORRADE_COMPARE(info->pRegions[0].bufferOffset, 5);
    CORRADE_COMPARE(info->pRegions[0].imageSubresource.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT);
    CORRADE_COMPARE(info->pRegions[1].imageSubresource.aspectMask, VK_IMAGE_ASPECT_STENCIL_BIT);
    CORRADE_COMPARE(info->pRegions[1].imageSubresource.mipLevel, 3);
}

void ImageTest::copyBufferToImageInfoConstructNoInit() {
    CopyBufferToImageInfo info{NoInit};
    info->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    new(&info) CopyBufferToImageInfo{NoInit};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);

    CORRADE_VERIFY((std::is_nothrow_constructible<CopyBufferToImageInfo, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, CopyBufferToImageInfo>::value));
}

void ImageTest::copyBufferToImageInfoConstructFromVk() {
    VkCopyBufferToImageInfo2KHR vkInfo;
    vkInfo.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;

    CopyBufferToImageInfo info{vkInfo};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
}

void ImageTest::copyBufferToImageInfoConvertToVk() {
    CopyBufferToImageInfo info{VkBuffer{}, VkImage{}, ImageLayout{}, {
        BufferImageCopy1D{5, ImageAspect::Color, 0, {}},
        BufferImageCopy1D{0, ImageAspect::Stencil, 3, {}},
    }};

    Containers::Array<VkBufferImageCopy> copies = info.vkBufferImageCopies();
    CORRADE_COMPARE(copies.size(), 2);
    CORRADE_COMPARE(copies[0].bufferOffset, 5);
    CORRADE_COMPARE(copies[0].imageSubresource.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT);
    CORRADE_COMPARE(copies[1].imageSubresource.aspectMask, VK_IMAGE_ASPECT_STENCIL_BIT);
    CORRADE_COMPARE(copies[1].imageSubresource.mipLevel, 3);
}

void ImageTest::copyImageToBufferInfoConstruct() {
    auto a = reinterpret_cast<VkImage>(0xcafe);
    auto b = reinterpret_cast<VkBuffer>(0xdead);

    CopyImageToBufferInfo info{a, ImageLayout::TransferSource, b, {
        BufferImageCopy1D{5, ImageAspect::Color, 0, {}},
        BufferImageCopy1D{0, ImageAspect::Stencil, 3, {}}
    }};
    CORRADE_COMPARE(info->srcImage, a);
    CORRADE_COMPARE(info->srcImageLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    CORRADE_COMPARE(info->dstBuffer, b);
    CORRADE_COMPARE(info->regionCount, 2);
    CORRADE_VERIFY(info->pRegions);
    CORRADE_COMPARE(info->pRegions[0].bufferOffset, 5);
    CORRADE_COMPARE(info->pRegions[0].imageSubresource.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT);
    CORRADE_COMPARE(info->pRegions[1].imageSubresource.aspectMask, VK_IMAGE_ASPECT_STENCIL_BIT);
    CORRADE_COMPARE(info->pRegions[1].imageSubresource.mipLevel, 3);
}

void ImageTest::copyImageToBufferInfoConstructNoInit() {
    CopyImageToBufferInfo info{NoInit};
    info->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    new(&info) CopyImageToBufferInfo{NoInit};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);

    CORRADE_VERIFY((std::is_nothrow_constructible<CopyImageToBufferInfo, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, CopyImageToBufferInfo>::value));
}

void ImageTest::copyImageToBufferInfoConstructFromVk() {
    VkCopyImageToBufferInfo2KHR vkInfo;
    vkInfo.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;

    CopyImageToBufferInfo info{vkInfo};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
}

void ImageTest::copyImageToBufferInfoConvertToVk() {
    CopyImageToBufferInfo info{VkImage{}, ImageLayout{}, VkBuffer{}, {
        BufferImageCopy1D{5, ImageAspect::Color, 0, {}},
        BufferImageCopy1D{0, ImageAspect::Stencil, 3, {}},
    }};

    Containers::Array<VkBufferImageCopy> copies = info.vkBufferImageCopies();
    CORRADE_COMPARE(copies.size(), 2);
    CORRADE_COMPARE(copies[0].bufferOffset, 5);
    CORRADE_COMPARE(copies[0].imageSubresource.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT);
    CORRADE_COMPARE(copies[1].imageSubresource.aspectMask, VK_IMAGE_ASPECT_STENCIL_BIT);
    CORRADE_COMPARE(copies[1].imageSubresource.mipLevel, 3);
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
