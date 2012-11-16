#ifndef Magnum_AbstractShaderProgram_h
#define Magnum_AbstractShaderProgram_h
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
 * @brief Class Magnum::AbstractShaderProgram
 */

#include <cstdint>
#include <string>
#include <Containers/EnumSet.h>

#include "Magnum.h"
#include "TypeTraits.h"

#include "magnumVisibility.h"

/** @todo early asserts (no bool returns?) */

namespace Magnum {

namespace Math {
    template<std::size_t, std::size_t, class> class RectangularMatrix;
    template<std::size_t, class> class Matrix;
    template<std::size_t, class> class Vector;
}

template<class> class Color3;
template<class> class Color4;
class Context;
class Shader;

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {
    template<class> struct Attribute;
}
#endif

/**
@brief Base for shader program implementations

@section AbstractShaderProgram-subclassing Subclassing workflow

This class is designed to be used via subclassing. Subclasses define these
functions and properties:
 - <strong>%Attribute definitions</strong> with location and type for
   configuring meshes, for example:
@code
static const Attribute<0, Point3D> Position;
static const Attribute<1, Vector3> Normal;
static const Attribute<2, Vector2> TextureCoordinates;
@endcode
   @todoc Output attribute location (for bindFragmentDataLocationIndexed(),
        referenced also from Framebuffer::mapDefaultForDraw() / Framebuffer::mapForDraw())

 - **Layers for texture uniforms** to which the textures will be bound before
   rendering, for example:
@code
static const GLint DiffuseTextureLayer = 0;
static const GLint SpecularTextureLayer = 1;
@endcode
 - **Uniform locations** for setting uniform data (see below) (private
   constants), for example:
@code
static const GLint TransformationUniform = 0;
static const GLint ProjectionUniform = 1;
static const GLint DiffuseTextureUniform = 2;
static const GLint SpecularTextureUniform = 3;
@endcode
 - **Constructor**, which attaches particular shaders, links the program and
   gets uniform locations, for example:
@code
MyShader() {
    // Load shaders from file and attach them to the program
    attachShader(Shader::fromFile(Version::430, Shader::Type::Vertex, "PhongShader.vert"));
    attachShader(Shader::fromFile(Version::430, Shader::Type::Fragment, "PhongShader.frag"));

    // Link
    link();
}
@endcode
 - **Uniform setting functions**, which will provide public interface for
   protected setUniform() functions. For usability purposes you can implement
   also method chaining. Example:
@code
MyShader* setTransformation(const Matrix4& matrix) {
    setUniform(TransformationUniform, matrix);
    return this;
}
MyShader* setProjection(const Matrix4& matrix) {
    setUniform(ProjectionUniform, matrix);
    return this;
}
@endcode

@subsection AbstractShaderProgram-attribute-location Binding attribute location

The preferred workflow is to specify attribute location for vertex shader
input attributes and fragment shader output attributes explicitly in the
shader code, e.g.:
@code
// GLSL 3.30, or
#extension GL_ARB_explicit_attrib_location: enable
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textureCoordinates;
@endcode
Similarly for ouput attributes, you can also specify blend equation color
index for them (see Framebuffer::BlendFunction for more information about
using color input index):
@code
layout(location = 0, index = 0) out vec4 color;
layout(location = 1, index = 1) out vec4 ambient;
@endcode

If you don't have the required extension, you can use functions
bindAttributeLocation() and bindFragmentDataLocation() /
bindFragmentDataLocationIndexed() between attaching the shaders and linking
the program:
@code
// Shaders attached...

bindAttributeLocation(Position.Location, "position");
bindAttributeLocation(Normal.Location, "normal");
bindAttributeLocation(TextureCoords.Location, "textureCoordinates");

bindFragmentDataLocationIndexed(0, 0, "color");
bindFragmentDataLocationIndexed(1, 1, "ambient");

// Link...
@endcode

@requires_gl30 %Extension @extension{EXT,gpu_shader4} for using
    bindFragmentDataLocation().
@requires_gl33 %Extension @extension{ARB,blend_func_extended} for using
    bindFragmentDataLocationIndexed().
@requires_gl33 %Extension @extension{ARB,explicit_attrib_location} for
    explicit attribute location instead of using bindAttributeLocation(),
    bindFragmentDataLocation() or bindFragmentDataLocationIndexed().
@requires_gles30 Explicit location specification of input attributes is not
    supported in OpenGL ES 2.0, use bindAttributeLocation() instead.
@requires_gles30 Multiple fragment shader outputs are not available in OpenGL
    ES 2.0, similar functionality is available in extension
    @extension{NV,draw_buffers}.

@subsection AbstractShaderProgram-uniform-location Uniform locations

The preferred workflow is to specify uniform locations directly in the shader
code, e.g.:
@code
// GLSL 4.30, or
#extension GL_ARB_explicit_uniform_location: enable
layout(location = 0) uniform mat4 transformation;
layout(location = 1) uniform mat4 projection;
@endcode

If you don't have the required extension, you can get uniform location using
uniformLocation() after linking stage:
@code
GLint transformationUniform = uniformLocation("transformation");
GLint projectionUniform = uniformLocation("projection");
@endcode

@requires_gl43 Extension @extension{ARB,explicit_uniform_location} for
    explicit uniform location instead of using uniformLocation().
@requires_gl Explicit uniform location is not supported in OpenGL ES. Use
    uniformLocation() instead.

@subsection AbstractShaderProgram-texture-layer Binding texture layer uniforms

The preferred workflow is to specify texture layers directly in the shader
code, e.g.:
@code
// GLSL 4.20, or
#extension GL_ARB_shading_language_420pack: enable
layout(binding = 0) uniform sampler2D diffuseTexture;
layout(binding = 1) uniform sampler2D specularTexture;
@endcode

If you don't have the required extension (or if you want to change the layer
later), you can set the texture layer uniform using setUniform(GLint, GLint):
@code
setUniform(DiffuseTextureUniform, DiffuseTextureLayer);
setUniform(SpecularTextureUniform, SpecularTextureLayer);
@endcode

@requires_gl42 Extension @extension{ARB,shading_language_420pack} for explicit
    texture layer binding instead of using setUniform(GLint, GLint).
@requires_gl Explicit texture layer binding is not supported in OpenGL ES. Use
    setUniform(GLint, GLint) instead.

@section AbstractShaderProgram-rendering-workflow Rendering workflow

Basic workflow with %AbstractShaderProgram subclasses is to instance the class
and configuring attribute binding in meshes (see @ref Mesh-configuration "Mesh documentation"
for more information) at the beginning, then in draw event setting uniforms
and marking the shader for use, binding required textures to their respective
layers using AbstractTexture::bind(GLint) and calling Mesh::draw(). Example:
@code
shader->setTransformation(transformation)
    ->setProjection(projection)
    ->use();

diffuseTexture->bind(MyShader::DiffuseTextureLayer);
specularTexture->bind(MyShader::SpecularTextureLayer);

mesh.draw();
@endcode

@section AbstractShaderProgram-types Mapping between GLSL and Magnum types

- `vec2`, `vec3` and `vec4` is @ref Math::Vector "Math::Vector<2, GLfloat>",
  @ref Math::Vector "Math::Vector<3, GLfloat>" and
  @ref Math::Vector "Math::Vector<4, GLfloat>".

- `mat2`, `mat3` and `mat4` is @ref Math::Matrix "Math::Matrix<2, GLfloat>",
  @ref Math::Matrix "Math::Matrix<3, GLfloat>" and
  @ref Math::Matrix "Math::Matrix<4, GLfloat>".

- `mat2x3`, `mat3x2`, `mat2x4`, `mat4x2`, `mat3x4`, `mat4x3` is
  @ref Math::RectangularMatrix "Math::RectangularMatrix<2, 3, GLfloat>",
  @ref Math::RectangularMatrix "Math::RectangularMatrix<3, 2, GLfloat>",
  @ref Math::RectangularMatrix "Math::RectangularMatrix<2, 4, GLfloat>",
  @ref Math::RectangularMatrix "Math::RectangularMatrix<4, 2, GLfloat>",
  @ref Math::RectangularMatrix "Math::RectangularMatrix<3, 4, GLfloat>" and
  @ref Math::RectangularMatrix "Math::RectangularMatrix<4, 3, GLfloat>".

- `ivec2`, `ivec3` and `ivec4` is @ref Math::Vector "Math::Vector<2, GLint>",
  @ref Math::Vector "Math::Vector<3, GLint>" and
  @ref Math::Vector "Math::Vector<4, GLint>", `uvec2`, `uvec3` and `uvec4` is
  @ref Math::Vector "Math::Vector<2, GLuint>",
  @ref Math::Vector "Math::Vector<3, GLuint>" and
  @ref Math::Vector "Math::Vector<4, GLuint>".
  @requires_gl30 %Extension @extension{EXT,gpu_shader4} (for integer attributes
    and unsigned integer uniforms)
  @requires_gles30 Integer attributes and unsigned integer uniforms are not
    available in OpenGL ES 2.0.

- `dvec2`, `dvec3` and `dvec4` is @ref Math::Vector "Math::Vector<2, GLdouble>",
  @ref Math::Vector "Math::Vector<3, GLdouble>" and
  @ref Math::Vector "Math::Vector<4, GLdouble>", `dmat2`, `dmat3` and `dmat4`
  is @ref Math::Matrix "Math::Matrix<2, GLdouble>",
  @ref Math::Matrix "Math::Matrix<3, GLdouble>" and
  @ref Math::Matrix "Math::Matrix<4, GLdouble>", `dmat2x3`, `dmat3x2`,
  `dmat2x4`, `dmat4x2`, `dmat3x4`, `dmat4x3` is
  @ref Math::RectangularMatrix "Math::RectangularMatrix<2, 3, GLdouble>",
  @ref Math::RectangularMatrix "Math::RectangularMatrix<3, 2, GLdouble>",
  @ref Math::RectangularMatrix "Math::RectangularMatrix<2, 4, GLdouble>",
  @ref Math::RectangularMatrix "Math::RectangularMatrix<4, 2, GLdouble>",
  @ref Math::RectangularMatrix "Math::RectangularMatrix<3, 4, GLdouble>" and
  @ref Math::RectangularMatrix "Math::RectangularMatrix<4, 3, GLdouble>".
  @requires_gl41 %Extension @extension{ARB,vertex_attrib_64bit} (for double
    attributes)
  @requires_gl Double attributes are not available in OpenGL ES.

Only types listed here (and their subclasses and specializations, such as
@ref Matrix3 or Color4) can be used for setting uniforms and specifying
vertex attributes. See also TypeTraits::AttributeType.

@section AbstractShaderProgram-performance-optimization Performance optimizations

The engine tracks currently used shader program to avoid unnecessary calls to
@fn_gl{UseProgram}. %Shader limits (such as maxSupportedVertexAttributeCount())
are cached, so repeated queries don't result in repeated @fn_gl{Get} calls.

If extension @extension{ARB,separate_shader_objects} or
@extension{EXT,direct_state_access} is available, uniform setting functions
use DSA functions to avoid unnecessary calls to @fn_gl{UseProgram}. See
setUniform(GLint, GLfloat) documentation for more information.

To achieve least state changes, set all uniforms in one run -- method chaining
comes in handy.

@todo Uniform arrays support
 */
class MAGNUM_EXPORT AbstractShaderProgram {
    friend class Context;

