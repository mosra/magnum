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

#include "ShapeRenderer.h"

#include "Magnum/DebugTools/ResourceManager.h"
#include "Magnum/Shapes/Composition.h"
#include "Magnum/Shapes/Shape.h"
#include "Magnum/SceneGraph/Camera.h"

#include "Implementation/AxisAlignedBoxRenderer.h"
#include "Implementation/BoxRenderer.h"
#include "Implementation/CapsuleRenderer.h"
#include "Implementation/CylinderRenderer.h"
#include "Implementation/LineSegmentRenderer.h"
#include "Implementation/PointRenderer.h"
#include "Implementation/SphereRenderer.h"

namespace Magnum { namespace DebugTools {

namespace Implementation {

template<> void createDebugMesh(ShapeRenderer<2>& renderer, const Shapes::Implementation::AbstractShape<2>& shape) {
    switch(shape.type()) {
        case Shapes::AbstractShape2D::Type::AxisAlignedBox:
            renderer._renderers.push_back(new Implementation::AxisAlignedBoxRenderer<2>(shape));
            break;
        case Shapes::AbstractShape2D::Type::Box:
            renderer._renderers.push_back(new Implementation::BoxRenderer<2>(shape));
            break;
        case Shapes::AbstractShape2D::Type::LineSegment:
            renderer._renderers.push_back(new Implementation::LineSegmentRenderer<2>(shape));
            break;
        case Shapes::AbstractShape2D::Type::Point:
            renderer._renderers.push_back(new Implementation::PointRenderer<2>(shape));
            break;
        case Shapes::AbstractShape2D::Type::Sphere:
        case Shapes::AbstractShape2D::Type::InvertedSphere: /* Isn't publicly subclassed, but shouldn't matter */
            renderer._renderers.push_back(new Implementation::SphereRenderer<2>(shape));
            break;
        case Shapes::AbstractShape2D::Type::Capsule:
            renderer._renderers.push_back(new Implementation::CapsuleRenderer<2>(shape));
            break;
        case Shapes::AbstractShape2D::Type::Cylinder:
            renderer._renderers.push_back(new Implementation::CylinderRenderer<2>(shape));
            break;
        case Shapes::AbstractShape2D::Type::Composition: {
            const Shapes::Composition2D& composition =
                static_cast<const Shapes::Implementation::Shape<Shapes::Composition2D>&>(shape).shape;
            for(std::size_t i = 0; i != composition.size(); ++i)
                createDebugMesh(renderer, Shapes::Implementation::getAbstractShape(composition, i));
        } break;
        default:
            Warning() << "DebugTools::ShapeRenderer2D::createShapeRenderer(): type" << shape.type() << "not implemented";
    }
}

template<> void createDebugMesh(ShapeRenderer<3>& renderer, const Shapes::Implementation::AbstractShape<3>& shape) {
    switch(shape.type()) {
        case Shapes::AbstractShape3D::Type::AxisAlignedBox:
            renderer._renderers.push_back(new Implementation::AxisAlignedBoxRenderer<3>(shape));
            break;
        case Shapes::AbstractShape3D::Type::Box:
            renderer._renderers.push_back(new Implementation::BoxRenderer<3>(shape));
            break;
        case Shapes::AbstractShape3D::Type::LineSegment:
            renderer._renderers.push_back(new Implementation::LineSegmentRenderer<3>(shape));
            break;
        case Shapes::AbstractShape3D::Type::Point:
            renderer._renderers.push_back(new Implementation::PointRenderer<3>(shape));
            break;
        case Shapes::AbstractShape3D::Type::Sphere:
        case Shapes::AbstractShape3D::Type::InvertedSphere: /* Isn't publicly subclassed, but shouldn't matter */
            renderer._renderers.push_back(new Implementation::SphereRenderer<3>(shape));
            break;
        case Shapes::AbstractShape3D::Type::Capsule:
            renderer._renderers.push_back(new Implementation::CapsuleRenderer<3>(shape));
            break;
        case Shapes::AbstractShape3D::Type::Cylinder:
            renderer._renderers.push_back(new Implementation::CylinderRenderer<3>(shape));
            break;
        case Shapes::AbstractShape3D::Type::Composition: {
            const Shapes::Composition3D& composition =
                static_cast<const Shapes::Implementation::Shape<Shapes::Composition3D>&>(shape).shape;
            for(std::size_t i = 0; i != composition.size(); ++i)
                createDebugMesh(renderer, Shapes::Implementation::getAbstractShape(composition, i));
        } break;
        default:
            Warning() << "DebugTools::ShapeRenderer3D::createShapeRenderer(): type" << shape.type() << "not implemented";
    }
}

}

template<UnsignedInt dimensions> ShapeRenderer<dimensions>::ShapeRenderer(Shapes::AbstractShape<dimensions>& shape, ResourceKey options, SceneGraph::DrawableGroup<dimensions, Float>* drawables): SceneGraph::Drawable<dimensions, Float>(shape.object(), drawables), _options(ResourceManager::instance().get<ShapeRendererOptions>(options)) {
    Implementation::createDebugMesh(*this, Shapes::Implementation::getAbstractShape(shape));
}

template<UnsignedInt dimensions> ShapeRenderer<dimensions>::~ShapeRenderer() {
    for(auto i: _renderers) delete i;
}

template<UnsignedInt dimensions> void ShapeRenderer<dimensions>::draw(const MatrixTypeFor<dimensions, Float>&, SceneGraph::Camera<dimensions, Float>& camera) {
    const MatrixTypeFor<dimensions, Float> projectionMatrix = camera.projectionMatrix()*camera.cameraMatrix();
    for(auto i: _renderers) i->draw(_options, projectionMatrix);
}

template class ShapeRenderer<2>;
template class ShapeRenderer<3>;

}}
