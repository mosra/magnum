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

#include "Mesh.h"

#include <Corrade/Containers/String.h>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>

namespace Magnum {

namespace {

constexpr const char* MeshPrimitiveNames[] {
    #define _c(primitive) #primitive,
    #include "Magnum/Implementation/meshPrimitiveMapping.hpp"
    #undef _c
};

}

Debug& operator<<(Debug& debug, const MeshPrimitive value) {
    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;

    if(!packed)
        debug << "MeshPrimitive" << Debug::nospace;

    if(isMeshPrimitiveImplementationSpecific(value)) {
        return debug << (packed ? "ImplementationSpecific(" : "::ImplementationSpecific(") << Debug::nospace << Debug::hex << meshPrimitiveUnwrap(value) << Debug::nospace << ")";
    }

    if(UnsignedInt(value) - 1 < Containers::arraySize(MeshPrimitiveNames)) {
        return debug << (packed ? "" : "::") << Debug::nospace << MeshPrimitiveNames[UnsignedInt(value) - 1];
    }

    return debug << (packed ? "" : "(") << Debug::nospace << Debug::hex << UnsignedInt(value) << Debug::nospace << (packed ? "" : ")");
}

namespace {

constexpr const char* MeshIndexTypeNames[] {
    #define _c(type) #type,
    #include "Magnum/Implementation/meshIndexTypeMapping.hpp"
    #undef _c
};

}

Debug& operator<<(Debug& debug, const MeshIndexType value) {
    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;

    if(!packed)
        debug << "MeshIndexType" << Debug::nospace;

    if(isMeshIndexTypeImplementationSpecific(value)) {
        return debug << (packed ? "ImplementationSpecific(" : "::ImplementationSpecific(") << Debug::nospace << Debug::hex << meshIndexTypeUnwrap(value) << Debug::nospace << ")";
    }

    if(UnsignedInt(value) - 1 < Containers::arraySize(MeshIndexTypeNames)) {
        return debug << (packed ? "" : "::") << Debug::nospace << MeshIndexTypeNames[UnsignedInt(value) - 1];
    }

    return debug << (packed ? "" : "(") << Debug::nospace << Debug::hex << UnsignedInt(value) << Debug::nospace << (packed ? "" : ")");
}

UnsignedInt meshIndexTypeSize(const MeshIndexType type) {
    CORRADE_ASSERT(!isMeshIndexTypeImplementationSpecific(type),
        "meshIndexTypeSize(): can't determine size of an implementation-specific type" << Debug::hex << meshIndexTypeUnwrap(type), {});

    switch(type) {
        case MeshIndexType::UnsignedByte: return 1;
        case MeshIndexType::UnsignedShort: return 2;
        case MeshIndexType::UnsignedInt: return 4;
    }

    CORRADE_ASSERT_UNREACHABLE("meshIndexTypeSize(): invalid type" << type, {});
}

}

namespace Corrade { namespace Utility {

Containers::String ConfigurationValue<Magnum::MeshPrimitive>::toString(Magnum::MeshPrimitive value, ConfigurationValueFlags) {
    if(Magnum::UnsignedInt(value) - 1 < Containers::arraySize(Magnum::MeshPrimitiveNames))
        return Magnum::MeshPrimitiveNames[Magnum::UnsignedInt(value) - 1];

    return {};
}

Magnum::MeshPrimitive ConfigurationValue<Magnum::MeshPrimitive>::fromString(Containers::StringView stringValue, ConfigurationValueFlags) {
    for(std::size_t i = 0; i != Containers::arraySize(Magnum::MeshPrimitiveNames); ++i)
        if(stringValue == Magnum::MeshPrimitiveNames[i]) return Magnum::MeshPrimitive(i + 1);

    return {};
}

Containers::String ConfigurationValue<Magnum::MeshIndexType>::toString(Magnum::MeshIndexType value, ConfigurationValueFlags) {
    if(Magnum::UnsignedInt(value) - 1 < Containers::arraySize(Magnum::MeshIndexTypeNames))
        return Magnum::MeshIndexTypeNames[Magnum::UnsignedInt(value) - 1];

    return {};
}

Magnum::MeshIndexType ConfigurationValue<Magnum::MeshIndexType>::fromString(Containers::StringView stringValue, ConfigurationValueFlags) {
    for(std::size_t i = 0; i != Containers::arraySize(Magnum::MeshIndexTypeNames); ++i)
        if(stringValue == Magnum::MeshIndexTypeNames[i]) return Magnum::MeshIndexType(i + 1);

    return {};
}

}}
