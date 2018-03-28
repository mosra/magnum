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

if(BUILD_PLUGINS_STATIC)
    set(MAGNUM_OBJIMPORTER_BUILD_STATIC 1)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

set(ObjImporter_SRCS
    ObjImporter.cpp)

set(ObjImporter_HEADERS
    ObjImporter.h)

# Objects shared between plugin and test library
add_library(ObjImporterObjects OBJECT
    ${ObjImporter_SRCS}
    ${ObjImporter_HEADERS})
target_include_directories(ObjImporterObjects PUBLIC $<TARGET_PROPERTY:Magnum,INTERFACE_INCLUDE_DIRECTORIES>)
if(NOT BUILD_PLUGINS_STATIC)
    target_compile_definitions(ObjImporterObjects PRIVATE "ObjImporterObjects_EXPORTS")
endif()
if(NOT BUILD_PLUGINS_STATIC OR BUILD_STATIC_PIC)
    set_target_properties(ObjImporterObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
set_target_properties(ObjImporterObjects PROPERTIES FOLDER "MagnumPlugins/ObjImporter")

# ObjImporter plugin
add_plugin(ObjImporter
    "${MAGNUM_PLUGINS_IMPORTER_DEBUG_BINARY_INSTALL_DIR};${MAGNUM_PLUGINS_IMPORTER_DEBUG_LIBRARY_INSTALL_DIR}"
    "${MAGNUM_PLUGINS_IMPORTER_RELEASE_BINARY_INSTALL_DIR};${MAGNUM_PLUGINS_IMPORTER_RELEASE_LIBRARY_INSTALL_DIR}"
    ObjImporter.conf
    $<TARGET_OBJECTS:ObjImporterObjects>
    pluginRegistration.cpp)
if(BUILD_STATIC_PIC)
    set_target_properties(ObjImporter PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(ObjImporter Magnum MagnumMeshTools)
if(CORRADE_TARGET_WINDOWS)
    target_link_libraries(ObjImporter AnyImageImporter)
endif()

install(FILES ${ObjImporter_HEADERS} DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/ObjImporter)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/configure.h DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/ObjImporter)

# Automatic static plugin import
if(BUILD_PLUGINS_STATIC)
    install(FILES importStaticPlugin.cpp DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/ObjImporter)
    if(NOT CMAKE_VERSION VERSION_LESS 3.1)
        target_sources(ObjImporter INTERFACE importStaticPlugin.cpp)
    endif()
endif()

if(BUILD_TESTS)
    # On Win32 we need to avoid dllimporting AnyImageImporter symbols, because
    # it would search for the symbols in some DLL even when they were linked
    # statically. However it apparently doesn't matter that they were
    # dllexported when building the static library. EH. And because the
    # -DObjImporterObjects_EXPORTS is no longer set in this case, we need
    # to avoid dllimporting ObjImporter symbols as well.
    if(CORRADE_TARGET_WINDOWS)
        add_library(MagnumObjImporterTestLib STATIC
            ${ObjImporter_SRCS}
            ${ObjImporter_HEADERS})
        target_compile_definitions(MagnumObjImporterTestLib PRIVATE
            "MAGNUM_ANYIMAGEIMPORTER_BUILD_STATIC"
            "MAGNUM_OBJIMPORTER_BUILD_STATIC")
    else()
        add_library(MagnumObjImporterTestLib STATIC
            $<TARGET_OBJECTS:ObjImporterObjects>
            ${PROJECT_SOURCE_DIR}/src/dummy.cpp) # XCode workaround, see file comment for details
    endif()
    set_target_properties(MagnumObjImporterTestLib PROPERTIES FOLDER "MagnumPlugins/ObjImporter")
    target_link_libraries(MagnumObjImporterTestLib Magnum MagnumMeshTools MagnumAnyImageImporterTestLib)

    add_subdirectory(Test)
endif()

# Magnum ObjImporter library for superprojects
add_library(Magnum:::ObjImporter ALIAS ObjImporter)
