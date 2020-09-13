#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020 Vladimír Vondruš <mosra@centrum.cz>
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

cmake_minimum_required(VERSION 3.4)

option(HUNTER_ENABLED "Get dependencies (Corrade, GLFW, SDL2, ...) via Hunter" OFF)
if(HUNTER_ENABLED)
    include(${CMAKE_CURRENT_LIST_DIR}/package/hunter/HunterInit.cmake)
endif()

project(Magnum CXX)

if(HUNTER_ENABLED)
    include(${CMAKE_CURRENT_LIST_DIR}/package/hunter/HunterAddPackages.cmake)
endif()

# Use folders for nice tree in Visual Studio and XCode
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/modules/" ${CMAKE_MODULE_PATH})

# Find Corrade first so we can check on the target
find_package(Corrade REQUIRED Utility)

include(CMakeDependentOption)

# If targeting iOS, Android, Emscripten or Windows RT, set explicit OpenGL ES
# support
if(NOT CORRADE_TARGET_IOS AND NOT CORRADE_TARGET_ANDROID AND NOT CORRADE_TARGET_EMSCRIPTEN AND NOT CORRADE_TARGET_WINDOWS_RT)
    option(TARGET_GLES "Build for OpenGL ES / WebGL" OFF)
else()
    set(TARGET_GLES ON)
endif()

cmake_dependent_option(TARGET_GLES2 "Build for OpenGL ES 2 / WebGL 1.0" ON "TARGET_GLES" OFF)
cmake_dependent_option(TARGET_DESKTOP_GLES "Build for OpenGL ES on desktop" OFF "TARGET_GLES" OFF)

# Magnum GL Info (currently only using GLX/CGL/EGL on *nix, WGL/EGL on Windows
# and EGL on Emscripten)
if(CORRADE_TARGET_UNIX OR CORRADE_TARGET_WINDOWS OR CORRADE_TARGET_EMSCRIPTEN)
    option(WITH_GL_INFO "Build magnum-gl-info utility" OFF)
endif()

# Desktop-only utilities
if(CORRADE_TARGET_UNIX OR CORRADE_TARGET_WINDOWS)
    cmake_dependent_option(WITH_FONTCONVERTER "Build magnum-fontconverter utility" OFF "NOT TARGET_GLES" OFF)
    cmake_dependent_option(WITH_DISTANCEFIELDCONVERTER "Build magnum-distancefieldconverter utility" OFF "NOT TARGET_GLES" OFF)
endif()

# API-independent utilities
option(WITH_IMAGECONVERTER "Build magnum-imageconverter utility" OFF)
option(WITH_SCENECONVERTER "Build magnum-sceneconverter utility" OFF)
option(WITH_SHADERCONVERTER "Build magnum-shaderconverter utility" OFF)

# Magnum AL Info
option(WITH_AL_INFO "Build magnum-al-info utility" OFF)

# Magnum Vk Info
option(WITH_VK_INFO "Build magnum-vk-info utility" OFF)

# Plugins
option(WITH_ANYIMAGEIMPORTER "Build AnyImageImporter plugin" OFF)
option(WITH_ANYAUDIOIMPORTER "Build AnyAudioImporter plugin" OFF)
option(WITH_ANYIMAGECONVERTER "Build AnyImageConverter plugin" OFF)
option(WITH_ANYSCENECONVERTER "Build AnySceneConverter plugin" OFF)
option(WITH_ANYSCENEIMPORTER "Build AnySceneImporter plugin" OFF)
option(WITH_ANYSHADERCONVERTER "Build AnyShaderConverter plugin" OFF)
option(WITH_WAVAUDIOIMPORTER "Build WavAudioImporter plugin" OFF)
option(WITH_MAGNUMFONT "Build MagnumFont plugin" OFF)
option(WITH_MAGNUMFONTCONVERTER "Build MagnumFontConverter plugin" OFF)
option(WITH_OBJIMPORTER "Build ObjImporter plugin" OFF)
cmake_dependent_option(WITH_TGAIMAGECONVERTER "Build TgaImageConverter plugin" OFF "NOT WITH_MAGNUMFONTCONVERTER" ON)
cmake_dependent_option(WITH_TGAIMPORTER "Build TgaImporter plugin" OFF "NOT WITH_MAGNUMFONT" ON)

