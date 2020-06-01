#ifndef Magnum_Audio_Renderer_h
#define Magnum_Audio_Renderer_h
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

/** @file
 * @brief Class @ref Magnum::Audio::Renderer
 */

#include <array>
#include <al.h>

#include "Magnum/Magnum.h"
#include "Magnum/Audio/Context.h"
#include "Magnum/Audio/visibility.h"
#include "Magnum/Math/Vector3.h"

namespace Magnum { namespace Audio {

/** @brief Global renderer configuration */
class Renderer {
    public:
        Renderer() = delete;

        /**
         * @brief Error status
         *
         * @see @ref error()
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
         * @brief Listener position
         * @see @ref setListenerPosition(), @fn_al_keyword{GetListenerfv} with
         *      @def_al{POSITION}
         */
        static Vector3 listenerPosition() {
            Vector3 position;
            alGetListenerfv(AL_POSITION, position.data());
            return position;
        }

        /**
         * @brief Set listener position
         *
         * Default is @cpp {0.0f, 0.0f, 0.0f} @ce.
         * @see @fn_al_keyword{Listenerfv} with @def_al{POSITION}
         */
        static void setListenerPosition(const Vector3& position) {
            alListenerfv(AL_POSITION, position.data());
        }

        /** @overload
         * @see @fn_al_keyword{Listeneriv} with @def_al{POSITION}
         */
        static void setListenerPosition(const Vector3i& position) {
            alListeneriv(AL_POSITION, position.data());
        }

        /**
         * @brief Listener orientation
         *
         * The returned array consists of forward (index 0) and up (index 1) vectors.
         * @see @ref setListenerOrientation(), @fn_al_keyword{GetListenerfv}
         *      with @def_al{ORIENTATION}
         */
        static std::array<Vector3, 2> listenerOrientation() {
            std::array<Vector3, 2> data;
            alGetListenerfv(AL_ORIENTATION, data[0].data());
            return data;
        }

        /**
         * @brief Set listener orientation
         *
         * The values must be linearly independent and don't need to be
         * normalized. Default is -Z and +Y.
         * @see @fn_al_keyword{Listenerfv} with @def_al{ORIENTATION}
         */
        static void setListenerOrientation(const Vector3& forward, const Vector3& up) {
            const Vector3 data[] = {forward, up};
            alListenerfv(AL_ORIENTATION, data[0].data());
        }

        /** @overload
         * @see @fn_al_keyword{Listeneriv} with @def_al{ORIENTATION}
         */
        static void setListenerOrientation(const Vector3i& forward, const Vector3i& up) {
            const Vector3i data[] = {forward, up};
            alListeneriv(AL_ORIENTATION, data[0].data());
        }

        /**
         * @brief Listener position
         * @see @ref setListenerVelocity(), @fn_al_keyword{GetListenerfv} with
         *      @def_al{VELOCITY}
         */
        static Vector3 listenerVelocity() {
            Vector3 velocity;
            alGetListenerfv(AL_VELOCITY, velocity.data());
            return velocity;
        }

        /**
         * @brief Set listener velocity
         *
         * Default is @cpp {0.0f, 0.0f, 0.0f} @ce.
         * @see @fn_al_keyword{Listenerfv} with @def_al{VELOCITY}
         */
        static void setListenerVelocity(const Vector3& velocity) {
            alListenerfv(AL_VELOCITY, velocity.data());
        }

        /** @overload
         * @see @fn_al_keyword{Listeneriv} with @def_al{VELOCITY}
         */
        static void setListenerVelocity(const Vector3i& velocity) {
            alListeneriv(AL_VELOCITY, velocity.data());
        }

        /* Since 1.8.17, the original short-hand group closing doesn't work
           anymore. FFS. */
        /**
         * @}
         */

        /** @{ @name Global behavior */

        /**
         * @brief Distance model
         *
         * @see @ref setDistanceModel()
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
         * @brief Listener gain
         * @see @ref setListenerGain(), @fn_al_keyword{GetListenerf} with
         *      @def_al{GAIN}
         */
        static Float listenerGain() {
            Float gain;
            alGetListenerf(AL_GAIN, &gain);
            return gain;
        }

        /**
         * @brief Set listener gain
         *
         * Default is @cpp 1.0f @ce, which means that the sound is
         * unattenuated. If set to @cpp 0.0f @ce, all sound is muted.
         * @see @fn_al_keyword{Listenerf} with @def_al{GAIN}
         */
        static void setListenerGain(Float gain) {
            alListenerf(AL_GAIN, gain);
        }

        /**
         * @brief Doppler factor
         * @see @ref setDopplerFactor(), @fn_al{GetFloat} with
         *      @def_al_keyword{DOPPLER_FACTOR}
         */
        static Float dopplerFactor() {
            return alGetFloat(AL_DOPPLER_FACTOR);
        }

        /**
         * @brief Set Doppler factor
         *
         * Default is @cpp 1.0f @ce. If set to @cpp 0.0f @ce, the effect is
         * disabled.
         * @see @ref setSpeedOfSound(), @fn_al_keyword{DopplerFactor}
         */
        static void setDopplerFactor(Float factor) {
            alDopplerFactor(factor);
        }

        /**
         * @brief Speed of sound
         * @see @ref setSpeedOfSound(), @fn_al{GetFloat} with
         *      @def_al_keyword{SPEED_OF_SOUND}
         */
        static Float speedOfSound() {
            return alGetFloat(AL_SPEED_OF_SOUND);
        }

        /**
         * @brief Set speed of sound
         *
         * Default is @cpp 343.3f @ce (meters per second).
         * @see @ref setDopplerFactor(), @fn_al_keyword{SpeedOfSound}
         */
        static void setSpeedOfSound(Float speed) {
            alSpeedOfSound(speed);
        }

        /**
         * @brief Distance model
         * @see @ref setDistanceModel(), @fn_al{GetInteger} with
         *      @def_al_keyword{DISTANCE_MODEL}
         */
        static DistanceModel distanceModel() {
            return DistanceModel(alGetInteger(AL_DISTANCE_MODEL));
        }

        /**
         * @brief Set distance model
         *
         * Default is @ref DistanceModel "DistanceModel::InverseClamped".
         * @see @fn_al_keyword{DistanceModel}
         * @todoc why doxygen can't link to DistanceModel::InverseClamped?!
         */
        static void setDistanceModel(DistanceModel model) {
            alDistanceModel(ALenum(model));
        }

        /* Since 1.8.17, the original short-hand group closing doesn't work
           anymore. FFS. */
        /**
         * @}
         */
};

/** @debugoperatorclassenum{Renderer,Renderer::Error} */
MAGNUM_AUDIO_EXPORT Debug& operator<<(Debug& debug, Renderer::Error value);

/** @debugoperatorclassenum{Renderer,Renderer::DistanceModel} */
MAGNUM_AUDIO_EXPORT Debug& operator<<(Debug& debug, Renderer::DistanceModel value);

}}

#endif
