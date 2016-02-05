# - Find SDL2
#
# This module defines:
#
#  SDL2_FOUND               - True if SDL2 library is found
#  SDL2_LIBRARIES           - SDL2 library and dependent libraries
#  SDL2_INCLUDE_DIRS        - Root include dir and include dirs of dependencies
#
# Additionally these variables are defined for internal usage:
#  SDL2_INCLUDE_DIR         - Root include dir (w/o dependencies)
#  SDL2_LIBRARY             - SDL2 library (w/o dependencies)
#

#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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
    set(_SDL2_PATH_SUFFIXES SDL)
else()
    find_library(SDL2_LIBRARY
        # Compiling SDL2 from scratch on OSX creates dead libSDL2.so symlink
        # which CMake somehow prefers before the SDL2-2.0.dylib file. Making
        # the dylib first so it is preferred.
        NAMES SDL2-2.0 SDL2

        # Precompiled libraries for Windows are in x86/x64 subdirectories
        PATH_SUFFIXES lib/x86 lib/x64)
    set(SDL2_LIBRARY_NEEDED SDL2_LIBRARY)
    set(_SDL2_PATH_SUFFIXES SDL2)
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
    PATH_SUFFIXES ${_SDL2_PATH_SUFFIXES})

# iOS dependencies
if(CORRADE_TARGET_IOS)
    set(_SDL2_FRAMEWORKS
        AudioToolbox
        CoreGraphics
        CoreMotion
        Foundation
        GameController
        QuartzCore
        UIKit)
    set(_SDL2_FRAMEWORK_LIBRARIES )
    foreach(framework ${_SDL2_FRAMEWORKS})
        find_library(_SDL2_${framework}_LIBRARY ${framework})
        list(APPEND _SDL2_FRAMEWORK_LIBRARIES ${_SDL2_${framework}_LIBRARY})
        list(APPEND _SDL2_FRAMEWORK_LIBRARY_NAMES _SDL2_${framework}_LIBRARY)
    endforeach()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args("SDL2" DEFAULT_MSG
    ${SDL2_LIBRARY_NEEDED}
    ${_SDL2_FRAMEWORK_LIBRARY_NAMES}
    SDL2_INCLUDE_DIR)

set(SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIR})
set(SDL2_LIBRARIES ${SDL2_LIBRARY} ${_SDL2_FRAMEWORK_LIBRARIES})
