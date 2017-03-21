#ifndef Magnum_Shader_h
#define Magnum_Shader_h
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
 * @brief Class @ref Magnum::Shader
 */

#include <functional>
#include <string>
#include <vector>
#include <Corrade/Containers/ArrayView.h>

#include "Magnum/AbstractObject.h"
#include "Magnum/Magnum.h"

namespace Magnum {

/**
@brief Shader

See @ref AbstractShaderProgram for usage information.

## Performance optimizations

Shader limits and implementation-defined values (such as @ref maxUniformComponents())
are cached, so repeated queries don't result in repeated @fn_gl{Get} calls.
 */
class MAGNUM_EXPORT Shader: public AbstractObject {
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
         */
        enum class Type: GLenum {
            Vertex = GL_VERTEX_SHADER,      /**< Vertex shader */

            #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
            /**
             * Tessellation control shader
             * @requires_gl40 Extension @extension{ARB,tessellation_shader}
             * @requires_gles30 Not defined in OpenGL ES 2.0.
             * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
             *      @extension{EXT,tessellation_shader}
             * @requires_gles Tessellation shaders are not available in WebGL.
             */
            #ifndef MAGNUM_TARGET_GLES
            TessellationControl = GL_TESS_CONTROL_SHADER,
            #else
            TessellationControl = GL_TESS_CONTROL_SHADER_EXT,
            #endif

            /**
             * Tessellation evaluation shader
             * @requires_gl40 Extension @extension{ARB,tessellation_shader}
             * @requires_gles30 Not defined in OpenGL ES 2.0.
             * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
             *      @extension{EXT,tessellation_shader}
             * @requires_gles Tessellation shaders are not available in WebGL.
             */
            #ifndef MAGNUM_TARGET_GLES
            TessellationEvaluation = GL_TESS_EVALUATION_SHADER,
            #else
            TessellationEvaluation = GL_TESS_EVALUATION_SHADER_EXT,
            #endif

            /**
             * Geometry shader
             * @requires_gl32 Extension @extension{ARB,geometry_shader4}
             * @requires_gles30 Not defined in OpenGL ES 2.0.
             * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
             *      @extension{EXT,geometry_shader}
             * @requires_gles Geometry shaders are not available in WebGL.
             */
            #ifndef MAGNUM_TARGET_GLES
            Geometry = GL_GEOMETRY_SHADER,
            #else
            Geometry = GL_GEOMETRY_SHADER_EXT,
            #endif

            /**
             * Compute shader
             * @requires_gl43 Extension @extension{ARB,compute_shader}
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
         * @see @fn_gl{Get} with @def_gl{MAX_VERTEX_OUTPUT_COMPONENTS},
         *      @def_gl{MAX_VARYING_COMPONENTS} in OpenGL <3.2 or
         *      @def_gl{MAX_VARYING_VECTORS} in OpenGL ES 2.0
         */
        static Int maxVertexOutputComponents();

        /** @todo `GL_MAX_PATCH_VERTICES`, `GL_MAX_TESS_GEN_LEVEL`, `GL_MAX_TESS_PATCH_COMPONENTS` when @extension{ARB,tessellation_shader} is done */

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @brief Max supported component count of tessellation control shader input vertex
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither @extension{ARB,tessellation_shader} (part
         * of OpenGL 4.0) nor @extension{ANDROID,extension_pack_es31a}/
         * @extension{EXT,tessellation_shader} ES extension is available,
         * returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_TESS_CONTROL_INPUT_COMPONENTS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gl Tessellation shaders are not available in WebGL.
         */
        static Int maxTessellationControlInputComponents();

        /**
         * @brief Max supported component count of tessellation control shader output vertex
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither @extension{ARB,tessellation_shader} (part
         * of OpenGL 4.0) nor @extension{ANDROID,extension_pack_es31a}/
         * @extension{EXT,tessellation_shader} ES extension is available,
         * returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_TESS_CONTROL_OUTPUT_COMPONENTS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gl Tessellation shaders are not available in WebGL.
         */
        static Int maxTessellationControlOutputComponents();

        /**
         * @brief Max supported component count of all tessellation control shader output vertices combined
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither @extension{ARB,tessellation_shader} (part
         * of OpenGL 4.0) nor @extension{ANDROID,extension_pack_es31a}/
         * @extension{EXT,tessellation_shader} ES extension is available,
         * returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gl Tessellation shaders are not available in WebGL.
         */
        static Int maxTessellationControlTotalOutputComponents();

