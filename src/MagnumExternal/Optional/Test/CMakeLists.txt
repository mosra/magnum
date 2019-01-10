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

corrade_add_test(StdOptionalTest StdOptionalTest.cpp)
target_include_directories(StdOptionalTest PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}/src)
set_target_properties(StdOptionalTest PROPERTIES FOLDER "MagnumExternal/Optional/Test")

# Try using C++17 on newer compilers. Can't use VERSION_GREATER_EQUAL because
# older CMake doesn't support these. No idea how to detect libc++ presence on
# non-Apple platforms (or even version), so I'm giving up there, testing just
# elsewhere.
#
# Compiler versions where std::optional is supported:
# MSVC 2017 (trust me, I know)
# GCC libstdc++ 7.1 (see https://gcc.gnu.org/onlinedocs/libstdc++/manual/status.html#table.cxx17_status)
# libc++ 4.0 (see https://launchpad.net/ubuntu/+source/libc++)
# Apple Clang 9.0 correspons to LLVM 4.0 (see https://gist.github.com/yamaya/2924292#gistcomment-2289472)
if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "7.1") OR
  #(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.0") OR
   (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0") OR
   (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "19.10"))
    set_target_properties(StdOptionalTest PROPERTIES CORRADE_CXX_STANDARD 17)
endif()
