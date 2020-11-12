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

#include "Magnum/Vk/Handle.h"
#include "Magnum/Vk/Image.h"
#include "Magnum/Vk/Memory.h"
#include "Magnum/Vk/Result.h"
#include "Magnum/Vk/VulkanTester.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct ImageVkTest: VulkanTester {
    explicit ImageVkTest();

    void construct1D();
    void construct2D();
    void construct3D();
    void construct1DArray();
    void construct2DArray();
    void constructCubeMap();
    void constructCubeMapArray();
    void constructMove();

    void wrap();

    void memoryRequirements();
};

ImageVkTest::ImageVkTest() {
    addTests({&ImageVkTest::construct1D,
              &ImageVkTest::construct2D,
              &ImageVkTest::construct3D,
              &ImageVkTest::construct1DArray,
              &ImageVkTest::construct2DArray,
              &ImageVkTest::constructCubeMap,
              &ImageVkTest::constructCubeMapArray,
              &ImageVkTest::constructMove,

              &ImageVkTest::wrap,

              &ImageVkTest::memoryRequirements});
}

void ImageVkTest::construct1D() {
    {
        Image image{device(), ImageCreateInfo1D{ImageUsage::Sampled,
            VK_FORMAT_R8G8B8A8_UNORM, 256, 8}, NoAllocate};
        CORRADE_VERIFY(image.handle());
        CORRADE_COMPARE(image.handleFlags(), HandleFlag::DestroyOnDestruction);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void ImageVkTest::construct2D() {
    {
        Image image{device(), ImageCreateInfo2D{ImageUsage::Sampled,
            VK_FORMAT_R8G8B8A8_UNORM, {256, 256}, 8}, NoAllocate};
        CORRADE_VERIFY(image.handle());
        CORRADE_COMPARE(image.handleFlags(), HandleFlag::DestroyOnDestruction);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void ImageVkTest::construct3D() {
    {
        Image image{device(), ImageCreateInfo3D{ImageUsage::Sampled,
            VK_FORMAT_R8G8B8A8_UNORM, {256, 256, 64}, 8}, NoAllocate};
        CORRADE_VERIFY(image.handle());
        CORRADE_COMPARE(image.handleFlags(), HandleFlag::DestroyOnDestruction);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void ImageVkTest::construct1DArray() {
    {
        Image image{device(), ImageCreateInfo1DArray{ImageUsage::Sampled,
            VK_FORMAT_R8G8B8A8_UNORM, {256, 64}, 8}, NoAllocate};
        CORRADE_VERIFY(image.handle());
        CORRADE_COMPARE(image.handleFlags(), HandleFlag::DestroyOnDestruction);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void ImageVkTest::construct2DArray() {
    {
        Image image{device(), ImageCreateInfo2DArray{ImageUsage::Sampled,
            VK_FORMAT_R8G8B8A8_UNORM, {256, 256, 64}, 8}, NoAllocate};
        CORRADE_VERIFY(image.handle());
        CORRADE_COMPARE(image.handleFlags(), HandleFlag::DestroyOnDestruction);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void ImageVkTest::constructCubeMap() {
    {
        Image image{device(), ImageCreateInfoCubeMap{ImageUsage::Sampled,
            VK_FORMAT_R8G8B8A8_UNORM, {256, 256}, 8}, NoAllocate};
        CORRADE_VERIFY(image.handle());
        CORRADE_COMPARE(image.handleFlags(), HandleFlag::DestroyOnDestruction);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void ImageVkTest::constructCubeMapArray() {
    {
        Image image{device(), ImageCreateInfoCubeMapArray{ImageUsage::Sampled,
            VK_FORMAT_R8G8B8A8_UNORM, {256, 256, 36}, 8}, NoAllocate};
        CORRADE_VERIFY(image.handle());
        CORRADE_COMPARE(image.handleFlags(), HandleFlag::DestroyOnDestruction);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void ImageVkTest::constructMove() {
    Image a{device(), ImageCreateInfo2D{ImageUsage::ColorAttachment,
            VK_FORMAT_R8G8B8A8_UNORM, {256, 256}, 1}, NoAllocate};
    VkImage handle = a.handle();

    Image b = std::move(a);
    CORRADE_VERIFY(!a.handle());
    CORRADE_COMPARE(b.handle(), handle);
    CORRADE_COMPARE(b.handleFlags(), HandleFlag::DestroyOnDestruction);

    Image c{NoCreate};
    c = std::move(b);
    CORRADE_VERIFY(!b.handle());
    CORRADE_COMPARE(b.handleFlags(), HandleFlags{});
    CORRADE_COMPARE(c.handle(), handle);
    CORRADE_COMPARE(c.handleFlags(), HandleFlag::DestroyOnDestruction);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<Image>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<Image>::value);
}

void ImageVkTest::wrap() {
    VkImage image{};
    CORRADE_COMPARE(Result(device()->CreateImage(device(),
        ImageCreateInfo2D{ImageUsage::Sampled,
            VK_FORMAT_R8G8B8A8_UNORM, {256, 256}, 8},
        nullptr, &image)), Result::Success);
    CORRADE_VERIFY(image);

    auto wrapped = Image::wrap(device(), image, HandleFlag::DestroyOnDestruction);
    CORRADE_COMPARE(wrapped.handle(), image);

    /* Release the handle again, destroy by hand */
    CORRADE_COMPARE(wrapped.release(), image);
    CORRADE_VERIFY(!wrapped.handle());
    device()->DestroyImage(device(), image, nullptr);
}

void ImageVkTest::memoryRequirements() {
    /* Use linear tiling for a deterministic memory size */
    ImageCreateInfo2D info{ImageUsage::Sampled, VK_FORMAT_R8G8B8A8_UNORM, {128, 64}, 1};
    info->tiling = VK_IMAGE_TILING_LINEAR;
    Image image{device(), info, NoAllocate};

    MemoryRequirements requirements = image.memoryRequirements();
    CORRADE_COMPARE(requirements.size(), 128*64*4);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::ImageVkTest)
