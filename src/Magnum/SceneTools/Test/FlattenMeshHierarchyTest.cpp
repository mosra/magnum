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

#include <Corrade/Containers/String.h>
#include <Corrade/Containers/Triple.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>

/* There's no better way to disable file deprecation warnings */
#define _MAGNUM_NO_DEPRECATED_FLATTENMESHHIERARCHY

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

    void into2D();
    void into3D();
    void intoInvalidSize();
};

using namespace Math::Literals;

const struct {
    const char* name;
    Matrix3 globalTransformation2D;
    Matrix4 globalTransformation3D;
    std::size_t transformationsToExclude, meshesToExclude;
    std::size_t expectedOutputSize;
} TestData[]{
    {"", {}, {},
        2, 0,
        5},
    {"global transformation",
        Matrix3::scaling(Vector2{0.5f}), Matrix4::scaling(Vector3{0.5f}),
        2, 0,
        5},
    {"transformations not part of the hierarchy", {}, {},
        0, 0,
        5},
    {"no meshes", {}, {},
        2, 5,
        0},
};

const struct {
    const char* name;
    Matrix3 globalTransformation2D;
    Matrix4 globalTransformation3D;
    std::size_t expectedOutputSize;
} IntoData[]{
    {"", {}, {},
        5},
    {"global transformation",
        Matrix3::scaling(Vector2{0.5f}), Matrix4::scaling(Vector3{0.5f}),
        5},
};

FlattenMeshHierarchyTest::FlattenMeshHierarchyTest() {
    addInstancedTests({&FlattenMeshHierarchyTest::test2D,
                       &FlattenMeshHierarchyTest::test3D},
        Containers::arraySize(TestData));

    addTests({&FlattenMeshHierarchyTest::not2DNot3D,
              &FlattenMeshHierarchyTest::noParentField,
              &FlattenMeshHierarchyTest::noMeshField});

    addInstancedTests({&FlattenMeshHierarchyTest::into2D,
                       &FlattenMeshHierarchyTest::into3D},
        Containers::arraySize(IntoData));

    addTests({&FlattenMeshHierarchyTest::intoInvalidSize});
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
        Short meshMaterial;
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
    {{2, 113, 96},
     {3, 266, 74},
     {4, 525, 33},
     {3, 422, -1},
     {16, 113, 96}}
}};

void FlattenMeshHierarchyTest::test2D() {
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
        Trade::SceneFieldData{Trade::SceneField::Transformation,
            Containers::stridedArrayView(Data->transforms)
                .slice(&Scene::Transformation::object)
                .exceptSuffix(data.transformationsToExclude),
            Containers::stridedArrayView(Data->transforms)
                .slice(&Scene::Transformation::transformation2D)
                .exceptSuffix(data.transformationsToExclude)},
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::stridedArrayView(Data->meshes)
                .slice(&Scene::Mesh::object)
                .exceptSuffix(data.meshesToExclude),
            Containers::stridedArrayView(Data->meshes)
                .slice(&Scene::Mesh::mesh)
                .exceptSuffix(data.meshesToExclude)},
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
            Containers::stridedArrayView(Data->meshes)
                .slice(&Scene::Mesh::object)
                .exceptSuffix(data.meshesToExclude),
            Containers::stridedArrayView(Data->meshes)
                .slice(&Scene::Mesh::meshMaterial)
                .exceptSuffix(data.meshesToExclude)},
    }};

    Containers::Array<Containers::Triple<UnsignedInt, Int, Matrix3>> out;
    /* To test the parameter-less overload also */
    CORRADE_IGNORE_DEPRECATED_PUSH
    if(data.globalTransformation2D != Matrix3{})
        out = flattenMeshHierarchy2D(scene, data.globalTransformation2D);
    else
        out = flattenMeshHierarchy2D(scene);
    CORRADE_IGNORE_DEPRECATED_POP

    CORRADE_COMPARE_AS(out, (Containers::arrayView<Containers::Triple<UnsignedInt, Int, Matrix3>>({
        {113, 96, data.globalTransformation2D*
                  Matrix3::translation({1.0f, -1.5f})*
                  Matrix3::scaling({3.0f, 5.0f})},
        {266, 74, data.globalTransformation2D*
                  Matrix3::translation({1.0f, -1.5f})*
                  Matrix3::rotation(35.0_degf)},
        {525, 33, data.globalTransformation2D},
        {422, -1, data.globalTransformation2D*
                  Matrix3::translation({1.0f, -1.5f})*
                  Matrix3::rotation(35.0_degf)},
        {113, 96, data.globalTransformation2D*
                  Matrix3::translation({1.0f, -1.5f})*
                  Matrix3::scaling({3.0f, 5.0f})}
    })).prefix(data.expectedOutputSize), TestSuite::Compare::Container);
}

