/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include "ShapeRenderer.h"

#include "ResourceManager.h"
#include "Shapes/Composition.h"
#include "Shapes/Shape.h"
#include "SceneGraph/AbstractCamera.h"

#include "Implementation/AxisAlignedBoxRenderer.h"
#include "Implementation/BoxRenderer.h"
#include "Implementation/LineSegmentRenderer.h"
#include "Implementation/PointRenderer.h"
#include "Implementation/SphereRenderer.h"

namespace Magnum { namespace DebugTools {

namespace Implementation {

template<> void createDebugMesh(ShapeRenderer<2>* renderer, const Shapes::Implementation::AbstractShape<2>* shape) {
    switch(shape->type()) {
        case Shapes::AbstractShape2D::Type::AxisAlignedBox:
            renderer->renderers.push_back(new Implementation::AxisAlignedBoxRenderer<2>(shape));
            break;
        case Shapes::AbstractShape2D::Type::Box:
            renderer->renderers.push_back(new Implementation::BoxRenderer<2>(shape));
            break;
        case Shapes::AbstractShape2D::Type::LineSegment:
            renderer->renderers.push_back(new Implementation::LineSegmentRenderer<2>(shape));
            break;
        case Shapes::AbstractShape2D::Type::Point:
            renderer->renderers.push_back(new Implementation::PointRenderer<2>(shape));
            break;
        case Shapes::AbstractShape2D::Type::Composition: {
            const Shapes::Composition2D& composition =
                static_cast<const Shapes::Implementation::Shape<Shapes::Composition2D>*>(shape)->shape;
            for(std::size_t i = 0; i != composition.size(); ++i)
                createDebugMesh(renderer, Shapes::Implementation::getAbstractShape(composition, i));
        } break;
        case Shapes::AbstractShape2D::Type::Sphere:
            renderer->renderers.push_back(new Implementation::SphereRenderer<2>(shape));
            break;
        default:
            Warning() << "DebugTools::ShapeRenderer2D::createShapeRenderer(): type" << shape->type() << "not implemented";
    }
}

template<> void createDebugMesh(ShapeRenderer<3>* renderer, const Shapes::Implementation::AbstractShape<3>* shape) {
    switch(shape->type()) {
        case Shapes::AbstractShape3D::Type::AxisAlignedBox:
            renderer->renderers.push_back(new Implementation::AxisAlignedBoxRenderer<3>(shape));
            break;
        case Shapes::AbstractShape3D::Type::Box:
            renderer->renderers.push_back(new Implementation::BoxRenderer<3>(shape));
            break;
        case Shapes::AbstractShape3D::Type::LineSegment:
            renderer->renderers.push_back(new Implementation::LineSegmentRenderer<3>(shape));
            break;
        case Shapes::AbstractShape3D::Type::Point:
            renderer->renderers.push_back(new Implementation::PointRenderer<3>(shape));
            break;
        case Shapes::AbstractShape3D::Type::Composition: {
            const Shapes::Composition3D& composition =
                static_cast<const Shapes::Implementation::Shape<Shapes::Composition3D>*>(shape)->shape;
            for(std::size_t i = 0; i != composition.size(); ++i)
                createDebugMesh(renderer, Shapes::Implementation::getAbstractShape(composition, i));
        } break;
        default:
            Warning() << "DebugTools::ShapeRenderer3D::createShapeRenderer(): type" << shape->type() << "not implemented";
    }
}

}

template<UnsignedInt dimensions> ShapeRenderer<dimensions>::ShapeRenderer(Shapes::AbstractShape<dimensions>* shape, ResourceKey options, SceneGraph::BasicDrawableGroup<dimensions, Float>* drawables): SceneGraph::BasicDrawable<dimensions, Float>(shape->object(), drawables), options(ResourceManager::instance()->get<ShapeRendererOptions>(options)) {
    Implementation::createDebugMesh(this, Shapes::Implementation::getAbstractShape(shape));
}

template<UnsignedInt dimensions> ShapeRenderer<dimensions>::~ShapeRenderer() {
    for(auto it = renderers.begin(); it != renderers.end(); ++it)
        delete *it;
}

template<UnsignedInt dimensions> void ShapeRenderer<dimensions>::draw(const typename DimensionTraits<dimensions, Float>::MatrixType&, SceneGraph::AbstractBasicCamera<dimensions, Float>* camera) {
    typename DimensionTraits<dimensions, Float>::MatrixType projectionMatrix = camera->projectionMatrix()*camera->cameraMatrix();
    for(auto it = renderers.begin(); it != renderers.end(); ++it)
        (*it)->draw(options, projectionMatrix);
}

template class ShapeRenderer<2>;
template class ShapeRenderer<3>;

}}
