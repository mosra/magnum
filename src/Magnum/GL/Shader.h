#ifndef Magnum_GL_Shader_h
#define Magnum_GL_Shader_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2022 Vladislav Oleshko <vladislav.oleshko@gmail.com>

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
 * @brief Class @ref Magnum::GL::Shader
 */

#include <Corrade/Containers/Array.h>

#include "Magnum/Tags.h"
#include "Magnum/GL/AbstractObject.h"
#include "Magnum/GL/GL.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>
/* For label() / setLabel() and all name/source stuff, which used to be a
   std::string. Not ideal for the return types, but at least something. */
#include <Corrade/Containers/StringStl.h>
#endif

namespace Magnum { namespace GL {

namespace Implementation { struct ShaderState; }

/**
@brief Shader

See @ref AbstractShaderProgram for high-level usage information.

@section GL-Shader-workarounds Driver workarounds

Some driver workarounds used by Magnum affect also shader code, and the class
is implicitly defining the following macros depending on the @ref Version
passed to the constructor:

-   @cpp #define MAGNUM_DISABLE_GL_ARB_explicit_attrib_location @ce if the
    @gl_extension{ARB,explicit_attrib_location} desktop extension is reported
    as supported by the shader compiler but isn't usable on given GLSL version.
    In this case @ref GL::Context::isExtensionSupported(Version) const also
    returns @cpp false @ce for this extension to be able to make appropriate
    adjustments during shader compilation.
-   @cpp #define MAGNUM_DISABLE_GL_ARB_shading_language_420pack @ce if the
    @gl_extension{ARB,shading_language_420pack} desktop extension is reported
    as supported by the shader compiler but isn't usable on given GLSL version.
    In this case @ref GL::Context::isExtensionSupported(Version) const also
    returns @cpp false @ce for this extension to be able to make appropriate
    adjustments during shader compilation.
-   @cpp #define MAGNUM_DISABLE_GL_ARB_explicit_uniform_location @ce if the
    @gl_extension{ARB,explicit_attrib_location} desktop extension is reported
    as supported by the shader compiler but isn't usable on given GLSL version.
    In this case @ref GL::Context::isExtensionSupported(Version) const also
    returns @cpp false @ce for this extension to be able to make appropriate
    adjustments during shader compilation.
-   @cpp #define MAGNUM_DISABLE_GL_MAGNUM_shader_vertex_id @ce if the
    @glsl gl_VertexID @ce builtin should be present on given GLSL version but
    isn't working correctly. You can use the artificial
    `GL_MAGNUM_shader_vertex_id` desktop, ES and WebGL extension to check for
    this case in @ref GL::Context::isExtensionSupported(Version) const.
-   @cpp #define MAGNUM_GLSL_VERSION 310 @ce or
    @cpp #define MAGNUM_GLSL_VERSION 320 @ce if the builtin
    @glsl __VERSION__ @ce macro doesn't have correct value for GLSL ES 3.10 or
    3.20

See @ref opengl-workarounds for concrete information about driver workarounds
used. If @ref Version::None is passed to the constructor, none of the above
defines are added.

@section GL-Shader-errors Compilation error reporting

To help localize shader compilation errors, each @ref addSource() /
@ref addFile() call precedes the source with a @glsl #line n 1 @ce directive,
where `n` is the source number. Drivers then use the source number to print
error location, usually in a form `<file>(<line>):` or `<file>:<line>:`.

@attention Especially the latter form may often get confused with the usual
    `<line>:<column>:` used by C compilers --- be aware of the difference.
    Unfortunately GLSL only allows specifying a file *number*, not a name.

Source number `0` is a @glsl #version @ce directive added in the constructor
together with potential workaround defines listed above, which means the first
added source has a number `1`. If @ref Version::None is specified, the first
added source has a number `0` instead.

@section GL-Shader-performance-optimizations Performance optimizations

Shader limits and implementation-defined values (such as @ref maxUniformComponents())
are cached, so repeated queries don't result in repeated @fn_gl{Get} calls.
 */
class MAGNUM_GL_EXPORT Shader: public AbstractObject {
    friend Implementation::ShaderState;

