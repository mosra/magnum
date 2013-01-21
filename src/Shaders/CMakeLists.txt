corrade_add_resource(MagnumShaders_RCS MagnumShaders
    FlatShader2D.vert FlatShader3D.vert FlatShader.frag
    PhongShader.vert PhongShader.frag
    TextShader2D.vert TextShader3D.vert TextShader.frag
    VertexColorShader2D.vert VertexColorShader3D.vert VertexColorShader.frag
    compatibility.glsl)
set(MagnumShaders_SRCS
    FlatShader.cpp
    PhongShader.cpp
    TextShader.cpp
    VertexColorShader.cpp
    ${MagnumShaders_RCS})
set(MagnumShaders_HEADERS
    AbstractTextShader.h
    FlatShader.h
    PhongShader.h
    Shaders.h
    TextShader.h
    VertexColorShader.h

    magnumShadersVisibility.h)

add_library(MagnumShaders SHARED ${MagnumShaders_SRCS})

target_link_libraries(MagnumShaders Magnum)

install(TARGETS MagnumShaders DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumShaders_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Shaders)
