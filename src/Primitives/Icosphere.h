#ifndef Magnum_Primitives_Icosphere_h
#define Magnum_Primitives_Icosphere_h
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
 * @brief Class Magnum::Primitives::Icosphere
 */

#include "Trade/MeshData.h"
#include "MeshTools/Subdivide.h"
#include "MeshTools/Clean.h"

namespace Magnum { namespace Primitives {

template<size_t subdivisions> class Icosphere;

/**
@brief %Icosphere primitive with zero subdivisions

@todo Use own computed (and more precise) icosahedron data, not these stolen
from Blender.
*/
template<> class Icosphere<0>: public Trade::MeshData {
    public:
        /** @brief Constructor */
        Icosphere();
};

/**
 * @brief %Icosphere primitive
 * @tparam subdivisions     Number of subdivisions
 */
template<size_t subdivisions> class Icosphere: public Icosphere<0> {
    public:
        /** @brief Constructor */
        Icosphere() {
            for(size_t i = 0; i != subdivisions; ++i)
                MeshTools::subdivide(*indices(), *vertices(0), [](const Vector4& a, const Vector4& b) {
                    return (a+b).xyz().normalized();
                });

            MeshTools::clean(*indices(), *vertices(0));
        }
};

}}

#endif
