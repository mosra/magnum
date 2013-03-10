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

corrade_add_resource(MagnumTextureTools_RCS MagnumTextureTools
    DistanceFieldShader.vert DistanceFieldShader.frag
    ../Shaders/compatibility.glsl ALIAS compatibility.glsl)

set(MagnumTextureTools_SRCS
    Atlas.cpp
    DistanceField.cpp
    ${MagnumTextureTools_RCS})

set(MagnumTextureTools_HEADERS
    Atlas.h
    DistanceField.h

    magnumTextureToolsVisibility.h)

add_library(MagnumTextureTools SHARED ${MagnumTextureTools_SRCS})
target_link_libraries(MagnumTextureTools Magnum)

install(TARGETS MagnumTextureTools DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumTextureTools_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/TextureTools)

if(BUILD_TESTS)
    add_subdirectory(Test)
endif()
