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

#include <sstream>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Math/Color.h"
#include "Magnum/Math/Range.h"
#include "Magnum/Vk/BufferCreateInfo.h"
#include "Magnum/Vk/CommandPoolCreateInfo.h"
#include "Magnum/Vk/CommandBuffer.h"
#include "Magnum/Vk/DeviceProperties.h"
#include "Magnum/Vk/Extensions.h"
#include "Magnum/Vk/Fence.h"
#include "Magnum/Vk/Handle.h"
#include "Magnum/Vk/ImageCreateInfo.h"
#include "Magnum/Vk/MemoryAllocateInfo.h"
#include "Magnum/Vk/Pipeline.h"
#include "Magnum/Vk/Result.h"
#include "Magnum/Vk/VulkanTester.h"

#include "Magnum/Vk/Test/pixelFormatTraits.h"

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

    template<class T> void wrap();

    void memoryRequirements();

    void bindMemory();
    void bindDedicatedMemory();

    void directAllocation();

    void cmdClearColorImageFloat();
    void cmdClearColorImageSignedIntegral();
    void cmdClearColorImageUnsignedIntegral();
    void cmdClearDepthStencilImage();
    void cmdClearDepthImage();
    void cmdClearStencilImage();

    void cmdCopyImage2D();
    void cmdCopyImage2DArrayTo3D();
    void cmdCopyImageDisallowedConversion();

    void cmdCopyBufferImage1D();
    void cmdCopyBufferImage2D();
    void cmdCopyBufferImage3D();
    void cmdCopyBufferImage1DArray();
    void cmdCopyBufferImage2DArray();
    void cmdCopyBufferImageCubeMap();
    void cmdCopyBufferImageCubeMapArray();
    void cmdCopyBufferImageDisallowedConversion();
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

              &ImageVkTest::wrap<PixelFormat>,
              &ImageVkTest::wrap<Magnum::PixelFormat>,
              &ImageVkTest::wrap<Magnum::CompressedPixelFormat>,

              &ImageVkTest::memoryRequirements,

              &ImageVkTest::bindMemory,
              &ImageVkTest::bindDedicatedMemory,

              &ImageVkTest::directAllocation,

              &ImageVkTest::cmdClearColorImageFloat,
              &ImageVkTest::cmdClearColorImageSignedIntegral,
              &ImageVkTest::cmdClearColorImageUnsignedIntegral,
              &ImageVkTest::cmdClearDepthStencilImage,
              &ImageVkTest::cmdClearDepthImage,
              &ImageVkTest::cmdClearStencilImage,

              &ImageVkTest::cmdCopyImage2D,
              &ImageVkTest::cmdCopyImage2DArrayTo3D,
              &ImageVkTest::cmdCopyImageDisallowedConversion,

              &ImageVkTest::cmdCopyBufferImage1D,
              &ImageVkTest::cmdCopyBufferImage2D,
              &ImageVkTest::cmdCopyBufferImage3D,
              &ImageVkTest::cmdCopyBufferImage1DArray,
              &ImageVkTest::cmdCopyBufferImage2DArray,
              &ImageVkTest::cmdCopyBufferImageCubeMap,
              &ImageVkTest::cmdCopyBufferImageCubeMapArray,
              &ImageVkTest::cmdCopyBufferImageDisallowedConversion});
}

using namespace Containers::Literals;
using namespace Math::Literals;

