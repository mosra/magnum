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
#include "Query.h"
#include "Test/AbstractOpenGLTester.h"

namespace Magnum { namespace Test {

class AbstractQueryGLTest: public AbstractOpenGLTester {
    public:
        explicit AbstractQueryGLTest();

        void construct();
        void constructCopy();
        void constructMove();

        void label();
};

AbstractQueryGLTest::AbstractQueryGLTest() {
    addTests({&AbstractQueryGLTest::construct,
              &AbstractQueryGLTest::constructCopy,
              &AbstractQueryGLTest::constructMove,

              &AbstractQueryGLTest::label});
}

void AbstractQueryGLTest::construct() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::occlusion_query_boolean>())
        CORRADE_SKIP(Extensions::GL::EXT::occlusion_query_boolean::string() + std::string(" is not supported."));
    #endif

    {
        const SampleQuery query;

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(query.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void AbstractQueryGLTest::constructCopy() {
     CORRADE_VERIFY(!(std::is_constructible<SampleQuery, const SampleQuery&>{}));
     CORRADE_VERIFY(!(std::is_assignable<SampleQuery, const SampleQuery&>{}));
}

void AbstractQueryGLTest::constructMove() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::occlusion_query_boolean>())
        CORRADE_SKIP(Extensions::GL::EXT::occlusion_query_boolean::string() + std::string(" is not supported."));
    #endif

    SampleQuery a;
    const Int id = a.id();

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(id > 0);

    SampleQuery b(std::move(a));

    CORRADE_COMPARE(a.id(), 0);
    CORRADE_COMPARE(b.id(), id);

    SampleQuery c;
    const Int cId = c.id();
    c = std::move(b);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(cId > 0);
    CORRADE_COMPARE(b.id(), cId);
    CORRADE_COMPARE(c.id(), id);
}

void AbstractQueryGLTest::label() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::occlusion_query_boolean>())
        CORRADE_SKIP(Extensions::GL::EXT::occlusion_query_boolean::string() + std::string(" is not supported."));
    #endif

    /* No-Op version is tested in AbstractObjectGLTest */
    if(!Context::current()->isExtensionSupported<Extensions::GL::KHR::debug>() &&
       !Context::current()->isExtensionSupported<Extensions::GL::EXT::debug_label>())
        CORRADE_SKIP("Required extension is not available");

    SampleQuery query;
    CORRADE_COMPARE(query.label(), "");

    query.setLabel("MyQuery");
    CORRADE_COMPARE(query.label(), "MyQuery");

    MAGNUM_VERIFY_NO_ERROR();
}

}}

CORRADE_TEST_MAIN(Magnum::Test::AbstractQueryGLTest)
