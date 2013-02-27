#ifndef Magnum_SceneGraph_AnimableGroup_h
#define Magnum_SceneGraph_AnimableGroup_h
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
 * @brief Class Magnum::SceneGraph::AnimableGroup
 */

#include "FeatureGroup.h"

#include "magnumSceneGraphVisibility.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Group of animables

See Animable for more information.
@see @ref scenegraph, AnimableGroup2D, AnimableGroup3D
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<UnsignedInt dimensions, class T>
#else
template<UnsignedInt dimensions, class T = Float>
#endif
class MAGNUM_SCENEGRAPH_EXPORT AnimableGroup: public FeatureGroup<dimensions, Animable<dimensions, T>, T> {
    friend class Animable<dimensions, T>;

    public:
        /**
         * @brief Constructor
         */
        inline explicit AnimableGroup(): _runningCount(0), wakeUp(false) {}

        /**
         * @brief Count of running animations
         *
         * @see step()
         */
        inline std::size_t runningCount() const { return _runningCount; }

        /**
         * @brief Perform animation step
         * @param time      Absolute time (e.g. Timeline::previousFrameTime())
         * @param delta     Time delta for current frame (e.g. Timeline::previousFrameDuration())
         *
         * If there are no running animations the function does nothing.
         * @see runningCount()
         */
        void step(const Float time, const Float delta);

    private:
        std::size_t _runningCount;
        bool wakeUp;
};

#ifndef CORRADE_GCC46_COMPATIBILITY
/**
@brief Two-dimensional drawable

Convenience alternative to <tt>%AnimableGroup<2, T></tt>. See Animable for
more information.
@note Not available on GCC < 4.7. Use <tt>%AnimableGroup<2, T></tt> instead.
@see AnimableGroup3D
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T = Float>
#else
template<class T>
#endif
using AnimableGroup2D = AnimableGroup<2, T>;

/**
@brief Three-dimensional animable

Convenience alternative to <tt>%AnimableGroup<3, T></tt>. See Animable for
more information.
@note Not available on GCC < 4.7. Use <tt>%AnimableGroup<3, T></tt> instead.
@see AnimableGroup2D
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T = Float>
#else
template<class T>
#endif
using AnimableGroup3D = AnimableGroup<3, T>;
#endif

}}

#endif
