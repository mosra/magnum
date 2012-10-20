include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(MagnumMath_HEADERS
    Constants.h
    Math.h
    MathTypeTraits.h
    Matrix.h
    Matrix3.h
    Matrix4.h
    Point2D.h
    Point3D.h
    RectangularMatrix.h
    Vector.h
    Vector2.h
    Vector3.h
    Vector4.h)
install(FILES ${MagnumMath_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Math)

add_subdirectory(Algorithms)
add_subdirectory(Geometry)
add_subdirectory(VectorFields)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(Test)
endif()
