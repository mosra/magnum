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

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Platform/Gesture.h"

namespace Magnum { namespace Platform { namespace Test { namespace {

struct GestureTest: TestSuite::Tester {
    explicit GestureTest();

    void twoFinger();
    void twoFingerPressPrimaryAgain();
    void twoFingerPressPrimaryAfterSecondary();
    void twoFingerSecondaryWithoutPrimary();
    void twoFingerUnknownSecondary();
    void twoFingerReleasePrimary();
    void twoFingerReleaseSecondary();
    template<class T> void twoFingerNonTouchEvents();
};

using namespace Math::Literals;

enum class PointerEventSource {
    Mouse = -1337,
    Touch = 12
};

class PointerEvent {
    public:
        explicit PointerEvent(bool primary, Long id, const Vector2& position): _primary{primary}, _id{id}, _position{position} {}

        PointerEventSource source() const { return PointerEventSource::Touch; }
        bool isPrimary() const { return _primary; }
        Long id() const { return _id; }
        Vector2 position() const { return _position; }

    private:
        bool _primary;
        Long _id;
        Vector2 _position;
};

enum class PointerEventSourceMouseOnly {
    Mouse = -1337
};

GestureTest::GestureTest() {
    addTests({&GestureTest::twoFinger,
              &GestureTest::twoFingerPressPrimaryAgain,
              &GestureTest::twoFingerPressPrimaryAfterSecondary,
              &GestureTest::twoFingerSecondaryWithoutPrimary,
              &GestureTest::twoFingerUnknownSecondary,
              &GestureTest::twoFingerReleasePrimary,
              &GestureTest::twoFingerReleaseSecondary,

              &GestureTest::twoFingerNonTouchEvents<PointerEventSource>,
              &GestureTest::twoFingerNonTouchEvents<PointerEventSourceMouseOnly>});
}

void GestureTest::twoFinger() {
    /* Initially there's nothing */
    TwoFingerGesture g;
    CORRADE_COMPARE(g.fingerCount(), 0);
    CORRADE_VERIFY(!g.isGesture());
    CORRADE_VERIFY(!g);
    CORRADE_COMPARE(Math::isNan(g.position()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(g.direction()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(g.relativeTranslation()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(Vector2{g.relativeRotation()}), BitVector2{3});
    CORRADE_VERIFY(Math::isNan(g.relativeScaling()));

    /* After pressing just the primary there's no gesture yet. Using large IDs
       to verify they're stored as full 64-bit numbers */
    CORRADE_VERIFY(g.pressEvent(PointerEvent{true, 1ll << 37, {10.0f, 20.0f}}));
    CORRADE_COMPARE(g.fingerCount(), 1);
    CORRADE_VERIFY(!g.isGesture());
    CORRADE_VERIFY(!g);
    CORRADE_COMPARE(Math::isNan(g.position()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(g.direction()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(g.relativeTranslation()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(Vector2{g.relativeRotation()}), BitVector2{3});
    CORRADE_VERIFY(Math::isNan(g.relativeScaling()));

    /* With a secondary press there's a gesture. We don't check the ID in this
       case, just the primary/secondary distinction, so it's fine if both are
       the same. */
    CORRADE_VERIFY(g.pressEvent(PointerEvent{false, 1ll << 37, {20.0f, 10.0f}}));
    CORRADE_COMPARE(g.fingerCount(), 2);
    CORRADE_VERIFY(g.isGesture());
    CORRADE_VERIFY(g);
    CORRADE_COMPARE(g.position(), Vector2{15.0f});
    CORRADE_COMPARE(g.direction(), (Vector2{-5.0f, 5.0f}));
    /* Positive direction should point to the primary event, negative to the
       secondary */
    CORRADE_COMPARE(g.position() + g.direction(), (Vector2{10.0f, 20.0f}));
    CORRADE_COMPARE(g.position() - g.direction(), (Vector2{20.0f, 10.0f}));
    /* No movement yet, so default values */
    CORRADE_COMPARE(g.relativeTranslation(), Vector2{});
    CORRADE_COMPARE(g.relativeRotation(), Complex{});
    CORRADE_COMPARE(g.relativeScaling(), 1.0f);

    /* Move primary finger to the other side of the secondary, forming a
       translation and a 180° rotation */
    CORRADE_VERIFY(g.moveEvent(PointerEvent{true, 1ll << 37, {30.0f, 0.0f}}));
    CORRADE_COMPARE(g.fingerCount(), 2);
    CORRADE_VERIFY(g.isGesture());
    CORRADE_VERIFY(g);
    CORRADE_COMPARE(g.position(), (Vector2{25.0f, 5.0f}));
    CORRADE_COMPARE(g.direction(), (Vector2{5.0f, -5.0f}));
    CORRADE_COMPARE(g.relativeTranslation(), (Vector2{10.0f, -10.0f}));
    CORRADE_COMPARE(g.relativeRotation(), Complex::rotation(180.0_degf));
    CORRADE_COMPARE(g.relativeScaling(), 1.0f);

    /* Move secondary finger slightly to the right, forming a counterclockwise
       rotation, thus less than 180° */
    CORRADE_VERIFY(g.moveEvent(PointerEvent{false, 1ll << 37, {25.0f, 10.0f}}));
    CORRADE_COMPARE(g.fingerCount(), 2);
    CORRADE_VERIFY(g.isGesture());
    CORRADE_VERIFY(g);
    CORRADE_COMPARE(g.position(), (Vector2{27.5f, 5.0f}));
    CORRADE_COMPARE(g.direction(), (Vector2{2.5f, -5.0f}));
    CORRADE_COMPARE(g.relativeTranslation(), (Vector2{12.5f, -10.0f}));
    CORRADE_COMPARE(g.relativeRotation(), Complex::rotation(161.565001424104_degf));
    CORRADE_COMPARE(g.relativeScaling(), 0.790569f);

    /* Moving primary and secondary fingers back results in the same absolute
       values as initially, and relative values inverted compared to above */
    CORRADE_VERIFY(g.moveEvent(PointerEvent{true, 1ll << 37, {10.0f, 20.0f}}));
    CORRADE_VERIFY(g.moveEvent(PointerEvent{false, 1ll << 37, {20.0f, 10.0f}}));
    CORRADE_COMPARE(g.fingerCount(), 2);
    CORRADE_VERIFY(g.isGesture());
    CORRADE_VERIFY(g);
    CORRADE_COMPARE(g.position(), Vector2{15.0f});
    CORRADE_COMPARE(g.direction(), (Vector2{-5.0f, 5.0f}));
    CORRADE_COMPARE(g.relativeTranslation(), (Vector2{-12.5f, 10.0f}));
    CORRADE_COMPARE(g.relativeRotation(), Complex::rotation(-161.565001424104_degf));
    CORRADE_COMPARE(g.relativeScaling(), 1.0f/0.790569f);
}

void GestureTest::twoFingerPressPrimaryAgain() {
    TwoFingerGesture g;

    CORRADE_VERIFY(g.pressEvent(PointerEvent{true, 37, {10.0f, 20.0f}}));
    CORRADE_COMPARE(g.fingerCount(), 1);
    CORRADE_VERIFY(!g.isGesture());
    CORRADE_VERIFY(!g);
    CORRADE_COMPARE(Math::isNan(g.position()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(g.direction()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(g.relativeTranslation()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(Vector2{g.relativeRotation()}), BitVector2{3});
    CORRADE_VERIFY(Math::isNan(g.relativeScaling()));

    /* Another primary press replaces the original */
    CORRADE_VERIFY(g.pressEvent(PointerEvent{true, 76, {10.0f, 20.0f}}));
    CORRADE_COMPARE(g.fingerCount(), 1);
    CORRADE_VERIFY(!g.isGesture());
    CORRADE_VERIFY(!g);
    CORRADE_COMPARE(Math::isNan(g.position()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(g.direction()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(g.relativeTranslation()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(Vector2{g.relativeRotation()}), BitVector2{3});
    CORRADE_VERIFY(Math::isNan(g.relativeScaling()));
}

void GestureTest::twoFingerPressPrimaryAfterSecondary() {
    TwoFingerGesture g;

    CORRADE_VERIFY(g.pressEvent(PointerEvent{true, 37, {10.0f, 20.0f}}));
    CORRADE_VERIFY(g.pressEvent(PointerEvent{false, 26, {20.0f, 10.0f}}));
    CORRADE_COMPARE(g.fingerCount(), 2);
    CORRADE_VERIFY(g.isGesture());
    CORRADE_VERIFY(g);
    CORRADE_COMPARE(g.position(), Vector2{15.0f});
    CORRADE_COMPARE(g.direction(), (Vector2{-5.0f, 5.0f}));
    CORRADE_COMPARE(g.relativeTranslation(), Vector2{});
    CORRADE_COMPARE(g.relativeRotation(), Complex{});
    CORRADE_COMPARE(g.relativeScaling(), 1.0f);

    /* Another primary press replaces both */
    CORRADE_VERIFY(g.pressEvent(PointerEvent{true, 76, {10.0f, 20.0f}}));
    CORRADE_COMPARE(g.fingerCount(), 1);
    CORRADE_VERIFY(!g.isGesture());
    CORRADE_VERIFY(!g);
    CORRADE_COMPARE(Math::isNan(g.position()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(g.direction()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(g.relativeTranslation()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(Vector2{g.relativeRotation()}), BitVector2{3});
    CORRADE_VERIFY(Math::isNan(g.relativeScaling()));
}

void GestureTest::twoFingerSecondaryWithoutPrimary() {
    TwoFingerGesture g;

    /* Pressing a secondary pointer without a primary being recorded first
       does nothing, neither does move or release */
    CORRADE_VERIFY(!g.pressEvent(PointerEvent{false, 26, {20.0f, 10.0f}}));
    CORRADE_COMPARE(g.fingerCount(), 0);
    CORRADE_VERIFY(!g.isGesture());
    CORRADE_VERIFY(!g);
    CORRADE_COMPARE(Math::isNan(g.position()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(g.direction()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(g.relativeTranslation()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(Vector2{g.relativeRotation()}), BitVector2{3});
    CORRADE_VERIFY(Math::isNan(g.relativeScaling()));

    CORRADE_VERIFY(!g.moveEvent(PointerEvent{false, 26, {20.0f, 10.0f}}));
    CORRADE_COMPARE(g.fingerCount(), 0);
    CORRADE_VERIFY(!g.isGesture());
    CORRADE_VERIFY(!g);
    CORRADE_COMPARE(Math::isNan(g.position()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(g.direction()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(g.relativeTranslation()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(Vector2{g.relativeRotation()}), BitVector2{3});
    CORRADE_VERIFY(Math::isNan(g.relativeScaling()));

    CORRADE_VERIFY(!g.releaseEvent(PointerEvent{false, 26, {20.0f, 10.0f}}));
    CORRADE_COMPARE(g.fingerCount(), 0);
    CORRADE_VERIFY(!g.isGesture());
    CORRADE_VERIFY(!g);
    CORRADE_COMPARE(Math::isNan(g.position()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(g.direction()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(g.relativeTranslation()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(Vector2{g.relativeRotation()}), BitVector2{3});
    CORRADE_VERIFY(Math::isNan(g.relativeScaling()));
}

void GestureTest::twoFingerUnknownSecondary() {
    TwoFingerGesture g;

    /* Using large IDs to verify they're stored as full 64-bit numbers */
    CORRADE_VERIFY(g.pressEvent(PointerEvent{true, 1ll << 39, {10.0f, 20.0f}}));
    CORRADE_VERIFY(g.pressEvent(PointerEvent{false, 1ll << 37, {20.0f, 10.0f}}));
    CORRADE_COMPARE(g.fingerCount(), 2);
    CORRADE_VERIFY(g.isGesture());
    CORRADE_VERIFY(g);
    CORRADE_COMPARE(g.position(), Vector2{15.0f});
    CORRADE_COMPARE(g.direction(), (Vector2{-5.0f, 5.0f}));
    CORRADE_COMPARE(g.relativeTranslation(), Vector2{});
    CORRADE_COMPARE(g.relativeRotation(), Complex{});
    CORRADE_COMPARE(g.relativeScaling(), 1.0f);

    /* Neither of these should affect the internal state in any way as it has a
       different ID than the first recorded secondary press */
    CORRADE_VERIFY(!g.pressEvent(PointerEvent{false, 1ll << 39, {0.0f, 0.0f}}));
    CORRADE_COMPARE(g.fingerCount(), 2);
    CORRADE_VERIFY(g.isGesture());
    CORRADE_VERIFY(g);
    CORRADE_COMPARE(g.position(), Vector2{15.0f});
    CORRADE_COMPARE(g.direction(), (Vector2{-5.0f, 5.0f}));
    CORRADE_COMPARE(g.relativeTranslation(), Vector2{});
    CORRADE_COMPARE(g.relativeRotation(), Complex{});
    CORRADE_COMPARE(g.relativeScaling(), 1.0f);

    CORRADE_VERIFY(!g.moveEvent(PointerEvent{false, 1ll << 39, {0.0f, 0.0f}}));
    CORRADE_COMPARE(g.fingerCount(), 2);
    CORRADE_VERIFY(g.isGesture());
    CORRADE_VERIFY(g);
    CORRADE_COMPARE(g.position(), Vector2{15.0f});
    CORRADE_COMPARE(g.direction(), (Vector2{-5.0f, 5.0f}));
    CORRADE_COMPARE(g.relativeTranslation(), Vector2{});
    CORRADE_COMPARE(g.relativeRotation(), Complex{});
    CORRADE_COMPARE(g.relativeScaling(), 1.0f);

    CORRADE_VERIFY(!g.releaseEvent(PointerEvent{false, 1ll << 39, {0.0f, 0.0f}}));
    CORRADE_COMPARE(g.fingerCount(), 2);
    CORRADE_VERIFY(g.isGesture());
    CORRADE_VERIFY(g);
    CORRADE_COMPARE(g.position(), Vector2{15.0f});
    CORRADE_COMPARE(g.direction(), (Vector2{-5.0f, 5.0f}));
    CORRADE_COMPARE(g.relativeTranslation(), Vector2{});
    CORRADE_COMPARE(g.relativeRotation(), Complex{});
    CORRADE_COMPARE(g.relativeScaling(), 1.0f);
}

void GestureTest::twoFingerReleasePrimary() {
    TwoFingerGesture g;

    /* Using large IDs to verify they're stored as full 64-bit numbers */
    CORRADE_VERIFY(g.pressEvent(PointerEvent{true, 1ll << 37, {10.0f, 20.0f}}));
    CORRADE_VERIFY(g.pressEvent(PointerEvent{false, 1ll << 26, {20.0f, 10.0f}}));
    CORRADE_COMPARE(g.fingerCount(), 2);
    CORRADE_VERIFY(g.isGesture());
    CORRADE_VERIFY(g);
    CORRADE_COMPARE(g.position(), Vector2{15.0f});
    CORRADE_COMPARE(g.direction(), (Vector2{-5.0f, 5.0f}));
    CORRADE_COMPARE(g.relativeTranslation(), Vector2{});
    CORRADE_COMPARE(g.relativeRotation(), Complex{});
    CORRADE_COMPARE(g.relativeScaling(), 1.0f);

    /* Releasing a primary pointer with a different ID shouldn't affect
       anything */
    CORRADE_VERIFY(!g.releaseEvent(PointerEvent{true, 1ll << 26, {10.0f, 20.0f}}));
    CORRADE_COMPARE(g.fingerCount(), 2);
    CORRADE_VERIFY(g.isGesture());
    CORRADE_VERIFY(g);
    CORRADE_COMPARE(g.position(), Vector2{15.0f});
    CORRADE_COMPARE(g.direction(), (Vector2{-5.0f, 5.0f}));
    CORRADE_COMPARE(g.relativeTranslation(), Vector2{});
    CORRADE_COMPARE(g.relativeRotation(), Complex{});
    CORRADE_COMPARE(g.relativeScaling(), 1.0f);

    /* Releasing the recorded primary pointer resets everything, it'll wait for
       a new primary touch */
    CORRADE_VERIFY(g.releaseEvent(PointerEvent{true, 1ll << 37, {10.0f, 20.0f}}));
    CORRADE_COMPARE(g.fingerCount(), 0);
    CORRADE_VERIFY(!g.isGesture());
    CORRADE_VERIFY(!g);
    CORRADE_COMPARE(Math::isNan(g.position()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(g.direction()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(g.relativeTranslation()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(Vector2{g.relativeRotation()}), BitVector2{3});
    CORRADE_VERIFY(Math::isNan(g.relativeScaling()));
}

void GestureTest::twoFingerReleaseSecondary() {
    TwoFingerGesture g;

    /* Using large IDs to verify they're stored as full 64-bit numbers */
    CORRADE_VERIFY(g.pressEvent(PointerEvent{true, 1ll << 37, {10.0f, 20.0f}}));
    CORRADE_VERIFY(g.pressEvent(PointerEvent{false, 1ll << 26, {20.0f, 10.0f}}));
    CORRADE_COMPARE(g.fingerCount(), 2);
    CORRADE_VERIFY(g.isGesture());
    CORRADE_VERIFY(g);
    CORRADE_COMPARE(g.position(), Vector2{15.0f});
    CORRADE_COMPARE(g.direction(), (Vector2{-5.0f, 5.0f}));
    CORRADE_COMPARE(g.relativeTranslation(), Vector2{});
    CORRADE_COMPARE(g.relativeRotation(), Complex{});
    CORRADE_COMPARE(g.relativeScaling(), 1.0f);

    /* Releasing a secondary pointer with a different ID shouldn't affect
       anything */
    CORRADE_VERIFY(!g.releaseEvent(PointerEvent{false, 1ll << 37, {10.0f, 20.0f}}));
    CORRADE_COMPARE(g.fingerCount(), 2);
    CORRADE_VERIFY(g.isGesture());
    CORRADE_VERIFY(g);
    CORRADE_COMPARE(g.position(), Vector2{15.0f});
    CORRADE_COMPARE(g.direction(), (Vector2{-5.0f, 5.0f}));
    CORRADE_COMPARE(g.relativeTranslation(), Vector2{});
    CORRADE_COMPARE(g.relativeRotation(), Complex{});
    CORRADE_COMPARE(g.relativeScaling(), 1.0f);

    /* Releasing the recorded secondary pointer resets just the secondary
       pointer */
    CORRADE_VERIFY(g.releaseEvent(PointerEvent{false, 1ll << 26, {10.0f, 20.0f}}));
    CORRADE_COMPARE(g.fingerCount(), 1);
    CORRADE_VERIFY(!g.isGesture());
    CORRADE_VERIFY(!g);
    CORRADE_COMPARE(Math::isNan(g.position()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(g.direction()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(g.relativeTranslation()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(Vector2{g.relativeRotation()}), BitVector2{3});
    CORRADE_VERIFY(Math::isNan(g.relativeScaling()));

    /* Press another secondary pointer (though with the same ID), but at other
       side of the primary */
    CORRADE_VERIFY(g.pressEvent(PointerEvent{false, 1ll << 26, {0.0f, 30.0f}}));
    CORRADE_COMPARE(g.fingerCount(), 2);
    CORRADE_VERIFY(g.isGesture());
    CORRADE_VERIFY(g);
    CORRADE_COMPARE(g.position(), (Vector2{5.0f, 25.0f}));
    CORRADE_COMPARE(g.direction(), (Vector2{5.0f, -5.0f}));
    /* Positive direction should point to the primary event, negative to the
       secondary */
    CORRADE_COMPARE(g.position() + g.direction(), (Vector2{10.0f, 20.0f}));
    CORRADE_COMPARE(g.position() - g.direction(), (Vector2{0.0f, 30.0f}));
    /* The relative values shouldn't take the previous press into account,
       should be identities. */
    CORRADE_COMPARE(g.relativeTranslation(), Vector2{});
    CORRADE_COMPARE(g.relativeRotation(), Complex{});
    CORRADE_COMPARE(g.relativeScaling(), 1.0f);
}

template<class T> void GestureTest::twoFingerNonTouchEvents() {
    setTestCaseTemplateName(std::is_same<T, PointerEventSourceMouseOnly>::value ? "PointerEventSourceMouseOnly" : "PointerEventSource");

    TwoFingerGesture g;

    struct {
        T source() const { return T::Mouse; }
        bool isPrimary() const { return true; }
        Long id() const { return 0; }
        Vector2 position() const { return {}; }
    } event;

    /* The event should be ignored by all APIs because it's not a touch one */
    CORRADE_VERIFY(!g.pressEvent(event));
    CORRADE_COMPARE(g.fingerCount(), 0);
    CORRADE_VERIFY(!g.isGesture());
    CORRADE_VERIFY(!g);
    CORRADE_COMPARE(Math::isNan(g.position()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(g.direction()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(g.relativeTranslation()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(Vector2{g.relativeRotation()}), BitVector2{3});
    CORRADE_VERIFY(Math::isNan(g.relativeScaling()));

    CORRADE_VERIFY(!g.moveEvent(event));
    CORRADE_COMPARE(g.fingerCount(), 0);
    CORRADE_VERIFY(!g.isGesture());
    CORRADE_VERIFY(!g);
    CORRADE_COMPARE(Math::isNan(g.position()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(g.direction()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(g.relativeTranslation()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(Vector2{g.relativeRotation()}), BitVector2{3});
    CORRADE_VERIFY(Math::isNan(g.relativeScaling()));

    CORRADE_VERIFY(!g.releaseEvent(event));
    CORRADE_COMPARE(g.fingerCount(), 0);
    CORRADE_VERIFY(!g.isGesture());
    CORRADE_VERIFY(!g);
    CORRADE_COMPARE(Math::isNan(g.position()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(g.direction()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(g.relativeTranslation()), BitVector2{3});
    CORRADE_COMPARE(Math::isNan(Vector2{g.relativeRotation()}), BitVector2{3});
    CORRADE_VERIFY(Math::isNan(g.relativeScaling()));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Platform::Test::GestureTest)