void ImageVkTest::construct1D() {
    {
        Image image{device(), ImageCreateInfo1D{ImageUsage::Sampled,
            PixelFormat::RGBA8Unorm, 256, 8}, NoAllocate};
        CORRADE_VERIFY(image.handle());
        CORRADE_COMPARE(image.handleFlags(), HandleFlag::DestroyOnDestruction);
        CORRADE_COMPARE(image.format(), PixelFormat::RGBA8Unorm);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void ImageVkTest::construct2D() {
    {
        Image image{device(), ImageCreateInfo2D{ImageUsage::Sampled,
            PixelFormat::RGBA8Unorm, {256, 256}, 8}, NoAllocate};
        CORRADE_VERIFY(image.handle());
        CORRADE_COMPARE(image.handleFlags(), HandleFlag::DestroyOnDestruction);
        CORRADE_COMPARE(image.format(), PixelFormat::RGBA8Unorm);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void ImageVkTest::construct3D() {
    {
        Image image{device(), ImageCreateInfo3D{ImageUsage::Sampled,
            PixelFormat::RGBA8Unorm, {256, 256, 64}, 8}, NoAllocate};
        CORRADE_VERIFY(image.handle());
        CORRADE_COMPARE(image.handleFlags(), HandleFlag::DestroyOnDestruction);
        CORRADE_COMPARE(image.format(), PixelFormat::RGBA8Unorm);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void ImageVkTest::construct1DArray() {
    {
        Image image{device(), ImageCreateInfo1DArray{ImageUsage::Sampled,
            PixelFormat::RGBA8Unorm, {256, 64}, 8}, NoAllocate};
        CORRADE_VERIFY(image.handle());
        CORRADE_COMPARE(image.handleFlags(), HandleFlag::DestroyOnDestruction);
        CORRADE_COMPARE(image.format(), PixelFormat::RGBA8Unorm);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void ImageVkTest::construct2DArray() {
    {
        Image image{device(), ImageCreateInfo2DArray{ImageUsage::Sampled,
            PixelFormat::RGBA8Unorm, {256, 256, 64}, 8}, NoAllocate};
        CORRADE_VERIFY(image.handle());
        CORRADE_COMPARE(image.handleFlags(), HandleFlag::DestroyOnDestruction);
        CORRADE_COMPARE(image.format(), PixelFormat::RGBA8Unorm);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void ImageVkTest::constructCubeMap() {
    {
        Image image{device(), ImageCreateInfoCubeMap{ImageUsage::Sampled,
            PixelFormat::RGBA8Unorm, {256, 256}, 8}, NoAllocate};
        CORRADE_VERIFY(image.handle());
        CORRADE_COMPARE(image.handleFlags(), HandleFlag::DestroyOnDestruction);
        CORRADE_COMPARE(image.format(), PixelFormat::RGBA8Unorm);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void ImageVkTest::constructCubeMapArray() {
    {
        Image image{device(), ImageCreateInfoCubeMapArray{ImageUsage::Sampled,
            PixelFormat::RGBA8Unorm, {256, 256, 36}, 8}, NoAllocate};
        CORRADE_VERIFY(image.handle());
        CORRADE_COMPARE(image.handleFlags(), HandleFlag::DestroyOnDestruction);
        CORRADE_COMPARE(image.format(), PixelFormat::RGBA8Unorm);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void ImageVkTest::constructMove() {
    /* Verify that also the dedicated memory gets moved */
    Image a{device(), ImageCreateInfo2D{ImageUsage::ColorAttachment,
            PixelFormat::RGBA8Unorm, {256, 256}, 1},
        MemoryFlag::DeviceLocal};
    VkImage handle = a.handle();
    VkDeviceMemory memoryHandle = a.dedicatedMemory().handle();

    Image b = std::move(a);
    CORRADE_VERIFY(!a.handle());
    CORRADE_VERIFY(!a.hasDedicatedMemory());
    CORRADE_COMPARE(b.handle(), handle);
    CORRADE_COMPARE(b.handleFlags(), HandleFlag::DestroyOnDestruction);
    CORRADE_COMPARE(b.format(), PixelFormat::RGBA8Unorm);
    CORRADE_VERIFY(b.hasDedicatedMemory());
    CORRADE_COMPARE(b.dedicatedMemory().handle(), memoryHandle);

    Image c{NoCreate};
    c = std::move(b);
    CORRADE_VERIFY(!b.handle());
    CORRADE_VERIFY(!b.hasDedicatedMemory());
    CORRADE_COMPARE(b.handleFlags(), HandleFlags{});
    CORRADE_COMPARE(c.handle(), handle);
    CORRADE_COMPARE(c.handleFlags(), HandleFlag::DestroyOnDestruction);
    CORRADE_COMPARE(c.format(), PixelFormat::RGBA8Unorm);
    CORRADE_VERIFY(c.hasDedicatedMemory());
    CORRADE_COMPARE(c.dedicatedMemory().handle(), memoryHandle);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<Image>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<Image>::value);
}

template<class T> void ImageVkTest::wrap() {
    setTestCaseTemplateName(PixelFormatTraits<T>::name());

    VkImage image{};
    CORRADE_COMPARE(Result(device()->CreateImage(device(),
        ImageCreateInfo2D{ImageUsage::Sampled,
            PixelFormatTraits<T>::format(), {256, 256}, 8},
        nullptr, &image)), Result::Success);
    CORRADE_VERIFY(image);

    auto wrapped = Image::wrap(device(), image, PixelFormatTraits<T>::format(), HandleFlag::DestroyOnDestruction);
    CORRADE_COMPARE(wrapped.handle(), image);
    CORRADE_COMPARE(wrapped.format(), PixelFormat(PixelFormatTraits<T>::expected()));

    /* Release the handle again, destroy by hand */
    CORRADE_COMPARE(wrapped.release(), image);
    CORRADE_VERIFY(!wrapped.handle());
    device()->DestroyImage(device(), image, nullptr);
}

void ImageVkTest::memoryRequirements() {
    /* Use linear tiling for a deterministic memory size */
    ImageCreateInfo2D info{ImageUsage::TransferDestination, PixelFormat::RGBA8Unorm, {128, 64}, 1};
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
        PixelFormat::RGBA8Unorm, {256, 256}, 8}, NoAllocate};
    MemoryRequirements requirements = image.memoryRequirements();

    /* We're testing the offset, so ensure what we hardcode is correctly
       aligned. For Intel 1 kB was enough, SwiftShader wanted 4 kB, AMD wants
       128 kB. */
    constexpr UnsignedLong offset = 128*1024;
    CORRADE_COMPARE_AS(offset, requirements.alignment(),
        TestSuite::Compare::Divisible);

    Memory memory{device(), MemoryAllocateInfo{
        requirements.size() + offset,
        device().properties().pickMemory(MemoryFlag::DeviceLocal, requirements.memories())}};

    image.bindMemory(memory, offset);
    CORRADE_VERIFY(!image.hasDedicatedMemory());
}

void ImageVkTest::bindDedicatedMemory() {
    Image image{device(), ImageCreateInfo2D{ImageUsage::Sampled,
        PixelFormat::RGBA8Unorm, {256, 256}, 8}, NoAllocate};
    MemoryRequirements requirements = image.memoryRequirements();

    /** @todo expand once KHR_dedicated_allocation is implemented */

    Memory memory{device(), MemoryAllocateInfo{
        requirements.size(),
        device().properties().pickMemory(MemoryFlag::DeviceLocal, requirements.memories())}};
    VkDeviceMemory handle = memory.handle();
    CORRADE_VERIFY(handle);

    image.bindDedicatedMemory(std::move(memory));
    CORRADE_VERIFY(image.hasDedicatedMemory());
    CORRADE_COMPARE(image.dedicatedMemory().handle(), handle);
}

void ImageVkTest::directAllocation() {
    Image image{device(), ImageCreateInfo2D{ImageUsage::Sampled,
        PixelFormat::RGBA8Unorm, {256, 256}, 8}, MemoryFlag::DeviceLocal};

    /* Not sure what else to test here */
    CORRADE_VERIFY(image.hasDedicatedMemory());
    CORRADE_VERIFY(image.dedicatedMemory().handle());
}

void ImageVkTest::cmdClearColorImageFloat() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};
    CommandBuffer cmd = pool.allocate();

    /* To avoid going through a buffer which can guarantee the packing we want,
       the tests uses a linear tiling image. These are poorly supported, have
       weird paddings and the required allocation size is usually much larger
       than expected. To prevent issues as much as possible, we'll thus create
       images with non-insane sizes, 4-byte-aligned pixel format and explicitly
       slice the mapped memory. */

    /* Source image */
    ImageCreateInfo2D aInfo{ImageUsage::TransferDestination,
        PixelFormat::RGBA8Unorm, {4, 4}, 1, 1, ImageLayout::Undefined};
    aInfo->tiling = VK_IMAGE_TILING_LINEAR;
    Image a{device(), aInfo, MemoryFlag::HostVisible};

    cmd.begin()
       .pipelineBarrier(PipelineStage::TopOfPipe, PipelineStage::Transfer, {
            {{}, Access::TransferWrite,
             ImageLayout::Undefined, ImageLayout::TransferDestination, a},
        })
       .clearColorImage(a, ImageLayout::TransferDestination, 0xdeadc0de_rgbaf)
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Host, {
           {Access::TransferWrite, Access::HostRead}
        })
       .end();
    queue().submit({SubmitInfo{}.setCommandBuffers({cmd})}).wait();

    CORRADE_COMPARE_AS(Containers::arrayCast<const Color4ub>(a.dedicatedMemory().mapRead().prefix(4*4*4)), Containers::arrayView({
        0xdeadc0de_rgba, 0xdeadc0de_rgba, 0xdeadc0de_rgba, 0xdeadc0de_rgba,
        0xdeadc0de_rgba, 0xdeadc0de_rgba, 0xdeadc0de_rgba, 0xdeadc0de_rgba,
        0xdeadc0de_rgba, 0xdeadc0de_rgba, 0xdeadc0de_rgba, 0xdeadc0de_rgba,
        0xdeadc0de_rgba, 0xdeadc0de_rgba, 0xdeadc0de_rgba, 0xdeadc0de_rgba
    }), TestSuite::Compare::Container);
}

void ImageVkTest::cmdClearColorImageSignedIntegral() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};
    CommandBuffer cmd = pool.allocate();

    /* To avoid going through a buffer which can guarantee the packing we want,
       the tests uses a linear tiling image. These are poorly supported, have
       weird paddings and the required allocation size is usually much larger
       than expected. To prevent issues as much as possible, we'll thus create
       images with non-insane sizes, 4-byte pixel format and explicitly slice
       the mapped memory. */

    /* Source image */
    ImageCreateInfo2D aInfo{ImageUsage::TransferDestination,
        PixelFormat::RGBA8I, {4, 4}, 1, 1, ImageLayout::Undefined};
    aInfo->tiling = VK_IMAGE_TILING_LINEAR;
    Image a{device(), aInfo, MemoryFlag::HostVisible};

    cmd.begin()
       .pipelineBarrier(PipelineStage::TopOfPipe, PipelineStage::Transfer, {
            {{}, Access::TransferWrite,
             ImageLayout::Undefined, ImageLayout::TransferDestination, a},
        })
       .clearColorImage(a, ImageLayout::TransferDestination, Vector4i{15, -7, 2, -1})
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Host, {
           {Access::TransferWrite, Access::HostRead}
        })
       .end();
    queue().submit({SubmitInfo{}.setCommandBuffers({cmd})}).wait();

    CORRADE_COMPARE_AS(Containers::arrayCast<const Vector4b>(a.dedicatedMemory().mapRead().prefix(4*4*4)), Containers::arrayView<Vector4b>({
        {15, -7, 2, -1}, {15, -7, 2, -1}, {15, -7, 2, -1}, {15, -7, 2, -1},
        {15, -7, 2, -1}, {15, -7, 2, -1}, {15, -7, 2, -1}, {15, -7, 2, -1},
        {15, -7, 2, -1}, {15, -7, 2, -1}, {15, -7, 2, -1}, {15, -7, 2, -1},
        {15, -7, 2, -1}, {15, -7, 2, -1}, {15, -7, 2, -1}, {15, -7, 2, -1},
    }), TestSuite::Compare::Container);
}

void ImageVkTest::cmdClearColorImageUnsignedIntegral() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};
    CommandBuffer cmd = pool.allocate();

    /* To avoid going through a buffer which can guarantee the packing we want,
       the tests uses a linear tiling image. These are poorly supported, have
       weird paddings and the required allocation size is usually much larger
       than expected. To prevent issues as much as possible, we'll thus create
       images with non-insane sizes, 4-byte pixel format and explicitly slice
       the mapped memory. */

    /* Source image */
    ImageCreateInfo2D aInfo{ImageUsage::TransferDestination,
        PixelFormat::RGBA8UI, {4, 4}, 1, 1, ImageLayout::Undefined};
    aInfo->tiling = VK_IMAGE_TILING_LINEAR;
    Image a{device(), aInfo, MemoryFlag::HostVisible};

    cmd.begin()
       .pipelineBarrier(PipelineStage::TopOfPipe, PipelineStage::Transfer, {
            {{}, Access::TransferWrite,
             ImageLayout::Undefined, ImageLayout::TransferDestination, a},
        })
       .clearColorImage(a, ImageLayout::TransferDestination, Vector4ui{15, 37, 2, 1})
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Host, {
           {Access::TransferWrite, Access::HostRead}
        })
       .end();
    queue().submit({SubmitInfo{}.setCommandBuffers({cmd})}).wait();

    CORRADE_COMPARE_AS(Containers::arrayCast<const Vector4ub>(a.dedicatedMemory().mapRead().prefix(4*4*4)), Containers::arrayView<Vector4ub>({
        {15, 37, 2, 1}, {15, 37, 2, 1}, {15, 37, 2, 1}, {15, 37, 2, 1},
        {15, 37, 2, 1}, {15, 37, 2, 1}, {15, 37, 2, 1}, {15, 37, 2, 1},
        {15, 37, 2, 1}, {15, 37, 2, 1}, {15, 37, 2, 1}, {15, 37, 2, 1},
        {15, 37, 2, 1}, {15, 37, 2, 1}, {15, 37, 2, 1}, {15, 37, 2, 1},
    }), TestSuite::Compare::Container);
}

void ImageVkTest::cmdClearDepthStencilImage() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};
    CommandBuffer cmd = pool.allocate();

    /* Depth/stencil images aren't supported in a linear tiling, so do the
       verification through a buffer copy */

    /* Source image */
    Image a{device(), ImageCreateInfo2D{
        ImageUsage::TransferDestination|ImageUsage::TransferSource,
        PixelFormat::Depth32FStencil8UI, {4, 4}, 1, 1, ImageLayout::Undefined
    }, MemoryFlag::DeviceLocal};

    /* Destination buffers */
    Buffer depth{device(), BufferCreateInfo{
        BufferUsage::TransferDestination, 4*4*4
    }, MemoryFlag::HostVisible};
    Buffer stencil{device(), BufferCreateInfo{
        BufferUsage::TransferDestination, 4*4
    }, MemoryFlag::HostVisible};

    cmd.begin()
       .pipelineBarrier(PipelineStage::TopOfPipe, PipelineStage::Transfer, {
            {{}, Access::TransferWrite,
             ImageLayout::Undefined, ImageLayout::TransferDestination, a},
        })
       .clearDepthStencilImage(a, ImageLayout::TransferDestination, 0.75f, 133)
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Transfer, {
            {Access::TransferWrite, Access::TransferRead,
             ImageLayout::TransferDestination, ImageLayout::TransferSource, a},
        })
       .copyImageToBuffer(CopyImageToBufferInfo2D{a, ImageLayout::TransferSource, depth, {
           {0, ImageAspect::Depth, 0, {{}, {4, 4}}}
        }})
       .copyImageToBuffer(CopyImageToBufferInfo2D{a, ImageLayout::TransferSource, stencil, {
           {0, ImageAspect::Stencil, 0, {{}, {4, 4}}}
        }})
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Host, {
           {Access::TransferWrite, Access::HostRead}
        })
       .end();
    queue().submit({SubmitInfo{}.setCommandBuffers({cmd})}).wait();

    CORRADE_COMPARE_AS((Containers::arrayCast<const Float>(depth.dedicatedMemory().mapRead().prefix(4*4*4))), (Containers::arrayView<Float>({
        0.75f, 0.75f, 0.75f, 0.75f,
        0.75f, 0.75f, 0.75f, 0.75f,
        0.75f, 0.75f, 0.75f, 0.75f,
        0.75f, 0.75f, 0.75f, 0.75f
    })), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS((Containers::arrayCast<const UnsignedByte>(stencil.dedicatedMemory().mapRead().prefix(4*4))), (Containers::arrayView<UnsignedByte>({
        133, 133, 133, 133,
        133, 133, 133, 133,
        133, 133, 133, 133,
        133, 133, 133, 133
    })), TestSuite::Compare::Container);
}

