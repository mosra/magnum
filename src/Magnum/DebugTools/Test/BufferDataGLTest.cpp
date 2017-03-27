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

#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/OpenGLTester.h"
#include "Magnum/DebugTools/BufferData.h"

namespace Magnum { namespace DebugTools { namespace Test {

struct BufferDataGLTest: Magnum::OpenGLTester {
    explicit BufferDataGLTest();

    void data();
    void subData();
};

BufferDataGLTest::BufferDataGLTest() {
    addTests({&BufferDataGLTest::data,
              &BufferDataGLTest::subData});
}

namespace {
    constexpr Int Data[] = {2, 7, 5, 13, 25};
}

void BufferDataGLTest::data() {
    Buffer buffer;
    buffer.setData(Data, BufferUsage::StaticDraw);
    const Containers::Array<Int> contents = bufferData<Int>(buffer);
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE_AS(contents, Containers::arrayView(Data),
        TestSuite::Compare::Container);
}

void BufferDataGLTest::subData() {
    Buffer buffer;
    buffer.setData(Data, BufferUsage::StaticDraw);
    const Containers::Array<Int> contents = bufferSubData<Int>(buffer, 4, 3);
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE_AS(contents, Containers::arrayView(Data).slice(1, 4),
        TestSuite::Compare::Container);
}

}}}

CORRADE_TEST_MAIN(Magnum::DebugTools::Test::BufferDataGLTest)
