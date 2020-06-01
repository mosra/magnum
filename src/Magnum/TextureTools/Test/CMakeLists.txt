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

corrade_add_test(TextureToolsAtlasTest AtlasTest.cpp LIBRARIES MagnumTextureTools)
set_target_properties(TextureToolsAtlasTest PROPERTIES FOLDER "Magnum/TextureTools/Test")

if(CORRADE_TARGET_EMSCRIPTEN OR CORRADE_TARGET_ANDROID)
    set(DISTANCEFIELDGLTEST_FILES_DIR "DistanceFieldGLTestFiles")
else()
    set(DISTANCEFIELDGLTEST_FILES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/DistanceFieldGLTestFiles)
endif()

if(BUILD_GL_TESTS)
    # Otherwise CMake complains that Corrade::PluginManager is not found, wtf
    find_package(Corrade REQUIRED PluginManager)

    # CMake before 3.8 has broken $<TARGET_FILE*> expressions for iOS (see
    # https://gitlab.kitware.com/cmake/cmake/merge_requests/404) and since
    # Corrade doesn't support dynamic plugins on iOS, this sorta works around
    # that. Should be revisited when updating Travis to newer Xcode (xcode7.3
    # has CMake 3.6).
    if(NOT BUILD_PLUGINS_STATIC)
        if(WITH_ANYIMAGEIMPORTER)
            set(ANYIMAGEIMPORTER_PLUGIN_FILENAME $<TARGET_FILE:AnyImageImporter>)
        endif()
        if(WITH_TGAIMPORTER)
            set(TGAIMPORTER_PLUGIN_FILENAME $<TARGET_FILE:TgaImporter>)
        endif()
    endif()

    # First replace ${} variables, then $<> generator expressions
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
                    ${CMAKE_CURRENT_BINARY_DIR}/configure.h.in)
    file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/configure.h
        INPUT ${CMAKE_CURRENT_BINARY_DIR}/configure.h.in)

    set(TextureToolsDistanceFieldGLTest_SRCS DistanceFieldGLTest.cpp)
    if(CORRADE_TARGET_IOS)
        # TODO: do this in a generic way in corrade_add_test()
        set_source_files_properties(DistanceFieldGLTestFiles PROPERTIES
            MACOSX_PACKAGE_LOCATION Resources)
        list(APPEND TextureToolsDistanceFieldGLTest_SRCS DistanceFieldGLTestFiles)
    endif()
    corrade_add_test(TextureToolsDistanceFieldGLTest ${TextureToolsDistanceFieldGLTest_SRCS}
        LIBRARIES MagnumTextureTools MagnumGL MagnumTrade MagnumDebugTools MagnumOpenGLTester
        FILES
            DistanceFieldGLTestFiles/input.tga
            DistanceFieldGLTestFiles/output.tga)
    set_target_properties(TextureToolsDistanceFieldGLTest PROPERTIES FOLDER "Magnum/TextureTools/Test")
    target_include_directories(TextureToolsDistanceFieldGLTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
    if(BUILD_PLUGINS_STATIC)
        if(WITH_ANYIMAGEIMPORTER)
            target_link_libraries(TextureToolsDistanceFieldGLTest PRIVATE AnyImageImporter)
        endif()
        if(WITH_TGAIMPORTER)
            target_link_libraries(TextureToolsDistanceFieldGLTest PRIVATE TgaImporter)
        endif()
    endif()
endif()
