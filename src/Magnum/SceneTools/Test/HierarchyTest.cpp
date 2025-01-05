/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/Triple.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/SceneTools/Hierarchy.h"
#include "Magnum/Trade/SceneData.h"

namespace Magnum { namespace SceneTools { namespace Test { namespace {

struct HierarchyTest: TestSuite::Tester {
    explicit HierarchyTest();

    void parentsBreadthFirstChildrenDepthFirst();
    void parentsBreadthFirstChildrenDepthFirstSingleBranch();
    void parentsBreadthFirstChildrenDepthFirstNoParentField();
    void parentsBreadthFirstChildrenDepthFirstEmptyParentField();

    void parentsBreadthFirstChildrenDepthFirstIntoNoParentField();
    void parentsBreadthFirstChildrenDepthFirstIntoEmptyParentField();
    void parentsBreadthFirstChildrenDepthFirstIntoWrongDestinationSize();

    void parentsBreadthFirstChildrenDepthFirstSparse();
    void parentsBreadthFirstChildrenDepthFirstCyclic();
    void parentsBreadthFirstChildrenDepthFirstCyclicDeep();
    void parentsBreadthFirstChildrenDepthFirstSparseAndCyclic();

    void absoluteFieldTransformations2D();
    void absoluteFieldTransformations3D();

    void absoluteFieldTransformationsFieldNotFound();
    void absoluteFieldTransformationsNot2DNot3D();
    void absoluteFieldTransformationsNoParentField();

    void absoluteFieldTransformationsInto2D();
    void absoluteFieldTransformationsInto3D();
    void absoluteFieldTransformationsIntoInvalidSize();
};

using namespace Math::Literals;

const struct {
    const char* name;
    Matrix3 globalTransformation2D;
    Matrix4 globalTransformation3D;
    bool fieldIdInsteadOfName;
    std::size_t transformationsToExclude, meshesToExclude;
    std::size_t expectedOutputSize;
} TestData[]{
    {"", {}, {}, false,
        2, 0,
        5},
    {"field ID", {}, {}, true,
        2, 0,
        5},
    {"global transformation",
        Matrix3::scaling(Vector2{0.5f}), Matrix4::scaling(Vector3{0.5f}), false,
        2, 0,
        5},
    {"global transformation, field ID",
        Matrix3::scaling(Vector2{0.5f}), Matrix4::scaling(Vector3{0.5f}), true,
        2, 0,
        5},
    {"transformations not part of the hierarchy", {}, {}, false,
        0, 0,
        5},
    {"empty field", {}, {}, false,
        2, 5,
        0},
};

const struct {
    const char* name;
    Matrix3 globalTransformation2D;
    Matrix4 globalTransformation3D;
    bool fieldIdInsteadOfName;
    std::size_t expectedOutputSize;
} IntoData[]{
    {"", {}, {}, false,
        5},
    {"field ID", {}, {}, true,
        5},
    {"global transformation",
        Matrix3::scaling(Vector2{0.5f}), Matrix4::scaling(Vector3{0.5f}), false,
        5},
    {"global transformation, field ID",
        Matrix3::scaling(Vector2{0.5f}), Matrix4::scaling(Vector3{0.5f}), true,
        5},
};

HierarchyTest::HierarchyTest() {
    addTests({&HierarchyTest::parentsBreadthFirstChildrenDepthFirst,
              &HierarchyTest::parentsBreadthFirstChildrenDepthFirstSingleBranch,
              &HierarchyTest::parentsBreadthFirstChildrenDepthFirstNoParentField,
              &HierarchyTest::parentsBreadthFirstChildrenDepthFirstEmptyParentField,

              &HierarchyTest::parentsBreadthFirstChildrenDepthFirstIntoNoParentField,
              &HierarchyTest::parentsBreadthFirstChildrenDepthFirstIntoEmptyParentField,
              &HierarchyTest::parentsBreadthFirstChildrenDepthFirstIntoWrongDestinationSize,

              &HierarchyTest::parentsBreadthFirstChildrenDepthFirstSparse,
              &HierarchyTest::parentsBreadthFirstChildrenDepthFirstCyclic,
              &HierarchyTest::parentsBreadthFirstChildrenDepthFirstCyclicDeep,
              &HierarchyTest::parentsBreadthFirstChildrenDepthFirstSparseAndCyclic});

    addInstancedTests({&HierarchyTest::absoluteFieldTransformations2D,
                       &HierarchyTest::absoluteFieldTransformations3D},
        Containers::arraySize(TestData));

    addTests({&HierarchyTest::absoluteFieldTransformationsFieldNotFound,
              &HierarchyTest::absoluteFieldTransformationsNot2DNot3D,
              &HierarchyTest::absoluteFieldTransformationsNoParentField});

    addInstancedTests({&HierarchyTest::absoluteFieldTransformationsInto2D,
                       &HierarchyTest::absoluteFieldTransformationsInto3D},
        Containers::arraySize(IntoData));

    addTests({&HierarchyTest::absoluteFieldTransformationsIntoInvalidSize});
}

void HierarchyTest::parentsBreadthFirstChildrenDepthFirst() {
    struct Field {
        /* To verify we don't have unnecessarily hardcoded 32-bit types */
        UnsignedShort mapping;
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
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Trade::SceneMappingType::UnsignedShort, nullptr,
            Trade::SceneFieldType::UnsignedInt, nullptr},
        Trade::SceneFieldData{Trade::SceneField::Parent,
            view.slice(&Field::mapping),
            view.slice(&Field::parent)}
    }};

