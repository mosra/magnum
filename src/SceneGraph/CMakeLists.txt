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

# Files shared between main library and unit test library
set(MagnumSceneGraph_SRCS
    Animable.cpp)

# Files compiled with different flags for main library and unit test library
set(MagnumSceneGraph_GracefulAssert_SRCS
    instantiation.cpp)

set(MagnumSceneGraph_HEADERS
    AbstractCamera.h
    AbstractCamera.hpp
    AbstractFeature.h
    AbstractFeature.hpp
    AbstractGroupedFeature.h
    AbstractObject.h
    AbstractTransformation.h
    AbstractTranslationRotation2D.h
    AbstractTranslationRotation3D.h
    AbstractTranslationRotationScaling2D.h
    AbstractTranslationRotationScaling3D.h
    Animable.h
    Animable.hpp
    AnimableGroup.h
    Camera2D.h
    Camera2D.hpp
    Camera3D.h
    Camera3D.hpp
    Drawable.h
    DualComplexTransformation.h
    DualQuaternionTransformation.h
    RigidMatrixTransformation2D.h
    RigidMatrixTransformation3D.h
    FeatureGroup.h
    FeatureGroup.hpp
    MatrixTransformation2D.h
    MatrixTransformation3D.h
    Object.h
    Object.hpp
    Scene.h
    SceneGraph.h

    magnumSceneGraphVisibility.h)

# Set shared library flags for the objects, as they will be part of shared lib
# TODO: fix when CMake sets target_EXPORTS for OBJECT targets as well
add_library(MagnumSceneGraphObjects OBJECT ${MagnumSceneGraph_SRCS})
if(NOT BUILD_STATIC OR BUILD_STATIC_PIC)
    # TODO: CMake 2.8.9 has this as POSITION_INDEPENDENT_CODE property
    set_target_properties(MagnumSceneGraphObjects PROPERTIES COMPILE_FLAGS "-DMagnumSceneGraphObjects_EXPORTS ${CMAKE_SHARED_LIBRARY_CXX_FLAGS}")
else()
    set_target_properties(MagnumSceneGraphObjects PROPERTIES COMPILE_FLAGS "-DMagnumSceneGraphObjects_EXPORTS")
endif()

# SceneGraph library
add_library(MagnumSceneGraph ${SHARED_OR_STATIC}
    $<TARGET_OBJECTS:MagnumSceneGraphObjects>
    ${MagnumSceneGraph_GracefulAssert_SRCS})
target_link_libraries(MagnumSceneGraph Magnum)

install(TARGETS MagnumSceneGraph DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumSceneGraph_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/SceneGraph)

if(BUILD_TESTS)
    # Library with graceful assert for testing
    add_library(MagnumSceneGraphTestLib ${SHARED_OR_STATIC}
        $<TARGET_OBJECTS:MagnumSceneGraphObjects>
        ${MagnumSceneGraph_GracefulAssert_SRCS})
    set_target_properties(MagnumSceneGraphTestLib PROPERTIES COMPILE_FLAGS "-DCORRADE_GRACEFUL_ASSERT -DMagnumSceneGraph_EXPORTS")
    target_link_libraries(MagnumSceneGraphTestLib MagnumMathTestLib)

    # On Windows we need to install first and then run the tests to avoid "DLL
    # not found" hell, thus we need to install this too
    if(WIN32 AND NOT CMAKE_CROSSCOMPILING)
        install(TARGETS MagnumSceneGraphTestLib DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
    endif()

    add_subdirectory(Test)
endif()
