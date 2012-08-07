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

namespace Magnum {

#ifndef MAGNUM_TARGET_GLES
/** @addtogroup rendering
 * @{
 */

/**
@brief Base class for queries

See Query, SampleQuery, TimeQuery documentation for more information.
@todo Support for AMD's query buffer (@extension{AMD,query_buffer_object})
@requires_gl
*/
class MAGNUM_EXPORT AbstractQuery {
    public:
        /**
         * @brief Constructor
         *
         * Generates one OpenGL query.
         */
        inline AbstractQuery() { glGenQueries(1, &query); }

        /**
         * @brief Destructor
         *
         * Deletes assigned OpenGL query.
         */
        virtual inline ~AbstractQuery() { glDeleteQueries(1, &query); }

        /**
         * @brief Whether the result is available
         */
        bool resultAvailable();

        /**
         * @brief Result
         * @tparam T Result type. Can be either `bool`, `GLuint`,
         *      `GLint`, `GLuint64` or `GLint64`.
         *
         * Note that this function is blocking until the result is available.
         * See resultAvailable().
         *
         * @requires_gl33 Extension @extension{ARB,timer_query} (result type `GLuint64` and `GLint64`)
         */
        template<class T> T result();

    protected:
        GLuint query; /**< @brief OpenGL internal query ID */
};


#ifndef DOXYGEN_GENERATING_OUTPUT
template<> bool MAGNUM_EXPORT AbstractQuery::result<bool>();
template<> GLuint MAGNUM_EXPORT AbstractQuery::result<GLuint>();
template<> GLint MAGNUM_EXPORT AbstractQuery::result<GLint>();
template<> GLuint64 MAGNUM_EXPORT AbstractQuery::result<GLuint64>();
template<> GLint64 MAGNUM_EXPORT AbstractQuery::result<GLint64>();
#endif

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
@requires_gl
*/
class MAGNUM_EXPORT Query: public AbstractQuery {
    public:
        /** @brief %Query target */
        enum Target: GLenum {
            /**
             * Count of primitives generated from vertex shader or geometry
             * shader.
             *
             * @requires_gl31 (no extension providing this functionality)
             */
            PrimitivesGenerated = GL_PRIMITIVES_GENERATED,

            /** Count of primitives written to transform feedback buffer. */
            TransformFeedbackPrimitivesWritten = GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,

            /**
             * Elapsed time
             *
             * @requires_gl33 Extension @extension{ARB,timer_query}
             */
            TimeElapsed = GL_TIME_ELAPSED
        };

        inline Query(): target(nullptr) {}

        inline ~Query() { delete target; }

        /**
         * @brief Begin query
         *
         * Begins counting of given @p target until end() is called.
         */
        void begin(Target target);

        /**
         * @brief End query
         *
         * The result can be then retrieved by calling result().
         */
        void end();

    private:
        Target* target;
};

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
@requires_gl
@requires_gl30 Extension @extension{NV,conditional_render}
*/
class MAGNUM_EXPORT SampleQuery: public AbstractQuery {
    public:
        /** @brief %Query target */
        enum Target: GLenum {
            /** Count of samples passed from fragment shader */
            SamplesPassed = GL_SAMPLES_PASSED,

            /**
             * Whether any samples passed from fragment shader
             *
             * @requires_gl33 Extension @extension{ARB,occlusion_query2}
             */
            AnySamplesPassed = GL_ANY_SAMPLES_PASSED
        };

        /** @brief Conditional render mode */
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

        inline SampleQuery(): target(nullptr) {}

        inline ~SampleQuery() { delete target; }

        /** @copydoc Query::begin() */
        void begin(Target target);

        /** @copydoc Query::end() */
        void end();

        /** @brief Begin conditional rendering based on result value */
        inline void beginConditionalRender(ConditionalRenderMode mode) {
            glBeginConditionalRender(query, static_cast<GLenum>(mode));
        }

        /** @brief End conditional render */
        inline void endConditionalRender() {
            glEndConditionalRender();
        }

    private:
        Target* target;
};

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
@requires_gl
@requires_gl33 Extension @extension{ARB,timer_query}
*/
class TimeQuery: public AbstractQuery {
    public:
        /** @brief Query timestamp */
        inline void timestamp() {
            glQueryCounter(query, GL_TIMESTAMP);
        }
};

/*@}*/
#endif

}

#endif
