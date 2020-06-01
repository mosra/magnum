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

#include "Magnum/GL/Context.h"
#include "Magnum/GL/OpenGLTester.h"

#include "GlobalStateAcrossLibrariesLibrary.h"

namespace Magnum { namespace GL { namespace Test { namespace {

struct GlobalStateAcrossLibrariesGLTest: OpenGLTester {
    explicit GlobalStateAcrossLibrariesGLTest();

    void magnumContext();
    void functionPointers();
};

GlobalStateAcrossLibrariesGLTest::GlobalStateAcrossLibrariesGLTest() {
    addTests({&GlobalStateAcrossLibrariesGLTest::magnumContext,
              &GlobalStateAcrossLibrariesGLTest::functionPointers});
}

void GlobalStateAcrossLibrariesGLTest::magnumContext() {
    #if defined(MAGNUM_BUILD_STATIC_UNIQUE_GLOBALS) && !defined(MAGNUM_BUILD_STATIC)
    CORRADE_VERIFY(!"MAGNUM_BUILD_STATIC_UNIQUE_GLOBALS enabled but MAGNUM_BUILD_STATIC not");
    #endif

    CORRADE_VERIFY(GL::Context::hasCurrent());

    {
        #ifndef MAGNUM_BUILD_STATIC_UNIQUE_GLOBALS
        CORRADE_EXPECT_FAIL("MAGNUM_BUILD_STATIC_UNIQUE_GLOBALS not enabled.");
        #endif
        CORRADE_COMPARE(currentContextInALibrary(), &GL::Context::current());
    }
}

void GlobalStateAcrossLibrariesGLTest::functionPointers() {
    #if defined(MAGNUM_BUILD_STATIC_UNIQUE_GLOBALS) && !defined(MAGNUM_BUILD_STATIC)
    CORRADE_VERIFY(!"MAGNUM_BUILD_STATIC_UNIQUE_GLOBALS enabled but MAGNUM_BUILD_STATIC not");
    #endif

    CORRADE_VERIFY(glCreateProgram);

    {
        #ifndef MAGNUM_BUILD_STATIC_UNIQUE_GLOBALS
        CORRADE_EXPECT_FAIL("MAGNUM_BUILD_STATIC_UNIQUE_GLOBALS not enabled.");
        #endif
        /* Annotating the flextGL global with __attribute__((weak)) makes
           static builds crash on startup due to a null function pointer call.
           This is because even the GL 1.0 / 1.1 function pointers are accessed
           through this struct and somehow the weak symbol makes the struct all
           nulls. Not sure how to proceed. */
        CORRADE_EXPECT_FAIL("Deduplication of global GL function pointers across shared libraries isn't implemented yet.");
        CORRADE_COMPARE(createProgramInALibrary(), reinterpret_cast<void*>(glCreateProgram));
    }
}

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::GlobalStateAcrossLibrariesGLTest)

