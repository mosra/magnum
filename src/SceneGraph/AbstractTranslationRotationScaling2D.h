#ifndef Magnum_SceneGraph_AbstractTranslationRotationScaling2D_h
#define Magnum_SceneGraph_AbstractTranslationRotationScaling2D_h
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
 * @brief Class Magnum::SceneGraph::AbstractBasicTranslationRotationScaling2D, typedef Magnum::SceneGraph::AbstractTranslationRotationScaling2D
 */

#include "AbstractTranslationRotation2D.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Base transformation for two-dimensional scenes supporting translation, rotation and scaling

@see @ref AbstractTranslationRotationScaling2D, @ref scenegraph, @ref AbstractBasicTranslationRotationScaling2D
*/
template<class T> class AbstractBasicTranslationRotationScaling2D: public AbstractBasicTranslationRotation2D<T> {
    public:
        explicit AbstractBasicTranslationRotationScaling2D();

        /**
         * @brief Scale object
         * @param vector    Scaling vector
         * @param type      Transformation type
         * @return Pointer to self (for method chaining)
         *
         * @see Vector2::xScale(), Vector2::yScale()
         */
        AbstractBasicTranslationRotationScaling2D<T>* scale(const Math::Vector2<T>& vector, TransformationType type = TransformationType::Global) {
            doScale(vector, type);
            return this;
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        AbstractBasicTranslationRotationScaling2D<T>* resetTransformation() {
            AbstractBasicTranslationRotation2D<T>::resetTransformation();
            return this;
        }
        AbstractBasicTranslationRotationScaling2D<T>* translate(const Math::Vector2<T>& vector, TransformationType type = TransformationType::Global) {
            AbstractBasicTranslationRotation2D<T>::translate(vector, type);
            return this;
        }
        AbstractBasicTranslationRotationScaling2D<T>* rotate(Math::Rad<T> angle, TransformationType type = TransformationType::Global) {
            AbstractBasicTranslationRotation2D<T>::rotate(angle, type);
            return this;
        }
        #endif

    protected:
        ~AbstractBasicTranslationRotationScaling2D();

    #ifdef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /** @brief Polymorphic implementation for scale() */
        virtual void doScale(const Math::Vector2<T>& vector, TransformationType type) = 0;
};

template<class T> inline AbstractBasicTranslationRotationScaling2D<T>::AbstractBasicTranslationRotationScaling2D() = default;
template<class T> inline AbstractBasicTranslationRotationScaling2D<T>::~AbstractBasicTranslationRotationScaling2D() = default;

/**
@brief Base transformation for two-dimensional float scenes supporting translation, rotation and scaling

@see @ref AbstractTranslationRotationScaling3D
*/
typedef AbstractBasicTranslationRotationScaling2D<Float> AbstractTranslationRotationScaling2D;

}}

#endif