    AbstractShaderProgram(const AbstractShaderProgram& other) = delete;
    AbstractShaderProgram(AbstractShaderProgram&& other) = delete;
    AbstractShaderProgram& operator=(const AbstractShaderProgram& other) = delete;
    AbstractShaderProgram& operator=(AbstractShaderProgram&& other) = delete;

    public:
        /**
         * @brief Base struct for attribute location and type
         *
         * Template parameter @p location is vertex attribute location, number
         * between `0` and maxSupportedVertexAttributeCount(). To ensure
         * compatibility, you should always have vertex attribute with
         * location `0`.
         *
         * Template parameter @p T is the type which is used for shader
         * attribute, e.g. @ref Math::Vector4 "Vector4<GLint>" for `ivec4`.
         * DataType is type of passed data when adding vertex buffers to mesh.
         * By default it is the same as type used in shader (e.g.
         * @ref DataType "DataType::Int" for @ref Math::Vector4 "Vector4<GLint>").
         * It's also possible to pass integer data to floating-point shader
         * inputs. In this case you may want to normalize the values (e.g.
         * color components from 0-255 to 0.0f-1.0f) - see
         * @ref DataOption "DataOption::Normalize".
         *
         * Only some types are allowed as attribute types, see
         * @ref AbstractShaderProgram-types or TypeTraits::AttributeType for
         * more information.
         *
         * See @ref AbstractShaderProgram-subclassing for example usage in
         * shaders and @ref Mesh-configuration for example usage when adding
         * vertex buffers to mesh.
         */
        template<GLuint location, class T> class Attribute {
            public:
                /** @brief Location to which the attribute is bound */
                static const GLuint Location = location;

                /**
                 * @brief Type
                 *
                 * Type used in shader code.
                 * @see DataType
                 */
                typedef typename TypeTraits<T>::AttributeType Type;

                /**
                 * @brief Data type
                 *
                 * Type of data passed to shader.
                 * @see Type, DataOptions, Attribute()
                 */
                #ifdef DOXYGEN_GENERATING_OUTPUT
                enum class DataType: GLenum {
                    UnsignedByte = GL_UNSIGNED_BYTE,    /**< Unsigned byte */
                    Byte = GL_BYTE,                     /**< Byte */
                    UnsignedShort = GL_UNSIGNED_SHORT,  /**< Unsigned short */
                    Short = GL_SHORT,                   /**< Short */
                    UnsignedInt = GL_UNSIGNED_INT,      /**< Unsigned int */
                    Int = GL_INT,                       /**< Int */

                    /**
                     * Half float. Only for float attribute types.
                     * @requires_gl30 %Extension @extension{NV,half_float}
                     * @requires_gles30 %Extension @es_extension{OES,vertex_half_float}
                     */
                    Half = GL_HALF_FLOAT,

                    /** Float. Only for float attribute types. */
                    Float = GL_FLOAT,

                    #ifndef MAGNUM_TARGET_GLES
                    /**
                     * Double. Only for float and double attribute types.
                     * @requires_gl Only floats are available in OpenGL ES.
                     */
                    Double = GL_DOUBLE,
                    #endif

                    /* GL_FIXED not supported */

                    #ifndef MAGNUM_TARGET_GLES2
                    /**
                     * Unsigned 2.10.10.10 packed integer. Only for
                     * four-component float vector attribute type.
                     * @todo How about (incompatible) @es_extension{OES,vertex_type_10_10_10_2}?
                     * @requires_gl33 %Extension @extension{ARB,vertex_type_2_10_10_10_rev}
                     * @requires_gles30 (no extension providing this functionality)
                     */
                    UnsignedInt2101010REV = GL_UNSIGNED_INT_2_10_10_10_REV,

                    /**
                     * Signed 2.10.10.10 packed integer. Only for
                     * four-component float vector attribute type.
                     * @requires_gl33 %Extension @extension{ARB,vertex_type_2_10_10_10_rev}
                     * @requires_gles30 (no extension providing this functionality)
                     */
                    Int2101010REV = GL_INT_2_10_10_10_REV
                    #endif
                };
                #else
                typedef typename Implementation::Attribute<T>::DataType DataType;
                #endif

                /**
                 * @brief Data option
                 * @see DataOptions, Attribute()
                 */
                #ifdef DOXYGEN_GENERATING_OUTPUT
                enum class DataOption: std::uint8_t {
                    /**
                     * Normalize integer components. Only for float attribute
                     * types. Default is to not normalize.
                     */
                    Normalize = 1 << 0,

                    /**
                     * BGRA component ordering. Default is RGBA. Only for
                     * four-component float vector attribute type.
                     * @requires_gl32 %Extension @extension{ARB,vertex_array_bgra}
                     * @requires_gl Only RGBA component ordering is supported
                     *      in OpenGL ES.
                     */
                    BGRA = 1 << 1
                };
                #else
                typedef typename Implementation::Attribute<T>::DataOption DataOption;
                #endif

                /**
                 * @brief Data options
                 * @see Attribute()
                 */
                #ifdef DOXYGEN_GENERATING_OUTPUT
                typedef typename Corrade::Containers::EnumSet<DataOption, std::uint8_t> DataOptions;
                #else
                typedef typename Implementation::Attribute<T>::DataOptions DataOptions;
                #endif

                /**
                 * @brief Constructor
                 * @param dataType      Type of passed data. Default is the
                 *      same as type used in shader (e.g. DataType::Integer
                 *      for Vector4<GLint>).
                 * @param dataOptions   Data options. Default is no options.
                 */
                inline constexpr Attribute(DataType dataType = Implementation::Attribute<T>::DefaultDataType, DataOptions dataOptions = DataOptions()): _dataType(dataType), _dataOptions(dataOptions) {}

                /** @brief Type of passed data */
                inline constexpr DataType dataType() const { return _dataType; }

                /** @brief Data options */
                inline constexpr DataOptions dataOptions() const { return _dataOptions; }

            private:
                const DataType _dataType;
                const DataOptions _dataOptions;
        };

