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

find_package(Magnum REQUIRED Audio)

include_directories(${MAGNUM_AUDIO_INCLUDE_DIRS})

set(AnyAudioImporter_SRCS
    AnyImporter.cpp)

set(AnyAudioImporter_HEADERS
    AnyImporter.h)

# Objects shared between plugin and test library
add_library(AnyAudioImporterObjects OBJECT
    ${AnyAudioImporter_SRCS}
    ${AnyAudioImporter_HEADERS})
if(NOT BUILD_STATIC OR BUILD_STATIC_PIC)
    set_target_properties(AnyAudioImporterObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# AnyAudioImporter plugin
add_plugin(AnyAudioImporter ${MAGNUM_PLUGINS_AUDIOIMPORTER_DEBUG_INSTALL_DIR} ${MAGNUM_PLUGINS_AUDIOIMPORTER_RELEASE_INSTALL_DIR}
    AnyAudioImporter.conf
    $<TARGET_OBJECTS:AnyAudioImporterObjects>
    pluginRegistration.cpp)
if(BUILD_STATIC_PIC)
    set_target_properties(AnyAudioImporter PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

target_link_libraries(AnyAudioImporter ${MAGNUM_LIBRARIES} ${MAGNUM_AUDIO_LIBRARIES})

install(FILES ${AnyAudioImporter_HEADERS} DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/AnyAudioImporter)

if(BUILD_TESTS)
    add_library(MagnumAnyAudioImporterTestLib STATIC $<TARGET_OBJECTS:AnyAudioImporterObjects>)
    target_link_libraries(MagnumAnyAudioImporterTestLib ${MAGNUM_LIBRARIES} ${MAGNUM_AUDIO_LIBRARIES})
    add_subdirectory(Test)
endif()

# Magnum AnyAudioImporter library for superprojects
set(MAGNUMPLUGINS_ANYAUDIOIMPORTER_LIBRARY AnyAudioImporter CACHE INTERNAL "")
