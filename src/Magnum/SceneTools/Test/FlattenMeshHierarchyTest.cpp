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
#include <Corrade/Containers/Triple.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/SceneTools/FlattenMeshHierarchy.h"
#include "Magnum/Trade/SceneData.h"

namespace Magnum { namespace SceneTools { namespace Test { namespace {

struct FlattenMeshHierarchyTest: TestSuite::Tester {
    explicit FlattenMeshHierarchyTest();

    void test2D();
    void test3D();
    void not2DNot3D();
    void noParentField();
    void noMeshField();
};

using namespace Math::Literals;

struct {
    const char* name;
    Matrix3 globalTransformation2D;
    Matrix4 globalTransformation3D;
    std::size_t parentsToExclude, transformationsToExclude, meshesToExclude;
    std::size_t expectedOutputSize;
} TestData[]{
    {"", {}, {},
        0, 2, 3,
        5},
    {"global transformation",
        Matrix3::scaling(Vector2{0.5f}), Matrix4::scaling(Vector3{0.5f}),
        0, 2, 3,
        5},
    {"transformations not part of the hierarchy", {}, {},
        0, 0, 3,
        5},
    {"meshes not part of the hierarchy", {}, {},
        0, 2, 0,
        5},
    {"transformations and meshes not part of the hierarchy", {}, {},
        0, 0, 0,
        5},
    {"no parents", {}, {},
        9, 2, 3,
        0},
    {"no meshes", {}, {},
        0, 2, 8,
        0},
};

FlattenMeshHierarchyTest::FlattenMeshHierarchyTest() {
    addInstancedTests({&FlattenMeshHierarchyTest::test2D,
                       &FlattenMeshHierarchyTest::test3D},
        Containers::arraySize(TestData));

    addTests({&FlattenMeshHierarchyTest::not2DNot3D,
              &FlattenMeshHierarchyTest::noParentField,
              &FlattenMeshHierarchyTest::noMeshField});
}

void FlattenMeshHierarchyTest::test2D() {
    auto&& instanceData = TestData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);

    struct Data {
        /* Using smaller types to verify we don't have unnecessarily hardcoded
           32-bit types */
        struct Parent {
            UnsignedShort object;
            Byte parent;
        } parents[9];

        struct Transformation {
            UnsignedShort object;
            Matrix3 transformation;
        } transforms[7];

        struct Mesh {
            UnsignedShort object;
            UnsignedShort mesh;
            Short meshMaterial;
        } meshes[8];
    } data[]{{
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
        {{2, Matrix3::scaling({3.0f, 5.0f})},
         {1, Matrix3::translation({1.0f, -1.5f})},
         /* Same absolute transform as node 2 */
         {16, Matrix3::translation({1.0f, -1.5f})*
              Matrix3::scaling({3.0f, 5.0f})},
         {7, Matrix3::scaling({2.0f, 1.0f})},
         {5, Matrix3::rotation(35.0_degf)},
         /* These are not part of the hierarchy */
         {32, Matrix3::translation({1.0f, 0.5f})},
         {17, Matrix3::translation({2.0f, 1.0f})},
        },
        {{0, 262, 33},
         {32, 155, 47},
         {0, 127, -1},
         /* The above are not part of the hierarchy */
         {2, 113, 96},
         {3, 266, 74},
         {4, 525, 33},
         {3, 422, -1},
         {16, 113, 96}}
    }};

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 33, {}, data, {
        /* To verify it doesn't just pick the first field ever */
        Trade::SceneFieldData{Trade::SceneField::Camera, Trade::SceneMappingType::UnsignedShort, nullptr, Trade::SceneFieldType::UnsignedInt, nullptr},
        Trade::SceneFieldData{Trade::SceneField::Parent,
            Containers::stridedArrayView(data->parents)
                .slice(&Data::Parent::object)
                .exceptSuffix(instanceData.parentsToExclude),
            Containers::stridedArrayView(data->parents)
                .slice(&Data::Parent::parent)
                .exceptSuffix(instanceData.parentsToExclude)},
        Trade::SceneFieldData{Trade::SceneField::Transformation,
            Containers::stridedArrayView(data->transforms)
                .slice(&Data::Transformation::object)
                .exceptSuffix(instanceData.transformationsToExclude),
            Containers::stridedArrayView(data->transforms)
                .slice(&Data::Transformation::transformation)
                .exceptSuffix(instanceData.transformationsToExclude)},
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::stridedArrayView(data->meshes)
                .slice(&Data::Mesh::object)
                .exceptPrefix(instanceData.meshesToExclude),
            Containers::stridedArrayView(data->meshes)
                .slice(&Data::Mesh::mesh)
                .exceptPrefix(instanceData.meshesToExclude)},
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
            Containers::stridedArrayView(data->meshes)
                .slice(&Data::Mesh::object)
                .exceptPrefix(instanceData.meshesToExclude),
            Containers::stridedArrayView(data->meshes)
                .slice(&Data::Mesh::meshMaterial)
                .exceptPrefix(instanceData.meshesToExclude)},
    }};

