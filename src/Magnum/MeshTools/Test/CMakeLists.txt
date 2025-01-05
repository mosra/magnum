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
set(CMAKE_FOLDER "Magnum/MeshTools/Test")

corrade_add_test(MeshToolsBoundingVolumeTest BoundingVolumeTest.cpp LIBRARIES MagnumMeshToolsTestLib MagnumPrimitives)
corrade_add_test(MeshToolsCombineTest CombineTest.cpp LIBRARIES MagnumMeshToolsTestLib)
corrade_add_test(MeshToolsCompressIndicesTest CompressIndicesTest.cpp LIBRARIES MagnumMeshToolsTestLib)
corrade_add_test(MeshToolsConcatenateTest ConcatenateTest.cpp LIBRARIES MagnumMeshToolsTestLib)
corrade_add_test(MeshToolsCopyTest CopyTest.cpp LIBRARIES MagnumMeshToolsTestLib MagnumPrimitives)
corrade_add_test(MeshToolsDuplicateTest DuplicateTest.cpp LIBRARIES MagnumMeshToolsTestLib)
corrade_add_test(MeshToolsFilterTest FilterTest.cpp LIBRARIES MagnumMeshToolsTestLib)
corrade_add_test(MeshToolsFlipNormalsTest FlipNormalsTest.cpp LIBRARIES MagnumMeshToolsTestLib)
corrade_add_test(MeshToolsGenerateIndicesTest GenerateIndicesTest.cpp LIBRARIES MagnumMeshToolsTestLib)
corrade_add_test(MeshToolsGenerateLinesTest GenerateLinesTest.cpp
    # Needs to link to Shaders for debug output for LineVertexAnnotations
    LIBRARIES MagnumMeshToolsTestLib MagnumShaders)
corrade_add_test(MeshToolsGenerateNormalsTest GenerateNormalsTest.cpp LIBRARIES MagnumMeshToolsTestLib MagnumPrimitives)
corrade_add_test(MeshToolsInterleaveTest InterleaveTest.cpp LIBRARIES MagnumMeshToolsTestLib)

corrade_add_test(MeshToolsRemoveDuplicatesTest RemoveDuplicatesTest.cpp LIBRARIES MagnumMeshToolsTestLib)
# In Emscripten 3.1.27, the stack size was reduced from 5 MB (!) to 64 kB:
#   https://github.com/emscripten-core/emscripten/pull/18191
# Two benchmarks in this test use 160 kB of stack space (array of 10000
# Vector3i's and another 10000 ints), need to increase it a bit to not make it
# crash.
if(CORRADE_TARGET_EMSCRIPTEN AND NOT EMSCRIPTEN_VERSION VERSION_LESS 3.1.27)
    set_property(TARGET MeshToolsRemoveDuplicatesTest APPEND_STRING PROPERTY LINK_FLAGS " -s STACK_SIZE=256kB")
endif()

corrade_add_test(MeshToolsSubdivideTest SubdivideTest.cpp LIBRARIES Magnum MagnumPrimitives)
corrade_add_test(MeshToolsTipsifyTest TipsifyTest.cpp LIBRARIES MagnumMeshTools)
corrade_add_test(MeshToolsTransformTest TransformTest.cpp LIBRARIES MagnumMeshToolsTestLib)

# Graceful assert for testing
set_property(TARGET
    MeshToolsConcatenateTest
    MeshToolsDuplicateTest
    MeshToolsInterleaveTest
    MeshToolsRemoveDuplicatesTest
    MeshToolsSubdivideTest
    APPEND PROPERTY COMPILE_DEFINITIONS "CORRADE_GRACEFUL_ASSERT")

if(MAGNUM_BUILD_DEPRECATED)
    corrade_add_test(MeshToolsCombineIndexedArraysTest CombineIndexedArraysTest.cpp LIBRARIES MagnumMeshToolsTestLib)
    set_property(TARGET
        MeshToolsCombineIndexedArraysTest
        APPEND PROPERTY COMPILE_DEFINITIONS "CORRADE_GRACEFUL_ASSERT")
