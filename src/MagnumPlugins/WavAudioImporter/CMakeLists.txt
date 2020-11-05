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

find_package(Corrade REQUIRED PluginManager)

if(BUILD_PLUGINS_STATIC)
    set(MAGNUM_WAVAUDIOIMPORTER_BUILD_STATIC 1)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

# Objects shared between plugin and test library
add_library(WavAudioImporterObjects OBJECT
    WavHeader.cpp
    WavHeader.h)
target_include_directories(WavAudioImporterObjects PUBLIC
    $<TARGET_PROPERTY:Magnum,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:MagnumAudio,INTERFACE_INCLUDE_DIRECTORIES>)
if(NOT MAGNUM_WAVAUDIOIMPORTER_BUILD_STATIC)
    target_compile_definitions(WavAudioImporterObjects PRIVATE "WavAudioImporterObjects_EXPORTS")
endif()
if(NOT MAGNUM_WAVAUDIOIMPORTER_BUILD_STATIC OR BUILD_STATIC_PIC)
    set_target_properties(WavAudioImporterObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
set_target_properties(WavAudioImporterObjects PROPERTIES FOLDER "MagnumPlugins/WavAudioImporter")

# WavAudioImporter plugin
add_plugin(WavAudioImporter
    "${MAGNUM_PLUGINS_AUDIOIMPORTER_DEBUG_BINARY_INSTALL_DIR};${MAGNUM_PLUGINS_AUDIOIMPORTER_DEBUG_LIBRARY_INSTALL_DIR}"
    "${MAGNUM_PLUGINS_AUDIOIMPORTER_RELEASE_BINARY_INSTALL_DIR};${MAGNUM_PLUGINS_AUDIOIMPORTER_RELEASE_LIBRARY_INSTALL_DIR}"
    WavAudioImporter.conf
    WavImporter.cpp
    WavImporter.h
    $<TARGET_OBJECTS:WavAudioImporterObjects>)
if(MAGNUM_WAVAUDIOIMPORTER_BUILD_STATIC AND BUILD_STATIC_PIC)
    set_target_properties(WavAudioImporter PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(WavAudioImporter PUBLIC Magnum MagnumAudio)
# Modify output location only if all are set, otherwise it makes no sense
if(CMAKE_RUNTIME_OUTPUT_DIRECTORY AND CMAKE_LIBRARY_OUTPUT_DIRECTORY AND CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set_target_properties(WavAudioImporter PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/magnum$<$<CONFIG:Debug>:-d>/audioimporters
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/magnum$<$<CONFIG:Debug>:-d>/audioimporters
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/magnum$<$<CONFIG:Debug>:-d>/audioimporters)
endif()

install(FILES WavImporter.h ${CMAKE_CURRENT_BINARY_DIR}/configure.h
    DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/WavAudioImporter)

# Automatic static plugin import
if(MAGNUM_WAVAUDIOIMPORTER_BUILD_STATIC)
    install(FILES importStaticPlugin.cpp DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/WavAudioImporter)
    target_sources(WavAudioImporter INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/importStaticPlugin.cpp)
endif()

if(BUILD_TESTS)
    add_subdirectory(Test)
endif()

# Magnum WavAudioImporter target alias for superprojects
add_library(Magnum::WavAudioImporter ALIAS WavAudioImporter)
