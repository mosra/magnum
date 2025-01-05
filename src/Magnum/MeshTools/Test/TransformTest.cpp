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

#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/StaticArray.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/Format.h>

#include "Magnum/Math/Half.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Magnum.h"
#include "Magnum/MeshTools/Transform.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct TransformTest: TestSuite::Tester {
    explicit TransformTest();

    void transformVectors2D();
    void transformVectors3D();

    void transformPoints2D();
    void transformPoints3D();

    template<class T> void meshData2D();
    void meshData2DNoPosition();
    void meshData2DNot2D();
    void meshData2DImplementationSpecificIndexType();
    void meshData2DImplementationSpecificVertexFormat();
    void meshData2DRvaluePassthrough();
    void meshData2DRvaluePassthroughIndexDataNotOwned();
    void meshData2DRvaluePassthroughVertexDataNotOwned();
    void meshData2DRvaluePassthroughNoPosition();
    void meshData2DRvaluePassthroughWrongFormat();
    /* in-place variant called from the others and as such tested sufficiently,
       except for the asserts below */
    void meshData2DInPlaceNotMutable();
    void meshData2DInPlaceNoPosition();
    void meshData2DInPlaceWrongFormat();

    template<class T, class U, class V, class W> void meshData3D();
    void meshData3DNoPosition();
    void meshData3DNot3D();
    void meshData3DImplementationSpecificIndexType();
    void meshData3DImplementationSpecificVertexFormat();
    void meshData3DRvaluePassthrough();
    void meshData3DRvaluePassthroughIndexDataNotOwned();
    void meshData3DRvaluePassthroughVertexDataNotOwned();
    void meshData3DRvaluePassthroughNoPosition();
    void meshData3DRvaluePassthroughWrongFormat();
    /* in-place variant called from the others and as such tested sufficiently,
       except for the asserts below */
    void meshData3DInPlaceNotMutable();
    void meshData3DInPlaceNoPosition();
    void meshData3DInPlaceWrongFormat();

    template<class T> void meshDataTextureCoordinates2D();
    void meshDataTextureCoordinates2DNoCoordinates();
    void meshDataTextureCoordinates2DImplementationSpecificIndexType();
    void meshDataTextureCoordinates2DImplementationSpecificVertexFormat();
    void meshDataTextureCoordinates2DRvaluePassthrough();
    void meshDataTextureCoordinates2DRvaluePassthroughIndexDataNotOwned();
    void meshDataTextureCoordinates2DRvaluePassthroughVertexDataNotOwned();
    void meshDataTextureCoordinates2DRvaluePassthroughNoCoordinates();
    void meshDataTextureCoordinates2DRvaluePassthroughWrongFormat();
    /* in-place variant called from the others and as such tested sufficiently,
       except for the asserts below */
    void meshDataTextureCoordinates2DInPlaceNotMutable();
    void meshDataTextureCoordinates2DInPlaceNoCoordinates();
    void meshDataTextureCoordinates2DInPlaceWrongFormat();
};

using namespace Math::Literals;

const struct {
    const char* name;
    bool indexed;
    UnsignedInt id;
    Int morphTargetId;
    Matrix3 transformation;
} MeshData2DData[]{
    {"", false, 0, -1,
        Matrix3::translation({1.5f, 3.0f})*
        Matrix3::rotation(35.0_degf)},
    {"indexed", true, 0, -1,
        Matrix3::translation({1.5f, 3.0f})*
        Matrix3::rotation(35.0_degf)},
    {"second set", false, 1, -1,
        Matrix3::translation({1.5f, 3.0f})*
        Matrix3::rotation(35.0_degf)},
    {"morph target", false, 0, 37,
        Matrix3::translation({1.5f, 3.0f})*
        Matrix3::rotation(35.0_degf)}
    /** @todo negative scaling that flips face winding */
};

const struct {
    const char* name;
    UnsignedInt id;
    Int morphTargetId;
} NoAttributeData[]{
    {"", 1, -1},
    {"morph target", 0, 37}
};

const struct {
    const char* name;
    bool indexed;
    UnsignedInt id;
    Int morphTargetId;
} MeshData2DRvaluePassthroughData[]{
    {"", false, 0, -1},
    {"indexed", true, 0, -1},
    {"second set", false, 1, -1},
    {"morph target", false, 0, 37}
};

const struct {
    const char* name;
    bool indexed;
    bool tangents, tangents4, bitangents, normals;
    UnsignedInt id;
    Int morphTargetId;
    Matrix4 transformation;
    Matrix3x3 expectedNormalTransformation;
} MeshData3DData[]{
    {"", false, false, false, false, false, 0, -1,
        Matrix4::translation({1.5f, 3.0f, -0.5f})*
        Matrix4::rotationX(35.0_degf),
        Matrix4::rotationX(35.0_degf).rotationScaling()},
    {"indexed", true, false, false, false, false, 0, -1,
        Matrix4::rotationX(35.0_degf),
        Matrix4::rotationX(35.0_degf).rotationScaling()},
    {"normals", false, false, false, false, true, 0, -1,
        Matrix4::translation({1.5f, 3.0f, -0.5f})*
        Matrix4::rotationX(35.0_degf),
        Matrix4::rotationX(35.0_degf).rotationScaling()},
    {"tangents", false, true, false, false, false, 0, -1,
        Matrix4::translation({1.5f, 3.0f, -0.5f})*
        Matrix4::rotationX(35.0_degf),
        Matrix4::rotationX(35.0_degf).rotationScaling()},
    {"four-component tangents", false, true, true, false, false, 0, -1,
        Matrix4::translation({1.5f, 3.0f, -0.5f})*
        Matrix4::rotationX(35.0_degf),
        Matrix4::rotationX(35.0_degf).rotationScaling()},
    {"bitangents", false, false, false, true, false, 0, -1,
        Matrix4::translation({1.5f, 3.0f, -0.5f})*
        Matrix4::rotationX(35.0_degf),
        Matrix4::rotationX(35.0_degf).rotationScaling()},
    {"all + indexed", true, true, true, true, true, 0, -1,
        Matrix4::translation({1.5f, 3.0f, -0.5f})*
        Matrix4::rotationX(35.0_degf),
        Matrix4::rotationX(35.0_degf).rotationScaling()},
    {"second set", false, false, false, false, false, 1, -1,
        Matrix4::rotationX(35.0_degf),
        Matrix4::rotationX(35.0_degf).rotationScaling()},
    {"second set + normals", false, false, false, false, true, 1, -1,
        Matrix4::rotationX(35.0_degf),
        Matrix4::rotationX(35.0_degf).rotationScaling()},
    {"second set + tangents", false, true, false, false, false, 1, -1,
        Matrix4::rotationX(35.0_degf),
        Matrix4::rotationX(35.0_degf).rotationScaling()},
    {"second set + four-component tangents", false, true, true, false, false, 1, -1,
        Matrix4::rotationX(35.0_degf),
        Matrix4::rotationX(35.0_degf).rotationScaling()},
    {"second set + bitangents", false, false, false, true, false, 1, -1,
        Matrix4::rotationX(35.0_degf),
        Matrix4::rotationX(35.0_degf).rotationScaling()},
    {"second set + all + indexed", true, true, true, true, true, 1, -1,
        Matrix4::rotationX(35.0_degf),
        Matrix4::rotationX(35.0_degf).rotationScaling()},
    {"morph target", false, false, false, false, false, 0, 37,
        Matrix4::rotationX(35.0_degf),
        Matrix4::rotationX(35.0_degf).rotationScaling()},
    {"morph target + tangents", false, true, false, false, false, 0, 37,
        Matrix4::rotationX(35.0_degf),
        Matrix4::rotationX(35.0_degf).rotationScaling()},
    {"morph target + allindexed", false, true, true, true, true, 0, 37,
        Matrix4::rotationX(35.0_degf),
        Matrix4::rotationX(35.0_degf).rotationScaling()},
    {"non-uniform scaling", false, true, true, true, true, 0, -1,
        Matrix4::translation({1.5f, 3.0f, -0.5f})*
        Matrix4::rotationX(35.0_degf)*
        Matrix4::scaling({2.0f, 1.0f, 0.5f}),
       (Matrix4::rotationX(35.0_degf)*
        Matrix4::scaling({2.0f, 1.0f, 0.5f})).normalMatrix()},
    /** @todo negative scaling that flips face winding */
};

const struct {
    const char* name;
    VertexFormat positionFormat;
    Trade::MeshAttribute otherAttribute;
    VertexFormat otherAttributeFormat;
} MeshData3DIMplementationSpecificVertexFormatData[]{
    {"positions", vertexFormatWrap(0xcaca), Trade::MeshAttribute::Position, VertexFormat::Vector3},
    {"normals", VertexFormat::Vector3, Trade::MeshAttribute::Normal, vertexFormatWrap(0xcaca)},
    {"tangents", VertexFormat::Vector3, Trade::MeshAttribute::Tangent, vertexFormatWrap(0xcaca)},
    {"bitangents", VertexFormat::Vector3, Trade::MeshAttribute::Bitangent, vertexFormatWrap(0xcaca)},
};

const struct {
    const char* name;
    bool indexed;
    bool tangents, tangents4, bitangents, normals;
    UnsignedInt id;
    Int morphTargetId;
} MeshData3DRvaluePassthroughData[]{
    {"", false, false, false, false, false, 0, -1},
    {"indexed", true, false, false, false, false, 0, -1},
    {"normals", false, false, false, false, true, 0, -1},
    {"tangents", false, true, false, false, false, 0, -1},
    {"four-component tangents", false, true, true, false, false, 0, -1},
    {"bitangents", false, false, false, true, false, 0, -1},
    {"all + indexed", true, true, true, true, true, 0, -1},
    {"second set", false, false, false, false, false, 1, -1},
    {"second set + normals", false, false, false, false, true, 1, -1},
    {"second set + tangents", false, true, false, false, false, 1, -1},
    {"second set + four-component tangents", false, true, true, false, false, 1, -1},
    {"second set + bitangents", false, false, false, true, false, 1, -1},
    {"second set + all + indexed", true, true, true, true, true, 1, -1},
    {"morph target", false, false, false, false, false, 0, 37},
    {"morph target + tangents", false, true, false, false, false, 0, 37},
    {"morph target + all", false, true, true, true, true, 0, 37}
};

