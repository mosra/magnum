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

#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/Image.h"
#include "Magnum/Vk/ImageViewCreateInfo.h"

#include "Magnum/Vk/Test/pixelFormatTraits.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct ImageViewTest: TestSuite::Tester {
    explicit ImageViewTest();

    template<class T> void createInfoConstruct();
    void createInfoConstructFromImage();
    void createInfoConstructFromImageFormatUknown();
    template<class T> void createInfoConstruct1D();
    void createInfoConstruct1DFromImage();
    template<class T> void createInfoConstruct2D();
    void createInfoConstruct2DDepth();
    void createInfoConstruct2DFromImage();
    template<class T> void createInfoConstruct3D();
    void createInfoConstruct3DFromImage();
    template<class T> void createInfoConstruct1DArray();
    void createInfoConstruct1DArrayFromImage();
    template<class T> void createInfoConstruct2DArray();
    void createInfoConstruct2DArrayFromImage();
    template<class T> void createInfoConstructCubeMap();
    void createInfoConstructCubeMapFromImage();
    template<class T> void createInfoConstructCubeMapArray();
    void createInfoConstructCubeMapArrayFromImage();
    void createInfoConstructNoInit();
    void createInfoConstructFromVk();

    void constructNoCreate();
    void constructCopy();
};

ImageViewTest::ImageViewTest() {
    addTests({&ImageViewTest::createInfoConstruct<PixelFormat>,
              &ImageViewTest::createInfoConstruct<Magnum::PixelFormat>,
              &ImageViewTest::createInfoConstruct<Magnum::CompressedPixelFormat>,
              &ImageViewTest::createInfoConstructFromImage,
              &ImageViewTest::createInfoConstructFromImageFormatUknown,
              &ImageViewTest::createInfoConstruct1D<PixelFormat>,
              &ImageViewTest::createInfoConstruct1D<Magnum::PixelFormat>,
              &ImageViewTest::createInfoConstruct1D<Magnum::CompressedPixelFormat>,
              &ImageViewTest::createInfoConstruct1DFromImage,
              &ImageViewTest::createInfoConstruct2D<PixelFormat>,
              &ImageViewTest::createInfoConstruct2D<Magnum::PixelFormat>,
              &ImageViewTest::createInfoConstruct2D<Magnum::CompressedPixelFormat>,
              &ImageViewTest::createInfoConstruct2DDepth,

              &ImageViewTest::createInfoConstruct2DFromImage,
              &ImageViewTest::createInfoConstruct3D<PixelFormat>,
              &ImageViewTest::createInfoConstruct3D<Magnum::PixelFormat>,
              &ImageViewTest::createInfoConstruct3D<Magnum::CompressedPixelFormat>,
              &ImageViewTest::createInfoConstruct3DFromImage,
              &ImageViewTest::createInfoConstruct1DArray<PixelFormat>,
              &ImageViewTest::createInfoConstruct1DArray<Magnum::PixelFormat>,
              &ImageViewTest::createInfoConstruct1DArray<Magnum::CompressedPixelFormat>,
              &ImageViewTest::createInfoConstruct1DArrayFromImage,
              &ImageViewTest::createInfoConstruct2DArray<PixelFormat>,
              &ImageViewTest::createInfoConstruct2DArray<Magnum::PixelFormat>,
              &ImageViewTest::createInfoConstruct2DArray<Magnum::CompressedPixelFormat>,
              &ImageViewTest::createInfoConstruct2DArrayFromImage,
              &ImageViewTest::createInfoConstructCubeMap<PixelFormat>,
              &ImageViewTest::createInfoConstructCubeMap<Magnum::PixelFormat>,
              &ImageViewTest::createInfoConstructCubeMap<Magnum::CompressedPixelFormat>,
              &ImageViewTest::createInfoConstructCubeMapFromImage,
              &ImageViewTest::createInfoConstructCubeMapArray<PixelFormat>,
              &ImageViewTest::createInfoConstructCubeMapArray<Magnum::PixelFormat>,
              &ImageViewTest::createInfoConstructCubeMapArray<Magnum::CompressedPixelFormat>,
              &ImageViewTest::createInfoConstructCubeMapArrayFromImage,
              &ImageViewTest::createInfoConstructNoInit,
              &ImageViewTest::createInfoConstructFromVk,

              &ImageViewTest::constructNoCreate,
              &ImageViewTest::constructCopy});
}

