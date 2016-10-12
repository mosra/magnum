#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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

cmake_minimum_required(VERSION 2.8.12)
project(Magnum)

# CMake policies: enable MACOSX_RPATH by default
if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
endif()
# Don't treat imported targets with :: as files
if(POLICY CMP0028)
    cmake_policy(SET CMP0028 NEW)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/modules/")

# Find Corrade first so we can check on the target
find_package(Corrade REQUIRED PluginManager Utility)

include(CMakeDependentOption)

# If targeting iOS, Android, NaCl, Emscripten or Windows RT, set explicit
# OpenGL ES support
if(NOT CORRADE_TARGET_IOS AND NOT CORRADE_TARGET_ANDROID AND NOT CORRADE_TARGET_EMSCRIPTEN AND NOT CORRADE_TARGET_NACL AND NOT CORRADE_TARGET_WINDOWS_RT)
    option(TARGET_GLES "Build for OpenGL ES / WebGL" OFF)
else()
    set(TARGET_GLES ON)
endif()

cmake_dependent_option(TARGET_GLES2 "Build for OpenGL ES 2 / WebGL 1.0" ON "TARGET_GLES" OFF)
cmake_dependent_option(TARGET_DESKTOP_GLES "Build for OpenGL ES on desktop" OFF "TARGET_GLES" OFF)
option(TARGET_HEADLESS "Build command-line utilities for use on a headless machines" OFF)

# Magnum Info (currently only using GLX, CGL, WGL/EGL on Windows or on NaCl)
if(CORRADE_TARGET_UNIX OR CORRADE_TARGET_NACL OR CORRADE_TARGET_WINDOWS)
    option(WITH_MAGNUMINFO "Build magnum-info utility" OFF)
endif()

# Desktop-only utilities
if(CORRADE_TARGET_UNIX OR CORRADE_TARGET_WINDOWS)
    cmake_dependent_option(WITH_FONTCONVERTER "Build magnum-fontconverter utility" OFF "NOT TARGET_GLES" OFF)
    cmake_dependent_option(WITH_DISTANCEFIELDCONVERTER "Build magnum-distancefieldconverter utility" OFF "NOT TARGET_GLES" OFF)
endif()

# API-independent utilities
option(WITH_IMAGECONVERTER "Build magnum-imageconverter utility" OFF)

# Plugins
option(WITH_WAVAUDIOIMPORTER "Build WavAudioImporter plugin" OFF)
option(WITH_MAGNUMFONT "Build MagnumFont plugin" OFF)
cmake_dependent_option(WITH_MAGNUMFONTCONVERTER "Build MagnumFontConverter plugin" OFF "NOT TARGET_GLES" OFF)
option(WITH_OBJIMPORTER "Build ObjImporter plugin" OFF)
cmake_dependent_option(WITH_TGAIMAGECONVERTER "Build TgaImageConverter plugin" OFF "NOT WITH_MAGNUMFONTCONVERTER" ON)
cmake_dependent_option(WITH_TGAIMPORTER "Build TgaImporter plugin" OFF "NOT WITH_MAGNUMFONT" ON)

# Parts of the library
cmake_dependent_option(WITH_AUDIO "Build Audio library" OFF "NOT WITH_WAVAUDIOIMPORTER" ON)
option(WITH_DEBUGTOOLS "Build DebugTools library" ON)
cmake_dependent_option(WITH_MESHTOOLS "Build MeshTools library" ON "NOT WITH_DEBUGTOOLS;NOT WITH_OBJIMPORTER" ON)
cmake_dependent_option(WITH_PRIMITIVES "Builf Primitives library" ON "NOT WITH_DEBUGTOOLS" ON)
option(WITH_SHAPES "Build Shapes library" ON)
cmake_dependent_option(WITH_SCENEGRAPH "Build SceneGraph library" ON "NOT WITH_SHAPES" ON)
cmake_dependent_option(WITH_SHADERS "Build Shaders library" ON "NOT WITH_DEBUGTOOLS" ON)
cmake_dependent_option(WITH_TEXT "Build Text library" ON "NOT WITH_MAGNUMFONT;NOT WITH_MAGNUMFONTCONVERTER" ON)
cmake_dependent_option(WITH_TEXTURETOOLS "Build TextureTools library" ON "NOT WITH_TEXT;NOT WITH_DISTANCEFIELDCONVERTER" ON)

# Magnum AL Info
cmake_dependent_option(WITH_AL_INFO "Build magnum-al-info utility" OFF "WITH_AUDIO" OFF)

