#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
#             Vladimír Vondruš <mosra@centrum.cz>
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

# Files shared between main library and unit test library
find_package(Vulkan REQUIRED)

set(MagnumVk_SRCS
    Buffer.cpp
    CommandBuffer.cpp
    CommandPool.cpp
    Context.cpp
    DescriptorPool.cpp
    DescriptorSet.cpp
    Device.cpp
    DeviceMemory.cpp
    Framebuffer.cpp
    Image.cpp
    ImageView.cpp
    PhysicalDevice.cpp
    Pipeline.cpp
    Queue.cpp
    RenderPass.cpp
    Semaphore.cpp
    Shader.cpp
    Swapchain.cpp)

set(MagnumVk_HEADERS
    Buffer.h
    Command.h
    CommandBuffer.h
    CommandPool.h
    Context.h
    DescriptorPool.h
    DescriptorSet.h
    Device.h
    DeviceMemory.h
    Framebuffer.h
    Image.h
    ImageView.h
    Math.h
    PhysicalDevice.h
    Pipeline.h
    Queue.h
    RenderPass.h
    Semaphore.h
    Shader.h
    ShaderStage.h
    Swapchain.h

    visibility.h)

set(MagnumVkTest_HEADERS
    Test/AbstractVulkanTester.h)

# Header files to display in project view of IDEs only
set(MagnumVk_PRIVATE_HEADERS "")

# Magnum Vulkan library
add_library(MagnumVk ${SHARED_OR_STATIC}
    ${MagnumVk_SRCS}
    ${MagnumVk_HEADERS}

    ${MagnumVkTest_HEADERS}
    ${MagnumVk_PRIVATE_HEADERS})
set_target_properties(MagnumVk PROPERTIES DEBUG_POSTFIX "-d")
if(BUILD_STATIC_PIC)
    set_target_properties(MagnumVk PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(MagnumVk Magnum Vulkan::Vulkan)

set_target_properties(MagnumVk PROPERTIES
    CORRADE_CXX_STANDARD 14
    INTERFACE_CORRADE_CXX_STANDARD 14)


install(TARGETS MagnumVk
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

install(FILES ${MagnumVkTest_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Vk/Test)
install(FILES ${MagnumVk_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Vk)

if(BUILD_TESTS)
    set(VK_TEST_LIBRARIES MagnumVk)
    add_subdirectory(Test)
endif()

# Magnum Shaders library for superprojects
add_library(Magnum::Vk ALIAS MagnumVk)
