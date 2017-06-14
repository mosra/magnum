#ifndef Magnum_Shapes_AbstractShape_h
#define Magnum_Shapes_AbstractShape_h
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
 * @brief Class @ref Magnum::Shapes::AbstractShape, typedef @ref Magnum::Shapes::AbstractShape2D, @ref Magnum::Shapes::AbstractShape3D
 */

#include "Magnum/Magnum.h"
#include "Magnum/DimensionTraits.h"
#include "Magnum/SceneGraph/AbstractGroupedFeature.h"
#include "Magnum/Shapes/shapeImplementation.h"
#include "Magnum/Shapes/visibility.h"

namespace Magnum { namespace Shapes {

namespace Implementation {
    template<UnsignedInt dimensions> inline const AbstractShape<dimensions>& getAbstractShape(const Shapes::AbstractShape<dimensions>& shape) {
        return shape.abstractTransformedShape();
    }
}

/**
@brief Base class for object shapes

This class is not directly instantiable, use @ref Shape instead. See
@ref shapes for brief introduction.
@see @ref AbstractShape2D, @ref AbstractShape3D
*/
template<UnsignedInt dimensions> class MAGNUM_SHAPES_EXPORT AbstractShape: public SceneGraph::AbstractGroupedFeature<dimensions, AbstractShape<dimensions>, Float> {
    #ifndef CORRADE_MSVC2017_COMPATIBILITY
    friend const Implementation::AbstractShape<dimensions>& Implementation::getAbstractShape<>(const AbstractShape<dimensions>&);
    #else
    /* Otherwise it complains that this is not a function */
    template<UnsignedInt dimensions_> friend const Implementation::AbstractShape<dimensions_>& Implementation::getAbstractShape(const Shapes::AbstractShape<dimensions_>&);
    #endif

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
            Composition,    /**< @ref Composition "Shape group" */
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
        explicit AbstractShape(SceneGraph::AbstractObject<dimensions, Float>& object, ShapeGroup<dimensions>* group = nullptr);

        /**
         * @brief Shape group containing this shape
         *
         * If the shape doesn't belong to any group, returns `nullptr`.
         */
        ShapeGroup<dimensions>* group();
        const ShapeGroup<dimensions>* group() const; /**< @overload */

        /** @brief Shape type */
        Type type() const;

        /**
         * @brief Detect collision with other shape
         *
         * Default implementation returns false.
         */
        bool collides(const AbstractShape<dimensions>& other) const;

        /**
         * @brief Collision with other shape
         *
         * Default implementation returns empty collision.
         */
        Collision<dimensions> collision(const AbstractShape<dimensions>& other) const;

    protected:
        /** Marks also the group as dirty */
        void markDirty() override;

    private:
        virtual const Implementation::AbstractShape<dimensions> MAGNUM_SHAPES_LOCAL & abstractTransformedShape() const = 0;
};

/** @brief Base class for two-dimensional object shapes */
typedef AbstractShape<2> AbstractShape2D;

/** @brief Base class for three-dimensional object shapes */
typedef AbstractShape<3> AbstractShape3D;

}}

#endif
