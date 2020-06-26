#ifndef Magnum_GL_AbstractShaderProgram_h
#define Magnum_GL_AbstractShaderProgram_h
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
 * @brief Class @ref Magnum::GL::AbstractShaderProgram
 */

#include <string>
#include <Corrade/Containers/ArrayView.h>

#include "Magnum/Tags.h"
#include "Magnum/GL/AbstractObject.h"
#include "Magnum/GL/Attribute.h"
#include "Magnum/GL/GL.h"

#if defined(CORRADE_TARGET_WINDOWS) && !defined(MAGNUM_TARGET_GLES2)
#include <vector>
#endif

namespace Magnum { namespace GL {

namespace Implementation { struct ShaderProgramState; }

/**
@brief Base for shader program implementations

@section GL-AbstractShaderProgram-subclassing Subclassing workflow

This class is designed to be used via subclassing. Subclasses define these
functions and properties:

<ul>
<li> **Attribute definitions** using @ref Attribute typedefs with location and
    type for configuring meshes, for example:

    @snippet MagnumGL.cpp AbstractShaderProgram-input-attributes
</li>
<li> **Output attribute locations**, if desired, for example:

    @snippet MagnumGL.cpp AbstractShaderProgram-output-attributes
</li>
<li> **Constructor**, which loads, compiles and attaches particular shaders and
    links the program together, for example:

    @snippet MagnumGL.cpp AbstractShaderProgram-constructor
</li>
<li> **Uniform setting functions**, which will provide public interface for
    protected @ref setUniform() functions. For usability purposes you can
    implement also method chaining. Example:

    @snippet MagnumGL.cpp AbstractShaderProgram-uniforms
</li>
<li> **Texture and texture image binding functions** in which you bind the
    textures/images to particular texture/image units using
    @ref Texture::bind() "*Texture::bind()" /
    @ref Texture::bindImage() "*Texture::bindImage()" and similar, for example:

    @snippet MagnumGL.cpp AbstractShaderProgram-textures
</li>
<li> **Transform feedback setup function**, if needed, in which you bind
    buffers to particular indices using @ref TransformFeedback::attachBuffer()
    and similar, possibly with overloads based on desired use cases, e.g.:

    @snippet MagnumGL.cpp AbstractShaderProgram-xfb
</li>
<li>And optionally, **hiding irrelevant draw/dispatch functions** to prevent
    users from accidentally calling @ref draw() on compute shaders,
    @ref drawTransformFeedback() on shaders that don't have transform feedback
    or @ref dispatchCompute() on shaders that aren't compute. For example:

    @snippet MagnumGL.cpp AbstractShaderProgram-hide-irrelevant
</ul>

@subsection GL-AbstractShaderProgram-attribute-location Binding attribute and fragment data location

The preferred workflow is to specify attribute location for vertex shader input
attributes and fragment shader output attributes explicitly in the shader code,
e.g.:

@code{.glsl}
// GLSL 3.30, GLSL ES 3.00 or
#extension GL_ARB_explicit_attrib_location: require
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textureCoordinates;
@endcode

Similarly for ouput attributes, you can also specify blend equation color index
for them (see @ref Renderer::BlendFunction for more information about using
color input index):

@code{.glsl}
layout(location = 0, index = 0) out vec4 color;
layout(location = 1, index = 1) out vec3 normal;
@endcode

If you don't have the required version/extension, declare the attributes
without @glsl layout() @ce qualifier and use functions @ref bindAttributeLocation()
and @ref bindFragmentDataLocation() / @ref bindFragmentDataLocationIndexed()
between attaching the shaders and linking the program. Note that additional
syntax changes may be needed for GLSL 1.20 and GLSL ES.

@code{.glsl}
in vec4 position;
in vec3 normal;
in vec2 textureCoordinates;
@endcode

@code{.glsl}
out vec4 color;
out vec3 normal;
@endcode

@snippet MagnumGL.cpp AbstractShaderProgram-binding

@see @ref maxVertexAttributes(), @ref AbstractFramebuffer::maxDrawBuffers()
@requires_gl30 Extension @gl_extension{EXT,gpu_shader4} for using
    @ref bindFragmentDataLocation().
@requires_gl33 Extension @gl_extension{ARB,blend_func_extended} for using
    @ref bindFragmentDataLocationIndexed().
@requires_gl33 Extension @gl_extension{ARB,explicit_attrib_location} for
    explicit attribute location instead of using
    @ref bindAttributeLocation(), @ref bindFragmentDataLocation() or
    @ref bindFragmentDataLocationIndexed().
@requires_gles30 Explicit location specification of input attributes is not
    supported in OpenGL ES 2.0, use @ref bindAttributeLocation()
    instead.
@requires_gles30 Multiple fragment shader outputs are not available in OpenGL
    ES 2.0, similar functionality is available in extension
    @gl_extension{EXT,draw_buffers} or @gl_extension{NV,draw_buffers}.
@requires_webgl20 Explicit location specification of input attributes is not
    supported in WebGL 1.0, use @ref bindAttributeLocation() instead.
@requires_webgl20 Multiple fragment shader outputs are not available in WebGL
    1.0, similar functionality is available in extension
    @webgl_extension{WEBGL,draw_buffers}.

@todo @gl_extension2{EXT,separate_shader_objects,EXT_separate_shader_objects.gles}
    supports explicit attrib location

@subsection GL-AbstractShaderProgram-uniform-location Uniform locations

The preferred workflow is to specify uniform locations directly in the shader
code, e.g.:

@code{.glsl}
// GLSL 4.30, GLSL ES 3.10 or
#extension GL_ARB_explicit_uniform_location: require
layout(location = 0) uniform mat4 projectionMatrix;
layout(location = 1) uniform mat4 transformationMatrix;
layout(location = 2) uniform mat3 normalMatrix;
@endcode

If you don't have the required version/extension, declare the uniforms without
the @glsl layout() @ce qualifier, get uniform location using
@ref uniformLocation() *after* linking stage and then use the queried location
in uniform setting functions. Note that additional syntax changes may be needed
for GLSL 1.20 and GLSL ES.

@code{.glsl}
uniform mat4 projectionMatrix;
uniform mat4 transformationMatrix;
uniform mat3 normalMatrix;
@endcode

@snippet MagnumGL.cpp AbstractShaderProgram-uniform-location

@see @ref maxUniformLocations()
@requires_gl43 Extension @gl_extension{ARB,explicit_uniform_location} for
    explicit uniform location instead of using @ref uniformLocation().
@requires_gles31 Explicit uniform location is not supported in OpenGL ES 3.0
    and older. Use @ref uniformLocation() instead.
@requires_gles Explicit uniform location is not supported in WebGL. Use
    @ref uniformLocation() instead.

@subsection GL-AbstractShaderProgram-uniform-block-binding Uniform block bindings

The preferred workflow is to specify uniform block binding directly in the
shader code, e.g.:

@code{.glsl}
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
without the @glsl layout() @ce qualifier, get uniform block index using
@ref uniformBlockIndex() and then map it to the uniform buffer binding using
@ref setUniformBlockBinding(). Note that additional syntax changes may be
needed for GLSL ES.

@code{.glsl}
layout(std140) uniform matrices {
    mat4 projectionMatrix;
    mat4 transformationMatrix;
};
layout(std140) uniform material {
    vec4 diffuse;
    vec4 specular;
};
@endcode

@snippet MagnumGL.cpp AbstractShaderProgram-uniform-block-binding

@see @ref Buffer::maxUniformBindings()
@requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
@requires_gl42 Extension @gl_extension{ARB,shading_language_420pack} for explicit
    uniform block binding instead of using @ref uniformBlockIndex() and
    @ref setUniformBlockBinding().
@requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
@requires_gles31 Explicit uniform block binding is not supported in OpenGL ES
    3.0 and older. Use @ref uniformBlockIndex() and @ref setUniformBlockBinding()
    instead.
@requires_webgl20 Uniform buffers are not available in WebGL 1.0.
@requires_gles Explicit uniform block binding is not supported in WebGL. Use
    @ref uniformBlockIndex() and @ref setUniformBlockBinding() instead.

@subsection GL-AbstractShaderProgram-shader-storage-block-binding Shader storage block bindings

The workflow is to specify shader storage block binding directly in the shader
code, e.g.:

@code{.glsl}
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
@requires_gl43 Extension @gl_extension{ARB,shader_storage_buffer_object}
@requires_gles31 Shader storage is not available in OpenGL ES 3.0 and older.
@requires_gles Shader storage is not available in WebGL.

@subsection GL-AbstractShaderProgram-texture-units Specifying texture and image binding units

The preferred workflow is to specify texture/image binding unit directly in the
shader code, e.g.:

@code{.glsl}
// GLSL 4.20, GLSL ES 3.10 or
#extension GL_ARB_shading_language_420pack: require
layout(binding = 0) uniform sampler2D diffuseTexture;
layout(binding = 1) uniform sampler2D specularTexture;
@endcode

If you don't have the required version/extension, declare the uniforms without
the `binding` qualifier and set the texture binding unit using
@ref setUniform(Int, const T&) "setUniform(Int, Int)". Note that additional
syntax changes may be needed for GLSL ES.

@code{.glsl}
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
@endcode

@snippet MagnumGL.cpp AbstractShaderProgram-texture-uniforms

@see @ref Shader::maxTextureImageUnits(), @ref maxImageUnits()
@requires_gl42 Extension @gl_extension{ARB,shading_language_420pack} for explicit
    texture binding unit instead of using
    @ref setUniform(Int, const T&) "setUniform(Int, Int)".
@requires_gles31 Explicit texture binding unit is not supported in OpenGL ES
    3.0 and older. Use @ref setUniform(Int, const T&) "setUniform(Int, Int)"
    instead.
@requires_gles Explicit texture binding unit is not supported in WebGL. Use
    @ref setUniform(Int, const T&) "setUniform(Int, Int)" instead.

@subsection GL-AbstractShaderProgram-transform-feedback Specifying transform feedback binding points

The preferred workflow is to specify output binding points directly in the
shader code, e.g.:

@code{.glsl}
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

@code{.glsl}
out block {
    vec3 position;
    vec3 normal;
};
out vec3 velocity;
@endcode

@snippet MagnumGL.cpp AbstractShaderProgram-xfb-outputs

@see @ref TransformFeedback::maxInterleavedComponents(),
    @ref TransformFeedback::maxSeparateAttributes(),
    @ref TransformFeedback::maxSeparateComponents()
@requires_gl40 Extension @gl_extension{ARB,transform_feedback2}
@requires_gl40 Extension @gl_extension{ARB,transform_feedback3} for using
    `gl_NextBuffer` or `gl_SkipComponents#` names in
    @ref setTransformFeedbackOutputs() function.
@requires_gl44 Extension @gl_extension{ARB,enhanced_layouts} for explicit
    transform feedback output specification instead of using
    @ref setTransformFeedbackOutputs().
@requires_gles30 Transform feedback is not available in OpenGL ES 2.0.
@requires_gl Explicit transform feedback output specification is not available
    in OpenGL ES or WebGL.
@requires_webgl20 Transform feedback is not available in WebGL 1.0.

@section GL-AbstractShaderProgram-rendering-workflow Rendering workflow

Basic workflow with AbstractShaderProgram subclasses is: instance shader
class, configure attribute binding in meshes (see @ref GL-Mesh-configuration "Mesh documentation"
for more information) and map shader outputs to framebuffer attachments if
needed (see @ref GL-Framebuffer-usage "Framebuffer documentation" for more
information). In each draw event set all required shader parameters, bind
specific framebuffer (if needed) and then call @ref draw(). Example:

@snippet MagnumGL.cpp AbstractShaderProgram-rendering

@section GL-AbstractShaderProgram-compute-workflow Compute workflow

Add just the @ref Shader::Type::Compute shader and implement uniform/texture
setting functions as needed. After setting up required parameters call
@ref dispatchCompute().

@section GL-AbstractShaderProgram-types Mapping between GLSL and Magnum types

See @ref types for more information, only types with GLSL equivalent can be
used (and their super- or subclasses with the same size and underlying type).
See also @ref Attribute::DataType enum for additional type options.

@requires_gl30 Extension @gl_extension{EXT,gpu_shader4} is required when using
    integer attributes (i.e. @ref Magnum::UnsignedInt "UnsignedInt",
    @ref Magnum::Int "Int", @ref Magnum::Vector2ui "Vector2ui",
    @ref Magnum::Vector2i "Vector2i", @ref Magnum::Vector3ui "Vector3ui",
    @ref Magnum::Vector3i "Vector3i", @ref Magnum::Vector4ui "Vector4ui" and
    @ref Magnum::Vector4i "Vector4i") or unsigned integer uniforms (i.e.
    @ref Magnum::UnsignedInt "UnsignedInt", @ref Magnum::Vector2ui "Vector2ui",
    @ref Magnum::Vector3ui "Vector3ui" and @ref Magnum::Vector4ui "Vector4ui").
@requires_gl40 Extension @gl_extension{ARB,gpu_shader_fp64} is required when
    using double uniforms (i.e. @ref Magnum::Double "Double",
    @ref Magnum::Vector2d "Vector2d", @ref Magnum::Vector3d "Vector3d",
    @ref Magnum::Vector4d "Vector4d", @ref Magnum::Matrix2x2d "Matrix2x2d",
    @ref Magnum::Matrix3x3d "Matrix3x3d", @ref Magnum::Matrix4x4d "Matrix4x4d",
    @ref Magnum::Matrix2x3d "Matrix2x3d", @ref Magnum::Matrix3x2d "Matrix3x2d",
    @ref  Magnum::Matrix2x4d "Matrix2x4d", @ref Magnum::Matrix4x2d "Matrix4x2d",
    @ref Magnum::Matrix3x4d "Matrix3x4d" and @ref Magnum::Matrix4x3d "Matrix4x3d").
@requires_gl41 Extension @gl_extension{ARB,vertex_attrib_64bit} is required when
    using double attributes (i.e. @ref Magnum::Double "Double",
    @ref Magnum::Vector2d "Vector2d", @ref Magnum::Vector3d "Vector3d",
    @ref Magnum::Vector4d "Vector4d", @ref Magnum::Matrix2x2d "Matrix2x2d",
    @ref Magnum::Matrix3x3d "Matrix3x3d", @ref Magnum::Matrix4x4d "Matrix4x4d",
    @ref Magnum::Matrix2x3d "Matrix2x3d", @ref Magnum::Matrix3x2d "Matrix3x2d",
    @ref Magnum::Matrix2x4d "Matrix2x4d", @ref Magnum::Matrix4x2d "Matrix4x2d",
    @ref Magnum::Matrix3x4d "Matrix3x4d" and @ref Magnum::Matrix4x3d "Matrix4x3d").
@requires_gles30 Integer attributes, unsigned integer uniforms and non-square
    matrix attributes and uniforms (i.e. @ref Magnum::Matrix2x3 "Matrix2x3",
    @ref Magnum::Matrix3x2 "Matrix3x2", @ref Magnum::Matrix2x4 "Matrix2x4",
    @ref Magnum::Matrix4x2 "Matrix4x2", @ref Magnum::Matrix3x4 "Matrix3x4" and
    @ref Magnum::Matrix4x3 "Matrix4x3") are not available in OpenGL ES 2.0.
@requires_gl Double attributes and uniforms are not available in OpenGL ES or
    WebGL.
@requires_webgl20 Integer attributes, unsigned integer uniforms and non-square
    matrix attributes and uniforms (i.e. @ref Magnum::Matrix2x3 "Matrix2x3",
    @ref Magnum::Matrix3x2 "Matrix3x2", @ref Magnum::Matrix2x4 "Matrix2x4",
    @ref Magnum::Matrix4x2 "Matrix4x2", @ref Magnum::Matrix3x4 "Matrix3x4" and
    @ref Magnum::Matrix4x3 "Matrix4x3") are not available in WebGL 1.0.

@section GL-AbstractShaderProgram-performance-optimization Performance optimizations

The engine tracks currently used shader program to avoid unnecessary calls to
@fn_gl_keyword{UseProgram}. Shader limits (such as @ref maxVertexAttributes())
are cached, so repeated queries don't result in repeated @fn_gl{Get} calls. See
also @ref Context::resetState() and @ref Context::State::Shaders.

If extension @gl_extension{ARB,separate_shader_objects} (part of OpenGL 4.1),
@gl_extension2{EXT,separate_shader_objects,EXT_separate_shader_objects.gles}
OpenGL ES extension or OpenGL ES 3.1 is available, uniform setting functions
use DSA functions to avoid unnecessary calls to @fn_gl{UseProgram}. See
@ref setUniform() documentation for more information.

To achieve least state changes, set all uniforms in one run --- method chaining
comes in handy.

@see @ref portability-shaders

@todo `GL_NUM_{PROGRAM,SHADER}_BINARY_FORMATS` + `GL_{PROGRAM,SHADER}_BINARY_FORMATS` (vector), (@gl_extension{ARB,ES2_compatibility})
 */
class MAGNUM_GL_EXPORT AbstractShaderProgram: public AbstractObject {
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
         * @m_enum_values_as_keywords
         * @requires_gl30 Extension @gl_extension{EXT,transform_feedback}
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
         * @see @ref Attribute, @fn_gl{Get} with @def_gl_keyword{MAX_VERTEX_ATTRIBS}
         */
        static Int maxVertexAttributes();

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @brief Max supported count of vertices emitted by a geometry shader
         * @m_since{2020,06}
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither @gl_extension{ARB,geometry_shader4} (part of
         * OpenGL 3.2) nor @gl_extension{ANDROID,extension_pack_es31a} /
         * @gl_extension{EXT,geometry_shader} (part of OpenGL ES 3.2) is not
         * available, returns @cpp 0 @ce.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_GEOMETRY_OUTPUT_VERTICES}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Geometry shaders are not available in WebGL.
         */
        static Int maxGeometryOutputVertices();

