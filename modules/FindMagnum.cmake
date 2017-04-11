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
#  MAGNUM_PLUGINS_DEBUG_DIR     - Base directory with dynamic plugins for
#   debug builds, defaults to magnum-d/ subdirectory of dir where Magnum
#   library was found
#  MAGNUM_PLUGINS_RELEASE_DIR   - Base directory with dynamic plugins for
#   release builds, defaults to magnum/ subdirectory of dir where Magnum
#   library was found
#  MAGNUM_PLUGINS_DIR           - Base directory with dynamic plugins, defaults
#   to :variable:`MAGNUM_PLUGINS_RELEASE_DIR` in release builds and
#   multi-configuration builds or to :variable:`MAGNUM_PLUGINS_DEBUG_DIR` in
#   debug builds. You can modify all three variables (e.g. set them to ``.``
#   when deploying on Windows with plugins stored relatively to the
#   executable), the following ``MAGNUM_PLUGINS_*_DIR`` variables depend on it.
#  MAGNUM_PLUGINS_FONT[|_DEBUG|_RELEASE]_DIR - Directory with dynamic font
#   plugins
#  MAGNUM_PLUGINS_FONTCONVERTER[|_DEBUG|_RELEASE]_DIR - Directory with dynamic
#   font converter plugins
#  MAGNUM_PLUGINS_IMAGECONVERTER[|_DEBUG|_RELEASE]_DIR - Directory with dynamic
#   image converter plugins
#  MAGNUM_PLUGINS_IMPORTER[|_DEBUG|_RELEASE]_DIR  - Directory with dynamic
#   importer plugins
#  MAGNUM_PLUGINS_AUDIOIMPORTER[|_DEBUG|_RELEASE]_DIR - Directory with dynamic
#   audio importer plugins
#
# This command will try to find only the base library, not the optional
# components. The base library depends on Corrade and OpenGL libraries (or
# OpenGL ES libraries). Additional dependencies are specified by the
# components. The optional components are:
#
#  Audio                        - Audio library
#  DebugTools                   - DebugTools library
#  MeshTools                    - MeshTools library
#  Primitives                   - Primitives library
#  SceneGraph                   - SceneGraph library
#  Shaders                      - Shaders library
#  Shapes                       - Shapes library
#  Text                         - Text library
#  TextureTools                 - TextureTools library
#  GlfwApplication              - GLFW application
#  GlutApplication              - GLUT application
#  GlxApplication               - GLX application
#  NaClApplication              - NaCl application
#  Sdl2Application              - SDL2 application
#  XEglApplication              - X/EGL application
#  WindowlessCglApplication     - Windowless CGL application
#  WindowlessEglApplication     - Windowless EGL application
#  WindowlessGlxApplication     - Windowless GLX application
#  WindowlessIosApplication     - Windowless iOS application
#  WindowlessNaClApplication    - Windowless NaCl application
#  WindowlessWglApplication     - Windowless WGL application
#  WindowlessWindowsEglApplication - Windowless Windows/EGL application
#  CglContext                   - CGL context
#  EglContext                   - EGL context
#  GlxContext                   - GLX context
#  WglContext                   - WGL context
#  OpenGLTester                 - OpenGLTester class
#  MagnumFont                   - Magnum bitmap font plugin
#  MagnumFontConverter          - Magnum bitmap font converter plugin
#  ObjImporter                  - OBJ importer plugin
#  TgaImageConverter            - TGA image converter plugin
#  TgaImporter                  - TGA importer plugin
#  WavAudioImporter             - WAV audio importer plugin
#  distancefieldconverter       - magnum-distancefieldconverter executable
#  fontconverter                - magnum-fontconverter executable
#  imageconverter               - magnum-imageconverter executable
#  info                         - magnum-info executable
#  al-info                      - magnum-al-info executable
#
# Example usage with specifying additional components is::
#
#  find_package(Magnum REQUIRED MeshTools Primitives GlutApplication)
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
# found, its target is available in convenience alias ``Magnum::Context``.
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
#  MAGNUM_BUILD_MULTITHREADED   - Defined if compiled in a way that allows
#   having multiple thread-local Magnum contexts
#  MAGNUM_TARGET_GLES           - Defined if compiled for OpenGL ES
#  MAGNUM_TARGET_GLES2          - Defined if compiled for OpenGL ES 2.0
#  MAGNUM_TARGET_GLES3          - Defined if compiled for OpenGL ES 3.0
#  MAGNUM_TARGET_DESKTOP_GLES   - Defined if compiled with OpenGL ES
#   emulation on desktop OpenGL
#  MAGNUM_TARGET_WEBGL          - Defined if compiled for WebGL
#  MAGNUM_TARGET_HEADLESS       - Defined if compiled for headless machines
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
#  MAGNUM_PLUGINS_AUDIOIMPORTER_[DEBUG|RELEASE]_BINARY_INSTALL_DIR - Audio
#   importer plugin binary installation directory
#  MAGNUM_PLUGINS_AUDIOIMPORTER_[DEBUG|RELEASE]_LIBRARY_INSTALL_DIR - Audio
#   importer plugin library installation directory
#  MAGNUM_INCLUDE_INSTALL_DIR   - Header installation directory
#  MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR - Plugin header installation directory
#
# Workflows without imported targets are deprecated and the following variables
# are included just for backwards compatibility and only if
# :variable:`MAGNUM_BUILD_DEPRECATED` is enabled:
#
#  MAGNUM_LIBRARIES            - Expands to ``Magnum::Magnum`` target. Use
#   ``Magnum::Magnum`` target directly instead.
#  MAGNUM_*_LIBRARIES           - Expands to ``Magnum::*`` target. Use
#   ``Magnum::*`` target directly instead.
#  MAGNUM_APPLICATION_LIBRARIES / MAGNUM_WINDOWLESSAPPLICATION_LIBRARIES
#                               - Expands to ``Magnum::Application`` /
#   ``Magnum::WindowlessApplication`` target. Use ``Magnum::Application`` /
#   ``Magnum::WindowlessApplication`` target directly instead.
#  MAGNUM_CONTEXT_LIBRARIES     - Expands to ``Magnum::Context`` target. Use
#   ``Magnum::Context`` target directly instead.
#

