#.rst:
# Find OpenGL ES 3
# ----------------
#
# Finds the OpenGL ES 3 library. This module defines:
#
#  OpenGLES3_FOUND          - True if OpenGL ES 3 library is found
#  OpenGLES3::OpenGLES3     - OpenGL ES 3 imported target
#
# Additionally these variables are defined for internal usage:
#
#  OPENGLES3_LIBRARY        - OpenGL ES 3 library
#
# Please note this find module is tailored especially for the needs of Magnum.
# In particular, it depends on its platform definitions and doesn't look for
# OpenGL ES includes as Magnum has its own, generated using flextGL.
#

#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

# Under Emscripten, GL is linked implicitly. With MINIMAL_RUNTIME you need to
# specify -lGL. Simply set the library name to that.
if(CORRADE_TARGET_EMSCRIPTEN)
    set(OPENGLES3_LIBRARY GL CACHE STRING "Path to a library." FORCE)
else()
    find_library(OPENGLES3_LIBRARY NAMES
        GLESv3

        # On some platforms (e.g. desktop emulation with Mesa or NVidia) ES3
        # support is provided in ES2 lib
        GLESv2

        # ANGLE (CMake doesn't search for lib prefix on Windows)
        libGLESv2

        # iOS
        OpenGLES)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args("OpenGLES3" DEFAULT_MSG
    OPENGLES3_LIBRARY)

if(NOT TARGET OpenGLES3::OpenGLES3)
    # Work around BUGGY framework support on macOS. Do this also in case of
    # Emscripten, since there we don't have a location either.
    # http://public.kitware.com/pipermail/cmake/2016-April/063179.html
    if((CORRADE_TARGET_APPLE AND ${OPENGLES3_LIBRARY} MATCHES "\\.framework$") OR CORRADE_TARGET_EMSCRIPTEN)
        add_library(OpenGLES3::OpenGLES3 INTERFACE IMPORTED)
        set_property(TARGET OpenGLES3::OpenGLES3 APPEND PROPERTY
            INTERFACE_LINK_LIBRARIES ${OPENGLES3_LIBRARY})
    else()
        add_library(OpenGLES3::OpenGLES3 UNKNOWN IMPORTED)
        set_property(TARGET OpenGLES3::OpenGLES3 PROPERTY
            IMPORTED_LOCATION ${OPENGLES3_LIBRARY})
    endif()

    # Emscripten needs a special flag to use WebGL 2. CMake 3.13 allows to set
    # this via INTERFACE_LINK_OPTIONS, for older versions we modify the global
    # CMAKE_EXE_LINKER_FLAGS inside FindMagnum.cmake.
    if(CORRADE_TARGET_EMSCRIPTEN AND NOT CMAKE_VERSION VERSION_LESS 3.13)
        # I could probably use target_link_options() here, but let's be
        # consistent with the rest
        set_property(TARGET OpenGLES3::OpenGLES3 APPEND PROPERTY
            INTERFACE_LINK_OPTIONS "SHELL:-s USE_WEBGL2=1")
    endif()
endif()

mark_as_advanced(OPENGLES3_LIBRARY)
