#ifndef Magnum_Trade_PhongMaterial_h
#define Magnum_Trade_PhongMaterial_h
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
 * @brief Class Magnum::Trade::PhongMaterial
 */

#include "Magnum.h"
#include "AbstractMaterial.h"

namespace Magnum { namespace Trade {

/**
@brief Phong material
*/
class MAGNUM_EXPORT PhongMaterial: public AbstractMaterial {
    public:
        /**
         * @brief Constructor
         * @param ambientColor      Ambient color
         * @param diffuseColor      Diffuse color
         * @param specularColor     Specular color
         * @param shininess         Shininess
         */
        PhongMaterial(const Vector3& ambientColor, const Vector3& diffuseColor, const Vector3& specularColor, GLfloat shininess): AbstractMaterial(Phong), _ambientColor(ambientColor), _diffuseColor(diffuseColor), _specularColor(specularColor), _shininess(shininess) {}

        /** @brief Ambient color */
        inline Vector3 ambientColor() const { return _ambientColor; }

        /** @brief Diffuse color */
        inline Vector3 diffuseColor() const { return _diffuseColor; }

        /** @brief Specular color */
        inline Vector3 specularColor() const { return _specularColor; }

        /** @brief Shininess */
        inline GLfloat shininess() const { return _shininess; }

    private:
        Vector3 _ambientColor,
            _diffuseColor,
            _specularColor;
        GLfloat _shininess;
};

}}

#endif
