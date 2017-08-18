#ifndef Magnum_Attribute_h
#define Magnum_Attribute_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

/** @file
 * @brief Class @ref Magnum::Attribute
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Magnum.h"
#include "Magnum/OpenGL.h"
#include "Magnum/visibility.h"

namespace Magnum {

namespace Implementation { template<class> struct Attribute; }

/**
@brief Base class for attribute location and type

For use in @ref AbstractShaderProgram subclasses. Template parameter @p location
is vertex attribute location, number between `0` and
@ref AbstractShaderProgram::maxVertexAttributes(). To ensure compatibility, you
should always have vertex attribute with location `0`.

Template parameter @p T is the type which is used for shader attribute, e.g.
@ref Vector4i for `ivec4`. DataType is type of passed data when adding vertex
buffers to mesh. By default it is the same as type used in shader (e.g.
@ref DataType::Int for @ref Vector4i). It's also possible to pass integer data
to floating-point shader inputs. In this case you may want to normalize the
values (e.g. color components from 0-255 to 0.0f - 1.0f) -- see
@ref DataOption::Normalized.

Only some types are allowed as attribute types, see @ref AbstractShaderProgram-types
for more information.

See @ref AbstractShaderProgram-subclassing for example usage in shaders and
@ref Mesh-configuration for example usage when adding vertex buffers to mesh.
*/
template<UnsignedInt location, class T> class Attribute {
    public:
        enum: UnsignedInt {
            /**
             * Location to which the attribute is bound
             *
             * @see @ref AbstractShaderProgram::maxVertexAttributes()
             */
            Location = location,

            /**
             * Count of vectors in this type
             *
             * @see @ref vectorSize()
             */
            VectorCount = Implementation::Attribute<T>::VectorCount
        };

        /**
         * @brief Type
         *
         * Type used in shader code.
         * @see @ref ScalarType, @ref DataType
         */
        typedef T Type;

        /**
         * @brief Scalar type
         *
         * The underlying scalar type of the attribute.
         * @see @ref Type, @ref DataType
         */
        typedef typename Implementation::Attribute<T>::ScalarType ScalarType;

        /**
         * @brief Component count
         *
         * Count of components passed to the shader. If passing smaller count
         * of components than corresponding type has, unspecified components
         * are set to default values (second and third to `0`, fourth to `1`).
         * @see @ref Attribute()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        enum class Components: GLint {
            /**
             * Only first component is specified. Second, third and fourth
             * component are set to `0`, `0`, `1`, respectively. Only for
             * scalar and vector types, not matrices.
             */
            One = 1,

            /**
             * First two components are specified. Third and fourth component
             * are set to `0`, `1`, respectively. Only for two, three and
             * four-component vector types and 2x2, 3x2 and 4x2 matrix types.
             */
            Two = 2,

            /**
             * First three components are specified. Fourth component is set to
             * `1`. Only for three and four-component vector types, 2x3, 3x3
             * and 4x3 matrix types.
             */
            Three = 3,

            /**
             * All four components are specified. Only for four-component
             * vector types and 2x4, 3x4 and 4x4 matrix types.
             */
            Four = 4,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Four components with BGRA ordering. Only for four-component
             * float vector type. Must be used along with @ref DataType::UnsignedByte
             * and @ref DataOption::Normalized.
             * @requires_gl32 Extension @extension{ARB,vertex_array_bgra}
             * @requires_gl Only RGBA component ordering is supported in OpenGL
             *      ES and WebGL.
             */
            BGRA = GL_BGRA
            #endif
        };
        #else
        typedef typename Implementation::Attribute<T>::Components Components;
        #endif

        /**
         * @brief Data type
         *
         * Type of data passed to shader.
         * @see @ref Type, @ref DataOptions, @ref Attribute()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        enum class DataType: GLenum {
            UnsignedByte = GL_UNSIGNED_BYTE,    /**< Unsigned byte */
            Byte = GL_BYTE,                     /**< Byte */
            UnsignedShort = GL_UNSIGNED_SHORT,  /**< Unsigned short */
            Short = GL_SHORT,                   /**< Short */
            UnsignedInt = GL_UNSIGNED_INT,      /**< Unsigned int */
            Int = GL_INT,                       /**< Int */

            #ifndef MAGNUM_TARGET_WEBGL
            /**
             * Half float. Only for float attribute types.
             * @requires_gl30 Extension @extension{ARB,half_float_vertex}
             * @requires_gles30 Extension @extension{OES,vertex_half_float}
             *      in OpenGL ES 2.0
             * @requires_webgl20 Half float vertex attributes are not available
             *      in WebGL 1.0.
             */
            #ifndef MAGNUM_TARGET_GLES2
            HalfFloat = GL_HALF_FLOAT,
            #else
            HalfFloat = GL_HALF_FLOAT_OES,
            #endif
            #endif

            /** Float. Only for float attribute types. */
            Float = GL_FLOAT,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Double. Only for float and double attribute types.
             * @requires_gl Only floats are available in OpenGL ES or WebGL.
             */
            Double = GL_DOUBLE,

            /**
             * Unsigned 10.11.11 packed float. Only for three-component float
             * vector attribute type.
             * @requires_gl44 Extension @extension{ARB,vertex_type_10f_11f_11f_rev}
             * @requires_gl Packed float attributes are not available in OpenGL
             *      ES or WebGL.
             */
            UnsignedInt10f11f11fRev = GL_UNSIGNED_INT_10F_11F_11F_REV,
            #endif

            /* GL_FIXED not supported */

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Unsigned 2.10.10.10 packed integer. Only for four-component
             * float vector attribute type.
             * @todo How about (incompatible) @extension{OES,vertex_type_10_10_10_2}?
             * @requires_gl33 Extension @extension{ARB,vertex_type_2_10_10_10_rev}
             * @requires_gles30 Packed attributes are not available in OpenGL
             *      ES 2.0.
             * @requires_webgl20 Packed attributes are not available in WebGL
             *      1.0.
             */
            UnsignedInt2101010Rev = GL_UNSIGNED_INT_2_10_10_10_REV,

            /**
             * Signed 2.10.10.10 packed integer. Only for four-component float
             * vector attribute type.
             * @requires_gl33 Extension @extension{ARB,vertex_type_2_10_10_10_rev}
             * @requires_gles30 Packed attributes are not available in OpenGL
             *      ES 2.0.
             * @requires_webgl20 Packed attributes are not available in WebGL
             *      1.0.
             */
            Int2101010Rev = GL_INT_2_10_10_10_REV
            #endif
        };
        #else
        typedef typename Implementation::Attribute<T>::DataType DataType;
        #endif

        /**
         * @brief Data option
         * @see @ref DataOptions, @ref Attribute()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        enum class DataOption: UnsignedByte {
            /**
             * Normalize integer components. Only for float attribute types.
             * Default is to not normalize.
             */
            Normalized = 1 << 0
        };
        #else
        typedef typename Implementation::Attribute<T>::DataOption DataOption;
        #endif

        /**
         * @brief Data options
         * @see @ref Attribute()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        typedef typename Containers::EnumSet<DataOption> DataOptions;
        #else
        typedef typename Implementation::Attribute<T>::DataOptions DataOptions;
        #endif

        /**
         * @brief Constructor
         * @param components    Component count
         * @param dataType      Type of passed data. Default is the same as
         *      type used in shader (e.g. @ref DataType::Int for @ref Vector4i).
         * @param dataOptions   Data options. Default is no options.
         */
        constexpr Attribute(Components components, DataType dataType = Implementation::Attribute<T>::DefaultDataType, DataOptions dataOptions = DataOptions()): _components(components), _dataType(dataType), _dataOptions(dataOptions) {}

        /**
         * @brief Constructor
         * @param dataType      Type of passed data. Default is the same as
         *      type used in shader (e.g. @ref DataType::Int for @ref Vector4i).
         * @param dataOptions   Data options. Default is no options.
         *
         * Component count is set to the same value as in type used in shader
         * (e.g. @ref Components::Three for @ref Vector3).
         */
        constexpr Attribute(DataType dataType = Implementation::Attribute<T>::DefaultDataType, DataOptions dataOptions = DataOptions()): _components(Implementation::Attribute<T>::DefaultComponents), _dataType(dataType), _dataOptions(dataOptions) {}

        /** @brief Component count of passed data */
        constexpr Components components() const { return _components; }

        /** @brief Type of passed data */
        constexpr DataType dataType() const { return _dataType; }

        /**
         * @brief Size of each vector in passed data
         *
         * @see @ref VectorCount
         */
        UnsignedInt vectorSize() const {
            return Implementation::Attribute<T>::size(GLint(_components), _dataType);
        }

        /** @brief Data options */
        constexpr DataOptions dataOptions() const { return _dataOptions; }

    private:
        Components _components;
        DataType _dataType;
        DataOptions _dataOptions;
};

