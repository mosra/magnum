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

#include <Corrade/Containers/StringView.h>
#include <Corrade/TestSuite/Compare/Numeric.h>

#include "Magnum/Vk/DeviceProperties.h"
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

    void bindMemory();
    void bindDedicatedMemory();

    void directAllocation();
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

              &ImageVkTest::memoryRequirements,

              &ImageVkTest::bindMemory,
              &ImageVkTest::bindDedicatedMemory,

              &ImageVkTest::directAllocation});
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
    /* Verify that also the dedicated memory gets moved */
    Image a{device(), ImageCreateInfo2D{ImageUsage::ColorAttachment,
            VK_FORMAT_R8G8B8A8_UNORM, {256, 256}, 1},
        Vk::MemoryFlag::DeviceLocal};
    VkImage handle = a.handle();
    VkDeviceMemory memoryHandle = a.dedicatedMemory().handle();

    Image b = std::move(a);
    CORRADE_VERIFY(!a.handle());
    CORRADE_VERIFY(!a.hasDedicatedMemory());
    CORRADE_COMPARE(b.handle(), handle);
    CORRADE_COMPARE(b.handleFlags(), HandleFlag::DestroyOnDestruction);
    CORRADE_VERIFY(b.hasDedicatedMemory());
    CORRADE_COMPARE(b.dedicatedMemory().handle(), memoryHandle);

    Image c{NoCreate};
    c = std::move(b);
    CORRADE_VERIFY(!b.handle());
    CORRADE_VERIFY(!b.hasDedicatedMemory());
    CORRADE_COMPARE(b.handleFlags(), HandleFlags{});
    CORRADE_COMPARE(c.handle(), handle);
    CORRADE_COMPARE(c.handleFlags(), HandleFlag::DestroyOnDestruction);
    CORRADE_VERIFY(c.hasDedicatedMemory());
    CORRADE_COMPARE(c.dedicatedMemory().handle(), memoryHandle);

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
    {
        /* Can't use device().properties().driver() ==
           DeviceDriver::GoogleSwiftShader because that information is not
           available when we run with KHR_get_physical_device_properties2
           disabled :/ */
        CORRADE_EXPECT_FAIL_IF(device().properties().name().hasPrefix("SwiftShader") && requirements.size() == 128*64*4 + 16,
            "SwiftShader reports 16 bytes (two pointers?) more than expected for a linear tiling.");
        CORRADE_COMPARE(requirements.size(), 128*64*4);
    }
}

void ImageVkTest::bindMemory() {
    Image image{device(), ImageCreateInfo2D{ImageUsage::Sampled,
        VK_FORMAT_R8G8B8A8_UNORM, {256, 256}, 8}, NoAllocate};
    MemoryRequirements requirements = image.memoryRequirements();

    /* We're testing the offset, so ensure what we hardcode is correctly
       aligned. For Intel 1 kB was enough, SwiftShader wanted 4 kB, AMD wants
       128 kB. */
    constexpr UnsignedLong offset = 128*1024;
    CORRADE_COMPARE_AS(offset, requirements.alignment(),
        TestSuite::Compare::Divisible);

    Vk::Memory memory{device(), Vk::MemoryAllocateInfo{
        requirements.size() + offset,
        device().properties().pickMemory(Vk::MemoryFlag::DeviceLocal, requirements.memories())}};

    image.bindMemory(memory, offset);
    CORRADE_VERIFY(!image.hasDedicatedMemory());
}

void ImageVkTest::bindDedicatedMemory() {
    Image image{device(), ImageCreateInfo2D{ImageUsage::Sampled,
        VK_FORMAT_R8G8B8A8_UNORM, {256, 256}, 8}, NoAllocate};
    MemoryRequirements requirements = image.memoryRequirements();

    /** @todo expand once KHR_dedicated_allocation is implemented */

    Vk::Memory memory{device(), Vk::MemoryAllocateInfo{
        requirements.size(),
        device().properties().pickMemory(Vk::MemoryFlag::DeviceLocal, requirements.memories())}};
    VkDeviceMemory handle = memory.handle();
    CORRADE_VERIFY(handle);

    image.bindDedicatedMemory(std::move(memory));
    CORRADE_VERIFY(image.hasDedicatedMemory());
    CORRADE_COMPARE(image.dedicatedMemory().handle(), handle);
}

void ImageVkTest::directAllocation() {
    Image image{device(), ImageCreateInfo2D{ImageUsage::Sampled,
        VK_FORMAT_R8G8B8A8_UNORM, {256, 256}, 8}, Vk::MemoryFlag::DeviceLocal};

    /* Not sure what else to test here */
    CORRADE_VERIFY(image.hasDedicatedMemory());
    CORRADE_VERIFY(image.dedicatedMemory().handle());
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::ImageVkTest)
