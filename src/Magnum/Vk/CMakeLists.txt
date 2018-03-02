#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

# We don't need system-wide includes
set(Vulkan_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/MagnumExternal/Vulkan)
find_package(Vulkan REQUIRED)

set(MagnumVk_SRCS )
set(MagnumVk_HEADERS
    Vk.h

    visibility.h)
set(MagnumVk_PRIVATE_HEADERS )

# Vk library
add_library(MagnumVk ${SHARED_OR_STATIC}
    ${MagnumVk_SRCS}
    ${MagnumVk_HEADERS}
    ${MagnumVk_PRIVATE_HEADERS}
    $<TARGET_OBJECTS:MagnumFlextVkObjects>)
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

# Magnum Vk library target alias for superprojects
add_library(Magnum::Vk ALIAS MagnumVk)

if(BUILD_TESTS)
    add_subdirectory(Test)
endif()
