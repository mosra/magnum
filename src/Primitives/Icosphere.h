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
#ifndef DOXYGEN_GENERATING_OUTPUT
template<size_t subdivisions> class Icosphere: public Icosphere<0> {
#else
template<size_t subdivisions> class Icosphere {
#endif
    public:
        /** @brief Constructor */
        Icosphere() {
            for(size_t i = 0; i != subdivisions; ++i)
                MeshTools::subdivide(*indices(), *normals(0), [](const Vector3& a, const Vector3& b) {
                    return (a+b).normalized();
                });

            MeshTools::clean(*indices(), *normals(0));
            vertices(0)->assign(normals(0)->begin(), normals(0)->end());
        }
};

}}

#endif