const struct {
    const char* name;
    VertexFormat positionFormat;
    VertexFormat tangentFormat;
    VertexFormat bitangentFormat;
    VertexFormat normalFormat;
    const char* message;
} MeshData3DWrongFormatData[]{
    {"packed positions", VertexFormat::Vector3b,
        VertexFormat{}, VertexFormat{}, VertexFormat{},
        "MeshTools::transform3DInPlace(): expected VertexFormat::Vector3 positions but got VertexFormat::Vector3b\n"},
    {"packed normals", VertexFormat::Vector3,
        VertexFormat{}, VertexFormat{}, VertexFormat::Vector3sNormalized,
        "MeshTools::transform3DInPlace(): expected VertexFormat::Vector3 normals but got VertexFormat::Vector3sNormalized\n"},
    {"packed tangents", VertexFormat::Vector3,
        VertexFormat::Vector3bNormalized, VertexFormat{}, VertexFormat{},
        "MeshTools::transform3DInPlace(): expected VertexFormat::Vector3 or VertexFormat::Vector4 tangents but got VertexFormat::Vector3bNormalized\n"},
    {"packed four-component tangents", VertexFormat::Vector3,
        VertexFormat::Vector4bNormalized, VertexFormat{}, VertexFormat{},
        "MeshTools::transform3DInPlace(): expected VertexFormat::Vector3 or VertexFormat::Vector4 tangents but got VertexFormat::Vector4bNormalized\n"},
    {"packed bitangents", VertexFormat::Vector3,
        VertexFormat{}, VertexFormat::Vector3h, VertexFormat{},
        "MeshTools::transform3DInPlace(): expected VertexFormat::Vector3 bitangents but got VertexFormat::Vector3h\n"}
};

const struct {
    const char* name;
    bool indexed;
    UnsignedInt id;
    Int morphTargetId;
    Matrix3 transformation;
} MeshDataTextureCoordinatesData[]{
    {"", false, 0, -1,
        Matrix3::translation({1.5f, 3.0f})*
        Matrix3::rotation(35.0_degf)},
    {"indexed", true, 0, -1,
        Matrix3::translation({1.5f, 3.0f})*
        Matrix3::rotation(35.0_degf)},
    {"second set", false, 1, -1,
        Matrix3::translation({1.5f, 3.0f})*
        Matrix3::rotation(35.0_degf)},
    {"morph target", false, 0, 37,
        Matrix3::translation({1.5f, 3.0f})*
        Matrix3::rotation(35.0_degf)}
};

const struct {
    const char* name;
    bool indexed;
    UnsignedInt id;
    Int morphTargetId;
} MeshDataTextureCoordinatesRvaluePassthroughData[]{
    {"", false, 0, -1},
    {"indexed", true, 0, -1},
    {"second set", false, 1, -1},
    {"morph target", false, 0, 37}
};

TransformTest::TransformTest() {
    addTests({&TransformTest::transformVectors2D,
              &TransformTest::transformVectors3D,

              &TransformTest::transformPoints2D,
              &TransformTest::transformPoints3D});

    addInstancedTests<TransformTest>({
        &TransformTest::meshData2D<Float>,
        &TransformTest::meshData2D<Half>
    }, Containers::arraySize(MeshData2DData));

    addInstancedTests({&TransformTest::meshData2DNoPosition},
        Containers::arraySize(NoAttributeData));

    addTests({&TransformTest::meshData2DNot2D,
              &TransformTest::meshData2DImplementationSpecificIndexType,
              &TransformTest::meshData2DImplementationSpecificVertexFormat});

    addInstancedTests({&TransformTest::meshData2DRvaluePassthrough},
        Containers::arraySize(MeshData2DRvaluePassthroughData));

    addTests({&TransformTest::meshData2DRvaluePassthroughIndexDataNotOwned,
              &TransformTest::meshData2DRvaluePassthroughVertexDataNotOwned});

    addInstancedTests({&TransformTest::meshData2DRvaluePassthroughNoPosition},
        Containers::arraySize(NoAttributeData));

    addTests({&TransformTest::meshData2DRvaluePassthroughWrongFormat});

    addTests({&TransformTest::meshData2DInPlaceNotMutable});

    addInstancedTests({&TransformTest::meshData2DInPlaceNoPosition},
        Containers::arraySize(NoAttributeData));

    addTests({&TransformTest::meshData2DInPlaceWrongFormat});

    addInstancedTests<TransformTest>({
        &TransformTest::meshData3D<Float, Float, Float, Float>,
        &TransformTest::meshData3D<Half, Float, Half, Float>,
        &TransformTest::meshData3D<Float, Half, Float, Half>,
    }, Containers::arraySize(MeshData3DData));

    addInstancedTests({&TransformTest::meshData3DNoPosition},
        Containers::arraySize(NoAttributeData));

    addTests({&TransformTest::meshData3DNot3D,
              &TransformTest::meshData3DImplementationSpecificIndexType});

    addInstancedTests({&TransformTest::meshData3DImplementationSpecificVertexFormat},
        Containers::arraySize(MeshData3DIMplementationSpecificVertexFormatData));

    addInstancedTests({&TransformTest::meshData3DRvaluePassthrough},
        Containers::arraySize(MeshData3DRvaluePassthroughData));

    addTests({&TransformTest::meshData3DRvaluePassthroughIndexDataNotOwned,
              &TransformTest::meshData3DRvaluePassthroughVertexDataNotOwned});

    addInstancedTests({&TransformTest::meshData3DRvaluePassthroughNoPosition},
        Containers::arraySize(NoAttributeData));

    addInstancedTests({&TransformTest::meshData3DRvaluePassthroughWrongFormat},
        Containers::arraySize(MeshData3DWrongFormatData));

    addTests({&TransformTest::meshData3DInPlaceNotMutable});

    addInstancedTests({&TransformTest::meshData3DInPlaceNoPosition},
        Containers::arraySize(NoAttributeData));

    addInstancedTests({&TransformTest::meshData3DInPlaceWrongFormat},
        Containers::arraySize(MeshData3DWrongFormatData));

    addInstancedTests<TransformTest>({
        &TransformTest::meshDataTextureCoordinates2D<Float>,
        &TransformTest::meshDataTextureCoordinates2D<Half>
    }, Containers::arraySize(MeshDataTextureCoordinatesData));

    addInstancedTests({&TransformTest::meshDataTextureCoordinates2DNoCoordinates},
        Containers::arraySize(NoAttributeData));

    addTests({&TransformTest::meshDataTextureCoordinates2DImplementationSpecificIndexType,
              &TransformTest::meshDataTextureCoordinates2DImplementationSpecificVertexFormat});

    addInstancedTests({&TransformTest::meshDataTextureCoordinates2DRvaluePassthrough},
        Containers::arraySize(MeshDataTextureCoordinatesRvaluePassthroughData));

    addTests({&TransformTest::meshDataTextureCoordinates2DRvaluePassthroughIndexDataNotOwned,
              &TransformTest::meshDataTextureCoordinates2DRvaluePassthroughVertexDataNotOwned});

    addInstancedTests({&TransformTest::meshDataTextureCoordinates2DRvaluePassthroughNoCoordinates},
        Containers::arraySize(NoAttributeData));

    addTests({&TransformTest::meshDataTextureCoordinates2DRvaluePassthroughWrongFormat});

    addTests({&TransformTest::meshDataTextureCoordinates2DInPlaceNotMutable});

    addInstancedTests({&TransformTest::meshDataTextureCoordinates2DInPlaceNoCoordinates},
        Containers::arraySize(NoAttributeData));

    addTests({&TransformTest::meshDataTextureCoordinates2DInPlaceWrongFormat});
}

constexpr Containers::Array2<Vector2> points2D{{
    {-3.0f,   4.0f},
    { 2.5f, -15.0f}
}};

constexpr Containers::Array2<Vector2> points2DRotated{{
    {-4.0f, -3.0f},
    {15.0f,  2.5f}
}};

constexpr Containers::Array2<Vector2> points2DRotatedTranslated{{
    {-4.0f, -4.0f},
    {15.0f,  1.5f}
}};

constexpr Containers::Array2<Vector3> points3D{{
    {-3.0f,   4.0f, 34.0f},
    { 2.5f, -15.0f,  1.5f}
}};

constexpr Containers::Array2<Vector3> points3DRotated{{
    {-4.0f, -3.0f, 34.0f},
    {15.0f,  2.5f,  1.5f}
}};

constexpr Containers::Array2<Vector3> points3DRotatedTranslated{{
    {-4.0f, -4.0f, 34.0f},
    {15.0f,  1.5f,  1.5f}
}};

