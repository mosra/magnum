#ifndef Magnum_AbstractShaderProgram_h
#define Magnum_AbstractShaderProgram_h
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
 * @brief Class @ref Magnum::AbstractShaderProgram
 */

#include <functional>
#include <string>
#include <Corrade/Containers/ArrayView.h>

#include "Magnum/AbstractObject.h"
#include "Magnum/Attribute.h"
#include "Magnum/Tags.h"

#if defined(CORRADE_TARGET_WINDOWS) && !defined(MAGNUM_TARGET_GLES2)
#include <vector>
#endif

namespace Magnum {

namespace Implementation { struct ShaderProgramState; }

/**
@brief Base for shader program implementations

@anchor AbstractShaderProgram-subclassing
## Subclassing workflow

This class is designed to be used via subclassing. Subclasses define these
functions and properties:
-   **Attribute definitions** with location and type for
    configuring meshes, for example:
@code
typedef Attribute<0, Vector3> Position;
typedef Attribute<1, Vector3> Normal;
typedef Attribute<2, Vector2> TextureCoordinates;
@endcode
-   **Output attribute locations**, if desired, for example:
@code
enum: UnsignedInt {
    ColorOutput = 0,
    NormalOutput = 1
};
@endcode
-   **Constructor**, which loads, compiles and attaches particular shaders and
    links the program together, for example:
@code
MyShader() {
    // Load shader sources
    Shader vert(Version::GL430, Shader::Type::Vertex);
    Shader frag(Version::GL430, Shader::Type::Fragment);
    vert.addFile("MyShader.vert");
    frag.addFile("MyShader.frag");

    // Invoke parallel compilation for best performance
    CORRADE_INTERNAL_ASSERT_OUTPUT(Shader::compile({vert, frag}));

    // Attach the shaders
    attachShaders({vert, frag});

    // Link the program together
    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}
@endcode
-   **Uniform setting functions**, which will provide public interface for
    protected @ref setUniform() functions. For usability purposes you can
    implement also method chaining. Example:
@code
MyShader& setProjectionMatrix(const Matrix4& matrix) {
    setUniform(0, matrix);
    return *this;
}
MyShader& setTransformationMatrix(const Matrix4& matrix) {
    setUniform(1, matrix);
    return *this;
}
MyShader& setNormalMatrix(const Matrix3x3& matrix) {
    setUniform(2, matrix);
    return *this;
}
@endcode
-   **Texture and texture image setting functions** in which you bind the
    textures/images to particular texture/image units using
    @ref Texture::bind() "*Texture::bind()" /
    @ref Texture::bindImage() "*Texture::bindImage()" and similar, for example:
@code
MyShader& setDiffuseTexture(Texture2D& texture) {
    texture.bind(0);
    return *this;
}
MyShader& setSpecularTexture(Texture2D& texture) {
    texture.bind(1);
    return *this;
}
@endcode
-   **Transform feedback setup function**, if needed, in which you bind buffers
    to particular indices using @ref TransformFeedback::attachBuffer() and
    similar, possibly with overloads based on desired use cases, e.g.:
@code
MyShader& setTransformFeedback(TransformFeedback& feedback, Buffer& positions, Buffer& data) {
    feedback.attachBuffers(0, {positions, data});
    return *this;
}
MyShader& setTransformFeedback(TransformFeedback& feedback, Int totalCount, Buffer& positions, GLintptr positionsOffset, Buffer& data, GLintptr dataOffset) {
    feedback.attachBuffers(0, {
        std::make_tuple(positions, positionsOffset, totalCount*sizeof(Vector3)),
        std::make_tuple(data, dataOffset, totalCount*sizeof(Vector2ui))
    });
    return *this;
}
@endcode

@anchor AbstractShaderProgram-attribute-location
### Binding attribute location

The preferred workflow is to specify attribute location for vertex shader input
attributes and fragment shader output attributes explicitly in the shader code,
e.g.:
@code
// GLSL 3.30, GLSL ES 3.00 or
#extension GL_ARB_explicit_attrib_location: require
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textureCoordinates;
@endcode

Similarly for ouput attributes, you can also specify blend equation color index
for them (see @ref Renderer::BlendFunction for more information about using
color input index):
@code
layout(location = 0, index = 0) out vec4 color;
layout(location = 1, index = 1) out vec3 normal;
@endcode

If you don't have the required version/extension, declare the attributes
without `layout()` qualifier and use functions @ref bindAttributeLocation() and
@ref bindFragmentDataLocation() / @ref bindFragmentDataLocationIndexed() between
attaching the shaders and linking the program. Note that additional syntax
changes may be needed for GLSL 1.20 and GLSL ES.
@code
in vec4 position;
in vec3 normal;
in vec2 textureCoordinates;
@endcode
@code
out vec4 color;
out vec3 normal;
@endcode
@code
// Shaders attached...

bindAttributeLocation(Position::Location, "position");
bindAttributeLocation(Normal::Location, "normal");
bindAttributeLocation(TextureCoordinates::Location, "textureCoordinates");

bindFragmentDataLocationIndexed(ColorOutput, 0, "color");
bindFragmentDataLocationIndexed(NormalOutput, 1, "normal");

// Link...
@endcode

@see @ref maxVertexAttributes(), @ref AbstractFramebuffer::maxDrawBuffers()
@requires_gl30 Extension @extension{EXT,gpu_shader4} for using
    @ref bindFragmentDataLocation().
@requires_gl33 Extension @extension{ARB,blend_func_extended} for using
    @ref bindFragmentDataLocationIndexed().
@requires_gl33 Extension @extension{ARB,explicit_attrib_location} for
    explicit attribute location instead of using
    @ref bindAttributeLocation(), @ref bindFragmentDataLocation() or
    @ref bindFragmentDataLocationIndexed().
@requires_gles30 Explicit location specification of input attributes is not
    supported in OpenGL ES 2.0, use @ref bindAttributeLocation()
    instead.
@requires_gles30 Multiple fragment shader outputs are not available in OpenGL
    ES 2.0, similar functionality is available in extension
    @extension{EXT,draw_buffers} or @extension{NV,draw_buffers}.
@requires_webgl20 Explicit location specification of input attributes is not
    supported in WebGL 1.0, use @ref bindAttributeLocation() instead.
@requires_webgl20 Multiple fragment shader outputs are not available in WebGL
    1.0, similar functionality is available in extension
    @webgl_extension{WEBGL,draw_buffers}.

@todo @extension{EXT,separate_shader_objects} supports explicit attrib
    location

@anchor AbstractShaderProgram-uniform-location
### Uniform locations

The preferred workflow is to specify uniform locations directly in the shader
code, e.g.:
@code
// GLSL 4.30, GLSL ES 3.10 or
#extension GL_ARB_explicit_uniform_location: require
layout(location = 0) uniform mat4 projectionMatrix;
layout(location = 1) uniform mat4 transformationMatrix;
layout(location = 2) uniform mat3 normalMatrix;
@endcode

If you don't have the required version/extension, declare the uniforms without
the `layout()` qualifier, get uniform location using @ref uniformLocation() *after*
linking stage and then use the queried location in uniform setting functions.
Note that additional syntax changes may be needed for GLSL 1.20 and GLSL ES.
@code
uniform mat4 projectionMatrix;
uniform mat4 transformationMatrix;
uniform mat3 normalMatrix;
@endcode
@code
Int projectionMatrixUniform = uniformLocation("projectionMatrix");
Int transformationMatrixUniform = uniformLocation("transformationMatrix");
Int normalMatrixUniform = uniformLocation("normalMatrix");
@endcode

@see @ref maxUniformLocations()
@requires_gl43 Extension @extension{ARB,explicit_uniform_location} for
    explicit uniform location instead of using @ref uniformLocation().
@requires_gles31 Explicit uniform location is not supported in OpenGL ES 3.0
    and older. Use @ref uniformLocation() instead.
@requires_gles Explicit uniform location is not supported in WebGL. Use
    @ref uniformLocation() instead.

@anchor AbstractShaderProgram-uniform-block-binding
### Uniform block bindings

The preferred workflow is to specify uniform block binding directly in the
shader code, e.g.:
@code
// GLSL 4.20, GLSL ES 3.10 or
#extension GL_ARB_shading_language_420pack: require
layout(std140, binding = 0) uniform matrices {
    mat4 projectionMatrix;
    mat4 transformationMatrix;
};
layout(std140, binding = 1) uniform material {
    vec4 diffuse;
    vec4 specular;
};
@endcode

If you don't have the required version/extension, declare the uniform blocks
without the `layout()` qualifier, get uniform block index using
@ref uniformBlockIndex() and then map it to the uniform buffer binding using
@ref setUniformBlockBinding(). Note that additional syntax changes may be
needed for GLSL ES.
@code
layout(std140) uniform matrices {
    mat4 projectionMatrix;
    mat4 transformationMatrix;
};
layout(std140) uniform material {
    vec4 diffuse;
    vec4 specular;
};
@endcode
@code
setUniformBlockBinding(uniformBlockIndex("matrices"), 0);
setUniformBlockBinding(uniformBlockIndex("material"), 1);
@endcode

@see @ref Buffer::maxUniformBindings()
@requires_gl31 Extension @extension{ARB,uniform_buffer_object}
@requires_gl42 Extension @extension{ARB,shading_language_420pack} for explicit
    uniform block binding instead of using @ref uniformBlockIndex() and
    @ref setUniformBlockBinding().
@requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
@requires_gles31 Explicit uniform block binding is not supported in OpenGL ES
    3.0 and older. Use @ref uniformBlockIndex() and @ref setUniformBlockBinding()
    instead.
@requires_webgl20 Uniform buffers are not available in WebGL 1.0.
@requires_gles Explicit uniform block binding is not supported in WebGL. Use
    @ref uniformBlockIndex() and @ref setUniformBlockBinding() instead.

@anchor AbstractShaderProgram-shader-storage-block-binding
### Shader storage block bindings

The workflow is to specify shader storage block binding directly in the shader
code, e.g.:
@code
// GLSL 4.30 or GLSL ES 3.10
layout(std430, binding = 0) buffer vertices {
    vec3 position;
    vec3 color;
};
layout(std430, binding = 1) buffer normals {
    vec3 normal;
};
@endcode

@see @ref Buffer::maxShaderStorageBindings()
@requires_gl43 Extension @extension{ARB,shader_storage_buffer_object}
@requires_gles31 Shader storage is not available in OpenGL ES 3.0 and older.
@requires_gles Shader storage is not available in WebGL.

@anchor AbstractShaderProgram-texture-units
### Specifying texture and image binding units

The preferred workflow is to specify texture/image binding unit directly in the
shader code, e.g.:
@code
// GLSL 4.20, GLSL ES 3.10 or
#extension GL_ARB_shading_language_420pack: require
layout(binding = 0) uniform sampler2D diffuseTexture;
layout(binding = 1) uniform sampler2D specularTexture;
@endcode

If you don't have the required version/extension, declare the uniforms without
the `binding` qualifier and set the texture binding unit using
@ref setUniform(Int, const T&) "setUniform(Int, Int)". Note that additional
syntax changes may be needed for GLSL ES.
@code
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
@endcode
@code
setUniform(uniformLocation("diffuseTexture"), 0);
setUniform(uniformLocation("specularTexture"), 1);
@endcode

@see @ref Shader::maxTextureImageUnits(), @ref maxImageUnits()
@requires_gl42 Extension @extension{ARB,shading_language_420pack} for explicit
    texture binding unit instead of using
    @ref setUniform(Int, const T&) "setUniform(Int, Int)".
@requires_gles31 Explicit texture binding unit is not supported in OpenGL ES
    3.0 and older. Use @ref setUniform(Int, const T&) "setUniform(Int, Int)"
    instead.
@requires_gles Explicit texture binding unit is not supported in WebGL. Use
    @ref setUniform(Int, const T&) "setUniform(Int, Int)" instead.

@anchor AbstractShaderProgram-transform-feedback
### Specifying transform feedback binding points

The preferred workflow is to specify output binding points directly in the
shader code, e.g.:
@code
// GLSL 4.40, or
#extension GL_ARB_enhanced_layouts: require
layout(xfb_buffer = 0, xfb_stride = 32) out block {
    layout(xfb_offset = 0) vec3 position;
    layout(xfb_offset = 16) vec3 normal;
};
layout(xfb_buffer = 1) out vec3 velocity;
@endcode

If you don't have the required version/extension, declare the uniforms without
the `xfb_*` qualifier and set the binding points using @ref setTransformFeedbackOutputs().
Equivalent setup for the previous code would be the following:
@code
out block {
    vec3 position;
    vec3 normal;
};
out vec3 velocity;
@endcode
@code
setTransformFeedbackOutputs({
        // Buffer 0
        "position", "gl_SkipComponents1", "normal", "gl_SkipComponents1",
        // Buffer 1
        "gl_NextBuffer", "velocity"
    }, TransformFeedbackBufferMode::InterleavedAttributes);
@endcode

@see @ref TransformFeedback::maxInterleavedComponents(),
    @ref TransformFeedback::maxSeparateAttributes(),
    @ref TransformFeedback::maxSeparateComponents()
@requires_gl40 Extension @extension{ARB,transform_feedback2}
@requires_gl40 Extension @extension{ARB,transform_feedback3} for using
    `gl_NextBuffer` or `gl_SkipComponents#` names in
    @ref setTransformFeedbackOutputs() function.
@requires_gl44 Extension @extension{ARB,enhanced_layouts} for explicit
    transform feedback output specification instead of using
    @ref setTransformFeedbackOutputs().
@requires_gles30 Transform feedback is not available in OpenGL ES 2.0.
@requires_gl Explicit transform feedback output specification is not available
    in OpenGL ES or WebGL.
@requires_webgl20 Transform feedback is not available in WebGL 1.0.

@anchor AbstractShaderProgram-rendering-workflow
## Rendering workflow

Basic workflow with AbstractShaderProgram subclasses is: instance shader
class, configure attribute binding in meshes (see @ref Mesh-configuration "Mesh documentation"
for more information) and map shader outputs to framebuffer attachments if
needed (see @ref Framebuffer-usage "Framebuffer documentation" for more
information). In each draw event set all required shader parameters, bind
specific framebuffer (if needed) and then call @ref Mesh::draw(). Example:
@code
shader.setTransformation(transformation)
    .setProjection(projection)
    .setDiffuseTexture(diffuseTexture)
    .setSpecularTexture(specularTexture);

mesh.draw(shader);
@endcode

@anchor AbstractShaderProgram-compute-workflow
## Compute workflow

Add just the @ref Shader::Type::Compute shader and implement uniform/texture
setting functions as needed. After setting up required parameters call
@ref dispatchCompute().

@anchor AbstractShaderProgram-types
## Mapping between GLSL and Magnum types

See @ref types for more information, only types with GLSL equivalent can be used
(and their super- or subclasses with the same size and underlying type). See
also @ref Attribute::DataType enum for additional type options.

@requires_gl30 Extension @extension{EXT,gpu_shader4} is required when using
    integer attributes (i.e. @ref UnsignedInt, @ref Int, @ref Vector2ui,
    @ref Vector2i, @ref Vector3ui, @ref Vector3i, @ref Vector4ui and
    @ref Vector4i) or unsigned integer uniforms (i.e. @ref UnsignedInt,
    @ref Vector2ui, @ref Vector3ui and @ref Vector4ui).
@requires_gl40 Extension @extension{ARB,gpu_shader_fp64} is required when
    using double uniforms (i.e. @ref Double, @ref Vector2d, @ref Vector3d,
    @ref Vector4d, @ref Matrix2x2d, @ref Matrix3x3d, @ref Matrix4x4d,
    @ref Matrix2x3d, @ref Matrix3x2d, @ref Matrix2x4d, @ref Matrix4x2d,
    @ref Matrix3x4d and @ref Matrix4x3d).
@requires_gl41 Extension @extension{ARB,vertex_attrib_64bit} is required when
    using double attributes (i.e. @ref Double, @ref Vector2d, @ref Vector3d,
    @ref Vector4d, @ref Matrix2x2d, @ref Matrix3x3d, @ref Matrix4x4d,
    @ref Matrix2x3d, @ref Matrix3x2d, @ref Matrix2x4d, @ref Matrix4x2d,
    @ref Matrix3x4d and @ref Matrix4x3d).
@requires_gles30 Integer attributes, unsigned integer uniforms and non-square
    matrix attributes and uniforms (i.e. @ref Matrix2x3, @ref Matrix3x2,
    @ref Matrix2x4, @ref Matrix4x2, @ref Matrix3x4 and @ref Matrix4x3) are not
    available in OpenGL ES 2.0.
@requires_gl Double attributes and uniforms are not available in OpenGL ES or
    WebGL.
@requires_webgl20 Integer attributes, unsigned integer uniforms and non-square
    matrix attributes and uniforms (i.e. @ref Matrix2x3, @ref Matrix3x2,
    @ref Matrix2x4, @ref Matrix4x2, @ref Matrix3x4 and @ref Matrix4x3) are not
    available in WebGL 1.0.

@anchor AbstractShaderProgram-performance-optimization
## Performance optimizations

The engine tracks currently used shader program to avoid unnecessary calls to
@fn_gl{UseProgram}. Shader limits (such as @ref maxVertexAttributes()) are
cached, so repeated queries don't result in repeated @fn_gl{Get} calls. See
also @ref Context::resetState() and @ref Context::State::Shaders.

If extension @extension{ARB,separate_shader_objects} (part of OpenGL 4.1),
@extension{EXT,direct_state_access} desktop extension, @extension{EXT,separate_shader_objects}
OpenGL ES extension or OpenGL ES 3.1 is available, uniform setting functions
use DSA functions to avoid unnecessary calls to @fn_gl{UseProgram}. See
@ref setUniform() documentation for more information.

To achieve least state changes, set all uniforms in one run -- method chaining
comes in handy.

@see @ref portability-shaders

@todo `GL_NUM_{PROGRAM,SHADER}_BINARY_FORMATS` + `GL_{PROGRAM,SHADER}_BINARY_FORMATS` (vector), (@extension{ARB,ES2_compatibility})
 */
class MAGNUM_EXPORT AbstractShaderProgram: public AbstractObject {
    friend Mesh;
    friend MeshView;
    #ifndef MAGNUM_TARGET_GLES2
    friend TransformFeedback;
    #endif
    friend Implementation::ShaderProgramState;

