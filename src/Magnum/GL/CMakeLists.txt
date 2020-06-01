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

set(MagnumGL_SRCS
    AbstractObject.cpp
    AbstractQuery.cpp
    Buffer.cpp
    Context.cpp
    DefaultFramebuffer.cpp
    Framebuffer.cpp
    OpenGL.cpp
    Renderbuffer.cpp
    Renderer.cpp
    Shader.cpp
    Texture.cpp
    TextureFormat.cpp
    Version.cpp

    Implementation/BufferState.cpp
    Implementation/ContextState.cpp
    Implementation/FramebufferState.cpp
    Implementation/MeshState.cpp
    Implementation/QueryState.cpp
    Implementation/RendererState.cpp
    Implementation/ShaderProgramState.cpp
    Implementation/ShaderState.cpp
    Implementation/State.cpp
    Implementation/TextureState.cpp
    Implementation/driverSpecific.cpp
    Implementation/maxTextureSize.cpp)

set(MagnumGL_GracefulAssert_SRCS
    AbstractFramebuffer.cpp
    AbstractShaderProgram.cpp
    AbstractTexture.cpp
    Attribute.cpp
    CubeMapTexture.cpp
    Mesh.cpp
    MeshView.cpp
    PixelFormat.cpp
    Sampler.cpp)

set(MagnumGL_HEADERS
    AbstractFramebuffer.h
    AbstractObject.h
    AbstractQuery.h
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
    TimeQuery.h
    Version.h

    visibility.h)

# Header files to display in project view of IDEs only
set(MagnumGL_PRIVATE_HEADERS
    Implementation/BufferState.h
    Implementation/ContextState.h
    Implementation/FramebufferState.h
    Implementation/maxTextureSize.h
    Implementation/MeshState.h
    Implementation/QueryState.h
    Implementation/RendererState.h
    Implementation/ShaderProgramState.h
    Implementation/ShaderState.h
    Implementation/State.h
    Implementation/TextureState.h)

# Desktop-only stuff
if(NOT TARGET_GLES)
    list(APPEND MagnumGL_SRCS RectangleTexture.cpp)
    list(APPEND MagnumGL_HEADERS
        PipelineStatisticsQuery.h
        RectangleTexture.h)
endif()

# OpenGL ES 3.0 and WebGL 2.0 stuff
if(NOT TARGET_GLES2)
    list(APPEND MagnumGL_SRCS
        PrimitiveQuery.cpp
        TextureArray.cpp
        TransformFeedback.cpp

        Implementation/TransformFeedbackState.cpp)

    list(APPEND MagnumGL_GracefulAssert_SRCS
        BufferImage.cpp)

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
        DebugOutput.h)

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
    list(APPEND MagnumGL_HEADERS
        SampleQuery.h)
endif()

# Objects shared between main and test library
add_library(MagnumGLObjects OBJECT
    ${MagnumGL_SRCS}
    ${MagnumGL_HEADERS}
    ${MagnumGL_PRIVATE_HEADERS})
# We can use both implicit include path (GLES2/gl2.h) where our headers can
# be overriden with system ones or explicit (MagnumExternal/OpenGL/GLES2/gl2ext.h)
# where only our headers will be used
target_include_directories(MagnumGLObjects PUBLIC
    $<TARGET_PROPERTY:Magnum,INTERFACE_INCLUDE_DIRECTORIES>)
if(NOT BUILD_STATIC)
    target_compile_definitions(MagnumGLObjects PRIVATE "MagnumGLObjects_EXPORTS" "FlextGL_EXPORTS")
