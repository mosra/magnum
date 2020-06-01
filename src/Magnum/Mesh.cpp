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

#include "Mesh.h"

#include <string>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>

namespace Magnum {

UnsignedInt meshIndexTypeSize(MeshIndexType type) {
    switch(type) {
        case MeshIndexType::UnsignedByte: return 1;
        case MeshIndexType::UnsignedShort: return 2;
        case MeshIndexType::UnsignedInt: return 4;
    }

    CORRADE_ASSERT_UNREACHABLE("meshIndexTypeSize(): invalid type" << type, {});
}

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace {

constexpr const char* MeshPrimitiveNames[] {
    #define _c(primitive) #primitive,
    #include "Magnum/Implementation/meshPrimitiveMapping.hpp"
    #undef _c
};

}

Debug& operator<<(Debug& debug, const MeshPrimitive value) {
    debug << "MeshPrimitive" << Debug::nospace;

    if(isMeshPrimitiveImplementationSpecific(value)) {
        return debug << "::ImplementationSpecific(" << Debug::nospace << reinterpret_cast<void*>(meshPrimitiveUnwrap(value)) << Debug::nospace << ")";
    }

    if(UnsignedInt(value) - 1 < Containers::arraySize(MeshPrimitiveNames)) {
        return debug << "::" << Debug::nospace << MeshPrimitiveNames[UnsignedInt(value) - 1];
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedInt(value)) << Debug::nospace << ")";
}

namespace {

constexpr const char* MeshIndexTypeNames[] {
    #define _c(type) #type,
    #include "Magnum/Implementation/meshIndexTypeMapping.hpp"
    #undef _c
};

}

Debug& operator<<(Debug& debug, const MeshIndexType value) {
    debug << "MeshIndexType" << Debug::nospace;

    if(UnsignedInt(value) - 1 < Containers::arraySize(MeshIndexTypeNames)) {
        return debug << "::" << Debug::nospace << MeshIndexTypeNames[UnsignedInt(value) - 1];
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedInt(value)) << Debug::nospace << ")";
}
#endif

}

namespace Corrade { namespace Utility {

std::string ConfigurationValue<Magnum::MeshPrimitive>::toString(Magnum::MeshPrimitive value, ConfigurationValueFlags) {
    if(Magnum::UnsignedInt(value) - 1 < Containers::arraySize(Magnum::MeshPrimitiveNames))
        return Magnum::MeshPrimitiveNames[Magnum::UnsignedInt(value) - 1];

    return {};
}

Magnum::MeshPrimitive ConfigurationValue<Magnum::MeshPrimitive>::fromString(const std::string& stringValue, ConfigurationValueFlags) {
    for(std::size_t i = 0; i != Containers::arraySize(Magnum::MeshPrimitiveNames); ++i)
        if(stringValue == Magnum::MeshPrimitiveNames[i]) return Magnum::MeshPrimitive(i + 1);

    return {};
}

std::string ConfigurationValue<Magnum::MeshIndexType>::toString(Magnum::MeshIndexType value, ConfigurationValueFlags) {
    if(Magnum::UnsignedInt(value) - 1 < Containers::arraySize(Magnum::MeshIndexTypeNames))
        return Magnum::MeshIndexTypeNames[Magnum::UnsignedInt(value) - 1];

    return {};
}

Magnum::MeshIndexType ConfigurationValue<Magnum::MeshIndexType>::fromString(const std::string& stringValue, ConfigurationValueFlags) {
    for(std::size_t i = 0; i != Containers::arraySize(Magnum::MeshIndexTypeNames); ++i)
        if(stringValue == Magnum::MeshIndexTypeNames[i]) return Magnum::MeshIndexType(i + 1);

    return {};
}

}}
