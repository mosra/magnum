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

#include <vector>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Utility/ConfigurationGroup.h>
#include <Corrade/PluginManager/Manager.h>

#include "Magnum/Math/Color.h"
#include "Magnum/Math/FunctionsBatch.h"
#include "Magnum/MeshTools/Combine.h"
#include "Magnum/MeshTools/CompressIndices.h"
#include "Magnum/MeshTools/Concatenate.h"
#include "Magnum/MeshTools/Copy.h"
#include "Magnum/MeshTools/Duplicate.h"
#include "Magnum/MeshTools/Filter.h"
#include "Magnum/MeshTools/FlipNormals.h"
#include "Magnum/MeshTools/GenerateIndices.h"
#include "Magnum/MeshTools/GenerateNormals.h"
#include "Magnum/MeshTools/Interleave.h"
#include "Magnum/MeshTools/RemoveDuplicates.h"
#include "Magnum/MeshTools/Transform.h"
#include "Magnum/Primitives/Cube.h"
#include "Magnum/Trade/AbstractSceneConverter.h"
#include "Magnum/Trade/MeshData.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#define _MAGNUM_NO_DEPRECATED_COMBINEINDEXEDARRAYS
#include "Magnum/MeshTools/CombineIndexedArrays.h"
#endif

#define DOXYGEN_ELLIPSIS(...) __VA_ARGS__

using namespace Magnum;
using namespace Magnum::Math::Literals;

/* Make sure the name doesn't conflict with any other snippets to avoid linker
   warnings, unlike with `int main()` there now has to be a declaration to
   avoid -Wmisssing-prototypes */
