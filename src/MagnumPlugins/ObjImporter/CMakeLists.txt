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

find_package(Corrade REQUIRED PluginManager)

if(MAGNUM_BUILD_PLUGINS_STATIC AND NOT DEFINED MAGNUM_OBJIMPORTER_BUILD_STATIC)
    set(MAGNUM_OBJIMPORTER_BUILD_STATIC 1)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

# ObjImporter plugin
add_plugin(ObjImporter
    importers
    "${MAGNUM_PLUGINS_IMPORTER_DEBUG_BINARY_INSTALL_DIR};${MAGNUM_PLUGINS_IMPORTER_DEBUG_LIBRARY_INSTALL_DIR}"
    "${MAGNUM_PLUGINS_IMPORTER_RELEASE_BINARY_INSTALL_DIR};${MAGNUM_PLUGINS_IMPORTER_RELEASE_LIBRARY_INSTALL_DIR}"
    ObjImporter.conf
    ObjImporter.cpp
    ObjImporter.h)
if(MAGNUM_OBJIMPORTER_BUILD_STATIC AND MAGNUM_BUILD_STATIC_PIC)
    set_target_properties(ObjImporter PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(ObjImporter PUBLIC MagnumTrade MagnumMeshTools)
if(CORRADE_TARGET_EMSCRIPTEN)
    # Since 1.39.0, this needs to be enabled on the plugin. Before (on 1.38.44
    # at least) this wasn't needed, at least for the test -- having it enabled
    # for the test binary and in the linker was enough. Most probably related
    # to fastcomp->llvm backend switch.
    set_property(TARGET ObjImporter APPEND_STRING PROPERTY COMPILE_FLAGS " -s DISABLE_EXCEPTION_CATCHING=0")
endif()

install(FILES ObjImporter.h ${CMAKE_CURRENT_BINARY_DIR}/configure.h
    DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/ObjImporter)

# Automatic static plugin import
if(MAGNUM_OBJIMPORTER_BUILD_STATIC)
    install(FILES importStaticPlugin.cpp DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/ObjImporter)
    target_sources(ObjImporter INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/importStaticPlugin.cpp)
endif()

if(MAGNUM_BUILD_TESTS)
    add_subdirectory(Test ${EXCLUDE_FROM_ALL_IF_TEST_TARGET})
endif()

# Magnum ObjImporter library for superprojects
add_library(Magnum::ObjImporter ALIAS ObjImporter)