#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

# Dependencies
find_package(Corrade REQUIRED Utility PluginManager)

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
set(_magnumFlags
    BUILD_DEPRECATED
    BUILD_STATIC
    BUILD_MULTITHREADED
    TARGET_GLES
    TARGET_GLES2
    TARGET_GLES3
    TARGET_DESKTOP_GLES
    TARGET_WEBGL
    TARGET_HEADLESS)
foreach(_magnumFlag ${_magnumFlags})
    string(FIND "${_magnumConfigure}" "#define MAGNUM_${_magnumFlag}" _magnum_${_magnumFlag})
    if(NOT _magnum_${_magnumFlag} EQUAL -1)
        set(MAGNUM_${_magnumFlag} 1)
    endif()
endforeach()

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
        ${MAGNUM_INCLUDE_DIR}
        ${MAGNUM_INCLUDE_DIR}/MagnumExternal/OpenGL)

    # Dependent libraries
    set_property(TARGET Magnum::Magnum APPEND PROPERTY INTERFACE_LINK_LIBRARIES
         Corrade::Utility
         Corrade::PluginManager)

    # Dependent libraries and includes
    if(NOT MAGNUM_TARGET_GLES OR MAGNUM_TARGET_DESKTOP_GLES)
        find_package(OpenGL REQUIRED)
        set_property(TARGET Magnum::Magnum APPEND PROPERTY
            INTERFACE_LINK_LIBRARIES ${OPENGL_gl_LIBRARY})
    elseif(MAGNUM_TARGET_GLES2)
        find_package(OpenGLES2 REQUIRED)
        set_property(TARGET Magnum::Magnum APPEND PROPERTY
            INTERFACE_LINK_LIBRARIES OpenGLES2::OpenGLES2)
    elseif(MAGNUM_TARGET_GLES3)
        find_package(OpenGLES3 REQUIRED)
        set_property(TARGET Magnum::Magnum APPEND PROPERTY
            INTERFACE_LINK_LIBRARIES OpenGLES3::OpenGLES3)
    endif()

    # Emscripten needs special flag to use WebGL 2
    if(CORRADE_TARGET_EMSCRIPTEN AND NOT MAGNUM_TARGET_GLES2)
        # TODO: give me INTERFACE_LINK_OPTIONS or something, please
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_WEBGL2=1")
    endif()
else()
    set(MAGNUM_LIBRARY Magnum::Magnum)
endif()

