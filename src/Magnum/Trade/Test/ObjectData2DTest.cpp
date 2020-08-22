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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Trade/MeshObjectData2D.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

class ObjectData2DTest: public TestSuite::Tester {
    public:
        explicit ObjectData2DTest();

        void constructEmpty();
        void constructEmptyTransformations();
        void constructMesh();
        void constructMeshTransformations();
        void constructCamera();
        void constructCopy();
        void constructMoveTransformations();
        void constructMoveMesh();

        void accessInvalidTransformations();

        void debugType();
        void debugFlag();
        void debugFlags();
};

ObjectData2DTest::ObjectData2DTest() {
    addTests({&ObjectData2DTest::constructEmpty,
              &ObjectData2DTest::constructEmptyTransformations,
              &ObjectData2DTest::constructMesh,
              &ObjectData2DTest::constructMeshTransformations,
              &ObjectData2DTest::constructCamera,
              &ObjectData2DTest::constructCopy,
              &ObjectData2DTest::constructMoveTransformations,
              &ObjectData2DTest::constructMoveMesh,

              &ObjectData2DTest::accessInvalidTransformations,

              &ObjectData2DTest::debugType,
              &ObjectData2DTest::debugFlag,
              &ObjectData2DTest::debugFlags});
}

using namespace Math::Literals;

void ObjectData2DTest::constructEmpty() {
    const int a{};
    const ObjectData2D data{{0, 2, 3}, Matrix3::translation(Vector2::xAxis(-4.0f)), &a};

    CORRADE_COMPARE(data.children(), (std::vector<UnsignedInt>{0, 2, 3}));
    CORRADE_COMPARE(data.flags(), ObjectFlags2D{});
    CORRADE_COMPARE(data.transformation(), Matrix3::translation(Vector2::xAxis(-4.0f)));
    CORRADE_COMPARE(data.instanceType(), ObjectInstanceType2D::Empty);
    CORRADE_COMPARE(data.instance(), -1);
    CORRADE_COMPARE(data.importerState(), &a);
}

void ObjectData2DTest::constructEmptyTransformations() {
    const int a{};
    const ObjectData2D data{{0, 2, 3}, Vector2::xAxis(-4.0f), Complex::rotation(32.5_degf), Vector2::yScale(1.5f), &a};

    CORRADE_COMPARE(data.children(), (std::vector<UnsignedInt>{0, 2, 3}));
    CORRADE_COMPARE(data.flags(), ObjectFlag2D::HasTranslationRotationScaling);
    CORRADE_COMPARE(data.translation(), Vector2::xAxis(-4.0f));
    CORRADE_COMPARE(data.rotation(), Complex::rotation(32.5_degf));
    CORRADE_COMPARE(data.scaling(), Vector2::yScale(1.5f));
    CORRADE_COMPARE(data.transformation(),
        Matrix3::translation(Vector2::xAxis(-4.0f))*
        Matrix3::rotation(32.5_degf)*
        Matrix3::scaling(Vector2::yScale(1.5f)));
    CORRADE_COMPARE(data.instanceType(), ObjectInstanceType2D::Empty);
    CORRADE_COMPARE(data.instance(), -1);
    CORRADE_COMPARE(data.importerState(), &a);
}

void ObjectData2DTest::constructMesh() {
    const int a{};
    const MeshObjectData2D data{{1, 3}, Matrix3::translation(Vector2::yAxis(5.0f)), 13, 42, 5, &a};

    CORRADE_COMPARE(data.children(), (std::vector<UnsignedInt>{1, 3}));
    CORRADE_COMPARE(data.flags(), ObjectFlags2D{});
    CORRADE_COMPARE(data.transformation(), Matrix3::translation(Vector2::yAxis(5.0f)));
    CORRADE_COMPARE(data.instanceType(), ObjectInstanceType2D::Mesh);
    CORRADE_COMPARE(data.instance(), 13);
    CORRADE_COMPARE(data.material(), 42);
    CORRADE_COMPARE(data.skin(), 5);
    CORRADE_COMPARE(data.importerState(), &a);
}

