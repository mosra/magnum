#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

corrade_add_test(ArrayTest ArrayTest.cpp LIBRARIES Magnum)
corrade_add_test(AttributeTest AttributeTest.cpp LIBRARIES Magnum)
corrade_add_test(AbstractShaderProgramTest AbstractShaderProgramTest.cpp LIBRARIES Magnum)
corrade_add_test(BufferTest BufferTest.cpp LIBRARIES Magnum)
corrade_add_test(FormatTest FormatTest.cpp LIBRARIES Magnum)
corrade_add_test(ContextTest ContextTest.cpp LIBRARIES Magnum)
corrade_add_test(CubeMapTextureTest CubeMapTextureTest.cpp LIBRARIES Magnum)
corrade_add_test(DefaultFramebufferTest DefaultFramebufferTest.cpp LIBRARIES Magnum)
corrade_add_test(FramebufferTest FramebufferTest.cpp LIBRARIES Magnum)
corrade_add_test(ImageTest ImageTest.cpp LIBRARIES Magnum)
corrade_add_test(ImageViewTest ImageViewTest.cpp LIBRARIES Magnum)
corrade_add_test(MeshTest MeshTest.cpp LIBRARIES Magnum)
corrade_add_test(PixelStorageTest PixelStorageTest.cpp LIBRARIES Magnum)
corrade_add_test(RendererTest RendererTest.cpp LIBRARIES Magnum)
corrade_add_test(RenderbufferTest RenderbufferTest.cpp LIBRARIES Magnum)
corrade_add_test(ResourceManagerTest ResourceManagerTest.cpp LIBRARIES Magnum)
target_compile_definitions(ResourceManagerTest PRIVATE "CORRADE_GRACEFUL_ASSERT")
corrade_add_test(SamplerTest SamplerTest.cpp LIBRARIES Magnum)
corrade_add_test(ShaderTest ShaderTest.cpp LIBRARIES Magnum)
corrade_add_test(TagsTest TagsTest.cpp LIBRARIES Magnum)
corrade_add_test(TextureTest TextureTest.cpp LIBRARIES Magnum)
corrade_add_test(VersionTest VersionTest.cpp LIBRARIES Magnum)

set_target_properties(
    ArrayTest
    AttributeTest
    AbstractShaderProgramTest
    BufferTest
    FormatTest
    ContextTest
    CubeMapTextureTest
    DefaultFramebufferTest
    FramebufferTest
    ImageTest
    ImageViewTest
    MeshTest
    PixelStorageTest
    RendererTest
    RenderbufferTest
    ResourceManagerTest
    SamplerTest
    ShaderTest
    TagsTest
    TextureTest
    VersionTest
    PROPERTIES FOLDER "Magnum/Test")

if(NOT MAGNUM_TARGET_WEBGL)
    corrade_add_test(DebugOutputTest DebugOutputTest.cpp LIBRARIES Magnum)
    set_target_properties(DebugOutputTest PROPERTIES FOLDER "Magnum/Test")
endif()

if(NOT MAGNUM_TARGET_GLES2)
    corrade_add_test(BufferImageTest BufferImageTest.cpp LIBRARIES Magnum)
    corrade_add_test(PrimitiveQueryTest PrimitiveQueryTest.cpp LIBRARIES Magnum)
    corrade_add_test(TextureArrayTest TextureArrayTest.cpp LIBRARIES Magnum)
    corrade_add_test(TransformFeedbackTest TransformFeedbackTest.cpp LIBRARIES Magnum)

    set_target_properties(
        BufferImageTest
        PrimitiveQueryTest
        TextureArrayTest
        TransformFeedbackTest
        PROPERTIES FOLDER "Magnum/Test")
endif()

if(NOT MAGNUM_TARGET_GLES2 AND NOT MAGNUM_TARGET_WEBGL)
    corrade_add_test(BufferTextureTest BufferTextureTest.cpp LIBRARIES Magnum)
    corrade_add_test(CubeMapTextureArrayTest CubeMapTextureArrayTest.cpp LIBRARIES Magnum)
    corrade_add_test(MultisampleTextureTest MultisampleTextureTest.cpp LIBRARIES Magnum)

    set_target_properties(
        BufferTextureTest
        CubeMapTextureArrayTest
        MultisampleTextureTest
        PROPERTIES FOLDER "Magnum/Test")
endif()

if(NOT (MAGNUM_TARGET_WEBGL AND MAGNUM_TARGET_GLES2))
    corrade_add_test(SampleQueryTest SampleQueryTest.cpp LIBRARIES Magnum)
    set_target_properties(SampleQueryTest PROPERTIES FOLDER "Magnum/Test")
endif()

if(NOT MAGNUM_TARGET_WEBGL)
    corrade_add_test(TimeQueryTest TimeQueryTest.cpp LIBRARIES Magnum)
    set_target_properties(TimeQueryTest PROPERTIES FOLDER "Magnum/Test")
endif()

if(NOT MAGNUM_TARGET_GLES)
    corrade_add_test(RectangleTextureTest RectangleTextureTest.cpp LIBRARIES Magnum)
    set_target_properties(RectangleTextureTest PROPERTIES FOLDER "Magnum/Test")
endif()

