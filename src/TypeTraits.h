#ifndef Magnum_TypeTraits_h
#define Magnum_TypeTraits_h
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

/** @file /TypeTraits.h
 * @brief Enum Magnum::Type, class Magnum::TypeOf, Magnum::TypeInfo, Magnum::TypeTraits
 */

#include <Utility/ConfigurationValue.h>

#include "Math/MathTypeTraits.h"
#include "AbstractImage.h"

namespace Magnum {

namespace Math {
    template<std::size_t, std::size_t, class> class RectangularMatrix;
    template<std::size_t, class> class Matrix;
    template<std::size_t, class> class Vector;
}

/**
@brief Traits class for plain OpenGL types

@copydetails Math::MathTypeTraits

Where it makes sense, this class extends Math::MathTypeTraits with
OpenGL-specific traits.
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T> struct TypeTraits: Math::MathTypeTraits<T> {
    /**
     * @brief Corresponding type for vertex attributes
     *
     * Implemented only in types which can be used for vertex attributes. This
     * function is not present for types unusable for vertex attributes, like
     * five-component vectors or GLdouble in OpenGL ES. See also
     * @ref AbstractShaderProgram-types.
     */
    typedef U AttributeType;

    /**
     * @brief OpenGL plain type ID
     *
     * Returns e.g. Type::UnsignedInt for GLuint.
     */
    inline constexpr static Type type();

    /**
     * @brief OpenGL type ID for indices
     *
     * Implemented only in types which can be used for vertex indices (all
     * unsigned types). This function is not present for types unusable for
     * vertex indices, like GLfloat or GLint.
     */
    inline constexpr static Type indexType();

    /**
     * @brief OpenGL type ID for images
     *
     * Implemented only in types which can be used for image data, like
     * GLubyte. This function is not present for types unusable for image data,
     * like GLdouble and Matrix3.
     */
    inline constexpr static AbstractImage::ComponentType imageType();

    /**
     * @brief Size of plain OpenGL type
     *
     * Returns sizeof(GLfloat) for GLfloat, but also sizeof(GLfloat) for
     * Vector3. See count().
     */
    inline constexpr static std::size_t size();

    /**
     * @brief Count of plain elements in this type
     *
     * Returns 1 for plain OpenGL types like GLint, but e.g. 3 for Vector3.
     */
    inline constexpr static std::size_t count();
};
#else
template<class T> struct TypeTraits {};
#endif

/** @brief OpenGL plain types */
enum class Type: GLenum {
    UnsignedByte = GL_UNSIGNED_BYTE,    /**< Unsigned byte (char) */
    Byte = GL_BYTE,                     /**< Byte (char) */
    UnsignedShort = GL_UNSIGNED_SHORT,  /**< Unsigned short */
    Short = GL_SHORT,                   /**< Short */
    UnsignedInt = GL_UNSIGNED_INT,      /**< Unsigned int */
    Int = GL_INT,                       /**< Int */
    Float = GL_FLOAT                    /**< Float */

    #ifndef MAGNUM_TARGET_GLES
    ,
    /**
     * Double
     * @requires_gl Only floats are available in OpenGL ES.
     */
    Double = GL_DOUBLE
    #endif
};

/** @debugoperator{Magnum::TypeInfo} */
Debug MAGNUM_EXPORT operator<<(Debug debug, Type value);

/**
@brief Class for converting Type enum values to types

When you want to use TypeTraits on type specified only as enum value, you can
use this class to convert it into type, for example these two statements
are equivalent:
@code
type = TypeTraits<TypeOf<Type::UnsignedByte>::Type>::imageType();
type = TypeTraits<GLubyte>::imageType();
@endcode
*/
template<Type T> class TypeOf {
    #ifdef DOXYGEN_GENERATING_OUTPUT
    typedef U Type; /**< @brief Type */
    #endif
};

