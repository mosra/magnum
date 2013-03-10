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
 * @brief Class Magnum::Physics::ObjectShape, typedef Magnum::Physics::ObjectShape2D, Magnum::Physics::ObjectShape3D
 */

#include "SceneGraph/AbstractGroupedFeature.h"
#include "Physics/Physics.h"

#include "magnumPhysicsVisibility.h"

namespace Magnum { namespace Physics {

/**
@brief Object shape

Adds shape for collision detection to object. Each %ObjectShape is part of
some ObjectShapeGroup, which essentially maintains a set of objects which can
collide with each other.

@section ObjectShape-usage Usage

Add the feature to the object and some shape group (you can also use
ObjectShapeGroup::add() and ObjectShapeGroup::remove() later) and then set
desired object shape using setShape().
@code
Physics::ObjectShapeGroup3D shapes;

Object3D* object;
auto shape = new Physics::ObjectShape3D(object, &shapes);
shape->setShape(Physics::Sphere3D({}, 0.75f) || Physics::AxisAlignedBox3D({}, {3.0f, 1.5f, 2.0f}));
@endcode

@see @ref scenegraph, ObjectShape2D, ObjectShape3D, ObjectShapeGroup2D,
    ObjectShapeGroup3D, DebugTools::ShapeRenderer
*/
template<UnsignedInt dimensions> class MAGNUM_PHYSICS_EXPORT ObjectShape: public SceneGraph::AbstractGroupedFeature<dimensions, ObjectShape<dimensions>> {
    public:
        /**
         * @brief Constructor
         * @param object    Object holding this feature
         * @param group     Group this shape belongs to
         *
         * Creates empty object shape.
         * @see setShape()
         */
        explicit ObjectShape(SceneGraph::AbstractObject<dimensions>* object, ObjectShapeGroup<dimensions>* group = nullptr);

        /**
         * @brief Destructor
         *
         * Deletes associated shape.
         */
        ~ObjectShape();

        /** @brief Shape */
        inline AbstractShape<dimensions>* shape() { return _shape; }
        inline const AbstractShape<dimensions>* shape() const { return _shape; } /**< @overload */

        /**
         * @brief Set shape
         * @return Pointer to self (for method chaining)
         */
        inline ObjectShape<dimensions>* setShape(AbstractShape<dimensions>* shape) {
            _shape = shape;
            this->object()->setDirty();
            return this;
        }

        /**
         * @brief Set shape
         * @return Pointer to self (for method chaining)
         *
         * Convenience overload for setShape(AbstractShape*), allowing you to
         * use e.g. ShapeGroup operators:
         * @code
         * Physics::ObjectShape3D* shape;
         * shape->setShape(Physics::Sphere3D({}, 0.75f) || Physics::AxisAlignedBox3D({}, {3.0f, 1.5f, 2.0f}));
         * @endcode
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        template<class T> inline ObjectShape<dimensions>* setShape(T&& shape) {
        #else
        template<class T> inline typename std::enable_if<std::is_base_of<Physics::AbstractShape<dimensions>, T>::value, ObjectShape<dimensions>*>::type setShape(T&& shape) {
        #endif
            return setShape(new T(std::move(shape)));
        }

        /**
         * @brief Object shape group containing this shape
         *
         * If the shape doesn't belong to any group, returns `nullptr`.
         */
        ObjectShapeGroup<dimensions>* group();
        const ObjectShapeGroup<dimensions>* group() const; /**< @overload */

    protected:
        /** Marks also the group as dirty */
        void markDirty() override;

        /** Applies transformation to associated shape. */
        void clean(const typename DimensionTraits<dimensions>::MatrixType& absoluteTransformationMatrix) override;

    private:
        AbstractShape<dimensions>* _shape;
};

/** @brief Two-dimensional object shape */
typedef ObjectShape<2> ObjectShape2D;

/** @brief Three-dimensional object shape */
typedef ObjectShape<3> ObjectShape3D;

}}

#endif
