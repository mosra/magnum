#ifndef Magnum_SceneGraph_AbstractFeature_h
#define Magnum_SceneGraph_AbstractFeature_h
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
 * @brief Class Magnum::SceneGraph::AbstractBasicFeature, typedef Magnum::SceneGraph::AbstractFeature2D, Magnum::SceneGraph::AbstractFeature3D, enum Magnum::SceneGraph::CachedTransformation, enum set Magnum::SceneGraph::CachedTransformations
 */

#include <Containers/EnumSet.h>
#include <Containers/LinkedList.h>

#include "Magnum.h"
#include "SceneGraph/AbstractObject.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Which transformation to cache in given feature

@see @ref scenegraph-caching, CachedTransformations,
    AbstractBasicFeature::setCachedTransformations(), AbstractBasicFeature::clean(),
    AbstractBasicFeature::cleanInverted()
@todo Provide also simpler representations from which could benefit
    other transformation implementations, as they won't need to
    e.g. create transformation matrix from quaternion?
 */
enum class CachedTransformation: UnsignedByte {
    /**
     * Absolute transformation is cached.
     *
     * If enabled, clean() is called when cleaning object.
     */
    Absolute = 1 << 0,

    /**
     * Inverted absolute transformation is cached.
     *
     * If enabled, cleanInverted() is called when cleaning object.
     */
    InvertedAbsolute = 1 << 1
};

/**
@brief Which transformations to cache in this feature

@see @ref scenegraph-caching, AbstractBasicFeature::setCachedTransformations(),
    AbstractBasicFeature::clean(), AbstractBasicFeature::cleanInverted()
*/
typedef Containers::EnumSet<CachedTransformation, UnsignedByte> CachedTransformations;

CORRADE_ENUMSET_OPERATORS(CachedTransformations)

