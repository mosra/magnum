corrade_add_resource(MagnumShaders_RCS MagnumShaders PhongShader.frag PhongShader.vert)
set(MagnumShaders_SRCS
    PhongShader.cpp
    ${MagnumShaders_RCS})

add_library(MagnumShaders SHARED ${MagnumShaders_SRCS})

target_link_libraries(MagnumShaders Magnum)

install(TARGETS MagnumShaders DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
