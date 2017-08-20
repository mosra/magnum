#.rst:
# Find OpenGL ES 2
# ----------------
#
# Finds the OpenGL ES 2 library. This module defines:
#
#  OpenGLES2_FOUND          - True if OpenGL ES 2 library is found
#  OpenGLES2::OpenGLES2     - OpenGL ES 2 imported target
#
# Additionally these variables are defined for internal usage:
#
#  OPENGLES2_LIBRARY        - OpenGL ES 2 library
#  OPENGLES2_INCLUDE_DIR    - Include dir
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

# In Emscripten OpenGL ES 2 is linked automatically, thus no need to find the
# library.
if(NOT CORRADE_TARGET_EMSCRIPTEN)
    find_library(OPENGLES2_LIBRARY NAMES
        GLESv2

        # ANGLE (CMake doesn't search for lib prefix on Windows)
        libGLESv2

        # iOS
        OpenGLES)
    set(OPENGLES2_LIBRARY_NEEDED OPENGLES2_LIBRARY)
endif()

# Include dir
find_path(OPENGLES2_INCLUDE_DIR NAMES
    GLES2/gl2.h

    # iOS
    ES2/gl.h)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenGLES2 DEFAULT_MSG
    ${OPENGLES2_LIBRARY_NEEDED}
    OPENGLES2_INCLUDE_DIR)

if(NOT TARGET OpenGLES2::OpenGLES2)
    if(OPENGLES2_LIBRARY_NEEDED)
        # Work around BUGGY framework support on macOS
        # http://public.kitware.com/pipermail/cmake/2016-April/063179.html
        if(CORRADE_TARGET_APPLE AND ${OPENGLES2_LIBRARY} MATCHES "\\.framework$")
            add_library(OpenGLES2::OpenGLES2 INTERFACE IMPORTED)
            set_property(TARGET OpenGLES2::OpenGLES2 APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES ${OPENGLES2_LIBRARY})
        else()
            add_library(OpenGLES2::OpenGLES2 UNKNOWN IMPORTED)
            set_property(TARGET OpenGLES2::OpenGLES2 PROPERTY
                IMPORTED_LOCATION ${OPENGLES2_LIBRARY})
        endif()
    else()
        # This won't work in CMake 2.8.12, but that affects Emscripten only so
        # I assume people building for that are not on that crap old Ubuntu
        # 14.04 LTS
        add_library(OpenGLES2::OpenGLES2 INTERFACE IMPORTED)
    endif()

    set_property(TARGET OpenGLES2::OpenGLES2 PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES ${OPENGLES2_INCLUDE_DIR})
endif()
