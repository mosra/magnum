#ifndef Magnum_TypeTraits_h
#define Magnum_TypeTraits_h
/*
    Copyright © 2010, 2011 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Magnum::TypeTraits
 */

#include "Magnum.h"

namespace Magnum {

/**
@brief Traits class for plain OpenGL types

Traits classes are usable for detecting type features at compile time without
the need for repeated code such as method overloading or template
specialization for given types.

This class and class methods are specialized only for types where it makes
sense, it has empty implementation for unknown type or type which doesn't
support given feature, thus forcing the compilation stop with an error.
*/
template<class T> struct TypeTraits {
    #ifdef DOXYGEN_GENERATING_OUTPUT
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
     * Returns e.g. GL_UNSIGNED_INT for GLuint.
     */
    constexpr static GLenum glType();

    /**
     * @brief Size of plain OpenGL type
     *
     * Returns sizeof(GLfloat) for GLfloat, but also sizeof(GLfloat) for
     * Vector3. See count().
     */
    constexpr static size_t size();

    /**
     * @brief Count of plain elements in this type
     *
     * Returns 1 for plain OpenGL types like GLint, but e.g. 3 for Vector3.
     */
    constexpr static size_t count();
    #endif
};

/** @todo Other texture types, referenced in glTexImage2D function manual */
/** @todo Using Vector3 for textures? */

#ifndef DOXYGEN_GENERATING_OUTPUT
template<> struct TypeTraits<GLubyte> {
    typedef GLubyte IndexType;
    typedef GLubyte TextureType;

    inline constexpr static GLenum glType() { return GL_UNSIGNED_BYTE; }
    inline constexpr static size_t size() { return sizeof(GLubyte); }
    inline constexpr static size_t count() { return 1; }
};

template<> struct TypeTraits<GLbyte> {
    /* Can not be used for indices */
    typedef GLbyte TextureType;

    inline constexpr static GLenum glType() { return GL_BYTE; }
    inline constexpr static size_t size() { return sizeof(GLbyte); }
    inline constexpr static size_t count() { return 1; }
};

template<> struct TypeTraits<GLushort> {
    typedef GLushort IndexType;
    typedef GLushort TextureType;

    inline constexpr static GLenum glType() { return GL_UNSIGNED_SHORT; }
    inline constexpr static size_t size() { return sizeof(GLushort); }
    inline constexpr static size_t count() { return 1; }
};

template<> struct TypeTraits<GLshort> {
    /* Can not be used for indices */
    typedef GLshort TextureType;

    inline constexpr static GLenum glType() { return GL_SHORT; }
    inline constexpr static size_t size() { return sizeof(GLshort); }
    inline constexpr static size_t count() { return 1; }
};

template<> struct TypeTraits<GLuint> {
    typedef GLuint IndexType;
    typedef GLuint TextureType;

    inline constexpr static GLenum glType() { return GL_UNSIGNED_INT; }
    inline constexpr static size_t size() { return sizeof(GLuint); }
    inline constexpr static size_t count() { return 1; }
};

template<> struct TypeTraits<GLint> {
    /* Can not be used for indices */
    typedef GLint TextureType;

    inline constexpr static GLenum glType() { return GL_INT; }
    inline constexpr static size_t size() { return sizeof(GLint); }
    inline constexpr static size_t count() { return 1; }
};

template<> struct TypeTraits<GLfloat> {
    /* Can not be used for indices */
    typedef GLfloat TextureType;

    inline constexpr static GLenum glType() { return GL_FLOAT; }
    inline constexpr static size_t size() { return sizeof(GLfloat); }
    inline constexpr static size_t count() { return 1; }
};

template<> struct TypeTraits<GLdouble> {
    /* Can not be used for indices */
    /* Can not be used for textures */

    inline constexpr static GLenum glType() { return GL_DOUBLE; }
    inline constexpr static size_t size() { return sizeof(GLdouble); }
    inline constexpr static size_t count() { return 1; }
};

template<class T, size_t vectorSize> struct TypeTraits<Math::Vector<T, vectorSize>> {
    /* Can not be used for indices */
    /* Can not be used for textures */

    inline constexpr static GLenum glType() { return TypeTraits<T>::glType(); }
    inline constexpr static size_t size() { return sizeof(T); }
    inline constexpr static size_t count() { return vectorSize; }
};

template<class T> struct TypeTraits<Math::Vector2<T>>: public TypeTraits<Math::Vector<T, 2>> {};
template<class T> struct TypeTraits<Math::Vector3<T>>: public TypeTraits<Math::Vector<T, 3>> {};
template<class T> struct TypeTraits<Math::Vector4<T>>: public TypeTraits<Math::Vector<T, 4>> {};

template<class T, size_t matrixSize> struct TypeTraits<Math::Matrix<T, matrixSize>> {
    /* Can not be used for indices */
    /* Can not be used for textures, obviously */

    inline constexpr static GLenum glType() { return TypeTraits<T>::glType(); }
    inline constexpr static size_t size() { return sizeof(T); }
    inline constexpr static size_t count() { return matrixSize*matrixSize; }
};

template<class T> struct TypeTraits<Math::Matrix3<T>>: public TypeTraits<Math::Matrix<T, 3>> {};
template<class T> struct TypeTraits<Math::Matrix4<T>>: public TypeTraits<Math::Matrix<T, 4>> {};
#endif

}

#endif
