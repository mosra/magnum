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

if(NOT WITH_GL)
    message(SEND_ERROR "Shaders are available only if WITH_GL is enabled")
endif()

corrade_add_resource(MagnumShaders_RCS resources.conf)
set_target_properties(MagnumShaders_RCS-dependencies PROPERTIES FOLDER "Magnum/Shaders")

set(MagnumShaders_SRCS
    AbstractVector.cpp
    VertexColor.cpp

    ${MagnumShaders_RCS})

set(MagnumShaders_GracefulAssert_SRCS
    DistanceFieldVector.cpp
    Flat.cpp
    MeshVisualizer.cpp
    Phong.cpp
    Vector.cpp)

set(MagnumShaders_HEADERS
    DistanceFieldVector.h
    AbstractVector.h
    Flat.h
    Generic.h
    MeshVisualizer.h
    Phong.h
    Shaders.h
    Vector.h
    VertexColor.h

    visibility.h)

# Header files to display in project view of IDEs only
set(MagnumShaders_PRIVATE_HEADERS Implementation/CreateCompatibilityShader.h)

# Objects shared between main and test library
add_library(MagnumShadersObjects OBJECT
    ${MagnumShaders_SRCS}
    ${MagnumShaders_HEADERS})
if(BUILD_STATIC)
    # On the static build we're importing the resources manually, so no need to
    # have the implicit initializers as well.
    set_property(SOURCE ${MagnumShaders_RCS} APPEND PROPERTY COMPILE_DEFINITIONS
        "CORRADE_AUTOMATIC_INITIALIZER=CORRADE_NOOP"
        "CORRADE_AUTOMATIC_FINALIZER=CORRADE_NOOP")
endif()
target_include_directories(MagnumShadersObjects PUBLIC
    $<TARGET_PROPERTY:Magnum,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:MagnumGL,INTERFACE_INCLUDE_DIRECTORIES>)
if(NOT BUILD_STATIC)
    target_compile_definitions(MagnumShadersObjects PRIVATE "MagnumShadersObjects_EXPORTS")
endif()
if(NOT BUILD_STATIC OR BUILD_STATIC_PIC)
    set_target_properties(MagnumShadersObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
set_target_properties(MagnumShadersObjects PROPERTIES FOLDER "Magnum/Shaders")

# Main Shaders library
add_library(MagnumShaders ${SHARED_OR_STATIC}
    $<TARGET_OBJECTS:MagnumShadersObjects>
    ${MagnumShaders_GracefulAssert_SRCS})
set_target_properties(MagnumShaders PROPERTIES
    DEBUG_POSTFIX "-d"
    FOLDER "Magnum/Shaders")
if(NOT BUILD_STATIC)
    set_target_properties(MagnumShaders PROPERTIES VERSION ${MAGNUM_LIBRARY_VERSION} SOVERSION ${MAGNUM_LIBRARY_SOVERSION})
elseif(BUILD_STATIC_PIC)
    set_target_properties(MagnumShaders PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(MagnumShaders PUBLIC
    Magnum
    MagnumGL)

install(TARGETS MagnumShaders
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumShaders_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Shaders)

if(BUILD_TESTS)
    # Library with graceful assert for testing
    add_library(MagnumShadersTestLib ${SHARED_OR_STATIC}
        $<TARGET_OBJECTS:MagnumShadersObjects>
        ${MagnumShaders_GracefulAssert_SRCS})
    set_target_properties(MagnumShadersTestLib PROPERTIES
        DEBUG_POSTFIX "-d"
        FOLDER "Magnum/Shaders")
    target_compile_definitions(MagnumShadersTestLib PRIVATE
        "CORRADE_GRACEFUL_ASSERT" "MagnumShaders_EXPORTS")
    if(BUILD_STATIC_PIC)
        set_target_properties(MagnumShadersTestLib PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    target_link_libraries(MagnumShadersTestLib PUBLIC
        Magnum
        MagnumGL)

    add_subdirectory(Test)
endif()

# Magnum Shaders library for superprojects
add_library(Magnum::Shaders ALIAS MagnumShaders)
