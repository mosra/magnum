#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020, 2021, 2022, 2023, 2024, 2025
#             Vladimír Vondruš <mosra@centrum.cz>
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
set(CMAKE_FOLDER "Magnum/MaterialTools")

# Files shared between main library and unit test library
set(MagnumMaterialTools_SRCS
    Copy.cpp
    PhongToPbrMetallicRoughness.cpp)

# Files compiled with different flags for main library and unit test library
set(MagnumMaterialTools_GracefulAssert_SRCS
    Filter.cpp
    Merge.cpp
    RemoveDuplicates.cpp)

set(MagnumMaterialTools_HEADERS
    Copy.h
    Filter.h
    Merge.h
    PhongToPbrMetallicRoughness.h
    RemoveDuplicates.h

    visibility.h)

set(MagnumMaterialTools_PRIVATE_HEADERS
    Implementation/attributesEqual.h)

# Objects shared between main and test library
add_library(MagnumMaterialToolsObjects OBJECT
    ${MagnumMaterialTools_SRCS}
    ${MagnumMaterialTools_HEADERS}
    ${MagnumMaterialTools_PRIVATE_HEADERS})
target_include_directories(MagnumMaterialToolsObjects PUBLIC $<TARGET_PROPERTY:Magnum,INTERFACE_INCLUDE_DIRECTORIES>)
if(NOT MAGNUM_BUILD_STATIC)
    target_compile_definitions(MagnumMaterialToolsObjects PRIVATE "MagnumMaterialToolsObjects_EXPORTS")
endif()
if(NOT MAGNUM_BUILD_STATIC OR MAGNUM_BUILD_STATIC_PIC)
    set_target_properties(MagnumMaterialToolsObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Main MaterialTools library
add_library(MagnumMaterialTools ${SHARED_OR_STATIC}
    $<TARGET_OBJECTS:MagnumMaterialToolsObjects>
    ${MagnumMaterialTools_GracefulAssert_SRCS})
set_target_properties(MagnumMaterialTools PROPERTIES DEBUG_POSTFIX "-d")
if(NOT MAGNUM_BUILD_STATIC)
    set_target_properties(MagnumMaterialTools PROPERTIES VERSION ${MAGNUM_LIBRARY_VERSION} SOVERSION ${MAGNUM_LIBRARY_SOVERSION})
elseif(MAGNUM_BUILD_STATIC_PIC)
    set_target_properties(MagnumMaterialTools PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(MagnumMaterialTools PUBLIC
    Magnum
    MagnumTrade)

install(TARGETS MagnumMaterialTools
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumMaterialTools_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/MaterialTools)

if(MAGNUM_BUILD_TESTS)
    # Library with graceful assert for testing
    add_library(MagnumMaterialToolsTestLib ${SHARED_OR_STATIC} ${EXCLUDE_FROM_ALL_IF_TEST_TARGET}
        $<TARGET_OBJECTS:MagnumMaterialToolsObjects>
        ${MagnumMaterialTools_GracefulAssert_SRCS})
    set_target_properties(MagnumMaterialToolsTestLib PROPERTIES DEBUG_POSTFIX "-d")
    target_compile_definitions(MagnumMaterialToolsTestLib PRIVATE
        "CORRADE_GRACEFUL_ASSERT" "MagnumMaterialTools_EXPORTS")
    if(MAGNUM_BUILD_STATIC_PIC)
        set_target_properties(MagnumMaterialToolsTestLib PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    target_link_libraries(MagnumMaterialToolsTestLib PUBLIC
        Magnum
        MagnumTrade)

    add_subdirectory(Test ${EXCLUDE_FROM_ALL_IF_TEST_TARGET})
endif()

# Magnum MaterialTools target alias for superprojects
add_library(Magnum::MaterialTools ALIAS MagnumMaterialTools)
