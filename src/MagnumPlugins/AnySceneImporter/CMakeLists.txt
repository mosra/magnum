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

if(BUILD_STATIC)
    set(MAGNUM_ANYSCENEIMPORTER_BUILD_STATIC 1)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

set(AnySceneImporter_SRCS
    AnySceneImporter.cpp)

set(AnySceneImporter_HEADERS
    AnySceneImporter.h)

# Objects shared between plugin and test library
add_library(AnySceneImporterObjects OBJECT
    ${AnySceneImporter_SRCS}
    ${AnySceneImporter_HEADERS})
target_include_directories(AnySceneImporterObjects PUBLIC
    $<TARGET_PROPERTY:Magnum::Magnum,INTERFACE_INCLUDE_DIRECTORIES>
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}/src)
target_compile_definitions(AnySceneImporterObjects PRIVATE "AnySceneImporterObjects_EXPORTS")
if(NOT BUILD_STATIC OR BUILD_STATIC_PIC)
    set_target_properties(AnySceneImporterObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
set_target_properties(AnySceneImporterObjects PROPERTIES FOLDER "MagnumPlugins/AnySceneImporter")

# AnySceneImporter plugin
add_plugin(AnySceneImporter
    "${MAGNUM_PLUGINS_IMPORTER_DEBUG_BINARY_INSTALL_DIR};${MAGNUM_PLUGINS_IMPORTER_DEBUG_LIBRARY_INSTALL_DIR}"
    "${MAGNUM_PLUGINS_IMPORTER_RELEASE_BINARY_INSTALL_DIR};${MAGNUM_PLUGINS_IMPORTER_RELEASE_LIBRARY_INSTALL_DIR}"
    AnySceneImporter.conf
    $<TARGET_OBJECTS:AnySceneImporterObjects>
    pluginRegistration.cpp)
if(BUILD_STATIC_PIC)
    set_target_properties(AnySceneImporter PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_include_directories(AnySceneImporter PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}/src)
target_link_libraries(AnySceneImporter Magnum::Magnum)

install(FILES ${AnySceneImporter_HEADERS} DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/AnySceneImporter)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/configure.h DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/AnySceneImporter)

if(BUILD_TESTS)
    add_library(MagnumAnySceneImporterTestLib STATIC
        $<TARGET_OBJECTS:AnySceneImporterObjects>
        ${PROJECT_SOURCE_DIR}/src/dummy.cpp) # XCode workaround, see file comment for details
    target_include_directories(MagnumAnySceneImporterTestLib PUBLIC
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_BINARY_DIR}/src)
    set_target_properties(MagnumAnySceneImporterTestLib PROPERTIES FOLDER "MagnumPlugins/AnySceneImporter")
    target_link_libraries(MagnumAnySceneImporterTestLib Magnum::Magnum)
    add_subdirectory(Test)
endif()

# MagnumPlugins AnySceneImporter target alias for superprojects
add_library(MagnumPlugins::AnySceneImporter ALIAS AnySceneImporter)
