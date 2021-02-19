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
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Mesh.h"
#include "Magnum/VertexFormat.h"
#include "Magnum/Vk/MeshLayout.h"
#include "Magnum/Vk/VertexFormat.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct MeshLayoutTest: TestSuite::Tester {
    explicit MeshLayoutTest();

    void mapMeshPrimitive();
    void mapMeshPrimitiveImplementationSpecific();
    void mapMeshPrimitiveUnsupported();
    void mapMeshPrimitiveInvalid();

    template<class T> void construct();
    void constructNoInit();
    void constructFromVk();
    void constructCopy();
    void constructMove();

    void addBinding();
    void addInstancedBinding();
    void addInstancedBindingDivisor();
    void addBindingWrongOrder();
    template<class T> void addAttribute();
    void addAttributeWrongOrder();

    void rvalue();

    void compare();
    void compareExternalPointers();

    void debugMeshPrimitive();
};

MeshLayoutTest::MeshLayoutTest() {
    addTests({&MeshLayoutTest::mapMeshPrimitive,
              &MeshLayoutTest::mapMeshPrimitiveImplementationSpecific,
              &MeshLayoutTest::mapMeshPrimitiveUnsupported,
              &MeshLayoutTest::mapMeshPrimitiveInvalid,

              &MeshLayoutTest::construct<MeshPrimitive>,
              &MeshLayoutTest::construct<Magnum::MeshPrimitive>,
              &MeshLayoutTest::constructNoInit,
              &MeshLayoutTest::constructFromVk,
              &MeshLayoutTest::constructCopy,
              &MeshLayoutTest::constructMove,

              &MeshLayoutTest::addBinding,
              &MeshLayoutTest::addInstancedBinding,
              &MeshLayoutTest::addInstancedBindingDivisor,
              &MeshLayoutTest::addBindingWrongOrder,
              &MeshLayoutTest::addAttribute<VertexFormat>,
              &MeshLayoutTest::addAttribute<Magnum::VertexFormat>,
              &MeshLayoutTest::addAttributeWrongOrder,

              &MeshLayoutTest::rvalue,

              &MeshLayoutTest::compare,
              &MeshLayoutTest::compareExternalPointers,

              &MeshLayoutTest::debugMeshPrimitive});
}

template<class> struct VertexFormatTraits;
template<> struct VertexFormatTraits<VertexFormat> {
    static const char* name() { return "VertexFormat"; }
};
template<> struct VertexFormatTraits<Magnum::VertexFormat> {
    static const char* name() { return "Magnum::VertexFormat"; }
};

void MeshLayoutTest::mapMeshPrimitive() {
    CORRADE_VERIFY(hasMeshPrimitive(Magnum::MeshPrimitive::Points));
    CORRADE_COMPARE(meshPrimitive(Magnum::MeshPrimitive::Points), MeshPrimitive::Points);

    CORRADE_VERIFY(hasMeshPrimitive(Magnum::MeshPrimitive::Lines));
    CORRADE_COMPARE(meshPrimitive(Magnum::MeshPrimitive::Lines), MeshPrimitive::Lines);

    CORRADE_VERIFY(hasMeshPrimitive(Magnum::MeshPrimitive::LineStrip));
    CORRADE_COMPARE(meshPrimitive(Magnum::MeshPrimitive::LineStrip), MeshPrimitive::LineStrip);

    CORRADE_VERIFY(hasMeshPrimitive(Magnum::MeshPrimitive::Triangles));
    CORRADE_COMPARE(meshPrimitive(Magnum::MeshPrimitive::Triangles), MeshPrimitive::Triangles);

    CORRADE_VERIFY(hasMeshPrimitive(Magnum::MeshPrimitive::TriangleStrip));
    CORRADE_COMPARE(meshPrimitive(Magnum::MeshPrimitive::TriangleStrip), MeshPrimitive::TriangleStrip);

    CORRADE_VERIFY(hasMeshPrimitive(Magnum::MeshPrimitive::TriangleFan));
    CORRADE_COMPARE(meshPrimitive(Magnum::MeshPrimitive::TriangleFan), MeshPrimitive::TriangleFan);

    /* Ensure all generic primitives are handled. This goes through the first
       16 bits, which should be enough. Going through 32 bits takes 8 seconds,
       too much. */
    for(UnsignedInt i = 1; i <= 0xffff; ++i) {
        const auto primitive = Magnum::MeshPrimitive(i);
        #ifdef __GNUC__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic error "-Wswitch"
        #endif
        switch(primitive) {
            #define _c(primitive) \
                case Magnum::MeshPrimitive::primitive: \
                    if(hasMeshPrimitive(Magnum::MeshPrimitive::primitive))  \
                        CORRADE_VERIFY(Int(meshPrimitive(Magnum::MeshPrimitive::primitive)) >= 0); \
                    break;
            #include "Magnum/Implementation/meshPrimitiveMapping.hpp"
            #undef _c
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #endif
    }
}

void MeshLayoutTest::mapMeshPrimitiveImplementationSpecific() {
    CORRADE_VERIFY(hasMeshPrimitive(meshPrimitiveWrap(VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY)));
    CORRADE_COMPARE(meshPrimitive(meshPrimitiveWrap(VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY)),
        MeshPrimitive(VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY));
}

void MeshLayoutTest::mapMeshPrimitiveUnsupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    CORRADE_VERIFY(!hasMeshPrimitive(Magnum::MeshPrimitive::LineLoop));

    std::ostringstream out;
    {
        Error redirectError{&out};
        meshPrimitive(Magnum::MeshPrimitive::LineLoop);
    }
    CORRADE_COMPARE(out.str(),
        "Vk::meshPrimitive(): unsupported primitive MeshPrimitive::LineLoop\n");
}

