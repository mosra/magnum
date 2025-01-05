#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020, 2021, 2022, 2023, 2024, 2025
#             Vladimír Vondruš <mosra@centrum.cz>
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

# IDE folder in VS, Xcode etc. CMake 3.12+, older versions have only the FOLDER
# property that would have to be set on each target separately.
set(CMAKE_FOLDER "Magnum/Platform/Test")

find_package(Corrade REQUIRED Main)

corrade_add_test(PlatformGestureTest GestureTest.cpp LIBRARIES Magnum)

# Icons for SDL/GLFW
if(NOT CORRADE_TARGET_EMSCRIPTEN AND (MAGNUM_WITH_SDL2APPLICATION OR MAGNUM_WITH_GLFWAPPLICATION))
    corrade_add_resource(Platform_RESOURCES resources.conf)
endif()

if(MAGNUM_WITH_ANDROIDAPPLICATION)
    add_library(PlatformAndroidApplicationTest SHARED AndroidApplicationTest.cpp)
    target_link_libraries(PlatformAndroidApplicationTest PRIVATE MagnumAndroidApplication)
    if(CMAKE_ANDROID_NDK)
        android_create_apk(PlatformAndroidApplicationTest AndroidManifest.xml)
    endif()
endif()

if(MAGNUM_WITH_EMSCRIPTENAPPLICATION)
    if(CMAKE_VERSION VERSION_LESS 3.13)
        message(FATAL_ERROR "CMake 3.13+ is required in order to specify Emscripten linker options")
    endif()
    add_executable(PlatformEmscriptenApplicationTest EmscriptenApplicationTest.cpp)
    target_link_libraries(PlatformEmscriptenApplicationTest PRIVATE
        MagnumEmscriptenApplication MagnumGL)
    # Enable memory runtime checks
    target_link_options(PlatformEmscriptenApplicationTest PRIVATE
        "SHELL:-s SAFE_HEAP=1")
    # TODO Emscripten's own event handling generates float-to-int conversion
    # errors with ASSERTIONS=2, using ASSERTIONS=1 until fixed:
    #   https://github.com/emscripten-core/emscripten/issues/19655
    # Those were added in 3.1.38:
    #   https://github.com/emscripten-core/emscripten/pull/19251
    if(EMSCRIPTEN_VERSION VERSION_LESS 3.1.38)
        target_link_options(PlatformEmscriptenApplicationTest PRIVATE "SHELL:-s ASSERTIONS=2")
    else()
        target_link_options(PlatformEmscriptenApplicationTest PRIVATE "SHELL:-s ASSERTIONS=1")
    endif()
    add_custom_command(TARGET PlatformEmscriptenApplicationTest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/../WebApplication.css"
            "${CMAKE_CURRENT_SOURCE_DIR}/../EmscriptenApplication.js"
            $<TARGET_FILE_DIR:PlatformEmscriptenApplicationTest>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/EmscriptenApplicationTest.html"
            "$<TARGET_FILE_DIR:PlatformEmscriptenApplicationTest>/PlatformEmscriptenApplicationTest.html")

    add_executable(PlatformMultipleEmscriptenApplicationTest EmscriptenApplicationTest.cpp)
    target_link_libraries(PlatformMultipleEmscriptenApplicationTest PRIVATE
        MagnumEmscriptenApplication MagnumGL)
    # Enable memory runtime checks
    target_link_options(PlatformMultipleEmscriptenApplicationTest PRIVATE
        "SHELL:-s SAFE_HEAP=1")
    # TODO Emscripten's own event handling generates float-to-int conversion
    # errors with ASSERTIONS=2, using ASSERTIONS=1 until fixed:
    #   https://github.com/emscripten-core/emscripten/issues/19655
    # Those were added in 3.1.38:
    #   https://github.com/emscripten-core/emscripten/pull/19251
    if(EMSCRIPTEN_VERSION VERSION_LESS 3.1.38)
        target_link_options(PlatformMultipleEmscriptenApplicationTest PRIVATE "SHELL:-s ASSERTIONS=2")
    else()
        target_link_options(PlatformMultipleEmscriptenApplicationTest PRIVATE "SHELL:-s ASSERTIONS=1")
    endif()
    target_compile_definitions(PlatformMultipleEmscriptenApplicationTest PRIVATE CUSTOM_CLEAR_COLOR=0x3bd267_rgbf)
    add_custom_command(TARGET PlatformMultipleEmscriptenApplicationTest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/../WebApplication.css"
            "${CMAKE_CURRENT_SOURCE_DIR}/../EmscriptenApplication.js"
            $<TARGET_FILE_DIR:PlatformMultipleEmscriptenApplicationTest>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/MultipleEmscriptenApplicationTest.html"
            "$<TARGET_FILE_DIR:PlatformMultipleEmscriptenApplicationTest>/PlatformMultipleEmscriptenApplicationTest.html")
endif()

