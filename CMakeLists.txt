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

include(CMakeDependentOption)

option(TARGET_GLES "Build for OpenGL ES instead of desktop OpenGL" OFF)
cmake_dependent_option(TARGET_GLES2 "Build for OpenGL ES 2" ON "TARGET_GLES" OFF)
cmake_dependent_option(TARGET_DESKTOP_GLES "Build for OpenGL ES on desktop" OFF "TARGET_GLES" OFF)

# Parts of the library
option(WITH_DEBUGTOOLS "Build DebugTools library" ON)
cmake_dependent_option(WITH_MESHTOOLS "Build MeshTools library" ON "NOT WITH_DEBUGTOOLS" ON)
cmake_dependent_option(WITH_PHYSICS "Build Physics library" ON "NOT WITH_DEBUGTOOLS" ON)
cmake_dependent_option(WITH_PRIMITIVES "Builf Primitives library" ON "NOT WITH_DEBUGTOOLS" ON)
cmake_dependent_option(WITH_SCENEGRAPH "Build SceneGraph library" ON "NOT WITH_DEBUGTOOLS;NOT WITH_PHYSICS" ON)
cmake_dependent_option(WITH_SHADERS "Build Shaders library" ON "NOT WITH_DEBUGTOOLS" ON)
option(WITH_TEXT "Build Text library" ON)
cmake_dependent_option(WITH_TEXTURETOOLS "Build TextureTools library" ON "NOT WITH_TEXT" ON)
option(WITH_MAGNUMINFO "Build magnum-info utility" ON)

# Library features
cmake_dependent_option(USE_HARFBUZZ "Use HarfBuzz in Text library" ON "WITH_TEXT" OFF)

# Application libraries
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

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules/")

if(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} VERSION_LESS 2.8.8)
    set(CMAKE_NO_OBJECT_TARGET 1)
    message(WARNING "CMake version < 2.8.8 is used, compilation with tests enabled will take a lot more time.")

    cmake_policy(SET CMP0017 NEW)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${Magnum_SOURCE_DIR}/modules-compatibility/")
endif()

# If targeting NaCl, set explicit OpenGL ES 2.0 support
if(${CMAKE_SYSTEM_NAME} STREQUAL NaCl)
    set(TARGET_GLES 1)
    set(TARGET_GLES2 1)
    set(TARGET_NACL 1)
    set(MAGNUM_TARGET_NACL 1)
endif()

# Check dependencies
find_package(Corrade REQUIRED)
if(NOT TARGET_GLES OR TARGET_DESKTOP_GLES)
    find_package(OpenGL REQUIRED)
else()
    find_package(OpenGLES2 REQUIRED)
endif()
if(NOT TARGET_GLES)
    find_package(GLEW REQUIRED)
endif()

# Configuration variables (saved later to corradeConfigure.h)
if(TARGET_GLES)
    set(MAGNUM_TARGET_GLES 1)
endif()
if(TARGET_GLES2)
    set(MAGNUM_TARGET_GLES2 1)
endif()
if(TARGET_DESKTOP_GLES)
    set(MAGNUM_TARGET_DESKTOP_GLES 1)
endif()
if(USE_HARFBUZZ)
    set(MAGNUM_USE_HARFBUZZ 1)
endif()

# Installation paths
include(CorradeLibSuffix)
set(MAGNUM_BINARY_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/bin)
set(MAGNUM_LIBRARY_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
set(MAGNUM_CMAKE_MODULE_INSTALL_DIR ${CMAKE_ROOT}/Modules)
set(MAGNUM_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include/Magnum)

add_subdirectory(external)
add_subdirectory(modules)
add_subdirectory(src)
