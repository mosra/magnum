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

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Vk/Implementation/structureHelpers.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct StructureHelpersTest: TestSuite::Tester {
    explicit StructureHelpersTest();

    template<class T> void connectOne();
    template<class T> void connect();
    template<class T> void find();
    template<class T> void disconnectChain();
};

StructureHelpersTest::StructureHelpersTest() {
    addTests<StructureHelpersTest>({
        &StructureHelpersTest::connectOne<VkDeviceCreateInfo>,
        &StructureHelpersTest::connectOne<VkPhysicalDeviceFeatures2>,
        &StructureHelpersTest::connect<VkDeviceCreateInfo>,
        &StructureHelpersTest::connect<VkPhysicalDeviceFeatures2>,
        &StructureHelpersTest::find<VkDeviceCreateInfo>,
        &StructureHelpersTest::find<VkPhysicalDeviceFeatures2>,
        &StructureHelpersTest::disconnectChain<VkDeviceCreateInfo>,
        &StructureHelpersTest::disconnectChain<VkPhysicalDeviceFeatures2>});
}

template<class> struct Type;
template<> struct Type<const void*> {
    static const char* name() { return "const void*"; }
};
template<> struct Type<void*> {
    static const char* name() { return "void*"; }
};

template<class T> void StructureHelpersTest::connectOne() {
    typedef typename std::remove_reference<decltype(T{}.pNext)>::type NextType;
    setTestCaseTemplateName(Type<NextType>::name());

    VkPhysicalDeviceVariablePointersFeatures variableFeatures{};

    T info{};
    info.pNext = &variableFeatures;

    VkPhysicalDeviceMultiviewFeatures multiviewFeatures{};
    Implementation::structureConnectOne(info.pNext, multiviewFeatures, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES);
    CORRADE_COMPARE(info.pNext, &multiviewFeatures);
    CORRADE_COMPARE(multiviewFeatures.sType, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES);
    /* The pre-existing next pointer should be preserved */
    CORRADE_COMPARE(multiviewFeatures.pNext, &variableFeatures);
}

template<class T> void StructureHelpersTest::connect() {
    typedef typename std::remove_reference<decltype(T{}.pNext)>::type NextType;
    setTestCaseTemplateName(Type<NextType>::name());

    VkPhysicalDeviceVariablePointersFeatures variableFeatures{};

    T info{};
    info.pNext = &variableFeatures;

    Containers::Reference<NextType> next = info.pNext;

    VkPhysicalDeviceMultiviewFeatures multiviewFeatures{};
    Implementation::structureConnect(next, multiviewFeatures, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES);
    CORRADE_COMPARE(info.pNext, &multiviewFeatures);
    CORRADE_COMPARE(multiviewFeatures.sType, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES);
    CORRADE_COMPARE(const_cast<const void**>(&*next), const_cast<const void**>(&multiviewFeatures.pNext));
    /* The pre-existing next pointer should be preserved */
    CORRADE_COMPARE(*next, &variableFeatures);

    VkPhysicalDeviceSamplerYcbcrConversionFeatures ycbcrFeatures{};
    Implementation::structureConnect(next, ycbcrFeatures, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES);
    CORRADE_COMPARE(multiviewFeatures.pNext, &ycbcrFeatures);
    CORRADE_COMPARE(ycbcrFeatures.sType, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES);
    CORRADE_COMPARE(const_cast<const void**>(&*next), const_cast<const void**>(&ycbcrFeatures.pNext));
    /* The pre-existing next pointer should be preserved */
    CORRADE_COMPARE(*next, &variableFeatures);
}

template<class T> void StructureHelpersTest::find() {
    typedef typename std::remove_reference<decltype(T{}.pNext)>::type NextType;
    setTestCaseTemplateName(Type<NextType>::name());

    VkPhysicalDeviceVariablePointersFeatures variableFeatures{};

    VkPhysicalDeviceSamplerYcbcrConversionFeatures ycbcrFeatures{};
    ycbcrFeatures.pNext = &variableFeatures;

    VkPhysicalDeviceMultiviewFeatures multiviewFeatures{};
    multiviewFeatures.pNext = &ycbcrFeatures;

    T info{};
    info.pNext = &multiviewFeatures;

    CORRADE_COMPARE(const_cast<const void**>(Implementation::structureFind(info.pNext, variableFeatures)), const_cast<const void**>(&ycbcrFeatures.pNext));
    /* It shouldn't be modifying the input in any way (heh, what the hell did
       I manage to do here) */
    CORRADE_COMPARE(info.pNext, &multiviewFeatures);

    CORRADE_COMPARE(const_cast<const void**>(Implementation::structureFind(info.pNext, ycbcrFeatures)), const_cast<const void**>(&multiviewFeatures.pNext));
    CORRADE_COMPARE(info.pNext, &multiviewFeatures);

    CORRADE_COMPARE(const_cast<const void**>(Implementation::structureFind(info.pNext, multiviewFeatures)), const_cast<const void**>(&info.pNext));
    CORRADE_COMPARE(info.pNext, &multiviewFeatures);

    CORRADE_COMPARE(Implementation::structureFind(info.pNext, info), nullptr);
    CORRADE_COMPARE(info.pNext, &multiviewFeatures);
}

template<class T> void StructureHelpersTest::disconnectChain() {
    typedef typename std::remove_reference<decltype(T{}.pNext)>::type NextType;
    setTestCaseTemplateName(Type<NextType>::name());

    VkPhysicalDeviceFeatures2 a0{}, a1{}, a2{}, a3{}, a4{}, a5{}, a6{}, a7{}, a8{}, a9{};

    VkDeviceGroupSubmitInfo out{};

    int error{};

    T info;
    info.pNext = &a1;
    a0.pNext = &error;
    a1.pNext = &a3;
    a2.pNext = &error;
    a3.pNext = &a4;
    a4.pNext = &a5;
    a5.pNext = &a7;
    a6.pNext = &error;
    a7.pNext = &a8;
    a8.pNext = &out;
    a9.pNext = &error;

    Implementation::structureDisconnectChain(info.pNext, {
        a0, a1, a2, a3, a4, a5, a6, a7, a8, a9
    });
    CORRADE_COMPARE(info.pNext, &out);

    /* Everything else should stay as it was before */
    CORRADE_COMPARE(a0.pNext, &error);
    CORRADE_COMPARE(a1.pNext, &a3);
    CORRADE_COMPARE(a2.pNext, &error);
    CORRADE_COMPARE(a3.pNext, &a4);
    CORRADE_COMPARE(a4.pNext, &a5);
    CORRADE_COMPARE(a5.pNext, &a7);
    CORRADE_COMPARE(a6.pNext, &error);
    CORRADE_COMPARE(a7.pNext, &a8);
    CORRADE_COMPARE(a8.pNext, &out);
    CORRADE_COMPARE(a9.pNext, &error);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::StructureHelpersTest)