#ifdef DOXYGEN_GENERATING_OUTPUT
/** @debugoperatorclassenum{Magnum::Attribute,Magnum::Attribute::Components} */
template<class T> Debug& operator<<(Debug& debug, Attribute<T>::Components);

/** @debugoperatorclassenum{Magnum::Attribute,Magnum::Attribute::DataType} */
template<class T> Debug& operator<<(Debug& debug, Attribute<T>::DataType);
#endif

/**
@brief Base class for dynamic attribute location and type

Counterpart to @ref Attribute that allows runtime specification of attribute
location and base type. Note that unlike the compile-time specification, this
class doesn't do any sanity verification and leaves most of the responsibility
on the user.
*/
class DynamicAttribute {
    public:
        /**
         * @brief Attribute kind
         *
         * Specifies what kind of shader type matches the attribute.
         * @see @ref DynamicAttribute()
         */
        enum class Kind {
            /** Generic, matches single-precision floating-point shader type */
            Generic,

            /**
             * Normalized integral, matches single-precision floating-point
             * shader type
             */
            GenericNormalized,

            #ifndef MAGNUM_TARGET_GLES2
            /** Integral, matches integral shader type */
            Integral,
            #ifndef MAGNUM_TARGET_GLES

            /** Long, matches double-precision shader type */
            Long
            #endif
            #endif
        };