/**
@brief Functor for runtime information about given type

TypeTraits alone allows to get information about given type only at compile
time, this class alows to get some information also at runtime with tiny
performance loss.
*/
struct MAGNUM_EXPORT TypeInfo {
    /**
     * @brief Size of given type
     *
     * These two lines provide the same information, one at compile time,
     * one at runtime:
     * @code
     * std::size_t size = TypeTraits<TypeOf<Type::UnsignedByte>::size();
     * std::size_t size = TypeInfo::sizeOf(Type::UnsignedByte);
     * @endcode
     */
    static std::size_t sizeOf(Type type);

    /**
     * @brief Whether the type is integral
     * @return true for (un)signed byte, short and integer, false otherwise.
     */
    static bool isIntegral(Type type);
};

/** @todo Other texture types, referenced in glTexImage2D function manual */
/** @todo Using Vector3 for textures? */

#ifndef DOXYGEN_GENERATING_OUTPUT
template<> struct TypeOf<Type::UnsignedByte> { typedef GLubyte Type; };
template<> struct TypeOf<Type::Byte> { typedef GLbyte Type; };
template<> struct TypeOf<Type::UnsignedShort> { typedef GLushort Type; };
template<> struct TypeOf<Type::Short> { typedef GLshort Type; };
template<> struct TypeOf<Type::UnsignedInt> { typedef GLuint Type; };
template<> struct TypeOf<Type::Int> { typedef GLint Type; };
template<> struct TypeOf<Type::Float> { typedef GLfloat Type; };
#ifndef MAGNUM_TARGET_GLES
template<> struct TypeOf<Type::Double> { typedef GLdouble Type; };
#endif

template<> struct TypeTraits<GLubyte>: Math::MathTypeTraits<std::uint8_t> {
    /* Can not be used for attributes */
    inline constexpr static Type type() { return Type::UnsignedByte; }
    inline constexpr static Type indexType() { return Type::UnsignedByte; }
    inline constexpr static AbstractImage::ComponentType imageType() { return AbstractImage::ComponentType::UnsignedByte; }
    inline constexpr static std::size_t size() { return sizeof(GLubyte); }
    inline constexpr static std::size_t count() { return 1; }
};

template<> struct TypeTraits<GLbyte>: Math::MathTypeTraits<std::int8_t> {
    /* Can not be used for attributes */
    inline constexpr static Type type() { return Type::Byte; }
    /* Can not be used for indices */
    inline constexpr static AbstractImage::ComponentType imageType() { return AbstractImage::ComponentType::Byte; }
    inline constexpr static std::size_t size() { return sizeof(GLbyte); }
    inline constexpr static std::size_t count() { return 1; }
};

template<> struct TypeTraits<GLushort>: Math::MathTypeTraits<std::uint16_t> {
    /* Can not be used for attributes */
    inline constexpr static Type type() { return Type::UnsignedShort; }
    inline constexpr static Type indexType() { return Type::UnsignedShort; }
    inline constexpr static AbstractImage::ComponentType imageType() { return AbstractImage::ComponentType::UnsignedShort; }
    inline constexpr static std::size_t size() { return sizeof(GLushort); }
    inline constexpr static std::size_t count() { return 1; }
};

template<> struct TypeTraits<GLshort>: Math::MathTypeTraits<std::int16_t> {
    /* Can not be used for attributes */
    inline constexpr static Type type() { return Type::Short; }
    /* Can not be used for indices */
    inline constexpr static AbstractImage::ComponentType imageType() { return AbstractImage::ComponentType::Short; }
    inline constexpr static std::size_t size() { return sizeof(GLshort); }
    inline constexpr static std::size_t count() { return 1; }
};

template<> struct TypeTraits<GLuint>: Math::MathTypeTraits<std::uint32_t> {
    typedef GLuint AttributeType;
    inline constexpr static Type type() { return Type::UnsignedInt; }
    inline constexpr static Type indexType() { return Type::UnsignedInt; }
    inline constexpr static AbstractImage::ComponentType imageType() { return AbstractImage::ComponentType::UnsignedInt; }
    inline constexpr static std::size_t size() { return sizeof(GLuint); }
    inline constexpr static std::size_t count() { return 1; }
};

