#ifndef Magnum_DebugTools_Profiler_h
#define Magnum_DebugTools_Profiler_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

/** @file
 * @brief Class @ref Magnum::DebugTools::Profiler
 */

#include <chrono>
#include <initializer_list>
#include <string>
#include <vector>

#include "Magnum/Types.h"
#include "Magnum/DebugTools/visibility.h"

namespace Magnum { namespace DebugTools {

/**
@brief Profiler

Measures time passed during specified sections of each frame. It's meant to be
used in rendering and event loops (e.g. @ref Platform::Sdl2Application::drawEvent()),
but it's possible to use it standalone elsewhere. Example usage:
@code
DebugTools::Profiler p;

// Register named sections
struct {
    DebugTools::Profiler::Section ai, physics, draw, bufferSwap;
} sections;
sections.ai = p.addSection("AI");
sections.physics = p.addSection("Physics");
sections.draw = p.addSection("Drawing");
sections.bufferSwap = p.addSection("Buffer swap");

// Enable profiling
p.enable();

// Mark sections in draw function
void MyApplication::drawEvent() {
    p.start();

    // ... misc stuff belonging to "Other" section

    p.start(sections.ai);

    // ... AI computation

    p.start(sections.physics);

    // ... physics simulation

    p.start(sections.draw);

    camera.draw(drawables);

    p.start(sections.bufferSwap);

    swapBuffers();

    // Count everything before next call to drawEvent() into "Other" section
    p.start();

    // Mark start of next frame
    p.nextFrame();
}

// Print statistics to debug output, showing how much time each section took
p.printStatistics();
@endcode

It's possible to start profiler only for certain parts of the code and then
stop it again using @ref stop(), if you are not interested in profiling the rest.

@todo Some unit testing
@todo More time intervals
*/
class MAGNUM_DEBUGTOOLS_EXPORT Profiler {
    public:
        /**
         * @brief Section ID
         *
         * @see @ref otherSection, @ref addSection(), @ref start(Section)
         */
        typedef UnsignedInt Section;

        /**
         * @brief Default section
         *
         * @see @ref start()
         */
        static const Section otherSection = 0;

        explicit Profiler(): _enabled(false), _measureDuration(60), _currentFrame(0), _frameCount(0), _sections{"Other"}, _currentSection(otherSection) {}

        /**
         * @brief Set measure duration
         *
         * Measured data are averaged through given frame count. Default value
         * is 60.
         * @attention This function cannot be called if profiling is enabled.
         */
        void setMeasureDuration(std::size_t frames);

        /**
         * @brief Add named section
         *
         * @attention This function cannot be called if profiling is enabled.
         * @see @ref otherSection, @ref start(Section), @ref stop()
         */
        Section addSection(const std::string& name);

        /**
         * @brief Whether profiling is enabled
         *
         * If the profiling is not enabled, calls to @ref start() and
         * @ref stop() have no effect.
         */
        bool isEnabled() { return _enabled; }

        /**
         * @brief Enable profiling
         *
         * Clears already mesaured data.
         * @see @ref disable(), @ref isEnabled()
         */
        void enable();

        /**
         * @brief Disable profiling
         *
         * @see @ref enable(), @ref isEnabled()
         */
        void disable();

        /**
         * @brief Start profiling of given named section
         *
         * If profiling is already running, current time is saved for previous
         * section.
         * @see @ref otherSection, @ref start(Section)
         */
        void start(Section section);

        /**
         * @brief Start profiling of "other" section
         *
         * Same as calling `start(DebugTools::Profiler::otherSection)`.
         * @note Does nothing if profiling is disabled.
         */
        void start() { start(otherSection); }

        /**
         * @brief Stop profiling
         *
         * Current time is saved for previous section.
         * @note Does nothing if profiling is disabled.
         */
        void stop();

        /**
         * @brief Save data from previous frame and advance to another
         *
         * Call at the end of each frame.
         * @note Does nothing if profiling is disabled.
         */
        void nextFrame();

        /**
         * @brief Print statistics
         *
         * Prints statistics about previous frame ordered by duration.
         * @note Does nothing if profiling is disabled.
         */
        void printStatistics();

    private:
        void save();

        bool _enabled;
        std::size_t _measureDuration, _currentFrame, _frameCount;
        std::vector<std::string> _sections;
        std::vector<std::chrono::high_resolution_clock::duration> _frameData;
        std::vector<std::chrono::high_resolution_clock::duration> _totalData;
        std::chrono::high_resolution_clock::time_point _previousTime;
        Section _currentSection;
};

}}

#endif
