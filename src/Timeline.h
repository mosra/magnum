#ifndef Magnum_Timeline_h
#define Magnum_Timeline_h
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

#include <chrono>

#include "Magnum.h"

#include "magnumVisibility.h"

/** @file
 * @brief Class Magnum::Timeline
 */

namespace Magnum {

/** @brief %Timeline */
class MAGNUM_EXPORT Timeline {
    public:
        /**
         * @brief Constructor
         *
         * Constructs stopped timeline.
         * @see start()
         */
        inline constexpr Timeline(): _minimalFrameTime(0), _previousFrameDuration(0), running(false) {}

        /** @brief Minimal frame time (in seconds) */
        GLfloat minimalFrameTime() const { return _minimalFrameTime; }

        /**
         * @brief Set minimal frame time
         *
         * @see nextFrame()
         */
        void setMinimalFrameTime(GLfloat seconds) {
            _minimalFrameTime = seconds;
        }

        /**
         * @brief Start timeline
         *
         * Sets previous frame duration to `0`.
         * @see stop(), previousFrameDuration()
         */
        void start();

        /**
         * @brief Stop timeline
         */
        void stop();

        /**
         * @brief Advance to next frame
         *
         * If current frame time is smaller than minimal frame time, pauses
         * the execution for remaining time.
         * @note This function does nothing if the timeline is stopped.
         * @see setMinimalFrameTime()
         */
        void nextFrame();

        /** @brief Duration of previous frame */
        inline constexpr GLfloat previousFrameDuration() const {
            return _previousFrameDuration;
        }

    private:
        std::chrono::high_resolution_clock::time_point previousFrameTime;
        GLfloat _minimalFrameTime;
        GLfloat _previousFrameDuration;

        bool running;
};

}

#endif
