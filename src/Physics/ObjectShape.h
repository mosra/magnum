#ifndef Magnum_Physics_ObjectShape_h
#define Magnum_Physics_ObjectShape_h
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

/** @file
 * @brief Class Magnum::Physics::ObjectShape
 */

#include "Physics/AbstractObjectShape.h"
#include "Physics/Physics.h"

#include "magnumPhysicsVisibility.h"

namespace Magnum { namespace Physics {

namespace Implementation {
    template<class> struct ObjectShapeHelper;
}

/**
@brief Object shape

Adds shape for collision detection to object. Each %ObjectShape is part of
some ObjectShapeGroup, which essentially maintains a set of objects which can
collide with each other.

@section ObjectShape-usage Usage

Add the feature to the object and some shape group (you can also use
ObjectShapeGroup::add() and ObjectShapeGroup::remove() later) and configure the
shape.
@code
Physics::ObjectShapeGroup3D shapes;

Object3D* object;
auto shape = new Physics::ObjectShape<Physics::Sphere3D>(object, {{}, 0.75f}, &shapes);
@endcode

@see @ref scenegraph, ObjectShapeGroup2D, ObjectShapeGroup3D,
    DebugTools::ShapeRenderer
*/
template<class T> class MAGNUM_PHYSICS_EXPORT ObjectShape: public AbstractObjectShape<T::Dimensions> {
    friend struct Implementation::ObjectShapeHelper<T>;

    public:
        /**
         * @brief Constructor
         * @param object    Object holding this feature
         * @param shape     Shape
         * @param group     Group this shape belongs to
         */
        template<class ...U> explicit ObjectShape(SceneGraph::AbstractObject<T::Dimensions>* object, const T& shape, ObjectShapeGroup<T::Dimensions>* group = nullptr): AbstractObjectShape<T::Dimensions>(object, group) {
            Implementation::ObjectShapeHelper<T>::set(*this, shape);
        }

        /** @overload */
        template<class ...U> explicit ObjectShape(SceneGraph::AbstractObject<T::Dimensions>* object, T&& shape, ObjectShapeGroup<T::Dimensions>* group = nullptr): AbstractObjectShape<T::Dimensions>(object, group) {
            Implementation::ObjectShapeHelper<T>::set(*this, std::move(shape));
        }

        /** @overload */
        template<class ...U> explicit ObjectShape(SceneGraph::AbstractObject<T::Dimensions>* object, ObjectShapeGroup<T::Dimensions>* group = nullptr): AbstractObjectShape<T::Dimensions>(object, group) {}

        /** @brief Shape */
        inline const T& shape() const { return _shape.shape; }

        /**
         * @brief Set shape
         * @return Pointer to self (for method chaining)
         *
         * Marks the feature as dirty.
         */
        ObjectShape<T>* setShape(const T& shape);

        /**
         * @brief Transformed shape
         *
         * Cleans the feature before returning the shape.
         */
        const T& transformedShape();

    protected:
        /** Marks also the group as dirty */
        void markDirty() override;

        /** Applies transformation to associated shape. */
        void clean(const typename DimensionTraits<T::Dimensions>::MatrixType& absoluteTransformationMatrix) override;

    private:
        const Implementation::AbstractShape<T::Dimensions>* abstractTransformedShape() const override {
            return &_transformedShape;
        }

        Implementation::Shape<T> _shape, _transformedShape;
};

template<class T> inline ObjectShape<T>* ObjectShape<T>::setShape(const T& shape) {
    Implementation::ObjectShapeHelper<T>::set(*this, shape);
    this->object()->setDirty();
    return this;
}

template<class T> inline const T& ObjectShape<T>::transformedShape() {
    this->object()->setClean();
    return _transformedShape.shape;
}

template<class T> void ObjectShape<T>::markDirty() {
    if(this->group()) this->group()->setDirty();
}

template<class T> void ObjectShape<T>::clean(const typename DimensionTraits<T::Dimensions>::MatrixType& absoluteTransformationMatrix) {
    Implementation::ObjectShapeHelper<T>::transform(*this, absoluteTransformationMatrix);
}

namespace Implementation {
    template<class T> struct ObjectShapeHelper {
        inline static void set(ObjectShape<T>& objectShape, const T& shape) {
            objectShape._shape.shape = shape;
        }

        inline static void transform(ObjectShape<T>& objectShape, const typename DimensionTraits<T::Dimensions>::MatrixType& absoluteTransformationMatrix) {
            objectShape._transformedShape.shape = objectShape._shape.shape.transformed(absoluteTransformationMatrix);
        }
    };

    template<UnsignedInt dimensions> struct MAGNUM_PHYSICS_EXPORT ObjectShapeHelper<ShapeGroup<dimensions>> {
        static void set(ObjectShape<ShapeGroup<dimensions>>& objectShape, const ShapeGroup<dimensions>& shape);
        static void set(ObjectShape<ShapeGroup<dimensions>>& objectShape, ShapeGroup<dimensions>&& shape);

        static void transform(ObjectShape<ShapeGroup<dimensions>>& objectShape, const typename DimensionTraits<dimensions>::MatrixType& absoluteTransformationMatrix);
    };
}

}}

#endif