void ImageVkTest::cmdClearDepthImage() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};
    CommandBuffer cmd = pool.allocate();

    /* Depth/stencil images aren't supported in a linear tiling, so do the
       verification through a buffer copy */

    /* Source image */
    Image a{device(), ImageCreateInfo2D{
        ImageUsage::TransferDestination|ImageUsage::TransferSource,
        PixelFormat::Depth32F, {4, 4}, 1, 1, ImageLayout::Undefined
    }, MemoryFlag::DeviceLocal};

    /* Destination buffers */
    Buffer depth{device(), BufferCreateInfo{
        BufferUsage::TransferDestination, 4*4*4
    }, MemoryFlag::HostVisible};

    cmd.begin()
       .pipelineBarrier(PipelineStage::TopOfPipe, PipelineStage::Transfer, {
            {{}, Access::TransferWrite,
             ImageLayout::Undefined, ImageLayout::TransferDestination, a},
        })
       .clearDepthImage(a, ImageLayout::TransferDestination, 0.75f)
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Transfer, {
            {Access::TransferWrite, Access::TransferRead,
             ImageLayout::TransferDestination, ImageLayout::TransferSource, a},
        })
       .copyImageToBuffer(CopyImageToBufferInfo2D{a, ImageLayout::TransferSource, depth, {
           {0, ImageAspect::Depth, 0, {{}, {4, 4}}}
        }})
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Host, {
           {Access::TransferWrite, Access::HostRead}
        })
       .end();
    queue().submit({SubmitInfo{}.setCommandBuffers({cmd})}).wait();

    CORRADE_COMPARE_AS((Containers::arrayCast<const Float>(depth.dedicatedMemory().mapRead().prefix(4*4*4))), (Containers::arrayView<Float>({
        0.75f, 0.75f, 0.75f, 0.75f,
        0.75f, 0.75f, 0.75f, 0.75f,
        0.75f, 0.75f, 0.75f, 0.75f,
        0.75f, 0.75f, 0.75f, 0.75f
    })), TestSuite::Compare::Container);
}

