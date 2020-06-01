#ifndef Magnum_GL_SampleQuery_h
#define Magnum_GL_SampleQuery_h
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

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
/** @file
 * @brief Class @ref Magnum::GL::SampleQuery
 */
#endif

#include "Magnum/GL/AbstractQuery.h"

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
namespace Magnum { namespace GL {

/**
@brief Query for samples

Queries count of samples passed from fragment shader or boolean value
indicating whether any samples passed. Can be used for example for conditional
rendering:

@snippet MagnumGL.cpp SampleQuery-usage

This approach has some drawbacks, as the rendering is blocked until result is
available for the CPU to decide. This can be improved by using conditional
rendering on GPU itself. The drawing commands will be sent to the GPU and
processed or discarded later, so CPU can continue executing the code without
waiting for the result.

@snippet MagnumGL.cpp SampleQuery-conditional-render

@see @ref PipelineStatisticsQuery, @ref PrimitiveQuery, @ref TimeQuery
@requires_gles30 Extension @gl_extension{EXT,occlusion_query_boolean} in
    OpenGL ES 2.0.
@requires_webgl20 Queries are not available in WebGL 1.0.
*/
class SampleQuery: public AbstractQuery {
    public:
        /**
         * @brief Query target
         *
         * @m_enum_values_as_keywords
         */
        enum class Target: GLenum {
            #ifndef MAGNUM_TARGET_GLES
            /**
             * Count of samples passed from fragment shader. Use
             * @ref result<UnsignedInt>() or @ref result<Int>() to retrieve the
             * result.
             * @requires_gl Only boolean query is available in OpenGL ES and
             *      WebGL.
             */
            SamplesPassed = GL_SAMPLES_PASSED,
            #endif

            /**
             * Whether any samples passed from fragment shader. Use
             * @ref result<bool>() to retrieve the result.
             * @requires_gl33 Extension @gl_extension{ARB,occlusion_query2}
             */
            #ifndef MAGNUM_TARGET_GLES2
            AnySamplesPassed = GL_ANY_SAMPLES_PASSED,
            #else
            AnySamplesPassed = GL_ANY_SAMPLES_PASSED_EXT,
            #endif

            /**
             * Whether any samples passed from fragment shader (conservative).
             * An implementation may choose a less precise version of the test
             * at the expense of some false positives. Use @ref result<bool>()
             * to retrieve the result.
             * @requires_gl43 Extension @gl_extension{ARB,ES3_compatibility}
             */
            #ifndef MAGNUM_TARGET_GLES2
            AnySamplesPassedConservative = GL_ANY_SAMPLES_PASSED_CONSERVATIVE
            #else
            AnySamplesPassedConservative = GL_ANY_SAMPLES_PASSED_CONSERVATIVE_EXT
            #endif
        };

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Conditional render mode
         *
         * @m_enum_values_as_keywords
         * @requires_gl30 Extension @gl_extension{NV,conditional_render}
         * @requires_gl Conditional rendering is not available in OpenGL ES or
         *      WebGL.
         */
        enum class ConditionalRenderMode: GLenum {
            /**
             * If query result is not yet available, waits for it and then
             * begins rendering only if result is nonzero.
             */
            Wait = GL_QUERY_WAIT,

            /**
             * If query result is not yet available, waits for it and then
             * begins rendering only if result is zero.
             * @requires_gl45 Extension @gl_extension{ARB,conditional_render_inverted}
             */
            WaitInverted = GL_QUERY_WAIT_INVERTED,

            /**
             * If query result is not yet available, begins rendering like if
             * the result was nonzero.
             */
            NoWait = GL_QUERY_NO_WAIT,

            /**
             * If query result is not yet available, begins rendering like if
             * the result was zero.
             * @requires_gl45 Extension @gl_extension{ARB,conditional_render_inverted}
             */
            NoWaitInverted = GL_QUERY_NO_WAIT_INVERTED,

            /**
             * The same as @ref ConditionalRenderMode::Wait, but regions
             * untouched by the sample query may not be rendered at all.
             */
            ByRegionWait = GL_QUERY_BY_REGION_WAIT,

