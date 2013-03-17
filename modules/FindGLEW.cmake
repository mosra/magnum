# Find GLEW - OpenGL Extension Wrangler handling module for CMake
#
# This module defines:
#
# GLEW_FOUND                - True if GLEW library is found
# GLEW_LIBRARIES            - GLEW libraries
# GLEW_INCLUDE_DIR          - Include dir
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

# Include dir
find_path(GLEW_INCLUDE_DIR
    NAMES GL/glew.h)

# Library
if(NOT WIN32)
    find_library(GLEW_LIBRARY GLEW)
    set(GLEW_LIBRARIES_ GLEW_LIBRARY)
    mark_as_advanced(GLEW_LIBRARY)
else()
    find_library(GLEW_LIBRARY_DLL glew32)
    find_library(GLEW_LIBRARY_LIB glew32)
    set(GLEW_LIBRARIES_ GLEW_LIBRARY_DLL GLEW_LIBRARY_LIB)
    mark_as_advanced(GLEW_LIBRARY_DLL GLEW_LIBRARY_LIB)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args("GLEW" DEFAULT_MSG
    ${GLEW_LIBRARIES_}
    GLEW_INCLUDE_DIR)

if(NOT GLEW_FOUND)
    return()
endif()

unset(GLEW_LIBRARIES_)

if(NOT WIN32)
    set(GLEW_LIBRARIES ${GLEW_LIBRARY})
    mark_as_advanced(GLEW_LIBRARY)
else()
    set(GLEW_LIBRARIES ${GLEW_LIBRARY_DLL} ${GLEW_LIBRARY_LIB})
    mark_as_advanced(GLEW_LIBRARY_DLL GLEW_LIBRARY_LIB)
endif()
