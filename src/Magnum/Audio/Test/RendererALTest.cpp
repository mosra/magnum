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

#include "Magnum/Audio/Context.h"
#include "Magnum/Audio/Renderer.h"

namespace Magnum { namespace Audio { namespace Test { namespace {

struct RendererALTest: TestSuite::Tester {
    explicit RendererALTest();

    void listenerOrientation();
    void listenerPosition();
    void listenerVelocity();
    void listenerGain();
    void speedOfSound();
    void dopplerFactor();
    void distanceModel();

    Context _context;
};

RendererALTest::RendererALTest():
    TestSuite::Tester{TestSuite::Tester::TesterConfiguration{}.setSkippedArgumentPrefixes({"magnum"})},
    _context{arguments().first, arguments().second}
{
    addTests({&RendererALTest::listenerOrientation,
              &RendererALTest::listenerPosition,
              &RendererALTest::listenerVelocity,
              &RendererALTest::listenerGain,
              &RendererALTest::speedOfSound,
              &RendererALTest::dopplerFactor,
              &RendererALTest::distanceModel});
}

void RendererALTest::listenerOrientation() {
    constexpr Vector3 up{1.0f, 2.0f, 3.0f}, fwd{3.0f, 2.0f, 1.0f};
    Renderer::setListenerOrientation(fwd, up);
    std::array<Vector3, 2> orientation = Renderer::listenerOrientation();

    CORRADE_COMPARE(orientation[0], fwd);
    CORRADE_COMPARE(orientation[1], up);
}

void RendererALTest::listenerPosition() {
    constexpr Vector3 pos{1.0f, 3.0f, 2.0f};
    Renderer::setListenerPosition(pos);

    CORRADE_COMPARE(Renderer::listenerPosition(), pos);
}

void RendererALTest::listenerVelocity() {
    constexpr Vector3 vel{1.0f, 3.0f, 2.0f};
    Renderer::setListenerVelocity(vel);

    CORRADE_COMPARE(Renderer::listenerVelocity(), vel);
}

void RendererALTest::listenerGain() {
    constexpr Float gain = 0.512f;
    Renderer::setListenerGain(gain);

    CORRADE_COMPARE(Renderer::listenerGain(), gain);
}

void RendererALTest::speedOfSound() {
    constexpr Float speed = 1.25f;
    Renderer::setSpeedOfSound(speed);

    CORRADE_COMPARE(Renderer::speedOfSound(), speed);
}

void RendererALTest::dopplerFactor() {
    constexpr Float factor = 0.3335f;
    Renderer::setDopplerFactor(factor);

    CORRADE_COMPARE(Renderer::dopplerFactor(), factor);
}

void RendererALTest::distanceModel() {
    constexpr Renderer::DistanceModel model = Renderer::DistanceModel::InverseClamped;
    Renderer::setDistanceModel(model);

    CORRADE_COMPARE(Renderer::distanceModel(), model);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Audio::Test::RendererALTest)
