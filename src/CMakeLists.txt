set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wold-style-cast -Winit-self -pedantic -std=c++0x -fvisibility=hidden")

# -Wdouble-promotion is supported from GCC 4.6
# TODO: do this with check_c_compiler_flags()
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND NOT "${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS "4.6.0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdouble-promotion")
endif()

# If targeting GLES, save it into configuration header
if(TARGET_GLES)
    set(MAGNUM_TARGET_GLES 1)
endif()
if(TARGET_GLES2)
    set(MAGNUM_TARGET_GLES2 1)
endif()

if(GCC46_COMPATIBILITY)
    set(MAGNUM_GCC46_COMPATIBILITY 1)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/magnumConfigure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/magnumConfigure.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CORRADE_INCLUDE_DIR})

# Files shared between main library and unit test library
set(Magnum_SRCS
    AbstractImage.cpp
    AbstractTexture.cpp
    AbstractShaderProgram.cpp
    Buffer.cpp
    BufferedImage.cpp
    Context.cpp
    Framebuffer.cpp
    Image.cpp
    IndexedMesh.cpp
    Mesh.cpp
    Profiler.cpp
    Query.cpp
    Renderbuffer.cpp
    Shader.cpp
    SizeTraits.cpp
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
        BufferedTexture.cpp)
endif()

set(Magnum_HEADERS
    AbstractImage.h
    AbstractShaderProgram.h
    AbstractTexture.h
    BufferedImage.h
    Buffer.h
    Color.h
    Context.h
    CubeMapTexture.h
    DimensionTraits.h
    Extensions.h
    Framebuffer.h
    Image.h
    ImageWrapper.h
    IndexedMesh.h
    Magnum.h
    Mesh.h
    Profiler.h
    Query.h
    Renderbuffer.h
    ResourceManager.h
    Shader.h
    SizeTraits.h
    Swizzle.h
    Texture.h
    Timeline.h
    TypeTraits.h

    magnumCompatibility.h
    magnumVisibility.h)

# Desktop-only headers
if(NOT TARGET_GLES)
    set(Magnum_HEADERS ${Magnum_HEADERS}
        BufferedTexture.h
        CubeMapTextureArray.h)
endif()

if(NOT CMAKE_NO_OBJECT_TARGET)
    add_library(MagnumObjects OBJECT ${Magnum_SRCS})
endif()

# Files shared between main library and math unit test library
set(MagnumMath_SRCS
    Math/Math.cpp)
if(NOT CMAKE_NO_OBJECT_TARGET)
    add_library(MagnumMathObjects OBJECT ${MagnumMath_SRCS})
endif()

# Set shared library flags for the objects, as they will be part of shared lib
# TODO: fix when CMake sets target_EXPORTS for OBJECT targets as well
if(NOT CMAKE_NO_OBJECT_TARGET)
    set_target_properties(MagnumObjects MagnumMathObjects PROPERTIES COMPILE_FLAGS "-DMagnumObjects_EXPORTS ${CMAKE_SHARED_LIBRARY_CXX_FLAGS}")
endif()

# Main library
if(NOT CMAKE_NO_OBJECT_TARGET)
    add_library(Magnum SHARED
        $<TARGET_OBJECTS:MagnumObjects>
        $<TARGET_OBJECTS:MagnumMathObjects>)
else()
    add_library(Magnum SHARED
        ${Magnum_SRCS}
        ${MagnumMath_SRCS})
endif()
set(Magnum_LIBS
    ${CORRADE_UTILITY_LIBRARY}
    ${CORRADE_PLUGINMANAGER_LIBRARY})
if(NOT TARGET_GLES)
    set(Magnum_LIBS ${Magnum_LIBS}
        ${OPENGL_gl_LIBRARY}
        ${GLEW_LIBRARY})
else()
    set(Magnum_LIBS ${Magnum_LIBS}
        ${OPENGLES2_LIBRARY})
endif()
target_link_libraries(Magnum ${Magnum_LIBS})

install(TARGETS Magnum DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${Magnum_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR})

# Install also configure file
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/magnumConfigure.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR})

add_subdirectory(Contexts)
add_subdirectory(Math)
add_subdirectory(Trade)

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

if(BUILD_TESTS)
    enable_testing()

    # Library with graceful assert for testing
    if(NOT CMAKE_NO_OBJECT_TARGET)
        add_library(MagnumTestLib SHARED
            $<TARGET_OBJECTS:MagnumObjects>)
    else()
        add_library(MagnumTestLib SHARED
            ${Magnum_SRCS})
    endif()
    set_target_properties(MagnumTestLib PROPERTIES COMPILE_FLAGS -DCORRADE_GRACEFUL_ASSERT)
    target_link_libraries(MagnumTestLib ${Magnum_LIBS})

    add_subdirectory(Test)
endif()
