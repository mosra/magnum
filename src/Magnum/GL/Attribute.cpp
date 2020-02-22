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

#include "Attribute.h"

#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>

namespace Magnum { namespace GL {

Debug& operator<<(Debug& debug, const DynamicAttribute::Kind value) {
    debug << "GL::DynamicAttribute::Kind" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case DynamicAttribute::Kind::value: return debug << "::" #value;
        _c(Generic)
        _c(GenericNormalized)
        #ifndef MAGNUM_TARGET_GLES2
        _c(Integral)
        #ifndef MAGNUM_TARGET_GLES
        _c(Long)
        #endif
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLenum(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const DynamicAttribute::Components value) {
    debug << "GL::DynamicAttribute::Components" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case DynamicAttribute::Components::value: return debug << "::" #value;
        _c(One)
        _c(Two)
        _c(Three)
        _c(Four)
        #ifndef MAGNUM_TARGET_GLES
        _c(BGRA)
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLint(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const DynamicAttribute::DataType value) {
    debug << "GL::DynamicAttribute::DataType" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case DynamicAttribute::DataType::value: return debug << "::" #value;
        _c(UnsignedByte)
        _c(Byte)
        _c(UnsignedShort)
        _c(Short)
        _c(UnsignedInt)
        _c(Int)
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        _c(Half)
        #endif
        _c(Float)
        #ifndef MAGNUM_TARGET_GLES
        _c(Double)
        _c(UnsignedInt10f11f11fRev)
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        _c(UnsignedInt2101010Rev)
        _c(Int2101010Rev)
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLenum(value)) << Debug::nospace << ")";
}

namespace Implementation {

UnsignedInt FloatAttribute::size(GLint components, DataType dataType) {
    switch(dataType) {
        case DataType::UnsignedByte:
        case DataType::Byte:
            return components;
        case DataType::UnsignedShort:
        case DataType::Short:
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case DataType::Half:
        #endif
            return 2*components;
        case DataType::UnsignedInt:
        case DataType::Int:
        case DataType::Float:
            return 4*components;
        #ifndef MAGNUM_TARGET_GLES
        case DataType::Double:
            return 8*components;
        #endif
    }

    CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

#ifndef MAGNUM_TARGET_GLES2
UnsignedInt IntAttribute::size(GLint components, DataType dataType) {
    switch(dataType) {
        case DataType::UnsignedByte:
        case DataType::Byte:
            return components;
        case DataType::UnsignedShort:
        case DataType::Short:
            return 2*components;
        case DataType::UnsignedInt:
        case DataType::Int:
            return 4*components;
    }

    CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}
#endif

#ifndef MAGNUM_TARGET_GLES
UnsignedInt DoubleAttribute::size(GLint components, DataType dataType) {
    switch(dataType) {
        case DataType::Double:
            return 8*components;
    }

    CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}
#endif

UnsignedInt Attribute<Math::Vector<3, Float>>::size(GLint components, DataType dataType) {
    switch(dataType) {
        case DataType::UnsignedByte:
        case DataType::Byte:
            return components;
        case DataType::UnsignedShort:
        case DataType::Short:
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case DataType::Half:
        #endif
            return 2*components;
        case DataType::UnsignedInt:
        case DataType::Int:
        case DataType::Float:
            return 4*components;
        #ifndef MAGNUM_TARGET_GLES
        case DataType::Double:
            return 8*components;
        case DataType::UnsignedInt10f11f11fRev:
            CORRADE_INTERNAL_ASSERT(components == 3);
            return 4;
        #endif
    }

    CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

UnsignedInt Attribute<Math::Vector<4, Float>>::size(GLint components, DataType dataType) {
    #ifndef MAGNUM_TARGET_GLES
    if(components == GL_BGRA) components = 4;
    #endif

    switch(dataType) {
        case DataType::UnsignedByte:
        case DataType::Byte:
            return components;
        case DataType::UnsignedShort:
        case DataType::Short:
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case DataType::Half:
        #endif
            return 2*components;
        case DataType::UnsignedInt:
        case DataType::Int:
        case DataType::Float:
            return 4*components;
        #ifndef MAGNUM_TARGET_GLES
        case DataType::Double:
            return 8*components;
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        case DataType::UnsignedInt2101010Rev:
        case DataType::Int2101010Rev:
            CORRADE_INTERNAL_ASSERT(components == 4);
            return 4;
        #endif
    }

    CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

Debug& operator<<(Debug& debug, const SizedAttribute<1, 1>::Components value) {
    debug << "GL::Attribute::Components" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        case SizedAttribute<1, 1>::Components::One:
            return debug << "::One";
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLint(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const SizedAttribute<1, 2>::Components value) {
    debug << "GL::Attribute::Components" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        case SizedAttribute<1, 2>::Components::One:
            return debug << "::One";
        case SizedAttribute<1, 2>::Components::Two:
            return debug << "::Two";
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLint(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const SizedAttribute<1, 3>::Components value) {
    debug << "GL::Attribute::Components" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        case SizedAttribute<1, 3>::Components::One:
            return debug << "::One";
        case SizedAttribute<1, 3>::Components::Two:
            return debug << "::Two";
        case SizedAttribute<1, 3>::Components::Three:
            return debug << "::Three";
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLint(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const SizedAttribute<1, 4>::Components value) {
    debug << "GL::Attribute::Components" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        case SizedAttribute<1, 4>::Components::One:
            return debug << "::One";
        case SizedAttribute<1, 4>::Components::Two:
            return debug << "::Two";
        case SizedAttribute<1, 4>::Components::Three:
            return debug << "::Three";
        case SizedAttribute<1, 4>::Components::Four:
            return debug << "::Four";
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLint(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const SizedMatrixAttribute<2>::Components value) {
    debug << "GL::Attribute::Components" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        case SizedMatrixAttribute<2>::Components::Two:
            return debug << "::Two";
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLint(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const SizedMatrixAttribute<3>::Components value) {
    debug << "GL::Attribute::Components" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        case SizedMatrixAttribute<3>::Components::Three:
            return debug << "::Three";
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLint(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const SizedMatrixAttribute<4>::Components value) {
    debug << "GL::Attribute::Components" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        case SizedMatrixAttribute<4>::Components::Four:
            return debug << "::Four";
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLint(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const Attribute<Math::Vector<4, Float>>::Components value) {
    debug << "GL::Attribute::Components" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        case Attribute<Math::Vector<4, Float>>::Components::One:
            return debug << "::One";
        case Attribute<Math::Vector<4, Float>>::Components::Two:
            return debug << "::Two";
        case Attribute<Math::Vector<4, Float>>::Components::Three:
            return debug << "::Three";
        case Attribute<Math::Vector<4, Float>>::Components::Four:
            return debug << "::Four";
        #ifndef MAGNUM_TARGET_GLES
        case Attribute<Math::Vector<4, Float>>::Components::BGRA:
            return debug << "::BGRA";
        #endif
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLint(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const FloatAttribute::DataType value) {
    debug << "GL::Attribute::DataType" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case FloatAttribute::DataType::value: return debug << "::" #value;
        _c(UnsignedByte)
        _c(Byte)
        _c(UnsignedShort)
        _c(Short)
        _c(UnsignedInt)
        _c(Int)
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        _c(Half)
        #endif
        _c(Float)
        #ifndef MAGNUM_TARGET_GLES
        _c(Double)
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLenum(value)) << Debug::nospace << ")";
}

#ifndef MAGNUM_TARGET_GLES2
Debug& operator<<(Debug& debug, const IntAttribute::DataType value) {
    debug << "GL::Attribute::DataType" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case IntAttribute::DataType::value: return debug << "::" #value;
        _c(UnsignedByte)
        _c(Byte)
        _c(UnsignedShort)
        _c(Short)
        _c(UnsignedInt)
        _c(Int)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLenum(value)) << Debug::nospace << ")";
}
#endif

#ifndef MAGNUM_TARGET_GLES
Debug& operator<<(Debug& debug, const DoubleAttribute::DataType value) {
    debug << "GL::Attribute::DataType" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case DoubleAttribute::DataType::value: return debug << "::" #value;
        _c(Double)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLenum(value)) << Debug::nospace << ")";
}
#endif

Debug& operator<<(Debug& debug, const Attribute<Math::Vector<3, Float>>::DataType value) {
    debug << "GL::Attribute::DataType" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Attribute<Math::Vector<3, Float>>::DataType::value: return debug << "::" #value;
        _c(UnsignedByte)
        _c(Byte)
        _c(UnsignedShort)
        _c(Short)
        _c(UnsignedInt)
        _c(Int)
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        _c(Half)
        #endif
        _c(Float)
        #ifndef MAGNUM_TARGET_GLES
        _c(Double)
        _c(UnsignedInt10f11f11fRev)
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLenum(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const Attribute<Math::Vector<4, Float>>::DataType value) {
    debug << "GL::Attribute::DataType" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Attribute<Math::Vector<4, Float>>::DataType::value: return debug << "::" #value;
        _c(UnsignedByte)
        _c(Byte)
        _c(UnsignedShort)
        _c(Short)
        _c(UnsignedInt)
        _c(Int)
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        _c(Half)
        #endif
        _c(Float)
        #ifndef MAGNUM_TARGET_GLES
        _c(Double)
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        _c(UnsignedInt2101010Rev)
        _c(Int2101010Rev)
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLenum(value)) << Debug::nospace << ")";
}

}

}}
