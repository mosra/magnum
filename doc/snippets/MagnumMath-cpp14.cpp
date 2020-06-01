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

#include "Magnum/Math/BoolVector.h"
#include "Magnum/Magnum.h"

using namespace Magnum;

int main() {
{
/* [BoolVector-indexing] */
BoolVector4 a{0b0010};
Debug{} << a[1];  // true

Math::BoolVector<19> b{0b00001000, 0b00000011, 0b100};
Debug{} << b[3];  // true
Debug{} << b[8];  // true
Debug{} << b[9];  // true
Debug{} << b[18]; // true
/* [BoolVector-indexing] */
}

{
/* [BoolVector-debug] */
Debug{} << BoolVector4{0b1010}
        << Math::BoolVector<19>{0b00001000, 0b00000011, 0b100};
/* [BoolVector-debug] */
}
}
