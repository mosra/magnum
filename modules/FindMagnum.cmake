#.rst:
# Find Magnum
# -----------
#
# Finds the Magnum library. Basic usage::
#
#  find_package(Magnum REQUIRED)
#
# This module tries to find the base Magnum library and then defines the
# following:
#
#  Magnum_FOUND                 - Whether the base library was found
#  MAGNUM_DEPLOY_PREFIX         - Prefix where to put final application
#   executables, defaults to ``.``. If a relative path is used, it's relative
#   to :variable:`CMAKE_INSTALL_PREFIX`.
#  MAGNUM_PLUGINS_DEBUG_DIR     - Base directory with dynamic plugins for
#   debug builds, defaults to magnum-d/ subdirectory of dir where Magnum
#   library was found
#  MAGNUM_PLUGINS_RELEASE_DIR   - Base directory with dynamic plugins for
#   release builds, defaults to magnum/ subdirectory of dir where Magnum
#   library was found
#  MAGNUM_PLUGINS_DIR           - Base directory with dynamic plugins, defaults
#   to :variable:`MAGNUM_PLUGINS_RELEASE_DIR` in release builds and
#   multi-configuration builds or to :variable:`MAGNUM_PLUGINS_DEBUG_DIR` in
#   debug builds
#  MAGNUM_PLUGINS_FONT[|_DEBUG|_RELEASE]_DIR - Directory with dynamic font
#   plugins
#  MAGNUM_PLUGINS_FONTCONVERTER[|_DEBUG|_RELEASE]_DIR - Directory with dynamic
#   font converter plugins
#  MAGNUM_PLUGINS_IMAGECONVERTER[|_DEBUG|_RELEASE]_DIR - Directory with dynamic
#   image converter plugins
#  MAGNUM_PLUGINS_SCENECONVERTER[|_DEBUG|_RELEASE]_DIR - Directory with dynamic
#   scene converter plugins
#  MAGNUM_PLUGINS_IMPORTER[|_DEBUG|_RELEASE]_DIR  - Directory with dynamic
#   importer plugins
#  MAGNUM_PLUGINS_AUDIOIMPORTER[|_DEBUG|_RELEASE]_DIR - Directory with dynamic
#   audio importer plugins
#
# If Magnum is built for Emscripten, the following variables contain paths to
# various support files:
#
#  MAGNUM_EMSCRIPTENAPPLICATION_JS - Path to the EmscriptenApplication.js file
#  MAGNUM_WINDOWLESSEMSCRIPTENAPPLICATION_JS - Path to the
#   WindowlessEmscriptenApplication.js file
#  MAGNUM_WEBAPPLICATION_CSS    - Path to the WebApplication.css file
#
# This command will try to find only the base library, not the optional
# components. The base library depends on Corrade and OpenGL libraries (or
# OpenGL ES libraries). Additional dependencies are specified by the
# components. The optional components are:
#
#  AnyAudioImporter             - Any audio importer
#  AnyImageConverter            - Any image converter
#  AnyImageImporter             - Any image importer
#  AnySceneConverter            - Any scene converter
#  AnySceneImporter             - Any scene importer
#  Audio                        - Audio library
#  DebugTools                   - DebugTools library
#  GL                           - GL library
#  MeshTools                    - MeshTools library
#  Primitives                   - Primitives library
#  SceneGraph                   - SceneGraph library
#  SceneTools                   - SceneTools library
#  Shaders                      - Shaders library
#  ShaderTools                  - ShaderTools library
#  Text                         - Text library
#  TextureTools                 - TextureTools library
#  Trade                        - Trade library
#  Vk                           - Vk library
#  AndroidApplication           - Android application
#  EmscriptenApplication        - Emscripten application
#  GlfwApplication              - GLFW application
#  GlxApplication               - GLX application
#  Sdl2Application              - SDL2 application
#  XEglApplication              - X/EGL application
#  WindowlessCglApplication     - Windowless CGL application
#  WindowlessEglApplication     - Windowless EGL application
#  WindowlessGlxApplication     - Windowless GLX application
#  WindowlessIosApplication     - Windowless iOS application
#  WindowlessWglApplication     - Windowless WGL application
#  WindowlessWindowsEglApplication - Windowless Windows/EGL application
#  CglContext                   - CGL context
#  EglContext                   - EGL context
#  GlxContext                   - GLX context
#  WglContext                   - WGL context
#  OpenGLTester                 - OpenGLTester class
#  VulkanTester                 - VulkanTester class
#  MagnumFont                   - Magnum bitmap font plugin
#  MagnumFontConverter          - Magnum bitmap font converter plugin
#  ObjImporter                  - OBJ importer plugin
#  TgaImageConverter            - TGA image converter plugin
#  TgaImporter                  - TGA importer plugin
#  WavAudioImporter             - WAV audio importer plugin
#  distancefieldconverter       - magnum-distancefieldconverter executable
#  fontconverter                - magnum-fontconverter executable
#  imageconverter               - magnum-imageconverter executable
#  sceneconverterter            - magnum-sceneconverter executable
#  shaderconverterter           - magnum-shaderconverter executable
#  gl-info                      - magnum-gl-info executable
#  vk-info                      - magnum-vk-info executable
#  al-info                      - magnum-al-info executable
#
# Example usage with specifying additional components is::
#
#  find_package(Magnum REQUIRED Trade MeshTools Primitives GlfwApplication)
#
# For each component is then defined:
#
#  Magnum_*_FOUND               - Whether the component was found
#  Magnum::*                    - Component imported target
#
# If exactly one ``*Application`` or exactly one ``Windowless*Application``
# component is requested and found, its target is available in convenience
# alias ``Magnum::Application`` / ``Magnum::WindowlessApplication`` to simplify
# porting. Similarly, if exactly one ``*Context`` component is requested and
# found, its target is available in convenience alias ``Magnum::GLContext``.
#
# The package is found if either debug or release version of each requested
# library (or plugin) is found. If both debug and release libraries (or
# plugins) are found, proper version is chosen based on actual build
# configuration of the project (i.e. Debug build is linked to debug libraries,
# Release build to release libraries). Note that this autodetection might fail
# for the :variable:`MAGNUM_PLUGINS_DIR` variable, especially on
# multi-configuration build systems. You can make use of
# ``CORRADE_IS_DEBUG_BUILD`` preprocessor variable along with
# ``MAGNUM_PLUGINS_*_DEBUG_DIR`` / ``MAGNUM_PLUGINS_*_RELEASE_DIR`` variables
# to decide in preprocessing step.
#
# Features of found Magnum library are exposed in these variables:
#
#  MAGNUM_BUILD_DEPRECATED      - Defined if compiled with deprecated APIs
#   included
#  MAGNUM_BUILD_STATIC          - Defined if compiled as static libraries
#  MAGNUM_BUILD_STATIC_UNIQUE_GLOBALS - Defined if static libraries keep the
#   globals unique even across different shared libraries
#  MAGNUM_TARGET_GL             - Defined if compiled with OpenGL interop
#  MAGNUM_TARGET_GLES           - Defined if compiled for OpenGL ES
#  MAGNUM_TARGET_GLES2          - Defined if compiled for OpenGL ES 2.0
#  MAGNUM_TARGET_GLES3          - Defined if compiled for OpenGL ES 3.0
#  MAGNUM_TARGET_DESKTOP_GLES   - Defined if compiled with OpenGL ES
#   emulation on desktop OpenGL
#  MAGNUM_TARGET_WEBGL          - Defined if compiled for WebGL
#  MAGNUM_TARGET_HEADLESS       - Defined if compiled for headless machines
#  MAGNUM_TARGET_VK             - Defined if compiled with Vulkan interop
#
# The following variables are provided for backwards compatibility purposes
# only when MAGNUM_BUILD_DEPRECATED is enabled and will be removed in a future
# release:
#
#  MAGNUM_BUILD_MULTITHREADED   - Alias to CORRADE_BUILD_MULTITHREADED. Use
#   CORRADE_BUILD_MULTITHREADED instead.
#
# Additionally these variables are defined for internal usage:
#
#  MAGNUM_INCLUDE_DIR           - Root include dir (w/o dependencies)
#  MAGNUM_LIBRARY               - Magnum library (w/o dependencies)
#  MAGNUM_LIBRARY_DEBUG         - Debug version of Magnum library, if found
#  MAGNUM_LIBRARY_RELEASE       - Release version of Magnum library, if found
#  MAGNUM_*_LIBRARY             - Component libraries (w/o dependencies)
#  MAGNUM_*_LIBRARY_DEBUG       - Debug version of given library, if found
#  MAGNUM_*_LIBRARY_RELEASE     - Release version of given library, if found
#  MAGNUM_BINARY_INSTALL_DIR    - Binary installation directory
#  MAGNUM_LIBRARY_INSTALL_DIR   - Library installation directory
#  MAGNUM_DATA_INSTALL_DIR      - Data installation directory
#  MAGNUM_PLUGINS_[DEBUG|RELEASE]_BINARY_INSTALL_DIR - Plugin binary
#   installation directory
#  MAGNUM_PLUGINS_[DEBUG|RELEASE]_LIBRARY_INSTALL_DIR - Plugin library
#   installation directory
#  MAGNUM_PLUGINS_SHADERCONVERTER_[DEBUG|RELEASE]_BINARY_INSTALL_DIR - Shader
#   converter plugin binary installation directory
#  MAGNUM_PLUGINS_SHADERCONVERTER_[DEBUG|RELEASE]_LIBRARY_INSTALL_DIR - Shader
#   converter plugin library installation directory
#  MAGNUM_PLUGINS_FONT_[DEBUG|RELEASE]_BINARY_INSTALL_DIR - Font plugin binary
#   installation directory
#  MAGNUM_PLUGINS_FONT_[DEBUG|RELEASE]_LIBRARY_INSTALL_DIR - Font plugin
#   library installation directory
#  MAGNUM_PLUGINS_FONTCONVERTER_[DEBUG|RELEASE]_BINARY_INSTALL_DIR - Font
#   converter plugin binary installation directory
#  MAGNUM_PLUGINS_FONTCONVERTER_[DEBUG|RELEASE]_LIBRARY_INSTALL_DIR - Font
#   converter plugin library installation directory
#  MAGNUM_PLUGINS_IMAGECONVERTER_[DEBUG|RELEASE]_BINARY_INSTALL_DIR - Image
#   converter plugin binary installation directory
#  MAGNUM_PLUGINS_IMAGECONVERTER_[DEBUG|RELEASE]_LIBRARY_INSTALL_DIR - Image
#   converter plugin library installation directory
#  MAGNUM_PLUGINS_IMPORTER_[DEBUG|RELEASE]_BINARY_INSTALL_DIR  - Importer
#   plugin binary installation directory
#  MAGNUM_PLUGINS_IMPORTER_[DEBUG|RELEASE]_LIBRARY_INSTALL_DIR  - Importer
#   plugin library installation directory
#  MAGNUM_PLUGINS_SCENECONVERTER_[DEBUG|RELEASE]_BINARY_INSTALL_DIR - Scene
#   converter plugin binary installation directory
#  MAGNUM_PLUGINS_SCENECONVERTER_[DEBUG|RELEASE]_LIBRARY_INSTALL_DIR - Scene
#   converter plugin library installation directory
#  MAGNUM_PLUGINS_AUDIOIMPORTER_[DEBUG|RELEASE]_BINARY_INSTALL_DIR - Audio
#   importer plugin binary installation directory
#  MAGNUM_PLUGINS_AUDIOIMPORTER_[DEBUG|RELEASE]_LIBRARY_INSTALL_DIR - Audio
#   importer plugin library installation directory
#  MAGNUM_INCLUDE_INSTALL_DIR   - Header installation directory
#  MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR - Plugin header installation directory
#