    CORRADE_COMPARE_AS(SceneTools::parentsBreadthFirst(scene), (Containers::arrayView<Containers::Pair<UnsignedInt, Int>>({
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

    CORRADE_COMPARE_AS(SceneTools::childrenDepthFirst(scene), (Containers::arrayView<Containers::Pair<UnsignedInt, UnsignedInt>>({
        /* Node 3, root */
        {3, 6},
            /* Children of node 3, in order as found */
            {10, 3},
                /* Children of node 10 */
                {9, 2},
                    /* Children of node 9 */
                    {6, 1},
                        /* Children of node 6 */
                        {143, 0},
            {7, 0},
            {157, 0},
        /* Node 1, root */
        {1, 1},
            /* Children of node 1 */
            {5, 0},
        /* Node 2, root */
        {2, 0},
    })), TestSuite::Compare::Container);
}

void HierarchyTest::parentsBreadthFirstChildrenDepthFirstSingleBranch() {
    /* Verifies just that the internal allocation routines are correctly sized,
       as this should lead to the longest stack in childrenDepthFirst().
       Shouldn't trigger anything special in parentsBreadthFirst() but testing
       that one as well to be sure. */

    struct Field {
        UnsignedLong mapping;
        Long parent;
    } data[]{
        {2, 1},
        {1, 0},
        {3, 2},
        {0, -1},
    };
    Containers::StridedArrayView1D<Field> view = data;

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedLong, 4, {}, data, {
        Trade::SceneFieldData{Trade::SceneField::Parent,
            view.slice(&Field::mapping),
            view.slice(&Field::parent)}
    }};

    CORRADE_COMPARE_AS(SceneTools::parentsBreadthFirst(scene), (Containers::arrayView<Containers::Pair<UnsignedInt, Int>>({
        {0, -1},
        {1, 0},
        {2, 1},
        {3, 2},
    })), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(SceneTools::childrenDepthFirst(scene), (Containers::arrayView<Containers::Pair<UnsignedInt, UnsignedInt>>({
        {0, 3},
            {1, 2},
                {2, 1},
                    {3, 0},
    })), TestSuite::Compare::Container);
}

void HierarchyTest::parentsBreadthFirstChildrenDepthFirstNoParentField() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedByte, 0, nullptr, {}};

    Containers::String out;
    Error redirectError{&out};
    SceneTools::parentsBreadthFirst(scene);
    SceneTools::childrenDepthFirst(scene);
    CORRADE_COMPARE(out,
        "SceneTools::parentsBreadthFirst(): the scene has no hierarchy\n"
        "SceneTools::childrenDepthFirst(): the scene has no hierarchy\n");
}

void HierarchyTest::parentsBreadthFirstChildrenDepthFirstEmptyParentField() {
    Trade::SceneData scene{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {
        Trade::SceneFieldData{Trade::SceneField::Parent, Trade::SceneMappingType::UnsignedInt, nullptr, Trade::SceneFieldType::Int, nullptr}
    }};

    CORRADE_COMPARE_AS(SceneTools::parentsBreadthFirst(scene),
        (Containers::ArrayView<const Containers::Pair<UnsignedInt, Int>>{}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(SceneTools::childrenDepthFirst(scene),
        (Containers::ArrayView<const Containers::Pair<UnsignedInt, UnsignedInt>>{}),
        TestSuite::Compare::Container);
}

void HierarchyTest::parentsBreadthFirstChildrenDepthFirstIntoNoParentField() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedByte, 0, nullptr, {}};

    Containers::String out;
    Error redirectError{&out};
    parentsBreadthFirstInto(scene, nullptr, nullptr);
    childrenDepthFirstInto(scene, nullptr, nullptr);
    CORRADE_COMPARE(out,
        "SceneTools::parentsBreadthFirstInto(): the scene has no hierarchy\n"
        "SceneTools::childrenDepthFirstInto(): the scene has no hierarchy\n");
}

void HierarchyTest::parentsBreadthFirstChildrenDepthFirstIntoEmptyParentField() {
    Trade::SceneData scene{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {
        Trade::SceneFieldData{Trade::SceneField::Parent, Trade::SceneMappingType::UnsignedInt, nullptr, Trade::SceneFieldType::Int, nullptr}
    }};

    parentsBreadthFirstInto(scene, nullptr, nullptr);
    childrenDepthFirstInto(scene, nullptr, nullptr);
    CORRADE_VERIFY(true);
}

void HierarchyTest::parentsBreadthFirstChildrenDepthFirstIntoWrongDestinationSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Field {
        UnsignedInt mapping;
        Int parent;
    } data[]{
        {2, -1},
        {3, 2},
        {7, -1}
    };
    Containers::StridedArrayView1D<Field> view = data;

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedInt, 8, {}, data, {
        Trade::SceneFieldData{Trade::SceneField::Parent,
            view.slice(&Field::mapping),
            view.slice(&Field::parent)}
    }};