# Ensure that all inter-component dependencies are specified as well
set(_MAGNUM_ADDITIONAL_COMPONENTS )
foreach(_component ${Magnum_FIND_COMPONENTS})
    string(TOUPPER ${_component} _COMPONENT)

    if(_component STREQUAL Shapes)
        set(_MAGNUM_${_COMPONENT}_DEPENDENCIES SceneGraph)
    elseif(_component STREQUAL Text)
        set(_MAGNUM_${_COMPONENT}_DEPENDENCIES TextureTools)
    elseif(_component STREQUAL DebugTools)
        set(_MAGNUM_${_COMPONENT}_DEPENDENCIES MeshTools Primitives SceneGraph Shaders Shapes)
    elseif(_component STREQUAL OpenGLTester)
        if(MAGNUM_TARGET_HEADLESS)
            set(_MAGNUM_${_COMPONENT}_DEPENDENCIES WindowlessEglApplication)
        elseif(CORRADE_TARGET_IOS)
            set(_MAGNUM_${_COMPONENT}_DEPENDENCIES WindowlessIosApplication)
        elseif(CORRADE_TARGET_APPLE)
            set(_MAGNUM_${_COMPONENT}_DEPENDENCIES WindowlessCglApplication)
        elseif(CORRADE_TARGET_UNIX)
            if(MAGNUM_TARGET_GLES AND NOT MAGNUM_TARGET_DESKTOP_GLES)
                set(_MAGNUM_${_COMPONENT}_DEPENDENCIES WindowlessEglApplication)
            else()
                set(_MAGNUM_${_COMPONENT}_DEPENDENCIES WindowlessGlxApplication)
            endif()
        elseif(CORRADE_TARGET_WINDOWS)
            if(NOT MAGNUM_TARGET_GLES OR MAGNUM_TARGET_DESKTOP_GLES)
                set(_MAGNUM_${_COMPONENT}_DEPENDENCIES WindowlessWglApplication)
            else()
                set(_MAGNUM_${_COMPONENT}_DEPENDENCIES WindowlessWindowsEglApplication)
            endif()
        endif()
    elseif(_component STREQUAL MagnumFont)
        set(_MAGNUM_${_COMPONENT}_DEPENDENCIES TgaImporter) # and below
    elseif(_component STREQUAL MagnumFontConverter)
        set(_MAGNUM_${_COMPONENT}_DEPENDENCIES TgaImageConverter) # and below
    elseif(_component STREQUAL ObjImporter)
        set(_MAGNUM_${_COMPONENT}_DEPENDENCIES MeshTools)
    endif()

    if(_component MATCHES ".+AudioImporter")
        list(APPEND _MAGNUM_${_COMPONENT}_DEPENDENCIES Audio)
    elseif(_component MATCHES ".+(Font|FontConverter)")
        list(APPEND _MAGNUM_${_COMPONENT}_DEPENDENCIES Text TextureTools)
    endif()

    # Mark the dependencies as required if the component is also required
    if(Magnum_FIND_REQUIRED_${_component})
        foreach(_dependency ${_MAGNUM_${_COMPONENT}_DEPENDENCIES})
            set(Magnum_FIND_REQUIRED_${_dependency} TRUE)
        endforeach()
    endif()

    list(APPEND _MAGNUM_ADDITIONAL_COMPONENTS ${_MAGNUM_${_COMPONENT}_DEPENDENCIES})
endforeach()

# Join the lists, remove duplicate components
if(_MAGNUM_ADDITIONAL_COMPONENTS)
    list(INSERT Magnum_FIND_COMPONENTS 0 ${_MAGNUM_ADDITIONAL_COMPONENTS})
endif()
if(Magnum_FIND_COMPONENTS)
    list(REMOVE_DUPLICATES Magnum_FIND_COMPONENTS)
endif()

# Component distinction (listing them explicitly to avoid mistakes with finding
# components from other repositories)
set(_MAGNUM_LIBRARY_COMPONENTS "^(Audio|DebugTools|MeshTools|Primitives|SceneGraph|Shaders|Shapes|Text|TextureTools|AndroidApplication|GlfwApplication|GlutApplication|GlxApplication|NaClApplication|Sdl2Application|XEglApplication|WindowlessCglApplication|WindowlessEglApplication|WindowlessGlxApplication|WindowlessIosApplication|WindowlessNaClApplication|WindowlessWglApplication|WindowlessWindowsEglApplication|CglContext|EglContext|GlxContext|WglContext|OpenGLTester)$")
set(_MAGNUM_PLUGIN_COMPONENTS "^(MagnumFont|MagnumFontConverter|ObjImporter|TgaImageConverter|TgaImporter|WavAudioImporter)$")
set(_MAGNUM_EXECUTABLE_COMPONENTS "^(distancefieldconverter|fontconverter|imageconverter|info|al-info)$")

