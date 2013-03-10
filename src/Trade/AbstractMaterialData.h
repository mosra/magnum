#ifndef Magnum_Trade_AbstractMaterialData_h
#define Magnum_Trade_AbstractMaterialData_h
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
 * @brief Class Magnum::Trade::AbstractMaterialData
 */

#include <string>

namespace Magnum { namespace Trade {

/**
@brief Base for material data

Subclasses provide access to parameters for given material type.
*/
class AbstractMaterialData {
    AbstractMaterialData(const AbstractMaterialData& other) = delete;
    AbstractMaterialData(AbstractMaterialData&& other) = delete;
    AbstractMaterialData& operator=(const AbstractMaterialData& other) = delete;
    AbstractMaterialData& operator=(AbstractMaterialData&& other) = delete;

    public:
        /** @brief Material type */
        enum Type {
            Phong       /**< Phong shading */
        };

        /**
         * @brief Constructor
         * @param type      Material type
         */
        inline AbstractMaterialData(Type type): _type(type) {}

        /** @brief Destructor */
        virtual ~AbstractMaterialData() = 0;

        /** @brief Material type */
        inline Type type() const { return _type; }

    private:
        Type _type;
};

inline AbstractMaterialData::~AbstractMaterialData() {}

}}

#endif