    UnsignedInt mappingCorrect[3];
    UnsignedInt mapping[2];
    Int parentOffsetCorrect[3];
    Int parentOffset[2];
    UnsignedInt childCountCorrect[3];
    UnsignedInt childCount[2];

    Containers::String out;
    Error redirectError{&out};
    parentsBreadthFirstInto(scene, mappingCorrect, parentOffset);
    parentsBreadthFirstInto(scene, mapping, parentOffsetCorrect);
    childrenDepthFirstInto(scene, mappingCorrect, childCount);
    childrenDepthFirstInto(scene, mapping, childCountCorrect);
    CORRADE_COMPARE(out,
        "SceneTools::parentsBreadthFirstInto(): expected parent destination view with 3 elements but got 2\n"
        "SceneTools::parentsBreadthFirstInto(): expected mapping destination view with 3 elements but got 2\n"
        "SceneTools::childrenDepthFirstInto(): expected child count destination view with 3 elements but got 2\n"
        "SceneTools::childrenDepthFirstInto(): expected mapping destination view with 3 elements but got 2\n");
}

void HierarchyTest::parentsBreadthFirstChildrenDepthFirstSparse() {
    CORRADE_SKIP_IF_NO_ASSERT();

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
        Trade::SceneFieldData{Trade::SceneField::Parent,
            view.slice(&Field::object),
            view.slice(&Field::parent)}
    }};

    Containers::String out;
    Error redirectError{&out};
    SceneTools::parentsBreadthFirst(scene);
    SceneTools::childrenDepthFirst(scene);
    CORRADE_COMPARE(out,
        "SceneTools::parentsBreadthFirst(): hierarchy is sparse\n"
        "SceneTools::childrenDepthFirst(): hierarchy is sparse\n");
}

