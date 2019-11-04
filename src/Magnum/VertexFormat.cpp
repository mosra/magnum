/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include "VertexFormat.h"

#include <string>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>

namespace Magnum {

UnsignedInt vertexFormatSize(const VertexFormat format) {
    switch(format) {
        case VertexFormat::UnsignedByte:
        case VertexFormat::Byte:
            return 1;
        case VertexFormat::UnsignedShort:
        case VertexFormat::Short:
            return 2;
        case VertexFormat::Float:
        case VertexFormat::UnsignedInt:
        case VertexFormat::Int:
            return 4;
        case VertexFormat::Vector2: return 8;
        case VertexFormat::Vector3: return 12;
        case VertexFormat::Vector4: return 16;
    }

    CORRADE_ASSERT(false, "vertexFormatSize(): invalid format" << format, {});
}

namespace {

constexpr const char* VertexFormatNames[] {
    #define _c(format) #format,
    #include "Magnum/Implementation/vertexFormatMapping.hpp"
    #undef _c
};

}

Debug& operator<<(Debug& debug, const VertexFormat value) {
    debug << "VertexFormat" << Debug::nospace;

    if(UnsignedInt(value) - 1 < Containers::arraySize(VertexFormatNames)) {
        return debug << "::" << Debug::nospace << VertexFormatNames[UnsignedInt(value) - 1];
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedInt(value)) << Debug::nospace << ")";
}

}

namespace Corrade { namespace Utility {

std::string ConfigurationValue<Magnum::VertexFormat>::toString(Magnum::VertexFormat value, ConfigurationValueFlags) {
    if(Magnum::UnsignedInt(value) - 1 < Containers::arraySize(Magnum::VertexFormatNames))
        return Magnum::VertexFormatNames[Magnum::UnsignedInt(value) - 1];

    return {};
}

Magnum::VertexFormat ConfigurationValue<Magnum::VertexFormat>::fromString(const std::string& stringValue, ConfigurationValueFlags) {
    for(std::size_t i = 0; i != Containers::arraySize(Magnum::VertexFormatNames); ++i)
        if(stringValue == Magnum::VertexFormatNames[i]) return Magnum::VertexFormat(i + 1);

    return {};
}

}}
