/*
    This file is part of Magnum.

    {{copyright}}

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

// {{includes}}

#pragma ACME comments off

#pragma ACME path ../
#pragma ACME path ../../../corrade/src
#pragma ACME local Corrade
#pragma ACME local Magnum

/* For Corrade/configure.h and Magnum/configure.h */
#pragma ACME path ../../build/src
#pragma ACME path ../../../corrade/build/src

#pragma ACME disable CORRADE_BUILD_DEPRECATED
#pragma ACME disable MAGNUM_BUILD_DEPRECATED

#pragma ACME disable CORRADE_NO_ASSERT
#pragma ACME enable CORRADE_STANDARD_ASSERT
#pragma ACME enable CORRADE_NO_DEBUG
#pragma ACME enable CORRADE_NO_TWEAKABLE
#pragma ACME disable CORRADE_GRACEFUL_ASSERT
#pragma ACME disable DOXYGEN_GENERATING_OUTPUT

/* Make it possible to include Assert.h multiple times */
#pragma ACME disable Corrade_Utility_Assert_h

#pragma ACME revision corrade/src echo "$(git describe --long --match 'v*') ($(date -d @$(git log -1 --format=%at) +%Y-%m-%d))"
#pragma ACME revision magnum/src echo "$(git describe --long --match 'v*') ($(date -d @$(git log -1 --format=%at) +%Y-%m-%d))"
#pragma ACME stats loc wc -l
#pragma ACME stats preprocessed g++ -std=c++11 -P -E -x c++ - | wc -l
