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
    previousFrameTime = high_resolution_clock::now();
    _previousFrameDuration = 0;
}

void Timeline::stop() {
    running = false;
    previousFrameTime = high_resolution_clock::time_point();
    _previousFrameDuration = 0;
}

void Timeline::nextFrame() {
    if(!running) return;

    auto now = high_resolution_clock::now();
    unsigned int duration = duration_cast<microseconds>(now-previousFrameTime).count();
    _previousFrameDuration = duration/1e6f;

    if(_previousFrameDuration < _minimalFrameTime) {
        Corrade::Utility::sleep(_minimalFrameTime*1000 - duration/1000);
        now = high_resolution_clock::now();
        _previousFrameDuration = duration_cast<microseconds>(now-previousFrameTime).count()/1e6f;
    }

    previousFrameTime = now;
}

}
