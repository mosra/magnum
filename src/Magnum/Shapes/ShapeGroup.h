#ifndef Magnum_Shapes_ShapeGroup_h
#define Magnum_Shapes_ShapeGroup_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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
@brief Class @ref Magnum::Shapes::ShapeGroup, typedef @ref Magnum::Shapes::ShapeGroup2D, @ref Magnum::Shapes::ShapeGroup3D

@deprecated The @ref Magnum::Shapes library is a failed design experiment and
    is scheduled for removal in a future release. Related geometry algorithms
    were moved to @ref Magnum::Math::Distance and @ref Magnum::Math::Intersection;
    if you need a full-fledged physics library, please have look at
    [Bullet](https://bulletphysics.org), which has Magnum integration in
    @ref Magnum::BulletIntegration, or at [Box2D](https://box2d.org/), which
    has a @ref examples-box2d "Magnum example" as well.
*/

#include <vector>

#include "Magnum/SceneGraph/FeatureGroup.h"
#include "Magnum/Shapes/AbstractShape.h"
#include "Magnum/Shapes/visibility.h"

/* File-level deprecation warning issued from Shapes.h */

namespace Magnum { namespace Shapes {

CORRADE_IGNORE_DEPRECATED_PUSH
/**
@brief Group of shapes

@deprecated The @ref Shapes library is a failed design experiment and is
    scheduled for removal in a future release. Related geometry algorithms were
    moved to @ref Math::Distance and @ref Math::Intersection; if you need a
    full-fledged physics library, please have look at [Bullet](https://bulletphysics.org),
    which has Magnum integration in @ref BulletIntegration, or at
    [Box2D](https://box2d.org/), which has a @ref examples-box2d "Magnum example"
    as well.

See @ref Shape for more information. See @ref shapes for brief introduction.
@see @ref scenegraph, @ref ShapeGroup2D, @ref ShapeGroup3D
*/
template<UnsignedInt dimensions> class CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") MAGNUM_SHAPES_EXPORT ShapeGroup: public SceneGraph::FeatureGroup<dimensions, AbstractShape<dimensions>, Float> {
    friend AbstractShape<dimensions>;

    public:
        /**
         * @brief Constructor
         *
         * Marks the group as dirty.
         */
        explicit ShapeGroup(): dirty(true) {}

        /**
         * @brief Whether the group is dirty
         * @return True if any object in the group is dirty, false otherwise.
         */
        bool isDirty() const { return dirty; }

        /**
         * @brief Set the group as dirty
         *
         * If some body in the group changes its transformation, it sets dirty
         * status also on the group to indicate that the body and maybe also
         * group state needs to be cleaned before computing collisions.
         * @see @ref setClean()
         */
        void setDirty() { dirty = true; }

        /**
         * @brief Set the group and all bodies as clean
         *
         * This function is called before computing any collisions to ensure
         * all objects are cleaned.
         */
        void setClean();

        /**
         * @brief First collision of given shape with other shapes in the group
         *
         * Returns first shape colliding with given one. If there aren't any
         * collisions, returns @cpp nullptr @ce. Calls @ref setClean() before
         * the operation.
         */
        AbstractShape<dimensions>* firstCollision(const AbstractShape<dimensions>& shape);

    private:
        bool dirty;
};

/**
@brief Group of two-dimensional shapes

@deprecated The @ref Shapes library is a failed design experiment and is
    scheduled for removal in a future release. Related geometry algorithms were
    moved to @ref Math::Distance and @ref Math::Intersection; if you need a
    full-fledged physics library, please have look at [Bullet](https://bulletphysics.org),
    which has Magnum integration in @ref BulletIntegration, or at
    [Box2D](https://box2d.org/), which has a @ref examples-box2d "Magnum example"
    as well.

See @ref Shape for more information.
@see @ref ShapeGroup3D
*/
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") ShapeGroup<2> ShapeGroup2D;

/**
@brief Group of three-dimensional shapes

@deprecated The @ref Shapes library is a failed design experiment and is
    scheduled for removal in a future release. Related geometry algorithms were
    moved to @ref Math::Distance and @ref Math::Intersection; if you need a
    full-fledged physics library, please have look at [Bullet](https://bulletphysics.org),
    which has Magnum integration in @ref BulletIntegration, or at
    [Box2D](https://box2d.org/), which has a @ref examples-box2d "Magnum example"
    as well.

See @ref Shape for more information.
@see @ref ShapeGroup2D
*/
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") ShapeGroup<3> ShapeGroup3D;
CORRADE_IGNORE_DEPRECATED_POP

}}

#endif
