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

#include "Magnum/Trade/MeshObjectData3D.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

class ObjectData3DTest: public TestSuite::Tester {
    public:
        explicit ObjectData3DTest();

        void constructEmpty();
        void constructEmptyTransformations();
        void constructMesh();
        void constructMeshTransformations();
        void constructCamera();
        void constructLight();
        void constructCopy();
        void constructMoveTransformations();
        void constructMoveMesh();

        void accessInvalidTransformations();

        void debugType();
        void debugFlag();
        void debugFlags();
};

ObjectData3DTest::ObjectData3DTest() {
    addTests({&ObjectData3DTest::constructEmpty,
              &ObjectData3DTest::constructEmptyTransformations,
              &ObjectData3DTest::constructMesh,
              &ObjectData3DTest::constructMeshTransformations,
              &ObjectData3DTest::constructCamera,
              &ObjectData3DTest::constructLight,
              &ObjectData3DTest::constructCopy,
              &ObjectData3DTest::constructMoveTransformations,
              &ObjectData3DTest::constructMoveMesh,

              &ObjectData3DTest::accessInvalidTransformations,

              &ObjectData3DTest::debugType,
              &ObjectData3DTest::debugFlag,
              &ObjectData3DTest::debugFlags});
}

using namespace Math::Literals;

void ObjectData3DTest::constructEmpty() {
    const int a{};
    const ObjectData3D data{{0, 2, 3}, Matrix4::translation(Vector3::xAxis(-4.0f)), &a};

    CORRADE_COMPARE(data.children(), (std::vector<UnsignedInt>{0, 2, 3}));
    CORRADE_COMPARE(data.flags(), ObjectFlags3D{});
    CORRADE_COMPARE(data.transformation(), Matrix4::translation(Vector3::xAxis(-4.0f)));
    CORRADE_COMPARE(data.instanceType(), ObjectInstanceType3D::Empty);
    CORRADE_COMPARE(data.instance(), -1);
    CORRADE_COMPARE(data.importerState(), &a);
}

void ObjectData3DTest::constructEmptyTransformations() {
    const int a{};
    const ObjectData3D data{{0, 2, 3}, Vector3::xAxis(-4.0f), Quaternion::rotation(32.5_degf, Vector3::zAxis()), Vector3::yScale(1.5f), &a};

    CORRADE_COMPARE(data.children(), (std::vector<UnsignedInt>{0, 2, 3}));
    CORRADE_COMPARE(data.flags(), ObjectFlag3D::HasTranslationRotationScaling);
    CORRADE_COMPARE(data.translation(), Vector3::xAxis(-4.0f));
    CORRADE_COMPARE(data.rotation(), Quaternion::rotation(32.5_degf, Vector3::zAxis()));
    CORRADE_COMPARE(data.scaling(), Vector3::yScale(1.5f));
    CORRADE_COMPARE(data.transformation(),
        Matrix4::translation(Vector3::xAxis(-4.0f))*
        Matrix4::rotationZ(32.5_degf)*
        Matrix4::scaling(Vector3::yScale(1.5f)));
    CORRADE_COMPARE(data.instanceType(), ObjectInstanceType3D::Empty);
    CORRADE_COMPARE(data.instance(), -1);
    CORRADE_COMPARE(data.importerState(), &a);
}

void ObjectData3DTest::constructMesh() {
    const int a{};
    const MeshObjectData3D data{{1, 3}, Matrix4::translation(Vector3::yAxis(5.0f)), 13, 42, 5, &a};

    CORRADE_COMPARE(data.children(), (std::vector<UnsignedInt>{1, 3}));
    CORRADE_COMPARE(data.flags(), ObjectFlags3D{});
    CORRADE_COMPARE(data.transformation(), Matrix4::translation(Vector3::yAxis(5.0f)));
    CORRADE_COMPARE(data.instanceType(), ObjectInstanceType3D::Mesh);
    CORRADE_COMPARE(data.instance(), 13);
    CORRADE_COMPARE(data.material(), 42);
    CORRADE_COMPARE(data.skin(), 5);
    CORRADE_COMPARE(data.importerState(), &a);
}