void MeshLayoutTest::mapMeshPrimitiveInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    hasMeshPrimitive(Magnum::MeshPrimitive{});
    hasMeshPrimitive(Magnum::MeshPrimitive(0x12));
    meshPrimitive(Magnum::MeshPrimitive{});
    meshPrimitive(Magnum::MeshPrimitive(0x12));
    CORRADE_COMPARE(out.str(),
        "Vk::hasMeshPrimitive(): invalid primitive MeshPrimitive(0x0)\n"
        "Vk::hasMeshPrimitive(): invalid primitive MeshPrimitive(0x12)\n"
        "Vk::meshPrimitive(): invalid primitive MeshPrimitive(0x0)\n"
        "Vk::meshPrimitive(): invalid primitive MeshPrimitive(0x12)\n");
}

template<class T> void MeshLayoutTest::construct() {
    setTestCaseTemplateName(std::is_same<T, MeshPrimitive>::value ? "MeshPrimitive" : "Magnum::MeshPrimitive");

    MeshLayout layout{T::TriangleFan};
    CORRADE_COMPARE(layout.vkPipelineInputAssemblyStateCreateInfo().topology, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().vertexBindingDescriptionCount, 0);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().vertexAttributeDescriptionCount, 0);
    CORRADE_VERIFY(!layout.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions);
    CORRADE_VERIFY(!layout.vkPipelineVertexInputStateCreateInfo().pVertexAttributeDescriptions);
}

void MeshLayoutTest::constructNoInit() {
    MeshLayout layout{NoInit};
    layout.vkPipelineVertexInputStateCreateInfo().sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    layout.vkPipelineInputAssemblyStateCreateInfo().sType = VK_STRUCTURE_TYPE_EXTERNAL_FENCE_PROPERTIES;
    new(&layout) MeshLayout{NoInit};
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
    CORRADE_COMPARE(layout.vkPipelineInputAssemblyStateCreateInfo().sType, VK_STRUCTURE_TYPE_EXTERNAL_FENCE_PROPERTIES);

    CORRADE_VERIFY((std::is_nothrow_constructible<MeshLayout, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, MeshLayout>::value));
}

void MeshLayoutTest::constructFromVk() {
    VkPipelineVertexInputStateCreateInfo vertexInfo;
    vertexInfo.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    VkPipelineInputAssemblyStateCreateInfo assemblyInfo;
    assemblyInfo.sType = VK_STRUCTURE_TYPE_EXTERNAL_FENCE_PROPERTIES;

    MeshLayout layout{vertexInfo, assemblyInfo};
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
    CORRADE_COMPARE(layout.vkPipelineInputAssemblyStateCreateInfo().sType, VK_STRUCTURE_TYPE_EXTERNAL_FENCE_PROPERTIES);
}

void MeshLayoutTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<MeshLayout>{});
    CORRADE_VERIFY(!std::is_copy_assignable<MeshLayout>{});
}

