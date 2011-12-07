set(MagnumPrimitives_SRCS
    Cube.cpp
    Icosphere.cpp
)

add_library(MagnumPrimitives STATIC ${MagnumPrimitives_SRCS})
target_link_libraries(MagnumPrimitives Magnum)

install(TARGETS MagnumPrimitives DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
