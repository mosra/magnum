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
set(CMAKE_FOLDER "Magnum/GL/Test")

corrade_add_test(GLAttributeTest AttributeTest.cpp LIBRARIES MagnumGLTestLib)
corrade_add_test(GLAbstractShaderProgramTest AbstractShaderProgramTest.cpp LIBRARIES MagnumGL)
corrade_add_test(GLBufferTest BufferTest.cpp LIBRARIES MagnumGL)
corrade_add_test(GLContextTest ContextTest.cpp LIBRARIES MagnumGL)
corrade_add_test(GLCubeMapTextureTest CubeMapTextureTest.cpp LIBRARIES MagnumGL)
corrade_add_test(GLDefaultFramebufferTest DefaultFramebufferTest.cpp LIBRARIES MagnumGL)
corrade_add_test(GLFramebufferTest FramebufferTest.cpp LIBRARIES MagnumGL)
corrade_add_test(GLMeshTest MeshTest.cpp LIBRARIES MagnumGLTestLib)
corrade_add_test(GLPixelFormatTest PixelFormatTest.cpp LIBRARIES MagnumGLTestLib)
corrade_add_test(GLRendererTest RendererTest.cpp LIBRARIES MagnumGL)
corrade_add_test(GLRenderbufferTest RenderbufferTest.cpp LIBRARIES MagnumGL)
corrade_add_test(GLSamplerTest SamplerTest.cpp LIBRARIES MagnumGLTestLib)
corrade_add_test(GLShaderTest ShaderTest.cpp LIBRARIES MagnumGL)
corrade_add_test(GLTextureTest TextureTest.cpp LIBRARIES MagnumGL)
corrade_add_test(GLTimeQueryTest TimeQueryTest.cpp LIBRARIES MagnumGL)
corrade_add_test(GLVersionTest VersionTest.cpp LIBRARIES MagnumGL)

if(NOT MAGNUM_TARGET_WEBGL)
    corrade_add_test(GLDebugOutputTest DebugOutputTest.cpp LIBRARIES MagnumGL)
endif()

if(NOT MAGNUM_TARGET_GLES2)
    corrade_add_test(GLBufferImageTest BufferImageTest.cpp LIBRARIES MagnumGL)
    corrade_add_test(GLPrimitiveQueryTest PrimitiveQueryTest.cpp LIBRARIES MagnumGL)
    corrade_add_test(GLTextureArrayTest TextureArrayTest.cpp LIBRARIES MagnumGL)
    corrade_add_test(GLTransformFeedbackTest TransformFeedbackTest.cpp LIBRARIES MagnumGL)
endif()

if(NOT MAGNUM_TARGET_GLES2 AND NOT MAGNUM_TARGET_WEBGL)
    corrade_add_test(GLBufferTextureTest BufferTextureTest.cpp LIBRARIES MagnumGL)
    corrade_add_test(GLCubeMapTextureArrayTest CubeMapTextureArrayTest.cpp LIBRARIES MagnumGL)
    corrade_add_test(GLMultisampleTextureTest MultisampleTextureTest.cpp LIBRARIES MagnumGL)
endif()

if(NOT (MAGNUM_TARGET_WEBGL AND MAGNUM_TARGET_GLES2))
    corrade_add_test(GLSampleQueryTest SampleQueryTest.cpp LIBRARIES MagnumGL)
endif()

if(NOT MAGNUM_TARGET_GLES)
    corrade_add_test(GLPipelineStatisticsQueryTest PipelineStatisticsQueryTest.cpp LIBRARIES MagnumGL)
    corrade_add_test(GLRectangleTextureTest RectangleTextureTest.cpp LIBRARIES MagnumGL)
endif()