# Parts of the library
cmake_dependent_option(WITH_AUDIO "Build Audio library" OFF "NOT WITH_AL_INFO;NOT WITH_ANYAUDIOIMPORTER;NOT WITH_WAVAUDIOIMPORTER" ON)
option(WITH_DEBUGTOOLS "Build DebugTools library" ON)
cmake_dependent_option(WITH_MESHTOOLS "Build MeshTools library" ON "NOT WITH_OBJIMPORTER;NOT WITH_SCENECONVERTER" ON)
option(WITH_SCENEGRAPH "Build SceneGraph library" ON)
option(WITH_SHADERS "Build Shaders library" ON)
cmake_dependent_option(WITH_SHADERTOOLS "Build ShaderTools library" ON "NOT WITH_SHADERCONVERTER" ON)
cmake_dependent_option(WITH_TEXT "Build Text library" ON "NOT WITH_FONTCONVERTER;NOT WITH_MAGNUMFONT;NOT WITH_MAGNUMFONTCONVERTER" ON)
cmake_dependent_option(WITH_TEXTURETOOLS "Build TextureTools library" ON "NOT WITH_TEXT;NOT WITH_DISTANCEFIELDCONVERTER" ON)
cmake_dependent_option(WITH_TRADE "Build Trade library" ON "NOT WITH_MESHTOOLS;NOT WITH_PRIMITIVES;NOT WITH_IMAGECONVERTER;NOT WITH_ANYIMAGEIMPORTER;NOT WITH_ANYIMAGECONVERTER;NOT WITH_ANYSCENEIMPORTER;NOT WITH_OBJIMPORTER;NOT WITH_TGAIMAGECONVERTER;NOT WITH_TGAIMPORTER" ON)
cmake_dependent_option(WITH_GL "Build GL library" ON "NOT WITH_SHADERS;NOT WITH_GL_INFO;NOT WITH_ANDROIDAPPLICATION;NOT WITH_WINDOWLESSIOSAPPLICATION;NOT WITH_CGLCONTEXT;NOT WITH_GLXAPPLICATION;NOT WITH_GLXCONTEXT;NOT WITH_XEGLAPPLICATION;NOT WITH_WINDOWLESSWGLAPPLICATION;NOT WITH_WGLCONTEXT;NOT WITH_WINDOWLESSWINDOWSEGLAPPLICATION;NOT WITH_DISTANCEFIELDCONVERTER" ON)
option(WITH_PRIMITIVES "Builf Primitives library" ON)

cmake_dependent_option(TARGET_HEADLESS "Build command-line utilities for use on a headless machines" OFF "WITH_GL" OFF)
cmake_dependent_option(TARGET_GL "Build libraries with OpenGL interoperability" ON "WITH_GL" OFF)

# EGL context and windowless EGL application, available everywhere
cmake_dependent_option(WITH_WINDOWLESSEGLAPPLICATION "Build WindowlessEglApplication library" OFF "NOT TARGET_GLES OR TARGET_DESKTOP_GLES OR NOT WITH_GL_INFO;NOT TARGET_HEADLESS" ON)
option(WITH_EGLCONTEXT "Build EglContext library" OFF)

# Vulkan, everywhere except Emscripten
if(NOT CORRADE_TARGET_EMSCRIPTEN)
    cmake_dependent_option(WITH_VK "Build Vk library" OFF "NOT WITH_VK_INFO" ON)
    cmake_dependent_option(TARGET_VK "Build libraries with Vulkan interoperability" ON "WITH_VK" OFF)
endif()

# Android-specific application libraries
if(CORRADE_TARGET_ANDROID)
    option(WITH_ANDROIDAPPLICATION "Build AndroidApplication library" OFF)

