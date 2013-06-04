#ifndef Magnum_Trade_PhongMaterialData_h
#define Magnum_Trade_PhongMaterialData_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
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
class MAGNUM_EXPORT PhongMaterialData: public AbstractMaterialData {
    public:
        /**
         * @brief Constructor
         * @param ambientColor      Ambient color
         * @param diffuseColor      Diffuse color
         * @param specularColor     Specular color
         * @param shininess         Shininess
         */
        explicit PhongMaterialData(const Vector3& ambientColor, const Vector3& diffuseColor, const Vector3& specularColor, Float shininess);

        /** @brief Ambient color */
        Vector3 ambientColor() const { return _ambientColor; }

        /** @brief Diffuse color */
        Vector3 diffuseColor() const { return _diffuseColor; }

        /** @brief Specular color */
        Vector3 specularColor() const { return _specularColor; }

        /** @brief Shininess */
        Float shininess() const { return _shininess; }

    private:
        Vector3 _ambientColor,
            _diffuseColor,
            _specularColor;
        Float _shininess;
};

}}

#endif