        /**
         * @brief Max supported vertex attribute count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls.
         * @see Attribute, @fn_gl{Get} with @def_gl{MAX_VERTEX_ATTRIBS}
         */
        static GLint maxSupportedVertexAttributeCount();

        /**
         * @brief Constructor
         *
         * Creates one OpenGL shader program.
         * @see @fn_gl{CreateProgram}
         */
        inline AbstractShaderProgram(): state(Initialized) {
            _id = glCreateProgram();
        }

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL shader program.
         * @see @fn_gl{DeleteProgram}
         */
        virtual ~AbstractShaderProgram() = 0;

        /**
         * @brief Use shader for rendering
         * @return False if the program wasn't successfully linked, true
         *      otherwise.
         *
         * @see @fn_gl{UseProgram}
         */
        bool use();

    protected:
        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Allow retrieving program binary
         *
         * Initially disabled.
         * @note This function should be called after attachShader() calls and
         *      before link().
         * @see @fn_gl{ProgramParameter} with @def_gl{PROGRAM_BINARY_RETRIEVABLE_HINT}
         * @requires_gl41 Extension @extension{ARB,get_program_binary}
         * @requires_gles30 Always allowed in OpenGL ES 2.0.
         */
        inline void setRetrievableBinary(bool enabled) {
            glProgramParameteri(_id, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, enabled ? GL_TRUE : GL_FALSE);
        }
        #endif

