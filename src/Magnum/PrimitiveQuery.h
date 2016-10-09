#ifndef Magnum_PrimitiveQuery_h
#define Magnum_PrimitiveQuery_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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

#ifndef MAGNUM_TARGET_GLES2
/** @file
 * @brief Class @ref Magnum::PrimitiveQuery
 */
#endif

#include "Magnum/AbstractQuery.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>
#endif

#ifndef MAGNUM_TARGET_GLES2
namespace Magnum {

/**
@brief Query for primitives

Queries count of generated primitives from vertex shader, geometry shader or
transform feedback. Example usage:
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
@see @ref SampleQuery, @ref TimeQuery, @ref TransformFeedback
@requires_gl30 Extension @extension{EXT,transform_feedback}
@requires_gles30 Only sample queries are available in OpenGL ES 2.0.
@requires_webgl20 Queries are not available in WebGL 1.0.
@todo glBeginQueryIndexed
@todo @extension{ARB,transform_feedback_overflow_query}
*/
class PrimitiveQuery: public AbstractQuery {
    public:
        /** @brief Query target */
        enum class Target: GLenum {
            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Count of primitives generated from vertex shader or geometry
             * shader. Use @ref result<UnsignedInt>() or @ref result<Int>() to
             * retrieve the result.
             * @requires_gles30 Not defined in OpenGL ES 2.0.
             * @requires_es_extension Extension @es_extension{ANDROID,extension_pack_es31a}/
             *      @es_extension{EXT,geometry_shader}
             * @requires_gles Geometry shaders are not available in WebGL.
             */
            #ifndef MAGNUM_TARGET_GLES
            PrimitivesGenerated = GL_PRIMITIVES_GENERATED,
            #else
            PrimitivesGenerated = GL_PRIMITIVES_GENERATED_EXT,
            #endif
            #endif

            /**
             * Count of primitives written to transform feedback buffer. Use
             * @ref result<UnsignedInt>() or @ref result<Int>() to retrieve the
             * result.
             */
            TransformFeedbackPrimitivesWritten = GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN
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

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief PrimitiveQuery(Target)
         * @deprecated Use @ref PrimitiveQuery(Target) instead.
         */
        CORRADE_DEPRECATED("use PrimitiveQuery(Target) instead") explicit PrimitiveQuery() {}
        #endif

        /**
         * @brief Constructor
         *
         * Creates new OpenGL query object. If @extension{ARB,direct_state_access}
         * (part of OpenGL 4.5) is not available, the query is created on first
         * use.
         * @see @ref PrimitiveQuery(NoCreateT), @ref wrap(),
         *      @fn_gl{CreateQueries}, eventually @fn_gl{GenQueries}
         */
        explicit PrimitiveQuery(Target target): AbstractQuery(GLenum(target)) {}

        /**
         * @brief Construct without creating the underlying OpenGL object
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         * @see @ref PrimitiveQuery(Target), @ref wrap()
         */
        explicit PrimitiveQuery(NoCreateT) noexcept: AbstractQuery{NoCreate, GLenum(Target::TransformFeedbackPrimitivesWritten)} {}

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief AbstractQuery::begin()
         * @deprecated Use @ref begin() instead.
         */
        CORRADE_DEPRECATED("use begin() instead") void begin(Target target) {
            AbstractQuery::begin(GLenum(target));
        }

        using AbstractQuery::begin;
        #endif

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
};

}
#else
#error this header is not available in OpenGL ES 2.0 build
#endif

#endif