        /**
         * @brief Max supported atomic counter buffer size
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @gl_extension{ARB,shader_atomic_counters}
         * (part of OpenGL 4.2) nor OpenGL ES 3.1 is available, returns @cpp 0 @ce.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_ATOMIC_COUNTER_BUFFER_SIZE}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Atomic counters are not available in WebGL.
         */
        static Int maxAtomicCounterBufferSize();

        /**
         * @brief Max supported compute shared memory size
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @gl_extension{ARB,compute_shader}
         * (part of OpenGL 4.3) nor OpenGL ES 3.1 is available, returns @cpp 0 @ce.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_COMPUTE_SHARED_MEMORY_SIZE}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Compute shaders are not available in WebGL.
         */
        static Int maxComputeSharedMemorySize();

        /**
         * @brief Max supported compute work group invocation count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @gl_extension{ARB,compute_shader}
         * (part of OpenGL 4.3) nor OpenGL ES 3.1 is available, returns @cpp 0 @ce.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_COMPUTE_WORK_GROUP_INVOCATIONS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Compute shaders are not available in WebGL.
         */
        static Int maxComputeWorkGroupInvocations();

        /**
         * @brief Max supported compute work group count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @gl_extension{ARB,compute_shader}
         * (part of OpenGL 4.3) nor OpenGL ES 3.1 is available, returns zero
         * vector.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_COMPUTE_WORK_GROUP_COUNT}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Compute shaders are not available in WebGL.
         */
        static Vector3i maxComputeWorkGroupCount();

