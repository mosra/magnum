#ifndef Magnum_MeshTools_FlipNormals_h
#define Magnum_MeshTools_FlipNormals_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Function @ref Magnum::MeshTools::flipFaceWindingInPlace(), @ref Magnum::MeshTools::flipNormalsInPlace()
 */

#include <Corrade/Containers/Containers.h>

#include "Magnum/Magnum.h"
#include "Magnum/MeshTools/visibility.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/StlForwardVector.h>
#include <Corrade/Containers/ArrayViewStl.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Utility/Macros.h>
#endif

namespace Magnum { namespace MeshTools {

/**
@brief Flip mesh normals and face winding in-place
@param[in,out] indices  Index array to operate on
@param[in,out] normals  Normal array to operate on
@m_since{2020,06}

Flips normal vectors and face winding in index array for face culling to work
properly too. See also @ref flipNormalsInPlace(const Containers::StridedArrayView1D<Vector3>&)
and @ref flipFaceWindingInPlace(), which flip normals or face winding only.
Expects a triangle mesh, thus the index count has to be divisible by 3.
*/
void flipNormalsInPlace(const Containers::StridedArrayView1D<UnsignedInt>& indices, const Containers::StridedArrayView1D<Vector3>& normals);

/**
 * @overload
 * @m_since{2020,06}
 */
void flipNormalsInPlace(const Containers::StridedArrayView1D<UnsignedShort>& indices, const Containers::StridedArrayView1D<Vector3>& normals);

/**
 * @overload
 * @m_since{2020,06}
 */
void flipNormalsInPlace(const Containers::StridedArrayView1D<UnsignedByte>& indices, const Containers::StridedArrayView1D<Vector3>& normals);

/**
@brief Flip mesh normals and face winding in-place on a type-erased index array
@m_since{2020,06}

Expects that the second dimension of @p indices is contiguous and represents
the actual 1/2/4-byte index type. Based on its size then calls one of the
@ref flipNormalsInPlace(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView1D<Vector3>&)
etc. overloads.
*/
void flipNormalsInPlace(const Containers::StridedArrayView2D<char>& indices, const Containers::StridedArrayView1D<Vector3>& normals);

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief @copybrief flipNormalsInPlace(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView1D<Vector3>&)
@m_deprecated_since{2020,06} Use @ref flipNormalsInPlace(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView1D<Vector3>&)
    instead.
*/
CORRADE_DEPRECATED("use flipNormalsInPlace() instead") MAGNUM_MESHTOOLS_EXPORT void flipNormals(std::vector<UnsignedInt>& indices, std::vector<Vector3>& normals);
#endif

/**
@brief Flip face winding in-place
@param[in,out] indices  Index array to operate on
@m_since{2020,06}

Same as @ref flipNormalsInPlace(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView1D<Vector3>&),
but flips only face winding. Expects a triangle mesh, thus the index count has
to be divisible by 3.
*/
void MAGNUM_MESHTOOLS_EXPORT flipFaceWindingInPlace(const Containers::StridedArrayView1D<UnsignedInt>& indices);

/**
 * @overload
 * @m_since{2020,06}
 */
void MAGNUM_MESHTOOLS_EXPORT flipFaceWindingInPlace(const Containers::StridedArrayView1D<UnsignedShort>& indices);

/**
 * @overload
 * @m_since{2020,06}
 */
void MAGNUM_MESHTOOLS_EXPORT flipFaceWindingInPlace(const Containers::StridedArrayView1D<UnsignedByte>& indices);

/**
@brief Flip face winding in-place on a type-erased index array
@m_since{2020,06}

Expects that the second dimension of @p indices is contiguous and represents
the actual 1/2/4-byte index type. Based on its size then calls one of the
@ref flipFaceWindingInPlace(const Containers::StridedArrayView1D<UnsignedInt>&)
etc. overloads.
*/
void MAGNUM_MESHTOOLS_EXPORT flipFaceWindingInPlace(const Containers::StridedArrayView2D<char>& indices);

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief @copybrief flipFaceWindingInPlace(const Containers::StridedArrayView1D<UnsignedInt>&)
@m_deprecated_since{2020,06} Use @ref flipFaceWindingInPlace(const Containers::StridedArrayView1D<UnsignedInt>&)
    instead.
*/
CORRADE_DEPRECATED("use flipFaceWindingInPlace() instead") MAGNUM_MESHTOOLS_EXPORT void flipFaceWinding(std::vector<UnsignedInt>& indices);
#endif

/**
@brief Flip mesh normals in-place
@param[in,out] normals  Normal array to operate on

Same as @ref flipNormalsInPlace(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView1D<Vector3>&),
but flips only normals, not face winding.
*/
void MAGNUM_MESHTOOLS_EXPORT flipNormalsInPlace(const Containers::StridedArrayView1D<Vector3>& normals);

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@copybrief flipNormalsInPlace(const Containers::StridedArrayView1D<Vector3>&)
@m_deprecated_since{2020,06} Use @ref flipNormalsInPlace(const Containers::StridedArrayView1D<Vector3>&)
    instead.
*/
CORRADE_DEPRECATED("use flipNormalsInPlace() instead") MAGNUM_MESHTOOLS_EXPORT void flipNormals(std::vector<Vector3>& normals);
#endif

inline void flipNormalsInPlace(const Containers::StridedArrayView1D<UnsignedInt>& indices, const Containers::StridedArrayView1D<Vector3>& normals) {
    flipFaceWindingInPlace(indices);
    flipNormalsInPlace(normals);
}

inline void flipNormalsInPlace(const Containers::StridedArrayView1D<UnsignedShort>& indices, const Containers::StridedArrayView1D<Vector3>& normals) {
    flipFaceWindingInPlace(indices);
    flipNormalsInPlace(normals);
}

inline void flipNormalsInPlace(const Containers::StridedArrayView1D<UnsignedByte>& indices, const Containers::StridedArrayView1D<Vector3>& normals) {
    flipFaceWindingInPlace(indices);
    flipNormalsInPlace(normals);
}

inline void flipNormalsInPlace(const Containers::StridedArrayView2D<char>& indices, const Containers::StridedArrayView1D<Vector3>& normals) {
    flipFaceWindingInPlace(indices);
    flipNormalsInPlace(normals);
}

}}

#endif
