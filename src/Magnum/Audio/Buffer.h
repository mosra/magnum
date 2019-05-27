#ifndef Magnum_Audio_Buffer_h
#define Magnum_Audio_Buffer_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2019 Guillaume Jacquemin <williamjcm@users.noreply.github.com>

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

#include <climits>
#include <utility>
#include <al.h>
#include <alc.h>
#include "MagnumExternal/OpenAL/extensions.h"

#include <Corrade/Containers/ArrayView.h>

#include "Magnum/Magnum.h"
#include "Magnum/Audio/Audio.h"
#include "Magnum/Audio/visibility.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>
#include "Magnum/Audio/BufferFormat.h"
#endif

namespace Magnum { namespace Audio {

/** @brief Sample buffer */
class MAGNUM_AUDIO_EXPORT Buffer {
    public:
        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @brief @copybrief BufferFormat
         * @deprecated Use @ref BufferFormat instead.
         */
        typedef CORRADE_DEPRECATED("use BufferFormat instead") BufferFormat Format;
        #endif

        /**
         * @brief Constructor
         *
         * Creates OpenAL buffer object.
         * @see @fn_al_keyword{GenBuffers}
         */
        explicit Buffer() { alGenBuffers(1, &_id); }

        /**
         * @brief Destructor
         *
         * Deletes OpenAL buffer object.
         * @see @fn_al_keyword{DeleteBuffers}
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
         * @see @fn_al_keyword{BufferData}
         */
        Buffer& setData(BufferFormat format, Containers::ArrayView<const void> data, ALsizei frequency) {
            alBufferData(_id, ALenum(format), data, data.size(), frequency);
            return *this;
        }

        /**
         * @brief Set buffer loop points
         * @param loopStart The loop's start point in samples
         * @param loopEnd   The loop's end point in samples
         * @return Reference to self (for method chaining)
         *
         * @requires_al_extension Extension @al_extension{SOFT,loop_points}
         */
        Buffer& setLoopPoints(Int loopStart, Int loopEnd);

        /**
         * @brief Set buffer to loop from the beginning until a certain point
         *
         * Equivalent to calling @ref setLoopPoints() with @p loopStart equal to
         * 0.
         * @param loopEnd   The loop's end point in samples
         * @return Reference to self (for method chaining)
         *
         * @requires_al_extension Extension @al_extension{SOFT,loop_points}
         */
        Buffer& setLoopUntil(Int loopEnd) {
            return setLoopPoints(0, loopEnd);
        }

        /**
         * @brief Set buffer to loop from the a certain point until the end
         *
         * Equivalent to calling @ref setLoopPoints() with @p loopEnd equal to
         * @p INT_MAX.
         * @param loopStart The loop's start point in samples
         * @return Reference to self (for method chaining)
         *
         * @requires_al_extension Extension @al_extension{SOFT,loop_points}
         */
        Buffer& setLoopSince(Int loopStart) {
            return setLoopPoints(loopStart, INT_MAX);
        }

        /**
         * @brief Resets the loop points
         *
         * Equivalent to calling @ref setLoopPoints() with @p loopStart equal to
         * 0, and @p loopEnd equal to @p INT_MAX.
         * @return Reference to self (for method chaining)
         *
         * @requires_al_extension Extension @al_extension{SOFT,loop_points}
         */
        Buffer& resetLoopPoints() {
            return setLoopPoints(0, INT_MAX);
        }

    private:
        ALuint _id;
};

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
