/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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

#include "DeviceFeatures.h"

#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/BigEnumSet.hpp>

namespace Magnum { namespace Vk {

namespace {

#ifndef DOXYGEN_GENERATING_OUTPUT /* It gets *really* confused */
constexpr const char* FeatureNames[] {
    #define _c(value, field) #value,
    #define _cver(value, field, suffix, version) _c(value, field)
    #define _cext(value, field, suffix, extension) _c(value, field)
    #include "Magnum/Vk/Implementation/deviceFeatureMapping.hpp"
    #undef _c
    #undef _cver
    #undef _cext
};
#endif

}

Debug& operator<<(Debug& debug, const DeviceFeature value) {
    debug << "Vk::DeviceFeature" << Debug::nospace;

    if(UnsignedInt(value) < Containers::arraySize(FeatureNames)) {
        return debug << "::" << Debug::nospace << FeatureNames[UnsignedInt(value)];
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const DeviceFeatures& value) {
    return Containers::bigEnumSetDebugOutput(debug, value, "Vk::DeviceFeatures{}");
}

}}