const VkImage imageHandle{reinterpret_cast<VkImage>(0xdeadbeef)};

template<class T> void ImageViewTest::createInfoConstruct() {
    /** @todo use a real flag once it exists */
    ImageViewCreateInfo info{VK_IMAGE_VIEW_TYPE_2D, imageHandle, PixelFormatTraits<T>::format(), 3, 5, 7, 9, ImageViewCreateInfo::Flag(VK_NOT_READY)};
    CORRADE_COMPARE(info->flags, VK_NOT_READY);
    CORRADE_COMPARE(info->image, imageHandle);
    CORRADE_COMPARE(info->viewType, VK_IMAGE_VIEW_TYPE_2D);
    CORRADE_COMPARE(info->format, PixelFormatTraits<T>::expected());
    CORRADE_COMPARE(info->subresourceRange.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT);
    CORRADE_COMPARE(info->subresourceRange.baseArrayLayer, 3);
    CORRADE_COMPARE(info->subresourceRange.layerCount, 5);
    CORRADE_COMPARE(info->subresourceRange.baseMipLevel, 7);
    CORRADE_COMPARE(info->subresourceRange.levelCount, 9);
}

void ImageViewTest::createInfoConstructFromImage() {
    Device device{NoCreate};
    Image image = Image::wrap(device, imageHandle, PixelFormat::RGBA8Srgb);

    /** @todo use a real flag once it exists */
    ImageViewCreateInfo info{VK_IMAGE_VIEW_TYPE_2D, image, 3, 5, 7, 9, ImageViewCreateInfo::Flag(VK_NOT_READY)};
    CORRADE_COMPARE(info->flags, VK_NOT_READY);
    CORRADE_COMPARE(info->image, imageHandle);
    CORRADE_COMPARE(info->viewType, VK_IMAGE_VIEW_TYPE_2D);
    CORRADE_COMPARE(info->format, VK_FORMAT_R8G8B8A8_SRGB);
    CORRADE_COMPARE(info->subresourceRange.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT);
    CORRADE_COMPARE(info->subresourceRange.baseArrayLayer, 3);
    CORRADE_COMPARE(info->subresourceRange.layerCount, 5);
    CORRADE_COMPARE(info->subresourceRange.baseMipLevel, 7);
    CORRADE_COMPARE(info->subresourceRange.levelCount, 9);
}

void ImageViewTest::createInfoConstructFromImageFormatUknown() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Device device{NoCreate};
    Image image = Image::wrap(device, imageHandle, PixelFormat{});

    std::ostringstream out;
    Error redirectError{&out};
    ImageViewCreateInfo{VK_IMAGE_VIEW_TYPE_2D, image};
    CORRADE_COMPARE(out.str(),
        "Vk::ImageViewCreateInfo: the image has unknown format, you have to specify it explicitly\n"
        /* The second assert won't appear for the user, it's here only because
           the graceful assert can'ŧ do an early exist in a delegeated
           constructor call */
        "Vk::imageAspectsFor(): can't get an aspect for Vk::PixelFormat(0)\n");
}

template<class T> void ImageViewTest::createInfoConstruct1D() {
    setTestCaseTemplateName(PixelFormatTraits<T>::name());

    /** @todo use a real flag once it exists */
    ImageViewCreateInfo1D info{imageHandle, PixelFormatTraits<T>::format(), 3, 7, 9, ImageViewCreateInfo::Flag(VK_NOT_READY)};
    CORRADE_COMPARE(info->flags, VK_NOT_READY);
    CORRADE_COMPARE(info->image, imageHandle);
    CORRADE_COMPARE(info->viewType, VK_IMAGE_VIEW_TYPE_1D);
    CORRADE_COMPARE(info->format, PixelFormatTraits<T>::expected());
    CORRADE_COMPARE(info->subresourceRange.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT);
    CORRADE_COMPARE(info->subresourceRange.baseArrayLayer, 3);
    CORRADE_COMPARE(info->subresourceRange.layerCount, 1);
    CORRADE_COMPARE(info->subresourceRange.baseMipLevel, 7);
    CORRADE_COMPARE(info->subresourceRange.levelCount, 9);
}

