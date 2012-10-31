corrade_add_resource(MagnumShaders_RCS MagnumShaders
    FlatShader2D.vert FlatShader2D.frag
    PhongShader.frag PhongShader.vert)
set(MagnumShaders_SRCS
    FlatShader.cpp
    PhongShader.cpp
    ${MagnumShaders_RCS})
set(MagnumShaders_HEADERS
    FlatShader.h
    PhongShader.h

    magnumShadersVisibility.h)

add_library(MagnumShaders SHARED ${MagnumShaders_SRCS})

target_link_libraries(MagnumShaders Magnum)

install(TARGETS MagnumShaders DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumShaders_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Shaders)
