#ifndef Magnum_Primitives_Icosphere_h
#define Magnum_Primitives_Icosphere_h
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
                MeshTools::subdivide(*indices(), *normals(0), interpolator);

            MeshTools::clean(*indices(), *normals(0));
            positions(0)->assign(normals(0)->begin(), normals(0)->end());
        }

        #ifndef DOXYGEN_GENERATING_OUTPUT
        static Vector3 interpolator(const Vector3& a, const Vector3& b) {
            return (a+b).normalized();
        }
        #endif
};

}}

#endif
