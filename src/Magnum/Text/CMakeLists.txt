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

# Files shared between main library and unit test library
set(MagnumText_SRCS )

# Files compiled with different flags for main library and unit test library
set(MagnumText_GracefulAssert_SRCS
    AbstractFont.cpp
    AbstractFontConverter.cpp
    AbstractGlyphCache.cpp)

set(MagnumText_HEADERS
    AbstractFont.h
    AbstractFontConverter.h
    AbstractGlyphCache.h
    Alignment.h
    Text.h

    visibility.h)

if(TARGET_GL)
    list(APPEND MagnumText_SRCS
        DistanceFieldGlyphCache.cpp
        GlyphCache.cpp
        Renderer.cpp)
    list(APPEND MagnumText_HEADERS
        DistanceFieldGlyphCache.h
        GlyphCache.h
        Renderer.h)
else()
    # So MagnumTextObjects has at least something
    list(APPEND MagnumText_SRCS ${PROJECT_SOURCE_DIR}/src/dummy.cpp)
endif()

if(NOT CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
                   ${CMAKE_CURRENT_BINARY_DIR}/configure.h)
endif()

# Objects shared between main and test library
add_library(MagnumTextObjects OBJECT
    ${MagnumText_SRCS}
    ${MagnumText_HEADERS})
target_include_directories(MagnumTextObjects PUBLIC
    $<TARGET_PROPERTY:Corrade::PluginManager,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Magnum,INTERFACE_INCLUDE_DIRECTORIES>)
if(TARGET_GL)
    target_include_directories(MagnumTextObjects PUBLIC $<TARGET_PROPERTY:MagnumGL,INTERFACE_INCLUDE_DIRECTORIES>)
endif()
if(NOT BUILD_STATIC)
    target_compile_definitions(MagnumTextObjects PRIVATE "MagnumTextObjects_EXPORTS")
endif()
if(NOT BUILD_STATIC OR BUILD_STATIC_PIC)
    set_target_properties(MagnumTextObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
set_target_properties(MagnumTextObjects PROPERTIES FOLDER "Magnum/Text")

# Text library
add_library(MagnumText ${SHARED_OR_STATIC}
    $<TARGET_OBJECTS:MagnumTextObjects>
    ${MagnumText_GracefulAssert_SRCS})
set_target_properties(MagnumText PROPERTIES
    DEBUG_POSTFIX "-d"
    FOLDER "Magnum/Text")
if(NOT BUILD_STATIC)
    set_target_properties(MagnumText PROPERTIES VERSION ${MAGNUM_LIBRARY_VERSION} SOVERSION ${MAGNUM_LIBRARY_SOVERSION})
elseif(BUILD_STATIC_PIC)
    set_target_properties(MagnumText PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(MagnumText PUBLIC
    Magnum
    MagnumTextureTools
    Corrade::PluginManager)
if(TARGET_GL)
    target_link_libraries(MagnumText PUBLIC MagnumGL)
endif()

install(TARGETS MagnumText
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumText_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Text)

if(WITH_FONTCONVERTER)
    if(NOT TARGET_GL)
        message(SEND_ERROR "magnum-fontconverter is available only if TARGET_GL is enabled")
    endif()

    add_executable(magnum-fontconverter fontconverter.cpp)
    target_link_libraries(magnum-fontconverter PRIVATE
        Magnum
        MagnumText
        MagnumTrade)
    if(MAGNUM_TARGET_HEADLESS)
        target_link_libraries(magnum-fontconverter PRIVATE MagnumWindowlessEglApplication)
    elseif(CORRADE_TARGET_IOS)
        target_link_libraries(magnum-fontconverter PRIVATE MagnumWindowlessIosApplication)
    elseif(CORRADE_TARGET_APPLE)
        target_link_libraries(magnum-fontconverter PRIVATE MagnumWindowlessCglApplication)
    elseif(CORRADE_TARGET_UNIX)
        if(MAGNUM_TARGET_GLES AND NOT MAGNUM_TARGET_DESKTOP_GLES)
            target_link_libraries(magnum-fontconverter PRIVATE MagnumWindowlessEglApplication)
        else()
            target_link_libraries(magnum-fontconverter PRIVATE MagnumWindowlessGlxApplication)
        endif()
    elseif(CORRADE_TARGET_WINDOWS)
        if(MAGNUM_TARGET_GLES AND NOT MAGNUM_TARGET_DESKTOP_GLES)
            target_link_libraries(magnum-fontconverter PRIVATE MagnumWindowlessWindowsEglApplication)
        else()
            target_link_libraries(magnum-fontconverter PRIVATE MagnumWindowlessWglApplication)
        endif()
    else()
        message(FATAL_ERROR "magnum-fontconverter is not available on this platform. Set WITH_FONTCONVERTER to OFF to suppress this warning.")
    endif()
    set_target_properties(magnum-fontconverter PROPERTIES FOLDER "Magnum/Text")

    install(TARGETS magnum-fontconverter DESTINATION ${MAGNUM_BINARY_INSTALL_DIR})

    # Magnum fontconverter target alias for superprojects
    add_executable(Magnum::fontconverter ALIAS magnum-fontconverter)
endif()

if(BUILD_TESTS)
    # Library with graceful assert for testing
    add_library(MagnumTextTestLib ${SHARED_OR_STATIC}
        $<TARGET_OBJECTS:MagnumTextObjects>
        ${MagnumText_GracefulAssert_SRCS})
    set_target_properties(MagnumTextTestLib PROPERTIES
        DEBUG_POSTFIX "-d"
        FOLDER "Magnum/Text")
    target_compile_definitions(MagnumTextTestLib PRIVATE
        "CORRADE_GRACEFUL_ASSERT" "MagnumText_EXPORTS")
    if(BUILD_STATIC_PIC)
        set_target_properties(MagnumTextTestLib PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    target_link_libraries(MagnumTextTestLib PUBLIC
        Magnum
        MagnumTextureTools
        Corrade::PluginManager)
    if(TARGET_GL)
        target_link_libraries(MagnumText PUBLIC MagnumGL)
    endif()

    add_subdirectory(Test)
endif()

# Magnum Text target alias for superprojects
add_library(Magnum::Text ALIAS MagnumText)
