/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Trade/MeshObjectData2D.h"

namespace Magnum { namespace Trade { namespace Test {

class ObjectData2DTest: public TestSuite::Tester {
    public:
        explicit ObjectData2DTest();

        void constructEmpty();
        void constructMesh();
        void constructCamera();
        void constructCopy();
        void constructMoveMesh();

        void debugType();
};

ObjectData2DTest::ObjectData2DTest() {
    addTests({&ObjectData2DTest::constructEmpty,
              &ObjectData2DTest::constructMesh,
              &ObjectData2DTest::constructCamera,
              &ObjectData2DTest::constructCopy,
              &ObjectData2DTest::constructMoveMesh,

              &ObjectData2DTest::debugType});
}

void ObjectData2DTest::constructEmpty() {
    const int a{};
    const ObjectData2D data{{0, 2, 3}, Matrix3::translation(Vector2::xAxis(-4.0f)), &a};

    CORRADE_COMPARE(data.children(), (std::vector<UnsignedInt>{0, 2, 3}));
    CORRADE_COMPARE(data.transformation(), Matrix3::translation(Vector2::xAxis(-4.0f)));
    CORRADE_COMPARE(data.instanceType(), ObjectInstanceType2D::Empty);
    CORRADE_COMPARE(data.instance(), -1);
}

void ObjectData2DTest::constructMesh() {
    const int a{};
    const MeshObjectData2D data{{1, 3}, Matrix3::translation(Vector2::yAxis(5.0f)), 13, 42, &a};

    CORRADE_COMPARE(data.children(), (std::vector<UnsignedInt>{1, 3}));
    CORRADE_COMPARE(data.transformation(), Matrix3::translation(Vector2::yAxis(5.0f)));
    CORRADE_COMPARE(data.instanceType(), ObjectInstanceType2D::Mesh);
    CORRADE_COMPARE(data.instance(), 13);
    CORRADE_COMPARE(data.material(), 42);
}

void ObjectData2DTest::constructCamera() {
    const int a{};
    const ObjectData2D data{{1, 3}, Matrix3::translation(Vector2::yAxis(5.0f)), ObjectInstanceType2D::Camera, 42, &a};

    CORRADE_COMPARE(data.children(), (std::vector<UnsignedInt>{1, 3}));
    CORRADE_COMPARE(data.transformation(), Matrix3::translation(Vector2::yAxis(5.0f)));
    CORRADE_COMPARE(data.instanceType(), ObjectInstanceType2D::Camera);
    CORRADE_COMPARE(data.instance(), 42);
}

void ObjectData2DTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<ObjectData2D, const ObjectData2D&>{}));
    CORRADE_VERIFY(!(std::is_constructible<MeshObjectData2D, const MeshObjectData2D&>{}));
    CORRADE_VERIFY(!(std::is_assignable<ObjectData2D, const ObjectData2D&>{}));
    CORRADE_VERIFY(!(std::is_assignable<MeshObjectData2D, const MeshObjectData2D&>{}));
}

void ObjectData2DTest::constructMoveMesh() {
    const int a{};
    MeshObjectData2D data{{1, 3}, Matrix3::translation(Vector2::yAxis(5.0f)), 13, 42, &a};

    MeshObjectData2D b{std::move(data)};

    CORRADE_COMPARE(b.children(), (std::vector<UnsignedInt>{1, 3}));
    CORRADE_COMPARE(b.transformation(), Matrix3::translation(Vector2::yAxis(5.0f)));
    CORRADE_COMPARE(b.instanceType(), ObjectInstanceType2D::Mesh);
    CORRADE_COMPARE(b.instance(), 13);
    CORRADE_COMPARE(b.material(), 42);

    const int c{};
    MeshObjectData2D d{{0, 1}, {}, 27, -1, &c};

    d = std::move(b);

    CORRADE_COMPARE(d.children(), (std::vector<UnsignedInt>{1, 3}));
    CORRADE_COMPARE(d.transformation(), Matrix3::translation(Vector2::yAxis(5.0f)));
    CORRADE_COMPARE(d.instanceType(), ObjectInstanceType2D::Mesh);
    CORRADE_COMPARE(d.instance(), 13);
    CORRADE_COMPARE(d.material(), 42);
}

void ObjectData2DTest::debugType() {
    std::ostringstream o;
    Debug(&o) << ObjectInstanceType2D::Empty << ObjectInstanceType2D(0xbe);
    CORRADE_COMPARE(o.str(), "Trade::ObjectInstanceType2D::Empty Trade::ObjectInstanceType2D(0xbe)\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::ObjectData2DTest)
