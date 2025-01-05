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
set(CMAKE_FOLDER "Magnum/TextureTools")

# Somehow, due to MagnumTradeObjects having target_include_directories() with
# $<TARGET_PROPERTY:Corrade::PluginManager,INTERFACE_INCLUDE_DIRECTORIES>,
# if MAGNUM_DISTANCEFIELDCONVERTER_STATIC_PLUGINS is non-empty then CMake fails
# with
#
#   Target "Corrade::PluginManager" not found.
#
# unless the find_package() is here. Not sure why, probably some bug in CMake
# dependency handling? Changing target_include_directories() to PRIVATE doesn't
# help, removing it altogether helps.
find_package(Corrade REQUIRED PluginManager)

set(MagnumTextureTools_GracefulAssert_SRCS
    Atlas.cpp)

set(MagnumTextureTools_HEADERS
    Atlas.h
    TextureTools.h

    visibility.h)

if(MAGNUM_TARGET_GL)
    corrade_add_resource(MagnumTextureTools_RESOURCES resources.conf)
    if(MAGNUM_BUILD_STATIC)
        # On the static build we're importing the resources manually, so no
        # need to have the implicit initializers as well.
        set_property(SOURCE ${MagnumTextureTools_RESOURCES} APPEND PROPERTY
            COMPILE_DEFINITIONS
                "CORRADE_AUTOMATIC_INITIALIZER=CORRADE_NOOP"
                "CORRADE_AUTOMATIC_FINALIZER=CORRADE_NOOP")
    endif()

    list(APPEND MagnumTextureTools_GracefulAssert_SRCS
        DistanceFieldGL.cpp
        ${MagnumTextureTools_RESOURCES})

    list(APPEND MagnumTextureTools_HEADERS DistanceFieldGL.h)

    if(MAGNUM_BUILD_DEPRECATED)
        list(APPEND MagnumTextureTools_HEADERS DistanceField.h)
    endif()
endif()

# TextureTools library
add_library(MagnumTextureTools ${SHARED_OR_STATIC}
    ${MagnumTextureTools_GracefulAssert_SRCS}
    ${MagnumTextureTools_HEADERS})
set_target_properties(MagnumTextureTools PROPERTIES DEBUG_POSTFIX "-d")
if(NOT MAGNUM_BUILD_STATIC)
    set_target_properties(MagnumTextureTools PROPERTIES VERSION ${MAGNUM_LIBRARY_VERSION} SOVERSION ${MAGNUM_LIBRARY_SOVERSION})
elseif(MAGNUM_BUILD_STATIC_PIC)
    set_target_properties(MagnumTextureTools PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(MagnumTextureTools PUBLIC
    Magnum)
if(MAGNUM_TARGET_GL)
    target_link_libraries(MagnumTextureTools PUBLIC MagnumGL)
endif()

install(TARGETS MagnumTextureTools
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumTextureTools_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/TextureTools)

if(MAGNUM_WITH_DISTANCEFIELDCONVERTER)
    if(NOT MAGNUM_TARGET_GL)
        message(SEND_ERROR "The magnum-distancefieldconverter utility is available only if MAGNUM_TARGET_GL is enabled")
    endif()
    if(CORRADE_TARGET_EMSCRIPTEN)
        message(SEND_ERROR "The magnum-distancefieldconverter utility isn't available on Emscripten due to lack of GPU access from Node.js. Set MAGNUM_WITH_DISTANCEFIELDCONVERTER to OFF to skip building it.")
    endif()

    find_package(Corrade REQUIRED Main)

    add_executable(magnum-distancefieldconverter distancefieldconverter.cpp)
    target_link_libraries(magnum-distancefieldconverter PRIVATE
        Corrade::Main
        Magnum
        MagnumTextureTools
        MagnumTrade
        ${MAGNUM_DISTANCEFIELDCONVERTER_STATIC_PLUGINS})
    if(MAGNUM_TARGET_EGL)
        target_link_libraries(magnum-distancefieldconverter PRIVATE MagnumWindowlessEglApplication)
    elseif(CORRADE_TARGET_IOS)
        target_link_libraries(magnum-distancefieldconverter PRIVATE MagnumWindowlessIosApplication)
    elseif(CORRADE_TARGET_APPLE)
        target_link_libraries(magnum-distancefieldconverter PRIVATE MagnumWindowlessCglApplication)
    elseif(CORRADE_TARGET_UNIX)
        target_link_libraries(magnum-distancefieldconverter PRIVATE MagnumWindowlessGlxApplication)
    elseif(CORRADE_TARGET_WINDOWS)
        target_link_libraries(magnum-distancefieldconverter PRIVATE MagnumWindowlessWglApplication)
    else()
        message(FATAL_ERROR "magnum-distancefieldconverter is not available on this platform. Set MAGNUM_WITH_DISTANCEFIELDCONVERTER to OFF to suppress this warning.")
    endif()

    install(TARGETS magnum-distancefieldconverter DESTINATION ${MAGNUM_BINARY_INSTALL_DIR})

    # Magnum distancefieldconverter target alias for superprojects
    add_executable(Magnum::distancefieldconverter ALIAS magnum-distancefieldconverter)
endif()

if(MAGNUM_BUILD_TESTS)
    # Library with graceful assert for testing
    add_library(MagnumTextureToolsTestLib ${SHARED_OR_STATIC} ${EXCLUDE_FROM_ALL_IF_TEST_TARGET}
        ${MagnumTextureTools_GracefulAssert_SRCS})
    set_target_properties(MagnumTextureToolsTestLib PROPERTIES DEBUG_POSTFIX "-d")
    if(CMAKE_GENERATOR STREQUAL Xcode)
        # Xcode's "new build system" doesn't like when the same (generated)
        # source file is used by two different targets (MagnumTextureTools and
        # MagnumTextureToolsTestLib) that don't have any other dependency
        # between them. WTF.
        add_dependencies(MagnumTextureToolsTestLib MagnumTextureTools)
    endif()
    target_compile_definitions(MagnumTextureToolsTestLib PRIVATE
        "CORRADE_GRACEFUL_ASSERT" "MagnumTextureTools_EXPORTS")
    if(MAGNUM_BUILD_STATIC_PIC)
        set_target_properties(MagnumTextureToolsTestLib PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    target_link_libraries(MagnumTextureToolsTestLib PUBLIC
        Magnum)
    if(MAGNUM_TARGET_GL)
        target_link_libraries(MagnumTextureToolsTestLib PUBLIC MagnumGL)
    endif()

    add_subdirectory(Test ${EXCLUDE_FROM_ALL_IF_TEST_TARGET})
endif()

# Magnum TextureTools target alias for superprojects
add_library(Magnum::TextureTools ALIAS MagnumTextureTools)