        /**
         * @brief Component count
         *
         * Count of components passed to the shader. If passing smaller count
         * of components than corresponding type has, unspecified components
         * are set to default values (second and third to `0`, fourth to `1`).
         * @see @ref DynamicAttribute()
         */
        enum class Components: GLint {
            /**
             * Only first component is specified. Second, third and fourth
             * component are set to `0`, `0`, `1`, respectively. Only for
             * scalar and vector types, not matrices.
             */
            One = 1,

            /**
             * First two components are specified. Third and fourth component
             * are set to `0`, `1`, respectively. Only for two, three and
             * four-component vector types and 2x2, 3x2 and 4x2 matrix types.
             */
            Two = 2,

            /**
             * First three components are specified. Fourth component is set to
             * `1`. Only for three and four-component vector types, 2x3, 3x3
             * and 4x3 matrix types.
             */
            Three = 3,

            /**
             * All four components are specified. Only for four-component
             * vector types and 2x4, 3x4 and 4x4 matrix types.
             */
            Four = 4,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Four components with BGRA ordering. Only for four-component
             * float vector type. Must be used along with @ref DataType::UnsignedByte
             * and @ref Kind::GenericNormalized.
             * @requires_gl32 Extension @extension{ARB,vertex_array_bgra}
             * @requires_gl Only RGBA component ordering is supported in OpenGL
             *      ES and WebGL.
             */
            BGRA = GL_BGRA
            #endif
        };

        /**
         * @brief Data type
         *
         * Type of data passed to shader.
         * @see @ref Kind, @ref DynamicAttribute()
         */
        enum class DataType: GLenum {
            UnsignedByte = GL_UNSIGNED_BYTE,    /**< Unsigned byte */
            Byte = GL_BYTE,                     /**< Byte */
            UnsignedShort = GL_UNSIGNED_SHORT,  /**< Unsigned short */
            Short = GL_SHORT,                   /**< Short */
            UnsignedInt = GL_UNSIGNED_INT,      /**< Unsigned int */
            Int = GL_INT,                       /**< Int */

