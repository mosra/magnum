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

#include <numeric>
#include <vector>
#include <Corrade/Containers/ArrayView.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Algorithms/KahanSum.h"
#include "Magnum/Math/Algorithms/Svd.h"
#include "Magnum/Math/Packing.h"

using namespace Magnum;
using namespace Magnum::Math::Literals;

int main() {
{
/* [kahanSum] */
std::vector<Float> data(100000000, 1.0f);
Float a = std::accumulate(data.begin(), data.end(), 0.0f);      // 1.667e7f
Float b = Math::Algorithms::kahanSum(data.begin(), data.end()); // 1.000e8f
/* [kahanSum] */
static_cast<void>(a);
static_cast<void>(b);
}

{
/* [kahanSum-iterative] */
Containers::ArrayView<UnsignedByte> pixels;
Float sum = 0.0f, c = 0.0f;
for(UnsignedByte pixel: pixels) {
    Float value = Math::unpack<Float>(pixel);
    sum = Math::Algorithms::kahanSum(&value, &value + 1, sum, &c);
}
/* [kahanSum-iterative] */
}

{
enum: std::size_t { cols = 3, rows = 4 };
/* [svd] */
Math::RectangularMatrix<cols, rows, Double> m;

Math::RectangularMatrix<cols, rows, Double> uPart;
Math::Vector<cols, Double> wDiagonal;
Math::Matrix<cols, Double> v;

std::tie(uPart, wDiagonal, v) = Math::Algorithms::svd(m);

// Extend U
Math::Matrix<rows, Double> u{Math::ZeroInit};
for(std::size_t i = 0; i != rows; ++i)
    u[i] = uPart[i];

// Diagonal W
Math::RectangularMatrix<cols, rows, Double> w =
    Math::RectangularMatrix<cols, rows, Double>::fromDiagonal(wDiagonal);

// u*w*v.transposed() == m
/* [svd] */
static_cast<void>(w);
}

}