#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>
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

# Corrade library dependencies
set(_MAGNUM_CORRADE_DEPENDENCIES )
foreach(_component ${Magnum_FIND_COMPONENTS})
    string(TOUPPER ${_component} _COMPONENT)

    # Unrolling the transitive dependencies here so this doesn't need to be
    # after resolving inter-component dependencies. Listing also all plugins.
    if(_component MATCHES "^(Audio|DebugTools|MeshTools|Primitives|SceneTools|ShaderTools|Text|TextureTools|Trade|.+Importer|.+ImageConverter|.+Font|.+ShaderConverter)$")
        set(_MAGNUM_${_COMPONENT}_CORRADE_DEPENDENCIES PluginManager)
    endif()

    list(APPEND _MAGNUM_CORRADE_DEPENDENCIES ${_MAGNUM_${_COMPONENT}_CORRADE_DEPENDENCIES})
endforeach()
find_package(Corrade REQUIRED Utility ${_MAGNUM_CORRADE_DEPENDENCIES})

# Root include dir
find_path(MAGNUM_INCLUDE_DIR
    NAMES Magnum/Magnum.h)
mark_as_advanced(MAGNUM_INCLUDE_DIR)

# Configuration file
find_file(_MAGNUM_CONFIGURE_FILE configure.h
    HINTS ${MAGNUM_INCLUDE_DIR}/Magnum/)
mark_as_advanced(_MAGNUM_CONFIGURE_FILE)

# We need to open configure.h file from MAGNUM_INCLUDE_DIR before we check for
# the components. Bail out with proper error message if it wasn't found. The
# complete check with all components is further below.
if(NOT MAGNUM_INCLUDE_DIR)
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(Magnum
        REQUIRED_VARS MAGNUM_INCLUDE_DIR _MAGNUM_CONFIGURE_FILE)
endif()

# Read flags from configuration
file(READ ${_MAGNUM_CONFIGURE_FILE} _magnumConfigure)
string(REGEX REPLACE ";" "\\\\;" _magnumConfigure "${_magnumConfigure}")
string(REGEX REPLACE "\n" ";" _magnumConfigure "${_magnumConfigure}")
set(_magnumFlags
    BUILD_DEPRECATED
    BUILD_STATIC
    BUILD_STATIC_UNIQUE_GLOBALS
    TARGET_GL
    TARGET_GLES
    TARGET_GLES2
    TARGET_GLES3
    TARGET_DESKTOP_GLES
    TARGET_WEBGL
    TARGET_HEADLESS
    TARGET_VK)
foreach(_magnumFlag ${_magnumFlags})
    list(FIND _magnumConfigure "#define MAGNUM_${_magnumFlag}" _magnum_${_magnumFlag})
    if(NOT _magnum_${_magnumFlag} EQUAL -1)
        set(MAGNUM_${_magnumFlag} 1)
    endif()
endforeach()

# For compatibility only, to be removed at some point
if(MAGNUM_BUILD_DEPRECATED AND CORRADE_BUILD_MULTITHREADED)
    set(MAGNUM_BUILD_MULTITHREADED 1)
endif()

# OpenGL library preference. Prefer to use GLVND, since that's the better
# approach nowadays, but allow the users to override it from outside in case
# it is broken for some reason (Nvidia drivers in Debian's testing (Buster) --
# reported on 2019-04-09).
if(NOT CMAKE_VERSION VERSION_LESS 3.10 AND NOT OpenGL_GL_PREFERENCE)
    set(OpenGL_GL_PREFERENCE GLVND)
endif()

# Base Magnum library
if(NOT TARGET Magnum::Magnum)
    add_library(Magnum::Magnum UNKNOWN IMPORTED)

    # Try to find both debug and release version
    find_library(MAGNUM_LIBRARY_DEBUG Magnum-d)
    find_library(MAGNUM_LIBRARY_RELEASE Magnum)
    mark_as_advanced(MAGNUM_LIBRARY_DEBUG
        MAGNUM_LIBRARY_RELEASE)

    # Set the MAGNUM_LIBRARY variable based on what was found, use that
    # information to guess also build type of dynamic plugins
    if(MAGNUM_LIBRARY_DEBUG AND MAGNUM_LIBRARY_RELEASE)
        set(MAGNUM_LIBRARY ${MAGNUM_LIBRARY_RELEASE})
        get_filename_component(_MAGNUM_PLUGINS_DIR_PREFIX ${MAGNUM_LIBRARY_DEBUG} PATH)
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(_MAGNUM_PLUGINS_DIR_SUFFIX "-d")
        endif()
    elseif(MAGNUM_LIBRARY_DEBUG)
        set(MAGNUM_LIBRARY ${MAGNUM_LIBRARY_DEBUG})
        get_filename_component(_MAGNUM_PLUGINS_DIR_PREFIX ${MAGNUM_LIBRARY_DEBUG} PATH)
        set(_MAGNUM_PLUGINS_DIR_SUFFIX "-d")
    elseif(MAGNUM_LIBRARY_RELEASE)
        set(MAGNUM_LIBRARY ${MAGNUM_LIBRARY_RELEASE})
        get_filename_component(_MAGNUM_PLUGINS_DIR_PREFIX ${MAGNUM_LIBRARY_RELEASE} PATH)
    endif()

    # On DLL platforms the plugins are stored in bin/ instead of lib/, modify
    # _MAGNUM_PLUGINS_DIR_PREFIX accordingly
    if(CORRADE_TARGET_WINDOWS)
        get_filename_component(_MAGNUM_PLUGINS_DIR_PREFIX ${_MAGNUM_PLUGINS_DIR_PREFIX} PATH)
        set(_MAGNUM_PLUGINS_DIR_PREFIX ${_MAGNUM_PLUGINS_DIR_PREFIX}/bin)
    endif()

    if(MAGNUM_LIBRARY_RELEASE)
        set_property(TARGET Magnum::Magnum APPEND PROPERTY
            IMPORTED_CONFIGURATIONS RELEASE)
        set_property(TARGET Magnum::Magnum PROPERTY
            IMPORTED_LOCATION_RELEASE ${MAGNUM_LIBRARY_RELEASE})
    endif()

    if(MAGNUM_LIBRARY_DEBUG)
        set_property(TARGET Magnum::Magnum APPEND PROPERTY
            IMPORTED_CONFIGURATIONS DEBUG)
        set_property(TARGET Magnum::Magnum PROPERTY
            IMPORTED_LOCATION_DEBUG ${MAGNUM_LIBRARY_DEBUG})
    endif()

    # Include directories
    set_property(TARGET Magnum::Magnum APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
        ${MAGNUM_INCLUDE_DIR})

    # Dependent libraries
    set_property(TARGET Magnum::Magnum APPEND PROPERTY INTERFACE_LINK_LIBRARIES
         Corrade::Utility)
else()
    set(MAGNUM_LIBRARY Magnum::Magnum)
endif()

# Component distinction (listing them explicitly to avoid mistakes with finding
# components from other repositories)
set(_MAGNUM_LIBRARY_COMPONENTS
    Audio DebugTools GL MeshTools Primitives SceneGraph SceneTools Shaders
    ShaderTools Text TextureTools Trade
    WindowlessEglApplication EglContext OpenGLTester)
set(_MAGNUM_PLUGIN_COMPONENTS
    AnyAudioImporter AnyImageConverter AnyImageImporter AnySceneConverter
    AnySceneImporter MagnumFont MagnumFontConverter ObjImporter
    TgaImageConverter TgaImporter WavAudioImporter)
set(_MAGNUM_EXECUTABLE_COMPONENTS
    imageconverter sceneconverter shaderconverter gl-info al-info)
