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
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/Image.h"
#include "Magnum/Vk/Pipeline.h"
#include "Magnum/Vk/PixelFormat.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct PipelineTest: TestSuite::Tester {
    explicit PipelineTest();

    void memoryBarrierConstruct();
    void memoryBarrierConstructNoInit();
    void memoryBarrierConstructFromVk();

    void bufferMemoryBarrierConstruct();
    void bufferMemoryBarrierConstructNoInit();
    void bufferMemoryBarrierConstructFromVk();

    void imageMemoryBarrierConstruct();
    void imageMemoryBarrierConstructImplicitAspect();
    void imageMemoryBarrierConstructNoInit();
    void imageMemoryBarrierConstructFromVk();
};

PipelineTest::PipelineTest() {
    addTests({&PipelineTest::memoryBarrierConstruct,
              &PipelineTest::memoryBarrierConstructNoInit,
              &PipelineTest::memoryBarrierConstructFromVk,

              &PipelineTest::bufferMemoryBarrierConstruct,
              &PipelineTest::bufferMemoryBarrierConstructNoInit,
              &PipelineTest::bufferMemoryBarrierConstructFromVk,

              &PipelineTest::imageMemoryBarrierConstruct,
              &PipelineTest::imageMemoryBarrierConstructImplicitAspect,
              &PipelineTest::imageMemoryBarrierConstructNoInit,
              &PipelineTest::imageMemoryBarrierConstructFromVk});
}

void PipelineTest::memoryBarrierConstruct() {
    MemoryBarrier barrier{Access::ColorAttachmentWrite|Access::DepthStencilAttachmentWrite, Access::TransferRead};
    CORRADE_COMPARE(barrier->srcAccessMask, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT|VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
    CORRADE_COMPARE(barrier->dstAccessMask, VK_ACCESS_TRANSFER_READ_BIT);
}

void PipelineTest::memoryBarrierConstructNoInit() {
    MemoryBarrier barrier{NoInit};
    barrier->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    new(&barrier) MemoryBarrier{NoInit};
    CORRADE_COMPARE(barrier->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);

    CORRADE_VERIFY((std::is_nothrow_constructible<MemoryBarrier, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, MemoryBarrier>::value));
}

void PipelineTest::memoryBarrierConstructFromVk() {
    VkMemoryBarrier vkBarrier;
    vkBarrier.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;

    MemoryBarrier barrier{vkBarrier};
    CORRADE_COMPARE(barrier->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
}

void PipelineTest::bufferMemoryBarrierConstruct() {
    BufferMemoryBarrier barrier{Access::ColorAttachmentWrite|Access::DepthStencilAttachmentWrite, Access::TransferRead, reinterpret_cast<VkBuffer>(0xdead), 3, 5};
    CORRADE_COMPARE(barrier->srcAccessMask, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT|VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
    CORRADE_COMPARE(barrier->dstAccessMask, VK_ACCESS_TRANSFER_READ_BIT);
    CORRADE_COMPARE(barrier->buffer, reinterpret_cast<VkBuffer>(0xdead));
    CORRADE_COMPARE(barrier->offset, 3);
    CORRADE_COMPARE(barrier->size, 5);
}

void PipelineTest::bufferMemoryBarrierConstructNoInit() {
    BufferMemoryBarrier barrier{NoInit};
    barrier->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    new(&barrier) BufferMemoryBarrier{NoInit};
    CORRADE_COMPARE(barrier->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);

    CORRADE_VERIFY((std::is_nothrow_constructible<BufferMemoryBarrier, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, BufferMemoryBarrier>::value));
}

void PipelineTest::bufferMemoryBarrierConstructFromVk() {
    VkBufferMemoryBarrier vkBarrier;
    vkBarrier.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;

    BufferMemoryBarrier barrier{vkBarrier};
    CORRADE_COMPARE(barrier->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
}

void PipelineTest::imageMemoryBarrierConstruct() {
    ImageMemoryBarrier barrier{Access::ColorAttachmentWrite|Access::DepthStencilAttachmentWrite, Access::TransferRead, ImageLayout::Preinitialized, ImageLayout::TransferSource, reinterpret_cast<VkImage>(0xdead), ImageAspect::Color|ImageAspect::Depth, 3, 5, 7, 9};
    CORRADE_COMPARE(barrier->srcAccessMask, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT|VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
    CORRADE_COMPARE(barrier->dstAccessMask, VK_ACCESS_TRANSFER_READ_BIT);
    CORRADE_COMPARE(barrier->oldLayout, VK_IMAGE_LAYOUT_PREINITIALIZED);
    CORRADE_COMPARE(barrier->newLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    CORRADE_COMPARE(barrier->image, reinterpret_cast<VkImage>(0xdead));
    CORRADE_COMPARE(barrier->subresourceRange.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT|VK_IMAGE_ASPECT_DEPTH_BIT);
    CORRADE_COMPARE(barrier->subresourceRange.baseMipLevel, 7);
    CORRADE_COMPARE(barrier->subresourceRange.levelCount, 9);
    CORRADE_COMPARE(barrier->subresourceRange.baseArrayLayer, 3);
    CORRADE_COMPARE(barrier->subresourceRange.layerCount, 5);
}

void PipelineTest::imageMemoryBarrierConstructImplicitAspect() {
    Device device{NoCreate};
    Image image = Image::wrap(device, reinterpret_cast<VkImage>(0xdead), PixelFormat::Depth24UnormStencil8UI);

    ImageMemoryBarrier barrier{
        Access::ColorAttachmentRead, Access::TransferWrite,
        ImageLayout::ColorAttachment, ImageLayout::TransferDestination,
        image, 3, 5, 7, 9};
    CORRADE_COMPARE(barrier->srcAccessMask, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT);
    CORRADE_COMPARE(barrier->dstAccessMask, VK_ACCESS_TRANSFER_WRITE_BIT);
    CORRADE_COMPARE(barrier->oldLayout, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    CORRADE_COMPARE(barrier->newLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    CORRADE_COMPARE(barrier->image, reinterpret_cast<VkImage>(0xdead));
    CORRADE_COMPARE(barrier->subresourceRange.aspectMask, VK_IMAGE_ASPECT_DEPTH_BIT|VK_IMAGE_ASPECT_STENCIL_BIT);
    CORRADE_COMPARE(barrier->subresourceRange.baseMipLevel, 7);
    CORRADE_COMPARE(barrier->subresourceRange.levelCount, 9);
    CORRADE_COMPARE(barrier->subresourceRange.baseArrayLayer, 3);
    CORRADE_COMPARE(barrier->subresourceRange.layerCount, 5);
}

void PipelineTest::imageMemoryBarrierConstructNoInit() {
    ImageMemoryBarrier barrier{NoInit};
    barrier->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    new(&barrier) ImageMemoryBarrier{NoInit};
    CORRADE_COMPARE(barrier->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);

    CORRADE_VERIFY((std::is_nothrow_constructible<ImageMemoryBarrier, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, ImageMemoryBarrier>::value));
}

void PipelineTest::imageMemoryBarrierConstructFromVk() {
    VkImageMemoryBarrier vkBarrier;
    vkBarrier.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;

    ImageMemoryBarrier barrier{vkBarrier};
    CORRADE_COMPARE(barrier->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::PipelineTest)
