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

#include <array>
#include <vector>
#include <Corrade/Containers/Array.h>
#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/Buffer.h"
#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/OpenGLTester.h"

namespace Magnum { namespace Test {

struct BufferGLTest: OpenGLTester {
    explicit BufferGLTest();

    void construct();
    void constructCopy();
    void constructMove();
    void wrap();

    void label();

    #ifndef MAGNUM_TARGET_GLES2
    void bindBase();
    void bindRange();
    #endif

    void data();
    void map();
    #ifdef CORRADE_TARGET_NACL
    void mapSub();
    #endif
    void mapRange();
    void mapRangeExplicitFlush();
    #ifndef MAGNUM_TARGET_GLES2
    void copy();
    #endif
    void invalidate();
};

BufferGLTest::BufferGLTest() {
    addTests({&BufferGLTest::construct,
              &BufferGLTest::constructCopy,
              &BufferGLTest::constructMove,
              &BufferGLTest::wrap,

              &BufferGLTest::label,

              #ifndef MAGNUM_TARGET_GLES2
              &BufferGLTest::bindBase,
              &BufferGLTest::bindRange,
              #endif

              &BufferGLTest::data,
              &BufferGLTest::map,
              #ifdef CORRADE_TARGET_NACL
              &BufferGLTest::mapSub,
              #endif
              &BufferGLTest::mapRange,
              &BufferGLTest::mapRangeExplicitFlush,
              #ifndef MAGNUM_TARGET_GLES2
              &BufferGLTest::copy,
              #endif
              &BufferGLTest::invalidate});
}

void BufferGLTest::construct() {
    {
        Buffer buffer;

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(buffer.id() > 0);
        CORRADE_COMPARE(buffer.targetHint(), Buffer::TargetHint::Array);
        CORRADE_COMPARE(buffer.size(), 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void BufferGLTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<Buffer, const Buffer&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Buffer, const Buffer&>{}));
}

void BufferGLTest::constructMove() {
    Buffer a;
    const Int id = a.id();

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(id > 0);

    Buffer b(std::move(a));

    CORRADE_COMPARE(a.id(), 0);
    CORRADE_COMPARE(b.id(), id);

    Buffer c;
    const Int cId = c.id();
    c = std::move(b);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(cId > 0);
    CORRADE_COMPARE(b.id(), cId);
    CORRADE_COMPARE(c.id(), id);
}

void BufferGLTest::wrap() {
    GLuint id;
    glGenBuffers(1, &id);

    /* Releasing won't delete anything */
    {
        auto buffer = Buffer::wrap(id, ObjectFlag::DeleteOnDestruction);
        CORRADE_COMPARE(buffer.release(), id);
    }

    /* ...so we can wrap it again */
    Buffer::wrap(id);
    glDeleteBuffers(1, &id);
}

void BufferGLTest::label() {
    /* No-Op version is tested in AbstractObjectGLTest */
    if(!Context::current().isExtensionSupported<Extensions::GL::KHR::debug>() &&
       !Context::current().isExtensionSupported<Extensions::GL::EXT::debug_label>())
        CORRADE_SKIP("Required extension is not available");

    Buffer buffer;

    CORRADE_COMPARE(buffer.label(), "");
    MAGNUM_VERIFY_NO_ERROR();

    buffer.setLabel("MyBuffer");
    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(buffer.label(), "MyBuffer");
}

#ifndef MAGNUM_TARGET_GLES2
void BufferGLTest::bindBase() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::uniform_buffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::uniform_buffer_object::string() + std::string{" is not supported."});
    #endif

    Buffer buffer;
    buffer.bind(Buffer::Target::Uniform, 15);

    MAGNUM_VERIFY_NO_ERROR();

    Buffer::unbind(Buffer::Target::Uniform, 15);

    MAGNUM_VERIFY_NO_ERROR();

    Buffer::bind(Buffer::Target::Uniform, 7, {&buffer, nullptr, &buffer});

    MAGNUM_VERIFY_NO_ERROR();

    Buffer::unbind(Buffer::Target::Uniform, 7, 3);

    MAGNUM_VERIFY_NO_ERROR();
}

void BufferGLTest::bindRange() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::uniform_buffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::uniform_buffer_object::string() + std::string{" is not supported."});
    #endif

    /* Check that we have correct offset alignment */
    CORRADE_INTERNAL_ASSERT(256 % Buffer::uniformOffsetAlignment() == 0);

    Buffer buffer;
    buffer.setData({nullptr, 1024}, BufferUsage::StaticDraw)
         .bind(Buffer::Target::Uniform, 15, 256, 13);

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo C++14: get rid of std::make_tuple */
    Buffer::bind(Buffer::Target::Uniform, 7, {
        std::make_tuple(&buffer, 256, 13), {},
        std::make_tuple(&buffer, 768, 64)});

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void BufferGLTest::data() {
    Buffer buffer;

    /* Plain array */
    constexpr Int data[] = {2, 7, 5, 13, 25};
    buffer.setData({data, 5}, BufferUsage::StaticDraw);
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(buffer.size(), 5*4);

    /* STL vector */
    std::vector<Int> data2{2, 7, 5, 13, 25};
    buffer.setData(data2, BufferUsage::StaticDraw);
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(buffer.size(), 5*4);

    /* STL array */
    std::array<Int, 5> data3{{2, 7, 5, 13, 25}};
    buffer.setData(data3, BufferUsage::StaticDraw);
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(buffer.size(), 5*4);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    const Containers::Array<Int> contents = buffer.data<Int>();
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE_AS(contents, Containers::arrayView(data),
        TestSuite::Compare::Container);
    #endif

    /* Plain array */
    constexpr Int subData[] = {125, 3, 15};
    buffer.setSubData(4, {subData, 3});
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(buffer.size(), 5*4);

    /* STL vector */
    std::vector<Int> subData2{125, 3, 15};
    buffer.setSubData(4, subData2);
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(buffer.size(), 5*4);

    /* STL array */
    std::array<Int, 3> subData3{{125, 3, 15}};
    buffer.setSubData(4, subData3);
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(buffer.size(), 5*4);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    const Containers::Array<Int> subContents = buffer.subData<Int>(4, 3);
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE_AS(subContents, Containers::arrayView(subData),
        TestSuite::Compare::Container);
    #endif
}

