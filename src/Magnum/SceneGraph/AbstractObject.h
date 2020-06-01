#ifndef Magnum_SceneGraph_AbstractObject_h
#define Magnum_SceneGraph_AbstractObject_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

Uses @ref Corrade::Containers::LinkedList for efficient feature management.
Traversing through the feature list can be done using range-based for:

@snippet MagnumSceneGraph.cpp AbstractObject-features-range

Or, if you need more flexibility, like in the following code. It is also
possible to go in reverse order using @ref Corrade::Containers::LinkedList::last()
and @ref AbstractFeature::previousFeature().

@snippet MagnumSceneGraph.cpp AbstractObject-features

@section SceneGraph-AbstractObject-explicit-specializations Explicit template specializations

The following specializations are explicitly compiled into @ref SceneGraph
library. For other specializations (e.g. using @ref Magnum::Double "Double"
type) you have to use @ref Object.hpp implementation file to avoid linker
errors. See also relevant sections in @ref SceneGraph-Object-explicit-specializations "Object"
and @ref SceneGraph-AbstractTransformation-explicit-specializations "AbstractTransformation"
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
    public:
        /**
         * @brief Transformation underlying type
         * @m_since{2019,10}
         */
        typedef T Type;

        /** @brief Matrix type */
        typedef MatrixTypeFor<dimensions, T> MatrixType;

        /** @brief Feature type */
        typedef AbstractFeature<dimensions, T> FeatureType;

        enum: UnsignedInt {
            /**
             * Dimension count
             * @m_since{2019,10}
             */
            Dimensions = dimensions
        };

        /* This isn't protected because having virtual destructor here is
           better than having it duplicated in all Object specializations */
        explicit AbstractObject();
        virtual ~AbstractObject();

        /**
         * @brief Object features
         *
         * @see @ref AbstractFeature::object(),
         *      @ref AbstractFeature::previousFeature(),
         *      @ref AbstractFeature::nextFeature()
         */
        Containers::LinkedList<AbstractFeature<dimensions, T>>& features() {
            return static_cast<Containers::LinkedList<AbstractFeature<dimensions, T>>&>(*this);
        }

        /** @overload */
        const Containers::LinkedList<AbstractFeature<dimensions, T>>& features() const {
            return static_cast<const Containers::LinkedList<AbstractFeature<dimensions, T>>&>(*this);
        }

        /**
         * @brief Add a feature
         *
         * Calling `object.addFeature<MyFeature>(args...)` is equivalent to
         * `new MyFeature{object, args...}`.
         */
        template<class U, class ...Args> U& addFeature(Args&&... args) {
            return *(new U{*this, std::forward<Args>(args)...});
        }

        /**
         * @brief Scene
         * @return Scene or @cpp nullptr @ce, if the object is not part of any
         *      scene.
         */
        AbstractObject<dimensions, T>* scene() { return doScene(); }

        /** @overload */
        const AbstractObject<dimensions, T>* scene() const { return doScene(); }

        /** @brief Parent object or `nullptr`, if this is root object */
        AbstractObject<dimensions, T>* parent() { return doParent(); }

        /** @overload */
        const AbstractObject<dimensions, T>* parent() const { return doParent(); }

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
         * All transformations are post-multiplied with
         * @p finalTransformationMatrix, if specified (it gets applied on the
         * left-most side, suitable for example for an inverse camera
         * transformation or a projection matrix).
         * @warning This function cannot check if all objects are of the same
         *      @ref Object type, use typesafe @ref Object::transformationMatrices()
         *      when possible.
         */
        std::vector<MatrixType> transformationMatrices(const std::vector<std::reference_wrapper<AbstractObject<dimensions, T>>>& objects, const MatrixType& finalTransformationMatrix = MatrixType()) const {
            return doTransformationMatrices(objects, finalTransformationMatrix);
        }

        /* Since 1.8.17, the original short-hand group closing doesn't work
           anymore. FFS. */
        /**
         * @}
         */

        /**
         * @{ @name Transformation caching
         *
         * See @ref scenegraph-features-caching for more information.
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

        /**
         * @brief Whether absolute transformation is dirty
         *
         * Returns @cpp true @ce if transformation of the object or any parent
         * has changed since last call to @ref setClean(), @cpp false @ce
         * otherwise. All objects are dirty by default.
         * @see @ref scenegraph-features-caching
         */
        bool isDirty() const { return doIsDirty(); }

        /**
         * @brief Set object absolute transformation as dirty
         *
         * Calls @ref AbstractFeature::markDirty() on all object features and
         * recursively calls @ref setDirty() on every child object which is not
         * already dirty. If the object is already marked as dirty, the
         * function does nothing.
         * @see @ref scenegraph-features-caching, @ref setClean(),
         *      @ref isDirty()
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
         * See also @ref setClean(const std::vector<std::reference_wrapper<AbstractObject<dimensions, T>>>&),
         * which cleans given set of objects more efficiently than when calling
         * @ref setClean() on each object individually.
         * @see @ref scenegraph-features-caching, @ref setDirty(),
         *      @ref isDirty()
         */
        void setClean() { doSetClean(); }

        /* Since 1.8.17, the original short-hand group closing doesn't work
           anymore. FFS. */
        /**
         * @}
         */

    private:
        #ifndef DOXYGEN_GENERATING_OUTPUT /* https://bugzilla.gnome.org/show_bug.cgi?id=776986 */
        friend Containers::LinkedList<AbstractFeature<dimensions, T>>;
        friend Containers::LinkedListItem<AbstractFeature<dimensions, T>, AbstractObject<dimensions, T>>;
        friend AbstractFeature<dimensions, T>;
        #endif

        virtual AbstractObject<dimensions, T>* doScene() = 0;
        virtual const AbstractObject<dimensions, T>* doScene() const = 0;

        virtual AbstractObject<dimensions, T>* doParent() = 0;
        virtual const AbstractObject<dimensions, T>* doParent() const = 0;

        virtual MatrixType doTransformationMatrix() const = 0;
        virtual MatrixType doAbsoluteTransformationMatrix() const = 0;
        virtual std::vector<MatrixType> doTransformationMatrices(const std::vector<std::reference_wrapper<AbstractObject<dimensions, T>>>& objects, const MatrixType& finalTransformationMatrix) const = 0;

        virtual bool doIsDirty() const = 0;
        virtual void doSetDirty() = 0;
        virtual void doSetClean() = 0;
        virtual void doSetClean(const std::vector<std::reference_wrapper<AbstractObject<dimensions, T>>>& objects) = 0;
};

/**
@brief Base object for two-dimensional scenes

Convenience alternative to @cpp AbstractObject<2, T> @ce. See
@ref AbstractObject for more information.
@see @ref AbstractObject2D, @ref AbstractBasicObject3D
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using AbstractBasicObject2D = AbstractObject<2, T>;
#endif

/**
@brief Base object for two-dimensional float scenes

@see @ref AbstractObject3D
*/
typedef AbstractBasicObject2D<Float> AbstractObject2D;

/**
@brief Base object for three-dimensional scenes

Convenience alternative to @cpp AbstractObject<3, T> @ce. See
@ref AbstractObject for more information.
@see @ref AbstractObject3D, @ref AbstractBasicObject2D
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
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

#if defined(CORRADE_TARGET_WINDOWS) && !(defined(CORRADE_TARGET_MINGW) && !defined(CORRADE_TARGET_CLANG))
extern template class MAGNUM_SCENEGRAPH_EXPORT AbstractObject<2, Float>;
extern template class MAGNUM_SCENEGRAPH_EXPORT AbstractObject<3, Float>;
#endif

}}

#endif
