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

#include "Magnum/Math/TimeStl.h"

namespace Magnum { namespace Math { namespace Test { namespace {

struct TimeStlTest: TestSuite::Tester {
    explicit TimeStlTest();

    void chronoDurationTypedefs();
    void chronoDurationFloatingPoint();
    void chronoTimePoint();
};

TimeStlTest::TimeStlTest() {
    addTests({&TimeStlTest::chronoDurationTypedefs,
              &TimeStlTest::chronoDurationFloatingPoint,
              &TimeStlTest::chronoTimePoint});
}

using Magnum::Nanoseconds;

using namespace Math::Literals;

void TimeStlTest::chronoDurationTypedefs() {
    /* Negative values should work as well */
    std::chrono::nanoseconds a1{1234567891234567890ll};
    std::chrono::nanoseconds a2{-1234567891234567890ll};
    /* The rest is implemented in a generic way, so no need to test both
       variants of every */
    std::chrono::microseconds b1{4567891234567890ll};
    std::chrono::milliseconds c1{-7891234567890ll};
    std::chrono::seconds d1{1234567890ll};
    std::chrono::minutes e1{-34567890ll};
    std::chrono::hours f1{567890ll};
    Nanoseconds a3{a1};
    Nanoseconds a4{a2};
    CORRADE_COMPARE(a3, 1234567891234567890_nsec);
    CORRADE_COMPARE(a4, -1234567891234567890_nsec);
    /* Using the _nsec literal to circumvent potential rounding errors when
       using the _usec etc literals on platforms without an 80-bit long
       double */
    CORRADE_COMPARE(Nanoseconds{b1},   4567891234567890000_nsec);
    CORRADE_COMPARE(Nanoseconds{c1},  -7891234567890000000_nsec);
    CORRADE_COMPARE(Nanoseconds{d1},   1234567890000000000_nsec);
    CORRADE_COMPARE(Nanoseconds{e1}, 60*-34567890000000000_nsec);
    CORRADE_COMPARE(Nanoseconds{f1}, 60*60*567890000000000_nsec);

    /* Only nanoseconds can be converted back */
    std::chrono::nanoseconds a5(a3);
    std::chrono::nanoseconds a6(a4);
    CORRADE_COMPARE(a5.count(), 1234567891234567890ll);
    CORRADE_COMPARE(a6.count(), -1234567891234567890ll);

    constexpr std::chrono::nanoseconds ca1{1234567891234567890ll};
    constexpr std::chrono::nanoseconds ca2{-1234567891234567890ll};
    constexpr std::chrono::microseconds cb1{4567891234567890ll};
    constexpr std::chrono::milliseconds cc1{-7891234567890ll};
    constexpr std::chrono::seconds cd1{1234567890ll};
    constexpr std::chrono::minutes ce1{-34567890ll};
    constexpr std::chrono::hours cf1{567890ll};
    constexpr Nanoseconds ca3{ca1};
    constexpr Nanoseconds ca4{ca2};
    constexpr Nanoseconds cb2{cb1};
    constexpr Nanoseconds cc2{cc1};
    constexpr Nanoseconds cd2{cd1};
    constexpr Nanoseconds ce2{ce1};
    constexpr Nanoseconds cf2{cf1};
    CORRADE_COMPARE(ca3, 1234567891234567890_nsec);
    CORRADE_COMPARE(ca4, -1234567891234567890_nsec);
    CORRADE_COMPARE(cb2,   4567891234567890000_nsec);
    CORRADE_COMPARE(cc2,  -7891234567890000000_nsec);
    CORRADE_COMPARE(cd2,   1234567890000000000_nsec);
    CORRADE_COMPARE(ce2, 60*-34567890000000000_nsec);
    CORRADE_COMPARE(cf2, 60*60*567890000000000_nsec);

    constexpr std::chrono::nanoseconds ca5(ca3);
    constexpr std::chrono::nanoseconds ca6(ca4);
    CORRADE_COMPARE(ca5.count(), 1234567891234567890ll);
    CORRADE_COMPARE(ca6.count(), -1234567891234567890ll);
}

void TimeStlTest::chronoDurationFloatingPoint() {
    /* Same as chronoDurationTypedefs(), except that this is using a
       floating-point type, for which the precision shouldn't be lost for the
       fractional part. In C++14 with std::chrono_literals this would be
       9087654321987654321.0ns, -9087654321987.654321ms etc. */
    std::chrono::duration<long double, std::nano> a1{9087654321987654321.0l};
    std::chrono::duration<long double, std::nano> a2{-9087654321987654321.0l};
    /* Again, everything except nanoseconds is implemented in a generic way, so
       no need to test both variants of every */
    std::chrono::duration<long double, std::micro> b1{9087654321987654.321l};
    std::chrono::duration<long double, std::milli> c1{-9087654321987.654321l};
    std::chrono::duration<long double> d1{9087654321.987654321l};
    Nanoseconds a3{a1};
    Nanoseconds a4{a2};
    /* Not sure what Emscripten does here, but it behaves as if long double was
       actually the full precision. Similar to the logic in
       TimeTest::literals(). */
    #if !defined(CORRADE_LONG_DOUBLE_SAME_AS_DOUBLE) || defined(CORRADE_TARGET_EMSCRIPTEN)
    CORRADE_COMPARE(a3, 9087654321987654321_nsec);
    CORRADE_COMPARE(a4, -9087654321987654321_nsec);
    CORRADE_COMPARE(Nanoseconds{b1}, 9087654321987654321_nsec);
    /* The conversion suffers from the same minor precision loss as with
       the floating-point Nanosecond literals themselves. Again see
       TimeTest::literals() for details. */
    #if (defined(CORRADE_TARGET_ARM) && !defined(CORRADE_TARGET_32BIT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
    CORRADE_COMPARE(Nanoseconds{c1}, -9087654321987654321_nsec);
    CORRADE_COMPARE(Nanoseconds{d1}, 9087654321987654320_nsec);
    #else
    CORRADE_COMPARE(Nanoseconds{c1}, -9087654321987654320_nsec);
    CORRADE_COMPARE(Nanoseconds{d1}, 9087654321987654321_nsec);
    #endif
    #else
    CORRADE_COMPARE(a3, 9087654321987654656_nsec);
    CORRADE_COMPARE(a4, -9087654321987654656_nsec);
    CORRADE_COMPARE(Nanoseconds{b1}, 9087654321987653632_nsec);
    CORRADE_COMPARE(Nanoseconds{c1}, -9087654321987654656_nsec);
    CORRADE_COMPARE(Nanoseconds{d1}, 9087654321987653632_nsec);
    #endif

    /* Only nanoseconds can be converted back */
    /** @todo enable conversion in the other direction for all ratios if using
        a floating-point representation */
    std::chrono::duration<long double, std::nano> a5(a3);
    std::chrono::duration<long double, std::nano> a6(a4);
    CORRADE_COMPARE(a5.count(), 9087654321987654321.0l);
    CORRADE_COMPARE(a6.count(), -9087654321987654321.0l);

    constexpr std::chrono::duration<long double, std::nano> ca1{9087654321987654321.0l};
    constexpr std::chrono::duration<long double, std::nano> ca2{-9087654321987654321.0l};
    constexpr std::chrono::duration<long double, std::micro> cb1{9087654321987654.321l};
    constexpr std::chrono::duration<long double, std::milli> cc1{-9087654321987.654321l};
    constexpr std::chrono::duration<long double> cd1{9087654321.987654321l};
    constexpr Nanoseconds ca3{ca1};
    constexpr Nanoseconds ca4{ca2};
    constexpr Nanoseconds cb2{cb1};
    constexpr Nanoseconds cc2{cc1};
    constexpr Nanoseconds cd2{cd1};
    #if !defined(CORRADE_LONG_DOUBLE_SAME_AS_DOUBLE) || defined(CORRADE_TARGET_EMSCRIPTEN)
    CORRADE_COMPARE(ca3, 9087654321987654321_nsec);
    CORRADE_COMPARE(ca4, -9087654321987654321_nsec);
    CORRADE_COMPARE(cb2, 9087654321987654321_nsec);
    #if (defined(CORRADE_TARGET_ARM) && !defined(CORRADE_TARGET_32BIT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
    CORRADE_COMPARE(cc2, -9087654321987654321_nsec);
    CORRADE_COMPARE(cd2, 9087654321987654320_nsec);
    #else
    CORRADE_COMPARE(cc2, -9087654321987654320_nsec);
    CORRADE_COMPARE(cd2, 9087654321987654321_nsec);
    #endif
    #else
    CORRADE_COMPARE(ca3, 9087654321987654656_nsec);
    CORRADE_COMPARE(ca4, -9087654321987654656_nsec);
    CORRADE_COMPARE(cb2, 9087654321987653632_nsec);
    CORRADE_COMPARE(cc2, -9087654321987654656_nsec);
    CORRADE_COMPARE(cd2, 9087654321987653632_nsec);
    #endif

    /* Only nanoseconds can be converted back */
    /** @todo enable conversion in the other direction for all ratios if using
        a floating-point representation */
    constexpr std::chrono::duration<long double, std::nano> ca5(ca3);
    constexpr std::chrono::duration<long double, std::nano> ca6(ca4);
    CORRADE_COMPARE(ca5.count(), 9087654321987654321.0l);
    CORRADE_COMPARE(ca6.count(), -9087654321987654321.0l);
}

void TimeStlTest::chronoTimePoint() {
    std::chrono::system_clock::time_point systemNow = std::chrono::system_clock::now();
    std::chrono::steady_clock::time_point steadyNow = std::chrono::steady_clock::now();
    std::chrono::high_resolution_clock::time_point highNow = std::chrono::high_resolution_clock::now();

    Nanoseconds system{systemNow};
    Nanoseconds steady{steadyNow};
    Nanoseconds high{highNow};
    CORRADE_COMPARE(Long(system), std::chrono::duration_cast<std::chrono::nanoseconds>(systemNow.time_since_epoch()).count());
    CORRADE_COMPARE(Long(steady), std::chrono::duration_cast<std::chrono::nanoseconds>(steadyNow.time_since_epoch()).count());
    CORRADE_COMPARE(Long(high), std::chrono::duration_cast<std::chrono::nanoseconds>(highNow.time_since_epoch()).count());

    /* Conversion back is possible only if the STL clock is in nanoseconds.
       That's the case for all three in libstdc++. In libc++ the system_clock
       has only a microsecond precision:
        https://github.com/llvm/llvm-project/blob/44d85c5b15bbf6226f442126735b764d81cbf6e3/libcxx/include/__chrono/system_clock.h#L28
       In MSVC STL system_clock has a 100-nanosecond resolution:
        https://github.com/microsoft/STL/blob/192a84008a59ac4d2e55681e1ffac73535788674/stl/inc/__msvc_chrono.hpp#L636.

       std::ostream operators for std::chrono::duration are only available
       since C++20 so the comparison is this insane. */
    #if !defined(CORRADE_TARGET_LIBCXX) && !defined(CORRADE_TARGET_DINKUMWARE)
    std::chrono::system_clock::time_point systemAgain(system);
    #endif
    std::chrono::steady_clock::time_point steadyAgain(steady);
    std::chrono::high_resolution_clock::time_point highAgain(high);
    #if !defined(CORRADE_TARGET_LIBCXX) && !defined(CORRADE_TARGET_DINKUMWARE)
    CORRADE_COMPARE(systemAgain.time_since_epoch().count(), systemNow.time_since_epoch().count());
    #endif
    CORRADE_COMPARE(steadyAgain.time_since_epoch().count(), steadyNow.time_since_epoch().count());
    CORRADE_COMPARE(highAgain.time_since_epoch().count(), highNow.time_since_epoch().count());

    /* Constexpr variants with a custom value. Spec says the time_point
       constructor is constexpr since C++14. Libstdc++ and MSVC STL has them
       since C++11 already, libc++ explicitly only since C++14. */
    #ifndef CORRADE_TARGET_LIBCXX
    constexpr
    #endif
    std::chrono::high_resolution_clock::time_point ca{std::chrono::nanoseconds{1234567891234567890ll}};
    #ifndef CORRADE_TARGET_LIBCXX
    constexpr
    #endif
    Nanoseconds cb{ca};
    #ifndef CORRADE_TARGET_LIBCXX
    constexpr
    #endif
    std::chrono::high_resolution_clock::time_point cc{cb};
    CORRADE_COMPARE(Long(cb),std::chrono::duration_cast<std::chrono::nanoseconds>(ca.time_since_epoch()).count());
    CORRADE_COMPARE(cc.time_since_epoch().count(), ca.time_since_epoch().count());
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::TimeStlTest)
