corrade_add_test(MathTypeTraitsTest TypeTraitsTest.h TypeTraitsTest.cpp)

corrade_add_test(MathVectorTest VectorTest.h VectorTest.cpp ${CORRADE_UTILITY_LIBRARY})
corrade_add_test(MathVector2Test Vector2Test.h Vector2Test.cpp ${CORRADE_UTILITY_LIBRARY})
corrade_add_test(MathVector3Test Vector3Test.h Vector3Test.cpp ${CORRADE_UTILITY_LIBRARY})
corrade_add_test(MathVector4Test Vector4Test.h Vector4Test.cpp ${CORRADE_UTILITY_LIBRARY})

corrade_add_test(MathMatrixTest MatrixTest.h MatrixTest.cpp ${CORRADE_UTILITY_LIBRARY})
corrade_add_test(MathMatrix3Test Matrix3Test.h Matrix3Test.cpp ${CORRADE_UTILITY_LIBRARY})
corrade_add_test(MathMatrix4Test Matrix4Test.h Matrix4Test.cpp ${CORRADE_UTILITY_LIBRARY})

corrade_add_test(MathTest MathTest.h MathTest.cpp MagnumMath)
corrade_add_test(MathGeometryUtilsTest GeometryUtilsTest.h GeometryUtilsTest.cpp)