            #ifndef MAGNUM_TARGET_WEBGL
            /**
             * Half float. Only for float attribute types.
             * @requires_gl30 Extension @extension{ARB,half_float_vertex}
             * @requires_gles30 Extension @extension{OES,vertex_half_float}
             *      in OpenGL ES 2.0
             * @requires_webgl20 Half float vertex attributes are not available
             *      in WebGL 1.0.
             */
            #ifndef MAGNUM_TARGET_GLES2
            HalfFloat = GL_HALF_FLOAT,
            #else
            HalfFloat = GL_HALF_FLOAT_OES,
            #endif
            #endif

            /** Float. Only for float attribute types. */
            Float = GL_FLOAT,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Double. Only for float and double attribute types.
             * @requires_gl Only floats are available in OpenGL ES or WebGL.
             */
            Double = GL_DOUBLE,

            /**
             * Unsigned 10.11.11 packed float. Only for three-component float
             * vector attribute type.
             * @requires_gl44 Extension @extension{ARB,vertex_type_10f_11f_11f_rev}
             * @requires_gl Packed float attributes are not available in OpenGL
             *      ES or WebGL.
             */
            UnsignedInt10f11f11fRev = GL_UNSIGNED_INT_10F_11F_11F_REV,
            #endif

            /* GL_FIXED not supported */

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Unsigned 2.10.10.10 packed integer. Only for four-component
             * float vector attribute type.
             * @todo How about (incompatible) @extension{OES,vertex_type_10_10_10_2}?
             * @requires_gl33 Extension @extension{ARB,vertex_type_2_10_10_10_rev}
             * @requires_gles30 Packed attributes are not available in OpenGL
             *      ES 2.0.
             * @requires_webgl20 Packed attributes are not available in WebGL
             *      1.0.
             */
            UnsignedInt2101010Rev = GL_UNSIGNED_INT_2_10_10_10_REV,

            /**
             * Signed 2.10.10.10 packed integer. Only for four-component float
             * vector attribute type.
             * @requires_gl33 Extension @extension{ARB,vertex_type_2_10_10_10_rev}
             * @requires_gles30 Packed attributes are not available in OpenGL
             *      ES 2.0.
             * @requires_webgl20 Packed attributes are not available in WebGL
             *      1.0.
             */
            Int2101010Rev = GL_INT_2_10_10_10_REV
            #endif
        };

        /**
         * @brief Constructor
         * @param kind          Attribute kind
         * @param location      Attribute location
         * @param components    Component count
         * @param dataType      Type of passed data
         */
        constexpr DynamicAttribute(Kind kind, UnsignedInt location, Components components, DataType dataType): _kind{kind}, _location{location}, _components{components}, _dataType{dataType} {}

        /** @brief Attribute kind */
        constexpr Kind kind() const { return _kind; }

        /** @brief Attribute location */
        constexpr UnsignedInt location() const { return _location; }

        /** @brief Component count of passed data */
        constexpr Components components() const { return _components; }

        /** @brief Type of passed data */
        constexpr DataType dataType() const { return _dataType; }

    private:
        Kind _kind;
        UnsignedInt _location;
        Components _components;
        DataType _dataType;
};

