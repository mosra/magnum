#ifndef Magnum_PrimitiveQuery_h
#define Magnum_PrimitiveQuery_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief PrimitiveQuery(Target)
         * @deprecated Use @ref Magnum::PrimitiveQuery::PrimitiveQuery(Target) "PrimitiveQuery(Target)"
         *      instead.
         */
        CORRADE_DEPRECATED("use PrimitiveQuery(Target) instead") explicit PrimitiveQuery() {}
        #endif

        /**
         * @brief Constructor
         *
         * Creates new OpenGL query object. If @extension{ARB,direct_state_access}
         * (part of OpenGL 4.5) is not supported, the query is created on first
         * use.
         * @see @fn_gl{CreateQueries}, eventually @fn_gl{GenQueries}
         */
        explicit PrimitiveQuery(Target target): AbstractQuery(GLenum(target)) {}

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief AbstractQuery::begin()
         * @deprecated Use @ref Magnum::AbstractQuery::begin() "begin()"
         *      instead.
         */
        CORRADE_DEPRECATED("use begin() instead") void begin(Target target) {
            AbstractQuery::begin(GLenum(target));
        }

        using AbstractQuery::begin;
        #endif

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        PrimitiveQuery& setLabel(const std::string& label) {
            AbstractQuery::setLabel(label);
            return *this;
        }
        template<std::size_t size> PrimitiveQuery& setLabel(const char(&label)[size]) {
            AbstractQuery::setLabel<size>(label);
            return *this;
        }
        #endif
};

}
#endif

#endif
