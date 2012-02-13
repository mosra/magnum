#ifndef Magnum_Trade_Light_h
#define Magnum_Trade_Light_h
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
 * @brief Class Magnum::Trade::Light
 */

namespace Magnum { namespace Trade {

/**
@brief %Light
*/
class MAGNUM_EXPORT Light {
    Light(const Light& other) = delete;
    Light(Light&& other) = delete;
    Light& operator=(const Light& other) = delete;
    Light& operator=(Light&& other) = delete;
};

}}

#endif
