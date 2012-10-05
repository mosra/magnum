set(MagnumPrimitives_SRCS
    Capsule.cpp
    Cube.cpp
    Cylinder.cpp
    Icosphere.cpp
    Plane.cpp
    Square.cpp
    UVSphere.cpp)
set(MagnumPrimitives_HEADERS
    Capsule.h
    Cube.h
    Cylinder.h
    Icosphere.h
    Plane.h
    Square.h
    UVSphere.h)

add_library(MagnumPrimitives STATIC ${MagnumPrimitives_SRCS})
set_target_properties(MagnumPrimitives PROPERTIES COMPILE_FLAGS "${CMAKE_SHARED_LIBRARY_CXX_FLAGS}")
target_link_libraries(MagnumPrimitives Magnum)

install(TARGETS MagnumPrimitives DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumPrimitives_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Primitives)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(Test)
endif()