# Emscripten-specific application libraries
elseif(CORRADE_TARGET_EMSCRIPTEN)
    option(WITH_EMSCRIPTENAPPLICATION "Build EmscriptenApplication library" OFF)

# iOS-specific application libraries
elseif(CORRADE_TARGET_IOS)
    option(WITH_WINDOWLESSIOSAPPLICATION "Build WindowlessIosApplication library" OFF)

# macOS-specific application libraries
elseif(CORRADE_TARGET_APPLE)
    cmake_dependent_option(WITH_WINDOWLESSCGLAPPLICATION "Build WindowlessCglApplication library" OFF "NOT WITH_GL_INFO;NOT WITH_FONTCONVERTER;NOT WITH_DISTANCEFIELDCONVERTER" ON)
    option(WITH_CGLCONTEXT "Build CglContext library" OFF)

# X11 + GLX/EGL-specific application libraries
elseif(CORRADE_TARGET_UNIX)
    option(WITH_GLXAPPLICATION "Build GlxApplication library" OFF)
    if(NOT TARGET_GLES OR TARGET_DESKTOP_GLES)
        cmake_dependent_option(WITH_WINDOWLESSGLXAPPLICATION "Build WindowlessGlxApplication library" OFF "NOT WITH_GL_INFO;NOT WITH_FONTCONVERTER;NOT WITH_DISTANCEFIELDCONVERTER" ON)
        option(WITH_GLXCONTEXT "Build GlxContext library" OFF)
    endif()
    option(WITH_XEGLAPPLICATION "Build XEglApplication library" OFF)

# Windows-specific application libraries
elseif(CORRADE_TARGET_WINDOWS)
    if(NOT TARGET_GLES OR TARGET_DESKTOP_GLES)
        cmake_dependent_option(WITH_WINDOWLESSWGLAPPLICATION "Build WindowlessWglApplication library" OFF "NOT WITH_GL_INFO;NOT WITH_FONTCONVERTER;NOT WITH_DISTANCEFIELDCONVERTER" ON)
        option(WITH_WGLCONTEXT "Build WglContext library" OFF)
    else()
        cmake_dependent_option(WITH_WINDOWLESSWINDOWSEGLAPPLICATION "Build WindowlessWindowsEglApplication library" OFF "NOT WITH_GL_INFO;NOT WITH_FONTCONVERTER;NOT WITH_DISTANCEFIELDCONVERTER" ON)
    endif()
endif()

# Platform-independent (almost) application libraries
if(NOT CORRADE_TARGET_ANDROID)
    option(WITH_SDL2APPLICATION "Build Sdl2Application library" OFF)
endif()
if(NOT CORRADE_TARGET_ANDROID AND NOT CORRADE_TARGET_IOS AND NOT CORRADE_TARGET_EMSCRIPTEN)
    option(WITH_GLFWAPPLICATION "Build GlfwApplication library" OFF)
endif()

option(BUILD_DEPRECATED "Include deprecated API in the build" ON)
if(BUILD_DEPRECATED)
    set(MAGNUM_BUILD_DEPRECATED 1)
endif()

# BUILD_MULTITHREADED got moved to Corrade itself. In case we're building with
# deprecated features enabled, print a warning in case it's set but Corrade
# reports a different value. We can't print a warning in case it's set because
# that would cause false positives when both Corrade and Magnum are subprojects
# (and thus this option is visible to both). Otherwise it's silent --- for
# non-deprecated builds CMake will at most warn about "variable being unused".
if(MAGNUM_BUILD_DEPRECATED)
    if(DEFINED BUILD_MULTITHREADED AND ((NOT CORRADE_BUILD_MULTITHREADED AND BUILD_MULTITHREADED) OR (CORRADE_BUILD_MULTITHREADED AND NOT BUILD_MULTITHREADED)))
        message(DEPRECATION "BUILD_MULTITHREADED (set to ${BUILD_MULTITHREADED}) is now ignored — you need to set it when building Corrade instead (there it's ${CORRADE_BUILD_MULTITHREADED} now)")
    endif()
