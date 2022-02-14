/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/Containers/Pair.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/SceneTools/OrderClusterParents.h"
#include "Magnum/Trade/SceneData.h"

namespace Magnum { namespace SceneTools { namespace Test { namespace {

struct OrderClusterParentsTest: TestSuite::Tester {
    explicit OrderClusterParentsTest();

    void test();
    void noParentField();
    void emptyParentField();

    void intoNoParentField();
    void intoEmptyParentField();
    void intoWrongDestinationSize();

    void sparse();
    void cyclic();
    void cyclicDeep();
    void sparseAndCyclic();
};

OrderClusterParentsTest::OrderClusterParentsTest() {
    addTests({&OrderClusterParentsTest::test,
              &OrderClusterParentsTest::noParentField,
              &OrderClusterParentsTest::emptyParentField,

              &OrderClusterParentsTest::intoNoParentField,
              &OrderClusterParentsTest::intoEmptyParentField,
              &OrderClusterParentsTest::intoWrongDestinationSize,

              &OrderClusterParentsTest::sparse,
              &OrderClusterParentsTest::cyclic,
              &OrderClusterParentsTest::cyclicDeep,
              &OrderClusterParentsTest::sparseAndCyclic});
}

void OrderClusterParentsTest::test() {
    struct Field {
        /* To verify we don't have unnecessarily hardcoded 32-bit types */
        UnsignedShort object;
        Byte parent;
    } data[]{
        /* Backward parent reference */
        {5, 1},
        /* Forward parent reference */
        {6, 9},
        /* Root elements */
        {3, -1},
        {1, -1},
        /* Deep hierarchy */
        {9, 10},
        {10, 3},
        /* Multiple children */
        {7, 3},
        {157, 3},
        {143, 6},
        /* More root elements */
        {2, -1}
        /* Elements 0, 4, 8, 11-142, 144-156 deliberately not used */
    };
    Containers::StridedArrayView1D<Field> view = data;

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 158, {}, data, {
        /* To verify it doesn't just pick the first field ever */
        Trade::SceneFieldData{Trade::SceneField::Mesh, Trade::SceneMappingType::UnsignedShort, nullptr, Trade::SceneFieldType::UnsignedInt, nullptr},
        Trade::SceneFieldData{Trade::SceneField::Parent, view.slice(&Field::object), view.slice(&Field::parent)}
    }};

    CORRADE_COMPARE_AS(orderClusterParents(scene), (Containers::arrayView<Containers::Pair<UnsignedInt, Int>>({
        /* Root objects first, in order as found */
        {3, -1},
        {1, -1},
        {2, -1},
        /* Then children of node 3, clustered together, in order as found */
        {10, 3},
        {7, 3},
        {157, 3},
        /* Then children of node 1 */
        {5, 1},
        /* Children of node 10 */
        {9, 10},
        /* Children of node 9 */
        {6, 9},
        /* Children of node 6 */
        {143, 6},
    })), TestSuite::Compare::Container);
}

void OrderClusterParentsTest::noParentField() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedByte, 0, nullptr, {}};

    std::ostringstream out;
    Error redirectError{&out};
    orderClusterParents(scene);
    CORRADE_COMPARE(out.str(),
        "SceneTools::orderClusterParents(): the scene has no hierarchy\n");
}

void OrderClusterParentsTest::emptyParentField() {
    Trade::SceneData scene{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {
        Trade::SceneFieldData{Trade::SceneField::Parent, Trade::SceneMappingType::UnsignedInt, nullptr, Trade::SceneFieldType::Int, nullptr}
    }};

    CORRADE_COMPARE_AS(orderClusterParents(scene),
        (Containers::ArrayView<const Containers::Pair<UnsignedInt, Int>>{}),
        TestSuite::Compare::Container);
}

void OrderClusterParentsTest::intoNoParentField() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedByte, 0, nullptr, {}};

    std::ostringstream out;
    Error redirectError{&out};
    orderClusterParentsInto(scene, nullptr, nullptr);
    CORRADE_COMPARE(out.str(),
        "SceneTools::orderClusterParentsInto(): the scene has no hierarchy\n");
}

void OrderClusterParentsTest::intoEmptyParentField() {
    Trade::SceneData scene{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {
        Trade::SceneFieldData{Trade::SceneField::Parent, Trade::SceneMappingType::UnsignedInt, nullptr, Trade::SceneFieldType::Int, nullptr}
    }};

    orderClusterParentsInto(scene, nullptr, nullptr);
    CORRADE_VERIFY(true);
}