# Find all components
foreach(_component ${Magnum_FIND_COMPONENTS})
    string(TOUPPER ${_component} _COMPONENT)

    # Create imported target in case the library is found. If the project is
    # added as subproject to CMake, the target already exists and all the
    # required setup is already done from the build tree.
    if(TARGET Magnum::${_component})
        set(Magnum_${_component}_FOUND TRUE)
    else()
        # Library components
        if(_component MATCHES ${_MAGNUM_LIBRARY_COMPONENTS})
            add_library(Magnum::${_component} UNKNOWN IMPORTED)

            # Set library defaults, find the library
            set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_SUFFIX Magnum/${_component})
            set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES ${_component}.h)

            # Try to find both debug and release version
            find_library(MAGNUM_${_COMPONENT}_LIBRARY_DEBUG Magnum${_component}-d)
            find_library(MAGNUM_${_COMPONENT}_LIBRARY_RELEASE Magnum${_component})
            mark_as_advanced(MAGNUM_${_COMPONENT}_LIBRARY_DEBUG
                MAGNUM_${_COMPONENT}_LIBRARY_RELEASE)
        endif()

        # Plugin components
        if(_component MATCHES ${_MAGNUM_PLUGIN_COMPONENTS})
            add_library(Magnum::${_component} UNKNOWN IMPORTED)

            # AudioImporter plugin specific name suffixes
            if(_component MATCHES ".+AudioImporter$")
                set(_MAGNUM_${_COMPONENT}_PATH_SUFFIX audioimporters)

                # Audio importer class is Audio::*Importer, thus we need to
                # convert *AudioImporter.h to *Importer.h
                string(REPLACE "AudioImporter" "Importer" _MAGNUM_${_COMPONENT}_HEADER_NAME "${_component}")
                set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES ${_MAGNUM_${_COMPONENT}_HEADER_NAME}.h)

            # Importer plugin specific name suffixes
            elseif(_component MATCHES ".+Importer$")
                set(_MAGNUM_${_COMPONENT}_PATH_SUFFIX importers)

            # Font plugin specific name suffixes
            elseif(_component MATCHES ".+Font$")
                set(_MAGNUM_${_COMPONENT}_PATH_SUFFIX fonts)

            # ImageConverter plugin specific name suffixes
            elseif(_component MATCHES ".+ImageConverter$")
                set(_MAGNUM_${_COMPONENT}_PATH_SUFFIX imageconverters)

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
            # accidentaly break something else
            set(_tmp_prefixes ${CMAKE_FIND_LIBRARY_PREFIXES})
            set(CMAKE_FIND_LIBRARY_PREFIXES ${CMAKE_FIND_LIBRARY_PREFIXES} "")

            # Try to find both debug and release version. Dynamic and static
            # debug libraries are in different places.
            find_library(MAGNUM_${_COMPONENT}_LIBRARY_DEBUG ${_component}
                PATH_SUFFIXES magnum-d/${_MAGNUM_${_COMPONENT}_PATH_SUFFIX})
            find_library(MAGNUM_${_COMPONENT}_LIBRARY_DEBUG ${_component}-d
                PATH_SUFFIXES magnum/${_MAGNUM_${_COMPONENT}_PATH_SUFFIX})
            find_library(MAGNUM_${_COMPONENT}_LIBRARY_RELEASE ${_component}
                PATH_SUFFIXES magnum/${_MAGNUM_${_COMPONENT}_PATH_SUFFIX})
            mark_as_advanced(MAGNUM_${_COMPONENT}_LIBRARY_DEBUG
                MAGNUM_${_COMPONENT}_LIBRARY_RELEASE)

            # Reset back
            set(CMAKE_FIND_LIBRARY_PREFIXES "${_tmp_prefixes}")
        endif()

        # Library location for libraries/plugins
        if(_component MATCHES ${_MAGNUM_LIBRARY_COMPONENTS} OR _component MATCHES ${_MAGNUM_PLUGIN_COMPONENTS})
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

        # Executables
        if(_component MATCHES ${_MAGNUM_EXECUTABLE_COMPONENTS})
            add_executable(Magnum::${_component} IMPORTED)

            find_program(MAGNUM_${_COMPONENT}_EXECUTABLE magnum-${_component})
            mark_as_advanced(MAGNUM_${_COMPONENT}_EXECUTABLE)

            if(MAGNUM_${_COMPONENT}_EXECUTABLE)
                set_property(TARGET Magnum::${_component} PROPERTY
                    IMPORTED_LOCATION ${MAGNUM_${_COMPONENT}_EXECUTABLE})
            endif()
        endif()

        # Applications
        if(_component MATCHES ".+Application")
            set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_SUFFIX Magnum/Platform)

            # Android application dependencies
            if(_component STREQUAL AndroidApplication)
                find_package(EGL)
                set_property(TARGET Magnum::${_component} APPEND PROPERTY
                    INTERFACE_INCLUDE_DIRECTORIES ${ANDROID_NATIVE_APP_GLUE_INCLUDE_DIR})
                set_property(TARGET Magnum::${_component} APPEND PROPERTY
                    INTERFACE_LINK_LIBRARIES android EGL::EGL)

            # GLFW application dependencies
            elseif(_component STREQUAL GlfwApplication)
                find_package(GLFW)
                set_property(TARGET Magnum::${_component} APPEND PROPERTY
                    INTERFACE_LINK_LIBRARIES GLFW::GLFW)

            # GLUT application dependencies
            elseif(_component STREQUAL GlutApplication)
                find_package(GLUT)
                set_property(TARGET Magnum::${_component} APPEND PROPERTY
                    INTERFACE_INCLUDE_DIRECTORIES ${GLUT_INCLUDE_DIR})
                set_property(TARGET Magnum::${_component} APPEND PROPERTY
                    INTERFACE_LINK_LIBRARIES ${GLUT_glut_LIBRARY})

            # SDL2 application dependencies
            elseif(_component STREQUAL Sdl2Application)
                find_package(SDL2)
                set_property(TARGET Magnum::${_component} APPEND PROPERTY
                    INTERFACE_LINK_LIBRARIES SDL2::SDL2)

            # (Windowless) NaCl application dependencies
            elseif(_component STREQUAL NaClApplication OR _component STREQUAL WindowlessNaClApplication)
                set_property(TARGET Magnum::${_component} APPEND PROPERTY
                    INTERFACE_LINK_LIBRARIES ppapi_cpp ppapi)

            # (Windowless) GLX application dependencies
            elseif(_component STREQUAL GlxApplication OR _component STREQUAL WindowlessGlxApplication)
                find_package(X11)
                set_property(TARGET Magnum::${_component} APPEND PROPERTY
                    INTERFACE_INCLUDE_DIRECTORIES ${X11_INCLUDE_DIR})
                set_property(TARGET Magnum::${_component} APPEND PROPERTY
                    INTERFACE_LINK_LIBRARIES ${X11_LIBRARIES})

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
            set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES Context.h)

            # GLX context dependencies
            if(_component STREQUAL GlxContext)
                find_package(X11)
                set_property(TARGET Magnum::${_component} APPEND PROPERTY
                    INTERFACE_INCLUDE_DIRECTORIES ${X11_INCLUDE_DIR})
                set_property(TARGET Magnum::${_component} APPEND PROPERTY
                    INTERFACE_LINK_LIBRARIES ${X11_LIBRARIES})

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
                INTERFACE_INCLUDE_DIRECTORIES ${OPENAL_INCLUDE_DIR})
            set_property(TARGET Magnum::${_component} APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES ${OPENAL_LIBRARY})

        # No special setup for DebugTools library

        # MeshTools library
        elseif(_component STREQUAL MeshTools)
            set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES CompressIndices.h)

        # OpenGLTester library
        elseif(_component STREQUAL OpenGLTester)
            set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_SUFFIX Magnum)

        # Primitives library
        elseif(_component STREQUAL Primitives)
            set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES Cube.h)

        # No special setup for SceneGraph library
        # No special setup for Shaders library
        # No special setup for Shapes library
        # No special setup for Text library

        # TextureTools library
        elseif(_component STREQUAL TextureTools)
            set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES Atlas.h)
        endif()

        # Find library/plugin includes
        if(_component MATCHES ${_MAGNUM_LIBRARY_COMPONENTS} OR _component MATCHES ${_MAGNUM_PLUGIN_COMPONENTS})
            find_path(_MAGNUM_${_COMPONENT}_INCLUDE_DIR
                NAMES ${_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES}
                HINTS ${MAGNUM_INCLUDE_DIR}/${_MAGNUM_${_COMPONENT}_INCLUDE_PATH_SUFFIX})
            mark_as_advanced(_MAGNUM_${_COMPONENT}_INCLUDE_DIR)
        endif()

        # Link to core Magnum library, add inter-library dependencies
        if(_component MATCHES ${_MAGNUM_LIBRARY_COMPONENTS} OR _component MATCHES ${_MAGNUM_PLUGIN_COMPONENTS})
            set_property(TARGET Magnum::${_component} APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES Magnum::Magnum)
            foreach(_dependency ${_MAGNUM_${_COMPONENT}_DEPENDENCIES})
                set_property(TARGET Magnum::${_component} APPEND PROPERTY
                    INTERFACE_LINK_LIBRARIES Magnum::${_dependency})
            endforeach()
        endif()

        # Decide if the library was found
        if(((_component MATCHES ${_MAGNUM_LIBRARY_COMPONENTS} OR _component MATCHES ${_MAGNUM_PLUGIN_COMPONENTS}) AND _MAGNUM_${_COMPONENT}_INCLUDE_DIR AND (MAGNUM_${_COMPONENT}_LIBRARY_DEBUG OR MAGNUM_${_COMPONENT}_LIBRARY_RELEASE)) OR (_component MATCHES ${_MAGNUM_EXECUTABLE_COMPONENTS} AND MAGNUM_${_COMPONENT}_EXECUTABLE))
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
        if(NOT DEFINED _MAGNUM_CONTEXT_ALIAS)
            set(_MAGNUM_CONTEXT_ALIAS Magnum::${_component})
        else()
            unset(_MAGNUM_CONTEXT_ALIAS)
        endif()
    endif()

    # Deprecated variables
    if(MAGNUM_BUILD_DEPRECATED AND _component MATCHES ${_MAGNUM_LIBRARY_COMPONENTS} OR _component MATCHES ${_MAGNUM_PLUGIN_COMPONENTS})
        set(MAGNUM_${_COMPONENT}_LIBRARIES Magnum::${_component})
    endif()