void MeshLayoutTest::constructMove() {
    MeshLayout a{MeshPrimitive::Patches};
    a.addInstancedBinding(3, 5, 555)
        .addAttribute(15, 23, VertexFormat::UnsignedShort, 11);

    MeshLayout b = std::move(a);
    CORRADE_VERIFY(!a.vkPipelineVertexInputStateCreateInfo().pNext);
    CORRADE_COMPARE(a.vkPipelineVertexInputStateCreateInfo().vertexBindingDescriptionCount, 0);
    CORRADE_VERIFY(!a.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions);
    CORRADE_COMPARE(a.vkPipelineVertexInputStateCreateInfo().vertexAttributeDescriptionCount, 0);
    CORRADE_VERIFY(!a.vkPipelineVertexInputStateCreateInfo().pVertexAttributeDescriptions);
    CORRADE_VERIFY(b.vkPipelineVertexInputStateCreateInfo().pNext);
    CORRADE_COMPARE(static_cast<const VkPipelineVertexInputDivisorStateCreateInfoEXT*>(b.vkPipelineVertexInputStateCreateInfo().pNext)->sType, VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_DIVISOR_STATE_CREATE_INFO_EXT);
    CORRADE_COMPARE(b.vkPipelineVertexInputStateCreateInfo().vertexBindingDescriptionCount, 1);
    CORRADE_VERIFY(b.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions);
    CORRADE_COMPARE(b.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions[0].stride, 5);
    CORRADE_COMPARE(b.vkPipelineVertexInputStateCreateInfo().vertexAttributeDescriptionCount, 1);
    CORRADE_VERIFY(b.vkPipelineVertexInputStateCreateInfo().pVertexAttributeDescriptions);
    CORRADE_COMPARE(b.vkPipelineVertexInputStateCreateInfo().pVertexAttributeDescriptions[0].format, VK_FORMAT_R16_UINT);

    MeshLayout c{{}, {}};
    c = std::move(b);
    CORRADE_VERIFY(!b.vkPipelineVertexInputStateCreateInfo().pNext);
    CORRADE_COMPARE(b.vkPipelineVertexInputStateCreateInfo().vertexBindingDescriptionCount, 0);
    CORRADE_VERIFY(!b.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions);
    CORRADE_COMPARE(b.vkPipelineVertexInputStateCreateInfo().vertexAttributeDescriptionCount, 0);
    CORRADE_VERIFY(!b.vkPipelineVertexInputStateCreateInfo().pVertexAttributeDescriptions);
    CORRADE_VERIFY(c.vkPipelineVertexInputStateCreateInfo().pNext);
    CORRADE_COMPARE(static_cast<const VkPipelineVertexInputDivisorStateCreateInfoEXT*>(c.vkPipelineVertexInputStateCreateInfo().pNext)->sType, VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_DIVISOR_STATE_CREATE_INFO_EXT);
    CORRADE_COMPARE(c.vkPipelineVertexInputStateCreateInfo().vertexBindingDescriptionCount, 1);
    CORRADE_VERIFY(c.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions);
    CORRADE_COMPARE(c.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions[0].stride, 5);
    CORRADE_COMPARE(c.vkPipelineVertexInputStateCreateInfo().vertexAttributeDescriptionCount, 1);
    CORRADE_VERIFY(c.vkPipelineVertexInputStateCreateInfo().pVertexAttributeDescriptions);
    CORRADE_COMPARE(c.vkPipelineVertexInputStateCreateInfo().pVertexAttributeDescriptions[0].format, VK_FORMAT_R16_UINT);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<MeshLayout>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<MeshLayout>::value);
}

void MeshLayoutTest::addBinding() {
    MeshLayout layout{MeshPrimitive::Triangles};
    layout.addBinding(35, 2)
          .addBinding(36, 17);

    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().vertexBindingDescriptionCount, 2);
    CORRADE_VERIFY(layout.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions[0].binding, 35);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions[0].stride, 2);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions[0].inputRate, VK_VERTEX_INPUT_RATE_VERTEX);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions[1].binding, 36);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions[1].stride, 17);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions[1].inputRate, VK_VERTEX_INPUT_RATE_VERTEX);
}

