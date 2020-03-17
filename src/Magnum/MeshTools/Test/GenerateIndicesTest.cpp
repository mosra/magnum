/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
              Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <sstream>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/MeshTools/GenerateIndices.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct GenerateIndicesTest: TestSuite::Tester {
    explicit GenerateIndicesTest();

    void generateLineStripIndices();
    void generateLineStripIndicesWrongVertexCount();
    void generateLineStripIndicesIntoWrongSize();

    void generateLineLoopIndices();
    void generateLineLoopIndicesWrongVertexCount();
    void generateLineLoopIndicesIntoWrongSize();

    void generateTriangleStripIndices();
    void generateTriangleStripIndicesWrongVertexCount();
    void generateTriangleStripIndicesIntoWrongSize();

    void generateTriangleFanIndices();
    void generateTriangleFanIndicesWrongVertexCount();
    void generateTriangleFanIndicesIntoWrongSize();
};

GenerateIndicesTest::GenerateIndicesTest() {
    addTests({&GenerateIndicesTest::generateLineStripIndices,
              &GenerateIndicesTest::generateLineStripIndicesWrongVertexCount,
              &GenerateIndicesTest::generateLineStripIndicesIntoWrongSize,

              &GenerateIndicesTest::generateLineLoopIndices,
              &GenerateIndicesTest::generateLineLoopIndicesWrongVertexCount,
              &GenerateIndicesTest::generateLineLoopIndicesIntoWrongSize,

              &GenerateIndicesTest::generateTriangleStripIndices,
              &GenerateIndicesTest::generateTriangleStripIndicesWrongVertexCount,
              &GenerateIndicesTest::generateTriangleStripIndicesIntoWrongSize,

              &GenerateIndicesTest::generateTriangleFanIndices,
              &GenerateIndicesTest::generateTriangleFanIndicesWrongVertexCount,
              &GenerateIndicesTest::generateTriangleFanIndicesIntoWrongSize});
}

void GenerateIndicesTest::generateLineStripIndices() {
    /* Minimal input */
    CORRADE_COMPARE_AS(MeshTools::generateLineStripIndices(2),
        Containers::arrayView<UnsignedInt>({
            0, 1
        }), TestSuite::Compare::Container);

    /* Odd */
    CORRADE_COMPARE_AS(MeshTools::generateLineStripIndices(5),
        Containers::arrayView<UnsignedInt>({
            0, 1,
            1, 2,
            2, 3,
            3, 4
        }), TestSuite::Compare::Container);

    /* Even */
    CORRADE_COMPARE_AS(MeshTools::generateLineStripIndices(6),
        Containers::arrayView<UnsignedInt>({
            0, 1,
            1, 2,
            2, 3,
            3, 4,
            4, 5
        }), TestSuite::Compare::Container);
}

void GenerateIndicesTest::generateLineStripIndicesWrongVertexCount() {
    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::generateLineStripIndicesInto(1, nullptr);
    CORRADE_COMPARE(out.str(),
        "MeshTools::generateLineStripIndicesInto(): expected at least two vertices, got 1\n");
}

void GenerateIndicesTest::generateLineStripIndicesIntoWrongSize() {
    UnsignedInt indices[7];

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::generateLineStripIndicesInto(5, indices);
    CORRADE_COMPARE(out.str(),
        "MeshTools::generateLineStripIndicesInto(): bad output size, expected 8 but got 7\n");
}

void GenerateIndicesTest::generateLineLoopIndices() {
    /* Minimal input */
    CORRADE_COMPARE_AS(MeshTools::generateLineLoopIndices(2),
        Containers::arrayView<UnsignedInt>({
            0, 1,
            1, 0
        }), TestSuite::Compare::Container);

    /* Odd */
    CORRADE_COMPARE_AS(MeshTools::generateLineLoopIndices(5),
        Containers::arrayView<UnsignedInt>({
            0, 1,
            1, 2,
            2, 3,
            3, 4,
            4, 0
        }), TestSuite::Compare::Container);

    /* Even */
    CORRADE_COMPARE_AS(MeshTools::generateLineLoopIndices(6),
        Containers::arrayView<UnsignedInt>({
            0, 1,
            1, 2,
            2, 3,
            3, 4,
            4, 5,
            5, 0
        }), TestSuite::Compare::Container);
}