void HierarchyTest::parentsBreadthFirstChildrenDepthFirstCyclic() {
    CORRADE_SKIP_IF_NO_ASSERT();

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
        Trade::SceneFieldData{Trade::SceneField::Parent,
            view.slice(&Field::object),
            view.slice(&Field::parent)}
    }};

    Containers::String out;
    Error redirectError{&out};
    SceneTools::parentsBreadthFirst(scene);
    SceneTools::childrenDepthFirst(scene);
    CORRADE_COMPARE(out,
        "SceneTools::parentsBreadthFirst(): hierarchy is sparse\n"
        "SceneTools::childrenDepthFirst(): hierarchy is sparse\n");
}

void HierarchyTest::parentsBreadthFirstChildrenDepthFirstCyclicDeep() {
    CORRADE_SKIP_IF_NO_ASSERT();

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
        Trade::SceneFieldData{Trade::SceneField::Parent,
            view.slice(&Field::object),
            view.slice(&Field::parent)}
    }};

    Containers::String out;
    Error redirectError{&out};
    SceneTools::parentsBreadthFirst(scene);
    SceneTools::childrenDepthFirst(scene);
    CORRADE_COMPARE(out,
        "SceneTools::parentsBreadthFirst(): hierarchy is cyclic\n"
        "SceneTools::childrenDepthFirst(): hierarchy is cyclic\n");
}

void HierarchyTest::parentsBreadthFirstChildrenDepthFirstSparseAndCyclic() {
    CORRADE_SKIP_IF_NO_ASSERT();

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
        Trade::SceneFieldData{Trade::SceneField::Parent,
            view.slice(&Field::object),
            view.slice(&Field::parent)}
    }};

    Containers::String out;
    Error redirectError{&out};
    SceneTools::parentsBreadthFirst(scene);
    SceneTools::childrenDepthFirst(scene);
    CORRADE_EXPECT_FAIL("The implementation needs to track already visited objects with a BitArray to detect this, it'd also provide a much better diagnostic.");
    CORRADE_COMPARE(out,
        "SceneTools::parentsBreadthFirst(): hierarchy is cyclic\n"
        "SceneTools::childrenDepthFirst(): hierarchy is cyclic\n");
}

const struct Scene {
    /* Using smaller types to verify we don't have unnecessarily hardcoded
       32-bit types */
    struct Parent {
        UnsignedShort object;
        Byte parent;
    } parents[9];

    struct Transformation {
        UnsignedShort object;
        Matrix3 transformation2D;
        Matrix4 transformation3D;
    } transforms[7];

    struct Mesh {
        UnsignedShort object;
        UnsignedShort mesh;
    } meshes[5];
} Data[]{{
    /*
        Cases to test:

        -   leaf paths with no attachments which don't contribute to the
            output in any way
        -   nodes with transforms but no meshes
        -   nodes with meshes but no transforms
        -   nodes with multiple meshes
        -   nodes with neither transforms nor meshes
        -   object 4 has a mesh with identity transform (or, rather, no
            transformation entry at all)
        -   objects 2 and 16 have the same mesh attached with the exact
            same transform -- this is a nonsense (they would overlap) and
            as such isn't deduplicated in any way
        -   objects 0, 32 and 17 have transformations/meshes, but not part
            of the hierarchy; these are cut away from the views in the
            first test case to keep it simple

            1T       4M
           / \       |              32M 0MM
          5T 2TM     11
         / \   \     |               32T 17T
       3MM  7T  6   16TM
    */
    {{3, 5},
     {11, 4},
     {5, 1},
     {1, -1},
     {7, 5},
     {6, 2},
     {2, 1},
     {4, -1},
     {16, 11}},
    {{2, Matrix3::scaling({3.0f, 5.0f}),
         Matrix4::scaling({3.0f, 5.0f, 2.0f})},
     {1, Matrix3::translation({1.0f, -1.5f}),
         Matrix4::translation({1.0f, -1.5f, 0.5f})},
     /* Same absolute transform as node 2 */
     {16, Matrix3::translation({1.0f, -1.5f})*
          Matrix3::scaling({3.0f, 5.0f}),
          Matrix4::translation({1.0f, -1.5f, 0.5f})*
          Matrix4::scaling({3.0f, 5.0f, 2.0f})},
     {7, Matrix3::scaling({2.0f, 1.0f}),
         Matrix4::scaling({2.0f, 1.0f, 0.5f})},
     {5, Matrix3::rotation(35.0_degf),
         Matrix4::rotationZ(35.0_degf)},
     /* These are not part of the hierarchy */
     {32, Matrix3::translation({1.0f, 0.5f}),
          Matrix4::translation({1.0f, 0.5f, 2.0f})},
     {17, Matrix3::translation({2.0f, 1.0f}),
          Matrix4::translation({2.0f, 1.0f, 4.0f})},
    },
    /* The mesh IDs aren't used for anything, just setting them to something
       random (and high) to avoid their misuses as some offsets / IDs */
    {{2, 113},
     {3, 266},
     {4, 525},
     {3, 422},
     {16, 113}}
}};

