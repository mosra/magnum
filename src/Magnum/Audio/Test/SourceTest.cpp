/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Audio/Context.h"
#include "Magnum/Audio/Source.h"

namespace Magnum { namespace Audio { namespace Test {

struct SourceTest: TestSuite::Tester {
    explicit SourceTest();

    void debugState();
    void position();
    void direction();
    void velocity();
    void gain();
    void looping();
    void relative();
    void maxDistance();
    void maxGain();
    void minGain();
    void coneAnglesAndGain();
    void rolloffFactor();

    Context _context;
};

SourceTest::SourceTest() {
    addTests({&SourceTest::debugState,
              &SourceTest::position,
              &SourceTest::direction,
              &SourceTest::velocity,
              &SourceTest::gain,
              &SourceTest::looping,
              &SourceTest::relative,
              &SourceTest::maxDistance,
              &SourceTest::maxGain,
              &SourceTest::minGain,
              &SourceTest::coneAnglesAndGain,
              &SourceTest::rolloffFactor});
}

void SourceTest::debugState() {
    std::ostringstream out;
    Debug(&out) << Source::State::Playing;
    CORRADE_COMPARE(out.str(), "Audio::Source::State::Playing\n");
}

void SourceTest::position() {
    Source source;
    constexpr Vector3 pos{3.0f, 5.0f, 6.0f};
    source.setPosition(pos);

    CORRADE_COMPARE(source.position(), pos);
}

void SourceTest::direction() {
    Source source;
    constexpr Vector3 dir{3.0f, 1.0f, 2.0f};
    source.setDirection(dir);

    CORRADE_COMPARE(source.direction(), dir);
}

void SourceTest::velocity() {
    Source source;
    constexpr Vector3 vel{-3.0f, 5.0f, -6.0f};
    source.setVelocity(vel);

    CORRADE_COMPARE(source.velocity(), vel);
}

void SourceTest::gain() {
    Source source;
    constexpr Float gain = 0.1234f;
    source.setGain(gain);

    CORRADE_COMPARE(source.gain(), gain);
}

void SourceTest::looping() {
    Source source;
    source.setLooping(true);
    CORRADE_VERIFY(source.isLooping());
    source.setLooping(false);
    CORRADE_VERIFY(!source.isLooping());
}

void SourceTest::relative() {
    Source source;
    source.setRelative(true);
    CORRADE_VERIFY(source.isRelative());
    source.setRelative(false);
    CORRADE_VERIFY(!source.isRelative());
}

void SourceTest::maxDistance() {
    Source source;
    constexpr Float dist = 0.222f;
    source.setMaxDistance(dist);

    CORRADE_COMPARE(source.maxDistance(), dist);
}

void SourceTest::maxGain() {
    Source source;
    constexpr Float gain = 0.3131f;
    source.setMaxGain(gain);

    CORRADE_COMPARE(source.maxGain(), gain);
}

void SourceTest::minGain() {
    Source source;
    constexpr Float gain = 0.4144f;
    source.setMinGain(gain);

    CORRADE_COMPARE(source.minGain(), gain);
}

void SourceTest::coneAnglesAndGain() {
    Source source;
    constexpr auto outerAngle = 12.0_degf;
    constexpr auto innerAngle = 21.0_degf;
    constexpr Float outerGain = 0.05f;

    source.setInnerConeAngle(innerAngle);
    source.setOuterConeAngle(outerAngle);
    source.setOuterConeGain(outerGain);

    CORRADE_COMPARE(source.outerConeAngle(), outerAngle);
    CORRADE_COMPARE(source.innerConeAngle(), innerAngle);
    CORRADE_COMPARE(source.outerConeGain(), outerGain);
}

void SourceTest::rolloffFactor() {
    Source source;
    constexpr Float fact = 42.0f;
    source.setRolloffFactor(fact);

    CORRADE_COMPARE(source.rolloffFactor(), fact);
}


}}}

CORRADE_TEST_MAIN(Magnum::Audio::Test::SourceTest)