        /**
         * @brief Max supported compute work group size
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @gl_extension{ARB,compute_shader}
         * (part of OpenGL 4.3) nor OpenGL ES 3.1 is available, returns zero
         * vector.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_COMPUTE_WORK_GROUP_SIZE}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Compute shaders are not available in WebGL.
         */
        static Vector3i maxComputeWorkGroupSize();

        /**
         * @brief Max supported image unit count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @gl_extension{ARB,shader_image_load_store}
         * (part of OpenGL 4.2) or OpenGL ES 3.1 is not available, returns
         * @cpp 0 @ce.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_IMAGE_UNITS}
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
         * OpenGL calls. If extension @gl_extension{ARB,shader_image_load_store}
         * (part of OpenGL 4.2) is not available, returns @cpp 0 @ce.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_IMAGE_SAMPLES}
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
         * OpenGL calls. If neither extension @gl_extension{ARB,shader_image_load_store}
         * (part of OpenGL 4.2) nor extension @gl_extension{ARB,shader_storage_buffer_object}
         * (part of OpenGL 4.3) nor OpenGL ES 3.1 is available, returns
         * @cpp 0 @ce.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_COMBINED_SHADER_OUTPUT_RESOURCES}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Shader image load/store is not available in WebGL.
         */
        static Int maxCombinedShaderOutputResources();

