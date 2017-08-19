#ifndef Magnum_Timeline_h
#define Magnum_Timeline_h
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
 * @brief Class @ref Magnum::Timeline
 */

#include <chrono>

#include "Magnum/Types.h"
#include "Magnum/visibility.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>
#endif

namespace Magnum {

/**
@brief Timeline

Keeps track of time delta between frames. Can be used as source for animation
speed computations.

## Basic usage

Construct the timeline on initialization so the instance is available for
whole lifetime of the application. Call @ref start() before first draw event is
performed, after everything is properly initialized.

@note When timeline is started, it immediately starts measuring frame time.
    Be prepared that time of first frame will be much longer than time of
    following frames. It mainly depends on where you called @ref start() in
    your initialization routine, but can be also affected by driver- and
    GPU-specific lazy texture binding, shader recompilations etc.

In your draw event implementation don't forget to call @ref nextFrame() after
buffer swap. You can use @ref previousFrameDuration() to compute animation
speed. To limit application framerate you can use
@ref Platform::Sdl2Application::setSwapInterval() "Platform::*Application::setSwapInterval()" or
@ref Platform::Sdl2Application::setMinimalLoopPeriod() "Platform::*Application::setMinimalLoopPeriod()".
Note that on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten" the framerate is
governed by browser and you can't do anything about it.

Example usage:
@code
MyApplication::MyApplication(const Parameters& parameters): Platform::Application(parameters) {
    // Initialization ...

    // Enable VSync or set minimal loop period for the application, if
    // needed/applicable ...

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
         * @see @ref start()
         */
        explicit Timeline():
            #ifdef MAGNUM_BUILD_DEPRECATED
            _minimalFrameTime(0),
            #endif
            _previousFrameDuration(0), running(false) {}

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Minimal frame time (in seconds)
         * @deprecated Replaced with @ref Platform::Sdl2Application::setSwapInterval() "Platform::*Application::setSwapInterval()" and
         *      @ref Platform::Sdl2Application::setMinimalLoopPeriod() "Platform::*Application::setMinimalLoopPeriod()".
         */
        CORRADE_DEPRECATED("replaced with Platfomr::*Application functionality") Float minimalFrameTime() const { return _minimalFrameTime; }

        /**
         * @brief Set minimal frame time
         * @return Reference to self (for method chaining)
         *
         * Default value is `0.0f`. Cannot be used on some platforms where
         * blocking the main loop is not allowed (such as
         * @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten").
         * @deprecated Use @ref Platform::Sdl2Application::setSwapInterval() "Platform::*Application::setSwapInterval()" or
         *      @ref Platform::Sdl2Application::setMinimalLoopPeriod() "Platform::*Application::setMinimalLoopPeriod()"
         *      instead.
         */
        CORRADE_DEPRECATED("use Platfomr::*Application::setMinimalLoopPeriod() instead") Timeline& setMinimalFrameTime(Float seconds) {
            _minimalFrameTime = seconds;
            return *this;
        }
        #endif

        /**
         * @brief Start timeline
         *
         * Sets previous frame time and duration to `0`.
         * @see @ref stop(), @ref previousFrameDuration()
         */
        void start();

        /**
         * @brief Stop timeline
         *
         * @see @ref start(), @ref nextFrame()
         */
        void stop();

        /**
         * @brief Advance to next frame
         *
         * @note This function does nothing if the timeline is stopped.
         * @see @ref stop()
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
        Float previousFrameDuration() const { return _previousFrameDuration; }

    private:
        std::chrono::high_resolution_clock::time_point _startTime;
        std::chrono::high_resolution_clock::time_point _previousFrameTime;
        #ifdef MAGNUM_BUILD_DEPRECATED
        Float _minimalFrameTime;
        #endif
        Float _previousFrameDuration;

        bool running;
};

}

#endif
