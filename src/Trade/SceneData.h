#ifndef Magnum_Trade_SceneData_h
#define Magnum_Trade_SceneData_h
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
 * @brief Class Magnum::Trade::SceneData
 */

namespace Magnum { namespace Trade {

/**
@brief %Scene
*/
class MAGNUM_EXPORT SceneData {
    SceneData(const SceneData& other) = delete;
    SceneData(SceneData&& other) = delete;
    SceneData& operator=(const SceneData& other) = delete;
    SceneData& operator=(SceneData&& other) = delete;
};

}}

#endif
