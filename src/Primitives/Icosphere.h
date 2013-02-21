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

#include "Math/Vector3.h"
#include "MeshTools/Subdivide.h"
#include "MeshTools/Clean.h"
#include "Trade/MeshData3D.h"

#include "Primitives/magnumPrimitivesVisibility.h"

namespace Magnum { namespace Primitives {

/** @todoc Remove `ifndef` when Doxygen is sane again */
#ifndef DOXYGEN_GENERATING_OUTPUT
template<std::size_t subdivisions> class MAGNUM_PRIMITIVES_EXPORT Icosphere;
#endif

/**
@brief 3D icosphere primitive with zero subdivisions

Indexed @ref Mesh::Primitive "Triangles" with normals.
*/
template<> class Icosphere<0>: public Trade::MeshData3D {
    public:
        /** @brief Constructor */
        explicit Icosphere();
};

/**
@brief 3D icosphere primitive
@tparam subdivisions     Number of subdivisions

Indexed @ref Mesh::Primitive "Triangles" with normals.
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<std::size_t subdivisions> class Icosphere: public Icosphere<0> {
#else
template<std::size_t subdivisions> class Icosphere {
#endif
    public:
        /** @brief Constructor */
        explicit Icosphere() {
            for(std::size_t i = 0; i != subdivisions; ++i)
                MeshTools::subdivide(*indices(), *normals(0), [](const Vector3& a, const Vector3& b) {
                    return (a+b).normalized();
                });

            MeshTools::clean(*indices(), *normals(0));
            positions(0)->clear();
            positions(0)->reserve(normals(0)->size());
            for(auto i: *normals(0)) positions(0)->push_back(Point3D(i));
        }
};

}}

#endif