        /**
         * @brief Max supported component count of tessellation evaluation shader input vertex
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither @extension{ARB,tessellation_shader} (part
         * of OpenGL 4.0) nor @extension{ANDROID,extension_pack_es31a}/
         * @extension{EXT,tessellation_shader} ES extension is available,
         * returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_TESS_EVALUATION_INPUT_COMPONENTS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gl Tessellation shaders are not available in WebGL.
         */
        static Int maxTessellationEvaluationInputComponents();

        /**
         * @brief Max supported component count of tessellation evaluation shader output vertex
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither @extension{ARB,tessellation_shader} (part
         * of OpenGL 4.0) nor @extension{ANDROID,extension_pack_es31a}/
         * @extension{EXT,tessellation_shader} ES extension is available,
         * returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_TESS_EVALUATION_OUTPUT_COMPONENTS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gl Tessellation shaders are not available in WebGL.
         */
        static Int maxTessellationEvaluationOutputComponents();

        /**
         * @brief Max supported component count of geometry shader input vertex
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither @extension{ARB,geometry_shader4} (part of
         * OpenGL 3.2) nor @extension{ANDROID,extension_pack_es31a}/
         * @extension{EXT,geometry_shader} ES extension is not available,
         * returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_GEOMETRY_INPUT_COMPONENTS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Geometry shaders are not available in WebGL.
         */
        static Int maxGeometryInputComponents();

        /**
         * @brief Max supported component count of geometry shader output vertex
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither @extension{ARB,geometry_shader4} (part of
         * OpenGL 3.2) nor @extension{ANDROID,extension_pack_es31a}/
         * @extension{EXT,geometry_shader} ES extension is not available,
         * returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_GEOMETRY_OUTPUT_COMPONENTS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Geometry shaders are not available in WebGL.
         */
        static Int maxGeometryOutputComponents();

        /**
         * @brief Max supported component count of all geometry shader output vertices combined
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither @extension{ARB,geometry_shader4} (part of
         * OpenGL 3.2) nor @extension{ANDROID,extension_pack_es31a}/
         * @extension{EXT,geometry_shader} ES extension is not available,
         * returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS}
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
         * @see @fn_gl{Get} with @def_gl{MAX_FRAGMENT_INPUT_COMPONENTS},
         *      @def_gl{MAX_VARYING_COMPONENTS} in OpenGL <3.2 or
         *      @def_gl{MAX_VARYING_VECTORS} in OpenGL ES 2.0
         */
        static Int maxFragmentInputComponents();

