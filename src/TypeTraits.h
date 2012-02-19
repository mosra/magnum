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

/** @file
 * @brief Enum Magnum::Type, class Magnum::TypeTraits
 */

#include "Magnum.h"

namespace Magnum {

/**
@brief Traits class for plain OpenGL types

@copydetails Math::TypeTraits

Where it makes sense, this class extends Math::TypeTraits with OpenGL-specific
traits.
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T> struct TypeTraits: public Math::TypeTraits<T> {
    /**
     * @brief Type which can be used for indices
     *
     * Implemented only in types which can be used for vertex indices (all
     * unsigned types). This typedef is not present for types unusable for
     * vertex indices, like GLfloat or GLint.
     */
    typedef T IndexType;

    /**
     * @brief Type which can be used in textures
     *
     * Implemented only in types which can be used for texture data, like
     * GLubyte. This typedef is not present for types unusable for texture data,
     * like GLdouble and Matrix3.
     */
    typedef T TextureType;

    /**
     * @brief OpenGL plain type ID
     *
     * Returns e.g. Type::UnsignedInt for GLuint.
     */
    constexpr inline static Type glType();

    /**
     * @brief Size of plain OpenGL type
     *
     * Returns sizeof(GLfloat) for GLfloat, but also sizeof(GLfloat) for
     * Vector3. See count().
     */
    constexpr inline static size_t size();

    /**
     * @brief Count of plain elements in this type
     *
     * Returns 1 for plain OpenGL types like GLint, but e.g. 3 for Vector3.
     */
    constexpr inline static size_t count();
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
    Float = GL_FLOAT,                   /**< Float */
    Double = GL_DOUBLE                  /**< Double */
};

/** @todo Other texture types, referenced in glTexImage2D function manual */
/** @todo Using Vector3 for textures? */

#ifndef DOXYGEN_GENERATING_OUTPUT
template<> struct TypeTraits<GLubyte>: public Math::TypeTraits<unsigned char> {
    typedef GLubyte IndexType;
    typedef GLubyte TextureType;

    inline constexpr static Type glType() { return Type::UnsignedByte; }
    inline constexpr static size_t size() { return sizeof(GLubyte); }
    inline constexpr static size_t count() { return 1; }
};

template<> struct TypeTraits<GLbyte>: public Math::TypeTraits<char> {
    /* Can not be used for indices */
    typedef GLbyte TextureType;

    inline constexpr static Type glType() { return Type::Byte; }
    inline constexpr static size_t size() { return sizeof(GLbyte); }
    inline constexpr static size_t count() { return 1; }
};

template<> struct TypeTraits<GLushort>: public Math::TypeTraits<unsigned short> {
    typedef GLushort IndexType;
    typedef GLushort TextureType;

    inline constexpr static Type glType() { return Type::UnsignedShort; }
    inline constexpr static size_t size() { return sizeof(GLushort); }
    inline constexpr static size_t count() { return 1; }
};

template<> struct TypeTraits<GLshort>: public Math::TypeTraits<short> {
    /* Can not be used for indices */
    typedef GLshort TextureType;

    inline constexpr static Type glType() { return Type::Short; }
    inline constexpr static size_t size() { return sizeof(GLshort); }
    inline constexpr static size_t count() { return 1; }
};

template<> struct TypeTraits<GLuint>: public Math::TypeTraits<unsigned int> {
    typedef GLuint IndexType;
    typedef GLuint TextureType;

    inline constexpr static Type glType() { return Type::UnsignedInt; }
    inline constexpr static size_t size() { return sizeof(GLuint); }
    inline constexpr static size_t count() { return 1; }
};

template<> struct TypeTraits<GLint>: public Math::TypeTraits<int> {
    /* Can not be used for indices */
    typedef GLint TextureType;

    inline constexpr static Type glType() { return Type::Int; }
    inline constexpr static size_t size() { return sizeof(GLint); }
    inline constexpr static size_t count() { return 1; }
};

template<> struct TypeTraits<GLfloat>: public Math::TypeTraits<float> {
    /* Can not be used for indices */
    typedef GLfloat TextureType;

    inline constexpr static Type glType() { return Type::Float; }
    inline constexpr static size_t size() { return sizeof(GLfloat); }
    inline constexpr static size_t count() { return 1; }
};

template<> struct TypeTraits<GLdouble>: public Math::TypeTraits<double> {
    /* Can not be used for indices */
    /* Can not be used for textures */

    inline constexpr static Type glType() { return Type::Double; }
    inline constexpr static size_t size() { return sizeof(GLdouble); }
    inline constexpr static size_t count() { return 1; }
};

template<class T, size_t vectorSize> struct TypeTraits<Math::Vector<T, vectorSize>> {
    /* Can not be used for indices */
    /* Can not be used for textures */

    inline constexpr static Type glType() { return TypeTraits<T>::glType(); }
    inline constexpr static size_t size() { return sizeof(T); }
    inline constexpr static size_t count() { return vectorSize; }
};

template<class T> struct TypeTraits<Math::Vector2<T>>: public TypeTraits<Math::Vector<T, 2>> {};
template<class T> struct TypeTraits<Math::Vector3<T>>: public TypeTraits<Math::Vector<T, 3>> {};
template<class T> struct TypeTraits<Math::Vector4<T>>: public TypeTraits<Math::Vector<T, 4>> {};

template<class T, size_t matrixSize> struct TypeTraits<Math::Matrix<T, matrixSize>> {
    /* Can not be used for indices */
    /* Can not be used for textures, obviously */

    inline constexpr static Type glType() { return TypeTraits<T>::glType(); }
    inline constexpr static size_t size() { return sizeof(T); }
    inline constexpr static size_t count() { return matrixSize*matrixSize; }
};

template<class T> struct TypeTraits<Math::Matrix3<T>>: public TypeTraits<Math::Matrix<T, 3>> {};
template<class T> struct TypeTraits<Math::Matrix4<T>>: public TypeTraits<Math::Matrix<T, 4>> {};
#endif

}

#endif