    public:
        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Buffer mode for transform feedback
         *
         * @see @ref setTransformFeedbackOutputs()
         * @requires_gl30 Extension @extension{EXT,transform_feedback}
         * @requires_gles30 Transform feedback is not available in OpenGL ES
         *      2.0.
         * @requires_webgl20 Transform feedback is not available in WebGL 1.0.
         */
        enum class TransformFeedbackBufferMode: GLenum {
            /** Attributes will be interleaved at one buffer binding point */
            InterleavedAttributes = GL_INTERLEAVED_ATTRIBS,

            /** Each attribute will be put into separate buffer binding point */
            SeparateAttributes = GL_SEPARATE_ATTRIBS
        };
        #endif

        /**
         * @brief Max supported vertex attribute count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls.
         * @see @ref Mesh::maxVertexAttributes(),
         *      @ref Attribute, @fn_gl{Get} with @def_gl{MAX_VERTEX_ATTRIBS}
         */
        static Int maxVertexAttributes();

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @brief Max supported atomic counter buffer size
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @extension{ARB,shader_atomic_counters}
         * (part of OpenGL 4.2) nor OpenGL ES 3.1 is available, returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_ATOMIC_COUNTER_BUFFER_SIZE}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Atomic counters are not available in WebGL.
         */
        static Int maxAtomicCounterBufferSize();

