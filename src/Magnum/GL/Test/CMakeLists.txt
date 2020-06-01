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

set_target_properties(
    GLAttributeTest
    GLAbstractShaderProgramTest
    GLBufferTest
    GLContextTest
    GLCubeMapTextureTest
    GLDefaultFramebufferTest
    GLFramebufferTest
    GLMeshTest
    GLPixelFormatTest
    GLRendererTest
    GLRenderbufferTest
    GLSamplerTest
    GLShaderTest
    GLTextureTest
    GLTimeQueryTest
    GLVersionTest
    PROPERTIES FOLDER "Magnum/GL/Test")

if(NOT MAGNUM_TARGET_WEBGL)
    corrade_add_test(GLDebugOutputTest DebugOutputTest.cpp LIBRARIES MagnumGL)
    set_target_properties(GLDebugOutputTest PROPERTIES FOLDER "Magnum/GL/Test")
endif()

if(NOT MAGNUM_TARGET_GLES2)
    corrade_add_test(GLBufferImageTest BufferImageTest.cpp LIBRARIES MagnumGL)
    corrade_add_test(GLPrimitiveQueryTest PrimitiveQueryTest.cpp LIBRARIES MagnumGL)
    corrade_add_test(GLTextureArrayTest TextureArrayTest.cpp LIBRARIES MagnumGL)
    corrade_add_test(GLTransformFeedbackTest TransformFeedbackTest.cpp LIBRARIES MagnumGL)

    set_target_properties(
        GLBufferImageTest
        GLPrimitiveQueryTest
        GLTextureArrayTest
        GLTransformFeedbackTest
        PROPERTIES FOLDER "Magnum/GL/Test")
endif()

if(NOT MAGNUM_TARGET_GLES2 AND NOT MAGNUM_TARGET_WEBGL)
    corrade_add_test(GLBufferTextureTest BufferTextureTest.cpp LIBRARIES MagnumGL)
    corrade_add_test(GLCubeMapTextureArrayTest CubeMapTextureArrayTest.cpp LIBRARIES MagnumGL)
    corrade_add_test(GLMultisampleTextureTest MultisampleTextureTest.cpp LIBRARIES MagnumGL)

    set_target_properties(
        GLBufferTextureTest
        GLCubeMapTextureArrayTest
        GLMultisampleTextureTest
        PROPERTIES FOLDER "Magnum/GL/Test")
endif()

if(NOT (MAGNUM_TARGET_WEBGL AND MAGNUM_TARGET_GLES2))
    corrade_add_test(GLSampleQueryTest SampleQueryTest.cpp LIBRARIES MagnumGL)
    set_target_properties(GLSampleQueryTest PROPERTIES FOLDER "Magnum/GL/Test")
endif()

if(NOT MAGNUM_TARGET_GLES)
    corrade_add_test(GLPipelineStatisticsQueryTest PipelineStatisticsQueryTest.cpp LIBRARIES MagnumGL)
    corrade_add_test(GLRectangleTextureTest RectangleTextureTest.cpp LIBRARIES MagnumGL)
    set_target_properties(
        GLPipelineStatisticsQueryTest
        GLRectangleTextureTest
        PROPERTIES FOLDER "Magnum/GL/Test")
endif()

