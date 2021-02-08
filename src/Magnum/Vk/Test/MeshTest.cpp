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
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Mesh.h"
#include "Magnum/Vk/Buffer.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/Mesh.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct MeshTest: TestSuite::Tester {
    explicit MeshTest();

    void mapIndexType();
    void mapIndexTypeInvalid();

    void construct();
    void countsOffsets();

    void addVertexBuffer();
    void addVertexBufferOwned();
    void addVertexBufferNoSuchBinding();

    template<class T> void setIndexBuffer();
    template<class T> void setIndexBufferOwned();

    void indexPropertiesNotIndexed();

    void debugIndexType();
};

MeshTest::MeshTest() {
    addTests({&MeshTest::mapIndexType,
              &MeshTest::mapIndexTypeInvalid,

              &MeshTest::construct,
              &MeshTest::countsOffsets,

              &MeshTest::addVertexBuffer,
              &MeshTest::addVertexBufferOwned,
              &MeshTest::addVertexBufferNoSuchBinding,

              &MeshTest::setIndexBuffer<MeshIndexType>,
              &MeshTest::setIndexBuffer<Magnum::MeshIndexType>,
              &MeshTest::setIndexBufferOwned<MeshIndexType>,
              &MeshTest::setIndexBufferOwned<Magnum::MeshIndexType>,

              &MeshTest::indexPropertiesNotIndexed,

              &MeshTest::debugIndexType});
}

template<class> struct IndexTypeTraits;
template<> struct IndexTypeTraits<MeshIndexType> {
    static const char* name() { return "MeshIndexType"; }
};
template<> struct IndexTypeTraits<Magnum::MeshIndexType> {
    static const char* name() { return "Magnum::MeshIndexType"; }
};

void MeshTest::mapIndexType() {
    CORRADE_COMPARE(meshIndexType(Magnum::MeshIndexType::UnsignedByte), MeshIndexType::UnsignedByte);
    CORRADE_COMPARE(meshIndexType(Magnum::MeshIndexType::UnsignedShort), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE(meshIndexType(Magnum::MeshIndexType::UnsignedInt), MeshIndexType::UnsignedInt);
}

void MeshTest::mapIndexTypeInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    meshIndexType(Magnum::MeshIndexType(0x0));
    meshIndexType(Magnum::MeshIndexType(0x12));
    CORRADE_COMPARE(out.str(),
        "Vk::meshIndexType(): invalid type MeshIndexType(0x0)\n"
        "Vk::meshIndexType(): invalid type MeshIndexType(0x12)\n");
}

