# - Find Magnum
#
# Basic usage:
#  find_package(Magnum [REQUIRED])
# This command tries to find base Magnum library and then defines:
#  MAGNUM_FOUND                 - Whether the base library was found
#  MAGNUM_LIBRARIES             - Magnum library and dependent libraries
#  MAGNUM_INCLUDE_DIRS          - Root include dir and include dirs of
#   dependencies
#  MAGNUM_PLUGINS_DIR           - Base directory with plugins. You can modify
#   it (e.g. set it to `.` when deploying on Windows with plugins stored
#   relatively to the executable), the following MAGNUM_PLUGINS_*_DIR
#   variables depend on it.
#  MAGNUM_PLUGINS_FONT_DIR      - Directory with font plugins
#  MAGNUM_PLUGINS_FONTCONVERTER_DIR - Directory with font converter plugins
#  MAGNUM_PLUGINS_IMAGECONVERTER_DIR - Directory with image converter plugins
#  MAGNUM_PLUGINS_IMPORTER_DIR  - Directory with importer plugins
#  MAGNUM_PLUGINS_AUDIOIMPORTER_DIR - Directory with audio importer plugins
# This command will try to find only the base library, not the optional
# components. The base library depends on Corrade and OpenGL libraries (or
# OpenGL ES libraries). Additional dependencies are specified by the
# components. The optional components are:
#  Audio            - Audio library
#  DebugTools       - DebugTools library (depends on MeshTools, Primitives,
#                     SceneGraph, Shaders and Shapes components)
#  MeshTools        - MeshTools library
#  Primitives       - Primitives library
#  SceneGraph       - SceneGraph library
#  Shaders          - Shaders library
#  Shapes           - Shapes library (depends on SceneGraph component)
#  Text             - Text library (depends on TextureTools component)
#  TextureTools     - TextureTools library
#  GlutApplication  - GLUT application
#  GlxApplication   - GLX application
#  NaClApplication  - NaCl application
#  Sdl2Application  - SDL2 application
#  XEglApplication  - X/EGL application
#  WindowlessNaClApplication - Windowless NaCl application
#  WindowlessGlxApplication - Windowless GLX application
# Example usage with specifying additional components is:
#  find_package(Magnum [REQUIRED|COMPONENTS]
#               MeshTools Primitives GlutApplication)
# For each component is then defined:
#  MAGNUM_*_FOUND   - Whether the component was found
#  MAGNUM_*_LIBRARIES - Component library and dependent libraries
#  MAGNUM_*_INCLUDE_DIRS - Include dirs of module dependencies
# If exactly one *Application or exactly one Windowless*Application
# component is requested and found, its libraries and include dirs are
# available in convenience aliases MAGNUM_APPLICATION_LIBRARIES /
# MAGNUM_WINDOWLESSAPPLICATION_LIBRARIES and MAGNUM_APPLICATION_INCLUDE_DIRS
# / MAGNUM_WINDOWLESSAPPLICATION_INCLUDE_DIRS to simplify porting.
#
# Features of found Magnum library are exposed in these variables:
#  MAGNUM_BUILD_DEPRECATED      - Defined if compiled with deprecated APIs
#   included
#  MAGNUM_BUILD_STATIC          - Defined if compiled as static libraries
#  MAGNUM_TARGET_GLES           - Defined if compiled for OpenGL ES
#  MAGNUM_TARGET_GLES2          - Defined if compiled for OpenGL ES 2.0
#  MAGNUM_TARGET_GLES3          - Defined if compiled for OpenGL ES 3.0
#  MAGNUM_TARGET_DESKTOP_GLES   - Defined if compiled with OpenGL ES
#   emulation on desktop OpenGL
#
# Additionally these variables are defined for internal usage:
#  MAGNUM_INCLUDE_DIR                   - Root include dir (w/o
#   dependencies)
#  MAGNUM_LIBRARY                       - Magnum library (w/o
#   dependencies)
#  MAGNUM_*_LIBRARY                     - Component libraries (w/o
#   dependencies)
#  MAGNUM_LIBRARY_INSTALL_DIR           - Library installation directory
#  MAGNUM_PLUGINS_INSTALL_DIR           - Plugin installation directory
#  MAGNUM_PLUGINS_FONT_INSTALL_DIR      - Font plugin installation
#   directory
#  MAGNUM_PLUGINS_FONTCONVERTER_INSTALL_DIR - Font converter plugin
#   installation directory
#  MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR - Image converter plugin
#   installation directory
#  MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR  - Importer plugin installation
#   directory
#  MAGNUM_PLUGINS_AUDIOIMPORTER_INSTALL_DIR - Audio omporter plugin
#   installation directory
#  MAGNUM_CMAKE_FIND_MODULE_INSTALL_DIR - Installation dir for CMake
#   Find* modules
#  MAGNUM_INCLUDE_INSTALL_DIR           - Header installation directory
#  MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR   - Plugin header installation
#   directory
#

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

