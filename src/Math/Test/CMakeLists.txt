corrade_add_test(MathBoolVectorTest BoolVectorTest.cpp)
corrade_add_test(MathConstantsTest ConstantsTest.cpp)
corrade_add_test(MathFunctionsTest FunctionsTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathMathTypeTraitsTest MathTypeTraitsTest.cpp)

corrade_add_test(MathVectorTest VectorTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathVector2Test Vector2Test.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathVector3Test Vector3Test.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathVector4Test Vector4Test.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathPoint2DTest Point2DTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathPoint3DTest Point3DTest.cpp LIBRARIES MagnumMathTestLib)

corrade_add_test(MathRectangularMatrixTest RectangularMatrixTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathMatrixTest MatrixTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathMatrix3Test Matrix3Test.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathMatrix4Test Matrix4Test.cpp LIBRARIES MagnumMathTestLib)

corrade_add_test(MathSwizzleTest SwizzleTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathUnitTest UnitTest.cpp)

corrade_add_test(MathComplexTest ComplexTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathDualTest DualTest.cpp)
corrade_add_test(MathQuaternionTest QuaternionTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(MathDualQuaternionTest DualQuaternionTest.cpp LIBRARIES MagnumMathTestLib)

set_target_properties(
    MathVectorTest
    MathMatrix3Test
    MathMatrix4Test
    MathDualQuaternionTest
    MathQuaternionTest
    PROPERTIES COMPILE_FLAGS -DCORRADE_GRACEFUL_ASSERT)