void ImageVkTest::cmdClearStencilImage() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};
    CommandBuffer cmd = pool.allocate();

    /* Depth/stencil images aren't supported in a linear tiling, so do the
       verification through a buffer copy */

    /* Source image */
    Image a{device(), ImageCreateInfo2D{
        ImageUsage::TransferDestination|ImageUsage::TransferSource,
        PixelFormat::Stencil8UI, {4, 4}, 1, 1, ImageLayout::Undefined
    }, MemoryFlag::DeviceLocal};

    /* Destination buffers */
    Buffer stencil{device(), BufferCreateInfo{
        BufferUsage::TransferDestination, 4*4
    }, MemoryFlag::HostVisible};

    cmd.begin()
       .pipelineBarrier(PipelineStage::TopOfPipe, PipelineStage::Transfer, {
            {{}, Access::TransferWrite,
             ImageLayout::Undefined, ImageLayout::TransferDestination, a},
        })
       .clearStencilImage(a, ImageLayout::TransferDestination, 133)
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Transfer, {
            {Access::TransferWrite, Access::TransferRead,
             ImageLayout::TransferDestination, ImageLayout::TransferSource, a},
        })
       .copyImageToBuffer(CopyImageToBufferInfo2D{a, ImageLayout::TransferSource, stencil, {
           {0, ImageAspect::Stencil, 0, {{}, {4, 4}}}
        }})
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Host, {
           {Access::TransferWrite, Access::HostRead}
        })
       .end();
    queue().submit({SubmitInfo{}.setCommandBuffers({cmd})}).wait();

    CORRADE_COMPARE_AS((Containers::arrayCast<const UnsignedByte>(stencil.dedicatedMemory().mapRead().prefix(4*4))), (Containers::arrayView<UnsignedByte>({
        133, 133, 133, 133,
        133, 133, 133, 133,
        133, 133, 133, 133,
        133, 133, 133, 133
    })), TestSuite::Compare::Container);
}

void ImageVkTest::cmdCopyImage2D() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};
    CommandBuffer cmd = pool.allocate();

    /* To avoid going through a buffer which can guarantee the packing we want,
       the tests uses a linear tiling image. These are poorly supported, have
       weird paddings and the required allocation size is usually much larger
       than expected. To prevent issues as much as possible, we'll thus create
       images with non-insane sizes (so not 6 or 7 pixels wide, but 8), 4-byte
       pixel format and explicitly slice the mapped memory. */

    /* Source image */
    ImageCreateInfo2D aInfo{ImageUsage::TransferSource,
        PixelFormat::RGBA8UI, {8, 10}, 1, 1, ImageLayout::Preinitialized};
    aInfo->tiling = VK_IMAGE_TILING_LINEAR;
    Image a{device(), aInfo, MemoryFlag::HostVisible};
    Utility::copy("________________________________"
                  "________________________________"
                  "________________________________"
                  "________________________________"
                  "____________AaaaAaaaAaaaAaaa____"
                  "____________BbbbBbbbBbbbBbbb____"
                  "____________CcccCcccCcccCccc____"
                  "____________DdddDdddDdddDddd____"
                  "________________________________"
                  "________________________________"_s, a.dedicatedMemory().map().prefix(8*10*4));

    /* Destination image */
    ImageCreateInfo2D bInfo{ImageUsage::TransferDestination,
        PixelFormat::RGBA8UI, {8, 5}, 1};
    bInfo->tiling = VK_IMAGE_TILING_LINEAR;
    Image b{device(), bInfo, MemoryFlag::HostVisible};

    cmd.begin()
       .pipelineBarrier(PipelineStage::TopOfPipe, PipelineStage::Transfer, {
            {Accesses{}, Access::TransferRead,
             ImageLayout::Preinitialized, ImageLayout::TransferSource, a},
            {Accesses{}, Access::TransferWrite,
             ImageLayout::Undefined, ImageLayout::TransferDestination, b},
        })
       .clearColorImage(b, ImageLayout::TransferDestination, Vector4ui{'-'})
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Transfer, {
            {Access::TransferWrite, Access::TransferWrite,
             ImageLayout::TransferDestination, ImageLayout::TransferDestination, b}
        })
       .copyImage({a, ImageLayout::TransferSource, b, ImageLayout::TransferDestination, {
            {ImageAspect::Color, 0, 0, 1, {3, 4, 0}, 0, 0, 1, {1, 1, 0}, {4, 4, 1}}
        }})
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Host, {
            {Access::TransferWrite, Access::HostRead,
             ImageLayout::TransferDestination, ImageLayout::TransferDestination, b}
        })
       .end();
    queue().submit({SubmitInfo{}.setCommandBuffers({cmd})}).wait();

    CORRADE_COMPARE(b.dedicatedMemory().mapRead().prefix(8*5*4),
        "--------------------------------"
        "----AaaaAaaaAaaaAaaa------------"
        "----BbbbBbbbBbbbBbbb------------"
        "----CcccCcccCcccCccc------------"
        "----DdddDdddDdddDddd------------"_s);
}

