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
set(CMAKE_FOLDER "Magnum/MeshTools")

# Files shared between main library and unit test library
set(MagnumMeshTools_SRCS
    BoundingVolume.cpp
    Tipsify.cpp)

# Files compiled with different flags for main library and unit test library
set(MagnumMeshTools_GracefulAssert_SRCS
    Combine.cpp
    CompressIndices.cpp
    Concatenate.cpp
    Copy.cpp
    Duplicate.cpp
    Filter.cpp
    FlipNormals.cpp
    GenerateIndices.cpp
    GenerateLines.cpp
    GenerateNormals.cpp
    Interleave.cpp
    RemoveDuplicates.cpp
    Transform.cpp)

set(MagnumMeshTools_HEADERS
    BoundingVolume.h
    Combine.h
    CompressIndices.h
    Concatenate.h
    Copy.h
    Duplicate.h
    Filter.h
    FlipNormals.h
    GenerateIndices.h
    GenerateLines.h
    GenerateNormals.h
    Interleave.h
    InterleaveFlags.h
    RemoveDuplicates.h
    Subdivide.h
    Tipsify.h
    Transform.h

    visibility.h)

set(MagnumMeshTools_INTERNAL_HEADERS
    Implementation/remapAttributeData.h
    Implementation/Tipsify.h)

if(MAGNUM_BUILD_DEPRECATED)
    list(APPEND MagnumMeshTools_GracefulAssert_SRCS
        CombineIndexedArrays.cpp)
    list(APPEND MagnumMeshTools_HEADERS
        CombineIndexedArrays.h
        FilterAttributes.h
        Reference.h)
endif()

if(MAGNUM_TARGET_GL)
    list(APPEND MagnumMeshTools_SRCS
        FullScreenTriangle.cpp)

    list(APPEND MagnumMeshTools_GracefulAssert_SRCS
        Compile.cpp)

    list(APPEND MagnumMeshTools_HEADERS
        Compile.h
        FullScreenTriangle.h)

    if(NOT MAGNUM_TARGET_GLES2)
        list(APPEND MagnumMeshTools_GracefulAssert_SRCS
            CompileLines.cpp)

        list(APPEND MagnumMeshTools_HEADERS
            CompileLines.h)
    endif()
endif()

# Objects shared between main and test library
add_library(MagnumMeshToolsObjects OBJECT
    ${MagnumMeshTools_SRCS}
    ${MagnumMeshTools_HEADERS}
    ${MagnumMeshTools_INTERNAL_HEADERS})
target_include_directories(MagnumMeshToolsObjects PUBLIC $<TARGET_PROPERTY:Magnum,INTERFACE_INCLUDE_DIRECTORIES>)
if(NOT MAGNUM_BUILD_STATIC)
    target_compile_definitions(MagnumMeshToolsObjects PRIVATE "MagnumMeshToolsObjects_EXPORTS")
endif()
if(NOT MAGNUM_BUILD_STATIC OR MAGNUM_BUILD_STATIC_PIC)
    set_target_properties(MagnumMeshToolsObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
if(MAGNUM_TARGET_GL)
    target_include_directories(MagnumMeshToolsObjects PUBLIC $<TARGET_PROPERTY:MagnumGL,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

# Main MeshTools library
add_library(MagnumMeshTools ${SHARED_OR_STATIC}
    $<TARGET_OBJECTS:MagnumMeshToolsObjects>
    ${MagnumMeshTools_GracefulAssert_SRCS})
set_target_properties(MagnumMeshTools PROPERTIES DEBUG_POSTFIX "-d")
if(NOT MAGNUM_BUILD_STATIC)
    set_target_properties(MagnumMeshTools PROPERTIES VERSION ${MAGNUM_LIBRARY_VERSION} SOVERSION ${MAGNUM_LIBRARY_SOVERSION})
elseif(MAGNUM_BUILD_STATIC_PIC)
    set_target_properties(MagnumMeshTools PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(MagnumMeshTools PUBLIC
    Magnum MagnumTrade)
if(MAGNUM_TARGET_GL)
    target_link_libraries(MagnumMeshTools PUBLIC MagnumGL)
endif()

install(TARGETS MagnumMeshTools
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumMeshTools_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/MeshTools)

if(MAGNUM_BUILD_TESTS)
    # Library with graceful assert for testing
    add_library(MagnumMeshToolsTestLib ${SHARED_OR_STATIC} ${EXCLUDE_FROM_ALL_IF_TEST_TARGET}
        $<TARGET_OBJECTS:MagnumMeshToolsObjects>
        ${MagnumMeshTools_GracefulAssert_SRCS})
    set_target_properties(MagnumMeshToolsTestLib PROPERTIES DEBUG_POSTFIX "-d")
    target_compile_definitions(MagnumMeshToolsTestLib PRIVATE
        "CORRADE_GRACEFUL_ASSERT" "MagnumMeshTools_EXPORTS")
    if(MAGNUM_BUILD_STATIC_PIC)
        set_target_properties(MagnumMeshToolsTestLib PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    target_link_libraries(MagnumMeshToolsTestLib PUBLIC
        Magnum MagnumTrade)
    if(MAGNUM_TARGET_GL)
        target_link_libraries(MagnumMeshToolsTestLib PUBLIC MagnumGL)
    endif()

    add_subdirectory(Test ${EXCLUDE_FROM_ALL_IF_TEST_TARGET})
endif()

# Magnum MeshTools target alias for superprojects
add_library(Magnum::MeshTools ALIAS MagnumMeshTools)
