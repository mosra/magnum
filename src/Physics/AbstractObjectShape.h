#ifndef Magnum_Physics_AbstractObjectShape_h
#define Magnum_Physics_AbstractObjectShape_h
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
 * @brief Class Magnum::Physics::AbstractObjectShape, typedef Magnum::Physics::AbstractObjectShape2D, Magnum::Physics::AbstractObjectShape3D
 */

#include "Magnum.h"
#include "DimensionTraits.h"
#include "Physics/magnumPhysicsVisibility.h"
#include "Physics/shapeImplementation.h"
#include "SceneGraph/AbstractGroupedFeature.h"

namespace Magnum { namespace Physics {

namespace Implementation {
    template<UnsignedInt dimensions> inline const AbstractShape<dimensions>* getAbstractShape(const AbstractObjectShape<dimensions>* objectShape) {
        return objectShape->abstractTransformedShape();
    }
}

/**
@brief Base class for object shapes

This class is not directly instantiable, see ObjectShape instead.
@see AbstractObjectShape2D, AbstractObjectShape3D
*/
template<UnsignedInt dimensions> class MAGNUM_PHYSICS_EXPORT AbstractObjectShape: public SceneGraph::AbstractGroupedFeature<dimensions, AbstractObjectShape<dimensions>> {
    friend const Implementation::AbstractShape<dimensions>* Implementation::getAbstractShape<>(const AbstractObjectShape<dimensions>* objectShape);

    public:
        enum: UnsignedInt {
            Dimensions = dimensions /**< Dimension count */
        };

        /** @brief Shape type */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        enum class Type {
            Point,          /**< Point */
            Line,           /**< Line */
            LineSegment,    /**< @ref LineSegment "Line segment" */
            Sphere,         /**< Sphere */
            Capsule,        /**< Capsule */
            AxisAlignedBox, /**< @ref AxisAlignedBox "Axis aligned box" */
            Box,            /**< Box */
            ShapeGroup,     /**< @ref ShapeGroup "Shape group" */
            Plane           /**< Plane (3D only) */
        };
        #else
        typedef typename Implementation::ShapeDimensionTraits<dimensions>::Type Type;
        #endif

        /**
         * @brief Constructor
         * @param object    Object holding this feature
         * @param group     Group this shape belongs to
         */
        explicit AbstractObjectShape(SceneGraph::AbstractObject<dimensions>* object, ObjectShapeGroup<dimensions>* group = nullptr);

        /**
         * @brief Object shape group containing this shape
         *
         * If the shape doesn't belong to any group, returns `nullptr`.
         */
        ObjectShapeGroup<dimensions>* group();
        const ObjectShapeGroup<dimensions>* group() const; /**< @overload */

        /**
         * @brief Shape type
         */
        Type type() const;

        /**
         * @brief Detect collision with other shape
         *
         * Default implementation returns false.
         */
        bool collides(const AbstractObjectShape<dimensions>* other) const;

    protected:
        /** Marks also the group as dirty */
        void markDirty() override;

    private:
        virtual const Implementation::AbstractShape<dimensions> MAGNUM_PHYSICS_LOCAL * abstractTransformedShape() const = 0;
};

/** @brief Base class for two-dimensional object shapes */
typedef AbstractObjectShape<2> AbstractObjectShape2D;

/** @brief Base class for three-dimensional object shapes */
typedef AbstractObjectShape<3> AbstractObjectShape3D;

}}

#endif
