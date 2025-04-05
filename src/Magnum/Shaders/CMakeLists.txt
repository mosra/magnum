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
set(CMAKE_FOLDER "Magnum/Shaders")

set(MagnumShaders_SRCS )

set(MagnumShaders_GracefulAssert_SRCS
    Line.cpp)

set(MagnumShaders_HEADERS
    DistanceFieldVector.h
    Flat.h
    Generic.h
    Line.h
    MeshVisualizer.h
    Phong.h
    Shaders.h
    Vector.h

    visibility.h)

# Header files to display in project view of IDEs only
set(MagnumShaders_PRIVATE_HEADERS
    Implementation/lineMiterLimit.h)

if(MAGNUM_TARGET_GL)
    if(MAGNUM_TARGET_GLES2)
        corrade_add_resource(MagnumShaders_RESOURCES_GL resources-gles2.conf)
    else()
        corrade_add_resource(MagnumShaders_RESOURCES_GL resources-gl.conf)
    endif()

    list(APPEND MagnumShaders_SRCS
        ${MagnumShaders_RESOURCES_GL})

    list(APPEND MagnumShaders_GracefulAssert_SRCS
        DistanceFieldVectorGL.cpp
        FlatGL.cpp
        MeshVisualizerGL.cpp
        PhongGL.cpp
        VectorGL.cpp
        VertexColorGL.cpp

        glShaderWrapper.cpp)

    list(APPEND MagnumShaders_HEADERS
        DistanceFieldVectorGL.h
        FlatGL.h
        GenericGL.h
        MeshVisualizerGL.h
        PhongGL.h
        VectorGL.h
        VertexColorGL.h

        glShaderWrapper.h)

    if(NOT MAGNUM_TARGET_GLES2)
        list(APPEND MagnumShaders_GracefulAssert_SRCS
            LineGL.cpp)

        list(APPEND MagnumShaders_HEADERS
            LineGL.h)
    endif()

    if(MAGNUM_BUILD_DEPRECATED)
        list(APPEND MagnumShaders_HEADERS
            VertexColor.h)
    endif()
endif()

# Objects shared between main and test library. On non-GL builds contains
# nothing so it's not even defined.
if(MAGNUM_TARGET_GL)
    add_library(MagnumShadersObjects OBJECT
        ${MagnumShaders_SRCS}
        ${MagnumShaders_HEADERS}
        ${MagnumShaders_PRIVATE_HEADERS})
    if(MAGNUM_BUILD_STATIC)
        # On the static build we're importing the resources manually, so no need to
        # have the implicit initializers as well.
        set_property(SOURCE ${MagnumShaders_RCS} APPEND PROPERTY COMPILE_DEFINITIONS
            "CORRADE_AUTOMATIC_INITIALIZER=CORRADE_NOOP"
            "CORRADE_AUTOMATIC_FINALIZER=CORRADE_NOOP")
    endif()
    target_include_directories(MagnumShadersObjects PUBLIC
        $<TARGET_PROPERTY:Magnum,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:MagnumGL,INTERFACE_INCLUDE_DIRECTORIES>)
    if(NOT MAGNUM_BUILD_STATIC)
        target_compile_definitions(MagnumShadersObjects PRIVATE "MagnumShadersObjects_EXPORTS")
    endif()
    if(NOT MAGNUM_BUILD_STATIC OR MAGNUM_BUILD_STATIC_PIC)
        set_target_properties(MagnumShadersObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    set(MagnumShadersObjects_OBJECTS $<TARGET_OBJECTS:MagnumShadersObjects>)
else()
    set(MagnumShadersObjects_OBJECTS )
endif()

# Main Shaders library
add_library(MagnumShaders ${SHARED_OR_STATIC}
    ${MagnumShadersObjects_OBJECTS}
    ${MagnumShaders_GracefulAssert_SRCS})
set_target_properties(MagnumShaders PROPERTIES DEBUG_POSTFIX "-d")
if(NOT MAGNUM_BUILD_STATIC)
    set_target_properties(MagnumShaders PROPERTIES VERSION ${MAGNUM_LIBRARY_VERSION} SOVERSION ${MAGNUM_LIBRARY_SOVERSION})
elseif(MAGNUM_BUILD_STATIC_PIC)
    set_target_properties(MagnumShaders PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(MagnumShaders PUBLIC Magnum)
if(MAGNUM_TARGET_GL)
    target_link_libraries(MagnumShaders PUBLIC MagnumGL)
endif()

install(TARGETS MagnumShaders
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumShaders_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Shaders)

if(MAGNUM_BUILD_TESTS)
    # Library with graceful assert for testing
    add_library(MagnumShadersTestLib ${SHARED_OR_STATIC} ${EXCLUDE_FROM_ALL_IF_TEST_TARGET}
        ${MagnumShadersObjects_OBJECTS}
        ${MagnumShaders_GracefulAssert_SRCS})
    set_target_properties(MagnumShadersTestLib PROPERTIES DEBUG_POSTFIX "-d")
    target_compile_definitions(MagnumShadersTestLib PRIVATE
        "CORRADE_GRACEFUL_ASSERT" "MagnumShaders_EXPORTS")
    if(MAGNUM_BUILD_STATIC_PIC)
        set_target_properties(MagnumShadersTestLib PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    target_link_libraries(MagnumShadersTestLib PUBLIC Magnum)
    if(MAGNUM_TARGET_GL)
        target_link_libraries(MagnumShadersTestLib PUBLIC MagnumGL)
    endif()

    add_subdirectory(Test ${EXCLUDE_FROM_ALL_IF_TEST_TARGET})
endif()

# Magnum Shaders library for superprojects
add_library(Magnum::Shaders ALIAS MagnumShaders)