void MeshTest::construct() {
    MeshLayout layout{MeshPrimitive::Triangles};
    layout.vkPipelineVertexInputStateCreateInfo().sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    layout.vkPipelineInputAssemblyStateCreateInfo().sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
    Mesh mesh{layout};
    /* These should be copies of the original layout */
    CORRADE_COMPARE(mesh.layout().vkPipelineVertexInputStateCreateInfo().sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
    CORRADE_COMPARE(mesh.layout().vkPipelineInputAssemblyStateCreateInfo().sType, VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2);
    CORRADE_COMPARE(mesh.count(), 0);
    CORRADE_COMPARE(mesh.vertexOffset(), 0);
    CORRADE_COMPARE(mesh.indexOffset(), 0);
    CORRADE_COMPARE(mesh.instanceCount(), 1);
    CORRADE_COMPARE(mesh.instanceOffset(), 0);
    CORRADE_VERIFY(mesh.vertexBuffers().empty());
    CORRADE_VERIFY(mesh.vertexBufferOffsets().empty());
    CORRADE_VERIFY(mesh.vertexBufferStrides().empty());
    CORRADE_VERIFY(!mesh.isIndexed());
}

void MeshTest::countsOffsets() {
    Mesh mesh{MeshLayout{MeshPrimitive::Triangles}};
    mesh.setCount(15)
        .setVertexOffset(3)
        .setIndexOffset(5)
        .setInstanceCount(7)
        .setInstanceOffset(9);
    CORRADE_COMPARE(mesh.count(), 15);
    CORRADE_COMPARE(mesh.vertexOffset(), 3);
    CORRADE_COMPARE(mesh.indexOffset(), 5);
    CORRADE_COMPARE(mesh.instanceCount(), 7);
    CORRADE_COMPARE(mesh.instanceOffset(), 9);
}

void MeshTest::addVertexBuffer() {
    Mesh mesh{MeshLayout{MeshPrimitive::TriangleFan}
        .addBinding(1, 2)
        .addInstancedBinding(5, 3)
    };
    CORRADE_COMPARE_AS(mesh.vertexBuffers(), Containers::arrayView({
        VkBuffer{}, VkBuffer{}
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(mesh.vertexBufferOffsets(), Containers::arrayView<UnsignedLong>({
        0, 0
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(mesh.vertexBufferStrides(), Containers::arrayView<UnsignedLong>({
        0, 0
    }), TestSuite::Compare::Container);

    mesh.addVertexBuffer(5, reinterpret_cast<VkBuffer>(0xdead), 15);
    CORRADE_COMPARE_AS(mesh.vertexBuffers(), Containers::arrayView({
        VkBuffer{}, reinterpret_cast<VkBuffer>(0xdead)
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(mesh.vertexBufferOffsets(), Containers::arrayView<UnsignedLong>({
        0, 15
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(mesh.vertexBufferStrides(), Containers::arrayView<UnsignedLong>({
        0, 3
    }), TestSuite::Compare::Container);

    mesh.addVertexBuffer(1, reinterpret_cast<VkBuffer>(0xbeef), 37);
    CORRADE_COMPARE_AS(mesh.vertexBuffers(), Containers::arrayView({
        reinterpret_cast<VkBuffer>(0xbeef), reinterpret_cast<VkBuffer>(0xdead)
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(mesh.vertexBufferOffsets(), Containers::arrayView<UnsignedLong>({
        37, 15
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(mesh.vertexBufferStrides(), Containers::arrayView<UnsignedLong>({
        2, 3
    }), TestSuite::Compare::Container);
}

void MeshTest::addVertexBufferOwned() {
    Mesh mesh{MeshLayout{MeshPrimitive::TriangleFan}
        .addBinding(1, 2)
        .addInstancedBinding(5, 3)
    };

    Device device{NoCreate};
    Buffer a = Buffer::wrap(device, reinterpret_cast<VkBuffer>(0xdead));
    Buffer b = Buffer::wrap(device, reinterpret_cast<VkBuffer>(0xbeef));
    mesh.addVertexBuffer(5, std::move(a), 15)
        .addVertexBuffer(1, std::move(b), 37);
    CORRADE_VERIFY(!a.handle());
    CORRADE_VERIFY(!b.handle());

    CORRADE_COMPARE_AS(mesh.vertexBuffers(), Containers::arrayView({
        reinterpret_cast<VkBuffer>(0xbeef), reinterpret_cast<VkBuffer>(0xdead)
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(mesh.vertexBufferOffsets(), Containers::arrayView<UnsignedLong>({
        37, 15
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(mesh.vertexBufferStrides(), Containers::arrayView<UnsignedLong>({
        2, 3
    }), TestSuite::Compare::Container);
}

void MeshTest::addVertexBufferNoSuchBinding() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Mesh noBindings{MeshLayout{MeshPrimitive::Triangles}};
    Mesh differentBindings{MeshLayout{MeshPrimitive::Lines}
        .addBinding(1, 2)
        .addInstancedBinding(5, 3)};

    std::ostringstream out;
    Error redirectError{&out};
    noBindings.addVertexBuffer(2, VkBuffer{}, 0);
    differentBindings.addVertexBuffer(3, Buffer{NoCreate}, 5);
    CORRADE_COMPARE(out.str(),
        "Vk::Mesh::addVertexBuffer(): binding 2 not present among 0 bindings in the layout\n"
        "Vk::Mesh::addVertexBuffer(): binding 3 not present among 2 bindings in the layout\n");
}

template<class T> void MeshTest::setIndexBuffer() {
    setTestCaseTemplateName(IndexTypeTraits<T>::name());

    Mesh mesh{MeshLayout{MeshPrimitive::Triangles}};
    CORRADE_VERIFY(!mesh.isIndexed());

    mesh.setIndexBuffer(reinterpret_cast<VkBuffer>(0xdead), 15, T::UnsignedByte);
    CORRADE_VERIFY(mesh.isIndexed());
    CORRADE_COMPARE(mesh.indexBuffer(), reinterpret_cast<VkBuffer>(0xdead));
    CORRADE_COMPARE(mesh.indexBufferOffset(), 15);
    CORRADE_COMPARE(mesh.indexType(), MeshIndexType::UnsignedByte);
}

template<class T> void MeshTest::setIndexBufferOwned() {
    setTestCaseTemplateName(IndexTypeTraits<T>::name());

    Device device{NoCreate};
    Buffer a = Buffer::wrap(device, reinterpret_cast<VkBuffer>(0xdead));

    Mesh mesh{MeshLayout{MeshPrimitive::Triangles}};
    mesh.setIndexBuffer(std::move(a), 15, T::UnsignedByte);
    CORRADE_VERIFY(!a.handle());
    CORRADE_VERIFY(mesh.isIndexed());
    CORRADE_COMPARE(mesh.indexBuffer(), reinterpret_cast<VkBuffer>(0xdead));
    CORRADE_COMPARE(mesh.indexBufferOffset(), 15);
    CORRADE_COMPARE(mesh.indexType(), MeshIndexType::UnsignedByte);
}

void MeshTest::indexPropertiesNotIndexed() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Mesh mesh{MeshLayout{MeshPrimitive::Triangles}};
    CORRADE_VERIFY(!mesh.isIndexed());

    std::ostringstream out;
    Error redirectError{&out};
    mesh.indexBuffer();
    mesh.indexBufferOffset();
    mesh.indexType();
    CORRADE_COMPARE(out.str(),
        "Vk::Mesh::indexBuffer(): the mesh is not indexed\n"
        "Vk::Mesh::indexBufferOffset(): the mesh is not indexed\n"
        "Vk::Mesh::indexType(): the mesh is not indexed\n");
}

void MeshTest::debugIndexType() {
    std::ostringstream out;
    Debug{&out} << MeshIndexType::UnsignedShort << MeshIndexType(-10007655);
    CORRADE_COMPARE(out.str(), "Vk::MeshIndexType::UnsignedShort Vk::MeshIndexType(-10007655)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::MeshTest)
