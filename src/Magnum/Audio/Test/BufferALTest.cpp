/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2019 Guillaume Jacquemin <williamjcm@users.noreply.github.com>

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
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Audio/Buffer.h"
#include "Magnum/Audio/BufferFormat.h"
#include "Magnum/Audio/Context.h"
#include "Magnum/Audio/Extensions.h"

namespace Magnum { namespace Audio { namespace Test { namespace {

struct BufferALTest: TestSuite::Tester {
    explicit BufferALTest();

    void construct();

    void setData();
    void setLoopPoints();
    void setLoopSince();
    void setLoopUntil();
    void resetLoopPoints();

    Context _context;
};

BufferALTest::BufferALTest() {
    addTests({&BufferALTest::construct,

              &BufferALTest::setData,
              &BufferALTest::setLoopPoints,
              &BufferALTest::setLoopSince,
              &BufferALTest::setLoopUntil,
              &BufferALTest::resetLoopPoints});
}

void BufferALTest::construct() {
    Buffer buf;
    CORRADE_VERIFY(buf.id() != 0);
}

void BufferALTest::setData() {
    Buffer buf;
    constexpr char data[] { 25, 17, 24, 122, 67, 24, 48, 96 };

    buf.setData(BufferFormat::Mono8, data, 22050);

    ALint bufSize;
    alGetBufferi(buf.id(), AL_SIZE, &bufSize);
    CORRADE_VERIFY(bufSize == 8);
}

void BufferALTest::setLoopPoints() {
    if(!_context.isExtensionSupported<Audio::Extensions::AL::SOFT::loop_points>()) {
        CORRADE_SKIP("Extension AL_SOFT_loop_points isn't supported on this system.");
    }

    Buffer buf;
    constexpr char data[] { 25, 17, 24, 122, 67, 24, 48, 96 };

    buf.setData(BufferFormat::Mono8, data, 22050).setLoopPoints(1, 6);

    Containers::Array<ALint> points{Containers::InPlaceInit, { 0, 0 }};
    alGetBufferiv(buf.id(), AL_LOOP_POINTS_SOFT, points.data());
    CORRADE_COMPARE_AS(points,
        (Containers::Array<ALint>{Containers::InPlaceInit, { 1, 6 }}),
        TestSuite::Compare::Container<Containers::ArrayView<const ALint>>);
}

void BufferALTest::setLoopSince() {
    if(!_context.isExtensionSupported<Audio::Extensions::AL::SOFT::loop_points>()) {
        CORRADE_SKIP("Extension AL_SOFT_loop_points isn't supported on this system.");
    }

    Buffer buf;
    constexpr char data[] { 25, 17, 24, 122, 67, 24, 48, 96 };

    buf.setData(BufferFormat::Mono8, data, 22050).setLoopSince(3);

    Containers::Array<ALint> points{Containers::InPlaceInit, { 0, 0 }};
    alGetBufferiv(buf.id(), AL_LOOP_POINTS_SOFT, points.data());
    CORRADE_COMPARE_AS(points,
        (Containers::Array<ALint>{Containers::InPlaceInit, { 3, 8 }}),
        TestSuite::Compare::Container<Containers::ArrayView<const ALint>>);
}

void BufferALTest::setLoopUntil() {
    if(!_context.isExtensionSupported<Audio::Extensions::AL::SOFT::loop_points>()) {
        CORRADE_SKIP("Extension AL_SOFT_loop_points isn't supported on this system.");
    }

    Buffer buf;
    constexpr char data[] { 25, 17, 24, 122, 67, 24, 48, 96 };

    buf.setData(BufferFormat::Mono8, data, 22050).setLoopUntil(5);

    Containers::Array<ALint> points{Containers::InPlaceInit, { 0, 0 }};
    alGetBufferiv(buf.id(), AL_LOOP_POINTS_SOFT, points.data());
    CORRADE_COMPARE_AS(points,
        (Containers::Array<ALint>{Containers::InPlaceInit, { 0, 5 }}),
        TestSuite::Compare::Container<Containers::ArrayView<const ALint>>);
}

void BufferALTest::resetLoopPoints() {
    if(!_context.isExtensionSupported<Audio::Extensions::AL::SOFT::loop_points>()) {
        CORRADE_SKIP("Extension AL_SOFT_loop_points isn't supported on this system.");
    }

    Buffer buf;
    constexpr char data[] { 25, 17, 24, 122, 67, 24, 48, 96 };

    buf.setData(BufferFormat::Mono8, data, 22050).resetLoopPoints();

    Containers::Array<ALint> points{Containers::InPlaceInit, { 0, 0 }};
    alGetBufferiv(buf.id(), AL_LOOP_POINTS_SOFT, points.data());
    CORRADE_COMPARE_AS(points,
        (Containers::Array<ALint>{Containers::InPlaceInit, { 0, 8 }}),
        TestSuite::Compare::Container<Containers::ArrayView<const ALint>>);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Audio::Test::BufferALTest)
