#ifndef Magnum_SceneGraph_Light_h
#define Magnum_SceneGraph_Light_h
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
 * @brief Class Magnum::SceneGraph::Light
 */

#include "Math/Point3D.h"
#include "Object.h"

namespace Magnum { namespace SceneGraph {

/**
 * @brief Basic light object
 *
 * Provides cached light position.
 */
class SCENEGRAPH_EXPORT Light: public Object3D {
    public:
        /**
         * @brief Constructor
         * @param parent        Parent object
         */
        inline Light(Object3D* parent = nullptr): Object3D(parent) {}

        /**
         * @brief Light position relative to root object (scene)
         */
        inline Point3D position() {
            setClean();
            return _position;
        }

    protected:
        /**
         * Recomputes light position.
         */
        void clean(const Matrix4& absoluteTransformation);

    private:
        Point3D _position;
};

}}

#endif