# Audio and Vk libs aren't enabled by default, and none of the Context,
# Application, Tester libs nor plugins are. Keep in sync with Magnum's root
# CMakeLists.txt.
set(_MAGNUM_IMPLICITLY_ENABLED_COMPONENTS
    DebugTools MeshTools SceneGraph Shaders ShaderTools Text TextureTools Trade
    GL Primitives)
if(NOT CORRADE_TARGET_EMSCRIPTEN)
    list(APPEND _MAGNUM_LIBRARY_COMPONENTS Vk VulkanTester)
    list(APPEND _MAGNUM_EXECUTABLE_COMPONENTS vk-info)
endif()
if(NOT CORRADE_TARGET_ANDROID)
    list(APPEND _MAGNUM_LIBRARY_COMPONENTS Sdl2Application)
endif()
if(NOT CORRADE_TARGET_ANDROID AND NOT CORRADE_TARGET_IOS AND NOT CORRADE_TARGET_EMSCRIPTEN)
    list(APPEND _MAGNUM_LIBRARY_COMPONENTS GlfwApplication)
endif()
if(CORRADE_TARGET_ANDROID)
    list(APPEND _MAGNUM_LIBRARY_COMPONENTS AndroidApplication)
endif()
if(CORRADE_TARGET_EMSCRIPTEN)
    list(APPEND _MAGNUM_LIBRARY_COMPONENTS EmscriptenApplication)
endif()
if(CORRADE_TARGET_IOS)
    list(APPEND _MAGNUM_LIBRARY_COMPONENTS WindowlessIosApplication)
elseif(CORRADE_TARGET_APPLE AND NOT MAGNUM_TARGET_GLES)
    list(APPEND _MAGNUM_LIBRARY_COMPONENTS WindowlessCglApplication CglContext)
endif()
if(CORRADE_TARGET_UNIX AND NOT CORRADE_TARGET_APPLE)
    list(APPEND _MAGNUM_LIBRARY_COMPONENTS GlxApplication XEglApplication WindowlessGlxApplication GlxContext)
endif()
if(CORRADE_TARGET_WINDOWS)
    list(APPEND _MAGNUM_LIBRARY_COMPONENTS WindowlessWglApplication WglContext WindowlessWindowsEglApplication)
endif()
if(CORRADE_TARGET_UNIX OR CORRADE_TARGET_WINDOWS)
    list(APPEND _MAGNUM_EXECUTABLE_COMPONENTS fontconverter distancefieldconverter)
endif()

# Inter-component dependencies
set(_MAGNUM_Audio_DEPENDENCIES )

# Trade is used by CompareImage. If Trade is not enabled, CompareImage is not
# compiled at all.
set(_MAGNUM_DebugTools_DEPENDENCIES Trade)
set(_MAGNUM_DebugTools_Trade_DEPENDENCY_IS_OPTIONAL ON)
# MeshTools, Primitives, SceneGraph and Shaders are used only for GL renderers
# in DebugTools. All of this is optional, compiled in only if the base library
# was selected.
if(MAGNUM_TARGET_GL)
    list(APPEND _MAGNUM_DebugTools_DEPENDENCIES MeshTools Primitives SceneGraph Shaders GL)
    set(_MAGNUM_DebugTools_MeshTools_DEPENDENCY_IS_OPTIONAL ON)
    set(_MAGNUM_DebugTools_Primitives_DEPENDENCY_IS_OPTIONAL ON)
    set(_MAGNUM_DebugTools_SceneGraph_DEPENDENCY_IS_OPTIONAL ON)
    set(_MAGNUM_DebugTools_Shaders_DEPENDENCY_IS_OPTIONAL ON)
    set(_MAGNUM_DebugTools_GL_DEPENDENCY_IS_OPTIONAL ON)
endif()

set(_MAGNUM_MeshTools_DEPENDENCIES Trade)
if(MAGNUM_TARGET_GL)
    list(APPEND _MAGNUM_MeshTools_DEPENDENCIES GL)
endif()

set(_MAGNUM_OpenGLTester_DEPENDENCIES GL)
if(MAGNUM_TARGET_HEADLESS OR CORRADE_TARGET_EMSCRIPTEN OR CORRADE_TARGET_ANDROID)
    list(APPEND _MAGNUM_OpenGLTester_DEPENDENCIES WindowlessEglApplication)
elseif(CORRADE_TARGET_IOS)
    list(APPEND _MAGNUM_OpenGLTester_DEPENDENCIES WindowlessIosApplication)
elseif(CORRADE_TARGET_APPLE AND NOT MAGNUM_TARGET_GLES)
    list(APPEND _MAGNUM_OpenGLTester_DEPENDENCIES WindowlessCglApplication)
elseif(CORRADE_TARGET_UNIX)
    if(MAGNUM_TARGET_GLES AND NOT MAGNUM_TARGET_DESKTOP_GLES)
        list(APPEND _MAGNUM_OpenGLTester_DEPENDENCIES WindowlessEglApplication)
    else()
        list(APPEND _MAGNUM_OpenGLTester_DEPENDENCIES WindowlessGlxApplication)
    endif()
elseif(CORRADE_TARGET_WINDOWS)
    if(NOT MAGNUM_TARGET_GLES OR MAGNUM_TARGET_DESKTOP_GLES)
        list(APPEND _MAGNUM_OpenGLTester_DEPENDENCIES WindowlessWglApplication)
    else()
        list(APPEND _MAGNUM_OpenGLTester_DEPENDENCIES WindowlessWindowsEglApplication)
    endif()
endif()

set(_MAGNUM_Primitives_DEPENDENCIES MeshTools Trade)
if(MAGNUM_TARGET_GL)
    # GL not required by Primitives themselves, but transitively by MeshTools
    list(APPEND _MAGNUM_Primitives_DEPENDENCIES GL)
endif()

set(_MAGNUM_SceneGraph_DEPENDENCIES )
set(_MAGNUM_SceneTools_DEPENDENCIES Trade)
set(_MAGNUM_Shaders_DEPENDENCIES GL)

set(_MAGNUM_Text_DEPENDENCIES TextureTools)
if(MAGNUM_TARGET_GL)
    list(APPEND _MAGNUM_Text_DEPENDENCIES GL)
endif()

set(_MAGNUM_TextureTools_DEPENDENCIES )
if(MAGNUM_TARGET_GL)
    list(APPEND _MAGNUM_TextureTools_DEPENDENCIES GL)
endif()

set(_MAGNUM_Trade_DEPENDENCIES )
set(_MAGNUM_VulkanTester_DEPENDENCIES Vk)
set(_MAGNUM_AndroidApplication_DEPENDENCIES GL)

set(_MAGNUM_EmscriptenApplication_DEPENDENCIES)
if(MAGNUM_TARGET_GL)
    list(APPEND _MAGNUM_EmscriptenApplication_DEPENDENCIES GL)
endif()

set(_MAGNUM_GlfwApplication_DEPENDENCIES )
if(MAGNUM_TARGET_GL)
    list(APPEND _MAGNUM_GlfwApplication_DEPENDENCIES GL)
endif()

set(_MAGNUM_GlxApplication_DEPENDENCIES GL)

set(_MAGNUM_Sdl2Application_DEPENDENCIES )
if(MAGNUM_TARGET_GL)
    list(APPEND _MAGNUM_Sdl2Application_DEPENDENCIES GL)
endif()

set(_MAGNUM_WindowlessCglApplication_DEPENDENCIES GL)
set(_MAGNUM_WindowlessEglApplication_DEPENDENCIES GL)
set(_MAGNUM_WindowlessGlxApplication_DEPENDENCIES GL)
set(_MAGNUM_WindowlessIosApplication_DEPENDENCIES GL)
set(_MAGNUM_WindowlessWglApplication_DEPENDENCIES GL)
set(_MAGNUM_WindowlessWindowsEglApplication_DEPENDENCIES GL)
set(_MAGNUM_XEglApplication_DEPENDENCIES GL)
set(_MAGNUM_CglContext_DEPENDENCIES GL)
set(_MAGNUM_EglContext_DEPENDENCIES GL)
set(_MAGNUM_GlxContext_DEPENDENCIES GL)
set(_MAGNUM_WglContext_DEPENDENCIES GL)

set(_MAGNUM_MagnumFont_DEPENDENCIES Trade TgaImporter GL) # and below
set(_MAGNUM_MagnumFontConverter_DEPENDENCIES Trade TgaImageConverter) # and below
set(_MAGNUM_ObjImporter_DEPENDENCIES MeshTools) # and below
foreach(_component ${_MAGNUM_PLUGIN_COMPONENTS})
    if(_component MATCHES ".+AudioImporter")
        list(APPEND _MAGNUM_${_component}_DEPENDENCIES Audio)
    elseif(_component MATCHES ".+ShaderConverter")
        list(APPEND _MAGNUM_${_component}_DEPENDENCIES ShaderTools)
    elseif(_component MATCHES ".+(Importer|ImageConverter|SceneConverter)")
        list(APPEND _MAGNUM_${_component}_DEPENDENCIES Trade)
    elseif(_component MATCHES ".+(Font|FontConverter)")
        list(APPEND _MAGNUM_${_component}_DEPENDENCIES Text TextureTools)
    endif()
endforeach()

# Ensure that all inter-component dependencies are specified as well
set(_MAGNUM_ADDITIONAL_COMPONENTS )
foreach(_component ${Magnum_FIND_COMPONENTS})
    # Mark the dependencies as required if the component is also required, but
    # only if they themselves are not optional (for example parts of DebugTools
    # are present only if their respective base library is compiled)
    if(Magnum_FIND_REQUIRED_${_component})
        foreach(_dependency ${_MAGNUM_${_component}_DEPENDENCIES})
            if(NOT _MAGNUM_${_component}_${_dependency}_DEPENDENCY_IS_OPTIONAL)
                set(Magnum_FIND_REQUIRED_${_dependency} TRUE)
            endif()
        endforeach()
    endif()

    list(APPEND _MAGNUM_ADDITIONAL_COMPONENTS ${_MAGNUM_${_component}_DEPENDENCIES})