    public:
        /**
         * @brief Shader type
         *
         * @see @ref Shader(Version, Type),
         *      @ref maxAtomicCounterBuffers(),
         *      @ref maxAtomicCounters(),
         *      @ref maxImageUniforms()
         *      @ref maxShaderStorageBlocks(),
         *      @ref maxTextureImageUnits(),
         *      @ref maxUniformBlocks(),
         *      @ref maxUniformComponents(),
         *      @ref maxCombinedUniformComponents()
         * @m_enum_values_as_keywords
         */
        enum class Type: GLenum {
            Vertex = GL_VERTEX_SHADER,      /**< Vertex shader */

            #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
            /**
             * Tessellation control shader
             * @requires_gl40 Extension @gl_extension{ARB,tessellation_shader}
             * @requires_gles30 Not defined in OpenGL ES 2.0.
             * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
             *      @gl_extension{EXT,tessellation_shader}
             * @requires_gles Tessellation shaders are not available in WebGL.
             */
            TessellationControl = GL_TESS_CONTROL_SHADER,

            /**
             * Tessellation evaluation shader
             * @requires_gl40 Extension @gl_extension{ARB,tessellation_shader}
             * @requires_gles30 Not defined in OpenGL ES 2.0.
             * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
             *      @gl_extension{EXT,tessellation_shader}
             * @requires_gles Tessellation shaders are not available in WebGL.
             */
            TessellationEvaluation = GL_TESS_EVALUATION_SHADER,

            /**
             * Geometry shader
             * @requires_gl32 Extension @gl_extension{ARB,geometry_shader4}
             * @requires_gles30 Not defined in OpenGL ES 2.0.
             * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
             *      @gl_extension{EXT,geometry_shader}
             * @requires_gles Geometry shaders are not available in WebGL.
             */
            Geometry = GL_GEOMETRY_SHADER,

            /**
             * Compute shader
             * @requires_gl43 Extension @gl_extension{ARB,compute_shader}
             * @requires_gles31 Compute shaders are not available in OpenGL ES
             *      3.0 and older.
             * @requires_gles Compute shaders are not available in WebGL.
             */
            Compute = GL_COMPUTE_SHADER,
            #endif

            Fragment = GL_FRAGMENT_SHADER   /**< Fragment shader */
        };

        /**
         * @brief Max supported component count on vertex shader output
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. In OpenGL ES 2.0 the four-component vector count is
         * queried and multiplied with 4.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_VERTEX_OUTPUT_COMPONENTS},
         *      @def_gl_keyword{MAX_VARYING_COMPONENTS} in OpenGL <3.2 or
         *      @def_gl_keyword{MAX_VARYING_VECTORS} in OpenGL ES 2.0
         */
        static Int maxVertexOutputComponents();

        /** @todo `GL_MAX_PATCH_VERTICES`, `GL_MAX_TESS_GEN_LEVEL`, `GL_MAX_TESS_PATCH_COMPONENTS` when @gl_extension{ARB,tessellation_shader} is done */

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @brief Max supported component count of tessellation control shader input vertex
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither @gl_extension{ARB,tessellation_shader} (part
         * of OpenGL 4.0) nor @gl_extension{ANDROID,extension_pack_es31a} /
         * @gl_extension{EXT,tessellation_shader} ES extension is available,
         * returns @cpp 0 @ce.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_TESS_CONTROL_INPUT_COMPONENTS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Tessellation shaders are not available in WebGL.
         */
        static Int maxTessellationControlInputComponents();

        /**
         * @brief Max supported component count of tessellation control shader output vertex
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither @gl_extension{ARB,tessellation_shader} (part
         * of OpenGL 4.0) nor @gl_extension{ANDROID,extension_pack_es31a} /
         * @gl_extension{EXT,tessellation_shader} (part of OpenGL ES 3.2) is
         * available, returns @cpp 0 @ce.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_TESS_CONTROL_OUTPUT_COMPONENTS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Tessellation shaders are not available in WebGL.
         */
        static Int maxTessellationControlOutputComponents();

        /**
         * @brief Max supported component count of all tessellation control shader output vertices combined
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither @gl_extension{ARB,tessellation_shader} (part
         * of OpenGL 4.0) nor @gl_extension{ANDROID,extension_pack_es31a} /
         * @gl_extension{EXT,tessellation_shader} (part of OpenGL ES 3.2) is
         * available, returns @cpp 0 @ce.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Tessellation shaders are not available in WebGL.
         */
        static Int maxTessellationControlTotalOutputComponents();