template<> struct TypeTraits<GLint>: Math::MathTypeTraits<std::int32_t> {
    typedef GLint AttributeType;
    inline constexpr static Type type() { return Type::Int; }
    /* Can not be used for indices */
    inline constexpr static AbstractImage::ComponentType imageType() { return AbstractImage::ComponentType::Int; }
    inline constexpr static std::size_t size() { return sizeof(GLint); }
    inline constexpr static std::size_t count() { return 1; }
};

template<> struct TypeTraits<GLfloat>: Math::MathTypeTraits<float> {
    typedef GLfloat AttributeType;
    inline constexpr static Type type() { return Type::Float; }
    /* Can not be used for indices */
    inline constexpr static AbstractImage::ComponentType imageType() { return AbstractImage::ComponentType::Float; }
    inline constexpr static std::size_t size() { return sizeof(GLfloat); }
    inline constexpr static std::size_t count() { return 1; }
};

#ifndef MAGNUM_TARGET_GLES
template<> struct TypeTraits<GLdouble>: Math::MathTypeTraits<double> {
    typedef GLdouble AttributeType;
    inline constexpr static Type type() { return Type::Double; }
    /* Can not be used for indices */
    /* Can not be used for images */
    inline constexpr static std::size_t size() { return sizeof(GLdouble); }
    inline constexpr static std::size_t count() { return 1; }
};
#endif

namespace Implementation {
    template<std::size_t vectorSize, class T> struct VectorTypeTraits {
        /* Might be used for attributes, see below */
        inline constexpr static Type type() { return TypeTraits<T>::type(); }
        /* Might be used for attributes, see below */
        /* Can not be used for indices */
        /* Can not be used for images */
        inline constexpr static std::size_t size() { return sizeof(T); }
        inline constexpr static std::size_t count() { return vectorSize; }
    };

    template<class> struct VectorAttributeType {};

    template<> struct VectorAttributeType<GLuint> {
        typedef GLuint AttributeType;
    };

    template<> struct VectorAttributeType<GLint> {
        typedef GLint AttributeType;
    };

    template<> struct VectorAttributeType<GLfloat> {
        typedef GLfloat AttributeType;
    };

    #ifndef MAGNUM_TARGET_GLES
    template<> struct VectorAttributeType<GLdouble> {
        typedef GLdouble AttributeType;
    };
    #endif
}

template<std::size_t vectorSize, class T> struct TypeTraits<Math::Vector<vectorSize, T>>: Implementation::VectorTypeTraits<vectorSize, T> {};

/* Only some vectors can be used as attributes */
template<class T> struct TypeTraits<Math::Vector<1, T>>: Implementation::VectorTypeTraits<1, T>, Implementation::VectorAttributeType<T> {};
template<class T> struct TypeTraits<Math::Vector<2, T>>: Implementation::VectorTypeTraits<2, T>, Implementation::VectorAttributeType<T> {};
template<class T> struct TypeTraits<Math::Vector<3, T>>: Implementation::VectorTypeTraits<3, T>, Implementation::VectorAttributeType<T> {};
template<class T> struct TypeTraits<Math::Vector<4, T>>: Implementation::VectorTypeTraits<4, T>, Implementation::VectorAttributeType<T> {};

template<class T> struct TypeTraits<Math::Vector2<T>>: TypeTraits<Math::Vector<2, T>> {};
template<class T> struct TypeTraits<Math::Vector3<T>>: TypeTraits<Math::Vector<3, T>> {};
template<class T> struct TypeTraits<Math::Vector4<T>>: TypeTraits<Math::Vector<4, T>> {};
template<class T> struct TypeTraits<Math::Point2D<T>>: TypeTraits<Math::Vector<3, T>> {};
template<class T> struct TypeTraits<Math::Point3D<T>>: TypeTraits<Math::Vector<4, T>> {};
template<class T> struct TypeTraits<Color3<T>>: TypeTraits<Math::Vector<3, T>> {};
template<class T> struct TypeTraits<Color4<T>>: TypeTraits<Math::Vector<4, T>> {};

