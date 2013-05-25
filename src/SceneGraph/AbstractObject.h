#ifndef Magnum_SceneGraph_AbstractObject_h
#define Magnum_SceneGraph_AbstractObject_h
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
 * @brief Class Magnum::SceneGraph::AbstractObject, alias Magnum::SceneGraph::AbstractObject2D, Magnum::SceneGraph::AbstractObject3D
 */

#include <vector>
#include <Containers/LinkedList.h>

#include "DimensionTraits.h"
#include "SceneGraph.h"

#include "SceneGraph/magnumSceneGraphVisibility.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Base for objects

Provides minimal interface for features, not depending on object transformation
implementation. This class is not directly instantiatable, use Object subclass
instead. See also @ref scenegraph for more information.

Uses Corrade::Containers::LinkedList for storing features. Traversing through
the list is done like in the following code. It is also possible to go in
reverse order using lastFeature() and AbstractFeature::previousFeature().
@code
for(AbstractFeature* feature = o->firstFeature(); feature; feature = feature->nextFeature()) {
    // ...
}
@endcode

@see AbstractObject2D, AbstractObject3D
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<UnsignedInt dimensions, class T> class MAGNUM_SCENEGRAPH_EXPORT AbstractObject: private Containers::LinkedList<AbstractFeature<dimensions, T>>
#else
template<UnsignedInt dimensions, class T = Float> class AbstractObject
#endif
{
    friend class Containers::LinkedList<AbstractFeature<dimensions, T>>;
    friend class Containers::LinkedListItem<AbstractFeature<dimensions, T>, AbstractObject<dimensions, T>>;
    friend class AbstractFeature<dimensions, T>;

    public:
        /** @brief Feature object type */
        typedef AbstractFeature<dimensions, T> FeatureType;

        explicit AbstractObject();
        virtual ~AbstractObject();

        /** @brief Whether this object has features */
        bool hasFeatures() const {
            return !Containers::LinkedList<AbstractFeature<dimensions, T>>::isEmpty();
        }

        /** @brief First object feature or `nullptr`, if this object has no features */
        FeatureType* firstFeature() {
            return Containers::LinkedList<AbstractFeature<dimensions, T>>::first();
        }

        /** @overload */
        const FeatureType* firstFeature() const {
            return Containers::LinkedList<AbstractFeature<dimensions, T>>::first();
        }

        /** @brief Last object feature or `nullptr`, if this object has no features */
        FeatureType* lastFeature() {
            return Containers::LinkedList<AbstractFeature<dimensions, T>>::last();
        }

        /** @overload */
        const FeatureType* lastFeature() const {
            return Containers::LinkedList<AbstractFeature<dimensions, T>>::last();
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
         * @brief Transformation matrix
         *
         * @see Object::transformation()
         */
        virtual typename DimensionTraits<dimensions, T>::MatrixType transformationMatrix() const = 0;

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
        virtual std::vector<typename DimensionTraits<dimensions, T>::MatrixType> transformationMatrices(const std::vector<AbstractObject<dimensions, T>*>& objects, const typename DimensionTraits<dimensions, T>::MatrixType& initialTransformationMatrix = (typename DimensionTraits<dimensions, T>::MatrixType())) const = 0;

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
         *
         * See also setClean(const std::vector& objects), which cleans given
         * set of objects more efficiently than when calling setClean() on
         * each object individually.
         * @see @ref scenegraph-caching, setDirty(), isDirty()
         */
        virtual void setClean() = 0;

        /**
         * @brief Clean absolute transformations of given set of objects
         *
         * Only dirty objects in the list are cleaned.
         * @warning This function cannot check if all objects are of the same
         *      Object type, use typesafe Object::setClean(const std::vector& objects) when
         *      possible.
         */
        virtual void setClean(const std::vector<AbstractObject<dimensions, T>*>& objects) const = 0;

        /*@}*/
};

#ifndef CORRADE_GCC46_COMPATIBILITY
/**
@brief Base for two-dimensional objects

Convenience alternative to <tt>%AbstractObject<2, T></tt>. See AbstractObject
for more information.
@note Not available on GCC < 4.7. Use <tt>%AbstractObject<2, T></tt> instead.
@see AbstractObject3D
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T = Float>
#else
template<class T>
#endif
using AbstractObject2D = AbstractObject<2, T>;

/**
@brief Base for three-dimensional objects

Convenience alternative to <tt>%AbstractObject<3, T></tt>. See AbstractObject
for more information.
@note Not available on GCC < 4.7. Use <tt>%AbstractObject<3, T></tt> instead.
@see AbstractObject2D
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T = Float>
#else
template<class T>
#endif
using AbstractObject3D = AbstractObject<3, T>;
#endif

}}

#endif