        /**
         * @brief Allow the program to be bound to individual pipeline stages
         *
         * Initially disabled.
         * @note This function should be called after attachShader() calls and
         *      before link().
         * @see @fn_gl{ProgramParameter} with @def_gl{PROGRAM_SEPARABLE}
         * @requires_gl41 Extension @extension{ARB,separate_shader_objects}
         * @requires_es_extension %Extension @es_extension{EXT,separate_shader_objects}
         */
        inline void setSeparable(bool enabled) {
            /** @todo Remove when extension wrangler is available for ES */
            #ifndef MAGNUM_TARGET_GLES
            glProgramParameteri(_id, GL_PROGRAM_SEPARABLE, enabled ? GL_TRUE : GL_FALSE);
            #else
            static_cast<void>(enabled);
            #endif
        }

        /**
         * @brief Load shader
         * @return False if the shader wasn't successfully compiled, true
         *      otherwise.
         *
         * Compiles the shader, if it is not already, and prepares it for
         * linking.
         * @see Shader::compile(), @fn_gl{AttachShader}
         */
        bool attachShader(Shader& shader);

        /** @overload */
        inline bool attachShader(Shader&& shader) {
            return attachShader(shader);
        }

        /**
         * @brief Bind attribute to given location
         * @param location      Location
         * @param name          Attribute name
         *
         * Binds attribute to location which is used later for binding vertex
         * buffers.
         * @note This function should be called after attachShader() calls and
         *      before link().
         * @deprecated Preferred usage is to specify attribute location
         *      explicitly in the shader instead of using this function. See
         *      @ref AbstractShaderProgram-attribute-location "class documentation"
         *      for more information.
         * @see @fn_gl{BindAttribLocation}
         */
        void bindAttributeLocation(GLuint location, const std::string& name);

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Bind fragment data to given location and color input index
         * @param location      Location
         * @param name          Fragment output variable name
         * @param index         Blend equation color input index (`0` or `1`)
         *
         * Binds fragment data to location which is used later for framebuffer
         * operations. See also Framebuffer::BlendFunction for more
         * information about using color input index.
         * @note This function should be called after attachShader() calls and
         *      before link().
         * @deprecated Preferred usage is to specify attribute location
         *      explicitly in the shader instead of using this function. See
         *      @ref AbstractShaderProgram-attribute-location "class documentation"
         *      for more information.
         * @see @fn_gl{BindFragDataLocationIndexed}
         * @requires_gl33 Extension @extension{ARB,blend_func_extended}
         * @requires_gl Multiple blend function inputs are not available in
         *      OpenGL ES.
         */
        void bindFragmentDataLocationIndexed(GLuint location, GLuint index, const std::string& name);

        /**
         * @brief Bind fragment data to given location and first color input index
         * @param location      Location
         * @param name          Fragment output variable name
         *
         * The same as bindFragmentDataLocationIndexed(), but with `index` set
         * to `0`.
         * @see @fn_gl{BindFragDataLocation}
         * @requires_gl30 Extension @extension{EXT,gpu_shader4}
         * @requires_gl Use explicit location specification in OpenGL ES 3.0
         *      instead.
         */
        void bindFragmentDataLocation(GLuint location, const std::string& name);
        #endif

        /**
         * @brief Link the shader
         *
         * Binds previously specified attributes to given indexes and links the
         * shader program together.
         * @see @fn_gl{LinkProgram}, @fn_gl{GetProgram} with
         *      @def_gl{LINK_STATUS}, @fn_gl{GetProgramInfoLog}
         */
        void link();

        /**
         * @brief Get uniform location
         * @param name          Uniform name
         *
         * @note This function should be called after link().
         * @deprecated Preferred usage is to specify uniform location
         *      explicitly in the shader instead of using this function. See
         *      @ref AbstractShaderProgram-uniform-location "class documentation"
         *      for more information.
         * @see @fn_gl{GetUniformLocation}
         */
        GLint uniformLocation(const std::string& name);

        /**
         * @brief Set uniform value
         * @param location      Uniform location (see uniformLocation())
         * @param value         Value
         *
         * If neither @extension{ARB,separate_shader_objects} nor
         * @extension{EXT,direct_state_access} is available, the shader is
         * marked for use before the operation.
         * @see @fn_gl{UseProgram}, @fn_gl{Uniform} or `glProgramUniform()`
         *      from @extension{ARB,separate_shader_objects}/@extension{EXT,direct_state_access}.
         */
        inline void setUniform(GLint location, GLfloat value) {
            (this->*uniform1fImplementation)(location, value);
        }

        /** @copydoc setUniform(GLint, GLfloat) */
        inline void setUniform(GLint location, const Math::Vector<2, GLfloat>& value) {
            (this->*uniform2fvImplementation)(location, value);
        }

        /** @copydoc setUniform(GLint, GLfloat) */
        inline void setUniform(GLint location, const Math::Vector<3, GLfloat>& value) {
            (this->*uniform3fvImplementation)(location, value);
        }

        /** @copydoc setUniform(GLint, GLfloat) */
        inline void setUniform(GLint location, const Math::Vector<4, GLfloat>& value) {
            (this->*uniform4fvImplementation)(location, value);
        }

        /** @copydoc setUniform(GLint, GLfloat) */
        inline void setUniform(GLint location, GLint value) {
            (this->*uniform1iImplementation)(location, value);
        }

