#ifndef Magnum_SceneGraph_AbstractObject_h
#define Magnum_SceneGraph_AbstractObject_h
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
 * @brief Class Magnum::SceneGraph::AbstractObject, alias Magnum::SceneGraph::AbstractObject2D, Magnum::SceneGraph::AbstractObject3D
 */

#include <Containers/LinkedList.h>

#include "DimensionTraits.h"
#include "SceneGraph.h"

#include "magnumCompatibility.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Base for objects

Provides minimal interface for features, not depending on object transformation
implementation. This class is not directly instantiatable, use Object subclass
instead. See also @ref scenegraph for more information.

Uses Corrade::Containers::LinkedList for storing features.

@see AbstractObject2D, AbstractObject3D
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<std::uint8_t dimensions, class T> class AbstractObject: private Corrade::Containers::LinkedList<AbstractFeature<dimensions, T>>
#else
template<std::uint8_t dimensions, class T = GLfloat> class AbstractObject
#endif
{
    friend class Corrade::Containers::LinkedList<AbstractFeature<dimensions, T>>;
    friend class Corrade::Containers::LinkedListItem<AbstractFeature<dimensions, T>, AbstractObject<dimensions, T>>;
    friend AbstractFeature<dimensions, T>::AbstractFeature(AbstractObject<dimensions, T>*);

    public:
        /** @brief Feature object type */
        typedef AbstractFeature<dimensions, T> FeatureType;

        inline virtual ~AbstractObject() {}

        /** @brief Whether this object has features */
        inline bool hasFeatures() const {
            return !Corrade::Containers::LinkedList<AbstractFeature<dimensions, T>>::isEmpty();
        }

        /** @brief First object feature or `nullptr`, if this object has no features */
        inline FeatureType* firstFeature() {
            return Corrade::Containers::LinkedList<AbstractFeature<dimensions, T>>::first();
        }

        /** @overload */
        inline const FeatureType* firstFeature() const {
            return Corrade::Containers::LinkedList<AbstractFeature<dimensions, T>>::first();
        }

        /** @brief Last object feature or `nullptr`, if this object has no features */
        inline FeatureType* lastFeature() {
            return Corrade::Containers::LinkedList<AbstractFeature<dimensions, T>>::last();
        }

        /** @overload */
        inline const FeatureType* lastFeature() const {
            return Corrade::Containers::LinkedList<AbstractFeature<dimensions, T>>::last();
        }

        /**
         * @brief %Scene object
         * @return Root object which is also scene or `nullptr`, if the object
         *      is not part of any scene.
         *
         * @todo Rename to scene() when I fully understand and fix covariant
         *      return issues.
         */
        virtual AbstractObject<dimensions, T>* sceneObject() = 0;

        /** @overload */
        virtual const AbstractObject<dimensions, T>* sceneObject() const = 0;

        /** @{ @name Object transformation */

        /**
         * @brief Transformation matrix relative to root object
         *
         * @see Object::absoluteTransformation()
         */
        virtual typename DimensionTraits<dimensions, T>::MatrixType absoluteTransformationMatrix() const = 0;

        /**
         * @brief Transformation matrices of given set of objects relative to this object
         *
         * All transformations are premultiplied with @p initialTransformationMatrix,
         * if specified.
         * @warning This function cannot check if all objects are of the same
         *      Object type, use typesafe Object::transformations() when
         *      possible.
         */
        virtual std::vector<typename DimensionTraits<dimensions, T>::MatrixType> transformationMatrices(const std::vector<AbstractObject<dimensions, T>*>& objects, const typename DimensionTraits<dimensions, T>::MatrixType& initialTransformationMatrix = typename DimensionTraits<dimensions, T>::MatrixType()) const = 0;

        /*@}*/

        /**
         * @{ @name Transformation caching
         *
         * See @ref scenegraph-caching for more information.
         */

        /**
         * @brief Whether absolute transformation is dirty
         *
         * Returns `true` if transformation of the object or any parent has
         * changed since last call to setClean(), `false` otherwise.
         *
         * All objects are dirty by default.
         *
         * @see @ref scenegraph-caching
         */
        virtual bool isDirty() const = 0;

        /**
         * @brief Set object absolute transformation as dirty
         *
         * Calls AbstractFeature::markDirty() on all object features and
         * recursively calls setDirty() on every child object which is not
         * already dirty. If the object is already marked as dirty, the
         * function does nothing.
         * @see @ref scenegraph-caching, setClean(), isDirty()
         */
        virtual void setDirty() = 0;

        /**
         * @brief Clean object absolute transformation
         *
         * Calls AbstractFeature::clean() and/or AbstractFeature::cleanInverted()
         * on all object features which have caching enabled and recursively
         * calls setClean() on every parent which is not already clean. If the
         * object is already clean, the function does nothing.
         * @see @ref scenegraph-caching, setDirty(), isDirty()
         */
        virtual void setClean() = 0;

        /*@}*/
};

/**
@brief Base for two-dimensional objects

Convenience alternative to <tt>%AbstractObject<2, T></tt>. See AbstractObject
for more information.
@note Not available on GCC < 4.7. Use <tt>%AbstractObject<2, T></tt> instead.
@see AbstractObject3D
@todoc Remove workaround when Doxygen supports alias template
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_GCC46_COMPATIBILITY
template<class T = GLfloat> using AbstractObject2D = AbstractObject<2, T>;
#endif
#else
typedef AbstractObject<2, T = GLfloat> AbstractObject2D;
#endif

/**
@brief Base for three-dimensional objects

Convenience alternative to <tt>%AbstractObject<3, T></tt>. See AbstractObject
for more information.
@note Not available on GCC < 4.7. Use <tt>%AbstractObject<3, T></tt> instead.
@see AbstractObject2D
@todoc Remove workaround when Doxygen supports alias template
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_GCC46_COMPATIBILITY
template<class T = GLfloat> using AbstractObject3D = AbstractObject<3, T>;
#endif
#else
typedef AbstractObject<3, T = GLfloat> AbstractObject3D;
#endif

}}

#endif