add_library(ResourceManagerLocalInstanceTestLib ${SHARED_OR_STATIC} ResourceManagerLocalInstanceTestLib.cpp)
target_link_libraries(ResourceManagerLocalInstanceTestLib Magnum)
if(NOT BUILD_STATIC)
    target_compile_definitions(ResourceManagerLocalInstanceTestLib PRIVATE "ResourceManagerLocalInstanceTestLib_EXPORTS")
endif()
corrade_add_test(ResourceManagerLocalInstanceTest ResourceManagerLocalInstanceTest.cpp LIBRARIES Magnum ResourceManagerLocalInstanceTestLib)
set_target_properties(
    ResourceManagerLocalInstanceTestLib
    ResourceManagerLocalInstanceTest
    PROPERTIES FOLDER "Magnum/Test")

if(BUILD_GL_TESTS)
    corrade_add_test(AbstractObjectGLTest AbstractObjectGLTest.cpp LIBRARIES MagnumOpenGLTester)
    corrade_add_test(AbstractQueryGLTest AbstractQueryGLTest.cpp LIBRARIES MagnumOpenGLTester)
    corrade_add_test(AbstractTextureGLTest AbstractTextureGLTest.cpp LIBRARIES MagnumOpenGLTester)
    corrade_add_test(BufferGLTest BufferGLTest.cpp LIBRARIES MagnumOpenGLTester)
    corrade_add_test(ContextGLTest ContextGLTest.cpp LIBRARIES MagnumOpenGLTester)
    corrade_add_test(CubeMapTextureGLTest CubeMapTextureGLTest.cpp LIBRARIES MagnumOpenGLTester)
    corrade_add_test(DebugOutputGLTest DebugOutputGLTest.cpp LIBRARIES MagnumOpenGLTester)
    corrade_add_test(FramebufferGLTest FramebufferGLTest.cpp LIBRARIES MagnumOpenGLTester)
    corrade_add_test(MeshGLTest MeshGLTest.cpp LIBRARIES MagnumOpenGLTester)
    corrade_add_test(PixelStorageGLTest PixelStorageGLTest.cpp LIBRARIES MagnumOpenGLTester)
    corrade_add_test(RenderbufferGLTest RenderbufferGLTest.cpp LIBRARIES MagnumOpenGLTester)
    corrade_add_test(SampleQueryGLTest SampleQueryGLTest.cpp LIBRARIES MagnumOpenGLTester)
    corrade_add_test(TextureGLTest TextureGLTest.cpp LIBRARIES MagnumOpenGLTester)
    corrade_add_test(TimeQueryGLTest TimeQueryGLTest.cpp LIBRARIES MagnumOpenGLTester)

    corrade_add_resource(AbstractShaderProgramGLTest_RES AbstractShaderProgramGLTestFiles/resources.conf)
    corrade_add_test(AbstractShaderProgramGLTest
        AbstractShaderProgramGLTest.cpp
        ${AbstractShaderProgramGLTest_RES}
        LIBRARIES MagnumOpenGLTester)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
                   ${CMAKE_CURRENT_BINARY_DIR}/configure.h)
    corrade_add_test(ShaderGLTest ShaderGLTest.cpp LIBRARIES MagnumOpenGLTester)
    target_include_directories(ShaderGLTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

    set_target_properties(
        AbstractObjectGLTest
        AbstractQueryGLTest
        AbstractTextureGLTest
        BufferGLTest
        ContextGLTest
        CubeMapTextureGLTest
        DebugOutputGLTest
        FramebufferGLTest
        MeshGLTest
        PixelStorageGLTest
        RenderbufferGLTest
        SampleQueryGLTest
        TextureGLTest
        TimeQueryGLTest

        AbstractShaderProgramGLTest
        AbstractShaderProgramGLTest_RES-dependencies

        ShaderGLTest
        PROPERTIES FOLDER "Magnum/Test")

    if(NOT MAGNUM_TARGET_GLES2)
        corrade_add_test(BufferImageGLTest BufferImageGLTest.cpp LIBRARIES MagnumOpenGLTester)
        corrade_add_test(BufferTextureGLTest BufferTextureGLTest.cpp LIBRARIES MagnumOpenGLTester)
        corrade_add_test(CubeMapTextureArrayGLTest CubeMapTextureArrayGLTest.cpp LIBRARIES MagnumOpenGLTester)
        corrade_add_test(MultisampleTextureGLTest MultisampleTextureGLTest.cpp LIBRARIES MagnumOpenGLTester)
        corrade_add_test(PrimitiveQueryGLTest PrimitiveQueryGLTest.cpp LIBRARIES MagnumOpenGLTester)
        corrade_add_test(TextureArrayGLTest TextureArrayGLTest.cpp LIBRARIES MagnumOpenGLTester)
        corrade_add_test(TransformFeedbackGLTest TransformFeedbackGLTest.cpp LIBRARIES MagnumOpenGLTester)

        set_target_properties(
            BufferImageGLTest
            BufferTextureGLTest
            CubeMapTextureArrayGLTest
            MultisampleTextureGLTest
            PrimitiveQueryGLTest
            TextureArrayGLTest
            TransformFeedbackGLTest
            PROPERTIES FOLDER "Magnum/Test")
    endif()

    if(NOT MAGNUM_TARGET_GLES)
        corrade_add_test(RectangleTextureGLTest RectangleTextureGLTest.cpp LIBRARIES MagnumOpenGLTester)
        set_target_properties(RectangleTextureGLTest PROPERTIES FOLDER "Magnum/Test")
    endif()
endif()
