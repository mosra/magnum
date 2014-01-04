/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Context.h"
#include "Extensions.h"
#include "Mesh.h"
#include "Test/AbstractOpenGLTester.h"

namespace Magnum { namespace Test {

class MeshGLTest: public AbstractOpenGLTester {
    public:
        explicit MeshGLTest();

        void construct();
        void constructCopy();
        void constructMove();

        void label();
};

MeshGLTest::MeshGLTest() {
    addTests({&MeshGLTest::construct,
              &MeshGLTest::constructCopy,
              &MeshGLTest::constructMove,

              &MeshGLTest::label});
}

void MeshGLTest::construct() {
    {
        const Mesh mesh;

        MAGNUM_VERIFY_NO_ERROR();

        #ifndef MAGNUM_TARGET_GLES
        if(Context::current()->isExtensionSupported<Extensions::GL::APPLE::vertex_array_object>())
        #elif defined(MAGNUM_TARGET_GLES2)
        if(Context::current()->isExtensionSupported<Extensions::GL::OES::vertex_array_object>())
        #endif
        {
            CORRADE_VERIFY(mesh.id() > 0);
        }
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void MeshGLTest::constructCopy() {
    #ifndef CORRADE_GCC44_COMPATIBILITY
    CORRADE_VERIFY(!(std::is_constructible<Mesh, const Mesh&>::value));
    /* GCC 4.6 doesn't have std::is_assignable */
    #ifndef CORRADE_GCC46_COMPATIBILITY
    CORRADE_VERIFY(!(std::is_assignable<Mesh, const Mesh&>{}));
    #endif
    #else
    CORRADE_SKIP("Type traits needed to test this are not available on GCC 4.4.");
    #endif
}

void MeshGLTest::constructMove() {
    Mesh a;
    const Int id = a.id();

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    if(Context::current()->isExtensionSupported<Extensions::GL::APPLE::vertex_array_object>())
    #elif defined(MAGNUM_TARGET_GLES2)
    if(Context::current()->isExtensionSupported<Extensions::GL::OES::vertex_array_object>())
    #endif
    {
        CORRADE_VERIFY(id > 0);
    }

    Mesh b(std::move(a));

    CORRADE_COMPARE(a.id(), 0);
    CORRADE_COMPARE(b.id(), id);

    Mesh c;
    const Int cId = c.id();
    c = std::move(b);

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    if(Context::current()->isExtensionSupported<Extensions::GL::APPLE::vertex_array_object>())
    #elif defined(MAGNUM_TARGET_GLES2)
    if(Context::current()->isExtensionSupported<Extensions::GL::OES::vertex_array_object>())
    #endif
    {
        CORRADE_VERIFY(cId > 0);
    }

    CORRADE_COMPARE(b.id(), cId);
    CORRADE_COMPARE(c.id(), id);
}

void MeshGLTest::label() {
    /* No-Op version is tested in AbstractObjectGLTest */
    if(!Context::current()->isExtensionSupported<Extensions::GL::KHR::debug>() &&
       !Context::current()->isExtensionSupported<Extensions::GL::EXT::debug_label>())
        CORRADE_SKIP("Required extension is not available");

    Mesh mesh;
    CORRADE_COMPARE(mesh.label(), "");

    mesh.setLabel("MyMesh");
    CORRADE_COMPARE(mesh.label(), "MyMesh");

    MAGNUM_VERIFY_NO_ERROR();
}

}}

CORRADE_TEST_MAIN(Magnum::Test::MeshGLTest)
