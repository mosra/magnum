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

#include "Magnum/Vk/BufferCreateInfo.h"
#include "Magnum/Vk/CommandBuffer.h"
#include "Magnum/Vk/CommandPoolCreateInfo.h"
#include "Magnum/Vk/DeviceProperties.h"
#include "Magnum/Vk/ImageCreateInfo.h"
#include "Magnum/Vk/Pipeline.h"
#include "Magnum/Vk/PixelFormat.h"
#include "Magnum/Vk/VulkanTester.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct PipelineVkTest: VulkanTester {
    explicit PipelineVkTest();

    void pipelineBarrier();
    void pipelineBarrierGeneral();
    void pipelineBarrierBuffer();
    void pipelineBarrierImage();
};

PipelineVkTest::PipelineVkTest() {
    addTests({&PipelineVkTest::pipelineBarrier,
              &PipelineVkTest::pipelineBarrierGeneral,
              &PipelineVkTest::pipelineBarrierBuffer,
              &PipelineVkTest::pipelineBarrierImage});
}

void PipelineVkTest::pipelineBarrier() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};

    Buffer buffer{device(), BufferCreateInfo{
        BufferUsage::TransferDestination|BufferUsage::VertexBuffer, 16
    }, MemoryFlag::DeviceLocal};
    Image image{device(), ImageCreateInfo2D{
        ImageUsage::TransferDestination|ImageUsage::Sampled,
        PixelFormat::RGBA8Unorm, {4, 4}, 1
    }, MemoryFlag::DeviceLocal};

    pool.allocate()
        .begin()
        .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Host|PipelineStage::VertexInput|PipelineStage::FragmentShader, {
            {Access::TransferWrite, Access::HostRead}
        }, {
            {Access::TransferWrite, Access::VertexAttributeRead, buffer}
        }, {
            {Access::TransferWrite, Access::ShaderRead,
             ImageLayout::Preinitialized, ImageLayout::ShaderReadOnly,
             image}
        })
        .end();

    /* Does not do anything visible, so just test that it didn't blow up */
    CORRADE_VERIFY(true);
}

void PipelineVkTest::pipelineBarrierGeneral() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};

    /* A subset of the above, just to test the convenience overloads */

    pool.allocate()
        .begin()
        .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Host, {
            {Access::TransferWrite, Access::HostRead}
        })
        .end();

    /* Does not do anything visible, so just test that it didn't blow up */
    CORRADE_VERIFY(true);
}

void PipelineVkTest::pipelineBarrierBuffer() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};

    Buffer buffer{device(), BufferCreateInfo{
        BufferUsage::TransferDestination|BufferUsage::VertexBuffer, 16
    }, MemoryFlag::DeviceLocal};

    pool.allocate()
        .begin()
        .pipelineBarrier(PipelineStage::Transfer, PipelineStage::VertexInput, {
            {Access::TransferWrite, Access::VertexAttributeRead, buffer}
        })
        .end();

    /* Does not do anything visible, so just test that it didn't blow up */
    CORRADE_VERIFY(true);
}

void PipelineVkTest::pipelineBarrierImage() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};

    Image image{device(), ImageCreateInfo2D{
        ImageUsage::TransferDestination|ImageUsage::Sampled,
        PixelFormat::RGBA8Unorm, {4, 4}, 1
    }, MemoryFlag::DeviceLocal};

    pool.allocate()
        .begin()
        .pipelineBarrier(PipelineStage::Transfer, PipelineStage::FragmentShader, {
            {Access::TransferWrite, Access::ShaderRead,
             ImageLayout::Preinitialized, ImageLayout::ShaderReadOnly,
             image}
        })
        .end();

    /* Does not do anything visible, so just test that it didn't blow up */
    CORRADE_VERIFY(true);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::PipelineVkTest)
