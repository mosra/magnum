#ifndef Magnum_SceneGraph_AbstractTranslationRotation3D_h
#define Magnum_SceneGraph_AbstractTranslationRotation3D_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
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
template<class T = GLfloat>
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
};

}}

#endif