namespace Implementation {

/* Base for sized attributes */
template<std::size_t cols, std::size_t rows> struct SizedAttribute;

/* Vector attribute sizes */
template<std::size_t cols> struct SizedVectorAttribute {
    enum: UnsignedInt { VectorCount = UnsignedInt(cols) };
};
template<> struct SizedAttribute<1, 1>: SizedVectorAttribute<1> {
    enum class Components: GLint { One = 1 };
    constexpr static Components DefaultComponents = Components::One;
};
template<> struct SizedAttribute<1, 2>: SizedVectorAttribute<1> {
    enum class Components: GLint { One = 1, Two = 2 };
    constexpr static Components DefaultComponents = Components::Two;
};
template<> struct SizedAttribute<1, 3>: SizedVectorAttribute<1> {
    enum class Components: GLint { One = 1, Two = 2, Three = 3 };
    constexpr static Components DefaultComponents = Components::Three;
};
template<> struct SizedAttribute<1, 4>: SizedVectorAttribute<1> {
    enum class Components: GLint { One = 1, Two = 2, Three = 3, Four = 4 };
    constexpr static Components DefaultComponents = Components::Four;
};
MAGNUM_EXPORT Debug& operator<<(Debug& debug, SizedAttribute<1, 1>::Components value);
MAGNUM_EXPORT Debug& operator<<(Debug& debug, SizedAttribute<1, 2>::Components value);
MAGNUM_EXPORT Debug& operator<<(Debug& debug, SizedAttribute<1, 3>::Components value);
MAGNUM_EXPORT Debug& operator<<(Debug& debug, SizedAttribute<1, 4>::Components value);

/* Matrix attribute sizes */
template<std::size_t rows> struct SizedMatrixAttribute;
template<> struct SizedMatrixAttribute<2> {
    enum class Components: GLint { Two   = 2 };
    constexpr static Components DefaultComponents = Components::Two;
};
template<> struct SizedMatrixAttribute<3> {
    enum class Components: GLint { Three = 3 };
    constexpr static Components DefaultComponents = Components::Three;
};
template<> struct SizedMatrixAttribute<4> {
    enum class Components: GLint { Four  = 4 };
    constexpr static Components DefaultComponents = Components::Four;
};
MAGNUM_EXPORT Debug& operator<<(Debug& debug, SizedMatrixAttribute<2>::Components value);
MAGNUM_EXPORT Debug& operator<<(Debug& debug, SizedMatrixAttribute<3>::Components value);
MAGNUM_EXPORT Debug& operator<<(Debug& debug, SizedMatrixAttribute<4>::Components value);
template<> struct SizedAttribute<2, 2>: SizedVectorAttribute<2>, SizedMatrixAttribute<2> {};
template<> struct SizedAttribute<3, 3>: SizedVectorAttribute<3>, SizedMatrixAttribute<3> {};
template<> struct SizedAttribute<4, 4>: SizedVectorAttribute<4>, SizedMatrixAttribute<4> {};
#ifndef MAGNUM_TARGET_GLES2
template<> struct SizedAttribute<2, 3>: SizedVectorAttribute<2>, SizedMatrixAttribute<3> {};
template<> struct SizedAttribute<3, 2>: SizedVectorAttribute<3>, SizedMatrixAttribute<2> {};
template<> struct SizedAttribute<2, 4>: SizedVectorAttribute<2>, SizedMatrixAttribute<4> {};
template<> struct SizedAttribute<4, 2>: SizedVectorAttribute<4>, SizedMatrixAttribute<2> {};
template<> struct SizedAttribute<3, 4>: SizedVectorAttribute<3>, SizedMatrixAttribute<4> {};
template<> struct SizedAttribute<4, 3>: SizedVectorAttribute<4>, SizedMatrixAttribute<3> {};
#endif

/* Base for attributes */
template<class> struct Attribute;

/* Base for float attributes */
struct FloatAttribute {
    typedef Float ScalarType;

    enum class DataType: GLenum {
        UnsignedByte = GL_UNSIGNED_BYTE,
        Byte = GL_BYTE,
        UnsignedShort = GL_UNSIGNED_SHORT,
        Short = GL_SHORT,
        UnsignedInt = GL_UNSIGNED_INT,
        Int = GL_INT,
        #ifndef MAGNUM_TARGET_GLES2
        HalfFloat = GL_HALF_FLOAT,
        #else
        HalfFloat = GL_HALF_FLOAT_OES,
        #endif
        Float = GL_FLOAT

        #ifndef MAGNUM_TARGET_GLES
        ,
        Double = GL_DOUBLE
        #endif
    };
    constexpr static DataType DefaultDataType = DataType::Float;

    enum class DataOption: UnsignedByte {
        Normalized = 1 << 0
    };
    typedef Containers::EnumSet<DataOption> DataOptions;

    static UnsignedInt MAGNUM_EXPORT size(GLint components, DataType dataType);
};

CORRADE_ENUMSET_OPERATORS(FloatAttribute::DataOptions)

MAGNUM_EXPORT Debug& operator<<(Debug& debug, FloatAttribute::DataType value);

#ifndef MAGNUM_TARGET_GLES2
/* Base for int attributes */
struct IntAttribute {
    typedef Int ScalarType;

