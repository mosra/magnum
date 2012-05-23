corrade_add_resource(Shaders shaders PhongShader.frag PhongShader.vert)
set(Shaders_SRCS
    PhongShader.cpp
    ${Shaders}
)

add_library(MagnumShaders SHARED ${Shaders_SRCS})
set_target_properties(MagnumShaders PROPERTIES COMPILE_FLAGS ${CMAKE_SHARED_LIBRARY_CXX_FLAGS})
target_link_libraries(MagnumShaders Magnum)

install(TARGETS MagnumShaders DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