        /** @copydoc setUniform(GLint, GLfloat) */
        inline void setUniform(GLint location, const Math::Vector<2, GLint>& value) {
            (this->*uniform2ivImplementation)(location, value);
        }

        /** @copydoc setUniform(GLint, GLfloat) */
        inline void setUniform(GLint location, const Math::Vector<3, GLint>& value) {
            (this->*uniform3ivImplementation)(location, value);
        }

        /** @copydoc setUniform(GLint, GLfloat) */
        inline void setUniform(GLint location, const Math::Vector<4, GLint>& value) {
            (this->*uniform4ivImplementation)(location, value);
        }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gl30 Extension @extension{EXT,gpu_shader4}
         * @requires_gles30 Only signed integers are available in OpenGL ES 2.0.
         */
        inline void setUniform(GLint location, GLuint value) {
            (this->*uniform1uiImplementation)(location, value);
        }

        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gl30 Extension @extension{EXT,gpu_shader4}
         * @requires_gles30 Only signed integers are available in OpenGL ES 2.0.
         */
        inline void setUniform(GLint location, const Math::Vector<2, GLuint>& value) {
            (this->*uniform2uivImplementation)(location, value);
        }

        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gl30 Extension @extension{EXT,gpu_shader4}
         * @requires_gles30 Only signed integers are available in OpenGL ES 2.0.
         */
        inline void setUniform(GLint location, const Math::Vector<3, GLuint>& value) {
            (this->*uniform3uivImplementation)(location, value);
        }

        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gl30 Extension @extension{EXT,gpu_shader4}
         * @requires_gles30 Only signed integers are available in OpenGL ES 2.0.
         */
        inline void setUniform(GLint location, const Math::Vector<4, GLuint>& value) {
            (this->*uniform4uivImplementation)(location, value);
        }
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gl40 Extension @extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES.
         */
        inline void setUniform(GLint location, GLdouble value) {
            (this->*uniform1dImplementation)(location, value);
        }

        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gl40 Extension @extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES.
         */
        inline void setUniform(GLint location, const Math::Vector<2, GLdouble>& value) {
            (this->*uniform2dvImplementation)(location, value);
        }

        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gl40 Extension @extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES.
         */
        inline void setUniform(GLint location, const Math::Vector<3, GLdouble>& value) {
            (this->*uniform3dvImplementation)(location, value);
        }

        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gl40 Extension @extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES.
         */
        inline void setUniform(GLint location, const Math::Vector<4, GLdouble>& value) {
            (this->*uniform4dvImplementation)(location, value);
        }
        #endif

        /** @copydoc setUniform(GLint, GLfloat) */
        inline void setUniform(GLint location, const Math::Matrix<2, GLfloat>& value) {
            (this->*uniformMatrix2fvImplementation)(location, value);
        }

        /** @copydoc setUniform(GLint, GLfloat) */
        inline void setUniform(GLint location, const Math::Matrix<3, GLfloat>& value) {
            (this->*uniformMatrix3fvImplementation)(location, value);
        }

        /** @copydoc setUniform(GLint, GLfloat) */
        inline void setUniform(GLint location, const Math::Matrix<4, GLfloat>& value) {
            (this->*uniformMatrix4fvImplementation)(location, value);
        }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gles30 Only square matrices are available in OpenGL ES 2.0.
         */
        inline void setUniform(GLint location, const Math::RectangularMatrix<2, 3, GLfloat>& value) {
            (this->*uniformMatrix2x3fvImplementation)(location, value);
        }

        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gles30 Only square matrices are available in OpenGL ES 2.0.
         */
        inline void setUniform(GLint location, const Math::RectangularMatrix<3, 2, GLfloat>& value) {
            (this->*uniformMatrix3x2fvImplementation)(location, value);
        }

        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gles30 Only square matrices are available in OpenGL ES 2.0.
         */
        inline void setUniform(GLint location, const Math::RectangularMatrix<2, 4, GLfloat>& value) {
            (this->*uniformMatrix2x4fvImplementation)(location, value);
        }

        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gles30 Only square matrices are available in OpenGL ES 2.0.
         */
        inline void setUniform(GLint location, const Math::RectangularMatrix<4, 2, GLfloat>& value) {
            (this->*uniformMatrix4x2fvImplementation)(location, value);
        }

        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gles30 Only square matrices are available in OpenGL ES 2.0.
         */
        inline void setUniform(GLint location, const Math::RectangularMatrix<3, 4, GLfloat>& value) {
            (this->*uniformMatrix3x4fvImplementation)(location, value);
        }

        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gles30 Only square matrices are available in OpenGL ES 2.0.
         */
        inline void setUniform(GLint location, const Math::RectangularMatrix<4, 3, GLfloat>& value) {
            (this->*uniformMatrix4x3fvImplementation)(location, value);
        }
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gl40 Extension @extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES.
         */
        inline void setUniform(GLint location, const Math::Matrix<2, GLdouble>& value) {
            (this->*uniformMatrix2dvImplementation)(location, value);
        }

        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gl40 Extension @extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES.
         */
        inline void setUniform(GLint location, const Math::Matrix<3, GLdouble>& value) {
            (this->*uniformMatrix3dvImplementation)(location, value);
        }

        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gl40 Extension @extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES.
         */
        inline void setUniform(GLint location, const Math::Matrix<4, GLdouble>& value) {
            (this->*uniformMatrix4dvImplementation)(location, value);
        }

        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gl40 Extension @extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES.
         */
        inline void setUniform(GLint location, const Math::RectangularMatrix<2, 3, GLdouble>& value) {
            (this->*uniformMatrix2x3dvImplementation)(location, value);
        }

        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gl40 Extension @extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES.
         */
        inline void setUniform(GLint location, const Math::RectangularMatrix<3, 2, GLdouble>& value) {
            (this->*uniformMatrix3x2dvImplementation)(location, value);
        }

        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gl40 Extension @extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES.
         */
        inline void setUniform(GLint location, const Math::RectangularMatrix<2, 4, GLdouble>& value) {
            (this->*uniformMatrix2x4dvImplementation)(location, value);
        }

        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gl40 Extension @extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES.
         */
        inline void setUniform(GLint location, const Math::RectangularMatrix<4, 2, GLdouble>& value) {
            (this->*uniformMatrix4x2dvImplementation)(location, value);
        }

        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gl40 Extension @extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES.
         */
        inline void setUniform(GLint location, const Math::RectangularMatrix<3, 4, GLdouble>& value) {
            (this->*uniformMatrix3x4dvImplementation)(location, value);
        }

        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gl40 Extension @extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES.
         */
        inline void setUniform(GLint location, const Math::RectangularMatrix<4, 3, GLdouble>& value) {
            (this->*uniformMatrix4x3dvImplementation)(location, value);
        }
        #endif

