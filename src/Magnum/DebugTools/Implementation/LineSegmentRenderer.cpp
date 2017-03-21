/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include "LineSegmentRenderer.h"

#include "Magnum/Mesh.h"
#include "Magnum/DebugTools/ShapeRenderer.h"
#include "Magnum/Shapes/LineSegment.h"
#include "Magnum/Primitives/Line.h"
#include "Magnum/Shaders/Flat.h"
#include "Magnum/Trade/MeshData2D.h"
#include "Magnum/Trade/MeshData3D.h"

#include "LineSegmentRendererTransformation.h"

namespace Magnum { namespace DebugTools { namespace Implementation {

namespace {
    template<UnsignedInt dimensions> ResourceKey meshKey();
    template<> inline ResourceKey meshKey<2>() { return ResourceKey("line2d"); }
    template<> inline ResourceKey meshKey<3>() { return ResourceKey("line3d"); }

    template<UnsignedInt dimensions> ResourceKey vertexBufferKey();
    template<> inline ResourceKey vertexBufferKey<2>() { return ResourceKey("line2d-vertices"); }
    template<> inline ResourceKey vertexBufferKey<3>() { return ResourceKey("line3d-vertices"); }

    template<UnsignedInt dimensions> typename MeshData<dimensions>::Type meshData();
    template<> inline Trade::MeshData2D meshData<2>() { return Primitives::Line2D::wireframe(); }
    template<> inline Trade::MeshData3D meshData<3>() { return Primitives::Line3D::wireframe(); }
}

template<UnsignedInt dimensions> LineSegmentRenderer<dimensions>::LineSegmentRenderer(const Shapes::Implementation::AbstractShape<dimensions>& line): AbstractShapeRenderer<dimensions>(meshKey<dimensions>(), vertexBufferKey<dimensions>(), {}), line(static_cast<const Shapes::Implementation::Shape<Shapes::LineSegment<dimensions>>&>(line).shape) {
    if(!AbstractShapeRenderer<dimensions>::wireframeMesh) AbstractShapeRenderer<dimensions>::createResources(meshData<dimensions>());
}

template<UnsignedInt dimensions> void LineSegmentRenderer<dimensions>::draw(Resource<ShapeRendererOptions>& options, const MatrixTypeFor<dimensions, Float>& projectionMatrix) {
    AbstractShapeRenderer<dimensions>::wireframeShader->setTransformationProjectionMatrix(projectionMatrix*
        Implementation::lineSegmentRendererTransformation<dimensions>(line.a(), line.b()))
        .setColor(options->color());
    AbstractShapeRenderer<dimensions>::wireframeMesh->draw(*AbstractShapeRenderer<dimensions>::wireframeShader);
}

template class LineSegmentRenderer<2>;
template class LineSegmentRenderer<3>;

}}}
