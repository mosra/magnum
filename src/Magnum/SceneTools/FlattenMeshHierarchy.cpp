/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#define _MAGNUM_NO_DEPRECATED_FLATTENMESHHIERARCHY /* So it doesn't yell here */

#include "FlattenMeshHierarchy.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Triple.h>

#include "Magnum/DimensionTraits.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/SceneTools/Hierarchy.h"
#include "Magnum/Trade/SceneData.h"

namespace Magnum { namespace SceneTools {

namespace {

template<UnsignedInt> struct DimensionTraits;
template<> struct DimensionTraits<2> {
    static Containers::Array<Matrix3> flatten(const Trade::SceneData& scene, const UnsignedInt fieldId, const Matrix3& globalTransformation) {
        return absoluteFieldTransformations2D(scene, fieldId, globalTransformation);
    }
    static void flattenInto(const Trade::SceneData& scene, const UnsignedInt fieldId, const Containers::StridedArrayView1D<Matrix3>& transformations, const Matrix3& globalTransformation) {
        return absoluteFieldTransformations2DInto(scene, fieldId, transformations, globalTransformation);
    }
};
template<> struct DimensionTraits<3> {
    static Containers::Array<Matrix4> flatten(const Trade::SceneData& scene, const UnsignedInt fieldId, const Matrix4& globalTransformation) {
        return absoluteFieldTransformations3D(scene, fieldId, globalTransformation);
    }
    static void flattenInto(const Trade::SceneData& scene, const UnsignedInt fieldId, const Containers::StridedArrayView1D<Matrix4>& transformations, const Matrix4& globalTransformation) {
        return absoluteFieldTransformations3DInto(scene, fieldId, transformations, globalTransformation);
    }
};

template<UnsignedInt dimensions> void flattenMeshHierarchyIntoImplementation(const Trade::SceneData& scene, const Containers::StridedArrayView1D<MatrixTypeFor<dimensions, Float>>& outputTransformations, const MatrixTypeFor<dimensions, Float>& globalTransformation) {
    const Containers::Optional<UnsignedInt> meshFieldId = scene.findFieldId(Trade::SceneField::Mesh);
    /* If there's no mesh field in the file, nothing to do. This is how the
       original API behaved, it's an assertion in the new one. */
    if(!meshFieldId) return;

    DimensionTraits<dimensions>::flattenInto(scene, *meshFieldId, outputTransformations, globalTransformation);
}

template<UnsignedInt dimensions> Containers::Array<Containers::Triple<UnsignedInt, Int, MatrixTypeFor<dimensions, Float>>> flattenMeshHierarchyImplementation(const Trade::SceneData& scene, const MatrixTypeFor<dimensions, Float>& globalTransformation) {
    const Containers::Optional<UnsignedInt> meshFieldId = scene.findFieldId(Trade::SceneField::Mesh);
    /* If there's no mesh field in the file, nothing to do. This is how the
       original API behaved, it's an assertion in the new one. */
    if(!meshFieldId) return {};

    /* Get the transformations. This will be a no-op if the mesh field isn't
       present, but will go through other assertions that may still be rather
       valuable */
    Containers::Array<Containers::Triple<UnsignedInt, Int, MatrixTypeFor<dimensions, Float>>> out{NoInit, scene.fieldSize(*meshFieldId)};
    DimensionTraits<dimensions>::flattenInto(scene, *meshFieldId,
        stridedArrayView(out).slice(&decltype(out)::Type::third),
        globalTransformation);

    /* Fetch the additional mesh and material ID as well, which are in the
       same order */
    scene.meshesMaterialsInto(nullptr,
        stridedArrayView(out).slice(&decltype(out)::Type::first),
        stridedArrayView(out).slice(&decltype(out)::Type::second));

    return out;
}

}

Containers::Array<Containers::Triple<UnsignedInt, Int, Matrix3>> flattenMeshHierarchy2D(const Trade::SceneData& scene, const Matrix3& globalTransformation) {
    return flattenMeshHierarchyImplementation<2>(scene, globalTransformation);
}

void flattenMeshHierarchy2DInto(const Trade::SceneData& scene, const Containers::StridedArrayView1D<Matrix3>& transformations, const Matrix3& globalTransformation) {
    return flattenMeshHierarchyIntoImplementation<2>(scene, transformations, globalTransformation);
}

Containers::Array<Containers::Triple<UnsignedInt, Int, Matrix4>> flattenMeshHierarchy3D(const Trade::SceneData& scene, const Matrix4& globalTransformation) {
    return flattenMeshHierarchyImplementation<3>(scene, globalTransformation);
}

void flattenMeshHierarchy3DInto(const Trade::SceneData& scene, const Containers::StridedArrayView1D<Matrix4>& transformations, const Matrix4& globalTransformation) {
    return flattenMeshHierarchyIntoImplementation<3>(scene, transformations, globalTransformation);
}

}}