        /**
         * @brief Max supported shader storage block size
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @gl_extension{ARB,shader_storage_buffer_object}
         * (part of OpenGL 4.3) nor OpenGL ES 3.1 is available, returns
         * @cpp 0 @ce.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_SHADER_STORAGE_BLOCK_SIZE}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Shader storage is not available in WebGL.
         */
        static Long maxShaderStorageBlockSize();
        #endif

        /**
         * @brief Max supported uniform block size
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @gl_extension{ARB,uniform_buffer_object}
         * (part of OpenGL 3.1) is not available, returns @cpp 0 @ce.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_UNIFORM_BLOCK_SIZE}
         * @requires_gles30 Uniform blocks are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform blocks are not available in WebGL 1.0.
         */
        static Int maxUniformBlockSize();

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Max supported explicit uniform location count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @gl_extension{ARB,explicit_uniform_location}
         * (part of OpenGL 4.3) nor OpenGL ES 3.1 is available, returns
         * @cpp 0 @ce.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_UNIFORM_LOCATIONS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Explicit uniform location is not available in WebGL.
         */
        static Int maxUniformLocations();
        #endif

        /**
         * @brief Min supported program texel offset
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @gl_extension{EXT,gpu_shader4} (part of
         * OpenGL 3.0) is not available, returns @cpp 0 @ce.
         * @see @fn_gl{Get} with @def_gl_keyword{MIN_PROGRAM_TEXEL_OFFSET}
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
         * OpenGL calls. If extension @gl_extension{EXT,gpu_shader4} (part of
         * OpenGL 3.0) is not available, returns @cpp 0 @ce.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_PROGRAM_TEXEL_OFFSET}
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
         * @see @fn_gl_keyword{CreateProgram}
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
         * However note that this is a low-level and a potentially dangerous
         * API, see the documentation of @ref NoCreate for alternatives.
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
         * @see @fn_gl_keyword{DeleteProgram}
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
         * OpenGL calls. If OpenGL 4.3 / OpenGL ES 3.2 is not supported and
         * neither @gl_extension{KHR,debug} (covered also by
         * @gl_extension{ANDROID,extension_pack_es31a}) nor @gl_extension{EXT,debug_label}
         * desktop or ES extension is available, this function returns empty
         * string.
         * @see @fn_gl_keyword{GetObjectLabel} with @def_gl{PROGRAM} or
         *      @fn_gl_extension_keyword{GetObjectLabel,EXT,debug_label} with
         *      @def_gl{PROGRAM_OBJECT_EXT}
         * @requires_gles Debug output is not available in WebGL.
         */
        std::string label() const;

