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

corrade_add_test(MathBoolVectorTest BoolVectorTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathConstantsTest ConstantsTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathFunctionsTest FunctionsTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathFunctionsBatchTest FunctionsBatchTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathHalfTest HalfTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathPackingTest PackingTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathPackingBatchTest PackingBatchTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathTagsTest TagsTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathTypeTraitsTest TypeTraitsTest.cpp LIBRARIES MagnumMathTestLib)

corrade_add_test(MathVectorTest VectorTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathVector2Test Vector2Test.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathVector3Test Vector3Test.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathVector4Test Vector4Test.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathColorTest ColorTest.cpp LIBRARIES MagnumMathTestLib)

corrade_add_test(MathRectangularMatrixTest RectangularMatrixTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathMatrixTest MatrixTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathMatrix3Test Matrix3Test.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathMatrix4Test Matrix4Test.cpp LIBRARIES MagnumMathTestLib)

corrade_add_test(MathSwizzleTest SwizzleTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathUnitTest UnitTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathAngleTest AngleTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathRangeTest RangeTest.cpp LIBRARIES MagnumMathTestLib)

corrade_add_test(MathDualTest DualTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathComplexTest ComplexTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathDualComplexTest DualComplexTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathQuaternionTest QuaternionTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathDualQuaternionTest DualQuaternionTest.cpp LIBRARIES MagnumMathTestLib)

corrade_add_test(MathBezierTest BezierTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathCubicHermiteTest CubicHermiteTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathFrustumTest FrustumTest.cpp LIBRARIES MagnumMathTestLib)

corrade_add_test(MathDistanceTest DistanceTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathIntersectionTest IntersectionTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathIntersectionBenchmark IntersectionBenchmark.cpp LIBRARIES MagnumMathTestLib)

corrade_add_test(MathInterpolationBenchmark InterpolationBenchmark.cpp LIBRARIES MagnumMathTestLib)

corrade_add_test(MathConfigurationValueTest ConfigurationValueTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathStrictWeakOrderingTest StrictWeakOrderingTest.cpp LIBRARIES MagnumMathTestLib)

corrade_add_test(MathVectorBenchmark VectorBenchmark.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathMatrixBenchmark MatrixBenchmark.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathFunctionsBenchmark FunctionsBenchmark.cpp LIBRARIES MagnumMathTestLib)

set_property(TARGET
    MathVectorTest
    MathMatrixTest
    MathMatrix3Test
    MathMatrix4Test
    MathComplexTest
    MathCubicHermiteTest
    MathDualComplexTest
    MathFrustumTest
    MathFunctionsTest
    MathQuaternionTest
    MathDualQuaternionTest

    MathDistanceTest
    MathIntersectionTest
    APPEND PROPERTY COMPILE_DEFINITIONS "CORRADE_GRACEFUL_ASSERT")

set_target_properties(
    MathBoolVectorTest
    MathConstantsTest
    MathFunctionsTest
    MathFunctionsBatchTest
    MathHalfTest
    MathPackingTest
    MathPackingBatchTest
    MathTagsTest
    MathTypeTraitsTest

    MathVectorTest
    MathVector2Test
    MathVector3Test
    MathVector4Test
    MathColorTest

    MathRectangularMatrixTest
    MathMatrixTest
    MathMatrix3Test
    MathMatrix4Test

    MathSwizzleTest
    MathUnitTest
    MathAngleTest
    MathRangeTest

    MathDualTest
    MathComplexTest
    MathDualComplexTest
    MathQuaternionTest
    MathDualQuaternionTest

    MathBezierTest
    MathCubicHermiteTest
    MathFrustumTest

    MathDistanceTest
    MathIntersectionTest
    MathIntersectionBenchmark

    MathConfigurationValueTest
    MathStrictWeakOrderingTest

    MathVectorBenchmark
    MathMatrixBenchmark
    MathFunctionsBenchmark
    PROPERTIES FOLDER "Magnum/Math/Test")