    private:
        enum State {
            Initialized,
            Linked,
            Failed
        };

        static void MAGNUM_LOCAL initializeContextBasedFunctionality(Context* context);

        typedef void(AbstractShaderProgram::*Uniform1fImplementation)(GLint, GLfloat);
        typedef void(AbstractShaderProgram::*Uniform2fvImplementation)(GLint, const Math::Vector<2, GLfloat>&);
        typedef void(AbstractShaderProgram::*Uniform3fvImplementation)(GLint, const Math::Vector<3, GLfloat>&);
        typedef void(AbstractShaderProgram::*Uniform4fvImplementation)(GLint, const Math::Vector<4, GLfloat>&);
        typedef void(AbstractShaderProgram::*Uniform1iImplementation)(GLint, GLint);
        typedef void(AbstractShaderProgram::*Uniform2ivImplementation)(GLint, const Math::Vector<2, GLint>&);
        typedef void(AbstractShaderProgram::*Uniform3ivImplementation)(GLint, const Math::Vector<3, GLint>&);
        typedef void(AbstractShaderProgram::*Uniform4ivImplementation)(GLint, const Math::Vector<4, GLint>&);
        #ifndef MAGNUM_TARGET_GLES2
        typedef void(AbstractShaderProgram::*Uniform1uiImplementation)(GLint, GLuint);
        typedef void(AbstractShaderProgram::*Uniform2uivImplementation)(GLint, const Math::Vector<2, GLuint>&);
        typedef void(AbstractShaderProgram::*Uniform3uivImplementation)(GLint, const Math::Vector<3, GLuint>&);
        typedef void(AbstractShaderProgram::*Uniform4uivImplementation)(GLint, const Math::Vector<4, GLuint>&);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        typedef void(AbstractShaderProgram::*Uniform1dImplementation)(GLint, GLdouble);
        typedef void(AbstractShaderProgram::*Uniform2dvImplementation)(GLint, const Math::Vector<2, GLdouble>&);
        typedef void(AbstractShaderProgram::*Uniform3dvImplementation)(GLint, const Math::Vector<3, GLdouble>&);
        typedef void(AbstractShaderProgram::*Uniform4dvImplementation)(GLint, const Math::Vector<4, GLdouble>&);
        #endif
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLfloat value);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::Vector<2, GLfloat>& value);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::Vector<3, GLfloat>& value);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::Vector<4, GLfloat>& value);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLint value);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::Vector<2, GLint>& value);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::Vector<3, GLint>& value);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::Vector<4, GLint>& value);
        #ifndef MAGNUM_TARGET_GLES2
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLuint value);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::Vector<2, GLuint>& value);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::Vector<3, GLuint>& value);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::Vector<4, GLuint>& value);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLdouble value);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::Vector<2, GLdouble>& value);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::Vector<3, GLdouble>& value);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::Vector<4, GLdouble>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLfloat value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::Vector<2, GLfloat>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::Vector<3, GLfloat>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::Vector<4, GLfloat>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLint value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::Vector<2, GLint>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::Vector<3, GLint>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::Vector<4, GLint>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLuint value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::Vector<2, GLuint>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::Vector<3, GLuint>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::Vector<4, GLuint>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, GLdouble value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::Vector<2, GLdouble>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::Vector<3, GLdouble>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::Vector<4, GLdouble>& value);
        #endif
        static Uniform1fImplementation uniform1fImplementation;
        static Uniform2fvImplementation uniform2fvImplementation;
        static Uniform3fvImplementation uniform3fvImplementation;
        static Uniform4fvImplementation uniform4fvImplementation;
        static Uniform1iImplementation uniform1iImplementation;
        static Uniform2ivImplementation uniform2ivImplementation;
        static Uniform3ivImplementation uniform3ivImplementation;
        static Uniform4ivImplementation uniform4ivImplementation;
        #ifndef MAGNUM_TARGET_GLES2
        static Uniform1uiImplementation uniform1uiImplementation;
        static Uniform2uivImplementation uniform2uivImplementation;
        static Uniform3uivImplementation uniform3uivImplementation;
        static Uniform4uivImplementation uniform4uivImplementation;
        #endif
        #ifndef MAGNUM_TARGET_GLES
        static Uniform1dImplementation uniform1dImplementation;
        static Uniform2dvImplementation uniform2dvImplementation;
        static Uniform3dvImplementation uniform3dvImplementation;
        static Uniform4dvImplementation uniform4dvImplementation;
        #endif

        typedef void(AbstractShaderProgram::*UniformMatrix2fvImplementation)(GLint, const Math::Matrix<2, GLfloat>&);
        typedef void(AbstractShaderProgram::*UniformMatrix3fvImplementation)(GLint, const Math::Matrix<3, GLfloat>&);
        typedef void(AbstractShaderProgram::*UniformMatrix4fvImplementation)(GLint, const Math::Matrix<4, GLfloat>&);
        #ifndef MAGNUM_TARGET_GLES2
        typedef void(AbstractShaderProgram::*UniformMatrix2x3fvImplementation)(GLint, const Math::RectangularMatrix<2, 3, GLfloat>&);
        typedef void(AbstractShaderProgram::*UniformMatrix3x2fvImplementation)(GLint, const Math::RectangularMatrix<3, 2, GLfloat>&);
        typedef void(AbstractShaderProgram::*UniformMatrix2x4fvImplementation)(GLint, const Math::RectangularMatrix<2, 4, GLfloat>&);
        typedef void(AbstractShaderProgram::*UniformMatrix4x2fvImplementation)(GLint, const Math::RectangularMatrix<4, 2, GLfloat>&);
        typedef void(AbstractShaderProgram::*UniformMatrix3x4fvImplementation)(GLint, const Math::RectangularMatrix<3, 4, GLfloat>&);
        typedef void(AbstractShaderProgram::*UniformMatrix4x3fvImplementation)(GLint, const Math::RectangularMatrix<4, 3, GLfloat>&);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        typedef void(AbstractShaderProgram::*UniformMatrix2dvImplementation)(GLint, const Math::Matrix<2, GLdouble>&);
        typedef void(AbstractShaderProgram::*UniformMatrix3dvImplementation)(GLint, const Math::Matrix<3, GLdouble>&);
        typedef void(AbstractShaderProgram::*UniformMatrix4dvImplementation)(GLint, const Math::Matrix<4, GLdouble>&);
        typedef void(AbstractShaderProgram::*UniformMatrix2x3dvImplementation)(GLint, const Math::RectangularMatrix<2, 3, GLdouble>&);
        typedef void(AbstractShaderProgram::*UniformMatrix3x2dvImplementation)(GLint, const Math::RectangularMatrix<3, 2, GLdouble>&);
        typedef void(AbstractShaderProgram::*UniformMatrix2x4dvImplementation)(GLint, const Math::RectangularMatrix<2, 4, GLdouble>&);
        typedef void(AbstractShaderProgram::*UniformMatrix4x2dvImplementation)(GLint, const Math::RectangularMatrix<4, 2, GLdouble>&);
        typedef void(AbstractShaderProgram::*UniformMatrix3x4dvImplementation)(GLint, const Math::RectangularMatrix<3, 4, GLdouble>&);
        typedef void(AbstractShaderProgram::*UniformMatrix4x3dvImplementation)(GLint, const Math::RectangularMatrix<4, 3, GLdouble>&);
        #endif
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::Matrix<2, GLfloat>& value);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::Matrix<3, GLfloat>& value);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::Matrix<4, GLfloat>& value);
        #ifndef MAGNUM_TARGET_GLES2
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::RectangularMatrix<2, 3, GLfloat>& value);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::RectangularMatrix<3, 2, GLfloat>& value);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::RectangularMatrix<2, 4, GLfloat>& value);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::RectangularMatrix<4, 2, GLfloat>& value);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::RectangularMatrix<3, 4, GLfloat>& value);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::RectangularMatrix<4, 3, GLfloat>& value);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::Matrix<2, GLdouble>& value);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::Matrix<3, GLdouble>& value);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::Matrix<4, GLdouble>& value);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::RectangularMatrix<2, 3, GLdouble>& value);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::RectangularMatrix<3, 2, GLdouble>& value);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::RectangularMatrix<2, 4, GLdouble>& value);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::RectangularMatrix<4, 2, GLdouble>& value);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::RectangularMatrix<3, 4, GLdouble>& value);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, const Math::RectangularMatrix<4, 3, GLdouble>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::Matrix<2, GLfloat>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::Matrix<3, GLfloat>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::Matrix<4, GLfloat>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::RectangularMatrix<2, 3, GLfloat>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::RectangularMatrix<3, 2, GLfloat>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::RectangularMatrix<2, 4, GLfloat>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::RectangularMatrix<4, 2, GLfloat>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::RectangularMatrix<3, 4, GLfloat>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::RectangularMatrix<4, 3, GLfloat>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::Matrix<2, GLdouble>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::Matrix<3, GLdouble>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::Matrix<4, GLdouble>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::RectangularMatrix<2, 3, GLdouble>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::RectangularMatrix<3, 2, GLdouble>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::RectangularMatrix<2, 4, GLdouble>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::RectangularMatrix<4, 2, GLdouble>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::RectangularMatrix<3, 4, GLdouble>& value);
        void MAGNUM_LOCAL uniformImplementationDSA(GLint location, const Math::RectangularMatrix<4, 3, GLdouble>& value);
        #endif
        static UniformMatrix2fvImplementation uniformMatrix2fvImplementation;
        static UniformMatrix3fvImplementation uniformMatrix3fvImplementation;
        static UniformMatrix4fvImplementation uniformMatrix4fvImplementation;
        #ifndef MAGNUM_TARGET_GLES2
        static UniformMatrix2x3fvImplementation uniformMatrix2x3fvImplementation;
        static UniformMatrix3x2fvImplementation uniformMatrix3x2fvImplementation;
        static UniformMatrix2x4fvImplementation uniformMatrix2x4fvImplementation;
        static UniformMatrix4x2fvImplementation uniformMatrix4x2fvImplementation;
        static UniformMatrix3x4fvImplementation uniformMatrix3x4fvImplementation;
        static UniformMatrix4x3fvImplementation uniformMatrix4x3fvImplementation;
        #endif
        #ifndef MAGNUM_TARGET_GLES
        static UniformMatrix2dvImplementation uniformMatrix2dvImplementation;
        static UniformMatrix3dvImplementation uniformMatrix3dvImplementation;
        static UniformMatrix4dvImplementation uniformMatrix4dvImplementation;
        static UniformMatrix2x3dvImplementation uniformMatrix2x3dvImplementation;
        static UniformMatrix3x2dvImplementation uniformMatrix3x2dvImplementation;
        static UniformMatrix2x4dvImplementation uniformMatrix2x4dvImplementation;
        static UniformMatrix4x2dvImplementation uniformMatrix4x2dvImplementation;
        static UniformMatrix3x4dvImplementation uniformMatrix3x4dvImplementation;
        static UniformMatrix4x3dvImplementation uniformMatrix4x3dvImplementation;
        #endif

        GLuint _id;
        State state;
};

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {

template<class> struct Attribute {};

template<> struct Attribute<GLfloat> {
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

    enum class DataOption: std::uint8_t {
        Normalized = 1 << 0
    };

    typedef Corrade::Containers::EnumSet<DataOption, std::uint8_t> DataOptions;

    static const DataType DefaultDataType = DataType::Float;

    inline constexpr static GLint size(DataOptions) { return 1; }
    inline constexpr static std::size_t vectorCount() { return 1; }
};

CORRADE_ENUMSET_OPERATORS(Attribute<GLfloat>::DataOptions)

template<size_t vectorSize> struct Attribute<Math::Vector<vectorSize, GLfloat>>: public Attribute<GLfloat> {
    inline constexpr static GLint size(DataOptions) { return vectorSize; }
    inline constexpr static std::size_t vectorCount() { return 1; }
};

template<size_t cols, size_t rows> struct Attribute<Math::RectangularMatrix<cols, rows, GLfloat>>: public Attribute<GLfloat> {
    inline constexpr static GLint size(DataOptions) { return rows; }
    inline constexpr static std::size_t vectorCount() { return cols; }
};

template<size_t matrixSize> struct Attribute<Math::Matrix<matrixSize, GLfloat>>: public Attribute<GLfloat> {
    inline constexpr static GLint size(DataOptions) { return matrixSize; }
    inline constexpr static std::size_t vectorCount() { return matrixSize; }
};

template<> struct Attribute<Math::Vector<4, GLfloat>> {
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
        UnsignedAlpha2RGB10 = GL_UNSIGNED_INT_2_10_10_10_REV,
        Alpha2RGB10 = GL_INT_2_10_10_10_REV
        #endif
    };

    enum class DataOption: std::uint8_t {
        Normalized = 1 << 0

        #ifndef MAGNUM_TARGET_GLES
        ,
        BGRA = 2 << 0
        #endif
    };

    typedef Corrade::Containers::EnumSet<DataOption, std::uint8_t> DataOptions;

    static const DataType DefaultDataType = DataType::Float;

    #ifndef MAGNUM_TARGET_GLES
    inline constexpr static GLint size(DataOptions options) {
        return options & DataOption::BGRA ? GL_BGRA : 4;
    }
    #else
    inline constexpr static GLint size(DataOptions) { return 4; }
    #endif

    inline constexpr static std::size_t vectorCount() { return 1; }
};

