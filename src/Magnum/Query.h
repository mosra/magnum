#ifndef Magnum_Query_h
#define Magnum_Query_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::AbstractQuery, @ref Magnum::PrimitiveQuery, @ref Magnum::SampleQuery, @ref Magnum::TimeQuery
 */

#include <Corrade/Utility/Assert.h>

#include "AbstractObject.h"
#include "magnumConfigure.h"

namespace Magnum {

/**
@brief Base class for queries

See @ref PrimitiveQuery, @ref SampleQuery and @ref TimeQuery documentation for
more information.
@todo Support for AMD's query buffer (@extension{AMD,query_buffer_object})
*/
class MAGNUM_EXPORT AbstractQuery: public AbstractObject {
    public:
        /** @brief Copying is not allowed */
        AbstractQuery(const AbstractQuery&) = delete;

        /** @brief Move constructor */
        AbstractQuery(AbstractQuery&& other) noexcept;

        /** @brief Copying is not allowed */
        AbstractQuery& operator=(const AbstractQuery&) = delete;

        /** @brief Move assignment */
        AbstractQuery& operator=(AbstractQuery&& other) noexcept;

        /** @brief OpenGL query ID */
        GLuint id() const { return _id; }

        /**
         * @brief %Query label
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls. If neither @extension{KHR,debug} nor
         * @extension2{EXT,debug_label} desktop or ES extension is available,
         * this function returns empty string.
         * @see @fn_gl{GetObjectLabel} with @def_gl{QUERY} or
         *      @fn_gl_extension2{GetObjectLabel,EXT,debug_label} with
         *      @def_gl{QUERY_OBJECT_EXT}
         */
        std::string label() const;

        /**
         * @brief Set query label
         * @return Reference to self (for method chaining)
         *
         * Default is empty string. If neither @extension{KHR,debug} nor
         * @extension2{EXT,debug_label} desktop or ES extension is available,
         * this function does nothing.
         * @see @ref maxLabelLength(), @fn_gl{ObjectLabel} with
         *      @def_gl{QUERY} or @fn_gl_extension2{LabelObject,EXT,debug_label}
         *      with @def_gl{QUERY_OBJECT_EXT}
         */
        AbstractQuery& setLabel(const std::string& label);

        /**
         * @brief Whether the result is available
         *
         * @see @fn_gl{GetQueryObject} with @def_gl{QUERY_RESULT_AVAILABLE}
         */
        bool resultAvailable();

        /**
         * @brief Result
         * @tparam T Result type. Can be either `bool`, @ref UnsignedInt,
         *      @ref Int, @ref UnsignedLong or @ref Long.
         *
         * Note that this function is blocking until the result is available.
         * See resultAvailable().
         * @see @fn_gl{GetQueryObject} with @def_gl{QUERY_RESULT}
         * @requires_gl33 %Extension @extension{ARB,timer_query} for result
         *      type @ref Magnum::UnsignedInt "UnsignedInt" and @ref Magnum::Long
         *      "Long"
         * @requires_es_extension %Extension @es_extension{EXT,disjoint_timer_query}
         *      for result types @ref Magnum::Int "Int", @ref Magnum::UnsignedLong "UnsignedLong"
         *      @ref Magnum::Long "Long".
         */
        template<class T> T result();

        /**
         * @brief End query
         *
         * The result can be then retrieved by calling @ref result().
         * @see @fn_gl{EndQuery}
         */
        void end();

    protected:
        /**
         * @brief Constructor
         *
         * Generates one OpenGL query.
         * @see @fn_gl{GenQueries}
         */
        explicit AbstractQuery();

        /**
         * @brief Destructor
         *
         * Deletes assigned OpenGL query.
         * @see @fn_gl{DeleteQueries}
         */
        ~AbstractQuery();

        void begin(GLenum target);

    private:
        GLuint _id;
        GLenum target;
};


#ifndef DOXYGEN_GENERATING_OUTPUT
template<> bool MAGNUM_EXPORT AbstractQuery::result<bool>();
template<> UnsignedInt MAGNUM_EXPORT AbstractQuery::result<UnsignedInt>();
template<> Int MAGNUM_EXPORT AbstractQuery::result<Int>();
template<> UnsignedLong MAGNUM_EXPORT AbstractQuery::result<UnsignedLong>();
template<> Long MAGNUM_EXPORT AbstractQuery::result<Long>();
#endif

#ifndef MAGNUM_TARGET_GLES2
/**
@brief Query for primitives and elapsed time

Queries count of generated primitives from vertex shader, geometry shader or
transform feedback and elapsed time. Example usage:
@code
PrimitiveQuery q;

q.begin(PrimitiveQuery::Target::PrimitivesGenerated);
// rendering...
q.end();

if(!q.resultAvailable()) {
    // do some work until to give OpenGL some time...
}

// ...or block until the result is available
UnsignedInt primitiveCount = q.result<UnsignedInt>();
@endcode
@requires_gl30 %Extension @extension{EXT,transform_feedback}
@requires_gles30 Only sample queries are available on OpenGL ES 2.0.

@see @ref SampleQuery, @ref TimeQuery
@todo glBeginQueryIndexed
*/
class PrimitiveQuery: public AbstractQuery {
    public:
        /** @brief Query target */
        enum class Target: GLenum {
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
        };