        /**
         * @brief Max supported uniform component count in default block
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If particular shader stage is not available, returns
         * `0`. In OpenGL ES 2.0 the four-component vector count is queried and
         * multiplied with 4.
         * @see @ref maxCombinedUniformComponents(),
         *      @fn_gl{Get} with @def_gl{MAX_VERTEX_UNIFORM_COMPONENTS},
         *      @def_gl{MAX_TESS_CONTROL_UNIFORM_COMPOENTS},
         *      @def_gl{MAX_TESS_EVALUATION_UNIFORM_COMPONENTS},
         *      @def_gl{MAX_GEOMETRY_UNIFORM_COMPONENTS},
         *      @def_gl{MAX_COMPUTE_UNIFORM_COMPONENTS},
         *      @def_gl{MAX_FRAGMENT_UNIFORM_COMPONENTS} or
         *      @def_gl{MAX_VERTEX_UNIFORM_VECTORS},
         *      @def_gl{MAX_FRAGMENT_UNIFORM_VECTORS} in OpenGL ES 2.0
         */
        static Int maxUniformComponents(Type type);

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @brief Max supported atomic counter buffer count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @extension{ARB,shader_atomic_counters}
         * (part of OpenGL 4.2) nor OpenGL ES 3.1 is available or if particular
         * shader stage is not available, returns `0`.
         * @see @ref maxCombinedAtomicCounterBuffers(), @ref maxAtomicCounters(),
         *      @fn_gl{Get} with @def_gl{MAX_VERTEX_ATOMIC_COUNTER_BUFFERS},
         *      @def_gl{MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS},
         *      @def_gl{MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS},
         *      @def_gl{MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS},
         *      @def_gl{MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS} or
         *      @def_gl{MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Atomic counters are not available in WebGL.
         */
        static Int maxAtomicCounterBuffers(Type type);

        /**
         * @brief Max supported atomic counter buffer count for all stages combined
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @extension{ARB,shader_atomic_counters}
         * (part of OpenGL 4.2) nor OpenGL ES 3.1 is available, returns `0`.
         * @see @ref maxAtomicCounterBuffers(), @ref maxCombinedAtomicCounters(),
         *      @fn_gl{Get} with @def_gl{MAX_COMBINED_ATOMIC_COUNTER_BUFFERS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Atomic counters are not available in WebGL.
         */
        static Int maxCombinedAtomicCounterBuffers();

        /**
         * @brief Max supported atomic counter count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @extension{ARB,shader_atomic_counters}
         * (part of OpenGL 4.2) nor OpenGL ES 3.1 is available or if particular
         * shader stage is not available, returns `0`.
         * @see @ref maxCombinedAtomicCounters(), @ref maxAtomicCounterBuffers(),
         *      @fn_gl{Get} with @def_gl{MAX_VERTEX_ATOMIC_COUNTERS},
         *      @def_gl{MAX_TESS_CONTROL_ATOMIC_COUNTERS},
         *      @def_gl{MAX_TESS_EVALUATION_ATOMIC_COUNTERS},
         *      @def_gl{MAX_GEOMETRY_ATOMIC_COUNTERS},
         *      @def_gl{MAX_COMPUTE_ATOMIC_COUNTERS} or
         *      @def_gl{MAX_FRAGMENT_ATOMIC_COUNTERS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Atomic counters are not available in WebGL.
         */
        static Int maxAtomicCounters(Type type);

        /**
         * @brief Max supported atomic counter count for all stages combined
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @extension{ARB,shader_atomic_counters}
         * (part of OpenGL 4.2) nor OpenGL ES 3.1 is available, returns `0`.
         * @see @ref maxAtomicCounters(), @ref maxCombinedAtomicCounterBuffers(),
         *      @fn_gl{Get} with @def_gl{MAX_COMBINED_ATOMIC_COUNTERS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Atomic counters are not available in WebGL.
         */
        static Int maxCombinedAtomicCounters();

        /**
         * @brief Max supported image uniform count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @extension{ARB,shader_image_load_store}
         * (part of OpenGL 4.2) nor OpenGL ES 3.1 is available or if particular
         * shader stage is not available, returns `0`.
         * @see @ref maxCombinedImageUniforms(),
         *      @fn_gl{Get} with @def_gl{MAX_VERTEX_IMAGE_UNIFORMS},
         *      @def_gl{MAX_TESS_CONTROL_IMAGE_UNIFORMS},
         *      @def_gl{MAX_TESS_EVALUATION_IMAGE_UNIFORMS},
         *      @def_gl{MAX_GEOMETRY_IMAGE_UNIFORMS},
         *      @def_gl{MAX_COMPUTE_IMAGE_UNIFORMS} or
         *      @def_gl{MAX_FRAGMENT_IMAGE_UNIFORMS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Shader image load/store is not available in WebGL.
         */
        static Int maxImageUniforms(Type type);

        /**
         * @brief Max supported image uniform count for all stages combined
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @extension{ARB,shader_image_load_store}
         * (part of OpenGL 4.2) nor OpenGL ES 3.1 is available, returns `0`.
         * @see @ref maxImageUniforms(),
         *      @fn_gl{Get} with @def_gl{MAX_COMBINED_IMAGE_UNIFORMS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Shader image load/store is not available in WebGL.
         */
        static Int maxCombinedImageUniforms();

        /**
         * @brief Max supported shader storage block count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @extension{ARB,shader_storage_buffer_object}
         * (part of OpenGL 4.3) nor OpenGL ES 3.1 is available or if particular
         * shader stage is not available, returns `0`.
         * @see @ref maxCombinedShaderStorageBlocks(),
         *      @fn_gl{Get} with @def_gl{MAX_VERTEX_SHADER_STORAGE_BLOCKS},
         *      @def_gl{MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS},
         *      @def_gl{MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS},
         *      @def_gl{MAX_GEOMETRY_SHADER_STORAGE_BLOCKS},
         *      @def_gl{MAX_COMPUTE_SHADER_STORAGE_BLOCKS} or
         *      @def_gl{MAX_FRAGMENT_SHADER_STORAGE_BLOCKS}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles Shader image load/store is not available in WebGL.
         */
        static Int maxShaderStorageBlocks(Type type);

        /**
         * @brief Max supported shader storage block count for all stages combined
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @extension{ARB,shader_storage_buffer_object}
         * (part of OpenGL 4.3) nor OpenGL ES 3.1 is available, returns `0`.
         * @see @ref maxShaderStorageBlocks(),
         *      @fn_gl{Get} with @def_gl{MAX_COMBINED_SHADER_STORAGE_BLOCKS}
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
         * `0`.
         * @see @ref maxCombinedTextureImageUnits(),
         *      @fn_gl{Get} with @def_gl{MAX_VERTEX_TEXTURE_IMAGE_UNITS},
         *      @def_gl{MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS},
         *      @def_gl{MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS},
         *      @def_gl{MAX_GEOMETRY_TEXTURE_IMAGE_UNITS},
         *      @def_gl{MAX_COMPUTE_TEXTURE_IMAGE_UNITS},
         *      @def_gl{MAX_TEXTURE_IMAGE_UNITS}
         */
        static Int maxTextureImageUnits(Type type);

        /**
         * @brief Max supported texture image unit count for all stages combined
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls.
         * @see @ref maxTextureImageUnits(), @fn_gl{Get} with
         *      @def_gl{MAX_COMBINED_TEXTURE_IMAGE_UNITS}
         */
        static Int maxCombinedTextureImageUnits();

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Max supported uniform block count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,uniform_buffer_objects}
         * (part of OpenGL 3.1) or particular shader stage is not available,
         * returns `0`.
         * @see @ref maxCombinedUniformBlocks(), @ref maxUniformComponents(),
         *      @ref maxCombinedUniformComponents(),
         *      @fn_gl{Get} with @def_gl{MAX_VERTEX_UNIFORM_BLOCKS},
         *      @def_gl{MAX_TESS_CONTROL_UNIFORM_BLOCKS},
         *      @def_gl{MAX_TESS_EVALUATION_UNIFORM_BLOCKS},
         *      @def_gl{MAX_GEOMETRY_UNIFORM_BLOCKS},
         *      @def_gl{MAX_COMPUTE_UNIFORM_BLOCKS} or
         *      @def_gl{MAX_FRAGMENT_UNIFORM_BLOCKS}
         * @requires_gles30 Uniform blocks are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform blocks are not available in WebGL 1.0.
         */
        static Int maxUniformBlocks(Type type);

