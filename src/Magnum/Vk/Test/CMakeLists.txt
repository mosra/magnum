#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020 Vladimír Vondruš <mosra@centrum.cz>
#   Copyright © 2016 Jonathan Hale <squareys@googlemail.com>
#
#   Permission is hereby granted, free of charge, to any person obtaining a
#   copy of this software and associated documentation files (the "Software"),
#   to deal in the Software without restriction, including without limitation
#   the rights to use, copy, modify, merge, publish, distribute, sublicense,
#   and/or sell copies of the Software, and to permit persons to whom the
#   Software is furnished to do so, subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included
#   in all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#   DEALINGS IN THE SOFTWARE.
#

corrade_add_test(VkDeviceTest DeviceTest.cpp LIBRARIES MagnumVk)
corrade_add_test(VkDevicePropertiesTest DevicePropertiesTest.cpp LIBRARIES MagnumVk)
corrade_add_test(VkEnumsTest EnumsTest.cpp LIBRARIES MagnumVkTestLib)
corrade_add_test(VkExtensionsTest ExtensionsTest.cpp LIBRARIES MagnumVk)
corrade_add_test(VkExtensionPropertiesTest ExtensionPropertiesTest.cpp LIBRARIES MagnumVk)
corrade_add_test(VkHandleTest HandleTest.cpp LIBRARIES MagnumVk)
corrade_add_test(VkInstanceTest InstanceTest.cpp LIBRARIES MagnumVk)
corrade_add_test(VkIntegrationTest IntegrationTest.cpp LIBRARIES MagnumVk)
corrade_add_test(VkLayerPropertiesTest LayerPropertiesTest.cpp LIBRARIES MagnumVk)
corrade_add_test(VkResultTest ResultTest.cpp LIBRARIES MagnumVk)
corrade_add_test(VkVersionTest VersionTest.cpp LIBRARIES MagnumVk)

if(BUILD_VK_TESTS)
    corrade_add_test(VkDeviceVkTest DeviceVkTest.cpp LIBRARIES MagnumVkTestLib)
    corrade_add_test(VkDevicePropertiesVkTest DevicePropertiesVkTest.cpp LIBRARIES MagnumVkTestLib)
    corrade_add_test(VkExtensionPropertiesVkTest ExtensionPropertiesVkTest.cpp LIBRARIES MagnumVkTestLib)
    corrade_add_test(VkLayerPropertiesVkTest LayerPropertiesVkTest.cpp LIBRARIES MagnumVkTestLib)
    corrade_add_test(VkInstanceVkTest InstanceVkTest.cpp LIBRARIES MagnumVk)
    corrade_add_test(VkVersionVkTest VersionVkTest.cpp LIBRARIES MagnumVk)
endif()
