#ifndef Magnum_SceneGraph_AbstractFeature_h
#define Magnum_SceneGraph_AbstractFeature_h
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
 * @brief Class @ref Magnum::SceneGraph::AbstractFeature, alias @ref Magnum::SceneGraph::AbstractBasicFeature2D, @ref Magnum::SceneGraph::AbstractBasicFeature3D, typedef @ref Magnum::SceneGraph::AbstractFeature2D, @ref Magnum::SceneGraph::AbstractFeature3D, enum @ref Magnum::SceneGraph::CachedTransformation, enum set @ref Magnum::SceneGraph::CachedTransformations
 */

#include <Corrade/Containers/EnumSet.h>
#include <Corrade/Containers/LinkedList.h>

#include "Magnum/Magnum.h"
#include "Magnum/SceneGraph/AbstractObject.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Which transformation to cache in given feature

@see @ref scenegraph-features-caching, @ref CachedTransformations,
    @ref AbstractFeature::setCachedTransformations(), @ref AbstractFeature::clean(),
    @ref AbstractFeature::cleanInverted()
@todo Provide also simpler representations from which could benefit
    other transformation implementations, as they won't need to
    e.g. create transformation matrix from quaternion?
 */
enum class CachedTransformation: UnsignedByte {
    /**
     * Absolute transformation is cached.
     *
     * If enabled, @ref AbstractFeature::clean() is called when cleaning
     * object.
     */
    Absolute = 1 << 0,

    /**
     * Inverted absolute transformation is cached.
     *
     * If enabled, @ref AbstractFeature::cleanInverted() is called when
     * cleaning object.
     */
    InvertedAbsolute = 1 << 1
};

/**
@brief Which transformations to cache in this feature

@see @ref scenegraph-features-caching, @ref AbstractFeature::setCachedTransformations(),
    @ref AbstractFeature::clean(), @ref AbstractFeature::cleanInverted()
*/
typedef Containers::EnumSet<CachedTransformation> CachedTransformations;

CORRADE_ENUMSET_OPERATORS(CachedTransformations)

