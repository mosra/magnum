/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Shaders/Generic.h"
/* Yes, really */
#include "Magnum/Shaders/generic.glsl"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct GenericTest: TestSuite::Tester {
    explicit GenericTest();

    void glslMatch();
    void glslMatchOutput();

    void tbnContiguous();
    void tbnBothNormalAndQuaternion();
    void textureTransformContiguous();
};

GenericTest::GenericTest() {
    addTests({&GenericTest::glslMatch,
              &GenericTest::glslMatchOutput,

              &GenericTest::tbnContiguous,
              &GenericTest::tbnBothNormalAndQuaternion,
              &GenericTest::textureTransformContiguous});
}

void GenericTest::glslMatch() {
    CORRADE_COMPARE(POSITION_ATTRIBUTE_LOCATION, Generic2D::Position::Location);
    CORRADE_COMPARE(POSITION_ATTRIBUTE_LOCATION, Generic3D::Position::Location);

    CORRADE_COMPARE(TEXTURECOORDINATES_ATTRIBUTE_LOCATION, Generic2D::TextureCoordinates::Location);
    CORRADE_COMPARE(TEXTURECOORDINATES_ATTRIBUTE_LOCATION, Generic3D::TextureCoordinates::Location);

    CORRADE_COMPARE(COLOR_ATTRIBUTE_LOCATION, Generic2D::Color3::Location);
    CORRADE_COMPARE(COLOR_ATTRIBUTE_LOCATION, Generic3D::Color3::Location);
    CORRADE_COMPARE(COLOR_ATTRIBUTE_LOCATION, Generic2D::Color4::Location);
    CORRADE_COMPARE(COLOR_ATTRIBUTE_LOCATION, Generic3D::Color4::Location);

    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(OBJECT_ID_ATTRIBUTE_LOCATION, Generic2D::ObjectId::Location);
    CORRADE_COMPARE(OBJECT_ID_ATTRIBUTE_LOCATION, Generic3D::ObjectId::Location);
    #endif

    CORRADE_COMPARE(TANGENT_ATTRIBUTE_LOCATION, Generic3D::Tangent::Location);
    CORRADE_COMPARE(TANGENT_ATTRIBUTE_LOCATION, Generic3D::Tangent4::Location);
    CORRADE_COMPARE(BITANGENT_ATTRIBUTE_LOCATION, Generic3D::Bitangent::Location);
    CORRADE_COMPARE(NORMAL_ATTRIBUTE_LOCATION, Generic3D::Normal::Location);

    CORRADE_COMPARE(TRANSFORMATION_MATRIX_ATTRIBUTE_LOCATION, Generic2D::TransformationMatrix::Location);
    CORRADE_COMPARE(TRANSFORMATION_MATRIX_ATTRIBUTE_LOCATION, Generic3D::TransformationMatrix::Location);

    CORRADE_COMPARE(NORMAL_MATRIX_ATTRIBUTE_LOCATION, Generic3D::NormalMatrix::Location);

    CORRADE_COMPARE(TEXTURE_OFFSET_ATTRIBUTE_LOCATION, Generic2D::TextureOffset::Location);
    CORRADE_COMPARE(TEXTURE_OFFSET_ATTRIBUTE_LOCATION, Generic3D::TextureOffset::Location);
}

void GenericTest::glslMatchOutput() {
    CORRADE_COMPARE(COLOR_OUTPUT_ATTRIBUTE_LOCATION, Generic2D::ColorOutput);
    CORRADE_COMPARE(COLOR_OUTPUT_ATTRIBUTE_LOCATION, Generic3D::ColorOutput);

    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(OBJECT_ID_OUTPUT_ATTRIBUTE_LOCATION, Generic2D::ObjectIdOutput);
    CORRADE_COMPARE(OBJECT_ID_OUTPUT_ATTRIBUTE_LOCATION, Generic3D::ObjectIdOutput);
    #endif
}

void GenericTest::tbnContiguous() {
    CORRADE_COMPARE(Generic3D::Tangent::Location + 1, Generic3D::Bitangent::Location);
    CORRADE_COMPARE(Generic3D::Bitangent::Location + 1, Generic3D::Normal::Location);
}

void GenericTest::tbnBothNormalAndQuaternion() {
    CORRADE_SKIP("Quaternion TBN not implemented yet.");

    //CORRADE_VERIFY(Generic3D::TbnQuaternion::Location != Generic3D::Normal::Location);
}

void GenericTest::textureTransformContiguous() {
    /* These depend on DualQuaternion-based (instanced) transformation */
    CORRADE_SKIP("TextureRotationScale and TextureMatrix attributes not implemented yet.");

    //CORRADE_COMPARE(Generic3D::TextureRotationScale::Location, Generic3D::TextureMatrix::Location);
    //CORRADE_COMPARE(Generic3D::TextureOffset::Location, Generic3D::TextureMatrix::Location + 2);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::GenericTest)
