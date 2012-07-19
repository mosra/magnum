# CMake 2.8.8 required for OBJECT library target
cmake_minimum_required(VERSION 2.8.8)
project(Magnum)

include(CMakeDependentOption)

option(TARGET_GLES "Build for OpenGL ES 2 instead of desktop OpenGL" OFF)

# Parts of the library
option(WITH_EVERYTHING "Build everything (doesn't include contexts)" ON)
option(WITH_MESHTOOLS "Build MeshTools library" OFF)
option(WITH_PHYSICS "Build Physics library" OFF)
option(WITH_PRIMITIVES "Builf Primitives library" OFF)
option(WITH_SHADERS "Build Shaders library" OFF)

cmake_dependent_option(WITH_EGLCONTEXT "Build EglContext library" OFF "TARGET_GLES" OFF)
cmake_dependent_option(WITH_GLUTCONTEXT "Build GlutContext library" OFF "NOT TARGET_GLES" OFF)
option(WITH_SDL2CONTEXT "Build Sdl2Context library" OFF)

option(BUILD_TESTS "Build unit tests (requires Qt4)." OFF)

if(WITH_EVERYTHING)
    set(WITH_MESHTOOLS ON)
    set(WITH_PHYSICS ON)
    set(WITH_PRIMITIVES ON)
    set(WITH_SHADERS ON)
endif()

if(BUILD_TESTS)
    find_package(Qt4)

    if(NOT QT4_FOUND)
        message(FATAL_ERROR "Qt4, required for building unit tests, was not found. Set BUILD_TESTS to OFF to skip building them.")
    endif()

    enable_testing()
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${Magnum_SOURCE_DIR}/modules/")

# Check dependencies
find_package(Corrade REQUIRED)
if(NOT TARGET_GLES)
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
else()
    find_package(OpenGLES2 REQUIRED)
endif()

# Installation paths
set(MAGNUM_LIBRARY_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
set(MAGNUM_CMAKE_MODULE_INSTALL_DIR ${CMAKE_ROOT}/Modules)
set(MAGNUM_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include/Magnum)

add_subdirectory(modules)
add_subdirectory(src)
