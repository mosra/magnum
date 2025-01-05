#ifndef Magnum_GL_AbstractShaderProgram_h
#define Magnum_GL_AbstractShaderProgram_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2022 Vladislav Oleshko <vladislav.oleshko@gmail.com>
    Copyright @ 2022 Hugo Amiard <hugo.amiard@wonderlandengine.com>

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
 * @brief Class @ref Magnum::GL::AbstractShaderProgram, macro @ref MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION(), @ref MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DISPATCH_IMPLEMENTATION()
 */

#include "Magnum/Tags.h"
#include "Magnum/GL/AbstractObject.h"
#include "Magnum/GL/GL.h"

#if defined(CORRADE_TARGET_WINDOWS) && !defined(MAGNUM_TARGET_GLES2)
#include <Corrade/Containers/ArrayTuple.h>
#endif

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>
/* For attachShaders(), which used to take a std::initializer_list<Reference>,
   and draw({MeshView}) as well */
#include <Corrade/Containers/Iterable.h>
/* For label() / setLabel() and all uniform/attribute/... name stuff that used
   to be a std::string. Since that's all only function parameters and no return
   types, it should cover all cases. */
#include <Corrade/Containers/StringStl.h>
/* For setTransformFeedbackOutputs(), which used to take
   std::initializer_list<std::string>. Usually people passed C string literals,
   so it should convert directly to a StringIterable. */
#include <Corrade/Containers/StringIterable.h>
#endif

namespace Magnum { namespace GL {

namespace Implementation { struct ShaderProgramState; }

/**
@brief Base for shader program implementations

Wraps an OpenGL shader program object. Meant to be only used through
subclasses, either via builtin shaders in the @ref Shaders namespace or by
creating a custom shader implementation.

@m_class{m-note m-success}

@par
    A simple custom shader implementation is shown in the
    @ref examples-texturedquad "Textured Quad example".

@section GL-AbstractShaderProgram-subclassing Subclassing workflow

This class is designed to be used via subclassing. Subclasses define these
functions and properties:

<ul>
<li> **Attribute definitions** using @ref Attribute typedefs with location and
    type for configuring meshes, for example:

    @snippet GL.cpp AbstractShaderProgram-input-attributes
</li>
<li> **Output attribute locations**, if desired, for example:

    @snippet GL.cpp AbstractShaderProgram-output-attributes
</li>
<li> **Constructor**, which loads, compiles and attaches particular shaders and
    links the program together, for example:

    @snippet GL.cpp AbstractShaderProgram-constructor
</li>
<li> **Uniform setting functions**, which will provide public interface for
    protected @ref setUniform() functions. For usability purposes you can
    implement also method chaining. Example:

    @snippet GL.cpp AbstractShaderProgram-uniforms
</li>
<li> **Texture and texture image binding functions** in which you bind the
    textures/images to particular texture/image units using
    @ref Texture::bind() "*Texture::bind()" /
    @ref Texture::bindImage() "*Texture::bindImage()" and similar, for example:

    @snippet GL.cpp AbstractShaderProgram-textures
</li>
<li> **Transform feedback setup function**, if needed, in which you bind
    buffers to particular indices using @ref TransformFeedback::attachBuffer()
    and similar, possibly with overloads based on desired use cases, e.g.:

    @snippet GL.cpp AbstractShaderProgram-xfb
</li>
<li>And optionally, **return derived type from relevant draw/dispatch functions**
    to make it possible for users to easily chain draw calls; and on the other
    hand **hide the irrelevant APIs** to prevent users from accidentally
    calling @ref draw() / @ref drawTransformFeedback() on compute shaders, or
    @ref dispatchCompute() on shaders that aren't compute. Because there's many
    overloads of those APIs and they differ based on target platform, it's
    recommended to be done via either
    @ref MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION() or
    @ref MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DISPATCH_IMPLEMENTATION():

    @snippet GL.cpp AbstractShaderProgram-subclass-macro
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

Similarly for output attributes, you can also specify blend equation color index
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

@snippet GL.cpp AbstractShaderProgram-binding

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
    ES 2.0, similar functionality is available in the
    @gl_extension{EXT,draw_buffers} or @gl_extension{NV,draw_buffers}
    extensions
@requires_es_extension OpenGL ES 3.0 and extension
    @gl_extension{EXT,blend_func_extended} for using
    @ref bindFragmentDataLocation() or @ref bindFragmentDataLocationIndexed()
@requires_webgl20 Explicit location specification of input attributes is not
    supported in WebGL 1.0, use @ref bindAttributeLocation() instead.
@requires_webgl20 Multiple fragment shader outputs are not available in WebGL
    1.0, similar functionality is available in the
    @webgl_extension{WEBGL,draw_buffers} or
    @webgl_extension{WEBGL,blend_func_extended} extensions. Additionally, the
    @webgl_extension{WEBGL,blend_func_extended} extension doesn't support
    application-side binding of fragment data locations.

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

@snippet GL.cpp AbstractShaderProgram-uniform-location

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

@snippet GL.cpp AbstractShaderProgram-uniform-block-binding

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
@ref setUniform(Int, Int). Note that additional syntax changes may be needed
for GLSL ES.

@code{.glsl}
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
@endcode

@snippet GL.cpp AbstractShaderProgram-texture-uniforms

@see @ref Shader::maxTextureImageUnits(), @ref maxImageUnits()
@requires_gl42 Extension @gl_extension{ARB,shading_language_420pack} for explicit
    texture binding unit instead of using @ref setUniform(Int, Int).
@requires_gles31 Explicit texture binding unit is not supported in OpenGL ES
    3.0 and older. Use @ref setUniform(Int, Int) instead.
@requires_gles Explicit texture binding unit is not supported in WebGL. Use
    @ref setUniform(Int, Int) instead.

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

@snippet GL.cpp AbstractShaderProgram-xfb-outputs

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

@snippet GL.cpp AbstractShaderProgram-rendering

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

@section GL-AbstractShaderProgram-async Asynchronous shader compilation and linking

The workflow described @ref GL-AbstractShaderProgram-subclassing "at the very top"
compiles and links the shader directly in a constructor. While that's fine for
many use cases, with heavier shaders, many shader combinations or on
platforms that translate GLSL to other APIs such as HLSL or MSL, the
compilation and linking can take a significant portion of application startup
time.

To mitigate this problem, nowadays drivers implement *asynchronous compilation*
--- when shader compilation or linking is requested, the driver offloads the
work to separate worker threads, and serializes it back to the application
thread only once the application wants to retrieve the result of the operation.
Which means, the ideal way to spread the operation over more CPU cores is to
first submit compilation & linking of several shaders at once and only then ask
for operation result. That allows the driver to perform compilation/linking of
multiple shaders at once. Furthermore, the
@gl_extension{KHR,parallel_shader_compile} extension adds a possibility to
query whether the operation was finished for a particular shader. That allows
the application to schedule other work in the meantime.

Async compilation and linking can be implemented by using
@ref Shader::submitCompile() and @ref submitLink(), followed by
@ref checkLink() (which optionally delegates to @ref Shader::checkCompile()),
instead of @ref Shader::compile() and @ref link(). Calling the submit functions
will trigger a (potentially async) compilation and linking, calling the check
functions will check the operation result, potentially stalling if the async
operation isn't finished yet.

The @ref Shader::isCompileFinished() and
@ref isLinkFinished() APIs then provide a way to query if the submitted
operation finished. If @gl_extension{KHR,parallel_shader_compile} is not
available, those two implicitly return @cpp true @ce, thus effectively causing
a stall if the operation isn't yet done at the time you call
@ref Shader::checkCompile() / @ref checkLink() --- but compared to the linear
workflow you still get the benefits from submitting multiple operations at
once.

A common way to equip an @ref AbstractShaderProgram subclass with async
creation capability while keeping also the simple constructor is the following:

1.  An internal @ref NoInit constructor for the subclass is added, which only
    creates the @ref AbstractShaderProgram base but does nothing else.
2.  A @cpp CompileState @ce inner class is defined as a subclass of
    @cpp MyShader @ce. Besides that it holds all temporary state needed to
    finish the construction --- in particular all @ref Shader instances.
3.  A @cpp static CompileState compile(…) @ce function does everything until
    and including linking as the original constructor did, except that it calls
    @ref Shader::submitCompile() and @ref submitLink() instead of
    @ref Shader::compile() and @ref link(), and returns a populated
    @cpp CompileState @ce instance.
4.  A @cpp MyShader(CompileState&&) @ce constructor then takes over the base
    of @cpp CompileState @ce by delegating it into the move constructor. Then
    it calls @ref checkLink(), passing all input shaders to it for a complete
    context in case of an error, and finally performs any remaining post-link
    steps such as uniform setup.
5.  The original @cpp MyShader(…) @ce constructor now only passes the result of
    @cpp compile() @ce to @cpp MyShader(CompileState&&) @ce.

@snippet GL.cpp AbstractShaderProgram-async

Usage-wise, it can look for example like below, with the last line waiting for
linking to finish and making the shader ready to use. On drivers that don't
perform any async compilation this will behave the same as if the construction
was done the usual way.

@snippet GL.cpp AbstractShaderProgram-async-usage

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
         * @brief Max supported shader storage block size in bytes
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
         * @brief Max supported uniform block size in bytes
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
        Containers::String label() const;

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
        AbstractShaderProgram& setLabel(Containers::StringView label);
        #endif

        /**
         * @brief Validate program
         *
         * Returns validation status and optional validation message.
         * @see @fn_gl_keyword{ValidateProgram}, @fn_gl_keyword{GetProgram} with
         *      @def_gl{VALIDATE_STATUS}, @def_gl{INFO_LOG_LENGTH},
         *      @fn_gl_keyword{GetProgramInfoLog}
         */
        Containers::Pair<bool, Containers::String> validate();