void ImageVkTest::cmdCopyImage2DArrayTo3D() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};
    CommandBuffer cmd = pool.allocate();

    /* Mostly just to test the assertions in the
       swiftshader-image-copy-extent-instead-of-layers workaround, but also if
       I actually understand the overcomplicated ambiguous parameters
       correctly. Apparently array/3D images can't be linear on SwiftShader,
       so going through a buffer instead. */

    /* Source buffer */
    Buffer a{device(), BufferCreateInfo{
        BufferUsage::TransferSource, 8*4*2*4
    }, MemoryFlag::HostVisible};
    Utility::copy("________________________________"
                  "________________________________"

                  "____________AaaaAaaaAaaaAaaa____"
                  "____________BbbbBbbbBbbbBbbb____"

                  "____________CcccCcccCcccCccc____"
                  "____________DdddDdddDdddDddd____"

                  "________________________________"
                  "________________________________"_s, a.dedicatedMemory().map());

    /* Source 2D array image, created from the buffer */
    Image b{device(), ImageCreateInfo2DArray{
        ImageUsage::TransferDestination|ImageUsage::TransferSource,
        PixelFormat::RGBA8UI, {8, 2, 4}, 1
    }, MemoryFlag::DeviceLocal};

    /* Destination 3D image, copied to a destination buffer */
    Image c{device(), ImageCreateInfo3D{
        ImageUsage::TransferDestination|ImageUsage::TransferSource,
        PixelFormat::RGBA8UI, {8, 4, 2}, 1
    }, MemoryFlag::DeviceLocal};

    /* Destination buffer */
    Buffer d{device(), BufferCreateInfo{
        BufferUsage::TransferDestination, 8*4*2*4
    }, MemoryFlag::HostVisible};

    cmd.begin()
       .pipelineBarrier(PipelineStage::TopOfPipe, PipelineStage::Transfer, {
            {Accesses{}, Access::TransferWrite,
             ImageLayout::Undefined, ImageLayout::TransferDestination, b},
            {Accesses{}, Access::TransferWrite,
             ImageLayout::Undefined, ImageLayout::TransferDestination, c}
        })
       .copyBufferToImage({a, b, ImageLayout::TransferDestination, {
            BufferImageCopy2DArray{0, ImageAspect::Color, 0, {{}, {8, 2, 4}}}
        }})
       .clearColorImage(c, ImageLayout::TransferDestination, Vector4ui{'-'})
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Transfer, {
            {Access::TransferWrite, Access::TransferRead,
             ImageLayout::TransferDestination, ImageLayout::TransferSource, b},
            {Access::TransferWrite, Access::TransferWrite,
             ImageLayout::TransferDestination, ImageLayout::TransferDestination, c}
        })
       .copyImage({b, ImageLayout::TransferSource, c, ImageLayout::TransferDestination, {
            {ImageAspect::Color, 0, 1, 2, {3, 0, 0}, 0, 0, 1, {1, 1, 0}, {4, 2, 2}}
        }})
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Transfer, {
            {Access::TransferWrite, Access::TransferRead,
             ImageLayout::TransferDestination, ImageLayout::TransferSource, c}
        })
       .copyImageToBuffer({c, ImageLayout::TransferSource, d, {
           BufferImageCopy3D{0, ImageAspect::Color, 0, {{}, {8, 4, 2}}}
        }})
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Host, {
            {Access::TransferWrite, Access::HostRead, d}
        })
       .end();
    queue().submit({SubmitInfo{}.setCommandBuffers({cmd})}).wait();

    CORRADE_COMPARE(arrayView(d.dedicatedMemory().mapRead()),
        "--------------------------------"
        "----AaaaAaaaAaaaAaaa------------"
        "----BbbbBbbbBbbbBbbb------------"
        "--------------------------------"

        "--------------------------------"
        "----CcccCcccCcccCccc------------"
        "----DdddDdddDdddDddd------------"
        "--------------------------------"_s);
}

void ImageVkTest::cmdCopyImageDisallowedConversion() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    if(device().isExtensionEnabled<Extensions::KHR::copy_commands2>())
        CORRADE_SKIP("KHR_copy_commands2 enabled on the device, can't test");

    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};
    CommandBuffer cmd = pool.allocate();

    CopyImageInfo a{{}, {}, {}, {}, {}};
    a->pNext = &a;

    /* The commands shouldn't do anything, so it should be fine to just call
       them without any render pass set up */
    std::ostringstream out;
    Error redirectError{&out};
    cmd.copyImage(a);
    CORRADE_COMPARE(out.str(),
        "Vk::CommandBuffer::copyImage(): disallowing extraction of CopyImageInfo with non-empty pNext to prevent information loss\n");
}