void TransformTest::transformVectors2D() {
    auto matrix = transformVectors(Matrix3::rotation(Deg(90.0f)), points2D);
    auto complex = transformVectors(Complex::rotation(Deg(90.0f)), points2D);

    CORRADE_COMPARE_AS(matrix, points2DRotated, TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(complex, points2DRotated, TestSuite::Compare::Container);
}

void TransformTest::transformVectors3D() {
    auto matrix = transformVectors(Matrix4::rotationZ(Deg(90.0f)), points3D);
    auto quaternion = transformVectors(Quaternion::rotation(Deg(90.0f), Vector3::zAxis()), points3D);

    CORRADE_COMPARE_AS(matrix, points3DRotated, TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(quaternion, points3DRotated, TestSuite::Compare::Container);
}

void TransformTest::transformPoints2D() {
    auto matrix = transformPoints(
        Matrix3::translation(Vector2::yAxis(-1.0f))*Matrix3::rotation(Deg(90.0f)), points2D);
    auto complex = transformPoints(
        DualComplex::translation(Vector2::yAxis(-1.0f))*DualComplex::rotation(Deg(90.0f)), points2D);

    CORRADE_COMPARE_AS(matrix, points2DRotatedTranslated, TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(complex, points2DRotatedTranslated, TestSuite::Compare::Container);
}

void TransformTest::transformPoints3D() {
    auto matrix = transformPoints(
        Matrix4::translation(Vector3::yAxis(-1.0f))*Matrix4::rotationZ(Deg(90.0f)), points3D);
    auto quaternion = transformPoints(
        DualQuaternion::translation(Vector3::yAxis(-1.0f))*DualQuaternion::rotation(Deg(90.0f), Vector3::zAxis()), points3D);

    CORRADE_COMPARE_AS(matrix, points3DRotatedTranslated, TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(quaternion, points3DRotatedTranslated, TestSuite::Compare::Container);
}

template<class T> void TransformTest::meshData2D() {
    auto&& data = MeshData2DData[testCaseInstanceId()];
    setTestCaseTemplateName(Math::TypeTraits<T>::name());
    setTestCaseDescription(data.name);

    const UnsignedShort indices[]{
        1, 2, 0
    };
    const struct Vertex {
        Vector2b secondaryPosition;
        Math::Vector2<T> position;
        Float somethingElse;
    } vertices[]{
        {{15, -34}, {T(0.0), T(0.0)}, 7.0f},
        {{11, -25}, {T(1.0), T(0.0)}, 5.5f},
        {{16, -27}, {T(0.0), T(2.0)}, 3.0f}
    };
    Containers::StridedArrayView1D<const Vertex> view = vertices;

    Containers::Array<Trade::MeshAttributeData> attributes;
    if(data.id == 1 || data.morphTargetId != -1)
        arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Position, view.slice(&Vertex::secondaryPosition)});
    else {
        CORRADE_COMPARE(data.id, 0);
        CORRADE_COMPARE(data.morphTargetId, -1);
    }
    arrayAppend(attributes, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, view.slice(&Vertex::position), data.morphTargetId},
        Trade::MeshAttributeData{Trade::meshAttributeCustom(0), view.slice(&Vertex::somethingElse)}
    });

    Trade::MeshData mesh{MeshPrimitive::TriangleStrip,
        {}, data.indexed ? Containers::arrayView(indices) : nullptr,
        data.indexed ? Trade::MeshIndexData{indices} : Trade::MeshIndexData{nullptr},
        {}, vertices, Utility::move(attributes)};
    CORRADE_COMPARE(mesh.isIndexed(), data.indexed);

    Trade::MeshData out = transform2D(mesh, data.transformation, data.id, data.morphTargetId);
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::TriangleStrip);

    /* Indices should be preserved */
    if(data.indexed) {
        CORRADE_VERIFY(out.isIndexed());
        CORRADE_COMPARE_AS(out.indices<UnsignedShort>(),
            Containers::arrayView(indices),
            TestSuite::Compare::Container);
    }

    /* The extra attribute should be preserved */
    CORRADE_COMPARE(out.attributeCount(), mesh.attributeCount());
    CORRADE_COMPARE_AS(out.attribute<Float>(Trade::meshAttributeCustom(0)),
        view.slice(&Vertex::somethingElse),
        TestSuite::Compare::Container);

    /* The vertices should be expanded to floats and transformed */
    CORRADE_COMPARE(out.attributeFormat(Trade::MeshAttribute::Position, data.id, data.morphTargetId), VertexFormat::Vector2);
    CORRADE_COMPARE_AS(out.attribute<Vector2>(Trade::MeshAttribute::Position, data.id, data.morphTargetId), Containers::arrayView({
        data.transformation.transformPoint({0.0f, 0.0f}),
        data.transformation.transformPoint({1.0f, 0.0f}),
        data.transformation.transformPoint({0.0f, 2.0f})
    }), TestSuite::Compare::Container);
}

void TransformTest::meshData2DNoPosition() {
    auto&& data = NoAttributeData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData mesh{MeshPrimitive::Points, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector2, nullptr},
    }};

    Containers::String out;
    Error redirectError{&out};
    transform2D(mesh, {}, data.id, data.morphTargetId);
    if(data.morphTargetId == -1)
        CORRADE_COMPARE(out, "MeshTools::transform2D(): the mesh has no positions with index 1\n");
    else
        CORRADE_COMPARE(out, "MeshTools::transform2D(): the mesh has no positions with index 0 in morph target 37\n");
}

void TransformTest::meshData2DNot2D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData mesh{MeshPrimitive::Points, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr}
    }};

    Containers::String out;
    Error redirectError{&out};
    transform2D(mesh, {});
    CORRADE_COMPARE(out, "MeshTools::transform2D(): expected 2D positions but got VertexFormat::Vector3\n");
}

void TransformTest::meshData2DImplementationSpecificIndexType() {
    const UnsignedShort indices[]{3, 1, 2, 0, 2};
    const Vector2 vertices[]{
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {0.0f, 2.0f}
    };
    Trade::MeshData mesh{MeshPrimitive::Points,
        {}, indices, Trade::MeshIndexData{meshIndexTypeWrap(0xcaca), Containers::stridedArrayView(indices).slice(1, 4)},
        {}, vertices, {Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::arrayView(vertices)}}};

    /* Just verify the index data get passed through with no information loss */
    Trade::MeshData out = transform2D(mesh, Matrix3{}, 0, -1, InterleaveFlag::PreserveStridedIndices);
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::Points);
    CORRADE_COMPARE(out.indexType(), meshIndexTypeWrap(0xcaca));
    CORRADE_COMPARE(out.indexOffset(), 2);
    CORRADE_COMPARE(out.indexStride(), 2);
    CORRADE_COMPARE(out.indexCount(), 3);
    CORRADE_COMPARE_AS((Containers::arrayCast<1, const UnsignedShort>(out.indices())),
        Containers::arrayView<UnsignedShort>({1, 2, 0}),
        TestSuite::Compare::Container);
}

void TransformTest::meshData2DImplementationSpecificVertexFormat() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData mesh{MeshPrimitive::Points, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector2, nullptr},
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertexFormatWrap(0xcaca), nullptr},
    }};

    Containers::String out;
    Error redirectError{&out};
    transform2D(mesh, {}, 1);
    CORRADE_COMPARE(out, "MeshTools::transform2D(): positions have an implementation-specific format 0xcaca\n");
}

void TransformTest::meshData2DRvaluePassthrough() {
    auto&& data = MeshData2DRvaluePassthroughData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Array<char> indexData{NoInit, 3*sizeof(UnsignedShort)};
    auto indices = Containers::arrayCast<UnsignedShort>(indexData);
    /* Not using copy({...}) as we'd have no way to compare against the
       original unchanged data */
    const UnsignedShort indicesExpected[]{1, 2, 0};
    Utility::copy(indicesExpected, indices);

    struct Vertex {
        Vector2b secondaryPosition;
        Vector2 position;
    };
    Containers::Array<char> vertexData{NoInit, 3*sizeof(Vertex)};
    auto vertices = stridedArrayView(Containers::arrayCast<Vertex>(vertexData));
    Utility::copy({
        {{15, -34}, {0.0f, 0.0f}},
        {{11, -25}, {1.0f, 0.0f}},
        {{16, -27}, {0.0f, 2.0f}}
    }, vertices);

    Containers::Array<Trade::MeshAttributeData> attributes;
    if(data.id == 1 || data.morphTargetId != -1)
        arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertices.slice(&Vertex::secondaryPosition)});
    else {
        CORRADE_COMPARE(data.id, 0);
        CORRADE_COMPARE(data.morphTargetId, -1);
    }
    arrayAppend(attributes, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertices.slice(&Vertex::position), data.morphTargetId}
    });
    const void* originalAttributeData = attributes;

    Trade::MeshData mesh{MeshPrimitive::TriangleFan,
        data.indexed ? Utility::move(indexData) : nullptr,
        data.indexed ? Trade::MeshIndexData{indices} : Trade::MeshIndexData{nullptr},
        Utility::move(vertexData), Utility::move(attributes)};

    const Matrix3 transformation = Matrix3::rotation(35.0_degf);
    Trade::MeshData out = transform2D(Utility::move(mesh), transformation, data.id, data.morphTargetId);
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::TriangleFan);

    /* Indices should be passed through unchanged */
    if(data.indexed) {
        CORRADE_VERIFY(out.isIndexed());
        CORRADE_COMPARE_AS(out.indices<UnsignedShort>(),
            Containers::arrayView(indicesExpected),
            TestSuite::Compare::Container);
    }

    /* The vertices should be expanded to floats and transformed */
    CORRADE_COMPARE(out.attributeFormat(Trade::MeshAttribute::Position, data.id, data.morphTargetId), VertexFormat::Vector2);
    CORRADE_COMPARE_AS(out.attribute<Vector2>(Trade::MeshAttribute::Position, data.id, data.morphTargetId), Containers::arrayView({
        transformation.transformPoint({0.0f, 0.0f}),
        transformation.transformPoint({1.0f, 0.0f}),
        transformation.transformPoint({0.0f, 2.0f})
    }), TestSuite::Compare::Container);

    /* The memory should point to the original data */
    if(data.indexed)
        CORRADE_COMPARE(out.indexData().data(), static_cast<void*>(indices.data()));
    CORRADE_COMPARE(out.vertexData().data(), static_cast<void*>(vertices.data()));
    CORRADE_COMPARE(out.attributeData().data(), originalAttributeData);
}

void TransformTest::meshData2DRvaluePassthroughIndexDataNotOwned() {
    const UnsignedShort indices[]{
        1, 2, 0
    };
    Containers::Array<char> vertexData{NoInit, 3*sizeof(Vector2)};
    auto vertices = Containers::arrayCast<Vector2>(vertexData);
    Utility::copy({
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {0.0f, 2.0f}
    }, vertices);

    Containers::Array<Trade::MeshAttributeData> attributes{InPlaceInit, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertices}
    }};
    const void* originalAttributeData = attributes;

    Trade::MeshData mesh{MeshPrimitive::TriangleStrip,
        {}, indices, Trade::MeshIndexData{indices},
        Utility::move(vertexData), Utility::move(attributes)};

    const Matrix3 transformation = Matrix3::rotation(35.0_degf);
    Trade::MeshData out = transform2D(Utility::move(mesh), transformation);

    /* Indices should be passed through unchanged */
    CORRADE_VERIFY(out.isIndexed());
    CORRADE_COMPARE_AS(out.indices<UnsignedShort>(),
        Containers::arrayView(indices),
        TestSuite::Compare::Container);

    /* The vertices should be transformed */
    CORRADE_COMPARE_AS(out.attribute<Vector2>(Trade::MeshAttribute::Position), Containers::arrayView({
        transformation.transformPoint({0.0f, 0.0f}),
        transformation.transformPoint({1.0f, 0.0f}),
        transformation.transformPoint({0.0f, 2.0f})
    }), TestSuite::Compare::Container);

    /* The memory should not point to the original data */
    CORRADE_VERIFY(out.indexData().data() != static_cast<const void*>(indices));
    CORRADE_VERIFY(out.vertexData().data() != static_cast<void*>(vertices.data()));
    CORRADE_VERIFY(out.attributeData().data() != static_cast<const void*>(originalAttributeData));
}

