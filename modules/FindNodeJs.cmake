#.rst:
# Find Node.js
# ------------
#
# Finds the Node.js executable. This module defines:
#
#  NodeJs_FOUND         - True if Node.js executable is found
#  NodeJs::NodeJs       - Node.js executable imported target
#

#
#   This file is part of Corrade.
#
#   Copyright © 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016,
#               2017, 2018, 2019, 2020 Vladimír Vondruš <mosra@centrum.cz>
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

find_program(NODEJS_EXECUTABLE node)
mark_as_advanced(NODEJS_EXECUTABLE)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args("NodeJs" DEFAULT_MSG NODEJS_EXECUTABLE)

if(NOT TARGET NodeJs::NodeJs)
    add_executable(NodeJs::NodeJs IMPORTED)
    set_property(TARGET NodeJs::NodeJs PROPERTY IMPORTED_LOCATION ${NODEJS_EXECUTABLE})
endif()
