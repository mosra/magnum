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

set(MagnumText_SRCS
    AbstractFont.cpp
    AbstractFontConverter.cpp
    DistanceFieldGlyphCache.cpp
    GlyphCache.cpp
    Renderer.cpp)
set(MagnumText_HEADERS
    AbstractFont.h
    AbstractFontConverter.h
    Alignment.h
    DistanceFieldGlyphCache.h
    GlyphCache.h
    Renderer.h
    Text.h

    visibility.h)

# Text library
add_library(MagnumText ${SHARED_OR_STATIC}
    ${MagnumText_SRCS}
    ${MagnumText_HEADERS})
set_target_properties(MagnumText PROPERTIES
    DEBUG_POSTFIX "-d"
    FOLDER "Magnum/Text")
if(BUILD_STATIC_PIC)
    set_target_properties(MagnumText PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(MagnumText Magnum MagnumTextureTools)

install(TARGETS MagnumText
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumText_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Text)

if(WITH_FONTCONVERTER)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/fontconverterConfigure.h.cmake
                   ${CMAKE_CURRENT_BINARY_DIR}/fontconverterConfigure.h)

    add_executable(magnum-fontconverter fontconverter.cpp)
    target_include_directories(magnum-fontconverter PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
    target_link_libraries(magnum-fontconverter Magnum MagnumText)
    if(MAGNUM_TARGET_HEADLESS)
        target_link_libraries(magnum-fontconverter MagnumWindowlessEglApplication)
    elseif(CORRADE_TARGET_IOS)
        target_link_libraries(magnum-fontconverter MagnumWindowlessIosApplication)
    elseif(CORRADE_TARGET_APPLE)
        target_link_libraries(magnum-fontconverter MagnumWindowlessCglApplication)
    elseif(CORRADE_TARGET_UNIX)
        if(MAGNUM_TARGET_GLES AND NOT MAGNUM_TARGET_DESKTOP_GLES)
            target_link_libraries(magnum-fontconverter MagnumWindowlessEglApplication)
        else()
            target_link_libraries(magnum-fontconverter MagnumWindowlessGlxApplication)
        endif()
    elseif(CORRADE_TARGET_WINDOWS)
        if(MAGNUM_TARGET_GLES AND NOT MAGNUM_TARGET_DESKTOP_GLES)
            target_link_libraries(magnum-fontconverter MagnumWindowlessWindowsEglApplication)
        else()
            target_link_libraries(magnum-fontconverter MagnumWindowlessWglApplication)
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
    add_subdirectory(Test)
endif()

# Magnum Text target alias for superprojects
add_library(Magnum::Text ALIAS MagnumText)
