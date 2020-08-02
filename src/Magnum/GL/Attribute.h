#ifndef Magnum_GL_Attribute_h
#define Magnum_GL_Attribute_h
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

/** @file
 * @brief Class @ref Magnum::GL::Attribute, @ref Magnum::GL::DynamicAttribute, function @ref Magnum::GL::hasVertexFormat()
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Magnum.h"
#include "Magnum/GL/OpenGL.h"
#include "Magnum/GL/visibility.h"
#include "Magnum/Math/TypeTraits.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>
#endif

namespace Magnum { namespace GL {

namespace Implementation { template<class> struct Attribute; }

/**
@brief Base class for vertex attribute location and type

For use in @ref AbstractShaderProgram subclasses. The @p location template
parameter is vertex attribute location, a number between @cpp 0 @ce and
@ref AbstractShaderProgram::maxVertexAttributes(). To ensure compatibility, you
should always have a vertex attribute with location @cpp 0 @ce.

The @p T template parameter is the type which is used for shader attribute,
e.g. @ref Magnum::Vector4i "Vector4i" for @glsl ivec4 @ce. @ref DataType is
type of passed data when adding vertex buffers to mesh. By default it is the
same as type used in shader (e.g. @ref DataType::Int for
@ref Magnum::Vector4i "Vector4i"). It's also possible to pass integer data to
floating-point shader inputs. In this case you may want to normalize the values
(e.g. color components from @cpp 0 @ce -- @cpp 255 @ce to @cpp 0.0f @ce --
@cpp 1.0f @ce) --- see @ref DataOption::Normalized.

@attention Only some types are allowed as attribute types and there are
    additional restrictions applied for OpenGL ES and WebGL, see
    @ref GL-AbstractShaderProgram-types for more information.

See @ref GL-AbstractShaderProgram-subclassing "AbstractShaderProgram subclassing workflow"
for example usage in shaders and @ref GL-Mesh-configuration for example usage
when adding vertex buffers to a mesh. @ref DynamicAttribute is a variant with
runtime-specified location and type, and is convertible from @ref VertexFormat.
*/
template<UnsignedInt location, class T> class Attribute {
    #ifdef MAGNUM_TARGET_GLES2
    static_assert(!Math::IsIntegral<T>::value, "integral attributes are not available on OpenGL ES 2.0 and WebGL 1");
    #endif
    #ifdef MAGNUM_TARGET_GLES
    static_assert(!std::is_same<Math::UnderlyingTypeOf<T>, Double>::value, "double attributes and uniforms are not available in OpenGL ES and WebGL");
    #endif

    public:
        enum: UnsignedInt {
            /**
             * Location to which the attribute is bound.
             *
             * @see @ref AbstractShaderProgram::maxVertexAttributes()
             */
            Location = location,

            /**
             * Count of vectors in this type.
             * @m_since{2020,06}
             *
             * Is @cpp 1 @ce for non-matrix attributes.
             * @see @ref vectorSize()
             */
            Vectors = Implementation::Attribute<T>::Vectors,

            #ifdef MAGNUM_BUILD_DEPRECATED
            /**
             * Count of vectors in this type,
             * @m_deprecated_since{2020,06} Use @ref Vectors instead.
             */
            VectorCount CORRADE_DEPRECATED_ENUM("use Vectors instead") = Vectors
            #endif
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
         * are set to default values (second and third to @cpp 0 @ce, fourth to
         * @cpp 1 @ce).
         * @see @ref Attribute()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        enum class Components: GLint {
            /**
             * Only first component is specified. Second, third and fourth
             * component are set to @cpp 0 @ce, @cpp 0 @ce, @cpp 1 @ce,
             * respectively. Only for scalar and vector types, not matrices.
             */
            One = 1,

            /**
             * First two components are specified. Third and fourth component
             * are set to @cpp 0 @ce, @cpp 1 @ce, respectively. Only for two,
             * three and four-component vector types and 2x2, 3x2 and 4x2
             * matrix types.
             */
            Two = 2,

            /**
             * First three components are specified. Fourth component is set to
             * @cpp 1 @ce. Only for three and four-component vector types, 2x3,
             * 3x3 and 4x3 matrix types.
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
             * @m_keywords{GL_BGRA}
             * @requires_gl32 Extension @gl_extension{ARB,vertex_array_bgra}
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
         * @m_enum_values_as_keywords
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
             * @m_since{2020,06}
             * @requires_gl30 Extension @gl_extension{ARB,half_float_vertex}
             * @requires_gles30 Extension @gl_extension{OES,vertex_half_float}
             *      in OpenGL ES 2.0
             * @requires_webgl20 Half float vertex attributes are not available
             *      in WebGL 1.0.
             */
            #ifndef MAGNUM_TARGET_GLES2
            Half = GL_HALF_FLOAT,
            #else
            Half = GL_HALF_FLOAT_OES,
            #endif

            #ifdef MAGNUM_BUILD_DEPRECATED
            /**
             * Half float.
             * @m_deprecated_since{2020,06} Use @ref DataType::Half instead.
             */
            HalfFloat CORRADE_DEPRECATED_ENUM("use Half instead") = Half,
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
             * @requires_gl44 Extension @gl_extension{ARB,vertex_type_10f_11f_11f_rev}
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
             * @todo How about (incompatible) @gl_extension{OES,vertex_type_10_10_10_2}?
             * @requires_gl33 Extension @gl_extension{ARB,vertex_type_2_10_10_10_rev}
             * @requires_gles30 Packed attributes are not available in OpenGL
             *      ES 2.0.
             * @requires_webgl20 Packed attributes are not available in WebGL
             *      1.0.
             */
            UnsignedInt2101010Rev = GL_UNSIGNED_INT_2_10_10_10_REV,

            /**
             * Signed 2.10.10.10 packed integer. Only for four-component float
             * vector attribute type.
             * @requires_gl33 Extension @gl_extension{ARB,vertex_type_2_10_10_10_rev}
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
         * @param dataType      Type of passed data. Default is the same as
         *      type used in shader (e.g. @ref DataType::Int for
         *      @ref Magnum::Vector4i "Vector4i").
         * @param dataOptions   Data options. Default is no options.
         *
         * Component count is set to the same value as in type used in shader
         * (e.g. @ref Components::Three for @ref Magnum::Vector3 "Vector3").
         */
        constexpr Attribute(DataType dataType = Implementation::Attribute<T>::DefaultDataType, DataOptions dataOptions = DataOptions()): Attribute{Implementation::Attribute<T>::DefaultComponents, dataType, dataOptions} {}

        /**
         * @brief Constructor
         * @param components    Component count
         * @param dataType      Type of passed data. Default is the same as
         *      type used in shader (e.g. @ref DataType::Int for
         *      @ref Magnum::Vector4i "Vector4i").
         * @param dataOptions   Data options. Default is no options.
         *
         * Vector stride is set to the size of the vector type (e.g. 9 for a
         * @ref Magnum::Matrix3 "Matrix3").
         */
        constexpr Attribute(Components components, DataType dataType = Implementation::Attribute<T>::DefaultDataType, DataOptions dataOptions = DataOptions()): Attribute{components, Implementation::Attribute<T>::size(GLint(components), dataType), dataType, dataOptions} {}

        /**
         * @brief Construct with a custom vector stride
         * @param vectorStride  Stride between consecutive matrix column
         *      vectors
         * @param dataType      Type of passed data. Default is the same as
         *      type used in shader (e.g. @ref DataType::Int for
         *      @ref Magnum::Vector4i "Vector4i").
         * @param dataOptions   Data options. Default is no options.
         * @m_since{2020,06}
         *
         * Component count is set to the same value as in type used in shader
         * (e.g. @ref Components::Three for @ref Magnum::Vector3 "Vector3").
         */
        constexpr Attribute(UnsignedInt vectorStride, DataType dataType = Implementation::Attribute<T>::DefaultDataType, DataOptions dataOptions = DataOptions()): Attribute{Implementation::Attribute<T>::DefaultComponents, vectorStride, dataType, dataOptions} {}

        /**
         * @brief Construct with a custom vector stride
         * @param components    Component count
         * @param vectorStride  Stride between consecutive matrix column
         *      vectors
         * @param dataType      Type of passed data. Default is the same as
         *      type used in shader (e.g. @ref DataType::Int for
         *      @ref Magnum::Vector4i "Vector4i").
         * @param dataOptions   Data options. Default is no options.
         * @m_since{2020,06}
         */
        constexpr Attribute(Components components, UnsignedInt vectorStride, DataType dataType = Implementation::Attribute<T>::DefaultDataType, DataOptions dataOptions = DataOptions()): _components{components}, _vectorStride{vectorStride}, _dataType{dataType}, _dataOptions{dataOptions} {}

        /** @brief Component count of passed data */
        constexpr Components components() const { return _components; }

        /** @brief Type of passed data */
        constexpr DataType dataType() const { return _dataType; }

        /**
         * @brief Stride between consecutive vector elements
         * @m_since{2020,06}
         *
         * Used for describing matrix attributes. Implicitly the same as size
         * of given vector type (e.g. @cpp 9 @ce for a
         * @ref Magnum::Matrix3 "Matrix3"), but can be overriden for example to
         * ensure four-byte column alignment with 1- and 2-byte data types.
         * @see @ref Vectors
         */
        constexpr UnsignedInt vectorStride() const { return _vectorStride; }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Size of each vector in passed data
         * @m_deprecated_since{2020,06} Use @ref vectorStride() instead.
         *
         * @see @ref Vectors
         */
        constexpr CORRADE_DEPRECATED("use vectorStride() instead") UnsignedInt vectorSize() const {
            return vectorStride();
        }
        #endif

        /** @brief Data options */
        constexpr DataOptions dataOptions() const { return _dataOptions; }

    private:
        Components _components;
        UnsignedInt _vectorStride;
        DataType _dataType;
        DataOptions _dataOptions;
};

#ifdef DOXYGEN_GENERATING_OUTPUT
/** @debugoperatorclassenum{Attribute,Attribute::Components} */
template<UnsignedInt location, class T> Debug& operator<<(Debug& debug, Attribute<location, T>::Components);

/** @debugoperatorclassenum{Attribute,Attribute::DataType} */
template<UnsignedInt location, class T> Debug& operator<<(Debug& debug, Attribute<location, T>::DataType);
#endif

/**
@brief Base class for dynamic vertex attribute location and type

Counterpart to @ref Attribute that allows runtime specification of attribute
location and base type. Note that unlike the compile-time specification, this
class doesn't do any sanity verification and leaves most of the responsibility
on the user.
*/
class MAGNUM_GL_EXPORT DynamicAttribute {
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
            /**
             * Integral, matches integral shader type
             *
             * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
             * @requires_gles30 Integral attributes are not available in OpenGL
             *      ES 2.0.
             * @requires_webgl20 Integral attributes are not available in WebGL
             *      1.0.
             */
            Integral,
            #ifndef MAGNUM_TARGET_GLES

            /**
             * Long, matches double-precision shader type
             * @requires_gl40 Extension @gl_extension{ARB,gpu_shader_fp64}
             * @requires_gl Double attributes and uniforms are not available in
             *      OpenGL ES and WebGL.
             */
            Long
            #endif
            #endif
        };

        /**
         * @brief Component count
         *
         * Count of components passed to the shader. If passing smaller count
         * of components than corresponding type has, unspecified components
         * are set to default values (second and third to @cpp 0 @ce, fourth to
         * @cpp 1 @ce).
         * @see @ref DynamicAttribute()
         */
        enum class Components: GLint {
            /**
             * Only first component is specified. Second, third and fourth
             * component are set to @cpp 0 @ce, @cpp 0 @ce, @cpp 1 @ce,
             * respectively. Only for scalar and vector types, not matrices.
             */
            One = 1,

            /**
             * First two components are specified. Third and fourth component
             * are set to @cpp 0 @ce, @cpp 1 @ce, respectively. Only for two,
             * three and four-component vector types and 2x2, 3x2 and 4x2
             * matrix types.
             */
            Two = 2,

            /**
             * First three components are specified. Fourth component is set to
             * @cpp 1 @ce. Only for three and four-component vector types, 2x3,
             * 3x3 and 4x3 matrix types.
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
             * @m_keywords{GL_BGRA}
             * @requires_gl32 Extension @gl_extension{ARB,vertex_array_bgra}
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
         * @m_enum_values_as_keywords
         */
        enum class DataType: GLenum {
            UnsignedByte = GL_UNSIGNED_BYTE,    /**< Unsigned byte */
            Byte = GL_BYTE,                     /**< Byte */
            UnsignedShort = GL_UNSIGNED_SHORT,  /**< Unsigned short */
            Short = GL_SHORT,                   /**< Short */
            UnsignedInt = GL_UNSIGNED_INT,      /**< Unsigned int */
            Int = GL_INT,                       /**< Int */

            #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
            /**
             * Half float. Only for float attribute types.
             * @m_since{2020,06}
             * @requires_gl30 Extension @gl_extension{ARB,half_float_vertex}
             * @requires_gles30 Extension @gl_extension{OES,vertex_half_float}
             *      in OpenGL ES 2.0
             * @requires_webgl20 Half float vertex attributes are not available
             *      in WebGL 1.0.
             */
            #ifndef MAGNUM_TARGET_GLES2
            Half = GL_HALF_FLOAT,
            #else
            Half = GL_HALF_FLOAT_OES,
            #endif

            #ifdef MAGNUM_BUILD_DEPRECATED
            /**
             * Half float.
             * @m_deprecated_since{2020,06} Use @ref DataType::Half instead.
             */
            HalfFloat CORRADE_DEPRECATED_ENUM("use Half instead") = Half,
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
             * @requires_gl44 Extension @gl_extension{ARB,vertex_type_10f_11f_11f_rev}
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
             * @todo How about (incompatible) @gl_extension{OES,vertex_type_10_10_10_2}?
             * @requires_gl33 Extension @gl_extension{ARB,vertex_type_2_10_10_10_rev}
             * @requires_gles30 Packed attributes are not available in OpenGL
             *      ES 2.0.
             * @requires_webgl20 Packed attributes are not available in WebGL
             *      1.0.
             */
            UnsignedInt2101010Rev = GL_UNSIGNED_INT_2_10_10_10_REV,

            /**
             * Signed 2.10.10.10 packed integer. Only for four-component float
             * vector attribute type.
             * @requires_gl33 Extension @gl_extension{ARB,vertex_type_2_10_10_10_rev}
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
         *
         * Vector count is set to @cpp 1 @ce, vector stride to size of the data
         * type times component count.
         */
        explicit DynamicAttribute(Kind kind, UnsignedInt location, Components components, DataType dataType): DynamicAttribute{kind, location, components, 1, dataType} {}

        /**
         * @brief Construct a matrix attribute
         * @param kind          Attribute kind
         * @param location      Attribute location
         * @param components    Component count
         * @param vectors       Vector count
         * @param dataType      Type of passed data
         * @m_since{2020,06}
         *
         * Vector stride is set to size of the data type times component count.
         */
        explicit DynamicAttribute(Kind kind, UnsignedInt location, Components components, UnsignedInt vectors, DataType dataType);

        /**
         * @brief Construct a matrix attribute with custom vector stride
         * @param kind          Attribute kind
         * @param location      Attribute location
         * @param components    Component count
         * @param vectors       Vector count
         * @param vectorStride  Stride between consecutive matrix column
         *      vectors
         * @param dataType      Type of passed data
         * @m_since{2020,06}
         */
        constexpr explicit DynamicAttribute(Kind kind, UnsignedInt location, Components components, UnsignedInt vectors, UnsignedInt vectorStride, DataType dataType): _kind{kind}, _location{location}, _components{components}, _vectors{vectors}, _vectorStride{vectorStride}, _dataType{dataType} {}

        /**
         * @brief Construct from a compile-time attribute
         * @m_since{2019,10}
         */
        template<UnsignedInt location_, class T> constexpr /*implicit*/ DynamicAttribute(const Attribute<location_, T>& attribute);

        /**
         * @brief Construct from a generic mesh attribute type
         * @m_since{2020,06}
         *
         * The @p type is expected to be available on given target and be
         * compatible with @p kind --- i.e., normalized or floating-point for
         * @ref Kind::GenericNormalized, non-normalized for @ref Kind::Integral
         * / @ref Kind::Long and integral for @ref Kind::Integral.
         * @see @ref hasVertexFormat()
         */
        explicit DynamicAttribute(Kind kind, UnsignedInt location, VertexFormat format): DynamicAttribute{kind, location, format, 4, 4} {}

        /**
         * @brief Construct from a compile-time attribute with a generic mesh attribute type override
         * @m_since{2020,06}
         *
         * Extracts kind and location from passed @ref Attribute type and calls
         * @ref DynamicAttribute(Kind, UnsignedInt, VertexFormat). Expects that
         * @p type's component count is not larger than the component count
         * defined in the @p Attribute type. Note that only the
         * compile-time-defined properties of the @p Attribute type are used,
         * the instance-specific data type, options and component count is
         * ignored.
         */
        template<UnsignedInt location_, class T> explicit DynamicAttribute(const Attribute<location_, T>&, VertexFormat format);

        /** @brief Attribute kind */
        constexpr Kind kind() const { return _kind; }

        /** @brief Attribute location */
        constexpr UnsignedInt location() const { return _location; }

        /** @brief Component count */
        constexpr Components components() const { return _components; }

        /**
         * @brief Vector count
         * @m_since{2020,06}
         *
         * Returns @cpp 1 @ce for non-matrix attributes.
         */
        constexpr UnsignedInt vectors() const { return _vectors; }

        /**
         * @brief Vector stride
         * @m_since{2020,06}
         */
        constexpr UnsignedInt vectorStride() const { return _vectorStride; }

        /** @brief Type of passed data */
        constexpr DataType dataType() const { return _dataType; }

    private:
        /* Used by the constructor taking Attribute, defined in cpp to avoid
           a dependency on <Magnum/Mesh.h> for the assertion */
        explicit DynamicAttribute(Kind kind, UnsignedInt location, VertexFormat format, UnsignedInt maxVectors, GLint maxComponents);

        Kind _kind;
        UnsignedInt _location;
        Components _components;
        UnsignedInt _vectors;
        UnsignedInt _vectorStride;
        DataType _dataType;
};

/** @debugoperatorclassenum{DynamicAttribute,DynamicAttribute::Kind} */
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, DynamicAttribute::Kind);

/** @debugoperatorclassenum{DynamicAttribute,DynamicAttribute::Components} */
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, DynamicAttribute::Components);

/** @debugoperatorclassenum{DynamicAttribute,DynamicAttribute::DataType} */
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, DynamicAttribute::DataType);