void MeshLayoutTest::addInstancedBinding() {
    MeshLayout layout{MeshPrimitive::Triangles};
    layout.addInstancedBinding(35, 17)
          .addBinding(36, 2);

    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().vertexBindingDescriptionCount, 2);
    CORRADE_VERIFY(layout.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions[0].binding, 35);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions[0].stride, 17);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions[0].inputRate, VK_VERTEX_INPUT_RATE_INSTANCE);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions[1].binding, 36);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions[1].stride, 2);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions[1].inputRate, VK_VERTEX_INPUT_RATE_VERTEX);
}

void MeshLayoutTest::addInstancedBindingDivisor() {
    MeshLayout layout{MeshPrimitive::Triangles};

    /* Set the pNext pointer to something to verify it's preserved */
    VkPhysicalDeviceVariablePointersFeatures variableFeatures{};
    layout.vkPipelineVertexInputStateCreateInfo().pNext = &variableFeatures;

    layout.addBinding(35, 2)
          .addInstancedBinding(36, 17, 555)
          .addInstancedBinding(37, 22, 0);

    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().vertexBindingDescriptionCount, 3);
    CORRADE_VERIFY(layout.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions[0].binding, 35);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions[0].stride, 2);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions[0].inputRate, VK_VERTEX_INPUT_RATE_VERTEX);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions[1].binding, 36);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions[1].stride, 17);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions[1].inputRate, VK_VERTEX_INPUT_RATE_INSTANCE);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions[2].binding, 37);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions[2].stride, 22);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions[2].inputRate, VK_VERTEX_INPUT_RATE_INSTANCE);

    /* Verify the extra structure is connected properly */
    CORRADE_VERIFY(layout.vkPipelineVertexInputStateCreateInfo().pNext);
    auto& vertexDivisorInfo = *static_cast<const VkPipelineVertexInputDivisorStateCreateInfoEXT*>(layout.vkPipelineVertexInputStateCreateInfo().pNext);
    CORRADE_VERIFY(&vertexDivisorInfo != static_cast<const void*>(&variableFeatures));
    CORRADE_COMPARE(vertexDivisorInfo.sType, VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_DIVISOR_STATE_CREATE_INFO_EXT);
    /* The original chain should be preserved */
    CORRADE_COMPARE(vertexDivisorInfo.pNext, &variableFeatures);

    CORRADE_COMPARE(vertexDivisorInfo.vertexBindingDivisorCount, 2);
    CORRADE_VERIFY(vertexDivisorInfo.pVertexBindingDivisors);
    CORRADE_COMPARE(vertexDivisorInfo.pVertexBindingDivisors[0].binding, 36);
    CORRADE_COMPARE(vertexDivisorInfo.pVertexBindingDivisors[0].divisor, 555);
    CORRADE_COMPARE(vertexDivisorInfo.pVertexBindingDivisors[1].binding, 37);
    CORRADE_COMPARE(vertexDivisorInfo.pVertexBindingDivisors[1].divisor, 0);
}

void MeshLayoutTest::addBindingWrongOrder() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    MeshLayout layout{MeshPrimitive::Triangles};
    layout.addBinding(15, 23);

    std::ostringstream out;
    Error redirectError{&out};
    layout.addBinding(15, 27)
        .addInstancedBinding(15, 27);
    CORRADE_COMPARE(out.str(),
        "Vk::MeshLayout::addBinding(): binding 15 can't be ordered after 15\n"
        "Vk::MeshLayout::addInstancedBinding(): binding 15 can't be ordered after 15\n");
}

template<class T> void MeshLayoutTest::addAttribute() {
    setTestCaseTemplateName(VertexFormatTraits<T>::name());

    MeshLayout layout{MeshPrimitive::Triangles};
    layout.addAttribute(1, 35, T::Vector2ui, 17)
        .addAttribute(2, 36, T::Double, 22);

    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().vertexAttributeDescriptionCount, 2);
    CORRADE_VERIFY(layout.vkPipelineVertexInputStateCreateInfo().pVertexAttributeDescriptions);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().pVertexAttributeDescriptions[0].location, 1);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().pVertexAttributeDescriptions[0].binding, 35);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().pVertexAttributeDescriptions[0].format, VK_FORMAT_R32G32_UINT);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().pVertexAttributeDescriptions[0].offset, 17);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().pVertexAttributeDescriptions[1].location, 2);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().pVertexAttributeDescriptions[1].binding, 36);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().pVertexAttributeDescriptions[1].format, VK_FORMAT_R64_SFLOAT);
    CORRADE_COMPARE(layout.vkPipelineVertexInputStateCreateInfo().pVertexAttributeDescriptions[1].offset, 22);
}

