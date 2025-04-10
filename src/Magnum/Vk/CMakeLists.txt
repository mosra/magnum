#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020, 2021, 2022, 2023, 2024, 2025
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

# IDE folder in VS, Xcode etc. CMake 3.12+, older versions have only the FOLDER
# property that would have to be set on each target separately.
set(CMAKE_FOLDER "Magnum/Vk")

find_package(Vulkan REQUIRED)

set(MagnumVk_SRCS
    CommandBuffer.cpp
    CommandPool.cpp
    DescriptorSet.cpp
    DescriptorSetLayout.cpp
    DescriptorType.cpp
    Extensions.cpp
    Fence.cpp
    Framebuffer.cpp
    Handle.cpp
    PipelineLayout.cpp
    Queue.cpp
    Result.cpp
    Shader.cpp
    Version.cpp

    Implementation/Arguments.cpp
    Implementation/DeviceState.cpp
    Implementation/DriverWorkaround.cpp
    Implementation/InstanceState.cpp)

set(MagnumVk_GracefulAssert_SRCS
    Buffer.cpp
    DescriptorPool.cpp
    Device.cpp
    DeviceProperties.cpp
    DeviceFeatures.cpp
    ExtensionProperties.cpp
    Image.cpp
    ImageView.cpp
    Instance.cpp
    LayerProperties.cpp
    Mesh.cpp
    MeshLayout.cpp
    Memory.cpp
    Pipeline.cpp
    PixelFormat.cpp
    RenderPass.cpp
    Sampler.cpp
    ShaderSet.cpp
    VertexFormat.cpp)

set(MagnumVk_HEADERS
    Assert.h
    Buffer.h
    BufferCreateInfo.h
    CommandBuffer.h
    CommandPool.h
    CommandPoolCreateInfo.h
    ComputePipelineCreateInfo.h
    DescriptorPool.h
    DescriptorPoolCreateInfo.h
    DescriptorSet.h
    DescriptorSetLayout.h
    DescriptorSetLayoutCreateInfo.h
    DescriptorType.h
    Device.h
    DeviceCreateInfo.h
    DeviceFeatures.h
    DeviceProperties.h
    Extensions.h
    ExtensionProperties.h
    Fence.h
    FenceCreateInfo.h
    Framebuffer.h
    FramebufferCreateInfo.h
    Handle.h
    Image.h
    ImageCreateInfo.h
    ImageView.h
    ImageViewCreateInfo.h
    Instance.h
    InstanceCreateInfo.h
    Integration.h
    LayerProperties.h
    Memory.h
    MemoryAllocateInfo.h
    Mesh.h
    MeshLayout.h
    Pipeline.h
    PipelineLayout.h
    PipelineLayoutCreateInfo.h
    PixelFormat.h
    Queue.h
    RasterizationPipelineCreateInfo.h
    RenderPass.h
    RenderPassCreateInfo.h
    Result.h
    Sampler.h
    SamplerCreateInfo.h
    Shader.h
    ShaderCreateInfo.h
    ShaderSet.h
    TypeTraits.h
    Version.h
    VertexFormat.h
    Vk.h
    Vulkan.h

    visibility.h)

set(MagnumVk_PRIVATE_HEADERS
    Implementation/Arguments.h
    Implementation/DeviceFeatures.h
    Implementation/DeviceState.h
    Implementation/DriverWorkaround.h
    Implementation/InstanceState.h

    Implementation/compressedPixelFormatMapping.hpp
    Implementation/deviceFeatureMapping.hpp
    Implementation/dynamicRasterizationStateMapping.hpp
    Implementation/pixelFormatMapping.hpp
    Implementation/structureHelpers.h
    Implementation/vertexFormatMapping.hpp)

if(MAGNUM_BUILD_DEPRECATED)
    list(APPEND MagnumVk_SRCS Enums.cpp)
    list(APPEND MagnumVk_HEADERS Enums.h)
endif()

# Objects shared between main and test library
add_library(MagnumVkObjects OBJECT
    ${MagnumVk_SRCS}
    ${MagnumVk_HEADERS}
    ${MagnumVk_PRIVATE_HEADERS})
target_include_directories(MagnumVkObjects PUBLIC
    $<TARGET_PROPERTY:Magnum,INTERFACE_INCLUDE_DIRECTORIES>)
