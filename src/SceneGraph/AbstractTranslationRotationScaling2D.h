#ifndef Magnum_SceneGraph_AbstractTranslationRotationScaling2D_h
#define Magnum_SceneGraph_AbstractTranslationRotationScaling2D_h
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
 * @brief Class Magnum::SceneGraph::AbstractTranslationRotationScaling2D
 */

#include "AbstractTranslationRotation2D.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Base for two-dimensional transformations supporting translation, rotation and scaling

@see AbstractTranslationRotationScaling2D
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T>
#else
template<class T = GLfloat>
#endif
class AbstractTranslationRotationScaling2D: public AbstractTranslationRotation2D<T> {
    public:
        /**
         * @brief Scale object
         * @param vector    Scaling vector
         * @param type      Transformation type
         * @return Pointer to self (for method chaining)
         *
         * @see Vector2::xScale(), Vector2::yScale()
         */
        virtual AbstractTranslationRotationScaling2D<T>* scale(const Math::Vector2<T>& vector, TransformationType type = TransformationType::Global) = 0;
};

}}

#endif
