#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020 Vladimír Vondruš <mosra@centrum.cz>
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
    Tipsify.cpp)

# Files compiled with different flags for main library and unit test library
set(MagnumMeshTools_GracefulAssert_SRCS
    Combine.cpp
    CompressIndices.cpp
    Concatenate.cpp
    Duplicate.cpp
    FlipNormals.cpp
    GenerateIndices.cpp
    GenerateNormals.cpp
    Interleave.cpp
    Reference.cpp
    RemoveDuplicates.cpp)

set(MagnumMeshTools_HEADERS
    Combine.h
    CompressIndices.h
    Concatenate.h
    Duplicate.h
    FlipNormals.h
    GenerateIndices.h
    GenerateNormals.h
    Interleave.h
    Reference.h
    RemoveDuplicates.h
    Subdivide.h
    Tipsify.h
    Transform.h

    visibility.h)

set(MagnumMeshTools_INTERNAL_HEADERS
    Implementation/Tipsify.h)

if(BUILD_DEPRECATED)
    list(APPEND MagnumMeshTools_GracefulAssert_SRCS
        CombineIndexedArrays.cpp)
    list(APPEND MagnumMeshTools_HEADERS
        CombineIndexedArrays.h
        GenerateFlatNormals.h)
endif()

if(TARGET_GL)
    list(APPEND MagnumMeshTools_SRCS
        FullScreenTriangle.cpp)

    list(APPEND MagnumMeshTools_GracefulAssert_SRCS
        Compile.cpp)

    list(APPEND MagnumMeshTools_HEADERS
        Compile.h
        FullScreenTriangle.h)
endif()

# Objects shared between main and test library
add_library(MagnumMeshToolsObjects OBJECT
    ${MagnumMeshTools_SRCS}
    ${MagnumMeshTools_HEADERS}
    ${MagnumMeshTools_INTERNAL_HEADERS})
target_include_directories(MagnumMeshToolsObjects PUBLIC $<TARGET_PROPERTY:Magnum,INTERFACE_INCLUDE_DIRECTORIES>)
if(NOT BUILD_STATIC)
    target_compile_definitions(MagnumMeshToolsObjects PRIVATE "MagnumMeshToolsObjects_EXPORTS")
endif()
if(NOT BUILD_STATIC OR BUILD_STATIC_PIC)
    set_target_properties(MagnumMeshToolsObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
set_target_properties(MagnumMeshToolsObjects PROPERTIES FOLDER "Magnum/MeshTools")
if(TARGET_GL)
    target_include_directories(MagnumMeshToolsObjects PUBLIC $<TARGET_PROPERTY:MagnumGL,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

# Main MeshTools library
add_library(MagnumMeshTools ${SHARED_OR_STATIC}
    $<TARGET_OBJECTS:MagnumMeshToolsObjects>
    ${MagnumMeshTools_GracefulAssert_SRCS})
set_target_properties(MagnumMeshTools PROPERTIES
    DEBUG_POSTFIX "-d"
    FOLDER "Magnum/MeshTools")
if(NOT BUILD_STATIC)
    set_target_properties(MagnumMeshTools PROPERTIES VERSION ${MAGNUM_LIBRARY_VERSION} SOVERSION ${MAGNUM_LIBRARY_SOVERSION})
elseif(BUILD_STATIC_PIC)
    set_target_properties(MagnumMeshTools PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(MagnumMeshTools PUBLIC
    Magnum MagnumTrade)
if(TARGET_GL)
    target_link_libraries(MagnumMeshTools PUBLIC MagnumGL)
endif()

install(TARGETS MagnumMeshTools
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumMeshTools_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/MeshTools)

if(WITH_SCENECONVERTER)
    add_executable(magnum-sceneconverter sceneconverter.cpp)
    target_link_libraries(magnum-sceneconverter PRIVATE
        Magnum
        MagnumMeshTools
        MagnumTrade)
    set_target_properties(magnum-sceneconverter PROPERTIES FOLDER "Magnum/MeshTools")

    install(TARGETS magnum-sceneconverter DESTINATION ${MAGNUM_BINARY_INSTALL_DIR})

    # Magnum sceneconverter target alias for superprojects
    add_executable(Magnum::sceneconverter ALIAS magnum-sceneconverter)
endif()

if(BUILD_TESTS)
    # Library with graceful assert for testing
    add_library(MagnumMeshToolsTestLib ${SHARED_OR_STATIC}
        $<TARGET_OBJECTS:MagnumMeshToolsObjects>
        ${MagnumMeshTools_GracefulAssert_SRCS})
    set_target_properties(MagnumMeshToolsTestLib PROPERTIES
        DEBUG_POSTFIX "-d"
        FOLDER "Magnum/MeshTools")
    target_compile_definitions(MagnumMeshToolsTestLib PRIVATE
        "CORRADE_GRACEFUL_ASSERT" "MagnumMeshTools_EXPORTS")
    if(BUILD_STATIC_PIC)
        set_target_properties(MagnumMeshToolsTestLib PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    target_link_libraries(MagnumMeshToolsTestLib PUBLIC
        Magnum MagnumTrade)
    if(TARGET_GL)
        target_link_libraries(MagnumMeshToolsTestLib PUBLIC MagnumGL)
    endif()

    add_subdirectory(Test)
endif()

# Magnum MeshTools target alias for superprojects
add_library(Magnum::MeshTools ALIAS MagnumMeshTools)