if(BUILD_GL_TESTS)
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

    corrade_add_test(GLRendererGLTest RendererGLTest.cpp
        LIBRARIES MagnumOpenGLTester MagnumDebugTools
        FILES RendererGLTestFiles/pointcoord.tga)
    target_include_directories(GLRendererGLTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
    if(BUILD_PLUGINS_STATIC)
        if(WITH_ANYIMAGEIMPORTER)
            target_link_libraries(GLRendererGLTest PRIVATE AnyImageImporter)
        endif()
        if(WITH_TGAIMPORTER)
            target_link_libraries(GLRendererGLTest PRIVATE TgaImporter)
        endif()
    endif()

    corrade_add_test(GLShaderGLTest ShaderGLTest.cpp
        LIBRARIES MagnumOpenGLTester
        FILES ShaderGLTestFiles/shader.glsl)
    target_include_directories(GLShaderGLTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)

    set_target_properties(
        GLAbstractTextureGLTest
        GLBufferGLTest
        GLContextGLTest
        GLCubeMapTextureGLTest
        GLFramebufferGLTest
        GLMeshGLTest
        GLRenderbufferGLTest
        GLTextureGLTest
        GLTimeQueryGLTest

        GLAbstractShaderProgramGLTest
        GLAbstractShaderProgramGLTest_RES-dependencies

        GLShaderGLTest
        PROPERTIES FOLDER "Magnum/GL/Test")

    if(NOT MAGNUM_TARGET_WEBGL)
        corrade_add_test(GLAbstractObjectGLTest AbstractObjectGLTest.cpp LIBRARIES MagnumOpenGLTester)
        corrade_add_test(GLDebugOutputGLTest DebugOutputGLTest.cpp LIBRARIES MagnumOpenGLTester)

        set_target_properties(
            GLAbstractObjectGLTest
            GLDebugOutputGLTest
            PROPERTIES FOLDER "Magnum/GL/Test")
    endif()

    if(NOT (MAGNUM_TARGET_WEBGL AND MAGNUM_TARGET_GLES2))
        corrade_add_test(GLAbstractQueryGLTest AbstractQueryGLTest.cpp LIBRARIES MagnumOpenGLTester)
        corrade_add_test(GLPixelStorageGLTest PixelStorageGLTest.cpp LIBRARIES MagnumOpenGLTester)
        corrade_add_test(GLSampleQueryGLTest SampleQueryGLTest.cpp LIBRARIES MagnumOpenGLTester)

        set_target_properties(
            GLAbstractQueryGLTest
            GLPixelStorageGLTest
            GLSampleQueryGLTest
            PROPERTIES FOLDER "Magnum/GL/Test")
    endif()

    if(NOT MAGNUM_TARGET_GLES2)
        corrade_add_test(GLBufferImageGLTest BufferImageGLTest.cpp LIBRARIES MagnumOpenGLTesterTestLib)
        corrade_add_test(GLPrimitiveQueryGLTest PrimitiveQueryGLTest.cpp LIBRARIES MagnumOpenGLTester)
        corrade_add_test(GLTextureArrayGLTest TextureArrayGLTest.cpp LIBRARIES MagnumOpenGLTester)
        corrade_add_test(GLTransformFeedbackGLTest TransformFeedbackGLTest.cpp LIBRARIES MagnumOpenGLTester)

        set_target_properties(
            GLBufferImageGLTest
            GLPrimitiveQueryGLTest
            GLTextureArrayGLTest
            GLTransformFeedbackGLTest
            PROPERTIES FOLDER "Magnum/GL/Test")
    endif()

    if(NOT MAGNUM_TARGET_GLES2 AND NOT MAGNUM_TARGET_WEBGL)
        corrade_add_test(GLBufferTextureGLTest BufferTextureGLTest.cpp LIBRARIES MagnumOpenGLTester)
        corrade_add_test(GLCubeMapTextureArrayGLTest CubeMapTextureArrayGLTest.cpp LIBRARIES MagnumOpenGLTester)
        corrade_add_test(GLMultisampleTextureGLTest MultisampleTextureGLTest.cpp LIBRARIES MagnumOpenGLTester)

        set_target_properties(
            GLBufferTextureGLTest
            GLCubeMapTextureArrayGLTest
            GLMultisampleTextureGLTest
            PROPERTIES FOLDER "Magnum/GL/Test")
    endif()

    if(NOT MAGNUM_TARGET_GLES)
        corrade_add_test(GLPipelineStatisticsQueryGLTest PipelineStatisticsQueryGLTest.cpp LIBRARIES MagnumOpenGLTester)
        corrade_add_test(GLRectangleTextureGLTest RectangleTextureGLTest.cpp LIBRARIES MagnumOpenGLTester)
        set_target_properties(
            GLPipelineStatisticsQueryGLTest
            GLRectangleTextureGLTest
            PROPERTIES FOLDER "Magnum/GL/Test")
    endif()

    if(MAGNUM_BUILD_STATIC AND NOT CORRADE_TARGET_EMSCRIPTEN AND NOT CORRADE_TARGET_IOS AND NOT CORRADE_TARGET_ANDROID AND NOT CORRADE_TARGET_WINDOWS_RT)
        add_library(GLGlobalStateAcrossLibrariesLibrary SHARED
            GlobalStateAcrossLibrariesLibrary.cpp)
        target_link_libraries(GLGlobalStateAcrossLibrariesLibrary PUBLIC MagnumGL)

        corrade_add_test(GLGlobalStateAcrossLibrariesGLTest
            GlobalStateAcrossLibrariesGLTest.cpp
            LIBRARIES GLGlobalStateAcrossLibrariesLibrary MagnumOpenGLTester)
        set_target_properties(
            GLGlobalStateAcrossLibrariesLibrary
            GLGlobalStateAcrossLibrariesGLTest
            PROPERTIES FOLDER "Magnum/GL/Test")
    endif()
endif()
