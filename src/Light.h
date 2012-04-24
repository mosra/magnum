#ifndef Magnum_Light_h
#define Magnum_Light_h
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
 * @brief Class Magnum::Light
 */

#include "Object.h"

namespace Magnum {

/**
 * @brief Basic light object
 *
 * Provides cached light position.
 */
class Light: public Object {
    public:
        /**
         * @brief Constructor
         * @param parent        Parent object
         */
        inline Light(Object* parent = nullptr): Object(parent), _camera(nullptr) {}

        /**
         * @brief Light position relative to given camera
         *
         * The position is cached until the camera is changed to another or
         * the light dirty bit is set.
         */
        Vector3 position(Camera* camera);

    protected:
        /**
         * Recomputes light position.
         */
        void clean(const Matrix4& absoluteTransformation);

    private:
        Camera* _camera;
        Vector3 _position;
};

}

#endif