void TransformTest::meshData2DRvaluePassthroughVertexDataNotOwned() {
    Containers::Array<char> indexData{NoInit, 3*sizeof(UnsignedShort)};
    auto indices = Containers::arrayCast<UnsignedShort>(indexData);
    /* Not using copy({...}) as we'd have no way to compare against the
       original unchanged data */
    const UnsignedShort indicesExpected[]{1, 2, 0};
    Utility::copy(indicesExpected, indices);
    const Vector2 vertices[]{
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {0.0f, 2.0f}
    };

    Containers::Array<Trade::MeshAttributeData> attributes{InPlaceInit, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::arrayView(vertices)}
    }};
    const void* originalAttributeData = attributes;

    Trade::MeshData mesh{MeshPrimitive::TriangleStrip,
        Utility::move(indexData), Trade::MeshIndexData{indices},
        {}, vertices, Utility::move(attributes)};

    const Matrix3 transformation = Matrix3::rotation(35.0_degf);
    Trade::MeshData out = transform2D(Utility::move(mesh), transformation);

    /* Indices should be passed through unchanged */
    CORRADE_VERIFY(out.isIndexed());
    CORRADE_COMPARE_AS(out.indices<UnsignedShort>(),
        Containers::arrayView(indices),
        TestSuite::Compare::Container);

    /* The vertices should be transformed */
    CORRADE_COMPARE_AS(out.attribute<Vector2>(Trade::MeshAttribute::Position), Containers::arrayView({
        transformation.transformPoint({0.0f, 0.0f}),
        transformation.transformPoint({1.0f, 0.0f}),
        transformation.transformPoint({0.0f, 2.0f})
    }), TestSuite::Compare::Container);

    /* The memory should not point to the original data */
    CORRADE_VERIFY(out.indexData().data() != static_cast<void*>(indices.data()));
    CORRADE_VERIFY(out.vertexData().data() != static_cast<const void*>(vertices));
    CORRADE_VERIFY(out.attributeData().data() != static_cast<const void*>(originalAttributeData));
}

void TransformTest::meshData2DRvaluePassthroughNoPosition() {
    auto&& data = NoAttributeData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    /* Mainly to verify there's no other accidental assertion from checking
       vertex format and that the ID + morph target ID gets correctly passed
       through, this message comes from the l-value overload */
    Containers::String out;
    Error redirectError{&out};
    transform2D(Trade::MeshData{MeshPrimitive::Points, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector2, nullptr},
    }}, {}, data.id, data.morphTargetId);
    if(data.morphTargetId == -1)
        CORRADE_COMPARE(out, "MeshTools::transform2D(): the mesh has no positions with index 1\n");
    else
        CORRADE_COMPARE(out, "MeshTools::transform2D(): the mesh has no positions with index 0 in morph target 37\n");
}

void TransformTest::meshData2DRvaluePassthroughWrongFormat() {
    Containers::Array<char> vertexData{NoInit, 3*sizeof(Vector2ub)};
    auto vertices = Containers::arrayCast<Vector2ub>(vertexData);
    Utility::copy({
        {0, 0},
        {1, 0},
        {0, 2}
    }, vertices);

    Containers::Array<Trade::MeshAttributeData> attributes{InPlaceInit, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertices}
    }};
    const void* originalAttributeData = attributes;

    Trade::MeshData mesh{MeshPrimitive::TriangleStrip,
        Utility::move(vertexData), Utility::move(attributes)};

    const Matrix3 transformation = Matrix3::rotation(35.0_degf);
    Trade::MeshData out = transform2D(Utility::move(mesh), transformation);

    /* The vertices should be expanded to floats and transformed */
    CORRADE_COMPARE_AS(out.attribute<Vector2>(Trade::MeshAttribute::Position), Containers::arrayView({
        transformation.transformPoint({0.0f, 0.0f}),
        transformation.transformPoint({1.0f, 0.0f}),
        transformation.transformPoint({0.0f, 2.0f})
    }), TestSuite::Compare::Container);

    /* The memory should not point to the original data */
    CORRADE_VERIFY(out.vertexData().data() != static_cast<void*>(vertices.data()));
    CORRADE_VERIFY(out.attributeData().data() != static_cast<const void*>(originalAttributeData));
}

void TransformTest::meshData2DInPlaceNotMutable() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData mesh{MeshPrimitive::Points, Trade::DataFlags{}, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector2, nullptr},
    }};

    Containers::String out;
    Error redirectError{&out};
    transform2DInPlace(mesh, {});
    CORRADE_COMPARE(out, "MeshTools::transform2DInPlace(): vertex data not mutable\n");
}

void TransformTest::meshData2DInPlaceNoPosition() {
    auto&& data = NoAttributeData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData mesh{MeshPrimitive::Points, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector2, nullptr},
    }};

    Containers::String out;
    Error redirectError{&out};
    transform2DInPlace(mesh, {}, data.id, data.morphTargetId);
    if(data.morphTargetId == -1)
        CORRADE_COMPARE(out, "MeshTools::transform2DInPlace(): the mesh has no positions with index 1\n");
    else
        CORRADE_COMPARE(out, "MeshTools::transform2DInPlace(): the mesh has no positions with index 0 in morph target 37\n");
}

void TransformTest::meshData2DInPlaceWrongFormat() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData mesh{MeshPrimitive::Points, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector2, nullptr},
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector2us, nullptr}
    }};

    Containers::String out;
    Error redirectError{&out};
    transform2DInPlace(mesh, {}, 1);
    CORRADE_COMPARE(out, "MeshTools::transform2DInPlace(): expected VertexFormat::Vector2 positions but got VertexFormat::Vector2us\n");
}

template<class T, class U, class V, class W> void TransformTest::meshData3D() {
    auto&& data = MeshData3DData[testCaseInstanceId()];
    setTestCaseTemplateName({Math::TypeTraits<T>::name(),
                             Math::TypeTraits<U>::name(),
                             Math::TypeTraits<V>::name(),
                             Math::TypeTraits<W>::name()});
    setTestCaseDescription(data.name);

    const UnsignedShort indices[]{
        1, 2, 0
    };
    const struct Vertex {
        Vector3b secondaryPositionTangentBitangentNormal;
        Math::Vector3<T> position;
        Math::Vector4<U> tangent;
        Math::Vector3<V> bitangent;
        Math::Vector3<W> normal;
        Float somethingElse;
    } vertices[]{
        /** @todo use some *real* values here to verify the handedness change
            also */
        {{15, -34, 6}, {T(0.0), T(0.0), T(-1.0)},
                       {U(0.0), U(1.0), U(0.0), U(1.0)},
                       {V(-1.0), V(0.0), V(0.0)},
                       {W(0.0), W(0.0), W(1.0)}, 7.0f},
        {{11, -25, 3}, {T(1.0), T(0.0), T(-2.0)},
                       {U(1.0), U(0.0), U(0.0), U(1.0)},
                       {V(0.0), V(-1.0), V(0.0)},
                       {W(0.0), W(0.0), W(1.0)}, 5.5f},
        {{16, -27, 0}, {T(0.0), T(2.0), T(-1.0)},
                       {U(0.0), U(1.0), U(0.0), U(-1.0)},
                       {V(0.0), V(0.0), V(-1.0)},
                       {W(1.0), W(0.0), W(0.0)}, 3.0f}
    };
    Containers::StridedArrayView1D<const Vertex> view = vertices;

    Containers::Array<Trade::MeshAttributeData> attributes;
    if(data.id == 1 || data.morphTargetId != -1) {
        arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Position, view.slice(&Vertex::secondaryPositionTangentBitangentNormal)});
        if(data.tangents)
            arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Tangent, VertexFormat::Vector3bNormalized, view.slice(&Vertex::secondaryPositionTangentBitangentNormal)});
        if(data.bitangents)
            arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Bitangent, VertexFormat::Vector3bNormalized, view.slice(&Vertex::secondaryPositionTangentBitangentNormal)});
        if(data.normals)
            arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3bNormalized, view.slice(&Vertex::secondaryPositionTangentBitangentNormal)});
    } else {
        CORRADE_COMPARE(data.id, 0);
        CORRADE_COMPARE(data.morphTargetId, -1);
    }
    arrayAppend(attributes, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, view.slice(&Vertex::position), data.morphTargetId},
        Trade::MeshAttributeData{Trade::meshAttributeCustom(0), view.slice(&Vertex::somethingElse)}
    });
    if(data.tangents)
        arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Tangent, data.tangents4 ? Trade::Implementation::vertexFormatFor<Math::Vector4<U>>() : Trade::Implementation::vertexFormatFor<Math::Vector3<U>>(), view.slice(&Vertex::tangent), 0, data.morphTargetId});
    if(data.bitangents)
        arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Bitangent, view.slice(&Vertex::bitangent), data.morphTargetId});
    if(data.normals)
        arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Normal, view.slice(&Vertex::normal), data.morphTargetId});

    Trade::MeshData mesh{MeshPrimitive::TriangleStrip,
        {}, data.indexed ? Containers::arrayView(indices) : nullptr,
        data.indexed ? Trade::MeshIndexData{indices} : Trade::MeshIndexData{nullptr},
        {}, vertices, Utility::move(attributes)};
    CORRADE_COMPARE(mesh.isIndexed(), data.indexed);

    Trade::MeshData out = transform3D(mesh, data.transformation, data.id, data.morphTargetId);
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::TriangleStrip);

    /* Indices should be preserved */
    if(data.indexed) {
        CORRADE_VERIFY(out.isIndexed());
        CORRADE_COMPARE_AS(out.indices<UnsignedShort>(),
            Containers::arrayView(indices),
            TestSuite::Compare::Container);
    }

    /* The extra attribute should be preserved */
    CORRADE_COMPARE(out.attributeCount(), mesh.attributeCount());
    CORRADE_COMPARE_AS(out.attribute<Float>(Trade::meshAttributeCustom(0)),
        view.slice(&Vertex::somethingElse),
        TestSuite::Compare::Container);

    /* The vertices should be expanded to floats and transformed */
    CORRADE_COMPARE(out.attributeFormat(Trade::MeshAttribute::Position, data.id, data.morphTargetId), VertexFormat::Vector3);
    CORRADE_COMPARE_AS(out.attribute<Vector3>(Trade::MeshAttribute::Position, data.id, data.morphTargetId), Containers::arrayView({
        data.transformation.transformPoint({0.0f, 0.0f, -1.0f}),
        data.transformation.transformPoint({1.0f, 0.0f, -2.0f}),
        data.transformation.transformPoint({0.0f, 2.0f, -1.0f})
    }), TestSuite::Compare::Container);

    /** @todo verify it stays orthogonal */
    const Matrix3x3 normalMatrix = data.transformation.normalMatrix();

    if(data.tangents) {
        CORRADE_COMPARE(out.attributeFormat(Trade::MeshAttribute::Tangent, data.id, data.morphTargetId), data.tangents4 ? VertexFormat::Vector4 : VertexFormat::Vector3);
        if(data.tangents4)
            CORRADE_COMPARE_AS(out.attribute<Vector4>(Trade::MeshAttribute::Tangent, data.id, data.morphTargetId), Containers::arrayView<Vector4>({
                {normalMatrix*Vector3{0.0f, 1.0f, 0.0f}, 1.0f},
                {normalMatrix*Vector3{1.0f, 0.0f, 0.0f}, 1.0f},
                {normalMatrix*Vector3{0.0f, 1.0f, 0.0f}, -1.0f}
            }), TestSuite::Compare::Container);
        else
            CORRADE_COMPARE_AS(out.attribute<Vector3>(Trade::MeshAttribute::Tangent, data.id, data.morphTargetId), Containers::arrayView<Vector3>({
                normalMatrix*Vector3{0.0f, 1.0f, 0.0f},
                normalMatrix*Vector3{1.0f, 0.0f, 0.0f},
                normalMatrix*Vector3{0.0f, 1.0f, 0.0f}
            }), TestSuite::Compare::Container);
    }

    if(data.bitangents) {
        CORRADE_COMPARE(out.attributeFormat(Trade::MeshAttribute::Bitangent, data.id, data.morphTargetId), VertexFormat::Vector3);
        CORRADE_COMPARE_AS(out.attribute<Vector3>(Trade::MeshAttribute::Bitangent, data.id, data.morphTargetId), Containers::arrayView<Vector3>({
            normalMatrix*Vector3{-1.0f, 0.0f, 0.0f},
            normalMatrix*Vector3{0.0f, -1.0f, 0.0f},
            normalMatrix*Vector3{0.0f, 0.0f, -1.0f}
        }), TestSuite::Compare::Container);
    }

    if(data.normals) {
        CORRADE_COMPARE(out.attributeFormat(Trade::MeshAttribute::Normal, data.id, data.morphTargetId), VertexFormat::Vector3);
        CORRADE_COMPARE_AS(out.attribute<Vector3>(Trade::MeshAttribute::Normal, data.id, data.morphTargetId), Containers::arrayView<Vector3>({
            normalMatrix*Vector3{0.0f, 0.0f, 1.0f},
            normalMatrix*Vector3{0.0f, 0.0f, 1.0f},
            normalMatrix*Vector3{1.0f, 0.0f, 0.0f}
        }), TestSuite::Compare::Container);
    }
}