/**
@brief Base for object features

Contained in Object, takes care of transformation caching. See @ref scenegraph
for introduction.

Uses Corrade::Containers::LinkedList for accessing holder object and sibling
features.

@section AbstractFeature-subclassing Subclassing

Feature is templated on dimension count and underlying transformation type, so
it can be used only on object having transformation with the same dimension
count and type.

@subsection AbstractFeature-subclassing-caching Caching transformations in features

Features can cache absolute transformation of the object instead of computing
it from scratch every time to achieve better performance. See
@ref scenegraph-caching for introduction.

In order to have caching, you must enable it first, because by default the
caching is disabled. You can enable it using setCachedTransformations() and
then implement corresponding cleaning function(s) -- either clean(),
cleanInverted() or both. Example:
@code
class CachingFeature: public SceneGraph::AbstractFeature3D {
    public:
        CachingFeature(SceneGraph::AbstractObject3D<>* object): SceneGraph::AbstractFeature3D(object) {
            setCachedTransformations(CachedTransformation::Absolute);
        }

    protected:
        void clean(const Matrix4& absoluteTransformationMatrix) override {
            absolutePosition = absoluteTransformationMatrix.translation();
        }

    private:
        Vector3 absolutePosition;
};
@endcode

Before using the cached value explicitly request object cleaning by calling
`object()->setClean()`.

@subsection AbstractFeature-subclassing-transformation Accessing object transformation

Features has by default access only to AbstractObject, which is base of Object
not depending on any particular transformation implementation. This has the
advantage that features doesn't have to be implemented for all possible
transformation implementations, thus preventing code duplication. However it
is impossible to transform the object using only pointer to AbstractObject.

The transformations have interfaces for common functionality, so the feature
can use that interface instead of being specialized for all relevant
transformation implementations. Using small trick we are able to get pointer
to both AbstractObject and needed transformation from one constructor
parameter:
@code
class TransformingFeature: public SceneGraph::AbstractFeature3D {
    public:
        template<class T> TransformingFeature(SceneGraph::Object<T>* object):
            SceneGraph::AbstractFeature3D<>(object), transformation(object) {}

    private:
        SceneGraph::AbstractTranslationRotation3D<>* transformation;
};
@endcode
If we take for example @ref Object "Object<MatrixTransformation3D<>>", it is
derived from @ref AbstractObject "AbstractObject3D<>" and
@ref MatrixTransformation3D "MatrixTransformation3D<>", which is derived from
@ref AbstractTranslationRotationScaling3D "AbstractTranslationRotationScaling3D<>",
which is derived from
@ref AbstractTranslationRotation3D "AbstractTranslationRotation3D<>",
which is automatically extracted from the pointer in our constructor.

@section AbstractFeature-explicit-specializations Explicit template specializations

The following specialization are explicitly compiled into %SceneGraph library.
For other specializations (e.g. using Double type) you have to use
AbstractFeature.hpp implementation file to avoid linker errors. See also
@ref compilation-speedup-hpp for more information.

 - @ref AbstractBasicFeature "AbstractBasicFeature<2, Float>"
 - @ref AbstractBasicFeature "AbstractBasicFeature<3, Float>"

@see @ref AbstractFeature2D, @ref AbstractFeature3D
*/
template<UnsignedInt dimensions, class T> class MAGNUM_SCENEGRAPH_EXPORT AbstractBasicFeature
#ifndef DOXYGEN_GENERATING_OUTPUT
: private Containers::LinkedListItem<AbstractBasicFeature<dimensions, T>, AbstractObject<dimensions, T>>
#endif
{
    friend class Containers::LinkedList<AbstractBasicFeature<dimensions, T>>;
    friend class Containers::LinkedListItem<AbstractBasicFeature<dimensions, T>, AbstractObject<dimensions, T>>;
    template<class Transformation> friend class Object;

    public:
        /**
         * @brief Constructor
         * @param object    %Object holding this feature
         */
        explicit AbstractBasicFeature(AbstractObject<dimensions, T>* object);

        virtual ~AbstractBasicFeature() = 0;

        /** @brief %Object holding this feature */
        AbstractObject<dimensions, T>* object() {
            return Containers::LinkedListItem<AbstractBasicFeature<dimensions, T>, AbstractObject<dimensions, T>>::list();
        }

        /** @overload */
        const AbstractObject<dimensions, T>* object() const {
            return Containers::LinkedListItem<AbstractBasicFeature<dimensions, T>, AbstractObject<dimensions, T>>::list();
        }

        /** @brief Previous feature or `nullptr`, if this is first feature */
        AbstractBasicFeature<dimensions, T>* previousFeature() {
            return Containers::LinkedListItem<AbstractBasicFeature<dimensions, T>, AbstractObject<dimensions, T>>::previous();
        }

        /** @overload */
        const AbstractBasicFeature<dimensions, T>* previousFeature() const {
            return Containers::LinkedListItem<AbstractBasicFeature<dimensions, T>, AbstractObject<dimensions, T>>::previous();
        }

        /** @brief Next feature or `nullptr`, if this is last feature */
        AbstractBasicFeature<dimensions, T>* nextFeature() {
            return Containers::LinkedListItem<AbstractBasicFeature<dimensions, T>, AbstractObject<dimensions, T>>::next();
        }

        /** @overload */
        const AbstractBasicFeature<dimensions, T>* nextFeature() const {
            return Containers::LinkedListItem<AbstractBasicFeature<dimensions, T>, AbstractObject<dimensions, T>>::next();
        }

        /**
         * @{ @name Transformation caching
         *
         * See @ref scenegraph-caching for more information.
         */

        /**
         * @brief Which transformations are cached
         *
         * @see @ref scenegraph-caching, clean(), cleanInverted()
         */
        CachedTransformations cachedTransformations() const {
            return _cachedTransformations;
        }

    protected:
        /**
         * @brief Set transformations to be cached
         *
         * Based on which transformation types are enabled, clean() or
         * cleanInverted() is called when cleaning absolute object
         * transformation.
         *
         * Nothing is enabled by default.
         * @see @ref scenegraph-caching
         */
        void setCachedTransformations(CachedTransformations transformations) {
            _cachedTransformations = transformations;
        }

        /**
         * @brief Mark feature as dirty
         *
         * Reimplement only if you want to invalidate some external data when
         * object is marked as dirty. All expensive computations should be
         * done in clean() and cleanInverted().
         *
         * Default implementation does nothing.
         * @see @ref scenegraph-caching
         */
        virtual void markDirty();

        /**
         * @brief Clean data based on absolute transformation
         *
         * When object is cleaned and
         * @ref CachedTransformation "CachedTransformation::Absolute" is
         * enabled in setCachedTransformations(), this function is called to
         * recalculate data based on absolute object transformation.
         *
         * Default implementation does nothing.
         * @see @ref scenegraph-caching, cleanInverted()
         */
        virtual void clean(const typename DimensionTraits<dimensions, T>::MatrixType& absoluteTransformationMatrix);

        /**
         * @brief Clean data based on inverted absolute transformation
         *
         * When object is cleaned and
         * @ref CachedTransformation "CachedTransformation::InvertedAbsolute"
         * is enabled in setCachedTransformations(), this function is called
         * to recalculate data based on inverted absolute object
         * transformation.
         *
         * Default implementation does nothing.
         * @see @ref scenegraph-caching, clean()
         */
        virtual void cleanInverted(const typename DimensionTraits<dimensions, T>::MatrixType& invertedAbsoluteTransformationMatrix);

        /*@}*/

    private:
        CachedTransformations _cachedTransformations;
};

/**
@brief Base feature for two-dimensional float scenes

@see @ref AbstractFeature3D
*/
typedef AbstractBasicFeature<2, Float> AbstractFeature2D;

/**
@brief Base feature for three-dimensional float scenes

@see @ref AbstractFeature2D
*/
typedef AbstractBasicFeature<3, Float> AbstractFeature3D;

}}

#endif