        /**
         * @brief Set shader program label
         * @return Reference to self (for method chaining)
         *
         * Default is empty string. If OpenGL 4.3 / OpenGL ES 3.2 is not
         * supported and neither @gl_extension{KHR,debug} (covered also by
         * @gl_extension{ANDROID,extension_pack_es31a}) nor @gl_extension{EXT,debug_label}
         * desktop or ES extension is available, this function does nothing.
         * @see @ref maxLabelLength(), @fn_gl_keyword{ObjectLabel} with
         *      @def_gl{PROGRAM} or @fn_gl_extension_keyword{LabelObject,EXT,debug_label}
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
         * @see @fn_gl_keyword{ValidateProgram}, @fn_gl_keyword{GetProgram} with
         *      @def_gl{VALIDATE_STATUS}, @def_gl{INFO_LOG_LENGTH},
         *      @fn_gl_keyword{GetProgramInfoLog}
         */
        std::pair<bool, std::string> validate();

        /**
         * @brief Draw a mesh
         * @param mesh      Mesh to draw
         * @m_since{2020,06}
         *
         * Expects that @p mesh is compatible with this shader and is fully set
         * up. If its vertex/index count or instance count is @cpp 0 @ce, no
         * draw commands are issued. See also
         * @ref GL-AbstractShaderProgram-rendering-workflow "class documentation"
         * for more information. If @gl_extension{ARB,vertex_array_object} (part
         * of OpenGL 3.0), OpenGL ES 3.0, WebGL 2.0, @gl_extension{OES,vertex_array_object}
         * in OpenGL ES 2.0 or @webgl_extension{OES,vertex_array_object} in
         * WebGL 1.0 is available, the associated vertex array object is bound
         * instead of setting up the mesh from scratch.
         * @see @ref Mesh::setCount(), @ref Mesh::setInstanceCount(),
         *      @ref draw(MeshView&),
         *      @ref draw(Containers::ArrayView<const Containers::Reference<MeshView>>),
         *      @ref drawTransformFeedback(),
         *      @fn_gl_keyword{UseProgram}, @fn_gl_keyword{EnableVertexAttribArray},
         *      @fn_gl{BindBuffer}, @fn_gl_keyword{VertexAttribPointer},
         *      @fn_gl_keyword{DisableVertexAttribArray} or @fn_gl_keyword{BindVertexArray},
         *      @fn_gl_keyword{DrawArrays}/@fn_gl_keyword{DrawArraysInstanced}/
         *      @fn_gl_keyword{DrawArraysInstancedBaseInstance} or @fn_gl_keyword{DrawElements}/
         *      @fn_gl_keyword{DrawRangeElements}/@fn_gl_keyword{DrawElementsBaseVertex}/
         *      @fn_gl_keyword{DrawRangeElementsBaseVertex}/@fn_gl_keyword{DrawElementsInstanced}/
         *      @fn_gl_keyword{DrawElementsInstancedBaseInstance}/
         *      @fn_gl_keyword{DrawElementsInstancedBaseVertex}/
         *      @fn_gl_keyword{DrawElementsInstancedBaseVertexBaseInstance}
         * @requires_gl32 Extension @gl_extension{ARB,draw_elements_base_vertex}
         *      if the mesh is indexed and @ref Mesh::baseVertex() is not `0`.
         * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays} if
         *      @ref Mesh::instanceCount() is more than `1`.
         * @requires_gl42 Extension @gl_extension{ARB,base_instance} if
         *      @ref Mesh::baseInstance() is not `0`.
         * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
         *      @gl_extension{EXT,instanced_arrays},
         *      @gl_extension{EXT,draw_instanced},
         *      @gl_extension{NV,instanced_arrays},
         *      @gl_extension{NV,draw_instanced} in OpenGL ES 2.0 if
         *      @ref Mesh::instanceCount() is more than `1`.
         * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
         *      in WebGL 1.0 if @ref Mesh::instanceCount() is more than `1`.
         * @requires_gl Specifying base vertex for indexed meshes is not
         *      available in OpenGL ES or WebGL.
         */
        void draw(Mesh& mesh);

        /**
         * @overload
         * @m_since{2020,06}
         */
        void draw(Mesh&& mesh) { draw(mesh); }

        /**
         * @brief Draw a mesh view
         * @m_since{2020,06}
         *
         * See @ref draw(Mesh&) for more information.
         * @see @ref draw(Containers::ArrayView<const Containers::Reference<MeshView>>),
         *      @ref drawTransformFeedback()
         * @requires_gl32 Extension @gl_extension{ARB,draw_elements_base_vertex}
         *      if the mesh is indexed and @ref MeshView::baseVertex() is not
         *      `0`.
         * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays} if
         *      @ref MeshView::instanceCount() is more than `1`.
         * @requires_gl42 Extension @gl_extension{ARB,base_instance} if
         *      @ref MeshView::baseInstance() is not `0`.
         * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
         *      @gl_extension{EXT,instanced_arrays},
         *      @gl_extension{EXT,draw_instanced},
         *      @gl_extension{NV,instanced_arrays},
         *      @gl_extension{NV,draw_instanced} in OpenGL ES 2.0 if
         *      @ref MeshView::instanceCount() is more than `1`.
         * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
         *      in WebGL 1.0 if @ref MeshView::instanceCount() is more than
         *      `1`
         * @requires_gl Specifying base vertex for indexed meshes is not
         *      available in OpenGL ES or WebGL.
         */
        void draw(MeshView& mesh);

        /**
         * @overload
         * @m_since{2020,06}
         */
        void draw(MeshView&& mesh) { draw(mesh); }

