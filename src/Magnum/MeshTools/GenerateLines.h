#ifndef Magnum_MeshTools_GenerateLines_h
#define Magnum_MeshTools_GenerateLines_h
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

/** @file
 * @brief Function @ref Magnum::MeshTools::generateLines()
 * @m_since_latest
 */

#include "Magnum/MeshTools/visibility.h"
#include "Magnum/Trade/MeshData.h" /* needs meshAttributeCustom() for now */

namespace Magnum { namespace MeshTools {

namespace Implementation {

/** @todo make these builtin, and once they are, deprecate compileLines() in
    favor of compile() handling these extra attributes as well */
constexpr Trade::MeshAttribute LineMeshAttributePreviousPosition = Trade::meshAttributeCustom(32765);
constexpr Trade::MeshAttribute LineMeshAttributeNextPosition = Trade::meshAttributeCustom(32766);
constexpr Trade::MeshAttribute LineMeshAttributeAnnotation = Trade::meshAttributeCustom(32767);

}

/**
@brief Generate a line mesh for use with @ref Shaders::LineGL
@m_since_latest

Creates a @ref MeshPrimitive::Triangles mesh with
@ref MeshIndexType::UnsignedInt indices, all input attributes preserved in
their original format, and additionally with custom attributes corresponding to
@ref Shaders::LineGL::PreviousPosition and @ref Shaders::LineGL::NextPosition
added in the same format as the input @ref Trade::MeshAttribute::Position, and
a custom attribute corresponding to the @ref Shaders::LineGL::Annotation
attribute as @ref VertexFormat::UnsignedInt. See
@ref Shaders-LineGL-mesh-representation "documentation of the shader" for
details about the internal representation.

Each line segment in the input vertices is converted to a quad, with first two
vertices inheriting vertex data from the first point of the segment and second
two vertices inheriting data from the second point of the segment. If the input
mesh is indexed, it's deindexed first. Neighbor information from a
@ref MeshPrimitive::LineStrip or @ref MeshPrimitive::LineLoop mesh is used to
form a single contiguous strip or a loop, @ref MeshPrimitive::Lines is treated
as loose segments.

For compatibility with shaders other than @ref Shaders::LineGL, the output mesh
can be also interpreted as indexed @ref MeshPrimitive::Lines --- out of every
six indices forming a quad, two will form a line segment between the two
original points, and the remaining four collapse into two degenerate line
segments.

Expects that the mesh contains at least a @ref Trade::MeshAttribute::Position
and is a line @relativeref{Magnum,MeshPrimitive}.

The returned @ref Trade::MeshData instance is meant to be passed to
@ref compileLines() for use with the shader. It can however be also processed
with other @ref MeshTools first, such as @ref compressIndices(const Trade::MeshData&, MeshIndexType) or @ref concatenate().

@experimental
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData generateLines(const Trade::MeshData& lineMesh);

}}

#endif
