#ifndef Magnum_MeshTools_FlipNormals_h
#define Magnum_MeshTools_FlipNormals_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
              Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/Containers/ArrayViewStl.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Utility/DeprecationMacros.h>
#include <Corrade/Utility/StlForwardVector.h>
#endif

namespace Magnum { namespace MeshTools {

/**
@brief Flip mesh normals in-place
@param[in,out] normals  Normal array to operate on

Flips normal vectors to point in the opposite direction. Note that for face
culling to work properly, face winding may need to be flipped as well --- use
@ref flipNormalsInPlace(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView1D<Vector3>&)
and overloads to perform both operations at once. See
@ref flipFaceWindingInPlace() for detailed explanation of face winding
direction and its effect on rendering.
*/
MAGNUM_MESHTOOLS_EXPORT void flipNormalsInPlace(const Containers::StridedArrayView1D<Vector3>& normals);

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@copybrief flipNormalsInPlace(const Containers::StridedArrayView1D<Vector3>&)
@m_deprecated_since{2020,06} Use @ref flipNormalsInPlace(const Containers::StridedArrayView1D<Vector3>&)
    instead.
*/
CORRADE_DEPRECATED("use flipNormalsInPlace() instead") MAGNUM_MESHTOOLS_EXPORT void flipNormals(std::vector<Vector3>& normals);
#endif

/**
@brief Flip mesh face winding in-place
@param[in,out] indices  Index array to operate on
@m_since{2020,06}

Flips order of indices in each triangle, turning `a, b, c` into `a, c, b` for
each. Expects a triangle mesh, thus size of the @p indices view has to be
divisible by 3.

Face winding direction --- either counterclockwise or clockwise --- is used to distinguish front and back faces of a mesh when doing back face culling, such
as with @ref GL::Renderer::Feature::FaceCulling, and when rendering
double-sided meshes with e.g. @ref Shaders::PhongGL::Flag::DoubleSided. For
correct rendering the winding direction is expected to match normal direction.

If the direction doesn't match, which can happen for example if a negative
scaling is applied on the mesh, or if the mesh data were produced with
expectation that front faces have clockwise winding but the renderer expects
counterclockwise, the winding can be flipped either directly in the mesh by
calling this function or by setting appropriate renderer state such as
@ref GL::Renderer::setFrontFace().

Most commonly you'd want to flip both normals and face winding, for example if
wanting to look at a cube from inside instead of from outside, which can be
done with @ref flipNormalsInPlace(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView1D<Vector3>&).

See also @ref SceneTools::absoluteTransformationScalingSigns3D() which can be
used to collect transformations with negative scaling for a complete scene
hierarchy.
*/
MAGNUM_MESHTOOLS_EXPORT void flipFaceWindingInPlace(const Containers::StridedArrayView1D<UnsignedInt>& indices);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_MESHTOOLS_EXPORT void flipFaceWindingInPlace(const Containers::StridedArrayView1D<UnsignedShort>& indices);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_MESHTOOLS_EXPORT void flipFaceWindingInPlace(const Containers::StridedArrayView1D<UnsignedByte>& indices);

/**
@brief Flip mesh face winding in-place on a type-erased index array
@m_since{2020,06}

Flips order of indices in each triangle, turning `a, b, c` into `a, c, b` for
each. Expects a triangle mesh, thus size of the @p indices view has to be
divisible by 3, and the second dimension is expected to be contiguous.

Can be used not just with index arrays but with vertex
attribute arrays of arbitrary types as well in cases of a non-indexed mesh. For
1/2/4-byte types delegates into one of the
@ref flipFaceWindingInPlace(const Containers::StridedArrayView1D<UnsignedInt>&)
etc. overloads, for other sizes falls back to byte-by-byte swapping. For
example:

@snippet MeshTools.cpp flipFaceWindingInPlace-attributes
*/
MAGNUM_MESHTOOLS_EXPORT void flipFaceWindingInPlace(const Containers::StridedArrayView2D<char>& indices);

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief @copybrief flipFaceWindingInPlace(const Containers::StridedArrayView1D<UnsignedInt>&)
@m_deprecated_since{2020,06} Use @ref flipFaceWindingInPlace(const Containers::StridedArrayView1D<UnsignedInt>&)
    instead.
*/
CORRADE_DEPRECATED("use flipFaceWindingInPlace() instead") MAGNUM_MESHTOOLS_EXPORT void flipFaceWinding(std::vector<UnsignedInt>& indices);
#endif

/**
@brief Flip mesh normals and face winding in-place
@param[in,out] indices  Index array to operate on
@param[in,out] normals  Normal array to operate on
@m_since{2020,06}

Same as calling both @ref flipNormalsInPlace(const Containers::StridedArrayView1D<Vector3>&)
and @ref flipFaceWindingInPlace(const Containers::StridedArrayView1D<UnsignedInt>&)
at the same time. See their documentation for more information.
*/
inline void flipNormalsInPlace(const Containers::StridedArrayView1D<UnsignedInt>& indices, const Containers::StridedArrayView1D<Vector3>& normals) {
    flipFaceWindingInPlace(indices);
    flipNormalsInPlace(normals);
}

/**
 * @overload
 * @m_since{2020,06}
 */
inline void flipNormalsInPlace(const Containers::StridedArrayView1D<UnsignedShort>& indices, const Containers::StridedArrayView1D<Vector3>& normals) {
    flipFaceWindingInPlace(indices);
    flipNormalsInPlace(normals);
}

/**
 * @overload
 * @m_since{2020,06}
 */
inline void flipNormalsInPlace(const Containers::StridedArrayView1D<UnsignedByte>& indices, const Containers::StridedArrayView1D<Vector3>& normals) {
    flipFaceWindingInPlace(indices);
    flipNormalsInPlace(normals);
}

/**
@brief Flip mesh normals and face winding in-place on a type-erased index array
@m_since{2020,06}

Expects that the second dimension of @p indices is contiguous and represents
the actual 1/2/4-byte index type. Based on its size then calls one of the
@ref flipNormalsInPlace(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView1D<Vector3>&)
etc. overloads.
*/
inline void flipNormalsInPlace(const Containers::StridedArrayView2D<char>& indices, const Containers::StridedArrayView1D<Vector3>& normals) {
    flipFaceWindingInPlace(indices);
    flipNormalsInPlace(normals);
}

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief @copybrief flipNormalsInPlace(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView1D<Vector3>&)
@m_deprecated_since{2020,06} Use @ref flipNormalsInPlace(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView1D<Vector3>&)
    instead.
*/
CORRADE_DEPRECATED("use flipNormalsInPlace() instead") MAGNUM_MESHTOOLS_EXPORT void flipNormals(std::vector<UnsignedInt>& indices, std::vector<Vector3>& normals);
#endif

}}

#endif
