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

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/BitArray.h>
#include <Corrade/Containers/BitArrayView.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/Triple.h>

#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/MeshTools/Concatenate.h"
#include "Magnum/MeshTools/Transform.h"
#include "Magnum/SceneTools/Filter.h"
#include "Magnum/SceneTools/Hierarchy.h"
#include "Magnum/Trade/SceneData.h"
#include "Magnum/Trade/MeshData.h"

#define DOXYGEN_ELLIPSIS(...) __VA_ARGS__

using namespace Magnum;

/* Make sure the name doesn't conflict with any other snippets to avoid linker
   warnings, unlike with `int main()` there now has to be a declaration to
   avoid -Wmisssing-prototypes */
void mainSceneTools();
void mainSceneTools() {
{
/* [filterFieldEntries-shared-mapping] */
Trade::SceneData scene = DOXYGEN_ELLIPSIS(Trade::SceneData{{}, 0, nullptr, {}});

Containers::BitArray transformationsToKeep = DOXYGEN_ELLIPSIS({});
Containers::BitArray lightsToKeep = DOXYGEN_ELLIPSIS({});

/* Mesh and MeshMaterial fields stay unchanged */
Trade::SceneData filtered = SceneTools::filterFieldEntries(scene, {
    {Trade::SceneField::Translation, transformationsToKeep},
    {Trade::SceneField::Rotation, transformationsToKeep},
    {Trade::SceneField::Light, lightsToKeep}
});
/* [filterFieldEntries-shared-mapping] */
}

{
/* [absoluteFieldTransformations2D-mesh-concatenate] */
Trade::SceneData scene = DOXYGEN_ELLIPSIS(Trade::SceneData{{}, 0, nullptr, {}});
Containers::Array<Trade::MeshData> meshes = DOXYGEN_ELLIPSIS({});

Containers::Array<Containers::Pair<UnsignedInt, Containers::Pair<UnsignedInt, Int>>>
    meshesMaterials = scene.meshesMaterialsAsArray();
Containers::Array<Matrix3> transformations =
    SceneTools::absoluteFieldTransformations2D(scene, Trade::SceneField::Mesh);

/* Since a mesh can be referenced multiple times, we can't operate in-place */
Containers::Array<Trade::MeshData> flattenedMeshes;
for(std::size_t i = 0; i != meshesMaterials.size(); ++i) {
    arrayAppend(flattenedMeshes, MeshTools::transform2D(
        meshes[meshesMaterials[i].second().first()], transformations[i]));
}

Trade::MeshData concatenated = MeshTools::concatenate(flattenedMeshes);
/* [absoluteFieldTransformations2D-mesh-concatenate] */
} {
/* [absoluteFieldTransformations3D-mesh-concatenate] */
Trade::SceneData scene = DOXYGEN_ELLIPSIS(Trade::SceneData{{}, 0, nullptr, {}});
Containers::Array<Trade::MeshData> meshes = DOXYGEN_ELLIPSIS({});

Containers::Array<Containers::Pair<UnsignedInt, Containers::Pair<UnsignedInt, Int>>>
    meshesMaterials = scene.meshesMaterialsAsArray();
Containers::Array<Matrix4> transformations =
    SceneTools::absoluteFieldTransformations3D(scene, Trade::SceneField::Mesh);

/* Since a mesh can be referenced multiple times, we can't operate in-place */
Containers::Array<Trade::MeshData> flattenedMeshes;
for(std::size_t i = 0; i != meshesMaterials.size(); ++i) {
    arrayAppend(flattenedMeshes, MeshTools::transform3D(
        meshes[meshesMaterials[i].second().first()], transformations[i]));
}

Trade::MeshData concatenated = MeshTools::concatenate(flattenedMeshes);
/* [absoluteFieldTransformations3D-mesh-concatenate] */
}

{
/* [childrenDepthFirst-extract-tree] */
Trade::SceneData scene = DOXYGEN_ELLIPSIS(Trade::SceneData{{}, 0, nullptr, {}});

Containers::Array<Containers::Pair<UnsignedInt, UnsignedInt>> childrenRanges =
    SceneTools::childrenDepthFirst(scene);

/* Bit array of objects to keep from the scene */
UnsignedInt objectToLookFor = DOXYGEN_ELLIPSIS(0);
Containers::BitArray objectsToKeep{ValueInit, std::size_t(scene.mappingBound())};

/* Look for the object in the list */
for(const Containers::Pair<UnsignedInt, UnsignedInt>& i: childrenRanges) {
    if(i.first() != objectToLookFor) continue;

    /* Right after the object appearing in the list is all its (nested)
       children, mark them in the bit array */
    for(const Containers::Pair<UnsignedInt, UnsignedInt>& j:
        childrenRanges.sliceSize(&i, i.second() + 1))
            objectsToKeep.set(j.first());

    break;
}

/* Filter the scene to contain just given object and its children, and reparent
   it to be in scene root */
Trade::SceneData filtered = SceneTools::filterObjects(scene, objectsToKeep);
filtered.mutableField<Int>(Trade::SceneField::Parent)[
    filtered.fieldObjectOffset(Trade::SceneField::Parent, objectToLookFor)
] = -1;
/* [childrenDepthFirst-extract-tree] */
}

{
/* [parentsBreadthFirst-transformations] */
Trade::SceneData scene = DOXYGEN_ELLIPSIS(Trade::SceneData{{}, 0, nullptr, {}});

/* Put all transformations into an array indexed by object ID. Objects
   implicitly have an identity transformation, first element is reserved for
   the global transformation. */
Containers::Array<Matrix4> transformations{std::size_t(scene.mappingBound() + 1)};
for(const Containers::Pair<UnsignedInt, Matrix4>& transformation:
    scene.transformations3DAsArray())
{
    transformations[transformation.first() + 1] = transformation.second();
}

/* Go through ordered parents and compose absolute transformations for all
   nodes in the hierarchy, objects in the root use transformations[0]. The
   function ensures that the parent transformation is already calculated when
   referenced by child nodes. */
for(const Containers::Pair<UnsignedInt, Int>& parent:
    SceneTools::parentsBreadthFirst(scene))
{
    transformations[parent.first() + 1] =
        transformations[parent.second() + 1]*
        transformations[parent.first() + 1];
}
/* [parentsBreadthFirst-transformations] */
}
}
