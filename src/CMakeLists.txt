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

# Disable `-pedantic` for GCC 4.6, as the compiler doesn't like
# list-initialization of array of classes (RectangularMatrix constructors). It
# is perfectly legal C++11 and both GCC 4.7 and Clang accept it without notice.
# Disable it also for GCC <=4.5, because otherwise the compiler output is
# ridden with "warning: comma at the end of enumerator list", which is also
# legal C++11.
if(CORRADE_GCC46_COMPATIBILITY)
    string(REPLACE "-pedantic" "" CORRADE_CXX_FLAGS "${CORRADE_CXX_FLAGS}")
endif()

# On 4.8.2 strict aliasing causes failure of DebugToolsCylinderRendererTest
# without any warning, only in release build, any attempt to add debug print
# results in issue disappearing. Not an issue on Clang or GCC < 4.8.
#
# Disable it also for GCC 4.4, as it breaks non-const Matrix[34]::up()
# etc. accessors (and I have no better solution for that yet)
if(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND NOT "${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS "4.8.0") OR CORRADE_GCC44_COMPATIBILITY)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing")

# For GCC 4.5 disable just the strict-aliasing warnings. They are mostly
# harmless and are just polluting the output. In case they aren't harmless
# anymore the strict-aliasing should be disabled here too.
elseif(CORRADE_GCC45_COMPATIBILITY AND NOT CORRADE_GCC44_COMPATIBILITY)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-strict-aliasing")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CORRADE_CXX_FLAGS}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CORRADE_INCLUDE_DIR})

# We can use both implicit include path (GLES2/gl2.h) where our headers can
# be overriden with system ones or explicit (MagnumExternal/OpenGL/GLES2/gl2ext.h)
# where only our headers will be used
include_directories(${CMAKE_SOURCE_DIR}/src/MagnumExternal/OpenGL)

add_subdirectory(MagnumExternal)
add_subdirectory(Magnum)
add_subdirectory(MagnumPlugins)