void ImageViewTest::createInfoConstruct1DFromImage() {
    Device device{NoCreate};
    Image image = Image::wrap(device, imageHandle, PixelFormat::RGBA8Srgb);

    /** @todo use a real flag once it exists */
    ImageViewCreateInfo1D info{image, 3, 7, 9, ImageViewCreateInfo::Flag(VK_NOT_READY)};
    CORRADE_COMPARE(info->flags, VK_NOT_READY);
    CORRADE_COMPARE(info->image, imageHandle);
    CORRADE_COMPARE(info->viewType, VK_IMAGE_VIEW_TYPE_1D);
    CORRADE_COMPARE(info->format, VK_FORMAT_R8G8B8A8_SRGB);
    CORRADE_COMPARE(info->subresourceRange.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT);
    CORRADE_COMPARE(info->subresourceRange.baseArrayLayer, 3);
    CORRADE_COMPARE(info->subresourceRange.layerCount, 1);
    CORRADE_COMPARE(info->subresourceRange.baseMipLevel, 7);
    CORRADE_COMPARE(info->subresourceRange.levelCount, 9);
}

template<class T> void ImageViewTest::createInfoConstruct2D() {
    setTestCaseTemplateName(PixelFormatTraits<T>::name());

    /** @todo use a real flag once it exists */
    ImageViewCreateInfo2D info{imageHandle, PixelFormatTraits<T>::format(), 3, 7, 9, ImageViewCreateInfo::Flag(VK_NOT_READY)};
    CORRADE_COMPARE(info->flags, VK_NOT_READY);
    CORRADE_COMPARE(info->image, imageHandle);
    CORRADE_COMPARE(info->viewType, VK_IMAGE_VIEW_TYPE_2D);
    CORRADE_COMPARE(info->format, PixelFormatTraits<T>::expected());
    CORRADE_COMPARE(info->subresourceRange.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT);
    CORRADE_COMPARE(info->subresourceRange.baseArrayLayer, 3);
    CORRADE_COMPARE(info->subresourceRange.layerCount, 1);
    CORRADE_COMPARE(info->subresourceRange.baseMipLevel, 7);
    CORRADE_COMPARE(info->subresourceRange.levelCount, 9);
}

