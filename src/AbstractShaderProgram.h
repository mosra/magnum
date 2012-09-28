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

#include <map>

#include "Math/RectangularMatrix.h"
#include "Magnum.h"

#include "magnumVisibility.h"

namespace Magnum {

class Shader;

/**
@brief Base class for shaders

@section AbstractShaderProgram-subclassing Subclassing workflow

This class is designed to be used via subclassing. Subclasses define these
functions and properties:
 - <strong>%Attribute location</strong> typedefs defining locations and types
   for attribute binding with Mesh::bindAttribute(), for example:
@code
typedef Attribute<0, Point3D> Position;
typedef Attribute<1, Vector3> Normal;
typedef Attribute<2, Vector2> TextureCoords;
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
static const GLint TransformationMatrixUniform = 0;
static const GLint ProjectionMatrixUniform = 1;
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
   protected setUniform() functions. Example:
@code
void setTransformationMatrixUniform(const Matrix4& matrix) {
    setUniform(TransformationMatrixUniform, matrix);
}
void setProjectionMatrixUniform(const Matrix4& matrix) {
    setUniform(ProjectionMatrixUniform, matrix);
}
@endcode

@subsection AbstractShaderProgram-attribute-location Binding attribute location
The preferred workflow is to specify attribute location for vertex shader
input attributes and fragment shader output attributes explicitly in the
shader code, e.g.:
@code
#version 330
// or #extension GL_ARB_explicit_attrib_location: enable
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textureCoords;
@endcode
Similarly for ouput attributes, you can also specify blend equation color
index for them (see Framebuffer::BlendFunction for more information about
using color input index):
@code
layout(location = 0, index = 0) out vec4 color;
layout(location = 1, index = 1) out vec4 ambient;
@endcode
@requires_gl (for explicit input attribute location instead of using
     bindAttributeLocation())
@requires_gl (for using bindFragmentDataLocation() /
    bindFragmentDataLocationIndexed())
@requires_gl30 Extension @extension{EXT,gpu_shader4} (for using
    bindFragmentDataLocation())
@requires_gl33 Extension @extension{ARB,blend_func_extended} (for using
    bindFragmentDataLocationIndexed())
@requires_gl33 Extension @extension{ARB,explicit_attrib_location} (for
    explicit attribute location instead of using bindAttributeLocation())
@requires_gles30 (no extension providing this functionality) (for explicit
    input and output attribute location)

If you don't have the required extension, you can use functions bindAttributeLocation()
and bindFragmentDataLocation() / bindFragmentDataLocationIndexed() between
attaching the shaders and linking the program:
@code
// Shaders attached...

bindAttributeLocation(Position::Location, "position");
bindAttributeLocation(Normal::Location, "normal");
bindAttributeLocation(TextureCoords::Location, "textureCoords");

bindFragmentDataLocationIndexed(0, 0, "color");
bindFragmentDataLocationIndexed(1, 1, "ambient");

// Link...
@endcode

@subsection AbstractShaderProgram-uniform-location Uniform locations
The preferred workflow is to specify uniform locations directly in the shader
code, e.g.:
@code
#version 430
// or #extension GL_ARB_explicit_uniform_location: enable
layout(location = 0) uniform mat4 transformationMatrix;
layout(location = 1) uniform mat4 projectionMatrix;
@endcode
@requires_gl (for explicit uniform location instead of using uniformLocation())
@requires_gl43 Extension @extension{ARB,explicit_uniform_location} (for
    explicit uniform location instead of using uniformLocation())

If you don't have the required extension, you can get uniform location using
uniformLocation() after linking stage:
@code
GLint transformationMatrixUniform = uniformLocation("transformationMatrix");
GLint projectionMatrixUniform = uniformLocation("projectionMatrix");
@endcode

@subsection AbstractShaderProgram-texture-layer Binding texture layer uniforms
The preferred workflow is to specify texture layers directly in the shader
code, e.g.:
@code
#version 420
// or #extension GL_ARB_shading_language_420pack: enable
layout(binding = 0) uniform sampler2D diffuseTexture;
layout(binding = 1) uniform sampler2D specularTexture;
@endcode
@requires_gl (for explicit texture layer binding instead of using
    setUniform(GLint, GLint))
@requires_gl42 Extension @extension{ARB,shading_language_420pack} (for explicit
    texture layer binding instead of using setUniform(GLint, GLint))

If you don't have the required extension (or if you want to change the layer
later), you can set the texture layer uniform using setUniform(GLint, GLint):
@code
use();
setUniform(DiffuseTextureUniform, DiffuseTextureLayer);
setUniform(SpecularTextureUniform, SpecularTextureLayer);
@endcode

@section AbstractShaderProgram-rendering-workflow Rendering workflow

Basic workflow with %AbstractShaderProgram subclasses is: instancing the class
(once at the beginning), then in Object::draw() reimplementation calling
use(), setting uniforms, binding required textures to their respective layers
using AbstractTexture::bind(GLint) and calling Mesh::draw(). For example:
@code
void draw(const Magnum::Matrix4& transformationMatrix, Magnum::Camera* camera) {
    shader.use();
    shader.setTransformationMatrixUniform(transformationMatrix);
    shader.setProjectionMatrixUniform(camera->projectionMatrix());
    diffuseTexture.bind(MyShader::DiffuseTextureLayer);
    specularTexture.bind(MyShader::SpecularTextureLayer);
    mesh.draw();
}
@endcode

@todo Uniform arrays support
@todo DSA for uniforms - glProgramUniform*() (OpenGL 4.1, @extension{ARB,separate_shader_objects})
 */
class MAGNUM_EXPORT AbstractShaderProgram {
    AbstractShaderProgram(const AbstractShaderProgram& other) = delete;
    AbstractShaderProgram(AbstractShaderProgram&& other) = delete;
    AbstractShaderProgram& operator=(const AbstractShaderProgram& other) = delete;
    AbstractShaderProgram& operator=(AbstractShaderProgram&& other) = delete;