endif()
if(NOT BUILD_STATIC OR BUILD_STATIC_PIC)
    set_target_properties(MagnumGLObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
set_target_properties(MagnumGLObjects PROPERTIES FOLDER "Magnum/GL")

# Link in GL function pointer variables on platforms that support it
if(NOT CORRADE_TARGET_EMSCRIPTEN)
    set(MagnumGL_FlextGL_SRCS $<TARGET_OBJECTS:MagnumFlextGLObjects>)
endif()

# GL library
add_library(MagnumGL ${SHARED_OR_STATIC}
    $<TARGET_OBJECTS:MagnumGLObjects>
    ${MagnumGL_FlextGL_SRCS}
    ${MagnumGL_GracefulAssert_SRCS})
set_target_properties(MagnumGL PROPERTIES
    DEBUG_POSTFIX "-d"
    FOLDER "Magnum/GL")
if(NOT BUILD_STATIC)
    target_compile_definitions(MagnumGL PRIVATE "FlextGL_EXPORTS")
    set_target_properties(MagnumGL PROPERTIES VERSION ${MAGNUM_LIBRARY_VERSION} SOVERSION ${MAGNUM_LIBRARY_SOVERSION})
elseif(BUILD_STATIC_PIC)
    set_target_properties(MagnumGL PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(MagnumGL PUBLIC Magnum)
if(NOT TARGET_GLES OR TARGET_DESKTOP_GLES)
    # If the GLVND library (CMake 3.11+) was found, link to the imported
    # target. Otherwise (and also on all systems except Linux) link to the
    # classic libGL. Can't use OpenGL_OpenGL_FOUND, because that one is set
    # also if GLVND is *not* found. WTF. Also can't just check for
    # OPENGL_opengl_LIBRARY because that's set even if OpenGL_GL_PREFERENCE is
    # explicitly set to LEGACY.
    if(OPENGL_opengl_LIBRARY AND OpenGL_GL_PREFERENCE STREQUAL GLVND)
        target_link_libraries(MagnumGL PUBLIC OpenGL::OpenGL)
    else()
        target_link_libraries(MagnumGL PUBLIC ${OPENGL_gl_LIBRARY})
    endif()
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
    if(NOT TARGET_GL)
        message(SEND_ERROR "OpenGLTester is available only if TARGET_GL is enabled")
    endif()

    find_package(Corrade REQUIRED TestSuite)

    set(MagnumOpenGLTester_SRCS OpenGLTester.cpp)
    set(MagnumOpenGLTester_HEADERS OpenGLTester.h)

    # We need a separate library for tests that links exclusively to
    # MagnumGLTestLib instead of MagnumGL otherwise there are problems on
    # Windows with global state like GL::Context::current(). So build an object
    # library and then include it in MagnumOpenGLTester and
    # MagnumOpenGLTesterTestLib. See the MagnumOpenGLTesterTestLib target for
    # further info.
    add_library(MagnumOpenGLTesterObjects OBJECT
        ${MagnumOpenGLTester_SRCS}
        ${MagnumOpenGLTester_HEADERS})
    # Assuming that PIC is not needed because the Tester lib is always linked
    # to the executable and not to any intermediate shared lib
    set_target_properties(MagnumOpenGLTesterObjects PROPERTIES FOLDER "Magnum/GL")
    target_include_directories(MagnumOpenGLTesterObjects PUBLIC
        $<TARGET_PROPERTY:Corrade::TestSuite,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:MagnumGL,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:${OPENGLTESTER_APPLICATION},INTERFACE_INCLUDE_DIRECTORIES>)

    add_library(MagnumOpenGLTester STATIC
        $<TARGET_OBJECTS:MagnumOpenGLTesterObjects>
        ${PROJECT_SOURCE_DIR}/src/dummy.cpp) # XCode workaround, see file comment for details
    set_target_properties(MagnumOpenGLTester PROPERTIES
        DEBUG_POSTFIX "-d"
        FOLDER "Magnum/GL")
    # OPENGLTESTER_APPLICATION defined in the root CMakeLists, because it also
    # enables the application library dependencies
    target_link_libraries(MagnumOpenGLTester PUBLIC Magnum Corrade::TestSuite ${OPENGLTESTER_APPLICATION})

    install(FILES ${MagnumOpenGLTester_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/GL)
    install(TARGETS MagnumOpenGLTester
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

    # Magnum OpenGLTester target alias for superprojects
    add_library(Magnum::OpenGLTester ALIAS MagnumOpenGLTester)
endif()

if(BUILD_TESTS)
    # Library with graceful assert for testing
    add_library(MagnumGLTestLib ${SHARED_OR_STATIC}
        $<TARGET_OBJECTS:MagnumGLObjects>
        ${MagnumGL_FlextGL_SRCS}
        ${MagnumGL_GracefulAssert_SRCS})
    set_target_properties(MagnumGLTestLib PROPERTIES
        DEBUG_POSTFIX "-d"
        FOLDER "Magnum/GL")
    target_compile_definitions(MagnumGLTestLib PRIVATE
        "CORRADE_GRACEFUL_ASSERT" "MagnumGL_EXPORTS" "FlextGL_EXPORTS")
    if(BUILD_STATIC_PIC)
        set_target_properties(MagnumGLTestLib PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    target_link_libraries(MagnumGLTestLib PUBLIC
        Magnum)
    if(NOT TARGET_GLES OR TARGET_DESKTOP_GLES)
        # If the GLVND library (CMake 3.11+) was found, link to the imported
        # target. Otherwise (and also on all systems except Linux) link to the
        # classic libGL. Can't use OpenGL_OpenGL_FOUND, because that one is set
        # also if GLVND is *not* found. WTF. Also can't just check for
        # OPENGL_opengl_LIBRARY because that's set even if OpenGL_GL_PREFERENCE
        # is explicitly set to LEGACY.
        if(OPENGL_opengl_LIBRARY AND OpenGL_GL_PREFERENCE STREQUAL GLVND)
            target_link_libraries(MagnumGLTestLib PUBLIC OpenGL::OpenGL)
        else()
            target_link_libraries(MagnumGLTestLib PUBLIC ${OPENGL_gl_LIBRARY})
        endif()
    elseif(TARGET_GLES2)
        target_link_libraries(MagnumGLTestLib PUBLIC OpenGLES2::OpenGLES2)
    else()
        target_link_libraries(MagnumGLTestLib PUBLIC OpenGLES3::OpenGLES3)
    endif()

    if(BUILD_GL_TESTS)
        add_library(MagnumOpenGLTesterTestLib STATIC
            $<TARGET_OBJECTS:MagnumOpenGLTesterObjects>
            ${PROJECT_SOURCE_DIR}/src/dummy.cpp) # XCode workaround, see file comment for details
        set_target_properties(MagnumOpenGLTesterTestLib PROPERTIES
            DEBUG_POSTFIX "-d"
            FOLDER "Magnum/GL")
        target_link_libraries(MagnumOpenGLTesterTestLib PUBLIC Magnum Corrade::TestSuite)
        # On Windows the ${OPENGLTESTER_APPLICATION} links to MagnumGL, which
        # is causing asserts in GL::Context::current() due to two different
        # instances of the global variable getting used. We circumvent that by
        # using just the library file from ${OPENGLTESTER_APPLICATION} and
        # linking to MagnumGLTestLib explicitly. It's not a problem on other
        # platforms and I don't want to go through the trouble of correctly
        # passing through all dependencies like X11, CGL etc., so I do it for
        # Windows only and elsewhere I just link the same way as with
        # MagnumOpenGLTester.
        if(CORRADE_TARGET_WINDOWS)
            if(MAGNUM_TARGET_GLES AND NOT MAGNUM_TARGET_DESKTOP_GLES)
                # Otherwise it complains that EGL::EGL does not exist here
                find_package(EGL)
            endif()
            target_link_libraries(MagnumOpenGLTesterTestLib PUBLIC
                $<TARGET_FILE:${OPENGLTESTER_APPLICATION}>
                MagnumGLTestLib) # this is different
            target_include_directories(MagnumOpenGLTesterTestLib PUBLIC
                $<TARGET_PROPERTY:${OPENGLTESTER_APPLICATION},INTERFACE_INCLUDE_DIRECTORIES>)
        else()
            target_link_libraries(MagnumOpenGLTesterTestLib PUBLIC Magnum Corrade::TestSuite MagnumGLTestLib ${OPENGLTESTER_APPLICATION})
        endif()
    endif()

    add_subdirectory(Test)
endif()

# Magnum GL library target alias for superprojects
add_library(Magnum::GL ALIAS MagnumGL)
