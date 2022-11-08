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

template<UnsignedInt dimensions> void flattenMeshHierarchyIntoImplementation(const Trade::SceneData& scene, const Containers::StridedArrayView1D<MatrixTypeFor<dimensions, Float>>& outputTransformations, const MatrixTypeFor<dimensions, Float>& globalTransformation) {
    CORRADE_ASSERT(SceneDataDimensionTraits<dimensions>::isDimensions(scene),
        "SceneTools::flattenMeshHierarchy(): the scene is not" << dimensions << Debug::nospace << "D", );
    const Containers::Optional<UnsignedInt> parentFieldId = scene.findFieldId(Trade::SceneField::Parent);
    CORRADE_ASSERT(parentFieldId,
        "SceneTools::flattenMeshHierarchy(): the scene has no hierarchy", );
    Containers::Optional<UnsignedInt> meshFieldId = scene.findFieldId(Trade::SceneField::Mesh);
    CORRADE_ASSERT(outputTransformations.size() == (meshFieldId ? scene.fieldSize(*meshFieldId) : 0),
        "SceneTools::flattenMeshHierarchyInto(): bad output size, expected" << scene.fieldSize(*meshFieldId) << "but got" << outputTransformations.size(), );

    /* If there's no mesh field in the file, nothing to do. Another case is
       that there is a mesh field but it's empty, then for simplicity we still
       go through everything. */
    if(!meshFieldId) return;

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
    orderClusterParentsInto(scene,
        stridedArrayView(orderedClusteredParents).slice(&decltype(orderedClusteredParents)::Type::first),
        stridedArrayView(orderedClusteredParents).slice(&decltype(orderedClusteredParents)::Type::second));
    SceneDataDimensionTraits<dimensions>::transformationsInto(scene,
        stridedArrayView(transformations).slice(&decltype(transformations)::Type::first),
        stridedArrayView(transformations).slice(&decltype(transformations)::Type::second));

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
    const auto mapping = Containers::arrayCast<UnsignedInt>(outputTransformations);
    scene.mappingInto(*meshFieldId, mapping);
    for(std::size_t i = 0; i != mapping.size(); ++i) {
        CORRADE_INTERNAL_ASSERT(mapping[i] < scene.mappingBound());
        outputTransformations[i] = absoluteTransformations[mapping[i] + 1];
    }
}

template<UnsignedInt dimensions> Containers::Array<Containers::Triple<UnsignedInt, Int, MatrixTypeFor<dimensions, Float>>> flattenMeshHierarchyImplementation(const Trade::SceneData& scene, const MatrixTypeFor<dimensions, Float>& globalTransformation) {
    const Containers::Optional<UnsignedInt> meshFieldId = scene.findFieldId(Trade::SceneField::Mesh);

    /* Get the transformations. This will be a no-op if the mesh field isn't
       present, but will go through other assertions that may still be rather
       valuable */
    Containers::Array<Containers::Triple<UnsignedInt, Int, MatrixTypeFor<dimensions, Float>>> out{NoInit, meshFieldId ? scene.fieldSize(*meshFieldId) : 0};
    flattenMeshHierarchyIntoImplementation<dimensions>(scene,
        stridedArrayView(out).slice(&decltype(out)::Type::third),
        globalTransformation);

    /* Fetch the additional mesh and material ID as well, which are in the
       same order */
    if(meshFieldId) scene.meshesMaterialsInto(nullptr,
        stridedArrayView(out).slice(&decltype(out)::Type::first),
        stridedArrayView(out).slice(&decltype(out)::Type::second));

    return out;
}

}

Containers::Array<Containers::Triple<UnsignedInt, Int, Matrix3>> flattenMeshHierarchy2D(const Trade::SceneData& scene, const Matrix3& globalTransformation) {
    return flattenMeshHierarchyImplementation<2>(scene, globalTransformation);
}

Containers::Array<Containers::Triple<UnsignedInt, Int, Matrix3>> flattenMeshHierarchy2D(const Trade::SceneData& scene) {
    return flattenMeshHierarchyImplementation<2>(scene, {});
}

void flattenMeshHierarchy2DInto(const Trade::SceneData& scene, const Containers::StridedArrayView1D<Matrix3>& transformations, const Matrix3& globalTransformation) {
    return flattenMeshHierarchyIntoImplementation<2>(scene, transformations, globalTransformation);
}

void flattenMeshHierarchy2DInto(const Trade::SceneData& scene, const Containers::StridedArrayView1D<Matrix3>& transformations) {
    return flattenMeshHierarchyIntoImplementation<2>(scene, transformations, {});
}

Containers::Array<Containers::Triple<UnsignedInt, Int, Matrix4>> flattenMeshHierarchy3D(const Trade::SceneData& scene, const Matrix4& globalTransformation) {
    return flattenMeshHierarchyImplementation<3>(scene, globalTransformation);
}

Containers::Array<Containers::Triple<UnsignedInt, Int, Matrix4>> flattenMeshHierarchy3D(const Trade::SceneData& scene) {
    return flattenMeshHierarchyImplementation<3>(scene, {});
}

void flattenMeshHierarchy3DInto(const Trade::SceneData& scene, const Containers::StridedArrayView1D<Matrix4>& transformations, const Matrix4& globalTransformation) {
    return flattenMeshHierarchyIntoImplementation<3>(scene, transformations, globalTransformation);
}

void flattenMeshHierarchy3DInto(const Trade::SceneData& scene, const Containers::StridedArrayView1D<Matrix4>& transformations) {
    return flattenMeshHierarchyIntoImplementation<3>(scene, transformations, {});
}

}}
