# - Find OpenGL ES 3
#
# This module defines:
#
#  OPENGLES3_FOUND          - True if OpenGL ES 3 library is found
#  OPENGLES3_LIBRARY        - OpenGL ES 3 library
#  OPENGLES3_INCLUDE_DIR    - Include dir
#

#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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

# In Emscripten OpenGL ES 3 is linked automatically, thus no need to find the
# library.
if(NOT CORRADE_TARGET_EMSCRIPTEN)
    find_library(OPENGLES3_LIBRARY NAMES
        GLESv3

        # On some platforms (e.g. desktop emulation with Mesa or NVidia) ES3
        # support is provided in ES2 lib
        GLESv2

        # ANGLE (CMake doesn't search for lib prefix on Windows)
        libGLESv2

        # iOS
        OpenGLES)
    set(OPENGLES3_LIBRARY_NEEDED OPENGLES3_LIBRARY)
endif()

# Include dir
find_path(OPENGLES3_INCLUDE_DIR NAMES
    GLES3/gl3.h

    # iOS
    ES3/gl.h)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args("OpenGLES3" DEFAULT_MSG
    ${OPENGLES3_LIBRARY_NEEDED}
    OPENGLES3_INCLUDE_DIR)
