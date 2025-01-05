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
set(CMAKE_FOLDER "Magnum/SceneGraph/Test")

corrade_add_test(SceneGraphAnimableTest AnimableTest.cpp LIBRARIES MagnumSceneGraph)
corrade_add_test(SceneGraphCameraTest CameraTest.cpp LIBRARIES MagnumSceneGraph)
corrade_add_test(SceneGraphDualComplexTransfor___Test DualComplexTransformationTest.cpp LIBRARIES MagnumSceneGraphTestLib)
corrade_add_test(SceneGraphDualQuaternionTrans___Test DualQuaternionTransformationTest.cpp LIBRARIES MagnumSceneGraphTestLib)
corrade_add_test(SceneGraphMatrixTransformation2DTest MatrixTransformation2DTest.cpp LIBRARIES MagnumSceneGraph)
corrade_add_test(SceneGraphMatrixTransformation3DTest MatrixTransformation3DTest.cpp LIBRARIES MagnumSceneGraph)
corrade_add_test(SceneGraphObjectTest ObjectTest.cpp LIBRARIES MagnumSceneGraphTestLib)
corrade_add_test(SceneGraphRigidMatrixTransf___2DTest RigidMatrixTransformation2DTest.cpp LIBRARIES MagnumSceneGraphTestLib)
corrade_add_test(SceneGraphRigidMatrixTransf___3DTest RigidMatrixTransformation3DTest.cpp LIBRARIES MagnumSceneGraphTestLib)
corrade_add_test(SceneGraphSceneTest SceneTest.cpp LIBRARIES MagnumSceneGraph)
corrade_add_test(SceneGraphTranslationRotati___2DTest TranslationRotationScalingTransformation2DTest.cpp LIBRARIES MagnumSceneGraph)
corrade_add_test(SceneGraphTranslationRotati___3DTest TranslationRotationScalingTransformation3DTest.cpp LIBRARIES MagnumSceneGraph)
corrade_add_test(SceneGraphTranslationTransfor___Test TranslationTransformationTest.cpp LIBRARIES MagnumSceneGraph)

set_property(TARGET
    SceneGraphDualComplexTransfor___Test
    SceneGraphDualQuaternionTrans___Test
    SceneGraphObjectTest
    SceneGraphRigidMatrixTransf___2DTest
    SceneGraphRigidMatrixTransf___3DTest
    SceneGraphTranslationRotati___2DTest
    SceneGraphTranslationRotati___3DTest
    SceneGraphTranslationTransfor___Test
    APPEND PROPERTY COMPILE_DEFINITIONS "CORRADE_GRACEFUL_ASSERT")

