#ifndef Magnum_British_h
#define Magnum_British_h
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

/* You bloody hell better not tell anybody you bumped into this, aye? Ta! */

#include "Magnum/Magnum.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Magnum {

namespace Math {
    template<class T> using Colour3 = Color3<T>;
    template<class T> using Colour4 = Color4<T>;
    template<class T> using ColourHsv = ColorHsv<T>;
}

typedef Math::Colour3<Float> Colour3;
typedef Math::Colour4<Float> Colour4;
typedef Math::ColourHsv<Float> ColourHsv;
typedef Math::Colour3<UnsignedByte> Colour3ub;
typedef Math::Colour3<UnsignedShort> Colour3us;
typedef Math::Colour4<UnsignedByte> Colour4ub;
typedef Math::Colour4<UnsignedShort> Colour4us;

}
#endif

#endif
