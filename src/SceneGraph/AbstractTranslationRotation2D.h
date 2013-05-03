#ifndef Magnum_SceneGraph_AbstractTranslationRotation2D_h
#define Magnum_SceneGraph_AbstractTranslationRotation2D_h
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
 * @brief Class Magnum::SceneGraph::AbstractTranslationRotation2D
 */

#include "AbstractTransformation.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Base for two-dimensional transformations supporting translation and rotation

@see @ref scenegraph, AbstractTranslationRotation3D
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T>
#else
template<class T = Float>
#endif
class AbstractTranslationRotation2D: public AbstractTransformation<2, T> {
    public:
        explicit AbstractTranslationRotation2D() = default;

        /**
         * @brief Translate object
         * @param vector    Translation vector
         * @param type      Transformation type
         * @return Pointer to self (for method chaining)
         *
         * @see Vector2::xAxis(), Vector2::yAxis()
         */
        virtual AbstractTranslationRotation2D<T>* translate(const Math::Vector2<T>& vector, TransformationType type = TransformationType::Global) = 0;

        /**
         * @brief Rotate object
         * @param angle     Angle (counterclockwise)
         * @param type      Transformation type
         * @return Pointer to self (for method chaining)
         */
        virtual AbstractTranslationRotation2D<T>* rotate(Math::Rad<T> angle, TransformationType type = TransformationType::Global) = 0;

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        AbstractTranslationRotation2D<T>* resetTransformation() override = 0;
        #endif
};

}}

#endif
