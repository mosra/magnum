set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wold-style-cast -pedantic -std=c++0x -fvisibility=hidden")

# If targeting GLES, save it into configuration header
if(TARGET_GLES)
    set(MAGNUM_TARGET_GLES 1)
endif()

if(GCC44_COMPATIBILITY)
    set(MAGNUM_GCC44_COMPATIBILITY 1)
    set(GCC45_COMPATIBILITY 1)
endif()

if(GCC45_COMPATIBILITY)
    set(MAGNUM_GCC45_COMPATIBILITY 1)
endif()

# -Wdouble-promotion is supported from GCC 4.6
# TODO: do this with check_c_compiler_flags()
if(NOT GCC45_COMPATIBILITY)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdouble-promotion")
    endif()
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/magnumConfigure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/magnumConfigure.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CORRADE_INCLUDE_DIR})

# Files shared between main library and unit test library
set(Magnum_SRCS
    AbstractImage.cpp
    AbstractTexture.cpp
    AbstractShaderProgram.cpp
    BufferedTexture.cpp
    Camera.cpp
    Framebuffer.cpp
    IndexedMesh.cpp
    Light.cpp
    Mesh.cpp
    Query.cpp
    Renderbuffer.cpp
    Shader.cpp
    SizeTraits.cpp
    TypeTraits.cpp

    Trade/AbstractImporter.cpp
    Trade/MeshData.cpp)
set(Magnum_HEADERS
    AbstractImage.h
    AbstractShaderProgram.h
    AbstractTexture.h
    BufferedImage.h
    BufferedTexture.h
    Buffer.h
    Camera.h
    CubeMapTextureArray.h
    CubeMapTexture.h
    Framebuffer.h
    Image.h
    ImageWrapper.h
    IndexedMesh.h
    Light.h
    Magnum.h
    Mesh.h
    Object.h
    Query.h
    Renderbuffer.h
    Scene.h
    Set.h
    Shader.h
    SizeTraits.h
    Texture.h
    TypeTraits.h

    magnumCompatibility.h
    magnumVisibility.h)
if(NOT CMAKE_NO_OBJECT_TARGET)
    add_library(MagnumObjects OBJECT ${Magnum_SRCS})
endif()

# Files shared between main library and math unit test library
set(MagnumMath_SRCS
    Math/Math.cpp)
if(NOT CMAKE_NO_OBJECT_TARGET)
    add_library(MagnumMathObjects OBJECT ${MagnumMath_SRCS})
endif()

# Files compiled with different flags for main library and unit test library
set(Magnum_GracefulAssert_SRCS
    Object.cpp)

# Set shared library flags for the objects, as they will be part of shared lib
# TODO: fix when CMake sets target_EXPORTS for OBJECT targets as well
if(NOT CMAKE_NO_OBJECT_TARGET)
    set_target_properties(MagnumObjects MagnumMathObjects PROPERTIES COMPILE_FLAGS "-DMagnumObjects_EXPORTS ${CMAKE_SHARED_LIBRARY_CXX_FLAGS}")
endif()

# Main library
if(NOT CMAKE_NO_OBJECT_TARGET)
    add_library(Magnum SHARED
        $<TARGET_OBJECTS:MagnumObjects>
        $<TARGET_OBJECTS:MagnumMathObjects>
        ${Magnum_GracefulAssert_SRCS})
else()
    add_library(Magnum SHARED
        ${Magnum_SRCS}
        ${MagnumMath_SRCS}
        ${Magnum_GracefulAssert_SRCS})
endif()
target_link_libraries(Magnum ${CORRADE_UTILITY_LIBRARY} ${CORRADE_PLUGINMANAGER_LIBRARY})
if(NOT TARGET_GLES)
    target_link_libraries(Magnum ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARY})
else()
    target_link_libraries(Magnum ${OPENGLES2_LIBRARY})
endif()

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

if(WITH_SHADERS)
    add_subdirectory(Shaders)
endif()

if(BUILD_TESTS)
    enable_testing()
    include_directories(${QT_INCLUDE_DIR})

    # Library with graceful assert for testing
    if(NOT CMAKE_NO_OBJECT_TARGET)
        add_library(MagnumTestLib SHARED
            $<TARGET_OBJECTS:MagnumObjects>
            ${Magnum_GracefulAssert_SRCS})
    else()
        add_library(MagnumTestLib SHARED
            ${Magnum_SRCS}
            ${Magnum_GracefulAssert_SRCS})
    endif()
    set_target_properties(MagnumTestLib PROPERTIES COMPILE_FLAGS -DCORRADE_GRACEFUL_ASSERT)
    target_link_libraries(MagnumTestLib ${CORRADE_UTILITY_LIBRARY} ${CORRADE_PLUGINMANAGER_LIBRARY})
    if(NOT TARGET_GLES)
        target_link_libraries(MagnumTestLib ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARY})
    else()
        target_link_libraries(MagnumTestLib ${OPENGLES2_LIBRARY})
    endif()

    add_subdirectory(Test)
endif()