    public:
        /**
         * @brief Base struct for attribute location and type
         *
         * See AbstractShaderProgram documentation or Mesh::bindAttribute()
         * for an example.
         *
         * @todo Support for BGRA attribute type (OpenGL 3.2, @extension{ARB,vertex_array_bgra})
         */
        template<size_t i, class T> struct Attribute {
            static const size_t Location = i;   /**< Location to which the attribute is bound */
            typedef T Type;                     /**< %Attribute type */
        };

        /**
         * @brief Constructor
         *
         * Creates one OpenGL shader program.
         * @see @fn_gl{CreateProgram}
         */
        inline AbstractShaderProgram(): state(Initialized) {
            program = glCreateProgram();
        }

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL shader program.
         * @see @fn_gl{DeleteProgram}
         */
        virtual ~AbstractShaderProgram() = 0;

        /**
         * @brief Use shader
         * @return False if the program wasn't successfully linked, true
         *      otherwise.
         *
         * @see @fn_gl{UseProgram}
         */
        bool use();

    protected:
        /**
         * @brief Allow retrieving program binary
         *
         * Initially disabled.
         * @note This function should be called after attachShader() calls and
         *      before link().
         * @see @fn_gl{ProgramParameter} with @def_gl{PROGRAM_BINARY_RETRIEVABLE_HINT}
         * @requires_gl
         * @requires_gl41 Extension @extension{ARB,get_program_binary}
         * @requires_gles30 (no extension providing this functionality)
         */
        inline void setRetrievableBinary(bool enabled) {
            glProgramParameteri(program, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, enabled ? GL_TRUE : GL_FALSE);
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Allow the program to be bound to individual pipeline stages
         *
         * Initially disabled.
         * @note This function should be called after attachShader() calls and
         *      before link().
         * @see @fn_gl{ProgramParameter} with @def_gl{PROGRAM_SEPARABLE}
         * @requires_gl
         * @requires_gl41 Extension @extension{ARB,separate_shader_objects}
         */
        inline void setSeparable(bool enabled) {
            glProgramParameteri(program, GL_PROGRAM_SEPARABLE, enabled ? GL_TRUE : GL_FALSE);
        }
        #endif

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
         * @requires_gl
         * @requires_gl33 Extension @extension{ARB,blend_func_extended}
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
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,gpu_shader4}
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
         * @attention This function doesn't check whether this shader is in use!
         * @see @fn_gl{Uniform}
         */
        inline void setUniform(GLint location, GLfloat value) {
            glUniform1f(location, value);
        }

        /** @copydoc setUniform(GLint, GLfloat) */
        inline void setUniform(GLint location, const Math::RectangularMatrix<1, 2, GLfloat>& value) {
            glUniform2fv(location, 1, value.data());
        }

        /** @copydoc setUniform(GLint, GLfloat) */
        inline void setUniform(GLint location, const Math::RectangularMatrix<1, 3, GLfloat>& value) {
            glUniform3fv(location, 1, value.data());
        }

        /** @copydoc setUniform(GLint, GLfloat) */
        inline void setUniform(GLint location, const Math::RectangularMatrix<1, 4, GLfloat>& value) {
            glUniform4fv(location, 1, value.data());
        }

