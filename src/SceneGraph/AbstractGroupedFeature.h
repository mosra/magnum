#ifndef Magnum_SceneGraph_AbstractGroupedFeature_h
#define Magnum_SceneGraph_AbstractGroupedFeature_h
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
 * @brief Class Magnum::SceneGraph::AbstractGroupedFeature, Magnum::SceneGraph::FeatureGroup, alias Magnum::SceneGraph::AbstractGroupedFeature2D, Magnum::SceneGraph::AbstractGroupedFeature3D, Magnum::SceneGraph::FeatureGroup2D, Magnum::SceneGraph::FeatureGroup3D
 */

#include <algorithm>
#include <vector>

#include "AbstractFeature.h"

namespace Magnum { namespace SceneGraph {

template<std::uint8_t, class, class> class FeatureGroup;

/**
@brief Base for grouped features

Used together with FeatureGroup.

@section AbstractGroupedFeature-subclassing Subclassing

Usage is via subclassing the feature using [CRTP](http://en.wikipedia.org/wiki/Curiously_recurring_template_pattern)
and typedef'ing FeatureGroup to accept only given type, e.g.:
@code
class Drawable: public SceneGraph::AbstractGroupedFeature3D<Drawable> {
    // ...
};

typedef SceneGraph::FeatureGroup3D<Drawable> DrawableGroup;
@endcode

@see AbstractGroupedFeature2D, AbstractGroupedFeature3D, FeatureGroup,
    FeatureGroup2D, FeatureGroup3D
*/
template<std::uint8_t dimensions, class Derived, class T = GLfloat> class AbstractGroupedFeature: public AbstractFeature<dimensions, T> {
    friend class FeatureGroup<dimensions, Derived, T>;

    public:
        /**
         * @brief Constructor
         * @param object    %Object this feature belongs to
         * @param group     Group this feature belongs to
         *
         * Adds the feature to the object and to group, if specified.
         * @see FeatureGroup::add(), FeatureGroup::remove()
         */
        inline AbstractGroupedFeature(AbstractObject<dimensions, T>* object, FeatureGroup<dimensions, Derived, T>* group = nullptr): AbstractFeature<dimensions, T>(object), _group(nullptr) {
            if(group) group->add(static_cast<Derived*>(this));
        }

        /**
         * @brief Destructor
         *
         * Removes the feature from object and from group, if it belongs to
         * any.
         */
        inline ~AbstractGroupedFeature() {
            if(_group) _group->remove(static_cast<Derived*>(this));
        }

        /** @brief Group this feature belongs to */
        inline FeatureGroup<dimensions, Derived, T>* group() {
            return _group;
        }

        /** @overload */
        inline const FeatureGroup<dimensions, Derived, T>* group() const {
            return _group;
        }

    private:
        FeatureGroup<dimensions, Derived, T>* _group;
};

/**
@brief Base for two-dimensional grouped features

Convenience alternative to <tt>%AbstractGroupedFeature<2, Derived, T></tt>. See
AbstractGroupedFeature for more information.
@see AbstractGroupedFeature3D
@note Not available on GCC < 4.7. Use <tt>%AbstractGroupedFeature<2, Derived, T></tt>
    instead.
@todoc Remove workaround when Doxygen supports alias template
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_GCC46_COMPATIBILITY
template<class Derived, class T = GLfloat> using AbstractGroupedFeature2D = AbstractGroupedFeature<2, Derived, T>;
#endif
#else
typedef AbstractGroupedFeature<2, Derived, T = GLfloat> AbstractGroupedFeature2D;
#endif

/**
@brief Base for three-dimensional grouped features

Convenience alternative to <tt>%AbstractGroupedFeature<3, Derived, T></tt>. See
AbstractGroupedFeature for more information.
@see AbstractGroupedFeature2D
@note Not available on GCC < 4.7. Use <tt>%AbstractGroupedFeature<3, Derived, T></tt>
    instead.
@todoc Remove workaround when Doxygen supports alias template
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_GCC46_COMPATIBILITY
template<class Derived, class T = GLfloat> using AbstractGroupedFeature3D = AbstractGroupedFeature<3, Derived, T>;
#endif
#else
typedef AbstractGroupedFeature<3, Derived, T = GLfloat> AbstractGroupedFeature3D;
#endif

/**
@brief Group of features

See AbstractGroupedFeature for more information.
@see FeatureGroup2D, FeatureGroup3D
*/
template<std::uint8_t dimensions, class Feature, class T = GLfloat> class FeatureGroup {
    friend class AbstractGroupedFeature<dimensions, Feature, T>;

    public:
        /**
         * @brief Destructor
         *
         * Deletes all features belogning to this group.
         */
        inline virtual ~FeatureGroup() {
            for(auto i: features) {
                i->_group = nullptr;
                delete i;
            }
        }

        /** @brief Whether the group is empty */
        inline bool isEmpty() const { return features.empty(); }

        /** @brief Count of features in the group */
        inline std::size_t size() const { return features.size(); }

        /** @brief Feature at given index */
        inline Feature* operator[](std::size_t index) {
            return features[index];
        }

        /** @overload */
        inline const Feature* operator[](std::size_t index) const {
            return features[index];
        }

        /**
         * @brief Add feature to the group
         *
         * If the features is part of another group, it is removed from it.
         */
        void add(Feature* feature) {
            /** @todo Assert the same scene for all items? -- can't easily
                watch when feature object is removed from hierarchy */

            /* Remove from previous group */
            if(feature->_group)
                feature->_group->remove(feature);

            /* Crossreference the feature and group together */
            features.push_back(feature);
            feature->_group = this;
        }

        /**
         * @brief Remove feature from the group
         *
         * The feature must be part of the group.
         */
        void remove(Feature* feature) {
            CORRADE_ASSERT(feature->_group == this,
                "SceneGraph::AbstractFeatureGroup::remove(): feature is not part of this group", );

            /* Remove the feature and reset group pointer */
            features.erase(std::find(features.begin(), features.end(), feature));
            feature->_group = nullptr;
        }

    private:
        std::vector<Feature*> features;
};

/**
@brief Base for two-dimensional object features

Convenience alternative to <tt>%FeatureGroup<2, Feature, T></tt>. See
AbstractGroupedFeature for more information.
@note Not available on GCC < 4.7. Use <tt>%FeatureGroup<2, Feature, T></tt>
    instead.
@see FeatureGroup3D
@todoc Remove workaround when Doxygen supports alias template
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_GCC46_COMPATIBILITY
template<class Feature, class T = GLfloat> using FeatureGroup2D = FeatureGroup<2, Feature, T>;
#endif
#else
typedef FeatureGroup<2, Feature, T = GLfloat> FeatureGroup2D;
#endif

/**
@brief Base for three-dimensional object features

Convenience alternative to <tt>%FeatureGroup<3, Feature, T></tt>. See
AbstractGroupedFeature for more information.
@note Not available on GCC < 4.7. Use <tt>%FeatureGroup<3, Feature, T></tt>
    instead.
@see FeatureGroup2D
@todoc Remove workaround when Doxygen supports alias template
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_GCC46_COMPATIBILITY
template<class Feature, class T = GLfloat> using FeatureGroup3D = FeatureGroup<3, Feature, T>;
#endif
#else
typedef FeatureGroup<3, Feature, T = GLfloat> FeatureGroup3D;
#endif

}}

#endif