void FlattenMeshHierarchyTest::test3D() {
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
        Trade::SceneFieldData{Trade::SceneField::Transformation,
            Containers::stridedArrayView(Data->transforms)
                .slice(&Scene::Transformation::object)
                .exceptSuffix(data.transformationsToExclude),
            Containers::stridedArrayView(Data->transforms)
                .slice(&Scene::Transformation::transformation3D)
                .exceptSuffix(data.transformationsToExclude)},
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::stridedArrayView(Data->meshes)
                .slice(&Scene::Mesh::object)
                .exceptSuffix(data.meshesToExclude),
            Containers::stridedArrayView(Data->meshes)
                .slice(&Scene::Mesh::mesh)
                .exceptSuffix(data.meshesToExclude)},
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
            Containers::stridedArrayView(Data->meshes)
                .slice(&Scene::Mesh::object)
                .exceptSuffix(data.meshesToExclude),
            Containers::stridedArrayView(Data->meshes)
                .slice(&Scene::Mesh::meshMaterial)
                .exceptSuffix(data.meshesToExclude)},
    }};

    Containers::Array<Containers::Triple<UnsignedInt, Int, Matrix4>> out;
    /* To test the parameter-less overload also */
    CORRADE_IGNORE_DEPRECATED_PUSH
    if(data.globalTransformation3D != Matrix4{})
        out = flattenMeshHierarchy3D(scene, data.globalTransformation3D);
    else
        out = flattenMeshHierarchy3D(scene);
    CORRADE_IGNORE_DEPRECATED_POP

    CORRADE_COMPARE_AS(out, (Containers::arrayView<Containers::Triple<UnsignedInt, Int, Matrix4>>({
        {113, 96, data.globalTransformation3D*
                  Matrix4::translation({1.0f, -1.5f, 0.5f})*
                  Matrix4::scaling({3.0f, 5.0f, 2.0f})},
        {266, 74, data.globalTransformation3D*
                  Matrix4::translation({1.0f, -1.5f, 0.5f})*
                  Matrix4::rotationZ(35.0_degf)},
        {525, 33, data.globalTransformation3D},
        {422, -1, data.globalTransformation3D*
                  Matrix4::translation({1.0f, -1.5f, 0.5f})*
                  Matrix4::rotationZ(35.0_degf)},
        {113, 96, data.globalTransformation3D*
                  Matrix4::translation({1.0f, -1.5f, 0.5f})*
                  Matrix4::scaling({3.0f, 5.0f, 2.0f})}
    })).prefix(data.expectedOutputSize), TestSuite::Compare::Container);
}

void FlattenMeshHierarchyTest::not2DNot3D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* Used to assert even on an empty scene, now it does an early-out if the
       mesh field doesn't exist because absoluteFieldTransformations() would
       assert instead. That behavioral change is fine for a deprecated API. */
    Trade::SceneData scene{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {
        Trade::SceneFieldData{Trade::SceneField::Mesh, Trade::SceneMappingType::UnsignedInt, nullptr, Trade::SceneFieldType::UnsignedInt, nullptr},
    }};

    Containers::String out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    flattenMeshHierarchy2D(scene);
    flattenMeshHierarchy3D(scene);
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out,
        "SceneTools::absoluteFieldTransformations(): the scene is not 2D\n"
        "SceneTools::absoluteFieldTransformations(): the scene is not 3D\n");
}

