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

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Vk/Implementation/structureHelpers.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct StructureHelpersTest: TestSuite::Tester {
    explicit StructureHelpersTest();

    template<class T> void connect();
};

StructureHelpersTest::StructureHelpersTest() {
    addTests<StructureHelpersTest>({
        &StructureHelpersTest::connect<VkDeviceCreateInfo>,
        &StructureHelpersTest::connect<VkPhysicalDeviceFeatures2>});
}

template<class> struct Type;
template<> struct Type<const void*> {
    static const char* name() { return "const void*"; }
};
template<> struct Type<void*> {
    static const char* name() { return "void*"; }
};

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

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::StructureHelpersTest)
