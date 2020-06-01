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

#include "Attribute.h"

#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/VertexFormat.h"

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

    CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
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

    CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}
#endif

#ifndef MAGNUM_TARGET_GLES
UnsignedInt DoubleAttribute::size(GLint components, DataType dataType) {
    switch(dataType) {
        case DataType::Double:
            return 8*components;
    }

    CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
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

    CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
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

    CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
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

bool hasVertexFormat(const VertexFormat format) {
    /* Non-square matrices are not supported on ES2 */
    #ifdef MAGNUM_TARGET_GLES2
    const UnsignedInt vectorCount = vertexFormatVectorCount(format);
    if(vectorCount != 1 && vectorCount != vertexFormatComponentCount(format))
        return false;
    #endif

    switch(vertexFormatComponentFormat(format)) {
        case VertexFormat::UnsignedByte:
        case VertexFormat::Byte:
        case VertexFormat::UnsignedShort:
        case VertexFormat::Short:
        case VertexFormat::UnsignedInt:
        case VertexFormat::Int:
        case VertexFormat::Float:
            return true;

        case VertexFormat::Half:
            #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
            return true;
            #else
            return false;
            #endif

        case VertexFormat::Double:
            #ifndef MAGNUM_TARGET_GLES
            return true;
            #else
            return false;
            #endif

        /* Nothing else expected to be returned from
           vertexFormatComponentFormat() */
        default: CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    }
}

namespace {

UnsignedInt attributeSize(DynamicAttribute::Components components, DynamicAttribute::DataType dataType) {
    Int componentCount = GLint(components);
    #ifndef MAGNUM_TARGET_GLES
    if(components == DynamicAttribute::Components::BGRA) componentCount = 4;
    #endif

    switch(dataType) {
        case DynamicAttribute::DataType::UnsignedByte:
        case DynamicAttribute::DataType::Byte:
            return componentCount;
        case DynamicAttribute::DataType::UnsignedShort:
        case DynamicAttribute::DataType::Short:
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case DynamicAttribute::DataType::Half:
        #endif
            return 2*componentCount;
        case DynamicAttribute::DataType::UnsignedInt:
        case DynamicAttribute::DataType::Int:
        case DynamicAttribute::DataType::Float:
            return 4*componentCount;
        #ifndef MAGNUM_TARGET_GLES
        case DynamicAttribute::DataType::Double:
            return 8*componentCount;
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        case DynamicAttribute::DataType::UnsignedInt2101010Rev:
        case DynamicAttribute::DataType::Int2101010Rev:
            CORRADE_INTERNAL_ASSERT(componentCount == 4);
            return 4;
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case DynamicAttribute::DataType::UnsignedInt10f11f11fRev:
            CORRADE_INTERNAL_ASSERT(componentCount == 3);
            return 4;
        #endif
    }

    CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

}

DynamicAttribute::DynamicAttribute(const Kind kind, const UnsignedInt location, const Components components, const UnsignedInt vectors, const DataType dataType): DynamicAttribute{kind, location, components, vectors, attributeSize(components, dataType), dataType} {}

DynamicAttribute::DynamicAttribute(const Kind kind, UnsignedInt location, const VertexFormat format, UnsignedInt maxVectors, GLint maxComponents): _kind{kind}, _location{location}, _components{Components(vertexFormatComponentCount(format))}, _vectors{vertexFormatVectorCount(format)}, _vectorStride{vertexFormatVectorStride(format)} {
    CORRADE_ASSERT(hasVertexFormat(format),
        "GL::DynamicAttribute:" << format << "isn't available on this target", );

    /* Translate component type to a GL-specific value */
    switch(vertexFormatComponentFormat(format)) {
        #define _c(format)                                                  \
            case VertexFormat::format:                                      \
                _dataType = DataType::format;                               \
                break;
        _c(UnsignedByte)
        _c(Byte)
        _c(UnsignedShort)
        _c(Short)
        _c(UnsignedInt)
        _c(Int)
        _c(Float)
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        _c(Half)
        #endif
        #ifndef MAGNUM_TARGET_GLES
        _c(Double)
        #endif
        #undef _c

        /* Nothing else expected to be returned from
           vertexFormatComponentFormat(), the unavailable formats were caught
           by the hasVertexFormat() above already */
        default: CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    }

    /* If the type is normalized, switch the type to GenericNormalized (if not
       already), and check that the attribute isn't expected to be integral or
       long */
    if(isVertexFormatNormalized(format)) {
        CORRADE_ASSERT(kind == Kind::Generic || kind == Kind::GenericNormalized,
            "GL::DynamicAttribute: can't use" << format << "for a" << kind << "attribute", );
        _kind = Kind::GenericNormalized;
    /* Otherwise check that non-normalized types aren't used for attributes
       that are expected to be normalized. Float is an exception. */
    } else if(_dataType != DataType::Float) {
        CORRADE_ASSERT(kind != Kind::GenericNormalized,
            "GL::DynamicAttribute: can't use" << format << "for a normalized attribute", );
    /* Finally, float data types can't be used for integer attributes */
    } else {
        #ifndef MAGNUM_TARGET_GLES2
        CORRADE_ASSERT(kind != Kind::Integral,
            "GL::DynamicAttribute: can't use" << format << "for an integral attribute", );
        #endif
    }

    #ifndef CORRADE_NO_ASSERT
    CORRADE_ASSERT(_vectors <= maxVectors,
        "GL::DynamicAttribute: can't use" << format << "for a" << maxVectors << Debug::nospace << "-vector attribute", );
    /* Should pass also if maxComponents is GL_BGRA */
    CORRADE_ASSERT(GLint(_components) <= maxComponents,
        "GL::DynamicAttribute: can't use" << format << "for a" << maxComponents << Debug::nospace << "-component attribute", );
    #else
    static_cast<void>(maxVectors);
    static_cast<void>(maxComponents);
    #endif
}

}}
