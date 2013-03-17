#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>
#
#   Permission is hereby granted, free of charge, to any person obtaining a
#   copy of this software and associated documentation files (the "Software"),
#   to deal in the Software without restriction, including without limitation
#   the rights to use, copy, modify, merge, publish, distribute, sublicense,
#   and/or sell copies of the Software, and to permit persons to whom the
#   Software is furnished to do so, subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included
#   in all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#   DEALINGS IN THE SOFTWARE.
#

corrade_add_resource(MagnumShaders_RCS MagnumShaders
    AbstractVectorShader2D.vert AbstractVectorShader3D.vert
    FlatShader2D.vert FlatShader3D.vert FlatShader.frag
    PhongShader.vert PhongShader.frag
    VectorShader.frag DistanceFieldVectorShader.frag
    VertexColorShader2D.vert VertexColorShader3D.vert VertexColorShader.frag
    compatibility.glsl)

set(MagnumShaders_SRCS
    DistanceFieldVectorShader.cpp
    FlatShader.cpp
    PhongShader.cpp
    VectorShader.cpp
    VertexColorShader.cpp
    ${MagnumShaders_RCS})

set(MagnumShaders_HEADERS
    DistanceFieldVectorShader.h
    AbstractVectorShader.h
    FlatShader.h
    PhongShader.h
    Shaders.h
    VectorShader.h
    VertexColorShader.h

    magnumShadersVisibility.h)

add_library(MagnumShaders SHARED ${MagnumShaders_SRCS})
target_link_libraries(MagnumShaders Magnum)

install(TARGETS MagnumShaders DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumShaders_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Shaders)
