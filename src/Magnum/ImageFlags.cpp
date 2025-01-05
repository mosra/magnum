/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include "ImageFlags.h"

#include <Corrade/Containers/EnumSet.hpp>

namespace Magnum {

Debug& operator<<(Debug& debug, const ImageFlag1D value) {
    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;

    if(!packed)
        debug << "ImageFlag1D" << Debug::nospace;

    /* MSVC, I want to have the switch here even if it's empty so I'm notified
       when there are new values that I should handle. Don't complain. */
    /** @todo drop this once there are values */
    #ifdef CORRADE_TARGET_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4060)
    #endif
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case ImageFlag1D::value: return debug << (packed ? "" : "::") << Debug::nospace << #value;
        #undef _c
        /* LCOV_EXCL_STOP */
    }
    #ifdef CORRADE_TARGET_MSVC
    #pragma warning(pop)
    #endif

    return debug << (packed ? "" : "(") << Debug::nospace << Debug::hex << UnsignedShort(value) << Debug::nospace << (packed ? "" : ")");
}

Debug& operator<<(Debug& debug, const ImageFlag2D value) {
    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;

    if(!packed)
        debug << "ImageFlag2D" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case ImageFlag2D::value: return debug << (packed ? "" : "::") << Debug::nospace << #value;
        _c(Array)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << (packed ? "" : "(") << Debug::nospace << Debug::hex << UnsignedShort(value) << Debug::nospace << (packed ? "" : ")");
}

Debug& operator<<(Debug& debug, const ImageFlag3D value) {
    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;

    if(!packed)
        debug << "ImageFlag3D" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case ImageFlag3D::value: return debug << (packed ? "" : "::") << Debug::nospace << #value;
        _c(Array)
        _c(CubeMap)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << (packed ? "" : "(") << Debug::nospace << Debug::hex << UnsignedShort(value) << Debug::nospace << (packed ? "" : ")");
}

Debug& operator<<(Debug& debug, const ImageFlags1D value) {
    return Containers::enumSetDebugOutput(debug, value, debug.immediateFlags() >= Debug::Flag::Packed ? "{}" : "ImageFlags1D{}", {
    });
}

Debug& operator<<(Debug& debug, const ImageFlags2D value) {
    return Containers::enumSetDebugOutput(debug, value, debug.immediateFlags() >= Debug::Flag::Packed ? "{}" : "ImageFlags2D{}", {
        ImageFlag2D::Array
    });
}

Debug& operator<<(Debug& debug, const ImageFlags3D value) {
    return Containers::enumSetDebugOutput(debug, value, debug.immediateFlags() >= Debug::Flag::Packed ? "{}" : "ImageFlags3D{}", {
        ImageFlag3D::Array,
        ImageFlag3D::CubeMap
    });
}

}
