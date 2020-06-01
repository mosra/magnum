#ifndef Magnum_SceneGraph_AbstractTranslationRotationScaling3D_h
#define Magnum_SceneGraph_AbstractTranslationRotationScaling3D_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

See @ref scenegraph-features-transformation for more information.

@see @ref scenegraph, @ref AbstractTranslationRotationScaling3D,
    @ref AbstractBasicTranslationRotationScaling2D,
    @ref BasicMatrixTransformation3D
*/
template<class T> class AbstractBasicTranslationRotationScaling3D: public AbstractBasicTranslationRotation3D<T> {
    public:
        explicit AbstractBasicTranslationRotationScaling3D() = default;

        /**
         * @brief Scale the object
         * @return Reference to self (for method chaining)
         *
         * @see @ref scaleLocal(), @ref Math::Vector3::xScale(),
         *      @ref Math::Vector3::yScale(), @ref Math::Vector3::zScale()
         */
        AbstractBasicTranslationRotationScaling3D<T>& scale(const Math::Vector3<T>& vector) {
            doScale(vector);
            return *this;
        }

        /**
         * @brief Scale the object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others.
         */
        AbstractBasicTranslationRotationScaling3D<T>& scaleLocal(const Math::Vector3<T>& vector) {
            doScaleLocal(vector);
            return *this;
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        AbstractBasicTranslationRotationScaling3D<T>& resetTransformation() {
            AbstractBasicTranslationRotation3D<T>::resetTransformation();
            return *this;
        }
        AbstractBasicTranslationRotationScaling3D<T>& translate(const Math::Vector3<T>& vector) {
            AbstractBasicTranslationRotation3D<T>::translate(vector);
            return *this;
        }
        AbstractBasicTranslationRotationScaling3D<T>& translateLocal(const Math::Vector3<T>& vector) {
            AbstractBasicTranslationRotation3D<T>::translateLocal(vector);
            return *this;
        }
        AbstractBasicTranslationRotationScaling3D<T>& rotate(const Math::Quaternion<T>& quaternion) {
            AbstractBasicTranslationRotation3D<T>::rotate(quaternion);
            return *this;
        }
        AbstractBasicTranslationRotationScaling3D<T>& rotateLocal(const Math::Quaternion<T>& quaternion) {
            AbstractBasicTranslationRotation3D<T>::rotateLocal(quaternion);
            return *this;
        }
        AbstractBasicTranslationRotationScaling3D<T>& rotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis) {
            AbstractBasicTranslationRotation3D<T>::rotate(angle, normalizedAxis);
            return *this;
        }
        AbstractBasicTranslationRotationScaling3D<T>& rotateLocal(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis) {
            AbstractBasicTranslationRotation3D<T>::rotateLocal(angle, normalizedAxis);
            return *this;
        }
        AbstractBasicTranslationRotationScaling3D<T>& rotateX(Math::Rad<T> angle) {
            AbstractBasicTranslationRotation3D<T>::rotateX(angle);
            return *this;
        }
        AbstractBasicTranslationRotationScaling3D<T>& rotateXLocal(Math::Rad<T> angle) {
            AbstractBasicTranslationRotation3D<T>::rotateXLocal(angle);
            return *this;
        }
        AbstractBasicTranslationRotationScaling3D<T>& rotateY(Math::Rad<T> angle) {
            AbstractBasicTranslationRotation3D<T>::rotateY(angle);
            return *this;
        }
        AbstractBasicTranslationRotationScaling3D<T>& rotateYLocal(Math::Rad<T> angle) {
            AbstractBasicTranslationRotation3D<T>::rotateYLocal(angle);
            return *this;
        }
        AbstractBasicTranslationRotationScaling3D<T>& rotateZ(Math::Rad<T> angle) {
            AbstractBasicTranslationRotation3D<T>::rotateZ(angle);
            return *this;
        }
        AbstractBasicTranslationRotationScaling3D<T>& rotateZLocal(Math::Rad<T> angle) {
            AbstractBasicTranslationRotation3D<T>::rotateZLocal(angle);
            return *this;
        }
        #endif

    protected:
        ~AbstractBasicTranslationRotationScaling3D() = default;

    private:
        /** @brief Polymorphic implementation for @ref scale() */
        virtual void doScale(const Math::Vector3<T>& vector) = 0;

        /** @brief Polymorphic implementation for @ref scaleLocal() */
        virtual void doScaleLocal(const Math::Vector3<T>& vector) = 0;
};

/**
@brief Base transformation for three-dimensional float scenes supporting translation, rotation and scaling

@see @ref AbstractTranslationRotationScaling2D
*/
typedef AbstractBasicTranslationRotationScaling3D<Float> AbstractTranslationRotationScaling3D;

}}

#endif