void OrderClusterParentsTest::intoWrongDestinationSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct Field {
        UnsignedInt object;
        Int parent;
    } data[]{
        {2, -1},
        {3, 2},
        {7, -1}
    };
    Containers::StridedArrayView1D<Field> view = data;

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedInt, 8, {}, data, {
        Trade::SceneFieldData{Trade::SceneField::Parent, view.slice(&Field::object), view.slice(&Field::parent)}
    }};

    UnsignedInt mappingCorrect[3];
    UnsignedInt mapping[2];
    Int parentOffsetCorrect[3];
    Int parentOffset[2];

    std::ostringstream out;
    Error redirectError{&out};
    orderClusterParentsInto(scene, mappingCorrect, parentOffset);
    orderClusterParentsInto(scene, mapping, parentOffsetCorrect);
    CORRADE_COMPARE(out.str(),
        "SceneTools::orderClusterParentsInto(): expected parent destination view with 3 elements but got 2\n"
        "SceneTools::orderClusterParentsInto(): expected mapping destination view with 3 elements but got 2\n");
}

void OrderClusterParentsTest::sparse() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct Field {
        UnsignedInt object;
        Int parent;
    } data[]{
        {2, -1},
        {3, 2},
        {7, -1},
        /* Not reachable from root */
        {15, 6},
        {14, 6},
        {11, 15},
    };
    Containers::StridedArrayView1D<Field> view = data;

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedInt, 16, {}, data, {
        Trade::SceneFieldData{Trade::SceneField::Parent, view.slice(&Field::object), view.slice(&Field::parent)}
    }};

    std::ostringstream out;
    Error redirectError{&out};
    orderClusterParents(scene);
    CORRADE_COMPARE(out.str(),
        "SceneTools::orderClusterParents(): hierarchy is sparse\n");
}

void OrderClusterParentsTest::cyclic() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct Field {
        UnsignedInt object;
        Int parent;
    } data[]{
        {2, -1},
        {3, 2},
        {7, -1},
        /* Cycle of length 1, which will be treated as sparse hierarchy */
        {13, 13}
    };
    Containers::StridedArrayView1D<Field> view = data;

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedInt, 16, {}, data, {
        Trade::SceneFieldData{Trade::SceneField::Parent, view.slice(&Field::object), view.slice(&Field::parent)}
    }};

    std::ostringstream out;
    Error redirectError{&out};
    orderClusterParents(scene);
    CORRADE_COMPARE(out.str(),
        "SceneTools::orderClusterParents(): hierarchy is sparse\n");
}

void OrderClusterParentsTest::cyclicDeep() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct Field {
        UnsignedInt object;
        Int parent;
    } data[]{
        {2, -1},
        {3, 2},
        {7, -1},
        /* Cycle of length 3 */
        {13, -1},
        {5, 13},
        {13, 3}
    };
    Containers::StridedArrayView1D<Field> view = data;

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedInt, 16, {}, data, {
        Trade::SceneFieldData{Trade::SceneField::Parent, view.slice(&Field::object), view.slice(&Field::parent)}
    }};

    std::ostringstream out;
    Error redirectError{&out};
    orderClusterParents(scene);
    CORRADE_COMPARE(out.str(),
        "SceneTools::orderClusterParents(): hierarchy is cyclic\n");
}

void OrderClusterParentsTest::sparseAndCyclic() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct Field {
        UnsignedInt object;
        Int parent;
    } data[]{
        {2, -1},
        {3, 2},
        {7, -1},
        /* Cycle of length 3 */
        {13, -1},
        {5, 13},
        {13, 3},
        /* Not reachable from root */
        {15, 6}
    };
    Containers::StridedArrayView1D<Field> view = data;

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedInt, 16, {}, data, {
        Trade::SceneFieldData{Trade::SceneField::Parent, view.slice(&Field::object), view.slice(&Field::parent)}
    }};

    std::ostringstream out;
    Error redirectError{&out};
    orderClusterParents(scene);
    CORRADE_EXPECT_FAIL("The implementation needs to track already visited objects with a BitArray to detect this, it'd also provide a much better diagnostic.");
    CORRADE_COMPARE(out.str(),
        "SceneTools::orderClusterParents(): hierarchy is cyclic\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneTools::Test::OrderClusterParentsTest)
