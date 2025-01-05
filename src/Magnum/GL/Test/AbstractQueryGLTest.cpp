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

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/SampleQuery.h"

namespace Magnum { namespace GL { namespace Test { namespace {

struct AbstractQueryGLTest: OpenGLTester {
    explicit AbstractQueryGLTest();

    void construct();
    void constructMove();

    /* label() tested in subclasses because these all have to provide overloads
       to return correct type for method chaining and these overloads have to
       be deinlined to avoid including a StringView */
};

AbstractQueryGLTest::AbstractQueryGLTest() {
    addTests({&AbstractQueryGLTest::construct,
              &AbstractQueryGLTest::constructMove});
}

void AbstractQueryGLTest::construct() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::EXT::occlusion_query_boolean>())
        CORRADE_SKIP(Extensions::EXT::occlusion_query_boolean::string() << "is not supported.");
    #endif

    {
        #ifndef MAGNUM_TARGET_GLES
        const SampleQuery query{SampleQuery::Target::SamplesPassed};
        #else
        const SampleQuery query{SampleQuery::Target::AnySamplesPassed};
        #endif

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_VERIFY(query.id() > 0);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void AbstractQueryGLTest::constructMove() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::EXT::occlusion_query_boolean>())
        CORRADE_SKIP(Extensions::EXT::occlusion_query_boolean::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    SampleQuery a{SampleQuery::Target::SamplesPassed};
    #else
    SampleQuery a{SampleQuery::Target::AnySamplesPassed};
    #endif
    const Int id = a.id();

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(id > 0);

    SampleQuery b(Utility::move(a));

    CORRADE_COMPARE(a.id(), 0);
    CORRADE_COMPARE(b.id(), id);

    #ifndef MAGNUM_TARGET_GLES
    SampleQuery c{SampleQuery::Target::SamplesPassed};
    #else
    SampleQuery c{SampleQuery::Target::AnySamplesPassed};
    #endif
    const Int cId = c.id();
    c = Utility::move(b);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(cId > 0);
    CORRADE_COMPARE(b.id(), cId);
    CORRADE_COMPARE(c.id(), id);

    /* nothrow move constructibility tested in subclasses */
}

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::AbstractQueryGLTest)
