/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

#include "Magnum/Math/Quaternion.h"
#include "Magnum/Animation/Track.h"

using namespace Magnum;
using namespace Magnum::Math::Literals;

int main() {
{
/* [Track-usage] */
const Animation::Track<Float, Vector2> jump{{
    {0.0f, Vector2::yAxis(0.0f)},
    {1.0f, Vector2::yAxis(0.5f)},
    {2.0f, Vector2::yAxis(0.75f)},
    {3.0f, Vector2::yAxis(0.875f)},
    {4.0f, Vector2::yAxis(0.75f)},
    {5.0f, Vector2::yAxis(0.5f)},
    {6.0f, Vector2::yAxis(0.0f)}
}, Math::lerp, Animation::Extrapolation::Constant};

Vector2 position = jump.at(2.2f);               // y = 0.775
/* [Track-usage] */

{
/* [Track-performance-hint] */
std::size_t hint = 0;
Vector2 position = jump.at(2.2f, hint);         // y = 0.775, hint = 2
/* [Track-performance-hint] */
static_cast<void>(position);
}

{
/* [Track-performance-strict] */
std::size_t hint = 0;
Vector2 position = jump.atStrict(2.2f, hint);   // y = 0.775, hint = 2
/* [Track-performance-strict] */
static_cast<void>(position);
}

static_cast<void>(position);
}

{
/* [Track-performance-cache] */
struct Keyframe {
    Float time;
    Vector2 position;
    Deg rotation;
};
const Keyframe data[]{
    {0.0f, Vector2::yAxis(0.0f), 0.0_degf},
    {1.0f, Vector2::yAxis(0.5f), 60.0_degf},
    {2.0f, Vector2::yAxis(0.75f), 80.0_degf},
    {3.0f, Vector2::yAxis(0.875f), 90.0_degf},
    {4.0f, Vector2::yAxis(0.75f), 100.0_degf},
    {5.0f, Vector2::yAxis(0.5f), 120.0_degf},
    {6.0f, Vector2::yAxis(0.0f), 180.0_degf}
};

Animation::TrackView<Float, Vector2> positions{
    {&data[0].time, Containers::arraySize(data), sizeof(Keyframe)},
    {&data[0].position, Containers::arraySize(data), sizeof(Keyframe)},
    Math::lerp};
Animation::TrackView<Float, Deg> rotations{
    {&data[0].time, Containers::arraySize(data), sizeof(Keyframe)},
    {&data[0].rotation, Containers::arraySize(data), sizeof(Keyframe)},
    Math::lerp};

Float time = 2.2f;
std::size_t hint = 0;
Vector2 position = positions.atStrict(time, hint);  // y = 0.775f
Deg rotation = rotations.atStrict(time, hint);      // φ = 82°
/* [Track-performance-cache] */
static_cast<void>(position);
static_cast<void>(rotation);
}

}
