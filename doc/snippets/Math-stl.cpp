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

#include <ctime>
#include <chrono>
#include <map>
#include <set>

#include "Magnum/Math/StrictWeakOrdering.h"
#include "Magnum/Math/TimeStl.h"
#include "Magnum/Math/Vector4.h"

#define DOXYGEN_ELLIPSIS(...) __VA_ARGS__
#define DOXYGEN_IGNORE(...) __VA_ARGS__

using namespace Magnum;
using namespace Magnum::Math::Literals;

/* Make sure the name doesn't conflict with any other snippets to avoid linker
   warnings, unlike with `int main()` there now has to be a declaration to
   avoid -Wmisssing-prototypes */
void mainMathStl();
void mainMathStl() {
{
Nanoseconds previousFrameTime;
/* The (void) is to avoid -Wvexing-parse */
void stillCanDoSomething(void);
/* The include is already above, so doing it again here should be harmless */
/* [types-time] */
#include <Magnum/Math/TimeStl.h>

DOXYGEN_ELLIPSIS()

using namespace Math::Literals;

Nanoseconds currentFrameTime{std::chrono::steady_clock::now()};

if(currentFrameTime - previousFrameTime < 16.667_msec)
    stillCanDoSomething();
/* [types-time] */
}

{
/* [StrictWeakOrdering] */
std::set<Vector2, Math::StrictWeakOrdering> mySet;
std::map<Vector4, Int, Math::StrictWeakOrdering> myMap;
/* [StrictWeakOrdering] */
static_cast<void>(myMap);
static_cast<void>(mySet);
}

{
void usleep(Long);
/* The DOXYGEN_IGNORE() is to avoid -Wvexing-parse */
/* [Nanoseconds-usage-convert] */
Float fileCopyDuration(DOXYGEN_IGNORE(void));

/* Assuming std::time_t is seconds */
Nanoseconds a{std::time(nullptr)*1000000000};
Seconds b{fileCopyDuration()};

/* And usleep() takes microseconds */
usleep(Long(2.0_sec)/1000);
/* [Nanoseconds-usage-convert] */
}

{
/* The include is already above, so doing it again here should be harmless */
/* [Nanoseconds-usage] */
#include <Magnum/Math/TimeStl.h>

Nanoseconds a{std::chrono::high_resolution_clock::now()};

std::chrono::nanoseconds b(16.67_msec);
/* [Nanoseconds-usage] */
static_cast<void>(b);
}

/* std::time_t is long long or convertible/aliased to long long only on 64-bit
   Linux, apparently. Not on 32-bit, not on Emscripten, not on macOS, not on
   Windows. */
#if defined(__linux__) && !defined(CORRADE_TARGET_32BIT)
{
/* [Nanoseconds-usage-time] */
Nanoseconds a1{std::time(nullptr)};              // wrong, the input is seconds
Nanoseconds a2{std::time(nullptr)*1000000000ll}; // correct
Nanoseconds a3 = std::time(nullptr)*1.0_sec;     // or, alternatively
std::time_t b1(35.0_sec);                    // wrong, the input is nanoseconds
std::time_t b2(35.0_sec/1000000000ll);       // correct
std::time_t b3 = 35.0_sec/1.0_sec;           // or, alternatively
/* [Nanoseconds-usage-time] */
static_cast<void>(a1);
static_cast<void>(a2);
static_cast<void>(a3);
static_cast<void>(b1);
static_cast<void>(b2);
static_cast<void>(b3);
}
#endif
}
