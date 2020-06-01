#ifndef Magnum_SceneGraph_AnimableGroup_h
#define Magnum_SceneGraph_AnimableGroup_h
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
 * @brief Class @ref Magnum::SceneGraph::AnimableGroup, alias @ref Magnum::SceneGraph::BasicAnimableGroup2D, @ref Magnum::SceneGraph::BasicAnimableGroup3D, typedef @ref Magnum::SceneGraph::AnimableGroup2D, @ref Magnum::SceneGraph::AnimableGroup3D
 */

#include "Magnum/SceneGraph/Animable.h"
#include "Magnum/SceneGraph/FeatureGroup.h"
#include "Magnum/SceneGraph/visibility.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Group of animables

See @ref Animable for more information.
@see @ref scenegraph, @ref BasicAnimableGroup2D, @ref BasicAnimableGroup3D,
    @ref AnimableGroup2D, @ref AnimableGroup3D
*/
template<UnsignedInt dimensions, class T> class AnimableGroup: public FeatureGroup<dimensions, Animable<dimensions, T>, T> {
    friend Animable<dimensions, T>;

    public:
        /**
         * @brief Constructor
         */
        explicit AnimableGroup(): _runningCount(0), wakeUp(false) {}

        /**
         * @brief Count of running animations
         *
         * @see @ref step()
         */
        std::size_t runningCount() const { return _runningCount; }

        /**
         * @brief Perform animation step
         * @param time      Absolute time (e.g. @ref Timeline::previousFrameTime())
         * @param delta     Time delta for current frame (e.g. @ref Timeline::previousFrameDuration())
         *
         * If there are no running animations the function does nothing.
         * @see @ref runningCount()
         */
        void step(Float time, Float delta);

    private:
        std::size_t _runningCount;
        bool wakeUp;
};

/**
@brief Animable group for two-dimensional scenes

Convenience alternative to @cpp AnimableGroup<2, T> @ce. See @ref Animable for
more information.
@see @ref AnimableGroup2D, @ref BasicAnimableGroup3D
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using BasicAnimableGroup2D = AnimableGroup<2, T>;
#endif

/**
@brief Animable group for two-dimensional float scenes

@see @ref AnimableGroup3D
*/
typedef BasicAnimableGroup2D<Float> AnimableGroup2D;

/**
@brief Animable group for three-dimensional scenes

Convenience alternative to @cpp AnimableGroup<3, T> @ce. See @ref Animable for
more information.
@see @ref AnimableGroup3D, @ref BasicAnimableGroup2D
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using BasicAnimableGroup3D = AnimableGroup<3, T>;
#endif

/**
@brief Animable group for three-dimensional float scenes

@see @ref AnimableGroup2D
*/
typedef BasicAnimableGroup3D<Float> AnimableGroup3D;

#if defined(CORRADE_TARGET_WINDOWS) && !(defined(CORRADE_TARGET_MINGW) && !defined(CORRADE_TARGET_CLANG))
extern template class MAGNUM_SCENEGRAPH_EXPORT AnimableGroup<2, Float>;
extern template class MAGNUM_SCENEGRAPH_EXPORT AnimableGroup<3, Float>;
#endif

}}

#endif