void ObjectData3DTest::constructMeshTransformations() {
    const int a{};
    const MeshObjectData3D data{{1, 3}, Vector3::xAxis(-4.0f), Quaternion::rotation(32.5_degf, Vector3::zAxis()), Vector3::yScale(1.5f), 13, 42, 5, &a};

    CORRADE_COMPARE(data.children(), (std::vector<UnsignedInt>{1, 3}));
    CORRADE_COMPARE(data.flags(), ObjectFlag3D::HasTranslationRotationScaling);
    CORRADE_COMPARE(data.translation(), Vector3::xAxis(-4.0f));
    CORRADE_COMPARE(data.rotation(), Quaternion::rotation(32.5_degf, Vector3::zAxis()));
    CORRADE_COMPARE(data.scaling(), Vector3::yScale(1.5f));
    CORRADE_COMPARE(data.transformation(),
        Matrix4::translation(Vector3::xAxis(-4.0f))*
        Matrix4::rotationZ(32.5_degf)*
        Matrix4::scaling(Vector3::yScale(1.5f)));
    CORRADE_COMPARE(data.instanceType(), ObjectInstanceType3D::Mesh);
    CORRADE_COMPARE(data.instance(), 13);
    CORRADE_COMPARE(data.material(), 42);
    CORRADE_COMPARE(data.skin(), 5);
    CORRADE_COMPARE(data.importerState(), &a);
}

void ObjectData3DTest::constructCamera() {
    const int a{};
    const ObjectData3D data{{1, 3}, Matrix4::translation(Vector3::yAxis(5.0f)), ObjectInstanceType3D::Camera, 42, &a};

    CORRADE_COMPARE(data.children(), (std::vector<UnsignedInt>{1, 3}));
    CORRADE_COMPARE(data.flags(), ObjectFlags3D{});
    CORRADE_COMPARE(data.transformation(), Matrix4::translation(Vector3::yAxis(5.0f)));
    CORRADE_COMPARE(data.instanceType(), ObjectInstanceType3D::Camera);
    CORRADE_COMPARE(data.instance(), 42);
    CORRADE_COMPARE(data.importerState(), &a);
}

void ObjectData3DTest::constructLight() {
    const int a{};
    const ObjectData3D data{{1, 3}, Matrix4::translation(Vector3::yAxis(5.0f)), ObjectInstanceType3D::Light, 42, &a};

    CORRADE_COMPARE(data.children(), (std::vector<UnsignedInt>{1, 3}));
    CORRADE_COMPARE(data.flags(), ObjectFlags3D{});
    CORRADE_COMPARE(data.transformation(), Matrix4::translation(Vector3::yAxis(5.0f)));
    CORRADE_COMPARE(data.instanceType(), ObjectInstanceType3D::Light);
    CORRADE_COMPARE(data.instance(), 42);
    CORRADE_COMPARE(data.importerState(), &a);
}

void ObjectData3DTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<ObjectData3D, const ObjectData3D&>{}));
    CORRADE_VERIFY(!(std::is_constructible<MeshObjectData3D, const MeshObjectData3D&>{}));
    CORRADE_VERIFY(!(std::is_assignable<ObjectData3D, const ObjectData3D&>{}));
    CORRADE_VERIFY(!(std::is_assignable<MeshObjectData3D, const MeshObjectData3D&>{}));
}

void ObjectData3DTest::constructMoveTransformations() {
    const int a{};
    ObjectData3D data{{1, 3}, Vector3::xAxis(-4.0f), Quaternion::rotation(32.5_degf, Vector3::zAxis()), Vector3::yScale(1.5f), ObjectInstanceType3D::Light, 13, &a};

    ObjectData3D b{std::move(data)};

    CORRADE_COMPARE(b.children(), (std::vector<UnsignedInt>{1, 3}));
    CORRADE_COMPARE(b.flags(), ObjectFlag3D::HasTranslationRotationScaling);
    CORRADE_COMPARE(b.translation(), Vector3::xAxis(-4.0f));
    CORRADE_COMPARE(b.rotation(), Quaternion::rotation(32.5_degf, Vector3::zAxis()));
    CORRADE_COMPARE(b.scaling(), Vector3::yScale(1.5f));
    CORRADE_COMPARE(b.transformation(),
        Matrix4::translation(Vector3::xAxis(-4.0f))*
        Matrix4::rotationZ(32.5_degf)*
        Matrix4::scaling(Vector3::yScale(1.5f)));
    CORRADE_COMPARE(b.instanceType(), ObjectInstanceType3D::Light);
    CORRADE_COMPARE(b.instance(), 13);
    CORRADE_COMPARE(b.importerState(), &a);

    const int c{};
    ObjectData3D d{{0, 1}, Matrix4{}, ObjectInstanceType3D::Empty, 27, &c};

    d = std::move(b);

    CORRADE_COMPARE(d.children(), (std::vector<UnsignedInt>{1, 3}));
    CORRADE_COMPARE(d.flags(), ObjectFlag3D::HasTranslationRotationScaling);
    CORRADE_COMPARE(d.translation(), Vector3::xAxis(-4.0f));
    CORRADE_COMPARE(d.rotation(), Quaternion::rotation(32.5_degf, Vector3::zAxis()));
    CORRADE_COMPARE(d.scaling(), Vector3::yScale(1.5f));
    CORRADE_COMPARE(d.transformation(),
        Matrix4::translation(Vector3::xAxis(-4.0f))*
        Matrix4::rotationZ(32.5_degf)*
        Matrix4::scaling(Vector3::yScale(1.5f)));
    CORRADE_COMPARE(d.instanceType(), ObjectInstanceType3D::Light);
    CORRADE_COMPARE(d.instance(), 13);
    CORRADE_COMPARE(d.importerState(), &a);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<ObjectData3D>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<ObjectData3D>::value);
}

