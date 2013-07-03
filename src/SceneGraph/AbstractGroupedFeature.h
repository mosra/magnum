#ifndef Magnum_SceneGraph_AbstractGroupedFeature_h
#define Magnum_SceneGraph_AbstractGroupedFeature_h
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
 * @brief Class Magnum::SceneGraph::AbstractBasicGroupedFeature, alias Magnum::SceneGraph::AbstractGroupedFeature2D, Magnum::SceneGraph::AbstractGroupedFeature3D
 */

#include <vector>

#include "AbstractFeature.h"
#include "FeatureGroup.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Base for grouped features

Used together with BasicFeatureGroup.

@section AbstractGroupedFeature-subclassing Subclassing

Usage is via subclassing the feature using [CRTP](http://en.wikipedia.org/wiki/Curiously_recurring_template_pattern)
and typedef'ing BasicFeatureGroup to accept only given type, e.g.:
@code
class Drawable: public SceneGraph::AbstractGroupedFeature3D<Drawable> {
    // ...
};

typedef SceneGraph::FeatureGroup3D<Drawable> DrawableGroup;
@endcode

@section AbstractGroupedFeature-explicit-specializations Explicit template specializations

The following specialization are explicitly compiled into %SceneGraph library.
For other specializations (e.g. using Double type) you have to use
AbstractGroupedFeature.hpp implementation file to avoid linker errors. See also
@ref compilation-speedup-hpp for more information.

 - @ref AbstractBasicFeatureGroup "AbstractBasicFeatureGroup<2, Float>"
 - @ref AbstractBasicFeatureGroup "AbstractBasicFeatureGroup<3, Float>"

@see @ref AbstractGroupedFeature2D, @ref AbstractGroupedFeature3D,
    @ref scenegraph, @ref BasicFeatureGroup, @ref FeatureGroup2D,
    @ref FeatureGroup3D
*/
template<UnsignedInt dimensions, class Derived, class T> class AbstractBasicGroupedFeature: public AbstractBasicFeature<dimensions, T> {
    friend class BasicFeatureGroup<dimensions, Derived, T>;

    public:
        /**
         * @brief Constructor
         * @param object    %Object this feature belongs to
         * @param group     Group this feature belongs to
         *
         * Adds the feature to the object and to group, if specified.
         * @see FeatureGroup::add()
         */
        explicit AbstractBasicGroupedFeature(AbstractBasicObject<dimensions, T>* object, BasicFeatureGroup<dimensions, Derived, T>* group = nullptr): AbstractBasicFeature<dimensions, T>(object), _group(nullptr) {
            if(group) group->add(static_cast<Derived*>(this));
        }

        /**
         * @brief Destructor
         *
         * Removes the feature from object and from group, if it belongs to
         * any.
         */
        ~AbstractBasicGroupedFeature() {
            if(_group) _group->remove(static_cast<Derived*>(this));
        }

        /** @brief Group this feature belongs to */
        BasicFeatureGroup<dimensions, Derived, T>* group() {
            return _group;
        }

        /** @overload */
        const BasicFeatureGroup<dimensions, Derived, T>* group() const {
            return _group;
        }

    private:
        BasicFeatureGroup<dimensions, Derived, T>* _group;
};

#ifndef CORRADE_GCC46_COMPATIBILITY
/**
@brief Base grouped feature for two-dimensional float scenes

Convenience alternative to <tt>%AbstractBasicGroupedFeature<2, Derived, Float></tt>.
@note Not available on GCC < 4.7. Use <tt>%AbstractBasicGroupedFeature<2, Derived, Float></tt>
    instead.
@see @ref AbstractGroupedFeature3D
*/
template<class Derived> using AbstractGroupedFeature2D = AbstractBasicGroupedFeature<2, Derived, Float>;

/**
@brief Base for three-dimensional grouped features

Convenience alternative to <tt>%AbstractBasicGroupedFeature<3, Derived, Float></tt>.
@note Not available on GCC < 4.7. Use <tt>%AbstractBasicGroupedFeature<3, Derived, Float></tt>
    instead.
@see @ref AbstractGroupedFeature2D
*/
template<class Derived> using AbstractGroupedFeature3D = AbstractBasicGroupedFeature<3, Derived, Float>;
#endif

}}

#endif
