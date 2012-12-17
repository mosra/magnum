# CMake 2.8.8 required for OBJECT library target
cmake_minimum_required(VERSION 2.8.8)
project(Magnum)

include(CMakeDependentOption)

option(TARGET_GLES "Build for OpenGL ES instead of desktop OpenGL" OFF)
cmake_dependent_option(TARGET_GLES2 "Build for OpenGL ES 2" ON "TARGET_GLES" OFF)

# Parts of the library
option(WITH_EVERYTHING "Build everything (doesn't include contexts)" ON)
cmake_dependent_option(WITH_MESHTOOLS "Build MeshTools library" OFF "NOT WITH_EVERYTHING;NOT WITH_PHYSICS" ON)
cmake_dependent_option(WITH_PHYSICS "Build Physics library" OFF "NOT WITH_EVERYTHING" ON)
cmake_dependent_option(WITH_PRIMITIVES "Builf Primitives library" OFF "NOT WITH_EVERYTHING;NOT WITH_PHYSICS" ON)
cmake_dependent_option(WITH_SCENEGRAPH "Build SceneGraph library" OFF "NOT WITH_EVERYTHING;NOT WITH_PHYSICS" ON)
cmake_dependent_option(WITH_SHADERS "Build Shaders library" OFF "NOT WITH_EVERYTHING;NOT WITH_PHYSICS" ON)

cmake_dependent_option(WITH_MAGNUMINFO "Build magnum-info utility" OFF "NOT WITH_EVERYTHING" ON)

if(${CMAKE_SYSTEM_NAME} STREQUAL NaCl)
    option(WITH_NACLAPPLICATION "Build NaClApplication library" OFF)
else()
    option(WITH_GLXAPPLICATION "Build GlxApplication library" OFF)
    cmake_dependent_option(WITH_WINDOWLESSGLXAPPLICATION "Build WindowlessGlxApplication library" OFF "NOT WITH_MAGNUMINFO" ON)
    cmake_dependent_option(WITH_XEGLAPPLICATION "Build XEglApplication library" OFF "TARGET_GLES" OFF)
    cmake_dependent_option(WITH_GLUTAPPLICATION "Build GlutApplication library" OFF "NOT TARGET_GLES" OFF)
    option(WITH_SDL2APPLICATION "Build Sdl2Application library" OFF)
endif()
option(BUILD_TESTS "Build unit tests." OFF)

if(BUILD_TESTS)
    enable_testing()
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${Magnum_SOURCE_DIR}/modules/")

# If targetting NaCl, set explicit OpenGL ES 2.0 support
if(${CMAKE_SYSTEM_NAME} STREQUAL NaCl)
    set(TARGET_GLES 1)
    set(TARGET_GLES2 1)
    set(MAGNUM_TARGET_NACL 1)
endif()

# Check dependencies
find_package(Corrade REQUIRED)
if(NOT TARGET_GLES)
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
else()
    find_package(OpenGLES2 REQUIRED)
endif()

# Configuration variables (saved later to corradeConfigure.h)
if(TARGET_GLES)
    set(MAGNUM_TARGET_GLES 1)
endif()
if(TARGET_GLES2)
    set(MAGNUM_TARGET_GLES2 1)
endif()

# Installation paths
set(MAGNUM_BINARY_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/bin)
set(MAGNUM_LIBRARY_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
set(MAGNUM_CMAKE_MODULE_INSTALL_DIR ${CMAKE_ROOT}/Modules)
set(MAGNUM_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include/Magnum)

include_directories(${CMAKE_SOURCE_DIR}/external)

add_subdirectory(external)
add_subdirectory(modules)
add_subdirectory(src)
