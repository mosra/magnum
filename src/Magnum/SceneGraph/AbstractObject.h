#ifndef Magnum_SceneGraph_AbstractObject_h
#define Magnum_SceneGraph_AbstractObject_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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
 * @brief Class @ref Magnum::SceneGraph::AbstractObject, alias @ref Magnum::SceneGraph::AbstractBasicObject2D, @ref Magnum::SceneGraph::AbstractBasicObject3D, typedef @ref Magnum::SceneGraph::AbstractObject2D, @ref Magnum::SceneGraph::AbstractObject3D
 */

#include <functional>
#include <vector>
#include <Corrade/Containers/LinkedList.h>

#include "Magnum/DimensionTraits.h"
#include "Magnum/SceneGraph/SceneGraph.h"
#include "Magnum/SceneGraph/visibility.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Base for objects

Provides minimal interface for features, not depending on object transformation
implementation. This class is not directly instantiatable, use @ref Object
subclass instead. See also @ref scenegraph for more information.

Uses @ref Corrade::Containers::LinkedList for storing features. Traversing
through the list is done like in the following code. It is also possible to go
in reverse order using @ref lastFeature() and @ref AbstractFeature::previousFeature().
@code
for(AbstractFeature* feature = o->firstFeature(); feature; feature = feature->nextFeature()) {
    // ...
}
@endcode

@section AbstractObject-explicit-specializations Explicit template specializations

The following specialization are explicitly compiled into @ref SceneGraph
library. For other specializations (e.g. using @ref Double type) you have to
use @ref Object.hpp implementation file to avoid linker errors. See also
relevant sections in @ref Object-explicit-specializations "Object" and
@ref AbstractTransformation-explicit-specializations "AbstractTransformation"
class documentation or @ref compilation-speedup-hpp for more information.

-   @ref AbstractObject2D
-   @ref AbstractObject3D

@see @ref AbstractBasicObject2D, @ref AbstractBasicObject3D,
    @ref AbstractObject2D, @ref AbstractObject3D