if(NOT MAGNUM_BUILD_STATIC)
    target_compile_definitions(MagnumVkObjects PRIVATE "MagnumVkObjects_EXPORTS" "FlextVk_EXPORTS")
endif()
if(NOT MAGNUM_BUILD_STATIC OR MAGNUM_BUILD_STATIC_PIC)
    set_target_properties(MagnumVkObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Vk library
add_library(MagnumVk ${SHARED_OR_STATIC}
    $<TARGET_OBJECTS:MagnumVkObjects>
    $<TARGET_OBJECTS:MagnumFlextVkObjects>
    ${MagnumVk_GracefulAssert_SRCS})
set_target_properties(MagnumVk PROPERTIES DEBUG_POSTFIX "-d")
if(NOT MAGNUM_BUILD_STATIC)
    target_compile_definitions(MagnumVk PRIVATE "FlextVk_EXPORTS")
    set_target_properties(MagnumVk PROPERTIES VERSION ${MAGNUM_LIBRARY_VERSION} SOVERSION ${MAGNUM_LIBRARY_SOVERSION})
elseif(MAGNUM_BUILD_STATIC_PIC)
    set_target_properties(MagnumVk PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_include_directories(MagnumVk PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}/src)
target_link_libraries(MagnumVk PUBLIC
    Magnum
    Vulkan::Vulkan)

install(TARGETS MagnumVk
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumVk_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Vk)

if(MAGNUM_WITH_VK_INFO)
    find_package(Corrade REQUIRED Main)

    add_executable(magnum-vk-info vk-info.cpp)
    target_link_libraries(magnum-vk-info PRIVATE
        Corrade::Main
        MagnumVk)

    install(TARGETS magnum-vk-info DESTINATION ${MAGNUM_BINARY_INSTALL_DIR})

    # Magnum Vk Info target alias for superprojects
    add_executable(Magnum::vk-info ALIAS magnum-vk-info)
endif()

if(MAGNUM_WITH_VULKANTESTER)
    if(NOT MAGNUM_TARGET_VK)
        message(SEND_ERROR "VulkanTester is available only if MAGNUM_TARGET_VK is enabled")
    endif()

    find_package(Corrade REQUIRED TestSuite)

    set(MagnumVulkanTester_SRCS VulkanTester.cpp)
    set(MagnumVulkanTester_HEADERS VulkanTester.h)

    # Unlike with OpenGLTester, we shouldn't need a separate library that links
    # exclusively to MagnumVkTestLib as there is no global state that could
    # cause problems on Windows.
    add_library(MagnumVulkanTester STATIC
        ${MagnumVulkanTester_SRCS}
        ${MagnumVulkanTester_HEADERS})
    set_target_properties(MagnumVulkanTester PROPERTIES DEBUG_POSTFIX "-d")
    target_link_libraries(MagnumVulkanTester PUBLIC MagnumVk Corrade::TestSuite)

    install(FILES ${MagnumVulkanTester_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Vk)
    install(TARGETS MagnumVulkanTester
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Magnum VulkanTester target alias for superprojects
    add_library(Magnum::VulkanTester ALIAS MagnumVulkanTester)
endif()

if(MAGNUM_BUILD_TESTS)
    # Library with graceful assert for testing
    add_library(MagnumVkTestLib ${SHARED_OR_STATIC} ${EXCLUDE_FROM_ALL_IF_TEST_TARGET}
        $<TARGET_OBJECTS:MagnumVkObjects>
        $<TARGET_OBJECTS:MagnumFlextVkObjects>
        ${MagnumVk_GracefulAssert_SRCS})
    set_target_properties(MagnumVkTestLib PROPERTIES DEBUG_POSTFIX "-d")
    target_compile_definitions(MagnumVkTestLib PRIVATE
        "CORRADE_GRACEFUL_ASSERT" "MagnumVk_EXPORTS" "FlextVk_EXPORTS")
    if(MAGNUM_BUILD_STATIC_PIC)
        set_target_properties(MagnumVkTestLib PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    target_link_libraries(MagnumVkTestLib PUBLIC
        Magnum
        Vulkan::Vulkan)

    add_subdirectory(Test ${EXCLUDE_FROM_ALL_IF_TEST_TARGET})
endif()

# Magnum Vk library target alias for superprojects
add_library(Magnum::Vk ALIAS MagnumVk)