void ImageViewTest::createInfoConstruct2DDepth() {
    /* Just to verify proper aspect is chosen. The rest is tested in
       ImageTest::aspectFor() */

    ImageViewCreateInfo2D info{imageHandle, PixelFormat::Depth24Unorm};
    CORRADE_COMPARE(info->format, VK_FORMAT_X8_D24_UNORM_PACK32);
    CORRADE_COMPARE(info->subresourceRange.aspectMask, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void ImageViewTest::createInfoConstruct2DFromImage() {
    Device device{NoCreate};
    Image image = Image::wrap(device, imageHandle, PixelFormat::RGBA8Srgb);

    /** @todo use a real flag once it exists */
    ImageViewCreateInfo2D info{image, 3, 7, 9, ImageViewCreateInfo::Flag(VK_NOT_READY)};
    CORRADE_COMPARE(info->flags, VK_NOT_READY);
    CORRADE_COMPARE(info->image, imageHandle);
    CORRADE_COMPARE(info->viewType, VK_IMAGE_VIEW_TYPE_2D);
    CORRADE_COMPARE(info->format, VK_FORMAT_R8G8B8A8_SRGB);
    CORRADE_COMPARE(info->subresourceRange.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT);
    CORRADE_COMPARE(info->subresourceRange.baseArrayLayer, 3);
    CORRADE_COMPARE(info->subresourceRange.layerCount, 1);
    CORRADE_COMPARE(info->subresourceRange.baseMipLevel, 7);
    CORRADE_COMPARE(info->subresourceRange.levelCount, 9);
}

template<class T> void ImageViewTest::createInfoConstruct3D() {
    setTestCaseTemplateName(PixelFormatTraits<T>::name());

    /** @todo use a real flag once it exists */
    ImageViewCreateInfo3D info{imageHandle, PixelFormatTraits<T>::format(), 3, 7, 9, ImageViewCreateInfo::Flag(VK_NOT_READY)};
    CORRADE_COMPARE(info->flags, VK_NOT_READY);
    CORRADE_COMPARE(info->image, imageHandle);
    CORRADE_COMPARE(info->viewType, VK_IMAGE_VIEW_TYPE_3D);
    CORRADE_COMPARE(info->format, PixelFormatTraits<T>::expected());
    CORRADE_COMPARE(info->subresourceRange.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT);
    CORRADE_COMPARE(info->subresourceRange.baseArrayLayer, 3);
    CORRADE_COMPARE(info->subresourceRange.layerCount, 1);
    CORRADE_COMPARE(info->subresourceRange.baseMipLevel, 7);
    CORRADE_COMPARE(info->subresourceRange.levelCount, 9);
}

void ImageViewTest::createInfoConstruct3DFromImage() {
    Device device{NoCreate};
    Image image = Image::wrap(device, imageHandle, PixelFormat::RGBA8Srgb);

    /** @todo use a real flag once it exists */
    ImageViewCreateInfo3D info{image, 3, 7, 9, ImageViewCreateInfo::Flag(VK_NOT_READY)};
    CORRADE_COMPARE(info->flags, VK_NOT_READY);
    CORRADE_COMPARE(info->image, imageHandle);
    CORRADE_COMPARE(info->viewType, VK_IMAGE_VIEW_TYPE_3D);
    CORRADE_COMPARE(info->format, VK_FORMAT_R8G8B8A8_SRGB);
    CORRADE_COMPARE(info->subresourceRange.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT);
    CORRADE_COMPARE(info->subresourceRange.baseArrayLayer, 3);
    CORRADE_COMPARE(info->subresourceRange.layerCount, 1);
    CORRADE_COMPARE(info->subresourceRange.baseMipLevel, 7);
    CORRADE_COMPARE(info->subresourceRange.levelCount, 9);
}

template<class T> void ImageViewTest::createInfoConstruct1DArray() {
    setTestCaseTemplateName(PixelFormatTraits<T>::name());

    /** @todo use a real flag once it exists */
    ImageViewCreateInfo1DArray info{imageHandle, PixelFormatTraits<T>::format(), 3, 5, 7, 9, ImageViewCreateInfo::Flag(VK_NOT_READY)};
    CORRADE_COMPARE(info->flags, VK_NOT_READY);
    CORRADE_COMPARE(info->image, imageHandle);
    CORRADE_COMPARE(info->viewType, VK_IMAGE_VIEW_TYPE_1D_ARRAY);
    CORRADE_COMPARE(info->format, PixelFormatTraits<T>::expected());
    CORRADE_COMPARE(info->subresourceRange.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT);
    CORRADE_COMPARE(info->subresourceRange.baseArrayLayer, 3);
    CORRADE_COMPARE(info->subresourceRange.layerCount, 5);
    CORRADE_COMPARE(info->subresourceRange.baseMipLevel, 7);
    CORRADE_COMPARE(info->subresourceRange.levelCount, 9);
}

void ImageViewTest::createInfoConstruct1DArrayFromImage() {
    Device device{NoCreate};
    Image image = Image::wrap(device, imageHandle, PixelFormat::RGBA8Srgb);

    /** @todo use a real flag once it exists */
    ImageViewCreateInfo1DArray info{image, 3, 5, 7, 9, ImageViewCreateInfo::Flag(VK_NOT_READY)};
    CORRADE_COMPARE(info->flags, VK_NOT_READY);
    CORRADE_COMPARE(info->image, imageHandle);
    CORRADE_COMPARE(info->viewType, VK_IMAGE_VIEW_TYPE_1D_ARRAY);
    CORRADE_COMPARE(info->format, VK_FORMAT_R8G8B8A8_SRGB);
    CORRADE_COMPARE(info->subresourceRange.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT);
    CORRADE_COMPARE(info->subresourceRange.baseArrayLayer, 3);
    CORRADE_COMPARE(info->subresourceRange.layerCount, 5);
    CORRADE_COMPARE(info->subresourceRange.baseMipLevel, 7);
    CORRADE_COMPARE(info->subresourceRange.levelCount, 9);
}

template<class T> void ImageViewTest::createInfoConstruct2DArray() {
    setTestCaseTemplateName(PixelFormatTraits<T>::name());

    /** @todo use a real flag once it exists */
    ImageViewCreateInfo2DArray info{imageHandle, PixelFormatTraits<T>::format(), 3, 5, 7, 9, ImageViewCreateInfo::Flag(VK_NOT_READY)};
    CORRADE_COMPARE(info->flags, VK_NOT_READY);
    CORRADE_COMPARE(info->image, imageHandle);
    CORRADE_COMPARE(info->viewType, VK_IMAGE_VIEW_TYPE_2D_ARRAY);
    CORRADE_COMPARE(info->format, PixelFormatTraits<T>::expected());
    CORRADE_COMPARE(info->subresourceRange.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT);
    CORRADE_COMPARE(info->subresourceRange.baseArrayLayer, 3);
    CORRADE_COMPARE(info->subresourceRange.layerCount, 5);
    CORRADE_COMPARE(info->subresourceRange.baseMipLevel, 7);
    CORRADE_COMPARE(info->subresourceRange.levelCount, 9);
}

void ImageViewTest::createInfoConstruct2DArrayFromImage() {
    Device device{NoCreate};
    Image image = Image::wrap(device, imageHandle, PixelFormat::RGBA8Srgb);

    /** @todo use a real flag once it exists */
    ImageViewCreateInfo2DArray info{image, 3, 5, 7, 9, ImageViewCreateInfo::Flag(VK_NOT_READY)};
    CORRADE_COMPARE(info->flags, VK_NOT_READY);
    CORRADE_COMPARE(info->image, imageHandle);
    CORRADE_COMPARE(info->viewType, VK_IMAGE_VIEW_TYPE_2D_ARRAY);
    CORRADE_COMPARE(info->format, VK_FORMAT_R8G8B8A8_SRGB);
    CORRADE_COMPARE(info->subresourceRange.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT);
    CORRADE_COMPARE(info->subresourceRange.baseArrayLayer, 3);
    CORRADE_COMPARE(info->subresourceRange.layerCount, 5);
    CORRADE_COMPARE(info->subresourceRange.baseMipLevel, 7);
    CORRADE_COMPARE(info->subresourceRange.levelCount, 9);
}

template<class T> void ImageViewTest::createInfoConstructCubeMap() {
    setTestCaseTemplateName(PixelFormatTraits<T>::name());

    /** @todo use a real flag once it exists */
    ImageViewCreateInfoCubeMap info{imageHandle, PixelFormatTraits<T>::format(), 3, 7, 9, ImageViewCreateInfo::Flag(VK_NOT_READY)};
    CORRADE_COMPARE(info->flags, VK_NOT_READY);
    CORRADE_COMPARE(info->image, imageHandle);
    CORRADE_COMPARE(info->viewType, VK_IMAGE_VIEW_TYPE_CUBE);
    CORRADE_COMPARE(info->format, PixelFormatTraits<T>::expected());
    CORRADE_COMPARE(info->subresourceRange.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT);
    CORRADE_COMPARE(info->subresourceRange.baseArrayLayer, 3);
    CORRADE_COMPARE(info->subresourceRange.layerCount, 6);
    CORRADE_COMPARE(info->subresourceRange.baseMipLevel, 7);
    CORRADE_COMPARE(info->subresourceRange.levelCount, 9);
}

void ImageViewTest::createInfoConstructCubeMapFromImage() {
    Device device{NoCreate};
    Image image = Image::wrap(device, imageHandle, PixelFormat::RGBA8Srgb);

    /** @todo use a real flag once it exists */
    ImageViewCreateInfoCubeMap info{image, 3, 7, 9, ImageViewCreateInfo::Flag(VK_NOT_READY)};
    CORRADE_COMPARE(info->flags, VK_NOT_READY);
    CORRADE_COMPARE(info->image, imageHandle);
    CORRADE_COMPARE(info->viewType, VK_IMAGE_VIEW_TYPE_CUBE);
    CORRADE_COMPARE(info->format, VK_FORMAT_R8G8B8A8_SRGB);
    CORRADE_COMPARE(info->subresourceRange.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT);
    CORRADE_COMPARE(info->subresourceRange.baseArrayLayer, 3);
    CORRADE_COMPARE(info->subresourceRange.layerCount, 6);
    CORRADE_COMPARE(info->subresourceRange.baseMipLevel, 7);
    CORRADE_COMPARE(info->subresourceRange.levelCount, 9);
}

template<class T> void ImageViewTest::createInfoConstructCubeMapArray() {
    setTestCaseTemplateName(PixelFormatTraits<T>::name());

    /** @todo use a real flag once it exists */
    ImageViewCreateInfoCubeMapArray info{imageHandle, PixelFormatTraits<T>::format(), 3, 18, 7, 9, ImageViewCreateInfo::Flag(VK_NOT_READY)};
    CORRADE_COMPARE(info->flags, VK_NOT_READY);
    CORRADE_COMPARE(info->image, imageHandle);
    CORRADE_COMPARE(info->viewType, VK_IMAGE_VIEW_TYPE_CUBE_ARRAY);
    CORRADE_COMPARE(info->format, PixelFormatTraits<T>::expected());
    CORRADE_COMPARE(info->subresourceRange.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT);
    CORRADE_COMPARE(info->subresourceRange.baseArrayLayer, 3);
    CORRADE_COMPARE(info->subresourceRange.layerCount, 18);
    CORRADE_COMPARE(info->subresourceRange.baseMipLevel, 7);
    CORRADE_COMPARE(info->subresourceRange.levelCount, 9);
}

void ImageViewTest::createInfoConstructCubeMapArrayFromImage() {
    Device device{NoCreate};
    Image image = Image::wrap(device, imageHandle, PixelFormat::RGBA8Srgb);

    /** @todo use a real flag once it exists */
    ImageViewCreateInfoCubeMapArray info{image, 3, 18, 7, 9, ImageViewCreateInfo::Flag(VK_NOT_READY)};
    CORRADE_COMPARE(info->flags, VK_NOT_READY);
    CORRADE_COMPARE(info->image, imageHandle);
    CORRADE_COMPARE(info->viewType, VK_IMAGE_VIEW_TYPE_CUBE_ARRAY);
    CORRADE_COMPARE(info->format, VK_FORMAT_R8G8B8A8_SRGB);
    CORRADE_COMPARE(info->subresourceRange.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT);
    CORRADE_COMPARE(info->subresourceRange.baseArrayLayer, 3);
    CORRADE_COMPARE(info->subresourceRange.layerCount, 18);
    CORRADE_COMPARE(info->subresourceRange.baseMipLevel, 7);
    CORRADE_COMPARE(info->subresourceRange.levelCount, 9);
}

void ImageViewTest::createInfoConstructNoInit() {
    ImageViewCreateInfo info{NoInit};
    info->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    new(&info) ImageViewCreateInfo{NoInit};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);

    CORRADE_VERIFY((std::is_nothrow_constructible<ImageViewCreateInfo, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, ImageViewCreateInfo>::value));
}

void ImageViewTest::createInfoConstructFromVk() {
    VkImageViewCreateInfo vkInfo;
    vkInfo.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;

    ImageViewCreateInfo info{vkInfo};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
}

void ImageViewTest::constructNoCreate() {
    {
        ImageView view{NoCreate};
        CORRADE_VERIFY(!view.handle());
    }

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoCreateT, ImageView>::value));
}

void ImageViewTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<ImageView>{});
    CORRADE_VERIFY(!std::is_copy_assignable<ImageView>{});
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::ImageViewTest)
