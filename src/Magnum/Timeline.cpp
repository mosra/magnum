/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2022 Stanislaw Halik <sthalik@misaki.pl>

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

#include "Timeline.h"

#include <Corrade/Utility/Debug.h>
#include <Corrade/Utility/System.h>

#include "Magnum/Magnum.h"

using namespace std::chrono;

namespace Magnum {

void Timeline::start() {
    _running = true;
    _startTime = high_resolution_clock::now();
    _previousFrameTime = _startTime;
    _previousFrameDuration = 0;
}

void Timeline::stop() {
    _running = false;
    _startTime = high_resolution_clock::time_point();
    _previousFrameTime = _startTime;
    _previousFrameDuration = 0;
}

void Timeline::nextFrame() {
    if(!_running) return;

    auto now = high_resolution_clock::now();
    auto duration = UnsignedInt(duration_cast<microseconds>(now-_previousFrameTime).count());
    _previousFrameDuration = duration/1e6f;
    _previousFrameTime = now;
}

Float Timeline::previousFrameTime() const {
    return duration_cast<microseconds>(_previousFrameTime-_startTime).count()/1e6f;
}

Float Timeline::currentFrameDuration() const {
    if(!_running) return 0;

    auto now = high_resolution_clock::now();
    auto duration = UnsignedInt(duration_cast<microseconds>(now-_previousFrameTime).count());
    return duration/1e6f;
}

Float Timeline::currentFrameTime() const {
    if(!_running) return 0;

    auto now = high_resolution_clock::now();
    return duration_cast<microseconds>(now-_startTime).count()/1e6f;
}

}
