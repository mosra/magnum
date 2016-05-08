#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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

set(MagnumFontConverter_SRCS
    MagnumFontConverter.cpp)

set(MagnumFontConverter_HEADERS
    MagnumFontConverter.h)

# Objects shared between plugin and test library
add_library(MagnumFontConverterObjects OBJECT
    ${MagnumFontConverter_SRCS}
    ${MagnumFontConverter_HEADERS})
target_include_directories(MagnumFontConverterObjects PUBLIC $<TARGET_PROPERTY:Magnum,INTERFACE_INCLUDE_DIRECTORIES>)
if(NOT BUILD_PLUGINS_STATIC OR BUILD_STATIC_PIC)
    set_target_properties(MagnumFontConverterObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# MagnumFontConverter plugin
add_plugin(MagnumFontConverter ${MAGNUM_PLUGINS_FONTCONVERTER_DEBUG_INSTALL_DIR} ${MAGNUM_PLUGINS_FONTCONVERTER_RELEASE_INSTALL_DIR}
    MagnumFontConverter.conf
    $<TARGET_OBJECTS:MagnumFontConverterObjects>
    pluginRegistration.cpp)
if(BUILD_STATIC_PIC)
    set_target_properties(MagnumFontConverter PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

target_link_libraries(MagnumFontConverter Magnum MagnumText)
if(CORRADE_TARGET_WINDOWS)
    target_link_libraries(MagnumFontConverter TgaImageConverter)
endif()

install(FILES ${MagnumFontConverter_HEADERS} DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/MagnumFontConverter)

if(BUILD_GL_TESTS)
    # On Win32 we need to avoid dllimporting TgaImageConverter symbols, because
    # it would search for the symbols in some DLL even when they were linked
    # statically. However it apparently doesn't matter that they were
    # dllexported when building the static library. EH.
    if(WIN32)
        add_library(MagnumMagnumFontConverterTestLib STATIC
            ${MagnumFontConverter_SRCS}
            ${MagnumFontConverter_HEADERS})
        target_compile_definitions(MagnumMagnumFontConverterTestLib PRIVATE "MAGNUM_TGAIMAGECONVERTER_BUILD_STATIC")
    else()
        add_library(MagnumMagnumFontConverterTestLib STATIC $<TARGET_OBJECTS:MagnumFontConverterObjects>)
    endif()
    target_link_libraries(MagnumMagnumFontConverterTestLib Magnum MagnumText MagnumTgaImageConverterTestLib)

    add_subdirectory(Test)
endif()

# Magnum MagnumFontConverter target alias for superprojects
add_library(Magnum::MagnumFontConverter ALIAS MagnumFontConverter)
