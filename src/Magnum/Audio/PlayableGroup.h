#ifndef Magnum_Audio_PlayableGroup_h
#define Magnum_Audio_PlayableGroup_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2015 Jonathan Hale <squareys@googlemail.com>

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
 * @brief Class @ref Magnum::Audio::PlayableGroup, typedef @ref Magnum::Audio::PlayableGroup2D, @ref Magnum::Audio::PlayableGroup3D
 */

#include <functional>
#include <string>
#include <vector>

#include <Magnum/SceneGraph/AbstractObject.h>
#include <Magnum/SceneGraph/SceneGraph.h>
#include <Magnum/SceneGraph/FeatureGroup.h>

#include "Magnum/Audio/Audio.h"
#include "Magnum/Audio/Playable.h"
#include "Magnum/Audio/Source.h"
#include "Magnum/Audio/visibility.h"

namespace Magnum { namespace Audio {

/**
@brief PlayableGroup

Manages @ref Audio::Playable features and provides means of setting gain or
transformation of a group of Playables, aswell as the ability of playing,
pausing, stopping or cleaning all sources of all Playables.

## Usage

@code
Object3D object;
Source source;
PlayableGroup3D group;
Playable3D playable{object, source, &group};

// ...

object.translate(offset);

// ... and every frame, update the sources positions:
group.setClean();
@endcode

For two dimensional scenes simply replace `3D` with `2D`. When using a
@ref Listener, prefer @ref Listener::update() over
@ref PlayableGroup::setClean().

-   @ref PlayableGroup2D
-   @ref PlayableGroup3D

@see @ref Playable, @ref SceneGraph::FeatureGroup, @ref Listener
*/
template<UnsignedInt dimensions> class PlayableGroup: public SceneGraph::FeatureGroup<dimensions, Playable<dimensions>, Float> {
    friend Playable<dimensions>;

    public:

        /** @brief Constructor */
        explicit PlayableGroup():
            SceneGraph::FeatureGroup<dimensions, Playable<dimensions>, Float>(),
            _gain{1.0f}
        {}

        /**
         * @brief Play all sound sources in this group
         * @return Reference to self (for method chaining)
         * @see @ref Source::play()
         */
        PlayableGroup<dimensions>& play() {
            Source::play(sources());
            return *this;
        }

        /**
         * @brief Pause all sound sources in this group
         * @return Reference to self (for method chaining)
         * @see @ref Source::pause()
         */
        PlayableGroup& pause() {
            Source::stop(sources());
            return *this;
        }

        /**
         * @brief Stop all sound sources in this group
         * @return Reference to self (for method chaining)
         * @see @ref Source::stop()
         */
        PlayableGroup& stop() {
            Source::stop(sources());
            return *this;
        }

        /** @brief Gain */
        Float gain() const {
            return _gain;
        }

        /**
         * @brief Set gain for all sound sources of Playables in this group
         * @param gain Gain
         *
         * Will calculate the sound sources gain relative to the gain of the
         * Playable and this playable group. The sources gain is computed as
         * `sourceGain = playableGain*groupGain`. Default of the groups gain
         * is `1.0f`.
         * @return Reference to self (for method chaining)
         */
        PlayableGroup& setGain(const Float gain) {
            _gain = gain;
            for(UnsignedInt i = 0; i < this->size(); ++i)
                (*this)[i].cleanGain();

            return *this;
        }

        /** @brief Sound transformation */
        const Matrix4& soundTransformation() const {
            return _soundTransform;
        }

        /**
         * @brief Set transformation of the sounds in this group
         * @return Reference to self (for method chaining)
         */
        PlayableGroup& setSoundTransformation(const Matrix4& matrix);

        /**
         * @brief Set all contained Playables clean
         * @see @ref AbstractObject::setClean()
         */
        void setClean();

    private:

        /* @brief Sources of all Playables in this group */
        std::vector<std::reference_wrapper<Source>> sources() {
            std::vector<std::reference_wrapper<Source>> srcs;
            srcs.reserve(this->size());
            for(UnsignedInt i = 0; i < this->size(); ++i)
                srcs.push_back((*this)[i].source());
            return srcs;
        }

        Matrix4 _soundTransform;
        Float _gain;
};

template<UnsignedInt dimensions> inline PlayableGroup<dimensions>& PlayableGroup<dimensions>::setSoundTransformation(const Matrix4& matrix) {
    _soundTransform = matrix;

    /* I cannot come up with a use case for which the sound
       transformation would be set frequently, so we are setting
       objects dirty whether the matrix changed or not. */
    for(UnsignedInt i = 0; i < this->size(); ++i)
        (*this)[i].object().setDirty();

    return *this;
}

template<UnsignedInt dimensions> inline void PlayableGroup<dimensions>::setClean() {
    std::vector<std::reference_wrapper<SceneGraph::AbstractObject<dimensions, Float>>> objects;
    objects.reserve(this->size());

    for(UnsignedInt i = 0; i < this->size(); ++i)
        objects.push_back((*this)[i].object());

    SceneGraph::AbstractObject<dimensions, Float>::setClean(objects);
}

/**
 * @brief Playable group for two dimensional float scenes
 *
 * @see @ref PlayableGroup3D
 */
typedef PlayableGroup<2> PlayableGroup2D;

/**
 * @brief Playable group for three dimensional float scenes
 *
 * @see @ref PlayableGroup2D
 */
typedef PlayableGroup<3> PlayableGroup3D;

}}

#endif
