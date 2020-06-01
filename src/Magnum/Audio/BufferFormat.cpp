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

#include "BufferFormat.h"

#include <Corrade/Utility/Debug.h>

namespace Magnum { namespace Audio {

Debug& operator<<(Debug& debug, const BufferFormat value) {
    debug << "Audio::BufferFormat" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case BufferFormat::value: return debug << "::" #value;
        _c(Mono8)
        _c(Mono16)
        _c(Stereo8)
        _c(Stereo16)

        _c(MonoALaw)
        _c(StereoALaw)

        _c(MonoMuLaw)
        _c(StereoMuLaw)

        _c(MonoFloat)
        _c(StereoFloat)
        _c(MonoDouble)
        _c(StereoDouble)

         _c(Quad8)
         _c(Quad16)
         _c(Quad32)

         _c(Rear8)
         _c(Rear16)
         _c(Rear32)

         _c(Surround51Channel8)
         _c(Surround51Channel16)
         _c(Surround51Channel32)

         _c(Surround61Channel8)
         _c(Surround61Channel16)
         _c(Surround61Channel32)

         _c(Surround71Channel8)
         _c(Surround71Channel16)
         _c(Surround71Channel32)

        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(ALenum(value)) << Debug::nospace << ")";
}

}}
