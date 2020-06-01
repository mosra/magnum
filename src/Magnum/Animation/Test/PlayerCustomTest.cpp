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

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Animation/Player.hpp"

namespace Magnum { namespace Animation { namespace Test { namespace {

struct PlayerCustomTest: TestSuite::Tester {
    explicit PlayerCustomTest();

    void test();
};

PlayerCustomTest::PlayerCustomTest() {
    addTests({&PlayerCustomTest::test});
}

#ifdef CORRADE_TARGET_EMSCRIPTEN
typedef std::uint64_t UnsignedLong; /** @todo what about this? */
#endif

const Animation::Track<UnsignedShort, Float> Track{{
    {24, 1.5f}, /* 1.0 sec */
    {60, 3.0f}, /* 2.5 sec */
    {72, 5.0f}, /* 3.0 sec */
    {96, 2.0f}  /* 4.0 sec */
}, Math::lerp};

void PlayerCustomTest::test() {
    Player<UnsignedLong, UnsignedShort> player{
        /* Keep this in sync with PlayerCustomTest */
        [](UnsignedLong time, UnsignedShort duration) {
            /* One frame is 1/24 second */
            const UnsignedLong durationNs = UnsignedLong(duration)*1000000ull/24;
            const UnsignedInt playCount = time/durationNs;
            const UnsignedShort factor = (time - playCount*durationNs)*24/1000000ull;
            return std::make_pair(playCount, factor);
        }};

    Float value = -1.0f;
    player.add(Track, value)
        .play(2000000ull);

    CORRADE_COMPARE(player.duration().size(), 24*3);

    /* 1.75 secs in */
    player.advance(3750000ull);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, 4.0f);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Animation::Test::PlayerCustomTest)