void HierarchyTest::absoluteFieldTransformations2D() {
    auto&& data = TestData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 33, {}, Data, {
        /* To verify it doesn't just pick the first field ever */
        Trade::SceneFieldData{Trade::SceneField::Camera, Trade::SceneMappingType::UnsignedShort, nullptr, Trade::SceneFieldType::UnsignedInt, nullptr},
        Trade::SceneFieldData{Trade::SceneField::Parent,
            Containers::stridedArrayView(Data->parents)
                .slice(&Scene::Parent::object),
            Containers::stridedArrayView(Data->parents)
                .slice(&Scene::Parent::parent)},
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::stridedArrayView(Data->meshes)
                .slice(&Scene::Mesh::object)
                .exceptSuffix(data.meshesToExclude),
            Containers::stridedArrayView(Data->meshes)
                .slice(&Scene::Mesh::mesh)
                .exceptSuffix(data.meshesToExclude)},
        Trade::SceneFieldData{Trade::SceneField::Transformation,
            Containers::stridedArrayView(Data->transforms)
                .slice(&Scene::Transformation::object)
                .exceptSuffix(data.transformationsToExclude),
            Containers::stridedArrayView(Data->transforms)
                .slice(&Scene::Transformation::transformation2D)
                .exceptSuffix(data.transformationsToExclude)},
    }};

    Containers::Array<Matrix3> out;
    /* To test all overloads */
    if(data.globalTransformation2D != Matrix3{}) {
        if(data.fieldIdInsteadOfName)
            out = SceneTools::absoluteFieldTransformations2D(scene, 2, data.globalTransformation2D);
        else
            out = SceneTools::absoluteFieldTransformations2D(scene, Trade::SceneField::Mesh, data.globalTransformation2D);
    } else {
        if(data.fieldIdInsteadOfName)
            out = SceneTools::absoluteFieldTransformations2D(scene, 2);
        else
            out = SceneTools::absoluteFieldTransformations2D(scene, Trade::SceneField::Mesh);
    }

    CORRADE_COMPARE_AS(out, Containers::arrayView({
        data.globalTransformation2D*
            Matrix3::translation({1.0f, -1.5f})*
            Matrix3::scaling({3.0f, 5.0f}),
        data.globalTransformation2D*
            Matrix3::translation({1.0f, -1.5f})*
            Matrix3::rotation(35.0_degf),
        data.globalTransformation2D,
        data.globalTransformation2D*
            Matrix3::translation({1.0f, -1.5f})*
            Matrix3::rotation(35.0_degf),
        data.globalTransformation2D*
            Matrix3::translation({1.0f, -1.5f})*
            Matrix3::scaling({3.0f, 5.0f})
    }).prefix(data.expectedOutputSize), TestSuite::Compare::Container);
}

