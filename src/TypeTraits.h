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
 * @brief Class Magnum::TypeTraits
 */

#include <Utility/ConfigurationValue.h>

#include "Math/MathTypeTraits.h"
#include "Magnum.h"

#include "magnumVisibility.h"

namespace Magnum {

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
template<class T> struct TypeTraits {
    TypeTraits() = delete;
};
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
template<> struct TypeTraits<GLubyte>: Math::MathTypeTraits<std::uint8_t> {
    /* Can not be used for attributes */
    inline constexpr static std::size_t size() { return sizeof(GLubyte); }
    inline constexpr static std::size_t count() { return 1; }
};

template<> struct TypeTraits<GLbyte>: Math::MathTypeTraits<std::int8_t> {
    /* Can not be used for attributes */
    inline constexpr static std::size_t size() { return sizeof(GLbyte); }
    inline constexpr static std::size_t count() { return 1; }
};

template<> struct TypeTraits<GLushort>: Math::MathTypeTraits<std::uint16_t> {
    /* Can not be used for attributes */
    inline constexpr static std::size_t size() { return sizeof(GLushort); }
    inline constexpr static std::size_t count() { return 1; }
};

template<> struct TypeTraits<GLshort>: Math::MathTypeTraits<std::int16_t> {
    /* Can not be used for attributes */
    inline constexpr static std::size_t size() { return sizeof(GLshort); }
    inline constexpr static std::size_t count() { return 1; }
};

template<> struct TypeTraits<GLuint>: Math::MathTypeTraits<std::uint32_t> {
    typedef GLuint AttributeType;
    inline constexpr static std::size_t size() { return sizeof(GLuint); }
    inline constexpr static std::size_t count() { return 1; }
};

template<> struct TypeTraits<GLint>: Math::MathTypeTraits<std::int32_t> {
    typedef GLint AttributeType;
    inline constexpr static std::size_t size() { return sizeof(GLint); }
    inline constexpr static std::size_t count() { return 1; }
};

template<> struct TypeTraits<GLfloat>: Math::MathTypeTraits<float> {
    typedef GLfloat AttributeType;
    inline constexpr static std::size_t size() { return sizeof(GLfloat); }
    inline constexpr static std::size_t count() { return 1; }
};

#ifndef MAGNUM_TARGET_GLES
template<> struct TypeTraits<GLdouble>: Math::MathTypeTraits<double> {
    typedef GLdouble AttributeType;
    inline constexpr static std::size_t size() { return sizeof(GLdouble); }
    inline constexpr static std::size_t count() { return 1; }
};
#endif

namespace Implementation {
    template<std::size_t vectorSize, class T> struct VectorTypeTraits {
        VectorTypeTraits() = delete;

        /* Might be used for attributes, see below */
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
        MatrixTypeTraits() = delete;

        /* Might be used for attributes, see below */
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

#endif