void ObjectData2DTest::constructMeshTransformations() {
    const int a{};
    const MeshObjectData2D data{{0, 2, 3}, Vector2::xAxis(-4.0f), Complex::rotation(32.5_degf), Vector2::yScale(1.5f), 13, 42, 5, &a};

    CORRADE_COMPARE(data.children(), (std::vector<UnsignedInt>{0, 2, 3}));
    CORRADE_COMPARE(data.flags(), ObjectFlag2D::HasTranslationRotationScaling);
    CORRADE_COMPARE(data.translation(), Vector2::xAxis(-4.0f));
    CORRADE_COMPARE(data.rotation(), Complex::rotation(32.5_degf));
    CORRADE_COMPARE(data.scaling(), Vector2::yScale(1.5f));
    CORRADE_COMPARE(data.transformation(),
        Matrix3::translation(Vector2::xAxis(-4.0f))*
        Matrix3::rotation(32.5_degf)*
        Matrix3::scaling(Vector2::yScale(1.5f)));
    CORRADE_COMPARE(data.instanceType(), ObjectInstanceType2D::Mesh);
    CORRADE_COMPARE(data.instance(), 13);
    CORRADE_COMPARE(data.material(), 42);
    CORRADE_COMPARE(data.skin(), 5);
    CORRADE_COMPARE(data.importerState(), &a);
}

void ObjectData2DTest::constructCamera() {
    const int a{};
    const ObjectData2D data{{1, 3}, Matrix3::translation(Vector2::yAxis(5.0f)), ObjectInstanceType2D::Camera, 42, &a};

    CORRADE_COMPARE(data.children(), (std::vector<UnsignedInt>{1, 3}));
    CORRADE_COMPARE(data.flags(), ObjectFlags2D{});
    CORRADE_COMPARE(data.transformation(), Matrix3::translation(Vector2::yAxis(5.0f)));
    CORRADE_COMPARE(data.instanceType(), ObjectInstanceType2D::Camera);
    CORRADE_COMPARE(data.instance(), 42);
    CORRADE_COMPARE(data.importerState(), &a);
}

void ObjectData2DTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<ObjectData2D, const ObjectData2D&>{}));
    CORRADE_VERIFY(!(std::is_constructible<MeshObjectData2D, const MeshObjectData2D&>{}));
    CORRADE_VERIFY(!(std::is_assignable<ObjectData2D, const ObjectData2D&>{}));
    CORRADE_VERIFY(!(std::is_assignable<MeshObjectData2D, const MeshObjectData2D&>{}));
}

void ObjectData2DTest::constructMoveTransformations() {
    const int a{};
    ObjectData2D data{{1, 3}, Vector2::xAxis(-4.0f), Complex::rotation(32.5_degf), Vector2::yScale(1.5f), ObjectInstanceType2D::Camera, 13, &a};

    ObjectData2D b{std::move(data)};

    CORRADE_COMPARE(b.children(), (std::vector<UnsignedInt>{1, 3}));
    CORRADE_COMPARE(b.flags(), ObjectFlag2D::HasTranslationRotationScaling);
    CORRADE_COMPARE(b.translation(), Vector2::xAxis(-4.0f));
    CORRADE_COMPARE(b.rotation(), Complex::rotation(32.5_degf));
    CORRADE_COMPARE(b.scaling(), Vector2::yScale(1.5f));
    CORRADE_COMPARE(b.transformation(),
        Matrix3::translation(Vector2::xAxis(-4.0f))*
        Matrix3::rotation(32.5_degf)*
        Matrix3::scaling(Vector2::yScale(1.5f)));
    CORRADE_COMPARE(b.instanceType(), ObjectInstanceType2D::Camera);
    CORRADE_COMPARE(b.instance(), 13);
    CORRADE_COMPARE(b.importerState(), &a);

    const int c{};
    ObjectData2D d{{0, 1}, Matrix3{}, ObjectInstanceType2D::Empty, 27, &c};

    d = std::move(b);

    CORRADE_COMPARE(d.children(), (std::vector<UnsignedInt>{1, 3}));
    CORRADE_COMPARE(d.flags(), ObjectFlag2D::HasTranslationRotationScaling);
    CORRADE_COMPARE(d.translation(), Vector2::xAxis(-4.0f));
    CORRADE_COMPARE(d.rotation(), Complex::rotation(32.5_degf));
    CORRADE_COMPARE(d.scaling(), Vector2::yScale(1.5f));
    CORRADE_COMPARE(d.transformation(),
        Matrix3::translation(Vector2::xAxis(-4.0f))*
        Matrix3::rotation(32.5_degf)*
        Matrix3::scaling(Vector2::yScale(1.5f)));
    CORRADE_COMPARE(d.instanceType(), ObjectInstanceType2D::Camera);
    CORRADE_COMPARE(d.instance(), 13);
    CORRADE_COMPARE(d.importerState(), &a);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<ObjectData2D>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<ObjectData2D>::value);
}

