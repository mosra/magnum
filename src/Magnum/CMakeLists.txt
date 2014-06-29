#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014
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

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

# Files shared between main library and unit test library
set(Magnum_SRCS
    AbstractFramebuffer.cpp
    AbstractImage.cpp
    AbstractObject.cpp
    AbstractTexture.cpp
    AbstractShaderProgram.cpp
    Buffer.cpp
    ColorFormat.cpp
    CubeMapTexture.cpp
    Context.cpp
    DebugMessage.cpp
    DefaultFramebuffer.cpp
    Framebuffer.cpp
    Image.cpp
    Mesh.cpp
    MeshView.cpp
    OpenGL.cpp
    Query.cpp
    Renderbuffer.cpp
    Renderer.cpp
    Resource.cpp
    Sampler.cpp
    Shader.cpp
    Texture.cpp
    Timeline.cpp
    Version.cpp

    Implementation/BufferState.cpp
    Implementation/DebugState.cpp
    Implementation/FramebufferState.cpp
    Implementation/MeshState.cpp
    Implementation/RendererState.cpp
    Implementation/ShaderProgramState.cpp
    Implementation/State.cpp
    Implementation/TextureState.cpp
    Implementation/detectedDriver.cpp
    Implementation/maxTextureSize.cpp
    Implementation/setupDriverWorkarounds.cpp

    Trade/AbstractImageConverter.cpp
    Trade/AbstractImporter.cpp
    Trade/AbstractMaterialData.cpp
    Trade/MeshData2D.cpp
    Trade/MeshData3D.cpp
    Trade/MeshObjectData2D.cpp
    Trade/MeshObjectData3D.cpp
    Trade/ObjectData2D.cpp
    Trade/ObjectData3D.cpp
    Trade/PhongMaterialData.cpp
    Trade/SceneData.cpp
    Trade/TextureData.cpp)

set(Magnum_HEADERS
    AbstractFramebuffer.h
    AbstractImage.h
    AbstractObject.h
    AbstractResourceLoader.h
    AbstractShaderProgram.h
    AbstractTexture.h
    Array.h
    Buffer.h
    Color.h
    ColorFormat.h
    Context.h
    CubeMapTexture.h
    DebugMessage.h
    DefaultFramebuffer.h
    DimensionTraits.h
    Extensions.h
    Framebuffer.h
    Image.h
    ImageReference.h
    Magnum.h
    Mesh.h
    MeshView.h
    OpenGL.h
    Query.h
    Renderbuffer.h
    RenderbufferFormat.h
    Renderer.h
    Resource.h
    ResourceManager.h
    Sampler.h
    Shader.h
    Texture.h
    TextureFormat.h
    Timeline.h
    Types.h
    Version.h

    visibility.h)

# Deprecated headers
if(BUILD_DEPRECATED)
    set(Magnum_HEADERS ${Magnum_HEADERS}
        DebugMarker.h)
endif()

# Desktop-only stuff
if(NOT TARGET_GLES)
    set(Magnum_HEADERS ${Magnum_HEADERS}
        BufferTexture.h
        CubeMapTextureArray.h
        MultisampleTexture.h
        RectangleTexture.h)
    set(Magnum_SRCS ${Magnum_SRCS}
        BufferTexture.cpp
        CubeMapTextureArray.cpp
        MultisampleTexture.cpp
        RectangleTexture.cpp
        $<TARGET_OBJECTS:MagnumGLLoadGenObjects>)
endif()

# Non-ES2 stuff
if(NOT TARGET_GLES2)
    set(Magnum_HEADERS ${Magnum_HEADERS}
        BufferImage.h
        TextureArray.h)
    set(Magnum_SRCS ${Magnum_SRCS}
        BufferImage.cpp
        TextureArray.cpp)
endif()

# Files shared between main library and math unit test library
set(MagnumMath_SRCS
    Math/Functions.cpp
    Math/instantiation.cpp)

