/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "TypeTraits.h"

#include <type_traits>
#include <Utility/Debug.h>

using namespace std;

namespace Magnum {

#ifndef DOXYGEN_GENERATING_OUTPUT
static_assert(is_same<GLubyte, uint8_t>::value, "GLubyte is not the same as uint8_t");
static_assert(is_same<GLbyte, int8_t>::value, "GLbyte is not the same as int8_t");
static_assert(is_same<GLushort, uint16_t>::value, "GLushort is not the same as uint16_t");
static_assert(is_same<GLshort, int16_t>::value, "GLshort is not the same as int16_t");
static_assert(is_same<GLuint, uint32_t>::value, "GLuint is not the same as uint32_t");
static_assert(is_same<GLint, int32_t>::value, "GLint is not the same as int32_t");
static_assert(is_same<GLsizei, int32_t>::value, "GLsizei is not the same as int32_t");
static_assert(is_same<GLfloat, float>::value, "GLfloat is not the same as float");
#ifndef MAGNUM_TARGET_GLES
static_assert(is_same<GLdouble, double>::value, "GLdouble is not the same as double");
#endif
#endif

size_t TypeInfo::sizeOf(Type type) {
    switch(type) {
        #define val(type) case Type::type: return TypeTraits<TypeOf<Type::type>::Type>::size();
        val(UnsignedByte)
        val(Byte)
        val(UnsignedShort)
        val(Short)
        val(UnsignedInt)
        val(Int)
        #ifndef MAGNUM_TARGET_GLES
        val(Double)
        #endif
        val(Float)
        #undef val

        default: return 0;
    }
}

bool TypeInfo::isIntegral(Type type) {
    switch(type) {
        case Type::UnsignedByte:
        case Type::Byte:
        case Type::UnsignedShort:
        case Type::Short:
        case Type::UnsignedInt:
        case Type::Int:
            return true;
        default:
            return false;
    }
}

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug operator<<(Debug debug, Type value) {
    switch(value) {
        #define _c(value) case Type::value: return debug << "Type::" #value;
        _c(UnsignedByte)
        _c(Byte)
        _c(UnsignedShort)
        _c(Short)
        _c(UnsignedInt)
        _c(Int)
        _c(Float)
        #ifndef MAGNUM_TARGET_GLES
        _c(Double)
        #endif
        #undef _c
    }

    return debug << "Type::(invalid)";
}
#endif

}

namespace Corrade { namespace Utility {

std::string ConfigurationValue<Magnum::Type>::toString(Magnum::Type value, ConfigurationValueFlags) {
    switch(value) {
        #define _c(value) case Magnum::Type::value: return #value;
        _c(UnsignedByte)
        _c(Byte)
        _c(UnsignedShort)
        _c(Short)
        _c(UnsignedInt)
        _c(Int)
        _c(Float)
        #ifndef MAGNUM_TARGET_GLES
        _c(Double)
        #endif
        #undef _c
    }

    return "";
}

Magnum::Type ConfigurationValue<Magnum::Type>::fromString(const std::string& stringValue, ConfigurationValueFlags) {
    #define _c(value) if(stringValue == #value) return Magnum::Type::value;
        _c(UnsignedByte)
        _c(Byte)
        _c(UnsignedShort)
        _c(Short)
        _c(UnsignedInt)
        _c(Int)
        #ifndef MAGNUM_TARGET_GLES
        _c(Double)
        #endif
    #undef _c

    return Magnum::Type::Float;
}

}}
