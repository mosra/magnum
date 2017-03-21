/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include "Color.h"

namespace Magnum { namespace Math {

namespace {
    constexpr const char Hex[]{"0123456789abcdef"};
}

Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const Color3<UnsignedByte>& value) {
    char out[] = "#______";
    out[1] = Hex[(value.r() >> 4) & 0xf];
    out[2] = Hex[(value.r() >> 0) & 0xf];
    out[3] = Hex[(value.g() >> 4) & 0xf];
    out[4] = Hex[(value.g() >> 0) & 0xf];
    out[5] = Hex[(value.b() >> 4) & 0xf];
    out[6] = Hex[(value.b() >> 0) & 0xf];
    return debug << out;
}

Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const Color4<UnsignedByte>& value) {
    char out[] = "#________";
    out[1] = Hex[(value.r() >> 4) & 0xf];
    out[2] = Hex[(value.r() >> 0) & 0xf];
    out[3] = Hex[(value.g() >> 4) & 0xf];
    out[4] = Hex[(value.g() >> 0) & 0xf];
    out[5] = Hex[(value.b() >> 4) & 0xf];
    out[6] = Hex[(value.b() >> 0) & 0xf];
    out[7] = Hex[(value.a() >> 4) & 0xf];
    out[8] = Hex[(value.a() >> 0) & 0xf];
    return debug << out;
}

}}