endif()

if(MAGNUM_BUILD_GL_TESTS)
    # Otherwise CMake complains that Corrade::PluginManager is not found
    find_package(Corrade REQUIRED PluginManager)

    corrade_add_resource(FullScreenTriangleGLTest_RESOURCES resources.conf)

    corrade_add_test(MeshToolsFullScreenTriangleGLTest
        FullScreenTriangleGLTest.cpp ${FullScreenTriangleGLTest_RESOURCES}
        LIBRARIES MagnumDebugTools MagnumMeshTools MagnumGL MagnumOpenGLTester)

    if(CORRADE_TARGET_EMSCRIPTEN OR CORRADE_TARGET_ANDROID)
        set(MESHTOOLS_TEST_DIR ".")
    else()
        set(MESHTOOLS_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    if(NOT MAGNUM_BUILD_PLUGINS_STATIC)
        if(MAGNUM_WITH_ANYIMAGEIMPORTER)
            set(ANYIMAGEIMPORTER_PLUGIN_FILENAME $<TARGET_FILE:AnyImageImporter>)
        endif()
        if(MAGNUM_WITH_TGAIMPORTER)
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
            CompileTestFiles/phong-smooth.tga
            CompileTestFiles/phong-flat.tga
            CompileTestFiles/skinning.tga
            CompileTestFiles/textured2D.tga
            CompileTestFiles/textured3D.tga)
    target_include_directories(MeshToolsCompileGLTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
    if(MAGNUM_BUILD_PLUGINS_STATIC)
        if(MAGNUM_WITH_ANYIMAGEIMPORTER)
            target_link_libraries(MeshToolsCompileGLTest PRIVATE AnyImageImporter)
        endif()
        if(MAGNUM_WITH_TGAIMPORTER)
            target_link_libraries(MeshToolsCompileGLTest PRIVATE TgaImporter)
        endif()
    else()
        # So the plugins get properly built when building the test
        if(MAGNUM_WITH_ANYIMAGEIMPORTER)
            add_dependencies(MeshToolsCompileGLTest AnyImageImporter)
        endif()
        if(MAGNUM_WITH_TGAIMPORTER)
            add_dependencies(MeshToolsCompileGLTest TgaImporter)
        endif()
    endif()

    if(NOT MAGNUM_TARGET_GLES2)
        corrade_add_test(MeshToolsCompileLinesGLTest CompileLinesGLTest.cpp
            LIBRARIES
                MagnumDebugTools
                MagnumMeshToolsTestLib
                MagnumOpenGLTester
                MagnumShaders
            FILES
                CompileLinesTestFiles/bevel.tga
                CompileLinesTestFiles/line-primitive.tga
                CompileLinesTestFiles/miter.tga
                CompileLinesTestFiles/vertex-color.tga)
        target_include_directories(MeshToolsCompileLinesGLTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
        if(MAGNUM_BUILD_PLUGINS_STATIC)
            if(MAGNUM_WITH_ANYIMAGEIMPORTER)
                target_link_libraries(MeshToolsCompileLinesGLTest PRIVATE AnyImageImporter)
            endif()
            if(MAGNUM_WITH_TGAIMPORTER)
                target_link_libraries(MeshToolsCompileLinesGLTest PRIVATE TgaImporter)
            endif()
        else()
            # So the plugins get properly built when building the test
            if(MAGNUM_WITH_ANYIMAGEIMPORTER)
                add_dependencies(MeshToolsCompileLinesGLTest AnyImageImporter)
            endif()
            if(MAGNUM_WITH_TGAIMPORTER)
                add_dependencies(MeshToolsCompileLinesGLTest TgaImporter)
            endif()
        endif()
    endif()
endif()
