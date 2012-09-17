# CMake 2.8.8 required for OBJECT library target
cmake_minimum_required(VERSION 2.8.8)
project(Magnum)

include(CMakeDependentOption)

option(TARGET_GLES "Build for OpenGL ES instead of desktop OpenGL" OFF)
cmake_dependent_option(TARGET_GLES2 "Build for OpenGL ES 2" ON "TARGET_GLES" OFF)

# Parts of the library
option(WITH_EVERYTHING "Build everything (doesn't include contexts)" ON)
cmake_dependent_option(WITH_MESHTOOLS "Build MeshTools library" OFF "NOT WITH_EVERYTHING" ON)
cmake_dependent_option(WITH_PHYSICS "Build Physics library" OFF "NOT WITH_EVERYTHING" ON)
cmake_dependent_option(WITH_PRIMITIVES "Builf Primitives library" OFF "NOT WITH_EVERYTHING" ON)
cmake_dependent_option(WITH_SCENEGRAPH "Build SceneGraph library" OFF "NOT WITH_EVERYTHING" ON)
cmake_dependent_option(WITH_SHADERS "Build Shaders library" OFF "NOT WITH_EVERYTHING" ON)

option(WITH_GLXWINDOWCONTEXT "Build GlxWindowContext library" OFF)
cmake_dependent_option(WITH_XEGLWINDOWCONTEXT "Build XEglWindowContext library" OFF "TARGET_GLES" OFF)
cmake_dependent_option(WITH_GLUTWINDOWCONTEXT "Build GlutWindowContext library" OFF "NOT TARGET_GLES" OFF)
option(WITH_SDL2WINDOWCONTEXT "Build Sdl2WindowContext library" OFF)

option(BUILD_TESTS "Build unit tests." OFF)

if(BUILD_TESTS)
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

include_directories(${CMAKE_SOURCE_DIR}/external)

add_subdirectory(external)
add_subdirectory(modules)
add_subdirectory(src)