endif()

set(MAGNUM_DEPLOY_PREFIX "."
    CACHE STRING "Prefix where to put final application executables")
set(MAGNUM_INCLUDE_INSTALL_PREFIX "."
    CACHE STRING "Prefix where to put platform-independent include and other files")

option(BUILD_STATIC "Build static libraries (default are shared)" OFF)
# Disable PIC on Emscripten by default (but still allow it to be enabled
# explicitly if one so desires). Currently causes linker errors related to
# __memory_base etc.: https://github.com/emscripten-core/emscripten/issues/8761
if(CORRADE_TARGET_EMSCRIPTEN)
    set(ON_EXCEPT_EMSCRIPTEN OFF)
else()
    set(ON_EXCEPT_EMSCRIPTEN ON)
endif()
option(BUILD_STATIC_PIC "Build static libraries and plugins with position-independent code" ${ON_EXCEPT_EMSCRIPTEN})
cmake_dependent_option(BUILD_STATIC_UNIQUE_GLOBALS "Build static libraries with globals unique across shared libraries" ${ON_EXCEPT_EMSCRIPTEN} "BUILD_STATIC" OFF)
option(BUILD_PLUGINS_STATIC "Build static plugins (default are dynamic)" OFF)
option(BUILD_TESTS "Build unit tests" OFF)
cmake_dependent_option(BUILD_GL_TESTS "Build unit tests for OpenGL code" OFF "BUILD_TESTS;TARGET_GL" OFF)
cmake_dependent_option(BUILD_AL_TESTS "Build unit tests for OpenAL code" ON "BUILD_TESTS;WITH_AUDIO" OFF)
cmake_dependent_option(BUILD_VK_TESTS "Build unit tests for Vulkan code" OFF "BUILD_TESTS;TARGET_VK" OFF)
if(BUILD_TESTS)
    find_package(Corrade REQUIRED TestSuite)
    if(CORRADE_TARGET_IOS)
        set(CORRADE_TESTSUITE_BUNDLE_IDENTIFIER_PREFIX "cz.mosra.magnum")
    endif()
    enable_testing()

    # For experimental Gradle-less Android APK creation. Only with CMake > 3.7
    # that has builtin Android support -- Gradle uses 3.6 so this will nicely
    # work in both cases.
    if(CORRADE_TARGET_ANDROID AND CMAKE_ANDROID_NDK)
        include(${PROJECT_SOURCE_DIR}/toolchains/modules/UseAndroid.cmake)
    endif()
endif()

# OpenGLTester / VulkanTester libraries, built by default only if GL / VK tests
# are enabled
cmake_dependent_option(WITH_OPENGLTESTER "Build OpenGLTester library" OFF "NOT BUILD_GL_TESTS" ON)
cmake_dependent_option(WITH_VULKANTESTER "Build VulkanTester library" OFF "NOT BUILD_VK_TESTS" ON)

# Dynamic linking is meaningless on Emscripten and too inconvenient on Android
if(CORRADE_TARGET_EMSCRIPTEN OR CORRADE_TARGET_ANDROID)
    set(BUILD_STATIC ON)
    set(BUILD_PLUGINS_STATIC ON)
# Dynamic plugins are not working on iOS or Windows RT at the moment
elseif(CORRADE_TARGET_IOS OR CORRADE_TARGET_WINDOWS_RT)
    set(BUILD_PLUGINS_STATIC ON)
endif()

if(BUILD_STATIC)
    set(MAGNUM_BUILD_STATIC 1)
    if(BUILD_STATIC_UNIQUE_GLOBALS)
        set(MAGNUM_BUILD_STATIC_UNIQUE_GLOBALS 1)
    endif()
endif()

