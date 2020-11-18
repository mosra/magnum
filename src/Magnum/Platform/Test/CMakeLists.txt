#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020 Vladimír Vondruš <mosra@centrum.cz>
#   Copyright © 2020 Pablo Escobar <mail@rvrs.in>
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

find_package(Corrade REQUIRED Main)

# Icons for SDL/GLFW
if(NOT CORRADE_TARGET_EMSCRIPTEN AND (WITH_SDL2APPLICATION OR WITH_GLFWAPPLICATION))
    corrade_add_resource(Platform_RESOURCES resources.conf)
endif()

if(WITH_ANDROIDAPPLICATION)
    add_library(PlatformAndroidApplicationTest SHARED AndroidApplicationTest.cpp)
    target_link_libraries(PlatformAndroidApplicationTest PRIVATE MagnumAndroidApplication)
    set_target_properties(PlatformAndroidApplicationTest PROPERTIES FOLDER "Magnum/Platform/Test")
    if(CMAKE_ANDROID_NDK)
        android_create_apk(PlatformAndroidApplicationTest AndroidManifest.xml)
    endif()
endif()

if(WITH_EMSCRIPTENAPPLICATION)
    add_executable(PlatformEmscriptenApplicationTest EmscriptenApplicationTest.cpp)
    target_link_libraries(PlatformEmscriptenApplicationTest PRIVATE
        MagnumEmscriptenApplication MagnumGL
        # Test that the canvas and keylistener can be found with both the old
        # and the new Emscripten target behaviors
        # TODO: use target_link_options() once we can require CMake 3.13
        "-s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=0"
        # Enable memory runtime checks
        "-s ASSERTIONS=2 -s SAFE_HEAP=1")
    set_target_properties(PlatformEmscriptenApplicationTest PROPERTIES FOLDER "Magnum/Platform/Test")
    add_custom_command(TARGET PlatformEmscriptenApplicationTest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/../WebApplication.css
            ${CMAKE_CURRENT_SOURCE_DIR}/../EmscriptenApplication.js
            $<TARGET_FILE_DIR:PlatformEmscriptenApplicationTest>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/EmscriptenApplicationTest.html
            $<TARGET_FILE_DIR:PlatformEmscriptenApplicationTest>/PlatformEmscriptenApplicationTest.html)

    add_executable(PlatformMultipleEmscriptenApplicationTest EmscriptenApplicationTest.cpp)
    target_link_libraries(PlatformMultipleEmscriptenApplicationTest PRIVATE
        MagnumEmscriptenApplication MagnumGL
        # TODO: use target_link_options() once we can require CMake 3.13
        "-s MODULARIZE -s EXPORT_NAME=createModule"
        "-s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1"
        "-s ASSERTIONS=2 -s SAFE_HEAP=1")
    target_compile_definitions(PlatformMultipleEmscriptenApplicationTest PRIVATE CUSTOM_CLEAR_COLOR=0x3bd267_rgbf)
    set_target_properties(PlatformMultipleEmscriptenApplicationTest PROPERTIES FOLDER "Magnum/Platform/Test")
    add_custom_command(TARGET PlatformMultipleEmscriptenApplicationTest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/../WebApplication.css
            ${CMAKE_CURRENT_SOURCE_DIR}/../EmscriptenApplication.js
            $<TARGET_FILE_DIR:PlatformMultipleEmscriptenApplicationTest>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/MultipleEmscriptenApplicationTest.html
            $<TARGET_FILE_DIR:PlatformMultipleEmscriptenApplicationTest>/PlatformMultipleEmscriptenApplicationTest.html)
endif()

if(WITH_GLFWAPPLICATION)
    add_executable(PlatformGlfwApplicationTest WIN32 GlfwApplicationTest.cpp)
    # HiDPi.manifest not needed, as GLFW sets that on its own
    target_link_libraries(PlatformGlfwApplicationTest PRIVATE MagnumGlfwApplication Corrade::Main)
    # Window icon loading
    target_sources(PlatformGlfwApplicationTest PRIVATE ${Platform_RESOURCES})
    target_link_libraries(PlatformGlfwApplicationTest PRIVATE MagnumTrade)
    set_target_properties(PlatformGlfwApplicationTest PROPERTIES FOLDER "Magnum/Platform/Test")
    if(CORRADE_TARGET_APPLE)
        # The plist is needed in order to mark the app as DPI-aware
        set_target_properties(PlatformGlfwApplicationTest PROPERTIES
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/GlfwApplicationTest.plist
            MACOSX_BUNDLE ON
            XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "YES")
    endif()
endif()

if(WITH_GLXAPPLICATION)
    add_executable(PlatformGlxApplicationTest GlxApplicationTest.cpp)
    target_link_libraries(PlatformGlxApplicationTest PRIVATE MagnumGlxApplication)
    set_target_properties(PlatformGlxApplicationTest PROPERTIES FOLDER "Magnum/Platform/Test")
endif()

