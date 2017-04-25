#.rst:
# Find SDL2
# ---------
#
# Finds the SDL2 library. This module defines:
#
#  SDL2_FOUND               - True if SDL2 library is found
#  SDL2::SDL2               - SDL2 imported target
#
# Additionally these variables are defined for internal usage:
#
#  SDL2_LIBRARY             - SDL2 library
#  SDL2_INCLUDE_DIR         - Root include dir
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

# In Emscripten SDL is linked automatically, thus no need to find the library.
# Also the includes are in SDL subdirectory, not SDL2.
if(CORRADE_TARGET_EMSCRIPTEN)
    set(_SDL2_PATH_SUFFIXES SDL)
else()
    # Precompiled libraries for Windows are in x86/x64 subdirectories
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(_SDL_LIBRARY_PATH_SUFFIX lib/x64)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(_SDL_LIBRARY_PATH_SUFFIX lib/x86)
    endif()

    find_library(SDL2_LIBRARY
        # Compiling SDL2 from scratch on OSX creates dead libSDL2.so symlink
        # which CMake somehow prefers before the SDL2-2.0.dylib file. Making
        # the dylib first so it is preferred.
        NAMES SDL2-2.0 SDL2
        PATH_SUFFIXES ${_SDL_LIBRARY_PATH_SUFFIX})
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
        AVFoundation
        CoreGraphics
        CoreMotion
        Foundation
        GameController
        QuartzCore
        UIKit)
    set(_SDL2_FRAMEWORK_LIBRARIES )
    foreach(framework ${_SDL2_FRAMEWORKS})
        find_library(_SDL2_${framework}_LIBRARY ${framework})
        mark_as_advanced(_SDL2_${framework}_LIBRARY)
        list(APPEND _SDL2_FRAMEWORK_LIBRARIES ${_SDL2_${framework}_LIBRARY})
        list(APPEND _SDL2_FRAMEWORK_LIBRARY_NAMES _SDL2_${framework}_LIBRARY)
    endforeach()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args("SDL2" DEFAULT_MSG
    ${SDL2_LIBRARY_NEEDED}
    ${_SDL2_FRAMEWORK_LIBRARY_NAMES}
    SDL2_INCLUDE_DIR)

if(NOT TARGET SDL2::SDL2)
    if(SDL2_LIBRARY_NEEDED)
        add_library(SDL2::SDL2 UNKNOWN IMPORTED)

        # Work around BUGGY framework support on OSX
        # https://cmake.org/Bug/view.php?id=14105
        if(CORRADE_TARGET_APPLE AND ${SDL2_LIBRARY} MATCHES "\\.framework$")
            set_property(TARGET SDL2::SDL2 PROPERTY IMPORTED_LOCATION ${SDL2_LIBRARY}/SDL2)
        else()
            set_property(TARGET SDL2::SDL2 PROPERTY IMPORTED_LOCATION ${SDL2_LIBRARY})
        endif()

        # Link frameworks on iOS
        if(CORRADE_TARGET_IOS)
            set_property(TARGET SDL2::SDL2 APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES ${_SDL2_FRAMEWORK_LIBRARIES})
        endif()

        # Link also EGL library, if on ES (and not on WebGL)
        if(MAGNUM_TARGET_GLES AND NOT MAGNUM_TARGET_DESKTOP_GLES AND NOT MAGNUM_TARGET_WEBGL)
            find_package(EGL REQUIRED)
            set_property(TARGET SDL2::SDL2 APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES EGL::EGL)
        endif()
    else()
        # This won't work in CMake 2.8.12, but that affects Emscripten only so
        # I assume people building for that are not on that crap old Ubuntu
        # 14.04 LTS
        add_library(SDL2::SDL2 INTERFACE IMPORTED)
    endif()

    set_property(TARGET SDL2::SDL2 PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES ${SDL2_INCLUDE_DIR})
endif()