void HierarchyTest::absoluteFieldTransformations3D() {
    auto&& data = TestData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 33, {}, Data, {
        /* To verify it doesn't just pick the first field ever */
        Trade::SceneFieldData{Trade::SceneField::Camera, Trade::SceneMappingType::UnsignedShort, nullptr, Trade::SceneFieldType::UnsignedInt, nullptr},
        Trade::SceneFieldData{Trade::SceneField::Parent,
            Containers::stridedArrayView(Data->parents)
                .slice(&Scene::Parent::object),
            Containers::stridedArrayView(Data->parents)
                .slice(&Scene::Parent::parent)},
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::stridedArrayView(Data->meshes)
                .slice(&Scene::Mesh::object)
                .exceptSuffix(data.meshesToExclude),
            Containers::stridedArrayView(Data->meshes)
                .slice(&Scene::Mesh::mesh)
                .exceptSuffix(data.meshesToExclude)},
        Trade::SceneFieldData{Trade::SceneField::Transformation,
            Containers::stridedArrayView(Data->transforms)
                .slice(&Scene::Transformation::object)
                .exceptSuffix(data.transformationsToExclude),
            Containers::stridedArrayView(Data->transforms)
                .slice(&Scene::Transformation::transformation3D)
                .exceptSuffix(data.transformationsToExclude)},
    }};

    Containers::Array<Matrix4> out;
    /* To test all overloads */
    if(data.globalTransformation3D != Matrix4{}) {
        if(data.fieldIdInsteadOfName)
            out = SceneTools::absoluteFieldTransformations3D(scene, 2, data.globalTransformation3D);
        else
            out = SceneTools::absoluteFieldTransformations3D(scene, Trade::SceneField::Mesh, data.globalTransformation3D);
    } else {
        if(data.fieldIdInsteadOfName)
            out = SceneTools::absoluteFieldTransformations3D(scene, 2);
        else
            out = SceneTools::absoluteFieldTransformations3D(scene, Trade::SceneField::Mesh);
    }

    CORRADE_COMPARE_AS(out, Containers::arrayView({
        data.globalTransformation3D*
            Matrix4::translation({1.0f, -1.5f, 0.5f})*
            Matrix4::scaling({3.0f, 5.0f, 2.0f}),
        data.globalTransformation3D*
            Matrix4::translation({1.0f, -1.5f, 0.5f})*
            Matrix4::rotationZ(35.0_degf),
        data.globalTransformation3D,
        data.globalTransformation3D*
            Matrix4::translation({1.0f, -1.5f, 0.5f})*
            Matrix4::rotationZ(35.0_degf),
        data.globalTransformation3D*
            Matrix4::translation({1.0f, -1.5f, 0.5f})*
            Matrix4::scaling({3.0f, 5.0f, 2.0f})
    }).prefix(data.expectedOutputSize), TestSuite::Compare::Container);
}

void HierarchyTest::absoluteFieldTransformationsFieldNotFound() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {
        Trade::SceneFieldData{Trade::SceneField::Parent, Trade::SceneMappingType::UnsignedInt, nullptr, Trade::SceneFieldType::Int, nullptr},
        Trade::SceneFieldData{Trade::SceneField::Transformation, Trade::SceneMappingType::UnsignedInt, nullptr, Trade::SceneFieldType::Matrix3x3, nullptr}
    }};

    Containers::String out;
    Error redirectError{&out};
    SceneTools::absoluteFieldTransformations2D(scene, Trade::SceneField::Mesh);
    SceneTools::absoluteFieldTransformations3D(scene, Trade::SceneField::Mesh);
    SceneTools::absoluteFieldTransformations2D(scene, 2);
    SceneTools::absoluteFieldTransformations3D(scene, 2);
    CORRADE_COMPARE(out,
        "SceneTools::absoluteFieldTransformations(): field Trade::SceneField::Mesh not found\n"
        "SceneTools::absoluteFieldTransformations(): field Trade::SceneField::Mesh not found\n"
        "SceneTools::absoluteFieldTransformations(): index 2 out of range for 2 fields\n"
        "SceneTools::absoluteFieldTransformations(): index 2 out of range for 2 fields\n");
}

void HierarchyTest::absoluteFieldTransformationsNot2DNot3D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {
        Trade::SceneFieldData{Trade::SceneField::Parent, Trade::SceneMappingType::UnsignedInt, nullptr, Trade::SceneFieldType::Int, nullptr}
    }};

    Containers::String out;
    Error redirectError{&out};
    SceneTools::absoluteFieldTransformations2D(scene, Trade::SceneField::Parent);
    SceneTools::absoluteFieldTransformations2D(scene, 0);
    SceneTools::absoluteFieldTransformations3D(scene, Trade::SceneField::Parent);
    SceneTools::absoluteFieldTransformations3D(scene, 0);
    CORRADE_COMPARE(out,
        "SceneTools::absoluteFieldTransformations(): the scene is not 2D\n"
        "SceneTools::absoluteFieldTransformations(): the scene is not 2D\n"
        "SceneTools::absoluteFieldTransformations(): the scene is not 3D\n"
        "SceneTools::absoluteFieldTransformations(): the scene is not 3D\n");
}