    enum class DataType: GLenum {
        UnsignedByte = GL_UNSIGNED_BYTE,
        Byte = GL_BYTE,
        UnsignedShort = GL_UNSIGNED_SHORT,
        Short = GL_SHORT,
        UnsignedInt = GL_UNSIGNED_INT,
        Int = GL_INT
    };
    constexpr static DataType DefaultDataType = DataType::Int;

    enum class DataOption: UnsignedByte {};
    typedef Containers::EnumSet<DataOption> DataOptions;

    static UnsignedInt MAGNUM_EXPORT size(GLint components, DataType dataType);
};

CORRADE_ENUMSET_OPERATORS(IntAttribute::DataOptions)

MAGNUM_EXPORT Debug& operator<<(Debug& debug, IntAttribute::DataType value);

/* Base for unsigned int attributes */
struct UnsignedIntAttribute {
    typedef UnsignedInt ScalarType;

    typedef IntAttribute::DataType DataType;
    constexpr static DataType DefaultDataType = DataType::UnsignedInt;

    typedef IntAttribute::DataOption DataOption;
    typedef IntAttribute::DataOptions DataOptions;

    static UnsignedInt size(GLint components, DataType dataType) {
        return IntAttribute::size(components, dataType);
    }
};
#endif

#ifndef MAGNUM_TARGET_GLES
/* Base for double attributes */
struct DoubleAttribute {
    typedef Double ScalarType;

    enum class DataType: GLenum {
        Double = GL_DOUBLE
    };
    constexpr static DataType DefaultDataType = DataType::Double;

    typedef IntAttribute::DataOption DataOption;
    typedef IntAttribute::DataOptions DataOptions;

