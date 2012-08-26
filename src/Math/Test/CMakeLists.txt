corrade_add_test2(MathMathTypeTraitsTest MathTypeTraitsTest.cpp)

corrade_add_test2(MathRectangularMatrixTest RectangularMatrixTest.cpp)

corrade_add_test2(MathVectorTest VectorTest.cpp)
set_target_properties(MathVectorTest PROPERTIES COMPILE_FLAGS -DCORRADE_GRACEFUL_ASSERT)
corrade_add_test2(MathVector2Test Vector2Test.cpp)
corrade_add_test2(MathVector3Test Vector3Test.cpp)
corrade_add_test2(MathVector4Test Vector4Test.cpp)

corrade_add_test2(MathMatrixTest MatrixTest.cpp)
corrade_add_test2(MathMatrix3Test Matrix3Test.cpp)
corrade_add_test2(MathMatrix4Test Matrix4Test.cpp)

if(NOT CMAKE_NO_OBJECT_TARGET)
    set(MathTest_SRCS MathTest.cpp $<TARGET_OBJECTS:MagnumMathObjects>)
else()
    set(MathTest_SRCS MathTest.cpp ${MagnumMath_SRCS})
endif()
corrade_add_test2(MathTest ${MathTest_SRCS})
