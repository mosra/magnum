#ifndef Magnum_SceneGraph_AbstractGroupedFeature_h
#define Magnum_SceneGraph_AbstractGroupedFeature_h
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
 * @brief Class @ref Magnum::SceneGraph::AbstractGroupedFeature, alias @ref Magnum::SceneGraph::AbstractBasicGroupedFeature2D, @ref Magnum::SceneGraph::AbstractBasicGroupedFeature3D, @ref Magnum::SceneGraph::AbstractGroupedFeature2D, @ref Magnum::SceneGraph::AbstractGroupedFeature3D
 */

#include <vector>

#include "Magnum/SceneGraph/AbstractFeature.h"
#include "Magnum/SceneGraph/FeatureGroup.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Base for grouped features

Used together with @ref FeatureGroup.

@section SceneGraph-AbstractGroupedFeature-subclassing Subclassing

Usage is via subclassing the feature using [CRTP](http://en.wikipedia.org/wiki/Curiously_recurring_template_pattern)
and @cpp typedef @ce'ing @ref FeatureGroup to accept only given type, e.g.:

@snippet MagnumSceneGraph.cpp AbstractGroupedFeature-subclassing

@section SceneGraph-AbstractGroupedFeature-explicit-specializations Explicit template specializations

The following specializations are explicitly compiled into @ref SceneGraph
library. For other specializations (e.g. using @ref Magnum::Double "Double" type)
you have to use @ref FeatureGroup.hpp implementation file to avoid linker
errors. See also @ref compilation-speedup-hpp for more information.

-   @ref FeatureGroup2D
-   @ref FeatureGroup3D

@see @ref scenegraph, @ref AbstractBasicGroupedFeature2D,
    @ref AbstractBasicGroupedFeature3D, @ref AbstractGroupedFeature2D,
    @ref AbstractGroupedFeature3D, @ref FeatureGroup
*/
template<UnsignedInt dimensions, class Derived, class T> class AbstractGroupedFeature: public AbstractFeature<dimensions, T> {
    friend FeatureGroup<dimensions, Derived, T>;

    public:
        /**
         * @brief Constructor
         * @param object    Object this feature belongs to
         * @param group     Group this feature belongs to
         *
         * Adds the feature to the object and to group, if specified.
         * @see @ref FeatureGroup::add()
         */
        explicit AbstractGroupedFeature(AbstractObject<dimensions, T>& object, FeatureGroup<dimensions, Derived, T>* group = nullptr): AbstractFeature<dimensions, T>(object), _group(nullptr) {
            if(group) group->add(static_cast<Derived&>(*this));
        }

        /**
         * @brief Destructor
         *
         * Removes the feature from object and from group, if it belongs to
         * any.
         */
        ~AbstractGroupedFeature() {
            if(_group) _group->remove(static_cast<Derived&>(*this));
        }

        /** @brief Group this feature belongs to */
        FeatureGroup<dimensions, Derived, T>* group() {
            return _group;
        }

        /** @overload */
        const FeatureGroup<dimensions, Derived, T>* group() const {
            return _group;
        }

    private:
        FeatureGroup<dimensions, Derived, T>* _group;
};

/**
@brief Base grouped feature for two-dimensional scenes

Convenience alternative to @cpp AbstractGroupedFeature<2, Derived, T> @ce. See
@ref AbstractGroupedFeature for more information.
@see @ref AbstractGroupedFeature2D, @ref AbstractBasicGroupedFeature3D
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class Derived, class T> using AbstractBasicGroupedFeature2D = AbstractGroupedFeature<2, Derived, T>;
#endif

/**
@brief Base grouped feature for two-dimensional float scenes

Convenience alternative to @cpp AbstractBasicGroupedFeature2D<Derived, Float> @ce.
See @ref AbstractGroupedFeature for more information.
@see @ref AbstractGroupedFeature3D
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class Derived> using AbstractGroupedFeature2D = AbstractBasicGroupedFeature2D<Derived, Float>;
#endif

/**
@brief Base grouped feature for three-dimensional scenes

Convenience alternative to @cpp AbstractGroupedFeature<3, Derived, T> @ce. See
@ref AbstractGroupedFeature for more information.
@see @ref AbstractGroupedFeature3D, @ref AbstractBasicGroupedFeature2D
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class Derived, class T> using AbstractBasicGroupedFeature3D = AbstractGroupedFeature<3, Derived, T>;
#endif

/**
@brief Base grouped feature for three-dimensional float scenes

Convenience alternative to @cpp AbstractBasicGroupedFeature3D<Derived, Float> @ce.
See @ref AbstractGroupedFeature for more information.
@see @ref AbstractGroupedFeature2D
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class Derived> using AbstractGroupedFeature3D = AbstractBasicGroupedFeature3D<Derived, Float>;
#endif

}}

#endif