# Check dependencies
if(WITH_GL)
    if(NOT TARGET_GLES OR TARGET_DESKTOP_GLES)
        # OpenGL library preference. Prefer to use GLVND, since that's the
        # better approach nowadays, but allow the users to override it from
        # outside in case it is broken for some reason (Nvidia drivers in
        # Debian's testing (Buster) -- reported on 2019-04-09).
        if(NOT CMAKE_VERSION VERSION_LESS 3.10 AND NOT OpenGL_GL_PREFERENCE)
            set(OpenGL_GL_PREFERENCE GLVND)
        endif()
        find_package(OpenGL REQUIRED)
    elseif(TARGET_GLES2)
        find_package(OpenGLES2 REQUIRED)
    else()
        find_package(OpenGLES3 REQUIRED)
    endif()
else()
    if(TARGET_GL)
        message(FATAL_ERROR "WITH_GL is OFF but TARGET_GL is enabled. Hmm.")
    endif()
    # These are implicitly enabled for embedded platforms, disable them all for
    # consistency
    set(TARGET_GLES OFF)
    set(TARGET_GLES2 OFF)
    set(TARGET_DESKTOP_GLES OFF)
endif()

# Configuration variables (saved later to configure.h)
if(TARGET_GL)
    set(MAGNUM_TARGET_GL 1)
endif()
if(TARGET_GLES)
    set(MAGNUM_TARGET_GLES 1)
    if(TARGET_GLES2)
        set(MAGNUM_TARGET_GLES2 1)
    else()
        set(MAGNUM_TARGET_GLES3 1)
    endif()
    if(CORRADE_TARGET_EMSCRIPTEN)
        set(TARGET_WEBGL 1)
        set(MAGNUM_TARGET_WEBGL 1)
    endif()
    if(TARGET_DESKTOP_GLES)
        set(MAGNUM_TARGET_DESKTOP_GLES 1)
    endif()
endif()
if(TARGET_HEADLESS)
    set(MAGNUM_TARGET_HEADLESS 1)
endif()
if(TARGET_VK)
    set(MAGNUM_TARGET_VK 1)
endif()

if(CORRADE_TARGET_EMSCRIPTEN)
    include(UseEmscripten)
endif()

if(WITH_OPENGLTESTER)
    if(MAGNUM_TARGET_HEADLESS OR CORRADE_TARGET_EMSCRIPTEN OR CORRADE_TARGET_ANDROID)
        set(WITH_WINDOWLESSEGLAPPLICATION ON)
        set(OPENGLTESTER_APPLICATION MagnumWindowlessEglApplication)
    elseif(CORRADE_TARGET_IOS)
        set(WITH_WINDOWLESSIOSAPPLICATION ON)
        set(OPENGLTESTER_APPLICATION MagnumWindowlessIosApplication)
    elseif(CORRADE_TARGET_APPLE)
        set(WITH_WINDOWLESSCGLAPPLICATION ON)
        set(OPENGLTESTER_APPLICATION MagnumWindowlessCglApplication)
    elseif(CORRADE_TARGET_UNIX)
        if(MAGNUM_TARGET_GLES AND NOT MAGNUM_TARGET_DESKTOP_GLES)
            set(WITH_WINDOWLESSEGLAPPLICATION ON)
            set(OPENGLTESTER_APPLICATION MagnumWindowlessEglApplication)
        else()
            set(WITH_WINDOWLESSGLXAPPLICATION ON)
            set(OPENGLTESTER_APPLICATION MagnumWindowlessGlxApplication)
        endif()
    elseif(CORRADE_TARGET_WINDOWS)
        if(NOT MAGNUM_TARGET_GLES OR MAGNUM_TARGET_DESKTOP_GLES)
            set(WITH_WINDOWLESSWGLAPPLICATION ON)
            set(OPENGLTESTER_APPLICATION MagnumWindowlessWglApplication)
        else()
            set(WITH_WINDOWLESSWINDOWSEGLAPPLICATION ON)
            set(OPENGLTESTER_APPLICATION MagnumWindowlessWindowsEglApplication)
        endif()
    else()
        # Assuming this gets hit only if BUILD_GL_TESTS are enabled
        message(FATAL_ERROR "Cannot run tests for OpenGL code on this platform. Set BUILD_GL_TESTS to OFF to skip building them.")
    endif()
