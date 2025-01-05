/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2022 Pablo Escobar <mail@rvrs.in>

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

#include <Corrade/Containers/Array.h>
#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/DebugTools/BufferData.h"
#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/OpenGLTester.h"

namespace Magnum { namespace DebugTools { namespace Test { namespace {

struct BufferDataGLTest: GL::OpenGLTester {
    explicit BufferDataGLTest();

    /* To avoid complex ifdefing on Esmcripten, assumes that tests are built
       on 2.0.17+. On older versions the linker will fail due to missing
       glGetBufferSubData(). */

    void data();
    void subData();
};

BufferDataGLTest::BufferDataGLTest() {
    addTests({&BufferDataGLTest::data,
              &BufferDataGLTest::subData});
}

constexpr Int Data[] = {2, 7, 5, 13, 25};

void BufferDataGLTest::data() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::map_buffer_range>())
        CORRADE_SKIP(GL::Extensions::EXT::map_buffer_range::string() << "is not supported.");
    #endif

    GL::Buffer buffer;

    Containers::Array<char> emptyContents = bufferData(buffer);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(emptyContents.isEmpty());

    buffer.setData(Data, GL::BufferUsage::StaticDraw);
    Containers::Array<char> contents = bufferData(buffer);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_AS(
        Containers::arrayCast<Int>(contents),
        Containers::arrayView(Data),
        TestSuite::Compare::Container);
}

void BufferDataGLTest::subData() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::map_buffer_range>())
        CORRADE_SKIP(GL::Extensions::EXT::map_buffer_range::string() << "is not supported.");
    #endif

    GL::Buffer buffer;
    buffer.setData(Data, GL::BufferUsage::StaticDraw);
    Containers::Array<char> contents = bufferSubData(buffer, 4, 12);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_AS(
        Containers::arrayCast<Int>(contents),
        Containers::arrayView(Data).slice(1, 4),
        TestSuite::Compare::Container);

    Containers::Array<char> emptyContents = bufferSubData(buffer, 4, 0);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(emptyContents.isEmpty());
}

}}}}

CORRADE_TEST_MAIN(Magnum::DebugTools::Test::BufferDataGLTest)