if(WITH_SDL2APPLICATION)
    add_executable(PlatformSdl2ApplicationTest WIN32 Sdl2ApplicationTest.cpp)
    if(CORRADE_TARGET_WINDOWS AND NOT CORRADE_TARGET_WINDOWS_RT)
        if(MSVC)
            target_sources(PlatformSdl2ApplicationTest PRIVATE WindowsHiDPI.manifest)
        elseif(MINGW)
            target_sources(PlatformSdl2ApplicationTest PRIVATE WindowsHiDPI.rc)
        endif()
    endif()
    target_link_libraries(PlatformSdl2ApplicationTest PRIVATE MagnumSdl2Application Corrade::Main)
    # Window icon loading
    if(NOT CORRADE_TARGET_EMSCRIPTEN)
        target_sources(PlatformSdl2ApplicationTest PRIVATE ${Platform_RESOURCES})
        target_link_libraries(PlatformSdl2ApplicationTest PRIVATE MagnumTrade)
    endif()
    set_target_properties(PlatformSdl2ApplicationTest PROPERTIES FOLDER "Magnum/Platform/Test")
    if(CORRADE_TARGET_EMSCRIPTEN)
        add_custom_command(TARGET PlatformSdl2ApplicationTest POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_CURRENT_SOURCE_DIR}/../WebApplication.css
                ${CMAKE_CURRENT_SOURCE_DIR}/../EmscriptenApplication.js
                $<TARGET_FILE_DIR:PlatformSdl2ApplicationTest>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_CURRENT_SOURCE_DIR}/Sdl2ApplicationTest.html
                $<TARGET_FILE_DIR:PlatformSdl2ApplicationTest>/PlatformSdl2ApplicationTest.html)
    elseif(CORRADE_TARGET_IOS OR CORRADE_TARGET_APPLE)
        # The plist is needed in order to mark the app as DPI-aware
        set_target_properties(PlatformSdl2ApplicationTest PROPERTIES
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Sdl2ApplicationTest.plist
            MACOSX_BUNDLE ON
            XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "YES")
    endif()
endif()

if(WITH_XEGLAPPLICATION)
    add_executable(PlatformXEglApplicationTest XEglApplicationTest.cpp)
    target_link_libraries(PlatformXEglApplicationTest PRIVATE MagnumXEglApplication)
    set_target_properties(PlatformXEglApplicationTest PROPERTIES FOLDER "Magnum/Platform/Test")
endif()

if(WITH_WINDOWLESSCGLAPPLICATION)
    add_executable(PlatformWindowlessCglApplicationTest WindowlessCglApplicationTest.cpp)
    target_link_libraries(PlatformWindowlessCglApplicationTest PRIVATE MagnumWindowlessCglApplication)
    set_target_properties(PlatformWindowlessCglApplicationTest PROPERTIES FOLDER "Magnum/Platform/Test")
endif()

if(WITH_WINDOWLESSEGLAPPLICATION)
    add_executable(PlatformWindowlessEglApplicationTest WindowlessEglApplicationTest.cpp)
    target_link_libraries(PlatformWindowlessEglApplicationTest PRIVATE MagnumWindowlessEglApplication)
    set_target_properties(PlatformWindowlessEglApplicationTest PROPERTIES FOLDER "Magnum/Platform/Test")
    if(CORRADE_TARGET_EMSCRIPTEN)
        add_custom_command(TARGET PlatformWindowlessEglApplicationTest POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_CURRENT_SOURCE_DIR}/../WebApplication.css
                ${CMAKE_CURRENT_SOURCE_DIR}/../WindowlessEmscriptenApplication.js
                $<TARGET_FILE_DIR:PlatformWindowlessEglApplicationTest>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_CURRENT_SOURCE_DIR}/WindowlessEglApplicationTest.html
                $<TARGET_FILE_DIR:PlatformWindowlessEglApplicationTest>/PlatformWindowlessEglApplicationTest.html)
    endif()
endif()

if(WITH_WINDOWLESSGLXAPPLICATION)
    add_executable(PlatformWindowlessGlxApplicationTest WindowlessGlxApplicationTest.cpp)
    target_link_libraries(PlatformWindowlessGlxApplicationTest PRIVATE MagnumWindowlessGlxApplication)
    set_target_properties(PlatformWindowlessGlxApplicationTest PROPERTIES FOLDER "Magnum/Platform/Test")
endif()

if(WITH_WINDOWLESSIOSAPPLICATION)
    add_executable(PlatformWindowlessIosApplicationTest WindowlessIosApplicationTest.cpp)
    target_link_libraries(PlatformWindowlessIosApplicationTest PRIVATE MagnumWindowlessIosApplication)
    set_target_properties(PlatformWindowlessIosApplicationTest PROPERTIES
        MACOSX_BUNDLE ON
        XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "YES"
        FOLDER "Magnum/Platform/Test")
endif()

if(WITH_WINDOWLESSWGLAPPLICATION)
    add_executable(PlatformWindowlessWglApplicationTest WindowlessWglApplicationTest.cpp)
    target_link_libraries(PlatformWindowlessWglApplicationTest PRIVATE MagnumWindowlessWglApplication)
    set_target_properties(PlatformWindowlessWglApplicationTest PROPERTIES FOLDER "Magnum/Platform/Test")
endif()

if(WITH_WINDOWLESSWINDOWSEGLAPPLICATION)
    add_executable(PlatformWindowlessWindowsEglApplicationTest WindowlessWindowsEglApplicationTest.cpp)
    target_link_libraries(PlatformWindowlessWindowsEglApplicationTest PRIVATE MagnumWindowlessWindowsEglApplication)
    set_target_properties(PlatformWindowlessWindowsEglApplicationTest PROPERTIES FOLDER "Magnum/Platform/Test")
endif()