# Dependencies
find_package(Corrade REQUIRED)

# Magnum library
find_library(MAGNUM_LIBRARY Magnum)

# Root include dir
find_path(MAGNUM_INCLUDE_DIR
    NAMES Magnum.h
    PATH_SUFFIXES Magnum)

# Configuration
file(READ ${MAGNUM_INCLUDE_DIR}/magnumConfigure.h _magnumConfigure)

string(FIND "${_magnumConfigure}" "#define MAGNUM_BUILD_DEPRECATED" _BUILD_DEPRECATED)
if(NOT _BUILD_DEPRECATED EQUAL -1)
    set(MAGNUM_BUILD_DEPRECATED 1)
endif()
string(FIND "${_magnumConfigure}" "#define MAGNUM_BUILD_STATIC" _BUILD_STATIC)
if(NOT _BUILD_STATIC EQUAL -1)
    set(MAGNUM_BUILD_STATIC 1)
endif()
string(FIND "${_magnumConfigure}" "#define MAGNUM_TARGET_GLES" _TARGET_GLES)
if(NOT _TARGET_GLES EQUAL -1)
    set(MAGNUM_TARGET_GLES 1)
endif()
string(FIND "${_magnumConfigure}" "#define MAGNUM_TARGET_GLES2" _TARGET_GLES2)
if(NOT _TARGET_GLES2 EQUAL -1)
    set(MAGNUM_TARGET_GLES2 1)
endif()
string(FIND "${_magnumConfigure}" "#define MAGNUM_TARGET_GLES3" _TARGET_GLES3)
if(NOT _TARGET_GLES3 EQUAL -1)
    set(MAGNUM_TARGET_GLES3 1)
endif()
string(FIND "${_magnumConfigure}" "#define MAGNUM_TARGET_DESKTOP_GLES" _TARGET_DESKTOP_GLES)
if(NOT _TARGET_DESKTOP_GLES EQUAL -1)
    set(MAGNUM_TARGET_DESKTOP_GLES 1)
endif()

if(NOT MAGNUM_TARGET_GLES OR MAGNUM_TARGET_DESKTOP_GLES)
    find_package(OpenGL REQUIRED)
else()
    find_package(OpenGLES2 REQUIRED)
endif()

# On Windows and in static builds, *Application libraries need to have
# ${MAGNUM_LIBRARIES} listed in dependencies also after all other library names
# to avoid linker errors. Applicaiton libraries are often last thus it is
# +- sufficient to add it there only.
if(WIN32 OR MAGNUM_BUILD_STATIC)
    set(_WINDOWCONTEXT_MAGNUM_LIBRARIES_DEPENDENCY ${MAGNUM_LIBRARIES})
endif()