    Containers::Array<Containers::Triple<UnsignedInt, Int, Matrix3>> out;
    /* To test the parameter-less overload also */
    if(instanceData.globalTransformation2D != Matrix3{})
        out = flattenMeshHierarchy2D(scene, instanceData.globalTransformation2D);
    else
        out = flattenMeshHierarchy2D(scene);

    CORRADE_EXPECT_FAIL_IF(instanceData.meshesToExclude == 0 || instanceData.parentsToExclude != 0,
        "Meshes that are not part of the hierarchy are not excluded at the moment.");
    CORRADE_COMPARE_AS(out, (Containers::arrayView<Containers::Triple<UnsignedInt, Int, Matrix3>>({
        {113, 96, instanceData.globalTransformation2D*
                  Matrix3::translation({1.0f, -1.5f})*
                  Matrix3::scaling({3.0f, 5.0f})},
        {266, 74, instanceData.globalTransformation2D*
                  Matrix3::translation({1.0f, -1.5f})*
                  Matrix3::rotation(35.0_degf)},
        {525, 33, instanceData.globalTransformation2D},
        {422, -1, instanceData.globalTransformation2D*
                  Matrix3::translation({1.0f, -1.5f})*
                  Matrix3::rotation(35.0_degf)},
        {113, 96, instanceData.globalTransformation2D*
                  Matrix3::translation({1.0f, -1.5f})*
                  Matrix3::scaling({3.0f, 5.0f})}
    })).prefix(instanceData.expectedOutputSize), TestSuite::Compare::Container);
}

