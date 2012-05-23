set(MagnumPhysics_SRCS
    AbstractShape.cpp
    AxisAlignedBox.cpp
    Capsule.cpp
    Line.cpp
    Plane.cpp
    ShapeGroup.cpp
    Sphere.cpp
)

add_library(MagnumPhysics SHARED ${MagnumPhysics_SRCS})
set_target_properties(MagnumPhysics PROPERTIES COMPILE_FLAGS ${CMAKE_SHARED_LIBRARY_CXX_FLAGS})

target_link_libraries(MagnumPhysics Magnum)

install(TARGETS MagnumPhysics DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(Test)
endif()
