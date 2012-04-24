#ifndef Magnum_utilities_h
#define Magnum_utilities_h
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
 * @brief Various utilities and macros
 */

namespace Magnum {

#ifdef _WIN32
#ifdef MAGNUM_EXPORTING
    #define MAGNUM_EXPORT __declspec(dllexport)
#else
    #define MAGNUM_EXPORT __declspec(dllimport)
#endif
#ifdef MESHTOOLS_EXPORTING
    #define MESHTOOLS_EXPORT __declspec(dllexport)
#else
    #define MESHTOOLS_EXPORT __declspec(dllimport)
#endif
#ifdef PHYSICS_EXPORTING
    #define PHYSICS_EXPORT __declspec(dllexport)
#else
    #define PHYSICS_EXPORT __declspec(dllimport)
#endif
#else
    #define MAGNUM_EXPORT
    #define MESHTOOLS_EXPORT
    #define PHYSICS_EXPORT
#endif

}

#endif
