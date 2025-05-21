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
set(CMAKE_FOLDER "Magnum/Primitives")

set(MagnumPrimitives_SRCS
    Axis.cpp
    Crosshair.cpp
    Cube.cpp
    Gradient.cpp
    Grid.cpp
    Icosphere.cpp
    Line.cpp
    Plane.cpp
    Square.cpp

    Implementation/Spheroid.cpp
    Implementation/WireframeSpheroid.cpp)

set(MagnumPrimitives_GracefulAssert_SRCS
    Capsule.cpp
    Circle.cpp
    Cone.cpp
    Cylinder.cpp
    UVSphere.cpp)

set(MagnumPrimitives_HEADERS
    Axis.h
    Capsule.h
    Circle.h
    Crosshair.h
    Cone.h
    Cube.h
    Cylinder.h
    Gradient.h
    Grid.h
    Icosphere.h
    Line.h
    Plane.h
    Square.h
    UVSphere.h

    visibility.h)

# Header files to display in project view of IDEs only
set(MagnumPrimitives_PRIVATE_HEADERS
    Implementation/Spheroid.h
    Implementation/WireframeSpheroid.h)

# Objects shared between main and test library
add_library(MagnumPrimitivesObjects OBJECT
    ${MagnumPrimitives_SRCS}
    ${MagnumPrimitives_HEADERS}
    ${MagnumPrimitives_PRIVATE_HEADERS})
target_include_directories(MagnumPrimitivesObjects PUBLIC $<TARGET_PROPERTY:Magnum,INTERFACE_INCLUDE_DIRECTORIES>)
if(NOT MAGNUM_BUILD_STATIC)
    target_compile_definitions(MagnumPrimitivesObjects PRIVATE "MagnumPrimitivesObjects_EXPORTS")
endif()
if(NOT MAGNUM_BUILD_STATIC OR MAGNUM_BUILD_STATIC_PIC)
    set_target_properties(MagnumPrimitivesObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Primitives library
add_library(MagnumPrimitives ${SHARED_OR_STATIC}
    $<TARGET_OBJECTS:MagnumPrimitivesObjects>
    ${MagnumPrimitives_GracefulAssert_SRCS})
set_target_properties(MagnumPrimitives PROPERTIES
    DEBUG_POSTFIX "-d")
if(NOT MAGNUM_BUILD_STATIC)
    set_target_properties(MagnumPrimitives PROPERTIES VERSION ${MAGNUM_LIBRARY_VERSION} SOVERSION ${MAGNUM_LIBRARY_SOVERSION})
elseif(MAGNUM_BUILD_STATIC_PIC)
    set_target_properties(MagnumPrimitives PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(MagnumPrimitives PUBLIC
    Magnum
    MagnumMeshTools
    MagnumTrade)

install(TARGETS MagnumPrimitives
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumPrimitives_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Primitives)

if(MAGNUM_BUILD_TESTS)
    # Library with graceful assert for testing
    add_library(MagnumPrimitivesTestLib ${SHARED_OR_STATIC} ${EXCLUDE_FROM_ALL_IF_TEST_TARGET}
        $<TARGET_OBJECTS:MagnumPrimitivesObjects>
        ${MagnumPrimitives_GracefulAssert_SRCS})
    set_target_properties(MagnumPrimitivesTestLib PROPERTIES DEBUG_POSTFIX "-d")
    target_compile_definitions(MagnumPrimitivesTestLib PRIVATE
        "CORRADE_GRACEFUL_ASSERT" "MagnumPrimitives_EXPORTS")
    if(MAGNUM_BUILD_STATIC_PIC)
        set_target_properties(MagnumPrimitivesTestLib PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    target_link_libraries(MagnumPrimitivesTestLib PUBLIC
        Magnum
        MagnumMeshTools
        MagnumTrade)

    add_subdirectory(Test ${EXCLUDE_FROM_ALL_IF_TEST_TARGET})
endif()

# Magnum Primitives target alias for superprojects
add_library(Magnum::Primitives ALIAS MagnumPrimitives)
