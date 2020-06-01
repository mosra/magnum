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

#include "Magnum/Math/Color.h"
#include "Magnum/Primitives/Gradient.h"
#include "Magnum/Primitives/Line.h"
#include "Magnum/Trade/MeshData.h"

using namespace Magnum;

int main() {
{
Color4 colorLeft, colorRight, colorBottom, colorTop;
/* [gradient2DHorizontal] */
Primitives::gradient2D({-1.0f, 0.0f}, colorLeft, {1.0f, 0.0f}, colorRight);
/* [gradient2DHorizontal] */

/* [gradient2DVertical] */
Primitives::gradient2D({0.0f, -1.0f}, colorBottom, {0.0f, 1.0f}, colorTop);
/* [gradient2DVertical] */

/* [gradient3DHorizontal] */
Primitives::gradient3D({-1.0f, 0.0f, 0.0f}, colorLeft,
                       { 1.0f, 0.0f, 0.0f}, colorRight);
/* [gradient3DHorizontal] */

/* [gradient3DVertical] */
Primitives::gradient3D({0.0f, -1.0f, 0.0f}, colorBottom,
                       {0.0f,  1.0f, 0.0f}, colorTop);
/* [gradient3DVertical] */
}

{
/* [line2D-identity] */
Primitives::line2D({0.0f, 0.0f}, {1.0f, 0.0f});
/* [line2D-identity] */
}

{
/* [line3D-identity] */
Primitives::line3D({0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
/* [line3D-identity] */
}
}