void ImageVkTest::cmdCopyBufferImage1D() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};
    CommandBuffer cmd = pool.allocate();

    /* Source buffer */
    Buffer a{device(), BufferCreateInfo{
        BufferUsage::TransferSource, 7*4
    }, MemoryFlag::HostVisible};
    Utility::copy("________AaaaBbbbCcccDddd____"_s, a.dedicatedMemory().map());

    /* Destination & source image */
    Image b{device(), ImageCreateInfo1D{
        ImageUsage::TransferDestination|ImageUsage::TransferSource,
        PixelFormat::RGBA8UI, 6, 1
    }, MemoryFlag::HostVisible};

    /* Destination buffer, clear as well */
    Buffer c{device(), BufferCreateInfo{
        BufferUsage::TransferDestination, 7*4
    }, MemoryFlag::HostVisible};
    Utility::copy("............................"_s, c.dedicatedMemory().map());

    cmd.begin()
       .pipelineBarrier(PipelineStage::TopOfPipe, PipelineStage::Transfer, {
            {Accesses{}, Access::TransferWrite,
             ImageLayout::Undefined, ImageLayout::TransferDestination, b}
        })
       .fillBuffer(c, 0x2e2e2e2e)
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Transfer, {
            {Access::TransferWrite, Access::TransferRead, c}
        })
       .copyBufferToImage(CopyBufferToImageInfo1D{a, b, ImageLayout::TransferDestination, {
           {2*4, ImageAspect::Color, 0, Range1Di::fromSize(2, 4)}
        }})
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Transfer, {
            {Access::TransferWrite, Access::TransferRead,
             ImageLayout::TransferDestination, ImageLayout::TransferSource, b}
        })
       .copyImageToBuffer(CopyImageToBufferInfo1D{b, ImageLayout::TransferSource, c, {
           {2*4, ImageAspect::Color, 0, Range1Di::fromSize(2, 4)}
        }})
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Host, {
           {Access::TransferWrite, Access::HostRead, c}
        })
       .end();
    queue().submit({SubmitInfo{}.setCommandBuffers({cmd})}).wait();

    CORRADE_COMPARE(arrayView(c.dedicatedMemory().mapRead()),
        "........AaaaBbbbCcccDddd...."_s);
}

void ImageVkTest::cmdCopyBufferImage2D() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};
    CommandBuffer cmd = pool.allocate();

    /* Source buffer */
    Buffer a{device(), BufferCreateInfo{
        BufferUsage::TransferSource, 7*10*4
    }, MemoryFlag::HostVisible};
    Utility::copy("____________________________"
                  "____________________________"
                  "____________________________"
                  "____________________________"
                  "________AaaaAaaaAaaaAaaa____"
                  "________BbbbBbbbBbbbBbbb____"
                  "________CcccCcccCcccCccc____"
                  "________DdddDdddDdddDddd____"
                  "____________________________"
                  "____________________________"_s, a.dedicatedMemory().map());

    /* Destination & source image */
    Image b{device(), ImageCreateInfo2D{
        ImageUsage::TransferDestination|ImageUsage::TransferSource,
        PixelFormat::RGBA8UI, {6, 5}, 1
    }, MemoryFlag::HostVisible};

    /* Destination buffer */
    Buffer c{device(), BufferCreateInfo{
        BufferUsage::TransferDestination, 7*10*4
    }, MemoryFlag::HostVisible};

    cmd.begin()
       .pipelineBarrier(PipelineStage::TopOfPipe, PipelineStage::Transfer, {
            {Accesses{}, Access::TransferWrite,
             ImageLayout::Undefined, ImageLayout::TransferDestination, b}
        })
       .fillBuffer(c, 0x2e2e2e2e)
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Transfer, {
            {Access::TransferWrite, Access::TransferRead, c}
        })
       .copyBufferToImage(CopyBufferToImageInfo2D{a, b, ImageLayout::TransferDestination, {
           {(4*7 + 2)*4, 7, ImageAspect::Color, 0, Range2Di::fromSize({2, 1}, {4, 4})}
        }})
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Transfer, {
            {Access::TransferWrite, Access::TransferRead,
             ImageLayout::TransferDestination, ImageLayout::TransferSource, b}
        })
       .copyImageToBuffer(CopyImageToBufferInfo2D{b, ImageLayout::TransferSource, c, {
           {(4*7 + 2)*4, 7, ImageAspect::Color, 0, Range2Di::fromSize({2, 1}, {4, 4})}
        }})
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Host, {
           {Access::TransferWrite, Access::HostRead, c}
        })
       .end();
    queue().submit({SubmitInfo{}.setCommandBuffers({cmd})}).wait();

    CORRADE_COMPARE(arrayView(c.dedicatedMemory().mapRead()),
        "............................"
        "............................"
        "............................"
        "............................"
        "........AaaaAaaaAaaaAaaa...."
        "........BbbbBbbbBbbbBbbb...."
        "........CcccCcccCcccCccc...."
        "........DdddDdddDdddDddd...."
        "............................"
        "............................"_s);
}

void ImageVkTest::cmdCopyBufferImage3D() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};
    CommandBuffer cmd = pool.allocate();

    /* Source buffer */
    Buffer a{device(), BufferCreateInfo{
        BufferUsage::TransferSource, 6*7*2*4
    }, MemoryFlag::HostVisible};
    Utility::copy("________________________"
                  "________________________"
                  "________________________"
                  "________AaaaAaaaAaaa____"
                  "________BbbbBbbbBbbb____"
                  "________CcccCcccCccc____"
                  "________________________"

                  "________________________"
                  "________________________"
                  "________________________"
                  "________DdddDdddDddd____"
                  "________EeeeEeeeEeee____"
                  "________FfffFfffFfff____"
                  "________________________"_s, a.dedicatedMemory().map());

    /* Destination & source image */
    Image b{device(), ImageCreateInfo3D{
        ImageUsage::TransferDestination|ImageUsage::TransferSource,
        PixelFormat::RGBA8UI, {5, 4, 3}, 1
    }, MemoryFlag::HostVisible};

    /* Destination buffer */
    Buffer c{device(), BufferCreateInfo{
        BufferUsage::TransferDestination, 6*7*2*4
    }, MemoryFlag::HostVisible};

    cmd.begin()
       .pipelineBarrier(PipelineStage::TopOfPipe, PipelineStage::Transfer, {
            {Accesses{}, Access::TransferWrite,
             ImageLayout::Undefined, ImageLayout::TransferDestination, b}
        })
       .fillBuffer(c, 0x2e2e2e2e)
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Transfer, {
            {Access::TransferWrite, Access::TransferRead, c}
        })
       .copyBufferToImage(CopyBufferToImageInfo3D{a, b, ImageLayout::TransferDestination, {
           {(3*6 + 2)*4, 6, 7, ImageAspect::Color, 0, Range3Di::fromSize({2, 1, 1}, {3, 3, 2})}
        }})
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Transfer, {
            {Access::TransferWrite, Access::TransferRead,
             ImageLayout::TransferDestination, ImageLayout::TransferSource, b}
        })
       .copyImageToBuffer(CopyImageToBufferInfo3D{b, ImageLayout::TransferSource, c, {
           {(3*6 + 2)*4, 6, 7, ImageAspect::Color, 0, Range3Di::fromSize({2, 1, 1}, {3, 3, 2})}
        }})
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Host, {
           {Access::TransferWrite, Access::HostRead, c}
        })
       .end();
    queue().submit({SubmitInfo{}.setCommandBuffers({cmd})}).wait();

    CORRADE_COMPARE(arrayView(c.dedicatedMemory().mapRead()),
        "........................"
        "........................"
        "........................"
        "........AaaaAaaaAaaa...."
        "........BbbbBbbbBbbb...."
        "........CcccCcccCccc...."
        "........................"

        "........................"
        "........................"
        "........................"
        "........DdddDdddDddd...."
        "........EeeeEeeeEeee...."
        "........FfffFfffFfff...."
        "........................"_s);
}

