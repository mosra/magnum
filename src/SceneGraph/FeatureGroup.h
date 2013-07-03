#ifndef Magnum_SceneGraph_FeatureGroup_h
#define Magnum_SceneGraph_FeatureGroup_h
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
 * @brief Class Magnum::SceneGraph::AbstractBasicFeatureGroup, Magnum::SceneGraph::BasicFeatureGroup, alias Magnum::SceneGraph::FeatureGroup2D, Magnum::SceneGraph::FeatureGroup3D
 */

#include <vector>
#include <Utility/Assert.h>

#include "SceneGraph/SceneGraph.h"
#include "SceneGraph/magnumSceneGraphVisibility.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Base for group of features

See BasicFeatureGroup.
*/
template<UnsignedInt dimensions, class T> class MAGNUM_SCENEGRAPH_EXPORT AbstractBasicFeatureGroup {
    template<UnsignedInt, class, class> friend class BasicFeatureGroup;

    explicit AbstractBasicFeatureGroup();
    virtual ~AbstractBasicFeatureGroup();

    void add(AbstractBasicFeature<dimensions, T>* feature);
    void remove(AbstractBasicFeature<dimensions, T>* feature);

    std::vector<AbstractBasicFeature<dimensions, T>*> features;
};

/**
@brief Group of features

See AbstractBasicGroupedFeature for more information.
@see @ref FeatureGroup2D, @ref FeatureGroup3D, @ref scenegraph
*/
template<UnsignedInt dimensions, class Feature, class T> class BasicFeatureGroup: public AbstractBasicFeatureGroup<dimensions, T> {
    friend class AbstractBasicGroupedFeature<dimensions, Feature, T>;

    public:
        explicit BasicFeatureGroup() = default;

        /**
         * @brief Destructor
         *
         * Removes all features belonging to this group, but not deletes them.
         */
        ~BasicFeatureGroup();

        /** @brief Whether the group is empty */
        bool isEmpty() const { return this->features.empty(); }

        /** @brief Count of features in the group */
        std::size_t size() const { return this->features.size(); }

        /** @brief Feature at given index */
        Feature* operator[](std::size_t index) {
            return static_cast<Feature*>(this->features[index]);
        }

        /** @overload */
        const Feature* operator[](std::size_t index) const {
            return static_cast<Feature*>(this->features[index]);
        }

        /**
         * @brief Add feature to the group
         * @return Pointer to self (for method chaining)
         *
         * If the features is part of another group, it is removed from it.
         * @see remove(), AbstractBasicGroupedFeature::AbstractBasicGroupedFeature()
         */
        BasicFeatureGroup<dimensions, Feature, T>* add(Feature* feature);

        /**
         * @brief Remove feature from the group
         * @return Pointer to self (for method chaining)
         *
         * The feature must be part of the group.
         * @see add()
         */
        BasicFeatureGroup<dimensions, Feature, T>* remove(Feature* feature);
};

#ifndef CORRADE_GCC46_COMPATIBILITY
/**
@brief Base feature for two-dimensional float scenes

Convenience alternative to <tt>%BasicFeatureGroup<2, Feature, Float></tt>.
@note Not available on GCC < 4.7. Use <tt>%BasicFeatureGroup<2, Feature, Float></tt>
    instead.
@see @ref FeatureGroup3D
*/
template<class Feature> using FeatureGroup2D = BasicFeatureGroup<2, Feature, Float>;

/**
@brief Base feature for three-dimensional float scenes

Convenience alternative to <tt>%BasicFeatureGroup<3, Feature, Float></tt>.
@note Not available on GCC < 4.7. Use <tt>%FeatureGroup<3, Feature, Float></tt>
    instead.
@see @ref FeatureGroup2D
*/
template<class Feature> using FeatureGroup3D = BasicFeatureGroup<3, Feature, Float>;
#endif

template<UnsignedInt dimensions, class Feature, class T> BasicFeatureGroup<dimensions, Feature, T>::~BasicFeatureGroup() {
    for(auto i: this->features) static_cast<Feature*>(i)->_group = nullptr;
}

template<UnsignedInt dimensions, class Feature, class T> BasicFeatureGroup<dimensions, Feature, T>* BasicFeatureGroup<dimensions, Feature, T>::add(Feature* feature) {
    /* Remove from previous group */
    if(feature->_group)
        feature->_group->remove(feature);

    /* Crossreference the feature and group together */
    AbstractBasicFeatureGroup<dimensions, T>::add(feature);
    feature->_group = this;
    return this;
}

template<UnsignedInt dimensions, class Feature, class T> BasicFeatureGroup<dimensions, Feature, T>* BasicFeatureGroup<dimensions, Feature, T>::remove(Feature* feature) {
    CORRADE_ASSERT(feature->_group == this,
        "SceneGraph::AbstractBasicFeatureGroup::remove(): feature is not part of this group", this);

    AbstractBasicFeatureGroup<dimensions, T>::remove(feature);
    feature->_group = nullptr;
    return this;
}

}}

#endif