endif()

if(NOT BUILD_STATIC)
    set(SHARED_OR_STATIC SHARED)
else()
    set(SHARED_OR_STATIC STATIC)
endif()

# If we're in a CMake subproject, find_package(Magnum) will be looking for
# these when targeting Emscripten, so supply their in-source location
if(CORRADE_TARGET_EMSCRIPTEN AND NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(MAGNUM_EMSCRIPTENAPPLICATION_JS ${PROJECT_SOURCE_DIR}/src/Magnum/Platform/EmscriptenApplication.js CACHE INTERNAL "" FORCE)
    set(MAGNUM_WINDOWLESSEMSCRIPTENAPPLICATION_JS ${PROJECT_SOURCE_DIR}/src/Magnum/Platform/WindowlessEmscriptenApplication.js CACHE INTERNAL "" FORCE)
    set(MAGNUM_WEBAPPLICATION_CSS ${PROJECT_SOURCE_DIR}/src/Magnum/Platform/WebApplication.css CACHE INTERNAL "" FORCE)
endif()

# Installation paths
include(${CORRADE_LIB_SUFFIX_MODULE})
set(MAGNUM_BINARY_INSTALL_DIR bin)
set(MAGNUM_LIBRARY_INSTALL_DIR lib${LIB_SUFFIX})
set(MAGNUM_DATA_INSTALL_DIR ${MAGNUM_INCLUDE_INSTALL_PREFIX}/share/magnum)
set(MAGNUM_CMAKE_MODULE_INSTALL_DIR ${MAGNUM_INCLUDE_INSTALL_PREFIX}/share/cmake/Magnum)
set(MAGNUM_INCLUDE_INSTALL_DIR ${MAGNUM_INCLUDE_INSTALL_PREFIX}/include/Magnum)
set(MAGNUM_EXTERNAL_INCLUDE_INSTALL_DIR ${MAGNUM_INCLUDE_INSTALL_PREFIX}/include/MagnumExternal)
set(MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR ${MAGNUM_INCLUDE_INSTALL_PREFIX}/include/MagnumPlugins)

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
set(MAGNUM_PLUGINS_SCENECONVERTER_DEBUG_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_BINARY_INSTALL_DIR}/sceneconverters)
set(MAGNUM_PLUGINS_SCENECONVERTER_DEBUG_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_LIBRARY_INSTALL_DIR}/sceneconverters)
set(MAGNUM_PLUGINS_SCENECONVERTER_RELEASE_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_BINARY_INSTALL_DIR}/sceneconverters)
set(MAGNUM_PLUGINS_SCENECONVERTER_RELEASE_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_LIBRARY_INSTALL_DIR}/sceneconverters)
set(MAGNUM_PLUGINS_SHADERCONVERTER_DEBUG_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_BINARY_INSTALL_DIR}/shaderconverters)
set(MAGNUM_PLUGINS_SHADERCONVERTER_DEBUG_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_LIBRARY_INSTALL_DIR}/shaderconverters)
set(MAGNUM_PLUGINS_SHADERCONVERTER_RELEASE_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_BINARY_INSTALL_DIR}/shaderconverters)
set(MAGNUM_PLUGINS_SHADERCONVERTER_RELEASE_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_LIBRARY_INSTALL_DIR}/shaderconverters)
set(MAGNUM_PLUGINS_AUDIOIMPORTER_DEBUG_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_BINARY_INSTALL_DIR}/audioimporters)
set(MAGNUM_PLUGINS_AUDIOIMPORTER_DEBUG_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_LIBRARY_INSTALL_DIR}/audioimporters)
set(MAGNUM_PLUGINS_AUDIOIMPORTER_RELEASE_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_BINARY_INSTALL_DIR}/audioimporters)
set(MAGNUM_PLUGINS_AUDIOIMPORTER_RELEASE_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_LIBRARY_INSTALL_DIR}/audioimporters)

