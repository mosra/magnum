corrade_add_resource(MagnumShaders_RCS MagnumShaders
    FlatShader2D.vert FlatShader2D.frag
    PhongShader.vert PhongShader.frag
    VertexColorShader2D.vert VertexColorShader2D.frag
    compatibility.glsl)
set(MagnumShaders_SRCS
    FlatShader.cpp
    PhongShader.cpp
    VertexColorShader.cpp
    ${MagnumShaders_RCS})
set(MagnumShaders_HEADERS
    FlatShader.h
    PhongShader.h
    VertexColorShader.h

    magnumShadersVisibility.h)

add_library(MagnumShaders SHARED ${MagnumShaders_SRCS})

target_link_libraries(MagnumShaders Magnum)

install(TARGETS MagnumShaders DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumShaders_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Shaders)
