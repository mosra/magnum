/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2020 janos <janos.meny@googlemail.com>

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

#include <Corrade/Utility/Assert.h>

#include "Functions.h"

namespace Magnum { namespace Math {

#if !defined(CORRADE_TARGET_GCC) && !defined(CORRADE_TARGET_CLANG)
namespace {

/* https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
   there's also https://stackoverflow.com/a/109025 which mostly just copies
   parts of this together with a lot of noise and syntax errors. */
template<class T> inline UnsignedInt popcountImplementation(T v) {
    v = v - ((v >> 1) & ~T(0)/3);
    v = (v & ~T(0)/15*3) + ((v >> 2) & ~T(0)/15*3);
    v = (v + (v >> 4)) & ~T(0)/255*15;
    return (v*(~T(0)/255)) >> (sizeof(T) - 1)*8;
}

}

UnsignedInt popcount(UnsignedInt number) {
    return popcountImplementation(number);
}

UnsignedInt popcount(UnsignedLong number) {
    return popcountImplementation(number);
}
#endif

UnsignedInt log(UnsignedInt base, UnsignedInt number) {
    UnsignedInt log = 0;
    while(number /= base)
        ++log;
    return log;
}

UnsignedInt log2(UnsignedInt number) {
    UnsignedInt log = 0;
    while(number >>= 1)
        ++log;
    return log;
}

UnsignedLong binomialCoefficient(const UnsignedInt n, UnsignedInt k) {
    CORRADE_ASSERT(n >= k,
        "Math::binomialCoefficient(): k can't be greater than n in (" << Corrade::Utility::Debug::nospace << n << "choose" << k << Corrade::Utility::Debug::nospace << ")", {});

    /* k and n - k gives the same value, optimize the calculation to do fewer
       steps */
    if(k*2 > n) k = n - k;

    if(k == 0) return 1;

    UnsignedLong result = n;
    for(UnsignedInt i = 2; i <= k; ++i) {
        CORRADE_ASSERT(result < ~UnsignedLong{} / (n-i+1), "Math::binomialCoefficient(): overflow for (" << Corrade::Utility::Debug::nospace << n << "choose" << k << Corrade::Utility::Debug::nospace << ")", {});

        result *= n - i + 1;
        result /= i;
    }

    return result;
}

}}
