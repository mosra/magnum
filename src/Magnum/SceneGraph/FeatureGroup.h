#ifndef Magnum_SceneGraph_FeatureGroup_h
#define Magnum_SceneGraph_FeatureGroup_h
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
 * @brief Class @ref Magnum::SceneGraph::AbstractFeatureGroup, @ref Magnum::SceneGraph::FeatureGroup, alias @ref Magnum::SceneGraph::BasicFeatureGroup2D, @ref Magnum::SceneGraph::BasicFeatureGroup3D, @ref Magnum::SceneGraph::FeatureGroup2D, @ref Magnum::SceneGraph::FeatureGroup3D
 */

#include <vector>
#include <Corrade/Utility/Assert.h>

#include "Magnum/SceneGraph/SceneGraph.h"
#include "Magnum/SceneGraph/visibility.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Base for group of features

See @ref FeatureGroup.
*/
template<UnsignedInt dimensions, class T> class AbstractFeatureGroup {
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

    private:
        template<UnsignedInt, class, class> friend class FeatureGroup;

        explicit AbstractFeatureGroup();
        virtual ~AbstractFeatureGroup();

        void add(AbstractFeature<dimensions, T>& feature);
        void remove(AbstractFeature<dimensions, T>& feature);

        std::vector<std::reference_wrapper<AbstractFeature<dimensions, T>>> _features;
};

/**
@brief Group of features

See @ref AbstractGroupedFeature for more information.
@see @ref scenegraph, @ref BasicFeatureGroup2D, @ref BasicFeatureGroup3D,
    @ref FeatureGroup2D, @ref FeatureGroup3D
*/
template<UnsignedInt dimensions, class Feature, class T> class FeatureGroup: public AbstractFeatureGroup<dimensions, T> {
    friend AbstractGroupedFeature<dimensions, Feature, T>;

    public:
        explicit FeatureGroup() = default;

        /**
         * @brief Destructor
         *
         * Removes all features belonging to this group, but not deletes them.
         */
        ~FeatureGroup();

        /** @brief Whether the group is empty */
        bool isEmpty() const {
            return AbstractFeatureGroup<dimensions, T>::_features.empty();
        }

        /** @brief Count of features in the group */
        std::size_t size() const {
            return AbstractFeatureGroup<dimensions, T>::_features.size();
        }

        /** @brief Feature at given index */
        Feature& operator[](std::size_t index) {
            return static_cast<Feature&>(AbstractFeatureGroup<dimensions, T>::_features[index].get());
        }

        /** @overload */
        const Feature& operator[](std::size_t index) const {
            return static_cast<Feature&>(AbstractFeatureGroup<dimensions, T>::_features[index].get());
        }

        /**
         * @brief Add a feature to the group
         * @return Reference to self (for method chaining)
         *
         * If the features is part of another group, it is removed from it.
         * @see @ref remove(), @ref AbstractGroupedFeature::AbstractGroupedFeature()
         */
        FeatureGroup<dimensions, Feature, T>& add(Feature& feature);

        /**
         * @brief Remove a feature from the group
         * @return Reference to self (for method chaining)
         *
         * The feature must be part of the group.
         * @see @ref add()
         */
        FeatureGroup<dimensions, Feature, T>& remove(Feature& feature);
};

/**
@brief Base feature group for two-dimensional scenes

Convenience alternative to @cpp FeatureGroup<2, Feature, T> @ce. See
@ref AbstractGroupedFeature for more information.
@see @ref FeatureGroup2D, @ref BasicFeatureGroup3D
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class Feature, class T> using BasicFeatureGroup2D = FeatureGroup<2, Feature, T>;
#endif

/**
@brief Base feature group for two-dimensional float scenes

Convenience alternative to @cpp BasicFeatureGroup2D<Feature, Float> @ce. See
@ref AbstractGroupedFeature for more information.
@see @ref FeatureGroup3D
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class Feature> using FeatureGroup2D = BasicFeatureGroup2D<Feature, Float>;
#endif

/**
@brief Base feature group for three-dimensional scenes

Convenience alternative to @cpp FeatureGroup<3, Feature, T> @ce. See
@ref AbstractGroupedFeature for more information.
@see @ref FeatureGroup3D, @ref BasicFeatureGroup2D
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class Feature, class T> using BasicFeatureGroup3D = FeatureGroup<3, Feature, T>;
#endif

/**
@brief Base feature group for three-dimensional float scenes

Convenience alternative to @cpp BasicFeatureGroup3D<Feature, Float> @ce. See
@ref AbstractGroupedFeature for more information.
@see @ref FeatureGroup2D
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class Feature> using FeatureGroup3D = BasicFeatureGroup3D<Feature, Float>;
#endif

template<UnsignedInt dimensions, class Feature, class T> FeatureGroup<dimensions, Feature, T>::~FeatureGroup() {
    for(auto i: AbstractFeatureGroup<dimensions, T>::_features) static_cast<Feature&>(i.get())._group = nullptr;
}

template<UnsignedInt dimensions, class Feature, class T> FeatureGroup<dimensions, Feature, T>& FeatureGroup<dimensions, Feature, T>::add(Feature& feature) {
    /* Remove from previous group */
    if(feature._group)
        feature._group->remove(feature);

    /* Crossreference the feature and group together */
    AbstractFeatureGroup<dimensions, T>::add(feature);
    feature._group = this;
    return *this;
}

template<UnsignedInt dimensions, class Feature, class T> FeatureGroup<dimensions, Feature, T>& FeatureGroup<dimensions, Feature, T>::remove(Feature& feature) {
    CORRADE_ASSERT(feature._group == this,
        "SceneGraph::AbstractFeatureGroup::remove(): feature is not part of this group", *this);

    AbstractFeatureGroup<dimensions, T>::remove(feature);
    feature._group = nullptr;
    return *this;
}

#if defined(CORRADE_TARGET_WINDOWS) && !(defined(CORRADE_TARGET_MINGW) && !defined(CORRADE_TARGET_CLANG))
extern template class MAGNUM_SCENEGRAPH_EXPORT AbstractFeatureGroup<2, Float>;
extern template class MAGNUM_SCENEGRAPH_EXPORT AbstractFeatureGroup<3, Float>;
#endif

}}

#endif
