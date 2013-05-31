#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>
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
set(MagnumMeshTools_SRCS
    CompressIndices.cpp
    Tipsify.cpp)

# Files compiled with different flags for main library and unit test library
set(MagnumMeshTools_GracefulAssert_SRCS
    FlipNormals.cpp
    GenerateFlatNormals.cpp)

set(MagnumMeshTools_HEADERS
    CombineIndexedArrays.h
    CompressIndices.h
    Duplicate.h
    FlipNormals.h
    GenerateFlatNormals.h
    Interleave.h
    RemoveDuplicates.h
    Subdivide.h
    Tipsify.h
    Transform.h

    magnumMeshToolsVisibility.h)

# Set shared library flags for the objects, as they will be part of shared lib
# TODO: fix when CMake sets target_EXPORTS for OBJECT targets as well
add_library(MagnumMeshToolsObjects OBJECT ${MagnumMeshTools_SRCS})
if(NOT BUILD_SHARED OR BUILD_STATIC_PIC)
    # TODO: CMake 2.8.9 has this as POSITION_INDEPENDENT_CODE property
    set_target_properties(MagnumMeshToolsObjects PROPERTIES COMPILE_FLAGS "-DMagnumMeshToolsObjects_EXPORTS ${CMAKE_SHARED_LIBRARY_CXX_FLAGS}")
else()
    set_target_properties(MagnumMeshToolsObjects PROPERTIES COMPILE_FLAGS "-DMagnumMeshToolsObjects_EXPORTS")
endif()

# Main library
add_library(MagnumMeshTools ${SHARED_OR_STATIC}
    $<TARGET_OBJECTS:MagnumMeshToolsObjects>
    ${MagnumMeshTools_GracefulAssert_SRCS})
if(BUILD_STATIC_PIC)
    # TODO: CMake 2.8.9 has this as POSITION_INDEPENDENT_CODE property
    set_target_properties(MagnumMeshTools PROPERTIES COMPILE_FLAGS ${CMAKE_SHARED_LIBRARY_CXX_FLAGS})
endif()
target_link_libraries(MagnumMeshTools Magnum)

install(TARGETS MagnumMeshTools DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumMeshTools_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/MeshTools)

if(BUILD_TESTS)
    # Library with graceful assert for testing
    add_library(MagnumMeshToolsTestLib ${SHARED_OR_STATIC}
        $<TARGET_OBJECTS:MagnumMeshToolsObjects>
        ${MagnumMeshTools_GracefulAssert_SRCS})
    set_target_properties(MagnumMeshToolsTestLib PROPERTIES COMPILE_FLAGS "-DCORRADE_GRACEFUL_ASSERT -DMagnumMeshTools_EXPORTS")
    target_link_libraries(MagnumMeshToolsTestLib Magnum)

    add_subdirectory(Test)
endif()