void FlattenMeshHierarchyTest::noParentField() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* Used to assert even on an empty scene, now it does an early-out if the
       mesh field doesn't exist because absoluteFieldTransformations() would
       assert instead. That behavioral change is fine for a deprecated API. */
    Trade::SceneData scene{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {
        Trade::SceneFieldData{Trade::SceneField::Mesh, Trade::SceneMappingType::UnsignedInt, nullptr, Trade::SceneFieldType::UnsignedInt, nullptr},
        Trade::SceneFieldData{Trade::SceneField::Transformation, Trade::SceneMappingType::UnsignedInt, nullptr, Trade::SceneFieldType::Matrix3x3, nullptr}
    }};

    Containers::String out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    flattenMeshHierarchy2D(scene);
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out,
        "SceneTools::absoluteFieldTransformations(): the scene has no hierarchy\n");
}

void FlattenMeshHierarchyTest::noMeshField() {
    Trade::SceneData scene{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {
        Trade::SceneFieldData{Trade::SceneField::Parent, Trade::SceneMappingType::UnsignedInt, nullptr, Trade::SceneFieldType::Int, nullptr},
        Trade::SceneFieldData{Trade::SceneField::Transformation, Trade::SceneMappingType::UnsignedInt, nullptr, Trade::SceneFieldType::Matrix3x3, nullptr}
    }};

    /* This should not blow up, just return nothing */
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE_AS(flattenMeshHierarchy2D(scene),
        (Containers::ArrayView<const Containers::Triple<UnsignedInt, Int, Matrix3>>{}),
        TestSuite::Compare::Container);
    CORRADE_IGNORE_DEPRECATED_POP
}

void FlattenMeshHierarchyTest::into2D() {
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
    /* To test the parameter-less overload also */
    CORRADE_IGNORE_DEPRECATED_PUSH
    if(data.globalTransformation2D != Matrix3{})
        flattenMeshHierarchy2DInto(scene, out, data.globalTransformation2D);
    else
        flattenMeshHierarchy2DInto(scene, out);
    CORRADE_IGNORE_DEPRECATED_POP

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

void FlattenMeshHierarchyTest::into3D() {
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
    /* To test the parameter-less overload also */
    CORRADE_IGNORE_DEPRECATED_PUSH
    if(data.globalTransformation3D != Matrix4{})
        flattenMeshHierarchy3DInto(scene, out, data.globalTransformation3D);
    else
        flattenMeshHierarchy3DInto(scene, out);
    CORRADE_IGNORE_DEPRECATED_POP

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

void FlattenMeshHierarchyTest::intoInvalidSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Data {
        UnsignedInt mapping;
        UnsignedInt mesh;
    } data[5]{};

    Trade::SceneData scene2D{Trade::SceneMappingType::UnsignedInt, 1, {}, data, {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::stridedArrayView(data).slice(&Data::mapping),
            Containers::stridedArrayView(data).slice(&Data::mesh)},
        Trade::SceneFieldData{Trade::SceneField::Parent, Trade::SceneMappingType::UnsignedInt, nullptr, Trade::SceneFieldType::Int, nullptr},
        Trade::SceneFieldData{Trade::SceneField::Transformation, Trade::SceneMappingType::UnsignedInt, nullptr, Trade::SceneFieldType::Matrix3x3, nullptr}
    }};
    Trade::SceneData scene3D{Trade::SceneMappingType::UnsignedInt, 1, {}, data, {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::stridedArrayView(data).slice(&Data::mapping),
            Containers::stridedArrayView(data).slice(&Data::mesh)},
        Trade::SceneFieldData{Trade::SceneField::Parent, Trade::SceneMappingType::UnsignedInt, nullptr, Trade::SceneFieldType::Int, nullptr},
        Trade::SceneFieldData{Trade::SceneField::Transformation, Trade::SceneMappingType::UnsignedInt, nullptr, Trade::SceneFieldType::Matrix4x4, nullptr}
    }};

    Matrix3 transformations2D[6];
    Matrix4 transformations3D[4];

    Containers::String out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    flattenMeshHierarchy2DInto(scene2D, transformations2D);
    flattenMeshHierarchy3DInto(scene3D, transformations3D);
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out,
        "SceneTools::absoluteFieldTransformationsInto(): bad output size, expected 5 but got 6\n"
        "SceneTools::absoluteFieldTransformationsInto(): bad output size, expected 5 but got 4\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneTools::Test::FlattenMeshHierarchyTest)
