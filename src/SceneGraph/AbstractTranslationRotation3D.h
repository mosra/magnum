#ifndef Magnum_SceneGraph_AbstractTranslationRotation3D_h
#define Magnum_SceneGraph_AbstractTranslationRotation3D_h
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
 * @brief Class Magnum::SceneGraph::AbstractBasicTranslationRotation3D, typedef Magnum::SceneGraph::AbstractTranslationRotation3D
 */

#include "AbstractTransformation.h"
#include "Math/Vector3.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Base transformation for three-dimensional scenes supporting translation and rotation

@see @ref AbstractTranslationRotation3D @ref scenegraph, @ref AbstractBasicTranslationRotation2D
@todo Use AbstractBasicTransformation3D<T> when support for GCC 4.6 is dropped
*/
template<class T> class AbstractBasicTranslationRotation3D: public AbstractTransformation<3, T> {
    public:
        explicit AbstractBasicTranslationRotation3D() = default;

        /**
         * @brief Translate object
         * @param vector            Translation vector
         * @param type              Transformation type
         * @return Reference to self (for method chaining)
         *
         * @see Vector3::xAxis(), Vector3::yAxis(), Vector3::zAxis()
         */
        AbstractBasicTranslationRotation3D<T>& translate(const Math::Vector3<T>& vector, TransformationType type = TransformationType::Global) {
            doTranslate(vector, type);
            return *this;
        }

        /**
         * @brief Rotate object
         * @param angle             Angle (counterclockwise)
         * @param normalizedAxis    Normalized rotation axis
         * @param type              Transformation type
         * @return Reference to self (for method chaining)
         *
         * @see rotateX(), rotateY(), rotateZ(), Vector3::xAxis(),
         *      Vector3::yAxis(), Vector3::zAxis()
         */
        AbstractBasicTranslationRotation3D<T>& rotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis, TransformationType type = TransformationType::Global) {
            doRotate(angle, normalizedAxis, type);
            return *this;
        }

        /**
         * @brief Rotate object around X axis
         * @param angle             Angle (counterclockwise)
         * @param type              Transformation type
         * @return Reference to self (for method chaining)
         *
         * In some implementations faster than calling
         * `rotate(angle, Vector3::xAxis())`.
         */
        AbstractBasicTranslationRotation3D<T>& rotateX(Math::Rad<T> angle, TransformationType type = TransformationType::Global) {
            doRotateX(angle, type);
            return *this;
        }

        /**
         * @brief Rotate object around Y axis
         * @param angle             Angle (counterclockwise)
         * @param type              Transformation type
         * @return Reference to self (for method chaining)
         *
         * In some implementations faster than calling
         * `rotate(angle, Vector3::yAxis())`.
         */
        AbstractBasicTranslationRotation3D<T>& rotateY(Math::Rad<T> angle, TransformationType type = TransformationType::Global) {
            doRotateX(angle, type);
            return *this;
        }

        /**
         * @brief Rotate object around Z axis
         * @param angle             Angle (counterclockwise)
         * @param type              Transformation type
         * @return Reference to self (for method chaining)
         *
         * In some implementations faster than calling
         * `rotate(angle, Vector3::zAxis())`.
         */
        AbstractBasicTranslationRotation3D<T>& rotateZ(Math::Rad<T> angle, TransformationType type = TransformationType::Global) {
            doRotateZ(angle, type);
            return *this;
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        AbstractBasicTranslationRotation3D<T>& resetTransformation() {
            AbstractTransformation<3, T>::resetTransformation();
            return *this;
        }
        #endif

    protected:
        ~AbstractBasicTranslationRotation3D() = default;

    #ifdef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /** @brief Polymorphic implementation for translate() */
        virtual void doTranslate(const Math::Vector3<T>& vector, TransformationType type) = 0;

        /** @brief Polymorphic implementation for rotate() */
        virtual void doRotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis, TransformationType type) = 0;

        /**
         * @brief Polymorphic implementation for rotateX()
         *
         * Default implementation calls rotate() with Math::Vector3::xAxis().
         */
        virtual void doRotateX(Math::Rad<T> angle, TransformationType type) {
            rotate(angle, Math::Vector3<T>::xAxis(), type);
        }

        /**
         * @brief Polymorphic implementation for rotateY()
         *
         * Default implementation calls rotate() with Math::Vector3::yAxis().
         */
        virtual void doRotateY(Math::Rad<T> angle, TransformationType type) {
            rotate(angle, Math::Vector3<T>::yAxis(), type);
        }

        /**
         * @brief Polymorphic implementation for rotateZ()
         *
         * Default implementation calls rotate() with Math::Vector3::zAxis().
         */
        virtual void doRotateZ(Math::Rad<T> angle, TransformationType type) {
            rotate(angle, Math::Vector3<T>::zAxis(), type);
        }
};

/**
@brief Base transformation for three-dimensional float scenes supporting translation and rotation

@see @ref AbstractTranslationRotation2D
*/
typedef AbstractBasicTranslationRotation3D<Float> AbstractTranslationRotation3D;

}}

#endif
