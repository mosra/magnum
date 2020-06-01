#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020 Vladimír Vondruš <mosra@centrum.cz>
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

corrade_add_test(DebugToolsFrameProfilerTest FrameProfilerTest.cpp
    LIBRARIES MagnumDebugToolsTestLib)
set_target_properties(DebugToolsFrameProfilerTest PROPERTIES FOLDER "Magnum/DebugTools/Test")

if(WITH_TRADE)
    # Otherwise CMake complains that Corrade::PluginManager is not found, wtf
    find_package(Corrade REQUIRED PluginManager)

    if(CORRADE_TARGET_EMSCRIPTEN OR CORRADE_TARGET_ANDROID)
        set(DEBUGTOOLS_TEST_DIR ".")
        set(SCREENSHOTTEST_SAVE_DIR "write")
        set(COMPAREIMAGETEST_SAVE_DIR "write")
    else()
        set(DEBUGTOOLS_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
        set(SCREENSHOTTEST_SAVE_DIR ${CMAKE_CURRENT_BINARY_DIR}/write)
        set(COMPAREIMAGETEST_SAVE_DIR ${CMAKE_CURRENT_BINARY_DIR}/write)
    endif()

    # CMake before 3.8 has broken $<TARGET_FILE*> expressions for iOS (see
    # https://gitlab.kitware.com/cmake/cmake/merge_requests/404) and since
    # Corrade doesn't support dynamic plugins on iOS, this sorta works around
    # that. Should be revisited when updating Travis to newer Xcode (xcode7.3
    # has CMake 3.6).
    if(NOT BUILD_PLUGINS_STATIC)
        if(WITH_ANYIMAGEIMPORTER)
            set(ANYIMAGEIMPORTER_PLUGIN_FILENAME $<TARGET_FILE:AnyImageImporter>)
        endif()
        if(WITH_ANYIMAGECONVERTER)
            set(ANYIMAGECONVERTER_PLUGIN_FILENAME $<TARGET_FILE:AnyImageConverter>)
        endif()
        if(WITH_TGAIMPORTER)
            set(TGAIMPORTER_PLUGIN_FILENAME $<TARGET_FILE:TgaImporter>)
        endif()
        if(WITH_TGAIMAGECONVERTER)
            set(TGAIMAGECONVERTER_PLUGIN_FILENAME $<TARGET_FILE:TgaImageConverter>)
        endif()
    endif()

    # First replace ${} variables, then $<> generator expressions
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
                   ${CMAKE_CURRENT_BINARY_DIR}/configure.h.in)
    file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/configure.h
        INPUT ${CMAKE_CURRENT_BINARY_DIR}/configure.h.in)

    corrade_add_test(DebugToolsCompareImageTest CompareImageTest.cpp
        LIBRARIES MagnumDebugToolsTestLib
        FILES
            CompareImageActual.tga
            CompareImageExpected.tga
            CompareImageCompressed.dds)
    set_target_properties(DebugToolsCompareImageTest PROPERTIES FOLDER "Magnum/DebugTools/Test")
    target_include_directories(DebugToolsCompareImageTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
    if(BUILD_PLUGINS_STATIC)
        if(WITH_ANYIMAGECONVERTER)
            target_link_libraries(DebugToolsCompareImageTest PRIVATE AnyImageConverter)
        endif()
        if(WITH_ANYIMAGEIMPORTER)
            target_link_libraries(DebugToolsCompareImageTest PRIVATE AnyImageImporter)
        endif()
        if(WITH_TGAIMAGECONVERTER)
            target_link_libraries(DebugToolsCompareImageTest PRIVATE TgaImageConverter)
        endif()
        if(WITH_TGAIMPORTER)
            target_link_libraries(DebugToolsCompareImageTest PRIVATE TgaImporter)
        endif()
    endif()
endif()

if(TARGET_GL)
    if(WITH_SCENEGRAPH)
        corrade_add_test(DebugToolsForceRendererTest ForceRendererTest.cpp LIBRARIES MagnumMathTestLib)
        set_target_properties(DebugToolsForceRendererTest PROPERTIES FOLDER "Magnum/DebugTools/Test")
    endif()

    if(BUILD_GL_TESTS)
        corrade_add_test(DebugToolsFrameProfilerGLTest FrameProfilerGLTest.cpp
            LIBRARIES MagnumDebugTools MagnumOpenGLTester)
        set_target_properties(DebugToolsFrameProfilerTest PROPERTIES FOLDER "Magnum/DebugTools/Test")

        corrade_add_test(DebugToolsTextureImageGLTest TextureImageGLTest.cpp LIBRARIES MagnumDebugTools MagnumOpenGLTester)
        set_target_properties(DebugToolsTextureImageGLTest PROPERTIES FOLDER "Magnum/DebugTools/Test")

        if(NOT MAGNUM_TARGET_WEBGL)
            corrade_add_test(DebugToolsBufferDataGLTest BufferDataGLTest.cpp LIBRARIES MagnumDebugTools MagnumOpenGLTester)

            set_target_properties(DebugToolsBufferDataGLTest PROPERTIES FOLDER "Magnum/DebugTools/Test")
        endif()

        if(WITH_TRADE)
            corrade_add_test(DebugToolsScreenshotGLTest ScreenshotGLTest.cpp LIBRARIES MagnumDebugTools MagnumOpenGLTester)
            set_target_properties(DebugToolsScreenshotGLTest PROPERTIES FOLDER "Magnum/DebugTools/Test")
            # The configure.h file is provided for DebugToolsCompareImageTest
            # already
            target_include_directories(DebugToolsScreenshotGLTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
            if(BUILD_PLUGINS_STATIC)
                if(WITH_ANYIMAGECONVERTER)
                    target_link_libraries(DebugToolsScreenshotGLTest PRIVATE AnyImageConverter)
                endif()
                if(WITH_ANYIMAGEIMPORTER)
                    target_link_libraries(DebugToolsScreenshotGLTest PRIVATE AnyImageImporter)
                endif()
                if(WITH_TGAIMAGECONVERTER)
                    target_link_libraries(DebugToolsScreenshotGLTest PRIVATE TgaImageConverter)
                endif()
                if(WITH_TGAIMPORTER)
                    target_link_libraries(DebugToolsScreenshotGLTest PRIVATE TgaImporter)
                endif()
            endif()

            if(CORRADE_BUILD_STATIC AND NOT BUILD_PLUGINS_STATIC)
                # CMake < 3.4 does this implicitly, but 3.4+ not anymore (see
                # CMP0065). That's generally okay, *except if* the build is
                # static, the executable uses a plugin manager and needs to
                # share globals with the plugins (such as output redirection
                # and so on).
                set_target_properties(DebugToolsScreenshotGLTest PROPERTIES ENABLE_EXPORTS ON)
            endif()
        endif()

        if(WITH_SCENEGRAPH AND WITH_TRADE)
            corrade_add_test(DebugToolsForceRendererGLTest ForceRendererGLTest.cpp
                LIBRARIES MagnumDebugTools MagnumOpenGLTester
                FILES ForceRenderer2D.tga ForceRenderer3D.tga)
            corrade_add_test(DebugToolsObjectRendererGLTest ObjectRendererGLTest.cpp
                LIBRARIES MagnumDebugTools MagnumOpenGLTester
                FILES ObjectRenderer2D.tga ObjectRenderer3D.tga)
            set_target_properties(
                DebugToolsForceRendererGLTest
                DebugToolsObjectRendererGLTest
            PROPERTIES FOLDER "Magnum/DebugTools/Test")
            target_include_directories(DebugToolsForceRendererGLTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
            target_include_directories(DebugToolsObjectRendererGLTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
            if(BUILD_PLUGINS_STATIC)
                if(WITH_ANYIMAGEIMPORTER)
                    target_link_libraries(DebugToolsForceRendererGLTest PRIVATE AnyImageImporter)
                    target_link_libraries(DebugToolsObjectRendererGLTest PRIVATE AnyImageImporter)
                endif()
                if(WITH_TGAIMPORTER)
                    target_link_libraries(DebugToolsForceRendererGLTest PRIVATE TgaImporter)
                    target_link_libraries(DebugToolsObjectRendererGLTest PRIVATE TgaImporter)
                endif()
            endif()
        endif()
    endif()
endif()
