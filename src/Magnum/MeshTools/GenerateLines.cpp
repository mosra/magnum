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

#include "GenerateLines.h"

#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/MeshTools/Duplicate.h"
#include "Magnum/MeshTools/GenerateIndices.h"

/* This header is included only privately and doesn't introduce any linker
   dependency (taking just the LineVertexAnnotations enum), thus it's
   completely safe to not link to the Shaders library */
#include "Magnum/Shaders/Line.h"

namespace Magnum { namespace MeshTools {

Trade::MeshData generateLines(const Trade::MeshData& lineMesh) {
    CORRADE_ASSERT(lineMesh.primitive() == MeshPrimitive::Lines ||
                   lineMesh.primitive() == MeshPrimitive::LineStrip ||
                   lineMesh.primitive() == MeshPrimitive::LineLoop,
        "MeshTools::generateLines(): expected a line primitive, got" << lineMesh.primitive(), (Trade::MeshData{MeshPrimitive::Triangles, 0}));

    /** @todo this will assert if the count in MeshData is wrong, check here
        already */
    const UnsignedInt quadCount = primitiveCount(lineMesh.primitive(), lineMesh.isIndexed() ? lineMesh.indexCount() : lineMesh.vertexCount());

    /** @todo combine this allocation with pointDuplicationIndices below, and
        then reuse for the final index buffer */
    Containers::Array<UnsignedInt> originalIndices;
    if(lineMesh.primitive() == MeshPrimitive::Lines) {
        if(lineMesh.isIndexed())
            originalIndices = lineMesh.indicesAsArray();
    } else {
        if(lineMesh.primitive() == MeshPrimitive::LineStrip) {
            if(lineMesh.isIndexed())
                originalIndices = generateLineStripIndices(lineMesh.indices());
            else
                originalIndices = generateLineStripIndices(lineMesh.vertexCount());
        } else if(lineMesh.primitive() == MeshPrimitive::LineLoop) {
            if(lineMesh.isIndexed())
                originalIndices = generateLineLoopIndices(lineMesh.indices());
            else
                originalIndices = generateLineLoopIndices(lineMesh.vertexCount());
        } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    }

    /* Create a source index array for duplicate() by combining indices of a
       form 00112233 (i.e., duplicating every point twice) with the original
       mesh indices (if there are any) */
    Containers::Array<UnsignedInt> pointDuplicationIndices{NoInit, quadCount*4};
    for(UnsignedInt i = 0; i != quadCount; ++i) {
        pointDuplicationIndices[i*4 + 0] =
            pointDuplicationIndices[i*4 + 1] = i*2 + 0;
        pointDuplicationIndices[i*4 + 2] =
            pointDuplicationIndices[i*4 + 3] = i*2 + 1;
    }
    if(originalIndices)
        duplicateInto(pointDuplicationIndices, Containers::arrayCast<2, char>(stridedArrayView(originalIndices)), Containers::arrayCast<2, char>(stridedArrayView(pointDuplicationIndices)));

    /* Position is required, everything else is optional */
    const Containers::Optional<UnsignedInt> positionAttributeId = lineMesh.findAttributeId(Trade::MeshAttribute::Position);
    CORRADE_ASSERT(positionAttributeId,
        "MeshTools::generateLines(): the mesh has no positions", (Trade::MeshData{MeshPrimitive::Triangles, 0}));

    /* Duplicate the input mesh to have each input line segment turned into
       four vertices for a quad. Allocate space for the additional attributes
       as well. */
    Trade::MeshData mesh = duplicate(Trade::MeshData{MeshPrimitive::Triangles, {}, pointDuplicationIndices, Trade::MeshIndexData{pointDuplicationIndices}, {}, lineMesh.vertexData(), Trade::meshAttributeDataNonOwningArray(lineMesh.attributeData()), lineMesh.vertexCount()}, {
        Trade::MeshAttributeData{Implementation::LineMeshAttributePreviousPosition,
            lineMesh.attributeFormat(*positionAttributeId), nullptr},
        Trade::MeshAttributeData{Implementation::LineMeshAttributeNextPosition,
            lineMesh.attributeFormat(*positionAttributeId), nullptr},
        /** @todo use a 8-bit type and make the attribute non-interleaved to
            save space? */
        Trade::MeshAttributeData{Implementation::LineMeshAttributeAnnotation,
            VertexFormat::UnsignedInt, nullptr},
    });

    CORRADE_INTERNAL_ASSERT(mesh.attributeName(*positionAttributeId) == Trade::MeshAttribute::Position);

    /* Fill in previous/next positions, if we have any vertices at all */
    if(quadCount) {
        /* Form 3D arrays where the second dimension is 2 elements */
        const Containers::StridedArrayView2D<const char> positions = mesh.attribute(Trade::MeshAttribute::Position);
        const Containers::StridedArrayView3D<const char> positions3{mesh.vertexData(), static_cast<const char*>(positions.data()),
            {positions.size()[0]/2, 2, positions.size()[1]},
            {positions.stride()[0]*2, positions.stride()[0], positions.stride()[1]}};

        const Containers::StridedArrayView2D<char> previousPositions = mesh.mutableAttribute(Implementation::LineMeshAttributePreviousPosition);
        const Containers::StridedArrayView3D<char> previousPositions3{mesh.mutableVertexData(), static_cast<char*>(previousPositions.data()),
            {previousPositions.size()[0]/2, 2, previousPositions.size()[1]},
            {previousPositions.stride()[0]*2, previousPositions.stride()[0], previousPositions.stride()[1]}};

        const Containers::StridedArrayView2D<char> nextPositions = mesh.mutableAttribute(Implementation::LineMeshAttributeNextPosition);
        const Containers::StridedArrayView3D<char> nextPositions3{mesh.mutableVertexData(), static_cast<char*>(nextPositions.data()),
            {nextPositions.size()[0]/2, 2, nextPositions.size()[1]},
            {nextPositions.stride()[0]*2, nextPositions.stride()[0], nextPositions.stride()[1]}};

        /* Zero-init all previous/next positions for predictable output */
        /** @todo have NoInit / ValueInit overload of duplicate(), interleave()
            and interleavedLayout() instea, significantly faster than doing it
            manually on sparse views after */
        {
            constexpr const char Zero[sizeof(Float)*3]{};
            Containers::StridedArrayView2D<const char> zeros{Zero, positions.size(), {0, 1}};
            Utility::copy(zeros, previousPositions);
            Utility::copy(zeros, nextPositions);
        }

        /* Given AABBCCDDEEFF, we want to copy Position from AA__CC__EE__ to
           __BB__DD__FF's PreviousPosition, and Position from __BB__DD__FF to
           AA__CC__EE__'s NextPosition. Strip one group of 2 from either prefix
           or suffix, pick every 2nd in the first dimension, and copy. */
        Utility::copy(
            positions3.exceptSuffix(1).every(2),
            previousPositions3.exceptPrefix(1).every(2));
        Utility::copy(
            positions3.exceptPrefix(1).every(2),
            nextPositions3.exceptSuffix(1).every(2));

        /* Fill in previous/next neighbor positions if this is a line loop /
           line strip and there's more than one quad. Given AABBCCDDEEFF, want
           to copy Position from AA__CC______ to ____CC__EE__'s
           PreviousPosition, and Position from ______DD__FF to __BB__DD____'s
           NextPosition, and in case of loops also Position from ________EE__
           to AA__________'s PreviousPosition and Position from __BB________ to
           __________FF's NextPosition. */
        /** @todo put together with the annotations once it's generalized to
            the index buffer */
        if((lineMesh.primitive() == MeshPrimitive::LineStrip ||
            lineMesh.primitive() == MeshPrimitive::LineLoop) && quadCount > 1) {
            Utility::copy(
                positions3.exceptSuffix(2).every(2),
                previousPositions3.exceptPrefix(2).every(2));
            Utility::copy(
                positions3.exceptPrefix(3).every(2),
                nextPositions3.exceptPrefix(1).exceptSuffix(2).every(2));
        }
        if(lineMesh.primitive() == MeshPrimitive::LineLoop) {
            Utility::copy(positions3[positions3.size()[0] - 2], previousPositions3.front());
            Utility::copy(positions3[1], nextPositions3.back());
        }
    }

    /* Fill in point annotation */
    const Containers::StridedArrayView1D<Shaders::LineVertexAnnotations> annotations = Containers::arrayCast<Shaders::LineVertexAnnotations>(mesh.mutableAttribute<UnsignedInt>(Implementation::LineMeshAttributeAnnotation));
    for(UnsignedInt i = 0; i != quadCount; ++i) {
        annotations[i*4 + 0] = Shaders::LineVertexAnnotation::Up|Shaders::LineVertexAnnotation::Begin;
        annotations[i*4 + 1] = Shaders::LineVertexAnnotation::Begin;
        annotations[i*4 + 2] = Shaders::LineVertexAnnotation::Up;
        annotations[i*4 + 3] = {};
    }

    /* A line strip has joins everywhere except the first and last two
       vertices; line loop joins also the first and last two vertices if it's
       non-empty */
    /** @todo add a flag to use the original index buffer somehow to figure out
        abitrary joins and loops */
    if(lineMesh.primitive() == MeshPrimitive::LineStrip ||
       lineMesh.primitive() == MeshPrimitive::LineLoop) {
        for(UnsignedInt i = 0; i != quadCount; ++i) {
            annotations[i*4 + 0] |= Shaders::LineVertexAnnotation::Join;
            annotations[i*4 + 1] |= Shaders::LineVertexAnnotation::Join;
            annotations[i*4 + 2] |= Shaders::LineVertexAnnotation::Join;
            annotations[i*4 + 3] |= Shaders::LineVertexAnnotation::Join;
        }
    }
    if(quadCount && lineMesh.primitive() == MeshPrimitive::LineStrip) {
        annotations[0] &= ~Shaders::LineVertexAnnotation::Join;
        annotations[1] &= ~Shaders::LineVertexAnnotation::Join;
        annotations[quadCount*4 - 2] &= ~Shaders::LineVertexAnnotation::Join;
        annotations[quadCount*4 - 1] &= ~Shaders::LineVertexAnnotation::Join;
    }

    /* Create an index buffer */
    Containers::Array<UnsignedInt> indexData;
    arrayReserve(indexData, quadCount*6);
    for(UnsignedInt i = 0; i != quadCount; ++i) {
        /* The order is chosen in a way that makes it possible to interpret
           the 6 indices as 3 lines instead of 2 triangles, and additionally
           those forming only one line, with the other two degenerating to an
           invisible point to avoid overlaps that would break blending.

            0---2 2
            |  / /|       0---2
            | / / |
            |/ /  |      11   32
            1 1---3 */
        arrayAppend(indexData, {
            i*4 + 2,
            i*4 + 0,
            i*4 + 1,

            i*4 + 1,
            i*4 + 3,
            i*4 + 2
        });

        /* Add also indices for the bevel in both orientations (one will always
           degenerate). For the line fallback these will all degenerate.

            2 2   2---4 4   4--
             /|   |  / /|   |        23    44
            / |   | / / |   | /
              |   |/ /  |   |/          35
            --3   3 3---5   5 5 */
        if(i + 1 != quadCount && annotations[i*4 + 3] & Shaders::LineVertexAnnotation::Join) {
            arrayAppend(indexData, {
                i*4 + 2,
                i*4 + 3,
                i*4 + 4,

                i*4 + 4,
                i*4 + 3,
                i*4 + 5,
            });
        }
    }

    /* And finally also bevel indices between the last and first segment in
       case of loops, if the loop isn't empty

        -2  -2---0 0   0-
        /|   |  / /|   |
         |   | / / |   |
         |   |/ /  |   |/
        -1  -1 -1--1   1 */
    if(quadCount && annotations[0] & Shaders::LineVertexAnnotation::Join) {
        CORRADE_INTERNAL_ASSERT(annotations[quadCount*4 - 1] & Shaders::LineVertexAnnotation::Join);

        arrayAppend(indexData, {
            quadCount*4 - 2,
            quadCount*4 - 1,
            0u,

            0u,
            quadCount*4 - 1,
            1u
        });
    }

    Trade::MeshIndexData indices{indexData};
    return Trade::MeshData{mesh.primitive(),
        indexData ? Containers::arrayAllocatorCast<char>(Utility::move(indexData)) : Containers::Array<char>{}, indices,
        mesh.releaseVertexData(), mesh.releaseAttributeData()};
}

}}
