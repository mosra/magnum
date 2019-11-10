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
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Mesh.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/MeshView.h"

namespace Magnum { namespace GL { namespace Test { namespace {

/* Tests MeshView as well */

struct MeshTest: TestSuite::Tester {
    explicit MeshTest();

    void constructNoCreate();
    void constructViewNoCreate();

    void constructCopy();
    void constructMoveNoCreate();
    /* View *is* copyable */

    void drawCountNotSet();
    void drawViewCountNotSet();

    void mapPrimitive();
    void mapPrimitiveInvalid();
    void mapIndexType();
    void mapIndexTypeInvalid();

    void debugPrimitive();
    void debugIndexType();
};

MeshTest::MeshTest() {
    addTests({&MeshTest::constructNoCreate,
              &MeshTest::constructViewNoCreate,

              &MeshTest::constructCopy,
              &MeshTest::constructMoveNoCreate,

              &MeshTest::drawCountNotSet,
              &MeshTest::drawViewCountNotSet,

              &MeshTest::mapPrimitive,
              &MeshTest::mapPrimitiveInvalid,
              &MeshTest::mapIndexType,
              &MeshTest::mapIndexTypeInvalid,

              &MeshTest::debugPrimitive,
              &MeshTest::debugIndexType});
}

void MeshTest::constructNoCreate() {
    {
        Mesh mesh{NoCreate};
        CORRADE_COMPARE(mesh.id(), 0);
    }

    CORRADE_VERIFY(true);
}

void MeshTest::constructViewNoCreate() {
    {
        Mesh mesh{NoCreate};
        MeshView view{mesh};
        CORRADE_COMPARE(&view.mesh(), &mesh);
    }

    CORRADE_VERIFY(true);
}

void MeshTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<Mesh, const Mesh&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Mesh, const Mesh&>{}));
}

void MeshTest::constructMoveNoCreate() {
    /* Neither of these should be accessing the GL context */
    {
        Mesh a{NoCreate};
        CORRADE_COMPARE(a.id(), 0);

        Mesh b{std::move(a)};
        CORRADE_COMPARE(b.id(), 0);

        Mesh c{NoCreate};
        c = std::move(b);
        CORRADE_COMPARE(c.id(), 0);
    }

    CORRADE_VERIFY(true);
}

struct Shader: AbstractShaderProgram {
    explicit Shader(NoCreateT): AbstractShaderProgram{NoCreate} {}
};

void MeshTest::drawCountNotSet() {
    std::ostringstream out;
    Error redirectError{&out};

    Mesh mesh{NoCreate};
    mesh.draw(Shader{NoCreate});

    CORRADE_COMPARE(out.str(),
        "GL::Mesh::draw(): setCount() was never called, probably a mistake?\n");
}

void MeshTest::drawViewCountNotSet() {
    std::ostringstream out;
    Error redirectError{&out};

    Mesh mesh{NoCreate};
    MeshView view{mesh};
    view.draw(Shader{NoCreate});

    CORRADE_COMPARE(out.str(),
        "GL::MeshView::draw(): setCount() was never called, probably a mistake?\n");
}

void MeshTest::mapPrimitive() {
    CORRADE_COMPARE(meshPrimitive(Magnum::MeshPrimitive::Points), MeshPrimitive::Points);
    CORRADE_COMPARE(meshPrimitive(Magnum::MeshPrimitive::Lines), MeshPrimitive::Lines);
    CORRADE_COMPARE(meshPrimitive(Magnum::MeshPrimitive::LineLoop), MeshPrimitive::LineLoop);
    CORRADE_COMPARE(meshPrimitive(Magnum::MeshPrimitive::LineStrip), MeshPrimitive::LineStrip);
    CORRADE_COMPARE(meshPrimitive(Magnum::MeshPrimitive::Triangles), MeshPrimitive::Triangles);
    CORRADE_COMPARE(meshPrimitive(Magnum::MeshPrimitive::TriangleStrip), MeshPrimitive::TriangleStrip);
    CORRADE_COMPARE(meshPrimitive(Magnum::MeshPrimitive::TriangleFan), MeshPrimitive::TriangleFan);

    /* Ensure all generic primitives are handled. This goes through the first
       16 bits, which should be enough. Going through 32 bits takes 8 seconds,
       too much. */
    for(UnsignedInt i = 1; i <= 0xffff; ++i) {
        const auto primitive = Magnum::MeshPrimitive(i);
        #ifdef __GNUC__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic error "-Wswitch"
        #endif
        switch(primitive) {
            #define _c(primitive) \
                case Magnum::MeshPrimitive::primitive: \
                    CORRADE_VERIFY(UnsignedInt(meshPrimitive(Magnum::MeshPrimitive::primitive)) >= 0); \
                    break;
            #include "Magnum/Implementation/meshPrimitiveMapping.hpp"
            #undef _c
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #endif
    }
}

void MeshTest::mapPrimitiveInvalid() {
    std::ostringstream out;
    Error redirectError{&out};

    meshPrimitive(Magnum::MeshPrimitive{});
    meshPrimitive(Magnum::MeshPrimitive(0x12));
    CORRADE_COMPARE(out.str(),
        "GL::meshPrimitive(): invalid primitive MeshPrimitive(0x0)\n"
        "GL::meshPrimitive(): invalid primitive MeshPrimitive(0x12)\n");
}

void MeshTest::mapIndexType() {
    CORRADE_COMPARE(meshIndexType(Magnum::MeshIndexType::UnsignedByte), MeshIndexType::UnsignedByte);
    CORRADE_COMPARE(meshIndexType(Magnum::MeshIndexType::UnsignedShort), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE(meshIndexType(Magnum::MeshIndexType::UnsignedInt), MeshIndexType::UnsignedInt);

    /* Ensure all generic index types are handled. This goes through the first
       16 bits, which should be enough. Going through 32 bits takes 8 seconds,
       too much. */
    for(UnsignedInt i = 1; i <= 0xffff; ++i) {
        const auto type = Magnum::MeshIndexType(i);
        #ifdef __GNUC__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic error "-Wswitch"
        #endif
        switch(type) {
            #define _c(type) \
                case Magnum::MeshIndexType::type: \
                    CORRADE_VERIFY(UnsignedInt(meshIndexType(Magnum::MeshIndexType::type)) >= 0); \
                    break;
            #include "Magnum/Implementation/meshIndexTypeMapping.hpp"
            #undef _c
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #endif
    }
}

void MeshTest::mapIndexTypeInvalid() {
    std::ostringstream out;
    Error redirectError{&out};

    meshIndexType(Magnum::MeshIndexType(0x0));
    meshIndexType(Magnum::MeshIndexType(0x12));
    CORRADE_COMPARE(out.str(),
        "GL::meshIndexType(): invalid type MeshIndexType(0x0)\n"
        "GL::meshIndexType(): invalid type MeshIndexType(0x12)\n");
}

void MeshTest::debugPrimitive() {
    std::ostringstream o;
    Debug(&o) << MeshPrimitive::TriangleFan << MeshPrimitive(0xdead);
    CORRADE_COMPARE(o.str(), "GL::MeshPrimitive::TriangleFan GL::MeshPrimitive(0xdead)\n");
}

void MeshTest::debugIndexType() {
    std::ostringstream o;
    Debug(&o) << MeshIndexType::UnsignedShort << MeshIndexType(0xdead);
    CORRADE_COMPARE(o.str(), "GL::MeshIndexType::UnsignedShort GL::MeshIndexType(0xdead)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::MeshTest)
