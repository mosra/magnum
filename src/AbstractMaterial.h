#ifndef Magnum_AbstractMaterial_h
#define Magnum_AbstractMaterial_h
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
 * @brief Class Magnum::AbstractMaterial
 */

#include "Magnum.h"

namespace Magnum {

/** @brief Base class for materials */
class AbstractMaterial {
    AbstractMaterial(const AbstractMaterial& other) = delete;
    AbstractMaterial(AbstractMaterial&& other) = delete;
    AbstractMaterial& operator=(const AbstractMaterial& other) = delete;
    AbstractMaterial& operator=(AbstractMaterial&& other) = delete;

    public:
        AbstractMaterial() = default;

        /**
         * @brief Use material
         *
         * Uses associated shader and sets uniforms.
         */
        virtual bool use(const Matrix4& transformationMatrix, const Matrix4& projectionMatrix) = 0;
};

}

#endif
