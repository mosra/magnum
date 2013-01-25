set(MagnumPrimitives_SRCS
    Capsule.cpp
    Crosshair.cpp
    Cube.cpp
    Cylinder.cpp
    Icosphere.cpp
    Plane.cpp
    Square.cpp
    UVSphere.cpp)
set(MagnumPrimitives_HEADERS
    Capsule.h
    Crosshair.h
    Cube.h
    Cylinder.h
    Icosphere.h
    Plane.h
    Square.h
    UVSphere.h

    magnumPrimitivesVisibility.h)

add_library(MagnumPrimitives SHARED ${MagnumPrimitives_SRCS})
target_link_libraries(MagnumPrimitives Magnum)

install(TARGETS MagnumPrimitives DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumPrimitives_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Primitives)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(Test)
endif()