void mainMeshTools();
void mainMeshTools() {
{
/* [meshtools-interleave] */
Trade::MeshData mesh = DOXYGEN_ELLIPSIS(Trade::MeshData{{}, 0});

mesh = MeshTools::interleave(std::move(mesh));
/* [meshtools-interleave] */

/* [meshtools-compressindices] */
if(mesh.isIndexed())
    mesh = MeshTools::compressIndices(std::move(mesh));
/* [meshtools-compressindices] */

/* [meshtools-meshoptimizer] */
PluginManager::Manager<Trade::AbstractSceneConverter> manager;
Containers::Pointer<Trade::AbstractSceneConverter> meshOptimizer =
    manager.loadAndInstantiate("MeshOptimizerSceneConverter");

meshOptimizer->convertInPlace(mesh);
/* [meshtools-meshoptimizer] */
}

{
Trade::MeshData mesh{{}, 0};
void performSomeProcessing(const Containers::StridedArrayView1D<const UnsignedInt>& indices, const Containers::StridedArrayView1D<const Vector3>& positions);
/* [meshtools-generateindices] */
Trade::MeshData indexed = MeshTools::generateIndices(mesh);

performSomeProcessing(indexed.indices<UnsignedInt>(),
                      indexed.positions3DAsArray());
/* [meshtools-generateindices] */
}

{
Trade::MeshData mesh{{}, 0};
/* [meshtools-transform] */
mesh = MeshTools::transform3D(std::move(mesh), Matrix4::scaling({0.5f, 2.0f, 1.0f}));
/* [meshtools-transform] */
}

{
/* [meshtools-interleave-insert] */
Containers::ArrayView<const Color3> vertexColors = DOXYGEN_ELLIPSIS({});

Trade::MeshData coloredCube = MeshTools::interleave(Primitives::cubeSolid(), {
    Trade::MeshAttributeData{Trade::MeshAttribute::Color, vertexColors}
});
/* [meshtools-interleave-insert] */
}

{
/* [meshtools-interleave-insert-placeholder] */
Trade::MeshData coloredCube = MeshTools::interleave(Primitives::cubeSolid(), {
    Trade::MeshAttributeData{Trade::MeshAttribute::Color, VertexFormat::Vector3, nullptr}
});

for(Color3& i: coloredCube.mutableAttribute<Color3>(Trade::MeshAttribute::Color))
    i = DOXYGEN_ELLIPSIS({});
/* [meshtools-interleave-insert-placeholder] */
}

{
Trade::MeshData mesh{{}, 0};
/* [meshtools-duplicate-insert] */
Trade::MeshAttribute VertexIdAttribute = Trade::meshAttributeCustom(DOXYGEN_ELLIPSIS(0));

Trade::MeshData vertexIdMesh = MeshTools::duplicate(mesh, {
    Trade::MeshAttributeData{VertexIdAttribute, VertexFormat::UnsignedInt, nullptr}
});

UnsignedInt id = 0;
for(UnsignedInt& i: vertexIdMesh.mutableAttribute<UnsignedInt>(VertexIdAttribute))
    i = id++;
/* [meshtools-duplicate-insert] */
}

{
Trade::MeshData mesh{{}, 0};
/* [meshtools-filter] */
Trade::MeshData positionsNormals = MeshTools::filterOnlyAttributes(mesh, {
    Trade::MeshAttribute::Position,
    Trade::MeshAttribute::Normal
});
/* [meshtools-filter] */

/* [meshtools-filter-unsparse] */
positionsNormals = MeshTools::interleave(positionsNormals, {}, {});
/* [meshtools-filter-unsparse] */
}

{
Trade::MeshData mesh{{}, 0};
/* [meshtools-removeduplicates] */
Trade::MeshData deduplicated = MeshTools::removeDuplicatesFuzzy(mesh);
/* [meshtools-removeduplicates] */
}

{
Trade::MeshData mesh{{}, 0};
/* [meshtools-meshoptimizer-simplify] */
PluginManager::Manager<Trade::AbstractSceneConverter> manager;
Containers::Pointer<Trade::AbstractSceneConverter> meshOptimizer =
    manager.loadAndInstantiate("MeshOptimizerSceneConverter");
meshOptimizer->configuration().setValue("simplify", true);
meshOptimizer->configuration().setValue("simplifyTargetIndexCountThreshold", 0.1f);

Containers::Optional<Trade::MeshData> simplified = meshOptimizer->convert(mesh);
/* [meshtools-meshoptimizer-simplify] */
}

{
Trade::MeshData mesh{{}, 0};
void performSimulation(const Containers::StridedArrayView1D<const UnsignedInt>& indices, const Containers::StridedArrayView1D<const Vector3>& positions);
/* [meshtools-removeduplicates-position-only] */
Containers::Array<Vector3> positions = mesh.positions3DAsArray();

/* Deduplicate the positions and create a mapping array */
Containers::Pair<Containers::Array<UnsignedInt>, std::size_t> out =
    MeshTools::removeDuplicatesFuzzyInPlace(
        stridedArrayView(positions).slice(&Vector3::data));
Containers::Array<UnsignedInt> indexMapping = std::move(out.first());
arrayResize(positions, out.second());

/* Combine the original index buffer with the mapping array */
Containers::Array<UnsignedInt> positionIndices = MeshTools::duplicate(
    Containers::StridedArrayView1D<const UnsignedInt>{indexMapping},
    Containers::StridedArrayView1D<const UnsignedInt>{mesh.indicesAsArray()});
/* [meshtools-removeduplicates-position-only] */

/* [meshtools-removeduplicates-position-only-copy] */
performSimulation(positionIndices, positions);

/* Copy updated positions back to the original locations in the mesh */
MeshTools::duplicateInto(
    Containers::StridedArrayView1D<const UnsignedInt>{indexMapping},
    Containers::StridedArrayView1D<const Vector3>{positions},
    mesh.mutableAttribute<Vector3>(Trade::MeshAttribute::Position));
/* [meshtools-removeduplicates-position-only-copy] */
}

{
/* [meshtools-copy] */
Trade::MeshData skybox = MeshTools::copy(Primitives::cubeSolid());
MeshTools::flipNormalsInPlace(skybox.mutableIndices<UnsignedInt>(),
    skybox.mutableAttribute<Vector3>(Trade::MeshAttribute::Normal));
/* [meshtools-copy] */
}

{
Trade::MeshData mesh{{}, 0};
/* [combineFaceAttributes] */
Containers::ArrayView<const Color3> faceColors = DOXYGEN_ELLIPSIS({});

Trade::MeshData meshWithFaceColors = MeshTools::combineFaceAttributes(mesh, {
    Trade::MeshAttributeData{Trade::MeshAttribute::Color, faceColors}
});
/* [combineFaceAttributes] */
}

#ifdef MAGNUM_BUILD_DEPRECATED
{
CORRADE_IGNORE_DEPRECATED_PUSH
/* [combineIndexedArrays] */
std::vector<UnsignedInt> vertexIndices;
std::vector<Vector3> positions;
std::vector<UnsignedInt> normalTextureIndices;
std::vector<Vector3> normals;
std::vector<Vector2> textureCoordinates;

std::vector<UnsignedInt> indices = MeshTools::combineIndexedArrays(
    std::make_pair(std::cref(vertexIndices), std::ref(positions)),
    std::make_pair(std::cref(normalTextureIndices), std::ref(normals)),
    std::make_pair(std::cref(normalTextureIndices), std::ref(textureCoordinates))
);
/* [combineIndexedArrays] */
CORRADE_IGNORE_DEPRECATED_POP
}
#endif

{
/* [compressIndices-offset] */
Containers::ArrayView<const UnsignedInt> indices;
UnsignedInt offset = Math::min(indices);
Containers::Pair<Containers::Array<char>, MeshIndexType> compressed =
    MeshTools::compressIndices(indices, offset);

// use `offset` to adjust vertex attribute offset …
/* [compressIndices-offset] */
}

#ifdef MAGNUM_BUILD_DEPRECATED
{
CORRADE_IGNORE_DEPRECATED_PUSH
/* [compressIndicesAs] */
std::vector<UnsignedInt> indices;
Containers::Array<UnsignedShort> indexData =
    MeshTools::compressIndicesAs<UnsignedShort>(indices);
/* [compressIndicesAs] */
CORRADE_IGNORE_DEPRECATED_POP
}
#endif

{
/* [concatenate] */
Trade::MeshData sphere = DOXYGEN_ELLIPSIS(Trade::MeshData{{}, 0});
Trade::MeshData cube = DOXYGEN_ELLIPSIS(Trade::MeshData{{}, 0});
Trade::MeshData cylinder = DOXYGEN_ELLIPSIS(Trade::MeshData{{}, 0});

Trade::MeshData primitives = MeshTools::concatenate({sphere, cube, cylinder});
/* [concatenate] */

/* [concatenate-offsets] */
UnsignedInt sphereIndexOffset = 0;
UnsignedInt sphereVertexOffset = 0;
UnsignedInt cubeIndexOffset = sphereIndexOffset + sphere.indexCount(),
    cubeVertexOffset = sphereVertexOffset + sphere.vertexCount();
UnsignedInt cylinderIndexOffset = cubeIndexOffset + cube.indexCount(),
    cylinderVertexOffset = cubeVertexOffset + cube.vertexCount();
/* [concatenate-offsets] */
static_cast<void>(cylinderIndexOffset);
static_cast<void>(cylinderVertexOffset);
}

{
/* [generateFlatNormals] */
Containers::ArrayView<UnsignedInt> indices;
Containers::ArrayView<Vector3> indexedPositions;

Containers::Array<Vector3> positions =
    MeshTools::duplicate<UnsignedInt, Vector3>(indices, indexedPositions);

Containers::Array<Vector3> normals =
    MeshTools::generateFlatNormals(positions);
/* [generateFlatNormals] */
}

{
/* [interleave2] */
Containers::ArrayView<const Vector4> positions;
Containers::ArrayView<const UnsignedShort> weights;
Containers::ArrayView<const Color3ub> vertexColors;

auto data = MeshTools::interleave(positions, weights, 2, vertexColors, 1);
/* [interleave2] */
}

{
/* [interleave-meshdata] */
Containers::ArrayView<const UnsignedInt> indices = DOXYGEN_ELLIPSIS({});
Containers::ArrayView<const Vector3> positions = DOXYGEN_ELLIPSIS({});
Containers::ArrayView<const Vector3> normals = DOXYGEN_ELLIPSIS({});

Trade::MeshData mesh = MeshTools::interleave(
    MeshPrimitive::Triangles,
    Trade::MeshIndexData{indices}, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, positions},
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal, normals},
    });
