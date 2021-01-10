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

#include "Magnum/Vk/DeviceCreateInfo.h"
#include "Magnum/Vk/DeviceFeatures.h"
#include "Magnum/Vk/DeviceProperties.h"
#include "Magnum/Vk/Handle.h"
#include "Magnum/Vk/ImageCreateInfo.h"
#include "Magnum/Vk/ImageViewCreateInfo.h"
#include "Magnum/Vk/PixelFormat.h"
#include "Magnum/Vk/Result.h"
#include "Magnum/Vk/VulkanTester.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct ImageViewVkTest: VulkanTester {
    explicit ImageViewVkTest();

    void construct1D();
    void construct2D();
    void construct3D();
    void construct1DArray();
    void construct2DArray();
    void constructCubeMap();
    void constructCubeMapArray();
    void constructMove();

    void wrap();
};

ImageViewVkTest::ImageViewVkTest() {
    addTests({&ImageViewVkTest::construct1D,
              &ImageViewVkTest::construct2D,
              &ImageViewVkTest::construct3D,
              &ImageViewVkTest::construct1DArray,
              &ImageViewVkTest::construct2DArray,
              &ImageViewVkTest::constructCubeMap,
              &ImageViewVkTest::constructCubeMapArray,
              &ImageViewVkTest::constructMove,

              &ImageViewVkTest::wrap});
}

void ImageViewVkTest::construct1D() {
    Image image{device(), ImageCreateInfo1D{ImageUsage::Sampled,
        PixelFormat::RGBA8Unorm, 256, 1}, MemoryFlag::DeviceLocal};

    {
        ImageView view{device(), ImageViewCreateInfo1D{image}};
        CORRADE_VERIFY(image.handle());
        CORRADE_COMPARE(image.handleFlags(), HandleFlag::DestroyOnDestruction);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void ImageViewVkTest::construct2D() {
    Image image{device(), ImageCreateInfo2D{ImageUsage::Sampled,
        PixelFormat::RGBA8Unorm, {256, 256}, 1}, MemoryFlag::DeviceLocal};

    {
        ImageView view{device(), ImageViewCreateInfo2D{image}};
        CORRADE_VERIFY(image.handle());
        CORRADE_COMPARE(image.handleFlags(), HandleFlag::DestroyOnDestruction);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void ImageViewVkTest::construct3D() {
    Image image{device(), ImageCreateInfo3D{ImageUsage::Sampled,
        PixelFormat::RGBA8Unorm, {256, 256, 10}, 1}, MemoryFlag::DeviceLocal};

    {
        ImageView view{device(), ImageViewCreateInfo3D{image}};
        CORRADE_VERIFY(image.handle());
        CORRADE_COMPARE(image.handleFlags(), HandleFlag::DestroyOnDestruction);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void ImageViewVkTest::construct1DArray() {
    Image image{device(), ImageCreateInfo1DArray{ImageUsage::Sampled,
        PixelFormat::RGBA8Unorm, {256, 10}, 1}, MemoryFlag::DeviceLocal};

    {
        ImageView view{device(), ImageViewCreateInfo1DArray{image}};
        CORRADE_VERIFY(image.handle());
        CORRADE_COMPARE(image.handleFlags(), HandleFlag::DestroyOnDestruction);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void ImageViewVkTest::construct2DArray() {
    Image image{device(), ImageCreateInfo2DArray{ImageUsage::Sampled,
        PixelFormat::RGBA8Unorm, {256, 256, 10}, 1}, MemoryFlag::DeviceLocal};

    {
        ImageView view{device(), ImageViewCreateInfo2DArray{image}};
        CORRADE_VERIFY(image.handle());
        CORRADE_COMPARE(image.handleFlags(), HandleFlag::DestroyOnDestruction);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void ImageViewVkTest::constructCubeMap() {
    Image image{device(), ImageCreateInfoCubeMap{ImageUsage::Sampled,
        PixelFormat::RGBA8Unorm, {256, 256}, 1}, MemoryFlag::DeviceLocal};

    {
        ImageView view{device(), ImageViewCreateInfoCubeMap{image}};
        CORRADE_VERIFY(image.handle());
        CORRADE_COMPARE(image.handleFlags(), HandleFlag::DestroyOnDestruction);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void ImageViewVkTest::constructCubeMapArray() {
    if(!(device().properties().features() & DeviceFeature::ImageCubeArray))
        CORRADE_SKIP("ImageCubeArray feature not supported, can't test.");

    /* Create the image on a new device with the feature enabled */
    Queue queue2{NoCreate};
    Device device2{instance(), DeviceCreateInfo{device().properties()}
        .addQueues(0, {0.0f}, {queue2})
        .setEnabledFeatures(DeviceFeature::ImageCubeArray)};
    Image image{device2, ImageCreateInfoCubeMapArray{ImageUsage::Sampled,
        PixelFormat::RGBA8Unorm, {256, 256, 18}, 1}, MemoryFlag::DeviceLocal};

    {
        ImageView view{device2, ImageViewCreateInfoCubeMapArray{image}};
        CORRADE_VERIFY(image.handle());
        CORRADE_COMPARE(image.handleFlags(), HandleFlag::DestroyOnDestruction);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void ImageViewVkTest::constructMove() {
    Image image{device(), ImageCreateInfo2D{ImageUsage::Sampled,
        PixelFormat::RGBA8Unorm, {256, 256}, 1}, MemoryFlag::DeviceLocal};
    ImageView a{device(), ImageViewCreateInfo2D{image}};
    VkImageView handle = a.handle();

    ImageView b = std::move(a);
    CORRADE_VERIFY(!a.handle());
    CORRADE_COMPARE(b.handle(), handle);
    CORRADE_COMPARE(b.handleFlags(), HandleFlag::DestroyOnDestruction);

    ImageView c{NoCreate};
    c = std::move(b);
    CORRADE_VERIFY(!b.handle());
    CORRADE_COMPARE(b.handleFlags(), HandleFlags{});
    CORRADE_COMPARE(c.handle(), handle);
    CORRADE_COMPARE(c.handleFlags(), HandleFlag::DestroyOnDestruction);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<Image>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<Image>::value);
}

void ImageViewVkTest::wrap() {
    Image image{device(), ImageCreateInfo2D{ImageUsage::Sampled,
        PixelFormat::RGBA8Unorm, {256, 256}, 1}, MemoryFlag::DeviceLocal};

    VkImageView view{};
    CORRADE_COMPARE(Result(device()->CreateImageView(device(),
        ImageViewCreateInfo2D{image}, nullptr, &view)), Result::Success);
    CORRADE_VERIFY(view);

    auto wrapped = ImageView::wrap(device(), view, HandleFlag::DestroyOnDestruction);
    CORRADE_COMPARE(wrapped.handle(), view);

    /* Release the handle again, destroy by hand */
    CORRADE_COMPARE(wrapped.release(), view);
    CORRADE_VERIFY(!wrapped.handle());
    device()->DestroyImageView(device(), view, nullptr);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::ImageViewVkTest)
