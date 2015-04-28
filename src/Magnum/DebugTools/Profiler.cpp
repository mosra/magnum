/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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

#include "Profiler.h"

#include <algorithm>
#include <numeric>
#include <Corrade/Utility/Assert.h>

#include "Magnum/Magnum.h"

using namespace std::chrono;

namespace Magnum { namespace DebugTools {

Profiler::Section Profiler::addSection(const std::string& name) {
    CORRADE_ASSERT(!enabled, "Profiler: cannot add section when profiling is enabled", 0);
    sections.push_back(name);
    return sections.size()-1;
}

void Profiler::setMeasureDuration(std::size_t frames) {
    CORRADE_ASSERT(!enabled, "Profiler: cannot set measure duration when profiling is enabled", );
    measureDuration = frames;
}

void Profiler::enable() {
    enabled = true;
    frameData.assign(measureDuration*sections.size(), high_resolution_clock::duration::zero());
    totalData.assign(sections.size(), high_resolution_clock::duration::zero());
    frameCount = 0;
}

void Profiler::disable() {
    enabled = false;
}

void Profiler::start(Section section) {
    if(!enabled) return;
    CORRADE_ASSERT(section < sections.size(), "Profiler: unknown section passed to start()", );

    save();

    currentSection = section;
}

void Profiler::stop() {
    if(!enabled) return;

    save();

    previousTime = high_resolution_clock::time_point();
}

void Profiler::save() {
    auto now = high_resolution_clock::now();

    /* If the profiler is already running, add time to given section */
    if(previousTime != high_resolution_clock::time_point())
        frameData[currentFrame*sections.size()+currentSection] += now-previousTime;

    /* Set current time as previous for next section */
    previousTime = now;
}

void Profiler::nextFrame() {
    if(!enabled) return;

    /* Next frame index */
    std::size_t nextFrame = (currentFrame+1) % measureDuration;

    /* Add times of current frame to total */
    for(std::size_t i = 0; i != sections.size(); ++i)
        totalData[i] += frameData[currentFrame*sections.size()+i];

    /* Subtract times of next frame from total and erase them */
    for(std::size_t i = 0; i != sections.size(); ++i) {
        totalData[i] -= frameData[nextFrame*sections.size()+i];
        frameData[nextFrame*sections.size()+i] = high_resolution_clock::duration::zero();
    }

    /* Advance to next frame */
    currentFrame = nextFrame;

    if(frameCount < measureDuration) ++frameCount;
}

void Profiler::printStatistics() {
    if(!enabled) return;

    std::vector<std::size_t> totalSorted(sections.size());
    std::iota(totalSorted.begin(), totalSorted.end(), 0);

    #ifndef CORRADE_GCC44_COMPATIBILITY
    std::sort(totalSorted.begin(), totalSorted.end(), [this](std::size_t i, std::size_t j){return totalData[i] > totalData[j];});
    #else
    std::sort(totalSorted.begin(), totalSorted.end(), Compare(this));
    #endif

    Debug() << "Statistics for last" << measureDuration << "frames:";
    for(std::size_t i = 0; i != sections.size(); ++i)
        Debug() << " " << sections[totalSorted[i]] << duration_cast<microseconds>(totalData[totalSorted[i]]).count()/frameCount
                #if !defined(CORRADE_GCC44_COMPATIBILITY) && !defined(CORRADE_MSVC2013_COMPATIBILITY)
                << u8"µs";
                #else
                << "µs";
                #endif
}

}}