# Plugin dirs based on wheter we are in debug or release build, needed by some
# command-line tools
if(CORRADE_TARGET_WINDOWS)
    set(MAGNUM_PLUGINS_DEBUG_DIR_INIT ${CMAKE_INSTALL_PREFIX}/${MAGNUM_PLUGINS_DEBUG_BINARY_INSTALL_DIR})
    set(MAGNUM_PLUGINS_RELEASE_DIR_INIT ${CMAKE_INSTALL_PREFIX}/${MAGNUM_PLUGINS_RELEASE_BINARY_INSTALL_DIR})
else()
    set(MAGNUM_PLUGINS_DEBUG_DIR_INIT ${CMAKE_INSTALL_PREFIX}/${MAGNUM_PLUGINS_DEBUG_LIBRARY_INSTALL_DIR})
    set(MAGNUM_PLUGINS_RELEASE_DIR_INIT ${CMAKE_INSTALL_PREFIX}/${MAGNUM_PLUGINS_RELEASE_LIBRARY_INSTALL_DIR})
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(MAGNUM_PLUGINS_DIR_INIT ${MAGNUM_PLUGINS_DEBUG_DIR_INIT})
else()
    set(MAGNUM_PLUGINS_DIR_INIT ${MAGNUM_PLUGINS_RELEASE_DIR_INIT})
endif()

# Make these paths configurable from outside. This is *not* PATH, because CMake
# always converts the path to an absolute location internally, making it
# impossible to specify relative paths there. Sorry in advance for not having
# the dir selection button in CMake GUI.
set(MAGNUM_PLUGINS_DEBUG_DIR ""
    CACHE STRING "Base directory where to look for Magnum plugins for debug builds")
set(MAGNUM_PLUGINS_RELEASE_DIR ""
    CACHE STRING "Base directory where to look for Magnum plugins for release builds")
set(MAGNUM_PLUGINS_DIR ""
    CACHE STRING "Base directory where to look for Magnum plugins")

# Plugin directories. Set only if the above are non-empty. otherwise empty as
# well.
if(MAGNUM_PLUGINS_DIR)
    set(MAGNUM_PLUGINS_SHADERCONVERTER_DIR ${MAGNUM_PLUGINS_DIR}/shaderconverters)
    set(MAGNUM_PLUGINS_FONT_DIR ${MAGNUM_PLUGINS_DIR}/fonts)
    set(MAGNUM_PLUGINS_FONTCONVERTER_DIR ${MAGNUM_PLUGINS_DIR}/fontconverters)
    set(MAGNUM_PLUGINS_IMAGECONVERTER_DIR ${MAGNUM_PLUGINS_DIR}/imageconverters)
    set(MAGNUM_PLUGINS_IMPORTER_DIR ${MAGNUM_PLUGINS_DIR}/importers)
    set(MAGNUM_PLUGINS_SCENECONVERTER_DIR ${MAGNUM_PLUGINS_DIR}/sceneconverters)
    set(MAGNUM_PLUGINS_AUDIOIMPORTER_DIR ${MAGNUM_PLUGINS_DIR}/audioimporters)
endif()
if(MAGNUM_PLUGINS_DEBUG_DIR)
    set(MAGNUM_PLUGINS_SHADERCONVERTER_DEBUG_DIR ${MAGNUM_PLUGINS_DEBUG_DIR}/shaderconverters)
    set(MAGNUM_PLUGINS_FONT_DEBUG_DIR ${MAGNUM_PLUGINS_DEBUG_DIR}/fonts)
    set(MAGNUM_PLUGINS_FONTCONVERTER_DEBUG_DIR ${MAGNUM_PLUGINS_DEBUG_DIR}/fontconverters)
    set(MAGNUM_PLUGINS_IMAGECONVERTER_DEBUG_DIR ${MAGNUM_PLUGINS_DEBUG_DIR}/imageconverters)
    set(MAGNUM_PLUGINS_IMPORTER_DEBUG_DIR ${MAGNUM_PLUGINS_DEBUG_DIR}/importers)
    set(MAGNUM_PLUGINS_FONT_RELEASE_DIR ${MAGNUM_PLUGINS_RELEASE_DIR}/fonts)
    set(MAGNUM_PLUGINS_SCENECONVERTER_DEBUG_DIR ${MAGNUM_PLUGINS_DEBUG_DIR}/sceneconverters)
    set(MAGNUM_PLUGINS_AUDIOIMPORTER_DEBUG_DIR ${MAGNUM_PLUGINS_DEBUG_DIR}/audioimporters)