namespace Implementation {
    template<std::size_t cols, std::size_t rows, class T> struct MatrixTypeTraits {
        inline constexpr static Type type() { return TypeTraits<T>::type(); }
        /* Might be used for attributes, see below */
        /* Can not be used for indices */
        /* Can not be used for images */
        inline constexpr static std::size_t size() { return sizeof(T); }
        inline constexpr static std::size_t count() { return rows; }
        inline constexpr static std::size_t vectors() { return cols; }
    };

    template<class> struct MatrixAttributeType {};

    template<> struct MatrixAttributeType<GLfloat> {
        typedef GLfloat AttributeType;
    };

    #ifndef MAGNUM_TARGET_GLES
    template<> struct MatrixAttributeType<GLdouble> {
        typedef GLdouble AttributeType;
    };
    #endif
}

template<std::size_t cols, std::size_t rows, class T> struct TypeTraits<Math::RectangularMatrix<cols, rows, T>>: Implementation::MatrixTypeTraits<cols, rows, T> {};

/* Only some floating-point matrices can be used as attributes */
template<class T> struct TypeTraits<Math::RectangularMatrix<2, 2, T>>: Implementation::MatrixTypeTraits<2, 2, T>, Implementation::MatrixAttributeType<T> {};
template<class T> struct TypeTraits<Math::RectangularMatrix<3, 3, T>>: Implementation::MatrixTypeTraits<3, 3, T>, Implementation::MatrixAttributeType<T> {};
template<class T> struct TypeTraits<Math::RectangularMatrix<4, 4, T>>: Implementation::MatrixTypeTraits<4, 4, T>, Implementation::MatrixAttributeType<T> {};
template<class T> struct TypeTraits<Math::RectangularMatrix<2, 3, T>>: Implementation::MatrixTypeTraits<2, 3, T>, Implementation::MatrixAttributeType<T> {};
template<class T> struct TypeTraits<Math::RectangularMatrix<3, 2, T>>: Implementation::MatrixTypeTraits<3, 2, T>, Implementation::MatrixAttributeType<T> {};
template<class T> struct TypeTraits<Math::RectangularMatrix<2, 4, T>>: Implementation::MatrixTypeTraits<2, 4, T>, Implementation::MatrixAttributeType<T> {};
template<class T> struct TypeTraits<Math::RectangularMatrix<4, 2, T>>: Implementation::MatrixTypeTraits<4, 2, T>, Implementation::MatrixAttributeType<T> {};
template<class T> struct TypeTraits<Math::RectangularMatrix<3, 4, T>>: Implementation::MatrixTypeTraits<3, 4, T>, Implementation::MatrixAttributeType<T> {};
template<class T> struct TypeTraits<Math::RectangularMatrix<4, 3, T>>: Implementation::MatrixTypeTraits<4, 3, T>, Implementation::MatrixAttributeType<T> {};

template<std::size_t matrixSize, class T> struct TypeTraits<Math::Matrix<matrixSize, T>>: TypeTraits<Math::RectangularMatrix<matrixSize, matrixSize, T>> {};

template<class T> struct TypeTraits<Math::Matrix3<T>>: TypeTraits<Math::Matrix<3, T>> {};
template<class T> struct TypeTraits<Math::Matrix4<T>>: TypeTraits<Math::Matrix<4, T>> {};
#endif

}

namespace Corrade { namespace Utility {

/** @configurationvalue{Magnum::TypeInfo} */
template<> struct MAGNUM_EXPORT ConfigurationValue<Magnum::Type> {
    /**
     * @brief Writes enum value as string
     *
     * If the value is invalid, returns empty string.
     */
    static std::string toString(Magnum::Type value, ConfigurationValueFlags);

    /**
     * @brief Reads enum value as string
     *
     * If the value is invalid, returns @ref Magnum::Type "Magnum::Type::Float".
     */
    static Magnum::Type fromString(const std::string& stringValue, ConfigurationValueFlags);
};

}}

#endif