void ObjectData3DTest::constructMoveMesh() {
    const int a{};
    MeshObjectData3D data{{1, 3}, Matrix4::translation(Vector3::yAxis(5.0f)), 13, 42, 5, &a};

    MeshObjectData3D b{std::move(data)};

    CORRADE_COMPARE(b.children(), (std::vector<UnsignedInt>{1, 3}));
    CORRADE_COMPARE(data.flags(), ObjectFlags3D{});
    CORRADE_COMPARE(b.transformation(), Matrix4::translation(Vector3::yAxis(5.0f)));
    CORRADE_COMPARE(b.instanceType(), ObjectInstanceType3D::Mesh);
    CORRADE_COMPARE(b.instance(), 13);
    CORRADE_COMPARE(b.material(), 42);
    CORRADE_COMPARE(b.skin(), 5);
    CORRADE_COMPARE(b.importerState(), &a);

    const int c{};
    MeshObjectData3D d{{0, 1}, {}, 27, -1, -1, &c};

    d = std::move(b);

    CORRADE_COMPARE(d.children(), (std::vector<UnsignedInt>{1, 3}));
    CORRADE_COMPARE(data.flags(), ObjectFlags3D{});
    CORRADE_COMPARE(d.transformation(), Matrix4::translation(Vector3::yAxis(5.0f)));
    CORRADE_COMPARE(d.instanceType(), ObjectInstanceType3D::Mesh);
    CORRADE_COMPARE(d.instance(), 13);
    CORRADE_COMPARE(d.material(), 42);
    CORRADE_COMPARE(d.skin(), 5);
    CORRADE_COMPARE(d.importerState(), &a);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<MeshObjectData3D>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<MeshObjectData3D>::value);
}

void ObjectData3DTest::accessInvalidTransformations() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectOutput{&out};

    const ObjectData3D data{{}, Matrix4{}};
    data.translation();
    data.rotation();
    data.scaling();

    CORRADE_COMPARE(out.str(),
        "Trade::ObjectData3D::translation(): object has only a combined transformation\n"
        "Trade::ObjectData3D::rotation(): object has only a combined transformation\n"
        "Trade::ObjectData3D::scaling(): object has only a combined transformation\n");
}

void ObjectData3DTest::debugType() {
    std::ostringstream o;
    Debug(&o) << ObjectInstanceType3D::Light << ObjectInstanceType3D(0xbe);
    CORRADE_COMPARE(o.str(), "Trade::ObjectInstanceType3D::Light Trade::ObjectInstanceType3D(0xbe)\n");
}

void ObjectData3DTest::debugFlag() {
    std::ostringstream o;
    Debug(&o) << ObjectFlag3D::HasTranslationRotationScaling << ObjectFlag3D(0xbe);
    CORRADE_COMPARE(o.str(), "Trade::ObjectFlag3D::HasTranslationRotationScaling Trade::ObjectFlag3D(0xbe)\n");
}

void ObjectData3DTest::debugFlags() {
    std::ostringstream o;
    Debug(&o) << (ObjectFlag3D::HasTranslationRotationScaling|ObjectFlags3D{}) << ObjectFlags3D{};
    CORRADE_COMPARE(o.str(), "Trade::ObjectFlag3D::HasTranslationRotationScaling Trade::ObjectFlags3D{}\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::ObjectData3DTest)
