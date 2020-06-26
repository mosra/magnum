#ifndef Magnum_GL_PipelineStatisticsQuery_h
#define Magnum_GL_PipelineStatisticsQuery_h
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

#ifndef MAGNUM_TARGET_GLES
/** @file
 * @brief Class @ref Magnum::GL::PipelineStatisticsQuery
 * @m_since{2020,06}
 */
#endif

#include "Magnum/GL/AbstractQuery.h"

#ifndef MAGNUM_TARGET_GLES
namespace Magnum { namespace GL {

/**
@brief Pipeline statistics query
@m_since{2020,06}

Provides various data about the rendering pipeline, useful for profiling and
performance measurements.
@requires_gl46 Extension @gl_extension{ARB,pipeline_statistics_query}
@requires_gl Pipeline statistics queries are not available in OpenGL ES and
    WebGL.
@see @ref PrimitiveQuery, @ref SampleQuery, @ref TimeQuery
*/
class PipelineStatisticsQuery: public AbstractQuery {
    public:
        /**
         * @brief Query target
         *
         * @m_enum_values_as_keywords
         */
        enum class Target: GLenum {
            /**
             * Count of vertices submitted to the primitive assembler. Note
             * that this doesn't take index buffer or strips/fans into account.
             */
            VerticesSubmitted = GL_VERTICES_SUBMITTED,

            /** Count of primitives submitted to the primitive assembler. */
            PrimitivesSubmitted = GL_PRIMITIVES_SUBMITTED,

            /**
             * Count of vertex shader invocations. For indexed draws this is
             * usually less than @ref Target::VerticesSubmitted, depending on
             * how well is the post-transform vertex cache used.
             */
            VertexShaderInvocations = GL_VERTEX_SHADER_INVOCATIONS,

            /**
             * Count of patches processed by tessellation control shader stage.
             */
            TessellationControlShaderPatches = GL_TESS_CONTROL_SHADER_PATCHES,

            /** Count of tessellation evaluation shader invocations. */
            TessellationEvaluationShaderInvocations = GL_TESS_EVALUATION_SHADER_INVOCATIONS,

            /** Count of geometry shader invocations. */
            GeometryShaderInvocations = GL_GEOMETRY_SHADER_INVOCATIONS,

            /**
             * Count of primitives emitted by a geometry shader. Compared to
             * @ref PrimitiveQuery::Target::PrimitivesGenerated, the query
             * considers all vertex streams and implementation may not count
             * primitives that aren't processed further.
             */
            GeometryShaderPrimitivesEmitted = GL_GEOMETRY_SHADER_PRIMITIVES_EMITTED,

            /** Count of fragment shader invocations. */
            FragmentShaderInvocations = GL_FRAGMENT_SHADER_INVOCATIONS,

            /** Count of compute shader invocations. */
            ComputeShaderInvocations = GL_COMPUTE_SHADER_INVOCATIONS,

            /** Count of primitives that entered the clipping stage. */
            ClippingInputPrimitives = GL_CLIPPING_INPUT_PRIMITIVES,

            /**
             * Count of primitives that passed the clipping stage. In an ideal
             * case of CPU-side frustum culling, the value reported by this
             * query is the same as @ref Target::ClippingInputPrimitives.
             */
            ClippingOutputPrimitives = GL_CLIPPING_OUTPUT_PRIMITIVES
        };

        /**
         * @brief Wrap existing OpenGL time query object
         * @param id            OpenGL time query ID
         * @param target        Query target
         * @param flags         Object creation flags
         *
         * The @p id is expected to be of an existing OpenGL query object.
         * Unlike query created using constructor, the OpenGL object is by
         * default not deleted on destruction, use @p flags for different
         * behavior.
         * @see @ref release()
         */
        static PipelineStatisticsQuery wrap(GLuint id, Target target, ObjectFlags flags = {}) {
            return PipelineStatisticsQuery{id, target, flags};
        }

        /**
         * @brief Constructor
         *
         * Creates new OpenGL query object. If @gl_extension{ARB,direct_state_access}
         * (part of OpenGL 4.5) is not available, the query is created on first
         * use.
         * @see @ref PipelineStatisticsQuery(NoCreateT), @ref wrap(), @fn_gl_keyword{CreateQueries},
         *      eventually @fn_gl_keyword{GenQueries}
         */
        explicit PipelineStatisticsQuery(Target target): AbstractQuery(GLenum(target)) {}

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
         * @see @ref PipelineStatisticsQuery(Target), @ref wrap()
         */
        explicit PipelineStatisticsQuery(NoCreateT) noexcept: AbstractQuery{NoCreate, GLenum(Target::VerticesSubmitted)} {}

        /** @brief Copying is not allowed */
        PipelineStatisticsQuery(const PipelineStatisticsQuery&) = delete;

        /** @brief Move constructor */
        PipelineStatisticsQuery(PipelineStatisticsQuery&&) noexcept = default;

        /** @brief Copying is not allowed */
        PipelineStatisticsQuery& operator=(const PipelineStatisticsQuery&) = delete;

        /** @brief Move assignment */
        PipelineStatisticsQuery& operator=(PipelineStatisticsQuery&&) noexcept = default;

        /* Overloads to remove WTF-factor from method chaining order */
        #if !defined(DOXYGEN_GENERATING_OUTPUT) && !defined(MAGNUM_TARGET_WEBGL)
        PipelineStatisticsQuery& setLabel(const std::string& label) {
            AbstractQuery::setLabel(label);
            return *this;
        }
        template<std::size_t size> PipelineStatisticsQuery& setLabel(const char(&label)[size]) {
            AbstractQuery::setLabel<size>(label);
            return *this;
        }
        #endif

    private:
        explicit PipelineStatisticsQuery(GLuint id, Target target, ObjectFlags flags) noexcept: AbstractQuery{id, GLenum(target), flags} {}
};

}}
#else
#error this header is not available in OpenGL ES build
#endif

#endif