# EGL context and windowless EGL application, available everywhere except on
# platforms which don't support extension loading
if(NOT CORRADE_TARGET_EMSCRIPTEN AND NOT CORRADE_TARGET_NACL)
    option(WITH_EGLCONTEXT "Build EglContext library" OFF)
    option(WITH_WINDOWLESSEGLAPPLICATION "Build WindowlessEglApplication library" OFF)
endif()

# NaCl-specific application libraries
if(CORRADE_TARGET_NACL)
    option(WITH_NACLAPPLICATION "Build NaClApplication library" OFF)
    cmake_dependent_option(WITH_WINDOWLESSNACLAPPLICATION "Build WindowlessNaClApplication library" OFF "NOT WITH_MAGNUMINFO" ON)

# Android-specific application libraries
elseif(CORRADE_TARGET_ANDROID)
    option(WITH_ANDROIDAPPLICATION "Build AndroidApplication library" OFF)

# iOS-specific application libraries
elseif(CORRADE_TARGET_IOS)
    option(WITH_WINDOWLESSIOSAPPLICATION "Build WindowlessIosApplication library" OFF)

# OS X-specific application libraries
elseif(CORRADE_TARGET_APPLE)
    cmake_dependent_option(WITH_WINDOWLESSCGLAPPLICATION "Build WindowlessCglApplication library" OFF "NOT WITH_MAGNUMINFO;NOT WITH_FONTCONVERTER;NOT WITH_DISTANCEFIELDCONVERTER" ON)
    option(WITH_CGLCONTEXT "Build CglContext library" OFF)

# X11 + GLX/EGL-specific application libraries
elseif(CORRADE_TARGET_UNIX)
    option(WITH_GLXAPPLICATION "Build GlxApplication library" OFF)
    cmake_dependent_option(WITH_WINDOWLESSGLXAPPLICATION "Build WindowlessGlxApplication library" OFF "NOT WITH_MAGNUMINFO;NOT WITH_FONTCONVERTER;NOT WITH_DISTANCEFIELDCONVERTER" ON)
    option(WITH_XEGLAPPLICATION "Build XEglApplication library" OFF)
    option(WITH_GLXCONTEXT "Build GlxContext library" OFF)

# Windows-specific application libraries
elseif(CORRADE_TARGET_WINDOWS)
    if(NOT TARGET_GLES OR TARGET_DESKTOP_GLES)
        cmake_dependent_option(WITH_WINDOWLESSWGLAPPLICATION "Build WindowlessWglApplication library" OFF "NOT WITH_MAGNUMINFO;NOT WITH_FONTCONVERTER;NOT WITH_DISTANCEFIELDCONVERTER" ON)
        option(WITH_WGLCONTEXT "Build WglContext library" OFF)
    else()
        cmake_dependent_option(WITH_WINDOWLESSWINDOWSEGLAPPLICATION "Build WindowlessWindowsEglApplication library" OFF "NOT WITH_MAGNUMINFO;NOT WITH_FONTCONVERTER;NOT WITH_DISTANCEFIELDCONVERTER" ON)
    endif()
endif()

# Platform-independent (almost) application libraries
if(NOT CORRADE_TARGET_NACL AND NOT CORRADE_TARGET_ANDROID)
    option(WITH_GLFWAPPLICATION "Build GlfwApplication library")
    cmake_dependent_option(WITH_GLUTAPPLICATION "Build GlutApplication library" OFF "NOT TARGET_GLES" OFF)
    option(WITH_SDL2APPLICATION "Build Sdl2Application library" OFF)
endif()

option(BUILD_DEPRECATED "Include deprecated API in the build" ON)
if(BUILD_DEPRECATED)
    set(MAGNUM_BUILD_DEPRECATED 1)
endif()

option(BUILD_MULTITHREADED "Build in a way that makes it possible to use multiple thread-local Magnum contexts" ON)
if(BUILD_MULTITHREADED)
    set(MAGNUM_BUILD_MULTITHREADED 1)
endif()

option(BUILD_STATIC "Build static libraries (default are shared)" OFF)
option(BUILD_STATIC_PIC "Build static libraries and plugins with position-independent code" ON)
option(BUILD_PLUGINS_STATIC "Build static plugins (default are dynamic)" OFF)
option(BUILD_TESTS "Build unit tests" OFF)
cmake_dependent_option(BUILD_GL_TESTS "Build unit tests for OpenGL code" OFF "BUILD_TESTS" OFF)
cmake_dependent_option(BUILD_AL_TESTS "Build unit tests for OpenAL code" ON "BUILD_TESTS;WITH_AUDIO" OFF)
if(BUILD_TESTS)
    find_package(Corrade REQUIRED TestSuite)
    if(CORRADE_TARGET_IOS)
        set(CORRADE_TESTSUITE_BUNDLE_IDENTIFIER_PREFIX "cz.mosra.magnum")
    endif()
    enable_testing()
