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

#include <new>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Vk/Device.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct DeviceTest: TestSuite::Tester {
    explicit DeviceTest();

    void createInfoConstructNoInit();
    void createInfoConstructFromVk();

    void constructNoCreate();
    void constructCopy();
};

DeviceTest::DeviceTest() {
    addTests({&DeviceTest::createInfoConstructNoInit,
              &DeviceTest::createInfoConstructFromVk,

              &DeviceTest::constructNoCreate,
              &DeviceTest::constructCopy});
}

void DeviceTest::createInfoConstructNoInit() {
    DeviceCreateInfo info{NoInit};
    info->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    new(&info) DeviceCreateInfo{NoInit};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);

    CORRADE_VERIFY((std::is_nothrow_constructible<DeviceCreateInfo, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, DeviceCreateInfo>::value));
}

void DeviceTest::createInfoConstructFromVk() {
    VkDeviceCreateInfo vkInfo;
    vkInfo.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;

    DeviceCreateInfo info{VkPhysicalDevice{}, vkInfo};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
}

void DeviceTest::constructNoCreate() {
    {
        Device device{NoCreate};
        CORRADE_VERIFY(!device.handle());
        /* Device function pointers should be null */
        CORRADE_VERIFY(!device->CreateBuffer);
    }

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoCreateT, Device>::value));
}

void DeviceTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<Device, const Device&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Device, const Device&>{}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::DeviceTest)
