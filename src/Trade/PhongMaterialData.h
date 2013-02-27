#ifndef Magnum_Trade_PhongMaterialData_h
#define Magnum_Trade_PhongMaterialData_h
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
 * @brief Class Magnum::Trade::PhongMaterialData
 */

#include "Math/Vector3.h"
#include "Magnum.h"
#include "AbstractMaterialData.h"

namespace Magnum { namespace Trade {

/**
@brief Phong material data
*/
class PhongMaterialData: public AbstractMaterialData {
    public:
        /**
         * @brief Constructor
         * @param ambientColor      Ambient color
         * @param diffuseColor      Diffuse color
         * @param specularColor     Specular color
         * @param shininess         Shininess
         */
        PhongMaterialData(const Vector3& ambientColor, const Vector3& diffuseColor, const Vector3& specularColor, Float shininess): AbstractMaterialData(Phong), _ambientColor(ambientColor), _diffuseColor(diffuseColor), _specularColor(specularColor), _shininess(shininess) {}

        /** @brief Ambient color */
        inline Vector3 ambientColor() const { return _ambientColor; }

        /** @brief Diffuse color */
        inline Vector3 diffuseColor() const { return _diffuseColor; }

        /** @brief Specular color */
        inline Vector3 specularColor() const { return _specularColor; }

        /** @brief Shininess */
        inline Float shininess() const { return _shininess; }

    private:
        Vector3 _ambientColor,
            _diffuseColor,
            _specularColor;
        Float _shininess;
};

}}

#endif