endif()

# If targeting NaCl, set explicit OpenGL ES 2.0 support. For Android and
# Emscripten the decision between ES 2.0 / WebGL 1.0 and ES 3.0 / WebGL 2.0
# must be done by the user.
if(CORRADE_TARGET_NACL)
    set(TARGET_GLES2 ON)
endif()

# NaCl newlib toolchain supports only static linking, dynamic linking is
# meaningless on Emscripten and too inconvenient on Android
if(CORRADE_TARGET_NACL_NEWLIB OR CORRADE_TARGET_EMSCRIPTEN OR CORRADE_TARGET_ANDROID)
    set(BUILD_STATIC ON)
    set(BUILD_PLUGINS_STATIC ON)
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

# Configuration variables (saved later to configure.h)
if(TARGET_GLES)
    set(MAGNUM_TARGET_GLES 1)
    if(TARGET_GLES2)
        set(MAGNUM_TARGET_GLES2 1)
    else()
        set(MAGNUM_TARGET_GLES3 1)
    endif()
endif()
if(CORRADE_TARGET_EMSCRIPTEN)
    set(TARGET_WEBGL 1)
    set(MAGNUM_TARGET_WEBGL 1)
endif()
if(TARGET_DESKTOP_GLES)
    set(MAGNUM_TARGET_DESKTOP_GLES 1)
endif()
if(TARGET_HEADLESS)
    set(MAGNUM_TARGET_HEADLESS 1)
endif()

if(CORRADE_TARGET_EMSCRIPTEN)
    include(UseEmscripten)
endif()

if(BUILD_GL_TESTS)
    if(MAGNUM_TARGET_HEADLESS)
        set(WITH_WINDOWLESSEGLAPPLICATION ON)
        set(GL_TEST_LIBRARIES Magnum MagnumWindowlessEglApplication)
    elseif(CORRADE_TARGET_IOS)
        set(WITH_WINDOWLESSIOSAPPLICATION ON)
        set(GL_TEST_LIBRARIES Magnum MagnumWindowlessIosApplication)
    elseif(CORRADE_TARGET_APPLE)
        set(WITH_WINDOWLESSCGLAPPLICATION ON)
        set(GL_TEST_LIBRARIES Magnum MagnumWindowlessCglApplication)
    elseif(CORRADE_TARGET_UNIX)
        if(MAGNUM_TARGET_GLES AND NOT MAGNUM_TARGET_DESKTOP_GLES)
            set(WITH_WINDOWLESSEGLAPPLICATION ON)
            set(GL_TEST_LIBRARIES Magnum MagnumWindowlessEglApplication)
        else()
            set(WITH_WINDOWLESSGLXAPPLICATION ON)
            set(GL_TEST_LIBRARIES Magnum MagnumWindowlessGlxApplication)
        endif()
    elseif(CORRADE_TARGET_WINDOWS)
        if(NOT MAGNUM_TARGET_GLES OR MAGNUM_TARGET_DESKTOP_GLES)
            set(WITH_WINDOWLESSWGLAPPLICATION ON)
            set(GL_TEST_LIBRARIES Magnum MagnumWindowlessWglApplication)
        else()
            set(WITH_WINDOWLESSWINDOWSEGLAPPLICATION ON)
            set(GL_TEST_LIBRARIES Magnum MagnumWindowlessWindowsEglApplication)
        endif()
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
include(${CORRADE_LIB_SUFFIX_MODULE})
set(MAGNUM_BINARY_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/bin)
set(MAGNUM_LIBRARY_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
set(MAGNUM_DATA_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/share/magnum)
set(MAGNUM_CMAKE_MODULE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/share/cmake/Magnum)
set(MAGNUM_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include/Magnum)
set(MAGNUM_EXTERNAL_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include/MagnumExternal)
set(MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include/MagnumPlugins)

# Separate install dirs for debug and release plugins
set(MAGNUM_PLUGINS_DEBUG_BINARY_INSTALL_DIR ${MAGNUM_BINARY_INSTALL_DIR}/magnum-d)
set(MAGNUM_PLUGINS_DEBUG_LIBRARY_INSTALL_DIR ${MAGNUM_LIBRARY_INSTALL_DIR}/magnum-d)
set(MAGNUM_PLUGINS_RELEASE_BINARY_INSTALL_DIR ${MAGNUM_BINARY_INSTALL_DIR}/magnum)
set(MAGNUM_PLUGINS_RELEASE_LIBRARY_INSTALL_DIR ${MAGNUM_LIBRARY_INSTALL_DIR}/magnum)

set(MAGNUM_PLUGINS_FONT_DEBUG_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_BINARY_INSTALL_DIR}/fonts)
set(MAGNUM_PLUGINS_FONT_DEBUG_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_LIBRARY_INSTALL_DIR}/fonts)
set(MAGNUM_PLUGINS_FONT_RELEASE_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_BINARY_INSTALL_DIR}/fonts)
set(MAGNUM_PLUGINS_FONT_RELEASE_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_LIBRARY_INSTALL_DIR}/fonts)
set(MAGNUM_PLUGINS_FONTCONVERTER_DEBUG_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_BINARY_INSTALL_DIR}/fontconverters)
set(MAGNUM_PLUGINS_FONTCONVERTER_DEBUG_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_LIBRARY_INSTALL_DIR}/fontconverters)
set(MAGNUM_PLUGINS_FONTCONVERTER_RELEASE_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_BINARY_INSTALL_DIR}/fontconverters)
set(MAGNUM_PLUGINS_FONTCONVERTER_RELEASE_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_LIBRARY_INSTALL_DIR}/fontconverters)
set(MAGNUM_PLUGINS_IMAGECONVERTER_DEBUG_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_BINARY_INSTALL_DIR}/imageconverters)
set(MAGNUM_PLUGINS_IMAGECONVERTER_DEBUG_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_LIBRARY_INSTALL_DIR}/imageconverters)
set(MAGNUM_PLUGINS_IMAGECONVERTER_RELEASE_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_BINARY_INSTALL_DIR}/imageconverters)
set(MAGNUM_PLUGINS_IMAGECONVERTER_RELEASE_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_LIBRARY_INSTALL_DIR}/imageconverters)
set(MAGNUM_PLUGINS_IMPORTER_DEBUG_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_BINARY_INSTALL_DIR}/importers)
set(MAGNUM_PLUGINS_IMPORTER_DEBUG_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_LIBRARY_INSTALL_DIR}/importers)
set(MAGNUM_PLUGINS_IMPORTER_RELEASE_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_BINARY_INSTALL_DIR}/importers)
set(MAGNUM_PLUGINS_IMPORTER_RELEASE_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_LIBRARY_INSTALL_DIR}/importers)
set(MAGNUM_PLUGINS_AUDIOIMPORTER_DEBUG_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_BINARY_INSTALL_DIR}/audioimporters)
set(MAGNUM_PLUGINS_AUDIOIMPORTER_DEBUG_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_LIBRARY_INSTALL_DIR}/audioimporters)
set(MAGNUM_PLUGINS_AUDIOIMPORTER_RELEASE_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_BINARY_INSTALL_DIR}/audioimporters)
set(MAGNUM_PLUGINS_AUDIOIMPORTER_RELEASE_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_LIBRARY_INSTALL_DIR}/audioimporters)

