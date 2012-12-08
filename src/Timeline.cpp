#include "Timeline.h"
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include <Utility/Debug.h>
#include <Utility/utilities.h>

using namespace std::chrono;

namespace Magnum {

void Timeline::start() {
    running = true;
    _startTime = high_resolution_clock::now();
    _previousFrameTime = _startTime;
    _previousFrameDuration = 0;
}

void Timeline::stop() {
    running = false;
    _startTime = high_resolution_clock::time_point();
    _previousFrameTime = _startTime;
    _previousFrameDuration = 0;
}

void Timeline::nextFrame() {
    if(!running) return;

    auto now = high_resolution_clock::now();
    std::uint32_t duration = duration_cast<microseconds>(now-_previousFrameTime).count();
    _previousFrameDuration = duration/1e6f;

    if(_previousFrameDuration < _minimalFrameTime) {
        Corrade::Utility::sleep(_minimalFrameTime*1000 - duration/1000);
        now = high_resolution_clock::now();
        _previousFrameDuration = duration_cast<microseconds>(now-_previousFrameTime).count()/1e6f;
    }

    _previousFrameTime = now;
}

GLfloat Timeline::previousFrameTime() const {
    return duration_cast<microseconds>(_previousFrameTime-_startTime).count()/1e6f;
}

}
