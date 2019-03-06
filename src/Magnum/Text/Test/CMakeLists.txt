#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

if(CORRADE_TARGET_EMSCRIPTEN OR CORRADE_TARGET_ANDROID)
    set(TEXT_TEST_DIR ".")
    set(TEXT_TEST_OUTPUT_DIR "./write")
else()
    set(TEXT_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    set(TEXT_TEST_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

corrade_add_test(TextAbstractFontTest AbstractFontTest.cpp
    LIBRARIES Magnum MagnumTextTestLib
    FILES data.bin)
target_include_directories(TextAbstractFontTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
corrade_add_test(TextAbstractFontConverterTest AbstractFontConverterTest.cpp
    LIBRARIES Magnum MagnumText
    FILES data.bin)
target_include_directories(TextAbstractFontConverterTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
corrade_add_test(TextAbstractGlyphCacheTest AbstractGlyphCacheTest.cpp LIBRARIES MagnumTextTestLib)
corrade_add_test(TextAbstractLayouterTest AbstractLayouterTest.cpp LIBRARIES Magnum MagnumText)

set_target_properties(
    TextAbstractFontTest
    TextAbstractFontConverterTest
    TextAbstractGlyphCacheTest
    TextAbstractLayouterTest
    PROPERTIES FOLDER "Magnum/Text/Test")

if(TARGET_GL AND BUILD_GL_TESTS)
    corrade_add_test(TextDistanceFieldGlyphCacheGLTest DistanceFieldGlyphCacheGLTest.cpp LIBRARIES MagnumText MagnumOpenGLTester)
    corrade_add_test(TextGlyphCacheGLTest GlyphCacheGLTest.cpp LIBRARIES MagnumText MagnumOpenGLTester)
    corrade_add_test(TextRendererGLTest RendererGLTest.cpp LIBRARIES MagnumText MagnumOpenGLTester)

    set_target_properties(
        TextDistanceFieldGlyphCacheGLTest
        TextGlyphCacheGLTest
        TextRendererGLTest
        PROPERTIES FOLDER "Magnum/Text/Test")
endif()
