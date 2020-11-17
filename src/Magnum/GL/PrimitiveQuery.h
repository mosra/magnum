#ifndef Magnum_GL_PrimitiveQuery_h
#define Magnum_GL_PrimitiveQuery_h
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

#ifndef MAGNUM_TARGET_GLES2
/** @file
 * @brief Class @ref Magnum::GL::PrimitiveQuery
 */
#endif

#include "Magnum/GL/AbstractQuery.h"

#ifndef MAGNUM_TARGET_GLES2
namespace Magnum { namespace GL {

/**
@brief Query for primitives

Queries count of generated primitives from vertex shader, geometry shader or
transform feedback. Example usage:

@snippet MagnumGL.cpp PrimitiveQuery-usage

@see @ref PipelineStatisticsQuery, @ref SampleQuery, @ref TimeQuery,
    @ref TransformFeedback
@requires_gl30 Extension @gl_extension{EXT,transform_feedback}
@requires_gles30 Only sample queries are available in OpenGL ES 2.0.
@requires_webgl20 Queries are not available in WebGL 1.0.
*/
class MAGNUM_GL_EXPORT PrimitiveQuery: public AbstractQuery {
    public:
        /**
         * @brief Query target
         *
         * @see @ref PrimitiveQuery(Target)
         * @m_enum_values_as_keywords
         */
        enum class Target: GLenum {
            #ifndef MAGNUM_TARGET_WEBGL
            /**
             * Count of primitives generated from vertex shader or geometry
             * shader. When used with @ref begin(UnsignedInt), the index must
             * be lower than @ref TransformFeedback::maxVertexStreams(). Use
             * @ref result<UnsignedInt>() or @ref result<Int>() to retrieve the
             * result.
             * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
             *      @gl_extension{EXT,geometry_shader}
             * @requires_gles Geometry shaders are not available in WebGL.
             */
            #ifndef MAGNUM_TARGET_GLES
            PrimitivesGenerated = GL_PRIMITIVES_GENERATED,
            #else
            PrimitivesGenerated = GL_PRIMITIVES_GENERATED_EXT,
            #endif
            #endif

            /**
             * Count of primitives written to transform feedback buffer. When
             * used with @ref begin(UnsignedInt), the index must be lower than
             * @ref TransformFeedback::maxVertexStreams(). Use
             * @ref result<UnsignedInt>() or @ref result<Int>() to retrieve the
             * result.
             */
            TransformFeedbackPrimitivesWritten = GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Transform feedback overflow. When used with @ref begin(UnsignedInt),
             * the index must be `0`. Use @ref result<bool>() to retrieve the
             * result.
             * @requires_gl46 Extension @gl_extension{ARB,transform_feedback_overflow_query}
             * @requires_gl Transform feedback overflow query is not available
             *      in OpenGL ES or WebGL.
             */
            TransformFeedbackOverflow = GL_TRANSFORM_FEEDBACK_OVERFLOW,

            /**
             * Transform feedback stream overflow. When used with
             * @ref begin(UnsignedInt), the index must be lower than
             * @ref TransformFeedback::maxVertexStreams(). Use @ref result<bool>()
             * to retrieve the result.
             * @requires_gl46 Extension @gl_extension{ARB,transform_feedback_overflow_query}
             * @requires_gl Transform feedback overflow query is not available
             *      in OpenGL ES or WebGL.
             */
            TransformFeedbackStreamOverflow = GL_TRANSFORM_FEEDBACK_STREAM_OVERFLOW
            #endif
        };

        /**
         * @brief Wrap existing OpenGL primitive query object
         * @param id            OpenGL primitive query ID
         * @param target        Query target
         * @param flags         Object creation flags
         *
         * The @p id is expected to be of an existing OpenGL query object.
         * Unlike query created using constructor, the OpenGL object is by
         * default not deleted on destruction, use @p flags for different
         * behavior.
         * @see @ref release()
         */
        static PrimitiveQuery wrap(GLuint id, Target target, ObjectFlags flags = {}) {
            return PrimitiveQuery{id, target, flags};
        }

        /**
         * @brief Constructor
         *
         * Creates new OpenGL query object. If @gl_extension{ARB,direct_state_access}
         * (part of OpenGL 4.5) is not available, the query is created on first
         * use.
         * @see @ref PrimitiveQuery(NoCreateT), @ref wrap(),
         *      @fn_gl_keyword{CreateQueries}, eventually
         *      @fn_gl_keyword{GenQueries}
         */
        explicit PrimitiveQuery(Target target): AbstractQuery(GLenum(target)) {}

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
         * @see @ref PrimitiveQuery(Target), @ref wrap()
         */
        explicit PrimitiveQuery(NoCreateT) noexcept: AbstractQuery{NoCreate, GLenum(Target::TransformFeedbackPrimitivesWritten)} {}

        /** @brief Copying is not allowed */
        PrimitiveQuery(const PrimitiveQuery&) = delete;

        /** @brief Move constructor */
        PrimitiveQuery(PrimitiveQuery&&) noexcept = default;

        /** @brief Copying is not allowed */
        PrimitiveQuery& operator=(const PrimitiveQuery&) = delete;

        /** @brief Move assignment */
        PrimitiveQuery& operator=(PrimitiveQuery&&) noexcept = default;

        /**
         * @brief Begin query
         *
         * Begins counting until @ref end() is called. Equivalent to calling
         * @ref begin(UnsignedInt) with @p index set to @cpp 0 @ce.
         * @see @fn_gl_keyword{BeginQuery}
         */
        void begin();

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Begin indexed query
         *
         * Begins counting until @ref end() is called.
         * @see @fn_gl_keyword{BeginQueryIndexed}
         * @requires_gl40 Extension @gl_extension{ARB,transform_feedback3}
         * @requires_gl Indexed queries are not available in OpenGL ES or WebGL.
         */
        void begin(UnsignedInt index);
        #endif

        /**
         * @brief End query
         *
         * Ends the non-indexed or indexed query started with @ref begin() or
         * @ref begin(UnsignedInt). The result can be then retrieved by calling
         * @ref result().
         * @see @fn_gl_keyword{EndQuery}, @fn_gl2_keyword{EndQueryIndexed,BeginQueryIndexed}
         * @requires_gl40 Extension @gl_extension{ARB,transform_feedback3} for
         *      indexed queries
         * @requires_gl Indexed queries are not available in OpenGL ES or
         *      WebGL.
         */
        void end();

        /* Overloads to remove WTF-factor from method chaining order */
        #if !defined(DOXYGEN_GENERATING_OUTPUT) && !defined(MAGNUM_TARGET_WEBGL)
        PrimitiveQuery& setLabel(const std::string& label) {
            AbstractQuery::setLabel(label);
            return *this;
        }
        template<std::size_t size> PrimitiveQuery& setLabel(const char(&label)[size]) {
            AbstractQuery::setLabel<size>(label);
            return *this;
        }
        #endif

    private:
        explicit PrimitiveQuery(GLuint id, Target target, ObjectFlags flags) noexcept: AbstractQuery{id, GLenum(target), flags} {}

        #ifndef MAGNUM_TARGET_GLES
        GLuint _index{};
        #endif
};

}}
#else
#error this header is not available in OpenGL ES 2.0 build
#endif

#endif