void HierarchyTest::absoluteFieldTransformationsNoParentField() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {
        Trade::SceneFieldData{Trade::SceneField::Transformation, Trade::SceneMappingType::UnsignedInt, nullptr, Trade::SceneFieldType::Matrix3x3, nullptr}
    }};

    Containers::String out;
    Error redirectError{&out};
    SceneTools::absoluteFieldTransformations2D(scene, Trade::SceneField::Transformation);
    SceneTools::absoluteFieldTransformations2D(scene, 0);
    CORRADE_COMPARE(out,
        "SceneTools::absoluteFieldTransformations(): the scene has no hierarchy\n"
        "SceneTools::absoluteFieldTransformations(): the scene has no hierarchy\n");
}

void HierarchyTest::absoluteFieldTransformationsInto2D() {
    auto&& data = IntoData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* The *Into() variant is the actual base implementation, so just verify
       that the data get correctly propagated through. Everything else is
       tested above already. */

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 33, {}, Data, {
        Trade::SceneFieldData{Trade::SceneField::Parent,
            Containers::stridedArrayView(Data->parents)
                .slice(&Scene::Parent::object),
            Containers::stridedArrayView(Data->parents)
                .slice(&Scene::Parent::parent)},
        Trade::SceneFieldData{Trade::SceneField::Transformation,
            Containers::stridedArrayView(Data->transforms)
                .slice(&Scene::Transformation::object),
            Containers::stridedArrayView(Data->transforms)
                .slice(&Scene::Transformation::transformation2D)},
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::stridedArrayView(Data->meshes)
                .slice(&Scene::Mesh::object),
            Containers::stridedArrayView(Data->meshes)
                .slice(&Scene::Mesh::mesh)}
    }};

    Containers::Array<Matrix3> out{NoInit, scene.fieldSize(Trade::SceneField::Mesh)};
    /* To test all overloads */
    if(data.globalTransformation2D != Matrix3{}) {
        if(data.fieldIdInsteadOfName)
            absoluteFieldTransformations2DInto(scene, 2, out, data.globalTransformation2D);
        else
            absoluteFieldTransformations2DInto(scene, Trade::SceneField::Mesh, out, data.globalTransformation2D);
    } else {
        if(data.fieldIdInsteadOfName)
            absoluteFieldTransformations2DInto(scene, 2, out);
        else
            absoluteFieldTransformations2DInto(scene, Trade::SceneField::Mesh, out);
    }

    CORRADE_COMPARE_AS(out, Containers::arrayView<Matrix3>({
        data.globalTransformation2D*
            Matrix3::translation({1.0f, -1.5f})*
            Matrix3::scaling({3.0f, 5.0f}),
        data.globalTransformation2D*
            Matrix3::translation({1.0f, -1.5f})*
            Matrix3::rotation(35.0_degf),
        data.globalTransformation2D,
        data.globalTransformation2D*
            Matrix3::translation({1.0f, -1.5f})*
            Matrix3::rotation(35.0_degf),
        data.globalTransformation2D*
            Matrix3::translation({1.0f, -1.5f})*
            Matrix3::scaling({3.0f, 5.0f})
    }), TestSuite::Compare::Container);
}

