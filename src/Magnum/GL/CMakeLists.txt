#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

set(MagnumGL_SRCS
    AbstractFramebuffer.cpp
    AbstractObject.cpp
    AbstractTexture.cpp
    AbstractShaderProgram.cpp
    Attribute.cpp
    Buffer.cpp
    CubeMapTexture.cpp
    Context.cpp
    DefaultFramebuffer.cpp
    Framebuffer.cpp
    ../Image.cpp # temporary
    ../ImageView.cpp # temporary
    Mesh.cpp
    MeshView.cpp
    OpenGL.cpp
    PixelFormat.cpp
    Renderbuffer.cpp
    Renderer.cpp
    Sampler.cpp
    Shader.cpp
    Texture.cpp
    Version.cpp

    Implementation/BufferState.cpp
    Implementation/ContextState.cpp
    Implementation/FramebufferState.cpp
    Implementation/MeshState.cpp
    Implementation/RendererState.cpp
    Implementation/ShaderProgramState.cpp
    Implementation/ShaderState.cpp
    Implementation/State.cpp
    Implementation/TextureState.cpp
    Implementation/driverSpecific.cpp
    Implementation/maxTextureSize.cpp)

set(MagnumGL_HEADERS
    AbstractFramebuffer.h
    AbstractObject.h
    AbstractShaderProgram.h
    AbstractTexture.h
    Attribute.h
    Buffer.h
    Context.h
    CubeMapTexture.h
    DefaultFramebuffer.h
    Extensions.h
    Framebuffer.h
    GL.h
    Mesh.h
    MeshView.h
    OpenGL.h
    PixelFormat.h
    Renderbuffer.h
    RenderbufferFormat.h
    Renderer.h
    Sampler.h
    Shader.h
    Texture.h
    TextureFormat.h
    Version.h

    visibility.h)

# Header files to display in project view of IDEs only
set(MagnumGL_PRIVATE_HEADERS
    Implementation/BufferState.h
    Implementation/ContextState.h
    Implementation/FramebufferState.h
    Implementation/maxTextureSize.h
    Implementation/MeshState.h
    Implementation/RendererState.h
    Implementation/ShaderProgramState.h
    Implementation/ShaderState.h
    Implementation/State.h
    Implementation/TextureState.h)

# Desktop-only stuff
if(NOT TARGET_GLES)
    list(APPEND MagnumGL_SRCS RectangleTexture.cpp)
    list(APPEND MagnumGL_HEADERS RectangleTexture.h)
endif()

# OpenGL ES 3.0 and WebGL 2.0 stuff
if(NOT TARGET_GLES2)
    list(APPEND MagnumGL_SRCS
        BufferImage.cpp
        PrimitiveQuery.cpp
        TextureArray.cpp
        TransformFeedback.cpp

        Implementation/TransformFeedbackState.cpp)

    list(APPEND MagnumGL_HEADERS
        BufferImage.h
        PrimitiveQuery.h
        TextureArray.h
        TransformFeedback.h)

    list(APPEND MagnumGL_PRIVATE_HEADES
        Implementation/TransformFeedbackState.h)
endif()

# Desktop and OpenGL ES stuff that is not available in WebGL
if(NOT TARGET_WEBGL)
    list(APPEND MagnumGL_SRCS
        DebugOutput.cpp

        Implementation/DebugState.cpp)

    list(APPEND MagnumGL_HEADERS
        DebugOutput.h
        TimeQuery.h)

    list(APPEND MagnumGL_PRIVATE_HEADERS
        Implementation/DebugState.h)

    # Desktop and OpenGL ES 3.0 stuff that is not available in ES2 and WebGL
    if(NOT TARGET_GLES2)
        list(APPEND MagnumGL_SRCS
            BufferTexture.cpp
            CubeMapTextureArray.cpp
            MultisampleTexture.cpp)
        list(APPEND MagnumGL_HEADERS
            BufferTexture.h
            BufferTextureFormat.h
            CubeMapTextureArray.h
            ImageFormat.h
            MultisampleTexture.h)
    endif()
endif()

