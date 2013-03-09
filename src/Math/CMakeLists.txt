set(MagnumMath_HEADERS
    Angle.h
    BoolVector.h
    Complex.h
    Constants.h
    Dual.h
    DualComplex.h
    DualQuaternion.h
    Functions.h
    Math.h
    MathTypeTraits.h
    Matrix.h
    Matrix3.h
    Matrix4.h
    Quaternion.h
    RectangularMatrix.h
    Swizzle.h
    Unit.h
    Vector.h
    Vector2.h
    Vector3.h
    Vector4.h)

install(FILES ${MagnumMath_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Math)

add_subdirectory(Algorithms)
add_subdirectory(Geometry)

if(BUILD_TESTS)
    add_subdirectory(Test)
endif()
