#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>
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

set(TgaImageConverter_SRCS
    TgaImageConverter.cpp)

set(TgaImageConverter_HEADERS
    TgaImageConverter.h)

add_library(TgaImageConverterObjects OBJECT ${TgaImageConverter_SRCS})
set_target_properties(TgaImageConverterObjects PROPERTIES COMPILE_FLAGS "-DTgaImageConverterObjects_EXPORTS ${CMAKE_SHARED_LIBRARY_CXX_FLAGS}")

add_plugin(TgaImageConverter ${MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR}
    TgaImageConverter.conf
    $<TARGET_OBJECTS:TgaImageConverterObjects>
    pluginRegistrationTgaImageConverter.cpp)
target_link_libraries(TgaImageConverter Magnum)

install(FILES ${TgaImageConverter_HEADERS} DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/TgaImageConverter)

if(BUILD_TESTS)
    add_library(TgaImageConverterTestLib SHARED $<TARGET_OBJECTS:TgaImageConverterObjects>)
    # Otherwise llvm-link complains about multiply defined Debug::globalOutput.
    # TODO: can this be done properly?
    if(NOT CORRADE_TARGET_EMSCRIPTEN)
        target_link_libraries(TgaImageConverterTestLib Magnum)
    endif()
    add_subdirectory(Test)
endif()
