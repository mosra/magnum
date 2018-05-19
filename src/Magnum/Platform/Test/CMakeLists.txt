#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

if(WITH_ANDROIDAPPLICATION)
    add_library(PlatformAndroidApplicationTest SHARED AndroidApplicationTest.cpp)
    target_link_libraries(PlatformAndroidApplicationTest PRIVATE MagnumAndroidApplication)
endif()

if(WITH_GLFWAPPLICATION)
    add_executable(PlatformGlfwApplicationTest GlfwApplicationTest.cpp)
    target_link_libraries(PlatformGlfwApplicationTest PRIVATE MagnumGlfwApplication)
endif()

if(WITH_GLUTAPPLICATION)
    add_executable(PlatformGlutApplicationTest GlutApplicationTest.cpp)
    target_link_libraries(PlatformGlutApplicationTest PRIVATE MagnumGlutApplication)
endif()

if(WITH_GLXAPPLICATION)
    add_executable(PlatformGlxApplicationTest GlxApplicationTest.cpp)
    target_link_libraries(PlatformGlxApplicationTest PRIVATE MagnumGlxApplication)
endif()

if(WITH_SDL2APPLICATION)
    add_executable(PlatformSdl2ApplicationTest Sdl2ApplicationTest.cpp)
    target_link_libraries(PlatformSdl2ApplicationTest PRIVATE MagnumSdl2Application)
    if(CORRADE_TARGET_IOS)
        set_target_properties(PlatformSdl2ApplicationTest PROPERTIES
            MACOSX_BUNDLE ON
            XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "YES")
    endif()
endif()

if(WITH_XEGLAPPLICATION)
    add_executable(PlatformXEglApplicationTest XEglApplicationTest.cpp)
    target_link_libraries(PlatformXEglApplicationTest PRIVATE MagnumXEglApplication)
endif()

if(WITH_WINDOWLESSCGLAPPLICATION)
    add_executable(PlatformWindowlessCglApplicationTest WindowlessCglApplicationTest.cpp)
    target_link_libraries(PlatformWindowlessCglApplicationTest PRIVATE MagnumWindowlessCglApplication)
endif()

if(WITH_WINDOWLESSEGLAPPLICATION)
    add_executable(PlatformWindowlessEglApplicationTest WindowlessEglApplicationTest.cpp)
    target_link_libraries(PlatformWindowlessEglApplicationTest PRIVATE MagnumWindowlessEglApplication)
endif()

if(WITH_WINDOWLESSGLXAPPLICATION)
    add_executable(PlatformWindowlessGlxApplicationTest WindowlessGlxApplicationTest.cpp)
    target_link_libraries(PlatformWindowlessGlxApplicationTest PRIVATE MagnumWindowlessGlxApplication)
endif()

if(WITH_WINDOWLESSIOSAPPLICATION)
    add_executable(PlatformWindowlessIosApplicationTest WindowlessIosApplicationTest.cpp)
    target_link_libraries(PlatformWindowlessIosApplicationTest PRIVATE MagnumWindowlessIosApplication)
    set_target_properties(PlatformWindowlessIosApplicationTest PROPERTIES
        MACOSX_BUNDLE ON
        XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "YES")
endif()

if(WITH_WINDOWLESSWGLAPPLICATION)
    add_executable(PlatformWindowlessWglApplicationTest WindowlessWglApplicationTest.cpp)
    target_link_libraries(PlatformWindowlessWglApplicationTest PRIVATE MagnumWindowlessWglApplication)
endif()

if(WITH_WINDOWLESSWINDOWSEGLAPPLICATION)
    add_executable(PlatformWindowlessWindowsEglApplicationTest WindowlessWindowsEglApplicationTest.cpp)
    target_link_libraries(PlatformWindowlessWindowsEglApplicationTest PRIVATE MagnumWindowlessWindowsEglApplication)
endif()