void TransformTest::meshData3DNoPosition() {
    auto&& data = NoAttributeData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData mesh{MeshPrimitive::Points, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr},
    }};

    Containers::String out;
    Error redirectError{&out};
    transform3D(mesh, {}, data.id, data.morphTargetId);
    if(data.morphTargetId == -1)
        CORRADE_COMPARE(out, "MeshTools::transform3D(): the mesh has no positions with index 1\n");
    else
        CORRADE_COMPARE(out, "MeshTools::transform3D(): the mesh has no positions with index 0 in morph target 37\n");
}

void TransformTest::meshData3DNot3D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData mesh{MeshPrimitive::Points, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector2, nullptr}
    }};

    Containers::String out;
    Error redirectError{&out};
    transform3D(mesh, {});
    CORRADE_COMPARE(out, "MeshTools::transform3D(): expected 3D positions but got VertexFormat::Vector2\n");
}

void TransformTest::meshData3DImplementationSpecificIndexType() {
    const UnsignedShort indices[]{3, 1, 2, 0, 2};
    const Vector3 vertices[]{
        {0.0f, 0.0f, -1.0f},
        {1.0f, 0.0f, -2.0f},
        {0.0f, 2.0f, -1.0f}
    };
    Trade::MeshData mesh{MeshPrimitive::Points,
        {}, indices, Trade::MeshIndexData{meshIndexTypeWrap(0xcaca), Containers::stridedArrayView(indices).slice(1, 4)},
        {}, vertices, {Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::arrayView(vertices)}}};

    /* Just verify the index data get passed through with no information loss */
    Trade::MeshData out = transform3D(mesh, Matrix4{}, 0, -1, InterleaveFlag::PreserveStridedIndices);
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::Points);
    CORRADE_COMPARE(out.indexType(), meshIndexTypeWrap(0xcaca));
    CORRADE_COMPARE(out.indexOffset(), 2);
    CORRADE_COMPARE(out.indexStride(), 2);
    CORRADE_COMPARE(out.indexCount(), 3);
    CORRADE_COMPARE_AS((Containers::arrayCast<1, const UnsignedShort>(out.indices())),
        Containers::arrayView<UnsignedShort>({1, 2, 0}),
        TestSuite::Compare::Container);
}

void TransformTest::meshData3DImplementationSpecificVertexFormat() {
    auto&& data = MeshData3DIMplementationSpecificVertexFormatData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData mesh{MeshPrimitive::Points, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr},
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, data.positionFormat, nullptr},
        Trade::MeshAttributeData{data.otherAttribute, VertexFormat::Vector3, nullptr},
        Trade::MeshAttributeData{data.otherAttribute, data.otherAttributeFormat, nullptr},
    }};

    Containers::String out;
    Error redirectError{&out};
    transform3D(mesh, {}, 1);
    CORRADE_COMPARE(out, Utility::format("MeshTools::transform3D(): {} have an implementation-specific format 0xcaca\n", data.name));
}

void TransformTest::meshData3DRvaluePassthrough() {
    auto&& data = MeshData3DRvaluePassthroughData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Array<char> indexData{NoInit, 3*sizeof(UnsignedShort)};
    auto indices = Containers::arrayCast<UnsignedShort>(indexData);
    /* Not using copy({...}) as we'd have no way to compare against the
       original unchanged data */
    const UnsignedShort indicesExpected[]{1, 2, 0};
    Utility::copy(indicesExpected, indices);

    struct Vertex {
        Vector3b secondaryPositionTangentBitangentNormal;
        Vector3 position;
        Vector4 tangent;
        Vector3 bitangent;
        Vector3 normal;
    };
    Containers::Array<char> vertexData{NoInit, 3*sizeof(Vertex)};
    auto vertices = stridedArrayView(Containers::arrayCast<Vertex>(vertexData));
    Utility::copy({
        {{15, -34, 6}, {0.0f, 0.0f, -1.0f},
                       {0.0f, 1.0f, 0.0f, 1.0f},
                       {-1.0f, 0.0f, 0.0f},
                       {0.0f, 0.0f, 1.0f}},
        {{11, -25, 3}, {1.0f, 0.0f, -2.0f},
                       {1.0f, 0.0f, 0.0f, 1.0f},
                       {0.0f, -1.0f, 0.0f},
                       {0.0f, 0.0f, 1.0f}},
        {{16, -27, 0}, {0.0f, 2.0f, -1.0f},
                       {0.0f, 1.0f, 0.0f, -1.0f},
                       {0.0f, 0.0f, -1.0f},
                       {1.0f, 0.0f, 0.0f}}
    }, vertices);

    Containers::Array<Trade::MeshAttributeData> attributes;
    if(data.id == 1 || data.morphTargetId != -1) {
        arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertices.slice(&Vertex::secondaryPositionTangentBitangentNormal)});
        if(data.tangents)
            arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Tangent, VertexFormat::Vector3bNormalized, vertices.slice(&Vertex::secondaryPositionTangentBitangentNormal)});
        if(data.bitangents)
            arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Bitangent, VertexFormat::Vector3bNormalized, vertices.slice(&Vertex::secondaryPositionTangentBitangentNormal)});
        if(data.normals)
            arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3bNormalized, vertices.slice(&Vertex::secondaryPositionTangentBitangentNormal)});
    } else {
        CORRADE_COMPARE(data.id, 0);
        CORRADE_COMPARE(data.morphTargetId, -1);
    }
    arrayAppend(attributes, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertices.slice(&Vertex::position), data.morphTargetId}
    });
    if(data.tangents)
        arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Tangent, data.tangents4 ? VertexFormat::Vector4 : VertexFormat::Vector3, vertices.slice(&Vertex::tangent), 0, data.morphTargetId});
    if(data.bitangents)
        arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Bitangent, vertices.slice(&Vertex::bitangent), data.morphTargetId});
    if(data.normals)
        arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Normal, vertices.slice(&Vertex::normal), data.morphTargetId});
    const void* originalAttributeData = attributes;

    Trade::MeshData mesh{MeshPrimitive::TriangleFan,
        data.indexed ? Utility::move(indexData) : nullptr,
        data.indexed ? Trade::MeshIndexData{indices} : Trade::MeshIndexData{nullptr},
        Utility::move(vertexData), Utility::move(attributes)};

    const Matrix4 transformation = Matrix4::rotationX(35.0_degf);
    Trade::MeshData out = transform3D(Utility::move(mesh), transformation, data.id, data.morphTargetId);
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::TriangleFan);

    /* Indices should be passed through unchanged */
    if(data.indexed) {
        CORRADE_VERIFY(out.isIndexed());
        CORRADE_COMPARE_AS(out.indices<UnsignedShort>(),
            Containers::arrayView(indicesExpected),
            TestSuite::Compare::Container);
    }

    /* The vertices should be expanded to floats and transformed */
    CORRADE_COMPARE(out.attributeFormat(Trade::MeshAttribute::Position, data.id, data.morphTargetId), VertexFormat::Vector3);
    CORRADE_COMPARE_AS(out.attribute<Vector3>(Trade::MeshAttribute::Position, data.id, data.morphTargetId), Containers::arrayView({
        transformation.transformPoint({0.0f, 0.0f, -1.0f}),
        transformation.transformPoint({1.0f, 0.0f, -2.0f}),
        transformation.transformPoint({0.0f, 2.0f, -1.0f})
    }), TestSuite::Compare::Container);

    if(data.tangents) {
        CORRADE_COMPARE(out.attributeFormat(Trade::MeshAttribute::Tangent, data.id, data.morphTargetId), data.tangents4 ? VertexFormat::Vector4 : VertexFormat::Vector3);
        if(data.tangents4)
            CORRADE_COMPARE_AS(out.attribute<Vector4>(Trade::MeshAttribute::Tangent, data.id, data.morphTargetId), Containers::arrayView<Vector4>({
                {transformation.transformVector({0.0f, 1.0f, 0.0f}), 1.0f},
                {transformation.transformVector({1.0f, 0.0f, 0.0f}), 1.0f},
                {transformation.transformVector({0.0f, 1.0f, 0.0f}), -1.0f}
            }), TestSuite::Compare::Container);
        else
            CORRADE_COMPARE_AS(out.attribute<Vector3>(Trade::MeshAttribute::Tangent, data.id, data.morphTargetId), Containers::arrayView({
                transformation.transformVector({0.0f, 1.0f, 0.0f}),
                transformation.transformVector({1.0f, 0.0f, 0.0f}),
                transformation.transformVector({0.0f, 1.0f, 0.0f})
            }), TestSuite::Compare::Container);
    }

    if(data.bitangents) {
        CORRADE_COMPARE(out.attributeFormat(Trade::MeshAttribute::Bitangent, data.id, data.morphTargetId), VertexFormat::Vector3);
        CORRADE_COMPARE_AS(out.attribute<Vector3>(Trade::MeshAttribute::Bitangent, data.id, data.morphTargetId), Containers::arrayView({
            transformation.transformVector({-1.0f, 0.0f, 0.0f}),
            transformation.transformVector({0.0f, -1.0f, 0.0f}),
            transformation.transformVector({0.0f, 0.0f, -1.0f})
        }), TestSuite::Compare::Container);
    }

    if(data.normals) {
        CORRADE_COMPARE(out.attributeFormat(Trade::MeshAttribute::Normal, data.id, data.morphTargetId), VertexFormat::Vector3);
        CORRADE_COMPARE_AS(out.attribute<Vector3>(Trade::MeshAttribute::Normal, data.id, data.morphTargetId), Containers::arrayView({
            transformation.transformVector({0.0f, 0.0f, 1.0f}),
            transformation.transformVector({0.0f, 0.0f, 1.0f}),
            transformation.transformVector({1.0f, 0.0f, 0.0f})
        }), TestSuite::Compare::Container);
    }

    /* The memory should point to the original data */
    if(data.indexed)
        CORRADE_COMPARE(out.indexData().data(), static_cast<void*>(indices.data()));
    CORRADE_COMPARE(out.vertexData().data(), static_cast<void*>(vertices.data()));
    CORRADE_COMPARE(out.attributeData().data(), originalAttributeData);
}

