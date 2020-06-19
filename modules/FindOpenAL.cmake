#.rst:
# Find OpenAL
# -----------
#
# Finds the OpenAL library. This module defines:
#
#  OpenAL_FOUND         - True if the OpenAL library is found
#  OpenAL::OpenAL       - OpenAL imported target
#
# Additionally these variables are defined for internal usage:
#
#  OPENAL_LIBRARY       - OpenAL library
#  OPENAL_DLL_RELEASE   - OpenAL release DLL on Windows, if found. Note that
#   in case of the binary OpenAL Soft distribution it's named soft_oal.dll and
#   you need to rename it to OpenAL32.dll to make it work.
#  OPENAL_INCLUDE_DIR   - Include dir
#

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

# OpenAL Soft installs cmake package config files which handles dependencies in
# case OpenAL Soft is built statically. Try to find first, quietly, so it
# doesn't print loud messages when it's not found, since that's okay. If the
# OpenAL target already exists, it means we're using it through a CMake
# subproject -- don't attempt to find the package in that case.
#
# In case of Emscripten we don't want any of this -- the library name and
# includes are implicit.
if(NOT CORRADE_TARGET_EMSCRIPTEN AND NOT TARGET OpenAL)
    find_package(OpenAL CONFIG QUIET)
endif()

# If either an OpenAL Soft config file was found or we have a subproject, point
# OpenAL::OpenAL to that and exit -- nothing else to do here.
if(TARGET OpenAL OR TARGET OpenAL::OpenAL)
    # OpenAL Soft config file already defines this one, so this is just for
    # the subproject case.
    if(NOT TARGET OpenAL::OpenAL)
        # Aliases of (global) targets are only supported in CMake 3.11, so we
        # work around it by this. This is easier than fetching all possible
        # properties (which are impossible to track of) and then attempting to
        # rebuild them into a new target.
        add_library(OpenAL::OpenAL INTERFACE IMPORTED)
        set_target_properties(OpenAL::OpenAL PROPERTIES INTERFACE_LINK_LIBRARIES OpenAL)

        # The OpenAL target doesn't define any usable
        # INTERFACE_INCLUDE_DIRECTORIES for some reason (apparently the
        # $<BUILD_INTERFACE:> in there doesn't work or whatever), so let's do
        # that ourselves.
        get_target_property(_OPENAL_SOURCE_DIR OpenAL SOURCE_DIR)
        set_target_properties(OpenAL::OpenAL PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${_OPENAL_SOURCE_DIR}/include/AL)

    # For the imported target get the DLL location
    else()
        if(CORRADE_TARGET_WINDOWS)
            get_target_property(OPENAL_DLL_DEBUG OpenAL::OpenAL   IMPORTED_LOCATION_DEBUG)
            get_target_property(OPENAL_DLL_RELEASE OpenAL::OpenAL IMPORTED_LOCATION_RELEASE)
            # Release not found, fall back to RelWithDebInfo
            if(NOT OPENAL_DLL_RELEASE)
                get_target_property(OPENAL_DLL_RELEASE OpenAL::OpenAL IMPORTED_LOCATION_RELWITHDEBINFO)
            endif()
        endif()
    endif()

    # Just to make FPHSA print some meaningful location, nothing else.
    # Fortunately because of the INTERFACE_INCLUDE_DIRECTORIES workaround above
    # we can have the same handling both in case of an imported target and a
    # CMake subproject.
    get_target_property(_OPENAL_INTERFACE_INCLUDE_DIRECTORIES OpenAL::OpenAL INTERFACE_INCLUDE_DIRECTORIES)

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args("OpenAL" DEFAULT_MSG
        _OPENAL_INTERFACE_INCLUDE_DIRECTORIES)

    return()
endif()

# Under Emscripten, OpenAL is linked implicitly. With MINIMAL_RUNTIME you need
# to specify -lopenal. Simply set the library name to that.
if(CORRADE_TARGET_EMSCRIPTEN)
    set(OPENAL_LIBRARY openal CACHE STRING "Path to a library." FORCE)
else()
    # OpenAL Soft Windows binary distribution puts the library into a subdir,
    # the legacy one from Creative uses the same. OpenAL Soft puts DLLs into
    # bin/Win{32,64}/soft_oal.dll
    if(WIN32)
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(_OPENAL_LIBRARY_PATH_SUFFIX libs/Win64)
            set(_OPENAL_DLL_PATH_SUFFIX bin/Win64)
        elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
            set(_OPENAL_LIBRARY_PATH_SUFFIX libs/Win32)
            set(_OPENAL_DLL_PATH_SUFFIX bin/Win32)
        endif()
    endif()

    find_library(OPENAL_LIBRARY
        # Names same as in CMake's vanilla FindOpenAL
        NAMES OpenAL al openal OpenAL32
        # For binary OpenAL Soft distribution on Windows
        PATH_SUFFIXES ${_OPENAL_LIBRARY_PATH_SUFFIX}
        # The other PATHS from CMake's vanilla FindOpenAL seem to be a legacy
        # cruft, skipping those. The Windows registry used by the vanilla
        # FindOpenAL doesn't seem to be set anymore either.
        )
endif()

# Include dir
find_path(OPENAL_INCLUDE_DIR NAMES al.h
    # AL/ used by OpenAL Soft, OpenAL/ used by the macOS framework. The legacy
    # Creative SDK puts al.h directly into include/, ffs.
    PATH_SUFFIXES AL OpenAL
    # As above, skipping the obsolete PATHS and registry in vanilla FindOpenAL
    )

# OpenAL DLL on Windows
if(CORRADE_TARGET_WINDOWS)
    # TODO: debug?
    find_file(OPENAL_DLL_RELEASE
        NAMES soft_oal.dll
        PATH_SUFFIXES ${_OPENAL_DLL_PATH_SUFFIX})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenAL DEFAULT_MSG
    OPENAL_LIBRARY
    OPENAL_INCLUDE_DIR)

if(NOT TARGET OpenAL::OpenAL)
    # Work around BUGGY framework support on macOS. Do this also in case of
    # Emscripten, since there we don't have a location either.
    # http://public.kitware.com/pipermail/cmake/2016-April/063179.html
    if((APPLE AND ${OPENAL_LIBRARY} MATCHES "\\.framework$") OR CORRADE_TARGET_EMSCRIPTEN)
        add_library(OpenAL::OpenAL INTERFACE IMPORTED)
        set_property(TARGET OpenAL::OpenAL APPEND PROPERTY
            INTERFACE_LINK_LIBRARIES ${OPENAL_LIBRARY})
    else()
        add_library(OpenAL::OpenAL UNKNOWN IMPORTED)
        set_property(TARGET OpenAL::OpenAL PROPERTY
            IMPORTED_LOCATION ${OPENAL_LIBRARY})
    endif()

    set_target_properties(OpenAL::OpenAL PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${OPENAL_INCLUDE_DIR})
endif()

mark_as_advanced(OPENAL_LIBRARY OPENAL_INCLUDE_DIR)
