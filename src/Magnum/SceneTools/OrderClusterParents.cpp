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

#include "OrderClusterParents.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/ArrayTuple.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>

#include "Magnum/Trade/SceneData.h"

namespace Magnum { namespace SceneTools {

Containers::Array<Containers::Pair<UnsignedInt, Int>> orderClusterParents(const Trade::SceneData& scene) {
    const Containers::Optional<UnsignedInt> parentFieldId = scene.findFieldId(Trade::SceneField::Parent);
    CORRADE_ASSERT(parentFieldId,
        "SceneTools::orderClusterParents(): the scene has no hierarchy", {});
    Containers::Array<Containers::Pair<UnsignedInt, Int>> out{NoInit, scene.fieldSize(*parentFieldId)};
    Containers::StridedArrayView1D<UnsignedInt> mapping{out, &out.data()->first(), out.size(), sizeof(decltype(out)::Type)};
    Containers::StridedArrayView1D<Int> parentOffset{out, &out.data()->second(), out.size(), sizeof(decltype(out)::Type)};
    orderClusterParentsInto(scene, mapping, parentOffset);
    return out;
}

void orderClusterParentsInto(const Trade::SceneData& scene, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Int>& parentDestination) {
    const Containers::Optional<UnsignedInt> parentFieldId = scene.findFieldId(Trade::SceneField::Parent);
    CORRADE_ASSERT(parentFieldId,
        "SceneTools::orderClusterParentsInto(): the scene has no hierarchy", );
    const std::size_t parentFieldSize = scene.fieldSize(*parentFieldId);
    CORRADE_ASSERT(mappingDestination.size() == parentFieldSize,
        "SceneTools::orderClusterParentsInto(): expected mapping destination view with" << parentFieldSize << "elements but got" << mappingDestination.size(), );
    CORRADE_ASSERT(parentDestination.size() == scene.fieldSize(*parentFieldId),
        "SceneTools::orderClusterParentsInto(): expected parent destination view with" << parentFieldSize << "elements but got" << parentDestination.size(), );

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
        /* List of parents to process. Can't reuse mappingDestination because
           this includes one more element for root objects. */
        {NoInit, parentFieldSize + 1, parentsToProcess}
    };

    /* Convert the parent list to a child list to sort them toplogically.
       Explicit slice() template parameters needed by GCC 4.8 and MSVC 2015 */
    scene.parentsInto(
        stridedArrayView(parents).slice<UnsignedInt>(&Containers::Pair<UnsignedInt, Int>::first),
        stridedArrayView(parents).slice<Int>(&Containers::Pair<UnsignedInt, Int>::second)
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
            /** @todo better diagnostic once we can use a BitArray to detect
                which nodes are parented more than once (OTOH maybe that's
                unnecessary extra work which isn't desired to be done here but
                should be instead in a dedicated cycle/sparse checker utility?) */
            CORRADE_ASSERT_OUTPUT(outputOffset < parents.size(),
                "SceneTools::orderClusterParents(): hierarchy is cyclic", );
            parentsToProcess[outputOffset + 1] = children[j];
            mappingDestination[outputOffset] = children[j];
            parentDestination[outputOffset] = objectId;
            ++outputOffset;
        }
    }

    /** @todo better diagnostic once we can use a BitArray to detect which
        nodes are unreachable from root (OTOH again maybe that's undesirable
        extra work that doesn't belong here?) */
    CORRADE_ASSERT(outputOffset == parents.size(),
        "SceneTools::orderClusterParents(): hierarchy is sparse", );
}

}}
