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

corrade_add_test(MeshToolsCombineTest CombineTest.cpp LIBRARIES MagnumMeshToolsTestLib)
corrade_add_test(MeshToolsCompressIndicesTest CompressIndicesTest.cpp LIBRARIES MagnumMeshToolsTestLib)
corrade_add_test(MeshToolsConcatenateTest ConcatenateTest.cpp LIBRARIES MagnumMeshToolsTestLib)
corrade_add_test(MeshToolsDuplicateTest DuplicateTest.cpp LIBRARIES MagnumMeshToolsTestLib)
corrade_add_test(MeshToolsFlipNormalsTest FlipNormalsTest.cpp LIBRARIES MagnumMeshToolsTestLib)
corrade_add_test(MeshToolsGenerateIndicesTest GenerateIndicesTest.cpp LIBRARIES MagnumMeshToolsTestLib)
corrade_add_test(MeshToolsGenerateNormalsTest GenerateNormalsTest.cpp LIBRARIES MagnumMeshToolsTestLib MagnumPrimitives)
corrade_add_test(MeshToolsInterleaveTest InterleaveTest.cpp LIBRARIES MagnumMeshToolsTestLib)
corrade_add_test(MeshToolsReferenceTest ReferenceTest.cpp LIBRARIES MagnumMeshToolsTestLib MagnumPrimitives)
corrade_add_test(MeshToolsRemoveDuplicatesTest RemoveDuplicatesTest.cpp LIBRARIES MagnumMeshToolsTestLib)
corrade_add_test(MeshToolsSubdivideTest SubdivideTest.cpp LIBRARIES Magnum MagnumPrimitives)
corrade_add_test(MeshToolsTipsifyTest TipsifyTest.cpp LIBRARIES MagnumMeshTools)
corrade_add_test(MeshToolsTransformTest TransformTest.cpp LIBRARIES MagnumMeshTools)

# Graceful assert for testing
set_property(TARGET
    MeshToolsConcatenateTest
    MeshToolsDuplicateTest
    MeshToolsInterleaveTest
    MeshToolsRemoveDuplicatesTest
    MeshToolsSubdivideTest
    APPEND PROPERTY COMPILE_DEFINITIONS "CORRADE_GRACEFUL_ASSERT")

set_target_properties(
    MeshToolsCombineTest
    MeshToolsCompressIndicesTest
    MeshToolsConcatenateTest
    MeshToolsDuplicateTest
    MeshToolsFlipNormalsTest
    MeshToolsGenerateIndicesTest
    MeshToolsGenerateNormalsTest
    MeshToolsInterleaveTest
    MeshToolsRemoveDuplicatesTest
    MeshToolsSubdivideTest
    MeshToolsTipsifyTest
    MeshToolsTransformTest
    PROPERTIES FOLDER "Magnum/MeshTools/Test")

if(BUILD_DEPRECATED)
    corrade_add_test(MeshToolsCombineIndexedArraysTest CombineIndexedArraysTest.cpp LIBRARIES MagnumMeshToolsTestLib)
    set_property(TARGET
        MeshToolsCombineIndexedArraysTest
        APPEND PROPERTY COMPILE_DEFINITIONS "CORRADE_GRACEFUL_ASSERT")
    set_target_properties(
        MeshToolsCombineIndexedArraysTest
        PROPERTIES FOLDER "Magnum/MeshTools/Test")
endif()

if(BUILD_GL_TESTS)
    # Otherwise CMake complains that Corrade::PluginManager is not found
    find_package(Corrade REQUIRED PluginManager)

    corrade_add_resource(FullScreenTriangleGLTest_RESOURCES resources.conf)
    set_target_properties(FullScreenTriangleGLTest_RESOURCES-dependencies PROPERTIES FOLDER "Magnum/MeshTools/Test")

    corrade_add_test(MeshToolsFullScreenTriangleGLTest
        FullScreenTriangleGLTest.cpp ${FullScreenTriangleGLTest_RESOURCES}
        LIBRARIES MagnumMeshTools MagnumGL MagnumOpenGLTester)

    if(CORRADE_TARGET_EMSCRIPTEN OR CORRADE_TARGET_ANDROID)
        set(COMPILEGLTEST_TEST_DIR ".")
    else()
        set(COMPILEGLTEST_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/CompileTestFiles)
    endif()

    # CMake before 3.8 has broken $<TARGET_FILE*> expressions for iOS (see
    # https://gitlab.kitware.com/cmake/cmake/merge_requests/404) and since
    # Corrade doesn't support dynamic plugins on iOS, this sorta works around
    # that. Should be revisited when updating Travis to newer Xcode (xcode7.3
    # has CMake 3.6).
    if(NOT BUILD_PLUGINS_STATIC)
        if(WITH_ANYIMAGEIMPORTER)
            set(ANYIMAGEIMPORTER_PLUGIN_FILENAME $<TARGET_FILE:AnyImageImporter>)
        endif()
        if(WITH_TGAIMPORTER)
            set(TGAIMPORTER_PLUGIN_FILENAME $<TARGET_FILE:TgaImporter>)
        endif()
    endif()

    # First replace ${} variables, then $<> generator expressions
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
                   ${CMAKE_CURRENT_BINARY_DIR}/configure.h.in)
    file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/configure.h
        INPUT ${CMAKE_CURRENT_BINARY_DIR}/configure.h.in)

    corrade_add_test(MeshToolsCompileGLTest CompileGLTest.cpp
        LIBRARIES
            MagnumDebugTools
            MagnumGL
            MagnumOpenGLTester
            MagnumMeshToolsTestLib
            MagnumShaders
        FILES
            CompileTestFiles/color2D.tga
            CompileTestFiles/color3D.tga
            CompileTestFiles/flat2D.tga
            CompileTestFiles/flat3D.tga
            CompileTestFiles/phong.tga
            CompileTestFiles/phong-flat.tga
            CompileTestFiles/textured2D.tga
            CompileTestFiles/textured3D.tga)
    set_target_properties(MeshToolsCompileGLTest PROPERTIES FOLDER "Magnum/MeshTools/Test")
    target_include_directories(MeshToolsCompileGLTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
    if(BUILD_PLUGINS_STATIC)
        if(WITH_ANYIMAGEIMPORTER)
            target_link_libraries(MeshToolsCompileGLTest PRIVATE AnyImageImporter)
        endif()
        if(WITH_TGAIMPORTER)
            target_link_libraries(MeshToolsCompileGLTest PRIVATE TgaImporter)
        endif()
    endif()
endif()