void TransformTest::meshData3DRvaluePassthroughIndexDataNotOwned() {
    const UnsignedShort indices[]{
        1, 2, 0
    };
    Containers::Array<char> vertexData{NoInit, 3*sizeof(Vector3)};
    auto vertices = Containers::arrayCast<Vector3>(vertexData);
    Utility::copy({
        {0.0f, 0.0f, -1.0f},
        {1.0f, 0.0f, -2.0f},
        {0.0f, 2.0f, -1.0f}
    }, vertices);

    Containers::Array<Trade::MeshAttributeData> attributes{InPlaceInit, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertices}
    }};
    const void* originalAttributeData = attributes;

    Trade::MeshData mesh{MeshPrimitive::TriangleStrip,
        {}, indices, Trade::MeshIndexData{indices},
        Utility::move(vertexData), Utility::move(attributes)};

    const Matrix4 transformation = Matrix4::rotationX(35.0_degf);
    Trade::MeshData out = transform3D(Utility::move(mesh), transformation);

    /* Indices should be passed through unchanged */
    CORRADE_VERIFY(out.isIndexed());
    CORRADE_COMPARE_AS(out.indices<UnsignedShort>(),
        Containers::arrayView(indices),
        TestSuite::Compare::Container);

    /* The vertices should be transformed */
    CORRADE_COMPARE_AS(out.attribute<Vector3>(Trade::MeshAttribute::Position), Containers::arrayView({
        transformation.transformPoint({0.0f, 0.0f, -1.0f}),
        transformation.transformPoint({1.0f, 0.0f, -2.0f}),
        transformation.transformPoint({0.0f, 2.0f, -1.0f})
    }), TestSuite::Compare::Container);

    /* The memory should not point to the original data */
    CORRADE_VERIFY(out.indexData().data() != static_cast<const void*>(indices));
    CORRADE_VERIFY(out.vertexData().data() != static_cast<void*>(vertices.data()));
    CORRADE_VERIFY(out.attributeData().data() != static_cast<const void*>(originalAttributeData));
}

void TransformTest::meshData3DRvaluePassthroughVertexDataNotOwned() {
    Containers::Array<char> indexData{NoInit, 3*sizeof(UnsignedShort)};
    auto indices = Containers::arrayCast<UnsignedShort>(indexData);
    /* Not using copy({...}) as we'd have no way to compare against the
       original unchanged data */
    const UnsignedShort indicesExpected[]{1, 2, 0};
    Utility::copy(indicesExpected, indices);
    const Vector3 vertices[]{
        {0.0f, 0.0f, -1.0f},
        {1.0f, 0.0f, -2.0f},
        {0.0f, 2.0f, -1.0f}
    };

    Containers::Array<Trade::MeshAttributeData> attributes{InPlaceInit, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::arrayView(vertices)}
    }};
    const void* originalAttributeData = attributes;

    Trade::MeshData mesh{MeshPrimitive::TriangleStrip,
        Utility::move(indexData), Trade::MeshIndexData{indices},
        {}, vertices, Utility::move(attributes)};

    const Matrix4 transformation = Matrix4::rotationX(35.0_degf);
    Trade::MeshData out = transform3D(Utility::move(mesh), transformation);

    /* Indices should be passed through unchanged */
    CORRADE_VERIFY(out.isIndexed());
    CORRADE_COMPARE_AS(out.indices<UnsignedShort>(),
        Containers::arrayView(indices),
        TestSuite::Compare::Container);

    /* The vertices should be transformed */
    CORRADE_COMPARE_AS(out.attribute<Vector3>(Trade::MeshAttribute::Position), Containers::arrayView({
        transformation.transformPoint({0.0f, 0.0f, -1.0f}),
        transformation.transformPoint({1.0f, 0.0f, -2.0f}),
        transformation.transformPoint({0.0f, 2.0f, -1.0f})
    }), TestSuite::Compare::Container);

    /* The memory should not point to the original data */
    CORRADE_VERIFY(out.indexData().data() != static_cast<void*>(indices.data()));
    CORRADE_VERIFY(out.vertexData().data() != static_cast<const void*>(vertices));
    CORRADE_VERIFY(out.attributeData().data() != static_cast<const void*>(originalAttributeData));
}

void TransformTest::meshData3DRvaluePassthroughNoPosition() {
    auto&& data = NoAttributeData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    /* Mainly to verify there's no other accidental assertion from checking
       vertex format and that the ID + morph target ID gets correctly passed
       through, this message comes from the l-value overload */
    Containers::String out;
    Error redirectError{&out};
    transform3D(Trade::MeshData{MeshPrimitive::Points, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr},
    }}, {}, data.id, data.morphTargetId);
    if(data.morphTargetId == -1)
        CORRADE_COMPARE(out, "MeshTools::transform3D(): the mesh has no positions with index 1\n");
    else
        CORRADE_COMPARE(out, "MeshTools::transform3D(): the mesh has no positions with index 0 in morph target 37\n");
}

void TransformTest::meshData3DRvaluePassthroughWrongFormat() {
    auto&& data = MeshData3DWrongFormatData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct Vertex {
        Vector3b position;
        Vector4b tangent;
        Vector3h bitangent;
        Vector3s normal;
    };
    Containers::Array<char> vertexData{NoInit, 3*sizeof(Vertex)};
    auto vertices = stridedArrayView(Containers::arrayCast<Vertex>(vertexData));
    Utility::copy({
        {{0, 0, -1},
         {0, 127, 0, 127},
         {-1.0_h, 0.0_h, 0.0_h},
         {0, 0, 32767}},
        {{1, 0, -2},
         {127, 0, 0, 127},
         {0.0_h, -1.0_h, 0.0_h},
         {0, 0, 32767}},
        {{0, 2, -1},
         {0, 127, 0, -127},
         {0.0_h, 0.0_h, -1.0_h},
         {32767, 0, 0}}
    }, vertices);

    Containers::Array<Trade::MeshAttributeData> attributes{InPlaceInit, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertices.slice(&Vertex::position)}
    }};
    if(data.tangentFormat != VertexFormat{})
        arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Tangent, vertexFormatComponentCount(data.tangentFormat) == 4 ? VertexFormat::Vector4bNormalized : VertexFormat::Vector3bNormalized, vertices.slice(&Vertex::tangent)});
    if(data.bitangentFormat != VertexFormat{})
        arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Bitangent, vertices.slice(&Vertex::bitangent)});
    if(data.normalFormat != VertexFormat{})
        arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3sNormalized, vertices.slice(&Vertex::normal)});
    const void* originalAttributeData = attributes;

    Trade::MeshData mesh{MeshPrimitive::TriangleStrip,
        Utility::move(vertexData), Utility::move(attributes)};

    const Matrix4 transformation = Matrix4::rotationX(35.0_degf);
    Trade::MeshData out = transform3D(Utility::move(mesh), transformation);

    /* The vertices should be expanded to floats and transformed */
    CORRADE_COMPARE_AS(out.attribute<Vector3>(Trade::MeshAttribute::Position), Containers::arrayView({
        transformation.transformPoint({0.0f, 0.0f, -1.0f}),
        transformation.transformPoint({1.0f, 0.0f, -2.0f}),
        transformation.transformPoint({0.0f, 2.0f, -1.0f})
    }), TestSuite::Compare::Container);

    if(data.tangentFormat != VertexFormat{}) {
        if(vertexFormatComponentCount(data.tangentFormat) == 4)
            CORRADE_COMPARE_AS(out.attribute<Vector4>(Trade::MeshAttribute::Tangent), Containers::arrayView<Vector4>({
                {transformation.transformVector({0.0f, 1.0f, 0.0f}), 1.0f},
                {transformation.transformVector({1.0f, 0.0f, 0.0f}), 1.0f},
                {transformation.transformVector({0.0f, 1.0f, 0.0f}), -1.0f}
            }), TestSuite::Compare::Container);
        else
            CORRADE_COMPARE_AS(out.attribute<Vector3>(Trade::MeshAttribute::Tangent), Containers::arrayView({
                transformation.transformVector({0.0f, 1.0f, 0.0f}),
                transformation.transformVector({1.0f, 0.0f, 0.0f}),
                transformation.transformVector({0.0f, 1.0f, 0.0f})
            }), TestSuite::Compare::Container);
    }

    if(data.bitangentFormat != VertexFormat{}) {
        CORRADE_COMPARE_AS(out.attribute<Vector3>(Trade::MeshAttribute::Bitangent), Containers::arrayView({
            transformation.transformVector({-1.0f, 0.0f, 0.0f}),
            transformation.transformVector({0.0f, -1.0f, 0.0f}),
            transformation.transformVector({0.0f, 0.0f, -1.0f})
        }), TestSuite::Compare::Container);
    }

    if(data.normalFormat != VertexFormat{}) {
        CORRADE_COMPARE_AS(out.attribute<Vector3>(Trade::MeshAttribute::Normal), Containers::arrayView({
            transformation.transformVector({0.0f, 0.0f, 1.0f}),
            transformation.transformVector({0.0f, 0.0f, 1.0f}),
            transformation.transformVector({1.0f, 0.0f, 0.0f})
        }), TestSuite::Compare::Container);
    }

    /* The memory should not point to the original data */
    CORRADE_VERIFY(out.vertexData().data() != static_cast<void*>(vertices.data()));
    CORRADE_VERIFY(out.attributeData().data() != static_cast<const void*>(originalAttributeData));
}

