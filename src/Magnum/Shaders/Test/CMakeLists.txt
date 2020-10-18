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

corrade_add_test(ShadersDistanceFieldVectorTest DistanceFieldVectorTest.cpp LIBRARIES MagnumShaders)
corrade_add_test(ShadersFlatTest FlatTest.cpp LIBRARIES MagnumShaders)
corrade_add_test(ShadersGenericTest GenericTest.cpp LIBRARIES MagnumShaders)
corrade_add_test(ShadersMeshVisualizerTest MeshVisualizerTest.cpp LIBRARIES MagnumShaders)
corrade_add_test(ShadersPhongTest PhongTest.cpp LIBRARIES MagnumShaders)
corrade_add_test(ShadersVectorTest VectorTest.cpp LIBRARIES MagnumShaders)
corrade_add_test(ShadersVertexColorTest VertexColorTest.cpp LIBRARIES MagnumShaders)

set_target_properties(
    ShadersDistanceFieldVectorTest
    ShadersFlatTest
    ShadersMeshVisualizerTest
    ShadersPhongTest
    ShadersVectorTest
    ShadersVertexColorTest
    PROPERTIES FOLDER "Magnum/Shaders/Test")

if(BUILD_GL_TESTS)
    # Otherwise CMake complains that Corrade::PluginManager is not found, wtf
    find_package(Corrade REQUIRED PluginManager)

    if(CORRADE_TARGET_EMSCRIPTEN OR CORRADE_TARGET_ANDROID)
        set(SHADERS_TEST_DIR ".")
    else()
        set(SHADERS_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
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

    set(ShadersDistanceFieldVectorGLTest_SRCS DistanceFieldVectorGLTest.cpp)
    if(CORRADE_TARGET_IOS)
        list(APPEND ShadersDistanceFieldVectorGLTest_SRCS TestFiles VectorTestFiles)
    endif()
    corrade_add_test(ShadersDistanceFieldVectorGLTest ${ShadersDistanceFieldVectorGLTest_SRCS}
        LIBRARIES
            MagnumDebugTools
            MagnumMeshTools
            MagnumPrimitives
            MagnumShadersTestLib
            MagnumOpenGLTester
        FILES
            TestFiles/vector-distancefield.tga

            VectorTestFiles/defaults.tga
            VectorTestFiles/defaults-distancefield.tga
            VectorTestFiles/smooth0.1-2D.tga
            VectorTestFiles/smooth0.1-3D.tga
            VectorTestFiles/smooth0.2-2D.tga
            VectorTestFiles/smooth0.2-3D.tga
            VectorTestFiles/outline2D.tga
            VectorTestFiles/outline3D.tga)
    target_include_directories(ShadersDistanceFieldVectorGLTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
    if(BUILD_PLUGINS_STATIC)
        if(WITH_ANYIMAGEIMPORTER)
            target_link_libraries(ShadersDistanceFieldVectorGLTest PRIVATE AnyImageImporter)
        endif()
        if(WITH_TGAIMPORTER)
            target_link_libraries(ShadersDistanceFieldVectorGLTest PRIVATE TgaImporter)
        endif()
    endif()

    set(ShadersFlatGLTest_SRCS FlatGLTest.cpp)
    if(CORRADE_TARGET_IOS)
        list(APPEND ShadersFlatGLTest_SRCS TestFiles FlatTestFiles)
    endif()
    corrade_add_test(ShadersFlatGLTest ${ShadersFlatGLTest_SRCS}
        LIBRARIES
            MagnumDebugTools
            MagnumMeshTools
            MagnumPrimitives
            MagnumShadersTestLib
            MagnumOpenGLTester
        FILES
            TestFiles/diffuse-alpha-texture.tga
            TestFiles/diffuse-texture.tga
            TestFiles/alpha-mask1.0.tga

            FlatTestFiles/colored2D.tga
            FlatTestFiles/colored3D.tga
            FlatTestFiles/defaults.tga
            FlatTestFiles/instanced2D.tga
            FlatTestFiles/instanced3D.tga
            FlatTestFiles/textured2D.tga
            FlatTestFiles/textured3D.tga
            FlatTestFiles/textured2D-alpha.tga
            FlatTestFiles/textured3D-alpha.tga
            FlatTestFiles/textured2D-alpha-mask0.5.tga
            FlatTestFiles/textured3D-alpha-mask0.5.tga
            FlatTestFiles/vertexColor2D.tga
            FlatTestFiles/vertexColor3D.tga)
    target_include_directories(ShadersFlatGLTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
    if(BUILD_PLUGINS_STATIC)
        if(WITH_ANYIMAGEIMPORTER)
            target_link_libraries(ShadersFlatGLTest PRIVATE AnyImageImporter)
        endif()
        if(WITH_TGAIMPORTER)
            target_link_libraries(ShadersFlatGLTest PRIVATE TgaImporter)
        endif()
    endif()

    set(ShadersMeshVisualizerGLTest_SRCS MeshVisualizerGLTest.cpp)
    if(CORRADE_TARGET_IOS)
        list(APPEND ShadersMeshVisualizerGLTest_SRCS FlatTestFiles MeshVisualizerTestFiles)
    endif()
    corrade_add_test(ShadersMeshVisualizerGLTest ${ShadersMeshVisualizerGLTest_SRCS}
        LIBRARIES
            MagnumDebugTools
            MagnumMeshTools
            MagnumPrimitives
            MagnumShadersTestLib
            MagnumOpenGLTester
        FILES
            MeshVisualizerTestFiles/bitangents-from-tangents.tga
            MeshVisualizerTestFiles/defaults-objectid2D.tga
            MeshVisualizerTestFiles/defaults-objectid3D.tga
            MeshVisualizerTestFiles/defaults-primitiveid2D.tga
            MeshVisualizerTestFiles/defaults-primitiveid3D.tga
            MeshVisualizerTestFiles/defaults-tbn.tga
            MeshVisualizerTestFiles/defaults-vertexid2D.tga
            MeshVisualizerTestFiles/defaults-vertexid3D.tga
            MeshVisualizerTestFiles/defaults-wireframe2D.tga
            MeshVisualizerTestFiles/defaults-wireframe3D.tga
            MeshVisualizerTestFiles/objectid2D.tga
            MeshVisualizerTestFiles/objectid3D.tga
            MeshVisualizerTestFiles/primitiveid-tn.tga
            MeshVisualizerTestFiles/primitiveid2D.tga
            MeshVisualizerTestFiles/primitiveid3D.tga
            MeshVisualizerTestFiles/tbn-wide.tga
            MeshVisualizerTestFiles/tbn.tga
            MeshVisualizerTestFiles/vertexid2D.tga
            MeshVisualizerTestFiles/vertexid3D.tga
            MeshVisualizerTestFiles/wireframe-nogeo2D.tga
            MeshVisualizerTestFiles/wireframe-nogeo3D.tga
            MeshVisualizerTestFiles/wireframe-objectid2D.tga
            MeshVisualizerTestFiles/wireframe-objectid3D.tga
            MeshVisualizerTestFiles/wireframe-nogeo-objectid2D.tga
            MeshVisualizerTestFiles/wireframe-nogeo-objectid3D.tga
            MeshVisualizerTestFiles/wireframe-perspective.tga
            MeshVisualizerTestFiles/wireframe-primitiveid-tn.tga
            MeshVisualizerTestFiles/wireframe-tn-smooth.tga
            MeshVisualizerTestFiles/wireframe-vertexid2D.tga
            MeshVisualizerTestFiles/wireframe-vertexid3D.tga
            MeshVisualizerTestFiles/wireframe-wide2D.tga
            MeshVisualizerTestFiles/wireframe-wide3D.tga
            MeshVisualizerTestFiles/wireframe2D.tga
            MeshVisualizerTestFiles/wireframe3D.tga)
    target_include_directories(ShadersMeshVisualizerGLTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
    if(BUILD_PLUGINS_STATIC)
        if(WITH_ANYIMAGEIMPORTER)
            target_link_libraries(ShadersMeshVisualizerGLTest PRIVATE AnyImageImporter)
        endif()
        if(WITH_TGAIMPORTER)
            target_link_libraries(ShadersMeshVisualizerGLTest PRIVATE TgaImporter)
        endif()
    endif()

    set(ShadersPhongGLTest_SRCS PhongGLTest.cpp)
    if(CORRADE_TARGET_IOS)
        list(APPEND ShadersPhongGLTest_SRCS TestFiles PhongTestFiles FlatTestFiles)
    endif()
    corrade_add_test(ShadersPhongGLTest ${ShadersPhongGLTest_SRCS}
        LIBRARIES
            MagnumDebugTools
            MagnumMeshTools
            MagnumPrimitives
            MagnumShadersTestLib
            MagnumOpenGLTester
        FILES
            TestFiles/alpha-texture.tga
            TestFiles/ambient-texture.tga
            TestFiles/diffuse-alpha-texture.tga
            TestFiles/diffuse-texture.tga
            TestFiles/normal-texture.tga
            TestFiles/specular-texture.tga
            TestFiles/alpha-mask1.0.tga

            PhongTestFiles/colored.tga
            PhongTestFiles/defaults.tga
            PhongTestFiles/instanced.tga
            PhongTestFiles/instanced-normal.tga
            PhongTestFiles/low-light-angle.tga
            PhongTestFiles/shininess-black-specular.tga
            PhongTestFiles/shininess0-overflow.tga
            PhongTestFiles/shininess0.tga
            PhongTestFiles/shininess10.tga
            PhongTestFiles/shininess80.tga
            PhongTestFiles/textured-ambient.tga
            PhongTestFiles/textured-diffuse-alpha.tga
            PhongTestFiles/textured-diffuse-alpha-mask0.5.tga
            PhongTestFiles/textured-diffuse-alpha.tga
            PhongTestFiles/textured-diffuse.tga
            PhongTestFiles/textured-diffuse-transformed.tga
            PhongTestFiles/textured-normal.tga
            PhongTestFiles/textured-normal-left.tga
            PhongTestFiles/textured-normal0.0.tga
            PhongTestFiles/textured-normal0.5.tga
            PhongTestFiles/textured-specular.tga
            PhongTestFiles/textured.tga
            PhongTestFiles/vertexColor.tga
            PhongTestFiles/light-directional-intensity2.tga
            PhongTestFiles/light-directional.tga
            PhongTestFiles/light-none.tga
            PhongTestFiles/light-point-attenuated-specular.tga
            PhongTestFiles/light-point-intensity10-range1.0.tga
            PhongTestFiles/light-point-range1.5.tga
            PhongTestFiles/light-point-specular-color.tga
            PhongTestFiles/light-point.tga

            # For zero lights test (equivalency to Flat3D)
            FlatTestFiles/textured3D-alpha-mask0.5.tga)
    target_include_directories(ShadersPhongGLTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
    if(BUILD_PLUGINS_STATIC)
        if(WITH_ANYIMAGEIMPORTER)
            target_link_libraries(ShadersPhongGLTest PRIVATE AnyImageImporter)
        endif()
        if(WITH_TGAIMPORTER)
            target_link_libraries(ShadersPhongGLTest PRIVATE TgaImporter)
        endif()
    endif()

    set(ShadersVectorGLTest_SRCS VectorGLTest.cpp)
    if(CORRADE_TARGET_IOS)
        list(APPEND ShadersVectorGLTest_SRCS TestFiles VectorTestFiles)
    endif()
    corrade_add_test(ShadersVectorGLTest ${ShadersVectorGLTest_SRCS}
        LIBRARIES
            MagnumDebugTools
            MagnumMeshTools
            MagnumPrimitives
            MagnumShadersTestLib
            MagnumOpenGLTester
        FILES
            TestFiles/vector.tga

            VectorTestFiles/defaults.tga
            VectorTestFiles/vector2D.tga
            VectorTestFiles/vector3D.tga)
    target_include_directories(ShadersVectorGLTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
    if(BUILD_PLUGINS_STATIC)
        if(WITH_ANYIMAGEIMPORTER)
            target_link_libraries(ShadersVectorGLTest PRIVATE AnyImageImporter)
        endif()
        if(WITH_TGAIMPORTER)
            target_link_libraries(ShadersVectorGLTest PRIVATE TgaImporter)
        endif()
    endif()

    set(ShadersVertexColorGLTest_SRCS VertexColorGLTest.cpp)
    if(CORRADE_TARGET_IOS)
        list(APPEND ShadersVertexColorGLTest_SRCS FlatTestFiles VertexColorTestFiles)
    endif()
    corrade_add_test(ShadersVertexColorGLTest ${ShadersVertexColorGLTest_SRCS}
        LIBRARIES
            MagnumDebugTools
            MagnumMeshTools
            MagnumPrimitives
            MagnumShadersTestLib
            MagnumOpenGLTester
        FILES
            FlatTestFiles/defaults.tga

            VertexColorTestFiles/vertexColor2D.tga
            VertexColorTestFiles/vertexColor3D.tga)
    target_include_directories(ShadersVertexColorGLTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
    if(BUILD_PLUGINS_STATIC)
        if(WITH_ANYIMAGEIMPORTER)
            target_link_libraries(ShadersVertexColorGLTest PRIVATE AnyImageImporter)
        endif()
        if(WITH_TGAIMPORTER)
            target_link_libraries(ShadersVertexColorGLTest PRIVATE TgaImporter)
        endif()
    endif()

    if(CORRADE_TARGET_IOS)
        set_source_files_properties(
            TestFiles
            VectorTestFiles
            FlatTestFiles
            MeshVisualizerTestFiles
            PhongTestFiles
            VectorTestFiles
            VertexColorTestFiles
            PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    endif()

    set_target_properties(
        ShadersDistanceFieldVectorGLTest
        ShadersFlatGLTest
        ShadersMeshVisualizerGLTest
        ShadersPhongGLTest
        ShadersVectorGLTest
        ShadersVertexColorGLTest
        PROPERTIES FOLDER "Magnum/Shaders/Test")
endif()