void MeshLayoutTest::addAttributeWrongOrder() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    MeshLayout layout{MeshPrimitive::Triangles};
    layout.addAttribute(5, 17, VertexFormat{}, 0);

    std::ostringstream out;
    Error redirectError{&out};
    layout.addAttribute(5, 25, VertexFormat{}, 1);
    CORRADE_COMPARE(out.str(),
        "Vk::MeshLayout::addAttribute(): location 5 can't be ordered after 5\n");
}

void MeshLayoutTest::rvalue() {
    MeshLayout&& layout = MeshLayout{MeshPrimitive::TriangleFan}
        .addBinding(0, 37)
        .addInstancedBinding(1, 26)
        .addAttribute(0, 0, VertexFormat{}, 0)
        .addAttribute(1, 1, Magnum::VertexFormat::Vector2, 0);

    /* Just to test something, main point is that the above compiles, links and
       returns a &&. Can't test anything related to the contents because the
       destructor gets called at the end of the expression. */
    CORRADE_VERIFY(&layout);
}

void MeshLayoutTest::compare() {
    MeshLayout emptyTriangles1{MeshPrimitive::Triangles};
    MeshLayout emptyTriangles2{MeshPrimitive::Triangles};
    CORRADE_VERIFY(emptyTriangles1 == emptyTriangles1);
    CORRADE_VERIFY(emptyTriangles1 == emptyTriangles2);
    CORRADE_VERIFY(emptyTriangles2 == emptyTriangles1);
    CORRADE_VERIFY(!(emptyTriangles1 != emptyTriangles2));

    MeshLayout emptyLines{MeshPrimitive::Lines};
    CORRADE_VERIFY(emptyLines != emptyTriangles1);
    CORRADE_VERIFY(emptyTriangles1 != emptyLines);

    MeshLayout basic1{MeshPrimitive::Triangles};
    basic1.addBinding(7, 25)
        .addBinding(8, 3)
        .addAttribute(0, 3, VertexFormat::Vector2h, 26)
        .addAttribute(1, 7, VertexFormat::Vector2b, 27);
    MeshLayout basic2{MeshPrimitive::Triangles};
    basic2.addBinding(7, 25)
        .addBinding(8, 3)
        .addAttribute(0, 3, VertexFormat::Vector2h, 26)
        .addAttribute(1, 7, VertexFormat::Vector2b, 27);
    CORRADE_VERIFY(basic1 == basic1);
    CORRADE_VERIFY(basic1 == basic2);
    CORRADE_VERIFY(basic2 == basic1);

    MeshLayout different1{MeshPrimitive::Triangles};
    different1.addBinding(7, 25)
        .addBinding(8, 3)
        .addAttribute(0, 3, VertexFormat::Vector2h, 26)
        .addAttribute(1, 7, VertexFormat::Vector2h, 27); /* different format */
    CORRADE_VERIFY(basic1 != different1);

    MeshLayout different2{MeshPrimitive::Triangles};
    different2.addBinding(7, 25)
        .addBinding(8, 4) /* different stride */
        .addAttribute(0, 3, VertexFormat::Vector2h, 26)
        .addAttribute(1, 7, VertexFormat::Vector2b, 27);
    CORRADE_VERIFY(basic1 != different2);

    MeshLayout larger1{MeshPrimitive::Triangles};
    larger1.addBinding(7, 25)
        .addBinding(8, 3)
        .addBinding(9, 27) /* new entry */
        .addAttribute(0, 3, VertexFormat::Vector2h, 26)
        .addAttribute(1, 7, VertexFormat::Vector2b, 27);
    CORRADE_VERIFY(basic1 != larger1);

    /* It should take this value into account, not the internal array size */
    larger1.vkPipelineVertexInputStateCreateInfo().vertexBindingDescriptionCount = 2;
    CORRADE_VERIFY(basic1 == larger1);

    MeshLayout larger2{MeshPrimitive::Triangles};
    larger2.addBinding(7, 25)
        .addBinding(8, 3)
        .addAttribute(0, 3, VertexFormat::Vector2h, 26)
        .addAttribute(1, 7, VertexFormat::Vector2b, 27)
        .addAttribute(2, 15, VertexFormat::Vector2, 3); /* new entry */
    CORRADE_VERIFY(basic1 != larger2);

    /* It should take this value into account, not the internal array size */
    larger2.vkPipelineVertexInputStateCreateInfo().vertexAttributeDescriptionCount = 2;
    CORRADE_VERIFY(basic1 == larger2);

    MeshLayout instanced1{MeshPrimitive::Triangles};
    instanced1.addInstancedBinding(15, 35)
        .addBinding(17, 25);
    MeshLayout instanced2{MeshPrimitive::Triangles};
    instanced2.addInstancedBinding(15, 35)
        .addBinding(17, 25);
    CORRADE_VERIFY(instanced1 == instanced1);
    CORRADE_VERIFY(instanced1 == instanced2);
    CORRADE_VERIFY(instanced2 == instanced1);

    MeshLayout nonInstanced{MeshPrimitive::Triangles};
    nonInstanced.addBinding(15, 35) /* not instanced, but same otherwise */
        .addBinding(17, 25);
    CORRADE_VERIFY(instanced1 != nonInstanced);

    MeshLayout divisor1{MeshPrimitive::Triangles};
    divisor1.addInstancedBinding(15, 35)
        .addInstancedBinding(16, 8, 75)
        .addBinding(17, 25);
    MeshLayout divisor2{MeshPrimitive::Triangles};
    divisor2.addInstancedBinding(15, 35)
        .addInstancedBinding(16, 8, 75)
        .addBinding(17, 25);
    CORRADE_VERIFY(divisor1 == divisor1);
    CORRADE_VERIFY(divisor1 == divisor2);
    CORRADE_VERIFY(divisor2 == divisor1);

    MeshLayout larger3{MeshPrimitive::Triangles};
    larger3.addInstancedBinding(15, 35)
        .addInstancedBinding(16, 8, 75)
        .addBinding(17, 25)
        .addInstancedBinding(18, 2, 11); /* new entry */
    CORRADE_VERIFY(divisor1 != larger3);

    /* It should take this value into account, not the internal array size */
    CORRADE_VERIFY(larger3.vkPipelineVertexInputStateCreateInfo().pNext);
    static_cast<VkPipelineVertexInputDivisorStateCreateInfoEXT*>(const_cast<void*>(larger3.vkPipelineVertexInputStateCreateInfo().pNext))->vertexBindingDivisorCount = 1;
    larger3.vkPipelineVertexInputStateCreateInfo().vertexBindingDescriptionCount = 3;
    CORRADE_VERIFY(divisor1 == larger3);

    MeshLayout divisor3{MeshPrimitive::Triangles};
    divisor3.addInstancedBinding(15, 35, 75) /* divisor here instead of 2nd */
        .addInstancedBinding(16, 8)
        .addBinding(17, 25);
    CORRADE_VERIFY(divisor1 != divisor3);
}