        /**
         * @brief Draw a mesh
         * @return Reference to self (for method chaining)
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
         *      @ref draw(const Containers::Iterable<MeshView>&),
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
        AbstractShaderProgram& draw(Mesh& mesh);

        /**
         * @overload
         * @m_since{2020,06}
         */
        AbstractShaderProgram& draw(Mesh&& mesh) {
            return draw(mesh);
        }

        /**
         * @brief Draw a mesh view
         * @return Reference to self (for method chaining)
         * @m_since{2020,06}
         *
         * See @ref draw(Mesh&) for more information.
         * @see @ref draw(const Containers::Iterable<MeshView>&),
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
        AbstractShaderProgram& draw(MeshView& mesh);

        /**
         * @overload
         * @m_since{2020,06}
         */
        AbstractShaderProgram& draw(MeshView&& mesh) {
            return draw(mesh);
        }

        /**
         * @brief Draw multiple mesh views at once
         * @param mesh          The mesh from which to draw
         * @param counts        Vertex/index counts for each draw
         * @param vertexOffsets Offsets into the vertex array for non-indexed
         *      meshes, base vertex for indexed meshes. Expected to have the
         *      same size as @p counts, for indexed meshes it can be also empty
         *      in which case the base vertex is assumed to be @cpp 0 @ce for
         *      all draws.
         * @param indexOffsets  Offsets into the index buffer for indexed
         *      meshes, *in bytes*. Expected to have the same size as @p counts
         *      for indexed meshes, ignored for non-indexed.
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @p mesh is compatible with this shader and is fully set
         * up. If @p counts is empty, no draw commands are issued. If
         * @gl_extension{ARB,vertex_array_object} (part of OpenGL 3.0), OpenGL
         * ES 3.0, WebGL 2.0, @gl_extension{OES,vertex_array_object} in OpenGL
         * ES 2.0 or @webgl_extension{OES,vertex_array_object} in WebGL 1.0 is
         * available, the associated vertex array object is bound instead of
         * setting up the mesh from scratch.
         *
         * If @p counts, @p vertexOffsets and @p indexOffsets are contiguous
         * views, they get passed directly to the underlying GL functions,
         * otherwise a temporary contiguous copy is allocated. There are the
         * folllowing special cases, however:
         *
         * -    On @ref CORRADE_TARGET_32BIT "64-bit builds" the
         *      @p indexOffsets additionally have to be 64-bit in order to
         *      avoid a copy because the @fn_gl{MultiDrawElements} /
         *      @fn_gl_keyword{MultiDrawElementsBaseVertex} functions accept
         *      them as pointers, see the @ref draw(Mesh&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedLong>&)
         *      overload below.
         * -    If the @p mesh is indexed, @p vertexOffsets are not empty and
         *      the platform is WebGL or OpenGL ES with
         *      @gl_extension{OES,draw_elements_base_vertex} /
         *      @gl_extension{EXT,draw_elements_base_vertex} supported but
         *      @gl_extension{EXT,multi_draw_arrays} not, the function has to
         *      delegate to @ref draw(Mesh&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&)
         *      and allocate trivial instance counts and offsets. To avoid this
         *      extra allocation, use the overload directly if the
         *      @m_class{m-doc-external} [ANGLE_base_vertex_base_instance](https://chromium.googlesource.com/angle/angle/+/refs/heads/main/extensions/ANGLE_base_vertex_base_instance.txt)
         *      OpenGL ES or @webgl_extension{WEBGL,multi_draw_instanced_base_vertex_base_instance}
         *      WebGL extension is supported.
         *
         * @see @ref draw(MeshView&), @fn_gl{UseProgram},
         *      @fn_gl_keyword{EnableVertexAttribArray}, @fn_gl{BindBuffer},
         *      @fn_gl_keyword{VertexAttribPointer},
         *      @fn_gl_keyword{DisableVertexAttribArray}
         *      or @fn_gl{BindVertexArray}, @fn_gl_keyword{MultiDrawArrays} or
         *      @fn_gl_keyword{MultiDrawElements} /
         *      @fn_gl_keyword{MultiDrawElementsBaseVertex}
         * @requires_gl32 Extension @gl_extension{ARB,draw_elements_base_vertex}
         *      if the mesh is indexed and the @p vertexOffsets view is not
         *      empty.
         * @requires_es_extension Extension @gl_extension{EXT,multi_draw_arrays}
         *      or @m_class{m-doc-external} [ANGLE_multi_draw](https://chromium.googlesource.com/angle/angle/+/master/extensions/ANGLE_multi_draw.txt)
         * @requires_es_extension OpenGL ES 3.0 and extension
         *      @gl_extension{OES,draw_elements_base_vertex} or
         *      @gl_extension{EXT,draw_elements_base_vertex} if the mesh is
         *      indexed and the @p vertexOffsets view is not empty.
         * @requires_webgl_extension Extension @webgl_extension{WEBGL,multi_draw}.
         *      Note that this extension is only implemented since Emscripten
         *      2.0.0 and thus it's not even advertised on older versions.
         * @requires_webgl_extension WebGL 2.0 and extension
         *      @webgl_extension{WEBGL,multi_draw_instanced_base_vertex_base_instance}
         *      if the mesh is indexed and the @p vertexOffsets view is not
         *      empty.
         */
        AbstractShaderProgram& draw(Mesh& mesh, const Containers::StridedArrayView1D<const UnsignedInt>& counts, const Containers::StridedArrayView1D<const UnsignedInt>& vertexOffsets, const Containers::StridedArrayView1D<const UnsignedInt>& indexOffsets);

        #ifndef CORRADE_TARGET_32BIT
        /**
         * @brief Draw multiple mesh views at once
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Defined only on @ref CORRADE_TARGET_32BIT "64-bit builds". Compared
         * to @ref draw(Mesh&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&)
         * this overload can avoid allocating an array of 64-bit pointers for
         * the @fn_gl_keyword{MultiDrawElements} /
         * @fn_gl_keyword{MultiDrawElementsBaseVertex} function and can instead
         * directly reuse the @p indexOffsets view if it's contiguous.
         */
        AbstractShaderProgram& draw(Mesh& mesh, const Containers::StridedArrayView1D<const UnsignedInt>& counts, const Containers::StridedArrayView1D<const UnsignedInt>& vertexOffsets, const Containers::StridedArrayView1D<const UnsignedLong>& indexOffsets);

        /**
         * @overload
         * @m_since_latest
         */
        AbstractShaderProgram& draw(Mesh& mesh, const Containers::StridedArrayView1D<const UnsignedInt>& counts, const Containers::StridedArrayView1D<const UnsignedInt>& vertexOffsets, std::nullptr_t);
        #endif