endforeach()

# Join the lists, remove duplicate components
set(_MAGNUM_ORIGINAL_FIND_COMPONENTS ${Magnum_FIND_COMPONENTS})
if(_MAGNUM_ADDITIONAL_COMPONENTS)
    list(INSERT Magnum_FIND_COMPONENTS 0 ${_MAGNUM_ADDITIONAL_COMPONENTS})
endif()
if(Magnum_FIND_COMPONENTS)
    list(REMOVE_DUPLICATES Magnum_FIND_COMPONENTS)
endif()

# Find all components. Maintain a list of components that'll need to have
# their optional dependencies checked.
set(_MAGNUM_OPTIONAL_DEPENDENCIES_TO_ADD )
foreach(_component ${Magnum_FIND_COMPONENTS})
    string(TOUPPER ${_component} _COMPONENT)

    # Create imported target in case the library is found. If the project is
    # added as subproject to CMake, the target already exists and all the
    # required setup is already done from the build tree.
    if(TARGET Magnum::${_component})
        set(Magnum_${_component}_FOUND TRUE)
    else()
        # Library components
        if(_component IN_LIST _MAGNUM_LIBRARY_COMPONENTS)
            add_library(Magnum::${_component} UNKNOWN IMPORTED)

            # Set library defaults, find the library
            set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_SUFFIX Magnum/${_component})
            set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES ${_component}.h)

            # Try to find both debug and release version
            find_library(MAGNUM_${_COMPONENT}_LIBRARY_DEBUG Magnum${_component}-d)
            find_library(MAGNUM_${_COMPONENT}_LIBRARY_RELEASE Magnum${_component})
            mark_as_advanced(MAGNUM_${_COMPONENT}_LIBRARY_DEBUG
                MAGNUM_${_COMPONENT}_LIBRARY_RELEASE)

        # Plugin components
        elseif(_component IN_LIST _MAGNUM_PLUGIN_COMPONENTS)
            add_library(Magnum::${_component} UNKNOWN IMPORTED)

            # AudioImporter plugin specific name suffixes
            if(_component MATCHES ".+AudioImporter$")
                set(_MAGNUM_${_COMPONENT}_PATH_SUFFIX audioimporters)

                # Audio importer class is Audio::*Importer, thus we need to
                # convert *AudioImporter.h to *Importer.h
                string(REPLACE "AudioImporter" "Importer" _MAGNUM_${_COMPONENT}_HEADER_NAME "${_component}")
                set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES ${_MAGNUM_${_COMPONENT}_HEADER_NAME}.h)

            # ShaderConverter plugin specific name suffixes
            elseif(_component MATCHES ".+ShaderConverter$")
                set(_MAGNUM_${_COMPONENT}_PATH_SUFFIX shaderconverters)

            # Importer plugin specific name suffixes
            elseif(_component MATCHES ".+Importer$")
                set(_MAGNUM_${_COMPONENT}_PATH_SUFFIX importers)

            # Font plugin specific name suffixes
            elseif(_component MATCHES ".+Font$")
                set(_MAGNUM_${_COMPONENT}_PATH_SUFFIX fonts)

            # ImageConverter plugin specific name suffixes
            elseif(_component MATCHES ".+ImageConverter$")
                set(_MAGNUM_${_COMPONENT}_PATH_SUFFIX imageconverters)

            # SceneConverter plugin specific name suffixes
            elseif(_component MATCHES ".+SceneConverter$")
                set(_MAGNUM_${_COMPONENT}_PATH_SUFFIX sceneconverters)

            # FontConverter plugin specific name suffixes
            elseif(_component MATCHES ".+FontConverter$")
                set(_MAGNUM_${_COMPONENT}_PATH_SUFFIX fontconverters)
            endif()

            # Don't override the exception for *AudioImporter plugins
            set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_SUFFIX MagnumPlugins/${_component})
            if(NOT _MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES)
                set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES ${_component}.h)
            endif()

            # Dynamic plugins don't have any prefix (e.g. `lib` on Linux),
            # search with empty prefix and then reset that back so we don't
            # accidentally break something else
            set(_tmp_prefixes "${CMAKE_FIND_LIBRARY_PREFIXES}")
            set(CMAKE_FIND_LIBRARY_PREFIXES "${CMAKE_FIND_LIBRARY_PREFIXES};")

            # Try to find both debug and release version. Dynamic and static
            # debug libraries are in different places. Static debug plugins are
            # in magnum/ with a -d suffix while dynamic debug plugins are in
            # magnum-d/ with no suffix. Problem is that Vcpkg's library linking
            # automagic needs the static libs to be in the root library
            # directory along with everything else and so we need to search for
            # the -d suffixed version *before* the unsuffixed so it doesn't
            # pick the release library for both debug and release.
            find_library(MAGNUM_${_COMPONENT}_LIBRARY_DEBUG ${_component}-d
                PATH_SUFFIXES magnum/${_MAGNUM_${_COMPONENT}_PATH_SUFFIX})
            find_library(MAGNUM_${_COMPONENT}_LIBRARY_DEBUG ${_component}
                PATH_SUFFIXES magnum-d/${_MAGNUM_${_COMPONENT}_PATH_SUFFIX})
            find_library(MAGNUM_${_COMPONENT}_LIBRARY_RELEASE ${_component}
                PATH_SUFFIXES magnum/${_MAGNUM_${_COMPONENT}_PATH_SUFFIX})
            mark_as_advanced(MAGNUM_${_COMPONENT}_LIBRARY_DEBUG
                MAGNUM_${_COMPONENT}_LIBRARY_RELEASE)

            # Reset back
            set(CMAKE_FIND_LIBRARY_PREFIXES "${_tmp_prefixes}")

        # Executables
        elseif(_component IN_LIST _MAGNUM_EXECUTABLE_COMPONENTS)
            add_executable(Magnum::${_component} IMPORTED)

            find_program(MAGNUM_${_COMPONENT}_EXECUTABLE magnum-${_component})
            mark_as_advanced(MAGNUM_${_COMPONENT}_EXECUTABLE)

            if(MAGNUM_${_COMPONENT}_EXECUTABLE)
                set_property(TARGET Magnum::${_component} PROPERTY
                    IMPORTED_LOCATION ${MAGNUM_${_COMPONENT}_EXECUTABLE})
            endif()

        # Something unknown, skip. FPHSA will take care of handling this below.
        else()
            continue()
        endif()

        # Library location for libraries/plugins
        if(_component IN_LIST _MAGNUM_LIBRARY_COMPONENTS OR _component IN_LIST _MAGNUM_PLUGIN_COMPONENTS)
            if(MAGNUM_${_COMPONENT}_LIBRARY_RELEASE)
                set_property(TARGET Magnum::${_component} APPEND PROPERTY
                    IMPORTED_CONFIGURATIONS RELEASE)
                set_property(TARGET Magnum::${_component} PROPERTY
                    IMPORTED_LOCATION_RELEASE ${MAGNUM_${_COMPONENT}_LIBRARY_RELEASE})
            endif()

            if(MAGNUM_${_COMPONENT}_LIBRARY_DEBUG)
                set_property(TARGET Magnum::${_component} APPEND PROPERTY
                    IMPORTED_CONFIGURATIONS DEBUG)
                set_property(TARGET Magnum::${_component} PROPERTY
                    IMPORTED_LOCATION_DEBUG ${MAGNUM_${_COMPONENT}_LIBRARY_DEBUG})
            endif()
        endif()

        # Applications
        if(_component MATCHES ".+Application")
            set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_SUFFIX Magnum/Platform)

            # Android application dependencies
            if(_component STREQUAL AndroidApplication)
                find_package(EGL)
                set_property(TARGET Magnum::${_component} APPEND PROPERTY
                    INTERFACE_LINK_LIBRARIES android EGL::EGL)

            # EmscriptenApplication has no additional dependencies

            # GLFW application dependencies
            elseif(_component STREQUAL GlfwApplication)
                find_package(GLFW)
                set_property(TARGET Magnum::${_component} APPEND PROPERTY
                    INTERFACE_LINK_LIBRARIES GLFW::GLFW)
                # Use the Foundation framework on Apple to query the DPI awareness
                if(CORRADE_TARGET_APPLE)
                    find_library(_MAGNUM_APPLE_FOUNDATION_FRAMEWORK_LIBRARY Foundation)
                    mark_as_advanced(_MAGNUM_APPLE_FOUNDATION_FRAMEWORK_LIBRARY)
                    set_property(TARGET Magnum::${_component} APPEND PROPERTY
                        INTERFACE_LINK_LIBRARIES ${_MAGNUM_APPLE_FOUNDATION_FRAMEWORK_LIBRARY})
                # Needed for opt-in DPI queries
                elseif(CORRADE_TARGET_UNIX)
                    set_property(TARGET Magnum::${_component} APPEND PROPERTY
                        INTERFACE_LINK_LIBRARIES ${CMAKE_DL_LIBS})
                endif()

                # With GLVND (since CMake 3.11) we need to explicitly link to
                # GLX/EGL because libOpenGL doesn't provide it. For EGL we have
                # our own EGL find module, which makes things simpler. The
                # upstream FindOpenGL is anything but simple. Also can't use
                # OpenGL_OpenGL_FOUND, because that one is set also if GLVND is
                # *not* found. WTF. Also can't just check for
                # OPENGL_opengl_LIBRARY because that's set even if
                # OpenGL_GL_PREFERENCE is explicitly set to LEGACY.
                if(MAGNUM_TARGET_GL)
                    if(CORRADE_TARGET_UNIX AND NOT CORRADE_TARGET_APPLE AND (NOT MAGNUM_TARGET_GLES OR MAGNUM_TARGET_DESKTOP_GLES))
                        find_package(OpenGL)
                        if(OPENGL_opengl_LIBRARY AND OpenGL_GL_PREFERENCE STREQUAL GLVND)
                            set_property(TARGET Magnum::${_component} APPEND
                            PROPERTY INTERFACE_LINK_LIBRARIES OpenGL::GLX)
                        endif()
                    elseif(MAGNUM_TARGET_GLES AND NOT MAGNUM_TARGET_DESKTOP_GLES AND NOT CORRADE_TARGET_EMSCRIPTEN)
                        find_package(EGL)
                        set_property(TARGET Magnum::${_component} APPEND
                            PROPERTY INTERFACE_LINK_LIBRARIES EGL::EGL)
                    endif()
                endif()

            # SDL2 application dependencies
            elseif(_component STREQUAL Sdl2Application)
                find_package(SDL2)
                set_property(TARGET Magnum::${_component} APPEND PROPERTY
                    INTERFACE_LINK_LIBRARIES SDL2::SDL2)
                # Use the Foundation framework on Apple to query the DPI awareness
                if(CORRADE_TARGET_APPLE)
                    find_library(_MAGNUM_APPLE_FOUNDATION_FRAMEWORK_LIBRARY Foundation)
                    mark_as_advanced(_MAGNUM_APPLE_FOUNDATION_FRAMEWORK_LIBRARY)
                    set_property(TARGET Magnum::${_component} APPEND PROPERTY
                        INTERFACE_LINK_LIBRARIES ${_MAGNUM_APPLE_FOUNDATION_FRAMEWORK_LIBRARY})
                # Needed for opt-in DPI queries
                elseif(CORRADE_TARGET_UNIX)
                    set_property(TARGET Magnum::${_component} APPEND PROPERTY
                        INTERFACE_LINK_LIBRARIES ${CMAKE_DL_LIBS})
                endif()

                # With GLVND (since CMake 3.11) we need to explicitly link to
                # GLX/EGL because libOpenGL doesn't provide it. For EGL we have
                # our own EGL find module, which makes things simpler. The
                # upstream FindOpenGL is anything but simple. Also can't use
                # OpenGL_OpenGL_FOUND, because that one is set also if GLVND is
                # *not* found. WTF. Also can't just check for
                # OPENGL_opengl_LIBRARY because that's set even if
                # OpenGL_GL_PREFERENCE is explicitly set to LEGACY.
                if(MAGNUM_TARGET_GL)
                    if(CORRADE_TARGET_UNIX AND NOT CORRADE_TARGET_APPLE AND (NOT MAGNUM_TARGET_GLES OR MAGNUM_TARGET_DESKTOP_GLES))
                        find_package(OpenGL)
                        if(OPENGL_opengl_LIBRARY AND OpenGL_GL_PREFERENCE STREQUAL GLVND)
                            set_property(TARGET Magnum::${_component} APPEND
                            PROPERTY INTERFACE_LINK_LIBRARIES OpenGL::GLX)
                        endif()
                    elseif(MAGNUM_TARGET_GLES AND NOT MAGNUM_TARGET_DESKTOP_GLES AND NOT CORRADE_TARGET_EMSCRIPTEN)
                        find_package(EGL)
                        set_property(TARGET Magnum::${_component} APPEND
                            PROPERTY INTERFACE_LINK_LIBRARIES EGL::EGL)
                    endif()
                endif()

            # (Windowless) GLX application dependencies
            elseif(_component STREQUAL GlxApplication OR _component STREQUAL WindowlessGlxApplication)
                find_package(X11)
                set_property(TARGET Magnum::${_component} APPEND PROPERTY
                    INTERFACE_INCLUDE_DIRECTORIES ${X11_INCLUDE_DIR})
                set_property(TARGET Magnum::${_component} APPEND PROPERTY
                    INTERFACE_LINK_LIBRARIES ${X11_LIBRARIES})

                # With GLVND (since CMake 3.11) we need to explicitly link to
                # GLX because libOpenGL doesn't provide it. Also can't use
                # OpenGL_OpenGL_FOUND, because that one is set also if GLVND is
                # *not* found. WTF. Also can't just check for
                # OPENGL_opengl_LIBRARY because that's set even if
                # OpenGL_GL_PREFERENCE is explicitly set to LEGACY.
                find_package(OpenGL)
                if(OPENGL_opengl_LIBRARY AND OpenGL_GL_PREFERENCE STREQUAL GLVND)
                    set_property(TARGET Magnum::${_component} APPEND PROPERTY
                        INTERFACE_LINK_LIBRARIES OpenGL::GLX)
                endif()

            # Windowless CGL application has no additional dependencies

            # Windowless EGL application dependencies
            elseif(_component STREQUAL WindowlessEglApplication)
                find_package(EGL)
                set_property(TARGET Magnum::${_component} APPEND PROPERTY
                    INTERFACE_LINK_LIBRARIES EGL::EGL)

            # Windowless iOS application dependencies
            elseif(_component STREQUAL WindowlessIosApplication)
                # We need to link to Foundation framework to use ObjC
                find_library(_MAGNUM_IOS_FOUNDATION_FRAMEWORK_LIBRARY Foundation)
                mark_as_advanced(_MAGNUM_IOS_FOUNDATION_FRAMEWORK_LIBRARY)
                find_package(EGL)
                set_property(TARGET Magnum::${_component} APPEND PROPERTY
                    INTERFACE_LINK_LIBRARIES EGL::EGL ${_MAGNUM_IOS_FOUNDATION_FRAMEWORK_LIBRARY})

            # Windowless WGL application has no additional dependencies

            # Windowless Windows/EGL application dependencies
            elseif(_component STREQUAL WindowlessWindowsEglApplication)
                find_package(EGL)
                set_property(TARGET Magnum::${_component} APPEND PROPERTY
                    INTERFACE_LINK_LIBRARIES EGL::EGL)

            # X/EGL application dependencies
            elseif(_component STREQUAL XEglApplication)
                find_package(EGL)
                find_package(X11)
                set_property(TARGET Magnum::${_component} APPEND PROPERTY
                    INTERFACE_INCLUDE_DIRECTORIES ${X11_INCLUDE_DIR})
                set_property(TARGET Magnum::${_component} APPEND PROPERTY
                    INTERFACE_LINK_LIBRARIES EGL::EGL ${X11_LIBRARIES})
            endif()

        # Context libraries
        elseif(_component MATCHES ".+Context")
            set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_SUFFIX Magnum/Platform)
            set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES GLContext.h)

            # GLX context dependencies
            if(_component STREQUAL GlxContext)
                # With GLVND (since CMake 3.11) we need to explicitly link to
                # GLX because libOpenGL doesn't provide it. Also can't use
                # OpenGL_OpenGL_FOUND, because that one is set also if GLVND is
                # *not* found. If GLVND is not used, link to X11 instead. Also
                # can't just check for OPENGL_opengl_LIBRARY because that's set
                # even if OpenGL_GL_PREFERENCE is explicitly set to LEGACY.
                find_package(OpenGL)
                if(OPENGL_opengl_LIBRARY AND OpenGL_GL_PREFERENCE STREQUAL GLVND)
                    set_property(TARGET Magnum::${_component} APPEND PROPERTY
                        INTERFACE_LINK_LIBRARIES OpenGL::GLX)
                else()
                    find_package(X11)
                    set_property(TARGET Magnum::${_component} APPEND PROPERTY
                        INTERFACE_INCLUDE_DIRECTORIES ${X11_INCLUDE_DIR})
                    set_property(TARGET Magnum::${_component} APPEND PROPERTY
                        INTERFACE_LINK_LIBRARIES ${X11_LIBRARIES})
                endif()

            # EGL context dependencies
            elseif(_component STREQUAL EglContext)
                find_package(EGL)
                set_property(TARGET Magnum::${_component} APPEND PROPERTY
                    INTERFACE_LINK_LIBRARIES EGL::EGL)
            endif()

            # No additional dependencies for CGL context
            # No additional dependencies for WGL context

        # Audio library
        elseif(_component STREQUAL Audio)
            find_package(OpenAL)
            set_property(TARGET Magnum::${_component} APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES Corrade::PluginManager OpenAL::OpenAL)

        # No special setup for DebugTools library

        # GL library
        elseif(_component STREQUAL GL)
            if(NOT MAGNUM_TARGET_GLES OR MAGNUM_TARGET_DESKTOP_GLES)
                # If the GLVND library (CMake 3.11+) was found, link to the
                # imported target. Otherwise (and also on all systems except
                # Linux) link to the classic libGL. Can't use
                # OpenGL_OpenGL_FOUND, because that one is set also if GLVND is
                # *not* found. WTF. Also can't just check for
                # OPENGL_opengl_LIBRARY because that's set even if
                # OpenGL_GL_PREFERENCE is explicitly set to LEGACY.
                find_package(OpenGL REQUIRED)
                if(OPENGL_opengl_LIBRARY AND OpenGL_GL_PREFERENCE STREQUAL GLVND)
                    set_property(TARGET Magnum::${_component} APPEND PROPERTY
                        INTERFACE_LINK_LIBRARIES OpenGL::OpenGL)
                else()
                    set_property(TARGET Magnum::${_component} APPEND PROPERTY
                        INTERFACE_LINK_LIBRARIES ${OPENGL_gl_LIBRARY})
                endif()
            elseif(MAGNUM_TARGET_GLES2)
                find_package(OpenGLES2 REQUIRED)
                set_property(TARGET Magnum::${_component} APPEND PROPERTY
                    INTERFACE_LINK_LIBRARIES OpenGLES2::OpenGLES2)
            elseif(MAGNUM_TARGET_GLES3)
                find_package(OpenGLES3 REQUIRED)
                set_property(TARGET Magnum::${_component} APPEND PROPERTY
                    INTERFACE_LINK_LIBRARIES OpenGLES3::OpenGLES3)
            endif()

        # MeshTools library
        elseif(_component STREQUAL MeshTools)
            set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES CompressIndices.h)

        # OpenGLTester library
        elseif(_component STREQUAL OpenGLTester)
            set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_SUFFIX Magnum/GL)

        # VulkanTester library
        elseif(_component STREQUAL VulkanTester)
            set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_SUFFIX Magnum/Vk)

        # Primitives library
        elseif(_component STREQUAL Primitives)
            set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES Cube.h)

        # No special setup for SceneGraph library

        # ShaderTools library
        elseif(_component STREQUAL ShaderTools)
            set_property(TARGET Magnum::${_component} APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES Corrade::PluginManager)

        # No special setup for Shaders library

        # Text library
        elseif(_component STREQUAL Text)
            set_property(TARGET Magnum::${_component} APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES Corrade::PluginManager)

        # TextureTools library
        elseif(_component STREQUAL TextureTools)
            set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES Atlas.h)

        # Trade library
        elseif(_component STREQUAL Trade)
            set_property(TARGET Magnum::${_component} APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES Corrade::PluginManager)

        # Vk library
        elseif(_component STREQUAL Vk)
            find_package(Vulkan REQUIRED)
            set_property(TARGET Magnum::${_component} APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES Vulkan::Vulkan)
        endif()

        # No special setup for AnyAudioImporter plugin
        # No special setup for AnyImageConverter plugin
        # No special setup for AnyImageImporter plugin
        # No special setup for AnySceneImporter plugin
        # No special setup for MagnumFont plugin
        # No special setup for MagnumFontConverter plugin
        # No special setup for ObjImporter plugin
        # No special setup for TgaImageConverter plugin
        # No special setup for TgaImporter plugin
        # No special setup for WavAudioImporter plugin

        # Find library/plugin includes
        if(_component IN_LIST _MAGNUM_LIBRARY_COMPONENTS OR _component IN_LIST _MAGNUM_PLUGIN_COMPONENTS)
            find_path(_MAGNUM_${_COMPONENT}_INCLUDE_DIR
                NAMES ${_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES}
                HINTS ${MAGNUM_INCLUDE_DIR}/${_MAGNUM_${_COMPONENT}_INCLUDE_PATH_SUFFIX})
            mark_as_advanced(_MAGNUM_${_COMPONENT}_INCLUDE_DIR)
        endif()

        # Automatic import of static plugins. Skip in case the include dir was
        # not found -- that'll fail later with a proper message.
        if(_component IN_LIST _MAGNUM_PLUGIN_COMPONENTS AND _MAGNUM_${_COMPONENT}_INCLUDE_DIR)
            # Automatic import of static plugins
            file(READ ${_MAGNUM_${_COMPONENT}_INCLUDE_DIR}/configure.h _magnum${_component}Configure)
            string(FIND "${_magnum${_component}Configure}" "#define MAGNUM_${_COMPONENT}_BUILD_STATIC" _magnum${_component}_BUILD_STATIC)
            if(NOT _magnum${_component}_BUILD_STATIC EQUAL -1)
                set_property(TARGET Magnum::${_component} APPEND PROPERTY
                    INTERFACE_SOURCES ${_MAGNUM_${_COMPONENT}_INCLUDE_DIR}/importStaticPlugin.cpp)
            endif()
        endif()

        # Link to core Magnum library, add inter-library dependencies. If there
        # are optional dependencies, defer adding them to later once we know if
        # they were found or not.
        if(_component IN_LIST _MAGNUM_LIBRARY_COMPONENTS OR _component IN_LIST _MAGNUM_PLUGIN_COMPONENTS)
            set_property(TARGET Magnum::${_component} APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES Magnum::Magnum)
            set(_MAGNUM_${component}_OPTIONAL_DEPENDENCIES_TO_ADD )
            foreach(_dependency ${_MAGNUM_${_component}_DEPENDENCIES})
                if(NOT _MAGNUM_${_component}_${_dependency}_DEPENDENCY_IS_OPTIONAL)
                    set_property(TARGET Magnum::${_component} APPEND PROPERTY
                        INTERFACE_LINK_LIBRARIES Magnum::${_dependency})
                else()
                    list(APPEND _MAGNUM_${_component}_OPTIONAL_DEPENDENCIES_TO_ADD
                        ${_dependency})
                endif()
            endforeach()
            if(_MAGNUM_${_component}_OPTIONAL_DEPENDENCIES_TO_ADD)
                list(APPEND _MAGNUM_OPTIONAL_DEPENDENCIES_TO_ADD ${_component})
            endif()
        endif()

        # Decide if the library was found
        if(((_component IN_LIST _MAGNUM_LIBRARY_COMPONENTS OR _component IN_LIST _MAGNUM_PLUGIN_COMPONENTS) AND _MAGNUM_${_COMPONENT}_INCLUDE_DIR AND (MAGNUM_${_COMPONENT}_LIBRARY_DEBUG OR MAGNUM_${_COMPONENT}_LIBRARY_RELEASE)) OR (_component IN_LIST _MAGNUM_EXECUTABLE_COMPONENTS AND MAGNUM_${_COMPONENT}_EXECUTABLE))
            set(Magnum_${_component}_FOUND TRUE)
        else()
            set(Magnum_${_component}_FOUND FALSE)
        endif()
    endif()

    # Global aliases for Windowless*Application, *Application and *Context
    # components. If already set, unset them to avoid ambiguity.
    if(_component MATCHES "Windowless.+Application")
        if(NOT DEFINED _MAGNUM_WINDOWLESSAPPLICATION_ALIAS)
            set(_MAGNUM_WINDOWLESSAPPLICATION_ALIAS Magnum::${_component})
        else()
            unset(_MAGNUM_WINDOWLESSAPPLICATION_ALIAS)
        endif()
    elseif(_component MATCHES ".+Application")
        if(NOT DEFINED _MAGNUM_APPLICATION_ALIAS)
            set(_MAGNUM_APPLICATION_ALIAS Magnum::${_component})
        else()
            unset(_MAGNUM_APPLICATION_ALIAS)
        endif()
    elseif(_component MATCHES ".+Context")
        if(NOT DEFINED _MAGNUM_GLCONTEXT_ALIAS)
            set(_MAGNUM_GLCONTEXT_ALIAS Magnum::${_component})
        else()
            unset(_MAGNUM_GLCONTEXT_ALIAS)
        endif()
    endif()
