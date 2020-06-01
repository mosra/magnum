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

#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/DebugTools/BufferData.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/OpenGLTester.h"

namespace Magnum { namespace DebugTools { namespace Test { namespace {

struct BufferDataGLTest: GL::OpenGLTester {
    explicit BufferDataGLTest();

    void data();
    void subData();
};

BufferDataGLTest::BufferDataGLTest() {
    addTests({&BufferDataGLTest::data,
              &BufferDataGLTest::subData});
}

constexpr Int Data[] = {2, 7, 5, 13, 25};

void BufferDataGLTest::data() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::map_buffer_range>())
        CORRADE_SKIP(GL::Extensions::ARB::map_buffer_range::string() + std::string(" is not supported"));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::map_buffer_range>())
        CORRADE_SKIP(GL::Extensions::EXT::map_buffer_range::string() + std::string(" is not supported"));
    #endif

    GL::Buffer buffer;
    buffer.setData(Data, GL::BufferUsage::StaticDraw);
    const Containers::Array<Int> contents = bufferData<Int>(buffer);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_AS(contents, Containers::arrayView(Data),
        TestSuite::Compare::Container);
}

void BufferDataGLTest::subData() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::map_buffer_range>())
        CORRADE_SKIP(GL::Extensions::ARB::map_buffer_range::string() + std::string(" is not supported"));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::map_buffer_range>())
        CORRADE_SKIP(GL::Extensions::EXT::map_buffer_range::string() + std::string(" is not supported"));
    #endif

    GL::Buffer buffer;
    buffer.setData(Data, GL::BufferUsage::StaticDraw);
    const Containers::Array<Int> contents = bufferSubData<Int>(buffer, 4, 3);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_AS(contents, Containers::arrayView(Data).slice(1, 4),
        TestSuite::Compare::Container);
}

}}}}

CORRADE_TEST_MAIN(Magnum::DebugTools::Test::BufferDataGLTest)
