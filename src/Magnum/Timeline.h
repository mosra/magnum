#ifndef Magnum_Timeline_h
#define Magnum_Timeline_h
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

/** @file
 * @brief Class @ref Magnum::Timeline
 */

#include <chrono>

#include "Magnum/Types.h"
#include "Magnum/visibility.h"

namespace Magnum {

/**
@brief Timeline

Keeps track of time delta between frames. Can be used for advancing animation
playback.

@section Timeline-usage Basic usage

Construct the timeline on initialization so the instance is available for the
whole lifetime of the application. Call @ref start() after the application
state is fully initialized and before the first draw event is performed.

@note When the timeline is started, it immediately starts measuring frame time.
    Be prepared that time of the first frame may be much longer than time of
    the following frames. It mainly depends on where you called @ref start() in
    your initialization routine, but can be also affected by various
    driver-specific operations that are done lazily during the first frame.

In your draw event implementation don't forget to call @ref nextFrame() after
buffer swap. You can use @ref previousFrameDuration() to compute animation
speed. To limit application framerate you can use
@ref Platform::Sdl2Application::setSwapInterval() "Platform::*Application::setSwapInterval()" or
@ref Platform::Sdl2Application::setMinimalLoopPeriod() "Platform::*Application::setMinimalLoopPeriod()".
Note that on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten" the framerate is
governed by the browser and you can't do anything about it.

Example usage:

@snippet Magnum-application.cpp Timeline-usage

Apart from directly using the returned time values, the @ref Timeline can also
be used together with @ref Animation::Player for a more controlled behavior. In
that case, it's recommended to never call @ref Timeline::stop() but control the
player start/pause/stop state instead. See @ref Animation::Player documentation
for more information.
*/
class MAGNUM_EXPORT Timeline {
    public:
        /**
         * @brief Constructor
         *
         * Creates a stopped timeline.
         * @see @ref start()
         */
        explicit Timeline() = default;

        /**
         * @brief Start the timeline
         *
         * Sets previous frame time and duration to @cpp 0.0f @ce.
         * @see @ref stop(), @ref previousFrameDuration()
         */
        void start();

        /**
         * @brief Stop the timeline
         *
         * @see @ref start(), @ref nextFrame()
         */
        void stop();

        /**
         * @brief Advance to next frame
         *
         * Does nothing if the timeline is stopped.
         * @see @ref stop()
         */
        void nextFrame();

        /**
         * @brief Time at previous frame in seconds
         *
         * Returns time elapsed since @ref start() was called. If the timeline
         * is stopped, the function returns @cpp 0.0f @ce.
         * @see @ref currentFrameTime()
         */
        Float previousFrameTime() const;

        /**
         * @brief Duration of previous frame in seconds
         *
         * Return time measured between last two @ref nextFrame() calls, or
         * between @ref start() and @ref nextFrame(), if the previous frame
         * was the first. If the timeline is stopped, the function returns
         * @cpp 0.0f @ce.
         * @đee @ref currentFrameDuration()
         */
        Float previousFrameDuration() const { return _previousFrameDuration; }

        /**
         * @brief Current time in seconds
         * @m_since_latest
         *
         * Returns time elapsed since @ref start() was called. Never smaller
         * than @ref previousFrameTime(). If the timeline  is stopped, the
         * function returns @cpp 0.0f @ce.
         */
        Float currentFrameTime() const;

        /**
         * @brief Time since the last frame in seconds
         * @m_since_latest
         *
         * Returns time elapsed since @ref start() or @ref nextFrame() was
         * called, whichever happened last. Compared to
         * @ref previousFrameDuration() the returned value is different every
         * time. If the timeline is stopped, the function returns
         * @cpp 0.0f @ce.
         */
        Float currentFrameDuration() const;

    private:
        std::chrono::high_resolution_clock::time_point _startTime{};
        std::chrono::high_resolution_clock::time_point _previousFrameTime{};
        Float _previousFrameDuration{};

        bool _running = false;
};

}

#endif