        /**
         * @brief Max supported component count of tessellation evaluation shader input vertex
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither @gl_extension{ARB,tessellation_shader} (part
         * of OpenGL 4.0) nor @gl_extension{ANDROID,extension_pack_es31a} /
         * @gl_extension{EXT,tessellation_shader} (part of OpenGL ES 3.2) is
         * available, returns @cpp 0 @ce.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_TESS_EVALUATION_INPUT_COMPONENTS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Tessellation shaders are not available in WebGL.
         */
        static Int maxTessellationEvaluationInputComponents();

        /**
         * @brief Max supported component count of tessellation evaluation shader output vertex
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither @gl_extension{ARB,tessellation_shader} (part
         * of OpenGL 4.0) nor @gl_extension{ANDROID,extension_pack_es31a} /
         * @gl_extension{EXT,tessellation_shader} (part of OpenGL ES 3.2) is
         * available, returns @cpp 0 @ce.
         * @see @fn_gl{Get} with @def_gl{MAX_TESS_EVALUATION_OUTPUT_COMPONENTS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Tessellation shaders are not available in WebGL.
         */
        static Int maxTessellationEvaluationOutputComponents();

        /**
         * @brief Max supported component count of geometry shader input vertex
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither @gl_extension{ARB,geometry_shader4} (part of
         * OpenGL 3.2) nor @gl_extension{ANDROID,extension_pack_es31a} /
         * @gl_extension{EXT,geometry_shader} (part of OpenGL ES 3.2) is not
         * available, returns @cpp 0 @ce.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_GEOMETRY_INPUT_COMPONENTS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Geometry shaders are not available in WebGL.
         */
        static Int maxGeometryInputComponents();

        /**
         * @brief Max supported component count of geometry shader output vertex
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither @gl_extension{ARB,geometry_shader4} (part of
         * OpenGL 3.2) nor @gl_extension{ANDROID,extension_pack_es31a} /
         * @gl_extension{EXT,geometry_shader} (part of OpenGL ES 3.2) is not
         * available, returns @cpp 0 @ce.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_GEOMETRY_OUTPUT_COMPONENTS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Geometry shaders are not available in WebGL.
         */
        static Int maxGeometryOutputComponents();

        /**
         * @brief Max supported component count of all geometry shader output vertices combined
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither @gl_extension{ARB,geometry_shader4} (part of
         * OpenGL 3.2) nor @gl_extension{ANDROID,extension_pack_es31a} /
         * @gl_extension{EXT,geometry_shader} (part of OpenGL ES 3.2) is not
         * available, returns @cpp 0 @ce.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Geometry shaders are not available in WebGL.
         */
        static Int maxGeometryTotalOutputComponents();
        #endif

        /**
         * @brief Max supported component count on fragment shader input
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. In OpenGL ES 2.0 the four-component vector count is
         * queried and multiplied with 4.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_FRAGMENT_INPUT_COMPONENTS},
         *      @def_gl_keyword{MAX_VARYING_COMPONENTS} in OpenGL <3.2 or
         *      @def_gl_keyword{MAX_VARYING_VECTORS} in OpenGL ES 2.0
         */
        static Int maxFragmentInputComponents();

