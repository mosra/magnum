#ifndef Magnum_SceneGraph_Scene_h
#define Magnum_SceneGraph_Scene_h
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
 * @brief Class Magnum::SceneGraph::Scene
 */

#include "Object.h"

namespace Magnum { namespace SceneGraph {

/** @brief %Scene */
template<class MatrixType, class VectorType, class ObjectType, class CameraType> class SCENEGRAPH_EXPORT Scene: public ObjectType {
    public:
        /** @brief Constructor */
        inline Scene() { this->_parent = this; }

        #ifndef DOXYGEN_GENERATING_OUTPUT
        void setParent(ObjectType* parent) = delete;
        void setTransformation(const MatrixType& transformation) = delete;
        void multiplyTransformation(const MatrixType& transformation, typename ObjectType::Transformation type = ObjectType::Transformation::Global) = delete;
        void translate(const VectorType& vec, typename ObjectType::Transformation type = ObjectType::Transformation::Global) = delete;
        void scale(const VectorType& vec, typename ObjectType::Transformation type = ObjectType::Transformation::Global) = delete;
        void rotate(GLfloat angle, const VectorType& vec, typename ObjectType::Transformation type = ObjectType::Transformation::Global) = delete;
        #endif

    private:
        inline void draw(const MatrixType&, CameraType*) {}
};

/** @brief Two-dimensional scene */
typedef Scene<Matrix3, Vector2, Object2D, Camera2D> Scene2D;

/** @brief Three-dimensional scene */
typedef Scene<Matrix4, Vector3, Object3D, Camera3D> Scene3D;

}}

#endif
