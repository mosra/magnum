#ifndef Magnum_AbstractQuery_h
#define Magnum_AbstractQuery_h
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

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
/** @file
 * @brief Class @ref Magnum::AbstractQuery
 */
#endif

#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Utility/Assert.h>

#include "Magnum/AbstractObject.h"
#include "Magnum/Tags.h"
#include "Magnum/configure.h"

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
namespace Magnum {

namespace Implementation { struct QueryState; }

/**
@brief Base class for queries

See @ref PrimitiveQuery, @ref SampleQuery and @ref TimeQuery documentation for
more information.
@requires_webgl20 Queries are not available in WebGL 1.0.
@todo `QUERY_COUNTER_BITS` (not sure since when this is supported)
*/
class MAGNUM_EXPORT AbstractQuery: public AbstractObject {
    friend Implementation::QueryState;

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
         * @brief Release OpenGL object
         *
         * Releases ownership of OpenGL query object and returns its ID so it
         * is not deleted on destruction. The internal state is then equivalent
         * to moved-from state.
         * @see @ref PrimitiveQuery::wrap(), @ref SampleQuery::wrap(),
         *      @ref TimeQuery::wrap()
         */
        GLuint release();

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Query label
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls. If OpenGL 4.3 is not supported and neither
         * @extension{KHR,debug} (covered also by @extension{ANDROID,extension_pack_es31a})
         * nor @extension{EXT,debug_label} desktop or ES extension is
         * available, this function returns empty string.
         * @see @fn_gl{GetObjectLabel} with @def_gl{QUERY} or
         *      @fn_gl_extension{GetObjectLabel,EXT,debug_label} with
         *      @def_gl{QUERY_OBJECT_EXT}
         * @requires_gles Debug output is not available in WebGL.
         */
        std::string label() const;

        /**
         * @brief Set query label
         * @return Reference to self (for method chaining)
         *
         * Default is empty string. If OpenGL 4.3 is not supported and neither
         * @extension{KHR,debug} (covered also by @extension{ANDROID,extension_pack_es31a})
         * nor @extension{EXT,debug_label} desktop or ES extension is
         * available, this function does nothing.
         * @see @ref maxLabelLength(), @fn_gl{ObjectLabel} with
         *      @def_gl{QUERY} or @fn_gl_extension{LabelObject,EXT,debug_label}
         *      with @def_gl{QUERY_OBJECT_EXT}
         * @requires_gles Debug output is not available in WebGL.
         */
        AbstractQuery& setLabel(const std::string& label) {
            return setLabelInternal({label.data(), label.size()});
        }

        /** @overload */
        template<std::size_t size> AbstractQuery& setLabel(const char(&label)[size]) {
            return setLabelInternal({label, size - 1});
        }
        #endif

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
         * See @ref resultAvailable().
         * @see @fn_gl{GetQueryObject} with @def_gl{QUERY_RESULT}
         * @requires_gl33 Extension @extension{ARB,timer_query} for result
         *      type @ref Magnum::UnsignedLong "UnsignedLong" and @ref Magnum::Long "Long"
         * @requires_es_extension Extension @extension{EXT,disjoint_timer_query}
         *      for result types @ref Magnum::Int "Int", @ref Magnum::UnsignedLong "UnsignedLong"
         *      and @ref Magnum::Long "Long".
         * @requires_gles Only @ref Magnum::UnsignedInt "UnsignedInt" result
         *      type is available in WebGL.
         */
        template<class T> T result();

        /**
         * @brief Begin query
         *
         * Begins counting until @ref end() is called.
         * @see @fn_gl{BeginQuery}
         */
        void begin();

        /**
         * @brief End query
         *
         * The result can be then retrieved by calling @ref result().
         * @see @fn_gl{EndQuery}
         */
        void end();

    protected:
        /**
         * @brief Destructor
         *
         * Deletes assigned OpenGL query.
         * @see @ref PrimitiveQuery::wrap(), @ref SampleQuery::wrap(),
         *      @ref TimeQuery::wrap(), @ref release(), @fn_gl{DeleteQueries}
         */
        ~AbstractQuery();

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #endif
        explicit AbstractQuery(GLenum target);
        explicit AbstractQuery(NoCreateT, GLenum target) noexcept: _id{0}, _target{target}, _flags{ObjectFlag::DeleteOnDestruction} {}
        explicit AbstractQuery(GLuint id, GLenum target, ObjectFlags flags) noexcept: _id{id}, _target{target}, _flags{flags} {}

        #ifdef MAGNUM_BUILD_DEPRECATED
        explicit AbstractQuery();
        void begin(GLenum target);
        #endif

        GLuint _id;
        GLenum _target;

    private:
        #ifndef MAGNUM_TARGET_WEBGL
        AbstractQuery& setLabelInternal(Containers::ArrayView<const char> label);
        #endif

        void MAGNUM_LOCAL createImplementationDefault();
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL createImplementationDSA();
        #endif

        ObjectFlags _flags;
};

#ifndef DOXYGEN_GENERATING_OUTPUT
template<> bool MAGNUM_EXPORT AbstractQuery::result<bool>();
template<> UnsignedInt MAGNUM_EXPORT AbstractQuery::result<UnsignedInt>();
template<> Int MAGNUM_EXPORT AbstractQuery::result<Int>();
#ifndef MAGNUM_TARGET_WEBGL
template<> UnsignedLong MAGNUM_EXPORT AbstractQuery::result<UnsignedLong>();
template<> Long MAGNUM_EXPORT AbstractQuery::result<Long>();
#endif
#endif

inline AbstractQuery::AbstractQuery(AbstractQuery&& other) noexcept: _id(other._id), _target(other._target) {
    other._id = 0;
}

inline AbstractQuery& AbstractQuery::operator=(AbstractQuery&& other) noexcept {
    using std::swap;
    swap(_id, other._id);
    swap(_target, other._target);
    return *this;
}

inline GLuint AbstractQuery::release() {
    const GLuint id = _id;
    _id = 0;
    return id;
}

}
#else
#error this header is not available in WebGL 1.0 build
#endif

#endif
