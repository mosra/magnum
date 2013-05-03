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
 * @brief Class Magnum::SceneGraph::AbstractTranslationRotation3D
 */

#include "AbstractTransformation.h"
#include "Math/Vector3.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Base for three-dimensional transformations supporting translation and rotation

@see @ref scenegraph, AbstractTranslationRotation2D
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T>
#else
template<class T = Float>
#endif
class AbstractTranslationRotation3D: public AbstractTransformation<3, T> {
    public:
        explicit AbstractTranslationRotation3D() = default;

        /**
         * @brief Translate object
         * @param vector            Translation vector
         * @param type              Transformation type
         * @return Pointer to self (for method chaining)
         *
         * @see Vector3::xAxis(), Vector3::yAxis(), Vector3::zAxis()
         */
        virtual AbstractTranslationRotation3D<T>* translate(const Math::Vector3<T>& vector, TransformationType type = TransformationType::Global) = 0;

        /**
         * @brief Rotate object
         * @param angle             Angle (counterclockwise)
         * @param normalizedAxis    Normalized rotation axis
         * @param type              Transformation type
         * @return Pointer to self (for method chaining)
         *
         * @see rotateX(), rotateY(), rotateZ(), Vector3::xAxis(),
         *      Vector3::yAxis(), Vector3::zAxis()
         */
        virtual AbstractTranslationRotation3D<T>* rotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis, TransformationType type = TransformationType::Global) = 0;

        /**
         * @brief Rotate object around X axis
         * @param angle             Angle (counterclockwise)
         * @param type              Transformation type
         * @return Pointer to self (for method chaining)
         *
         * In some implementations faster than calling
         * `rotate(angle, Vector3::xAxis())`.
         */
        virtual AbstractTranslationRotation3D<T>* rotateX(Math::Rad<T> angle, TransformationType type = TransformationType::Global) {
            return rotate(angle, Math::Vector3<T>::xAxis(), type);
        }

        /**
         * @brief Rotate object around Y axis
         * @param angle             Angle (counterclockwise)
         * @param type              Transformation type
         * @return Pointer to self (for method chaining)
         *
         * In some implementations faster than calling
         * `rotate(angle, Vector3::yAxis())`.
         */
        virtual AbstractTranslationRotation3D<T>* rotateY(Math::Rad<T> angle, TransformationType type = TransformationType::Global) {
            return rotate(angle, Math::Vector3<T>::yAxis(), type);
        }

        /**
         * @brief Rotate object around Z axis
         * @param angle             Angle (counterclockwise)
         * @param type              Transformation type
         * @return Pointer to self (for method chaining)
         *
         * In some implementations faster than calling
         * `rotate(angle, Vector3::zAxis())`.
         */
        virtual AbstractTranslationRotation3D<T>* rotateZ(Math::Rad<T> angle, TransformationType type = TransformationType::Global) {
            return rotate(angle, Math::Vector3<T>::zAxis(), type);
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        AbstractTranslationRotation3D<T>* resetTransformation() override = 0;
        #endif
};

}}

#endif