void HierarchyTest::absoluteFieldTransformationsInto3D() {
    auto&& data = IntoData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* The *Into() variant is the actual base implementation, so just verify
       that the data get correctly propagated through. Everything else is
       tested above already. */

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 33, {}, Data, {
        Trade::SceneFieldData{Trade::SceneField::Parent,
            Containers::stridedArrayView(Data->parents)
                .slice(&Scene::Parent::object),
            Containers::stridedArrayView(Data->parents)
                .slice(&Scene::Parent::parent)},
        Trade::SceneFieldData{Trade::SceneField::Transformation,
            Containers::stridedArrayView(Data->transforms)
                .slice(&Scene::Transformation::object),
            Containers::stridedArrayView(Data->transforms)
                .slice(&Scene::Transformation::transformation3D)},
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::stridedArrayView(Data->meshes)
                .slice(&Scene::Mesh::object),
            Containers::stridedArrayView(Data->meshes)
                .slice(&Scene::Mesh::mesh)}
    }};

    Containers::Array<Matrix4> out{NoInit, scene.fieldSize(Trade::SceneField::Mesh)};
    /* To test all overloads */
    if(data.globalTransformation3D != Matrix4{}) {
        if(data.fieldIdInsteadOfName)
            absoluteFieldTransformations3DInto(scene, 2, out, data.globalTransformation3D);
        else
            absoluteFieldTransformations3DInto(scene, Trade::SceneField::Mesh, out, data.globalTransformation3D);
    } else {
        if(data.fieldIdInsteadOfName)
            absoluteFieldTransformations3DInto(scene, 2, out);
        else
            absoluteFieldTransformations3DInto(scene, Trade::SceneField::Mesh, out);
    }

    CORRADE_COMPARE_AS(out, Containers::arrayView<Matrix4>({
        data.globalTransformation3D*
            Matrix4::translation({1.0f, -1.5f, 0.5f})*
            Matrix4::scaling({3.0f, 5.0f, 2.0f}),
        data.globalTransformation3D*
            Matrix4::translation({1.0f, -1.5f, 0.5f})*
            Matrix4::rotationZ(35.0_degf),
        data.globalTransformation3D,
        data.globalTransformation3D*
            Matrix4::translation({1.0f, -1.5f, 0.5f})*
            Matrix4::rotationZ(35.0_degf),
        data.globalTransformation3D*
            Matrix4::translation({1.0f, -1.5f, 0.5f})*
            Matrix4::scaling({3.0f, 5.0f, 2.0f})
    }), TestSuite::Compare::Container);
}

void HierarchyTest::absoluteFieldTransformationsIntoInvalidSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Data {
        UnsignedInt mapping;
        UnsignedInt mesh;
    } data[5]{};

    Trade::SceneData scene2D{Trade::SceneMappingType::UnsignedInt, 1, {}, data, {
        Trade::SceneFieldData{Trade::SceneField::Parent, Trade::SceneMappingType::UnsignedInt, nullptr, Trade::SceneFieldType::Int, nullptr},
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::stridedArrayView(data).slice(&Data::mapping),
            Containers::stridedArrayView(data).slice(&Data::mesh)},
        Trade::SceneFieldData{Trade::SceneField::Transformation, Trade::SceneMappingType::UnsignedInt, nullptr, Trade::SceneFieldType::Matrix3x3, nullptr}
    }};
    Trade::SceneData scene3D{Trade::SceneMappingType::UnsignedInt, 1, {}, data, {
        Trade::SceneFieldData{Trade::SceneField::Parent, Trade::SceneMappingType::UnsignedInt, nullptr, Trade::SceneFieldType::Int, nullptr},
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::stridedArrayView(data).slice(&Data::mapping),
            Containers::stridedArrayView(data).slice(&Data::mesh)},
        Trade::SceneFieldData{Trade::SceneField::Transformation, Trade::SceneMappingType::UnsignedInt, nullptr, Trade::SceneFieldType::Matrix4x4, nullptr}
    }};

    Matrix3 transformations2D[6];
    Matrix4 transformations3D[4];

    Containers::String out;
    Error redirectError{&out};
    absoluteFieldTransformations2DInto(scene2D, Trade::SceneField::Mesh, transformations2D);
    absoluteFieldTransformations2DInto(scene2D, 1, transformations2D);
    absoluteFieldTransformations3DInto(scene3D, Trade::SceneField::Mesh, transformations3D);
    absoluteFieldTransformations3DInto(scene3D, 1, transformations3D);
    CORRADE_COMPARE(out,
        "SceneTools::absoluteFieldTransformationsInto(): bad output size, expected 5 but got 6\n"
        "SceneTools::absoluteFieldTransformationsInto(): bad output size, expected 5 but got 6\n"
        "SceneTools::absoluteFieldTransformationsInto(): bad output size, expected 5 but got 4\n"
        "SceneTools::absoluteFieldTransformationsInto(): bad output size, expected 5 but got 4\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneTools::Test::HierarchyTest)
