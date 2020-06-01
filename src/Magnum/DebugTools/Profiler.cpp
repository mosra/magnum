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

#define _MAGNUM_NO_DEPRECATED_PROFILER

#include "Profiler.h"

#include <algorithm>
#include <numeric>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Magnum.h"

using namespace std::chrono;

namespace Magnum { namespace DebugTools {

/* MSVC complains about deprecated Section here */
CORRADE_IGNORE_DEPRECATED_PUSH
Profiler::Section Profiler::addSection(const std::string& name) {
    CORRADE_ASSERT(!_enabled, "Profiler: cannot add section when profiling is enabled", 0);
    _sections.push_back(name);
    return _sections.size()-1;
}
CORRADE_IGNORE_DEPRECATED_POP

void Profiler::setMeasureDuration(std::size_t frames) {
    CORRADE_ASSERT(!_enabled, "Profiler: cannot set measure duration when profiling is enabled", );
    _measureDuration = frames;
}

void Profiler::enable() {
    _enabled = true;
    _frameData.assign(_measureDuration*_sections.size(), high_resolution_clock::duration::zero());
    _totalData.assign(_sections.size(), high_resolution_clock::duration::zero());
    _frameCount = 0;
}

void Profiler::disable() {
    _enabled = false;
}

/* MSVC complains about deprecated Section here */
CORRADE_IGNORE_DEPRECATED_PUSH
void Profiler::start(Section section) {
    if(!_enabled) return;
    CORRADE_ASSERT(section < _sections.size(), "Profiler: unknown section passed to start()", );

    save();

    _currentSection = section;
}
CORRADE_IGNORE_DEPRECATED_POP

void Profiler::stop() {
    if(!_enabled) return;

    save();

    _previousTime = high_resolution_clock::time_point();
}

void Profiler::save() {
    auto now = high_resolution_clock::now();

    /* If the profiler is already running, add time to given section */
    if(_previousTime != high_resolution_clock::time_point())
        _frameData[_currentFrame*_sections.size()+_currentSection] += now-_previousTime;

    /* Set current time as previous for next section */
    _previousTime = now;
}

void Profiler::nextFrame() {
    if(!_enabled) return;

    /* Next frame index */
    std::size_t nextFrame = (_currentFrame+1) % _measureDuration;

    /* Add times of current frame to total */
    for(std::size_t i = 0; i != _sections.size(); ++i)
        _totalData[i] += _frameData[_currentFrame*_sections.size()+i];

    /* Subtract times of next frame from total and erase them */
    for(std::size_t i = 0; i != _sections.size(); ++i) {
        _totalData[i] -= _frameData[nextFrame*_sections.size()+i];
        _frameData[nextFrame*_sections.size()+i] = high_resolution_clock::duration::zero();
    }

    /* Advance to next frame */
    _currentFrame = nextFrame;

    if(_frameCount < _measureDuration) ++_frameCount;
}

void Profiler::printStatistics() {
    if(!_enabled) return;

    std::vector<std::size_t> totalSorted(_sections.size());
    std::iota(totalSorted.begin(), totalSorted.end(), 0);

    /* MSVC complains about deprecated _totalData here (WTF) */
    CORRADE_IGNORE_DEPRECATED_PUSH
    std::sort(totalSorted.begin(), totalSorted.end(), [this](std::size_t i, std::size_t j){return _totalData[i] > _totalData[j];});
    CORRADE_IGNORE_DEPRECATED_POP

    Debug() << "Statistics for last" << _measureDuration << "frames:";
    for(std::size_t i = 0; i != _sections.size(); ++i)
        Debug() << " " << _sections[totalSorted[i]] << duration_cast<microseconds>(_totalData[totalSorted[i]]).count()/_frameCount << u8"µs";
}

}}
