#ifndef Magnum_Audio_WavHeader_h
#define Magnum_Audio_WavHeader_h
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
 * @brief Struct @ref Magnum::Audio::WavHeader
 */

#include "Magnum/Types.h"

#define WAVE_FORMAT_PCM 0x0001
#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#define WAVE_FORMAT_ALAW 0x0006
#define WAVE_FORMAT_MULAW 0x0007
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE

namespace Magnum { namespace Audio {

#pragma pack(1)
/** @brief RIFF chunk */
struct RiffChunk {
    char chunkId[4];            /**< @brief chunk name (4 characters)
    UnsignedInt chunkSize;      /**< @brief size of chunk (does not include chunk header)
};
#pragma pack()

#pragma pack(1)
/** @brief WAV file header */
struct WavHeaderChunk {
    RiffChunk chunk;
    char format[4];                 /**< @brief `WAVE` characters */
};
#pragma pack()

#pragma pack(1)
/** @brief WAV 'fmt' header */
struct WavFormatChunk {
    RiffChunk chunk;
    UnsignedShort audioFormat;      /**< @brief 1 = PCM */
    UnsignedShort numChannels;      /**< @brief 1 = Mono, 2 = Stereo */
    UnsignedInt sampleRate;         /**< @brief Sample rate in Hz */
    UnsignedInt byteRate;           /**< @brief Bytes per second */
    UnsignedShort blockAlign;       /**< @brief Bytes per sample (all channels) */
    UnsignedShort bitsPerSample;    /**< @brief Bits per sample (one channel) */
};
#pragma pack()

#pragma pack(1)
/** @brief WAV 'fact' header */
struct WavFactChunk {
    RiffChunk chunk;
    UnsignedInt dwSampleLength;     /**< @brief sample length */
};
#pragma pack()

#pragma pack(1)
/** @brief WAV 'data' header */
struct WavDataChunk {
    RiffChunk chunk;
};
#pragma pack()

static_assert(sizeof(WavHeaderChunk) + sizeof(WavFormatChunk) + sizeof(WavDataChunk) == 44, "WavHeader size is not 44 bytes");

}}

#endif
