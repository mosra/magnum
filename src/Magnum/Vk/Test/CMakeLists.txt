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

corrade_add_test(VkBufferTest BufferTest.cpp LIBRARIES MagnumVkTestLib)
corrade_add_test(VkCommandBufferTest CommandBufferTest.cpp LIBRARIES MagnumVk)
corrade_add_test(VkCommandPoolTest CommandPoolTest.cpp LIBRARIES MagnumVk)
corrade_add_test(VkDeviceTest DeviceTest.cpp LIBRARIES MagnumVk)
corrade_add_test(VkDevicePropertiesTest DevicePropertiesTest.cpp LIBRARIES MagnumVk)
corrade_add_test(VkEnumsTest EnumsTest.cpp LIBRARIES MagnumVkTestLib)
corrade_add_test(VkExtensionsTest ExtensionsTest.cpp LIBRARIES MagnumVk)
corrade_add_test(VkExtensionPropertiesTest ExtensionPropertiesTest.cpp LIBRARIES MagnumVk)
corrade_add_test(VkHandleTest HandleTest.cpp LIBRARIES MagnumVk)
corrade_add_test(VkImageTest ImageTest.cpp LIBRARIES MagnumVkTestLib)
corrade_add_test(VkInstanceTest InstanceTest.cpp LIBRARIES MagnumVk)
corrade_add_test(VkIntegrationTest IntegrationTest.cpp LIBRARIES MagnumVk)
corrade_add_test(VkLayerPropertiesTest LayerPropertiesTest.cpp LIBRARIES MagnumVk)
corrade_add_test(VkMemoryTest MemoryTest.cpp LIBRARIES MagnumVkTestLib)
corrade_add_test(VkResultTest ResultTest.cpp LIBRARIES MagnumVk)
corrade_add_test(VkShaderTest ShaderTest.cpp LIBRARIES MagnumVk)

add_library(VkAssertTestObjects OBJECT AssertTest.cpp)
target_include_directories(VkAssertTestObjects PRIVATE $<TARGET_PROPERTY:MagnumVk,INTERFACE_INCLUDE_DIRECTORIES>)
corrade_add_test(VkAssertTest
    $<TARGET_OBJECTS:VkAssertTestObjects>
    ${PROJECT_SOURCE_DIR}/src/dummy.cpp
    LIBRARIES MagnumVk)
# WILL_FAIL doesn't work for abort() on desktop, test this only on embedded
# then. Oh well. Also the tests could be just one executable added multiple
# times with different arguments, but corrade_add_test() doesn't support that,
# so I'm doing it at least via an OBJECT library.
if(CORRADE_TARGET_ANDROID)
    corrade_add_test(VkAssertTestFailAssertSuccess
        $<TARGET_OBJECTS:VkAssertTestObjects>
        ${PROJECT_SOURCE_DIR}/src/dummy.cpp
        ARGUMENTS --fail-on-assert-success true
        LIBRARIES MagnumVk)
    set_tests_properties(VkAssertTestFailAssertSuccess PROPERTIES
        PASS_REGULAR_EXPRESSION "Call a = r failed with Vk::Result::ErrorFragmentedPool at ")
    corrade_add_test(VkAssertTestFailAssertSucce___Incomplete
        $<TARGET_OBJECTS:VkAssertTestObjects>
        ${PROJECT_SOURCE_DIR}/src/dummy.cpp
        ARGUMENTS --fail-on-assert-success-or-incomplete true
        LIBRARIES MagnumVk)
    set_tests_properties(VkAssertTestFailAssertSucce___Incomplete PROPERTIES
        PASS_REGULAR_EXPRESSION "Call a = r failed with Vk::Result::ErrorExtensionNotPresent at ")
    corrade_add_test(VkAssertTestFailAssertVkSuccess
        $<TARGET_OBJECTS:VkAssertTestObjects>
        ${PROJECT_SOURCE_DIR}/src/dummy.cpp
        ARGUMENTS --fail-on-assert-vk-success true
        LIBRARIES MagnumVk)
    set_tests_properties(VkAssertTestFailAssertVkSuccess PROPERTIES
        PASS_REGULAR_EXPRESSION "Call a = s failed with Vk::Result::ErrorFragmentedPool at ")
    corrade_add_test(VkAssertTestFailAssertVkSuc___Incomplete
        $<TARGET_OBJECTS:VkAssertTestObjects>
        ${PROJECT_SOURCE_DIR}/src/dummy.cpp
        ARGUMENTS --fail-on-assert-vk-success-or-incomplete true
        LIBRARIES MagnumVk)
    set_tests_properties(VkAssertTestFailAssertVkSuc___Incomplete PROPERTIES
        PASS_REGULAR_EXPRESSION "Call a = s failed with Vk::Result::ErrorExtensionNotPresent at ")
endif()

# The same as above, but using CORRADE_STANDARD_ASSERT
corrade_add_test(VkAssertStandardTest AssertTest.cpp LIBRARIES MagnumVk)
target_compile_definitions(VkAssertStandardTest PRIVATE
    "CORRADE_STANDARD_ASSERT")

corrade_add_test(VkAssertDisabledTest AssertDisabledTest.cpp LIBRARIES MagnumVk)
# The same as above, but using CORRADE_STANDARD_ASSERT and NDEBUG
corrade_add_test(VkAssertStandardDisabledTest AssertDisabledTest.cpp LIBRARIES MagnumVk)
target_compile_definitions(VkAssertStandardDisabledTest PRIVATE
    "CORRADE_STANDARD_ASSERT" "NDEBUG")

corrade_add_test(VkVersionTest VersionTest.cpp LIBRARIES MagnumVk)

if(BUILD_VK_TESTS)
    if(CORRADE_TARGET_ANDROID)
        set(VK_TEST_DIR ".")
    else()
        set(VK_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
                   ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

    corrade_add_test(VkBufferVkTest BufferVkTest.cpp LIBRARIES MagnumVulkanTester)
    corrade_add_test(VkCommandBufferVkTest CommandBufferVkTest.cpp LIBRARIES MagnumVulkanTester)
    corrade_add_test(VkCommandPoolVkTest CommandPoolVkTest.cpp LIBRARIES MagnumVulkanTester)
    corrade_add_test(VkDeviceVkTest DeviceVkTest.cpp LIBRARIES MagnumVkTestLib MagnumVulkanTester)
    corrade_add_test(VkDevicePropertiesVkTest DevicePropertiesVkTest.cpp LIBRARIES  MagnumVkTestLib MagnumVulkanTester)
    corrade_add_test(VkExtensionPropertiesVkTest ExtensionPropertiesVkTest.cpp LIBRARIES MagnumVkTestLib)
    corrade_add_test(VkLayerPropertiesVkTest LayerPropertiesVkTest.cpp LIBRARIES MagnumVkTestLib)
    corrade_add_test(VkImageVkTest ImageVkTest.cpp LIBRARIES MagnumVk MagnumVulkanTester)
    corrade_add_test(VkInstanceVkTest InstanceVkTest.cpp LIBRARIES MagnumVk)
    corrade_add_test(VkMemoryVkTest MemoryVkTest.cpp LIBRARIES MagnumVk MagnumVulkanTester)
    corrade_add_test(VkShaderVkTest ShaderVkTest.cpp LIBRARIES MagnumVk MagnumVulkanTester)
    target_include_directories(VkShaderVkTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
    corrade_add_test(VkVersionVkTest VersionVkTest.cpp LIBRARIES MagnumVk)
endif()