void TransformTest::meshData3DInPlaceNotMutable() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData mesh{MeshPrimitive::Points, Trade::DataFlags{}, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr},
    }};

    Containers::String out;
    Error redirectError{&out};
    transform3DInPlace(mesh, {});
    CORRADE_COMPARE(out, "MeshTools::transform3DInPlace(): vertex data not mutable\n");
}

void TransformTest::meshData3DInPlaceNoPosition() {
    auto&& data = NoAttributeData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData mesh{MeshPrimitive::Points, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr},
    }};

    Containers::String out;
    Error redirectError{&out};
    transform3DInPlace(mesh, {}, data.id, data.morphTargetId);
    if(data.morphTargetId == -1)
        CORRADE_COMPARE(out, "MeshTools::transform3DInPlace(): the mesh has no positions with index 1\n");
    else
        CORRADE_COMPARE(out, "MeshTools::transform3DInPlace(): the mesh has no positions with index 0 in morph target 37\n");
}

void TransformTest::meshData3DInPlaceWrongFormat() {
    CORRADE_SKIP_IF_NO_ASSERT();

    auto&& data = MeshData3DWrongFormatData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Array<Trade::MeshAttributeData> attributes{InPlaceInit, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr},
        Trade::MeshAttributeData{Trade::MeshAttribute::Tangent, VertexFormat::Vector3, nullptr},
        Trade::MeshAttributeData{Trade::MeshAttribute::Bitangent, VertexFormat::Vector3, nullptr},
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3, nullptr},
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, data.positionFormat, nullptr},
    }};
    if(data.tangentFormat != VertexFormat{})
        arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Tangent, data.tangentFormat, nullptr});
    if(data.bitangentFormat != VertexFormat{})
        arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Bitangent, data.bitangentFormat, nullptr});
    if(data.normalFormat != VertexFormat{})
        arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Normal, data.normalFormat, nullptr});

    Trade::MeshData mesh{MeshPrimitive::Points, nullptr, Utility::move(attributes)};

    Containers::String out;
    Error redirectError{&out};
    transform3DInPlace(mesh, {}, 1);
    CORRADE_COMPARE(out, data.message);
}

template<class T> void TransformTest::meshDataTextureCoordinates2D() {
    auto&& data = MeshDataTextureCoordinatesData[testCaseInstanceId()];
    setTestCaseTemplateName(Math::TypeTraits<T>::name());
    setTestCaseDescription(data.name);

    const UnsignedShort indices[]{
        1, 2, 0
    };
    const struct Vertex {
        Vector2us secondaryTextureCoordinates;
        Math::Vector2<T> textureCoordinates;
        Float somethingElse;
    } vertices[]{
        {{15, 34}, {T(0.0), T(0.0)}, 7.0f},
        {{11, 25}, {T(1.0), T(0.0)}, 5.5f},
        {{16, 27}, {T(0.0), T(2.0)}, 3.0f}
    };
    Containers::StridedArrayView1D<const Vertex> view = vertices;

    Containers::Array<Trade::MeshAttributeData> attributes;
    if(data.id == 1 || data.morphTargetId != -1)
        arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, view.slice(&Vertex::secondaryTextureCoordinates)});
    else {
        CORRADE_COMPARE(data.id, 0);
        CORRADE_COMPARE(data.morphTargetId, -1);
    }
    arrayAppend(attributes, {
        Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, view.slice(&Vertex::textureCoordinates), data.morphTargetId},
        Trade::MeshAttributeData{Trade::meshAttributeCustom(0), view.slice(&Vertex::somethingElse)}
    });

    Trade::MeshData mesh{MeshPrimitive::TriangleStrip,
        {}, data.indexed ? Containers::arrayView(indices) : nullptr,
        data.indexed ? Trade::MeshIndexData{indices} : Trade::MeshIndexData{nullptr},
        {}, vertices, Utility::move(attributes)};
    CORRADE_COMPARE(mesh.isIndexed(), data.indexed);

    Trade::MeshData out = transformTextureCoordinates2D(mesh, data.transformation, data.id, data.morphTargetId);
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::TriangleStrip);

    /* Indices should be preserved */
    if(data.indexed) {
        CORRADE_VERIFY(out.isIndexed());
        CORRADE_COMPARE_AS(out.indices<UnsignedShort>(),
            Containers::arrayView(indices),
            TestSuite::Compare::Container);
    }

    /* The extra attribute should be preserved */
    CORRADE_COMPARE(out.attributeCount(), mesh.attributeCount());
    CORRADE_COMPARE_AS(out.attribute<Float>(Trade::meshAttributeCustom(0)),
        view.slice(&Vertex::somethingElse),
        TestSuite::Compare::Container);

    /* The vertices should be expanded to floats and transformed */
    CORRADE_COMPARE(out.attributeFormat(Trade::MeshAttribute::TextureCoordinates, data.id, data.morphTargetId), VertexFormat::Vector2);
    CORRADE_COMPARE_AS(out.attribute<Vector2>(Trade::MeshAttribute::TextureCoordinates, data.id, data.morphTargetId), Containers::arrayView({
        data.transformation.transformPoint({0.0f, 0.0f}),
        data.transformation.transformPoint({1.0f, 0.0f}),
        data.transformation.transformPoint({0.0f, 2.0f})
    }), TestSuite::Compare::Container);
}

void TransformTest::meshDataTextureCoordinates2DNoCoordinates() {
    auto&& data = NoAttributeData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData mesh{MeshPrimitive::Points, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, VertexFormat::Vector2, nullptr},
    }};

    Containers::String out;
    Error redirectError{&out};
    transformTextureCoordinates2D(mesh, {}, data.id, data.morphTargetId);
    if(data.morphTargetId == -1)
        CORRADE_COMPARE(out, "MeshTools::transformTextureCoordinates2D(): the mesh has no texture coordinates with index 1\n");
    else
        CORRADE_COMPARE(out, "MeshTools::transformTextureCoordinates2D(): the mesh has no texture coordinates with index 0 in morph target 37\n");
}

void TransformTest::meshDataTextureCoordinates2DImplementationSpecificIndexType() {
    const UnsignedShort indices[]{3, 1, 2, 0, 2};
    const Vector2 vertices[]{
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {0.0f, 2.0f}
    };
    Trade::MeshData mesh{MeshPrimitive::Points,
        {}, indices, Trade::MeshIndexData{meshIndexTypeWrap(0xcaca), Containers::stridedArrayView(indices).slice(1, 4)},
        {}, vertices, {Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, Containers::arrayView(vertices)}}};

    /* Just verify the index data get passed through with no information loss */
    Trade::MeshData out = transformTextureCoordinates2D(mesh, Matrix3{}, 0, -1, InterleaveFlag::PreserveStridedIndices);
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::Points);
    CORRADE_COMPARE(out.indexType(), meshIndexTypeWrap(0xcaca));
    CORRADE_COMPARE(out.indexOffset(), 2);
    CORRADE_COMPARE(out.indexStride(), 2);
    CORRADE_COMPARE(out.indexCount(), 3);
    CORRADE_COMPARE_AS((Containers::arrayCast<1, const UnsignedShort>(out.indices())),
        Containers::arrayView<UnsignedShort>({1, 2, 0}),
        TestSuite::Compare::Container);
}

void TransformTest::meshDataTextureCoordinates2DImplementationSpecificVertexFormat() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData mesh{MeshPrimitive::Points, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, VertexFormat::Vector2, nullptr},
        Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, vertexFormatWrap(0xcaca), nullptr},
    }};

    Containers::String out;
    Error redirectError{&out};
    transformTextureCoordinates2D(mesh, {}, 1);
    CORRADE_COMPARE(out, "MeshTools::transformTextureCoordinates2D(): texture coordinates have an implementation-specific format 0xcaca\n");
}

