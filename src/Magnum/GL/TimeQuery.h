#ifndef Magnum_GL_TimeQuery_h
#define Magnum_GL_TimeQuery_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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
 * @brief Class @ref Magnum::GL::TimeQuery
 */
#endif

#include "Magnum/GL/AbstractQuery.h"

#ifndef MAGNUM_TARGET_WEBGL
namespace Magnum { namespace GL {

/**
@brief Query for elapsed time

Queries timestamp after all previous OpenGL calls have been processed. It can
query either duration of sequence of commands or absolute timestamp. Example
usage of both methods:

@snippet MagnumGL.cpp TimeQuery-usage1

@snippet MagnumGL.cpp TimeQuery-usage2

Using the latter results in fewer OpenGL calls when doing more measures.

@requires_gl33 Extension @gl_extension{ARB,timer_query}
@requires_es_extension Extension @gl_extension{EXT,disjoint_timer_query}
@requires_gles Time query is not available in WebGL.

@see @ref PrimitiveQuery, @ref SampleQuery
@todo timestamp with glGet + example usage
@todo @gl_extension{EXT,disjoint_timer_query} --- GL_GPU_DISJOINT_EXT support? where?
*/
class TimeQuery: public AbstractQuery {
    public:
        /**
         * @brief Query target
         *
         * @m_enum_values_as_keywords
         */
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
         * Creates new OpenGL query object. If @gl_extension{ARB,direct_state_access}
         * (part of OpenGL 4.5) is not available, the query is created on first
         * use.
         * @see @ref TimeQuery(NoCreateT), @ref wrap(), @fn_gl_keyword{CreateQueries},
         *      eventually @fn_gl_keyword{GenQueries}
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

        /** @brief Copying is not allowed */
        TimeQuery(const TimeQuery&) = delete;

        /** @brief Move constructor */
        TimeQuery(TimeQuery&&) noexcept = default;

        /** @brief Copying is not allowed */
        TimeQuery& operator=(const TimeQuery&) = delete;

        /** @brief Move assignment */
        TimeQuery& operator=(TimeQuery&&) noexcept = default;

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
         * @see @fn_gl_keyword{QueryCounter} with @def_gl{TIMESTAMP}
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

    private:
        explicit TimeQuery(GLuint id, Target target, ObjectFlags flags) noexcept: AbstractQuery{id, GLenum(target), flags} {}
};

}

#ifdef MAGNUM_BUILD_DEPRECATED
/* Note: needs to be prefixed with Magnum:: otherwise Doxygen can't find it */

/** @brief @copybrief GL::TimeQuery
 * @deprecated Use @ref GL::TimeQuery instead.
 */
typedef CORRADE_DEPRECATED("use GL::TimeQuery instead") Magnum::GL::TimeQuery TimeQuery;
#endif

}
#else
#error this header is not available in WebGL 1.0 build
#endif

#endif
