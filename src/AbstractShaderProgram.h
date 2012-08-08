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

#include "Shader.h"

namespace Magnum {

/** @ingroup rendering
@brief Base class for shaders

@section AbstractShaderProgram-subclassing Subclassing workflow

This class is designed to be used via subclassing. Subclasses define these
functions and properties:
 - <strong>%Attribute location</strong> typedefs defining locations and types
   for attribute binding with Mesh::bindAttribute(), for example:
@code
typedef Attribute<0, Vector4> Vertex;
typedef Attribute<1, Vector3> Normal;
typedef Attribute<2, Vector2> TextureCoords;
@endcode
 - **Layers for texture uniforms** to which the textures will be bound before
   rendering, for example:
@code
static const GLint DiffuseTextureLayer = 0;
static const GLint SpecularTextureLayer = 1;
@endcode
 - **Constructor**, which attaches particular shaders, links the program and
   gets uniform locations, for example:
@code
MyShader() {
    // Load shaders from file and attach them to the program
    attachShader(Shader::fromFile(Shader::Vertex, "PhongShader.vert"));
    attachShader(Shader::fromFile(Shader::Fragment, "PhongShader.frag"));

    // Link
    link();

    // Get locations of uniforms
    transformationMatrixUniform = uniformLocation("transformationMatrix");
    projectionMatrixUniform = uniformLocation("projectionMatrix");
    // more uniforms like light location, colors etc.
}
@endcode
 - **Uniform setting functions**, which will provide public interface for
   protected setUniform() functions. Example:
@code
void setTransformationMatrixUniform(const Matrix4& matrix) {
    setUniform(transformationMatrixUniform, matrix);
}
void setProjectionMatrixUniform(const Matrix4& matrix) {
    setUniform(projectionMatrixUniform, matrix);
}
@endcode

@subsection AbstractShaderProgram-attribute-location Binding attribute location
The preferred workflow is to specify attribute location for vertex shader
input attributes and fragment shader output attributes explicitly in the
shader code, e.g.:
@code
layout(location = 0) in vec4 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textureCoords;
@endcode
@requires_gl (for explicit input attribute location instead of using
     bindAttributeLocation())
@requires_gl (for explicit output attribute location or using
    bindFragmentDataLocation())
@requires_gl30 Extension @extension{EXT,gpu_shader4} (for using
    bindFragmentDataLocation())
@requires_gl33 Extension @extension{ARB,explicit_attrib_location} (for
    explicit attribute location instead of using bindAttributeLocation())

If you don't have the required extension, you can use functions bindAttributeLocation()
and bindFragmentDataLocation() between attaching of shaders and linking the
program:
@code
// Shaders attached...

bindAttributeLocation(Vertex::Location, "vertex");
bindAttributeLocation(Normal::Location, "normal");
bindAttributeLocation(TextureCoords::Location, "textureCoords");

// Link...
@endcode

@subsection AbstractShaderProgram-texture-layer Binding texture layer uniforms
The preferred workflow is to specify texture layers directly in the shader
code, e.g.:
@code
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
setUniform(uniformLocation("diffuseTexture"), DiffuseTextureLayer);
setUniform(uniformLocation("specularTexture"), SpecularTextureLayer);
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
         */
        inline AbstractShaderProgram(): state(Initialized) {
            program = glCreateProgram();
        }

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL shader program.
         */
        virtual ~AbstractShaderProgram() = 0;

        /**
         * @brief Use shader
         * @return False if the program wasn't successfully linked, true
         *      otherwise.
         */
        bool use();

    protected:
        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Allow retrieving program binary
         *
         * Disabled by default.
         * @requires_gl
         * @requires_gl41 Extension @extension{ARB,get_program_binary}
         * @note This function should be called after attachShader() calls and
         *      before link().
         */
        inline void setRetrievableBinary(bool enabled) {
            glProgramParameteri(program, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, enabled ? GL_TRUE : GL_FALSE);
        }

        /**
         * @brief Allow the program to be bound to individual pipeline stages
         *
         * Disabled by default.
         * @requires_gl
         * @requires_gl41 Extension @extension{ARB,separate_shader_objects}
         * @note This function should be called after attachShader() calls and
         *      before link().
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
         */
        bool attachShader(Shader& shader);

