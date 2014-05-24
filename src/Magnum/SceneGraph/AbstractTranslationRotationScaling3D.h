#ifndef Magnum_SceneGraph_AbstractTranslationRotationScaling3D_h
#define Magnum_SceneGraph_AbstractTranslationRotationScaling3D_h
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

/** @file
 * @brief Class @ref Magnum::SceneGraph::AbstractBasicTranslationRotationScaling3D, typedef @ref Magnum::SceneGraph::AbstractTranslationRotationScaling3D
 */

#include "Magnum/SceneGraph/AbstractTranslationRotation3D.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Base transformation for three-dimensional scenes supporting translation, rotation and scaling

@see @ref AbstractTranslationRotationScaling3D, @ref scenegraph,
    @ref AbstractBasicTranslationRotationScaling2D,
    @ref BasicMatrixTransformation3D
*/
template<class T> class AbstractBasicTranslationRotationScaling3D: public AbstractBasicTranslationRotation3D<T> {
    public:
        explicit AbstractBasicTranslationRotationScaling3D();

        /**
         * @brief Scale object
         * @param vector    Scaling vector
         * @param type      Transformation type
         * @return Reference to self (for method chaining)
         *
         * @see @ref Math::Vector3::xScale(), @ref Math::Vector3::yScale(),
         *      @ref Math::Vector3::zScale()
         */
        AbstractBasicTranslationRotationScaling3D<T>& scale(const Math::Vector3<T>& vector, TransformationType type = TransformationType::Global) {
            doScale(vector, type);
            return *this;
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        AbstractBasicTranslationRotationScaling3D<T>& resetTransformation() {
            AbstractBasicTranslationRotation3D<T>::resetTransformation();
            return *this;
        }
        AbstractBasicTranslationRotationScaling3D<T>& translate(const Math::Vector3<T>& vector, TransformationType type = TransformationType::Global) {
            AbstractBasicTranslationRotation3D<T>::translate(vector, type);
            return *this;
        }
        AbstractBasicTranslationRotationScaling3D<T>& rotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis, TransformationType type = TransformationType::Global) {
            AbstractBasicTranslationRotation3D<T>::rotate(angle, normalizedAxis, type);
            return *this;
        }
        AbstractBasicTranslationRotationScaling3D<T>& rotateX(Math::Rad<T> angle, TransformationType type = TransformationType::Global) {
            AbstractBasicTranslationRotation3D<T>::rotateX(angle, type);
            return *this;
        }
        AbstractBasicTranslationRotationScaling3D<T>& rotateY(Math::Rad<T> angle, TransformationType type = TransformationType::Global) {
            AbstractBasicTranslationRotation3D<T>::rotateY(angle, type);
            return *this;
        }
        AbstractBasicTranslationRotationScaling3D<T>& rotateZ(Math::Rad<T> angle, TransformationType type = TransformationType::Global) {
            AbstractBasicTranslationRotation3D<T>::rotateZ(angle, type);
            return *this;
        }
        #endif

    protected:
        ~AbstractBasicTranslationRotationScaling3D();

    #ifdef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /** @brief Polymorphic implementation for @ref scale() */
        virtual void doScale(const Math::Vector3<T>& vector, TransformationType type) = 0;
};

template<class T> inline AbstractBasicTranslationRotationScaling3D<T>::AbstractBasicTranslationRotationScaling3D() = default;
template<class T> inline AbstractBasicTranslationRotationScaling3D<T>::~AbstractBasicTranslationRotationScaling3D() = default;

/**
@brief Base transformation for three-dimensional float scenes supporting translation, rotation and scaling

@see @ref AbstractTranslationRotationScaling2D
*/
typedef AbstractBasicTranslationRotationScaling3D<Float> AbstractTranslationRotationScaling3D;

}}

#endif