void BufferGLTest::map() {
    #ifdef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::OES::mapbuffer>())
        CORRADE_SKIP(Extensions::GL::OES::mapbuffer::string() + std::string(" is not supported"));
    #endif
    Buffer buffer;

    constexpr char data[] = {2, 7, 5, 13, 25};
    buffer.setData(data, BufferUsage::StaticDraw);

    #ifndef MAGNUM_TARGET_GLES
    char* contents = buffer.map<char>(Buffer::MapAccess::ReadWrite);
    #else
    char* contents = buffer.map<char>(Buffer::MapAccess::WriteOnly);
    #endif
    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_VERIFY(contents);
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(contents[2], 5);
    #endif
    contents[3] = 107;

    CORRADE_VERIFY(buffer.unmap());
    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    Containers::Array<char> changedContents = buffer.data<char>();
    CORRADE_COMPARE(changedContents.size(), 5);
    CORRADE_COMPARE(changedContents[3], 107);
    #endif
}

#ifdef CORRADE_TARGET_NACL
void BufferGLTest::mapSub() {
    if(!Context::current().isExtensionSupported<Extensions::GL::CHROMIUM::map_sub>())
        CORRADE_SKIP(Extensions::GL::CHROMIUM::map_sub::string() + std::string(" is not supported"));

    Buffer buffer;

    constexpr char data[] = {2, 7, 5, 13, 25};
    buffer.setData(data, BufferUsage::StaticDraw);

    char* contents = buffer.mapSub<char>(1, 4, Buffer::MapAccess::WriteOnly);
    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_VERIFY(contents);
    contents[3] = 107;

    buffer.unmapSub();
    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to verify the contents in ES? */
}
#endif

void BufferGLTest::mapRange() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::map_buffer_range>())
        CORRADE_SKIP(Extensions::GL::ARB::map_buffer_range::string() + std::string(" is not supported"));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::map_buffer_range>())
        CORRADE_SKIP(Extensions::GL::EXT::map_buffer_range::string() + std::string(" is not supported"));
    #endif

    constexpr char data[] = {2, 7, 5, 13, 25};
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    char* contents = buffer.map<char>(1, 4, Buffer::MapFlag::Read|Buffer::MapFlag::Write);
    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_VERIFY(contents);
    CORRADE_COMPARE(contents[2], 13);
    contents[3] = 107;

    CORRADE_VERIFY(buffer.unmap());
    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    Containers::Array<char> changedContents = buffer.data<char>();
    CORRADE_COMPARE(changedContents.size(), 5);
    CORRADE_COMPARE(changedContents[4], 107);
    #endif
}

void BufferGLTest::mapRangeExplicitFlush() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::map_buffer_range>())
        CORRADE_SKIP(Extensions::GL::ARB::map_buffer_range::string() + std::string(" is not supported"));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::map_buffer_range>())
        CORRADE_SKIP(Extensions::GL::EXT::map_buffer_range::string() + std::string(" is not supported"));
    #endif

    constexpr char data[] = {2, 7, 5, 13, 25};
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    /* Map, set byte, don't flush and unmap */
    char* contents = buffer.map<char>(1, 4, Buffer::MapFlag::Write|Buffer::MapFlag::FlushExplicit);
    CORRADE_VERIFY(contents);
    contents[2] = 99;
    CORRADE_VERIFY(buffer.unmap());
    MAGNUM_VERIFY_NO_ERROR();

    /* Unflushed range _might_ not be changed, thus nothing to test */

    /* Map, set byte, flush and unmap */
    contents = buffer.map<char>(1, 4, Buffer::MapFlag::Write|Buffer::MapFlag::FlushExplicit);
    CORRADE_VERIFY(contents);
    contents[3] = 107;
    buffer.flushMappedRange(3, 1);
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(buffer.unmap());
    MAGNUM_VERIFY_NO_ERROR();

    /* Flushed range should be changed */
    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    Containers::Array<char> changedContents = buffer.data<char>();
    CORRADE_COMPARE(changedContents.size(), 5);
    CORRADE_COMPARE(changedContents[4], 107);
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void BufferGLTest::copy() {
    Buffer buffer1;
    constexpr char data[] = {2, 7, 5, 13, 25};
    buffer1.setData(data, BufferUsage::StaticCopy);

    Buffer buffer2;
    buffer2.setData({nullptr, 5}, BufferUsage::StaticRead);

    Buffer::copy(buffer1, buffer2, 1, 2, 3);
    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    const Containers::Array<char> subContents = buffer2.subData<char>(2, 3);
    CORRADE_COMPARE_AS(subContents, Containers::arrayView(data).slice(1, 4),
        TestSuite::Compare::Container);
    #endif
}
#endif

void BufferGLTest::invalidate() {
    Buffer buffer;
    constexpr char data[] = {2, 7, 5, 13, 25};
    buffer.setData(data, BufferUsage::StaticDraw);

    /* Just test that no errors are emitted */

    buffer.invalidateSubData(3, 2);
    MAGNUM_VERIFY_NO_ERROR();

    buffer.invalidateData();
    MAGNUM_VERIFY_NO_ERROR();
}

}}

CORRADE_TEST_MAIN(Magnum::Test::BufferGLTest)
