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
template<std::uint8_t dimensions, class T>
#else
template<std::uint8_t dimensions, class T = GLfloat>
#endif
class MAGNUM_SCENEGRAPH_EXPORT AnimableGroup: public FeatureGroup<dimensions, Animable<dimensions, T>, T> {
    friend class Animable<dimensions, T>;

    public:
        /**
         * @brief Constructor
         */
        inline AnimableGroup(): _runningCount(0), wakeUp(false) {}

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
        void step(const GLfloat time, const GLfloat delta);

    private:
        std::size_t _runningCount;
        bool wakeUp;
};

/**
@brief Two-dimensional drawable

Convenience alternative to <tt>%AnimableGroup<2, T></tt>. See Animable for
more information.
@note Not available on GCC < 4.7. Use <tt>%AnimableGroup<2, T></tt> instead.
@see AnimableGroup3D
@todoc Remove workaround when Doxygen supports alias template
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class T = GLfloat> using AnimableGroup2D = AnimableGroup<2, T>;
#endif
#else
typedef AnimableGroup<2, T = GLfloat> AnimableGroup2D;
#endif

/**
@brief Three-dimensional animable

Convenience alternative to <tt>%AnimableGroup<3, T></tt>. See Animable for
more information.
@note Not available on GCC < 4.7. Use <tt>%AnimableGroup<3, T></tt> instead.
@see AnimableGroup2D
@todoc Remove workaround when Doxygen supports alias template
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class T = GLfloat> using AnimableGroup3D = AnimableGroup<3, T>;
#endif
#else
typedef AnimableGroup<3, T = GLfloat> AnimableGroup3D;
#endif

}}

#endif
