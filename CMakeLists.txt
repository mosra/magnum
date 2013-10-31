#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>
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

# CMake 2.8.8 required for OBJECT library target
cmake_minimum_required(VERSION 2.8)
project(Magnum)

# Find Corrade first so we can check on the target
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules/")
find_package(Corrade REQUIRED)

include(CMakeDependentOption)

option(TARGET_GLES "Build for OpenGL ES instead of desktop OpenGL" OFF)
cmake_dependent_option(TARGET_GLES2 "Build for OpenGL ES 2" ON "TARGET_GLES" OFF)
cmake_dependent_option(TARGET_DESKTOP_GLES "Build for OpenGL ES on desktop" OFF "TARGET_GLES" OFF)

option(WITH_FIND_MODULE "Install FindMagnum.cmake module into CMake's module dir (might require admin privileges)" OFF)

# Parts of the library
option(WITH_AUDIO "Build Audio library" OFF)
option(WITH_DEBUGTOOLS "Build DebugTools library" ON)
cmake_dependent_option(WITH_MESHTOOLS "Build MeshTools library" ON "NOT WITH_DEBUGTOOLS" ON)
cmake_dependent_option(WITH_PRIMITIVES "Builf Primitives library" ON "NOT WITH_DEBUGTOOLS" ON)
cmake_dependent_option(WITH_SCENEGRAPH "Build SceneGraph library" ON "NOT WITH_DEBUGTOOLS;NOT WITH_SHAPES" ON)
cmake_dependent_option(WITH_SHADERS "Build Shaders library" ON "NOT WITH_DEBUGTOOLS" ON)
cmake_dependent_option(WITH_SHAPES "Build Shapes library" ON "NOT WITH_DEBUGTOOLS" ON)
option(WITH_TEXT "Build Text library" ON)
cmake_dependent_option(WITH_TEXTURETOOLS "Build TextureTools library" ON "NOT WITH_TEXT;NOT WITH_DISTANCEFIELDCONVERTER" ON)
option(WITH_MAGNUMINFO "Build magnum-info utility" OFF)
cmake_dependent_option(WITH_DISTANCEFIELDCONVERTER "Build magnum-distancefield utility" OFF "NOT TARGET_GLES" OFF)

# Application libraries
if(CORRADE_TARGET_NACL)
    option(WITH_NACLAPPLICATION "Build NaClApplication library" OFF)
    cmake_dependent_option(WITH_WINDOWLESSNACLAPPLICATION "Build WindowlessNaClApplication library" OFF "NOT WITH_MAGNUMINFO" ON)
else()
    option(WITH_GLXAPPLICATION "Build GlxApplication library" OFF)
    cmake_dependent_option(WITH_WINDOWLESSGLXAPPLICATION "Build WindowlessGlxApplication library" OFF "NOT WITH_MAGNUMINFO;NOT WITH_DISTANCEFIELDCONVERTER" ON)
    cmake_dependent_option(WITH_XEGLAPPLICATION "Build XEglApplication library" OFF "TARGET_GLES" OFF)
    cmake_dependent_option(WITH_GLUTAPPLICATION "Build GlutApplication library" OFF "NOT TARGET_GLES" OFF)
    option(WITH_SDL2APPLICATION "Build Sdl2Application library" OFF)
endif()

option(BUILD_DEPRECATED "Include deprecated API in the build" ON)
if(BUILD_DEPRECATED)
    set(MAGNUM_BUILD_DEPRECATED 1)
endif()

option(BUILD_STATIC "Build static libraries (default are shared)" OFF)
cmake_dependent_option(BUILD_STATIC_PIC "Build static libraries with position-independent code" OFF "BUILD_STATIC" OFF)
option(BUILD_TESTS "Build unit tests." OFF)
cmake_dependent_option(BUILD_GL_TESTS "Build unit tests for OpenGL code." OFF "BUILD_TESTS" OFF)
if(BUILD_TESTS)
    enable_testing()
endif()

if(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} VERSION_LESS 2.8.8)
    set(CMAKE_NO_OBJECT_TARGET 1)
    message(WARNING "CMake version < 2.8.8 is used, compilation with tests enabled will take a lot more time.")

    cmake_policy(SET CMP0017 NEW)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${Magnum_SOURCE_DIR}/modules-compatibility/")
endif()

# If targeting NaCl or Emscripten, set explicit OpenGL ES 2.0 support
if(CORRADE_TARGET_NACL OR CORRADE_TARGET_EMSCRIPTEN)
    set(TARGET_GLES 1)
    set(TARGET_GLES2 1)
endif()

# NaCl newlib toolchain supports only static linking, dynamic linking is
# meaningless on Emscripten
if(CORRADE_TARGET_NACL_NEWLIB OR CORRADE_TARGET_EMSCRIPTEN)
    set(BUILD_STATIC ON)
endif()

if(BUILD_STATIC)
    set(MAGNUM_BUILD_STATIC 1)
endif()

# Check dependencies
if(NOT TARGET_GLES OR TARGET_DESKTOP_GLES)
    find_package(OpenGL REQUIRED)
elseif(TARGET_GLES2)
    find_package(OpenGLES2 REQUIRED)
else()
    find_package(OpenGLES3 REQUIRED)
endif()

# Configuration variables (saved later to corradeConfigure.h)
if(TARGET_GLES)
    set(MAGNUM_TARGET_GLES 1)
    if(TARGET_GLES2)
        set(MAGNUM_TARGET_GLES2 1)
    else()
        set(MAGNUM_TARGET_GLES3 1)
    endif()
endif()
if(TARGET_DESKTOP_GLES)
    set(MAGNUM_TARGET_DESKTOP_GLES 1)
endif()

if(BUILD_GL_TESTS)
    if(UNIX AND (NOT MAGNUM_TARGET_GLES OR MAGNUM_TARGET_DESKTOP_GLES))
        set(WITH_WINDOWLESSGLXAPPLICATION ON)
        find_package(X11 REQUIRED)
        set(GL_TEST_LIBRARIES Magnum MagnumWindowlessGlxApplication ${X11_LIBRARIES})
    else()
        message(FATAL_ERROR "Cannot run tests for OpenGL code on this platform. Set BUILD_GL_TESTS to OFF to skip building them.")
    endif()
endif()

if(NOT BUILD_STATIC)
    set(SHARED_OR_STATIC SHARED)
else()
    set(SHARED_OR_STATIC STATIC)
endif()

# Installation paths
include(CorradeLibSuffix)
set(MAGNUM_BINARY_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/bin)
set(MAGNUM_LIBRARY_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
set(MAGNUM_DATA_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/share/magnum)
set(MAGNUM_CMAKE_FIND_MODULE_INSTALL_DIR ${CMAKE_ROOT}/Modules)
set(MAGNUM_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include/Magnum)

add_subdirectory(external)
add_subdirectory(modules)
add_subdirectory(src)
