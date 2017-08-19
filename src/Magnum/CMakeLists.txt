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

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

# Files shared between main library and unit test library
set(Magnum_SRCS
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
    Image.cpp
    Mesh.cpp
    MeshView.cpp
    OpenGL.cpp
    PixelFormat.cpp
    PixelStorage.cpp
    Renderbuffer.cpp
    Renderer.cpp
    Resource.cpp
    Sampler.cpp
    Shader.cpp
    Texture.cpp
    Timeline.cpp
    Version.cpp

    Implementation/BufferState.cpp
    Implementation/ContextState.cpp
    Implementation/FramebufferState.cpp
    Implementation/MeshState.cpp
    Implementation/RendererState.cpp
    Implementation/ShaderProgramState.cpp
    Implementation/State.cpp
    Implementation/TextureState.cpp
    Implementation/driverSpecific.cpp
    Implementation/maxTextureSize.cpp

    Trade/AbstractImageConverter.cpp
    Trade/AbstractImporter.cpp
    Trade/AbstractMaterialData.cpp
    Trade/ImageData.cpp
    Trade/LightData.cpp
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
    AbstractObject.h
    AbstractResourceLoader.h
    AbstractShaderProgram.h
    AbstractTexture.h
    Array.h
    Attribute.h
    Buffer.h
    Context.h
    CubeMapTexture.h
    DefaultFramebuffer.h
    DimensionTraits.h
    Extensions.h
    Framebuffer.h
    Image.h
    ImageView.h
    Magnum.h
    Mesh.h
    MeshView.h
    OpenGL.h
    PixelFormat.h
    PixelStorage.h
    Renderbuffer.h
    RenderbufferFormat.h
    Renderer.h
    Resource.h
    ResourceManager.h
    ResourceManager.hpp
    Sampler.h
    Shader.h
    Tags.h
    Texture.h
    TextureFormat.h
    Timeline.h
    Types.h
    Version.h

    visibility.h)

