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

set(MagnumTextureTools_SRCS
    Atlas.cpp)

set(MagnumTextureTools_HEADERS
    Atlas.h

    visibility.h)

if(TARGET_GL)
    corrade_add_resource(MagnumTextureTools_RCS resources.conf)
    set_target_properties(MagnumTextureTools_RCS-dependencies PROPERTIES FOLDER "Magnum/TextureTools")
    if(BUILD_STATIC)
        # On the static build we're importing the resources manually, so no
        # need to have the implicit initializers as well.
        set_property(SOURCE ${MagnumTextureTools_RCS} APPEND PROPERTY
            COMPILE_DEFINITIONS
                "CORRADE_AUTOMATIC_INITIALIZER=CORRADE_NOOP"
                "CORRADE_AUTOMATIC_FINALIZER=CORRADE_NOOP")
    endif()

    list(APPEND MagnumTextureTools_SRCS
        DistanceField.cpp
        ${MagnumTextureTools_RCS})

    list(APPEND MagnumTextureTools_HEADERS DistanceField.h)
endif()

# TextureTools library
add_library(MagnumTextureTools ${SHARED_OR_STATIC}
    ${MagnumTextureTools_SRCS}
    ${MagnumTextureTools_HEADERS})
set_target_properties(MagnumTextureTools PROPERTIES
    DEBUG_POSTFIX "-d"
    FOLDER "Magnum/TextureTools")
if(NOT BUILD_STATIC)
    set_target_properties(MagnumTextureTools PROPERTIES VERSION ${MAGNUM_LIBRARY_VERSION} SOVERSION ${MAGNUM_LIBRARY_SOVERSION})
elseif(BUILD_STATIC_PIC)
    set_target_properties(MagnumTextureTools PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(MagnumTextureTools PUBLIC
    Magnum)
if(WITH_GL)
    target_link_libraries(MagnumTextureTools PUBLIC MagnumGL)
endif()

install(TARGETS MagnumTextureTools
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumTextureTools_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/TextureTools)

if(WITH_DISTANCEFIELDCONVERTER)
    if(NOT TARGET_GL)
        message(SEND_ERROR "magnum-distancefieldconverter is available only if TARGET_GL is enabled")
    endif()

    add_executable(magnum-distancefieldconverter distancefieldconverter.cpp)
    target_link_libraries(magnum-distancefieldconverter PRIVATE
        Magnum
        MagnumTextureTools
        MagnumTrade)
    if(MAGNUM_TARGET_HEADLESS)
        target_link_libraries(magnum-distancefieldconverter PRIVATE MagnumWindowlessEglApplication)
    elseif(CORRADE_TARGET_IOS)
        target_link_libraries(magnum-distancefieldconverter PRIVATE MagnumWindowlessIosApplication)
    elseif(CORRADE_TARGET_APPLE)
        target_link_libraries(magnum-distancefieldconverter PRIVATE MagnumWindowlessCglApplication)
    elseif(CORRADE_TARGET_UNIX)
        if(MAGNUM_TARGET_GLES AND NOT MAGNUM_TARGET_DESKTOP_GLES)
            target_link_libraries(magnum-distancefieldconverter PRIVATE MagnumWindowlessEglApplication)
        else()
            target_link_libraries(magnum-distancefieldconverter PRIVATE MagnumWindowlessGlxApplication)
        endif()
    elseif(CORRADE_TARGET_WINDOWS)
        if(MAGNUM_TARGET_GLES AND NOT MAGNUM_TARGET_DESKTOP_GLES)
            target_link_libraries(magnum-distancefieldconverter PRIVATE MagnumWindowlessWindowsEglApplication)
        else()
            target_link_libraries(magnum-distancefieldconverter PRIVATE MagnumWindowlessWglApplication)
        endif()
    else()
        message(FATAL_ERROR "magnum-distancefieldconverter is not available on this platform. Set WITH_DISTANCEFIELDCONVERTER to OFF to suppress this warning.")
    endif()
    set_target_properties(magnum-distancefieldconverter PROPERTIES FOLDER "Magnum/TextureTools")

    install(TARGETS magnum-distancefieldconverter DESTINATION ${MAGNUM_BINARY_INSTALL_DIR})

    # Magnum distancefieldconverter target alias for superprojects
    add_executable(Magnum::distancefieldconverter ALIAS magnum-distancefieldconverter)
endif()

if(BUILD_TESTS)
    add_subdirectory(Test)
endif()

# Magnum TextureTools target alias for superprojects
add_library(Magnum::TextureTools ALIAS MagnumTextureTools)
