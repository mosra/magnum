#ifndef Magnum_Audio_Buffer_h
#define Magnum_Audio_Buffer_h
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

            /**
             * @brief 8-bit mono [A-Law Compressed Sound Format](https://en.wikipedia.org/wiki/A-law_algorithm)
             *
             * @requires_al_extension Extension @al_extension{EXT,ALAW}
             */
            MonoALaw = AL_FORMAT_MONO_ALAW_EXT,

            /**
             * @brief 8-bit interleaved stereo [A-Law Compressed Sound Format](https://en.wikipedia.org/wiki/A-law_algorithm)
             *
             * @requires_al_extension Extension @al_extension{EXT,ALAW}
             */
            StereoALaw = AL_FORMAT_STEREO_ALAW_EXT,

            /**
             * @brief 8-bit mono [μ-Law Compressed Sound Format](https://en.wikipedia.org/wiki/Μ-law_algorithm)
             *
             * @requires_al_extension Extension @al_extension{EXT,MULAW}
             */
            MonoMuLaw = AL_FORMAT_MONO_MULAW_EXT,

            /**
             * @brief 8-bit interleaved [μ-Law Compressed Sound Format](https://en.wikipedia.org/wiki/Μ-law_algorithm)
             *
             * @requires_al_extension Extension @al_extension{EXT,MULAW}
             */
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

            /**
             * @brief 8-bit unsigned quadrophonic
             *
             * @requires_al_extension Extension @al_extension{EXT,MCFORMATS}
             */
            Quad8 = AL_FORMAT_QUAD8,

            /**
             * @brief 16-bit signed quadrophonic
             *
             * @requires_al_extension Extension @al_extension{EXT,MCFORMATS}
             */
            Quad16 = AL_FORMAT_QUAD16,

            /**
             * @brief 32-bit interleaved floating-point quadrophonic
             *
             * @requires_al_extension Extension @al_extension{EXT,MCFORMATS}
             */
            Quad32 = AL_FORMAT_QUAD32,

            /**
             * @brief 8-bit unsigned rear
             *
             * @requires_al_extension Extension @al_extension{EXT,MCFORMATS}
             */
            Rear8 = AL_FORMAT_REAR8,

            /**
             * @brief 16-bit signed rear
             *
             * @requires_al_extension Extension @al_extension{EXT,MCFORMATS}
             */
            Rear16 = AL_FORMAT_REAR16,

            /**
             * @brief 32-bit interleaved floating-point rear
             *
             * @requires_al_extension Extension @al_extension{EXT,MCFORMATS}
             */
            Rear32 = AL_FORMAT_REAR32,

            /**
             * @brief 8-bit unsigned 5.1 surround
             *
             * @requires_al_extension Extension @al_extension{EXT,MCFORMATS}
             */
            Surround51Channel8 = AL_FORMAT_51CHN8,

            /**
             * @brief 16-bit signed 5.1 surround
             *
             * @requires_al_extension Extension @al_extension{EXT,MCFORMATS}
             */
            Surround51Channel16 = AL_FORMAT_51CHN16,

            /**
             * @brief 32-bit interleaved floating-point 5.1 surround
             *
             * @requires_al_extension Extension @al_extension{EXT,MCFORMATS}
             */
            Surround51Channel32 = AL_FORMAT_51CHN32,

            /**
             * @brief 8-bit unsigned 6.1 surround
             *
             * @requires_al_extension Extension @al_extension{EXT,MCFORMATS}
             */
            Surround61Channel8 = AL_FORMAT_61CHN8,

            /**
             * @brief 16-bit signed 6.1 surround
             *
             * @requires_al_extension Extension @al_extension{EXT,MCFORMATS}
             */
            Surround61Channel16 = AL_FORMAT_61CHN16,

            /**
             * @brief 32-bit interleaved floating-point 6.1 surround
             *
             * @requires_al_extension Extension @al_extension{EXT,MCFORMATS}
             */
            Surround61Channel32 = AL_FORMAT_61CHN32,

            /**
             * @brief 8-bit unsigned 7.1 surround
             *
             * @requires_al_extension Extension @al_extension{EXT,MCFORMATS}
             */
            Surround71Channel8 = AL_FORMAT_71CHN8,

            /**
             * @brief 16-bit signed 7.1 surround
             *
             * @requires_al_extension Extension @al_extension{EXT,MCFORMATS}
             */
            Surround71Channel16 = AL_FORMAT_71CHN16,

            /**
             * @brief 32-bit interleaved floating-point 7.1 surround
             *
             * @requires_al_extension Extension @al_extension{EXT,MCFORMATS}
             */
            Surround71Channel32 = AL_FORMAT_71CHN32
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
