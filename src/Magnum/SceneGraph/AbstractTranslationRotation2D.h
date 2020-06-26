#ifndef Magnum_SceneGraph_AbstractTranslationRotation2D_h
#define Magnum_SceneGraph_AbstractTranslationRotation2D_h
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
 * @brief Class @ref Magnum::SceneGraph::AbstractBasicTranslationRotation2D, typedef @ref Magnum::SceneGraph::AbstractTranslationRotation2D
 */

#include "Magnum/SceneGraph/AbstractTranslation.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Base transformation for two-dimensional scenes supporting translation and rotation

See @ref scenegraph-features-transformation for more information.

@see @ref scenegraph, @ref AbstractTranslationRotation2D,
    @ref AbstractBasicTranslationRotation3D,
    @ref BasicRigidMatrixTransformation2D, @ref BasicDualComplexTransformation
*/
template<class T> class AbstractBasicTranslationRotation2D: public AbstractBasicTranslation2D<T> {
    public:
        explicit AbstractBasicTranslationRotation2D() = default;

        /**
         * @brief Rotate the object using a complex number
         * @param complex       Normalized complex number
         * @return Reference to self (for method chaining)
         * @m_since{2020,06}
         *
         * Expects that the complex number is normalized.
         * @see @ref rotate(Math::Rad<T>),
         *      @ref rotateLocal(const Math::Complex<T>&)
         */
        AbstractBasicTranslationRotation2D<T>& rotate(const Math::Complex<T>& complex) {
            doRotate(complex);
            return *this;
        }

        /**
         * @brief Rotate the object using a complex number as a local transformation
         * @m_since{2020,06}
         *
         * Similar to the above, except that the transformation is applied
         * before all others.
         */
        AbstractBasicTranslationRotation2D<T>& rotateLocal(const Math::Complex<T>& complex) {
            doRotateLocal(complex);
            return *this;
        }

        /**
         * @brief Rotate the object
         * @param angle     Angle (counterclockwise)
         * @return Reference to self (for method chaining)
         *
         * @see @ref rotate(const Math::Complex<T>&), @ref rotateLocal()
         */
        AbstractBasicTranslationRotation2D<T>& rotate(Math::Rad<T> angle) {
            doRotate(angle);
            return *this;
        }

        /**
         * @brief Rotate the object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others.
         */
        AbstractBasicTranslationRotation2D<T>& rotateLocal(Math::Rad<T> angle) {
            doRotateLocal(angle);
            return *this;
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        AbstractBasicTranslationRotation2D<T>& resetTransformation() {
            AbstractBasicTranslation2D<T>::resetTransformation();
            return *this;
        }
        AbstractBasicTranslationRotation2D<T>& translate(const Math::Vector2<T>& vector) {
            AbstractBasicTranslation2D<T>::translate(vector);
            return *this;
        }
        AbstractBasicTranslationRotation2D<T>& translateLocal(const Math::Vector2<T>& vector) {
            AbstractBasicTranslation2D<T>::translateLocal(vector);
            return *this;
        }
        #endif

    protected:
        ~AbstractBasicTranslationRotation2D() = default;

    private:
        /**
         * @brief Polymorphic implementation for @ref rotate(const Math::Complex<T>&)
         * @m_since{2020,06}
         */
        virtual void doRotate(const Math::Complex<T>&) = 0;

        /**
         * @brief Polymorphic implementation for @ref rotateLocal(const Math::Complex<T>&)
         * @m_since{2020,06}
         */
        virtual void doRotateLocal(const Math::Complex<T>&) = 0;

        /** @brief Polymorphic implementation for @ref rotate(Math::Rad<T>) */
        virtual void doRotate(Math::Rad<T> angle) = 0;

        /** @brief Polymorphic implementation for @ref rotateLocal(Math::Rad<T>) */
        virtual void doRotateLocal(Math::Rad<T> angle) = 0;
};

/**
@brief Base transformation for two-dimensional float scenes supporting translation and rotation

@see @ref AbstractTranslationRotation3D
*/
typedef AbstractBasicTranslationRotation2D<Float> AbstractTranslationRotation2D;

}}

#endif
