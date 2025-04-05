#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020, 2021, 2022, 2023, 2024, 2025
#             Vladimír Vondruš <mosra@centrum.cz>
#   Copyright © 2023 Vincent Le Garrec <github@le-garrec.fr>
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

# Matches Corrade requirement, see its root CMakeLists for more information.
cmake_minimum_required(VERSION 3.5...3.10)

option(HUNTER_ENABLED "Get dependencies (Corrade, GLFW, SDL2, ...) via Hunter" OFF)
# This option is meant to be passed on the very first invocation. If not done
# then, it doesn't really make sense to enable it at a later time. (Plus the
# Hunter support is long unmaintained here, so it doesn't make sense to show it
# at all.)
mark_as_advanced(HUNTER_ENABLED)
if(HUNTER_ENABLED)
    include(${CMAKE_CURRENT_LIST_DIR}/package/hunter/HunterInit.cmake)
endif()

# CMake 3.12+ uses the policy max version specified in
# cmake_minimum_required(), meaning that with ...3.10, everything until CMP0071
# gets set to NEW implicitly. We however want to keep compatibility with
# versions before 3.12, so the NEW policies are still being hand-picked. Also
# don't want to do a blanket cmake_policy(VERSION) because that may break
# behavior for existing projects that rely on the OLD behavior.

# Don't restrict INTERPROCEDURAL_OPTIMIZATION only for icc on Linux
if(POLICY CMP0069)
    cmake_policy(SET CMP0069 NEW)
endif()
# If CMAKE_AUTOMOC is set, all uses of corrade_add_resource() would otherwise
# complain on 3.10 that AUTOMOC is not processing GENERATED files
if(POLICY CMP0071)
    cmake_policy(SET CMP0071 NEW)
