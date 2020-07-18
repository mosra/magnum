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

#include <Corrade/Utility/Debug.h>

#include "Functions.h"

namespace Magnum { namespace Math {

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


UnsignedLong binomialCoefficient(UnsignedInt n, UnsignedInt k) {
    if (k > n) return 0;
    if (k * 2 > n)
        k = n-k;
    if (k == 0) return 1;

    UnsignedLong result = n;
    for(UnsignedInt i = 2; i <= k; ++i ) {
        CORRADE_ASSERT(result < ~UnsignedLong{} / (n-i+1), "Math::binomialCoefficient(): overflow for (" << Corrade::Utility::Debug::nospace << n << "choose" << k << Corrade::Utility::Debug::nospace << ")", 0ul);
        result *= (n-i+1);
        result /= i;
    }
    return result;
}

}}
