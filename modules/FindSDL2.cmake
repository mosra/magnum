# - Find SDL2
#
# This module defines:
#
#  SDL2_FOUND               - True if SDL2 library is found
#  SDL2_LIBRARY             - SDL2 dynamic library
#  SDL2_INCLUDE_DIR         - Include dir
#

#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014
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

# In Emscripten SDL is linked automatically, thus no need to find the library.
# Also the includes are in SDL subdirectory, not SDL2.
if(CORRADE_TARGET_EMSCRIPTEN)
    set(PATH_SUFFIXES SDL)
else()
    find_library(SDL2_LIBRARY SDL2)
    set(SDL2_LIBRARY_NEEDED SDL2_LIBRARY)
    set(PATH_SUFFIXES SDL2)
endif()

# Include dir
find_path(SDL2_INCLUDE_DIR
    # We must search file which is present only in SDL2 and not in SDL1.
    # Apparently when both SDL.h and SDL_scancode.h are specified, CMake is
    # happy enough that it found SDL.h and doesn't bother about the other.
    #
    # On OSX, where the includes are not in SDL2/SDL.h form (which would solve
    # this issue), but rather SDL2.framework/Headers/SDL.h, CMake might find
    # SDL.framework/Headers/SDL.h if SDL1 is installed, which is wrong.
    NAMES SDL_scancode.h
    PATH_SUFFIXES ${PATH_SUFFIXES}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args("SDL2" DEFAULT_MSG
    ${SDL2_LIBRARY_NEEDED}
    SDL2_INCLUDE_DIR
)