        /** @copydoc attachShader(Shader&) */
        inline bool attachShader(Shader&& shader) {
            return attachShader(shader);
        }

        /**
         * @brief Bind attribute to given location
         * @param location      Location
         * @param name          Attribute name
         *
         * Binds attribute to location which is be used later for binding
         * vertex buffers. Preferred usage is to
         * @note This function should be called after attachShader() calls and
         *      before link().
         * @deprecated Preferred usage is to specify attribute location
         *      explicitly in the shader instead of using this function. See
         *      @ref AbstractShaderProgram-attribute-location "class documentation"
         *      for more information.
         */
        void bindAttributeLocation(GLuint location, const std::string& name);

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Bind fragment data to given location
         * @param location      Location
         * @param name          Fragment output variable name
         *
         * @note This function should be called after attachShader() calls and
         *      before link().
         * @deprecated Preferred usage is to specify attribute location
         *      explicitly in the shader instead of using this function. See
         *      @ref AbstractShaderProgram-attribute-location "class documentation"
         *      for more information.
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
         */
        void link();

        /**
         * @brief Get uniform location
         * @param name          Uniform name
         *
         * @note This function should be called after link().
         */
        GLint uniformLocation(const std::string& name);

        /**
         * @brief Set uniform value
         * @param location      Uniform location (see uniformLocation())
         * @param value         Value
         *
         * @attention This function doesn't check whether this shader is in use!
         */
        inline void setUniform(GLint location, GLfloat value) {
            glUniform1f(location, value);
        }

        /** @copydoc setUniform(GLint, GLfloat) */
        inline void setUniform(GLint location, const Vector2& value) {
            glUniform2fv(location, 1, value.data());
        }

        /** @copydoc setUniform(GLint, GLfloat) */
        inline void setUniform(GLint location, const Vector3& value) {
            glUniform3fv(location, 1, value.data());
        }

        /** @copydoc setUniform(GLint, GLfloat) */
        inline void setUniform(GLint location, const Vector4& value) {
            glUniform4fv(location, 1, value.data());
        }

        /** @copydoc setUniform(GLint, GLfloat) */
        inline void setUniform(GLint location, GLint value) {
            glUniform1i(location, value);
        }

        /** @copydoc setUniform(GLint, GLint) */
        inline void setUniform(GLint location, const Math::Vector2<GLint>& value) {
            glUniform2iv(location, 1, value.data());
        }

        /** @copydoc setUniform(GLint, GLint) */
        inline void setUniform(GLint location, const Math::Vector3<GLint>& value) {
            glUniform3iv(location, 1, value.data());
        }

        /** @copydoc setUniform(GLint, GLint) */
        inline void setUniform(GLint location, const Math::Vector4<GLint>& value) {
            glUniform4iv(location, 1, value.data());
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @copydoc setUniform(GLint, GLint)
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,gpu_shader4}
         */
        inline void setUniform(GLint location, GLuint value) {
            glUniform1ui(location, value);
        }

        /**
         * @copydoc setUniform(GLint, GLint)
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,gpu_shader4}
         */
        inline void setUniform(GLint location, const Math::Vector2<GLuint>& value) {
            glUniform2uiv(location, 1, value.data());
        }

        /**
         * @copydoc setUniform(GLint, GLint)
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,gpu_shader4}
         */
        inline void setUniform(GLint location, const Math::Vector3<GLuint>& value) {
            glUniform3uiv(location, 1, value.data());
        }

        /**
         * @copydoc setUniform(GLint, GLuint)
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,gpu_shader4}
         */
        inline void setUniform(GLint location, const Math::Vector4<GLuint>& value) {
            glUniform4uiv(location, 1, value.data());
        }
        #endif

        /** @copydoc setUniform(GLint, GLint) */
        inline void setUniform(GLint location, const Matrix3& value) {
            glUniformMatrix3fv(location, 1, GL_FALSE, value.data());
        }

        /** @copydoc setUniform(GLint, GLint) */
        inline void setUniform(GLint location, const Matrix4& value) {
            glUniformMatrix4fv(location, 1, GL_FALSE, value.data());
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
