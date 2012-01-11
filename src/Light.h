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
        inline Light(Object* parent = nullptr): Object(parent) {}

        /**
         * @brief Light position relative to the camera
         */
        inline Vector3 position() {
            setClean();
            return _position;
        }

        /**
         * Recomputes light position.
         */
        inline virtual void setClean() {
            if(!isDirty()) return;
            _position = transformation(true).at(3).xyz();
            Object::setClean();
        }

    private:
        Vector3 _position;
};

}

#endif