            /**
             * The same as @ref ConditionalRenderMode::WaitInverted, but
             * regions untouched by the sample query may not be rendered at
             * all.
             * @requires_gl45 Extension @gl_extension{ARB,conditional_render_inverted}
             */
            ByRegionWaitInverted = GL_QUERY_BY_REGION_WAIT_INVERTED,

            /**
             * The same as @ref ConditionalRenderMode::NoWait, but regions
             * untouched by the sample query may not be rendered at all.
             */
            ByRegionNoWait = GL_QUERY_BY_REGION_NO_WAIT,

            /**
             * The same as @ref ConditionalRenderMode::NoWaitInverted, but
             * regions untouched by the sample query may not be rendered at
             * all.
             * @requires_gl45 Extension @gl_extension{ARB,conditional_render_inverted}
             */
            ByRegionNoWaitInverted = GL_QUERY_BY_REGION_NO_WAIT_INVERTED
        };
        #endif

        /**
         * @brief Wrap existing OpenGL sample query object
         * @param id            OpenGL sample query ID
         * @param target        Query target
         * @param flags         Object creation flags
         *
         * The @p id is expected to be of an existing OpenGL query object.
         * Unlike query created using constructor, the OpenGL object is by
         * default not deleted on destruction, use @p flags for different
         * behavior.
         * @see @ref release(), @fn_gl{IsQuery}
         */
        static SampleQuery wrap(GLuint id, Target target, ObjectFlags flags = {}) {
            return SampleQuery{id, target, flags};
        }

        /**
         * @brief Constructor
         *
         * Creates new OpenGL query object. If @gl_extension{ARB,direct_state_access}
         * (part of OpenGL 4.5) is not available, the query is created on first
         * use.
         * @see @ref SampleQuery(NoCreateT), @ref wrap(),
         *      @fn_gl_keyword{CreateQueries}, eventually @fn_gl_keyword{GenQueries}
         */
        explicit SampleQuery(Target target): AbstractQuery(GLenum(target)) {}

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
         * @see @ref SampleQuery(Target), @ref wrap()
         */
        explicit SampleQuery(NoCreateT) noexcept: AbstractQuery{NoCreate, GLenum(Target::AnySamplesPassed)} {}

        /** @brief Copying is not allowed */
        SampleQuery(const SampleQuery&) = delete;

        /** @brief Move constructor */
        SampleQuery(SampleQuery&&) noexcept = default;

        /** @brief Copying is not allowed */
        SampleQuery& operator=(const SampleQuery&) = delete;

        /** @brief Move assignment */
        SampleQuery& operator=(SampleQuery&&) noexcept = default;

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Begin conditional rendering based on result value
         *
         * @see @fn_gl_keyword{BeginConditionalRender}
         * @requires_gl30 Extension @gl_extension{NV,conditional_render}
         * @requires_gl Conditional rendering is not available in OpenGL ES or
         *      WebGL.
         */
        void beginConditionalRender(ConditionalRenderMode mode) {
            glBeginConditionalRender(id(), GLenum(mode));
        }

        /**
         * @brief End conditional render
         *
         * @see @fn_gl_keyword{EndConditionalRender}
         * @requires_gl30 Extension @gl_extension{NV,conditional_render}
         * @requires_gl Conditional rendering is not available in OpenGL ES or
         *      WebGL.
         */
        void endConditionalRender() {
            glEndConditionalRender();
        }
        #endif

        /* Overloads to remove WTF-factor from method chaining order */
        #if !defined(DOXYGEN_GENERATING_OUTPUT) && !defined(MAGNUM_TARGET_WEBGL)
        SampleQuery& setLabel(const std::string& label) {
            AbstractQuery::setLabel(label);
            return *this;
        }
        template<std::size_t size> SampleQuery& setLabel(const char(&label)[size]) {
            AbstractQuery::setLabel<size>(label);
            return *this;
        }
        #endif

    private:
        explicit SampleQuery(GLuint id, Target target, ObjectFlags flags) noexcept: AbstractQuery{id, GLenum(target), flags} {}
};

}}
#else
#error this header is not available in WebGL 1.0 build
#endif

#endif
