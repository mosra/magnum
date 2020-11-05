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

if(NOT TARGET_GL)
    # TODO: remove (and update all other docs) when we have better
    # createGlyphCache() APIs
    message(SEND_ERROR "MagnumFont is available only if TARGET_GL is enabled")
endif()

find_package(Corrade REQUIRED PluginManager)

if(BUILD_PLUGINS_STATIC)
    set(MAGNUM_MAGNUMFONT_BUILD_STATIC 1)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

# MagnumFont plugin
add_plugin(MagnumFont
    "${MAGNUM_PLUGINS_FONT_DEBUG_BINARY_INSTALL_DIR};${MAGNUM_PLUGINS_FONT_DEBUG_LIBRARY_INSTALL_DIR}"
    "${MAGNUM_PLUGINS_FONT_RELEASE_BINARY_INSTALL_DIR};${MAGNUM_PLUGINS_FONT_RELEASE_LIBRARY_INSTALL_DIR}"
    MagnumFont.conf
    MagnumFont.cpp
    MagnumFont.h)
if(MAGNUM_MAGNUMFONT_BUILD_STATIC AND BUILD_STATIC_PIC)
    set_target_properties(MagnumFont PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(MagnumFont PUBLIC Magnum MagnumText MagnumTrade)
if(CORRADE_TARGET_WINDOWS)
    target_link_libraries(MagnumFont PUBLIC TgaImporter)
endif()
# Modify output location only if all are set, otherwise it makes no sense
if(CMAKE_RUNTIME_OUTPUT_DIRECTORY AND CMAKE_LIBRARY_OUTPUT_DIRECTORY AND CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set_target_properties(MagnumFont PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/magnum$<$<CONFIG:Debug>:-d>/fonts
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/magnum$<$<CONFIG:Debug>:-d>/fonts
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/magnum$<$<CONFIG:Debug>:-d>/fonts)
endif()

install(FILES MagnumFont.h ${CMAKE_CURRENT_BINARY_DIR}/configure.h
    DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/MagnumFont)

# Automatic static plugin import
if(MAGNUM_MAGNUMFONT_BUILD_STATIC)
    install(FILES importStaticPlugin.cpp DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/MagnumFont)
    target_sources(MagnumFont INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/importStaticPlugin.cpp)
endif()

if(BUILD_TESTS)
    add_subdirectory(Test)
endif()

# Magnum MagnumFont target alias for superprojects
add_library(Magnum::MagnumFont ALIAS MagnumFont)