if(MAGNUM_BUILD_GL_TESTS)
    # Otherwise CMake complains that Corrade::PluginManager is not found, wtf
    find_package(Corrade REQUIRED PluginManager)

    corrade_add_test(GLAbstractTextureGLTest AbstractTextureGLTest.cpp LIBRARIES MagnumOpenGLTesterTestLib)
    corrade_add_test(GLBufferGLTest BufferGLTest.cpp LIBRARIES MagnumOpenGLTester)
    corrade_add_test(GLCubeMapTextureGLTest CubeMapTextureGLTest.cpp LIBRARIES MagnumOpenGLTesterTestLib)
    corrade_add_test(GLFramebufferGLTest FramebufferGLTest.cpp LIBRARIES MagnumOpenGLTesterTestLib)
    corrade_add_test(GLMeshGLTest MeshGLTest.cpp LIBRARIES MagnumOpenGLTesterTestLib)
    corrade_add_test(GLRenderbufferGLTest RenderbufferGLTest.cpp LIBRARIES MagnumOpenGLTester)
    corrade_add_test(GLTextureGLTest TextureGLTest.cpp LIBRARIES MagnumOpenGLTesterTestLib)
    corrade_add_test(GLTimeQueryGLTest TimeQueryGLTest.cpp LIBRARIES MagnumOpenGLTester)

    corrade_add_resource(GLAbstractShaderProgramGLTest_RES AbstractShaderProgramGLTestFiles/resources.conf)
    corrade_add_test(GLAbstractShaderProgramGLTest
        AbstractShaderProgramGLTest.cpp
        ${GLAbstractShaderProgramGLTest_RES}
        LIBRARIES MagnumOpenGLTester)

    corrade_add_test(GLContextGLTest ContextGLTest.cpp LIBRARIES MagnumOpenGLTester)
    if(NOT CORRADE_TARGET_EMSCRIPTEN)
        set(THREADS_PREFER_PTHREAD_FLAG TRUE)
        find_package(Threads REQUIRED)
        target_link_libraries(GLContextGLTest PRIVATE Threads::Threads)
    endif()

    if(CORRADE_TARGET_EMSCRIPTEN OR CORRADE_TARGET_ANDROID)
        set(SHADERGLTEST_FILES_DIR "ShaderGLTestFiles")
        set(RENDERERGLTEST_FILES_DIR "RendererGLTestFiles")
    else()
        set(SHADERGLTEST_FILES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ShaderGLTestFiles)
        set(RENDERERGLTEST_FILES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/RendererGLTestFiles)
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

    corrade_add_test(GLRendererGLTest RendererGLTest.cpp
        LIBRARIES MagnumOpenGLTester MagnumDebugTools
        FILES RendererGLTestFiles/pointcoord.tga)
    target_include_directories(GLRendererGLTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
    if(MAGNUM_BUILD_PLUGINS_STATIC)
        if(MAGNUM_WITH_ANYIMAGEIMPORTER)
            target_link_libraries(GLRendererGLTest PRIVATE AnyImageImporter)
        endif()
        if(MAGNUM_WITH_TGAIMPORTER)
            target_link_libraries(GLRendererGLTest PRIVATE TgaImporter)
        endif()
    else()
        # So the plugins get properly built when building the test
        if(MAGNUM_WITH_ANYIMAGEIMPORTER)
            add_dependencies(GLRendererGLTest AnyImageImporter)
        endif()
        if(MAGNUM_WITH_TGAIMPORTER)
            add_dependencies(GLRendererGLTest TgaImporter)
        endif()
    endif()

    corrade_add_test(GLShaderGLTest ShaderGLTest.cpp
        LIBRARIES MagnumOpenGLTesterTestLib
        FILES ShaderGLTestFiles/shader.glsl)
    target_include_directories(GLShaderGLTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)

    if(NOT MAGNUM_TARGET_WEBGL)
        corrade_add_test(GLAbstractObjectGLTest AbstractObjectGLTest.cpp LIBRARIES MagnumOpenGLTester)
        corrade_add_test(GLDebugOutputGLTest DebugOutputGLTest.cpp LIBRARIES MagnumOpenGLTester)
    endif()

    if(NOT (MAGNUM_TARGET_WEBGL AND MAGNUM_TARGET_GLES2))
        corrade_add_test(GLAbstractQueryGLTest AbstractQueryGLTest.cpp LIBRARIES MagnumOpenGLTester)
        corrade_add_test(GLPixelStorageGLTest PixelStorageGLTest.cpp LIBRARIES MagnumOpenGLTester)
        corrade_add_test(GLSampleQueryGLTest SampleQueryGLTest.cpp LIBRARIES MagnumOpenGLTester)
    endif()

    if(NOT MAGNUM_TARGET_GLES2)
        corrade_add_test(GLBufferImageGLTest BufferImageGLTest.cpp LIBRARIES MagnumOpenGLTesterTestLib)
        corrade_add_test(GLPrimitiveQueryGLTest PrimitiveQueryGLTest.cpp LIBRARIES MagnumOpenGLTester)
        corrade_add_test(GLTextureArrayGLTest TextureArrayGLTest.cpp LIBRARIES MagnumOpenGLTester)
        corrade_add_test(GLTransformFeedbackGLTest TransformFeedbackGLTest.cpp LIBRARIES MagnumOpenGLTester)
    endif()

    if(NOT MAGNUM_TARGET_GLES2 AND NOT MAGNUM_TARGET_WEBGL)
        corrade_add_test(GLBufferTextureGLTest BufferTextureGLTest.cpp LIBRARIES MagnumOpenGLTester)
        corrade_add_test(GLCubeMapTextureArrayGLTest CubeMapTextureArrayGLTest.cpp LIBRARIES MagnumOpenGLTester)
        corrade_add_test(GLMultisampleTextureGLTest MultisampleTextureGLTest.cpp LIBRARIES MagnumOpenGLTester)
    endif()

    if(NOT MAGNUM_TARGET_GLES)
        corrade_add_test(GLPipelineStatisticsQueryGLTest PipelineStatisticsQueryGLTest.cpp LIBRARIES MagnumOpenGLTester)
        corrade_add_test(GLRectangleTextureGLTest RectangleTextureGLTest.cpp LIBRARIES MagnumOpenGLTester)
    endif()

    if(MAGNUM_BUILD_STATIC AND NOT CORRADE_TARGET_EMSCRIPTEN AND NOT CORRADE_TARGET_IOS AND NOT CORRADE_TARGET_ANDROID AND NOT CORRADE_TARGET_WINDOWS_RT)
        add_library(GLGlobalStateAcrossLibrariesLibrary SHARED
            GlobalStateAcrossLibrariesLibrary.cpp)
        target_link_libraries(GLGlobalStateAcrossLibrariesLibrary PUBLIC MagnumGL)

        corrade_add_test(GLGlobalStateAcrossLibrariesGLTest
            GlobalStateAcrossLibrariesGLTest.cpp
            LIBRARIES GLGlobalStateAcrossLibrariesLibrary MagnumOpenGLTester)
    endif()
endif()