# Additional components
foreach(component ${Magnum_FIND_COMPONENTS})
    string(TOUPPER ${component} _COMPONENT)

    # Find the library
    find_library(MAGNUM_${_COMPONENT}_LIBRARY Magnum${component})

    set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_SUFFIX ${component})

    # Applications
    if(${component} MATCHES .+Application)
        set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_SUFFIX Platform)
        set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES ${component}.h)

        # GLUT application dependencies
        if(${component} STREQUAL GlutApplication)
            find_package(GLUT)
            if(GLUT_FOUND)
                set(_MAGNUM_${_COMPONENT}_LIBRARIES ${GLUT_LIBRARIES} ${_WINDOWCONTEXT_MAGNUM_LIBRARIES_DEPENDENCY})
            else()
                unset(MAGNUM_${_COMPONENT}_LIBRARY)
            endif()
        endif()

        # SDL2 application dependencies
        if(${component} STREQUAL Sdl2Application)
            find_package(SDL2)
            if(SDL2_FOUND)
                set(_MAGNUM_${_COMPONENT}_LIBRARIES ${SDL2_LIBRARY} ${_WINDOWCONTEXT_MAGNUM_LIBRARIES_DEPENDENCY})
                set(_MAGNUM_${_COMPONENT}_INCLUDE_DIRS ${SDL2_INCLUDE_DIR})
            else()
                unset(MAGNUM_${_COMPONENT}_LIBRARY)
            endif()
        endif()

        # (Windowless) NaCl application dependencies
        if(${component} STREQUAL NaClApplication OR ${component} STREQUAL WindowlessNaClApplication)
            set(_MAGNUM_${_COMPONENT}_LIBRARIES ppapi_cpp ppapi ${_WINDOWCONTEXT_MAGNUM_LIBRARIES_DEPENDENCY})
        endif()

        # GLX application dependencies
        if(${component} STREQUAL GlxApplication OR ${component} STREQUAL WindowlessGlxApplication)
            find_package(X11)
            if(X11_FOUND)
                set(_MAGNUM_${_COMPONENT}_LIBRARIES ${X11_LIBRARIES} ${_WINDOWCONTEXT_MAGNUM_LIBRARIES_DEPENDENCY})
            else()
                unset(MAGNUM_${_COMPONENT}_LIBRARY)
            endif()
        endif()

        # X/EGL application dependencies
        if(${component} STREQUAL XEglApplication)
            find_package(EGL)
            find_package(X11)
            if(EGL_FOUND AND X11_FOUND)
                set(_MAGNUM_${_COMPONENT}_LIBRARIES ${EGL_LIBRARY} ${X11_LIBRARIES} ${_WINDOWCONTEXT_MAGNUM_LIBRARIES_DEPENDENCY})
            else()
                unset(MAGNUM_${_COMPONENT}_LIBRARY)
            endif()
        endif()
    endif()

    # Audio library
    if(${component} STREQUAL Audio)
        set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES Audio.h)

        find_package(OpenAL)
        if(OPENAL_FOUND)
            set(_MAGNUM_${_COMPONENT}_LIBRARIES ${OPENAL_LIBRARY})
            set(_MAGNUM_${_COMPONENT}_INCLUDE_DIRS ${OPENAL_INCLUDE_DIR})
        else()
            unset(MAGNUM_${_COMPONENT}_LIBRARY)
        endif()
    endif()

    # DebugTools library
    if(${component} STREQUAL DebugTools)
        set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES DebugTools.h)
    endif()

    # Mesh tools library
    if(${component} STREQUAL MeshTools)
        set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES CompressIndices.h)
    endif()

    # Primitives library
    if(${component} STREQUAL Primitives)
        set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES Cube.h)
    endif()

    # Scene graph library
    if(${component} STREQUAL SceneGraph)
        set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES SceneGraph.h)
    endif()

    # Shaders library
    if(${component} STREQUAL Shaders)
        set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES Shaders.h)
    endif()

    # Shapes library
    if(${component} STREQUAL Shapes)
        set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES Shapes.h)
    endif()

    # Text library
    if(${component} STREQUAL Text)
        set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES Text.h)
    endif()

    # TextureTools library
    if(${component} STREQUAL TextureTools)
        set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES Atlas.h)
    endif()

    # Try to find the includes
    if(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES)
        find_path(_MAGNUM_${_COMPONENT}_INCLUDE_DIR
            NAMES ${_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES}
            PATHS ${MAGNUM_INCLUDE_DIR}/${_MAGNUM_${_COMPONENT}_INCLUDE_PATH_SUFFIX})
    endif()

    # Decide if the library was found
    if(MAGNUM_${_COMPONENT}_LIBRARY AND _MAGNUM_${_COMPONENT}_INCLUDE_DIR)
        set(MAGNUM_${_COMPONENT}_LIBRARIES ${MAGNUM_${_COMPONENT}_LIBRARY} ${_MAGNUM_${_COMPONENT}_LIBRARIES})
        set(MAGNUM_${_COMPONENT}_INCLUDE_DIRS ${_MAGNUM_${_COMPONENT}_INCLUDE_DIRS})

        set(Magnum_${component}_FOUND TRUE)

        # Don't expose variables w/o dependencies to end users
        mark_as_advanced(FORCE MAGNUM_${_COMPONENT}_LIBRARY _MAGNUM_${_COMPONENT}_INCLUDE_DIR)

        # Global aliases for Windowless*Application and *Application components.
        # If already set, unset them to avoid ambiguity.
        if(${component} MATCHES Windowless.+Application)
            if(NOT DEFINED MAGNUM_WINDOWLESSAPPLICATION_LIBRARIES AND NOT DEFINED MAGNUM_WINDOWLESSAPPLICATION_INCLUDE_DIRS)
                set(MAGNUM_WINDOWLESSAPPLICATION_LIBRARIES ${MAGNUM_${_COMPONENT}_LIBRARIES})
                set(MAGNUM_WINDOWLESSAPPLICATION_INCLUDE_DIRS ${MAGNUM_${_COMPONENT}_INCLUDE_DIRS})
            else()
                unset(MAGNUM_WINDOWLESSAPPLICATION_LIBRARIES)
                unset(MAGNUM_WINDOWLESSAPPLICATION_INCLUDE_DIRS)
            endif()
        elseif(${component} MATCHES .+Application)
            if(NOT DEFINED MAGNUM_APPLICATION_LIBRARIES AND NOT DEFINED MAGNUM_APPLICATION_INCLUDE_DIRS)
                set(MAGNUM_APPLICATION_LIBRARIES ${MAGNUM_${_COMPONENT}_LIBRARIES})
                set(MAGNUM_APPLICATION_INCLUDE_DIRS ${MAGNUM_${_COMPONENT}_INCLUDE_DIRS})
            else()
                unset(MAGNUM_APPLICATION_LIBRARIES)
                unset(MAGNUM_APPLICATION_INCLUDE_DIRS)
            endif()
        endif()
    else()
        set(Magnum_${component}_FOUND FALSE)
    endif()
