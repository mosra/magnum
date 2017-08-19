#ifndef Magnum_TimeQuery_h
#define Magnum_TimeQuery_h
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

#ifndef MAGNUM_TARGET_WEBGL
/** @file
 * @brief Class @ref Magnum::TimeQuery
 */
#endif

#include "Magnum/AbstractQuery.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>
#endif

#ifndef MAGNUM_TARGET_WEBGL
namespace Magnum {

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
@requires_gl33 Extension @extension{ARB,timer_query}
@requires_es_extension Extension @extension{EXT,disjoint_timer_query}
@requires_gles Time query is not available in WebGL.

@see @ref PrimitiveQuery, @ref SampleQuery
@todo timestamp with glGet + example usage
@todo @extension{EXT,disjoint_timer_query} -- GL_GPU_DISJOINT_EXT support? where?
*/
class TimeQuery: public AbstractQuery {
    public:
        /** @brief Query target */
        enum class Target: GLenum {
            /**
             * Elapsed time. Use @ref result<UnsignedLong>() or @ref result<Long>()
             * to retrieve the result.
             * @see @ref timestamp()
             */
            #ifndef MAGNUM_TARGET_GLES
            TimeElapsed = GL_TIME_ELAPSED,
            #else
            TimeElapsed = GL_TIME_ELAPSED_EXT,
            #endif

            /**
             * Timestamp. For use with @ref timestamp() only, use
             * @ref result<UnsignedLong>() or @ref result<Long>() to retrieve
             * the result.
             */
            #ifndef MAGNUM_TARGET_GLES
            Timestamp = GL_TIMESTAMP
            #else
            Timestamp = GL_TIMESTAMP_EXT
            #endif
        };

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief TimeQuery(Target)
         * @deprecated Use @ref TimeQuery(Target) instead.
         */
        CORRADE_DEPRECATED("use TimeQuery(Target) instead") explicit TimeQuery() {}
        #endif

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
        static TimeQuery wrap(GLuint id, Target target, ObjectFlags flags = {}) {
            return TimeQuery{id, target, flags};
        }

        /**
         * @brief Constructor
         *
         * Creates new OpenGL query object. If @extension{ARB,direct_state_access}
         * (part of OpenGL 4.5) is not available, the query is created on first
         * use.
         * @see @ref TimeQuery(NoCreateT), @ref wrap(), @fn_gl{CreateQueries},
         *      eventually @fn_gl{GenQueries}
         */
        explicit TimeQuery(Target target): AbstractQuery(GLenum(target)) {}

        /**
         * @brief Construct without creating the underlying OpenGL object
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         *
         * This function can be safely used for constructing (and later
         * destructing) objects even without any OpenGL context being active.
         * @see @ref TimeQuery(Target), @ref wrap()
         */
        explicit TimeQuery(NoCreateT) noexcept: AbstractQuery{NoCreate, GLenum(Target::TimeElapsed)} {}

        /* Overloads to remove WTF-factor from method chaining order */
        #if !defined(DOXYGEN_GENERATING_OUTPUT) && !defined(MAGNUM_TARGET_WEBGL)
        TimeQuery& setLabel(const std::string& label) {
            AbstractQuery::setLabel(label);
            return *this;
        }
        template<std::size_t size> TimeQuery& setLabel(const char(&label)[size]) {
            AbstractQuery::setLabel<size>(label);
            return *this;
        }
        #endif

        /**
         * @brief Query timestamp
         *
         * Use @ref result<UnsignedLong>() or @ref result<Long>() to retrieve
         * the result.
         * @see @fn_gl{QueryCounter} with @def_gl{TIMESTAMP}
         */
        void timestamp() {
            #ifndef MAGNUM_TARGET_GLES
            glQueryCounter(id(), GL_TIMESTAMP);
            #elif !defined(CORRADE_TARGET_EMSCRIPTEN)
            glQueryCounterEXT(id(), GL_TIMESTAMP_EXT);
            #else
            CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
            #endif
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief AbstractQuery::begin()
         * @deprecated Use @ref AbstractQuery::begin() instead.
         */
        CORRADE_DEPRECATED("use begin() instead") void begin(Target target) {
            AbstractQuery::begin(GLenum(target));
        }

        using AbstractQuery::begin;
        #endif

    private:
        explicit TimeQuery(GLuint id, Target target, ObjectFlags flags) noexcept: AbstractQuery{id, GLenum(target), flags} {}
};

}
#else
#error this header is not available in WebGL 1.0 build
#endif

#endif
