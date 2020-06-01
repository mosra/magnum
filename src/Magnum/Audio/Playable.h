#ifndef Magnum_Audio_Playable_h
#define Magnum_Audio_Playable_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
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

#include "Magnum/Audio/Source.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/SceneGraph/AbstractGroupedFeature.h"

namespace Magnum { namespace Audio {

/**
@brief Playable

Feature which manages the position, orientation and gain of a @ref Source for
an @ref SceneGraph::Object.

@section Audio-Playable-usage Usage

Attach the instance to an existing object and set a filled buffer to it. In
order to reflect transformation changes from the scene in the spatial audio,
the playable should be added to some @ref PlayableGroup, which is periodically
updated from a currently active @ref Listener using @ref Listener::update():

@snippet MagnumAudio-scenegraph.cpp Playable-usage

Note that @ref Source::setPosition(), @ref Source::setDirection() and
@ref Source::setGain() called on @ref source() will be overwritten on next call
to @ref Listener::update() / @ref PlayableGroup::setGain() / @ref setGain() and
you have to use other means to update them:

-   Transformation of the source is inherited from the scene. If you want to
    transform it, transform the @ref SceneGraph::Object the playable is
    attached to. It's also possible to affect the global listener
    and group transformation using @ref Listener::setSoundTransformation() and
    @ref PlayableGroup::setSoundTransformation()
-   The source is by default omnidirectional (i.e., passing a zero vector to
    @ref Source::setDirection()). You can set the initial direction using the
    @ref Playable(SceneGraph::AbstractObject<dimensions, Float>&, const VectorTypeFor<dimensions, Float>&, PlayableGroup<dimensions>*)
    constructor, the direction will be automatically rotated based on playable
    transformation.
-   Source gain is set as a combination of @ref PlayableGroup gain and
    @ref Playable gain and updated on every call to @ref setGain() or
    @ref PlayableGroup::setGain().

@see @ref Playable2D, @ref Playable3D
*/
template<UnsignedInt dimensions> class Playable: public SceneGraph::AbstractGroupedFeature<dimensions, Playable<dimensions>, Float> {
    public:
        /**
         * @brief Construct a playable with omnidirectional source.
         * @param object    Object this playable belongs to
         * @param group     Group this playable belongs to
         *
         * Creates a source with a zero direction vector. See
         * @ref Playable(SceneGraph::AbstractObject<dimensions, Float>&, const VectorTypeFor<dimensions, Float>&, PlayableGroup<dimensions>*)
         * for an alternative.
         * @see @ref setGain(), @ref PlayableGroup::add()
         */
        explicit Playable(SceneGraph::AbstractObject<dimensions, Float>& object, PlayableGroup<dimensions>* group = nullptr): Playable<dimensions>{object, {}, group} {}

        /**
         * @brief Construct a playable with directional source.
         * @param object    Object this playable belongs to
         * @param direction Source direction
         * @param group     Group this playable belongs to
         *
         * Uses @p direction as a base for @ref Source::setDirection(). Passing
         * a zero vector is equivalent to calling
         * @ref Playable(SceneGraph::AbstractObject<dimensions, Float>&, PlayableGroup<dimensions>*).
         * @see @ref setGain(), @ref PlayableGroup::add()
         */
        explicit Playable(SceneGraph::AbstractObject<dimensions, Float>& object, const VectorTypeFor<dimensions, Float>& direction, PlayableGroup<dimensions>* group = nullptr);

        ~Playable();

        /** @brief Source which is managed by this feature */
        Source& source() { return _source; }

        /** @brief Gain */
        Float gain() const { return _gain; }

        /**
         * @brief Set gain of the playable and source respecting the PlayableGroups gain
         * @return Reference to self (for method chaining)
         *
         * The source gain is calculated as
         * @cpp sourceGain = playableGain*groupGain @ce. Default is @cpp 1.0f @ce.
         * @see @ref PlayableGroup::setGain(), @ref Source::setGain()
         */
        Playable& setGain(const Float gain);

        /**
         * @brief Group containing this playable
         *
         * If the playable doesn't belong to any group, returns @cpp nullptr @ce.
         */
        PlayableGroup<dimensions>* playables();

        const PlayableGroup<dimensions>* playables() const; /**< @overload */

    private:
        friend PlayableGroup<dimensions>;

        MAGNUM_AUDIO_LOCAL void clean(const MatrixTypeFor<dimensions, Float>& absoluteTransformationMatrix) override;

        /* Updates the gain of the underlying source to reflect changes in
           _group and/or _gain. Called from setGain() and
           PlayableGroup::setGain() */
        MAGNUM_AUDIO_LOCAL void cleanGain();

        VectorTypeFor<dimensions, Float> _direction;
        Float _gain;
        Source _source;
};

/**
 * @brief Playable for two-dimensional float scenes
 *
 * @see @ref Playable3D
 */
typedef Playable<2> Playable2D;

/**
 * @brief Playable for three-dimensional float scenes
 *
 * @see @ref Playable2D
 */
typedef Playable<3> Playable3D;

#if defined(CORRADE_TARGET_WINDOWS) && !(defined(CORRADE_TARGET_MINGW) && !defined(CORRADE_TARGET_CLANG))
extern template class MAGNUM_AUDIO_EXPORT Playable<2>;
extern template class MAGNUM_AUDIO_EXPORT Playable<3>;
#endif

}}

#endif
