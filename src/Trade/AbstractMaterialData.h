#ifndef Magnum_Trade_AbstractMaterialData_h
#define Magnum_Trade_AbstractMaterialData_h
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
 * @brief Class Magnum::Trade::AbstractMaterialData
 */

#include <string>

namespace Magnum { namespace Trade {

/**
@brief Base class for material data

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
         * @param name      Material name
         * @param type      Material type
         */
        inline AbstractMaterialData(const std::string& name, Type type): _name(name), _type(type) {}

        /** @brief Destructor */
        virtual ~AbstractMaterialData() = 0;

        /** @brief Material name */
        inline std::string name() const { return _name; }

        /** @brief Material type */
        inline Type type() const { return _type; }

    private:
        std::string _name;
        Type _type;
};

inline AbstractMaterialData::~AbstractMaterialData() {}

}}

#endif
