corrade_add_resource(MagnumPhysics_RCS MagnumPhysics
    Implementation/ShapeShader2D.vert ALIAS ShapeShader2D.vert
    Implementation/ShapeShader2D.frag ALIAS ShapeShader2D.frag)

set(MagnumPhysics_SRCS
    AbstractShape.cpp
    AxisAlignedBox.cpp
    Box.cpp
    Capsule.cpp
    DebugDrawResourceManager.cpp
    Line.cpp
    Plane.cpp
    Point.cpp
    ShapedObject.cpp
    ShapedObjectGroup.cpp
    ShapeGroup.cpp
    Sphere.cpp

    Implementation/AbstractDebugRenderer.cpp
    Implementation/BoxRenderer.cpp
    Implementation/ShapeShader.cpp
    ${MagnumPhysics_RCS})
set(MagnumPhysics_HEADERS
    AbstractShape.h
    AxisAlignedBox.h
    Box.h
    Capsule.h
    DebugDrawResourceManager.h
    Line.h
    LineSegment.h
    Plane.h
    Point.h
    ShapedObject.h
    ShapedObjectGroup.h
    ShapeGroup.h
    Sphere.h

    magnumPhysicsVisibility.h)

add_library(MagnumPhysics SHARED ${MagnumPhysics_SRCS})

target_link_libraries(MagnumPhysics Magnum MagnumPrimitives MagnumSceneGraph)

install(TARGETS MagnumPhysics DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumPhysics_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Physics)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(Test)
endif()