endif()
# Allow <PackageName>_ROOT to be used on 3.12+ to point to per-package install
# locations that find_package(PackageName) subsequently picks up
if(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif()
# Allow also <PACKAGENAME>_ROOT (i.e., uppercase), on 3.27+
if(POLICY CMP0144)
    cmake_policy(SET CMP0144 NEW)
endif()
# Prefer GLVND when finding OpenGL. If this causes problems (known to fail with
# NVidia drivers in Debian Buster, reported on 2019-04-09), users can override
# this by setting OpenGL_GL_PREFERENCE to LEGACY.
if(POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()
# Superprojects can use just set(MAGNUM_WITH_BLAH ON) without FORCE CACHE on
# 3.13+
if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
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

# Options that used to be unprefixed. 66 values for 67 option() and
# cmake_dependent_option() calls, one of which is HUNTER_ENABLED that we don't
# prefix. New options shouldn't be added to this list.
set(_MAGNUM_DEPRECATED_UNPREFIXED_OPTIONS
    WITH_AUDIO
    WITH_DEBUGTOOLS
    WITH_GL
    WITH_MESHTOOLS
    WITH_PRIMITIVES
    WITH_SCENEGRAPH
    WITH_SCENETOOLS
    WITH_SHADERS
    WITH_SHADERTOOLS
    WITH_TEXT
    WITH_TEXTURETOOLS
    WITH_TRADE
    WITH_VK
    WITH_OPENGLTESTER
    WITH_VULKANTESTER
    WITH_ANDROIDAPPLICATION
    WITH_EMSCRIPTENAPPLICATION
    WITH_GLXAPPLICATION
    WITH_GLFWAPPLICATION
    WITH_SDL2APPLICATION
    WITH_XEGLAPPLICATION
    WITH_WINDOWLESSCGLAPPLICATION
    WITH_WINDOWLESSEGLAPPLICATION
    WITH_WINDOWLESSGLXAPPLICATION
    WITH_WINDOWLESSIOSAPPLICATION
    WITH_WINDOWLESSWGLAPPLICATION
    WITH_CGLCONTEXT
    WITH_EGLCONTEXT
    WITH_GLXCONTEXT
    WITH_WGLCONTEXT
    WITH_GL_INFO
    WITH_AL_INFO
    WITH_VK_INFO
    WITH_FONTCONVERTER
    WITH_DISTANCEFIELDCONVERTER
    WITH_IMAGECONVERTER
    WITH_SCENECONVERTER
    WITH_SHADERCONVERTER
    WITH_ANYAUDIOIMPORTER
    WITH_ANYIMAGECONVERTER
    WITH_ANYIMAGEIMPORTER
    WITH_ANYSCENECONVERTER
    WITH_ANYSCENEIMPORTER
    WITH_ANYSHADERCONVERTER
    WITH_MAGNUMFONT
    WITH_MAGNUMFONTCONVERTER
    WITH_OBJIMPORTER
    WITH_TGAIMPORTER
    WITH_TGAIMAGECONVERTER
    WITH_WAVAUDIOIMPORTER
    BUILD_DEPRECATED
    BUILD_STATIC
    BUILD_STATIC_PIC
    BUILD_STATIC_UNIQUE_GLOBALS
    BUILD_PLUGINS_STATIC
    BUILD_TESTS
    BUILD_GL_TESTS
    BUILD_AL_TESTS
    BUILD_VK_TESTS
    TARGET_GL
    TARGET_GLES
    TARGET_GLES2
    TARGET_DESKTOP_GLES
    TARGET_HEADLESS
    TARGET_VK)
# If during the first run (i.e., when the variable isn't in cache yet), check
# if any of the prefixed options are already set. If so, we assume the user is
# already switched to the prefixed options and won't accept the deprecated
# unprefixed options for backwards compatibility. This way it's possible for
# projects to reuse these variables for other purposes without affecting
# Magnum in any way.
if(NOT DEFINED _MAGNUM_ACCEPT_DEPRECATED_UNPREFIXED_OPTIONS)
    set(_MAGNUM_ACCEPT_DEPRECATED_UNPREFIXED_OPTIONS ON CACHE INTERNAL "")
    foreach(option ${_MAGNUM_DEPRECATED_UNPREFIXED_OPTIONS})
        if(DEFINED MAGNUM_${option})
            set(_MAGNUM_ACCEPT_DEPRECATED_UNPREFIXED_OPTIONS OFF CACHE INTERNAL "")
            break()
        endif()
    endforeach()
endif()

# Magnum GL Info (currently only using GLX/CGL/EGL on *nix, WGL/EGL on Windows
# and EGL on Emscripten)
if(CORRADE_TARGET_UNIX OR CORRADE_TARGET_WINDOWS OR CORRADE_TARGET_EMSCRIPTEN)
    option(MAGNUM_WITH_GL_INFO "Build magnum-gl-info utility" OFF)
endif()

# Desktop-only utilities. Not guaranteed to build on GLES, but showing the
# option everywhere for simplicity.
if(CORRADE_TARGET_UNIX OR CORRADE_TARGET_WINDOWS)
    option(MAGNUM_WITH_FONTCONVERTER "Build magnum-fontconverter utility" OFF)
    if(MAGNUM_WITH_FONTCONVERTER AND MAGNUM_BUILD_STATIC AND MAGNUM_BUILD_PLUGINS_STATIC)
        set(MAGNUM_FONTCONVERTER_STATIC_PLUGINS "" CACHE STRING "Static plugins to link to the magnum-fontconverter utility")
    endif()

    option(MAGNUM_WITH_DISTANCEFIELDCONVERTER "Build magnum-distancefieldconverter utility" OFF)
    if(MAGNUM_WITH_DISTANCEFIELDCONVERTER AND MAGNUM_BUILD_STATIC AND MAGNUM_BUILD_PLUGINS_STATIC)
        set(MAGNUM_DISTANCEFIELDCONVERTER_STATIC_PLUGINS "" CACHE STRING "Static plugins to link to the magnum-distancefieldconverter utility")
    endif()
endif()

# API-independent utilities
option(MAGNUM_WITH_IMAGECONVERTER "Build magnum-imageconverter utility" OFF)
if(MAGNUM_WITH_IMAGECONVERTER AND MAGNUM_BUILD_STATIC AND MAGNUM_BUILD_PLUGINS_STATIC)
    set(MAGNUM_IMAGECONVERTER_STATIC_PLUGINS "" CACHE STRING "Static plugins to link to the magnum-imageconverter utility")
endif()

option(MAGNUM_WITH_SCENECONVERTER "Build magnum-sceneconverter utility" OFF)
if(MAGNUM_WITH_SCENECONVERTER AND MAGNUM_BUILD_STATIC AND MAGNUM_BUILD_PLUGINS_STATIC)
    set(MAGNUM_SCENECONVERTER_STATIC_PLUGINS "" CACHE STRING "Static plugins to link to the magnum-sceneconverter utility")
endif()

option(MAGNUM_WITH_SHADERCONVERTER "Build magnum-shaderconverter utility" OFF)
if(MAGNUM_WITH_SHADERCONVERTER AND MAGNUM_BUILD_STATIC AND MAGNUM_BUILD_PLUGINS_STATIC)
    set(MAGNUM_SHADERCONVERTER_STATIC_PLUGINS "" CACHE STRING "Static plugins to link to the magnum-shaderconverter utility")
endif()

# Magnum AL Info
option(MAGNUM_WITH_AL_INFO "Build magnum-al-info utility" OFF)

# Magnum Vk Info
option(MAGNUM_WITH_VK_INFO "Build magnum-vk-info utility" OFF)

# Plugins
option(MAGNUM_WITH_ANYIMAGEIMPORTER "Build AnyImageImporter plugin" OFF)
option(MAGNUM_WITH_ANYAUDIOIMPORTER "Build AnyAudioImporter plugin" OFF)
option(MAGNUM_WITH_ANYIMAGECONVERTER "Build AnyImageConverter plugin" OFF)
option(MAGNUM_WITH_ANYSCENECONVERTER "Build AnySceneConverter plugin" OFF)
option(MAGNUM_WITH_ANYSCENEIMPORTER "Build AnySceneImporter plugin" OFF)
option(MAGNUM_WITH_ANYSHADERCONVERTER "Build AnyShaderConverter plugin" OFF)
option(MAGNUM_WITH_WAVAUDIOIMPORTER "Build WavAudioImporter plugin" OFF)
option(MAGNUM_WITH_MAGNUMFONT "Build MagnumFont plugin" OFF)
option(MAGNUM_WITH_MAGNUMFONTCONVERTER "Build MagnumFontConverter plugin" OFF)
option(MAGNUM_WITH_OBJIMPORTER "Build ObjImporter plugin" OFF)
cmake_dependent_option(MAGNUM_WITH_TGAIMAGECONVERTER "Build TgaImageConverter plugin" OFF "NOT MAGNUM_WITH_MAGNUMFONTCONVERTER" ON)
cmake_dependent_option(MAGNUM_WITH_TGAIMPORTER "Build TgaImporter plugin" OFF "NOT MAGNUM_WITH_MAGNUMFONT" ON)

# Parts of the library
cmake_dependent_option(MAGNUM_WITH_AUDIO "Build Audio library" OFF "NOT MAGNUM_WITH_AL_INFO;NOT MAGNUM_WITH_ANYAUDIOIMPORTER;NOT MAGNUM_WITH_WAVAUDIOIMPORTER" ON)
option(MAGNUM_WITH_DEBUGTOOLS "Build DebugTools library" ON)
cmake_dependent_option(MAGNUM_WITH_MATERIALTOOLS "Build MaterialTools library" ON "NOT MAGNUM_WITH_SCENECONVERTER" ON)
option(MAGNUM_WITH_PRIMITIVES "Build Primitives library" ON)
cmake_dependent_option(MAGNUM_WITH_MESHTOOLS "Build MeshTools library" ON "NOT MAGNUM_WITH_OBJIMPORTER;NOT MAGNUM_WITH_SCENECONVERTER;NOT MAGNUM_WITH_PRIMITIVES" ON)
option(MAGNUM_WITH_SCENEGRAPH "Build SceneGraph library" ON)
cmake_dependent_option(MAGNUM_WITH_SCENETOOLS "Build SceneTools library" ON "NOT MAGNUM_WITH_SCENECONVERTER" ON)
option(MAGNUM_WITH_SHADERS "Build Shaders library" ON)
cmake_dependent_option(MAGNUM_WITH_SHADERTOOLS "Build ShaderTools library" ON "NOT MAGNUM_WITH_SHADERCONVERTER" ON)
cmake_dependent_option(MAGNUM_WITH_TEXT "Build Text library" ON "NOT MAGNUM_WITH_FONTCONVERTER;NOT MAGNUM_WITH_MAGNUMFONT;NOT MAGNUM_WITH_MAGNUMFONTCONVERTER" ON)
cmake_dependent_option(MAGNUM_WITH_TEXTURETOOLS "Build TextureTools library" ON "NOT MAGNUM_WITH_TEXT;NOT MAGNUM_WITH_DISTANCEFIELDCONVERTER" ON)
cmake_dependent_option(MAGNUM_WITH_TRADE "Build Trade library" ON "NOT MAGNUM_WITH_MATERIALTOOLS;NOT MAGNUM_WITH_MESHTOOLS;NOT MAGNUM_WITH_PRIMITIVES;NOT MAGNUM_WITH_SCENETOOLS;NOT MAGNUM_WITH_IMAGECONVERTER;NOT MAGNUM_WITH_ANYIMAGEIMPORTER;NOT MAGNUM_WITH_ANYIMAGECONVERTER;NOT MAGNUM_WITH_ANYSCENEIMPORTER;NOT MAGNUM_WITH_OBJIMPORTER;NOT MAGNUM_WITH_TGAIMAGECONVERTER;NOT MAGNUM_WITH_TGAIMPORTER" ON)
cmake_dependent_option(MAGNUM_WITH_GL "Build GL library" ON "NOT MAGNUM_WITH_GL_INFO;NOT MAGNUM_WITH_ANDROIDAPPLICATION;NOT MAGNUM_WITH_WINDOWLESSIOSAPPLICATION;NOT MAGNUM_WITH_WINDOWLESSCGLAPPLICATION;NOT MAGNUM_WITH_WINDOWLESSGLXAPPLICATION;NOT MAGNUM_WITH_CGLCONTEXT;NOT MAGNUM_WITH_GLXAPPLICATION;NOT MAGNUM_WITH_GLXCONTEXT;NOT MAGNUM_WITH_XEGLAPPLICATION;NOT MAGNUM_WITH_WINDOWLESSWGLAPPLICATION;NOT MAGNUM_WITH_WGLCONTEXT;NOT MAGNUM_WITH_DISTANCEFIELDCONVERTER" ON)

cmake_dependent_option(MAGNUM_TARGET_GL "Build libraries with OpenGL interoperability" ON "MAGNUM_WITH_GL" OFF)

# If targeting iOS, Android, Emscripten or Windows RT, implicitly enable GLES.
# Otherwise default to desktop GL.
if(CORRADE_TARGET_IOS OR CORRADE_TARGET_ANDROID OR CORRADE_TARGET_EMSCRIPTEN OR CORRADE_TARGET_WINDOWS_RT)
    set(MAGNUM_TARGET_GLES ON)
elseif(MAGNUM_WITH_GL)
    cmake_dependent_option(MAGNUM_TARGET_GLES "Build for OpenGL ES / WebGL" OFF "MAGNUM_WITH_GL" OFF)
endif()

# If targeting Android, Emscripten or Windows RT, implicitly enable EGL.
# Otherwise enable EGL by default only if targeting GLES and not on iOS (where
# it's EAGL instead)
if(CORRADE_TARGET_ANDROID OR CORRADE_TARGET_EMSCRIPTEN OR CORRADE_TARGET_WINDOWS_RT)
    set(MAGNUM_TARGET_EGL ON)
else()
    if(MAGNUM_TARGET_GLES AND NOT CORRADE_TARGET_IOS)
        set(_MAGNUM_TARGET_EGL_DEFAULT ON)
    else()
        set(_MAGNUM_TARGET_EGL_DEFAULT OFF)
    endif()
    cmake_dependent_option(MAGNUM_TARGET_EGL "Build for EGL instead of EAGL / CGL / GLX / WGL" ${_MAGNUM_TARGET_EGL_DEFAULT} "MAGNUM_WITH_GL" OFF)
endif()

cmake_dependent_option(MAGNUM_TARGET_GLES2 "Build for OpenGL ES 2 / WebGL 1.0" ON "MAGNUM_TARGET_GLES" OFF)

# Vulkan, everywhere except Emscripten
if(NOT CORRADE_TARGET_EMSCRIPTEN)
    cmake_dependent_option(MAGNUM_WITH_VK "Build Vk library" OFF "NOT MAGNUM_WITH_VK_INFO" ON)
    cmake_dependent_option(MAGNUM_TARGET_VK "Build libraries with Vulkan interoperability" ON "MAGNUM_WITH_VK" OFF)
endif()

# EGL context and windowless EGL application, available everywhere. If
# targeting EGL and not on Windows, it's implied by the CLI tools, otherwise
# it's independent.
if(MAGNUM_TARGET_EGL AND NOT CORRADE_TARGET_WINDOWS)
    cmake_dependent_option(MAGNUM_WITH_WINDOWLESSEGLAPPLICATION "Build WindowlessEglApplication library" OFF "NOT MAGNUM_WITH_GL_INFO;NOT MAGNUM_WITH_DISTANCEFIELDCONVERTER;NOT MAGNUM_WITH_FONTCONVERTER" ON)
else()
    # TODO when CMake 3.22 can be relied on, clean this up to use a proper
    # condition instead
    cmake_dependent_option(MAGNUM_WITH_WINDOWLESSEGLAPPLICATION "Build WindowlessEglApplication library" OFF "ON" OFF)
endif()
option(MAGNUM_WITH_EGLCONTEXT "Build EglContext library" OFF)

# Android-specific application libraries
if(CORRADE_TARGET_ANDROID)
    option(MAGNUM_WITH_ANDROIDAPPLICATION "Build AndroidApplication library" OFF)

# Emscripten-specific application libraries
elseif(CORRADE_TARGET_EMSCRIPTEN)
    option(MAGNUM_WITH_EMSCRIPTENAPPLICATION "Build EmscriptenApplication library" OFF)

# iOS-specific application libraries
elseif(CORRADE_TARGET_IOS)
    option(MAGNUM_WITH_WINDOWLESSIOSAPPLICATION "Build WindowlessIosApplication library" OFF)

# macOS-specific application libraries
elseif(CORRADE_TARGET_APPLE AND NOT MAGNUM_TARGET_EGL)
    # WindowlessCglApplication implied by the CLI tools unless targeting EGL
    if(NOT MAGNUM_TARGET_EGL)
        cmake_dependent_option(MAGNUM_WITH_WINDOWLESSCGLAPPLICATION "Build WindowlessCglApplication library" OFF "NOT MAGNUM_WITH_GL_INFO;NOT MAGNUM_WITH_FONTCONVERTER;NOT MAGNUM_WITH_DISTANCEFIELDCONVERTER" ON)
    else()
        # TODO when CMake 3.22 can be relied on, clean this up to use a proper
        # condition instead
        cmake_dependent_option(MAGNUM_WITH_WINDOWLESSCGLAPPLICATION "Build WindowlessCglApplication library" OFF "ON" OFF)
    endif()
    option(MAGNUM_WITH_CGLCONTEXT "Build CglContext library" OFF)

# X11 + GLX/EGL-specific application libraries
elseif(CORRADE_TARGET_UNIX)
    option(MAGNUM_WITH_GLXAPPLICATION "Build GlxApplication library" OFF)
    # WindowlessGlxApplication implied by the CLI tools unless targeting EGL
    if(NOT MAGNUM_TARGET_EGL)
        cmake_dependent_option(MAGNUM_WITH_WINDOWLESSGLXAPPLICATION "Build WindowlessGlxApplication library" OFF "NOT MAGNUM_WITH_GL_INFO;NOT MAGNUM_WITH_FONTCONVERTER;NOT MAGNUM_WITH_DISTANCEFIELDCONVERTER" ON)
    else()
        # TODO when CMake 3.22 can be relied on, clean this up to use a proper
        # condition instead
        cmake_dependent_option(MAGNUM_WITH_WINDOWLESSGLXAPPLICATION "Build WindowlessGlxApplication library" OFF "ON" OFF)
    endif()
    option(MAGNUM_WITH_GLXCONTEXT "Build GlxContext library" OFF)
    option(MAGNUM_WITH_XEGLAPPLICATION "Build XEglApplication library" OFF)

# Windows-specific application libraries
elseif(CORRADE_TARGET_WINDOWS)
    # WindowlessWglApplication implied by the CLI tools unless targeting EGL
    if(NOT MAGNUM_TARGET_EGL)
        cmake_dependent_option(MAGNUM_WITH_WINDOWLESSWGLAPPLICATION "Build WindowlessWglApplication library" OFF "NOT MAGNUM_WITH_GL_INFO;NOT MAGNUM_WITH_FONTCONVERTER;NOT MAGNUM_WITH_DISTANCEFIELDCONVERTER" ON)
    else()
        # TODO when CMake 3.22 can be relied on, clean this up to use a proper
        # condition instead
        cmake_dependent_option(MAGNUM_WITH_WINDOWLESSWGLAPPLICATION "Build WindowlessWglApplication library" OFF "ON" OFF)
    endif()
    option(MAGNUM_WITH_WGLCONTEXT "Build WglContext library" OFF)
endif()

# Platform-independent (almost) application libraries
if(NOT CORRADE_TARGET_ANDROID)
    option(MAGNUM_WITH_SDL2APPLICATION "Build Sdl2Application library" OFF)
endif()
if(NOT CORRADE_TARGET_ANDROID AND NOT CORRADE_TARGET_IOS AND NOT CORRADE_TARGET_EMSCRIPTEN)
    option(MAGNUM_WITH_GLFWAPPLICATION "Build GlfwApplication library" OFF)
endif()

option(MAGNUM_BUILD_DEPRECATED "Include deprecated API in the build" ON)

set(MAGNUM_DEPLOY_PREFIX "."
    CACHE STRING "Prefix where to put final application executables")

# It's inconvenient to manually load all shared libs using Android / JNI,
# similarly on Emscripten, so there default to static.
if(CORRADE_TARGET_EMSCRIPTEN OR CORRADE_TARGET_ANDROID)
    set(OFF_EXCEPT_ANDROID_EMSCRIPTEN ON)
else()
    set(OFF_EXCEPT_ANDROID_EMSCRIPTEN OFF)
endif()
option(MAGNUM_BUILD_STATIC "Build static libraries" ${OFF_EXCEPT_ANDROID_EMSCRIPTEN})
# Disable PIC on Emscripten by default (but still allow it to be enabled
# explicitly if one so desires). Currently causes linker errors related to
# __memory_base etc.: https://github.com/emscripten-core/emscripten/issues/8761
if(CORRADE_TARGET_EMSCRIPTEN)
    set(ON_EXCEPT_EMSCRIPTEN OFF)
else()
    set(ON_EXCEPT_EMSCRIPTEN ON)
endif()
option(MAGNUM_BUILD_STATIC_PIC "Build static libraries and plugins with position-independent code" ${ON_EXCEPT_EMSCRIPTEN})
cmake_dependent_option(MAGNUM_BUILD_STATIC_UNIQUE_GLOBALS "Build static libraries with globals unique across shared libraries" ${ON_EXCEPT_EMSCRIPTEN} "MAGNUM_BUILD_STATIC" OFF)
# It makes no sense to build dynamic plugins if PluginManager can't load them
if(CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT)
    set(MAGNUM_BUILD_PLUGINS_STATIC ON CACHE BOOL "Build static plugins" FORCE)
else()
    option(MAGNUM_BUILD_PLUGINS_STATIC "Build static plugins" OFF)
endif()
option(MAGNUM_BUILD_TESTS "Build unit tests" OFF)
cmake_dependent_option(MAGNUM_BUILD_GL_TESTS "Build unit tests for OpenGL code" OFF "MAGNUM_BUILD_TESTS;MAGNUM_TARGET_GL" OFF)
cmake_dependent_option(MAGNUM_BUILD_AL_TESTS "Build unit tests for OpenAL code" ON "MAGNUM_BUILD_TESTS;MAGNUM_WITH_AUDIO" OFF)
cmake_dependent_option(MAGNUM_BUILD_VK_TESTS "Build unit tests for Vulkan code" OFF "MAGNUM_BUILD_TESTS;MAGNUM_TARGET_VK" OFF)

if(CORRADE_TARGET_WINDOWS AND NOT CORRADE_TARGET_WINDOWS_RT)
    # TODO is there some cmake_dependent_option() but for strings? I.e., to
    #   hide this if a static build isn't enabled? I don't want to repeat the
    #   complicated logic from inside cmake_dependent_option() just for a
    #   single option.
    set(MAGNUM_BUILD_STATIC_UNIQUE_GLOBALS_DLL_NAME ""
        CACHE STRING "Name of a DLL in which to search for unique globals symbols if MAGNUM_BUILD_STATIC_UNIQUE_GLOBALS is enabled")
endif()

# OpenGLTester / VulkanTester libraries, built by default only if GL / VK tests
# are enabled
cmake_dependent_option(MAGNUM_WITH_OPENGLTESTER "Build OpenGLTester library" OFF "NOT MAGNUM_BUILD_GL_TESTS" ON)
cmake_dependent_option(MAGNUM_WITH_VULKANTESTER "Build VulkanTester library" OFF "NOT MAGNUM_BUILD_VK_TESTS" ON)

# Backwards compatibility for unprefixed CMake options. If the user isn't
# explicitly using prefixed options in the first run already, accept the
# unprefixed options, and remember this decision for subsequent runs
if(NOT DEFINED _MAGNUM_ACCEPT_DEPRECATED_UNPREFIXED_OPTIONS)
    set(_MAGNUM_ACCEPT_DEPRECATED_UNPREFIXED_OPTIONS ON CACHE INTERNAL "")
endif()
# If the user wasn't explicitly using prefixed options in the first run and the
# MAGNUM_BUILD_DEPRECATED option is not currently disabled (which can get
# changed subsequently), accept the unprefixed options and print a warning if
# they're different from the prefixed ones.
if(_MAGNUM_ACCEPT_DEPRECATED_UNPREFIXED_OPTIONS AND MAGNUM_BUILD_DEPRECATED)
    # The following variables need extra care -- mirroring what's done in
    # cmake_dependent_option(), but only the cases that enable a dependency
    # that is off by default, not cases that hide an option from the GUI when
    # it's needed by something else or cases that disable and hide the option
    # from the GUI on unsupported platforms. In other words, if the original
    # user flag setup didn't make sense before but the option dependency fixed
    # it, it'll be broken now, but if it made sense, it should keep working.
    #
    # Doing this before propagating the unprefixed options to avoid a
    # false-positive warning when e.g. MAGNUM_BUILD_STATIC_PIC is implicitly ON
    # but BUILD_STATIC_PIC not yet.
    if(TARGET_GLES)
        if(NOT DEFINED TARGET_GLES2)
            set(TARGET_GLES2 ON)
        endif()
    endif()
    if(BUILD_STATIC)
        if(NOT CORRADE_TARGET_EMSCRIPTEN AND NOT DEFINED BUILD_STATIC_PIC)
            set(BUILD_STATIC_PIC ON)
        endif()
        if(NOT CORRADE_TARGET_EMSCRIPTEN AND NOT DEFINED BUILD_STATIC_UNIQUE_GLOBALS)
            set(BUILD_STATIC_UNIQUE_GLOBALS ON)
        endif()
    endif()
    if(BUILD_TESTS)
        if(NOT DEFINED BUILD_AL_TESTS)
            set(BUILD_AL_TESTS ON)
        endif()
    endif()
    if(BUILD_GL_TESTS)
        if(NOT DEFINED WITH_OPENGLTESTER)
            set(WITH_OPENGLTESTER ON)
        endif()
    endif()
    if(BUILD_VK_TESTS)
        if(NOT DEFINED WITH_VULKANTESTER)
            set(WITH_VULKANTESTER ON)
        endif()
    endif()
    if(WITH_MAGNUMFONTCONVERTER)
        if(NOT DEFINED WITH_TGAIMAGECONVERTER)
            set(WITH_TGAIMAGECONVERTER ON)
        endif()
    endif()
    if(WITH_MAGNUMFONT)
        if(NOT DEFINED WITH_TGAIMPORTER)
            set(WITH_TGAIMPORTER ON)
        endif()
    endif()
    if(WITH_AL_INFO OR WITH_ANYAUDIOIMPORTER OR WITH_WAVAUDIOIMPORTER)
        if(NOT DEFINED WITH_AUDIO)
            set(WITH_AUDIO ON)
        endif()
    endif()
    if(WITH_GL_INFO OR WITH_OPENGLTESTER OR WITH_DISTANCEFIELDCONVERTER)
        if(CORRADE_TARGET_ANDROID OR CORRADE_TARGET_EMSCRIPTEN)
            if(NOT DEFINED WITH_WINDOWLESSEGLAPPLICATION)
                set(WITH_WINDOWLESSEGLAPPLICATION ON)
            endif()
        elseif(CORRADE_TARGET_IOS)
            if(NOT DEFINED WITH_WINDOWLESSIOSAPPLICATION)
                set(WITH_WINDOWLESSIOSAPPLICATION ON)
            endif()
        elseif(CORRADE_TARGET_APPLE AND NOT TARGET_GLES AND NOT CORRADE_TARGET_IOS AND NOT CORRADE_TARGET_ANDROID AND NOT CORRADE_TARGET_EMSCRIPTEN AND NOT CORRADE_TARGET_WINDOWS_RT)
            if(NOT DEFINED WITH_WINDOWLESSCGLAPPLICATION)
                set(WITH_WINDOWLESSCGLAPPLICATION ON)
            endif()
        elseif(CORRADE_TARGET_UNIX)
            # Checking the old deprecated options here, checking
            # MAGNUM_TARGET_EGL wouldn't make sense as that's an option the
            # old code definitely won't use.
            if((NOT TARGET_GLES AND NOT CORRADE_TARGET_IOS AND NOT CORRADE_TARGET_ANDROID AND NOT CORRADE_TARGET_EMSCRIPTEN AND NOT CORRADE_TARGET_WINDOWS_RT AND NOT TARGET_HEADLESS) OR TARGET_DESKTOP_GLES)
                if(NOT DEFINED WITH_WINDOWLESSGLXAPPLICATION)
                    set(WITH_WINDOWLESSGLXAPPLICATION ON)
                endif()
            else()
                if(NOT DEFINED WITH_WINDOWLESSEGLAPPLICATION)
                    set(WITH_WINDOWLESSEGLAPPLICATION ON)
                endif()
            endif()
        elseif(CORRADE_TARGET_WINDOWS)
            if((NOT TARGET_GLES AND NOT CORRADE_TARGET_IOS AND NOT CORRADE_TARGET_ANDROID AND NOT CORRADE_TARGET_EMSCRIPTEN AND NOT CORRADE_TARGET_WINDOWS_RT) OR TARGET_DESKTOP_GLES)
                if(NOT DEFINED WITH_WINDOWLESSWGLAPPLICATION)
                    set(WITH_WINDOWLESSWGLAPPLICATION ON)
                endif()
            else()
                if(NOT DEFINED WITH_WINDOWLESSEGLAPPLICATION)
                    set(WITH_WINDOWLESSEGLAPPLICATION ON)
                endif()
            endif()
        endif()
    endif()
    if(WITH_VK_INFO OR WITH_VULKANTESTER)
        if(NOT DEFINED WITH_VK)
            set(WITH_VK ON)
        endif()
    endif()
    if(WITH_VK)
        if(NOT DEFINED TARGET_VK)
            set(TARGET_VK ON)
        endif()
    endif()

    set(_MAGNUM_WARN_DEPRECATED_UNPREFIXED_OPTION )
    foreach(option ${_MAGNUM_DEPRECATED_UNPREFIXED_OPTIONS})
        if(DEFINED ${option})
            # CMake has no comparison of boolean values (EQUAL returns false if
            # comparing ON and 1 or OFF and FALSE, STREQUAL also), so we have
            # to do it this way. Also warn only on the first encountered
            # variable so people can fix it, reconfigure and go to the next one
            # that warns.
            if((${option} AND NOT MAGNUM_${option}) OR
               (NOT ${option} AND MAGNUM_${option}) AND NOT _MAGNUM_WARN_DEPRECATED_UNPREFIXED_OPTION)
                set(_MAGNUM_WARN_DEPRECATED_UNPREFIXED_OPTION ${option})
            endif()
            set(MAGNUM_${option} ${${option}})
            # If variables specified on the command line don't match any
            # options, they're kept in cache but set as UNINITIALIZED, meaning
            # they don't appear in cmake-gui or ccmake, so there's no way to
            # fix the warning apart from hand-enditing the CMakeCache.txt or
            # recreating the build dir. Update their cached type to be BOOL to
            # make them appear.
            set(${option} ${${option}} CACHE BOOL "Deprecated, use MAGNUM_${option} instead" FORCE)
        endif()
    endforeach()

    if(_MAGNUM_WARN_DEPRECATED_UNPREFIXED_OPTION)
        message(DEPRECATION "Unprefixed options such as ${_MAGNUM_WARN_DEPRECATED_UNPREFIXED_OPTION} are deprecated, use MAGNUM_${_MAGNUM_WARN_DEPRECATED_UNPREFIXED_OPTION} instead. Delete the unprefixed variable from CMake cache or set both to the same value to silence this warning.")
    endif()
endif()

# Handle other deprecated options. For non-deprecated builds CMake will at most
# warn about "variable being unused". Done after the MAGNUM_ prefix backwards
# compatibility above to pick up also the old names, i.e. TARGET_HEADLESS ->
# MAGNUM_TARGET_EGL.
if(MAGNUM_BUILD_DEPRECATED)
    # BUILD_MULTITHREADED got moved to Corrade itself. Print a warning in case
    # it's set but Corrade reports a different value. We can't print a warning
    # in case it's set because that would cause false positives when both
    # Corrade and Magnum are subprojects (and thus this option is visible to
    # both).
    if(DEFINED BUILD_MULTITHREADED AND ((NOT CORRADE_BUILD_MULTITHREADED AND BUILD_MULTITHREADED) OR (CORRADE_BUILD_MULTITHREADED AND NOT BUILD_MULTITHREADED)))
        message(DEPRECATION "BUILD_MULTITHREADED (set to ${BUILD_MULTITHREADED}) is now ignored — you need to set it when building Corrade instead (there it's ${CORRADE_BUILD_MULTITHREADED} now)")
    endif()

    # The following two options were desktop-only, so don't handle any
    # backwards compatibility on mobile / web platforms
    if(NOT CORRADE_TARGET_IOS AND NOT CORRADE_TARGET_ANDROID AND NOT CORRADE_TARGET_EMSCRIPTEN AND NOT CORRADE_TARGET_WINDOWS_RT)
        # MAGNUM_TARGET_HEADLESS is now MAGNUM_TARGET_EGL. Print a warning in
        # case we're on desktop GL (where it was meant to be used) and the two
        # are set to a different value, and sync them.
        if(NOT MAGNUM_TARGET_GLES AND DEFINED MAGNUM_TARGET_HEADLESS AND ((NOT MAGNUM_TARGET_EGL AND MAGNUM_TARGET_HEADLESS) OR (MAGNUM_TARGET_EGL AND NOT MAGNUM_TARGET_HEADLESS)))
            message(DEPRECATION "MAGNUM_TARGET_HEADLESS is deprecated, use MAGNUM_TARGET_EGL instead")
            set(MAGNUM_TARGET_EGL ${MAGNUM_TARGET_HEADLESS})
        endif()

        # MAGNUM_TARGET_DESKTOP_GLES is now an inverse of MAGNUM_TARGET_EGL.
        # Print a warning in case we're on GLES (where it was meant to be used)
        # and the two are set to a different value, and sync them.
        if(MAGNUM_TARGET_GLES AND DEFINED MAGNUM_TARGET_DESKTOP_GLES AND ((MAGNUM_TARGET_EGL AND MAGNUM_TARGET_DESKTOP_GLES) OR (NOT MAGNUM_TARGET_EGL AND NOT MAGNUM_TARGET_DESKTOP_GLES)))
            message(DEPRECATION "MAGNUM_TARGET_DESKTOP_GLES is deprecated, use MAGNUM_TARGET_EGL instead")
            if(MAGNUM_TARGET_DESKTOP_GLES)
                set(MAGNUM_TARGET_EGL OFF)
            else()
                set(MAGNUM_TARGET_EGL ON)
            endif()
        endif()
    endif()
endif()

# Check dependencies
if(MAGNUM_WITH_GL)
    if(NOT MAGNUM_TARGET_GLES OR (MAGNUM_TARGET_GLES AND NOT MAGNUM_TARGET_EGL AND NOT CORRADE_TARGET_IOS))
        find_package(OpenGL REQUIRED)
    elseif(MAGNUM_TARGET_GLES2)
        find_package(OpenGLES2 REQUIRED)
    else()
        find_package(OpenGLES3 REQUIRED)
    endif()
else()
    # Especially now with the backwards compatibility for unprefixed variables
    # it's easiest to just force-disable all this
    set(MAGNUM_TARGET_GL OFF)
    # These are implicitly enabled for embedded platforms, disable them all for
    # consistency
    set(MAGNUM_TARGET_GLES OFF)
    set(MAGNUM_TARGET_GLES2 OFF)
    set(MAGNUM_TARGET_EGL OFF)
endif()

if(NOT MAGNUM_WITH_VK)
    # Especially now with the backwards compatibility for unprefixed variables
    # it's easiest to just force-disable this
    set(MAGNUM_TARGET_VK OFF)
endif()

# Dependent configuration variables
if(MAGNUM_TARGET_GLES)
    if(MAGNUM_BUILD_DEPRECATED AND NOT MAGNUM_TARGET_GLES2)
        set(MAGNUM_TARGET_GLES3 1)
    endif()
    if(CORRADE_TARGET_EMSCRIPTEN)
        set(MAGNUM_TARGET_WEBGL 1)
    endif()
endif()

# Testing setup
if(MAGNUM_BUILD_TESTS)
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

    # If CORRADE_TESTSUITE_TEST_TARGET is set, tests aren't built by default
    # (in the ALL target) but instead set as dependencies of a target named
    # after the value of CORRADE_TESTSUITE_TEST_TARGET. This is a copy of
    # what's done in corrade_add_test(), because we also build various test
    # libraries and plugins in addition to the test executables.
    if(CORRADE_TESTSUITE_TEST_TARGET)
        if(NOT TARGET ${CORRADE_TESTSUITE_TEST_TARGET})
            add_custom_target(${CORRADE_TESTSUITE_TEST_TARGET})
        endif()
        set(EXCLUDE_FROM_ALL_IF_TEST_TARGET EXCLUDE_FROM_ALL)
    endif()
endif()

if(MAGNUM_WITH_OPENGLTESTER)
    if(MAGNUM_TARGET_EGL)
        set(MAGNUM_WITH_WINDOWLESSEGLAPPLICATION ON)
        set(OPENGLTESTER_APPLICATION MagnumWindowlessEglApplication)
    elseif(CORRADE_TARGET_IOS)
        set(MAGNUM_WITH_WINDOWLESSIOSAPPLICATION ON)
        set(OPENGLTESTER_APPLICATION MagnumWindowlessIosApplication)
    elseif(CORRADE_TARGET_APPLE AND NOT MAGNUM_TARGET_GLES)
        set(MAGNUM_WITH_WINDOWLESSCGLAPPLICATION ON)
        set(OPENGLTESTER_APPLICATION MagnumWindowlessCglApplication)
    elseif(CORRADE_TARGET_UNIX)
        set(MAGNUM_WITH_WINDOWLESSGLXAPPLICATION ON)
        set(OPENGLTESTER_APPLICATION MagnumWindowlessGlxApplication)
    elseif(CORRADE_TARGET_WINDOWS)
        set(MAGNUM_WITH_WINDOWLESSWGLAPPLICATION ON)
        set(OPENGLTESTER_APPLICATION MagnumWindowlessWglApplication)
    else()
        # Assuming this gets hit only if MAGNUM_BUILD_GL_TESTS are enabled
        message(FATAL_ERROR "Cannot run tests for OpenGL code on this platform. Set MAGNUM_BUILD_GL_TESTS to OFF to skip building them.")
    endif()
endif()

if(NOT MAGNUM_BUILD_STATIC)
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
set(MAGNUM_DATA_INSTALL_DIR share/magnum)
set(MAGNUM_CMAKE_MODULE_INSTALL_DIR share/cmake/Magnum)
set(MAGNUM_INCLUDE_INSTALL_DIR include/Magnum)
set(MAGNUM_EXTERNAL_INCLUDE_INSTALL_DIR include/MagnumExternal)
set(MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR include/MagnumPlugins)
if(MAGNUM_BUILD_DEPRECATED AND MAGNUM_INCLUDE_INSTALL_PREFIX AND NOT MAGNUM_INCLUDE_INSTALL_PREFIX STREQUAL ".")
    message(DEPRECATION "MAGNUM_INCLUDE_INSTALL_PREFIX is obsolete as its primary use was for old Android NDK versions. Please switch to the NDK r19+ layout instead of using this variable and recreate your build directory to get rid of this warning.")
    set(MAGNUM_DATA_INSTALL_DIR ${MAGNUM_INCLUDE_INSTALL_PREFIX}/${MAGNUM_DATA_INSTALL_DIR})
    set(MAGNUM_CMAKE_MODULE_INSTALL_DIR ${MAGNUM_INCLUDE_INSTALL_PREFIX}/${MAGNUM_CMAKE_MODULE_INSTALL_DIR})
    set(MAGNUM_INCLUDE_INSTALL_DIR ${MAGNUM_INCLUDE_INSTALL_PREFIX}/${MAGNUM_INCLUDE_INSTALL_DIR})
    set(MAGNUM_EXTERNAL_INCLUDE_INSTALL_DIR ${MAGNUM_INCLUDE_INSTALL_PREFIX}/${MAGNUM_EXTERNAL_INCLUDE_INSTALL_DIR})
    set(MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR ${MAGNUM_INCLUDE_INSTALL_PREFIX}/${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR})
endif()

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
#   Utility::Path::libraryLocation().
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
    # There should be no need for the "90% use case" user to adjust these, so
    # don't show them in the default view
    mark_as_advanced(
        CMAKE_RUNTIME_OUTPUT_DIRECTORY
        CMAKE_LIBRARY_OUTPUT_DIRECTORY
        CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
endif()

add_subdirectory(modules)
add_subdirectory(src)

# Build snippets as part of testing. Unlike all other Test/ directories, this
# one isn't added with ${EXCLUDE_FROM_ALL_IF_TEST_TARGET} because the targets
# from there aren't referenced by CTest and thus it'd likely happen that they
# accidentally don't get added as a dependency to that target, causing them to
# be never built. Instead, each target there is handled separately, to minimize
# the chance of an accident.
if(MAGNUM_BUILD_TESTS)
    add_subdirectory(doc/snippets)
endif()
