#ifndef Magnum_Shapes_Shape_h
#define Magnum_Shapes_Shape_h
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

/** @file
 * @brief Class @ref Magnum::Shapes::Shape
 */

#include "Magnum/Shapes/AbstractShape.h"
#include "Magnum/Shapes/Shapes.h"
#include "Magnum/Shapes/visibility.h"

namespace Magnum { namespace Shapes {

namespace Implementation {
    template<class> struct ShapeHelper;
}

/**
@brief Object shape

Adds shape for collision detection to object. Each Shape is part of
some @ref ShapeGroup, which essentially maintains a set of objects which can
collide with each other. See @ref shapes for brief introduction.

The shape contains original shape with relative transformation under
@ref shape() and also caches a shape with absolute transformation under
@ref transformedShape(), which can be used for collision detection. To
conveniently use collision detection among many objects, you need to add the
shape to @ref ShapeGroup, which then provides collision detection for given
group of shapes using either @ref collides(), @ref collision() or
@ref ShapeGroup::firstCollision(). You can also use @ref ShapeGroup::add() and
@ref ShapeGroup::remove() later to manage e.g. collision islands.
@code
Shapes::ShapeGroup3D shapes;

Object3D object;
auto shape = new Shapes::Shape<Shapes::Sphere3D>(object, {{}, 0.75f}, &shapes);

Shapes::AbstractShape3D* firstCollision = shapes.firstCollision(shape);
@endcode

@see @ref scenegraph, @ref ShapeGroup2D, @ref ShapeGroup3D,
    @ref DebugTools::ShapeRenderer
*/
template<class T> class Shape: public AbstractShape<T::Dimensions> {
    friend Implementation::ShapeHelper<T>;

    public:
        /**
         * @brief Constructor
         * @param object    Object holding this feature
         * @param shape     Shape
         * @param group     Group this shape belongs to
         */
        explicit Shape(SceneGraph::AbstractObject<T::Dimensions, Float>& object, const T& shape, ShapeGroup<T::Dimensions>* group = nullptr): AbstractShape<T::Dimensions>(object, group) {
            Implementation::ShapeHelper<T>::set(*this, shape);
        }

        /** @overload */
        explicit Shape(SceneGraph::AbstractObject<T::Dimensions, Float>& object, T&& shape, ShapeGroup<T::Dimensions>* group = nullptr): AbstractShape<T::Dimensions>(object, group) {
            Implementation::ShapeHelper<T>::set(*this, std::move(shape));
        }

        /** @overload */
        explicit Shape(SceneGraph::AbstractObject<T::Dimensions, Float>& object, ShapeGroup<T::Dimensions>* group = nullptr): AbstractShape<T::Dimensions>(object, group) {}

        /** @brief Shape */
        const T& shape() const { return _shape.shape; }

        /**
         * @brief Set shape
         * @return Reference to self (for method chaining)
         *
         * Marks the feature as dirty.
         */
        Shape<T>& setShape(const T& shape);

        /**
         * @brief Transformed shape
         *
         * Cleans the feature before returning the shape.
         */
        const T& transformedShape();

    protected:
        /** Applies transformation to associated shape. */
        void clean(const MatrixTypeFor<T::Dimensions, Float>& absoluteTransformationMatrix) override;

    private:
        const Implementation::AbstractShape<T::Dimensions>& abstractTransformedShape() const override {
            return _transformedShape;
        }

        Implementation::Shape<T> _shape, _transformedShape;
};

template<class T> inline Shape<T>& Shape<T>::setShape(const T& shape) {
    Implementation::ShapeHelper<T>::set(*this, shape);
    this->object().setDirty();
    return *this;
}

template<class T> inline const T& Shape<T>::transformedShape() {
    this->object().setClean();
    return _transformedShape.shape;
}

template<class T> void Shape<T>::clean(const MatrixTypeFor<T::Dimensions, Float>& absoluteTransformationMatrix) {
    Implementation::ShapeHelper<T>::transform(*this, absoluteTransformationMatrix);
}

namespace Implementation {
    template<class T> struct ShapeHelper {
        static void set(Shapes::Shape<T>& shape, const T& s) {
            shape._shape.shape = s;
        }

        static void transform(Shapes::Shape<T>& shape, const MatrixTypeFor<T::Dimensions, Float>& absoluteTransformationMatrix) {
            shape._transformedShape.shape = shape._shape.shape.transformed(absoluteTransformationMatrix);
        }
    };

    template<UnsignedInt dimensions> struct MAGNUM_SHAPES_EXPORT ShapeHelper<Composition<dimensions>> {
        static void set(Shapes::Shape<Composition<dimensions>>& shape, const Composition<dimensions>& composition);
        static void set(Shapes::Shape<Composition<dimensions>>& shape, Composition<dimensions>&& composition);

        static void transform(Shapes::Shape<Composition<dimensions>>& shape, const MatrixTypeFor<dimensions, Float>& absoluteTransformationMatrix);
    };
}

}}

#endif