        /**
         * @brief Max supported compute shared memory size
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @extension{ARB,compute_shader}
         * (part of OpenGL 4.3) nor OpenGL ES 3.1 is available, returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_COMPUTE_SHARED_MEMORY_SIZE}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Compute shaders are not available in WebGL.
         */
        static Int maxComputeSharedMemorySize();

        /**
         * @brief Max supported compute work group invocation count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @extension{ARB,compute_shader}
         * (part of OpenGL 4.3) nor OpenGL ES 3.1 is available, returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_COMPUTE_WORK_GROUP_INVOCATIONS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Compute shaders are not available in WebGL.
         */
        static Int maxComputeWorkGroupInvocations();

        /**
         * @brief Max supported compute work group count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @extension{ARB,compute_shader}
         * (part of OpenGL 4.3) nor OpenGL ES 3.1 is available, returns zero
         * vector.
         * @see @fn_gl{Get} with @def_gl{MAX_COMPUTE_WORK_GROUP_COUNT}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Compute shaders are not available in WebGL.
         */
        static Vector3i maxComputeWorkGroupCount();

        /**
         * @brief Max supported compute work group size
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @extension{ARB,compute_shader}
         * (part of OpenGL 4.3) nor OpenGL ES 3.1 is available, returns zero
         * vector.
         * @see @fn_gl{Get} with @def_gl{MAX_COMPUTE_WORK_GROUP_SIZE}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Compute shaders are not available in WebGL.
         */
        static Vector3i maxComputeWorkGroupSize();