        /**
         * @brief Max supported uniform block count for all stages combined
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,uniform_buffer_objects}
         * (part of OpenGL 3.1) is not available, returns `0`.
         * @see @ref maxUniformBlocks(), @ref maxUniformComponents(),
         *      @ref maxCombinedUniformComponents(),
         *      @fn_gl{Get} with @def_gl{MAX_COMBINED_UNIFORM_BLOCKS}
         * @requires_gles30 Uniform blocks are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform blocks are not available in WebGL 1.0.
         */
        static Int maxCombinedUniformBlocks();

        /**
         * @brief Max supported uniform component count in all blocks combined
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,uniform_buffer_objects}
         * (part of OpenGL 3.1) or particular shader stage is not available,
         * returns `0`.
         * @see @ref maxUniformComponents(), @ref maxUniformBlocks(),
         *      @fn_gl{Get} with @def_gl{MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS},
         *      @def_gl{MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS},
         *      @def_gl{MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS},
         *      @def_gl{MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS},
         *      @def_gl{MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS} or
         *      @def_gl{MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS}
         * @requires_gles30 Uniform blocks are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform blocks are not available in WebGL 1.0.
         */
        static Int maxCombinedUniformComponents(Type type);
        #endif

        /**
         * @brief Compile multiple shaders simultaneously
         *
         * Returns `false` if compilation of any shader failed, `true` if
         * everything succeeded. Compiler messages (if any) are printed to
         * error output. The operation is batched in a way that allows the
         * driver to perform multiple compilations simultaneously (i.e. in
         * multiple threads).
         * @see @fn_gl{ShaderSource}, @fn_gl{CompileShader}, @fn_gl{GetShader}
         *      with @def_gl{COMPILE_STATUS} and @def_gl{INFO_LOG_LENGTH},
         *      @fn_gl{GetShaderInfoLog}
         */
        static bool compile(std::initializer_list<std::reference_wrapper<Shader>> shaders);

