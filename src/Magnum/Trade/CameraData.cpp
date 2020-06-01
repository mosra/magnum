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

#include "CameraData.h"

#include <Corrade/Utility/Assert.h>
#include <Magnum/Math/Functions.h>

namespace Magnum { namespace Trade {

CameraData::CameraData(const CameraType type, const Rad fov, const Float aspectRatio, const Float near, const Float far, const void* const importerState) noexcept: _type{type}, _size{2*near*Math::tan(fov*0.5f)*Vector2::yScale(1.0f/aspectRatio)}, _near{near}, _far{far}, _importerState{importerState} {
    CORRADE_ASSERT(type == CameraType::Perspective3D,
        "Trade::CameraData: only perspective cameras can have FoV specified", );
}

CameraData::CameraData(const CameraType type, const Vector2& size, const Float near, const Float far, const void* const importerState) noexcept: _type{type}, _size{size}, _near{near}, _far{far}, _importerState{importerState} {
    CORRADE_ASSERT(type != CameraType::Orthographic2D || (!near && !far),
        "Trade::CameraData: 2D cameras can't be specified with near and far clipping planes", );
}

Rad CameraData::fov() const {
    CORRADE_ASSERT(_type == CameraType::Perspective3D,
        "Trade::CameraData::fov(): the camera is not perspective", {});
    return 2*Math::atan(_size.x()/(2*_near));
}

Debug& operator<<(Debug& debug, const CameraType value) {
    debug << "Trade::CameraType" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case CameraType::value: return debug << "::" #value;
        _c(Orthographic2D)
        _c(Orthographic3D)
        _c(Perspective3D)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

}}