        /**
         * @brief Max supported image unit count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,shader_image_load_store}
         * (part of OpenGL 4.2) or OpenGL ES 3.1 is not available, returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_IMAGE_UNITS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Shader image load/store is not available in WebGL.
         */
        static Int maxImageUnits();
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Max supported image sample count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,shader_image_load_store}
         * (part of OpenGL 4.2) is not available, returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_IMAGE_SAMPLES}
         * @requires_gl Multisample image load/store is not available in OpenGL
         *      ES or WebGL.
         */
        static Int maxImageSamples();
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Max supported combined shader output resource count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @extension{ARB,shader_image_load_store}
         * (part of OpenGL 4.2) nor extension @extension{ARB,shader_storage_buffer_object}
         * (part of OpenGL 4.3) nor OpenGL ES 3.1 is available, returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_COMBINED_SHADER_OUTPUT_RESOURCES}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Shader image load/store is not available in WebGL.
         */
        static Int maxCombinedShaderOutputResources();

        /**
         * @brief Max supported shader storage block size
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @extension{ARB,shader_storage_buffer_object}
         * (part of OpenGL 4.3) nor OpenGL ES 3.1 is available, returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_SHADER_STORAGE_BLOCK_SIZE}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Shader storage is not available in WebGL.
         */
        static Long maxShaderStorageBlockSize();
        #endif

        /**
         * @brief Max supported uniform block size
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,uniform_buffer_object}
         * (part of OpenGL 3.1) is not available, returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_UNIFORM_BLOCK_SIZE}
         * @requires_gles30 Uniform blocks are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform blocks are not available in WebGL 1.0.
         */
        static Int maxUniformBlockSize();

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Max supported explicit uniform location count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @extension{ARB,explicit_uniform_location}
         * (part of OpenGL 4.3) nor OpenGL ES 3.1 is available, returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_UNIFORM_LOCATIONS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Explicit uniform location is not available in WebGL.
         */
        static Int maxUniformLocations();
        #endif

        /**
         * @brief Min supported program texel offset
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{EXT,gpu_shader4} (part of
         * OpenGL 3.0) is not available, returns `0`.
         * @see @fn_gl{Get} with @def_gl{MIN_PROGRAM_TEXEL_OFFSET}
         * @requires_gles30 Texture lookup with offset is not available in
         *      OpenGL ES 2.0.
         * @requires_webgl20 Texture lookup with offset is not available in
         *      WebGL 1.0.
         */
        static Int minTexelOffset();

        /**
         * @brief Max supported program texel offset
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{EXT,gpu_shader4} (part of
         * OpenGL 3.0) is not available, returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_PROGRAM_TEXEL_OFFSET}
         * @requires_gles30 Texture lookup with offset is not available in
         *      OpenGL ES 2.0.
         * @requires_webgl20 Texture lookup with offset is not available in
         *      WebGL 1.0.
         */
        static Int maxTexelOffset();
        #endif

        /**
         * @brief Constructor
         *
         * Creates one OpenGL shader program.
         * @see @fn_gl{CreateProgram}
         */
        explicit AbstractShaderProgram();

        /**
         * @brief Construct without creating the underlying OpenGL object
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         *
         * This function can be safely used for constructing (and later
         * destructing) objects even without any OpenGL context being active.
         * @see @ref AbstractShaderProgram()
         */
        explicit AbstractShaderProgram(NoCreateT) noexcept;

        /** @brief Copying is not allowed */
        AbstractShaderProgram(const AbstractShaderProgram&) = delete;

        /** @brief Move constructor */
        AbstractShaderProgram(AbstractShaderProgram&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL shader program.
         * @see @fn_gl{DeleteProgram}
         */
        virtual ~AbstractShaderProgram() = 0;

        /** @brief Copying is not allowed */
        AbstractShaderProgram& operator=(const AbstractShaderProgram&) = delete;

        /** @brief Move assignment */
        AbstractShaderProgram& operator=(AbstractShaderProgram&& other) noexcept;

        /** @brief OpenGL program ID */
        GLuint id() const { return _id; }

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Shader program label
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls. If OpenGL 4.3 is not supported and neither
         * @extension{KHR,debug} (covered also by @extension{ANDROID,extension_pack_es31a})
         * nor @extension{EXT,debug_label} desktop or ES extension is
         * available, this function returns empty string.
         * @see @fn_gl{GetObjectLabel} with @def_gl{PROGRAM} or
         *      @fn_gl_extension{GetObjectLabel,EXT,debug_label} with
         *      @def_gl{PROGRAM_OBJECT_EXT}
         * @requires_gles Debug output is not available in WebGL.
         */
        std::string label() const;

        /**
         * @brief Set shader program label
         * @return Reference to self (for method chaining)
         *
         * Default is empty string. If OpenGL 4.3 is not supported and neither
         * @extension{KHR,debug} (covered also by @extension{ANDROID,extension_pack_es31a})
         * nor @extension{EXT,debug_label} desktop or ES extension is
         * available, this function does nothing.
         * @see @ref maxLabelLength(), @fn_gl{ObjectLabel} with
         *      @def_gl{PROGRAM} or @fn_gl_extension{LabelObject,EXT,debug_label}
         *      with @def_gl{PROGRAM_OBJECT_EXT}
         * @requires_gles Debug output is not available in WebGL.
         */
        AbstractShaderProgram& setLabel(const std::string& label) {
            return setLabelInternal({label.data(), label.size()});
        }

        /** @overload */
        template<std::size_t size> AbstractShaderProgram& setLabel(const char (&label)[size]) {
            return setLabelInternal({label, size - 1});
        }
        #endif

        /**
         * @brief Validate program
         *
         * Returns validation status and optional validation message.
         * @see @fn_gl{ValidateProgram}, @fn_gl{GetProgram} with
         *      @def_gl{VALIDATE_STATUS}, @def_gl{INFO_LOG_LENGTH},
         *      @fn_gl{GetProgramInfoLog}
         */
        std::pair<bool, std::string> validate();

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @brief Dispatch compute
         * @param workgroupCount    Workgroup count in given dimension
         *
         * Valid only on programs with compute shader attached.
         * @see @fn_gl{DispatchCompute}
         * @requires_gl43 Extension @extension{ARB,compute_shader}
         * @requires_gles31 Compute shaders are not available in OpenGL ES 3.0
         *      and older.
         * @requires_gles Compute shaders are not available in WebGL.
         */
        void dispatchCompute(const Vector3ui& workgroupCount);
        #endif

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Use shader for rendering
         * @deprecated Use @ref Mesh::draw(AbstractShaderProgram&) instead.
         */
        void use();
        #endif

    protected:
        /**
         * @brief Link the shader
         *
         * Returns `false` if linking of any shader failed, `true` if
         * everything succeeded. Linker message (if any) is printed to error
         * output. All attached shaders must be compiled with
         * @ref Shader::compile() before linking. The operation is batched in a
         * way that allows the driver to link multiple shaders simultaneously
         * (i.e. in multiple threads).
         * @see @fn_gl{LinkProgram}, @fn_gl{GetProgram} with
         *      @def_gl{LINK_STATUS} and @def_gl{INFO_LOG_LENGTH},
         *      @fn_gl{GetProgramInfoLog}
         */
        static bool link(std::initializer_list<std::reference_wrapper<AbstractShaderProgram>> shaders);

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @brief Allow retrieving program binary
         *
         * Initially disabled.
         * @see @fn_gl{ProgramParameter} with @def_gl{PROGRAM_BINARY_RETRIEVABLE_HINT}
         * @requires_gl41 Extension @extension{ARB,get_program_binary}
         * @requires_gles30 Always allowed in OpenGL ES 2.0.
         * @requires_gles Binary program representations are not supported in
         *      WebGL.
         */
        void setRetrievableBinary(bool enabled) {
            glProgramParameteri(_id, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, enabled ? GL_TRUE : GL_FALSE);
        }
        #endif

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Allow the program to be bound to individual pipeline stages
         *
         * Initially disabled.
         * @see @fn_gl{ProgramParameter} with @def_gl{PROGRAM_SEPARABLE}
         * @requires_gl41 Extension @extension{ARB,separate_shader_objects}
         * @requires_es_extension Extension @extension{EXT,separate_shader_objects}
         * @requires_gles Separate shader objects are not supported in WebGL.
         */
        void setSeparable(bool enabled) {
            #ifndef MAGNUM_TARGET_GLES
            glProgramParameteri(_id, GL_PROGRAM_SEPARABLE, enabled ? GL_TRUE : GL_FALSE);
            #else
            glProgramParameteriEXT(_id, GL_PROGRAM_SEPARABLE_EXT, enabled ? GL_TRUE : GL_FALSE);
            #endif
        }
        #endif

        /**
         * @brief Attach shader
         *
         * @see @fn_gl{AttachShader}
         */
        void attachShader(Shader& shader);

        /**
         * @brief Attach shaders
         *
         * Convenience overload to the above, allowing the user to specify more
         * than one shader at once. Other than that there is no other
         * (performance) difference when using this function.
         */
        void attachShaders(std::initializer_list<std::reference_wrapper<Shader>> shaders);

        /**
         * @brief Bind attribute to given location
         * @param location      Location
         * @param name          Attribute name
         *
         * Binds attribute to location which is used later for binding vertex
         * buffers.
         * @see @fn_gl{BindAttribLocation}
         * @deprecated_gl Preferred usage is to specify attribute location
         *      explicitly in the shader instead of using this function. See
         *      @ref AbstractShaderProgram-attribute-location "class documentation"
         *      for more information.
         */
        void bindAttributeLocation(UnsignedInt location, const std::string& name) {
            bindAttributeLocationInternal(location, {name.data(), name.size()});
        }

        /** @overload */
        template<std::size_t size> void bindAttributeLocation(UnsignedInt location, const char(&name)[size]) {
            bindAttributeLocationInternal(location, {name, size - 1});
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Bind fragment data to given location and color input index
         * @param location      Location
         * @param index         Blend equation color input index (`0` or `1`)
         * @param name          Fragment output variable name
         *
         * Binds fragment data to location which is used later for framebuffer
         * operations. See also @ref Renderer::BlendFunction for more
         * information about using color input index.
         * @see @fn_gl{BindFragDataLocationIndexed}
         * @deprecated_gl Preferred usage is to specify attribute location
         *      explicitly in the shader instead of using this function. See
         *      @ref AbstractShaderProgram-attribute-location "class documentation"
         *      for more information.
         * @requires_gl33 Extension @extension{ARB,blend_func_extended}
         * @requires_gl Multiple blend function inputs are not available in
         *      OpenGL ES or WebGL.
         */
        void bindFragmentDataLocationIndexed(UnsignedInt location, UnsignedInt index, const std::string& name) {
            bindFragmentDataLocationIndexedInternal(location, index, {name.data(), name.size()});
        }

        /** @overload */
        template<std::size_t size> void bindFragmentDataLocationIndexed(UnsignedInt location, UnsignedInt index, const char(&name)[size]) {
            bindFragmentDataLocationIndexedInternal(location, index, {name, size - 1});
        }

        /**
         * @brief Bind fragment data to given location and first color input index
         * @param location      Location
         * @param name          Fragment output variable name
         *
         * The same as @ref bindFragmentDataLocationIndexed(), but with `index`
         * set to `0`.
         * @see @fn_gl{BindFragDataLocation}
         * @deprecated_gl Preferred usage is to specify attribute location
         *      explicitly in the shader instead of using this function. See
         *      @ref AbstractShaderProgram-attribute-location "class documentation"
         *      for more information.
         * @requires_gl30 Extension @extension{EXT,gpu_shader4}
         * @requires_gl Use explicit location specification in OpenGL ES 3.0 or
         *      WebGL 2.0 and `gl_FragData[n]` provided by @extension{NV,draw_buffers}
         *      in OpenGL ES 2.0 and @webgl_extension{WEBGL,draw_buffers} in
         *      WebGL 1.0.
         */
        void bindFragmentDataLocation(UnsignedInt location, const std::string& name) {
            bindFragmentDataLocationInternal(location, {name.data(), name.size()});
        }

        /** @overload */
        template<std::size_t size> void bindFragmentDataLocation(UnsignedInt location, const char(&name)[size]) {
            /* Not using const char* parameter, because this way it avoids most accidents with non-zero-terminated strings */
            bindFragmentDataLocationInternal(location, {name, size - 1});
        }
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Specify shader outputs to be recorded in transform feedback
         * @param outputs       Names of output variables
         * @param bufferMode    Buffer mode
         *
         * Binds given output variables from vertex, geometry or tessellation
         * shader to transform feedback buffer binding points. If
         * @ref TransformFeedbackBufferMode::SeparateAttributes is used, each
         * output is bound to separate binding point. If
         * @ref TransformFeedbackBufferMode::InterleavedAttributes is used, the
         * outputs are interleaved into single buffer binding point. In this
         * case, special output name `gl_NextBuffer` causes the following
         * output to be recorded into next buffer binding point and
         * `gl_SkipComponents#` causes the transform feedback to offset the
         * following output variable by `#` components.
         * @see @fn_gl{TransformFeedbackVaryings}
         * @deprecated_gl Preferred usage is to specify transform feedback
         *      outputs explicitly in the shader instead of using this
         *      function. See @ref AbstractShaderProgram-transform-feedback "class documentation"
         *      for more information.
         * @requires_gl30 Extension @extension{EXT,transform_feedback}
         * @requires_gl40 Extension @extension{ARB,transform_feedback3} for
         *      using `gl_NextBuffer` or `gl_SkipComponents#` names in
         *      @p outputs array
         * @requires_gles30 Transform feedback is not available in OpenGL ES
         *      2.0.
         * @requires_webgl20 Transform feedback is not available in WebGL 1.0.
         * @requires_gl Special output names `gl_NextBuffer` and
         *      `gl_SkipComponents#` are not available in OpenGL ES or WebGL.
         */
        void setTransformFeedbackOutputs(std::initializer_list<std::string> outputs, TransformFeedbackBufferMode bufferMode);
        #endif

        /**
         * @brief Link the shader
         *
         * Links single shader. If possible, prefer to link multiple shaders
         * at once using @ref link(std::initializer_list<std::reference_wrapper<AbstractShaderProgram>>)
         * for improved performance, see its documentation for more
         * information.
         */
        bool link() { return link({*this}); }

        /**
         * @brief Get uniform location
         * @param name          Uniform name
         *
         * If given uniform is not found in the linked shader, a warning is
         * printed and `-1` is returned.
         * @see @ref setUniform(), @fn_gl{GetUniformLocation}
         * @deprecated_gl Preferred usage is to specify uniform location
         *      explicitly in the shader instead of using this function. See
         *      @ref AbstractShaderProgram-uniform-location "class documentation"
         *      for more information.
         */
        Int uniformLocation(const std::string& name) {
            return uniformLocationInternal({name.data(), name.size()});
        }

        /** @overload */
        template<std::size_t size> Int uniformLocation(const char(&name)[size]) {
            return uniformLocationInternal({name, size - 1});
        }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Get uniform block index
         * @param name          Uniform block name
         *
         * If given uniform block name is not found in the linked shader, a
         * warning is printed and `0xffffffffu` is returned.
         * @see @ref setUniformBlockBinding(), @fn_gl{GetUniformBlockIndex}
         * @requires_gl31 Extension @extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         * @deprecated_gl Preferred usage is to specify uniform block binding
         *      explicitly in the shader instead of using this function. See
         *      @ref AbstractShaderProgram-uniform-block-binding "class documentation"
         *      for more information.
         */
        UnsignedInt uniformBlockIndex(const std::string& name) {
            return uniformBlockIndexInternal({name.data(), name.size()});
        }

        /** @overload */
        template<std::size_t size> UnsignedInt uniformBlockIndex(const char(&name)[size]) {
            return uniformBlockIndexInternal({name, size - 1});
        }
        #endif

        /**
         * @brief Set uniform value
         * @param location      Uniform location
         * @param value         Value
         *
         * Convenience alternative for setting one value, see
         * @ref setUniform(Int, Containers::ArrayView<const Float>) for more
         * information.
         * @see @ref uniformLocation()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        template<class T> inline void setUniform(Int location, const T& value);
        #else
        void setUniform(Int location, Float value) {
            setUniform(location, {&value, 1});
        }
        void setUniform(Int location, Int value) {
            setUniform(location, {&value, 1});
        }
        #ifndef MAGNUM_TARGET_GLES2
        void setUniform(Int location, UnsignedInt value) {
            setUniform(location, {&value, 1});
        }
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void setUniform(Int location, Double value) {
            setUniform(location, {&value, 1});
        }
        #endif
        template<std::size_t size, class T> void setUniform(Int location, const Math::Vector<size, T>& value) {
            setUniform(location, {&value, 1});
        }
        template<std::size_t cols, std::size_t rows, class T> void setUniform(Int location, const Math::RectangularMatrix<cols, rows, T>& value) {
            setUniform(location, {&value, 1});
        }
        #endif

        /**
         * @brief Set uniform values
         * @param location      Uniform location
         * @param values        Values
         *
         * If neither @extension{ARB,separate_shader_objects} (part of OpenGL
         * 4.1) nor @extension{EXT,direct_state_access} desktop extension nor
         * @extension{EXT,separate_shader_objects} OpenGL ES extension nor
         * OpenGL ES 3.1 is available, the shader is marked for use before the
         * operation.
         * @see @ref setUniform(Int, const T&), @ref uniformLocation(),
         *      @fn_gl{UseProgram}, @fn_gl{Uniform} or @fn_gl{ProgramUniform}/
         *      @fn_gl_extension{ProgramUniform,EXT,direct_state_access}.
         */
        void setUniform(Int location, Containers::ArrayView<const Float> values);
        void setUniform(Int location, Containers::ArrayView<const Math::Vector<2, Float>> values); /**< @overload */
        void setUniform(Int location, Containers::ArrayView<const Math::Vector<3, Float>> values); /**< @overload */
        void setUniform(Int location, Containers::ArrayView<const Math::Vector<4, Float>> values); /**< @overload */

        /** @copydoc setUniform(Int, Containers::ArrayView<const Float>) */
        void setUniform(Int location, Containers::ArrayView<const Int> values);
        void setUniform(Int location, Containers::ArrayView<const Math::Vector<2, Int>> values); /**< @overload */
        void setUniform(Int location, Containers::ArrayView<const Math::Vector<3, Int>> values); /**< @overload */
        void setUniform(Int location, Containers::ArrayView<const Math::Vector<4, Int>> values); /**< @overload */

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @copydoc setUniform(Int, Containers::ArrayView<const Float>)
         * @requires_gl30 Extension @extension{EXT,gpu_shader4}
         * @requires_gles30 Only signed integers are available in OpenGL ES 2.0.
         * @requires_webgl20 Only signed integers are available in WebGL 1.0.
         */
        void setUniform(Int location, Containers::ArrayView<const UnsignedInt> values);
        void setUniform(Int location, Containers::ArrayView<const Math::Vector<2, UnsignedInt>> values); /**< @overload */
        void setUniform(Int location, Containers::ArrayView<const Math::Vector<3, UnsignedInt>> values); /**< @overload */
        void setUniform(Int location, Containers::ArrayView<const Math::Vector<4, UnsignedInt>> values); /**< @overload */
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @copydoc setUniform(Int, Containers::ArrayView<const Float>)
         * @requires_gl40 Extension @extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES or WebGL.
         */
        void setUniform(Int location, Containers::ArrayView<const Double> values);
        void setUniform(Int location, Containers::ArrayView<const Math::Vector<2, Double>> values); /**< @overload */
        void setUniform(Int location, Containers::ArrayView<const Math::Vector<3, Double>> values); /**< @overload */
        void setUniform(Int location, Containers::ArrayView<const Math::Vector<4, Double>> values); /**< @overload */
        #endif

        /** @copydoc setUniform(Int, Containers::ArrayView<const Float>) */
        void setUniform(Int location, Containers::ArrayView<const Math::RectangularMatrix<2, 2, Float>> values);
        void setUniform(Int location, Containers::ArrayView<const Math::RectangularMatrix<3, 3, Float>> values); /**< @overload */
        void setUniform(Int location, Containers::ArrayView<const Math::RectangularMatrix<4, 4, Float>> values); /**< @overload */

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @copydoc setUniform(Int, Containers::ArrayView<const Float>)
         * @requires_gles30 Only square matrices are available in OpenGL ES 2.0.
         * @requires_webgl20 Only square matrices are available in WebGL 1.0.
         */
        void setUniform(Int location, Containers::ArrayView<const Math::RectangularMatrix<2, 3, Float>> values);
        void setUniform(Int location, Containers::ArrayView<const Math::RectangularMatrix<3, 2, Float>> values); /**< @overload */
        void setUniform(Int location, Containers::ArrayView<const Math::RectangularMatrix<2, 4, Float>> values); /**< @overload */
        void setUniform(Int location, Containers::ArrayView<const Math::RectangularMatrix<4, 2, Float>> values); /**< @overload */
        void setUniform(Int location, Containers::ArrayView<const Math::RectangularMatrix<3, 4, Float>> values); /**< @overload */
        void setUniform(Int location, Containers::ArrayView<const Math::RectangularMatrix<4, 3, Float>> values); /**< @overload */
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @copydoc setUniform(Int, Containers::ArrayView<const Float>)
         * @requires_gl40 Extension @extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES or WebGL.
         */
        void setUniform(Int location, Containers::ArrayView<const Math::RectangularMatrix<2, 2, Double>> values);
        void setUniform(Int location, Containers::ArrayView<const Math::RectangularMatrix<3, 3, Double>> values); /**< @overload */
        void setUniform(Int location, Containers::ArrayView<const Math::RectangularMatrix<4, 4, Double>> values); /**< @overload */
        void setUniform(Int location, Containers::ArrayView<const Math::RectangularMatrix<2, 3, Double>> values); /**< @overload */
        void setUniform(Int location, Containers::ArrayView<const Math::RectangularMatrix<3, 2, Double>> values); /**< @overload */
        void setUniform(Int location, Containers::ArrayView<const Math::RectangularMatrix<2, 4, Double>> values); /**< @overload */
        void setUniform(Int location, Containers::ArrayView<const Math::RectangularMatrix<4, 2, Double>> values); /**< @overload */
        void setUniform(Int location, Containers::ArrayView<const Math::RectangularMatrix<3, 4, Double>> values); /**< @overload */
        void setUniform(Int location, Containers::ArrayView<const Math::RectangularMatrix<4, 3, Double>> values); /**< @overload */
        #endif

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Set uniform values
         * @deprecated Use @ref setUniform(Int, Containers::ArrayView<const Float>)
         *      and similar instead.
         */
        template<class T> CORRADE_DEPRECATED("use setUniform(Int, Containers::ArrayView<const T>) instead") void setUniform(Int location, UnsignedInt count, const T* values) {
            setUniform(location, {values, count});
        }
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Set uniform block binding
         * @param index     Uniform block index
         * @param binding   Uniform block binding
         *
         * @see @ref uniformBlockIndex(), @ref Buffer::maxUniformBindings(),
         *      @fn_gl{UniformBlockBinding}
         * @requires_gl31 Extension @extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         * @deprecated_gl Preferred usage is to specify uniform block binding
         *      explicitly in the shader instead of using this function. See
         *      @ref AbstractShaderProgram-uniform-block-binding "class documentation"
         *      for more information.
         */
        void setUniformBlockBinding(UnsignedInt index, UnsignedInt binding) {
            glUniformBlockBinding(_id, index, binding);
        }
        #endif

    private:
        #ifndef MAGNUM_TARGET_WEBGL
        AbstractShaderProgram& setLabelInternal(Containers::ArrayView<const char> label);
        #endif

        void bindAttributeLocationInternal(UnsignedInt location, Containers::ArrayView<const char> name);
        void bindFragmentDataLocationIndexedInternal(UnsignedInt location, UnsignedInt index, Containers::ArrayView<const char> name);
        void bindFragmentDataLocationInternal(UnsignedInt location, Containers::ArrayView<const char> name);
        Int uniformLocationInternal(Containers::ArrayView<const char> name);
        UnsignedInt uniformBlockIndexInternal(Containers::ArrayView<const char> name);

        #ifndef MAGNUM_TARGET_GLES2
        void MAGNUM_LOCAL transformFeedbackVaryingsImplementationDefault(Containers::ArrayView<const std::string> outputs, TransformFeedbackBufferMode bufferMode);
        #ifdef CORRADE_TARGET_WINDOWS
        void MAGNUM_LOCAL transformFeedbackVaryingsImplementationDanglingWorkaround(Containers::ArrayView<const std::string> outputs, TransformFeedbackBufferMode bufferMode);
        #endif
        #endif

        #ifndef MAGNUM_BUILD_DEPRECATED
        void use();
        #endif

        /*
            Currently, there are four supported ways to call glProgramUniform():

            - EXT_direct_state_access (desktop GL only, EXT suffix)
            - EXT_separate_shader_objects (OpenGL ES extension, EXT suffix)
            - ARB_separate_shader_objects (desktop GL only, no suffix)
            - OpenGL ES 3.1, no suffix

            To avoid copypasta and filesize bloat, this is merged to just two
            variants of implementation functions:

            - uniformImplementationSSO() - functions without suffix, used if
                ARB_separate_shader_objects desktop extension or OpenGL ES 3.1
                is available, completely disabled for ES2
            - uniformImplementationDSAEXT_SSOEXT() / uniformImplementationDSAEXT() --
                functions with EXT suffix, used if EXT_direct_state_access
                desktop exttension or EXT_separate_shader_objects ES 2.0 / ES 3.0
                extension is available, functions which don't have equivalents
                on ES (double arguments) don't have the _SSOEXT suffix
        */

        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const GLfloat* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<2, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<3, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<4, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const GLint* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<2, GLint>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<3, GLint>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<4, GLint>* values);
        #ifndef MAGNUM_TARGET_GLES2
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const GLuint* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<2, GLuint>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<3, GLuint>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<4, GLuint>* values);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const GLdouble* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<2, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<3, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<4, GLdouble>* values);
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const GLfloat* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::Vector<2, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::Vector<3, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::Vector<4, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const GLint* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::Vector<2, GLint>* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::Vector<3, GLint>* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::Vector<4, GLint>* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const GLuint* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::Vector<2, GLuint>* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::Vector<3, GLuint>* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::Vector<4, GLuint>* values);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const GLdouble* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::Vector<2, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::Vector<3, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::Vector<4, GLdouble>* values);
        #endif
        #ifndef MAGNUM_TARGET_WEBGL
        void MAGNUM_LOCAL uniformImplementationDSAEXT_SSOEXT(GLint location, GLsizei count, const GLfloat* values);
        void MAGNUM_LOCAL uniformImplementationDSAEXT_SSOEXT(GLint location, GLsizei count, const Math::Vector<2, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDSAEXT_SSOEXT(GLint location, GLsizei count, const Math::Vector<3, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDSAEXT_SSOEXT(GLint location, GLsizei count, const Math::Vector<4, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDSAEXT_SSOEXT(GLint location, GLsizei count, const GLint* values);
        void MAGNUM_LOCAL uniformImplementationDSAEXT_SSOEXT(GLint location, GLsizei count, const Math::Vector<2, GLint>* values);
        void MAGNUM_LOCAL uniformImplementationDSAEXT_SSOEXT(GLint location, GLsizei count, const Math::Vector<3, GLint>* values);
        void MAGNUM_LOCAL uniformImplementationDSAEXT_SSOEXT(GLint location, GLsizei count, const Math::Vector<4, GLint>* values);
        #ifndef MAGNUM_TARGET_GLES2
        void MAGNUM_LOCAL uniformImplementationDSAEXT_SSOEXT(GLint location, GLsizei count, const GLuint* values);
        void MAGNUM_LOCAL uniformImplementationDSAEXT_SSOEXT(GLint location, GLsizei count, const Math::Vector<2, GLuint>* values);
        void MAGNUM_LOCAL uniformImplementationDSAEXT_SSOEXT(GLint location, GLsizei count, const Math::Vector<3, GLuint>* values);
        void MAGNUM_LOCAL uniformImplementationDSAEXT_SSOEXT(GLint location, GLsizei count, const Math::Vector<4, GLuint>* values);
        #endif
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL uniformImplementationDSAEXT(GLint location, GLsizei count, const GLdouble* values);
        void MAGNUM_LOCAL uniformImplementationDSAEXT(GLint location, GLsizei count, const Math::Vector<2, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDSAEXT(GLint location, GLsizei count, const Math::Vector<3, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDSAEXT(GLint location, GLsizei count, const Math::Vector<4, GLdouble>* values);
        #endif

        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<2, 2, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<3, 3, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<4, 4, GLfloat>* values);
        #ifndef MAGNUM_TARGET_GLES2
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<2, 3, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<3, 2, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<2, 4, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<4, 2, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<3, 4, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<4, 3, GLfloat>* values);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<2, 2, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<3, 3, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<4, 4, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<2, 3, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<3, 2, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<2, 4, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<4, 2, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<3, 4, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<4, 3, GLdouble>* values);
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<2, 2, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<3, 3, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<4, 4, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<2, 3, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<3, 2, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<2, 4, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<4, 2, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<3, 4, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<4, 3, GLfloat>* values);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<2, 2, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<3, 3, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<4, 4, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<2, 3, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<3, 2, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<2, 4, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<4, 2, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<3, 4, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<4, 3, GLdouble>* values);
        #endif
        #ifndef MAGNUM_TARGET_WEBGL
        void MAGNUM_LOCAL uniformImplementationDSAEXT_SSOEXT(GLint location, GLsizei count, const Math::RectangularMatrix<2, 2, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDSAEXT_SSOEXT(GLint location, GLsizei count, const Math::RectangularMatrix<3, 3, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDSAEXT_SSOEXT(GLint location, GLsizei count, const Math::RectangularMatrix<4, 4, GLfloat>* values);
        #ifndef MAGNUM_TARGET_GLES2
        void MAGNUM_LOCAL uniformImplementationDSAEXT_SSOEXT(GLint location, GLsizei count, const Math::RectangularMatrix<2, 3, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDSAEXT_SSOEXT(GLint location, GLsizei count, const Math::RectangularMatrix<3, 2, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDSAEXT_SSOEXT(GLint location, GLsizei count, const Math::RectangularMatrix<2, 4, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDSAEXT_SSOEXT(GLint location, GLsizei count, const Math::RectangularMatrix<4, 2, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDSAEXT_SSOEXT(GLint location, GLsizei count, const Math::RectangularMatrix<3, 4, GLfloat>* values);
        void MAGNUM_LOCAL uniformImplementationDSAEXT_SSOEXT(GLint location, GLsizei count, const Math::RectangularMatrix<4, 3, GLfloat>* values);
        #endif
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL uniformImplementationDSAEXT(GLint location, GLsizei count, const Math::RectangularMatrix<2, 2, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDSAEXT(GLint location, GLsizei count, const Math::RectangularMatrix<3, 3, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDSAEXT(GLint location, GLsizei count, const Math::RectangularMatrix<4, 4, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDSAEXT(GLint location, GLsizei count, const Math::RectangularMatrix<2, 3, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDSAEXT(GLint location, GLsizei count, const Math::RectangularMatrix<3, 2, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDSAEXT(GLint location, GLsizei count, const Math::RectangularMatrix<2, 4, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDSAEXT(GLint location, GLsizei count, const Math::RectangularMatrix<4, 2, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDSAEXT(GLint location, GLsizei count, const Math::RectangularMatrix<3, 4, GLdouble>* values);
        void MAGNUM_LOCAL uniformImplementationDSAEXT(GLint location, GLsizei count, const Math::RectangularMatrix<4, 3, GLdouble>* values);
        #endif

        GLuint _id;

        #if defined(CORRADE_TARGET_WINDOWS) && !defined(MAGNUM_TARGET_GLES2)
        /* Needed for the nv-windows-dangling-transform-feedback-varying-names
           workaround */
        std::vector<std::string> _transformFeedbackVaryingNames;
        #endif
};

}

#endif
