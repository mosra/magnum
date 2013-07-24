#ifndef Magnum_SceneGraph_AnimableGroup_h
#define Magnum_SceneGraph_AnimableGroup_h
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
 * @brief Class Magnum::SceneGraph::AnimableGroup, alias Magnum::SceneGraph::BasicAnimableGroup2D, Magnum::SceneGraph::BasicAnimableGroup3D, typedef Magnum::SceneGraph::AnimableGroup2D, Magnum::SceneGraph::AnimableGroup3D
 */

#include "FeatureGroup.h"

#include "magnumSceneGraphVisibility.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Group of animables

See Animable for more information.
@see @ref scenegraph, @ref BasicAnimableGroup2D, @ref BasicAnimableGroup3D,
    @ref AnimableGroup2D, @ref AnimableGroup3D
*/
template<UnsignedInt dimensions, class T> class MAGNUM_SCENEGRAPH_EXPORT AnimableGroup: public FeatureGroup<dimensions, Animable<dimensions, T>, T> {
    friend class Animable<dimensions, T>;

    public:
        /**
         * @brief Constructor
         */
        explicit AnimableGroup(): _runningCount(0), wakeUp(false) {}

        /**
         * @brief Count of running animations
         *
         * @see step()
         */
        std::size_t runningCount() const { return _runningCount; }

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
@brief %Animable group for two-dimensional scenes

Convenience alternative to <tt>%AnimableGroup<2, T></tt>. See Animable for
more information.
@note Not available on GCC < 4.7. Use <tt>%AnimableGroup<2, T></tt> instead.
@see @ref AnimableGroup2D, @ref BasicAnimableGroup3D
*/
template<class T> using BasicAnimableGroup2D = AnimableGroup<2, T>;
#endif

/**
@brief %Animable group for two-dimensional float scenes

@see @ref AnimableGroup3D
*/
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef BasicAnimableGroup2D<Float> AnimableGroup2D;
#else
typedef AnimableGroup<2, Float> AnimableGroup2D;
#endif

#ifndef CORRADE_GCC46_COMPATIBILITY
/**
@brief %Animable group for three-dimensional scenes

Convenience alternative to <tt>%AnimableGroup<3, T></tt>. See Animable for
more information.
@note Not available on GCC < 4.7. Use <tt>%AnimableGroup<3, T></tt> instead.
@see @ref AnimableGroup3D, @ref BasicAnimableGroup2D
*/
template<class T> using BasicAnimableGroup3D = AnimableGroup<3, T>;
#endif

/**
@brief %Animable group for three-dimensional float scenes

@see @ref AnimableGroup2D
*/
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef BasicAnimableGroup3D<Float> AnimableGroup3D;
#else
typedef AnimableGroup<3, Float> AnimableGroup3D;
#endif

}}

#endif