endforeach()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Magnum
    REQUIRED_VARS MAGNUM_LIBRARY MAGNUM_INCLUDE_DIR
    HANDLE_COMPONENTS)

# Dependent libraries and includes
set(MAGNUM_INCLUDE_DIRS ${MAGNUM_INCLUDE_DIR}
    ${MAGNUM_INCLUDE_DIR}/OpenGL
    ${CORRADE_INCLUDE_DIR})
set(MAGNUM_LIBRARIES ${MAGNUM_LIBRARY}
    ${CORRADE_UTILITY_LIBRARIES}
    ${CORRADE_PLUGINMANAGER_LIBRARIES})
if(NOT MAGNUM_TARGET_GLES OR MAGNUM_TARGET_DESKTOP_GLES)
    set(MAGNUM_LIBRARIES ${MAGNUM_LIBRARIES} ${OPENGL_gl_LIBRARY})
else()
    set(MAGNUM_LIBRARIES ${MAGNUM_LIBRARIES} ${OPENGLES2_LIBRARY})
endif()

# Installation dirs
include(CorradeLibSuffix)
set(MAGNUM_LIBRARY_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
set(MAGNUM_PLUGINS_INSTALL_DIR ${MAGNUM_LIBRARY_INSTALL_DIR}/magnum)
set(MAGNUM_PLUGINS_FONT_INSTALL_DIR ${MAGNUM_PLUGINS_INSTALL_DIR}/fonts)
set(MAGNUM_PLUGINS_FONTCONVERTER_INSTALL_DIR ${MAGNUM_PLUGINS_INSTALL_DIR}/fontconverters)
set(MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR ${MAGNUM_PLUGINS_INSTALL_DIR}/imageconverters)
set(MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR ${MAGNUM_PLUGINS_INSTALL_DIR}/importers)
set(MAGNUM_PLUGINS_AUDIOIMPORTER_INSTALL_DIR ${MAGNUM_PLUGINS_INSTALL_DIR}/audioimporters)
set(MAGNUM_CMAKE_FIND_MODULE_INSTALL_DIR ${CMAKE_ROOT}/Modules)
set(MAGNUM_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include/Magnum)
set(MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include/Magnum/Plugins)
mark_as_advanced(FORCE
    MAGNUM_LIBRARY
    MAGNUM_INCLUDE_DIR
    MAGNUM_LIBRARY_INSTALL_DIR
    MAGNUM_PLUGINS_INSTALL_DIR
    MAGNUM_PLUGINS_FONT_INSTALL_DIR
    MAGNUM_PLUGINS_FONTCONVERTER_INSTALL_DIR
    MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR
    MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR
    MAGNUM_PLUGINS_AUDIOIMPORTER_INSTALL_DIR
    MAGNUM_CMAKE_MODULE_INSTALL_DIR
    MAGNUM_INCLUDE_INSTALL_DIR
    MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR)

set(MAGNUM_PLUGINS_DIR ${MAGNUM_PLUGINS_INSTALL_DIR}
    CACHE PATH "Base directory where to look for Magnum plugins")

# Plugin directories
set(MAGNUM_PLUGINS_FONT_DIR ${MAGNUM_PLUGINS_DIR}/fonts)
set(MAGNUM_PLUGINS_FONTCONVERTER_DIR ${MAGNUM_PLUGINS_DIR}/fontconverters)
set(MAGNUM_PLUGINS_IMAGECONVERTER_DIR ${MAGNUM_PLUGINS_DIR}/imageconverters)
set(MAGNUM_PLUGINS_IMPORTER_DIR ${MAGNUM_PLUGINS_DIR}/importers)
set(MAGNUM_PLUGINS_AUDIOIMPORTER_DIR ${MAGNUM_PLUGINS_DIR}/audioimporters)