        /**
         * @brief Draw multiple meshes at once
         * @m_since{2020,06}
         *
         * In OpenGL ES, if @gl_extension{EXT,multi_draw_arrays} is not
         * present, the functionality is emulated using a sequence of
         * @ref draw(MeshView&) calls.
         *
         * If @gl_extension{ARB,vertex_array_object} (part of OpenGL 3.0),
         * OpenGL ES 3.0, WebGL 2.0, @gl_extension{OES,vertex_array_object} in
         * OpenGL ES 2.0 or @webgl_extension{OES,vertex_array_object} in WebGL
         * 1.0 is available, the associated vertex array object is bound
         * instead of setting up the mesh from scratch.
         * @attention All meshes must be views of the same original mesh and
         *      must not be instanced.
         * @see @ref draw(MeshView&), @fn_gl{UseProgram},
         *      @fn_gl_keyword{EnableVertexAttribArray}, @fn_gl{BindBuffer},
         *      @fn_gl_keyword{VertexAttribPointer}, @fn_gl_keyword{DisableVertexAttribArray}
         *      or @fn_gl{BindVertexArray}, @fn_gl_keyword{MultiDrawArrays} or
         *      @fn_gl_keyword{MultiDrawElements}/@fn_gl_keyword{MultiDrawElementsBaseVertex}
         * @requires_gl32 Extension @gl_extension{ARB,draw_elements_base_vertex}
         *      if the mesh is indexed and @ref MeshView::baseVertex() is not
         *      `0`
         * @requires_gl Specifying base vertex for indexed meshes is not
         *      available in OpenGL ES or WebGL.
         */
        void draw(Containers::ArrayView<const Containers::Reference<MeshView>> meshes);

        /**
         * @overload
         * @m_since{2020,06}
         */
        void draw(std::initializer_list<Containers::Reference<MeshView>> meshes);

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Draw a mesh with vertices coming out of transform feedback
         * @param mesh      Mesh to draw
         * @param xfb       Transform feedback to use for vertex count
         * @param stream    Transform feedback stream ID
         * @m_since{2020,06}
         *
         * Expects that @p mesh is compatible with this shader, is fully set up
         * and that the output buffer(s) from @p xfb are used as vertex buffers
         * in the mesh. Everything set by @ref Mesh::setCount(),
         * @ref Mesh::setBaseInstance(), @ref Mesh::setBaseVertex() and
         * @ref Mesh::setIndexBuffer() is ignored, the mesh is drawn as
         * non-indexed and the vertex count is taken from the @p xfb object. If
         * @p stream is @cpp 0 @ce, non-stream draw command is used. If
         * @gl_extension{ARB,vertex_array_object} (part of OpenGL 3.0) is
         * available, the associated vertex array object is bound instead of
         * setting up the mesh from scratch.
         * @see @ref Mesh::setInstanceCount(), @ref draw(Mesh&),
         *      @ref drawTransformFeedback(MeshView&, TransformFeedback&, UnsignedInt),
         *      @fn_gl_keyword{UseProgram}, @fn_gl_keyword{EnableVertexAttribArray},
         *      @fn_gl{BindBuffer}, @fn_gl_keyword{VertexAttribPointer},
         *      @fn_gl_keyword{DisableVertexAttribArray} or @fn_gl_keyword{BindVertexArray},
         *      @fn_gl_keyword{DrawTransformFeedback}/@fn_gl_keyword{DrawTransformFeedbackInstanced} or
         *      @fn_gl_keyword{DrawTransformFeedbackStream}/@fn_gl_keyword{DrawTransformFeedbackStreamInstanced}
         * @requires_gl40 Extension @gl_extension{ARB,transform_feedback2}
         * @requires_gl40 Extension @gl_extension{ARB,transform_feedback3} if
         *      @p stream is not `0`
         * @requires_gl42 Extension @gl_extension{ARB,transform_feedback_instanced}
         *      if @ref Mesh::instanceCount() is more than `1`
         */
        void drawTransformFeedback(Mesh& mesh, TransformFeedback& xfb, UnsignedInt stream = 0);

        /**
         * @brief Draw a mesh view with vertices coming out of transform feedback
         * @m_since{2020,06}
         *
         * Everything set by @ref MeshView::setCount(),
         * @ref MeshView::setBaseInstance(), @ref MeshView::setBaseVertex(),
         * @ref MeshView::setIndexRange() and @ref Mesh::setIndexBuffer() is
         * ignored, the mesh is drawn as non-indexed and the vertex count is
         * taken from the @p xfb object. See
         * @ref drawTransformFeedback(Mesh&, TransformFeedback&, UnsignedInt)
         * for more information.
         * @see @ref draw(Mesh&)
         * @requires_gl40 Extension @gl_extension{ARB,transform_feedback2}
         * @requires_gl40 Extension @gl_extension{ARB,transform_feedback3} if
         *      @p stream is not `0`
         * @requires_gl42 Extension @gl_extension{ARB,transform_feedback_instanced}
         *      if @ref MeshView::instanceCount() is more than `1`
         */
        void drawTransformFeedback(MeshView& mesh, TransformFeedback& xfb, UnsignedInt stream = 0);
        #endif

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @brief Dispatch compute
         * @param workgroupCount    Workgroup count in given dimension
         *
         * Valid only on programs with compute shader attached.
         * @see @fn_gl{DispatchCompute}
         * @requires_gl43 Extension @gl_extension{ARB,compute_shader}
         * @requires_gles31 Compute shaders are not available in OpenGL ES 3.0
         *      and older.
         * @requires_gles Compute shaders are not available in WebGL.
         */
        void dispatchCompute(const Vector3ui& workgroupCount);
        #endif

    protected:
        /**
         * @brief Link the shader
         *
         * Returns @cpp false @ce if linking of any shader failed, @cpp true @ce
         * if everything succeeded. Linker message (if any) is printed to error
         * output. All attached shaders must be compiled with
         * @ref Shader::compile() before linking. The operation is batched in a
         * way that allows the driver to link multiple shaders simultaneously
         * (i.e. in multiple threads).
         * @see @fn_gl_keyword{LinkProgram}, @fn_gl_keyword{GetProgram} with
         *      @def_gl{LINK_STATUS} and @def_gl{INFO_LOG_LENGTH},
         *      @fn_gl_keyword{GetProgramInfoLog}
         */
        static bool link(std::initializer_list<Containers::Reference<AbstractShaderProgram>> shaders);

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @brief Allow retrieving program binary
         *
         * Initially disabled.
         * @see @fn_gl_keyword{ProgramParameter} with
         *      @def_gl{PROGRAM_BINARY_RETRIEVABLE_HINT}
         * @requires_gl41 Extension @gl_extension{ARB,get_program_binary}
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
         * @see @fn_gl_keyword{ProgramParameter} with @def_gl{PROGRAM_SEPARABLE}
         * @requires_gl41 Extension @gl_extension{ARB,separate_shader_objects}
         * @requires_es_extension Extension
         *      @gl_extension2{EXT,separate_shader_objects,EXT_separate_shader_objects.gles}
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
         * @brief Attach a shader
         *
         * @see @fn_gl_keyword{AttachShader}
         */
        void attachShader(Shader& shader);

