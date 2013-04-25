#ifndef Magnum_Physics_ObjectShapeGroup_h
#define Magnum_Physics_ObjectShapeGroup_h
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
 * @brief Class Magnum::Physics::ObjectShapeGroup, typedef Magnum::Physics::ObjectShapeGroup2D, Magnum::Physics::ObjectShapeGroup3D
 */

#include <vector>

#include "Physics/Physics.h"
#include "SceneGraph/FeatureGroup.h"

#include "magnumPhysicsVisibility.h"

namespace Magnum { namespace Physics {

/**
@brief Group of object shapes

See ObjectShape for more information.
@see @ref scenegraph, ObjectShapeGroup2D, ObjectShapeGroup3D
*/
template<UnsignedInt dimensions> class MAGNUM_PHYSICS_EXPORT ObjectShapeGroup: public SceneGraph::FeatureGroup<dimensions, AbstractObjectShape<dimensions>> {
    friend class AbstractObjectShape<dimensions>;

    public:
        /**
         * @brief Constructor
         *
         * Marks the group as dirty.
         */
        inline explicit ObjectShapeGroup(): dirty(true) {}

        /**
         * @brief Whether the group is dirty
         * @return True if any object in the group is dirty, false otherwise.
         */
        inline bool isDirty() const { return dirty; }

        /**
         * @brief Set the group as dirty
         *
         * If some body in the group changes its transformation, it sets dirty
         * status also on the group to indicate that the body and maybe also
         * group state needs to be cleaned before computing collisions.
         *
         * @see setClean()
         */
        inline void setDirty() { dirty = true; }

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
         * collisions, returns `nullptr`. Calls setClean() before the
         * operation.
         */
        AbstractObjectShape<dimensions>* firstCollision(const AbstractObjectShape<dimensions>* shape);

    private:
        bool dirty;
};

/**
@brief Group of two-dimensional shaped objects

See ObjectShape for more information.
@see ObjectShapeGroup3D
*/
typedef ObjectShapeGroup<2> ObjectShapeGroup2D;

/**
@brief Group of three-dimensional shaped objects

See ObjectShape for more information.
@see ObjectShapeGroup2D
*/
typedef ObjectShapeGroup<3> ObjectShapeGroup3D;

}}

#endif