endif()
if(MAGNUM_PLUGINS_RELEASE_DIR)
    set(MAGNUM_PLUGINS_SHADERCONVERTER_RELEASE_DIR ${MAGNUM_PLUGINS_RELEASE_DIR}/shaderconverters)
    set(MAGNUM_PLUGINS_FONTCONVERTER_RELEASE_DIR ${MAGNUM_PLUGINS_RELEASE_DIR}/fontconverters)
    set(MAGNUM_PLUGINS_IMAGECONVERTER_RELEASE_DIR ${MAGNUM_PLUGINS_RELEASE_DIR}/imageconverters)
    set(MAGNUM_PLUGINS_IMPORTER_RELEASE_DIR ${MAGNUM_PLUGINS_RELEASE_DIR}/importers)
    set(MAGNUM_PLUGINS_SCENECONVERTER_RELEASE_DIR ${MAGNUM_PLUGINS_RELEASE_DIR}/sceneconverters)
    set(MAGNUM_PLUGINS_AUDIOIMPORTER_RELEASE_DIR ${MAGNUM_PLUGINS_RELEASE_DIR}/audioimporters)
endif()

# Library version. MAGNUM_VERSION_YEAR/MONTH is used in
# src/Magnum/CMakeLists.txt to generate the version.h header.
set(MAGNUM_LIBRARY_VERSION 2.4)
set(MAGNUM_LIBRARY_SOVERSION 2)
set(MAGNUM_VERSION_YEAR 2020)
set(MAGNUM_VERSION_MONTH 6)

# A single output location. After a decade of saying NO THIS IS A NON-SOLUTION
# TO A NON-PROBLEM I reconsidered my views and enabled this, because:
#
# - On Windows (which don't have RPATH), this makes test execution finally
#   possible without having to install all the stuff first (including the
#   test-only libs, which is ugh).
# - With CMake subprojects, this makes it finally possible to use dynamic
#   plugins directly from the build dir (again without installing anything) ---
#   all plugins are put into the same place, so PluginManager has a single
#   place to look into; and thanks to the dynamic libraries being there as
#   well, this location can be automagically detected as relative to
#   Directory::libraryLocation().
# - Thanks to the $<CONFIG> being part of the output path, you are always sure
#   you never accidentally mix up debug/release libraries when switching
#   CMAKE_BUILD_TYPE in an existing build dir.
#
# The runtime location is set to CMAKE_BINARY_DIR and not PROJECT_BINARY_DIR
# because have one runtime location per CMake subproject would not solve much
# either. If the user already provides CMAKE_RUNTIME_OUTPUT_DIRECTORY (even
# empty), it's respected and nothing is being done.
#
# Explicitly using a generator expression to ensure plugins are added to e.g.
# <CONFIG>/lib/magnum/importers/ instead of lib/magnum/importers/<CONFIG>. Also
# adding this to cache, making superprojects pick that up implicitly as well,
# without forcing them to explicitly mirror this setting.
if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY AND NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY AND NOT DEFINED CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/bin CACHE PATH "" FORCE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/lib CACHE PATH "" FORCE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/lib CACHE PATH "" FORCE)
endif()

add_subdirectory(modules)
add_subdirectory(src)

# Build snippets as part of testing
if(BUILD_TESTS)
    add_subdirectory(doc/snippets)
endif()