/**
@brief Check availability of a generic mesh attribute type
@m_since{2020,06}

Some OpenGL targets don't support all mesh attribute types (for example OpenGL
ES doesn't support double-precision types). Returns @cpp false @ce if current
target can't support such type, @cpp true @ce otherwise. The @p type value is
expected to be valid.

Note that, unlike with pixel format mapping, there's no way to represent an
implementation-specific mesh attribute type using a single 32-bit value and
thus this function returns @cpp false @ce also for all formats for which
@ref isVertexFormatImplementationSpecific() is @cpp true @ce --- you need to do
such mapping by hand by creating a corresponding @ref DynamicAttribute.

@note Support of some formats depends on presence of a particular OpenGL
    extension. Such check is outside of the scope of this function and you are
    expected to verify extension availability before using such type.

@see @ref DynamicAttribute::DynamicAttribute(Kind, UnsignedInt, VertexFormat)
*/
MAGNUM_GL_EXPORT bool hasVertexFormat(Magnum::VertexFormat format);

namespace Implementation {

template<UnsignedInt location, class T> constexpr DynamicAttribute::Kind kindFor(typename std::enable_if<std::is_same<typename Implementation::Attribute<T>::ScalarType, Float>::value, typename GL::Attribute<location, T>::DataOptions>::type options) {
    return options & GL::Attribute<location, T>::DataOption::Normalized ?
        DynamicAttribute::Kind::GenericNormalized : DynamicAttribute::Kind::Generic;
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt location, class T> constexpr DynamicAttribute::Kind kindFor(typename std::enable_if<std::is_integral<typename Implementation::Attribute<T>::ScalarType>::value, typename GL::Attribute<location, T>::DataOptions>::type) {
    return DynamicAttribute::Kind::Integral;
}

#ifndef MAGNUM_TARGET_GLES
template<UnsignedInt location, class T> constexpr DynamicAttribute::Kind kindFor(typename std::enable_if<std::is_same<typename Implementation::Attribute<T>::ScalarType, Double>::value, typename GL::Attribute<location, T>::DataOptions>::type) {
    return DynamicAttribute::Kind::Long;
}
#endif
#endif

/* Base for sized attributes */
template<std::size_t cols, std::size_t rows> struct SizedAttribute;

/* Vector attribute sizes */
template<std::size_t cols> struct SizedVectorAttribute {
    enum: UnsignedInt { Vectors = UnsignedInt(cols) };
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
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, SizedAttribute<1, 1>::Components value);
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, SizedAttribute<1, 2>::Components value);
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, SizedAttribute<1, 3>::Components value);
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, SizedAttribute<1, 4>::Components value);

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
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, SizedMatrixAttribute<2>::Components value);
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, SizedMatrixAttribute<3>::Components value);
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, SizedMatrixAttribute<4>::Components value);
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
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        #ifndef MAGNUM_TARGET_GLES2
        Half = GL_HALF_FLOAT,
        #else
        Half = GL_HALF_FLOAT_OES,
        #endif
        #ifdef MAGNUM_BUILD_DEPRECATED
        HalfFloat CORRADE_DEPRECATED_ENUM("use Half instead") = Half,
        #endif
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

    static UnsignedInt MAGNUM_GL_EXPORT size(GLint components, DataType dataType);
};

