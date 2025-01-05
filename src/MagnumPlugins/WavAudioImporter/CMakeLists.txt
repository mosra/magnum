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
set(CMAKE_FOLDER "MagnumPlugins/WavAudioImporter")

find_package(Corrade REQUIRED PluginManager)

if(MAGNUM_BUILD_PLUGINS_STATIC AND NOT DEFINED MAGNUM_WAVAUDIOIMPORTER_BUILD_STATIC)
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
if(NOT MAGNUM_WAVAUDIOIMPORTER_BUILD_STATIC OR MAGNUM_BUILD_STATIC_PIC)
    set_target_properties(WavAudioImporterObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# WavAudioImporter plugin
add_plugin(WavAudioImporter
    audioimporters
    "${MAGNUM_PLUGINS_AUDIOIMPORTER_DEBUG_BINARY_INSTALL_DIR};${MAGNUM_PLUGINS_AUDIOIMPORTER_DEBUG_LIBRARY_INSTALL_DIR}"
    "${MAGNUM_PLUGINS_AUDIOIMPORTER_RELEASE_BINARY_INSTALL_DIR};${MAGNUM_PLUGINS_AUDIOIMPORTER_RELEASE_LIBRARY_INSTALL_DIR}"
    WavImporter.conf
    WavImporter.cpp
    WavImporter.h
    $<TARGET_OBJECTS:WavAudioImporterObjects>)
if(MAGNUM_WAVAUDIOIMPORTER_BUILD_STATIC AND MAGNUM_BUILD_STATIC_PIC)
    set_target_properties(WavAudioImporter PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(WavAudioImporter PUBLIC Magnum MagnumAudio)

install(FILES WavImporter.h ${CMAKE_CURRENT_BINARY_DIR}/configure.h
    DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/WavAudioImporter)

# Automatic static plugin import
if(MAGNUM_WAVAUDIOIMPORTER_BUILD_STATIC)
    install(FILES importStaticPlugin.cpp DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/WavAudioImporter)
    target_sources(WavAudioImporter INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/importStaticPlugin.cpp)
endif()

if(MAGNUM_BUILD_TESTS)
    add_subdirectory(Test ${EXCLUDE_FROM_ALL_IF_TEST_TARGET})
endif()

# Magnum WavAudioImporter target alias for superprojects
add_library(Magnum::WavAudioImporter ALIAS WavAudioImporter)
