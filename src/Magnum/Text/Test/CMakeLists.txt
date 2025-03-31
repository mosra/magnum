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

# IDE folder in VS, Xcode etc. CMake 3.12+, older versions have only the FOLDER
# property that would have to be set on each target separately.
set(CMAKE_FOLDER "Magnum/Text/Test")

if(CORRADE_TARGET_EMSCRIPTEN OR CORRADE_TARGET_ANDROID)
    set(TEXT_TEST_DIR ".")
    set(TEXT_TEST_OUTPUT_DIR "./write")
    set(TEXTURETOOLS_DISTANCEFIELDGLTEST_DIR ".")
else()
    set(TEXT_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    set(TEXT_TEST_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
    set(TEXTURETOOLS_DISTANCEFIELDGLTEST_DIR ${PROJECT_SOURCE_DIR}/src/Magnum/TextureTools/Test/DistanceFieldGLTestFiles)
endif()

if(MAGNUM_TARGET_GL AND MAGNUM_BUILD_GL_TESTS)
    if(NOT MAGNUM_BUILD_PLUGINS_STATIC)
        if(MAGNUM_WITH_ANYIMAGEIMPORTER)
            set(ANYIMAGEIMPORTER_PLUGIN_FILENAME $<TARGET_FILE:AnyImageImporter>)
        endif()
        if(MAGNUM_WITH_TGAIMPORTER)
            set(TGAIMPORTER_PLUGIN_FILENAME $<TARGET_FILE:TgaImporter>)
        endif()
    endif()
endif()

# First replace ${} variables, then $<> generator expressions
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configure.h.in)
file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/configure.h
    INPUT ${CMAKE_CURRENT_BINARY_DIR}/configure.h.in)

corrade_add_test(TextAbstractFontTest AbstractFontTest.cpp
    LIBRARIES Magnum MagnumTextTestLib
    FILES data.bin)
target_include_directories(TextAbstractFontTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)

corrade_add_test(TextAbstractFontConverterTest AbstractFontConverterTest.cpp
    LIBRARIES Magnum MagnumTextTestLib
    FILES data.bin)
target_include_directories(TextAbstractFontConverterTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)

corrade_add_test(TextAbstractGlyphCacheTest AbstractGlyphCacheTest.cpp
    LIBRARIES MagnumTextTestLib MagnumDebugTools)
if(CORRADE_TARGET_EMSCRIPTEN)
    if(CMAKE_VERSION VERSION_LESS 3.13)
        message(FATAL_ERROR "CMake 3.13+ is required in order to specify Emscripten linker options")
    endif()
    # Some of the glyph caches tested are rather large
    target_link_options(TextAbstractGlyphCacheTest PRIVATE "SHELL:-s ALLOW_MEMORY_GROWTH=1")
endif()

corrade_add_test(TextAbstractShaperTest AbstractShaperTest.cpp
    LIBRARIES MagnumTextTestLib)

corrade_add_test(TextAlignmentTest AlignmentTest.cpp LIBRARIES MagnumTextTestLib)
corrade_add_test(TextDirectionTest DirectionTest.cpp LIBRARIES MagnumText)
corrade_add_test(TextFeatureTest FeatureTest.cpp LIBRARIES MagnumTextTestLib)
corrade_add_test(TextRendererTest RendererTest.cpp LIBRARIES MagnumTextTestLib)
corrade_add_test(TextScriptTest ScriptTest.cpp LIBRARIES MagnumTextTestLib)

if(MAGNUM_TARGET_GL)
    corrade_add_test(TextGlyphCacheGL_Test GlyphCacheGL_Test.cpp LIBRARIES MagnumText)
    corrade_add_test(TextDistanceFieldGlyphCacheGL_Test DistanceFieldGlyphCacheGL_Test.cpp LIBRARIES MagnumText)
    corrade_add_test(TextRendererGL_Test RendererGL_Test.cpp LIBRARIES MagnumText)

    if(MAGNUM_BUILD_GL_TESTS)
        corrade_add_test(TextDistanceFieldGlyphCacheGLTest DistanceFieldGlyphCacheGLTest.cpp
            LIBRARIES
                MagnumTextTestLib
                MagnumOpenGLTester
                MagnumDebugTools
            FILES
                ${PROJECT_SOURCE_DIR}/src/Magnum/TextureTools/Test/DistanceFieldGLTestFiles/input.tga
                ${PROJECT_SOURCE_DIR}/src/Magnum/TextureTools/Test/DistanceFieldGLTestFiles/output.tga)
        target_include_directories(TextDistanceFieldGlyphCacheGLTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
        if(MAGNUM_BUILD_PLUGINS_STATIC)
            if(MAGNUM_WITH_ANYIMAGEIMPORTER)
                target_link_libraries(TextDistanceFieldGlyphCacheGLTest PRIVATE AnyImageImporter)
            endif()
            if(MAGNUM_WITH_TGAIMPORTER)
                target_link_libraries(TextDistanceFieldGlyphCacheGLTest PRIVATE TgaImporter)
            endif()
        else()
            # So the plugins get properly built when building the test
            if(MAGNUM_WITH_ANYIMAGEIMPORTER)
                add_dependencies(TextDistanceFieldGlyphCacheGLTest AnyImageImporter)
            endif()
            if(MAGNUM_WITH_TGAIMPORTER)
                add_dependencies(TextDistanceFieldGlyphCacheGLTest TgaImporter)
            endif()
        endif()

        corrade_add_test(TextGlyphCacheGLTest GlyphCacheGLTest.cpp
            LIBRARIES
                MagnumTextTestLib
                MagnumOpenGLTester
                MagnumDebugTools)

        corrade_add_test(TextRendererGLTest RendererGLTest.cpp
            LIBRARIES
                MagnumDebugTools
                MagnumShaders
                MagnumTextTestLib
                MagnumOpenGLTester
            FILES
                render.tga
                render-nothing.tga)
        target_include_directories(TextRendererGLTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
        if(MAGNUM_BUILD_PLUGINS_STATIC)
            if(MAGNUM_WITH_ANYIMAGEIMPORTER)
                target_link_libraries(TextRendererGLTest PRIVATE AnyImageImporter)
            endif()
            if(MAGNUM_WITH_TGAIMPORTER)
                target_link_libraries(TextRendererGLTest PRIVATE TgaImporter)
            endif()
        else()
            # So the plugins get properly built when building the test
            if(MAGNUM_WITH_ANYIMAGEIMPORTER)
                add_dependencies(TextRendererGLTest AnyImageImporter)
            endif()
            if(MAGNUM_WITH_TGAIMPORTER)
                add_dependencies(TextRendererGLTest TgaImporter)
            endif()
        endif()
    endif()
endif()
