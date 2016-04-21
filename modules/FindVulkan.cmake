#.rst:
# Find Vulkan
# ---------
#
# Finds the Vulkan library. This module defines:
#
#  VULKAN_FOUND               - True if VULKAN library is found
#  VULKAN::VULKAN             - VULKAN imported target
#
# Additionally these variables are defined for internal usage:
#
#  VULKAN_LIBRARY             - VULKAN library
#  VULKAN_INCLUDE_DIR         - Root include dir
#

#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
#             Vladimír Vondruš <mosra@centrum.cz>
#   Copyright © 2016 Jonathan Hale <squareys@googlemail.com>
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

if (WIN32)
    find_path(VULKAN_INCLUDE_DIR NAMES vulkan.h HINTS
        "$ENV{VULKAN_SDK}/Include"
        "$ENV{VK_SDK_PATH}/Include"
        PATH_SUFFIXES vulkan)
    if (CMAKE_CL_64)
        find_library(VULKAN_LIBRARY NAMES vulkan-1 HINTS
            "$ENV{VULKAN_SDK}/Bin"
            "$ENV{VK_SDK_PATH}/Bin")
    else()
        find_library(VULKAN_LIBRARY NAMES vulkan-1 HINTS
            "$ENV{VULKAN_SDK}/Bin32"
            "$ENV{VK_SDK_PATH}/Bin32")
    endif()
else()
    find_path(VULKAN_INCLUDE_DIR NAMES vulkan.h HINTS
        "$ENV{VULKAN_SDK}/include" PATH_SUFFIXES vulkan)
    find_library(VULKAN_LIBRARY NAMES vulkan HINTS
        "$ENV{VULKAN_SDK}/lib")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args("VULKAN" DEFAULT_MSG
    VULKAN_LIBRARY
    VULKAN_INCLUDE_DIR)

if(NOT TARGET Vulkan::Vulkan)
    add_library(Vulkan::Vulkan UNKNOWN IMPORTED)

    # Work around BUGGY framework support on OSX
    # https://cmake.org/Bug/view.php?id=14105
    if(CORRADE_TARGET_APPLE AND ${VULKAN_LIBRARY} MATCHES "\\.framework$")
        set_property(TARGET Vulkan::Vulkan PROPERTY IMPORTED_LOCATION ${VULKAN_LIBRARY}/VULKAN)
    else()
        set_property(TARGET Vulkan::Vulkan PROPERTY IMPORTED_LOCATION ${VULKAN_LIBRARY})
    endif()

    set_property(TARGET Vulkan::Vulkan PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES ${VULKAN_INCLUDE_DIR})
endif()


