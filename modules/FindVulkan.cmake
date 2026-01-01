#.rst:
# Find Vulkan
# -----------
#
# Finds the Vulkan library. This module defines:
#
#  Vulkan_FOUND         - True if Vulkan library is found
#  Vulkan::Vulkan       - Vulkan imported target
#
# Additionally these variables are defined for internal usage:
#
#  Vulkan_LIBRARY       - Vulkan library
#
# Please note this find module is tailored especially for the needs of Magnum.
# In particular, it doesn't look for Vulkan includes as Magnum has its own,
# generated using flextGL.
#

#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020, 2021, 2022, 2023, 2024, 2025, 2026
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

# LunarG SDK on Windows. Same as in CMake's vanilla FindVulkan.cmake in 3.18.
if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        find_library(Vulkan_LIBRARY
            NAMES vulkan-1
            HINTS
                "$ENV{VULKAN_SDK}/Lib"
                "$ENV{VULKAN_SDK}/Bin"
        )
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        find_library(Vulkan_LIBRARY
            NAMES vulkan-1
            HINTS
                "$ENV{VULKAN_SDK}/Lib32"
                "$ENV{VULKAN_SDK}/Bin32")
    endif()
endif()

# Other distributions / systems
find_library(Vulkan_LIBRARY
    NAMES
        # Linux
        vulkan

        # MSYS packages have libvulkan.dll
        libvulkan

        # MoltenVK (installed by Homebrew). Specified after (lib)vulkan because
        # the official loader is preferred, however MoltenVK alone works as
        # well: https://www.lunarg.com/benefits-of-the-vulkan-macos-sdk/
        MoltenVK
    PATHS
        "$ENV{VULKAN_SDK}/lib")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Vulkan DEFAULT_MSG Vulkan_LIBRARY)

# libvulkan.so *should* link against pthread on its own, but in some rare
# corner cases (like the 1.2.203 ArchLinux package) it doesn't -- most probably
# due to the vulkan loader wrongly using `-lpthread` instead of `-pthread` and
# then the linker DCEing out the reference to it based on some unfortunate
# combination of compiler/linker flags?
#
# In that case using drivers that rely on pthread (such as SwiftShader) would
# lead to a nasty std::system_error telling me to link to pthreads. As I can't
# have control over how the loader links to pthread or whether at all, and what
# random packaging issues happened in various distributions, I have to link to
# it explicitly to be immune against such issues.
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
set(_Vulkan_DEPENDENCIES Threads::Threads)

# Dependencies for statically-built MoltenVk on MacOS. Can't conditionally do
# this just if `Vulkan_LIBRARY MATCHES "${CMAKE_STATIC_LIBRARY_SUFFIX}$"`
# because the found library could be also just `MoltenVK.xcframework` instead
# of the concrete `MoltenVK.xcframework/macos-arm64_x86_64/libMoltenVK.a` file.
# Support for `*.xcframework` is since CMake 3.28:
#   https://gitlab.kitware.com/cmake/cmake/-/issues/21752
if(CORRADE_TARGET_APPLE AND Vulkan_LIBRARY MATCHES MoltenVK)
    # List from https://github.com/KhronosGroup/MoltenVK/blob/f2d14864d7639b97234e50899e67d91d4c16cc14/Docs/MoltenVK_Runtime_UserGuide.md#optionally-link-to-required-system-libraries
    set(_Vulkan_MoltenVk_FRAMEWORKS Metal Foundation QuartzCore CoreGraphics IOSurface IOKit AppKit)
    if(CORRADE_TARGET_IOS)
        list(APPEND _Vulkan_MoltenVk_FRAMEWORKS UIKit)
    endif()
    foreach(framework ${_Vulkan_MoltenVk_FRAMEWORKS})
        find_library(_Vulkan_MoltenVk_${framework}_LIBRARY ${framework})
        mark_as_advanced(_Vulkan_MoltenVk_${framework}_LIBRARY)
        list(APPEND _Vulkan_DEPENDENCIES ${_Vulkan_MoltenVk_${framework}_LIBRARY})
    endforeach()
endif()

if(NOT TARGET Vulkan::Vulkan)
    add_library(Vulkan::Vulkan UNKNOWN IMPORTED)
    set_target_properties(Vulkan::Vulkan PROPERTIES
        IMPORTED_LOCATION ${Vulkan_LIBRARY}
        INTERFACE_LINK_LIBRARIES "${_Vulkan_DEPENDENCIES}")
endif()

mark_as_advanced(Vulkan_LIBRARY)
