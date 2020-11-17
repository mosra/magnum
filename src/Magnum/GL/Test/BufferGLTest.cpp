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

#include <array>
#include <tuple>
#include <vector>
#include <Corrade/Containers/Array.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/OpenGLTester.h"

namespace Magnum { namespace GL { namespace Test { namespace {

struct BufferGLTest: OpenGLTester {
    explicit BufferGLTest();

    void construct();
    void constructFromData();
    void constructMove();
    void wrap();

    #ifndef MAGNUM_TARGET_WEBGL
    void label();
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    void bindBase();
    void bindRange();
    #endif

    #ifndef MAGNUM_TARGET_GLES
    void storage();
    #endif

    void data();
    #ifndef MAGNUM_TARGET_WEBGL
    void map();
    void mapRange();
    void mapRangeExplicitFlush();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void copy();
    #endif
    void invalidate();
};

BufferGLTest::BufferGLTest() {
    addTests({&BufferGLTest::construct,
              &BufferGLTest::constructFromData,
              &BufferGLTest::constructMove,
              &BufferGLTest::wrap,

              #ifndef MAGNUM_TARGET_WEBGL
              &BufferGLTest::label,
              #endif

              #ifndef MAGNUM_TARGET_GLES2
              &BufferGLTest::bindBase,
              &BufferGLTest::bindRange,
              #endif

              #ifndef MAGNUM_TARGET_GLES
              &BufferGLTest::storage,
              #endif

              &BufferGLTest::data,
              #ifndef MAGNUM_TARGET_WEBGL
              &BufferGLTest::map,
              &BufferGLTest::mapRange,
              &BufferGLTest::mapRangeExplicitFlush,
              #endif
              #ifndef MAGNUM_TARGET_GLES2
              &BufferGLTest::copy,
              #endif
              &BufferGLTest::invalidate});
}

void BufferGLTest::construct() {
    {
        Buffer buffer;

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_VERIFY(buffer.id() > 0);
        CORRADE_COMPARE(buffer.targetHint(), Buffer::TargetHint::Array);
        CORRADE_COMPARE(buffer.size(), 0);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void BufferGLTest::constructFromData() {
    constexpr Int data[] = {2, 7, 5, 13, 25};

    Buffer a{Buffer::TargetHint::ElementArray, data};
    Buffer b{Buffer::TargetHint::ElementArray, {2, 7, 5, 13, 25}};
    Buffer c{data};
    Buffer d{{nullptr, 5*4}}; /* This should work too for reserving memory */

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(a.size(), 5*4);
    CORRADE_COMPARE(b.size(), 5*4);
    CORRADE_COMPARE(c.size(), 5*4);
    CORRADE_COMPARE(d.size(), 5*4);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(Containers::arrayCast<Int>(a.data()),
        Containers::arrayView(data),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(Containers::arrayCast<Int>(b.data()),
        Containers::arrayView(data),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(Containers::arrayCast<Int>(c.data()),
        Containers::arrayView(data),
        TestSuite::Compare::Container);
    /* d's data is undefined, not testing */
    #endif
}

void BufferGLTest::constructMove() {
    Buffer a;
    const Int id = a.id();

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(id > 0);

    Buffer b(std::move(a));

    CORRADE_COMPARE(a.id(), 0);
    CORRADE_COMPARE(b.id(), id);

    Buffer c;
    const Int cId = c.id();
    c = std::move(b);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(cId > 0);
    CORRADE_COMPARE(b.id(), cId);
    CORRADE_COMPARE(c.id(), id);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<Buffer>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<Buffer>::value);
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

#ifndef MAGNUM_TARGET_WEBGL
void BufferGLTest::label() {
    /* No-Op version is tested in AbstractObjectGLTest */
    if(!Context::current().isExtensionSupported<Extensions::KHR::debug>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::debug_label>())
        CORRADE_SKIP("Required extension is not available");

    Buffer buffer;

    CORRADE_COMPARE(buffer.label(), "");
    MAGNUM_VERIFY_NO_GL_ERROR();

    buffer.setLabel("MyBuffer");
    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(buffer.label(), "MyBuffer");
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void BufferGLTest::bindBase() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(Extensions::ARB::uniform_buffer_object::string() + std::string{" is not supported."});
    #endif

    Buffer buffer;
    buffer.bind(Buffer::Target::Uniform, 15);

    MAGNUM_VERIFY_NO_GL_ERROR();

    Buffer::unbind(Buffer::Target::Uniform, 15);

    MAGNUM_VERIFY_NO_GL_ERROR();

    Buffer::bind(Buffer::Target::Uniform, 7, {&buffer, nullptr, &buffer});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Buffer::unbind(Buffer::Target::Uniform, 7, 3);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void BufferGLTest::bindRange() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(Extensions::ARB::uniform_buffer_object::string() + std::string{" is not supported."});
    #endif

    /* Check that we have correct offset alignment */
    CORRADE_INTERNAL_ASSERT(256 % Buffer::uniformOffsetAlignment() == 0);

    Buffer buffer;
    buffer.setData({nullptr, 1024}, BufferUsage::StaticDraw)
         .bind(Buffer::Target::Uniform, 15, 256, 13);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo C++14: get rid of std::make_tuple */
    Buffer::bind(Buffer::Target::Uniform, 7, {
        std::make_tuple(&buffer, 256, 13), {},
        std::make_tuple(&buffer, 768, 64)});

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES
void BufferGLTest::storage() {
    Buffer buffer;

    constexpr Int data[] = {2, 7, 5, 13, 25};

    buffer.setStorage(data, Buffer::StorageFlag::MapRead|Buffer::StorageFlag::ClientStorage);
    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_AS(Containers::arrayCast<Int>(buffer.data()),
        Containers::arrayView(data),
        TestSuite::Compare::Container);
}
#endif

void BufferGLTest::data() {
    Buffer buffer;

    /* Plain array */
    constexpr Int data[] = {2, 7, 5, 13, 25};
    buffer.setData(data, BufferUsage::StaticDraw);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(buffer.size(), 5*4);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_AS(Containers::arrayCast<Int>(buffer.data()),
        Containers::arrayView(data),
        TestSuite::Compare::Container);
    #endif

    /* STL initializer list */
    buffer.setData({2, 7, 5, 13, 25}, BufferUsage::StaticDraw);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(buffer.size(), 5*4);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_AS(Containers::arrayCast<Int>(buffer.data()),
        Containers::arrayView(data),
        TestSuite::Compare::Container);
    #endif

    /* Plain array */
    constexpr Int subData[] = {125, 3, 15};
    buffer.setSubData(4, subData);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(buffer.size(), 5*4);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_AS(Containers::arrayCast<Int>(buffer.subData(4, 3*4)),
        Containers::arrayView(subData),
        TestSuite::Compare::Container);
    #endif

    /* STL initializer list */
    buffer.setSubData(4, {125, 3, 15});
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(buffer.size(), 5*4);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_AS(Containers::arrayCast<Int>(buffer.subData(4, 3*4)),
        Containers::arrayView(subData),
        TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_WEBGL
void BufferGLTest::map() {
    #ifdef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::OES::mapbuffer>())
        CORRADE_SKIP(Extensions::OES::mapbuffer::string() + std::string(" is not supported"));
    #endif
    Buffer buffer;

    constexpr char data[] = {2, 7, 5, 13, 25};
    buffer.setData(data, BufferUsage::StaticDraw);

    char* contents = buffer.map(
        #ifndef MAGNUM_TARGET_GLES
        Buffer::MapAccess::ReadWrite
        #else
        Buffer::MapAccess::WriteOnly
        #endif
        );
    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_VERIFY(contents);
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(contents[2], 5);
    #endif
    contents[3] = 107;

    CORRADE_VERIFY(buffer.unmap());
    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    Containers::Array<char> changedContents = buffer.data();
    CORRADE_COMPARE(changedContents.size(), 5);
    CORRADE_COMPARE(changedContents[3], 107);
    #endif
}

void BufferGLTest::mapRange() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::map_buffer_range>())
        CORRADE_SKIP(Extensions::ARB::map_buffer_range::string() + std::string(" is not supported"));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::EXT::map_buffer_range>())
        CORRADE_SKIP(Extensions::EXT::map_buffer_range::string() + std::string(" is not supported"));
    #endif

    constexpr char data[] = {2, 7, 5, 13, 25};
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Containers::ArrayView<char> contents = buffer.map(1, 4, Buffer::MapFlag::Read|Buffer::MapFlag::Write);
    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_VERIFY(contents);
    CORRADE_COMPARE(contents.size(), 4);
    CORRADE_COMPARE(contents[2], 13);
    contents[3] = 107;

    CORRADE_VERIFY(buffer.unmap());
    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    Containers::Array<char> changedContents = buffer.data();
    CORRADE_COMPARE(changedContents.size(), 5);
    CORRADE_COMPARE(changedContents[4], 107);
    #endif
}

void BufferGLTest::mapRangeExplicitFlush() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::map_buffer_range>())
        CORRADE_SKIP(Extensions::ARB::map_buffer_range::string() + std::string(" is not supported"));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::EXT::map_buffer_range>())
        CORRADE_SKIP(Extensions::EXT::map_buffer_range::string() + std::string(" is not supported"));
    #endif

    constexpr char data[] = {2, 7, 5, 13, 25};
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    /* Map, set byte, don't flush and unmap */
    Containers::ArrayView<char> contents = buffer.map(1, 4, Buffer::MapFlag::Write|Buffer::MapFlag::FlushExplicit);
    CORRADE_VERIFY(contents);
    contents[2] = 99;
    CORRADE_VERIFY(buffer.unmap());
    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Unflushed range _might_ not be changed, thus nothing to test */

    /* Map, set byte, flush and unmap */
    contents = buffer.map(1, 4, Buffer::MapFlag::Write|Buffer::MapFlag::FlushExplicit);
    CORRADE_VERIFY(contents);
    contents[3] = 107;
    buffer.flushMappedRange(3, 1);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(buffer.unmap());
    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Flushed range should be changed */
    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    Containers::Array<char> changedContents = buffer.data();
    CORRADE_COMPARE(changedContents.size(), 5);
    CORRADE_COMPARE(changedContents[4], 107);
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void BufferGLTest::copy() {
    Buffer buffer1;
    constexpr char data[] = {2, 7, 5, 13, 25};
    buffer1.setData(data, BufferUsage::StaticCopy);

    Buffer buffer2;
    buffer2.setData({nullptr, 5}, BufferUsage::StaticRead);

    Buffer::copy(buffer1, buffer2, 1, 2, 3);
    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    const Containers::Array<char> subContents = buffer2.subData(2, 3);
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
    MAGNUM_VERIFY_NO_GL_ERROR();

    buffer.invalidateData();
    MAGNUM_VERIFY_NO_GL_ERROR();
}

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::BufferGLTest)