/**
@brief Base for object features

Contained in @ref Object, takes care of transformation caching. See
@ref scenegraph for introduction.

Uses @ref Corrade::Containers::LinkedList for accessing holder object and
sibling features.

@section SceneGraph-AbstractFeature-subclassing Subclassing

Feature is templated on dimension count and underlying transformation type, so
it can be used only on object having transformation with the same dimension
count and type.

@subsection SceneGraph-AbstractFeature-subclassing-caching Caching transformations in features

Features can cache absolute transformation of the object instead of computing
it from scratch every time to achieve better performance. See
@ref scenegraph-features-caching for introduction.

In order to have caching, you must enable it first, because by default the
caching is disabled. You can enable it using @ref setCachedTransformations()
and then implement corresponding cleaning function(s) --- either @ref clean(),
@ref cleanInverted() or both. Example:

@snippet MagnumSceneGraph.cpp AbstractFeature-caching

Before using the cached value explicitly request object cleaning by calling
@cpp object()->setClean() @ce.

@subsection SceneGraph-AbstractFeature-subclassing-object-transformation Accessing object transformation

The feature has by default only access to @ref AbstractObject, which doesn't
know about any used transformation. By using small template trick in the
constructor it is possible to gain access to transformation interface in the
constructor:

@snippet MagnumSceneGraph.cpp AbstractFeature-object-transformation

See @ref scenegraph-features-transformation for more detailed information.

@section SceneGraph-AbstractFeature-explicit-specializations Explicit template specializations

The following specializations are explicitly compiled into @ref SceneGraph
library. For other specializations (e.g. using @ref Magnum::Double "Double" type)
you have to use @ref AbstractFeature.hpp implementation file to avoid linker
errors. See also @ref compilation-speedup-hpp for more information.

-   @ref AbstractFeature2D
-   @ref AbstractFeature3D

@see @ref AbstractBasicFeature2D, @ref AbstractBasicFeature3D,
    @ref AbstractFeature2D, @ref AbstractFeature3D
*/
template<UnsignedInt dimensions, class T> class AbstractFeature
    #ifndef DOXYGEN_GENERATING_OUTPUT
    : private Containers::LinkedListItem<AbstractFeature<dimensions, T>, AbstractObject<dimensions, T>>
    #endif
{
    public:
        /**
         * @brief Object transformation underlying type
         * @m_since{2019,10}
         */
        typedef T Type;

        enum: UnsignedInt {
            /**
             * Dimension count
             * @m_since{2019,10}
             */
            Dimensions = dimensions
        };

        /**
         * @brief Constructor
         * @param object    Object holding this feature
         */
        explicit AbstractFeature(AbstractObject<dimensions, T>& object);

        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* This is here to avoid ambiguity with deleted copy constructor when
           passing `*this` from class subclassing both AbstractFeature and
           AbstractObject */
        template<class U, class = typename std::enable_if<std::is_base_of<AbstractObject<dimensions, T>, U>::value>::type> AbstractFeature(U& object): AbstractFeature(static_cast<AbstractObject<dimensions, T>&>(object)) {}
        #endif

        virtual ~AbstractFeature() = 0;

        /** @brief Object holding this feature */
        AbstractObject<dimensions, T>& object() {
            return *Containers::LinkedListItem<AbstractFeature<dimensions, T>, AbstractObject<dimensions, T>>::list();
        }

        /** @overload */
        const AbstractObject<dimensions, T>& object() const {
            return *Containers::LinkedListItem<AbstractFeature<dimensions, T>, AbstractObject<dimensions, T>>::list();
        }

        /** @brief Previous feature or `nullptr`, if this is first feature */
        AbstractFeature<dimensions, T>* previousFeature() {
            return Containers::LinkedListItem<AbstractFeature<dimensions, T>, AbstractObject<dimensions, T>>::previous();
        }

        /** @overload */
        const AbstractFeature<dimensions, T>* previousFeature() const {
            return Containers::LinkedListItem<AbstractFeature<dimensions, T>, AbstractObject<dimensions, T>>::previous();
        }

        /** @brief Next feature or `nullptr`, if this is last feature */
        AbstractFeature<dimensions, T>* nextFeature() {
            return Containers::LinkedListItem<AbstractFeature<dimensions, T>, AbstractObject<dimensions, T>>::next();
        }

        /** @overload */
        const AbstractFeature<dimensions, T>* nextFeature() const {
            return Containers::LinkedListItem<AbstractFeature<dimensions, T>, AbstractObject<dimensions, T>>::next();
        }

        /**
         * @{ @name Transformation caching
         *
         * See @ref scenegraph-features-caching for more information.
         */

        /**
         * @brief Which transformations are cached
         *
         * @see @ref scenegraph-features-caching, @ref clean(),
         *      @ref cleanInverted()
         */
        CachedTransformations cachedTransformations() const {
            return _cachedTransformations;
        }

    protected:
        /**
         * @brief Set transformations to be cached
         *
         * Based on which transformation types are enabled, @ref clean() or
         * @ref cleanInverted() is called when cleaning absolute object
         * transformation.
         *
         * Nothing is enabled by default.
         * @see @ref scenegraph-features-caching
         */
        void setCachedTransformations(CachedTransformations transformations) {
            _cachedTransformations = transformations;
        }

        /**
         * @brief Mark feature as dirty
         *
         * Reimplement only if you want to invalidate some external data when
         * object is marked as dirty. All expensive computations should be
         * done in @ref clean() and @ref cleanInverted().
         *
         * Default implementation does nothing.
         * @see @ref scenegraph-features-caching
         */
        virtual void markDirty();

        /**
         * @brief Clean data based on absolute transformation
         *
         * When object is cleaned and
         * @ref SceneGraph::CachedTransformation::Absolute "CachedTransformation::Absolute"
         * is enabled in @ref setCachedTransformations(), this function is
         * called to recalculate data based on absolute object transformation.
         *
         * Default implementation does nothing.
         * @see @ref scenegraph-features-caching, @ref cleanInverted()
         */
        virtual void clean(const MatrixTypeFor<dimensions, T>& absoluteTransformationMatrix);

        /**
         * @brief Clean data based on inverted absolute transformation
         *
         * When object is cleaned and @ref CachedTransformation::InvertedAbsolute
         * is enabled in @ref setCachedTransformations(), this function is
         * called to recalculate data based on inverted absolute object
         * transformation.
         *
         * Default implementation does nothing.
         * @see @ref scenegraph-features-caching, @ref clean()
         */
        virtual void cleanInverted(const MatrixTypeFor<dimensions, T>& invertedAbsoluteTransformationMatrix);

        /* Since 1.8.17, the original short-hand group closing doesn't work
           anymore. FFS. */
        /**
         * @}
         */

    private:
        #ifndef DOXYGEN_GENERATING_OUTPUT /* https://bugzilla.gnome.org/show_bug.cgi?id=776986 */
        friend Containers::LinkedList<AbstractFeature<dimensions, T>>;
        friend Containers::LinkedListItem<AbstractFeature<dimensions, T>, AbstractObject<dimensions, T>>;
        template<class> friend class Object;
        #endif

        CachedTransformations _cachedTransformations;
};

/**
@brief Base feature for two-dimensional scenes

Convenience alternative to @cpp AbstractFeature<2, T> @ce. See
@ref AbstractFeature for more information.
@see @ref AbstractFeature2D, @ref AbstractBasicFeature3D
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using AbstractBasicFeature2D = AbstractFeature<2, T>;
#endif

/**
@brief Base feature for two-dimensional float scenes

@see @ref AbstractFeature3D
*/
typedef AbstractBasicFeature2D<Float> AbstractFeature2D;

/**
@brief Base feature for three-dimensional scenes

Convenience alternative to @cpp AbstractFeature<3, T> @ce. See
@ref AbstractFeature for more information.
@see @ref AbstractFeature3D, @ref AbstractBasicFeature2D
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using AbstractBasicFeature3D = AbstractFeature<3, T>;
#endif

/**
@brief Base feature for three-dimensional float scenes

@see @ref AbstractFeature2D
*/
typedef AbstractBasicFeature3D<Float> AbstractFeature3D;

#if defined(CORRADE_TARGET_WINDOWS) && !(defined(CORRADE_TARGET_MINGW) && !defined(CORRADE_TARGET_CLANG))
extern template class MAGNUM_SCENEGRAPH_EXPORT AbstractFeature<2, Float>;
extern template class MAGNUM_SCENEGRAPH_EXPORT AbstractFeature<3, Float>;
#endif

}}

#endif