        /** @copydoc setUniform(GLint, GLfloat) */
        inline void setUniform(GLint location, GLint value) {
            glUniform1i(location, value);
        }

        /** @copydoc setUniform(GLint, GLfloat) */
        inline void setUniform(GLint location, const Math::RectangularMatrix<1, 2, GLint>& value) {
            glUniform2iv(location, 1, value.data());
        }

        /** @copydoc setUniform(GLint, GLfloat) */
        inline void setUniform(GLint location, const Math::RectangularMatrix<1, 3, GLint>& value) {
            glUniform3iv(location, 1, value.data());
        }

        /** @copydoc setUniform(GLint, GLfloat) */
        inline void setUniform(GLint location, const Math::RectangularMatrix<1, 4, GLint>& value) {
            glUniform4iv(location, 1, value.data());
        }

        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gl30 Extension @extension{EXT,gpu_shader4}
         * @requires_gles30 (no extension providing this functionality)
         */
        inline void setUniform(GLint location, GLuint value) {
            glUniform1ui(location, value);
        }

        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gl30 Extension @extension{EXT,gpu_shader4}
         * @requires_gles30 (no extension providing this functionality)
         */
        inline void setUniform(GLint location, const Math::RectangularMatrix<1, 2, GLuint>& value) {
            glUniform2uiv(location, 1, value.data());
        }

        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gl30 Extension @extension{EXT,gpu_shader4}
         * @requires_gles30 (no extension providing this functionality)
         */
        inline void setUniform(GLint location, const Math::RectangularMatrix<1, 3, GLuint>& value) {
            glUniform3uiv(location, 1, value.data());
        }

        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gl30 Extension @extension{EXT,gpu_shader4}
         * @requires_gles30 (no extension providing this functionality)
         */
        inline void setUniform(GLint location, const Math::RectangularMatrix<1, 4, GLuint>& value) {
            glUniform4uiv(location, 1, value.data());
        }

        /** @copydoc setUniform(GLint, GLfloat) */
        inline void setUniform(GLint location, const Math::RectangularMatrix<2, 2, GLfloat>& value) {
            glUniformMatrix2fv(location, 1, GL_FALSE, value.data());
        }

        /** @copydoc setUniform(GLint, GLfloat) */
        inline void setUniform(GLint location, const Math::RectangularMatrix<3, 3, GLfloat>& value) {
            glUniformMatrix3fv(location, 1, GL_FALSE, value.data());
        }

        /** @copydoc setUniform(GLint, GLfloat) */
        inline void setUniform(GLint location, const Math::RectangularMatrix<4, 4, GLfloat>& value) {
            glUniformMatrix4fv(location, 1, GL_FALSE, value.data());
        }

        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gles30 (no extension providing this functionality)
         */
        inline void setUniform(GLint location, const Math::RectangularMatrix<2, 3, GLfloat>& value) {
            glUniformMatrix2x3fv(location, 1, GL_FALSE, value.data());
        }

        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gles30 (no extension providing this functionality)
         */
        inline void setUniform(GLint location, const Math::RectangularMatrix<3, 2, GLfloat>& value) {
            glUniformMatrix3x2fv(location, 1, GL_FALSE, value.data());
        }

        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gles30 (no extension providing this functionality)
         */
        inline void setUniform(GLint location, const Math::RectangularMatrix<2, 4, GLfloat>& value) {
            glUniformMatrix2x4fv(location, 1, GL_FALSE, value.data());
        }

        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gles30 (no extension providing this functionality)
         */
        inline void setUniform(GLint location, const Math::RectangularMatrix<4, 2, GLfloat>& value) {
            glUniformMatrix4x2fv(location, 1, GL_FALSE, value.data());
        }

        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gles30 (no extension providing this functionality)
         */
        inline void setUniform(GLint location, const Math::RectangularMatrix<3, 4, GLfloat>& value) {
            glUniformMatrix3x4fv(location, 1, GL_FALSE, value.data());
        }

        /**
         * @copydoc setUniform(GLint, GLfloat)
         * @requires_gles30 (no extension providing this functionality)
         */
        inline void setUniform(GLint location, const Math::RectangularMatrix<4, 3, GLfloat>& value) {
            glUniformMatrix4x3fv(location, 1, GL_FALSE, value.data());
        }

    private:
        enum State {
            Initialized,
            Linked,
            Failed
        };

        GLuint program;
        State state;
};

inline AbstractShaderProgram::~AbstractShaderProgram() { glDeleteProgram(program); }

}

#endif