        explicit PrimitiveQuery() {}

        /**
         * @brief Begin query
         *
         * Begins counting of given @p target until @ref end() is called.
         * @see @fn_gl{BeginQuery}
         */
        void begin(Target target) {
            AbstractQuery::begin(GLenum(target));
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        PrimitiveQuery& setLabel(const std::string& label) {
            AbstractQuery::setLabel(label);
            return *this;
        }
        #endif
};
#endif

/**
@brief Query for samples

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

@see @ref PrimitiveQuery, @ref TimeQuery
*/
class SampleQuery: public AbstractQuery {
    public:
        /** @brief Query target */
        enum class Target: GLenum {
            #ifndef MAGNUM_TARGET_GLES
            /**
             * Count of samples passed from fragment shader
             * @requires_gl Only boolean query is available in OpenGL ES.
             */
            SamplesPassed = GL_SAMPLES_PASSED,
            #endif

            /**
             * Whether any samples passed from fragment shader
             * @requires_gl33 %Extension @extension{ARB,occlusion_query2}
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
             * @requires_gl43 %Extension @extension{ARB,ES3_compatibility}
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
         * @requires_gl30 %Extension @extension{NV,conditional_render}
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

        explicit SampleQuery() {}

        /** @copydoc PrimitiveQuery::begin() */
        void begin(Target target) {
            AbstractQuery::begin(GLenum(target));
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Begin conditional rendering based on result value
         *
         * @see @fn_gl{BeginConditionalRender}
         * @requires_gl30 %Extension @extension{NV,conditional_render}
         * @requires_gl Conditional rendering is not available in OpenGL ES.
         */
        void beginConditionalRender(ConditionalRenderMode mode) {
            glBeginConditionalRender(id(), GLenum(mode));
        }

        /**
         * @brief End conditional render
         *
         * @see @fn_gl{EndConditionalRender}
         * @requires_gl30 %Extension @extension{NV,conditional_render}
         * @requires_gl Conditional rendering is not available in OpenGL ES.
         */
        void endConditionalRender() {
            glEndConditionalRender();
        }
        #endif

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        SampleQuery& setLabel(const std::string& label) {
            AbstractQuery::setLabel(label);
            return *this;
        }
        #endif
};

/**
@brief Query for elapsed time

Queries timestamp after all previous OpenGL calls have been processed. It can
query either duration of sequence of commands or absolute timestamp. Example
usage of both methods:
@code
TimeQuery q1, q2;
q1.begin(TimeQuery::Target::TimeElapsed);
// rendering...
q1.end();
q2.begin(TimeQuery::Target::TimeElapsed);
// another rendering...
q2.end();
UnsignedInt timeElapsed1 = q1.result<UnsignedInt>();
UnsignedInt timeElapsed2 = q2.result<UnsignedInt>();
@endcode
@code
TimeQuery q1, q2, q3;
q1.timestamp();
// rendering...
q2.timestamp();
// another rendering...
q3.timestamp();
UnsignedInt tmp = q2.result<UnsignedInt>();
UnsignedInt timeElapsed1 = tmp-q1.result<UnsignedInt>();
UnsignedInt timeElapsed2 = q3.result<UnsignedInt>()-tmp;
@endcode
Using the latter results in fewer OpenGL calls when doing more measures.
@requires_gl33 %Extension @extension{ARB,timer_query}
@requires_es_extension %Extension @es_extension{EXT,disjoint_timer_query}

@see @ref PrimitiveQuery, @ref SampleQuery
@todo timestamp with glGet + example usage
@todo @es_extension{EXT,disjoint_timer_query} -- GL_GPU_DISJOINT_EXT support? where?
*/
class TimeQuery: public AbstractQuery {
    public:
        /** @brief Query target */
        enum class Target: GLenum {
            /** Elapsed time */
            #ifndef MAGNUM_TARGET_GLES
            TimeElapsed = GL_TIME_ELAPSED
            #else
            TimeElapsed = GL_TIME_ELAPSED_EXT
            #endif
        };

        explicit TimeQuery() {}

        /**
         * @brief Query timestamp
         *
         * @see @fn_gl{QueryCounter} with @def_gl{TIMESTAMP}
         */
        void timestamp() {
            /** @todo Enable when extension wrangler for ES is available */
            #ifndef MAGNUM_TARGET_GLES
            glQueryCounter(id(), GL_TIMESTAMP);
            #else
            //glQueryCounterEXT(id(), GL_TIMESTAMP);
            CORRADE_INTERNAL_ASSERT(false);
            #endif
        }

        /** @copydoc PrimitiveQuery::begin() */
        void begin(Target target) {
            AbstractQuery::begin(GLenum(target));
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        TimeQuery& setLabel(const std::string& label) {
            AbstractQuery::setLabel(label);
            return *this;
        }
        #endif
};

inline AbstractQuery::AbstractQuery(AbstractQuery&& other) noexcept: _id(other._id), target(other.target) {
    other._id = 0;
}

inline AbstractQuery& AbstractQuery::operator=(AbstractQuery&& other) noexcept {
    std::swap(_id, other._id);
    std::swap(target, other.target);
    return *this;
}

}

#endif
