#.rst:
# Find EGL
# --------
#
# Finds the EGL library. This module defines:
#
#  EGL_FOUND            - True if EGL library is found
#  EGL::EGL             - EGL imported target
#
# Additionally these variables are defined for internal usage:
#
#  EGL_LIBRARY          - EGL library
#  EGL_INCLUDE_DIR      - Include dir
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

# Under Emscripten, GL is linked implicitly. With MINIMAL_RUNTIME you need to
# specify -lGL. Simply set the library name to that.
if(CORRADE_TARGET_EMSCRIPTEN)
    set(EGL_LIBRARY GL CACHE STRING "Path to a library." FORCE)
else()
    find_library(EGL_LIBRARY NAMES
        EGL

        # ANGLE (CMake doesn't search for lib prefix on Windows)
        libEGL

        # On iOS a part of OpenGLES
        OpenGLES)
endif()

# Include dir
find_path(EGL_INCLUDE_DIR NAMES
    EGL/egl.h

    # iOS
    EAGL.h)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(EGL DEFAULT_MSG
    EGL_LIBRARY
    EGL_INCLUDE_DIR)

if(NOT TARGET EGL::EGL)
    # Work around BUGGY framework support on macOS. Do this also in case of
    # Emscripten, since there we don't have a location either.
    # http://public.kitware.com/pipermail/cmake/2016-April/063179.html
    if((APPLE AND ${EGL_LIBRARY} MATCHES "\\.framework$") OR CORRADE_TARGET_EMSCRIPTEN)
        add_library(EGL::EGL INTERFACE IMPORTED)
        set_property(TARGET EGL::EGL APPEND PROPERTY
            INTERFACE_LINK_LIBRARIES ${EGL_LIBRARY})
    else()
        add_library(EGL::EGL UNKNOWN IMPORTED)
        set_property(TARGET EGL::EGL PROPERTY
            IMPORTED_LOCATION ${EGL_LIBRARY})
    endif()

    set_target_properties(EGL::EGL PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${EGL_INCLUDE_DIR})
endif()

mark_as_advanced(EGL_LIBRARY EGL_INCLUDE_DIR)