typedef Math::Vector<4, GLfloat> _Vector4;
CORRADE_ENUMSET_OPERATORS(Attribute<_Vector4>::DataOptions)

template<> struct Attribute<GLint> {
    enum class DataType: GLenum {
        UnsignedByte = GL_UNSIGNED_BYTE,
        Byte = GL_BYTE,
        UnsignedShort = GL_UNSIGNED_SHORT,
        Short = GL_SHORT,
        UnsignedInt = GL_UNSIGNED_INT,
        Int = GL_INT
    };

    enum class DataOption: std::uint8_t {};

    typedef Corrade::Containers::EnumSet<DataOption, std::uint8_t> DataOptions;

    static const DataType DefaultDataType = DataType::Int;

    inline constexpr static GLint size() { return 1; }
};

template<> struct Attribute<GLuint> {
    typedef Attribute<GLint>::DataType DataType;
    typedef Attribute<GLint>::DataOption DataOption;

    typedef Corrade::Containers::EnumSet<DataOption, std::uint8_t> DataOptions;

    static const DataType DefaultDataType = DataType::UnsignedInt;

    inline constexpr static GLint size() { return 1; }
};

template<size_t size> struct Attribute<Math::Vector<size, GLint>>: public Attribute<GLint> {
    inline constexpr static GLint size() { return size; }
};

