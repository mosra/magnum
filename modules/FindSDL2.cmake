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
#  SDL2_LIBRARY_DEBUG       - SDL2 debug library, if found
#  SDL2_LIBRARY_RELEASE     - SDL2 release library, if found
#  SDL2_INCLUDE_DIR         - Root include dir
#

#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
#             Vladimír Vondruš <mosra@centrum.cz>
#   Copyright © 2018 Jonathan Hale <squareys@googlemail.com>
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
    set(_SDL2_PATH_SUFFIXES SDL2)
    if(WIN32)
        # Precompiled libraries for MSVC are in x86/x64 subdirectories
        if(MSVC)
            if(CMAKE_SIZEOF_VOID_P EQUAL 8)
                set(_SDL2_LIBRARY_PATH_SUFFIX lib/x64)
            elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
                set(_SDL2_LIBRARY_PATH_SUFFIX lib/x86)
            endif()

        # Both includes and libraries for MinGW are in some directory deep
        # inside. There's also a CMake config file but it has HARDCODED path
        # to /opt/local/i686-w64-mingw32, which doesn't make ANY SENSE,
        # especially on Windows.
        elseif(MINGW)
            if(CMAKE_SIZEOF_VOID_P EQUAL 8)
                set(_SDL2_LIBRARY_PATH_SUFFIX x86_64-w64-mingw32/lib)
                list(APPEND _SDL2_PATH_SUFFIXES x86_64-w64-mingw32/include/SDL2)
            elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
                set(_SDL2_LIBRARY_PATH_SUFFIX i686-w64-mingw32/lib)
                list(APPEND _SDL2_PATH_SUFFIXES i686-w64-mingw32/include/SDL2)
            endif()
        endif()
    endif()

    find_library(SDL2_LIBRARY_RELEASE
        # Compiling SDL2 from scratch on macOS creates dead libSDL2.so symlink
        # which CMake somehow prefers before the SDL2-2.0.dylib file. Making
        # the dylib first so it is preferred. Not sure how this maps to debug
        # config though :/
        NAMES SDL2-2.0 SDL2
        PATH_SUFFIXES ${_SDL2_LIBRARY_PATH_SUFFIX})
    find_library(SDL2_LIBRARY_DEBUG
        NAMES SDL2d
        PATH_SUFFIXES ${_SDL2_LIBRARY_PATH_SUFFIX})
    # FPHSA needs one of the _DEBUG/_RELEASE variables to check that the
    # library was found -- using SDL_LIBRARY, which will get populated by
    # select_library_configurations() below.
    set(SDL2_LIBRARY_NEEDED SDL2_LIBRARY)
endif()

include(SelectLibraryConfigurations)
select_library_configurations(SDL2)

# Include dir
find_path(SDL2_INCLUDE_DIR
    # We must search file which is present only in SDL2 and not in SDL1.
    # Apparently when both SDL.h and SDL_scancode.h are specified, CMake is
    # happy enough that it found SDL.h and doesn't bother about the other.
    #
    # On macOS, where the includes are not in SDL2/SDL.h form (which would
    # solve this issue), but rather SDL2.framework/Headers/SDL.h, CMake might
    # find SDL.framework/Headers/SDL.h if SDL1 is installed, which is wrong.
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

        # Work around BUGGY framework support on macOS
        # https://cmake.org/Bug/view.php?id=14105
        if(CORRADE_TARGET_APPLE AND SDL2_LIBRARY_RELEASE MATCHES "\\.framework$")
            set_property(TARGET SDL2::SDL2 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
            set_property(TARGET SDL2::SDL2 PROPERTY IMPORTED_LOCATION_RELEASE ${SDL2_LIBRARY_RELEASE}/SDL2)
        else()
            if(SDL2_LIBRARY_RELEASE)
                set_property(TARGET SDL2::SDL2 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
                set_property(TARGET SDL2::SDL2 PROPERTY IMPORTED_LOCATION_RELEASE ${SDL2_LIBRARY_RELEASE})
            endif()

            if(SDL2_LIBRARY_DEBUG)
                set_property(TARGET SDL2::SDL2 APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
                set_property(TARGET SDL2::SDL2 PROPERTY IMPORTED_LOCATION_DEBUG ${SDL2_LIBRARY_DEBUG})
            endif()
        endif()

        # Link additional `dl` and `pthread` libraries required by a static
        # build of SDL on Unixy platforms (except Apple, where it is most
        # probably some frameworks instead)
        if(CORRADE_TARGET_UNIX AND NOT CORRADE_TARGET_APPLE AND SDL2_LIBRARY MATCHES "${CMAKE_STATIC_LIBRARY_SUFFIX}$")
            find_package(Threads)
            set_property(TARGET SDL2::SDL2 APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})
        endif()

        # Link frameworks on iOS
        if(CORRADE_TARGET_IOS)
            set_property(TARGET SDL2::SDL2 APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES ${_SDL2_FRAMEWORK_LIBRARIES})
        endif()
    else()
        add_library(SDL2::SDL2 INTERFACE IMPORTED)
    endif()

    set_property(TARGET SDL2::SDL2 PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES ${SDL2_INCLUDE_DIR})
endif()

mark_as_advanced(SDL2_INCLUDE_DIR)
