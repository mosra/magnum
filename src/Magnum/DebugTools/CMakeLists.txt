#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>
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
    ForceRenderer.cpp
    ObjectRenderer.cpp
    Profiler.cpp
    ResourceManager.cpp
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

set(MagnumDebugTools_HEADERS
    ForceRenderer.h
    DebugTools.h
    ObjectRenderer.h
    Profiler.h
    ResourceManager.h
    ShapeRenderer.h

    magnumDebugToolsVisibility.h)

add_library(MagnumDebugTools ${SHARED_OR_STATIC} ${MagnumDebugTools_SRCS})
if(BUILD_STATIC_PIC)
    # TODO: CMake 2.8.9 has this as POSITION_INDEPENDENT_CODE property
    set_target_properties(MagnumDebugTools PROPERTIES COMPILE_FLAGS "${CMAKE_SHARED_LIBRARY_CXX_FLAGS}")
endif()
target_link_libraries(MagnumDebugTools
    Magnum
    MagnumMeshTools
    MagnumPrimitives
    MagnumSceneGraph
    MagnumShaders
    MagnumShapes)

install(TARGETS MagnumDebugTools
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumDebugTools_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/DebugTools)

if(BUILD_TESTS)
    add_subdirectory(Test)
endif()