/* [interleave-meshdata] */
}

{
Trade::MeshData data{MeshPrimitive::Lines, 0};
UnsignedInt vertexCount{};
Containers::Array<char> indexData;
/* [interleavedLayout-extra] */
Containers::ArrayView<const Trade::MeshAttributeData> attributes =
    data.attributeData();

/* Take just positions and normals and add a four-byte padding in between */
Trade::MeshData layout = MeshTools::interleavedLayout(
    Trade::MeshData{MeshPrimitive::Triangles, 0}, vertexCount, {
        attributes[data.attributeId(Trade::MeshAttribute::Position)],
        Trade::MeshAttributeData{4},
        attributes[data.attributeId(Trade::MeshAttribute::Normal)]
    });
/* [interleavedLayout-extra] */
}

{
Trade::MeshData data{MeshPrimitive::Lines, 0};
Containers::ArrayView<Trade::MeshAttributeData> extraAttributes;
UnsignedInt vertexCount{};
Containers::Array<char> indexData;
/* [interleavedLayout-indices] */
Trade::MeshData layout =
    MeshTools::interleavedLayout(data, vertexCount, extraAttributes);

Trade::MeshIndexData indices;
Trade::MeshData indexed{data.primitive(),
    std::move(indexData), indices,
    layout.releaseVertexData(), layout.releaseAttributeData()};
/* [interleavedLayout-indices] */
}

