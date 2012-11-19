#ifndef Magnum_SceneGraph_Object_h
#define Magnum_SceneGraph_Object_h
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
 * @brief Class Magnum::SceneGraph::Object
 */

#include <Containers/EnumSet.h>

#include "AbstractFeature.h"
#include "AbstractObject.h"

#include "magnumSceneGraphVisibility.h"

namespace Magnum { namespace SceneGraph {

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {
    enum class ObjectFlag: std::uint8_t {
        Dirty = 1 << 0,
        Visited = 1 << 1,
        Joint = 1 << 2
    };

    typedef Corrade::Containers::EnumSet<ObjectFlag, std::uint8_t> ObjectFlags;

    CORRADE_ENUMSET_OPERATORS(ObjectFlags)
}
#endif

/**
@brief %Object

Base of scene graph. Contains specific transformation implementation, takes
care of parent/children relationship and contains features. See @ref scenegraph
for introduction.

Uses Corrade::Containers::LinkedList for parent/children relationship.
Traversing through the list is done like in the following code. It is also
possible to go in reverse order using lastChild() and previousSibling().
@code
for(Object* child = o->firstChild(); child; child = child->nextSibling()) {
    // ...
}
@endcode

@section Object-explicit-specializations Explicit template specializations

The following specialization are explicitly compiled into SceneGraph library.
For other specializations you have to use Object.hpp implementation file to
avoid linker errors. See @ref compilation-speedup-hpp for more information.

 - @ref MatrixTransformation2D "Object<MatrixTransformation2D<>>"
 - @ref MatrixTransformation3D "Object<MatrixTransformation3D<>>"

@see Scene, AbstractFeature, AbstractTransformation
*/
template<class Transformation> class Object: public AbstractObject<Transformation::Dimensions, typename Transformation::Type>, public Transformation
    #ifndef DOXYGEN_GENERATING_OUTPUT
    , private Corrade::Containers::LinkedList<Object<Transformation>>, private Corrade::Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>
    #endif
{
    friend class Corrade::Containers::LinkedList<Object<Transformation>>;
    friend class Corrade::Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>;

    #ifndef DOXYGEN_GENERATING_OUTPUT
    Object(const Object<Transformation>& other) = delete;
    Object(Object<Transformation>&& other) = delete;
    Object<Transformation>& operator=(const Object<Transformation>& other) = delete;
    Object<Transformation>& operator=(Object<Transformation>&& other) = delete;
    #endif

    public:
        /**
         * @brief Clean absolute transformations of given set of objects
         *
         * Only dirty objects in the list are cleaned.
         * @see setClean(), AbstractObject::setClean()
         */
        /* `objects` passed by copy intentionally (to avoid copy internally) */
        static void setClean(std::vector<Object<Transformation>*> objects);

        /**
         * @brief Constructor
         * @param parent    Parent object
         */
        inline Object(Object<Transformation>* parent = nullptr): counter(0xFFFFu), flags(Flag::Dirty) {
            setParent(parent);
        }

        /**
         * @brief Destructor
         *
         * Removes itself from parent's children list and destroys all own
         * children.
         */
        inline virtual ~Object() {}

        /**
         * @{ @name Scene hierarchy
         *
         * See @ref scenegraph-hierarchy for more information.
         */

        /** @brief Whether this object is scene */
        virtual inline bool isScene() const { return false; }

        /**
         * @brief %Scene
         * @return %Scene or `nullptr`, if the object is not part of any scene.
         */
        Scene<Transformation>* scene();

        /** @overload */
        const Scene<Transformation>* scene() const;

        /** @brief Parent object or `nullptr`, if this is root object */
        inline Object<Transformation>* parent() {
            return Corrade::Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>::list();
        }

        /** @overload */
        inline const Object<Transformation>* parent() const {
            return Corrade::Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>::list();
        }

        /** @brief Previous sibling object or `nullptr`, if this is first object */
        inline Object<Transformation>* previousSibling() {
            return Corrade::Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>::previous();
        }

        /** @overload */
        inline const Object<Transformation>* previousSibling() const {
            return Corrade::Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>::previous();
        }

        /** @brief Next sibling object or `nullptr`, if this is last object */
        inline Object<Transformation>* nextSibling() {
            return Corrade::Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>::next();
        }

        /** @overload */
        inline const Object<Transformation>* nextSibling() const {
            return Corrade::Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>::next();
        }

        /** @brief Whether this object has children */
        inline bool hasChildren() const {
            return !Corrade::Containers::LinkedList<Object<Transformation>>::isEmpty();
        }

        /** @brief First child object or `nullptr`, if this object has no children */
        inline Object<Transformation>* firstChild() {
            return Corrade::Containers::LinkedList<Object<Transformation>>::first();
        }

        /** @overload */
        inline const Object<Transformation>* firstChild() const {
            return Corrade::Containers::LinkedList<Object<Transformation>>::first();
        }

        /** @brief Last child object or `nullptr`, if this object has no children */
        inline Object<Transformation>* lastChild() {
            return Corrade::Containers::LinkedList<Object<Transformation>>::last();
        }

        /** @overload */
        inline const Object<Transformation>* lastChild() const {
            return Corrade::Containers::LinkedList<Object<Transformation>>::last();
        }

        /**
         * @brief Set parent object
         * @return Pointer to self (for method chaining)
         */
        Object<Transformation>* setParent(Object<Transformation>* parent);

        /*@}*/

        /** @{ @name Object transformation */

        inline typename DimensionTraits<Transformation::Dimensions, typename Transformation::Type>::MatrixType absoluteTransformationMatrix() const override {
            return Transformation::toMatrix(absoluteTransformation());
        }

        /**
         * @brief Transformation relative to root object
         *
         * @see absoluteTransformationMatrix()
         */
        typename Transformation::DataType absoluteTransformation() const;

        /**
         * @brief Transformations of given group of objects relative to this object
         *
         * All transformations can be premultiplied with @p initialTransformation,
         * if specified.
         * @see AbstractObject::transformationMatrices()
         */
        /* `objects` passed by copy intentionally (to allow move from
           transformationMatrices() and avoid copy in the function itself) */
        std::vector<typename Transformation::DataType> transformations(std::vector<Object<Transformation>*> objects, const typename Transformation::DataType& initialTransformation = typename Transformation::DataType()) const;

        /*@}*/

        inline bool isDirty() const override { return !!(flags & Flag::Dirty); }
        void setDirty() override;
        void setClean() override;

    private:
        /* GCC 4.4 doesn't support lambda functions */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        struct DirtyCheck {
            inline bool operator()(Object<Transformation>* o) const { return !o->isDirty(); }
        };
        #endif

        Object<Transformation>* sceneObject() override;
        const Object<Transformation>* sceneObject() const override;

        std::vector<typename DimensionTraits<Transformation::Dimensions, typename Transformation::Type>::MatrixType> transformationMatrices(const std::vector<AbstractObject<Transformation::Dimensions, typename Transformation::Type>*>& objects, const typename DimensionTraits<Transformation::Dimensions, typename Transformation::Type>::MatrixType& initialTransformationMatrix = typename DimensionTraits<Transformation::Dimensions, typename Transformation::Type>::MatrixType()) const override;

        typename Transformation::DataType computeJointTransformation(const std::vector<Object<Transformation>*>& jointObjects, std::vector<typename Transformation::DataType>& jointTransformations, const std::size_t joint, const typename Transformation::DataType& initialTransformation) const;

        void setClean(const std::vector<AbstractObject<Transformation::Dimensions, typename Transformation::Type>*>& objects) const override;

        void setClean(const typename Transformation::DataType& absoluteTransformation);

        typedef Implementation::ObjectFlag Flag;
        typedef Implementation::ObjectFlags Flags;
        std::uint16_t counter;
        Flags flags;
};

}}

#endif
