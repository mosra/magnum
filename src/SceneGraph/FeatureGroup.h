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
 * @brief Class Magnum::SceneGraph::FeatureGroup, alias Magnum::SceneGraph::FeatureGroup2D, Magnum::SceneGraph::FeatureGroup3D
 */

#include <vector>
#include <Utility/Assert.h>

#include "SceneGraph/SceneGraph.h"
#include "SceneGraph/magnumSceneGraphVisibility.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Base for group of features

See FeatureGroup.
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<UnsignedInt dimensions, class T>
#else
template<UnsignedInt dimensions, class T = Float>
#endif
class MAGNUM_SCENEGRAPH_EXPORT AbstractFeatureGroup {
    template<UnsignedInt, class, class> friend class FeatureGroup;

    explicit AbstractFeatureGroup();
    ~AbstractFeatureGroup();

    void add(AbstractFeature<dimensions, T>* feature);
    void remove(AbstractFeature<dimensions, T>* feature);

    std::vector<AbstractFeature<dimensions, T>*> features;
};

/**
@brief Group of features

See AbstractGroupedFeature for more information.
@see @ref scenegraph, FeatureGroup2D, FeatureGroup3D
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<UnsignedInt dimensions, class Feature, class T>
#else
template<UnsignedInt dimensions, class Feature, class T = Float>
#endif
class FeatureGroup: public AbstractFeatureGroup<dimensions, T> {
    friend class AbstractGroupedFeature<dimensions, Feature, T>;

    public:
        explicit FeatureGroup() = default;

        /**
         * @brief Destructor
         *
         * Removes all features belonging to this group, but not deletes them.
         */
        virtual ~FeatureGroup();

        /** @brief Whether the group is empty */
        inline bool isEmpty() const { return this->features.empty(); }

        /** @brief Count of features in the group */
        inline std::size_t size() const { return this->features.size(); }

        /** @brief Feature at given index */
        inline Feature* operator[](std::size_t index) {
            return static_cast<Feature*>(this->features[index]);
        }

        /** @overload */
        inline const Feature* operator[](std::size_t index) const {
            return static_cast<Feature*>(this->features[index]);
        }

        /**
         * @brief Add feature to the group
         * @return Pointer to self (for method chaining)
         *
         * If the features is part of another group, it is removed from it.
         * @see remove(), AbstractGroupedFeature::AbstractGroupedFeature()
         */
        FeatureGroup<dimensions, Feature, T>* add(Feature* feature);

        /**
         * @brief Remove feature from the group
         * @return Pointer to self (for method chaining)
         *
         * The feature must be part of the group.
         * @see add()
         */
        FeatureGroup<dimensions, Feature, T>* remove(Feature* feature);
};

#ifndef CORRADE_GCC46_COMPATIBILITY
/**
@brief Base for two-dimensional object features

Convenience alternative to <tt>%FeatureGroup<2, Feature, T></tt>. See
AbstractGroupedFeature for more information.
@note Not available on GCC < 4.7. Use <tt>%FeatureGroup<2, Feature, T></tt>
    instead.
@see FeatureGroup3D
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class Feature, class T = Float>
#else
template<class Feature, class T>
#endif
using FeatureGroup2D = FeatureGroup<2, Feature, T>;

/**
@brief Base for three-dimensional object features

Convenience alternative to <tt>%FeatureGroup<3, Feature, T></tt>. See
AbstractGroupedFeature for more information.
@note Not available on GCC < 4.7. Use <tt>%FeatureGroup<3, Feature, T></tt>
    instead.
@see FeatureGroup2D
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class Feature, class T = Float>
#else
template<class Feature, class T>
#endif
using FeatureGroup3D = FeatureGroup<3, Feature, T>;
#endif

template<UnsignedInt dimensions, class Feature, class T> FeatureGroup<dimensions, Feature, T>::~FeatureGroup() {
    for(auto i: this->features) static_cast<Feature*>(i)->_group = nullptr;
}

template<UnsignedInt dimensions, class Feature, class T> FeatureGroup<dimensions, Feature, T>* FeatureGroup<dimensions, Feature, T>::add(Feature* feature) {
    /* Remove from previous group */
    if(feature->_group)
        feature->_group->remove(feature);

    /* Crossreference the feature and group together */
    AbstractFeatureGroup<dimensions, T>::add(feature);
    feature->_group = this;
    return this;
}

template<UnsignedInt dimensions, class Feature, class T> FeatureGroup<dimensions, Feature, T>* FeatureGroup<dimensions, Feature, T>::remove(Feature* feature) {
    CORRADE_ASSERT(feature->_group == this,
        "SceneGraph::AbstractFeatureGroup::remove(): feature is not part of this group", this);

    AbstractFeatureGroup<dimensions, T>::remove(feature);
    feature->_group = nullptr;
    return this;
}

}}

#endif