{
/* [removeDuplicates] */
Containers::ArrayView<Vector3i> data;

Containers::Pair<Containers::Array<UnsignedInt>, std::size_t> unique =
    MeshTools::removeDuplicatesInPlace(Containers::arrayCast<2, char>(data));
data = data.prefix(unique.second());
/* [removeDuplicates] */
}

{
/* [removeDuplicatesFuzzy] */
Containers::StridedArrayView1D<Float> data;

Containers::Pair<Containers::Array<UnsignedInt>, std::size_t> unique =
    MeshTools::removeDuplicatesFuzzyInPlace(data);
data = data.prefix(unique.second());
/* [removeDuplicatesFuzzy] */
}

{
/* [removeDuplicatesFuzzy-vector-slice] */
Containers::StridedArrayView1D<Vector3> positions;

Containers::Pair<Containers::Array<UnsignedInt>, std::size_t> unique =
    MeshTools::removeDuplicatesFuzzyInPlace(positions.slice(&Vector3::data));
positions = positions.prefix(unique.second());
/* [removeDuplicatesFuzzy-vector-slice] */
}

#ifdef MAGNUM_BUILD_DEPRECATED
{
CORRADE_IGNORE_DEPRECATED_PUSH
/* [removeDuplicates-multiple] */
std::vector<Vector3> positions;
std::vector<Vector2> texCoords;

std::vector<UnsignedInt> positionIndices = MeshTools::removeDuplicates(positions);
std::vector<UnsignedInt> texCoordIndices = MeshTools::removeDuplicates(texCoords);

std::vector<UnsignedInt> indices = MeshTools::combineIndexedArrays(
    std::make_pair(std::cref(positionIndices), std::ref(positions)),
    std::make_pair(std::cref(texCoordIndices), std::ref(texCoords))
);
/* [removeDuplicates-multiple] */
CORRADE_IGNORE_DEPRECATED_POP
}
#endif

{
/* [transformVectors] */
std::vector<Vector3> vectors;
auto transformation = Quaternion::rotation(35.0_degf, Vector3::yAxis());
MeshTools::transformVectorsInPlace(transformation, vectors);
/* [transformVectors] */
}

{
/* [transformPoints] */
std::vector<Vector3> points;
auto transformation =
    DualQuaternion::rotation(35.0_degf, Vector3::yAxis())*
    DualQuaternion::translation({0.5f, -1.0f, 3.0f});
MeshTools::transformPointsInPlace(transformation, points);
/* [transformPoints] */
}

}
