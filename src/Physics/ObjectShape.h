#ifndef Magnum_Physics_ObjectShape_h
#define Magnum_Physics_ObjectShape_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Class Magnum::Physics::ObjectShape
 */

#include "SceneGraph/AbstractGroupedFeature.h"
#include "ObjectShapeGroup.h"

#include "magnumPhysicsVisibility.h"

namespace Magnum { namespace Physics {

template<std::uint8_t> class ObjectShapeGroup;
template<std::uint8_t> class AbstractShape;

/**
@brief Object shape

Adds shape for collision detection to object.
@see ObjectShape2D, ObjectShape3D
*/
template<std::uint8_t dimensions> class PHYSICS_EXPORT ObjectShape: public SceneGraph::AbstractGroupedFeature<dimensions, ObjectShape<dimensions>> {
    public:
        /**
         * @brief Constructor
         * @param object    Object holding this feature
         * @param group     Group this shape belongs to
         *
         * Creates empty object shape.
         * @see setShape()
         */
        ObjectShape(SceneGraph::AbstractObject<dimensions>* object, ObjectShapeGroup<dimensions>* group = nullptr);

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

        inline ObjectShapeGroup<dimensions>* group() {
            return static_cast<ObjectShapeGroup<dimensions>*>(SceneGraph::AbstractGroupedFeature<dimensions, ObjectShape<dimensions>>::group());
        }

        inline const ObjectShapeGroup<dimensions>* group() const {
            return static_cast<const ObjectShapeGroup<dimensions>*>(SceneGraph::AbstractGroupedFeature<dimensions, ObjectShape<dimensions>>::group());
        }

    protected:
        /** Marks also the group as dirty */
        void markDirty() override;

        /** Applies transformation to associated shape. */
        void clean(const typename DimensionTraits<dimensions>::MatrixType& absoluteTransformation) override;

    private:
        AbstractShape<dimensions>* _shape;
};

/** @brief Two-dimensional object shape */
typedef ObjectShape<2> ObjectShape2D;

/** @brief Three-dimensional object shape */
typedef ObjectShape<3> ObjectShape3D;

}}

#endif
