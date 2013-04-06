#ifndef Magnum_Timeline_h
#define Magnum_Timeline_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include <chrono>
#include <corradeCompatibility.h>

#include "Types.h"

#include "magnumVisibility.h"

/** @file
 * @brief Class Magnum::Timeline
 */

namespace Magnum {

/**
@brief %Timeline

Keeps track of time delta between frames and allows FPS limiting. Can be used
as source for animation speed computations.

@section Timeline-usage Basic usage

Construct the timeline on initialization so the instance is available for
whole lifetime of the application. Call start() before first draw event is
performed, after everything is properly initialized.

@note When timeline is started, it immediately starts measuring frame time.
    Be prepared that time of first frame will be much longer than time of
    following frames. It mainly depends on where you called start() in your
    initialization routine, but can be also affected by driver- and
    GPU-specific lazy texture binding, shader recompilations etc.

In your draw event implementation don't forget to call nextFrame() after
buffer swap. You can use previousFrameDuration() to compute animation speed.

Example usage:
@code
MyApplication::MyApplication(const Parameters& parameters): Platform::Application(parameters) {
    // Initialization ...

    timeline.setMinimalFrameTime(1/120.0f); // 120 FPS at max
    timeline.start();
}

void MyApplication::drawEvent() {
    // Distance of object travelling at speed of 15 units per second
    Float distance = 15.0f*timeline.previousFrameDuration();

    // Move object, draw ...

    swapBuffers();
    redraw();
    timeline.nextFrame();
}
@endcode
*/
class MAGNUM_EXPORT Timeline {
    public:
        /**
         * @brief Constructor
         *
         * Creates stopped timeline.
         * @see start()
         */
        inline constexpr explicit Timeline(): _minimalFrameTime(0), _previousFrameDuration(0), running(false) {}

        /** @brief Minimal frame time (in seconds) */
        inline constexpr Float minimalFrameTime() const {
            return _minimalFrameTime;
        }

        /**
         * @brief Set minimal frame time
         *
         * Default value is 0.
         * @see nextFrame()
         */
        inline void setMinimalFrameTime(Float seconds) {
            _minimalFrameTime = seconds;
        }

        /**
         * @brief Start timeline
         *
         * Sets previous frame time and duration to `0`.
         * @see stop(), previousFrameDuration()
         */
        void start();

        /**
         * @brief Stop timeline
         *
         * @see start(), nextFrame()
         */
        void stop();

        /**
         * @brief Advance to next frame
         *
         * If current frame time is smaller than minimal frame time, pauses
         * the execution for remaining time.
         * @note This function does nothing if the timeline is stopped.
         * @see setMinimalFrameTime(), stop()
         */
        void nextFrame();

        /**
         * @brief Time at previous frame (in seconds)
         *
         * Returns time elapsed since start() was called. If the timeline is
         * stopped, the function returns `0.0f`.
         */
        Float previousFrameTime() const;

        /**
         * @brief Duration of previous frame (in seconds)
         *
         * If the timeline is stopped, the function returns `0.0f`.
         */
        inline constexpr Float previousFrameDuration() const {
            return _previousFrameDuration;
        }

    private:
        std::chrono::high_resolution_clock::time_point _startTime;
        std::chrono::high_resolution_clock::time_point _previousFrameTime;
        Float _minimalFrameTime;
        Float _previousFrameDuration;

        bool running;
};

}

#endif
