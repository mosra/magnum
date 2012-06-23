#ifndef Magnum_Trade_LightData_h
#define Magnum_Trade_LightData_h
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
 * @brief Class Magnum::Trade::LightData
 */

#include <string>

namespace Magnum { namespace Trade {

/**
@brief %Light data
*/
class MAGNUM_EXPORT LightData {
    LightData(const LightData& other) = delete;
    LightData(LightData&& other) = delete;
    LightData& operator=(const LightData& other) = delete;
    LightData& operator=(LightData&& other) = delete;

    public:
        /**
         * @brief Constructor
         * @param name      %Light name
         */
        inline LightData(const std::string& name): _name(name) {}

        /** @brief %Light name */
        inline std::string name() const { return _name; }

    private:
        std::string _name;
};

}}

#endif
