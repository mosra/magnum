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

namespace Magnum { namespace SceneGraph {

/**
@brief Base for three-dimensional transformations supporting translation and rotation

@see AbstractTranslationRotation2D
*/
template<class T = GLfloat> class AbstractTranslationRotation3D: public AbstractTransformation<3, T> {
    public:
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
         * @param angle             Angle in radians, counterclockwise
         * @param normalizedAxis    Normalized rotation axis
         * @param type              Transformation type
         * @return Pointer to self (for method chaining)
         *
         * @see deg(), rad(), Vector3::xAxis(), Vector3::yAxis(), Vector3::zAxis()
         */
        virtual AbstractTranslationRotation3D<T>* rotate(T angle, const Math::Vector3<T>& normalizedAxis, TransformationType type = TransformationType::Global) = 0;
};

}}

#endif
