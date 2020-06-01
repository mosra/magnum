#ifndef Magnum_Audio_WavHeader_h
#define Magnum_Audio_WavHeader_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2016 Alice Margatroid <loveoverwhelming@gmail.com>

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

#include "Magnum/Magnum.h"

namespace Magnum { namespace Audio { namespace Implementation {

/* WAV audio format */
enum class WavAudioFormat: UnsignedShort {
    Unknown = 0x0000,       /* Unknown */
    Pcm = 0x0001,           /* PCM */
    AdPcm = 0x0002,         /* Adaptive Differential PCM */
    IeeeFloat = 0x0003,     /* IEEE Float */
    ALaw = 0x0006,          /* A-Law */
    MuLaw = 0x0007,         /* μ-Law */
    Extensible = 0xfffe     /* Extensible */
};

#pragma pack(1)
/* RIFF chunk */
struct RiffChunk {
    char chunkId[4];        /* chunk name (4 characters) */
    UnsignedInt chunkSize;  /* size of chunk (does not include chunk header) */
};
#pragma pack()

#pragma pack(1)
/* WAV file header */
struct WavHeaderChunk {
    RiffChunk chunk;        /* Starting RIFF chunk */
    char format[4];         /* `WAVE` characters */
};
#pragma pack()

#pragma pack(1)
/* WAV 'fmt' header */
struct WavFormatChunk {
    /* GCC 4.8 needs this, otherwise it can't store this in an Optional. It
       also can't be just =default, otherwise it'll still try to construct the
       members when using {} */
    WavFormatChunk() {}

    RiffChunk chunk;                /* Starting RIFF chunk */
    WavAudioFormat audioFormat;     /* Audio format */
    UnsignedShort numChannels;      /* 1 = Mono, 2 = Stereo */
    UnsignedInt sampleRate;         /* Sample rate in Hz */
    UnsignedInt byteRate;           /* Bytes per second */
    UnsignedShort blockAlign;       /* Bytes per sample (all channels) */
    UnsignedShort bitsPerSample;    /* Bits per sample (one channel) */
};
#pragma pack()

Debug& operator<<(Debug& debug, WavAudioFormat value);

}}}

#endif