void FlattenMeshHierarchyTest::test3D() {
    auto&& instanceData = TestData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);

    /* Basically the same as test2D(), except that it has 3D transforms.
       Putting both into the same test case would be a bit too nasty. */

    struct Data {
        /* Using smaller types to verify we don't have unnecessarily hardcoded
           32-bit types */
        struct Parent {
            UnsignedShort object;
            Byte parent;
        } parents[9];

        struct Transformation {
            UnsignedShort object;
            Matrix4 transformation;
        } transforms[7];

        struct Mesh {
            UnsignedShort object;
            UnsignedShort mesh;
            Short meshMaterial;
        } meshes[8];
    } data[]{{
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
        {{2, Matrix4::scaling({3.0f, 5.0f, 2.0f})},
         {1, Matrix4::translation({1.0f, -1.5f, 0.5f})},
         /* Same absolute transform as node 2 */
         {16, Matrix4::translation({1.0f, -1.5f, 0.5f})*
              Matrix4::scaling({3.0f, 5.0f, 2.0f})},
         {7, Matrix4::scaling({2.0f, 1.0f, 0.5f})},
         {5, Matrix4::rotationZ(35.0_degf)},
         /* These are not part of the hierarchy */
         {32, Matrix4::translation({1.0f, 0.5f, 2.0f})},
         {17, Matrix4::translation({2.0f, 1.0f, 4.0f})},
        },
        {{0, 262, 33},
         {32, 155, 47},
         {0, 127, -1},
         /* The above are not part of the hierarchy */
         {2, 113, 96},
         {3, 266, 74},
         {4, 525, 33},
         {3, 422, -1},
         {16, 113, 96}}
    }};

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 33, {}, data, {
        /* To verify it doesn't just pick the first field ever */
        Trade::SceneFieldData{Trade::SceneField::Camera, Trade::SceneMappingType::UnsignedShort, nullptr, Trade::SceneFieldType::UnsignedInt, nullptr},
        Trade::SceneFieldData{Trade::SceneField::Parent,
            Containers::stridedArrayView(data->parents)
                .slice(&Data::Parent::object)
                .exceptSuffix(instanceData.parentsToExclude),
            Containers::stridedArrayView(data->parents)
                .slice(&Data::Parent::parent)
                .exceptSuffix(instanceData.parentsToExclude)},
        Trade::SceneFieldData{Trade::SceneField::Transformation,
            Containers::stridedArrayView(data->transforms)
                .slice(&Data::Transformation::object)
                .exceptSuffix(instanceData.transformationsToExclude),
            Containers::stridedArrayView(data->transforms)
                .slice(&Data::Transformation::transformation)
                .exceptSuffix(instanceData.transformationsToExclude)},
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::stridedArrayView(data->meshes)
                .slice(&Data::Mesh::object)
                .exceptPrefix(instanceData.meshesToExclude),
            Containers::stridedArrayView(data->meshes)
                .slice(&Data::Mesh::mesh)
                .exceptPrefix(instanceData.meshesToExclude)},
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
            Containers::stridedArrayView(data->meshes)
                .slice(&Data::Mesh::object)
                .exceptPrefix(instanceData.meshesToExclude),
            Containers::stridedArrayView(data->meshes)
                .slice(&Data::Mesh::meshMaterial)
                .exceptPrefix(instanceData.meshesToExclude)},
    }};

    Containers::Array<Containers::Triple<UnsignedInt, Int, Matrix4>> out;
    /* To test the parameter-less overload also */
    if(instanceData.globalTransformation3D != Matrix4{})
        out = flattenMeshHierarchy3D(scene, instanceData.globalTransformation3D);
    else
        out = flattenMeshHierarchy3D(scene);

    CORRADE_EXPECT_FAIL_IF(instanceData.meshesToExclude == 0 || instanceData.parentsToExclude != 0,
        "Meshes that are not part of the hierarchy are not excluded at the moment.");
    CORRADE_COMPARE_AS(out, (Containers::arrayView<Containers::Triple<UnsignedInt, Int, Matrix4>>({
        {113, 96, instanceData.globalTransformation3D*
                  Matrix4::translation({1.0f, -1.5f, 0.5f})*
                  Matrix4::scaling({3.0f, 5.0f, 2.0f})},
        {266, 74, instanceData.globalTransformation3D*
                  Matrix4::translation({1.0f, -1.5f, 0.5f})*
                  Matrix4::rotationZ(35.0_degf)},
        {525, 33, instanceData.globalTransformation3D},
        {422, -1, instanceData.globalTransformation3D*
                  Matrix4::translation({1.0f, -1.5f, 0.5f})*
                  Matrix4::rotationZ(35.0_degf)},
        {113, 96, instanceData.globalTransformation3D*
                  Matrix4::translation({1.0f, -1.5f, 0.5f})*
                  Matrix4::scaling({3.0f, 5.0f, 2.0f})}
    })).prefix(instanceData.expectedOutputSize), TestSuite::Compare::Container);
}

void FlattenMeshHierarchyTest::not2DNot3D() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {}};

    std::ostringstream out;
    Error redirectError{&out};
    flattenMeshHierarchy2D(scene);
    flattenMeshHierarchy3D(scene);
    CORRADE_COMPARE(out.str(),
        "SceneTools::flattenMeshHierarchy(): the scene is not 2D\n"
        "SceneTools::flattenMeshHierarchy(): the scene is not 3D\n");
}

void FlattenMeshHierarchyTest::noParentField() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {
        Trade::SceneFieldData{Trade::SceneField::Transformation, Trade::SceneMappingType::UnsignedInt, nullptr, Trade::SceneFieldType::Matrix3x3, nullptr}
    }};

    std::ostringstream out;
    Error redirectError{&out};
    flattenMeshHierarchy2D(scene);
    CORRADE_COMPARE(out.str(),
        "SceneTools::flattenMeshHierarchy(): the scene has no hierarchy\n");
}

void FlattenMeshHierarchyTest::noMeshField() {
    Trade::SceneData scene{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {
        Trade::SceneFieldData{Trade::SceneField::Parent, Trade::SceneMappingType::UnsignedInt, nullptr, Trade::SceneFieldType::Int, nullptr},
        Trade::SceneFieldData{Trade::SceneField::Transformation, Trade::SceneMappingType::UnsignedInt, nullptr, Trade::SceneFieldType::Matrix3x3, nullptr}
    }};

    /* This should not blow up, just return nothing */
    CORRADE_COMPARE_AS(flattenMeshHierarchy2D(scene),
        (Containers::ArrayView<const Containers::Triple<UnsignedInt, Int, Matrix3>>{}),
        TestSuite::Compare::Container);
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneTools::Test::FlattenMeshHierarchyTest)