endforeach()

# Emscripten-specific files and flags
if(CORRADE_TARGET_EMSCRIPTEN)
    find_file(MAGNUM_EMSCRIPTENAPPLICATION_JS EmscriptenApplication.js
        PATH_SUFFIXES share/magnum)
    find_file(MAGNUM_WINDOWLESSEMSCRIPTENAPPLICATION_JS WindowlessEmscriptenApplication.js
        PATH_SUFFIXES share/magnum)
    find_file(MAGNUM_WEBAPPLICATION_CSS WebApplication.css
        PATH_SUFFIXES share/magnum)
    mark_as_advanced(
        MAGNUM_EMSCRIPTENAPPLICATION_JS
        MAGNUM_WINDOWLESSEMSCRIPTENAPPLICATION_JS
        MAGNUM_WEBAPPLICATION_CSS)
    set(MAGNUM_EXTRAS_NEEDED
        MAGNUM_EMSCRIPTENAPPLICATION_JS
        MAGNUM_WINDOWLESSEMSCRIPTENAPPLICATION_JS
        MAGNUM_WEBAPPLICATION_CSS)

    # If we are on CMake 3.13 and up, `-s USE_WEBGL2=1` linker option is
    # propagated from FindOpenGLES3.cmake already. If not (and the GL library
    # is used), we need to modify the global CMAKE_EXE_LINKER_FLAGS. Do it here
    # instead of in FindOpenGLES3.cmake so it works also for CMake subprojects
    # (in which case find_package(OpenGLES3) is called in (and so
    # CMAKE_EXE_LINKER_FLAGS would be modified in) Magnum's root CMakeLists.txt
    # and thus can't affect the variable in the outer project). CMake supports
    # IN_LIST as an operator since 3.1 (Emscripten needs at least 3.7), but
    # it's behind a policy, so enable that one as well.
    cmake_policy(SET CMP0057 NEW)
    if(CMAKE_VERSION VERSION_LESS 3.13 AND GL IN_LIST Magnum_FIND_COMPONENTS AND NOT MAGNUM_TARGET_GLES2 AND NOT CMAKE_EXE_LINKER_FLAGS MATCHES "-s USE_WEBGL2=1")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_WEBGL2=1")
    endif()
