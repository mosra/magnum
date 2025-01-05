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

#include "Hierarchy.h"

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

namespace Magnum { namespace SceneTools {

Containers::Array<Containers::Pair<UnsignedInt, Int>> parentsBreadthFirst(const Trade::SceneData& scene) {
    const Containers::Optional<UnsignedInt> parentFieldId = scene.findFieldId(Trade::SceneField::Parent);
    CORRADE_ASSERT(parentFieldId,
        "SceneTools::parentsBreadthFirst(): the scene has no hierarchy", {});
    Containers::Array<Containers::Pair<UnsignedInt, Int>> out{NoInit, scene.fieldSize(*parentFieldId)};
    parentsBreadthFirstInto(scene,
        stridedArrayView(out).slice(&decltype(out)::Type::first),
        stridedArrayView(out).slice(&decltype(out)::Type::second));
    return out;
}

void parentsBreadthFirstInto(const Trade::SceneData& scene, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Int>& parentDestination) {
    const Containers::Optional<UnsignedInt> parentFieldId = scene.findFieldId(Trade::SceneField::Parent);
    CORRADE_ASSERT(parentFieldId,
        "SceneTools::parentsBreadthFirstInto(): the scene has no hierarchy", );
    const std::size_t parentFieldSize = scene.fieldSize(*parentFieldId);
    CORRADE_ASSERT(mappingDestination.size() == parentFieldSize,
        "SceneTools::parentsBreadthFirstInto(): expected mapping destination view with" << parentFieldSize << "elements but got" << mappingDestination.size(), );
    CORRADE_ASSERT(parentDestination.size() == parentFieldSize,
        "SceneTools::parentsBreadthFirstInto(): expected parent destination view with" << parentFieldSize << "elements but got" << parentDestination.size(), );

    /* Allocate a single storage for all temporary data */
    Containers::ArrayView<Containers::Pair<UnsignedInt, Int>> parents;
    Containers::ArrayView<UnsignedInt> childrenOffsets;
    Containers::ArrayView<UnsignedInt> children;
    Containers::ArrayView<Int> parentsToProcess;
    Containers::ArrayTuple storage{
        /* Output of scene.parentsInto() */
        {NoInit, parentFieldSize, parents},
        /* Running children offset (+1) for each node including root (+1), plus
           one more element when we shift the array by one below */
        {ValueInit, std::size_t(scene.mappingBound() + 3), childrenOffsets},
        {NoInit, parentFieldSize, children},
        /* Queue of parents to process. Can't reuse mappingDestination because
           this includes one more element for root objects. */
        {NoInit, parentFieldSize + 1, parentsToProcess}
    };

    /* Convert the parent list to a child list to sort them toplogically */
    scene.parentsInto(
        stridedArrayView(parents).slice(&decltype(parents)::Type::first),
        stridedArrayView(parents).slice(&decltype(parents)::Type::second)
    );

    /* Children offset for each node including root. First calculate the count
       of children for each, skipping the first element (parent.second() can be
       -1, accounting for that as well)... */
    for(const Containers::Pair<UnsignedInt, Int>& parent: parents) {
        CORRADE_INTERNAL_ASSERT(parent.first() < scene.mappingBound() && (parent.second() == -1 || UnsignedInt(parent.second()) < scene.mappingBound()));
        ++childrenOffsets[parent.second() + 2];
    }

    /* ... then convert the counts to a running offset. Now
       `[childrenOffsets[i + 2], childrenOffsets[i + 3])` contains a range in
       which the `children` array below contains a list of children for `i`. */
    UnsignedInt offset = 0;
    for(UnsignedInt& i: childrenOffsets) {
        UnsignedInt nextOffset = offset + i;
        i = offset;
        offset = nextOffset;
    }
    CORRADE_INTERNAL_ASSERT(offset == parents.size());

    /* Go through the parent list again, convert that to child ranges. The
       childrenOffsets array gets shifted by one element by the process, thus
       now `[childrenOffsets[i + 1], childrenOffsets[i + 2])` contains a range
       in which the `children` array below contains a list of children for
       `i`. */
    for(const Containers::Pair<UnsignedInt, Int>& parent: parents)
        children[childrenOffsets[parent.second() + 2]++] = parent.first();

    /* Go breadth-first (so we have nodes sharing the same parent next to each
       other) and build a list of (id, parent id) where a parent is always
       before its children */
    std::size_t outputOffset = 0;
    parentsToProcess[0] = -1;
    for(std::size_t i = 0; i != outputOffset + 1; ++i) {
        const Int objectId = parentsToProcess[i];
        for(std::size_t j = childrenOffsets[objectId + 1], jMax = childrenOffsets[objectId + 2]; j != jMax; ++j) {
            /** @todo better diagnostic with BitArray to detect which nodes are
                parented more than once (OTOH maybe that's unnecessary extra
                work which isn't desired to be done here but should be instead
                in a dedicated cycle/sparse checker utility?) */
            CORRADE_ASSERT(outputOffset < parents.size(),
                "SceneTools::parentsBreadthFirst(): hierarchy is cyclic", );
            parentsToProcess[outputOffset + 1] = children[j];
            mappingDestination[outputOffset] = children[j];
            parentDestination[outputOffset] = objectId;
            ++outputOffset;
        }
    }

    /** @todo better diagnostic with BitArray to detect which nodes are
        unreachable from root (OTOH again maybe that's undesirable extra work
        that doesn't belong here?) */
    CORRADE_ASSERT(outputOffset == parents.size(),
        "SceneTools::parentsBreadthFirst(): hierarchy is sparse", );
}

Containers::Array<Containers::Pair<UnsignedInt, UnsignedInt>> childrenDepthFirst(const Trade::SceneData& scene) {
    const Containers::Optional<UnsignedInt> parentFieldId = scene.findFieldId(Trade::SceneField::Parent);
    CORRADE_ASSERT(parentFieldId,
        "SceneTools::childrenDepthFirst(): the scene has no hierarchy", {});
    Containers::Array<Containers::Pair<UnsignedInt, UnsignedInt>> out{NoInit, scene.fieldSize(*parentFieldId)};
    childrenDepthFirstInto(scene,
        stridedArrayView(out).slice(&decltype(out)::Type::first),
        stridedArrayView(out).slice(&decltype(out)::Type::second));
    return out;
}

void childrenDepthFirstInto(const Trade::SceneData& scene, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<UnsignedInt>& childCountDestination) {
    const Containers::Optional<UnsignedInt> parentFieldId = scene.findFieldId(Trade::SceneField::Parent);
    CORRADE_ASSERT(parentFieldId,
        "SceneTools::childrenDepthFirstInto(): the scene has no hierarchy", );
    const std::size_t parentFieldSize = scene.fieldSize(*parentFieldId);
    CORRADE_ASSERT(mappingDestination.size() == parentFieldSize,
        "SceneTools::childrenDepthFirstInto(): expected mapping destination view with" << parentFieldSize << "elements but got" << mappingDestination.size(), );
    CORRADE_ASSERT(childCountDestination.size() == parentFieldSize,
        "SceneTools::childrenDepthFirstInto(): expected child count destination view with" << parentFieldSize << "elements but got" << childCountDestination.size(), );

    /* Allocate a single storage for all temporary data */
    Containers::ArrayView<Containers::Pair<UnsignedInt, Int>> parents;
    Containers::ArrayView<UnsignedInt> childrenOffsets;
    Containers::ArrayView<UnsignedInt> children;
    /* Parent ID, offset of the first child in `childCountDestination`, offset
       of next child in `children` to process */
    Containers::ArrayView<Containers::Triple<Int, UnsignedInt, UnsignedInt>> parentsToProcess;
    Containers::ArrayTuple storage{
        /* Output of scene.parentsInto() */
        {NoInit, parentFieldSize, parents},
        /* Running children offset (+1) for each node including root (+1), plus
           one more element when we shift the array by one below */
        {ValueInit, std::size_t(scene.mappingBound() + 3), childrenOffsets},
        {NoInit, parentFieldSize, children},
        /* A stack of parents to process. It only reaches `parentFieldSize + 1`
           if the hierarchy is a single branch, usually it's shorter. */
        {NoInit, parentFieldSize + 1, parentsToProcess}
    };

    /* Convert the parent list to a child list to sort them toplogically */
    scene.parentsInto(
        stridedArrayView(parents).slice(&decltype(parents)::Type::first),
        stridedArrayView(parents).slice(&decltype(parents)::Type::second)
    );

    /* Children offset for each node including root. First calculate the count
       of children for each, skipping the first element (parent.second() can be
       -1, accounting for that as well)... */
    for(const Containers::Pair<UnsignedInt, Int>& parent: parents) {
        CORRADE_INTERNAL_ASSERT(parent.first() < scene.mappingBound() && (parent.second() == -1 || UnsignedInt(parent.second()) < scene.mappingBound()));
        ++childrenOffsets[parent.second() + 2];
    }

    /* ... then convert the counts to a running offset. Now
       `[childrenOffsets[i + 2], childrenOffsets[i + 3])` contains a range in
       which the `children` array below contains a list of children for `i`. */
    UnsignedInt offset = 0;
    for(UnsignedInt& i: childrenOffsets) {
        UnsignedInt nextOffset = offset + i;
        i = offset;
        offset = nextOffset;
    }
    CORRADE_INTERNAL_ASSERT(offset == parents.size());

    /* Go through the parent list again, convert that to child ranges. The
       childrenOffsets array gets shifted by one element by the process, thus
       now `[childrenOffsets[i + 1], childrenOffsets[i + 2])` contains a range
       in which the `children` array below contains a list of children for
       `i`. */
    for(const Containers::Pair<UnsignedInt, Int>& parent: parents)
        children[childrenOffsets[parent.second() + 2]++] = parent.first();

    UnsignedInt outputOffset = 0;
    std::size_t parentsToProcessOffset = 0;
    parentsToProcess[parentsToProcessOffset++] = {-1, outputOffset, childrenOffsets[-1 + 1]};
    while(parentsToProcessOffset) {
        const Int objectId = parentsToProcess[parentsToProcessOffset - 1].first();
        UnsignedInt& childrenOffset = parentsToProcess[parentsToProcessOffset - 1].third();

        /* If all children were processed, we're done with this object */
        if(childrenOffset == childrenOffsets[objectId + 2]) {
            /* Save the total size. Only if it's not the root objects, for them
               the total size is implicitly the whole output size. */
            if(objectId != -1) {
                const UnsignedInt firstChildOutputOffset = parentsToProcess[parentsToProcessOffset - 1].second();
                childCountDestination[firstChildOutputOffset - 1] = outputOffset - firstChildOutputOffset;
            }

            /* Remove from the processing stack and continue with next */
            --parentsToProcessOffset;
            continue;
        }

        CORRADE_INTERNAL_DEBUG_ASSERT(childrenOffset < childrenOffsets[objectId + 2]);
        CORRADE_INTERNAL_DEBUG_ASSERT(parentsToProcessOffset < parentFieldSize + 1);
        /** @todo better diagnostic with BitArray to detect which nodes are
            parented more than once (OTOH maybe that's undesirable extra work
            that would be duplicated here and in parentsBreadthFirst()?) */
        CORRADE_ASSERT(outputOffset < parents.size(),
            "SceneTools::childrenDepthFirst(): hierarchy is cyclic", );

        /* Add the current child to the mapping output and to the list of
           parents to process next. Increment all offsets for the next
           round. */
        const UnsignedInt childObjectId = children[childrenOffset++];
        mappingDestination[outputOffset] = childObjectId;
        parentsToProcess[parentsToProcessOffset++] = {Int(childObjectId), ++outputOffset, childrenOffsets[childObjectId + 1]};
    }

    CORRADE_INTERNAL_ASSERT(parentsToProcessOffset == 0);

    /** @todo better diagnostic with BitArray to detect which nodes are
        unreachable from root (OTOH again maybe that's undesirable extra work
        that would be duplicated here and in parentsBreadthFirst()?) */
    CORRADE_ASSERT(outputOffset == parents.size(),
        "SceneTools::childrenDepthFirst(): hierarchy is sparse", );
}

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

template<UnsignedInt dimensions> void absoluteFieldTransformationsIntoImplementation(const Trade::SceneData& scene, const UnsignedInt fieldId, const Containers::StridedArrayView1D<MatrixTypeFor<dimensions, Float>>& outputTransformations, const MatrixTypeFor<dimensions, Float>& globalTransformation) {
    CORRADE_ASSERT(SceneDataDimensionTraits<dimensions>::isDimensions(scene),
        "SceneTools::absoluteFieldTransformations(): the scene is not" << dimensions << Debug::nospace << "D", );
    CORRADE_ASSERT(fieldId < scene.fieldCount(),
        "SceneTools::absoluteFieldTransformations(): index" << fieldId << "out of range for" << scene.fieldCount() << "fields", );
    const Containers::Optional<UnsignedInt> parentFieldId = scene.findFieldId(Trade::SceneField::Parent);
    CORRADE_ASSERT(parentFieldId,
        "SceneTools::absoluteFieldTransformations(): the scene has no hierarchy", );
    CORRADE_ASSERT(outputTransformations.size() == scene.fieldSize(fieldId),
        "SceneTools::absoluteFieldTransformationsInto(): bad output size, expected" << scene.fieldSize(fieldId) << "but got" << outputTransformations.size(), );

    /* Allocate a single storage for all temporary data */
    Containers::ArrayView<Containers::Pair<UnsignedInt, Int>> orderedClusteredParents;
    Containers::ArrayView<Containers::Pair<UnsignedInt, MatrixTypeFor<dimensions, Float>>> transformations;
    Containers::ArrayView<MatrixTypeFor<dimensions, Float>> absoluteTransformations;
    Containers::ArrayTuple storage{
        /* Output of parentsBreadthFirstInto() */
        {NoInit, scene.fieldSize(*parentFieldId), orderedClusteredParents},
        /* Output of scene.transformationsXDInto() */
        {NoInit, scene.transformationFieldSize(), transformations},
        /* Above transformations but indexed by object ID */
        {ValueInit, std::size_t(scene.mappingBound() + 1), absoluteTransformations}
    };
    parentsBreadthFirstInto(scene,
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
    scene.mappingInto(fieldId, mapping);
    for(std::size_t i = 0; i != mapping.size(); ++i) {
        CORRADE_INTERNAL_ASSERT(mapping[i] < scene.mappingBound());
        outputTransformations[i] = absoluteTransformations[mapping[i] + 1];
    }
}

template<UnsignedInt dimensions> void absoluteFieldTransformationsIntoImplementation(const Trade::SceneData& scene, const Trade::SceneField field, const Containers::StridedArrayView1D<MatrixTypeFor<dimensions, Float>>& outputTransformations, const MatrixTypeFor<dimensions, Float>& globalTransformation) {
    const Containers::Optional<UnsignedInt> fieldId = scene.findFieldId(field);
    CORRADE_ASSERT(fieldId,
        "SceneTools::absoluteFieldTransformationsInto(): field" << field << "not found", );

    absoluteFieldTransformationsIntoImplementation<dimensions>(scene, *fieldId, outputTransformations, globalTransformation);
}

template<UnsignedInt dimensions> Containers::Array<MatrixTypeFor<dimensions, Float>> absoluteFieldTransformationsImplementation(const Trade::SceneData& scene, const UnsignedInt fieldId, const MatrixTypeFor<dimensions, Float>& globalTransformation) {
    CORRADE_ASSERT(fieldId < scene.fieldCount(),
        "SceneTools::absoluteFieldTransformations(): index" << fieldId << "out of range for" << scene.fieldCount() << "fields", {});

    Containers::Array<MatrixTypeFor<dimensions, Float>> out{NoInit, scene.fieldSize(fieldId)};
    absoluteFieldTransformationsIntoImplementation<dimensions>(scene, fieldId, out, globalTransformation);
    return out;
}

template<UnsignedInt dimensions> Containers::Array<MatrixTypeFor<dimensions, Float>> absoluteFieldTransformationsImplementation(const Trade::SceneData& scene, const Trade::SceneField field, const MatrixTypeFor<dimensions, Float>& globalTransformation) {
    const Containers::Optional<UnsignedInt> fieldId = scene.findFieldId(field);
    CORRADE_ASSERT(fieldId,
        "SceneTools::absoluteFieldTransformations(): field" << field << "not found", {});

    Containers::Array<MatrixTypeFor<dimensions, Float>> out{NoInit, scene.fieldSize(*fieldId)};
    absoluteFieldTransformationsIntoImplementation<dimensions>(scene, *fieldId, out, globalTransformation);
    return out;
}

}

Containers::Array<Matrix3> absoluteFieldTransformations2D(const Trade::SceneData& scene, const Trade::SceneField field, const Matrix3& globalTransformation) {
    return absoluteFieldTransformationsImplementation<2>(scene, field, globalTransformation);
}

Containers::Array<Matrix3> absoluteFieldTransformations2D(const Trade::SceneData& scene, const Trade::SceneField field) {
    return absoluteFieldTransformationsImplementation<2>(scene, field, {});
}

Containers::Array<Matrix3> absoluteFieldTransformations2D(const Trade::SceneData& scene, const UnsignedInt fieldId, const Matrix3& globalTransformation) {
    return absoluteFieldTransformationsImplementation<2>(scene, fieldId, globalTransformation);
}

Containers::Array<Matrix3> absoluteFieldTransformations2D(const Trade::SceneData& scene, const UnsignedInt fieldId) {
    return absoluteFieldTransformationsImplementation<2>(scene, fieldId, {});
}

void absoluteFieldTransformations2DInto(const Trade::SceneData& scene, const Trade::SceneField field, const Containers::StridedArrayView1D<Matrix3>& transformations, const Matrix3& globalTransformation) {
    return absoluteFieldTransformationsIntoImplementation<2>(scene, field, transformations, globalTransformation);
}

void absoluteFieldTransformations2DInto(const Trade::SceneData& scene, const Trade::SceneField field, const Containers::StridedArrayView1D<Matrix3>& transformations) {
    return absoluteFieldTransformationsIntoImplementation<2>(scene, field, transformations, {});
}

void absoluteFieldTransformations2DInto(const Trade::SceneData& scene, const UnsignedInt fieldId, const Containers::StridedArrayView1D<Matrix3>& transformations, const Matrix3& globalTransformation) {
    return absoluteFieldTransformationsIntoImplementation<2>(scene, fieldId, transformations, globalTransformation);
}

void absoluteFieldTransformations2DInto(const Trade::SceneData& scene, const UnsignedInt fieldId, const Containers::StridedArrayView1D<Matrix3>& transformations) {
    return absoluteFieldTransformationsIntoImplementation<2>(scene, fieldId, transformations, {});
}

Containers::Array<Matrix4> absoluteFieldTransformations3D(const Trade::SceneData& scene, const Trade::SceneField field, const Matrix4& globalTransformation) {
    return absoluteFieldTransformationsImplementation<3>(scene, field, globalTransformation);
}

Containers::Array<Matrix4> absoluteFieldTransformations3D(const Trade::SceneData& scene, const Trade::SceneField field) {
    return absoluteFieldTransformationsImplementation<3>(scene, field, {});
}

Containers::Array<Matrix4> absoluteFieldTransformations3D(const Trade::SceneData& scene, const UnsignedInt fieldId, const Matrix4& globalTransformation) {
    return absoluteFieldTransformationsImplementation<3>(scene, fieldId, globalTransformation);
}

Containers::Array<Matrix4> absoluteFieldTransformations3D(const Trade::SceneData& scene, const UnsignedInt fieldId) {
    return absoluteFieldTransformationsImplementation<3>(scene, fieldId, {});
}

void absoluteFieldTransformations3DInto(const Trade::SceneData& scene, const Trade::SceneField field, const Containers::StridedArrayView1D<Matrix4>& transformations, const Matrix4& globalTransformation) {
    return absoluteFieldTransformationsIntoImplementation<3>(scene, field, transformations, globalTransformation);
}

void absoluteFieldTransformations3DInto(const Trade::SceneData& scene, const Trade::SceneField field, const Containers::StridedArrayView1D<Matrix4>& transformations) {
    return absoluteFieldTransformationsIntoImplementation<3>(scene, field, transformations, {});
}

void absoluteFieldTransformations3DInto(const Trade::SceneData& scene, const UnsignedInt fieldId, const Containers::StridedArrayView1D<Matrix4>& transformations, const Matrix4& globalTransformation) {
    return absoluteFieldTransformationsIntoImplementation<3>(scene, fieldId, transformations, globalTransformation);
}

void absoluteFieldTransformations3DInto(const Trade::SceneData& scene, const UnsignedInt fieldId, const Containers::StridedArrayView1D<Matrix4>& transformations) {
    return absoluteFieldTransformationsIntoImplementation<3>(scene, fieldId, transformations, {});
}

}}
