/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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
#include <Corrade/Utility/Configuration.h>

#include "Magnum/Mesh.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/MeshView.h"

namespace Magnum { namespace GL { namespace Test {

/* Tests MeshView as well */

struct MeshTest: TestSuite::Tester {
    explicit MeshTest();

    void constructNoCreate();
    void constructViewNoCreate();

    void drawCountNotSet();
    void drawViewCountNotSet();

    #ifdef MAGNUM_BUILD_DEPRECATED
    void indexSizeDeprecated();
    #endif

    void mapPrimitive();
    #if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL) && !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void mapPrimitiveDeprecated();
    #endif
    void mapPrimitiveInvalid();
    void mapIndexType();
    void mapIndexTypeInvalid();

    void debugPrimitive();
    void debugIndexType();
};

MeshTest::MeshTest() {
    addTests({&MeshTest::constructNoCreate,
              &MeshTest::constructViewNoCreate,

              &MeshTest::drawCountNotSet,
              &MeshTest::drawViewCountNotSet,

              #ifdef MAGNUM_BUILD_DEPRECATED
              &MeshTest::indexSizeDeprecated,
              #endif

              &MeshTest::mapPrimitive,
              #if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL) && !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
              &MeshTest::mapPrimitiveDeprecated,
              #endif
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

namespace {
    struct Shader: AbstractShaderProgram {
        explicit Shader(NoCreateT): AbstractShaderProgram{NoCreate} {}
    };
}

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

#ifdef MAGNUM_BUILD_DEPRECATED
void MeshTest::indexSizeDeprecated() {
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(Mesh::indexSize(Mesh::IndexType::UnsignedByte), 1);
    CORRADE_COMPARE(Mesh::indexSize(Mesh::IndexType::UnsignedShort), 2);
    CORRADE_COMPARE(Mesh::indexSize(Mesh::IndexType::UnsignedInt), 4);
    CORRADE_IGNORE_DEPRECATED_POP
}
#endif

void MeshTest::mapPrimitive() {
    CORRADE_COMPARE(meshPrimitive(Magnum::MeshPrimitive::Points), MeshPrimitive::Points);
    CORRADE_COMPARE(meshPrimitive(Magnum::MeshPrimitive::Lines), MeshPrimitive::Lines);
    CORRADE_COMPARE(meshPrimitive(Magnum::MeshPrimitive::LineLoop), MeshPrimitive::LineLoop);
    CORRADE_COMPARE(meshPrimitive(Magnum::MeshPrimitive::LineStrip), MeshPrimitive::LineStrip);
    CORRADE_COMPARE(meshPrimitive(Magnum::MeshPrimitive::Triangles), MeshPrimitive::Triangles);
    CORRADE_COMPARE(meshPrimitive(Magnum::MeshPrimitive::TriangleStrip), MeshPrimitive::TriangleStrip);
    CORRADE_COMPARE(meshPrimitive(Magnum::MeshPrimitive::TriangleFan), MeshPrimitive::TriangleFan);
}

#if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL) && !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void MeshTest::mapPrimitiveDeprecated() {
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(meshPrimitive(Magnum::MeshPrimitive::TriangleStripAdjacency),
        MeshPrimitive::TriangleStripAdjacency);
    CORRADE_IGNORE_DEPRECATED_POP
}
#endif

void MeshTest::mapPrimitiveInvalid() {
    std::ostringstream out;
    Error redirectError{&out};

    meshPrimitive(Magnum::MeshPrimitive(0x123));
    CORRADE_COMPARE(out.str(),
        "GL::meshPrimitive(): invalid primitive MeshPrimitive(0x123)\n");
}

void MeshTest::mapIndexType() {
    CORRADE_COMPARE(meshIndexType(Magnum::MeshIndexType::UnsignedByte), MeshIndexType::UnsignedByte);
    CORRADE_COMPARE(meshIndexType(Magnum::MeshIndexType::UnsignedShort), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE(meshIndexType(Magnum::MeshIndexType::UnsignedInt), MeshIndexType::UnsignedInt);
}

void MeshTest::mapIndexTypeInvalid() {
    std::ostringstream out;
    Error redirectError{&out};

    meshIndexType(Magnum::MeshIndexType(0x123));
    CORRADE_COMPARE(out.str(),
        "GL::meshIndexType(): invalid type MeshIndexType(0x123)\n");
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

}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::MeshTest)
