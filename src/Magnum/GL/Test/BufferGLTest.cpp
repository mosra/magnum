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

#include <array>
#include <vector>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Triple.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/Numeric.h>

#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/OpenGLTester.h"

#ifndef MAGNUM_TARGET_WEBGL
#include <Corrade/Containers/String.h>
#endif

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
    void bindBaseRangeUpdateRegularBinding();
    #ifndef MAGNUM_TARGET_WEBGL
    void bindBaseRangeCreatesObject();
    #endif
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void storage();
    void storagePreinitialized();
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

#ifndef MAGNUM_TARGET_GLES2
const struct {
    const char* name;
    bool bindRange;
    bool multi;
} BindBaseRangeUpdateRegularBindingData[]{
    {"bind base", false, false},
    {"bind bases", false, true},
    {"bind range", true, false},
    {"bind ranges", true, true},
};

#ifndef MAGNUM_TARGET_WEBGL
const struct {
    const char* name;
    bool multi;
} BindBaseRangeCreatesObjectData[]{
    {"bind base", false},
    {"bind bases", true},
};
#endif
#endif

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
              });

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests({&BufferGLTest::bindBaseRangeUpdateRegularBinding},
        Containers::arraySize(BindBaseRangeUpdateRegularBindingData));

    #ifndef MAGNUM_TARGET_WEBGL
    addInstancedTests({&BufferGLTest::bindBaseRangeCreatesObject},
        Containers::arraySize(BindBaseRangeCreatesObjectData));
    #endif
    #endif

    addTests({
              #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
              &BufferGLTest::storage,
              &BufferGLTest::storagePreinitialized,
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

#ifndef MAGNUM_TARGET_WEBGL
using namespace Containers::Literals;
#endif

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

    Buffer b(Utility::move(a));

    CORRADE_COMPARE(a.id(), 0);
    CORRADE_COMPARE(b.id(), id);

    Buffer c;
    const Int cId = c.id();
    c = Utility::move(b);

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

    /* Test the string size gets correctly used, instead of relying on null
       termination */
    buffer.setLabel("MyBuffer!"_s.exceptSuffix(1));
    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(buffer.label(), "MyBuffer");
    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void BufferGLTest::bindBase() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
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
        CORRADE_SKIP(Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    /* Check that we have correct offset alignment */
    CORRADE_COMPARE_AS(256, Buffer::uniformOffsetAlignment(),
        TestSuite::Compare::Divisible);

    Buffer buffer;
    buffer.setData({nullptr, 1024}, BufferUsage::StaticDraw)
         .bind(Buffer::Target::Uniform, 15, 512, 256);

    MAGNUM_VERIFY_NO_GL_ERROR();

    Buffer::bind(Buffer::Target::Uniform, 7, {
        {&buffer, 256, 512}, {},
        {&buffer, 768, 256}});

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void BufferGLTest::bindBaseRangeUpdateRegularBinding() {
    auto&& data = BindBaseRangeUpdateRegularBindingData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    if(Context::current().isExtensionSupported<Extensions::ARB::direct_state_access>())
        CORRADE_SKIP(Extensions::ARB::direct_state_access::string() << "is supported, can't test.");
    #endif

    /* glBindBufferRange() / glBindBufferBase() binds the buffer to the regular
       binding point as a side effect. Verify that the state tracker accounts
       for that when uploading data to another (larger) buffer via classic
       glBindBuffer() + glBufferSubData() -- if it wouldn't, the data upload
       would fail due to the range being too large.

       In comparison, the multi-bind APIs don't have this side effect. GL is
       "fun". */

    Buffer small{Buffer::TargetHint::Uniform};
    small.setData({nullptr, 16});

    Buffer large{Buffer::TargetHint::Uniform};
    /* Without DSA, this makes the current Uniform buffer binding set to
       `large`. */
    large.setData({nullptr, 128});

    /* And this makes the current Uniform buffer binding set to `small` again,
       but only as a side effect. Testing also the multi variants, they
       shouldn't do that though. */
    if(data.multi) {
        if(data.bindRange)
            Buffer::bind(Buffer::Target::Uniform, 0, {{&small, 0, 16}});
        else
            Buffer::bind(Buffer::Target::Uniform, 0, {&small});
    } else {
        if(data.bindRange)
            small.bind(Buffer::Target::Uniform, 0, 0, 16);
        else
            small.bind(Buffer::Target::Uniform, 0);
    }

    /* So this has to explicitly rebind `large` again as the binding was
       overwritten by the above, even though glBindBuffer() wasn't directly
       called */
    char zeros[128]{};
    large.setSubData(0, zeros);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Conversely, unbinding the indexed target then resets the regular binding
       point as a side effect. Again verify that the state tracker accounts for
       that by trying to upload data to the same buffer again -- it should
       rebind it instead of assuming it's still there. */
    if(data.multi)
        Buffer::unbind(Buffer::Target::Uniform, 0, 1);
    else
        Buffer::unbind(Buffer::Target::Uniform, 0);

    large.setSubData(0, zeros);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

#ifndef MAGNUM_TARGET_WEBGL
void BufferGLTest::bindBaseRangeCreatesObject() {
    auto&& data = BindBaseRangeCreatesObjectData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    if(Context::current().isExtensionSupported<Extensions::ARB::direct_state_access>())
        CORRADE_SKIP(Extensions::ARB::direct_state_access::string() << "is supported, can't test.");
    #endif
    if(!Context::current().isExtensionSupported<Extensions::KHR::debug>())
        CORRADE_SKIP(Extensions::KHR::debug::string() << "is not supported.");

    Buffer buffer;

    /* The glGenBuffers() API doesn't actually create a buffer object, creation
       only happens on the first glBindBuffer(). The DSA glCreateBuffer() API
       combines the two, and then some DSA APIs that take just an object ID
       such as glObjectLabel() require the object to be created.

       As the glBindBufferBase() / glBindBufferRange() binds the buffer to the
       regular binding point as a side effect, the implementation assumes it
       also performs the creation, and so sets the ObjectFlag::Created flag. To
       verify that, the glObjectLabel() call should then work without a GL
       error.

       On the other hand, the multi-bind APIs *don't* bind the buffer to the
       regular binding point, but conversely require the objects to be created.
       So for these, the multi-bind is actually internally preceded by an
       explicit glBindBuffer() that creates the buffer if not already. Calling
       the multi-bind variant here just to be sure it all works as intended.

       Also, only the "base" binding APIs are tested here, the range APIs fail
       on an error because size of 0 is not an allowed value. The
       implementation and ObjectFlag::Created flag setting however behaves the
       same for both for consistency, even though it's impossible to test. */

    if(data.multi)
        Buffer::bind(Buffer::Target::Uniform, 0, {&buffer});
    else
        buffer.bind(Buffer::Target::Uniform, 0);

    MAGNUM_VERIFY_NO_GL_ERROR();

    buffer.setLabel("hello");

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(buffer.label(), "hello");
}
#endif
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void BufferGLTest::storage() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::buffer_storage>())
        CORRADE_SKIP(Extensions::ARB::buffer_storage::string() << "is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::buffer_storage>())
        CORRADE_SKIP(Extensions::EXT::buffer_storage::string() << "is not supported.");
    #endif

    Buffer buffer;

    constexpr Int data[] = {2, 7, 5, 13, 25};

    buffer.setStorage(sizeof(data), Buffer::StorageFlag::DynamicStorage)
        .setSubData(0, data);
    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(Containers::arrayCast<Int>(buffer.data()),
        Containers::arrayView(data),
        TestSuite::Compare::Container);
    #endif
}

void BufferGLTest::storagePreinitialized() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::buffer_storage>())
        CORRADE_SKIP(Extensions::ARB::buffer_storage::string() << "is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::buffer_storage>())
        CORRADE_SKIP(Extensions::EXT::buffer_storage::string() << "is not supported.");
    #endif

    Buffer buffer;

    constexpr Int data[] = {2, 7, 5, 13, 25};

    buffer.setStorage(data, Buffer::StorageFlag::MapRead|Buffer::StorageFlag::ClientStorage);
    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(Containers::arrayCast<Int>(buffer.data()),
        Containers::arrayView(data),
        TestSuite::Compare::Container);
    #endif
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
        CORRADE_SKIP(Extensions::OES::mapbuffer::string() << "is not supported.");
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
        CORRADE_SKIP(Extensions::ARB::map_buffer_range::string() << "is not supported.");
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::EXT::map_buffer_range>())
        CORRADE_SKIP(Extensions::EXT::map_buffer_range::string() << "is not supported.");
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
        CORRADE_SKIP(Extensions::ARB::map_buffer_range::string() << "is not supported.");
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::EXT::map_buffer_range>())
        CORRADE_SKIP(Extensions::EXT::map_buffer_range::string() << "is not supported.");
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