endif()

# For CMake 3.16+ with REASON_FAILURE_MESSAGE, provide additional potentially
# useful info about the failed components.
if(NOT CMAKE_VERSION VERSION_LESS 3.16)
    set(_MAGNUM_REASON_FAILURE_MESSAGE )
    # Go only through the originally specified find_package() components, not
    # the dependencies added by us afterwards
    foreach(_component ${_MAGNUM_ORIGINAL_FIND_COMPONENTS})
        if(Magnum_${_component}_FOUND)
            continue()
        endif()

        # If it's not known at all, tell the user -- it might be a new library
        # and an old Find module, or something platform-specific.
        if(NOT _component IN_LIST _MAGNUM_LIBRARY_COMPONENTS AND NOT _component IN_LIST _MAGNUM_PLUGIN_COMPONENTS AND NOT _component IN_LIST _MAGNUM_EXECUTABLE_COMPONENTS)
            list(APPEND _MAGNUM_REASON_FAILURE_MESSAGE "${_component} is not a known component on this platform.")
        # Otherwise, if it's not among implicitly built components, hint that
        # the user may need to enable it
        # TODO: currently, the _FOUND variable doesn't reflect if dependencies
        #   were found. When it will, this needs to be updated to avoid
        #   misleading messages.
        elseif(NOT _component IN_LIST _MAGNUM_IMPLICITLY_ENABLED_COMPONENTS)
            string(TOUPPER ${_component} _COMPONENT)
            list(APPEND _MAGNUM_REASON_FAILURE_MESSAGE "${_component} is not built by default. Make sure you enabled WITH_${_COMPONENT} when building Magnum.")
        # Otherwise we have no idea. Better be silent than to print something
        # misleading.
        else()
        endif()
    endforeach()

    string(REPLACE ";" " " _MAGNUM_REASON_FAILURE_MESSAGE "${_MAGNUM_REASON_FAILURE_MESSAGE}")
    set(_MAGNUM_REASON_FAILURE_MESSAGE REASON_FAILURE_MESSAGE "${_MAGNUM_REASON_FAILURE_MESSAGE}")
