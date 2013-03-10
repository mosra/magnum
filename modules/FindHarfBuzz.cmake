# - Find HarfBuzz
#
# This module tries to find HarfBuzz library and then defines:
#  HARFBUZZ_FOUND          - True if HarfBuzz library is found
#  HARFBUZZ_INCLUDE_DIRS   - Include dirs
#  HARFBUZZ_LIBRARIES      - HarfBuzz libraries
#
# Additionally these variables are defined for internal usage:
#  HARFBUZZ_INCLUDE_DIR    - Include dir (w/o dependencies)
#  HARFBUZZ_LIBRARY        - HarfBuzz library (w/o dependencies)
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

# Library
find_library(HARFBUZZ_LIBRARY NAMES harfbuzz)

# Include dir
find_path(HARFBUZZ_INCLUDE_DIR
    NAMES hb.h
    PATH_SUFFIXES harfbuzz
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args("HarfBuzz" DEFAULT_MSG
    HARFBUZZ_LIBRARY
    HARFBUZZ_INCLUDE_DIR
)

set(HARFBUZZ_INCLUDE_DIRS ${HARFBUZZ_INCLUDE_DIR})
set(HARFBUZZ_LIBRARIES ${HARFBUZZ_LIBRARY})

mark_as_advanced(FORCE
    HARFBUZZ_LIBRARY
    HARFBUZZ_INCLUDE_DIR)
