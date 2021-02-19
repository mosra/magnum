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
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Vk/DeviceFeatures.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct DeviceFeaturesTest: TestSuite::Tester {
    explicit DeviceFeaturesTest();

    void mapping();

    void debugFeature();
    void debugFeatures();
};

DeviceFeaturesTest::DeviceFeaturesTest() {
    addTests({&DeviceFeaturesTest::mapping,

              &DeviceFeaturesTest::debugFeature,
              &DeviceFeaturesTest::debugFeatures});
}

void DeviceFeaturesTest::mapping() {
    UnsignedInt firstUnhandled = 0xff;
    UnsignedInt nextHandled = 0;
    for(UnsignedInt i = 0; i <= 0xff; ++i) {
        CORRADE_ITERATION(i);
        const auto feature = DeviceFeature(i);
        /* Each case verifies:
           - that the entries are ordered by number by comparing a function to
             expected result (so insertion here is done in proper place)
           - that there was no gap (unhandled value inside the range)
           - that the Debug output is equivalent to the name */
        #ifdef __GNUC__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic error "-Wswitch"
        #endif
        switch(feature) {
            #define _c(value, field) \
                case DeviceFeature::value: { \
                    std::ostringstream out; \
                    Debug{&out, Debug::Flag::NoNewlineAtTheEnd} << DeviceFeature::value; \
                    CORRADE_COMPARE(out.str(), "Vk::DeviceFeature::" #value); \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xff); \
                    ++nextHandled; \
                    continue; \
                }
            #define _cver(value, field, suffix, version) _c(value, field)
            #define _cext(value, field, suffix, extension) _c(value, field)
            #include "Magnum/Vk/Implementation/deviceFeatureMapping.hpp"
            #undef _c
            #undef _cver
            #undef _cext
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #endif

        /* Not handled by any value, remember -- we might either be at the end
           of the enum range (which is okay) or some value might be unhandled
           here */
        firstUnhandled = i;
    }

    CORRADE_COMPARE(firstUnhandled, 0xff);
}

void DeviceFeaturesTest::debugFeature() {
    std::ostringstream out;
    Debug{&out} << DeviceFeature::FullDrawIndexUnsignedInt << DeviceFeature::VulkanMemoryModel << DeviceFeature(0xab);
    CORRADE_COMPARE(out.str(), "Vk::DeviceFeature::FullDrawIndexUnsignedInt Vk::DeviceFeature::VulkanMemoryModel Vk::DeviceFeature(0xab)\n");
}

void DeviceFeaturesTest::debugFeatures() {
    std::ostringstream out;
    Debug{&out} << (DeviceFeature::FullDrawIndexUnsignedInt|DeviceFeature::VulkanMemoryModel|DeviceFeature(0xab)|DeviceFeature(0xcc)) << DeviceFeatures{};
    CORRADE_COMPARE(out.str(), "Vk::DeviceFeature::FullDrawIndexUnsignedInt|Vk::DeviceFeature::VulkanMemoryModel|Vk::DeviceFeature(0xab)|Vk::DeviceFeature(0xcc) Vk::DeviceFeatures{}\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::DeviceFeaturesTest)