void GenerateIndicesTest::generateLineLoopIndicesWrongVertexCount() {
    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::generateLineLoopIndicesInto(1, nullptr);
    CORRADE_COMPARE(out.str(),
        "MeshTools::generateLineLoopIndicesInto(): expected at least two vertices, got 1\n");
}

void GenerateIndicesTest::generateLineLoopIndicesIntoWrongSize() {
    UnsignedInt indices[9];

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::generateLineLoopIndicesInto(5, indices);
    CORRADE_COMPARE(out.str(),
        "MeshTools::generateLineLoopIndicesInto(): bad output size, expected 10 but got 9\n");
}

void GenerateIndicesTest::generateTriangleStripIndices() {
    /* Minimal input */
    CORRADE_COMPARE_AS(MeshTools::generateTriangleStripIndices(3),
        Containers::arrayView<UnsignedInt>({
            0, 1, 2
        }), TestSuite::Compare::Container);

    /* Odd */
    CORRADE_COMPARE_AS(MeshTools::generateTriangleStripIndices(7),
        Containers::arrayView<UnsignedInt>({
            0, 1, 2,
            2, 1, 3, /* Reversed */
            2, 3, 4,
            4, 3, 5, /* Reversed */
            4, 5, 6
        }), TestSuite::Compare::Container);

    /* Even */
    CORRADE_COMPARE_AS(MeshTools::generateTriangleStripIndices(8),
        Containers::arrayView<UnsignedInt>({
            0, 1, 2,
            2, 1, 3, /* Reversed */
            2, 3, 4,
            4, 3, 5, /* Reversed */
            4, 5, 6,
            6, 5, 7  /* Reversed */
        }), TestSuite::Compare::Container);
}

void GenerateIndicesTest::generateTriangleStripIndicesWrongVertexCount() {
    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::generateTriangleStripIndicesInto(2, nullptr);
    CORRADE_COMPARE(out.str(),
        "MeshTools::generateTriangleStripIndicesInto(): expected at least three vertices, got 2\n");
}

void GenerateIndicesTest::generateTriangleStripIndicesIntoWrongSize() {
    UnsignedInt indices[8];

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::generateTriangleStripIndicesInto(5, indices);
    CORRADE_COMPARE(out.str(),
        "MeshTools::generateTriangleStripIndicesInto(): bad output size, expected 9 but got 8\n");
}

void GenerateIndicesTest::generateTriangleFanIndices() {
    /* Minimal input */
    CORRADE_COMPARE_AS(MeshTools::generateTriangleFanIndices(3),
        Containers::arrayView<UnsignedInt>({
            0, 1, 2
        }), TestSuite::Compare::Container);

    /* Odd */
    CORRADE_COMPARE_AS(MeshTools::generateTriangleFanIndices(7),
        Containers::arrayView<UnsignedInt>({
            0, 1, 2,
            0, 2, 3,
            0, 3, 4,
            0, 4, 5,
            0, 5, 6
        }), TestSuite::Compare::Container);

    /* Even */
    CORRADE_COMPARE_AS(MeshTools::generateTriangleFanIndices(8),
        Containers::arrayView<UnsignedInt>({
            0, 1, 2,
            0, 2, 3,
            0, 3, 4,
            0, 4, 5,
            0, 5, 6,
            0, 6, 7
        }), TestSuite::Compare::Container);
}

void GenerateIndicesTest::generateTriangleFanIndicesWrongVertexCount() {
    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::generateTriangleFanIndicesInto(2, nullptr);
    CORRADE_COMPARE(out.str(),
        "MeshTools::generateTriangleFanIndicesInto(): expected at least three vertices, got 2\n");
}

void GenerateIndicesTest::generateTriangleFanIndicesIntoWrongSize() {
    UnsignedInt indices[8];

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::generateTriangleFanIndicesInto(5, indices);
    CORRADE_COMPARE(out.str(),
        "MeshTools::generateTriangleFanIndicesInto(): bad output size, expected 9 but got 8\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::GenerateIndicesTest)
