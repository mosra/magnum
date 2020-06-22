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

find_package(Vulkan REQUIRED)

set(MagnumVk_SRCS
    DeviceProperties.cpp
    Extensions.cpp
    Handle.cpp
    Instance.cpp
    Result.cpp
    Version.cpp

    Implementation/Arguments.cpp
    Implementation/InstanceState.cpp)

set(MagnumVk_GracefulAssert_SRCS
    Enums.cpp
    ExtensionProperties.cpp
    LayerProperties.cpp)

set(MagnumVk_HEADERS
    Device.h
    DeviceProperties.h
    Enums.h
    Extensions.h
    ExtensionProperties.h
    Handle.h
    Instance.h
    Integration.h
    LayerProperties.h
    Result.h
    TypeTraits.h
    Version.h
    Vk.h
    Vulkan.h

    visibility.h)

set(MagnumVk_PRIVATE_HEADERS
    Implementation/Arguments.h
    Implementation/InstanceState.h

    Implementation/compressedPixelFormatMapping.hpp
    Implementation/pixelFormatMapping.hpp
    Implementation/vertexFormatMapping.hpp)

# Objects shared between main and test library
add_library(MagnumVkObjects OBJECT
    ${MagnumVk_SRCS}
    ${MagnumVk_HEADERS}
    ${MagnumVk_PRIVATE_HEADERS})
target_include_directories(MagnumVkObjects PUBLIC
    $<TARGET_PROPERTY:Magnum,INTERFACE_INCLUDE_DIRECTORIES>)
if(NOT BUILD_STATIC)
    target_compile_definitions(MagnumVkObjects PRIVATE "MagnumVkObjects_EXPORTS" "FlextVk_EXPORTS")
endif()
if(NOT BUILD_STATIC OR BUILD_STATIC_PIC)
    set_target_properties(MagnumVkObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
set_target_properties(MagnumVkObjects PROPERTIES FOLDER "Magnum/Vk")

# Vk library
add_library(MagnumVk ${SHARED_OR_STATIC}
    $<TARGET_OBJECTS:MagnumVkObjects>
    $<TARGET_OBJECTS:MagnumFlextVkObjects>
    ${MagnumVk_GracefulAssert_SRCS})
set_target_properties(MagnumVk PROPERTIES
    DEBUG_POSTFIX "-d"
    FOLDER "Magnum/Vk")
if(NOT BUILD_STATIC)
    target_compile_definitions(MagnumVk PRIVATE "FlextVk_EXPORTS")
    set_target_properties(MagnumVk PROPERTIES VERSION ${MAGNUM_LIBRARY_VERSION} SOVERSION ${MAGNUM_LIBRARY_SOVERSION})
elseif(BUILD_STATIC_PIC)
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

if(BUILD_TESTS)
    # Library with graceful assert for testing
    add_library(MagnumVkTestLib ${SHARED_OR_STATIC}
        $<TARGET_OBJECTS:MagnumVkObjects>
        $<TARGET_OBJECTS:MagnumFlextVkObjects>
        ${MagnumVk_GracefulAssert_SRCS})
    set_target_properties(MagnumVkTestLib PROPERTIES
        DEBUG_POSTFIX "-d"
        FOLDER "Magnum/Vk")
    target_compile_definitions(MagnumVkTestLib PRIVATE
        "CORRADE_GRACEFUL_ASSERT" "MagnumVk_EXPORTS" "FlextVk_EXPORTS")
    if(BUILD_STATIC_PIC)
        set_target_properties(MagnumVkTestLib PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    target_link_libraries(MagnumVkTestLib PUBLIC
        Magnum
        Vulkan::Vulkan)

    add_subdirectory(Test)
endif()

# Magnum Vk library target alias for superprojects
add_library(Magnum::Vk ALIAS MagnumVk)