        /**
         * @brief Attach shaders
         *
         * Convenience overload to the above, allowing the user to specify more
         * than one shader at once. Other than that there is no other
         * (performance) difference when using this function.
         */
        void attachShaders(std::initializer_list<Containers::Reference<Shader>> shaders);

        /**
         * @brief Bind an attribute to given location
         * @param location      Location
         * @param name          Attribute name
         *
         * Binds attribute to location which is used later for binding vertex
         * buffers.
         * @see @fn_gl_keyword{BindAttribLocation}
         * @deprecated_gl Preferred usage is to specify attribute location
         *      explicitly in the shader instead of using this function. See
         *      @ref GL-AbstractShaderProgram-attribute-location "class documentation"
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
         * @param index         Blend equation color input index (@cpp 0 @ce or
         *      @cpp 1 @ce)
         * @param name          Fragment output variable name
         *
         * Binds fragment data to location which is used later for framebuffer
         * operations. See also @ref Renderer::BlendFunction for more
         * information about using color input index.
         * @see @fn_gl_keyword{BindFragDataLocationIndexed}
         * @deprecated_gl Preferred usage is to specify attribute location
         *      explicitly in the shader instead of using this function. See
         *      @ref GL-AbstractShaderProgram-attribute-location "class documentation"
         *      for more information.
         * @requires_gl33 Extension @gl_extension{ARB,blend_func_extended}
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
         * set to @cpp 0 @ce.
         * @see @fn_gl_keyword{BindFragDataLocation}
         * @deprecated_gl Preferred usage is to specify attribute location
         *      explicitly in the shader instead of using this function. See
         *      @ref GL-AbstractShaderProgram-attribute-location "class documentation"
         *      for more information.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gl Use explicit location specification in OpenGL ES 3.0 or
         *      WebGL 2.0 and `gl_FragData[n]` provided by @gl_extension{NV,draw_buffers}
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
         * @see @fn_gl_keyword{TransformFeedbackVaryings}
         * @deprecated_gl Preferred usage is to specify transform feedback
         *      outputs explicitly in the shader instead of using this
         *      function. See @ref GL-AbstractShaderProgram-transform-feedback "class documentation"
         *      for more information.
         * @requires_gl30 Extension @gl_extension{EXT,transform_feedback}
         * @requires_gl40 Extension @gl_extension{ARB,transform_feedback3} for
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
         * at once using @ref link(std::initializer_list<Containers::Reference<AbstractShaderProgram>>)
         * for improved performance, see its documentation for more
         * information.
         */
        bool link();

