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

set(MagnumDebugTools_SRCS
    Profiler.cpp
    ResourceManager.cpp
    TextureImage.cpp)

set(MagnumDebugTools_HEADERS
    DebugTools.h
    Profiler.h
    ResourceManager.h
    TextureImage.h
    visibility.h)

# Header files to display in project view of IDEs only
set(MagnumDebugTools_PRIVATE_HEADERS )

if(MAGNUM_TARGET_GLES AND NOT MAGNUM_TARGET_GLES2)
    corrade_add_resource(MagnumDebugTools_RESOURCES resources.conf)
    list(APPEND MagnumDebugTools_SRCS ${MagnumDebugTools_RESOURCES})
endif()

# Build the TestSuite-related functionality only if it is present
find_package(Corrade COMPONENTS TestSuite)
if(Corrade_TestSuite_FOUND)
    list(APPEND MagnumDebugTools_SRCS
        CompareImage.cpp)

    list(APPEND MagnumDebugTools_HEADERS
        CompareImage.h)
endif()

if(NOT MAGNUM_TARGET_WEBGL)
    list(APPEND MagnumDebugTools_SRCS
        BufferData.cpp)

    list(APPEND MagnumDebugTools_HEADERS
        BufferData.h)
endif()

if(WITH_SCENEGRAPH)
    list(APPEND MagnumDebugTools_SRCS
        ForceRenderer.cpp
        ObjectRenderer.cpp)

    list(APPEND MagnumDebugTools_HEADERS
        ForceRenderer.h
        ObjectRenderer.h)

    list(APPEND MagnumDebugTools_PRIVATE_HEADERS
        Implementation/ForceRendererTransformation.h)
endif()

if(WITH_SHAPES)
    list(APPEND MagnumDebugTools_SRCS
        ShapeRenderer.cpp

        Implementation/AbstractBoxRenderer.cpp
        Implementation/AbstractShapeRenderer.cpp
        Implementation/AxisAlignedBoxRenderer.cpp
        Implementation/BoxRenderer.cpp
        Implementation/CapsuleRenderer.cpp
        Implementation/CylinderRenderer.cpp
        Implementation/LineSegmentRenderer.cpp
        Implementation/PointRenderer.cpp
        Implementation/SphereRenderer.cpp)

    list(APPEND MagnumDebugTools_HEADERS
        ShapeRenderer.h)

    list(APPEND MagnumDebugTools_PRIVATE_HEADERS
        Implementation/AbstractBoxRenderer.h
        Implementation/AbstractShapeRenderer.h
        Implementation/AxisAlignedBoxRenderer.h
        Implementation/BoxRenderer.h
        Implementation/CapsuleRenderer.h
        Implementation/CapsuleRendererTransformation.h
        Implementation/CylinderRenderer.h
        Implementation/CylinderRendererTransformation.h
        Implementation/LineSegmentRenderer.h
        Implementation/LineSegmentRendererTransformation.h
        Implementation/PointRenderer.h
        Implementation/SphereRenderer.h)
endif()

# DebugTools library
add_library(MagnumDebugTools ${SHARED_OR_STATIC}
    ${MagnumDebugTools_SRCS}
    ${MagnumDebugTools_HEADERS}
    ${MagnumDebugTools_PRIVATE_HEADERS})
set_target_properties(MagnumDebugTools PROPERTIES
    DEBUG_POSTFIX "-d"
    FOLDER "Magnum/DebugTools")
if(BUILD_STATIC_PIC)
    set_target_properties(MagnumDebugTools PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(MagnumDebugTools Magnum)
if(Corrade_TestSuite_FOUND)
    target_link_libraries(MagnumDebugTools Corrade::TestSuite)
endif()
if(WITH_SCENEGRAPH)
    target_link_libraries(MagnumDebugTools MagnumSceneGraph)
endif()
if(WITH_SHAPES)
    target_link_libraries(MagnumDebugTools MagnumShapes MagnumPrimitives)
endif()
if(WITH_SCENEGRAPH OR WITH_SHAPES)
    target_link_libraries(MagnumDebugTools
        MagnumMeshTools
        MagnumShaders)
endif()

install(TARGETS MagnumDebugTools
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumDebugTools_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/DebugTools)

if(BUILD_TESTS)
    add_subdirectory(Test)
endif()

# Magnum DebugTools target alias for superprojects
add_library(Magnum::DebugTools ALIAS MagnumDebugTools)
