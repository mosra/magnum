set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wold-style-cast -Winit-self -Werror=return-type -Wmissing-declarations -pedantic -std=c++0x -fvisibility=hidden")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdouble-promotion")
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/magnumConfigure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/magnumConfigure.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CORRADE_INCLUDE_DIR})

# Files shared between main library and unit test library
set(Magnum_SRCS
    AbstractFramebuffer.cpp
    AbstractImage.cpp
    AbstractTexture.cpp
    AbstractShaderProgram.cpp
    Buffer.cpp
    Context.cpp
    DebugMarker.cpp
    DefaultFramebuffer.cpp
    Framebuffer.cpp
    Image.cpp
    IndexedMesh.cpp
    Mesh.cpp
    Query.cpp
    Renderbuffer.cpp
    Resource.cpp
    Shader.cpp
    Timeline.cpp
    TypeTraits.cpp

    Implementation/BufferState.cpp
    Implementation/State.cpp

    Trade/AbstractImporter.cpp
    Trade/MeshData2D.cpp
    Trade/MeshData3D.cpp)

# Desktop-only code
if(NOT TARGET_GLES)
    set(Magnum_SRCS ${Magnum_SRCS}
        BufferTexture.cpp)
endif()

# Not-ES2 code
if(NOT TARGET_GLES2)
    set(Magnum_SRCS ${Magnum_SRCS}
        BufferImage.cpp)
endif()

set(Magnum_HEADERS
    AbstractFramebuffer.h
    AbstractImage.h
    AbstractResourceLoader.h
    AbstractShaderProgram.h
    AbstractTexture.h
    Array.h
    Buffer.h
    Color.h
    Context.h
    CubeMapTexture.h
    DebugMarker.h
    DefaultFramebuffer.h
    DimensionTraits.h
    Extensions.h
    Framebuffer.h
    Image.h
    ImageWrapper.h
    IndexedMesh.h
    Magnum.h
    Mesh.h
    Query.h
    Renderbuffer.h
    Renderer.h
    Resource.h
    ResourceManager.h
    Shader.h
    Swizzle.h
    Texture.h
    Timeline.h
    TypeTraits.h

    magnumVisibility.h)

# Desktop-only headers
if(NOT TARGET_GLES)
    set(Magnum_HEADERS ${Magnum_HEADERS}
        BufferTexture.h
        CubeMapTextureArray.h)
endif()

# Not-ES2 headers
if(NOT TARGET_GLES2)
    set(Magnum_HEADERS ${Magnum_HEADERS}
        BufferImage.h)
endif()

add_library(MagnumObjects OBJECT ${Magnum_SRCS})

# Files shared between main library and math unit test library
set(MagnumMath_SRCS
    Math/Functions.cpp
    Math/Quaternion.cpp
    Math/RectangularMatrix.cpp
    Math/Vector.cpp)
add_library(MagnumMathObjects OBJECT ${MagnumMath_SRCS})

# Set shared library flags for the objects, as they will be part of shared lib
# TODO: fix when CMake sets target_EXPORTS for OBJECT targets as well
set_target_properties(MagnumObjects MagnumMathObjects PROPERTIES COMPILE_FLAGS "-DMagnumObjects_EXPORTS ${CMAKE_SHARED_LIBRARY_CXX_FLAGS}")

# Main library
add_library(Magnum SHARED
    $<TARGET_OBJECTS:MagnumObjects>
    $<TARGET_OBJECTS:MagnumMathObjects>)
set(Magnum_LIBS
    ${CORRADE_UTILITY_LIBRARY}
    ${CORRADE_PLUGINMANAGER_LIBRARY})
if(NOT TARGET_GLES OR TARGET_DESKTOP_GLES)
    set(Magnum_LIBS ${Magnum_LIBS} ${OPENGL_gl_LIBRARY})
else()
    set(Magnum_LIBS ${Magnum_LIBS} ${OPENGLES2_LIBRARY})
endif()
if(NOT TARGET_GLES)
    set(Magnum_LIBS ${Magnum_LIBS} ${GLEW_LIBRARY})
endif()
target_link_libraries(Magnum ${Magnum_LIBS})

install(TARGETS Magnum DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${Magnum_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR})

# Install also configure file
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/magnumConfigure.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR})

add_subdirectory(Platform)
add_subdirectory(Math)
add_subdirectory(Trade)

if(WITH_DEBUGTOOLS)
    add_subdirectory(DebugTools)
endif()

if(WITH_MESHTOOLS)
    add_subdirectory(MeshTools)
endif()

if(WITH_PHYSICS)
    add_subdirectory(Physics)
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

if(WITH_TEXT)
    add_subdirectory(Text)
endif()

if(WITH_TEXTURETOOLS)
    add_subdirectory(TextureTools)
endif()

if(BUILD_TESTS)
    enable_testing()

    # Library with graceful assert for testing
    add_library(MagnumMathTestLib SHARED
        $<TARGET_OBJECTS:MagnumMathObjects>)
    set_target_properties(MagnumMathTestLib PROPERTIES COMPILE_FLAGS -DCORRADE_GRACEFUL_ASSERT)

    add_library(MagnumTestLib SHARED
        $<TARGET_OBJECTS:MagnumObjects>
        $<TARGET_OBJECTS:MagnumMathObjects>)
    set_target_properties(MagnumTestLib PROPERTIES COMPILE_FLAGS -DCORRADE_GRACEFUL_ASSERT)
    target_link_libraries(MagnumTestLib ${Magnum_LIBS})

    add_subdirectory(Test)
endif()