endforeach()

# Complete the check with also all components
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Magnum
    REQUIRED_VARS MAGNUM_INCLUDE_DIR MAGNUM_LIBRARY
    HANDLE_COMPONENTS)

# Create Windowless*Application, *Application and *Context aliases
# TODO: ugh why can't I make an alias of IMPORTED target?
if(_MAGNUM_WINDOWLESSAPPLICATION_ALIAS AND NOT TARGET Magnum::WindowlessApplication)
    get_target_property(_MAGNUM_WINDOWLESSAPPLICATION_ALIASED_TARGET ${_MAGNUM_WINDOWLESSAPPLICATION_ALIAS} ALIASED_TARGET)
    if(_MAGNUM_WINDOWLESSAPPLICATION_ALIASED_TARGET)
        add_library(Magnum::WindowlessApplication ALIAS ${_MAGNUM_WINDOWLESSAPPLICATION_ALIASED_TARGET})
    else()
        add_library(Magnum::WindowlessApplication UNKNOWN IMPORTED)
        get_target_property(_MAGNUM_WINDOWLESSAPPLICATION_IMPORTED_CONFIGURATIONS ${_MAGNUM_WINDOWLESSAPPLICATION_ALIAS} IMPORTED_CONFIGURATIONS)
        get_target_property(_MAGNUM_WINDOWLESSAPPLICATION_IMPORTED_LOCATION_RELEASE ${_MAGNUM_WINDOWLESSAPPLICATION_ALIAS} IMPORTED_LOCATION_RELEASE)
        get_target_property(_MAGNUM_WINDOWLESSAPPLICATION_IMPORTED_LOCATION_DEBUG ${_MAGNUM_WINDOWLESSAPPLICATION_ALIAS} IMPORTED_LOCATION_DEBUG)
        set_target_properties(Magnum::WindowlessApplication PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:${_MAGNUM_WINDOWLESSAPPLICATION_ALIAS},INTERFACE_INCLUDE_DIRECTORIES>
            INTERFACE_COMPILE_DEFINITIONS $<TARGET_PROPERTY:${_MAGNUM_WINDOWLESSAPPLICATION_ALIAS},INTERFACE_COMPILE_DEFINITIONS>
            INTERFACE_COMPILE_OPTIONS $<TARGET_PROPERTY:${_MAGNUM_WINDOWLESSAPPLICATION_ALIAS},INTERFACE_COMPILE_OPTIONS>
            INTERFACE_LINK_LIBRARIES $<TARGET_PROPERTY:${_MAGNUM_WINDOWLESSAPPLICATION_ALIAS},INTERFACE_LINK_LIBRARIES>
            IMPORTED_CONFIGURATIONS "${_MAGNUM_WINDOWLESSAPPLICATION_IMPORTED_CONFIGURATIONS}")
        if(_MAGNUM_WINDOWLESSAPPLICATION_IMPORTED_LOCATION_RELEASE)
            set_target_properties(Magnum::WindowlessApplication PROPERTIES
                IMPORTED_LOCATION_RELEASE ${_MAGNUM_WINDOWLESSAPPLICATION_IMPORTED_LOCATION_RELEASE})
        endif()
        if(_MAGNUM_WINDOWLESSAPPLICATION_IMPORTED_LOCATION_DEBUG)
            set_target_properties(Magnum::WindowlessApplication PROPERTIES
                IMPORTED_LOCATION_DEBUG ${_MAGNUM_WINDOWLESSAPPLICATION_IMPORTED_LOCATION_DEBUG})
        endif()
    endif()
    if(MAGNUM_BUILD_DEPRECATED)
        set(MAGNUM_WINDOWLESSAPPLICATION_LIBRARIES Magnum::WindowlessApplication)
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
        get_target_property(_MAGNUM_APPLICATION_IMPORTED_CONFIGURATIONS ${_MAGNUM_APPLICATION_ALIAS} IMPORTED_CONFIGURATIONS)
        get_target_property(_MAGNUM_APPLICATION_IMPORTED_LOCATION_RELEASE ${_MAGNUM_APPLICATION_ALIAS} IMPORTED_LOCATION_RELEASE)
        get_target_property(_MAGNUM_APPLICATION_IMPORTED_LOCATION_DEBUG ${_MAGNUM_APPLICATION_ALIAS} IMPORTED_LOCATION_DEBUG)
        set_target_properties(Magnum::Application PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:${_MAGNUM_APPLICATION_ALIAS},INTERFACE_INCLUDE_DIRECTORIES>
            INTERFACE_COMPILE_DEFINITIONS $<TARGET_PROPERTY:${_MAGNUM_APPLICATION_ALIAS},INTERFACE_COMPILE_DEFINITIONS>
            INTERFACE_COMPILE_OPTIONS $<TARGET_PROPERTY:${_MAGNUM_APPLICATION_ALIAS},INTERFACE_COMPILE_OPTIONS>
            INTERFACE_LINK_LIBRARIES $<TARGET_PROPERTY:${_MAGNUM_APPLICATION_ALIAS},INTERFACE_LINK_LIBRARIES>
            IMPORTED_CONFIGURATIONS "${_MAGNUM_APPLICATION_IMPORTED_CONFIGURATIONS}")
        if(_MAGNUM_APPLICATION_IMPORTED_LOCATION_RELEASE)
            set_target_properties(Magnum::Application PROPERTIES
                IMPORTED_LOCATION_RELEASE ${_MAGNUM_APPLICATION_IMPORTED_LOCATION_RELEASE})
        endif()
        if(_MAGNUM_APPLICATION_IMPORTED_LOCATION_DEBUG)
            set_target_properties(Magnum::Application PROPERTIES
                IMPORTED_LOCATION_DEBUG ${_MAGNUM_APPLICATION_IMPORTED_LOCATION_DEBUG})
        endif()
    endif()
    if(MAGNUM_BUILD_DEPRECATED)
        set(MAGNUM_APPLICATION_LIBRARIES Magnum::Application)
    endif()
    # Prevent creating the alias again
    unset(_MAGNUM_APPLICATION_ALIAS)