if(MAGNUM_WITH_GLFWAPPLICATION)
    add_executable(PlatformGlfwApplicationTest WIN32 GlfwApplicationTest.cpp)
    # HiDPi.manifest not needed, as GLFW sets that on its own
    target_link_libraries(PlatformGlfwApplicationTest PRIVATE MagnumGlfwApplication Corrade::Main)
    # Window icon loading
    if(NOT MAGNUM_WITH_TRADE)
        message(FATAL_ERROR "GlfwApplication tests need the Trade library enabled")
    endif()
    target_sources(PlatformGlfwApplicationTest PRIVATE ${Platform_RESOURCES})
    target_link_libraries(PlatformGlfwApplicationTest PRIVATE MagnumTrade)
    if(CORRADE_TARGET_APPLE)
        # The plist is needed in order to mark the app as DPI-aware
        set_target_properties(PlatformGlfwApplicationTest PROPERTIES
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/GlfwApplicationTest.plist
            MACOSX_BUNDLE ON
            XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "YES")
    endif()
endif()

if(MAGNUM_WITH_GLXAPPLICATION)
    add_executable(PlatformGlxApplicationTest AbstractXApplicationTest.cpp)
    target_compile_definitions(PlatformGlxApplicationTest PRIVATE BUILD_GLXAPPLICATION)
    target_link_libraries(PlatformGlxApplicationTest PRIVATE MagnumGlxApplication)
endif()

if(MAGNUM_WITH_SDL2APPLICATION)
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
    if(NOT MAGNUM_WITH_TRADE)
        message(FATAL_ERROR "GlfwApplication tests need the Trade library enabled")
    endif()
    if(NOT CORRADE_TARGET_EMSCRIPTEN)
        target_sources(PlatformSdl2ApplicationTest PRIVATE ${Platform_RESOURCES})
        target_link_libraries(PlatformSdl2ApplicationTest PRIVATE MagnumTrade)
    endif()
    if(CORRADE_TARGET_EMSCRIPTEN)
        add_custom_command(TARGET PlatformSdl2ApplicationTest POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/../WebApplication.css"
                "${CMAKE_CURRENT_SOURCE_DIR}/../EmscriptenApplication.js"
                $<TARGET_FILE_DIR:PlatformSdl2ApplicationTest>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/Sdl2ApplicationTest.html"
                "$<TARGET_FILE_DIR:PlatformSdl2ApplicationTest>/PlatformSdl2ApplicationTest.html")
    elseif(CORRADE_TARGET_IOS OR CORRADE_TARGET_APPLE)
        # The plist is needed in order to mark the app as DPI-aware
        set_target_properties(PlatformSdl2ApplicationTest PROPERTIES
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Sdl2ApplicationTest.plist
            MACOSX_BUNDLE ON
            XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "YES")
    endif()
endif()

if(MAGNUM_WITH_XEGLAPPLICATION)
    add_executable(PlatformXEglApplicationTest AbstractXApplicationTest.cpp)
    target_compile_definitions(PlatformXEglApplicationTest PRIVATE BUILD_XEGLAPPLICATION)
    target_link_libraries(PlatformXEglApplicationTest PRIVATE MagnumXEglApplication)
endif()

if(MAGNUM_WITH_WINDOWLESSCGLAPPLICATION)
    add_executable(PlatformWindowlessCglApplicationTest WindowlessCglApplicationTest.cpp)
    target_link_libraries(PlatformWindowlessCglApplicationTest PRIVATE MagnumWindowlessCglApplication)
endif()

if(MAGNUM_WITH_WINDOWLESSEGLAPPLICATION)
    add_executable(PlatformWindowlessEglApplicationTest WindowlessEglApplicationTest.cpp)
    target_link_libraries(PlatformWindowlessEglApplicationTest PRIVATE MagnumWindowlessEglApplication)
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

if(MAGNUM_WITH_WINDOWLESSGLXAPPLICATION)
    add_executable(PlatformWindowlessGlxApplicationTest WindowlessGlxApplicationTest.cpp)
    target_link_libraries(PlatformWindowlessGlxApplicationTest PRIVATE MagnumWindowlessGlxApplication)
endif()

if(MAGNUM_WITH_WINDOWLESSIOSAPPLICATION)
    add_executable(PlatformWindowlessIosApplicationTest WindowlessIosApplicationTest.cpp)
    target_link_libraries(PlatformWindowlessIosApplicationTest PRIVATE MagnumWindowlessIosApplication)
    set_target_properties(PlatformWindowlessIosApplicationTest PROPERTIES
        MACOSX_BUNDLE ON
        XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "YES")
endif()

if(MAGNUM_WITH_WINDOWLESSWGLAPPLICATION)
    add_executable(PlatformWindowlessWglApplicationTest WindowlessWglApplicationTest.cpp)
    target_link_libraries(PlatformWindowlessWglApplicationTest PRIVATE MagnumWindowlessWglApplication)
endif()