void ObjectData2DTest::constructMoveMesh() {
    const int a{};
    MeshObjectData2D data{{1, 3}, Matrix3::translation(Vector2::yAxis(5.0f)), 13, 42, 5, &a};

    MeshObjectData2D b{std::move(data)};

    CORRADE_COMPARE(b.children(), (std::vector<UnsignedInt>{1, 3}));
    CORRADE_COMPARE(b.flags(), ObjectFlags2D{});
    CORRADE_COMPARE(b.transformation(), Matrix3::translation(Vector2::yAxis(5.0f)));
    CORRADE_COMPARE(b.instanceType(), ObjectInstanceType2D::Mesh);
    CORRADE_COMPARE(b.instance(), 13);
    CORRADE_COMPARE(b.material(), 42);
    CORRADE_COMPARE(b.skin(), 5);
    CORRADE_COMPARE(b.importerState(), &a);

    const int c{};
    MeshObjectData2D d{{0, 1}, {}, 27, -1, -1, &c};

    d = std::move(b);

    CORRADE_COMPARE(d.children(), (std::vector<UnsignedInt>{1, 3}));
    CORRADE_COMPARE(d.flags(), ObjectFlags2D{});
    CORRADE_COMPARE(d.transformation(), Matrix3::translation(Vector2::yAxis(5.0f)));
    CORRADE_COMPARE(d.instanceType(), ObjectInstanceType2D::Mesh);
    CORRADE_COMPARE(d.instance(), 13);
    CORRADE_COMPARE(d.material(), 42);
    CORRADE_COMPARE(d.skin(), 5);
    CORRADE_COMPARE(d.importerState(), &a);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<MeshObjectData2D>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<MeshObjectData2D>::value);
}

void ObjectData2DTest::accessInvalidTransformations() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectOutput{&out};

    const ObjectData2D data{{}, Matrix3{}};
    data.translation();
    data.rotation();
    data.scaling();

    CORRADE_COMPARE(out.str(),
        "Trade::ObjectData2D::translation(): object has only a combined transformation\n"
        "Trade::ObjectData2D::rotation(): object has only a combined transformation\n"
        "Trade::ObjectData2D::scaling(): object has only a combined transformation\n");
}

void ObjectData2DTest::debugType() {
    std::ostringstream o;
    Debug(&o) << ObjectInstanceType2D::Empty << ObjectInstanceType2D(0xbe);
    CORRADE_COMPARE(o.str(), "Trade::ObjectInstanceType2D::Empty Trade::ObjectInstanceType2D(0xbe)\n");
}

void ObjectData2DTest::debugFlag() {
    std::ostringstream o;
    Debug(&o) << ObjectFlag2D::HasTranslationRotationScaling << ObjectFlag2D(0xbe);
    CORRADE_COMPARE(o.str(), "Trade::ObjectFlag2D::HasTranslationRotationScaling Trade::ObjectFlag2D(0xbe)\n");
}

void ObjectData2DTest::debugFlags() {
    std::ostringstream o;
    Debug(&o) << (ObjectFlag2D::HasTranslationRotationScaling|ObjectFlags2D{}) << ObjectFlags2D{};
    CORRADE_COMPARE(o.str(), "Trade::ObjectFlag2D::HasTranslationRotationScaling Trade::ObjectFlags2D{}\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::ObjectData2DTest)
