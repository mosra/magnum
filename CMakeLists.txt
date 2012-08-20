# CMake 2.8.8 required for OBJECT library target
cmake_minimum_required(VERSION 2.8)
project(Magnum)

include(CMakeDependentOption)

option(TARGET_GLES "Build for OpenGL ES 2 instead of desktop OpenGL" OFF)
option(GCC44_COMPATIBILITY "Enable compatibility mode for GCC 4.4 (might disable some features)" OFF)
option(GCC45_COMPATIBILITY "Enable compatibility mode for GCC 4.5 (might disable some features)" OFF)

# Parts of the library
option(WITH_EVERYTHING "Build everything (doesn't include contexts)" ON)
option(WITH_MESHTOOLS "Build MeshTools library" OFF)
option(WITH_PHYSICS "Build Physics library" OFF)
option(WITH_PRIMITIVES "Builf Primitives library" OFF)
option(WITH_SCENEGRAPH "Build SceneGraph library" OFF)
option(WITH_SHADERS "Build Shaders library" OFF)

option(WITH_GLXCONTEXT "Build GlxContext library" OFF)
cmake_dependent_option(WITH_XEGLCONTEXT "Build XEglContext library" OFF "TARGET_GLES" OFF)
cmake_dependent_option(WITH_GLUTCONTEXT "Build GlutContext library" OFF "NOT TARGET_GLES" OFF)
option(WITH_SDL2CONTEXT "Build Sdl2Context library" OFF)

option(BUILD_TESTS "Build unit tests." OFF)

if(WITH_EVERYTHING)
    set(WITH_MESHTOOLS ON)
    set(WITH_PHYSICS ON)
    set(WITH_PRIMITIVES ON)
    set(WITH_SCENEGRAPH ON)
    set(WITH_SHADERS ON)
endif()

if(BUILD_TESTS)
    enable_testing()
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${Magnum_SOURCE_DIR}/modules/")

if(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} VERSION_LESS 2.8.8)
    set(CMAKE_NO_OBJECT_TARGET 1)
    message(WARNING "CMake version < 2.8.8 is used, compilation with tests enabled will take a lot more time.")

    cmake_policy(SET CMP0017 NEW)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${Magnum_SOURCE_DIR}/modules-compatibility/")
endif()

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