        /**
         * @brief Max supported uniform component count in default block
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If particular shader stage is not available, returns
         * @cpp 0 @ce. In OpenGL ES 2.0 the four-component vector count is
         * queried and multiplied with 4.
         * @see @ref maxCombinedUniformComponents(),
         *      @fn_gl{Get} with @def_gl_keyword{MAX_VERTEX_UNIFORM_COMPONENTS},
         *      @def_gl_keyword{MAX_TESS_CONTROL_UNIFORM_COMPOENTS},
         *      @def_gl_keyword{MAX_TESS_EVALUATION_UNIFORM_COMPONENTS},
         *      @def_gl_keyword{MAX_GEOMETRY_UNIFORM_COMPONENTS},
         *      @def_gl_keyword{MAX_COMPUTE_UNIFORM_COMPONENTS},
         *      @def_gl_keyword{MAX_FRAGMENT_UNIFORM_COMPONENTS} or
         *      @def_gl_keyword{MAX_VERTEX_UNIFORM_VECTORS},
         *      @def_gl_keyword{MAX_FRAGMENT_UNIFORM_VECTORS} in OpenGL ES 2.0
         */
        static Int maxUniformComponents(Type type);

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @brief Max supported atomic counter buffer count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @gl_extension{ARB,shader_atomic_counters}
         * (part of OpenGL 4.2) nor OpenGL ES 3.1 is available or if particular
         * shader stage is not available, returns @cpp 0 @ce.
         * @see @ref maxCombinedAtomicCounterBuffers(), @ref maxAtomicCounters(),
         *      @fn_gl{Get} with @def_gl_keyword{MAX_VERTEX_ATOMIC_COUNTER_BUFFERS},
         *      @def_gl_keyword{MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS},
         *      @def_gl_keyword{MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS},
         *      @def_gl_keyword{MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS},
         *      @def_gl_keyword{MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS} or
         *      @def_gl_keyword{MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Atomic counters are not available in WebGL.
         */
        static Int maxAtomicCounterBuffers(Type type);

        /**
         * @brief Max supported atomic counter buffer count for all stages combined
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @gl_extension{ARB,shader_atomic_counters}
         * (part of OpenGL 4.2) nor OpenGL ES 3.1 is available, returns @cpp 0 @ce.
         * @see @ref maxAtomicCounterBuffers(), @ref maxCombinedAtomicCounters(),
         *      @fn_gl{Get} with @def_gl_keyword{MAX_COMBINED_ATOMIC_COUNTER_BUFFERS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Atomic counters are not available in WebGL.
         */
        static Int maxCombinedAtomicCounterBuffers();

        /**
         * @brief Max supported atomic counter count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @gl_extension{ARB,shader_atomic_counters}
         * (part of OpenGL 4.2) nor OpenGL ES 3.1 is available or if particular
         * shader stage is not available, returns @cpp 0 @ce.
         * @see @ref maxCombinedAtomicCounters(), @ref maxAtomicCounterBuffers(),
         *      @fn_gl{Get} with @def_gl_keyword{MAX_VERTEX_ATOMIC_COUNTERS},
         *      @def_gl_keyword{MAX_TESS_CONTROL_ATOMIC_COUNTERS},
         *      @def_gl_keyword{MAX_TESS_EVALUATION_ATOMIC_COUNTERS},
         *      @def_gl_keyword{MAX_GEOMETRY_ATOMIC_COUNTERS},
         *      @def_gl_keyword{MAX_COMPUTE_ATOMIC_COUNTERS} or
         *      @def_gl_keyword{MAX_FRAGMENT_ATOMIC_COUNTERS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Atomic counters are not available in WebGL.
         */
        static Int maxAtomicCounters(Type type);

        /**
         * @brief Max supported atomic counter count for all stages combined
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @gl_extension{ARB,shader_atomic_counters}
         * (part of OpenGL 4.2) nor OpenGL ES 3.1 is available, returns @cpp 0 @ce.
         * @see @ref maxAtomicCounters(), @ref maxCombinedAtomicCounterBuffers(),
         *      @fn_gl{Get} with @def_gl_keyword{MAX_COMBINED_ATOMIC_COUNTERS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Atomic counters are not available in WebGL.
         */
        static Int maxCombinedAtomicCounters();

        /**
         * @brief Max supported image uniform count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @gl_extension{ARB,shader_image_load_store}
         * (part of OpenGL 4.2) nor OpenGL ES 3.1 is available or if particular
         * shader stage is not available, returns @cpp 0 @ce.
         * @see @ref maxCombinedImageUniforms(),
         *      @fn_gl{Get} with @def_gl_keyword{MAX_VERTEX_IMAGE_UNIFORMS},
         *      @def_gl_keyword{MAX_TESS_CONTROL_IMAGE_UNIFORMS},
         *      @def_gl_keyword{MAX_TESS_EVALUATION_IMAGE_UNIFORMS},
         *      @def_gl_keyword{MAX_GEOMETRY_IMAGE_UNIFORMS},
         *      @def_gl_keyword{MAX_COMPUTE_IMAGE_UNIFORMS} or
         *      @def_gl_keyword{MAX_FRAGMENT_IMAGE_UNIFORMS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Shader image load/store is not available in WebGL.
         */
        static Int maxImageUniforms(Type type);

        /**
         * @brief Max supported image uniform count for all stages combined
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @gl_extension{ARB,shader_image_load_store}
         * (part of OpenGL 4.2) nor OpenGL ES 3.1 is available, returns @cpp 0 @ce.
         * @see @ref maxImageUniforms(),
         *      @fn_gl{Get} with @def_gl_keyword{MAX_COMBINED_IMAGE_UNIFORMS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Shader image load/store is not available in WebGL.
         */
        static Int maxCombinedImageUniforms();

        /**
         * @brief Max supported shader storage block count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @gl_extension{ARB,shader_storage_buffer_object}
         * (part of OpenGL 4.3) nor OpenGL ES 3.1 is available or if particular
         * shader stage is not available, returns @cpp 0 @ce.
         * @see @ref maxCombinedShaderStorageBlocks(),
         *      @fn_gl{Get} with @def_gl_keyword{MAX_VERTEX_SHADER_STORAGE_BLOCKS},
         *      @def_gl_keyword{MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS},
         *      @def_gl_keyword{MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS},
         *      @def_gl_keyword{MAX_GEOMETRY_SHADER_STORAGE_BLOCKS},
         *      @def_gl_keyword{MAX_COMPUTE_SHADER_STORAGE_BLOCKS} or
         *      @def_gl_keyword{MAX_FRAGMENT_SHADER_STORAGE_BLOCKS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Shader image load/store is not available in WebGL.
         */
        static Int maxShaderStorageBlocks(Type type);

        /**
         * @brief Max supported shader storage block count for all stages combined
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @gl_extension{ARB,shader_storage_buffer_object}
         * (part of OpenGL 4.3) nor OpenGL ES 3.1 is available, returns @cpp 0 @ce.
         * @see @ref maxShaderStorageBlocks(),
         *      @fn_gl{Get} with @def_gl_keyword{MAX_COMBINED_SHADER_STORAGE_BLOCKS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Shader storage is not available in WebGL.
         */
        static Int maxCombinedShaderStorageBlocks();
        #endif

        /**
         * @brief Max supported texture image unit count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If particular shader stage is not available, returns
         * @cpp 0 @ce.
         * @see @ref maxCombinedTextureImageUnits(),
         *      @fn_gl{Get} with @def_gl_keyword{MAX_VERTEX_TEXTURE_IMAGE_UNITS},
         *      @def_gl_keyword{MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS},
         *      @def_gl_keyword{MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS},
         *      @def_gl_keyword{MAX_GEOMETRY_TEXTURE_IMAGE_UNITS},
         *      @def_gl_keyword{MAX_COMPUTE_TEXTURE_IMAGE_UNITS},
         *      @def_gl_keyword{MAX_TEXTURE_IMAGE_UNITS}
         */
        static Int maxTextureImageUnits(Type type);

        /**
         * @brief Max supported texture image unit count for all stages combined
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls.
         * @see @ref maxTextureImageUnits(), @fn_gl{Get} with
         *      @def_gl_keyword{MAX_COMBINED_TEXTURE_IMAGE_UNITS}
         */
        static Int maxCombinedTextureImageUnits();

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Max supported uniform block count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @gl_extension{ARB,uniform_buffer_objects}
         * (part of OpenGL 3.1) or particular shader stage is not available,
         * returns @cpp 0 @ce.
         * @see @ref maxCombinedUniformBlocks(), @ref maxUniformComponents(),
         *      @ref maxCombinedUniformComponents(),
         *      @fn_gl{Get} with @def_gl_keyword{MAX_VERTEX_UNIFORM_BLOCKS},
         *      @def_gl_keyword{MAX_TESS_CONTROL_UNIFORM_BLOCKS},
         *      @def_gl_keyword{MAX_TESS_EVALUATION_UNIFORM_BLOCKS},
         *      @def_gl_keyword{MAX_GEOMETRY_UNIFORM_BLOCKS},
         *      @def_gl_keyword{MAX_COMPUTE_UNIFORM_BLOCKS} or
         *      @def_gl_keyword{MAX_FRAGMENT_UNIFORM_BLOCKS}
         * @requires_gles30 Uniform blocks are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform blocks are not available in WebGL 1.0.
         */
        static Int maxUniformBlocks(Type type);

        /**
         * @brief Max supported uniform block count for all stages combined
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @gl_extension{ARB,uniform_buffer_objects}
         * (part of OpenGL 3.1) is not available, returns @cpp 0 @ce.
         * @see @ref maxUniformBlocks(), @ref maxUniformComponents(),
         *      @ref maxCombinedUniformComponents(),
         *      @fn_gl{Get} with @def_gl_keyword{MAX_COMBINED_UNIFORM_BLOCKS}
         * @requires_gles30 Uniform blocks are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform blocks are not available in WebGL 1.0.
         */
        static Int maxCombinedUniformBlocks();

        /**
         * @brief Max supported uniform component count in all blocks combined
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @gl_extension{ARB,uniform_buffer_objects}
         * (part of OpenGL 3.1) or particular shader stage is not available,
         * returns @cpp 0 @ce.
         * @see @ref maxUniformComponents(), @ref maxUniformBlocks(),
         *      @fn_gl{Get} with @def_gl_keyword{MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS},
         *      @def_gl_keyword{MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS},
         *      @def_gl_keyword{MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS},
         *      @def_gl_keyword{MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS},
         *      @def_gl_keyword{MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS} or
         *      @def_gl_keyword{MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS}
         * @requires_gles30 Uniform blocks are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform blocks are not available in WebGL 1.0.
         */
        static Int maxCombinedUniformComponents(Type type);
        #endif

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Compile multiple shaders simultaneously
         * @m_deprecated_since_latest Originally meant to batch multiple
         *      compile operations together in a way that allowed the driver to
         *      perform the compilation in multiple threads. Superseded by
         *      @ref submitCompile() and @ref checkCompile(), use either those
         *      or the zero-argument @ref compile() instead. See
         *      @ref GL-AbstractShaderProgram-async for more information.
         *
         * Calls @ref submitCompile() on all shaders first, then
         * @ref checkCompile(). Returns @cpp false @ce if compilation of any
         * shader failed, @cpp true @ce if everything succeeded.
         */
        static CORRADE_DEPRECATED("use either submitCompile() and checkCompile() or the zero-argument compile() instead") bool compile(std::initializer_list<Containers::Reference<Shader>> shaders);
        #endif

        /**
         * @brief Wrap existing OpenGL shader object
         * @param type          Shader type
         * @param id            OpenGL shader ID
         * @param flags         Object creation flags
         * @m_since_latest
         *
         * The @p id is expected to be of an existing OpenGL shader object.
         * Unlike a shader created using a constructor, the OpenGL object is by
         * default not deleted on destruction, use @p flags for different
         * behavior.
         * @see @ref release()
         */
        static Shader wrap(Type type, GLuint id, ObjectFlags flags = {}) {
            return Shader{type, id, flags};
        }

        /**
         * @brief Constructor
         * @param version   Target version
         * @param type      Shader type
         *
         * Creates empty OpenGL shader and adds @glsl #version @ce directive
         * corresponding to @p version parameter at the beginning. If
         * @ref Version::None is specified, (not) adding the @glsl #version @ce
         * directive is left to the user.
         * @see @ref Shader(NoCreateT), @ref wrap(),
         *      @fn_gl_keyword{CreateShader}
         */
        explicit Shader(Version version, Type type);

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
         * @see @ref Shader()
         */
        explicit Shader(NoCreateT) noexcept;

        /** @brief Copying is not allowed */
        Shader(const Shader&) = delete;

        /** @brief Move constructor */
        Shader(Shader&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL shader.
         * @see @ref wrap(), @ref release(), @fn_gl_keyword{DeleteShader}
         */
        ~Shader();

        /** @brief Copying is not allowed */
        Shader& operator=(const Shader&) = delete;

        /** @brief Move assignment */
        Shader& operator=(Shader&& other) noexcept;

        /** @brief OpenGL shader ID */
        GLuint id() const { return _id; }

        /**
         * @brief Release the underlying OpenGL object
         * @m_since_latest
         *
         * Releases ownership of the OpenGL shader object and returns its ID so
         * it's not deleted on destruction. The internal state is then
         * equivalent to a moved-from state.
         * @see @ref wrap()
         */
        /* MinGW complains loudly if the declaration doesn't also have inline */
        inline GLuint release();

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Shader label
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls. If OpenGL 4.3 / OpenGL ES 3.2 is not supported and
         * neither @gl_extension{KHR,debug} (covered also by
         * @gl_extension{ANDROID,extension_pack_es31a}) nor @gl_extension{EXT,debug_label}
         * desktop or ES extension is available, this function returns empty
         * string.
         * @see @fn_gl_keyword{GetObjectLabel} with @def_gl{SHADER} or
         *      @fn_gl_extension_keyword{GetObjectLabel,EXT,debug_label} with
         *      @def_gl{SHADER_OBJECT_EXT}
         * @requires_gles Debug output is not available in WebGL.
         */
        Containers::String label() const;

        /**
         * @brief Set shader label
         * @return Reference to self (for method chaining)
         *
         * Default is empty string. If OpenGL 4.3 / OpenGL ES 3.2 is not
         * supported and neither @gl_extension{KHR,debug} (covered also by
         * @gl_extension{ANDROID,extension_pack_es31a}) nor @gl_extension{EXT,debug_label}
         * desktop or ES extension is available, this function does nothing.
         * @see @ref maxLabelLength(), @fn_gl_keyword{ObjectLabel} with
         *      @def_gl{SHADER} or @fn_gl_extension_keyword{LabelObject,EXT,debug_label}
         *      with @def_gl{SHADER_OBJECT_EXT}
         * @requires_gles Debug output is not available in WebGL.
         */
        Shader& setLabel(Containers::StringView label);
        #endif

        /** @brief Shader type */
        Type type() const { return _type; }

        /**
         * @brief Shader sources
         *
         * The returned views are all
         * @relativeref{Corrade,Containers::StringViewFlag::NullTerminated},
         * @relativeref{Corrade,Containers::StringViewFlag::Global} is set for
         * the initial @glsl #version @ce directive and for global
         * null-terminated views that were passed to @ref addSource().
         */
        Containers::StringIterable sources() const;

        /**
         * @brief Add shader source
         * @param source    String with shader source
         * @return Reference to self (for method chaining)
         *
         * If the string is not empty, adds it to the shader source list,
         * preceded with a @glsl #line n 1 @ce directive for improved
         * @ref GL-Shader-errors "compilation error reporting". If it's empty,
         * the function is a no-op.
         *
         * If the view is both @relativeref{Corrade,Containers::StringViewFlag::NullTerminated}
         * and @relativeref{Corrade,Containers::StringViewFlag::Global}, it's
         * directly referenced, otherwise a copy is made internally. For
         * dynamic strings prefer to use the @ref addSource(Containers::String&&)
         * overload to avoid copies.
         * @see @ref addFile()
         */
        Shader& addSource(Containers::StringView source);
        /** @overload */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Shader& addSource(Containers::String&& source);
        #else
        /* Otherwise passing a char* is ambiguous between this and the
           StringView overload. Sigh, C++. */
        template<class U, typename std::enable_if<std::is_same<U&&, Containers::String&&>::value, int>::type = 0> Shader& addSource(U&& source) {
            return addSourceInternal(Utility::move(source));
        }
        #endif

        /**
         * @brief Add shader source file
         * @param filename  Name of source file to read from
         * @return Reference to self (for method chaining)
         *
         * The file must exist and must be readable. Calls @ref addSource()
         * with the contents.
         */
        Shader& addFile(const Containers::StringView filename);

        /**
         * @brief Compile the shader
         *
         * Calls @ref submitCompile(), immediately followed by
         * @ref checkCompile(), passing back its return value. See
         * documentation of those two functions for details.
         */
        bool compile();

        /**
         * @brief Submit the shader for compilation
         * @m_since_latest
         *
         * You can call @ref isCompileFinished() or @ref checkCompile() after,
         * but it's recommended to instead immediately call
         * @ref AbstractShaderProgram::attachShader() and
         * @relativeref{AbstractShaderProgram,submitLink()}, then optionally
         * continue with @relativeref{AbstractShaderProgram,isLinkFinished()}
         * and pass all input shaders to
         * @relativeref{AbstractShaderProgram,checkLink()} on the final program
         * --- if compilation would fail, subsequent linking will as well, and
         * @relativeref{AbstractShaderProgram,checkLink()} will print the
         * compilation error if linking failed due to that. See
         * @ref GL-AbstractShaderProgram-async for more information.
         * @see @fn_gl_keyword{ShaderSource}, @fn_gl_keyword{CompileShader}
         */
        void submitCompile();

        /**
         * @brief Check shader compilation status and await completion
         * @m_since_latest
         *
         * Has to be called only if @ref submitCompile() was called before.
         * It's however recommended to instead immediately call
         * @ref AbstractShaderProgram::attachShader() and
         * @relativeref{AbstractShaderProgram,submitLink()}, then optionally
         * continue with @relativeref{AbstractShaderProgram,isLinkFinished()}
         * and pass all input shaders to
         * @relativeref{AbstractShaderProgram,checkLink()} on the final program
         * --- if compilation would fail, subsequent linking will as well, and
         * @relativeref{AbstractShaderProgram,checkLink()} will print the
         * compilation error if linking failed due to that. See
         * @ref GL-AbstractShaderProgram-async for more information.
         * @see @fn_gl_keyword{GetShader} with @def_gl{COMPILE_STATUS} and
         *      @def_gl{INFO_LOG_LENGTH}, @fn_gl_keyword{GetShaderInfoLog}
         */
        bool checkCompile();

        /**
         * @brief Whether a @ref submitCompile() operation has finished
         * @m_since_latest
         *
         * Has to be called only if @ref submitCompile() was called before, and
         * before @ref checkCompile(). If returns @cpp false @ce, a subsequent
         * @ref checkCompile() call will block until the compilation is
         * finished. If @gl_extension{KHR,parallel_shader_compile} is not
         * available, the function always returns @cpp true @ce --- i.e., as if
         * the compilation was done synchronously.
         *
         * It's however recommended to wait only for the final link to finish,
         * and not for particular compilations --- i.e., right after
         * @ref submitCompile() continue with
         * @ref AbstractShaderProgram::attachShader() and
         * @relativeref{AbstractShaderProgram,submitLink()}, and then check
         * with @relativeref{AbstractShaderProgram,isLinkFinished()} on the
         * final program. See @ref GL-AbstractShaderProgram-async for more
         * information.
         * @see @fn_gl_keyword{GetProgram} with
         *      @def_gl_extension{COMPLETION_STATUS,KHR,parallel_shader_compile}
         */
        bool isCompileFinished();

    private:
        explicit Shader(Type type, GLuint id, ObjectFlags flags) noexcept;

        /* Used by addSource(Containers::String&&) */
        Shader& addSourceInternal(Containers::String&& source);

        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL) && !defined(CORRADE_TARGET_APPLE)
        static MAGNUM_GL_LOCAL Containers::StringView workaroundDefinesImplementationNoOp(Version version);
        static MAGNUM_GL_LOCAL Containers::StringView workaroundDefinesImplementationAdrenoVersion(Version version);
        #endif

        static void MAGNUM_GL_LOCAL addSourceImplementationDefault(Shader& self, Containers::String&& source);
        #if defined(CORRADE_TARGET_EMSCRIPTEN) && defined(__EMSCRIPTEN_PTHREADS__)
        static void MAGNUM_GL_LOCAL addSourceImplementationEmscriptenPthread(Shader& self, Containers::String&& source);
        #endif

        static MAGNUM_GL_LOCAL void cleanLogImplementationNoOp(Containers::String& message);
        #if defined(CORRADE_TARGET_WINDOWS) && !defined(MAGNUM_TARGET_GLES)
        static MAGNUM_GL_LOCAL void cleanLogImplementationIntelWindows(Containers::String& message);
        #endif

        MAGNUM_GL_LOCAL static void APIENTRY completionStatusImplementationFallback(GLuint, GLenum, GLint*);

        Type _type;
        GLuint _id;
        ObjectFlags _flags;
        #ifndef MAGNUM_TARGET_GLES
        /* Used by addSource() / addFile(), see there for details */
        bool _offsetLineByOneOnOldGlsl;
        #endif
        UnsignedByte _fileIndexOffset;

        Containers::Array<Containers::String> _sources;
};

/** @debugoperatorclassenum{Shader,Shader::Type} */
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, Shader::Type value);

inline GLuint Shader::release() {
    const GLuint id = _id;
    _id = 0;
    return id;
}

}}

#endif