endif()

# Complete the check with also all components
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Magnum
    REQUIRED_VARS MAGNUM_INCLUDE_DIR MAGNUM_LIBRARY ${MAGNUM_EXTRAS_NEEDED}
    HANDLE_COMPONENTS
    ${_MAGNUM_REASON_FAILURE_MESSAGE})

# Components with optional dependencies -- add them once we know if they were
# found or not.
foreach(_component ${_MAGNUM_OPTIONAL_DEPENDENCIES_TO_ADD})
    foreach(_dependency ${_MAGNUM_${_component}_OPTIONAL_DEPENDENCIES_TO_ADD})
        if(Magnum_${_dependency}_FOUND)
            set_property(TARGET Magnum::${_component} APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES Magnum::${_dependency})
        endif()
    endforeach()
endforeach()

# Create Windowless*Application, *Application and *Context aliases
# TODO: ugh why can't I make an alias of IMPORTED target?
if(_MAGNUM_WINDOWLESSAPPLICATION_ALIAS AND NOT TARGET Magnum::WindowlessApplication)
    get_target_property(_MAGNUM_WINDOWLESSAPPLICATION_ALIASED_TARGET ${_MAGNUM_WINDOWLESSAPPLICATION_ALIAS} ALIASED_TARGET)
    if(_MAGNUM_WINDOWLESSAPPLICATION_ALIASED_TARGET)
        add_library(Magnum::WindowlessApplication ALIAS ${_MAGNUM_WINDOWLESSAPPLICATION_ALIASED_TARGET})
    else()
        add_library(Magnum::WindowlessApplication UNKNOWN IMPORTED)
        foreach(property IMPORTED_CONFIGURATIONS INTERFACE_INCLUDE_DIRECTORIES INTERFACE_COMPILE_DEFINITIONS INTERFACE_COMPILE_OPTIONS INTERFACE_LINK_LIBRARIES)
            get_target_property(_MAGNUM_WINDOWLESSAPPLICATION_${property} ${_MAGNUM_WINDOWLESSAPPLICATION_ALIAS} ${property})
            if(_MAGNUM_WINDOWLESSAPPLICATION_${property})
                set_target_properties(Magnum::WindowlessApplication PROPERTIES
                    ${property} "${_MAGNUM_WINDOWLESSAPPLICATION_${property}}")
            endif()
        endforeach()
        get_target_property(_MAGNUM_WINDOWLESSAPPLICATION_IMPORTED_LOCATION_RELEASE ${_MAGNUM_WINDOWLESSAPPLICATION_ALIAS} IMPORTED_LOCATION_RELEASE)
        get_target_property(_MAGNUM_WINDOWLESSAPPLICATION_IMPORTED_LOCATION_DEBUG ${_MAGNUM_WINDOWLESSAPPLICATION_ALIAS} IMPORTED_LOCATION_DEBUG)
        if(_MAGNUM_WINDOWLESSAPPLICATION_IMPORTED_LOCATION_RELEASE)
            set_target_properties(Magnum::WindowlessApplication PROPERTIES
                IMPORTED_LOCATION_RELEASE ${_MAGNUM_WINDOWLESSAPPLICATION_IMPORTED_LOCATION_RELEASE})
        endif()
        if(_MAGNUM_WINDOWLESSAPPLICATION_IMPORTED_LOCATION_DEBUG)
            set_target_properties(Magnum::WindowlessApplication PROPERTIES
                IMPORTED_LOCATION_DEBUG ${_MAGNUM_WINDOWLESSAPPLICATION_IMPORTED_LOCATION_DEBUG})
        endif()
    endif()
    # Prevent creating the alias again
    unset(_MAGNUM_WINDOWLESSAPPLICATION_ALIAS)
endif()
if(_MAGNUM_APPLICATION_ALIAS AND NOT TARGET Magnum::Application)
    get_target_property(_MAGNUM_APPLICATION_ALIASED_TARGET ${_MAGNUM_APPLICATION_ALIAS} ALIASED_TARGET)
    if(_MAGNUM_APPLICATION_ALIASED_TARGET)
        add_library(Magnum::Application ALIAS ${_MAGNUM_APPLICATION_ALIASED_TARGET})
    else()
        add_library(Magnum::Application UNKNOWN IMPORTED)
        foreach(property IMPORTED_CONFIGURATIONS INTERFACE_INCLUDE_DIRECTORIES INTERFACE_COMPILE_DEFINITIONS INTERFACE_COMPILE_OPTIONS INTERFACE_LINK_LIBRARIES)
            get_target_property(_MAGNUM_APPLICATION_${property}
                ${_MAGNUM_APPLICATION_ALIAS} ${property})
            if(_MAGNUM_APPLICATION_${property})
                set_target_properties(Magnum::Application PROPERTIES ${property}
                    "${_MAGNUM_APPLICATION_${property}}")
            endif()
        endforeach()
        get_target_property(_MAGNUM_APPLICATION_IMPORTED_LOCATION_RELEASE ${_MAGNUM_APPLICATION_ALIAS} IMPORTED_LOCATION_RELEASE)
        get_target_property(_MAGNUM_APPLICATION_IMPORTED_LOCATION_DEBUG ${_MAGNUM_APPLICATION_ALIAS} IMPORTED_LOCATION_DEBUG)
        if(_MAGNUM_APPLICATION_IMPORTED_LOCATION_RELEASE)
            set_target_properties(Magnum::Application PROPERTIES
                IMPORTED_LOCATION_RELEASE ${_MAGNUM_APPLICATION_IMPORTED_LOCATION_RELEASE})
        endif()
        if(_MAGNUM_APPLICATION_IMPORTED_LOCATION_DEBUG)
            set_target_properties(Magnum::Application PROPERTIES
                IMPORTED_LOCATION_DEBUG ${_MAGNUM_APPLICATION_IMPORTED_LOCATION_DEBUG})
        endif()
    endif()
    # Prevent creating the alias again
    unset(_MAGNUM_APPLICATION_ALIAS)
endif()
if(_MAGNUM_GLCONTEXT_ALIAS AND NOT TARGET Magnum::GLContext)
    get_target_property(_MAGNUM_GLCONTEXT_ALIASED_TARGET ${_MAGNUM_GLCONTEXT_ALIAS} ALIASED_TARGET)
    if(_MAGNUM_GLCONTEXT_ALIASED_TARGET)
        add_library(Magnum::GLContext ALIAS ${_MAGNUM_GLCONTEXT_ALIASED_TARGET})
    else()
        add_library(Magnum::GLContext UNKNOWN IMPORTED)
        foreach(property IMPORTED_CONFIGURATIONS INTERFACE_INCLUDE_DIRECTORIES INTERFACE_COMPILE_DEFINITIONS INTERFACE_COMPILE_OPTIONS INTERFACE_LINK_LIBRARIES)
            get_target_property(_MAGNUM_GLCONTEXT_${property} ${_MAGNUM_GLCONTEXT_ALIAS} ${property})
            if(_MAGNUM_GLCONTEXT_${property})
                set_target_properties(Magnum::GLContext PROPERTIES ${property}
                    "${_MAGNUM_GLCONTEXT_${property}}")
            endif()
        endforeach()
        get_target_property(_MAGNUM_GLCONTEXT_IMPORTED_LOCATION_RELEASE ${_MAGNUM_GLCONTEXT_ALIAS} IMPORTED_LOCATION_RELEASE)
        get_target_property(_MAGNUM_GLCONTEXT_IMPORTED_LOCATION_DEBUG ${_MAGNUM_GLCONTEXT_ALIAS} IMPORTED_LOCATION_DEBUG)
        if(_MAGNUM_GLCONTEXT_IMPORTED_LOCATION_RELEASE)
            set_target_properties(Magnum::GLContext PROPERTIES
                IMPORTED_LOCATION_RELEASE ${_MAGNUM_GLCONTEXT_IMPORTED_LOCATION_RELEASE})
        endif()
        if(_MAGNUM_GLCONTEXT_IMPORTED_LOCATION_DEBUG)
            set_target_properties(Magnum::GLContext PROPERTIES
                IMPORTED_LOCATION_DEBUG ${_MAGNUM_GLCONTEXT_IMPORTED_LOCATION_DEBUG})
        endif()
    endif()
    # Prevent creating the alias again
    unset(_MAGNUM_GLCONTEXT_ALIAS)
endif()

# Installation and deploy dirs
set(MAGNUM_DEPLOY_PREFIX "."
    CACHE STRING "Prefix where to put final application executables")

