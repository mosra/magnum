#ifndef Magnum_Query_h
#define Magnum_Query_h
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
 * @brief Class Magnum::AbstractQuery, Magnum::Query, Magnum::SampleQuery, Magnum::TimeQuery
 */

#include "Magnum.h"

#include "magnumVisibility.h"

namespace Magnum {

/**
@brief Base class for queries

See Query, SampleQuery, TimeQuery documentation for more information.
@todo Support for AMD's query buffer (@extension{AMD,query_buffer_object})
@requires_gles30 %Extension @es_extension{EXT,occlusion_query_boolean}
*/
class MAGNUM_EXPORT AbstractQuery {
    public:
        /**
         * @brief Constructor
         *
         * Generates one OpenGL query.
         * @see @fn_gl{GenQueries}
         */
        inline AbstractQuery() {
            /** @todo Get some extension wrangler instead to avoid undeclared glGenQueries() on ES2 */
            #ifndef MAGNUM_TARGET_GLES2
            glGenQueries(1, &_id);
            #endif
        }

        /**
         * @brief Destructor
         *
         * Deletes assigned OpenGL query.
         * @see @fn_gl{DeleteQueries}
         */
        virtual inline ~AbstractQuery() {
            /** @todo Get some extension wrangler instead to avoid undeclared glGenQueries() on ES2 */
            #ifndef MAGNUM_TARGET_GLES2
            glDeleteQueries(1, &_id);
            #endif
        }

        /** @brief OpenGL query ID */
        inline GLuint id() const { return _id; }

        /**
         * @brief Whether the result is available
         *
         * @see @fn_gl{GetQueryObject} with @def_gl{QUERY_RESULT_AVAILABLE}
         */
        bool resultAvailable();

        /**
         * @brief Result
         * @tparam T Result type. Can be either `bool`, `GLuint`,
         *      `GLint`, `GLuint64` or `GLint64`.
         *
         * Note that this function is blocking until the result is available.
         * See resultAvailable().
         * @see @fn_gl{GetQueryObject} with @def_gl{QUERY_RESULT}
         * @requires_gl33 Extension @extension{ARB,timer_query} (result type `GLuint64` and `GLint64`)
         * @requires_gl Result types @c GLint, @c GLuint64 and @c GLint64 are
         *      not available in OpenGL ES.
         */
        template<class T> T result();

    private:
        GLuint _id;
};


#ifndef DOXYGEN_GENERATING_OUTPUT
template<> bool MAGNUM_EXPORT AbstractQuery::result<bool>();
template<> GLuint MAGNUM_EXPORT AbstractQuery::result<GLuint>();
#ifndef MAGNUM_TARGET_GLES
template<> GLint MAGNUM_EXPORT AbstractQuery::result<GLint>();
template<> GLuint64 MAGNUM_EXPORT AbstractQuery::result<GLuint64>();
template<> GLint64 MAGNUM_EXPORT AbstractQuery::result<GLint64>();
#endif
#endif

#ifndef MAGNUM_TARGET_GLES2
/**
@brief %Query for primitives and elapsed time

Queries count of generated primitives from vertex shader, geometry shader or
transform feedback and elapsed time. Example usage:
@code
Query q;

q.begin(Query::Target::PrimitivesGenerated);
// rendering...
q.end();

if(!q.resultAvailable()) {
    // do some work until to give OpenGL some time...
}

// ...or block until the result is available
GLuint primitiveCount = q.result<GLuint>();
@endcode
@requires_gl30 Extension @extension{EXT,transform_feedback}
@requires_gles30 Only sample queries are available on OpenGL ES 2.0.
*/
class MAGNUM_EXPORT Query: public AbstractQuery {
    public:
        /** @brief %Query target */
        enum Target: GLenum {
            #ifndef MAGNUM_TARGET_GLES
            /**
             * Count of primitives generated from vertex shader or geometry
             * shader.
             * @requires_gl Only transform feedback query is available in
             *      OpenGL ES.
             */
            PrimitivesGenerated = GL_PRIMITIVES_GENERATED,
            #endif

            /** Count of primitives written to transform feedback buffer. */
            TransformFeedbackPrimitivesWritten = GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN

            #ifndef MAGNUM_TARGET_GLES
            ,

            /**
             * Elapsed time
             * @requires_gl33 Extension @extension{ARB,timer_query}
             * @requires_gl Only transform feedback query is available in
             *      OpenGL ES.
             */
            TimeElapsed = GL_TIME_ELAPSED
            #endif
        };

        inline Query(): target(nullptr) {}

        inline ~Query() { delete target; }

        /**
         * @brief Begin query
         *
         * Begins counting of given @p target until end() is called.
         * @see @fn_gl{BeginQuery}
         */
        void begin(Target target);

        /**
         * @brief End query
         *
         * The result can be then retrieved by calling result().
         * @see @fn_gl{EndQuery}
         */
        void end();

