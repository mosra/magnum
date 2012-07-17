set(MagnumPhysics_SRCS
    AbstractShape.cpp
    AxisAlignedBox.cpp
    Capsule.cpp
    Line.cpp
    Plane.cpp
    ShapeGroup.cpp
    Sphere.cpp)

add_library(MagnumPhysics SHARED ${MagnumPhysics_SRCS})

target_link_libraries(MagnumPhysics Magnum)

install(TARGETS MagnumPhysics DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(Test)
endif()
