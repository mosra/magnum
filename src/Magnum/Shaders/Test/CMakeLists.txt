#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
#             Vladimír Vondruš <mosra@centrum.cz>
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

corrade_add_test(ShadersDistanceFieldVectorTest DistanceFieldVectorTest.cpp LIBRARIES MagnumShaders)
corrade_add_test(ShadersFlatTest FlatTest.cpp LIBRARIES MagnumShaders)
corrade_add_test(ShadersMeshVisualizerTest MeshVisualizerTest.cpp LIBRARIES MagnumShaders)
corrade_add_test(ShadersPhongTest PhongTest.cpp LIBRARIES MagnumShaders)
corrade_add_test(ShadersVectorTest VectorTest.cpp LIBRARIES MagnumShaders)
corrade_add_test(ShadersVertexColorTest VertexColorTest.cpp LIBRARIES MagnumShaders)

set_target_properties(
    ShadersDistanceFieldVectorTest
    ShadersFlatTest
    ShadersMeshVisualizerTest
    ShadersPhongTest
    ShadersVectorTest
    ShadersVertexColorTest
    PROPERTIES FOLDER "Magnum/Shaders/Test")

if(BUILD_GL_TESTS)
    corrade_add_test(ShadersDistanceFieldVectorGLTest DistanceFieldVectorGLTest.cpp LIBRARIES MagnumShaders MagnumOpenGLTester)
    corrade_add_test(ShadersFlatGLTest FlatGLTest.cpp LIBRARIES MagnumShaders MagnumOpenGLTester)
    corrade_add_test(ShadersMeshVisualizerGLTest MeshVisualizerGLTest.cpp LIBRARIES MagnumShaders MagnumOpenGLTester)
    corrade_add_test(ShadersPhongGLTest PhongGLTest.cpp LIBRARIES MagnumShaders MagnumOpenGLTester)
    corrade_add_test(ShadersVectorGLTest VectorGLTest.cpp LIBRARIES MagnumShaders MagnumOpenGLTester)
    corrade_add_test(ShadersVertexColorGLTest VertexColorGLTest.cpp LIBRARIES MagnumShaders MagnumOpenGLTester)

    set_target_properties(
        ShadersDistanceFieldVectorGLTest
        ShadersFlatGLTest
        ShadersMeshVisualizerGLTest
        ShadersPhongGLTest
        ShadersVectorGLTest
        ShadersVertexColorGLTest
        PROPERTIES FOLDER "Magnum/Shaders/Test")
endif()
