set(MagnumPrimitives_SRCS
    Capsule.cpp
    Cube.cpp
    Icosphere.cpp
    UVSphere.cpp
)

add_library(MagnumPrimitives STATIC ${MagnumPrimitives_SRCS})
target_link_libraries(MagnumPrimitives Magnum)

install(TARGETS MagnumPrimitives DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(Test)
endif()