include(${CORRADE_LIB_SUFFIX_MODULE})
set(MAGNUM_BINARY_INSTALL_DIR bin)
set(MAGNUM_LIBRARY_INSTALL_DIR lib${LIB_SUFFIX})
set(MAGNUM_DATA_INSTALL_DIR share/magnum)
set(MAGNUM_INCLUDE_INSTALL_DIR include/Magnum)
set(MAGNUM_EXTERNAL_INCLUDE_INSTALL_DIR include/MagnumExternal)
set(MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR include/MagnumPlugins)
if(MAGNUM_BUILD_DEPRECATED AND MAGNUM_INCLUDE_INSTALL_PREFIX AND NOT MAGNUM_INCLUDE_INSTALL_PREFIX STREQUAL ".")
    message(DEPRECATION "MAGNUM_INCLUDE_INSTALL_PREFIX is obsolete as its primary use was for old Android NDK versions. Please switch to the NDK r19+ layout instead of using this variable and recreate your build directory to get rid of this warning.")
    set(MAGNUM_DATA_INSTALL_DIR ${MAGNUM_INCLUDE_INSTALL_PREFIX}/${MAGNUM_DATA_INSTALL_DIR})
    set(MAGNUM_INCLUDE_INSTALL_DIR ${MAGNUM_INCLUDE_INSTALL_PREFIX}/${MAGNUM_INCLUDE_INSTALL_DIR})
    set(MAGNUM_EXTERNAL_INCLUDE_INSTALL_DIR ${MAGNUM_INCLUDE_INSTALL_PREFIX}/${MAGNUM_EXTERNAL_INCLUDE_INSTALL_DIR})
    set(MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR ${MAGNUM_INCLUDE_INSTALL_PREFIX}/${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR})
endif()

set(MAGNUM_PLUGINS_DEBUG_BINARY_INSTALL_DIR ${MAGNUM_BINARY_INSTALL_DIR}/magnum-d)
set(MAGNUM_PLUGINS_DEBUG_LIBRARY_INSTALL_DIR ${MAGNUM_LIBRARY_INSTALL_DIR}/magnum-d)
set(MAGNUM_PLUGINS_RELEASE_BINARY_INSTALL_DIR ${MAGNUM_BINARY_INSTALL_DIR}/magnum)
set(MAGNUM_PLUGINS_RELEASE_LIBRARY_INSTALL_DIR ${MAGNUM_LIBRARY_INSTALL_DIR}/magnum)

set(MAGNUM_PLUGINS_SHADERCONVERTER_DEBUG_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_BINARY_INSTALL_DIR}/shaderconverters)
set(MAGNUM_PLUGINS_SHADERCONVERTER_DEBUG_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_LIBRARY_INSTALL_DIR}/shaderconverters)
set(MAGNUM_PLUGINS_SHADERCONVERTER_RELEASE_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_LIBRARY_INSTALL_DIR}/shaderconverters)
set(MAGNUM_PLUGINS_SHADERCONVERTER_RELEASE_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_BINARY_INSTALL_DIR}/shaderconverters)
set(MAGNUM_PLUGINS_FONT_DEBUG_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_BINARY_INSTALL_DIR}/fonts)
set(MAGNUM_PLUGINS_FONT_DEBUG_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_LIBRARY_INSTALL_DIR}/fonts)
set(MAGNUM_PLUGINS_FONT_RELEASE_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_BINARY_INSTALL_DIR}/fonts)
set(MAGNUM_PLUGINS_FONT_RELEASE_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_LIBRARY_INSTALL_DIR}/fonts)
set(MAGNUM_PLUGINS_FONTCONVERTER_DEBUG_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_BINARY_INSTALL_DIR}/fontconverters)
set(MAGNUM_PLUGINS_FONTCONVERTER_RELEASE_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_LIBRARY_INSTALL_DIR}/fontconverters)
set(MAGNUM_PLUGINS_IMAGECONVERTER_DEBUG_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_BINARY_INSTALL_DIR}/imageconverters)
set(MAGNUM_PLUGINS_IMAGECONVERTER_DEBUG_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_LIBRARY_INSTALL_DIR}/imageconverters)
set(MAGNUM_PLUGINS_IMAGECONVERTER_RELEASE_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_LIBRARY_INSTALL_DIR}/imageconverters)
set(MAGNUM_PLUGINS_IMAGECONVERTER_RELEASE_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_BINARY_INSTALL_DIR}/imageconverters)
set(MAGNUM_PLUGINS_IMPORTER_DEBUG_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_BINARY_INSTALL_DIR}/importers)
set(MAGNUM_PLUGINS_IMPORTER_DEBUG_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_LIBRARY_INSTALL_DIR}/importers)
set(MAGNUM_PLUGINS_IMPORTER_RELEASE_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_BINARY_INSTALL_DIR}/importers)
set(MAGNUM_PLUGINS_IMPORTER_RELEASE_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_LIBRARY_INSTALL_DIR}/importers)
set(MAGNUM_PLUGINS_SCENECONVERTER_DEBUG_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_BINARY_INSTALL_DIR}/sceneconverters)
set(MAGNUM_PLUGINS_SCENECONVERTER_DEBUG_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_LIBRARY_INSTALL_DIR}/sceneconverters)
set(MAGNUM_PLUGINS_SCENECONVERTER_RELEASE_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_LIBRARY_INSTALL_DIR}/sceneconverters)
set(MAGNUM_PLUGINS_SCENECONVERTER_RELEASE_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_BINARY_INSTALL_DIR}/sceneconverters)
set(MAGNUM_PLUGINS_AUDIOIMPORTER_DEBUG_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_BINARY_INSTALL_DIR}/audioimporters)
set(MAGNUM_PLUGINS_AUDIOIMPORTER_DEBUG_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_LIBRARY_INSTALL_DIR}/audioimporters)
set(MAGNUM_PLUGINS_AUDIOIMPORTER_RELEASE_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_BINARY_INSTALL_DIR}/audioimporters)
set(MAGNUM_PLUGINS_AUDIOIMPORTER_RELEASE_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_LIBRARY_INSTALL_DIR}/audioimporters)

# Get base plugin directory from main library location. This is *not* PATH,
# because CMake always converts the path to an absolute location internally,
# making it impossible to specify relative paths there. Sorry in advance for
# not having the dir selection button in CMake GUI.
set(MAGNUM_PLUGINS_DEBUG_DIR ""
    CACHE STRING "Base directory where to look for Magnum plugins for debug builds")
set(MAGNUM_PLUGINS_RELEASE_DIR ""
    CACHE STRING "Base directory where to look for Magnum plugins for release builds")
set(MAGNUM_PLUGINS_DIR ""
    CACHE STRING "Base directory where to look for Magnum plugins")

# Plugin directories. Set only if the above are non-empty. otherwise empty as
# well.
if(MAGNUM_PLUGINS_DIR)
    set(MAGNUM_PLUGINS_FONT_DIR ${MAGNUM_PLUGINS_DIR}/fonts)
    set(MAGNUM_PLUGINS_FONTCONVERTER_DIR ${MAGNUM_PLUGINS_DIR}/fontconverters)
    set(MAGNUM_PLUGINS_IMAGECONVERTER_DIR ${MAGNUM_PLUGINS_DIR}/imageconverters)
    set(MAGNUM_PLUGINS_IMPORTER_DIR ${MAGNUM_PLUGINS_DIR}/importers)
    set(MAGNUM_PLUGINS_SCENECONVERTER_DIR ${MAGNUM_PLUGINS_DIR}/sceneconverters)
    set(MAGNUM_PLUGINS_AUDIOIMPORTER_DIR ${MAGNUM_PLUGINS_DIR}/audioimporters)
endif()
if(MAGNUM_PLUGINS_DEBUG_DIR)
    set(MAGNUM_PLUGINS_FONT_DEBUG_DIR ${MAGNUM_PLUGINS_DEBUG_DIR}/fonts)
    set(MAGNUM_PLUGINS_FONTCONVERTER_DEBUG_DIR ${MAGNUM_PLUGINS_DEBUG_DIR}/fontconverters)
    set(MAGNUM_PLUGINS_IMAGECONVERTER_DEBUG_DIR ${MAGNUM_PLUGINS_DEBUG_DIR}/imageconverters)
    set(MAGNUM_PLUGINS_IMPORTER_DEBUG_DIR ${MAGNUM_PLUGINS_DEBUG_DIR}/importers)
    set(MAGNUM_PLUGINS_FONT_RELEASE_DIR ${MAGNUM_PLUGINS_RELEASE_DIR}/fonts)
    set(MAGNUM_PLUGINS_SCENECONVERTER_DEBUG_DIR ${MAGNUM_PLUGINS_DEBUG_DIR}/sceneconverters)
    set(MAGNUM_PLUGINS_AUDIOIMPORTER_DEBUG_DIR ${MAGNUM_PLUGINS_DEBUG_DIR}/audioimporters)
endif()
if(MAGNUM_PLUGINS_RELEASE_DIR)
    set(MAGNUM_PLUGINS_FONTCONVERTER_RELEASE_DIR ${MAGNUM_PLUGINS_RELEASE_DIR}/fontconverters)
    set(MAGNUM_PLUGINS_IMAGECONVERTER_RELEASE_DIR ${MAGNUM_PLUGINS_RELEASE_DIR}/imageconverters)
    set(MAGNUM_PLUGINS_IMPORTER_RELEASE_DIR ${MAGNUM_PLUGINS_RELEASE_DIR}/importers)
    set(MAGNUM_PLUGINS_SCENECONVERTER_RELEASE_DIR ${MAGNUM_PLUGINS_RELEASE_DIR}/sceneconverters)
    set(MAGNUM_PLUGINS_AUDIOIMPORTER_RELEASE_DIR ${MAGNUM_PLUGINS_RELEASE_DIR}/audioimporters)
endif()
