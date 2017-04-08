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

corrade_add_test(MeshToolsCombineIndexedArraysTest CombineIndexedArraysTest.cpp LIBRARIES MagnumMeshToolsTestLib)
corrade_add_test(MeshToolsCompressIndicesTest CompressIndicesTest.cpp LIBRARIES MagnumMeshToolsTestLib)
corrade_add_test(MeshToolsDuplicateTest DuplicateTest.cpp LIBRARIES Magnum)
corrade_add_test(MeshToolsFlipNormalsTest FlipNormalsTest.cpp LIBRARIES MagnumMeshToolsTestLib)
corrade_add_test(MeshToolsGenerateFlatNormalsTest GenerateFlatNormalsTest.cpp LIBRARIES MagnumMeshToolsTestLib)
corrade_add_test(MeshToolsInterleaveTest InterleaveTest.cpp LIBRARIES Magnum)
corrade_add_test(MeshToolsRemoveDuplicatesTest RemoveDuplicatesTest.cpp LIBRARIES Magnum)
corrade_add_test(MeshToolsSubdivideTest SubdivideTest.cpp LIBRARIES Magnum)
corrade_add_test(MeshToolsSubdivideRemov___Benchmark SubdivideRemoveDuplicatesBenchmark.cpp LIBRARIES MagnumPrimitives)
corrade_add_test(MeshToolsTipsifyTest TipsifyTest.cpp LIBRARIES MagnumMeshTools)
corrade_add_test(MeshToolsTransformTest TransformTest.cpp LIBRARIES MagnumMeshTools)

# Graceful assert for testing
set_property(TARGET
    MeshToolsCombineIndexedArraysTest
    MeshToolsInterleaveTest
    MeshToolsSubdivideTest
    APPEND PROPERTY COMPILE_DEFINITIONS "CORRADE_GRACEFUL_ASSERT")

set_target_properties(
    MeshToolsCombineIndexedArraysTest
    MeshToolsCompressIndicesTest
    MeshToolsDuplicateTest
    MeshToolsFlipNormalsTest
    MeshToolsGenerateFlatNormalsTest
    MeshToolsInterleaveTest
    MeshToolsRemoveDuplicatesTest
    MeshToolsSubdivideTest
    MeshToolsSubdivideRemov___Benchmark
    MeshToolsTipsifyTest
    MeshToolsTransformTest
    PROPERTIES FOLDER "Magnum/MeshTools/Test")
