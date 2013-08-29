#ifndef Magnum_Audio_Renderer_h
#define Magnum_Audio_Renderer_h
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

/** @file
 * @brief Class Magnum::Audio::Renderer
 */

#include <al.h>

#include "Math/Vector3.h"
#include "Magnum.h"
#include "Audio/magnumAudioVisibility.h"

namespace Magnum { namespace Audio {

/** @brief Global renderer configuration */
class Renderer {
    public:
        Renderer() = delete;

        /**
         * @brief Error status
         *
         * @see error()
         */
        enum class Error: ALenum {
            NoError = AL_NO_ERROR,          /**< No error occured */
            InvalidName = AL_INVALID_NAME,  /**< Invalid name parameter */
            InvalidEnum = AL_INVALID_ENUM,  /**< Invalid enum parameter */
            InvalidValue = AL_INVALID_VALUE, /**< Invalid enum value parameter */
            InvalidOperation = AL_INVALID_OPERATION, /**< Illegal call */
            OutOfMemory = AL_OUT_OF_MEMORY  /**< Unable to allocate memory */
        };

        /** @brief Error status */
        static Error error() { return Error(alGetError()); }

        /** @{ @name Listener positioning */

        /**
         * @brief Set listener position
         *
         * Default is `{0.0f, 0.0f, 0.0f}`.
         * @see @fn_al{Listenerfv} with @def_al{POSITION}
         */
        static void setListenerPosition(const Vector3& position) {
            alListenerfv(AL_POSITION, position.data());
        }

        /** @overload
         * @see @fn_al{Listeneriv} with @def_al{POSITION}
         */
        static void setListenerPosition(const Vector3i& position) {
            alListeneriv(AL_POSITION, position.data());
        }

        /**
         * @brief Set listener orientation
         *
         * The values must be linearly independent and don't need to be
         * normalized. Default is -Z and +Y.
         * @see @fn_al{Listenerfv} with @def_al{ORIENTATION}
         */
        static void setListenerOrientation(const Vector3& forward, const Vector3& up);

        /** @overload
         * @see @fn_al{Listeneriv} with @def_al{ORIENTATION}
         */
        static void setListenerOrientation(const Vector3i& forward, const Vector3i& up);

        /**
         * @brief Set listener velocity
         *
         * Default is `{0.0f, 0.0f, 0.0f}`.
         * @see @fn_al{Listenerfv} with @def_al{VELOCITY}
         */
        static void setListenerVelocity(const Vector3& velocity) {
            alListenerfv(AL_VELOCITY, velocity.data());
        }

        /** @overload
         * @see @fn_al{Listeneriv} with @def_al{VELOCITY}
         */
        static void setListenerVelocity(const Vector3i& velocity) {
            alListeneriv(AL_VELOCITY, velocity.data());
        }

        /*@}*/

        /** @{ @name Global behavior */

        /**
         * @brief Distance model
         *
         * @see setDistanceModel()
         */
        enum class DistanceModel: ALenum {
            /** No distance attenuation calculation */
            None = AL_NONE,

            /** Inverse distance */
            Inverse = AL_INVERSE_DISTANCE,

            /** Inverse distance, clamped */
            InverseClamped = AL_INVERSE_DISTANCE_CLAMPED,

            /** Linear distance */
            Linear = AL_LINEAR_DISTANCE,

            /** Linear distance, clamped */
            LinearClamped = AL_LINEAR_DISTANCE_CLAMPED,

            /** Exponential distance */
            Exponent = AL_EXPONENT_DISTANCE,

            /** Exponential distance, clamped */
            ExponentClamped = AL_EXPONENT_DISTANCE_CLAMPED
        };

        /**
         * @brief Set listener gain
         *
         * Default is `1.0f`, which means that the sound is unattenuated.
         * If set to `0.0f`, all sound is muted.
         * @see @fn_al{Listenerf} with @def_al{GAIN}
         */
        static void setListenerGain(Float gain) {
            alListenerf(AL_GAIN, gain);
        }

        /**
         * @brief Set Doppler factor
         *
         * Default is `1.0f`. If set to `0.0f`, the effect is disabled.
         * @see @ref setSpeedOfSound(), @fn_al{DopplerFactor}
         */
        static void setDopplerFactor(Float factor) {
            alDopplerFactor(factor);
        }

        /**
         * @brief Set speed of sound
         *
         * Default is `343.3f` (meters per second).
         * @see @ref setDopplerFactor(), @fn_al{SpeedOfSound}
         */
        static void setSpeedOfSound(Float speed) {
            alSpeedOfSound(speed);
        }

        /**
         * @brief Set distance model
         *
         * Default is @ref DistanceModel::InverseClamped.
         * @see @fn_al{DistanceModel}
         */
        static void setDistanceModel(DistanceModel model) {
            alDistanceModel(ALenum(model));
        }

        /*@}*/
};

/** @debugoperator{Magnum::Audio::Renderer} */
Debug MAGNUM_AUDIO_EXPORT operator<<(Debug debug, Renderer::Error value);

inline void Renderer::setListenerOrientation(const Vector3& forward, const Vector3& up) {
    const Vector3 data[] = {forward, up};
    alListenerfv(AL_ORIENTATION, data[0].data());
}

inline void Renderer::setListenerOrientation(const Vector3i& forward, const Vector3i& up) {
    const Vector3i data[] = {forward, up};
    alListeneriv(AL_ORIENTATION, data[0].data());
}

}}

#endif