void ImageVkTest::cmdCopyBufferImage1DArray() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};
    CommandBuffer cmd = pool.allocate();

    /* Source buffer */
    Buffer a{device(), BufferCreateInfo{
        BufferUsage::TransferSource, 6*5*4
    }, MemoryFlag::HostVisible};
    Utility::copy("________________________"
                  "________________________"
                  "________AaaaAaaaAaaa____"
                  "________BbbbBbbbBbbb____"
                  "________________________"_s, a.dedicatedMemory().map());

    /* Destination & source image */
    Image b{device(), ImageCreateInfo1DArray{
        ImageUsage::TransferDestination|ImageUsage::TransferSource,
        PixelFormat::RGBA8UI, {4, 3}, 1
    }, MemoryFlag::HostVisible};

    /* Destination buffer */
    Buffer c{device(), BufferCreateInfo{BufferUsage::TransferDestination, 6*5*4}, MemoryFlag::HostVisible};

    cmd.begin()
       .pipelineBarrier(PipelineStage::TopOfPipe, PipelineStage::Transfer, {
            {Accesses{}, Access::TransferWrite,
             ImageLayout::Undefined, ImageLayout::TransferDestination, b}
        })
       .fillBuffer(c, 0x2e2e2e2e)
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Transfer, {
            {Access::TransferWrite, Access::TransferRead, c}
        })
       .copyBufferToImage(CopyBufferToImageInfo1DArray{a, b, ImageLayout::TransferDestination, {
           {(2*6 + 2)*4, 6, ImageAspect::Color, 0, Range2Di::fromSize({1, 1}, {3, 2})}
        }})
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Transfer, {
            {Access::TransferWrite, Access::TransferRead,
             ImageLayout::TransferDestination, ImageLayout::TransferSource, b}
        })
       .copyImageToBuffer(CopyImageToBufferInfo1DArray{b, ImageLayout::TransferSource, c, {
           {(2*6 + 2)*4, 6, ImageAspect::Color, 0, Range2Di::fromSize({1, 1}, {3, 2})}
        }})
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Host, {
           {Access::TransferWrite, Access::HostRead, c}
        })
       .end();
    queue().submit({SubmitInfo{}.setCommandBuffers({cmd})}).wait();

    CORRADE_COMPARE(arrayView(c.dedicatedMemory().mapRead()),
        "........................"
        "........................"
        "........AaaaAaaaAaaa...."
        "........BbbbBbbbBbbb...."
        "........................"_s);
}

void ImageVkTest::cmdCopyBufferImage2DArray() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};
    CommandBuffer cmd = pool.allocate();

    /* Source buffer */
    Buffer a{device(), BufferCreateInfo{
        BufferUsage::TransferSource, 4*5*2*4
    }, MemoryFlag::HostVisible};
    Utility::copy("____________________"
                  "________AaaaAaaa____"
                  "________BbbbBbbb____"
                  "____________________"

                  "____________________"
                  "________CcccCccc____"
                  "________DdddDddd____"
                  "____________________"_s, a.dedicatedMemory().map());

    /* Destination & source image */
    Image b{device(), ImageCreateInfo2DArray{
        ImageUsage::TransferDestination|ImageUsage::TransferSource,
        PixelFormat::RGBA8UI, {4, 4, 3}, 1
    }, MemoryFlag::HostVisible};

    /* Destination buffer */
    Buffer c{device(), BufferCreateInfo{BufferUsage::TransferDestination, 4*5*2*4}, MemoryFlag::HostVisible};

    cmd.begin()
       .pipelineBarrier(PipelineStage::TopOfPipe, PipelineStage::Transfer, {
            {Accesses{}, Access::TransferWrite,
             ImageLayout::Undefined, ImageLayout::TransferDestination, b}
        })
       .fillBuffer(c, 0x2e2e2e2e)
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Transfer, {
            {Access::TransferWrite, Access::TransferRead, c}
        })
       .copyBufferToImage(CopyBufferToImageInfo2DArray{a, b, ImageLayout::TransferDestination, {
           {(1*5 + 2)*4, 5, 4, ImageAspect::Color, 0, Range3Di::fromSize({2, 1, 1}, {2, 2, 2})}
        }})
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Transfer, {
            {Access::TransferWrite, Access::TransferRead,
             ImageLayout::TransferDestination, ImageLayout::TransferSource, b}
        })
       .copyImageToBuffer(CopyImageToBufferInfo2DArray{b, ImageLayout::TransferSource, c, {
           {(1*5 + 2)*4, 5, 4, ImageAspect::Color, 0, Range3Di::fromSize({2, 1, 1}, {2, 2, 2})}
        }})
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Host, {
           {Access::TransferWrite, Access::HostRead, c}
        })
       .end();
    queue().submit({SubmitInfo{}.setCommandBuffers({cmd})}).wait();

    CORRADE_COMPARE(arrayView(c.dedicatedMemory().mapRead()),
        "...................."
        "........AaaaAaaa...."
        "........BbbbBbbb...."
        "...................."

        "...................."
        "........CcccCccc...."
        "........DdddDddd...."
        "...................."_s);
}

