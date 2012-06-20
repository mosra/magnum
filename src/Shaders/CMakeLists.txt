corrade_add_resource(Shaders MagnumShaders PhongShader.frag PhongShader.vert)
set(Shaders_SRCS
    PhongShader.cpp
    ${Shaders}
)

add_library(MagnumShaders SHARED ${Shaders_SRCS})

target_link_libraries(MagnumShaders Magnum)

install(TARGETS MagnumShaders DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
