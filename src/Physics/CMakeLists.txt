set(MagnumPhysics_SRCS
    AbstractShape.cpp
    AxisAlignedBox.cpp
    Box.cpp
    Capsule.cpp
    Line.cpp
    Plane.cpp
    Point.cpp
    ShapeGroup.cpp
    Sphere.cpp)
set(MagnumPhysics_HEADERS
    AbstractShape.h
    AxisAlignedBox.h
    Box.h
    Capsule.h
    Line.h
    LineSegment.h
    Plane.h
    Point.h
    ShapeGroup.h
    Sphere.h

    magnumPhysicsVisibility.h)

add_library(MagnumPhysics SHARED ${MagnumPhysics_SRCS})

target_link_libraries(MagnumPhysics Magnum MagnumPrimitives MagnumSceneGraph MagnumShaders)

install(TARGETS MagnumPhysics DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumPhysics_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Physics)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(Test)
endif()