void MeshLayoutTest::compareExternalPointers() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    /* Disallowed pNext inside one struct */
    {
        MeshLayout empty{MeshPrimitive::Lines};
        MeshLayout layout{MeshPrimitive::Lines};
        layout.vkPipelineVertexInputStateCreateInfo().pNext = &layout;

        /* Test both comparison directions to verify the check is done for
           both */
        std::ostringstream out;
        Error redirectError{&out};
        layout.operator==(empty); /* to avoid unused expression warnings */
        empty.operator==(layout);
        CORRADE_COMPARE(out.str(),
            "Vk::MeshLayout: can't compare structures with external pointers\n"
            "Vk::MeshLayout: can't compare structures with external pointers\n");

    /* Disallowed pNext inside the other */
    } {
        MeshLayout layout{MeshPrimitive::Lines};
        layout.vkPipelineInputAssemblyStateCreateInfo().pNext = &layout;

        std::ostringstream out;
        Error redirectError{&out};
        layout.operator==(layout); /* to avoid unused expression warnings */
        CORRADE_COMPARE(out.str(), "Vk::MeshLayout: can't compare structures with external pointers\n");

    /* Disallowed pNext inside the divisor struct */
    } {
        MeshLayout layout{MeshPrimitive::Lines};
        layout.addInstancedBinding(3, 5, 7);
        /* At this point it should still work */
        CORRADE_VERIFY(layout == layout);
        CORRADE_VERIFY(layout.vkPipelineVertexInputStateCreateInfo().pNext);
        static_cast<VkPipelineVertexInputDivisorStateCreateInfoEXT*>(const_cast<void*>(layout.vkPipelineVertexInputStateCreateInfo().pNext))->pNext = &layout;

        std::ostringstream out;
        Error redirectError{&out};
        layout.operator==(layout); /* to avoid unused expression warnings */
        CORRADE_COMPARE(out.str(), "Vk::MeshLayout: can't compare structures with external pointers\n");

    /* External vertex bindings */
    } {
        VkVertexInputBindingDescription bindingData;
        MeshLayout layout{MeshPrimitive::Lines};
        layout.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions = &bindingData;

        std::ostringstream out;
        Error redirectError{&out};
        layout.operator==(layout); /* to avoid unused expression warnings */
        CORRADE_COMPARE(out.str(), "Vk::MeshLayout: can't compare structures with external pointers\n");

    /* Null vertex bindings but non-zero size */
    } {
        MeshLayout layout{MeshPrimitive::Lines};
        layout.vkPipelineVertexInputStateCreateInfo().vertexAttributeDescriptionCount = 3;

        std::ostringstream out;
        Error redirectError{&out};
        layout.operator==(layout); /* to avoid unused expression warnings */
        CORRADE_COMPARE(out.str(), "Vk::MeshLayout: can't compare structures with external pointers\n");

    /* External vertex divisors */
    } {
        VkVertexInputBindingDivisorDescriptionEXT bindingDivisorData;
        MeshLayout layout{MeshPrimitive::Lines};
        layout.addInstancedBinding(3, 5, 7);
        /* At this point it should still work */
        CORRADE_VERIFY(layout == layout);
        CORRADE_VERIFY(layout.vkPipelineVertexInputStateCreateInfo().pNext);
        static_cast<VkPipelineVertexInputDivisorStateCreateInfoEXT*>(const_cast<void*>(layout.vkPipelineVertexInputStateCreateInfo().pNext))->pVertexBindingDivisors = &bindingDivisorData;

        std::ostringstream out;
        Error redirectError{&out};
        layout.operator==(layout); /* to avoid unused expression warnings */
        CORRADE_COMPARE(out.str(), "Vk::MeshLayout: can't compare structures with external pointers\n");

    /* Null vertex divisors but non-zero size */
    } {
        MeshLayout layout{MeshPrimitive::Lines};
        layout.addInstancedBinding(3, 5, 7);
        /* At this point it should still work */
        CORRADE_VERIFY(layout == layout);
        CORRADE_VERIFY(layout.vkPipelineVertexInputStateCreateInfo().pNext);
        static_cast<VkPipelineVertexInputDivisorStateCreateInfoEXT*>(const_cast<void*>(layout.vkPipelineVertexInputStateCreateInfo().pNext))->pVertexBindingDivisors = nullptr;

        std::ostringstream out;
        Error redirectError{&out};
        layout.operator==(layout); /* to avoid unused expression warnings */
        CORRADE_COMPARE(out.str(), "Vk::MeshLayout: can't compare structures with external pointers\n");

    /* External attributes */
    } {
        VkVertexInputAttributeDescription attributeData;
        MeshLayout layout{MeshPrimitive::Lines};
        layout.vkPipelineVertexInputStateCreateInfo().pVertexAttributeDescriptions = &attributeData;

        std::ostringstream out;
        Error redirectError{&out};
        layout.operator==(layout); /* to avoid unused warnings */
        CORRADE_COMPARE(out.str(), "Vk::MeshLayout: can't compare structures with external pointers\n");

    /* Null attributes but non-zero size */
    } {
        MeshLayout layout{MeshPrimitive::Lines};
        layout.vkPipelineVertexInputStateCreateInfo().vertexAttributeDescriptionCount = 3;

        std::ostringstream out;
        Error redirectError{&out};
        layout.operator==(layout); /* to avoid unused warnings */
        CORRADE_COMPARE(out.str(), "Vk::MeshLayout: can't compare structures with external pointers\n");
    }
}

void MeshLayoutTest::debugMeshPrimitive() {
    std::ostringstream out;
    Debug{&out} << MeshPrimitive::TriangleFan << MeshPrimitive(-10007655);
    CORRADE_COMPARE(out.str(), "Vk::MeshPrimitive::TriangleFan Vk::MeshPrimitive(-10007655)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::MeshLayoutTest)
