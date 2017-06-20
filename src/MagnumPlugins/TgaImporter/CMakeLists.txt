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
    set(MAGNUM_TGAIMPORTER_BUILD_STATIC 1)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

set(TgaImporter_SRCS
    TgaImporter.cpp)

set(TgaImporter_HEADERS
    TgaHeader.h
    TgaImporter.h)

# Objects shared between plugin and test library
add_library(TgaImporterObjects OBJECT
    ${TgaImporter_SRCS}
    ${TgaImporter_HEADERS})
target_include_directories(TgaImporterObjects PUBLIC $<TARGET_PROPERTY:Magnum,INTERFACE_INCLUDE_DIRECTORIES>)
if(NOT BUILD_PLUGINS_STATIC)
    target_compile_definitions(TgaImporterObjects PRIVATE "TgaImporterObjects_EXPORTS")
endif()
if(NOT BUILD_PLUGINS_STATIC OR BUILD_STATIC_PIC)
    set_target_properties(TgaImporterObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
set_target_properties(TgaImporterObjects PROPERTIES FOLDER "MagnumPlugins/TgaImporter")

# TgaImporter plugin
add_plugin(TgaImporter
    "${MAGNUM_PLUGINS_IMPORTER_DEBUG_BINARY_INSTALL_DIR};${MAGNUM_PLUGINS_IMPORTER_DEBUG_LIBRARY_INSTALL_DIR}"
    "${MAGNUM_PLUGINS_IMPORTER_RELEASE_BINARY_INSTALL_DIR};${MAGNUM_PLUGINS_IMPORTER_RELEASE_LIBRARY_INSTALL_DIR}"
    TgaImporter.conf
    $<TARGET_OBJECTS:TgaImporterObjects>
    pluginRegistration.cpp)
if(BUILD_STATIC_PIC)
    set_target_properties(TgaImporter PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(TgaImporter Magnum)

install(FILES ${TgaImporter_HEADERS} DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/TgaImporter)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/configure.h DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/TgaImporter)

if(BUILD_TESTS)
    add_library(MagnumTgaImporterTestLib STATIC
        $<TARGET_OBJECTS:TgaImporterObjects>
        ${PROJECT_SOURCE_DIR}/src/dummy.cpp) # XCode workaround, see file comment for details
    set_target_properties(MagnumTgaImporterTestLib PROPERTIES FOLDER "MagnumPlugins/TgaImporter")
    target_link_libraries(MagnumTgaImporterTestLib Magnum)

    add_subdirectory(Test)
endif()

# Magnum TgaImporter target alias for superprojects
add_library(Magnum::TgaImporter ALIAS TgaImporter)
