#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020, 2021, 2022, 2023, 2024, 2025
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

set(Magnum_MODULES
    FindMagnum.cmake
    MagnumConfig.cmake)

# IMPORTANT: When adding a new module here, be sure to update the
# find_path(_MAGNUM_DEPENDENCY_MODULE_DIR ...) list in FindMagnum.cmake to
# avoid breakages when the directory contains only that new module.
set(Magnum_DEPENDENCY_MODULES )
# TODO update once AndroidApplication uses GL only optionally
if(MAGNUM_WITH_ANDROIDAPPLICATION OR MAGNUM_WITH_WINDOWLESSEGLAPPLICATION OR MAGNUM_WITH_WINDOWLESSIOSAPPLICATION OR MAGNUM_WITH_XEGLAPPLICATION OR MAGNUM_WITH_EGLCONTEXT OR ((MAGNUM_WITH_GLFWAPPLICATION OR MAGNUM_WITH_SDL2APPLICATION) AND MAGNUM_TARGET_GL AND MAGNUM_TARGET_EGL))
    list(APPEND Magnum_DEPENDENCY_MODULES
        FindEGL.cmake)
endif()
if(MAGNUM_WITH_GLFWAPPLICATION)
    list(APPEND Magnum_DEPENDENCY_MODULES
        FindGLFW.cmake)
endif()
if(MAGNUM_WITH_AUDIO)
    list(APPEND Magnum_DEPENDENCY_MODULES
        FindOpenAL.cmake)
endif()
if(MAGNUM_WITH_GL AND MAGNUM_TARGET_GLES)
    if(MAGNUM_TARGET_GLES2)
        list(APPEND Magnum_DEPENDENCY_MODULES
            FindOpenGLES2.cmake)
    else()
        list(APPEND Magnum_DEPENDENCY_MODULES
            FindOpenGLES3.cmake)
    endif()
endif()
if(MAGNUM_WITH_SDL2APPLICATION)
    list(APPEND Magnum_DEPENDENCY_MODULES
        FindSDL2.cmake)
endif()
if(MAGNUM_WITH_VK)
    list(APPEND Magnum_DEPENDENCY_MODULES
        FindVulkan.cmake)
endif()

install(FILES ${Magnum_MODULES} DESTINATION ${MAGNUM_CMAKE_MODULE_INSTALL_DIR})
if(Magnum_DEPENDENCY_MODULES)
    install(FILES ${Magnum_DEPENDENCY_MODULES} DESTINATION ${MAGNUM_CMAKE_MODULE_INSTALL_DIR}/dependencies)
endif()

# Magnum dependency module dir for superprojects
set(_MAGNUM_DEPENDENCY_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")
