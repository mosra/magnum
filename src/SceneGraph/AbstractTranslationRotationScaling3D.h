#ifndef Magnum_SceneGraph_AbstractTranslationRotationScaling3D_h
#define Magnum_SceneGraph_AbstractTranslationRotationScaling3D_h
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
template<class T = GLfloat>
#endif
class AbstractTranslationRotationScaling3D: public AbstractTranslationRotation3D<T> {
    public:
        /**
         * @brief Scale object
         * @param vector    Scaling vector
         * @param type      Transformation type
         * @return Pointer to self (for method chaining)
         *
         * @see Vector3::xScale(), Vector3::yScale(), Vector3::zScale()
         */
        virtual AbstractTranslationRotationScaling3D<T>* scale(const Math::Vector3<T>& vector, TransformationType type = TransformationType::Global) = 0;
};

}}

#endif
