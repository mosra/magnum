#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

corrade_add_test(ShadersDistanceFieldVectorTest DistanceFieldVectorTest.cpp LIBRARIES MagnumShaders)
corrade_add_test(ShadersFlatTest FlatTest.cpp LIBRARIES MagnumShaders)
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
    # that. Should be revisited when updating Travis to newer Xcode (current
    # has CMake 3.6).
    if(NOT BUILD_PLUGINS_STATIC)
        if(WITH_ANYIMAGEIMPORTER)
            set(ANYIMAGEIMPORTER_PLUGIN_FILENAME $<TARGET_FILE:AnyImageImporter>)
        endif()
        if(WITH_TGAIMPORTER)
            set(TGAIMPORTER_PLUGIN_FILENAME $<TARGET_FILE:TgaImporter>)
        endif()

        # First replace ${} variables, then $<> generator expressions
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
                       ${CMAKE_CURRENT_BINARY_DIR}/configure.h.in)
        file(GENERATE OUTPUT $<TARGET_FILE_DIR:ShadersPhongGLTest>/configure.h
            INPUT ${CMAKE_CURRENT_BINARY_DIR}/configure.h.in)
    else()
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
                       ${CMAKE_CURRENT_BINARY_DIR}/configure.h)
    endif()

    corrade_add_test(ShadersDistanceFieldVectorGLTest DistanceFieldVectorGLTest.cpp LIBRARIES MagnumShaders MagnumOpenGLTester)

    corrade_add_test(ShadersFlatGLTest FlatGLTest.cpp
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
            FlatTestFiles/textured2D.tga
            FlatTestFiles/textured3D.tga
            FlatTestFiles/textured2D-alpha.tga
            FlatTestFiles/textured3D-alpha.tga
            FlatTestFiles/textured2D-alpha-mask0.5.tga
            FlatTestFiles/textured3D-alpha-mask0.5.tga)
    if(NOT BUILD_PLUGINS_STATIC)
        target_include_directories(ShadersFlatGLTest PRIVATE $<TARGET_FILE_DIR:ShadersPhongGLTest>)
    else()
        target_include_directories(ShadersFlatGLTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
        if(WITH_ANYIMAGEIMPORTER)
            target_link_libraries(ShadersFlatGLTest PRIVATE AnyImageImporter)
        endif()
        if(WITH_TGAIMPORTER)
            target_link_libraries(ShadersFlatGLTest PRIVATE TgaImporter)
        endif()
    endif()

    corrade_add_test(ShadersMeshVisualizerGLTest MeshVisualizerGLTest.cpp LIBRARIES MagnumShaders MagnumOpenGLTester)

    corrade_add_test(ShadersPhongGLTest PhongGLTest.cpp
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
            PhongTestFiles/shininess-black-specular.tga
            PhongTestFiles/shininess0.tga
            PhongTestFiles/shininess10.tga
            PhongTestFiles/shininess80.tga
            PhongTestFiles/textured-ambient.tga
            PhongTestFiles/textured-diffuse-alpha.tga
            PhongTestFiles/textured-diffuse-alpha-mask0.5.tga
            PhongTestFiles/textured-diffuse-alpha.tga
            PhongTestFiles/textured-diffuse.tga
            PhongTestFiles/textured-normal.tga
            PhongTestFiles/textured-specular.tga
            PhongTestFiles/textured.tga)
    if(NOT BUILD_PLUGINS_STATIC)
        target_include_directories(ShadersPhongGLTest PRIVATE $<TARGET_FILE_DIR:ShadersPhongGLTest>)
    else()
        target_include_directories(ShadersPhongGLTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
        if(WITH_ANYIMAGEIMPORTER)
            target_link_libraries(ShadersPhongGLTest PRIVATE AnyImageImporter)
        endif()
        if(WITH_TGAIMPORTER)
            target_link_libraries(ShadersPhongGLTest PRIVATE TgaImporter)
        endif()
    endif()

    corrade_add_test(ShadersVectorGLTest VectorGLTest.cpp LIBRARIES MagnumShaders MagnumOpenGLTester)
    corrade_add_test(ShadersVertexColorGLTest VertexColorGLTest.cpp LIBRARIES MagnumShaders MagnumOpenGLTester)

    set_target_properties(
        ShadersDistanceFieldVectorGLTest
        ShadersFlatGLTest
        ShadersMeshVisualizerGLTest
        ShadersPhongGLTest
        ShadersVectorGLTest
        ShadersVertexColorGLTest
        PROPERTIES FOLDER "Magnum/Shaders/Test")
endif()
