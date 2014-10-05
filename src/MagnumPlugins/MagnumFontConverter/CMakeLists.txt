#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014
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

set(MagnumFontConverter_SOURCES
    MagnumFontConverter.cpp)

set(MagnumFontConverter_HEADERS
    MagnumFontConverter.h)

add_library(MagnumFontConverterObjects OBJECT ${MagnumFontConverter_SOURCES})
set_target_properties(MagnumFontConverterObjects PROPERTIES COMPILE_FLAGS "${CMAKE_SHARED_LIBRARY_CXX_FLAGS}")

add_plugin(MagnumFontConverter ${MAGNUM_PLUGINS_FONTCONVERTER_DEBUG_INSTALL_DIR} ${MAGNUM_PLUGINS_FONTCONVERTER_RELEASE_INSTALL_DIR}
    MagnumFontConverter.conf
    $<TARGET_OBJECTS:MagnumFontConverterObjects>
    pluginRegistration.cpp)
target_link_libraries(MagnumFontConverter Magnum MagnumText)

if(CORRADE_TARGET_WINDOWS)
    target_link_libraries(MagnumFontConverter TgaImageConverter)
endif()

install(FILES ${MagnumFontConverter_HEADERS} DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/MagnumFontConverter)

if(BUILD_GL_TESTS)
    add_library(MagnumMagnumFontConverterTestLib STATIC $<TARGET_OBJECTS:MagnumFontConverterObjects>)
    target_link_libraries(MagnumMagnumFontConverterTestLib Magnum MagnumText MagnumTgaImageConverterTestLib)
    add_subdirectory(Test)
endif()
