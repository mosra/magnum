#ifndef Magnum_SceneGraph_AbstractTranslationRotation2D_h
#define Magnum_SceneGraph_AbstractTranslationRotation2D_h
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
 * @brief Class Magnum::SceneGraph::AbstractTranslationRotation2D
 */

#include "AbstractTransformation.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Base for two-dimensional transformations supporting translation and rotation

@see AbstractTranslationRotation3D
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T>
#else
template<class T = GLfloat>
#endif
class AbstractTranslationRotation2D: public AbstractTransformation<2, T> {
    public:
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
         * @param angle     Angle in radians, counterclockwise
         * @param type      Transformation type
         * @return Pointer to self (for method chaining)
         *
         * @see deg(), rad()
         */
        virtual AbstractTranslationRotation2D<T>* rotate(T angle, TransformationType type = TransformationType::Global) = 0;
};

}}

#endif
