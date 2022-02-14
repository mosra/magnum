/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

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

#include "FlattenMeshHierarchy.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/ArrayTuple.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/Triple.h>

#include "Magnum/DimensionTraits.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Trade/SceneData.h"
#include "Magnum/SceneTools/OrderClusterParents.h"

namespace Magnum { namespace SceneTools {

namespace {

template<UnsignedInt> struct SceneDataDimensionTraits;
template<> struct SceneDataDimensionTraits<2> {
    static bool isDimensions(const Trade::SceneData& scene) {
        return scene.is2D();
    }
    static void transformationsInto(const Trade::SceneData& scene, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Matrix3>& transformationDestination) {
        return scene.transformations2DInto(mappingDestination, transformationDestination);
    }
};
template<> struct SceneDataDimensionTraits<3> {
    static bool isDimensions(const Trade::SceneData& scene) {
        return scene.is3D();
    }
    static void transformationsInto(const Trade::SceneData& scene, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Matrix4>& transformationDestination) {
        return scene.transformations3DInto(mappingDestination, transformationDestination);
    }
};

template<UnsignedInt dimensions>
Containers::Array<Containers::Triple<UnsignedInt, Int, MatrixTypeFor<dimensions, Float>>> flattenMeshHierarchyImplementation(const Trade::SceneData& scene, const MatrixTypeFor<dimensions, Float>& globalTransformation) {
    CORRADE_ASSERT(SceneDataDimensionTraits<dimensions>::isDimensions(scene),
        "SceneTools::flattenMeshHierarchy(): the scene is not" << dimensions << Debug::nospace << "D", {});
    const Containers::Optional<UnsignedInt> parentFieldId = scene.findFieldId(Trade::SceneField::Parent);
    CORRADE_ASSERT(parentFieldId,
        "SceneTools::flattenMeshHierarchy(): the scene has no hierarchy", {});

    /* If there's no mesh field in the file, nothing to do. Another case is
       that there is a mesh field but it's empty, then for simplicity we still
       go through everything. */
    if(!scene.hasField(Trade::SceneField::Mesh)) return {};

    /* Allocate a single storage for all temporary data */
    Containers::ArrayView<Containers::Pair<UnsignedInt, Int>> orderedClusteredParents;
    Containers::ArrayView<Containers::Pair<UnsignedInt, MatrixTypeFor<dimensions, Float>>> transformations;
    Containers::ArrayView<MatrixTypeFor<dimensions, Float>> absoluteTransformations;
    Containers::ArrayTuple storage{
        /* Output of orderClusterParentsInto() */
        {NoInit, scene.fieldSize(*parentFieldId), orderedClusteredParents},
        /* Output of scene.transformationsXDInto() */
        {NoInit, scene.transformationFieldSize(), transformations},
        /* Above transformations but indexed by object ID */
        {ValueInit, std::size_t(scene.mappingBound() + 1), absoluteTransformations}
    };
    /* Explicit slice() template parameters needed by GCC 4.8 and MSVC 2015 */
    orderClusterParentsInto(scene,
        stridedArrayView(orderedClusteredParents).slice<UnsignedInt>(&Containers::Pair<UnsignedInt, Int>::first),
        stridedArrayView(orderedClusteredParents).slice<Int>(&Containers::Pair<UnsignedInt, Int>::second));
    SceneDataDimensionTraits<dimensions>::transformationsInto(scene,
        stridedArrayView(transformations).template slice<UnsignedInt>(&Containers::Pair<UnsignedInt, MatrixTypeFor<dimensions, Float>>::first),
        stridedArrayView(transformations).template slice<MatrixTypeFor<dimensions, Float>>(&Containers::Pair<UnsignedInt, MatrixTypeFor<dimensions, Float>>::second));

    /* Retrieve transformations of all objects, indexed by object ID. Since not
       all nodes in the hierarchy may have a transformation assigned, the whole
       array got initialized to identity first. */
    /** @todo switch to a hashmap eventually? */
    absoluteTransformations[0] = globalTransformation;
    for(const Containers::Pair<UnsignedInt, MatrixTypeFor<dimensions, Float>>& transformation: transformations) {
        CORRADE_INTERNAL_ASSERT(transformation.first() < scene.mappingBound());
        absoluteTransformations[transformation.first() + 1] = transformation.second();
    }

    /* Turn the transformations into absolute */
    for(const Containers::Pair<UnsignedInt, Int>& parentOffset: orderedClusteredParents) {
        absoluteTransformations[parentOffset.first() + 1] =
            absoluteTransformations[parentOffset.second() + 1]*
            absoluteTransformations[parentOffset.first() + 1];
    }

    /* Allocate the output array, retrieve mesh & material IDs and assign
       absolute transformations to each. The matrix location is abused for
       object mapping, which is subsequently replaced by the absolute object
       transformation for given mesh. */
    /** @todo skip meshes that aren't part of the hierarchy once we have a
        BitArray to efficiently mark what's in the hierarchy and what not */
    Containers::Array<Containers::Triple<UnsignedInt, Int, MatrixTypeFor<dimensions, Float>>> out{NoInit, scene.fieldSize(Trade::SceneField::Mesh)};
    Containers::StridedArrayView1D<UnsignedInt> meshes{out, &out.data()->first(), out.size(), sizeof(typename decltype(out)::Type)};
    Containers::StridedArrayView1D<Int> meshMaterials{out, &out.data()->second(), out.size(), sizeof(typename decltype(out)::Type)};
    Containers::StridedArrayView1D<MatrixTypeFor<dimensions, Float>> matrices{out, &out.data()->third(), out.size(), sizeof(typename decltype(out)::Type)};
    Containers::StridedArrayView1D<UnsignedInt> mapping = Containers::arrayCast<UnsignedInt>(matrices);
    scene.meshesMaterialsInto(mapping, meshes, meshMaterials);
    for(std::size_t i = 0; i != out.size(); ++i) {
        CORRADE_INTERNAL_ASSERT(mapping[i] < scene.mappingBound());
        matrices[i] = absoluteTransformations[mapping[i] + 1];
    }

    return out;
}

}

Containers::Array<Containers::Triple<UnsignedInt, Int, Matrix3>> flattenMeshHierarchy2D(const Trade::SceneData& scene, const Matrix3& globalTransformation) {
    return flattenMeshHierarchyImplementation<2>(scene, globalTransformation);
}

Containers::Array<Containers::Triple<UnsignedInt, Int, Matrix3>> flattenMeshHierarchy2D(const Trade::SceneData& scene) {
    return flattenMeshHierarchyImplementation<2>(scene, {});
}

Containers::Array<Containers::Triple<UnsignedInt, Int, Matrix4>> flattenMeshHierarchy3D(const Trade::SceneData& scene, const Matrix4& globalTransformation) {
    return flattenMeshHierarchyImplementation<3>(scene, globalTransformation);
}

Containers::Array<Containers::Triple<UnsignedInt, Int, Matrix4>> flattenMeshHierarchy3D(const Trade::SceneData& scene) {
    return flattenMeshHierarchyImplementation<3>(scene, {});
}

}}
