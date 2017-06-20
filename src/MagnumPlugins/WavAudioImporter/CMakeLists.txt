#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

if(BUILD_PLUGINS_STATIC)
    set(MAGNUM_WAVAUDIOIMPORTER_BUILD_STATIC 1)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

set(WavAudioImporter_SRCS
    WavHeader.cpp
    WavImporter.cpp)

set(WavAudioImporter_HEADERS
    WavHeader.h
    WavImporter.h)

# Objects shared between plugin and test library
add_library(WavAudioImporterObjects OBJECT
    ${WavAudioImporter_SRCS}
    ${WavAudioImporter_HEADERS})
target_include_directories(WavAudioImporterObjects PUBLIC
    $<TARGET_PROPERTY:Magnum,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:MagnumAudio,INTERFACE_INCLUDE_DIRECTORIES>)
if(NOT BUILD_PLUGINS_STATIC)
    target_compile_definitions(WavAudioImporterObjects PRIVATE "WavAudioImporterObjects_EXPORTS")
endif()
if(NOT BUILD_PLUGINS_STATIC OR BUILD_STATIC_PIC)
    set_target_properties(WavAudioImporterObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
set_target_properties(WavAudioImporterObjects PROPERTIES FOLDER "MagnumPlugins/WavAudioImporter")

# WavAudioImporter plugin
add_plugin(WavAudioImporter
    "${MAGNUM_PLUGINS_AUDIOIMPORTER_DEBUG_BINARY_INSTALL_DIR};${MAGNUM_PLUGINS_AUDIOIMPORTER_DEBUG_LIBRARY_INSTALL_DIR}"
    "${MAGNUM_PLUGINS_AUDIOIMPORTER_RELEASE_BINARY_INSTALL_DIR};${MAGNUM_PLUGINS_AUDIOIMPORTER_RELEASE_LIBRARY_INSTALL_DIR}"
    WavAudioImporter.conf
    $<TARGET_OBJECTS:WavAudioImporterObjects>
    pluginRegistration.cpp)
if(BUILD_STATIC_PIC)
    set_target_properties(WavAudioImporter PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(WavAudioImporter Magnum MagnumAudio)

install(FILES ${WavAudioImporter_HEADERS} DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/WavAudioImporter)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/configure.h DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/WavAudioImporter)

if(BUILD_TESTS)
    add_library(MagnumWavAudioImporterTestLib STATIC
        $<TARGET_OBJECTS:WavAudioImporterObjects>
        ${PROJECT_SOURCE_DIR}/src/dummy.cpp) # XCode workaround, see file comment for details
    set_target_properties(MagnumWavAudioImporterTestLib PROPERTIES FOLDER "MagnumPlugins/WavAudioImporter")
    target_link_libraries(MagnumWavAudioImporterTestLib Magnum MagnumAudio)

    add_subdirectory(Test)
endif()

# Magnum WavAudioImporter target alias for superprojects
add_library(Magnum::WavAudioImporter ALIAS WavAudioImporter)
