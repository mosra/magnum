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

#include "Line.h"

#include <Corrade/Containers/String.h>
#include <Corrade/Containers/EnumSet.hpp>

#include "Magnum/Shaders/Implementation/lineMiterLimit.h"

namespace Magnum { namespace Shaders {

LineMaterialUniform& LineMaterialUniform::setMiterLengthLimit(const Float limit) {
    miterLimit = Implementation::lineMiterLengthLimit("Shaders::LineMaterialUniform::setMiterLengthLimit():", limit);
    return *this;
}

LineMaterialUniform& LineMaterialUniform::setMiterAngleLimit(const Rad limit) {
    miterLimit = Implementation::lineMiterAngleLimit("Shaders::LineMaterialUniform::setMiterAngleLimit():", limit);
    return *this;
}

Debug& operator<<(Debug& debug, const LineCapStyle value) {
    debug << "Shaders::LineCapStyle" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case LineCapStyle::v: return debug << "::" #v;
        _c(Butt)
        _c(Square)
        _c(Round)
        _c(Triangle)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << Debug::hex << UnsignedByte(value) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const LineJoinStyle value) {
    debug << "Shaders::LineJoinStyle" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case LineJoinStyle::v: return debug << "::" #v;
        _c(Miter)
        _c(Bevel)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << Debug::hex << UnsignedByte(value) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const LineVertexAnnotation value) {
    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;

    if(!packed)
        debug << "Shaders::LineVertexAnnotation" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case LineVertexAnnotation::v: return debug << (packed ? "" : "::") << Debug::nospace << #v;
        _c(Up)
        _c(Join)
        _c(Begin)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << (packed ? "" : "(") << Debug::nospace << Debug::hex << UnsignedInt(value) << Debug::nospace << (packed ? "" : ")");
}

Debug& operator<<(Debug& debug, const LineVertexAnnotations value) {
    return Containers::enumSetDebugOutput(debug, value, debug.immediateFlags() >= Debug::Flag::Packed ? "{}" : "Shaders::LineVertexAnnotations{}", {
        LineVertexAnnotation::Up,
        LineVertexAnnotation::Join,
        LineVertexAnnotation::Begin,
    });
}

}}