        /**
         * @brief Get uniform location
         * @param name          Uniform name
         *
         * If given uniform is not found in the linked shader, a warning is
         * printed and @cpp -1 @ce is returned.
         * @see @ref setUniform(), @fn_gl_keyword{GetUniformLocation}
         * @deprecated_gl Preferred usage is to specify uniform location
         *      explicitly in the shader instead of using this function. See
         *      @ref GL-AbstractShaderProgram-uniform-location "class documentation"
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
         * warning is printed and @cpp 0xffffffffu @ce is returned.
         * @see @ref setUniformBlockBinding(),
         *      @fn_gl_keyword{GetUniformBlockIndex}
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         * @deprecated_gl Preferred usage is to specify uniform block binding
         *      explicitly in the shader instead of using this function. See
         *      @ref GL-AbstractShaderProgram-uniform-block-binding "class documentation"
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
         * If neither @gl_extension{ARB,separate_shader_objects} (part of
         * OpenGL 4.1) nor @gl_extension{EXT,separate_shader_objects} OpenGL ES
         * extension nor OpenGL ES 3.1 is available, the shader is marked for
         * use before the operation.
         * @see @ref setUniform(Int, const T&), @ref uniformLocation(),
         *      @fn_gl{UseProgram}, @fn_gl_keyword{Uniform} or
         *      @fn_gl_keyword{ProgramUniform}
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
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
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
         * @requires_gl40 Extension @gl_extension{ARB,gpu_shader_fp64}
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
         * @requires_gl40 Extension @gl_extension{ARB,gpu_shader_fp64}
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

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Set uniform block binding
         * @param index     Uniform block index
         * @param binding   Uniform block binding
         *
         * @see @ref uniformBlockIndex(), @ref Buffer::maxUniformBindings(),
         *      @fn_gl_keyword{UniformBlockBinding}
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         * @deprecated_gl Preferred usage is to specify uniform block binding
         *      explicitly in the shader instead of using this function. See
         *      @ref GL-AbstractShaderProgram-uniform-block-binding "class documentation"
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
        #ifndef MAGNUM_TARGET_GLES
        void bindFragmentDataLocationIndexedInternal(UnsignedInt location, UnsignedInt index, Containers::ArrayView<const char> name);
        void bindFragmentDataLocationInternal(UnsignedInt location, Containers::ArrayView<const char> name);
        #endif
        Int uniformLocationInternal(Containers::ArrayView<const char> name);
        UnsignedInt uniformBlockIndexInternal(Containers::ArrayView<const char> name);

        #ifndef MAGNUM_TARGET_GLES2
        void MAGNUM_GL_LOCAL transformFeedbackVaryingsImplementationDefault(Containers::ArrayView<const std::string> outputs, TransformFeedbackBufferMode bufferMode);
        #ifdef CORRADE_TARGET_WINDOWS
        void MAGNUM_GL_LOCAL transformFeedbackVaryingsImplementationDanglingWorkaround(Containers::ArrayView<const std::string> outputs, TransformFeedbackBufferMode bufferMode);
        #endif
        #endif

        void use();

        /*
            Currently, there are three supported ways to call glProgramUniform():

            - EXT_separate_shader_objects (OpenGL ES extension, EXT suffix)
            - ARB_separate_shader_objects (desktop GL only, no suffix)
            - OpenGL ES 3.1, no suffix

            To avoid copypasta and filesize bloat, this is merged to just two
            variants of implementation functions:

            - uniformImplementationSSO() -- functions without suffix, used if
                ARB_separate_shader_objects desktop extension or OpenGL ES 3.1
                is available, completely disabled for ES2
            - uniformImplementationSSOEXT() -- functions with EXT suffix, used
                if EXT_separate_shader_objects ES 2.0 / ES 3.0 extension is
                available
        */

        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const GLfloat* values);
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<2, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<3, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<4, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const GLint* values);
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<2, GLint>* values);
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<3, GLint>* values);
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<4, GLint>* values);
        #ifndef MAGNUM_TARGET_GLES2
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const GLuint* values);
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<2, GLuint>* values);
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<3, GLuint>* values);
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<4, GLuint>* values);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const GLdouble* values);
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<2, GLdouble>* values);
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<3, GLdouble>* values);
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::Vector<4, GLdouble>* values);
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const GLfloat* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::Vector<2, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::Vector<3, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::Vector<4, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const GLint* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::Vector<2, GLint>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::Vector<3, GLint>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::Vector<4, GLint>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const GLuint* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::Vector<2, GLuint>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::Vector<3, GLuint>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::Vector<4, GLuint>* values);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const GLdouble* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::Vector<2, GLdouble>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::Vector<3, GLdouble>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::Vector<4, GLdouble>* values);
        #endif
        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
        void MAGNUM_GL_LOCAL uniformImplementationSSOEXT(GLint location, GLsizei count, const GLfloat* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSOEXT(GLint location, GLsizei count, const Math::Vector<2, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSOEXT(GLint location, GLsizei count, const Math::Vector<3, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSOEXT(GLint location, GLsizei count, const Math::Vector<4, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSOEXT(GLint location, GLsizei count, const GLint* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSOEXT(GLint location, GLsizei count, const Math::Vector<2, GLint>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSOEXT(GLint location, GLsizei count, const Math::Vector<3, GLint>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSOEXT(GLint location, GLsizei count, const Math::Vector<4, GLint>* values);
        #ifndef MAGNUM_TARGET_GLES2
        void MAGNUM_GL_LOCAL uniformImplementationSSOEXT(GLint location, GLsizei count, const GLuint* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSOEXT(GLint location, GLsizei count, const Math::Vector<2, GLuint>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSOEXT(GLint location, GLsizei count, const Math::Vector<3, GLuint>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSOEXT(GLint location, GLsizei count, const Math::Vector<4, GLuint>* values);
        #endif
        #endif

        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<2, 2, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<3, 3, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<4, 4, GLfloat>* values);
        #ifndef MAGNUM_TARGET_GLES2
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<2, 3, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<3, 2, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<2, 4, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<4, 2, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<3, 4, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<4, 3, GLfloat>* values);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<2, 2, GLdouble>* values);
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<3, 3, GLdouble>* values);
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<4, 4, GLdouble>* values);
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<2, 3, GLdouble>* values);
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<3, 2, GLdouble>* values);
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<2, 4, GLdouble>* values);
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<4, 2, GLdouble>* values);
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<3, 4, GLdouble>* values);
        void MAGNUM_GL_LOCAL uniformImplementationDefault(GLint location, GLsizei count, const Math::RectangularMatrix<4, 3, GLdouble>* values);
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<2, 2, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<3, 3, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<4, 4, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<2, 3, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<3, 2, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<2, 4, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<4, 2, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<3, 4, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<4, 3, GLfloat>* values);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<2, 2, GLdouble>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<3, 3, GLdouble>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<4, 4, GLdouble>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<2, 3, GLdouble>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<3, 2, GLdouble>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<2, 4, GLdouble>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<4, 2, GLdouble>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<3, 4, GLdouble>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSO(GLint location, GLsizei count, const Math::RectangularMatrix<4, 3, GLdouble>* values);
        #endif
        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
        void MAGNUM_GL_LOCAL uniformImplementationSSOEXT(GLint location, GLsizei count, const Math::RectangularMatrix<2, 2, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSOEXT(GLint location, GLsizei count, const Math::RectangularMatrix<3, 3, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSOEXT(GLint location, GLsizei count, const Math::RectangularMatrix<4, 4, GLfloat>* values);
        #ifndef MAGNUM_TARGET_GLES2
        void MAGNUM_GL_LOCAL uniformImplementationSSOEXT(GLint location, GLsizei count, const Math::RectangularMatrix<2, 3, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSOEXT(GLint location, GLsizei count, const Math::RectangularMatrix<3, 2, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSOEXT(GLint location, GLsizei count, const Math::RectangularMatrix<2, 4, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSOEXT(GLint location, GLsizei count, const Math::RectangularMatrix<4, 2, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSOEXT(GLint location, GLsizei count, const Math::RectangularMatrix<3, 4, GLfloat>* values);
        void MAGNUM_GL_LOCAL uniformImplementationSSOEXT(GLint location, GLsizei count, const Math::RectangularMatrix<4, 3, GLfloat>* values);
        #endif
        #endif

        GLuint _id;

        #if defined(CORRADE_TARGET_WINDOWS) && !defined(MAGNUM_TARGET_GLES2)
        /* Needed for the nv-windows-dangling-transform-feedback-varying-names
           workaround */
        std::vector<std::string> _transformFeedbackVaryingNames;
        #endif
};

}}

#endif
