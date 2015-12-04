#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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

find_package(OpenAL REQUIRED)

include_directories(${OPENAL_INCLUDE_DIR})

set(WavAudioImporter_SRCS
    WavImporter.cpp)

set(WavAudioImporter_HEADERS
    WavHeader.h
    WavImporter.h)

# Objects shared between plugin and test library
add_library(WavAudioImporterObjects OBJECT
    ${WavAudioImporter_SRCS}
    ${WavAudioImporter_HEADERS})
if(NOT BUILD_PLUGINS_STATIC OR BUILD_STATIC_PIC)
    set_target_properties(WavAudioImporterObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# WavAudioImporter plugin
add_plugin(WavAudioImporter ${MAGNUM_PLUGINS_AUDIOIMPORTER_DEBUG_INSTALL_DIR} ${MAGNUM_PLUGINS_AUDIOIMPORTER_RELEASE_INSTALL_DIR}
    WavAudioImporter.conf
    $<TARGET_OBJECTS:WavAudioImporterObjects>
    pluginRegistration.cpp)
if(BUILD_STATIC_PIC)
    set_target_properties(WavAudioImporter PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

target_link_libraries(WavAudioImporter Magnum MagnumAudio)

install(FILES ${WavAudioImporter_HEADERS} DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/WavAudioImporter)

if(BUILD_TESTS)
    add_library(MagnumWavAudioImporterTestLib STATIC $<TARGET_OBJECTS:WavAudioImporterObjects>)
    target_link_libraries(MagnumWavAudioImporterTestLib Magnum MagnumAudio)
    add_subdirectory(Test)
endif()

# Magnum WavAudioImporter library for superprojects
set(MAGNUM_WAVAUDIOIMPORTER_LIBRARY WavAudioImporter CACHE INTERNAL "")