endif()
if(_MAGNUM_CONTEXT_ALIAS AND NOT TARGET Magnum::Context)
    get_target_property(_MAGNUM_CONTEXT_ALIASED_TARGET ${_MAGNUM_CONTEXT_ALIAS} ALIASED_TARGET)
    if(_MAGNUM_CONTEXT_ALIASED_TARGET)
        add_library(Magnum::Context ALIAS ${_MAGNUM_CONTEXT_ALIASED_TARGET})
    else()
        add_library(Magnum::Context UNKNOWN IMPORTED)
        get_target_property(_MAGNUM_CONTEXT_IMPORTED_CONFIGURATIONS ${_MAGNUM_CONTEXT_ALIAS} IMPORTED_CONFIGURATIONS)
        get_target_property(_MAGNUM_CONTEXT_IMPORTED_LOCATION_RELEASE ${_MAGNUM_CONTEXT_ALIAS} IMPORTED_LOCATION_RELEASE)
        get_target_property(_MAGNUM_CONTEXT_IMPORTED_LOCATION_DEBUG ${_MAGNUM_CONTEXT_ALIAS} IMPORTED_LOCATION_DEBUG)
        set_target_properties(Magnum::Context PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:${_MAGNUM_CONTEXT_ALIAS},INTERFACE_INCLUDE_DIRECTORIES>
            INTERFACE_COMPILE_DEFINITIONS $<TARGET_PROPERTY:${_MAGNUM_CONTEXT_ALIAS},INTERFACE_COMPILE_DEFINITIONS>
            INTERFACE_COMPILE_OPTIONS $<TARGET_PROPERTY:${_MAGNUM_CONTEXT_ALIAS},INTERFACE_COMPILE_OPTIONS>
            INTERFACE_LINK_LIBRARIES $<TARGET_PROPERTY:${_MAGNUM_CONTEXT_ALIAS},INTERFACE_LINK_LIBRARIES>
            IMPORTED_CONFIGURATIONS "${_MAGNUM_CONTEXT_IMPORTED_CONFIGURATIONS}")
        if(_MAGNUM_CONTEXT_IMPORTED_LOCATION_RELEASE)
            set_target_properties(Magnum::Context PROPERTIES
                IMPORTED_LOCATION_RELEASE ${_MAGNUM_CONTEXT_IMPORTED_LOCATION_RELEASE})
        endif()
        if(_MAGNUM_CONTEXT_IMPORTED_LOCATION_DEBUG)
            set_target_properties(Magnum::Context PROPERTIES
                IMPORTED_LOCATION_DEBUG ${_MAGNUM_CONTEXT_IMPORTED_LOCATION_DEBUG})
        endif()
    endif()
    if(MAGNUM_BUILD_DEPRECATED)
        set(MAGNUM_CONTEXT_LIBRARIES Magnum::Context)
    endif()
    # Prevent creating the alias again
    unset(_MAGNUM_CONTEXT_ALIAS)
