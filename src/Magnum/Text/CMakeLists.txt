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
set(CMAKE_FOLDER "Magnum/Text")

find_package(Corrade REQUIRED PluginManager)

# Files shared between main library and unit test library
set(MagnumText_SRCS
    Direction.cpp)

# Files compiled with different flags for main library and unit test library
set(MagnumText_GracefulAssert_SRCS
    AbstractFont.cpp
    AbstractFontConverter.cpp
    AbstractGlyphCache.cpp
    AbstractShaper.cpp
    Alignment.cpp
    Feature.cpp
    Renderer.cpp
    Script.cpp)

set(MagnumText_HEADERS
    AbstractFont.h
    AbstractFontConverter.h
    AbstractGlyphCache.h
    AbstractShaper.h
    Alignment.h
    Direction.h
    Feature.h
    Renderer.h
    Script.h
    Text.h

    visibility.h)

set(MagnumText_PRIVATE_HEADERS
    Implementation/printFourCC.h
    Implementation/rendererState.h)

if(MAGNUM_TARGET_GL)
    list(APPEND MagnumText_GracefulAssert_SRCS
        DistanceFieldGlyphCacheGL.cpp
        GlyphCacheGL.cpp
        RendererGL.cpp)
    list(APPEND MagnumText_HEADERS
        DistanceFieldGlyphCacheGL.h
        GlyphCacheGL.h
        RendererGL.h)
    if(MAGNUM_BUILD_DEPRECATED)
        list(APPEND MagnumText_HEADERS
            DistanceFieldGlyphCache.h
            GlyphCache.h)
    endif()
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
    ${MagnumText_HEADERS}
    ${MagnumText_PRIVATE_HEADERS})
target_include_directories(MagnumTextObjects PUBLIC
    $<TARGET_PROPERTY:Magnum,INTERFACE_INCLUDE_DIRECTORIES>)
if(MAGNUM_TARGET_GL)
    target_include_directories(MagnumTextObjects PUBLIC $<TARGET_PROPERTY:MagnumGL,INTERFACE_INCLUDE_DIRECTORIES>)
endif()
# Include dependencies after Magnum itself, to avoid stale installed Magnum
# headers being preferred over the project-local ones
target_include_directories(MagnumTextObjects PUBLIC
    $<TARGET_PROPERTY:Corrade::PluginManager,INTERFACE_INCLUDE_DIRECTORIES>)
if(NOT MAGNUM_BUILD_STATIC)
    target_compile_definitions(MagnumTextObjects PRIVATE "MagnumTextObjects_EXPORTS")
endif()
if(NOT MAGNUM_BUILD_STATIC OR MAGNUM_BUILD_STATIC_PIC)
    set_target_properties(MagnumTextObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Text library
add_library(MagnumText ${SHARED_OR_STATIC}
    $<TARGET_OBJECTS:MagnumTextObjects>
    ${MagnumText_GracefulAssert_SRCS})
set_target_properties(MagnumText PROPERTIES DEBUG_POSTFIX "-d")
if(NOT MAGNUM_BUILD_STATIC)
    set_target_properties(MagnumText PROPERTIES VERSION ${MAGNUM_LIBRARY_VERSION} SOVERSION ${MAGNUM_LIBRARY_SOVERSION})
elseif(MAGNUM_BUILD_STATIC_PIC)
    set_target_properties(MagnumText PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(MagnumText PUBLIC
    Magnum
    MagnumTextureTools)
if(MAGNUM_TARGET_GL)
    target_link_libraries(MagnumText PUBLIC MagnumGL)
endif()
# Include dependencies after Magnum itself, to avoid stale installed Magnum
# headers being preferred over the project-local ones
target_link_libraries(MagnumText PUBLIC Corrade::PluginManager)

install(TARGETS MagnumText
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumText_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Text)

if(MAGNUM_WITH_FONTCONVERTER)
    if(NOT MAGNUM_TARGET_GL)
        message(SEND_ERROR "magnum-fontconverter is available only if MAGNUM_TARGET_GL is enabled")
    endif()
    if(CORRADE_TARGET_EMSCRIPTEN)
        message(SEND_ERROR "The magnum-fontconverter utility isn't available on Emscripten due to lack of GPU access from Node.js. Set MAGNUM_WITH_FONTCONVERTER to OFF to skip building it.")
    endif()

    find_package(Corrade REQUIRED Main)

    add_executable(magnum-fontconverter fontconverter.cpp)
    target_link_libraries(magnum-fontconverter PRIVATE
        Corrade::Main
        Magnum
        MagnumText
        MagnumTrade
        ${MAGNUM_FONTCONVERTER_STATIC_PLUGINS})
    if(MAGNUM_TARGET_EGL)
        target_link_libraries(magnum-fontconverter PRIVATE MagnumWindowlessEglApplication)
    elseif(CORRADE_TARGET_IOS)
        target_link_libraries(magnum-fontconverter PRIVATE MagnumWindowlessIosApplication)
    elseif(CORRADE_TARGET_APPLE)
        target_link_libraries(magnum-fontconverter PRIVATE MagnumWindowlessCglApplication)
    elseif(CORRADE_TARGET_UNIX)
        target_link_libraries(magnum-fontconverter PRIVATE MagnumWindowlessGlxApplication)
    elseif(CORRADE_TARGET_WINDOWS)
        target_link_libraries(magnum-fontconverter PRIVATE MagnumWindowlessWglApplication)
    else()
        message(FATAL_ERROR "magnum-fontconverter is not available on this platform. Set MAGNUM_WITH_FONTCONVERTER to OFF to suppress this warning.")
    endif()

    install(TARGETS magnum-fontconverter DESTINATION ${MAGNUM_BINARY_INSTALL_DIR})

    # Magnum fontconverter target alias for superprojects
    add_executable(Magnum::fontconverter ALIAS magnum-fontconverter)
endif()

if(MAGNUM_BUILD_TESTS)
    # Library with graceful assert for testing
    add_library(MagnumTextTestLib ${SHARED_OR_STATIC} ${EXCLUDE_FROM_ALL_IF_TEST_TARGET}
        $<TARGET_OBJECTS:MagnumTextObjects>
        ${MagnumText_GracefulAssert_SRCS})
    set_target_properties(MagnumTextTestLib PROPERTIES DEBUG_POSTFIX "-d")
    target_compile_definitions(MagnumTextTestLib PRIVATE
        "CORRADE_GRACEFUL_ASSERT" "MagnumText_EXPORTS")
    if(MAGNUM_BUILD_STATIC_PIC)
        set_target_properties(MagnumTextTestLib PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    target_link_libraries(MagnumTextTestLib PUBLIC
        Magnum
        MagnumTextureTools
        Corrade::PluginManager)
    if(MAGNUM_TARGET_GL)
        target_link_libraries(MagnumText PUBLIC MagnumGL)
    endif()

    add_subdirectory(Test ${EXCLUDE_FROM_ALL_IF_TEST_TARGET})
endif()

# Magnum Text target alias for superprojects
add_library(Magnum::Text ALIAS MagnumText)
