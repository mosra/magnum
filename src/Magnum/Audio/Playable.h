#ifndef Magnum_Audio_Playable_h
#define Magnum_Audio_Playable_h
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
 * @brief Class @ref Magnum::Audio::Playable, typedef @ref Magnum::Audio::Playable2D, @ref Magnum::Audio::Playable3D
 */

#include <string>
#include <al.h>

#include <Magnum/SceneGraph/AbstractGroupedFeature.h>
#include <Magnum/Math/Matrix3.h>
#include <Magnum/Math/Matrix4.h>

#include "Magnum/Audio/PlayableGroup.h"
#include "Magnum/Audio/Source.h"
#include "Magnum/Audio/visibility.h"

namespace Magnum { namespace Audio {

/**
@brief Playable

Feature which manages the position, orientation and gain of a @ref Source for
an @ref SceneGraph::Object.

## Usage

@code
Object3D object;
Source source;
Playable3D playable{object, source};

// ...

object.translate(offset);

// ... and every frame, update the sources positions by cleaning the object:
object.setClean();
@endcode

Position of the source will be updated to the translation of `object` when
@ref SceneGraph::Object::setClean() is called, which is done in
@ref Audio::Listener::update() or @ref Audio::PlayableGroup::setClean() for example.

To manage multiple Playables at once, use @ref PlayableGroup.

-   @ref Playable2D
-   @ref Playable3D

@see @ref Source, @ref PlayableGroup, @ref Listener
*/
template<UnsignedInt dimensions> class Playable: public SceneGraph::AbstractGroupedFeature<dimensions, Playable<dimensions>, Float> {
    friend PlayableGroup<dimensions>;

    public:

        /**
         * @brief Constructor
         * @param object    Object this playable belongs to
         * @param group     Group this playable belongs to
         *
         * Creates playable with a source and a forward vector of `{0.0f, -1.0f}`
         * for 2D and `{0.0f, 0.0f, -1.0f}` for 3D scenes. This forward vector
         * cannot be changed, the sources orientation and translation can be
         * instead affected by `object` or via
         * @ref PlayableGroup::setSoundTransformation().
         * @see @ref setGain(), @ref PlayableGroup::add()
         */
        explicit Playable(SceneGraph::AbstractObject<dimensions, Float>& object, PlayableGroup<dimensions>* group = nullptr):
            SceneGraph::AbstractGroupedFeature<dimensions, Playable<dimensions>, Float>(object, group),
            _fwd(0.0f),
            _gain(1.0f),
            _source()
        {
            SceneGraph::AbstractFeature<dimensions, Float>::setCachedTransformations(SceneGraph::CachedTransformation::Absolute);
            _fwd[dimensions - 1] = -1;
        }

        /** @brief Source which is managed by this feature */
        Source& source() {
            return _source;
        }

        /** @brief Gain */
        Float gain() const {
            return _gain;
        }

        /**
         * @brief Set gain of the playable and source respecting the PlayableGroups gain
         * @return Reference to self (for method chaining)
         *
         * The sources gain is computed as `sourceGain = playableGain*groupGain`.
         * Default for the playables gain is `1.0f`.
         * @see @ref PlayableGroup::setGain(), @ref Source::setGain()
         */
        Playable& setGain(const Float gain) {
            _gain = gain;
            cleanGain();
            return *this;
        }

        /**
         * @brief Group containing this playable
         *
         * If the playable doesn't belong to any group, returns `nullptr`.
         */
        PlayableGroup<dimensions>* playables() {
            return static_cast<PlayableGroup<dimensions>*>(this->group());
        }

        const PlayableGroup<dimensions>* playables() const { /**< @overload */
            return static_cast<const PlayableGroup<dimensions>*>(this->group());
        }

    private:

        void clean(const MatrixTypeFor<dimensions, Float>& absoluteTransformationMatrix) override {
            Vector3 position = Vector3::pad(absoluteTransformationMatrix.translation(), 0);
            if(playables()) {
                position = playables()->soundTransformation().transformVector(position);
            }
            _source.setPosition(position);
            _source.setDirection(Vector3::pad(absoluteTransformationMatrix.rotation()*_fwd));

            /** @todo velocity */
        }

        /* Update the gain of the underlying source to reflect changes in _group and/or _gain.
           Called in Playable::setGain() and PlayableGroup::setGain() */
        void cleanGain() {
            if(playables()) {
                _source.setGain(_gain*playables()->gain());
            } else {
                _source.setGain(_gain);
            }
        }

        VectorTypeFor<dimensions, Float> _fwd;
        Float _gain;
        Source _source;
};

/**
 * @brief Playable for two dimensional float scenes
 *
 * @see @ref Playable3D
 */
typedef Playable<2> Playable2D;

/**
 * @brief Playable for three dimensional float scenes
 *
 * @see @ref Playable2D
 */
typedef Playable<3> Playable3D;

}}

#endif