    static UnsignedInt MAGNUM_EXPORT size(GLint components, DataType dataType);
};

MAGNUM_EXPORT Debug& operator<<(Debug& debug, DoubleAttribute::DataType value);
#endif

/* Floating-point three-component vector has additional data type compared to
   classic floats */
template<> struct Attribute<Math::Vector<3, Float>>: SizedAttribute<1, 3> {
    typedef Float ScalarType;

    enum class DataType: GLenum {
        UnsignedByte = GL_UNSIGNED_BYTE,
        Byte = GL_BYTE,
        UnsignedShort = GL_UNSIGNED_SHORT,
        Short = GL_SHORT,
        UnsignedInt = GL_UNSIGNED_INT,
        Int = GL_INT,
        #ifndef MAGNUM_TARGET_GLES2
        HalfFloat = GL_HALF_FLOAT,
        #else
        HalfFloat = GL_HALF_FLOAT_OES,
        #endif
        Float = GL_FLOAT

        #ifndef MAGNUM_TARGET_GLES
        ,
        Double = GL_DOUBLE,
        UnsignedInt10f11f11fRev = GL_UNSIGNED_INT_10F_11F_11F_REV
        #endif
    };
    constexpr static DataType DefaultDataType = DataType::Float;

    typedef FloatAttribute::DataOption DataOption;
    typedef FloatAttribute::DataOptions DataOptions;

    static UnsignedInt MAGNUM_EXPORT size(GLint components, DataType dataType);
};

MAGNUM_EXPORT Debug& operator<<(Debug& debug, Attribute<Math::Vector<3, Float>>::DataType value);

/* Floating-point four-component vector is absolutely special case */
template<> struct Attribute<Math::Vector<4, Float>> {
    typedef Float ScalarType;

    enum class Components: GLint {
        One = 1,
        Two = 2,
        Three = 3,
        Four = 4
        #ifndef MAGNUM_TARGET_GLES
        ,
        BGRA = GL_BGRA
        #endif
    };
    constexpr static Components DefaultComponents = Components::Four;

    enum class DataType: GLenum {
        UnsignedByte = GL_UNSIGNED_BYTE,
        Byte = GL_BYTE,
        UnsignedShort = GL_UNSIGNED_SHORT,
        Short = GL_SHORT,
        UnsignedInt = GL_UNSIGNED_INT,
        Int = GL_INT,
        #ifndef MAGNUM_TARGET_GLES2
        HalfFloat = GL_HALF_FLOAT,
        #else
        HalfFloat = GL_HALF_FLOAT_OES,
        #endif
        Float = GL_FLOAT
        #ifndef MAGNUM_TARGET_GLES
        ,
        Double = GL_DOUBLE
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        ,
        UnsignedInt2101010Rev = GL_UNSIGNED_INT_2_10_10_10_REV,
        Int2101010Rev = GL_INT_2_10_10_10_REV
        #endif
    };
    constexpr static DataType DefaultDataType = DataType::Float;

    typedef FloatAttribute::DataOption DataOption;
    typedef FloatAttribute::DataOptions DataOptions;

    enum: UnsignedInt { VectorCount = 1 };

    static UnsignedInt MAGNUM_EXPORT size(GLint components, DataType dataType);
};

MAGNUM_EXPORT Debug& operator<<(Debug& debug, Attribute<Math::Vector<4, Float>>::Components value);
MAGNUM_EXPORT Debug& operator<<(Debug& debug, Attribute<Math::Vector<4, Float>>::DataType value);

/* Common float, int, unsigned int and double scalar attributes */
template<> struct Attribute<Float>: FloatAttribute, SizedAttribute<1, 1> {};
#ifndef MAGNUM_TARGET_GLES2
template<> struct Attribute<Int>: IntAttribute, SizedAttribute<1, 1> {};
template<> struct Attribute<UnsignedInt>: UnsignedIntAttribute, SizedAttribute<1, 1> {};
#ifndef MAGNUM_TARGET_GLES
template<> struct Attribute<Double>: DoubleAttribute, SizedAttribute<1, 1> {};
#endif
#endif

/* Common float, int, unsigned int and double vector attributes */
template<std::size_t size_> struct Attribute<Math::Vector<size_, Float>>: FloatAttribute, SizedAttribute<1, size_> {};
#ifndef MAGNUM_TARGET_GLES2
template<std::size_t size_> struct Attribute<Math::Vector<size_, Int>>: IntAttribute, SizedAttribute<1, size_> {};
template<std::size_t size_> struct Attribute<Math::Vector<size_, UnsignedInt>>: UnsignedIntAttribute, SizedAttribute<1, size_> {};
#ifndef MAGNUM_TARGET_GLES
template<std::size_t size_> struct Attribute<Math::Vector<size_, Double>>: DoubleAttribute, SizedAttribute<1, size_> {};
#endif
#endif
template<class T> struct Attribute<Math::Vector2<T>>: Attribute<Math::Vector<2, T>> {};
template<class T> struct Attribute<Math::Vector3<T>>: Attribute<Math::Vector<3, T>> {};
template<class T> struct Attribute<Math::Vector4<T>>: Attribute<Math::Vector<4, T>> {};
template<class T> struct Attribute<Math::Color3<T>>: Attribute<Math::Vector3<T>> {};
template<class T> struct Attribute<Math::Color4<T>>: Attribute<Math::Vector4<T>> {};

/* Common float and double rectangular matrix attributes */
template<std::size_t cols, std::size_t rows> struct Attribute<Math::RectangularMatrix<cols, rows, Float>>: FloatAttribute, SizedAttribute<cols, rows> {};
#ifndef MAGNUM_TARGET_GLES
template<std::size_t cols, std::size_t rows> struct Attribute<Math::RectangularMatrix<cols, rows, Double>>: DoubleAttribute, SizedAttribute<cols, rows> {};
#endif

/* Common float and double square matrix attributes */
template<std::size_t size_> struct Attribute<Math::Matrix<size_, Float>>: Attribute<Math::RectangularMatrix<size_, size_, Float>> {};
#ifndef MAGNUM_TARGET_GLES
template<std::size_t size_> struct Attribute<Math::Matrix<size_, Double>>: Attribute<Math::RectangularMatrix<size_, size_, Double>> {};
#endif
template<class T> struct Attribute<Math::Matrix3<T>>: Attribute<Math::Matrix<3, T>> {};
template<class T> struct Attribute<Math::Matrix4<T>>: Attribute<Math::Matrix<4, T>> {};

}

}

#endif