    private:
        Target* target;
};
#endif

/**
@brief %Query for samples

Queries count of samples passed from fragment shader or boolean value
indicating whether any samples passed. Can be used for example for conditional
rendering:
@code
SampleQuery q;

q.begin(SampleQuery::Target::AnySamplesPassed);
// render simplified object to test whether it is visible at all...
q.end();

// render full version of the object only if it is visible
if(q.result<bool>()) {
    // ...
}
@endcode
This approach has some drawbacks, as the rendering is blocked until result is
available for the CPU to decide. This can be improved by using conditional
rendering on GPU itself. The drawing commands will be sent to the GPU and
processed or discarded later, so CPU can continue executing the code without
waiting for the result.
@code
SampleQuery q;

q.begin(SampleQuery::Target::AnySamplesPassed);
// render simplified object to test whether it is visible at all...
q.end();

q.beginConditionalRender(SampleQuery::ConditionalRenderMode::Wait);
// render full version of the object only if the query returns nonzero result
q.endConditionalRender();
@endcode
@requires_gles30 %Extension @es_extension{EXT,occlusion_query_boolean}
*/
class MAGNUM_EXPORT SampleQuery: public AbstractQuery {
    public:
        /** @brief %Query target */
        enum Target: GLenum {
            #ifndef MAGNUM_TARGET_GLES
            /**
             * Count of samples passed from fragment shader
             * @requires_gl Only boolean query is available in OpenGL ES.
             */
            SamplesPassed = GL_SAMPLES_PASSED,
            #endif

            /**
             * Whether any samples passed from fragment shader
             * @requires_gl33 Extension @extension{ARB,occlusion_query2}
             */
            #ifndef MAGNUM_TARGET_GLES2
            AnySamplesPassed = GL_ANY_SAMPLES_PASSED,
            #else
            AnySamplesPassed = GL_ANY_SAMPLES_PASSED_EXT,
            #endif

            /**
             * Whether any samples passed from fragment shader (conservative)
             *
             * An implementation may choose a less precise version of the
             * test at the expense of some false positives.
             * @requires_gl43 Extension @extension{ARB,ES3_compatibility}
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
         * @requires_gl30 Extension @extension{NV,conditional_render}
         * @requires_gl Conditional rendering is not available in OpenGL ES.
         */
        enum class ConditionalRenderMode: GLenum {
            /**
             * If query result is not yet available, waits for it and
             * then begins conditional rendering based on result value.
             */
            Wait = GL_QUERY_WAIT,

            /**
             * If query result is not yet available, OpenGL may begin
             * rendering like if the result value was nonzero.
             */
            NoWait = GL_QUERY_NO_WAIT,

            /**
             * The same as Wait, but regions untouched by the sample query may
             * not be rendered at all.
             */
            ByRegionWait = GL_QUERY_BY_REGION_WAIT,

            /**
             * The same as NoWait, but regions untouched by the sample query
             * may not be rendered at all.
             */
            ByRegionNoWait = GL_QUERY_BY_REGION_NO_WAIT
        };
        #endif

        inline SampleQuery(): target(nullptr) {}

        inline ~SampleQuery() { delete target; }

        /** @copydoc Query::begin() */
        void begin(Target target);

        /** @copydoc Query::end() */
        void end();

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Begin conditional rendering based on result value
         *
         * @see @fn_gl{BeginConditionalRender}
         * @requires_gl30 Extension @extension{NV,conditional_render}
         * @requires_gl Conditional rendering is not available in OpenGL ES.
         */
        inline void beginConditionalRender(ConditionalRenderMode mode) {
            glBeginConditionalRender(id(), static_cast<GLenum>(mode));
        }

        /**
         * @brief End conditional render
         *
         * @see @fn_gl{EndConditionalRender}
         * @requires_gl30 Extension @extension{NV,conditional_render}
         * @requires_gl Conditional rendering is not available in OpenGL ES.
         */
        inline void endConditionalRender() {
            glEndConditionalRender();
        }
        #endif

    private:
        Target* target;
};

#ifndef MAGNUM_TARGET_GLES
/**
@brief %Query for elapsed time

Queries timestamp after all previous OpenGL calls have been processed. It is
similar to Query::Target::TimeElapsed query, but this query just retrieves
timestamp, not time duration between Query::begin() and Query::end() calls.
Example usage, compared to Query::Target::TimeElapsed:
@code
Query q1, q2;
q1.begin(Query::Target::TimeElapsed);
// rendering...
q1.end();
q2.begin(Query::Target::TimeElapsed);
// another rendering...
q2.end();
GLuint timeElapsed1 = q1.result<GLuint>();
GLuint timeElapsed2 = q2.result<GLuint>();
@endcode
@code
TimeQuery q1, q2, q3;
q1.timestamp();
// rendering...
q2.timestamp();
// another rendering...
q3.timestamp();
GLuint tmp = q2.result<GLuint>();
GLuint timeElapsed1 = tmp-q1.result<GLuint>();
GLuint timeElapsed2 = q3.result<GLuint>()-tmp;
@endcode
Using this query results in fewer OpenGL calls when doing more measures.
@requires_gl33 Extension @extension{ARB,timer_query}
@requires_gl Timer query is not available in OpenGL ES.
*/
class TimeQuery: public AbstractQuery {
    public:
        /**
         * @brief Query timestamp
         *
         * @see @fn_gl{QueryCounter} with @def_gl{TIMESTAMP}
         */
        inline void timestamp() {
            glQueryCounter(id(), GL_TIMESTAMP);
        }
};
#endif

}

#endif