        #if defined(MAGNUM_TARGET_GLES) || defined(DOXYGEN_GENERATING_OUTPUT)
        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Draw multiple instanced mesh views with instance offsets at once
         * @param mesh          The mesh from which to draw
         * @param counts        Vertex/index counts for each draw
         * @param instanceCounts Instance counts for each draw. Expected to
         *      have the same size as @p counts.
         * @param vertexOffsets Offsets into the vertex array for non-indexed
         *      meshes, base vertex for indexed meshes. Expected to have the
         *      same size as @p counts, for indexed meshes it can be also empty
         *      in which case the base vertex is assumed to be @cpp 0 @ce for
         *      all draws.
         * @param indexOffsets  Offsets into the index buffer for indexed
         *      meshes, *in bytes*. Expected to have the same size as @p counts
         *      for indexed meshes, ignored for non-indexed.
         * @param instanceOffsets Offsets to be added to the instance index for
         *      each draw. Expected to either be empty or have the same size as
         *      @p counts.
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * If @p counts, @p instanceCounts, @p vertexOffsets, @p indexOffsets
         * and @p instanceOffsets are contiguous views, they get passed
         * directly to the underlying GL functions, otherwise a temporary
         * contiguous copy is allocated. There are the following special cases,
         * however:
         *
         * -    On @ref CORRADE_TARGET_32BIT "64-bit builds" the
         *      @p indexOffsets additionally have to be 64-bit in order to
         *      avoid a copy because the @m_class{m-doc-external} [glMultiDrawArraysInstancedANGLE()](https://chromium.googlesource.com/angle/angle/+/refs/heads/main/extensions/ANGLE_multi_draw.txt)
         *      / @m_class{m-doc-external} [glMultiDrawElementsInstancedBaseVertexBaseInstanceANGLE()](https://chromium.googlesource.com/angle/angle/+/refs/heads/main/extensions/ANGLE_base_vertex_base_instance.txt)
         *      functions accept them as pointers, see the @ref draw(Mesh&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedLong>&, const Containers::StridedArrayView1D<const UnsignedInt>&)
         *      overload below.
         * -    If the @p mesh is indexed, @p vertexOffsets and
         *      @p instanceOffsets have to be either both specified or both
         *      empty in order to avoid a copy with the missing one filled with
         *      zeros, as there's only either @m_class{m-doc-external} [glMultiDrawElementsInstancedANGLE()](https://chromium.googlesource.com/angle/angle/+/refs/heads/main/extensions/ANGLE_multi_draw.txt)
         *      or @m_class{m-doc-external} [glMultiDrawElementsInstancedBaseVertexBaseInstanceANGLE()](https://chromium.googlesource.com/angle/angle/+/refs/heads/main/extensions/ANGLE_base_vertex_base_instance.txt),
         *      but no function accepting just one of them.
         *
         * @see @ref draw(MeshView&), @fn_gl{UseProgram},
         *      @fn_gl_keyword{EnableVertexAttribArray}, @fn_gl{BindBuffer},
         *      @fn_gl_keyword{VertexAttribPointer}, @fn_gl_keyword{DisableVertexAttribArray}
         *      or @fn_gl{BindVertexArray},
         *      @m_class{m-doc-external} [glMultiDrawArraysInstancedANGLE()](https://chromium.googlesource.com/angle/angle/+/refs/heads/main/extensions/ANGLE_multi_draw.txt)
         *      / @m_class{m-doc-external} [glMultiDrawArraysInstancedBaseInstanceANGLE()](https://chromium.googlesource.com/angle/angle/+/refs/heads/main/extensions/ANGLE_base_vertex_base_instance.txt) or
         *      @m_class{m-doc-external} [glMultiDrawElementsInstancedANGLE()](https://chromium.googlesource.com/angle/angle/+/refs/heads/main/extensions/ANGLE_multi_draw.txt)
         *      / @m_class{m-doc-external} [glMultiDrawElementsInstancedBaseVertexBaseInstanceANGLE()](https://chromium.googlesource.com/angle/angle/+/refs/heads/main/extensions/ANGLE_base_vertex_base_instance.txt)
         * @requires_gles_only Not available on desktop OpenGL.
         * @requires_gles30 Not defined in OpenGL ES 2.0. Use
         *      @ref draw(Mesh&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&) without the
         *      @p instanceOffsets parameter there instead.
         * @requires_es_extension Extension @m_class{m-doc-external} [ANGLE_multi_draw](https://chromium.googlesource.com/angle/angle/+/master/extensions/ANGLE_multi_draw.txt)
         * @requires_es_extension OpenGL ES 3.0 and extension
         *      @m_class{m-doc-external} [ANGLE_base_vertex_base_instance](https://chromium.googlesource.com/angle/angle/+/refs/heads/main/extensions/ANGLE_base_vertex_base_instance.txt)
         *      if the mesh is indexed and the @p vertexOffsets view is not
         *      empty. The extension says ES 3.1 is required, but since ANGLE
         *      is the only implementer of this extension and exposes just ES
         *      3.0, there's no point in requiring 3.1.
         * @requires_es_extension OpenGL ES 3.0 and extension
         *      @m_class{m-doc-external} [ANGLE_base_vertex_base_instance](https://chromium.googlesource.com/angle/angle/+/refs/heads/main/extensions/ANGLE_base_vertex_base_instance.txt)
         *      if the @p instanceOffsets view is not empty. The extension says
         *      ES 3.1 is required, but since ANGLE is the only implementer of
         *      this extension and exposes just ES 3.0, there's no point in
         *      requiring 3.1.
         * @requires_webgl_extension Extension @webgl_extension{WEBGL,multi_draw}.
         *      Note that this extension is only implemented since Emscripten
         *      2.0.0 and thus it's not even advertised on older versions.
         * @requires_webgl_extension WebGL 2.0 and extension
         *      @webgl_extension{WEBGL,multi_draw_instanced_base_vertex_base_instance}
         *      if the mesh is indexed and the @p vertexOffsets view is not
         *      empty.
         * @requires_webgl_extension WebGL 2.0 and extension
         *      @webgl_extension{WEBGL,multi_draw_instanced_base_vertex_base_instance}
         *      if the @p instanceOffsets view is not empty.
         * @m_keywords{glMultiDrawArraysInstancedANGLE() glMultiDrawArraysInstancedBaseInstanceANGLE() glMultiDrawElementsInstancedANGLE() glMultiDrawElementsInstancedBaseVertexBaseInstanceANGLE()}
         */
        AbstractShaderProgram& draw(Mesh& mesh, const Containers::StridedArrayView1D<const UnsignedInt>& counts, const Containers::StridedArrayView1D<const UnsignedInt>& instanceCounts, const Containers::StridedArrayView1D<const UnsignedInt>& vertexOffsets, const Containers::StridedArrayView1D<const UnsignedInt>& indexOffsets, const Containers::StridedArrayView1D<const UnsignedInt>& instanceOffsets);

        #ifndef CORRADE_TARGET_32BIT
        /**
         * @brief Draw multiple instanced mesh views with instance offsets at once
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Defined only on @ref CORRADE_TARGET_32BIT "64-bit builds". Compared
         * to @ref draw(Mesh&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&)
         * this overload can avoid allocating an array of 64-bit pointers for
         * the @m_class{m-doc-external} [glMultiDrawElementsInstancedANGLE()](https://chromium.googlesource.com/angle/angle/+/refs/heads/main/extensions/ANGLE_multi_draw.txt)
         * / @m_class{m-doc-external} [glMultiDrawElementsInstancedBaseVertexBaseInstanceANGLE()](https://chromium.googlesource.com/angle/angle/+/refs/heads/main/extensions/ANGLE_base_vertex_base_instance.txt)
         * function and can instead directly reuse the @p indexOffsets view if
         * it's contiguous. See the original overload for further information.
         * @requires_gles_only Not available on desktop OpenGL.
         * @requires_gles30 Not defined in OpenGL ES 2.0. Use
         *      @ref draw(Mesh&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&) without the
         *      @p instanceOffsets parameter there instead.
         * @requires_es_extension Extension @m_class{m-doc-external} [ANGLE_multi_draw](https://chromium.googlesource.com/angle/angle/+/master/extensions/ANGLE_multi_draw.txt)
         * @requires_es_extension OpenGL ES 3.0 and extension
         *      @m_class{m-doc-external} [ANGLE_base_vertex_base_instance](https://chromium.googlesource.com/angle/angle/+/refs/heads/main/extensions/ANGLE_base_vertex_base_instance.txt)
         *      if the mesh is indexed and the @p vertexOffsets view is not
         *      empty. The extension says ES 3.1 is required, but since ANGLE
         *      is the only implementer of this extension and exposes only ES
         *      3.0, there's no point in requiring 3.1.
         * @requires_es_extension OpenGL ES 3.0 and extension
         *      @m_class{m-doc-external} [ANGLE_base_vertex_base_instance](https://chromium.googlesource.com/angle/angle/+/refs/heads/main/extensions/ANGLE_base_vertex_base_instance.txt)
         *      if the @p instanceOffsets view is not empty. The extension says
         *      ES 3.1 is required, but since ANGLE is the only implementer of
         *      this extension and exposes only ES 3.0, there's no point in
         *      requiring 3.1.
         * @requires_webgl_extension Extension @webgl_extension{WEBGL,multi_draw}
         * @requires_webgl_extension WebGL 2.0 and extension
         *      @webgl_extension{WEBGL,multi_draw_instanced_base_vertex_base_instance}
         *      if the mesh is indexed and the @p vertexOffsets view is not
         *      empty.
         * @requires_webgl_extension WebGL 2.0 and extension
         *      @webgl_extension{WEBGL,multi_draw_instanced_base_vertex_base_instance}
         *      if the @p instanceOffsets view is not empty.
         */
        AbstractShaderProgram& draw(Mesh& mesh, const Containers::StridedArrayView1D<const UnsignedInt>& counts, const Containers::StridedArrayView1D<const UnsignedInt>& instanceCounts, const Containers::StridedArrayView1D<const UnsignedInt>& vertexOffsets, const Containers::StridedArrayView1D<const UnsignedLong>& indexOffsets, const Containers::StridedArrayView1D<const UnsignedInt>& instanceOffsets);

        /**
         * @overload
         * @m_since_latest
         */
        AbstractShaderProgram& draw(Mesh& mesh, const Containers::StridedArrayView1D<const UnsignedInt>& counts, const Containers::StridedArrayView1D<const UnsignedInt>& instanceCounts, const Containers::StridedArrayView1D<const UnsignedInt>& vertexOffsets, std::nullptr_t, const Containers::StridedArrayView1D<const UnsignedInt>& instanceOffsets);
        #endif
        #endif

        /**
         * @brief Draw multiple instanced mesh views at once
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Compared to @ref draw(Mesh&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&)
         * lacks the last @p instanceOffsets parameter and as such is available
         * also in OpenGL ES 2.0 and WebGL 1.0.
         *
         * If OpenGL ES 3.0, WebGL 2.0, @gl_extension{OES,vertex_array_object}
         * in OpenGL ES 2.0 or @webgl_extension{OES,vertex_array_object} in
         * WebGL 1.0 is available, the associated vertex array object is bound
         * instead of setting up the mesh from scratch.
         *
         * @requires_gles_only Not available on desktop OpenGL.
         * @requires_es_extension Extension @m_class{m-doc-external} [ANGLE_multi_draw](https://chromium.googlesource.com/angle/angle/+/master/extensions/ANGLE_multi_draw.txt)
         * @requires_es_extension OpenGL ES 3.0 and extension
         *      @m_class{m-doc-external} [ANGLE_base_vertex_base_instance](https://chromium.googlesource.com/angle/angle/+/refs/heads/main/extensions/ANGLE_base_vertex_base_instance.txt)
         *      if the mesh is indexed and the @p vertexOffsets view is not
         *      empty. The extension says ES 3.1 is required, but since ANGLE
         *      is the only implementer of this extension and exposes just ES
         *      3.0, there's no point in requiring 3.1.
         * @requires_webgl_extension Extension @webgl_extension{WEBGL,multi_draw}
         * @requires_webgl_extension WebGL 2.0 and extension
         *      @webgl_extension{WEBGL,multi_draw_instanced_base_vertex_base_instance}
         *      if the mesh is indexed and the @p vertexOffsets view is not
         *      empty.
         */
        AbstractShaderProgram& draw(Mesh& mesh, const Containers::StridedArrayView1D<const UnsignedInt>& counts, const Containers::StridedArrayView1D<const UnsignedInt>& instanceCounts, const Containers::StridedArrayView1D<const UnsignedInt>& vertexOffsets, const Containers::StridedArrayView1D<const UnsignedInt>& indexOffsets);

        #ifndef CORRADE_TARGET_32BIT
        /**
         * @brief Draw multiple instanced mesh views at once
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Defined only on @ref CORRADE_TARGET_32BIT "64-bit builds". Compared
         * to @ref draw(Mesh&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&)
         * this overload can avoid allocating an array of 64-bit pointers for
         * the @m_class{m-doc-external} [glMultiDrawElementsInstancedANGLE()](https://chromium.googlesource.com/angle/angle/+/refs/heads/main/extensions/ANGLE_multi_draw.txt)
         * / @m_class{m-doc-external} [glMultiDrawElementsInstancedBaseVertexBaseInstanceANGLE()](https://chromium.googlesource.com/angle/angle/+/refs/heads/main/extensions/ANGLE_base_vertex_base_instance.txt)
         * function and can instead directly reuse the @p indexOffsets view if
         * it's contiguous. See the original overload for further information.
         * @requires_gles_only Not available on desktop OpenGL.
         * @requires_es_extension Extension @m_class{m-doc-external} [ANGLE_multi_draw](https://chromium.googlesource.com/angle/angle/+/master/extensions/ANGLE_multi_draw.txt)
         * @requires_es_extension OpenGL ES 3.0 and extension
         *      @m_class{m-doc-external} [ANGLE_base_vertex_base_instance](https://chromium.googlesource.com/angle/angle/+/refs/heads/main/extensions/ANGLE_base_vertex_base_instance.txt)
         *      if the mesh is indexed and the @p vertexOffsets view is not
         *      empty. The extension says ES 3.1 is required, but since ANGLE
         *      is the only implementer of this extension and exposes just ES
         *      3.0, there's no point in requiring 3.1.
         * @requires_webgl_extension Extension @webgl_extension{WEBGL,multi_draw}
         * @requires_webgl_extension WebGL 2.0 and extension
         *      @webgl_extension{WEBGL,multi_draw_instanced_base_vertex_base_instance}
         *      if the mesh is indexed and the @p vertexOffsets view is not
         *      empty.
         */
        AbstractShaderProgram& draw(Mesh& mesh, const Containers::StridedArrayView1D<const UnsignedInt>& counts, const Containers::StridedArrayView1D<const UnsignedInt>& instanceCounts, const Containers::StridedArrayView1D<const UnsignedInt>& vertexOffsets, const Containers::StridedArrayView1D<const UnsignedLong>& indexOffsets);

        /**
         * @overload
         * @m_since_latest
         */
        AbstractShaderProgram& draw(Mesh& mesh, const Containers::StridedArrayView1D<const UnsignedInt>& counts, const Containers::StridedArrayView1D<const UnsignedInt>& instanceCounts, const Containers::StridedArrayView1D<const UnsignedInt>& vertexOffsets, std::nullptr_t);
        #endif
        #endif

        /**
         * @brief Draw multiple mesh views at once
         * @return Reference to self (for method chaining)
         * @m_since{2020,06}
         *
         * Extracts the vertex/index counts, vertex offsets and index offsets
         * out of the mesh list and then calls
         * @ref draw(Mesh&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&)
         * (or @ref draw(Mesh& mesh, const Containers::StridedArrayView1D<const UnsignedInt>& counts, const Containers::StridedArrayView1D<const UnsignedInt>& vertexOffsets, const Containers::StridedArrayView1D<const UnsignedLong>& indexOffsets) on 64-bit builds).
         *
         * On OpenGL ES, if neither @gl_extension{EXT,multi_draw_arrays} nor
         * @m_class{m-doc-external} [ANGLE_multi_draw](https://chromium.googlesource.com/angle/angle/+/master/extensions/ANGLE_multi_draw.txt)
         * is present, and on WebGL if @webgl_extension{WEBGL,multi_draw} is
         * not present, the functionality is instead emulated using a sequence
         * of @ref draw(MeshView&) calls. Note that
         * @webgl_extension{WEBGL,multi_draw} is only implemented since
         * Emscripten 2.0.0, so it's not even advertised on older versions.
         *
         * @attention All meshes must be views of the same original mesh and
         *      must not be instanced. Instanced multidraw is available only
         *      through @ref draw(Mesh&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const UnsignedInt>&)
         *      which relies on GLES- and WebGL-specific extensions. It exists
         *      mainly in order to provide a fast rendering path on
         *      resource-constrainted systems and as such doesn't have an
         *      overload taking @ref MeshView instances or a fallback path when
         *      the multidraw extensions are not available.
         *
         * @requires_gl32 Extension @gl_extension{ARB,draw_elements_base_vertex}
         *      if the mesh is indexed and @ref MeshView::baseVertex() is not
         *      `0`
         * @requires_es_extension OpenGL ES 3.0 and extension
         *      @gl_extension{OES,draw_elements_base_vertex} or
         *      @gl_extension{EXT,draw_elements_base_vertex} if the mesh is
         *      indexed and @ref MeshView::baseVertex() is not `0`
         * @requires_webgl_extension WebGL 2.0 and extension
         *      @webgl_extension{WEBGL,multi_draw_instanced_base_vertex_base_instance}
         *      if the mesh is indexed and @ref MeshView::baseVertex() is not
         *      `0`
         */
        AbstractShaderProgram& draw(const Containers::Iterable<MeshView>& meshes);

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Draw a mesh with vertices coming out of transform feedback
         * @param mesh      Mesh to draw
         * @param xfb       Transform feedback to use for vertex count
         * @param stream    Transform feedback stream ID
         * @return Reference to self (for method chaining)
         * @m_since{2020,06}
         *
         * Expects that @p mesh is compatible with this shader, is fully set up
         * and that the output buffer(s) from @p xfb are used as vertex buffers
         * in the mesh. If its instance count is @cpp 0 @ce, no draw commands
         * are issued. Everything set by @ref Mesh::setCount(),
         * @ref Mesh::setBaseInstance(), @ref Mesh::setBaseVertex(),
         * @ref Mesh::setIndexOffset() and @ref Mesh::setIndexBuffer() is
         * ignored, the mesh is drawn as non-indexed and the vertex count is
         * taken from the @p xfb object. If @p stream is @cpp 0 @ce, non-stream
         * draw command is used. If @gl_extension{ARB,vertex_array_object}
         * (part of OpenGL 3.0) is available, the associated vertex array
         * object is bound instead of setting up the mesh from scratch.
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
        AbstractShaderProgram& drawTransformFeedback(Mesh& mesh, TransformFeedback& xfb, UnsignedInt stream = 0);

        /**
         * @brief Draw a mesh view with vertices coming out of transform feedback
         * @return Reference to self (for method chaining)
         * @m_since{2020,06}
         *
         * Everything set by @ref MeshView::setCount(),
         * @ref MeshView::setBaseInstance(), @ref MeshView::setBaseVertex(),
         * @ref MeshView::setIndexOffset() and @ref Mesh::setIndexBuffer() is
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
        AbstractShaderProgram& drawTransformFeedback(MeshView& mesh, TransformFeedback& xfb, UnsignedInt stream = 0);
        #endif

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @brief Dispatch compute
         * @param workgroupCount    Workgroup count in given dimension
         * @return Reference to self (for method chaining)
         *
         * Valid only on programs with compute shader attached. If
         * @p workgroupCount is @cpp 0 @ce in any dimension, no compute
         * dispatch commands are issued.
         * @see @fn_gl{DispatchCompute}
         * @requires_gl43 Extension @gl_extension{ARB,compute_shader}
         * @requires_gles31 Compute shaders are not available in OpenGL ES 3.0
         *      and older.
         * @requires_gles Compute shaders are not available in WebGL.
         */
        AbstractShaderProgram& dispatchCompute(const Vector3ui& workgroupCount);
        #endif

        /**
         * @brief Whether a @ref submitLink() operation has finished
         * @m_since_latest
         *
         * Has to be called only if @ref submitLink() was called before, and
         * before @ref checkLink(). If returns @cpp false @ce, a subsequent
         * @ref checkLink() call will block until the linking is finished. If
         * @gl_extension{KHR,parallel_shader_compile} is not available, the
         * function always returns @cpp true @ce --- i.e., as if the linking
         * was done synchronously. See @ref GL-AbstractShaderProgram-async for
         * more information.
         * @see @ref Shader::isCompileFinished(),
         *      @fn_gl_keyword{GetProgram} with
         *      @def_gl_extension{COMPLETION_STATUS,KHR,parallel_shader_compile}
         */
        bool isLinkFinished();

    protected:
        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Link multiple shaders simultaenously
         * @m_deprecated_since_latest Originally meant to batch multiple link
         *      operations together in a way that allowed the driver to perform
         *      the linking in multiple threads. Superseded by @ref submitLink()
         *      and @ref checkLink(), use either those or the zero-argument
         *      @ref link() instead. See @ref GL-AbstractShaderProgram-async
         *      for more information.
         *
         * Calls @ref submitLink() on all shaders first, then @ref checkLink().
         * Returns @cpp false @ce if linking of any shader failed, @cpp true @ce
         * if everything succeeded.
         */
        static CORRADE_DEPRECATED("use either submitLink() and checkLink() or the zero-argument link() instead") bool link(std::initializer_list<Containers::Reference<AbstractShaderProgram>> shaders);
        #endif

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
        void attachShaders(const Containers::Iterable<Shader>& shaders);

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
        void bindAttributeLocation(UnsignedInt location, Containers::StringView name);

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
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
         * @requires_es_extension OpenGL ES 3.0 and extension
         *      @gl_extension{EXT,blend_func_extended}
         * @requires_gles The @webgl_extension{WEBGL,blend_func_extended}
         *      extension doesn't support application-side binding of fragment
         *      data locations
         */
        void bindFragmentDataLocationIndexed(UnsignedInt location, UnsignedInt index, Containers::StringView name);

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
         * @requires_es_extension OpenGL ES 3.0 and extension
         *      @gl_extension{EXT,blend_func_extended}. Alternatively use
         *      explicit location specification in OpenGL ES 3.0 and
         *      `gl_FragData[n]` provided by @gl_extension{NV,draw_buffers} in
         *      OpenGL ES 2.0
         * @requires_gles The @webgl_extension{WEBGL,blend_func_extended}
         *      extension doesn't support application-side binding of fragment
         *      data locations. Alternatively use explicit location
         *      specification in WebGL 2.0 and `gl_FragData[n]` provided by
         *      @webgl_extension{WEBGL,draw_buffers} in WebGL 1.0.
         */
        void bindFragmentDataLocation(UnsignedInt location, Containers::StringView name);
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Specify shader outputs to be recorded in transform feedback
         * @param outputs       Names of output variables
         * @param bufferMode    Buffer mode
         * @m_since_latest
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
        void setTransformFeedbackOutputs(const Containers::StringIterable& outputs, TransformFeedbackBufferMode bufferMode);
        #endif

        /**
         * @brief Link the shader
         *
         * Calls @ref submitLink(), immediately followed by @ref checkLink(),
         * passing back its return value. See documentation of those two
         * functions for details.
         * @see @ref Shader::compile()
         */
        bool link();

        /**
         * @brief Submit the shader for linking
         * @m_since_latest
         *
         * The attached shaders must be at least submitted for compilation
         * with @ref Shader::submitCompile() or @ref Shader::compile() before
         * linking. Call @ref isLinkFinished() or @ref checkLink() after, see
         * @ref GL-AbstractShaderProgram-async for more information.
         * @see @fn_gl_keyword{LinkProgram}
         */
        void submitLink();

        /**
         * @brief Check shader linking status and await completion
         * @m_since_latest
         *
         * Has to be called only if @ref submitLink() was called before.
         *
         * If @p shaders are not empty, first calls @ref Shader::checkCompile()
         * on each. If a compilation failure is reached, returns @cpp false @ce
         * without even checking link status. To have error messages with full
         * context in case of a failed shader compilation or linking, an
         * application is encouraged to pass all input @ref Shader instances to
         * this function or, if not possible, explicitly call
         * @ref Shader::checkCompile() on each.
         *
         * Then, link status is checked and a message (if any) is printed
         * Returns @cpp false @ce if linking failed, @cpp true @ce on success.
         * If linking failed, it first goes through @p shaders and calls
         * @ref Shader::checkCompile() on each until a failure is reached. If
         * no compilation failed, a linker message is printed to error output.
         * The function will stall until a (potentially async) linking
         * operation finishes, you can use @ref isLinkFinished() to check the
         * status instead. See @ref GL-AbstractShaderProgram-async for more
         * information.
         * @see @ref Shader::checkCompile(), @fn_gl_keyword{GetProgram} with
         *      @def_gl{LINK_STATUS} and @def_gl{INFO_LOG_LENGTH},
         *      @fn_gl_keyword{GetProgramInfoLog}
         */
        /* No default argument is provided in order to *really* encourage apps
           to pass the shaders here */
        bool checkLink(const Containers::Iterable<Shader>& shaders);

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
        Int uniformLocation(Containers::StringView name);

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
        UnsignedInt uniformBlockIndex(Containers::StringView name);
        #endif

        /**
         * @brief Set a float uniform value
         * @param location      Uniform location
         * @param value         Value
         *
         * If neither @gl_extension{ARB,separate_shader_objects} (part of
         * OpenGL 4.1) nor @gl_extension{EXT,separate_shader_objects} OpenGL ES
         * extension nor OpenGL ES 3.1 is available, the shader is marked for
         * use before the operation.
         * @see @ref setUniform(Int, Containers::ArrayView<const Float>),
         *      @ref uniformLocation(), @fn_gl{UseProgram},
         *      @fn_gl_keyword{Uniform} or @fn_gl_keyword{ProgramUniform}
         */
        void setUniform(Int location, Float value);
        void setUniform(Int location, const Math::Vector<2, Float>& value); /**< @overload */
        void setUniform(Int location, const Math::Vector<3, Float>& value); /**< @overload */
        void setUniform(Int location, const Math::Vector<4, Float>& value); /**< @overload */

        /**
         * @brief Set an integer uniform value
         * @param location      Uniform location
         * @param value         Value
         *
         * If neither @gl_extension{ARB,separate_shader_objects} (part of
         * OpenGL 4.1) nor @gl_extension{EXT,separate_shader_objects} OpenGL ES
         * extension nor OpenGL ES 3.1 is available, the shader is marked for
         * use before the operation.
         * @see @ref setUniform(Int, Containers::ArrayView<const Int>),
         *      @ref uniformLocation(), @fn_gl{UseProgram},
         *      @fn_gl_keyword{Uniform} or @fn_gl_keyword{ProgramUniform}
         */
        void setUniform(Int location, Int value); /**< @overload */
        void setUniform(Int location, const Math::Vector<2, Int>& value); /**< @overload */
        void setUniform(Int location, const Math::Vector<3, Int>& value); /**< @overload */
        void setUniform(Int location, const Math::Vector<4, Int>& value); /**< @overload */

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Set an unsigned integer uniform value
         * @param location      Uniform location
         * @param value         Value
         *
         * If neither @gl_extension{ARB,separate_shader_objects} (part of
         * OpenGL 4.1) nor @gl_extension{EXT,separate_shader_objects} OpenGL ES
         * extension nor OpenGL ES 3.1 is available, the shader is marked for
         * use before the operation.
         * @see @ref setUniform(Int, Containers::ArrayView<const UnsignedInt>),
         *      @ref uniformLocation(), @fn_gl{UseProgram},
         *      @fn_gl_keyword{Uniform} or @fn_gl_keyword{ProgramUniform}
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Only signed integers are available in OpenGL ES 2.0.
         * @requires_webgl20 Only signed integers are available in WebGL 1.0.
         */
        void setUniform(Int location, UnsignedInt value);
        void setUniform(Int location, const Math::Vector<2, UnsignedInt>& value); /**< @overload */
        void setUniform(Int location, const Math::Vector<3, UnsignedInt>& value); /**< @overload */
        void setUniform(Int location, const Math::Vector<4, UnsignedInt>& value); /**< @overload */
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Set a double uniform value
         * @param location      Uniform location
         * @param value         Value
         *
         * If neither @gl_extension{ARB,separate_shader_objects} (part of
         * OpenGL 4.1) nor @gl_extension{EXT,separate_shader_objects} OpenGL ES
         * extension nor OpenGL ES 3.1 is available, the shader is marked for
         * use before the operation.
         * @see @ref setUniform(Int, Containers::ArrayView<const Double>),
         *      @ref uniformLocation(), @fn_gl{UseProgram},
         *      @fn_gl_keyword{Uniform} or @fn_gl_keyword{ProgramUniform}
         * @requires_gl40 Extension @gl_extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES or WebGL.
         */
        void setUniform(Int location, Double value);
        void setUniform(Int location, const Math::Vector<2, Double>& value); /**< @overload */
        void setUniform(Int location, const Math::Vector<3, Double>& value); /**< @overload */
        void setUniform(Int location, const Math::Vector<4, Double>& value); /**< @overload */
        #endif

        /**
         * @brief Set float uniform values
         * @param location      Uniform location
         * @param values        Values
         *
         * If neither @gl_extension{ARB,separate_shader_objects} (part of
         * OpenGL 4.1) nor @gl_extension{EXT,separate_shader_objects} OpenGL ES
         * extension nor OpenGL ES 3.1 is available, the shader is marked for
         * use before the operation.
         * @see @ref setUniform(Int, Float), @ref uniformLocation(),
         *      @fn_gl{UseProgram}, @fn_gl_keyword{Uniform} or
         *      @fn_gl_keyword{ProgramUniform}
         */
        void setUniform(Int location, Containers::ArrayView<const Float> values);
        void setUniform(Int location, Containers::ArrayView<const Math::Vector<2, Float>> values); /**< @overload */
        void setUniform(Int location, Containers::ArrayView<const Math::Vector<3, Float>> values); /**< @overload */
        void setUniform(Int location, Containers::ArrayView<const Math::Vector<4, Float>> values); /**< @overload */

        /**
         * @brief Set integer uniform values
         * @param location      Uniform location
         * @param values        Values
         *
         * If neither @gl_extension{ARB,separate_shader_objects} (part of
         * OpenGL 4.1) nor @gl_extension{EXT,separate_shader_objects} OpenGL ES
         * extension nor OpenGL ES 3.1 is available, the shader is marked for
         * use before the operation.
         * @see @ref setUniform(Int, Int), @ref uniformLocation(),
         *      @fn_gl{UseProgram}, @fn_gl_keyword{Uniform} or
         *      @fn_gl_keyword{ProgramUniform}
         */
        void setUniform(Int location, Containers::ArrayView<const Int> values);
        void setUniform(Int location, Containers::ArrayView<const Math::Vector<2, Int>> values); /**< @overload */
        void setUniform(Int location, Containers::ArrayView<const Math::Vector<3, Int>> values); /**< @overload */
        void setUniform(Int location, Containers::ArrayView<const Math::Vector<4, Int>> values); /**< @overload */

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Set unsigned integer uniform values
         * @param location      Uniform location
         * @param values        Values
         *
         * If neither @gl_extension{ARB,separate_shader_objects} (part of
         * OpenGL 4.1) nor @gl_extension{EXT,separate_shader_objects} OpenGL ES
         * extension nor OpenGL ES 3.1 is available, the shader is marked for
         * use before the operation.
         * @see @ref setUniform(Int, UnsignedInt), @ref uniformLocation(),
         *      @fn_gl{UseProgram}, @fn_gl_keyword{Uniform} or
         *      @fn_gl_keyword{ProgramUniform}
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
         * @brief Set double uniform values
         * @param location      Uniform location
         * @param values        Values
         *
         * If neither @gl_extension{ARB,separate_shader_objects} (part of
         * OpenGL 4.1) nor @gl_extension{EXT,separate_shader_objects} OpenGL ES
         * extension nor OpenGL ES 3.1 is available, the shader is marked for
         * use before the operation.
         * @see @ref setUniform(Int, Double), @ref uniformLocation(),
         *      @fn_gl{UseProgram}, @fn_gl_keyword{Uniform} or
         *      @fn_gl_keyword{ProgramUniform}
         * @requires_gl40 Extension @gl_extension{ARB,gpu_shader_fp64}
         * @requires_gl Only floats are available in OpenGL ES or WebGL.
         */
        void setUniform(Int location, Containers::ArrayView<const Double> values);
        void setUniform(Int location, Containers::ArrayView<const Math::Vector<2, Double>> values); /**< @overload */
        void setUniform(Int location, Containers::ArrayView<const Math::Vector<3, Double>> values); /**< @overload */
        void setUniform(Int location, Containers::ArrayView<const Math::Vector<4, Double>> values); /**< @overload */
        #endif

        /**
         * @brief Set a float matrix uniform value
         * @param location      Uniform location
         * @param value         Value
         *
         * If neither @gl_extension{ARB,separate_shader_objects} (part of
         * OpenGL 4.1) nor @gl_extension{EXT,separate_shader_objects} OpenGL ES
         * extension nor OpenGL ES 3.1 is available, the shader is marked for
         * use before the operation.
         * @see @ref setUniform(Int, Float), @ref uniformLocation(),
         *      @fn_gl{UseProgram}, @fn_gl_keyword{Uniform} or
         *      @fn_gl_keyword{ProgramUniform}
         * @requires_gles30 Only square matrices are available in OpenGL ES 2.0.
         * @requires_webgl20 Only square matrices are available in WebGL 1.0.
         * @requires_gl40 Extension @gl_extension{ARB,gpu_shader_fp64} for
         *      doubles.
         * @requires_gl Only floats are available in OpenGL ES or WebGL.
         */
        template<std::size_t cols, std::size_t rows, class T> void setUniform(Int location, const Math::RectangularMatrix<cols, rows, T>& value) {
            setUniform(location, {&value, 1});
        }

        /**
         * @brief Set float matrix uniform values
         * @param location      Uniform location
         * @param values        Values
         *
         * If neither @gl_extension{ARB,separate_shader_objects} (part of
         * OpenGL 4.1) nor @gl_extension{EXT,separate_shader_objects} OpenGL ES
         * extension nor OpenGL ES 3.1 is available, the shader is marked for
         * use before the operation.
         * @see @ref setUniform(Int, Float), @ref uniformLocation(),
         *      @fn_gl{UseProgram}, @fn_gl_keyword{Uniform} or
         *      @fn_gl_keyword{ProgramUniform}
         */
        void setUniform(Int location, Containers::ArrayView<const Math::RectangularMatrix<2, 2, Float>> values);
        void setUniform(Int location, Containers::ArrayView<const Math::RectangularMatrix<3, 3, Float>> values); /**< @overload */
        void setUniform(Int location, Containers::ArrayView<const Math::RectangularMatrix<4, 4, Float>> values); /**< @overload */

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Set rectangular float matrix uniform values
         * @param location      Uniform location
         * @param values        Values
         *
         * If neither @gl_extension{ARB,separate_shader_objects} (part of
         * OpenGL 4.1) nor @gl_extension{EXT,separate_shader_objects} OpenGL ES
         * extension nor OpenGL ES 3.1 is available, the shader is marked for
         * use before the operation.
         * @see @ref setUniform(Int, Float), @ref uniformLocation(),
         *      @fn_gl{UseProgram}, @fn_gl_keyword{Uniform} or
         *      @fn_gl_keyword{ProgramUniform}
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
         * @brief Set double matrix uniform values
         * @param location      Uniform location
         * @param values        Values
         *
         * If neither @gl_extension{ARB,separate_shader_objects} (part of
         * OpenGL 4.1) nor @gl_extension{EXT,separate_shader_objects} OpenGL ES
         * extension nor OpenGL ES 3.1 is available, the shader is marked for
         * use before the operation.
         * @see @ref setUniform(Int, Float), @ref uniformLocation(),
         *      @fn_gl{UseProgram}, @fn_gl_keyword{Uniform} or
         *      @fn_gl_keyword{ProgramUniform}
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
        #ifndef MAGNUM_TARGET_GLES2
        static void MAGNUM_GL_LOCAL transformFeedbackVaryingsImplementationDefault(AbstractShaderProgram& self, const Containers::StringIterable& outputs, TransformFeedbackBufferMode bufferMode);
        #ifdef CORRADE_TARGET_WINDOWS
        /* See the "nv-windows-dangling-transform-feedback-varying-names"
           workaround */
        static void MAGNUM_GL_LOCAL transformFeedbackVaryingsImplementationDanglingWorkaround(AbstractShaderProgram& self, const Containers::StringIterable& outputs, TransformFeedbackBufferMode bufferMode);
        #endif
        #endif

        static MAGNUM_GL_LOCAL void cleanLogImplementationNoOp(Containers::String& message);
        #if defined(CORRADE_TARGET_WINDOWS) && !defined(MAGNUM_TARGET_GLES)
        static MAGNUM_GL_LOCAL void cleanLogImplementationIntelWindows(Containers::String& message);
        #endif
        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
        static MAGNUM_GL_LOCAL void cleanLogImplementationAngle(Containers::String& message);
        #endif

        MAGNUM_GL_LOCAL static void APIENTRY completionStatusImplementationFallback(GLuint, GLenum, GLint*);

        MAGNUM_GL_LOCAL static void use(GLuint id);
        void use();

        /* To avoid pointless extra function pointer indirections and copypaste
           for all suffixed/unsuffixed variants, these are all static with a
           signature matching the DSA APIs. On DSA-enabled platforms the
           glProgramUniform*() functions are used directly, otherwise these all
           use() the shader first and then call the old-style API. */
        MAGNUM_GL_LOCAL static void APIENTRY uniform1fvImplementationDefault(GLuint id, GLint location, GLsizei count, const GLfloat* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniform2fvImplementationDefault(GLuint id, GLint location, GLsizei count, const GLfloat* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniform3fvImplementationDefault(GLuint id, GLint location, GLsizei count, const GLfloat* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniform4fvImplementationDefault(GLuint id, GLint location, GLsizei count, const GLfloat* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniform1fImplementationDefault(GLuint id, GLint location, GLfloat v0);
        MAGNUM_GL_LOCAL static void APIENTRY uniform2fImplementationDefault(GLuint id, GLint location, GLfloat v0, GLfloat v1);
        MAGNUM_GL_LOCAL static void APIENTRY uniform3fImplementationDefault(GLuint id, GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
        MAGNUM_GL_LOCAL static void APIENTRY uniform4fImplementationDefault(GLuint id, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
        MAGNUM_GL_LOCAL static void APIENTRY uniform1ivImplementationDefault(GLuint id, GLint location, GLsizei count, const GLint* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniform2ivImplementationDefault(GLuint id, GLint location, GLsizei count, const GLint* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniform3ivImplementationDefault(GLuint id, GLint location, GLsizei count, const GLint* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniform4ivImplementationDefault(GLuint id, GLint location, GLsizei count, const GLint* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniform1iImplementationDefault(GLuint id, GLint location, GLint v0);
        MAGNUM_GL_LOCAL static void APIENTRY uniform2iImplementationDefault(GLuint id, GLint location, GLint v0, GLint v1);
        MAGNUM_GL_LOCAL static void APIENTRY uniform3iImplementationDefault(GLuint id, GLint location, GLint v0, GLint v1, GLint v2);
        MAGNUM_GL_LOCAL static void APIENTRY uniform4iImplementationDefault(GLuint id, GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
        #ifndef MAGNUM_TARGET_GLES2
        MAGNUM_GL_LOCAL static void APIENTRY uniform1uivImplementationDefault(GLuint id, GLint location, GLsizei count, const GLuint* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniform2uivImplementationDefault(GLuint id, GLint location, GLsizei count, const GLuint* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniform3uivImplementationDefault(GLuint id, GLint location, GLsizei count, const GLuint* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniform4uivImplementationDefault(GLuint id, GLint location, GLsizei count, const GLuint* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniform1uiImplementationDefault(GLuint id, GLint location, GLuint v0);
        MAGNUM_GL_LOCAL static void APIENTRY uniform2uiImplementationDefault(GLuint id, GLint location, GLuint v0, GLuint v1);
        MAGNUM_GL_LOCAL static void APIENTRY uniform3uiImplementationDefault(GLuint id, GLint location, GLuint v0, GLuint v1, GLuint v2);
        MAGNUM_GL_LOCAL static void APIENTRY uniform4uiImplementationDefault(GLuint id, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        MAGNUM_GL_LOCAL static void APIENTRY uniform1dvImplementationDefault(GLuint id, GLint location, GLsizei count, const GLdouble* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniform2dvImplementationDefault(GLuint id, GLint location, GLsizei count, const GLdouble* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniform3dvImplementationDefault(GLuint id, GLint location, GLsizei count, const GLdouble* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniform4dvImplementationDefault(GLuint id, GLint location, GLsizei count, const GLdouble* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniform1dImplementationDefault(GLuint id, GLint location, GLdouble v0);
        MAGNUM_GL_LOCAL static void APIENTRY uniform2dImplementationDefault(GLuint id, GLint location, GLdouble v0, GLdouble v1);
        MAGNUM_GL_LOCAL static void APIENTRY uniform3dImplementationDefault(GLuint id, GLint location, GLdouble v0, GLdouble v1, GLdouble v2);
        MAGNUM_GL_LOCAL static void APIENTRY uniform4dImplementationDefault(GLuint id, GLint location, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3);
        #endif

        MAGNUM_GL_LOCAL static void APIENTRY uniformMatrix2fvImplementationDefault(GLuint id, GLint location, GLsizei count, GLboolean transpose, const GLfloat* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniformMatrix3fvImplementationDefault(GLuint id, GLint location, GLsizei count, GLboolean transpose, const GLfloat* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniformMatrix4fvImplementationDefault(GLuint id, GLint location, GLsizei count, GLboolean transpose, const GLfloat* values);
        #ifndef MAGNUM_TARGET_GLES2
        MAGNUM_GL_LOCAL static void APIENTRY uniformMatrix2x3fvImplementationDefault(GLuint id, GLint location, GLsizei count, GLboolean transpose, const GLfloat* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniformMatrix3x2fvImplementationDefault(GLuint id, GLint location, GLsizei count, GLboolean transpose, const GLfloat* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniformMatrix2x4fvImplementationDefault(GLuint id, GLint location, GLsizei count, GLboolean transpose, const GLfloat* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniformMatrix4x2fvImplementationDefault(GLuint id, GLint location, GLsizei count, GLboolean transpose, const GLfloat* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniformMatrix3x4fvImplementationDefault(GLuint id, GLint location, GLsizei count, GLboolean transpose, const GLfloat* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniformMatrix4x3fvImplementationDefault(GLuint id, GLint location, GLsizei count, GLboolean transpose, const GLfloat* values);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        MAGNUM_GL_LOCAL static void APIENTRY uniformMatrix2dvImplementationDefault(GLuint id, GLint location, GLsizei count, GLboolean transpose, const GLdouble* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniformMatrix3dvImplementationDefault(GLuint id, GLint location, GLsizei count, GLboolean transpose, const GLdouble* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniformMatrix4dvImplementationDefault(GLuint id, GLint location, GLsizei count, GLboolean transpose, const GLdouble* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniformMatrix2x3dvImplementationDefault(GLuint id, GLint location, GLsizei count, GLboolean transpose, const GLdouble* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniformMatrix3x2dvImplementationDefault(GLuint id, GLint location, GLsizei count, GLboolean transpose, const GLdouble* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniformMatrix2x4dvImplementationDefault(GLuint id, GLint location, GLsizei count, GLboolean transpose, const GLdouble* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniformMatrix4x2dvImplementationDefault(GLuint id, GLint location, GLsizei count, GLboolean transpose, const GLdouble* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniformMatrix3x4dvImplementationDefault(GLuint id, GLint location, GLsizei count, GLboolean transpose, const GLdouble* values);
        MAGNUM_GL_LOCAL static void APIENTRY uniformMatrix4x3dvImplementationDefault(GLuint id, GLint location, GLsizei count, GLboolean transpose, const GLdouble* values);
        #endif

        GLuint _id;

        #if defined(CORRADE_TARGET_WINDOWS) && !defined(MAGNUM_TARGET_GLES2)
        /* Needed for the nv-windows-dangling-transform-feedback-varying-names
           workaround */
        Containers::ArrayTuple _transformFeedbackVaryingNames;
        #endif
};

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef CORRADE_TARGET_32BIT
#define _MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION_NOT_32BIT(...) \
    __VA_ARGS__& draw(Magnum::GL::Mesh& mesh, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& counts, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& vertexOffsets, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedLong>& indexOffsets) { \
        return static_cast<__VA_ARGS__&>(Magnum::GL::AbstractShaderProgram::draw(mesh, counts, vertexOffsets, indexOffsets)); \
    }                                                                       \
    __VA_ARGS__& draw(Magnum::GL::Mesh& mesh, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& counts, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& vertexOffsets, std::nullptr_t) { \
        return static_cast<__VA_ARGS__&>(Magnum::GL::AbstractShaderProgram::draw(mesh, counts, vertexOffsets, nullptr)); \
    }
#else
#define _MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION_NOT_32BIT(...)
#endif
#ifdef MAGNUM_TARGET_GLES
#ifndef MAGNUM_TARGET_GLES2
#ifndef CORRADE_TARGET_32BIT
#define _MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION_GLES_NOT_GLES2_NOT_32BIT(...) \
    __VA_ARGS__& draw(Magnum::GL::Mesh& mesh, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& counts, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& instanceCounts, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& vertexOffsets, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedLong>& indexOffsets, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& instanceOffsets) { \
        return static_cast<__VA_ARGS__&>(Magnum::GL::AbstractShaderProgram::draw(mesh, counts, instanceCounts, vertexOffsets, indexOffsets, instanceOffsets)); \
    }                                                                       \
    __VA_ARGS__& draw(Magnum::GL::Mesh& mesh, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& counts, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& instanceCounts, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& vertexOffsets, std::nullptr_t, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& instanceOffsets) { \
        return static_cast<__VA_ARGS__&>(Magnum::GL::AbstractShaderProgram::draw(mesh, counts, instanceCounts, vertexOffsets, nullptr, instanceOffsets)); \
    }
#else
#define _MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION_GLES_NOT_GLES2_NOT_32BIT(...)
#endif
#define _MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION_GLES_NOT_GLES2(...) \
    __VA_ARGS__& draw(Magnum::GL::Mesh& mesh, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& counts, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& instanceCounts, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& vertexOffsets, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& indexOffsets, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& instanceOffsets) { \
            return static_cast<__VA_ARGS__&>(Magnum::GL::AbstractShaderProgram::draw(mesh, counts, instanceCounts, vertexOffsets, indexOffsets, instanceOffsets)); \
        }                                                                   \
    _MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION_GLES_NOT_GLES2_NOT_32BIT(__VA_ARGS__)
#else
#define _MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION_GLES_NOT_GLES2(...)
#endif
    #ifndef CORRADE_TARGET_32BIT
#define _MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION_GLES_NOT_32BIT(...) \
    __VA_ARGS__& draw(Magnum::GL::Mesh& mesh, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& counts, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& instanceCounts, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& vertexOffsets, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedLong>& indexOffsets) { \
        return static_cast<__VA_ARGS__&>(Magnum::GL::AbstractShaderProgram::draw(mesh, counts, instanceCounts, vertexOffsets, indexOffsets)); \
    }                                                                       \
    __VA_ARGS__& draw(Magnum::GL::Mesh& mesh, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& counts, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& instanceCounts, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& vertexOffsets, std::nullptr_t) { \
        return static_cast<__VA_ARGS__&>(Magnum::GL::AbstractShaderProgram::draw(mesh, counts, instanceCounts, vertexOffsets, nullptr)); \
    }
#else
#define _MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION_GLES_NOT_32BIT(...)
#endif
#define _MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION_GLES(...) \
    _MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION_GLES_NOT_GLES2(__VA_ARGS__) \
    _MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION_GLES_NOT_32BIT(__VA_ARGS__) \
    __VA_ARGS__& draw(Magnum::GL::Mesh& mesh, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& counts, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& instanceCounts, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& vertexOffsets, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& indexOffsets) { \
        return static_cast<__VA_ARGS__&>(Magnum::GL::AbstractShaderProgram::draw(mesh, counts, instanceCounts, vertexOffsets, indexOffsets)); \
    }
#define _MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION_NOT_GLES(...)
#else
#define _MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION_GLES(...)
#define _MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION_NOT_GLES(...) \
    __VA_ARGS__& drawTransformFeedback(Magnum::GL::Mesh& mesh, Magnum::GL::TransformFeedback& xfb, Magnum::UnsignedInt stream = 0) { \
        return static_cast<__VA_ARGS__&>(Magnum::GL::AbstractShaderProgram::drawTransformFeedback(mesh, xfb, stream)); \
    }                                                                       \
    __VA_ARGS__& drawTransformFeedback(Magnum::GL::MeshView& mesh, Magnum::GL::TransformFeedback& xfb, Magnum::UnsignedInt stream = 0) { \
        return static_cast<__VA_ARGS__&>(Magnum::GL::AbstractShaderProgram::drawTransformFeedback(mesh, xfb, stream)); \
    }
#endif
#ifndef MAGNUM_TARGET_GLES
#define _MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION_HIDE_XFB \
    using Magnum::GL::AbstractShaderProgram::drawTransformFeedback;
#else
#define _MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION_HIDE_XFB
#endif
#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
#define _MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION_HIDE_COMPUTE \
    using Magnum::GL::AbstractShaderProgram::dispatchCompute;
#else
#define _MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION_HIDE_COMPUTE
#endif
#endif

/**
@brief @relativeref{Magnum::GL,AbstractShaderProgram} subclass method chaining implementation for draws
@m_since_latest

Overrides all variants of @relativeref{Magnum::GL,AbstractShaderProgram::draw()}
and @relativeref{Magnum::GL::AbstractShaderProgram,drawTransformFeedback()} in
given subclass to return a reference to that class type instead of
@relativeref{Magnum::GL,AbstractShaderProgram}, and additionally prevents
accidental calls to
@relativeref{Magnum::GL,AbstractShaderProgram::dispatchCompute()}. See
@ref GL-AbstractShaderProgram-subclassing for more information.
*/
#define MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION(...)   \
    private:                                                                \
        _MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION_HIDE_COMPUTE \
    public:                                                                 \
        __VA_ARGS__& draw(Magnum::GL::Mesh& mesh) {                         \
            return static_cast<__VA_ARGS__&>(Magnum::GL::AbstractShaderProgram::draw(mesh)); \
        }                                                                   \
        __VA_ARGS__& draw(Magnum::GL::Mesh&& mesh) {                        \
            return static_cast<__VA_ARGS__&>(Magnum::GL::AbstractShaderProgram::draw(mesh)); \
        }                                                                   \
        __VA_ARGS__& draw(Magnum::GL::MeshView& mesh) {                     \
            return static_cast<__VA_ARGS__&>(Magnum::GL::AbstractShaderProgram::draw(mesh)); \
        }                                                                   \
        __VA_ARGS__& draw(Magnum::GL::MeshView&& mesh) {                    \
            return static_cast<__VA_ARGS__&>(Magnum::GL::AbstractShaderProgram::draw(mesh));  \
        }                                                                   \
        __VA_ARGS__& draw(Magnum::GL::Mesh& mesh, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& counts, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& vertexOffsets, const Corrade::Containers::StridedArrayView1D<const Magnum::UnsignedInt>& indexOffsets) { \
            return static_cast<__VA_ARGS__&>(Magnum::GL::AbstractShaderProgram::draw(mesh, counts, vertexOffsets, indexOffsets)); \
        }                                                                   \
        _MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION_NOT_32BIT(__VA_ARGS__) \
        _MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION_GLES(__VA_ARGS__) \
        __VA_ARGS__& draw(const Corrade::Containers::Iterable<Magnum::GL::MeshView>& meshes) { \
            return static_cast<__VA_ARGS__&>(Magnum::GL::AbstractShaderProgram::draw(meshes)); \
        }                                                                   \
        _MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION_NOT_GLES(__VA_ARGS__)

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
/**
@brief @relativeref{Magnum::GL,AbstractShaderProgram} subclass method chaining implementation for compute dispatch
@m_since_latest

Overrides @relativeref{Magnum::GL,AbstractShaderProgram::dispatchCompute()} in
given subclass to return a reference to that class type instead of
@relativeref{Magnum::GL,AbstractShaderProgram}, and additionally prevents
accidental calls to @relativeref{Magnum::GL,AbstractShaderProgram::draw()} and
@relativeref{Magnum::GL::AbstractShaderProgram,drawTransformFeedback()}. See
@ref GL-AbstractShaderProgram-subclassing for more information.
@requires_gles31 Not defined on OpenGL ES 2.0 builds.
@requires_gles Not defined on WebGL builds.
*/
#define MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DISPATCH_IMPLEMENTATION(...) \
    private:                                                                \
        using Magnum::GL::AbstractShaderProgram::draw;                      \
        _MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION_HIDE_XFB \
    public:                                                                 \
        __VA_ARGS__& dispatchCompute(const Magnum::Vector3ui& workgroupCount) {     \
            return static_cast<__VA_ARGS__&>(Magnum::GL::AbstractShaderProgram::dispatchCompute(workgroupCount)); \
        }
#endif

}}

#endif