void TransformTest::meshDataTextureCoordinates2DRvaluePassthrough() {
    auto&& data = MeshDataTextureCoordinatesRvaluePassthroughData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Array<char> indexData{NoInit, 3*sizeof(UnsignedShort)};
    auto indices = Containers::arrayCast<UnsignedShort>(indexData);
    /* Not using copy({...}) as we'd have no way to compare against the
       original unchanged data */
    const UnsignedShort indicesExpected[]{1, 2, 0};
    Utility::copy(indicesExpected, indices);

    struct Vertex {
        Vector2ub secondaryTextureCoordinates;
        Vector2 textureCoordinates;
    };
    Containers::Array<char> vertexData{NoInit, 3*sizeof(Vertex)};
    auto vertices = stridedArrayView(Containers::arrayCast<Vertex>(vertexData));
    Utility::copy({
        {{15, 34}, {0.0f, 0.0f}},
        {{11, 25}, {1.0f, 0.0f}},
        {{16, 27}, {0.0f, 2.0f}}
    }, vertices);

    Containers::Array<Trade::MeshAttributeData> attributes;
    if(data.id == 1 || data.morphTargetId != -1)
        arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, vertices.slice(&Vertex::secondaryTextureCoordinates)});
    else {
        CORRADE_COMPARE(data.id, 0);
        CORRADE_COMPARE(data.morphTargetId, -1);
    }
    arrayAppend(attributes, {
        Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, vertices.slice(&Vertex::textureCoordinates), data.morphTargetId}
    });
    const void* originalAttributeData = attributes;

    Trade::MeshData mesh{MeshPrimitive::TriangleFan,
        data.indexed ? Utility::move(indexData) : nullptr,
        data.indexed ? Trade::MeshIndexData{indices} : Trade::MeshIndexData{nullptr},
        Utility::move(vertexData), Utility::move(attributes)};

    const Matrix3 transformation = Matrix3::rotation(35.0_degf);
    Trade::MeshData out = transformTextureCoordinates2D(Utility::move(mesh), transformation, data.id, data.morphTargetId);
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::TriangleFan);

    /* Indices should be passed through unchanged */
    if(data.indexed) {
        CORRADE_VERIFY(out.isIndexed());
        CORRADE_COMPARE_AS(out.indices<UnsignedShort>(),
            Containers::arrayView(indicesExpected),
            TestSuite::Compare::Container);
    }

    /* The vertices should be expanded to floats and transformed */
    CORRADE_COMPARE(out.attributeFormat(Trade::MeshAttribute::TextureCoordinates, data.id, data.morphTargetId), VertexFormat::Vector2);
    CORRADE_COMPARE_AS(out.attribute<Vector2>(Trade::MeshAttribute::TextureCoordinates, data.id, data.morphTargetId), Containers::arrayView({
        transformation.transformPoint({0.0f, 0.0f}),
        transformation.transformPoint({1.0f, 0.0f}),
        transformation.transformPoint({0.0f, 2.0f})
    }), TestSuite::Compare::Container);

    /* The memory should point to the original data */
    if(data.indexed)
        CORRADE_COMPARE(out.indexData().data(), static_cast<void*>(indices.data()));
    CORRADE_COMPARE(out.vertexData().data(), static_cast<void*>(vertices.data()));
    CORRADE_COMPARE(out.attributeData().data(), originalAttributeData);
}

void TransformTest::meshDataTextureCoordinates2DRvaluePassthroughIndexDataNotOwned() {
    const UnsignedShort indices[]{
        1, 2, 0
    };
    Containers::Array<char> vertexData{NoInit, 3*sizeof(Vector2)};
    auto vertices = Containers::arrayCast<Vector2>(vertexData);
    Utility::copy({
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {0.0f, 2.0f}
    }, vertices);

    Containers::Array<Trade::MeshAttributeData> attributes{InPlaceInit, {
        Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, vertices}
    }};
    const void* originalAttributeData = attributes;

    Trade::MeshData mesh{MeshPrimitive::TriangleStrip,
        {}, indices, Trade::MeshIndexData{indices},
        Utility::move(vertexData), Utility::move(attributes)};

    const Matrix3 transformation = Matrix3::rotation(35.0_degf);
    Trade::MeshData out = transformTextureCoordinates2D(Utility::move(mesh), transformation);

    /* Indices should be passed through unchanged */
    CORRADE_VERIFY(out.isIndexed());
    CORRADE_COMPARE_AS(out.indices<UnsignedShort>(),
        Containers::arrayView(indices),
        TestSuite::Compare::Container);

    /* The vertices should be transformed */
    CORRADE_COMPARE_AS(out.attribute<Vector2>(Trade::MeshAttribute::TextureCoordinates), Containers::arrayView({
        transformation.transformPoint({0.0f, 0.0f}),
        transformation.transformPoint({1.0f, 0.0f}),
        transformation.transformPoint({0.0f, 2.0f})
    }), TestSuite::Compare::Container);

    /* The memory should not point to the original data */
    CORRADE_VERIFY(out.indexData().data() != static_cast<const void*>(indices));
    CORRADE_VERIFY(out.vertexData().data() != static_cast<void*>(vertices.data()));
    CORRADE_VERIFY(out.attributeData().data() != static_cast<const void*>(originalAttributeData));
}

void TransformTest::meshDataTextureCoordinates2DRvaluePassthroughVertexDataNotOwned() {
    Containers::Array<char> indexData{NoInit, 3*sizeof(UnsignedShort)};
    auto indices = Containers::arrayCast<UnsignedShort>(indexData);
    /* Not using copy({...}) as we'd have no way to compare against the
       original unchanged data */
    const UnsignedShort indicesExpected[]{1, 2, 0};
    Utility::copy(indicesExpected, indices);
    const Vector2 vertices[]{
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {0.0f, 2.0f}
    };

    Containers::Array<Trade::MeshAttributeData> attributes{InPlaceInit, {
        Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, Containers::arrayView(vertices)}
    }};
    const void* originalAttributeData = attributes;

    Trade::MeshData mesh{MeshPrimitive::TriangleStrip,
        Utility::move(indexData), Trade::MeshIndexData{indices},
        {}, vertices, Utility::move(attributes)};

    const Matrix3 transformation = Matrix3::rotation(35.0_degf);
    Trade::MeshData out = transformTextureCoordinates2D(Utility::move(mesh), transformation);

    /* Indices should be passed through unchanged */
    CORRADE_VERIFY(out.isIndexed());
    CORRADE_COMPARE_AS(out.indices<UnsignedShort>(),
        Containers::arrayView(indices),
        TestSuite::Compare::Container);

    /* The vertices should be transformed */
    CORRADE_COMPARE_AS(out.attribute<Vector2>(Trade::MeshAttribute::TextureCoordinates), Containers::arrayView({
        transformation.transformPoint({0.0f, 0.0f}),
        transformation.transformPoint({1.0f, 0.0f}),
        transformation.transformPoint({0.0f, 2.0f})
    }), TestSuite::Compare::Container);

    /* The memory should not point to the original data */
    CORRADE_VERIFY(out.indexData().data() != static_cast<void*>(indices.data()));
    CORRADE_VERIFY(out.vertexData().data() != static_cast<const void*>(vertices));
    CORRADE_VERIFY(out.attributeData().data() != static_cast<const void*>(originalAttributeData));
}

void TransformTest::meshDataTextureCoordinates2DRvaluePassthroughNoCoordinates() {
    auto&& data = NoAttributeData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    /* Mainly to verify there's no other accidental assertion from checking
       vertex format and that the ID + morph target ID gets correctly passed
       through, this message comes from the l-value overload */
    Containers::String out;
    Error redirectError{&out};
    transformTextureCoordinates2D(Trade::MeshData{MeshPrimitive::Points, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, VertexFormat::Vector2, nullptr},
    }}, {}, data.id, data.morphTargetId);
    if(data.morphTargetId == -1)
        CORRADE_COMPARE(out, "MeshTools::transformTextureCoordinates2D(): the mesh has no texture coordinates with index 1\n");
    else
        CORRADE_COMPARE(out, "MeshTools::transformTextureCoordinates2D(): the mesh has no texture coordinates with index 0 in morph target 37\n");
}

void TransformTest::meshDataTextureCoordinates2DRvaluePassthroughWrongFormat() {
    Containers::Array<char> vertexData{NoInit, 3*sizeof(Vector2ub)};
    auto vertices = Containers::arrayCast<Vector2ub>(vertexData);
    Utility::copy({
        {0, 0},
        {1, 0},
        {0, 2}
    }, vertices);

    Containers::Array<Trade::MeshAttributeData> attributes{InPlaceInit, {
        Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, vertices}
    }};
    const void* originalAttributeData = attributes;

    Trade::MeshData mesh{MeshPrimitive::TriangleStrip,
        Utility::move(vertexData), Utility::move(attributes)};

    const Matrix3 transformation = Matrix3::rotation(35.0_degf);
    Trade::MeshData out = transformTextureCoordinates2D(Utility::move(mesh), transformation);

    /* The vertices should be expanded to floats and transformed */
    CORRADE_COMPARE_AS(out.attribute<Vector2>(Trade::MeshAttribute::TextureCoordinates), Containers::arrayView({
        transformation.transformPoint({0.0f, 0.0f}),
        transformation.transformPoint({1.0f, 0.0f}),
        transformation.transformPoint({0.0f, 2.0f})
    }), TestSuite::Compare::Container);

    /* The memory should not point to the original data */
    CORRADE_VERIFY(out.vertexData().data() != static_cast<void*>(vertices.data()));
    CORRADE_VERIFY(out.attributeData().data() != static_cast<const void*>(originalAttributeData));
}

void TransformTest::meshDataTextureCoordinates2DInPlaceNotMutable() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData mesh{MeshPrimitive::Points, Trade::DataFlags{}, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, VertexFormat::Vector2, nullptr},
    }};

    Containers::String out;
    Error redirectError{&out};
    transformTextureCoordinates2DInPlace(mesh, {});
    CORRADE_COMPARE(out, "MeshTools::transformTextureCoordinates2DInPlace(): vertex data not mutable\n");
}

void TransformTest::meshDataTextureCoordinates2DInPlaceNoCoordinates() {
    auto&& data = NoAttributeData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData mesh{MeshPrimitive::Points, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, VertexFormat::Vector2, nullptr},
    }};

    Containers::String out;
    Error redirectError{&out};
    transformTextureCoordinates2DInPlace(mesh, {}, data.id, data.morphTargetId);
    if(data.morphTargetId == -1)
        CORRADE_COMPARE(out, "MeshTools::transformTextureCoordinates2DInPlace(): the mesh has no texture coordinates with index 1\n");
    else
        CORRADE_COMPARE(out, "MeshTools::transformTextureCoordinates2DInPlace(): the mesh has no texture coordinates with index 0 in morph target 37\n");
}

void TransformTest::meshDataTextureCoordinates2DInPlaceWrongFormat() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData mesh{MeshPrimitive::Points, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, VertexFormat::Vector2, nullptr},
        Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, VertexFormat::Vector2us, nullptr}
    }};

    Containers::String out;
    Error redirectError{&out};
    transformTextureCoordinates2DInPlace(mesh, {}, 1);
    CORRADE_COMPARE(out, "MeshTools::transformTextureCoordinates2DInPlace(): expected VertexFormat::Vector2 texture coordinates but got VertexFormat::Vector2us\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::TransformTest)