# Header files to display in project view of IDEs only
set(Magnum_PRIVATE_HEADERS
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

# Deprecated stuff
if(BUILD_DEPRECATED)
    list(APPEND Magnum_HEADERS
        Color.h
        ColorFormat.h
        ImageReference.h)
endif()

# Desktop-only stuff
if(NOT TARGET_GLES)
    list(APPEND Magnum_SRCS RectangleTexture.cpp)
    list(APPEND Magnum_HEADERS RectangleTexture.h)
endif()

# OpenGL ES 3.0 and WebGL 2.0 stuff
if(NOT TARGET_GLES2)
    list(APPEND Magnum_SRCS
        BufferImage.cpp
        PrimitiveQuery.cpp
        TextureArray.cpp
        TransformFeedback.cpp

        Implementation/TransformFeedbackState.cpp)

    list(APPEND Magnum_HEADERS
        BufferImage.h
        PrimitiveQuery.h
        TextureArray.h
        TransformFeedback.h)

    list(APPEND Magnum_PRIVATE_HEADES
        Implementation/TransformFeedbackState.h)
endif()

# Desktop and OpenGL ES stuff that is not available in WebGL
if(NOT TARGET_WEBGL)
    list(APPEND Magnum_SRCS
        DebugOutput.cpp

        Implementation/DebugState.cpp)

    list(APPEND Magnum_HEADERS
        DebugOutput.h
        TimeQuery.h)

    list(APPEND Magnum_PRIVATE_HEADERS
        Implementation/DebugState.h)

    # Desktop and OpenGL ES 3.0 stuff that is not available in ES2 and WebGL
    if(NOT TARGET_GLES2)
        list(APPEND Magnum_SRCS
            BufferTexture.cpp
            CubeMapTextureArray.cpp
            MultisampleTexture.cpp)
        list(APPEND Magnum_HEADERS
            BufferTexture.h
            BufferTextureFormat.h
            CubeMapTextureArray.h
            ImageFormat.h
            MultisampleTexture.h)
    endif()

    if(BUILD_DEPRECATED)
        list(APPEND Magnum_HEADERS
            DebugMessage.h)
    endif()
endif()

# Desktop, OpenGL ES and WebGL 2.0 stuff that is not available in WebGL 1.0
if(NOT (TARGET_WEBGL AND TARGET_GLES2))
    list(APPEND Magnum_SRCS
        AbstractQuery.cpp

        Implementation/QueryState.cpp)

    list(APPEND Magnum_HEADERS
        AbstractQuery.h
        SampleQuery.h)

    list(APPEND Magnum_PRIVATE_HEADERS
        Implementation/QueryState.h)

    if(BUILD_DEPRECATED)
        list(APPEND Magnum_HEADERS
            Query.h)
    endif()
endif()

# Link in GL function pointer variables on platforms that support it
if(NOT CORRADE_TARGET_EMSCRIPTEN)
    list(APPEND Magnum_SRCS $<TARGET_OBJECTS:MagnumFlextGLObjects>)
endif()

# Files shared between main library and math unit test library
set(MagnumMath_SRCS
    Math/Color.cpp
    Math/Functions.cpp
    Math/Packing.cpp
    Math/instantiation.cpp)

# Objects shared between main and test library
add_library(MagnumMathObjects OBJECT ${MagnumMath_SRCS})
target_include_directories(MagnumMathObjects PUBLIC $<TARGET_PROPERTY:Magnum,INTERFACE_INCLUDE_DIRECTORIES>)
if(NOT BUILD_STATIC)
    target_compile_definitions(MagnumMathObjects PRIVATE "MagnumMathObjects_EXPORTS")
endif()
if(NOT BUILD_STATIC OR BUILD_STATIC_PIC)
    set_target_properties(MagnumMathObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
set_target_properties(MagnumMathObjects PROPERTIES FOLDER "Magnum/Math")

# Main library
add_library(Magnum ${SHARED_OR_STATIC}
    ${Magnum_SRCS}
    ${Magnum_HEADERS}
    ${Magnum_PRIVATE_HEADERS}
    $<TARGET_OBJECTS:MagnumMathObjects>)
set_target_properties(Magnum PROPERTIES
    DEBUG_POSTFIX "-d"
    FOLDER "Magnum")
if(NOT BUILD_STATIC)
    target_compile_definitions(Magnum PRIVATE "FlextGL_EXPORTS")
elseif(BUILD_STATIC_PIC)
    set_target_properties(Magnum PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
# We can use both implicit include path (GLES2/gl2.h) where our headers can
# be overriden with system ones or explicit (MagnumExternal/OpenGL/GLES2/gl2ext.h)
# where only our headers will be used
target_include_directories(Magnum PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/MagnumExternal/OpenGL)
target_link_libraries(Magnum
    Corrade::Utility
    Corrade::PluginManager)
if(NOT TARGET_GLES OR TARGET_DESKTOP_GLES)
    target_link_libraries(Magnum ${OPENGL_gl_LIBRARY})
elseif(TARGET_GLES2)
    target_link_libraries(Magnum OpenGLES2::OpenGLES2)
else()
    target_link_libraries(Magnum OpenGLES3::OpenGLES3)
endif()

install(TARGETS Magnum
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${Magnum_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/configure.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR})

# OpenGLTester class
if(WITH_OPENGLTESTER)
    find_package(Corrade REQUIRED TestSuite)

    set(MagnumOpenGLTester_SRCS OpenGLTester.cpp)
    set(MagnumOpenGLTester_HEADERS OpenGLTester.h)
    if(BUILD_DEPRECATED)
        set(MagnumOpenGLTester_DEPRECATED_HEADERS Test/AbstractOpenGLTester.h)
    endif()

    add_library(MagnumOpenGLTester STATIC
        ${MagnumOpenGLTester_SRCS}
        ${MagnumOpenGLTester_HEADERS}
        ${MagnumOpenGLTester_DEPRECATED_HEADERS}) # TODO: remove when the header is removed
    set_target_properties(MagnumOpenGLTester PROPERTIES
        DEBUG_POSTFIX "-d"
        FOLDER "Magnum")
    # Assuming that PIC is not needed because the Tester lib is always linked
    # to the executable and not to any intermediate shared lib

    # OPENGLTESTER_APPLICATION defined in the root CMakeLists, because it also
    # enables the application library dependencies
    target_link_libraries(MagnumOpenGLTester Magnum Corrade::TestSuite ${OPENGLTESTER_APPLICATION})

    install(FILES ${MagnumOpenGLTester_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR})
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
    # Library with graceful assert for testing
    add_library(MagnumMathTestLib ${SHARED_OR_STATIC}
        $<TARGET_OBJECTS:MagnumMathObjects>
        ${PROJECT_SOURCE_DIR}/src/dummy.cpp) # XCode workaround, see file comment for details
    target_include_directories(MagnumMathTestLib PUBLIC $<TARGET_PROPERTY:Magnum,INTERFACE_INCLUDE_DIRECTORIES>)
    target_compile_definitions(MagnumMathTestLib PRIVATE "CORRADE_GRACEFUL_ASSERT")
    set_target_properties(MagnumMathTestLib PROPERTIES
        DEBUG_POSTFIX "-d"
        FOLDER "Magnum/Math")
    target_link_libraries(MagnumMathTestLib Corrade::Utility)

    # On Windows we need to install first and then run the tests to avoid "DLL
    # not found" hell, thus we need to install this too
    if(CORRADE_TARGET_WINDOWS AND NOT CMAKE_CROSSCOMPILING AND NOT BUILD_STATIC)
        install(TARGETS MagnumMathTestLib
            RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
            LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
            ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
    endif()

    add_subdirectory(Test)
endif()

# Magnum library target alias and configure file for superprojects
add_library(Magnum::Magnum ALIAS Magnum)
set(_MAGNUM_CONFIGURE_FILE ${CMAKE_CURRENT_BINARY_DIR}/configure.h CACHE INTERNAL "")