CORRADE_ENUMSET_OPERATORS(FloatAttribute::DataOptions)

MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, FloatAttribute::DataType value);

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

    static UnsignedInt MAGNUM_GL_EXPORT size(GLint components, DataType dataType);
};

CORRADE_ENUMSET_OPERATORS(IntAttribute::DataOptions)

MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, IntAttribute::DataType value);

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

    static UnsignedInt MAGNUM_GL_EXPORT size(GLint components, DataType dataType);
};

MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, DoubleAttribute::DataType value);
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
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        #ifndef MAGNUM_TARGET_GLES2
        Half = GL_HALF_FLOAT,
        #else
        Half = GL_HALF_FLOAT_OES,
        #endif
        #ifdef MAGNUM_BUILD_DEPRECATED
        HalfFloat CORRADE_DEPRECATED_ENUM("use Half instead") = Half,
        #endif
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

    static UnsignedInt MAGNUM_GL_EXPORT size(GLint components, DataType dataType);
};

MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, Attribute<Math::Vector<3, Float>>::DataType value);

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
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        #ifndef MAGNUM_TARGET_GLES2
        Half = GL_HALF_FLOAT,
        #else
        Half = GL_HALF_FLOAT_OES,
        #endif
        #ifdef MAGNUM_BUILD_DEPRECATED
        HalfFloat CORRADE_DEPRECATED_ENUM("use Half instead") = Half,
        #endif
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

    enum: UnsignedInt { Vectors = 1 };

    static UnsignedInt MAGNUM_GL_EXPORT size(GLint components, DataType dataType);
};

MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, Attribute<Math::Vector<4, Float>>::Components value);
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, Attribute<Math::Vector<4, Float>>::DataType value);

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

template<UnsignedInt location_, class T> constexpr DynamicAttribute::DynamicAttribute(const Attribute<location_, T>& attribute): _kind{Implementation::kindFor<location_, T>(attribute.dataOptions())}, _location{location_}, _components{Components(GLint(attribute.components()))}, _vectors{Attribute<location_, T>::Vectors}, _vectorStride{attribute.vectorStride()}, _dataType{DataType(GLenum(attribute.dataType()))} {}

template<UnsignedInt location_, class T> DynamicAttribute::DynamicAttribute(const Attribute<location_, T>& attribute, const VertexFormat format): DynamicAttribute{Implementation::kindFor<location_, T>(attribute.dataOptions()), location_, format, Attribute<location_, T>::Vectors, GLint(Implementation::Attribute<T>::DefaultComponents)} {}

}}

#endif
