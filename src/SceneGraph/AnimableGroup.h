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
 * @brief Class Magnum::SceneGraph::BasicAnimableGroup, typedef Magnum::SceneGraph::AnimableGroup2D, Magnum::SceneGraph::AnimableGroup3D
 */

#include "FeatureGroup.h"

#include "magnumSceneGraphVisibility.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Group of animables

See BasicAnimable for more information.
@see @ref AnimableGroup2D, @ref AnimableGroup3D, @ref scenegraph
*/
template<UnsignedInt dimensions, class T> class MAGNUM_SCENEGRAPH_EXPORT BasicAnimableGroup: public BasicFeatureGroup<dimensions, BasicAnimable<dimensions, T>, T> {
    friend class BasicAnimable<dimensions, T>;

    public:
        /**
         * @brief Constructor
         */
        explicit BasicAnimableGroup(): _runningCount(0), wakeUp(false) {}

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

/**
@brief Animable group for two-dimensional float scenes

@see @ref AnimableGroup3D
*/
typedef BasicAnimableGroup<2, Float> AnimableGroup2D;

/**
@brief Animable group for three-dimensional float scenes

@see @ref AnimableGroup2D
*/
typedef BasicAnimableGroup<3, Float> AnimableGroup3D;

}}

#endif