*/
template<UnsignedInt dimensions, class T> class AbstractObject
    #ifndef DOXYGEN_GENERATING_OUTPUT
    : private Containers::LinkedList<AbstractFeature<dimensions, T>>
    #endif
{
    friend class Containers::LinkedList<AbstractFeature<dimensions, T>>;
    friend class Containers::LinkedListItem<AbstractFeature<dimensions, T>, AbstractObject<dimensions, T>>;
    friend class AbstractFeature<dimensions, T>;

    public:
        /** @brief Matrix type */
        typedef typename DimensionTraits<dimensions, T>::MatrixType MatrixType;

        /** @brief Feature object type */
        typedef AbstractFeature<dimensions, T> FeatureType;

        /* This isn't protected because having virtual destructor here is
           better than having it duplicated in all Object specializations */
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
         * @brief %Scene
         * @return %Scene or `nullptr`, if the object is not part of any scene.
         */
        AbstractObject<dimensions, T>* scene() { return doScene(); }

        /** @overload */
        const AbstractObject<dimensions, T>* scene() const { return doScene(); }

        /** @{ @name Object transformation */

        /**
         * @brief Transformation matrix
         *
         * See also `transformation()` function of various transformation
         * classes.
         */
        MatrixType transformationMatrix() const {
            return doTransformationMatrix();
        }

        /**
         * @brief Transformation matrix relative to root object
         *
         * @see @ref Object::absoluteTransformation()
         */
        MatrixType absoluteTransformationMatrix() const {
            return doAbsoluteTransformationMatrix();
        }

        /**
         * @brief Transformation matrices of given set of objects relative to this object
         *
         * All transformations are premultiplied with @p initialTransformationMatrix,
         * if specified.
         * @warning This function cannot check if all objects are of the same
         *      @ref Object type, use typesafe @ref Object::transformationMatrices()
         *      when possible.
         */
        std::vector<MatrixType> transformationMatrices(const std::vector<std::reference_wrapper<AbstractObject<dimensions, T>>>& objects, const MatrixType& initialTransformationMatrix = MatrixType()) const {
            return doTransformationMatrices(objects, initialTransformationMatrix);
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief transformationMatrices(const std::vector<std::reference_wrapper<AbstractObject<dimensions, T>>>&, const MatrixType&)
         * @deprecated Use @ref Magnum::SceneGraph::AbstractObject::transformationMatrices(const std::vector<std::reference_wrapper<AbstractObject<dimensions, T>>>&, const MatrixType&) "transformationMatrices(const std::vector<std::reference_wrapper<AbstractObject<dimensions, T>>>&, const MatrixType&)" instead.
         */
        CORRADE_DEPRECATED("use transformationMatrices(const std::vector<std::reference_wrapper<AbstractObject<dimensions, T>>>&, const MatrixType&) instead") std::vector<MatrixType> transformationMatrices(const std::vector<AbstractObject<dimensions, T>*>& objects, const MatrixType& initialTransformationMatrix = MatrixType()) const;

        #ifdef CORRADE_GCC47_COMPATIBILITY
        /* Workarounds to avoid ambiguous overload errors on GCC < 4.8. And I
           thought 4.7 was bug-free. */
        std::vector<MatrixType> transformationMatrices(std::initializer_list<std::reference_wrapper<AbstractObject<dimensions, T>>>& objects, const MatrixType& initialTransformationMatrix = MatrixType()) {
            return transformationMatrices(std::vector<std::reference_wrapper<AbstractObject<dimensions, T>>>{objects}, initialTransformationMatrix);
        }
        CORRADE_DEPRECATED("use transformationMatrices(const std::vector<std::reference_wrapper<AbstractObject<dimensions, T>>>&, const MatrixType&) instead") std::vector<MatrixType> transformationMatrices(std::initializer_list<AbstractObject<dimensions, T>*> objects, const MatrixType& initialTransformationMatrix = MatrixType()) const;
        #endif
        #endif

        /*@}*/

        /**
         * @{ @name Transformation caching
         *
         * See @ref scenegraph-caching for more information.
         */

        /**
         * @brief Clean absolute transformations of given set of objects
         *
         * Only dirty objects in the list are cleaned.
         * @warning This function cannot check if all objects are of the same
         *      @ref Object type, use typesafe @ref Object::setClean() when
         *      possible.
         */
        static void setClean(const std::vector<std::reference_wrapper<AbstractObject<dimensions, T>>>& objects) {
            if(objects.empty()) return;
            objects.front().get().doSetClean(objects);
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief setClean(const std::vector<std::reference_wrapper<AbstractObject<dimensions, T>>>&)
         * @deprecated Use @ref Magnum::SceneGraph::AbstractObject::setClean(const std::vector<std::reference_wrapper<AbstractObject<dimensions, T>>>&) "setClean(const std::vector<std::reference_wrapper<AbstractObject<dimensions, T>>>&)" instead.
         */
        static CORRADE_DEPRECATED("use setClean(const std::vector<std::reference_wrapper<AbstractObject<dimensions, T>>>&) instead") void setClean(const std::vector<AbstractObject<dimensions, T>*>& objects);

        #ifdef CORRADE_GCC47_COMPATIBILITY
        /* Workarounds to avoid ambiguous overload errors on GCC < 4.8. And I
           thought 4.7 was bug-free. */
        static void setClean(std::initializer_list<std::reference_wrapper<AbstractObject<dimensions, T>>> objects) {
            return setClean(std::vector<std::reference_wrapper<AbstractObject<dimensions, T>>>{objects});
        }
        static CORRADE_DEPRECATED("use setClean(const std::vector<std::reference_wrapper<AbstractObject<dimensions, T>>>&) instead") void setClean(std::initializer_list<AbstractObject<dimensions, T>*> objects);
        #endif
        #endif

        /**
         * @brief Whether absolute transformation is dirty
         *
         * Returns `true` if transformation of the object or any parent has
         * changed since last call to @ref setClean(), `false` otherwise. All
         * objects are dirty by default.
         * @see @ref scenegraph-caching
         */
        bool isDirty() const { return doIsDirty(); }

        /**
         * @brief Set object absolute transformation as dirty
         *
         * Calls @ref AbstractFeature::markDirty() on all object features and
         * recursively calls @ref setDirty() on every child object which is not
         * already dirty. If the object is already marked as dirty, the
         * function does nothing.
         * @see @ref scenegraph-caching, @ref setClean(), @ref isDirty()
         */
        void setDirty() { doSetDirty(); }

        /**
         * @brief Clean object absolute transformation
         *
         * Calls @ref AbstractFeature::clean() and/or @ref AbstractFeature::cleanInverted()
         * on all object features which have caching enabled and recursively
         * calls @ref setClean() on every parent which is not already clean. If
         * the object is already clean, the function does nothing.
         *
         * See also @ref setClean(const std::vector<AbstractObject<dimensions, T>*>&),
         * which cleans given set of objects more efficiently than when calling
         * @ref setClean() on each object individually.
         * @see @ref scenegraph-caching, @ref setDirty(), @ref isDirty()
         */
        void setClean() { doSetClean(); }

        /*@}*/

    private:
        virtual AbstractObject<dimensions, T>* doScene() = 0;
        virtual const AbstractObject<dimensions, T>* doScene() const = 0;

        virtual MatrixType doTransformationMatrix() const = 0;
        virtual MatrixType doAbsoluteTransformationMatrix() const = 0;
        virtual std::vector<MatrixType> doTransformationMatrices(const std::vector<std::reference_wrapper<AbstractObject<dimensions, T>>>& objects, const MatrixType& initialTransformationMatrix) const = 0;

        virtual bool doIsDirty() const = 0;
        virtual void doSetDirty() = 0;
        virtual void doSetClean() = 0;
        virtual void doSetClean(const std::vector<std::reference_wrapper<AbstractObject<dimensions, T>>>& objects) = 0;
};

#ifndef CORRADE_GCC46_COMPATIBILITY
/**
@brief Base object for two-dimensional scenes

Convenience alternative to <tt>%AbstractObject<2, T></tt>. See
@ref AbstractObject for more information.
@note Not available on GCC < 4.7. Use <tt>%AbstractObject<2, T></tt> instead.
@see @ref AbstractObject2D, @ref AbstractBasicObject3D
*/
template<class T> using AbstractBasicObject2D = AbstractObject<2, T>;
#endif

/**
@brief Base object for two-dimensional float scenes

@see @ref AbstractObject3D
*/
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef AbstractBasicObject2D<Float> AbstractObject2D;
#else
typedef AbstractObject<2, Float> AbstractObject2D;
#endif

#ifndef CORRADE_GCC46_COMPATIBILITY
/**
@brief Base object for three-dimensional scenes

Convenience alternative to <tt>%AbstractObject<3, T></tt>. See
@ref AbstractObject for more information.
@note Not available on GCC < 4.7. Use <tt>%AbstractObject<3, T></tt> instead.
@see @ref AbstractObject3D, @ref AbstractBasicObject2D
*/
template<class T> using AbstractBasicObject3D = AbstractObject<3, T>;
#endif

/**
@brief Base object for three-dimensional float scenes

@see @ref AbstractObject2D
*/
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef AbstractBasicObject3D<Float> AbstractObject3D;
#else
typedef AbstractObject<3, Float> AbstractObject3D;
#endif

#ifdef CORRADE_TARGET_WINDOWS
extern template class MAGNUM_SCENEGRAPH_EXPORT AbstractObject<2, Float>;
extern template class MAGNUM_SCENEGRAPH_EXPORT AbstractObject<3, Float>;
#endif

}}

#endif