# Main library
add_library(MagnumMathObjects OBJECT ${MagnumMath_SRCS})
add_library(Magnum ${SHARED_OR_STATIC}
    ${Magnum_SRCS}
    $<TARGET_OBJECTS:MagnumMathObjects>)
set_target_properties(Magnum PROPERTIES DEBUG_POSTFIX "-d")

# TODO: fix when CMake sets target_EXPORTS for OBJECT targets as well
if(NOT BUILD_STATIC OR BUILD_STATIC_PIC)
    # Set shared library flags for the objects, as they will be part of shared lib
    # TODO: CMake 2.8.9 has this as POSITION_INDEPENDENT_CODE property
    set_target_properties(MagnumMathObjects PROPERTIES COMPILE_FLAGS "-DMagnumMathObjects_EXPORTS ${CMAKE_SHARED_LIBRARY_CXX_FLAGS}")
    set_target_properties(Magnum PROPERTIES COMPILE_FLAGS "-DGLLoadGen_EXPORTS ${CMAKE_SHARED_LIBRARY_CXX_FLAGS}")
else()
    set_target_properties(MagnumMathObjects PROPERTIES COMPILE_FLAGS "-DMagnumMathObjects_EXPORTS")
    set_target_properties(Magnum PROPERTIES COMPILE_FLAGS "-DGLLoadGen_EXPORTS")
endif()

set(Magnum_LIBS
    ${CORRADE_UTILITY_LIBRARIES}
    ${CORRADE_PLUGINMANAGER_LIBRARIES})
if(NOT TARGET_GLES OR TARGET_DESKTOP_GLES)
    set(Magnum_LIBS ${Magnum_LIBS} ${OPENGL_gl_LIBRARY})
elseif(TARGET_GLES2)
    set(Magnum_LIBS ${Magnum_LIBS} ${OPENGLES2_LIBRARY})
else()
    set(Magnum_LIBS ${Magnum_LIBS} ${OPENGLES3_LIBRARY})
endif()
target_link_libraries(Magnum ${Magnum_LIBS})

install(TARGETS Magnum
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${Magnum_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/configure.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR})

add_subdirectory(Math)
add_subdirectory(Platform)
add_subdirectory(Trade)

if(WITH_AUDIO)
    add_subdirectory(Audio)
endif()

if(WITH_DEBUGTOOLS)
    add_subdirectory(DebugTools)
endif()

if(WITH_MESHTOOLS)
    add_subdirectory(MeshTools)
endif()

if(WITH_PRIMITIVES)
    add_subdirectory(Primitives)
endif()

if(WITH_SCENEGRAPH)
    add_subdirectory(SceneGraph)
endif()

if(WITH_SHADERS)
    add_subdirectory(Shaders)
endif()

if(WITH_SHAPES)
    add_subdirectory(Shapes)
endif()

if(WITH_TEXT)
    add_subdirectory(Text)
endif()

if(WITH_TEXTURETOOLS)
    add_subdirectory(TextureTools)
endif()

if(BUILD_TESTS)
    # Libraries with graceful assert for testing
    add_library(MagnumMathTestLib ${SHARED_OR_STATIC}
        $<TARGET_OBJECTS:MagnumMathObjects>)
    set_target_properties(MagnumMathTestLib PROPERTIES
        COMPILE_FLAGS "-DCORRADE_GRACEFUL_ASSERT"
        DEBUG_POSTFIX "-d")
    target_link_libraries(MagnumMathTestLib ${CORRADE_UTILITY_LIBRARY})

    # On Windows we need to install first and then run the tests to avoid "DLL
    # not found" hell, thus we need to install this too
    if(CORRADE_TARGET_WINDOWS AND NOT CMAKE_CROSSCOMPILING)
        install(TARGETS MagnumMathTestLib
            RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
            LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
            ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
    endif()

    add_subdirectory(Test)
endif()
