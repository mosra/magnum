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

#include "Result.h"

#include <Corrade/Utility/Debug.h>

namespace Magnum { namespace Vk {

Debug& operator<<(Debug& debug, const Result value) {
    debug << "Vk::Result" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Vk::Result::value: return debug << "::" << Debug::nospace << #value;
        _c(Success)
        _c(NotReady)
        _c(Timeout)
        _c(EventSet)
        _c(EventReset)
        _c(Incomplete)
        _c(ErrorOutOfHostMemory)
        _c(ErrorOutOfDeviceMemory)
        _c(ErrorInitializationFailed)
        _c(ErrorDeviceLost)
        _c(ErrorMemoryMapFailed)
        _c(ErrorLayerNotPresent)
        _c(ErrorExtensionNotPresent)
        _c(ErrorFeatureNotPresent)
        _c(ErrorIncompatibleDriver)
        _c(ErrorTooManyObjects)
        _c(ErrorFormatNotSupported)
        _c(ErrorFragmentedPool)
        _c(ErrorUnknown)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    /* Vulkan docs have the values in decimal, so not converting to hex */
    return debug << "(" << Debug::nospace << Int(value) << Debug::nospace << ")";
}

}}