        /**
         * @brief Constructor
         * @param version   Target version
         * @param type      Shader type
         *
         * Creates empty OpenGL shader and adds @c \#version directive
         * corresponding to @p version parameter at the beginning. If
         * @ref Version::None is specified, (not) adding the @c \#version
         * directive is left to the user.
         * @see @fn_gl{CreateShader}
         */
        explicit Shader(Version version, Type type);

        /** @brief Copying is not allowed */
        Shader(const Shader&) = delete;

        /** @brief Move constructor */
        Shader(Shader&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL shader.
         * @see @fn_gl{DeleteShader}
         */
        ~Shader();

        /** @brief Copying is not allowed */
        Shader& operator=(const Shader&) = delete;

        /** @brief Move assignment */
        Shader& operator=(Shader&& other) noexcept;

        /** @brief OpenGL shader ID */
        GLuint id() const { return _id; }

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Shader label
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls. If OpenGL 4.3 is not supported and neither
         * @extension{KHR,debug} (covered also by @extension{ANDROID,extension_pack_es31a})
         * nor @extension{EXT,debug_label} desktop or ES extension is
         * available, this function returns empty string.
         * @see @fn_gl{GetObjectLabel} with @def_gl{SHADER} or
         *      @fn_gl_extension{GetObjectLabel,EXT,debug_label} with
         *      @def_gl{SHADER_OBJECT_EXT}
         * @requires_gles Debug output is not available in WebGL.
         */
        std::string label() const;

        /**
         * @brief Set shader label
         * @return Reference to self (for method chaining)
         *
         * Default is empty string. If OpenGL 4.3 is not supported and neither
         * @extension{KHR,debug} (covered also by @extension{ANDROID,extension_pack_es31a})
         * nor @extension{EXT,debug_label} desktop or ES extension is
         * available, this function does nothing.
         * @see @ref maxLabelLength(), @fn_gl{ObjectLabel} with
         *      @def_gl{SHADER} or @fn_gl_extension{LabelObject,EXT,debug_label}
         *      with @def_gl{SHADER_OBJECT_EXT}
         * @requires_gles Debug output is not available in WebGL.
         */
        Shader& setLabel(const std::string& label) {
            return setLabelInternal({label.data(), label.size()});
        }

        /** @overload */
        template<std::size_t size> Shader& setLabel(const char(&label)[size]) {
            return setLabelInternal({label, size - 1});
        }
        #endif

        /** @brief Shader type */
        Type type() const { return _type; }

        /** @brief Shader sources */
        std::vector<std::string> sources() const;

        /**
         * @brief Add shader source
         * @param source    String with shader source
         * @return Reference to self (for method chaining)
         *
         * Adds given source to source list, preceeded with @c \#line directive
         * marking first line of the source as `n(1)` where n is number of
         * added source. The source number `0` is @c \#version directive added
         * in constructor, if any. If passed string is empty, the function does
         * nothing.
         * @see @ref addFile()
         */
        Shader& addSource(std::string source);

        /**
         * @brief Add source file
         * @param filename  Name of source file to read from
         * @return Reference to self (for method chaining)
         *
         * The file must exist and must be readable. Calls @ref addSource()
         * with the contents.
         */
        Shader& addFile(const std::string& filename);

        /**
         * @brief Compile shader
         *
         * Compiles single shader. Prefer to compile multiple shaders at once
         * using @ref compile(std::initializer_list<std::reference_wrapper<Shader>>)
         * for improved performance, see its documentation for more
         * information.
         */
        bool compile() { return compile({*this}); }

    private:
        Shader& setLabelInternal(Containers::ArrayView<const char> label);

        Type _type;
        GLuint _id;

        std::vector<std::string> _sources;
};

/** @debugoperatorclassenum{Magnum::Shader,Magnum::Shader::Type} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, Shader::Type value);

inline Shader::Shader(Shader&& other) noexcept: _type(other._type), _id(other._id), _sources(std::move(other._sources)) {
    other._id = 0;
}

inline Shader& Shader::operator=(Shader&& other) noexcept {
    using std::swap;
    swap(_type, other._type);
    swap(_id, other._id);
    swap(_sources, other._sources);
    return *this;
}

}

#endif
