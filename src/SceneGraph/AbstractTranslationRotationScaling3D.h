#ifndef Magnum_SceneGraph_AbstractTranslationRotationScaling3D_h
#define Magnum_SceneGraph_AbstractTranslationRotationScaling3D_h
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
 * @brief Class Magnum::SceneGraph::AbstractTranslationRotationScaling3D
 */

#include "AbstractTranslationRotation3D.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Base for three-dimensional transformations supporting translation, rotation and scaling

@see @ref scenegraph, AbstractTranslationRotationScaling2D
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T>
#else
template<class T = Float>
#endif
class AbstractTranslationRotationScaling3D: public AbstractTranslationRotation3D<T> {
    public:
        explicit AbstractTranslationRotationScaling3D();

        /**
         * @brief Scale object
         * @param vector    Scaling vector
         * @param type      Transformation type
         * @return Pointer to self (for method chaining)
         *
         * @see Vector3::xScale(), Vector3::yScale(), Vector3::zScale()
         */
        AbstractTranslationRotationScaling3D<T>* scale(const Math::Vector3<T>& vector, TransformationType type = TransformationType::Global) {
            doScale(vector, type);
            return this;
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        AbstractTranslationRotationScaling3D<T>* resetTransformation() {
            AbstractTranslationRotation3D<T>::resetTransformation();
            return this;
        }
        AbstractTranslationRotationScaling3D<T>* translate(const Math::Vector3<T>& vector, TransformationType type = TransformationType::Global) {
            AbstractTranslationRotation3D<T>::translate(vector, type);
            return this;
        }
        AbstractTranslationRotationScaling3D<T>* rotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis, TransformationType type = TransformationType::Global) {
            AbstractTranslationRotation3D<T>::rotate(angle, normalizedAxis, type);
            return this;
        }
        AbstractTranslationRotationScaling3D<T>* rotateX(Math::Rad<T> angle, TransformationType type = TransformationType::Global) {
            AbstractTranslationRotation3D<T>::rotateX(angle, type);
            return this;
        }
        AbstractTranslationRotationScaling3D<T>* rotateY(Math::Rad<T> angle, TransformationType type = TransformationType::Global) {
            AbstractTranslationRotation3D<T>::rotateY(angle, type);
            return this;
        }
        AbstractTranslationRotationScaling3D<T>* rotateZ(Math::Rad<T> angle, TransformationType type = TransformationType::Global) {
            AbstractTranslationRotation3D<T>::rotateZ(angle, type);
            return this;
        }
        #endif

    #ifdef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /** @brief Polymorphic implementation for scale() */
        virtual void doScale(const Math::Vector3<T>& vector, TransformationType type) = 0;
};

template<class T> inline AbstractTranslationRotationScaling3D<T>::AbstractTranslationRotationScaling3D() = default;

}}

#endif
