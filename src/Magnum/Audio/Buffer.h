#ifndef Magnum_Audio_Buffer_h
#define Magnum_Audio_Buffer_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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
 * @brief Class @ref Magnum::Audio::Buffer
 */

#include <utility>
#include <al.h>
#include <alc.h>
#include "MagnumExternal/OpenAL/extensions.h"

#include <Corrade/Containers/ArrayView.h>

#include "Magnum/Magnum.h"
#include "Magnum/Audio/visibility.h"

namespace Magnum { namespace Audio {

/** @brief Sample buffer */
class Buffer {
    public:
        /**
         * @brief Sample format
         *
         * @note Multi-channel format is played without 3D spatialization
         *      (useful for background music)
         * @see @ref setData()
         */
        enum class Format: ALenum {
            Mono8 = AL_FORMAT_MONO8,        /**< 8-bit unsigned mono */
            Mono16 = AL_FORMAT_MONO16,      /**< 16-bit signed mono */
            Stereo8 = AL_FORMAT_STEREO8,    /**< 8-bit interleaved unsigned stereo */
            Stereo16 = AL_FORMAT_STEREO16,  /**< 16-bit interleaved signed stereo */

            MonoALaw = AL_FORMAT_MONO_ALAW_EXT,
            StereoALaw = AL_FORMAT_STEREO_ALAW_EXT,

            MonoMuLaw = AL_FORMAT_MONO_MULAW_EXT,
            StereoMuLaw = AL_FORMAT_STEREO_MULAW_EXT,

            /**
             * @brief 32-bit floating-point mono
             *
             * @requires_al_extension Extension @al_extension{EXT,float32}
             */
            MonoFloat = AL_FORMAT_MONO_FLOAT32,

            /**
             * @brief 32-bit interleaved floating-point stereo
             *
             * @requires_al_extension Extension @al_extension{EXT,float32}
             */
            StereoFloat = AL_FORMAT_STEREO_FLOAT32,

            /**
             * @brief 64-bit floating-point mono
             *
             * @requires_al_extension Extension @al_extension{EXT,double}
             */
            MonoDouble = AL_FORMAT_MONO_DOUBLE_EXT,

            /**
             * @brief 64-bit interleaved floating-point stereo
             *
             * @requires_al_extension Extension @al_extension{EXT,double}
             */
            StereoDouble = AL_FORMAT_STEREO_DOUBLE_EXT,

            Quad8 = AL_FORMAT_QUAD8,
            Quad16 = AL_FORMAT_QUAD16,
            Quad32 = AL_FORMAT_QUAD32,

            Rear8 = AL_FORMAT_REAR8,
            Rear16 = AL_FORMAT_REAR16,
            Rear32 = AL_FORMAT_REAR32,

            Five_1Chnl8 = AL_FORMAT_51CHN8,
            Five_1Chnl16 = AL_FORMAT_51CHN16,
            Five_1Chnl32 = AL_FORMAT_51CHN32,

            Six_1Chnl8 = AL_FORMAT_61CHN8,
            Six_1Chnl16 = AL_FORMAT_61CHN16,
            Six_1Chnl32 = AL_FORMAT_61CHN32,

            Seven_1Chnl8 = AL_FORMAT_71CHN8,
            Seven_1Chnl16 = AL_FORMAT_71CHN16,
            Seven_1Chnl32 = AL_FORMAT_71CHN32
        };

        /**
         * @brief Constructor
         *
         * Creates OpenAL buffer object.
         * @see @fn_al{GenBuffers}
         */
        explicit Buffer() { alGenBuffers(1, &_id); }

        /**
         * @brief Destructor
         *
         * Deletes OpenAL buffer object.
         * @see @fn_al{DeleteBuffers}
         */
        ~Buffer() { if(_id) alDeleteBuffers(1, &_id); }

        /** @brief Copying is not allowed */
        Buffer(const Buffer&) = delete;

        /** @brief Move constructor */
        Buffer(Buffer&& other);

        /** @brief Copying is not allowed */
        Buffer& operator=(const Buffer&) = delete;

        /** @brief Move assignment */
        Buffer& operator=(Buffer&& other);

        /** @brief OpenAL buffer ID */
        ALuint id() const { return _id; }

        /**
         * @brief Set buffer data
         * @param format    Sample format
         * @param data      Data
         * @param frequency Frequency
         * @return Reference to self (for method chaining)
         *
         * @see @fn_al{BufferData}
         */
        Buffer& setData(Format format, Containers::ArrayView<const void> data, ALsizei frequency) {
            alBufferData(_id, ALenum(format), data, data.size(), frequency);
            return *this;
        }

    private:
        ALuint _id;
};

/** @debugoperatorclassenum{Magnum::Audio::Buffer,Magnum::Audio::Buffer::Format} */
MAGNUM_AUDIO_EXPORT Debug& operator<<(Debug& debug, Buffer::Format value);

inline Buffer::Buffer(Buffer&& other): _id(other._id) {
    other._id = 0;
}

inline Buffer& Buffer::operator=(Buffer&& other) {
    using std::swap;
    swap(_id, other._id);
    return *this;
}

}}

#endif
