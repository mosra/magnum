#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

if(BUILD_STATIC)
    set(MAGNUM_ANYAUDIOIMPORTER_BUILD_STATIC 1)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

set(AnyAudioImporter_SRCS
    AnyImporter.cpp)

set(AnyAudioImporter_HEADERS
    AnyImporter.h)

# Objects shared between plugin and test library
add_library(AnyAudioImporterObjects OBJECT
    ${AnyAudioImporter_SRCS}
    ${AnyAudioImporter_HEADERS})
target_include_directories(AnyAudioImporterObjects PUBLIC
    $<TARGET_PROPERTY:Magnum,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:MagnumAudio,INTERFACE_INCLUDE_DIRECTORIES>)
target_compile_definitions(AnyAudioImporterObjects PRIVATE "AnyAudioImporterObjects_EXPORTS")
if(NOT BUILD_STATIC OR BUILD_STATIC_PIC)
    set_target_properties(AnyAudioImporterObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
set_target_properties(AnyAudioImporterObjects PROPERTIES FOLDER "MagnumPlugins/AnyAudioImporter")

# AnyAudioImporter plugin
add_plugin(AnyAudioImporter
    "${MAGNUM_PLUGINS_AUDIOIMPORTER_DEBUG_BINARY_INSTALL_DIR};${MAGNUM_PLUGINS_AUDIOIMPORTER_DEBUG_LIBRARY_INSTALL_DIR}"
    "${MAGNUM_PLUGINS_AUDIOIMPORTER_RELEASE_BINARY_INSTALL_DIR};${MAGNUM_PLUGINS_AUDIOIMPORTER_RELEASE_LIBRARY_INSTALL_DIR}"
    AnyAudioImporter.conf
    $<TARGET_OBJECTS:AnyAudioImporterObjects>
    pluginRegistration.cpp)
if(BUILD_STATIC_PIC)
    set_target_properties(AnyAudioImporter PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_include_directories(AnyAudioImporter PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}/src)
target_link_libraries(AnyAudioImporter PUBLIC Magnum MagnumAudio)

install(FILES ${AnyAudioImporter_HEADERS} DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/AnyAudioImporter)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/configure.h DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/AnyAudioImporter)

# Automatic static plugin import
if(BUILD_PLUGINS_STATIC)
    install(FILES importStaticPlugin.cpp DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/AnyAudioImporter)
    if(NOT CMAKE_VERSION VERSION_LESS 3.1)
        target_sources(AnyAudioImporter INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/importStaticPlugin.cpp)
    endif()
endif()

if(BUILD_TESTS)
    add_library(MagnumAnyAudioImporterTestLib STATIC
        $<TARGET_OBJECTS:AnyAudioImporterObjects>
        ${PROJECT_SOURCE_DIR}/src/dummy.cpp) # XCode workaround, see file comment for details
    target_include_directories(MagnumAnyAudioImporterTestLib PUBLIC
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_BINARY_DIR}/src)
    set_target_properties(MagnumAnyAudioImporterTestLib PROPERTIES FOLDER "MagnumPlugins/AnyAudioImporter")
    target_link_libraries(MagnumAnyAudioImporterTestLib PUBLIC Magnum MagnumAudio)
    add_subdirectory(Test)
endif()

# MagnumPlugins AnyAudioImporter target alias for superprojects
add_library(MagnumPlugins::AnyAudioImporter ALIAS AnyAudioImporter)