endif()

# Installation dirs
include(${CORRADE_LIB_SUFFIX_MODULE})
set(MAGNUM_BINARY_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/bin)
set(MAGNUM_LIBRARY_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
set(MAGNUM_DATA_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/share/magnum)
set(MAGNUM_PLUGINS_DEBUG_BINARY_INSTALL_DIR ${MAGNUM_BINARY_INSTALL_DIR}/magnum-d)
set(MAGNUM_PLUGINS_DEBUG_LIBRARY_INSTALL_DIR ${MAGNUM_LIBRARY_INSTALL_DIR}/magnum-d)
set(MAGNUM_PLUGINS_RELEASE_BINARY_INSTALL_DIR ${MAGNUM_BINARY_INSTALL_DIR}/magnum)
set(MAGNUM_PLUGINS_RELEASE_LIBRARY_INSTALL_DIR ${MAGNUM_LIBRARY_INSTALL_DIR}/magnum)
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
set(MAGNUM_PLUGINS_AUDIOIMPORTER_DEBUG_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_BINARY_INSTALL_DIR}/audioimporters)
set(MAGNUM_PLUGINS_AUDIOIMPORTER_DEBUG_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_DEBUG_LIBRARY_INSTALL_DIR}/audioimporters)
set(MAGNUM_PLUGINS_AUDIOIMPORTER_RELEASE_BINARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_BINARY_INSTALL_DIR}/audioimporters)
set(MAGNUM_PLUGINS_AUDIOIMPORTER_RELEASE_LIBRARY_INSTALL_DIR ${MAGNUM_PLUGINS_RELEASE_LIBRARY_INSTALL_DIR}/audioimporters)
set(MAGNUM_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include/Magnum)
set(MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include/MagnumPlugins)

