# - Find OpenGL ES 2
#
# This module defines:
#
#  OPENGLES2_FOUND          - True if OpenGL ES 2 library is found
#  OPENGLES2_LIBRARY        - OpenGL ES 2 library
#  OPENGLES2_INCLUDE_DIR    - Include dir
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

# In Emscripten OpenGL ES 2 is linked automatically, thus no need to find the
# library.
if(NOT CORRADE_TARGET_EMSCRIPTEN)
    find_library(OPENGLES2_LIBRARY NAMES
        GLESv2
        ppapi_gles2) # NaCl
    set(OPENGLES2_LIBRARY_NEEDED OPENGLES2_LIBRARY)
endif()

# Include dir
find_path(OPENGLES2_INCLUDE_DIR
    NAMES GLES2/gl2.h)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args("OpenGLES2" DEFAULT_MSG
    ${OPENGLES2_LIBRARY_NEEDED}
    OPENGLES2_INCLUDE_DIR)