# Desktop, OpenGL ES and WebGL 2.0 stuff that is not available in WebGL 1.0
if(NOT (TARGET_WEBGL AND TARGET_GLES2))
    list(APPEND MagnumGL_SRCS
        AbstractQuery.cpp

        Implementation/QueryState.cpp)

    list(APPEND MagnumGL_HEADERS
        AbstractQuery.h
        SampleQuery.h)

    list(APPEND MagnumGL_PRIVATE_HEADERS
        Implementation/QueryState.h)
endif()

# Link in GL function pointer variables on platforms that support it
if(NOT CORRADE_TARGET_EMSCRIPTEN)
    list(APPEND MagnumGL_SRCS $<TARGET_OBJECTS:MagnumFlextGLObjects>)
endif()

# GL library
add_library(MagnumGL ${SHARED_OR_STATIC}
    ${MagnumGL_SRCS}
    ${MagnumGL_HEADERS}
    ${MagnumGL_PRIVATE_HEADERS})
set_target_properties(MagnumGL PROPERTIES
    DEBUG_POSTFIX "-d"
    FOLDER "Magnum/GL")
if(NOT BUILD_STATIC)
    target_compile_definitions(MagnumGL PRIVATE "FlextGL_EXPORTS")
    set_target_properties(MagnumGL PROPERTIES VERSION ${MAGNUM_LIBRARY_VERSION} SOVERSION ${MAGNUM_LIBRARY_SOVERSION})
elseif(BUILD_STATIC_PIC)
    set_target_properties(MagnumGL PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
# We can use both implicit include path (GLES2/gl2.h) where our headers can
# be overriden with system ones or explicit (MagnumExternal/OpenGL/GLES2/gl2ext.h)
# where only our headers will be used
target_include_directories(MagnumGL PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/MagnumExternal/OpenGL)
target_link_libraries(MagnumGL PUBLIC Magnum)
if(NOT TARGET_GLES OR TARGET_DESKTOP_GLES)
    target_link_libraries(MagnumGL PUBLIC ${OPENGL_gl_LIBRARY})
elseif(TARGET_GLES2)
    target_link_libraries(MagnumGL PUBLIC OpenGLES2::OpenGLES2)
else()
    target_link_libraries(MagnumGL PUBLIC OpenGLES3::OpenGLES3)
endif()

install(TARGETS MagnumGL
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumGL_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/GL)

# OpenGLTester class
if(WITH_OPENGLTESTER)
    find_package(Corrade REQUIRED TestSuite)

    set(MagnumOpenGLTester_SRCS OpenGLTester.cpp)
    set(MagnumOpenGLTester_HEADERS OpenGLTester.h)
    if(BUILD_DEPRECATED)
        set(MagnumOpenGLTester_DEPRECATED_HEADERS ../Test/AbstractOpenGLTester.h)
    endif()

    add_library(MagnumOpenGLTester STATIC
        ${MagnumOpenGLTester_SRCS}
        ${MagnumOpenGLTester_HEADERS}
        ${MagnumOpenGLTester_DEPRECATED_HEADERS}) # TODO: remove when the header is removed
    set_target_properties(MagnumOpenGLTester PROPERTIES
        DEBUG_POSTFIX "-d"
        FOLDER "Magnum/GL")
    # Assuming that PIC is not needed because the Tester lib is always linked
    # to the executable and not to any intermediate shared lib

    # OPENGLTESTER_APPLICATION defined in the root CMakeLists, because it also
    # enables the application library dependencies
    target_link_libraries(MagnumOpenGLTester Magnum Corrade::TestSuite ${OPENGLTESTER_APPLICATION})

    install(FILES ${MagnumOpenGLTester_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/GL)
    if(BUILD_DEPRECATED)
        install(FILES ${MagnumOpenGLTester_DEPRECATED_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Test)
    endif()
    install(TARGETS MagnumOpenGLTester
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Magnum OpenGLTester target alias for superprojects
    add_library(Magnum::OpenGLTester ALIAS MagnumOpenGLTester)
endif()

if(BUILD_TESTS)
    add_subdirectory(Test)
endif()

# Magnum GL library target alias for superprojects
add_library(Magnum::GL ALIAS MagnumGL)