void ImageVkTest::cmdCopyBufferImageCubeMap() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};
    CommandBuffer cmd = pool.allocate();

    /* Source buffer */
    Buffer a{device(), BufferCreateInfo{
        BufferUsage::TransferSource, 3*4*6*4
    }, MemoryFlag::HostVisible};
    Utility::copy("________________"
                  "________Aaaa____"
                  "________Bbbb____"

                  "________________"
                  "________Cccc____"
                  "________Dddd____"

                  "________________"
                  "________Eeee____"
                  "________Ffff____"

                  "________________"
                  "________Gggg____"
                  "________Hhhh____"

                  "________________"
                  "________Iiii____"
                  "________Jjjj____"

                  "________________"
                  "________Kkkk____"
                  "________Llll____"_s, a.dedicatedMemory().map());

    /* Destination & source image */
    Image b{device(), ImageCreateInfoCubeMap{
        ImageUsage::TransferDestination|ImageUsage::TransferSource,
        PixelFormat::RGBA8UI, {4, 4}, 1
    }, MemoryFlag::HostVisible};

    /* Destination buffer */
    Buffer c{device(), BufferCreateInfo{
        BufferUsage::TransferDestination, 3*4*6*4
    }, MemoryFlag::HostVisible};

    cmd.begin()
       .pipelineBarrier(PipelineStage::TopOfPipe, PipelineStage::Transfer, {
            {Accesses{}, Access::TransferWrite,
             ImageLayout::Undefined, ImageLayout::TransferDestination, b}
        })
       .fillBuffer(c, 0x2e2e2e2e)
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Transfer, {
            {Access::TransferWrite, Access::TransferRead, c}
        })
       .copyBufferToImage(CopyBufferToImageInfoCubeMap{a, b, ImageLayout::TransferDestination, {
           {(1*4 + 2)*4, 4, 3, ImageAspect::Color, 0, Range2Di::fromSize({3, 1}, {1, 2})}
        }})
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Transfer, {
            {Access::TransferWrite, Access::TransferRead,
             ImageLayout::TransferDestination, ImageLayout::TransferSource, b}
        })
       .copyImageToBuffer(CopyImageToBufferInfoCubeMap{b, ImageLayout::TransferSource, c, {
           {(1*4 + 2)*4, 4, 3, ImageAspect::Color, 0, Range2Di::fromSize({3, 1}, {1, 2})}
        }})
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Host, {
           {Access::TransferWrite, Access::HostRead, c}
        })
       .end();
    queue().submit({SubmitInfo{}.setCommandBuffers({cmd})}).wait();

    CORRADE_COMPARE(arrayView(c.dedicatedMemory().mapRead()),
        "................"
        "........Aaaa...."
        "........Bbbb...."

        "................"
        "........Cccc...."
        "........Dddd...."

        "................"
        "........Eeee...."
        "........Ffff...."

        "................"
        "........Gggg...."
        "........Hhhh...."

        "................"
        "........Iiii...."
        "........Jjjj...."

        "................"
        "........Kkkk...."
        "........Llll...."_s);
}

void ImageVkTest::cmdCopyBufferImageCubeMapArray() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};
    CommandBuffer cmd = pool.allocate();

    /* Source buffer */
    Buffer a{device(), BufferCreateInfo{
        BufferUsage::TransferSource, 3*4*7*4
    }, MemoryFlag::HostVisible};
    Utility::copy("________________"
                  "________Aaaa____"
                  "________Bbbb____"

                  "________________"
                  "________Cccc____"
                  "________Dddd____"

                  "________________"
                  "________Eeee____"
                  "________Ffff____"

                  "________________"
                  "________Gggg____"
                  "________Hhhh____"

                  "________________"
                  "________Iiii____"
                  "________Jjjj____"

                  "________________"
                  "________Kkkk____"
                  "________Llll____"

                  "________________"
                  "________Mmmm____"
                  "________Nnnn____"_s, a.dedicatedMemory().map());

    /* Destination & source image */
    Image b{device(), ImageCreateInfoCubeMapArray{
        ImageUsage::TransferDestination|ImageUsage::TransferSource,
        PixelFormat::RGBA8UI, {4, 4, 8}, 1
    }, MemoryFlag::HostVisible};

    /* Destination buffer */
    Buffer c{device(), BufferCreateInfo{BufferUsage::TransferDestination, 3*4*7*4}, MemoryFlag::HostVisible};

    cmd.begin()
       .pipelineBarrier(PipelineStage::TopOfPipe, PipelineStage::Transfer, {
            {Accesses{}, Access::TransferWrite,
             ImageLayout::Undefined, ImageLayout::TransferDestination, b}
        })
       .fillBuffer(c, 0x2e2e2e2e)
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Transfer, {
            {Access::TransferWrite, Access::TransferRead, c}
        })
       .copyBufferToImage(CopyBufferToImageInfoCubeMapArray{a, b, ImageLayout::TransferDestination, {
           {(1*4 + 2)*4, 4, 3, ImageAspect::Color, 0, Range3Di::fromSize({3, 1, 1}, {1, 2, 7})}
        }})
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Transfer, {
            {Access::TransferWrite, Access::TransferRead,
             ImageLayout::TransferDestination, ImageLayout::TransferSource, b}
        })
       .copyImageToBuffer(CopyImageToBufferInfoCubeMapArray{b, ImageLayout::TransferSource, c, {
           {(1*4 + 2)*4, 4, 3, ImageAspect::Color, 0, Range3Di::fromSize({3, 1, 1}, {1, 2, 7})}
        }})
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Host, {
           {Access::TransferWrite, Access::HostRead, c}
        })
       .end();
    queue().submit({SubmitInfo{}.setCommandBuffers({cmd})}).wait();

    CORRADE_COMPARE(arrayView(c.dedicatedMemory().mapRead()),
        "................"
        "........Aaaa...."
        "........Bbbb...."

        "................"
        "........Cccc...."
        "........Dddd...."

        "................"
        "........Eeee...."
        "........Ffff...."

        "................"
        "........Gggg...."
        "........Hhhh...."

        "................"
        "........Iiii...."
        "........Jjjj...."

        "................"
        "........Kkkk...."
        "........Llll...."

        "................"
        "........Mmmm...."
        "........Nnnn...."_s);
}

void ImageVkTest::cmdCopyBufferImageDisallowedConversion() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    if(device().isExtensionEnabled<Extensions::KHR::copy_commands2>())
        CORRADE_SKIP("KHR_copy_commands2 enabled on the device, can't test");

    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};
    CommandBuffer cmd = pool.allocate();

    CopyBufferToImageInfo a{{}, {}, {}, {}};
    a->pNext = &a;
    CopyImageToBufferInfo b{{}, {}, {}, {}};
    b->pNext = &b;

    /* The commands shouldn't do anything, so it should be fine to just call
       them without any render pass set up */
    std::ostringstream out;
    Error redirectError{&out};
    cmd.copyBufferToImage(a)
       .copyImageToBuffer(b);
    CORRADE_COMPARE(out.str(),
        "Vk::CommandBuffer::copyBufferToImage(): disallowing extraction of CopyBufferToImageInfo with non-empty pNext to prevent information loss\n"
        "Vk::CommandBuffer::copyImageToBuffer(): disallowing extraction of CopyImageToBufferInfo with non-empty pNext to prevent information loss\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::ImageVkTest)