# Plugin dirs based on wheter we are in debug or release build, needed by some
# command-line tools
if(CORRADE_TARGET_WINDOWS)
    set(MAGNUM_PLUGINS_DEBUG_DIR_INIT ${MAGNUM_PLUGINS_DEBUG_BINARY_INSTALL_DIR})
    set(MAGNUM_PLUGINS_RELEASE_DIR_INIT ${MAGNUM_PLUGINS_RELEASE_BINARY_INSTALL_DIR})
else()
    set(MAGNUM_PLUGINS_DEBUG_DIR_INIT ${MAGNUM_PLUGINS_DEBUG_LIBRARY_INSTALL_DIR})
    set(MAGNUM_PLUGINS_RELEASE_DIR_INIT ${MAGNUM_PLUGINS_RELEASE_LIBRARY_INSTALL_DIR})
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(MAGNUM_PLUGINS_DIR_INIT ${MAGNUM_PLUGINS_DEBUG_DIR_INIT})
else()
    set(MAGNUM_PLUGINS_DIR_INIT ${MAGNUM_PLUGINS_RELEASE_DIR_INIT})
endif()

# Make these paths configurable from outside
set(MAGNUM_PLUGINS_DEBUG_DIR ${MAGNUM_PLUGINS_DEBUG_DIR_INIT}
    CACHE PATH "Base directory where to look for Magnum plugins for debug builds")
set(MAGNUM_PLUGINS_RELEASE_DIR ${MAGNUM_PLUGINS_RELEASE_DIR_INIT}
    CACHE PATH "Base directory where to look for Magnum plugins for release builds")
set(MAGNUM_PLUGINS_DIR ${MAGNUM_PLUGINS_DIR_INIT}
    CACHE PATH "Base directory where to look for Magnum plugins")

add_subdirectory(modules)
add_subdirectory(src)