# Get base plugin directory from main library location. This is *not* PATH,
# because CMake always converts the path to an absolute location internally,
# making it impossible to specify relative paths there. Sorry in advance for
# not having the dir selection button in CMake GUI.
set(MAGNUM_PLUGINS_DEBUG_DIR ${_MAGNUM_PLUGINS_DIR_PREFIX}/magnum-d
    CACHE STRING "Base directory where to look for Magnum plugins for debug builds")
set(MAGNUM_PLUGINS_RELEASE_DIR ${_MAGNUM_PLUGINS_DIR_PREFIX}/magnum
    CACHE STRING "Base directory where to look for Magnum plugins for release builds")
set(MAGNUM_PLUGINS_DIR ${_MAGNUM_PLUGINS_DIR_PREFIX}/magnum${_MAGNUM_PLUGINS_DIR_SUFFIX}
    CACHE STRING "Base directory where to look for Magnum plugins")

# Plugin directories
set(MAGNUM_PLUGINS_FONT_DIR ${MAGNUM_PLUGINS_DIR}/fonts)
set(MAGNUM_PLUGINS_FONT_DEBUG_DIR ${MAGNUM_PLUGINS_DEBUG_DIR}/fonts)
set(MAGNUM_PLUGINS_FONT_RELEASE_DIR ${MAGNUM_PLUGINS_RELEASE_DIR}/fonts)
set(MAGNUM_PLUGINS_FONTCONVERTER_DIR ${MAGNUM_PLUGINS_DIR}/fontconverters)
set(MAGNUM_PLUGINS_FONTCONVERTER_DEBUG_DIR ${MAGNUM_PLUGINS_DEBUG_DIR}/fontconverters)
set(MAGNUM_PLUGINS_FONTCONVERTER_RELEASE_DIR ${MAGNUM_PLUGINS_RELEASE_DIR}/fontconverters)
set(MAGNUM_PLUGINS_IMAGECONVERTER_DIR ${MAGNUM_PLUGINS_DIR}/imageconverters)
set(MAGNUM_PLUGINS_IMAGECONVERTER_DEBUG_DIR ${MAGNUM_PLUGINS_DEBUG_DIR}/imageconverters)
set(MAGNUM_PLUGINS_IMAGECONVERTER_RELEASE_DIR ${MAGNUM_PLUGINS_RELEASE_DIR}/imageconverters)
set(MAGNUM_PLUGINS_IMPORTER_DIR ${MAGNUM_PLUGINS_DIR}/importers)
set(MAGNUM_PLUGINS_IMPORTER_DEBUG_DIR ${MAGNUM_PLUGINS_DEBUG_DIR}/importers)
set(MAGNUM_PLUGINS_IMPORTER_RELEASE_DIR ${MAGNUM_PLUGINS_RELEASE_DIR}/importers)
set(MAGNUM_PLUGINS_AUDIOIMPORTER_DIR ${MAGNUM_PLUGINS_DIR}/audioimporters)
set(MAGNUM_PLUGINS_AUDIOIMPORTER_DEBUG_DIR ${MAGNUM_PLUGINS_DEBUG_DIR}/audioimporters)
set(MAGNUM_PLUGINS_AUDIOIMPORTER_RELEASE_DIR ${MAGNUM_PLUGINS_RELEASE_DIR}/audioimporters)

# Deprecated variables
if(MAGNUM_BUILD_DEPRECATED)
    set(MAGNUM_LIBRARIES Magnum::Magnum)
endif()
