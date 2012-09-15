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
 * @brief Class Magnum::SceneGraph::Scene, typedef Magnum::SceneGraph::Scene2D, Magnum::SceneGraph::Scene3D
 */

#include "Object.h"

namespace Magnum { namespace SceneGraph {

/** @brief %Scene */
template<size_t dimensions> class SCENEGRAPH_EXPORT Scene: public Object<dimensions>::ObjectType {
    public:
        /** @copydoc Object::isScene() */
        inline bool isScene() const { return true; }

        /** @todo Some deleted functions belong only to Scene2D, some only to Scene3D - what to do? */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        void setParent(typename Object<dimensions>::ObjectType* parent) = delete;
        void setTransformation(const typename Object<dimensions>::MatrixType& transformation) = delete;
        void multiplyTransformation(const typename Object<dimensions>::MatrixType& transformation, typename Object<dimensions>::Transformation type = Object<dimensions>::Transformation::Global) = delete;
        void translate(const typename Object<dimensions>::VectorType& vec, typename Object<dimensions>::Transformation type = Object<dimensions>::Transformation::Global) = delete;
        void scale(const typename Object<dimensions>::VectorType& vec, typename Object<dimensions>::Transformation type = Object<dimensions>::Transformation::Global) = delete;
        void rotate(GLfloat angle, const typename Object<dimensions>::VectorType& vec, typename Object<dimensions>::Transformation type = Object<dimensions>::Transformation::Global) = delete;
        #endif

    private:
        inline void draw(const typename Object<dimensions>::MatrixType&, typename Object<dimensions>::CameraType*) {}
};

/** @brief Two-dimensional scene */
typedef Scene<2> Scene2D;

/** @brief Three-dimensional scene */
typedef Scene<3> Scene3D;

}}

#endif
