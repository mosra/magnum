#ifndef Magnum_Physics_ShapedObject_h
#define Magnum_Physics_ShapedObject_h
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
 * @brief Class Magnum::Physics::ShapedObject
 */

#include "SceneGraph/Object.h"

#include "magnumPhysicsVisibility.h"

namespace Magnum { namespace Physics {

template<size_t> class ShapedObjectGroup;
template<size_t> class AbstractShape;

/**
@brief Object with assigned shape

@see ShapedObject2D, ShapedObject3D
*/
template<size_t dimensions> class PHYSICS_EXPORT ShapedObject: public SceneGraph::AbstractObject<dimensions>::ObjectType {
    public:
        /**
         * @brief Constructor
         * @param group     Group this shaped object belongs to
         * @param parent    Parent object
         *
         * Creates object with no shape.
         * @see setShape()
         */
        ShapedObject(ShapedObjectGroup<dimensions>* group, typename SceneGraph::AbstractObject<dimensions>::ObjectType* parent = nullptr);

        /**
         * @brief Destructor
         *
         * Deletes associated shape.
         */
        ~ShapedObject();

        /** @brief Object shape */
        inline AbstractShape<dimensions>* shape() { return _shape; }
        inline const AbstractShape<dimensions>* shape() const { return _shape; } /**< @overload */

        /** @brief Set object shape */
        void setShape(AbstractShape<dimensions>* shape) { _shape = shape; }

        /**
         * @copybrief SceneGraph::AbstractObject::setDirty()
         *
         * Marks shaped object group as dirty.
         */
        void setDirty();

    protected:
        /**
         * @copybrief SceneGraph::AbstractObject::clean()
         *
         * Applies transformation to associated shape.
         */
        void clean(const typename SceneGraph::AbstractObject<dimensions>::MatrixType& absoluteTransformation);

    private:
        ShapedObjectGroup<dimensions>* group;
        AbstractShape<dimensions>* _shape;
};

#ifndef DOXYGEN_GENERATING_OUTPUT
extern template class PHYSICS_EXPORT ShapedObject<2>;
extern template class PHYSICS_EXPORT ShapedObject<3>;
#endif

/** @brief Two-dimensional shaped object */
typedef ShapedObject<2> ShapedObject2D;

/** @brief Three-dimensional shaped object */
typedef ShapedObject<3> ShapedObject3D;

}}

#endif
