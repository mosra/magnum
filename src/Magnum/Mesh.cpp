/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

#include "Mesh.h"

#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>

namespace Magnum {

UnsignedInt meshIndexTypeSize(MeshIndexType type) {
    switch(type) {
        case MeshIndexType::UnsignedByte: return 1;
        case MeshIndexType::UnsignedShort: return 2;
        case MeshIndexType::UnsignedInt: return 4;
    }

    CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug& operator<<(Debug& debug, MeshPrimitive value) {
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case MeshPrimitive::value: return debug << "MeshPrimitive::" #value;
        _c(Points)
        _c(Lines)
        _c(LineLoop)
        _c(LineStrip)
        _c(Triangles)
        _c(TriangleStrip)
        _c(TriangleFan)
        #undef _c

        /* Here mainly to suppress compiler warnings about unhandled cases and
           also to check that there are no accidentally conflicting values. */
        #if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL) && !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        CORRADE_IGNORE_DEPRECATED_PUSH
        #define _c(value) case MeshPrimitive::value: return debug << "GL::MeshPrimitive::" #value;
        _c(LinesAdjacency)
        _c(LineStripAdjacency)
        _c(TrianglesAdjacency)
        _c(TriangleStripAdjacency)
        _c(Patches)
        #undef _c
        CORRADE_IGNORE_DEPRECATED_POP
        #endif
        /* LCOV_EXCL_STOP */
    }

    return debug << "MeshPrimitive(" << Debug::nospace << reinterpret_cast<void*>(UnsignedInt(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, MeshIndexType value) {
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case MeshIndexType::value: return debug << "MeshIndexType::" #value;
        _c(UnsignedByte)
        _c(UnsignedShort)
        _c(UnsignedInt)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "MeshIndexType(" << Debug::nospace << reinterpret_cast<void*>(UnsignedInt(value)) << Debug::nospace << ")";
}
#endif

}

namespace Corrade { namespace Utility {

std::string ConfigurationValue<Magnum::MeshPrimitive>::toString(Magnum::MeshPrimitive value, ConfigurationValueFlags) {
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Magnum::MeshPrimitive::value: return #value;
        _c(Points)
        _c(Lines)
        _c(LineLoop)
        _c(LineStrip)
        _c(Triangles)
        _c(TriangleStrip)
        _c(TriangleFan)
        #undef _c

        #if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL) && !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        CORRADE_IGNORE_DEPRECATED_PUSH
        case Magnum::MeshPrimitive::LinesAdjacency:
        case Magnum::MeshPrimitive::LineStripAdjacency:
        case Magnum::MeshPrimitive::TrianglesAdjacency:
        case Magnum::MeshPrimitive::TriangleStripAdjacency:
        case Magnum::MeshPrimitive::Patches:
            return {};
        CORRADE_IGNORE_DEPRECATED_POP
        #endif
        /* LCOV_EXCL_STOP */
    }

    return {};
}

Magnum::MeshPrimitive ConfigurationValue<Magnum::MeshPrimitive>::fromString(const std::string& stringValue, ConfigurationValueFlags) {
    /* LCOV_EXCL_START */
    #define _c(value) if(stringValue == #value) return Magnum::MeshPrimitive::value;
    _c(LineStrip)
    _c(LineLoop)
    _c(Lines)
    _c(Triangles)
    _c(TriangleStrip)
    _c(TriangleFan)
    #undef _c
    /* LCOV_EXCL_STOP */

    return Magnum::MeshPrimitive::Points;
}

std::string ConfigurationValue<Magnum::MeshIndexType>::toString(Magnum::MeshIndexType value, ConfigurationValueFlags) {
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Magnum::MeshIndexType::value: return #value;
        _c(UnsignedByte)
        _c(UnsignedShort)
        _c(UnsignedInt)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return {};
}

Magnum::MeshIndexType ConfigurationValue<Magnum::MeshIndexType>::fromString(const std::string& stringValue, ConfigurationValueFlags) {
    /* LCOV_EXCL_START */
    #define _c(value) if(stringValue == #value) return Magnum::MeshIndexType::value;
    _c(UnsignedByte)
    _c(UnsignedShort)
    _c(UnsignedInt)
    #undef _c
    /* LCOV_EXCL_STOP */

    return Magnum::MeshIndexType::UnsignedInt;
}

}}
