/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2015 Jonathan Hale <squareys@googlemail.com>

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
#include "Magnum/Audio/Playable.h"
#include "Magnum/Audio/PlayableGroup.h"
#include "Magnum/SceneGraph/Scene.h"
#include "Magnum/SceneGraph/Object.h"
#include "Magnum/SceneGraph/MatrixTransformation3D.h"

namespace Magnum { namespace Audio { namespace Test { namespace {

typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;
typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;

struct PlayableALTest: TestSuite::Tester {
    explicit PlayableALTest();

    void feature();
    void group();

    Context _context;
};

PlayableALTest::PlayableALTest():
    TestSuite::Tester{TestSuite::Tester::TesterConfiguration{}.setSkippedArgumentPrefixes({"magnum"})},
    _context{arguments().first, arguments().second}
{
    addTests({&PlayableALTest::feature,
              &PlayableALTest::group});
}

void PlayableALTest::feature() {
    Scene3D scene;
    Object3D object{&scene};
    Source source;
    Playable3D playable{object};

    constexpr Vector3 offset{1.0f, 2.0f, 3.0f};
    object.translate(offset);
    object.setClean();

    CORRADE_COMPARE(playable.source().position(), offset);
}

void PlayableALTest::group() {
    Scene3D scene;
    Object3D object{&scene};
    Source source;
    PlayableGroup3D group;
    Playable3D playable{object, &group};

    group.setGain(0.5f);
    CORRADE_COMPARE(playable.source().gain(), 0.5f);

    playable.setGain(0.5f);
    CORRADE_COMPARE(playable.source().gain(), 0.25f);

    group.play();
    group.pause();
    group.stop();
}

}}}}

CORRADE_TEST_MAIN(Magnum::Audio::Test::PlayableALTest)