template<size_t size> struct Attribute<Math::Vector<size, GLuint>>: public Attribute<GLuint> {
    inline constexpr static GLint size() { return size; }
};

#ifndef MAGNUM_TARGET_GLES
template<> struct Attribute<GLdouble> {
    enum class DataType: GLenum {
        Double = GL_DOUBLE
    };

    enum class DataOption: std::uint8_t {};

    typedef Corrade::Containers::EnumSet<DataOption, std::uint8_t> DataOptions;

    static const DataType DefaultDataType = DataType::Double;

    inline constexpr static GLint size() { return 1; }
    inline constexpr static std::size_t vectorCount() { return 1; }
};

template<size_t cols, size_t rows> struct Attribute<Math::RectangularMatrix<cols, rows, GLdouble>>: public Attribute<GLdouble> {
    inline constexpr static GLint size() { return rows; }
    inline constexpr static std::size_t vectorCount() { return cols; }
};

template<size_t size> struct Attribute<Math::Matrix<size, GLdouble>>: public Attribute<GLdouble> {
    inline constexpr static GLint size() { return size; }
    inline constexpr static std::size_t vectorCount() { return size; }
};

template<size_t size> struct Attribute<Math::Vector<size, GLdouble>>: public Attribute<GLdouble> {
    inline constexpr static GLint size() { return size; }
    inline constexpr static std::size_t vectorCount() { return size; }
};
#endif

template<class T> struct Attribute<Math::Vector2<T>>: public Attribute<Math::Vector<2, T>> {};
template<class T> struct Attribute<Math::Vector3<T>>: public Attribute<Math::Vector<3, T>> {};
template<class T> struct Attribute<Math::Vector4<T>>: public Attribute<Math::Vector<4, T>> {};

template<class T> struct Attribute<Math::Point2D<T>>: public Attribute<Math::Vector3<T>> {};
template<class T> struct Attribute<Math::Point3D<T>>: public Attribute<Math::Vector4<T>> {};

template<class T> struct Attribute<Color3<T>>: public Attribute<Math::Vector3<T>> {};
template<class T> struct Attribute<Color4<T>>: public Attribute<Math::Vector4<T>> {};

template<class T> struct Attribute<Math::Matrix3<T>>: public Attribute<Math::Matrix<3, T>> {};
template<class T> struct Attribute<Math::Matrix4<T>>: public Attribute<Math::Matrix<4, T>> {};

}
#endif

}

#endif
