#ifndef Magnum_Audio_Playable_h
#define Magnum_Audio_Playable_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

#include "Magnum/Audio/Source.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/SceneGraph/AbstractGroupedFeature.h"

namespace Magnum { namespace Audio {

/**
@brief Playable

Feature which manages the position, orientation and gain of a @ref Source for
an @ref SceneGraph::Object.

@section Audio-Playable-usage Usage

@code{.cpp}
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
    public:
        /**
         * @brief Constructor
         * @param object    Object this playable belongs to
         * @param group     Group this playable belongs to
         *
         * Creates playable with a source and a forward vector of @cpp {0.0f, -1.0f} @ce
         * for 2D and @cpp {0.0f, 0.0f, -1.0f} @ce for 3D scenes. This forward
         * vector cannot be changed, the sources orientation and translation
         * can be instead affected by @p object or via
         * @ref PlayableGroup::setSoundTransformation().
         * @see @ref setGain(), @ref PlayableGroup::add()
         */
        explicit Playable(SceneGraph::AbstractObject<dimensions, Float>& object, PlayableGroup<dimensions>* group = nullptr);

        ~Playable();

        /** @brief Source which is managed by this feature */
        Source& source() { return _source; }

        /** @brief Gain */
        Float gain() const { return _gain; }

        /**
         * @brief Set gain of the playable and source respecting the PlayableGroups gain
         * @return Reference to self (for method chaining)
         *
         * The sources gain is computed as @cpp sourceGain = playableGain*groupGain @ce.
         * Default for the playables gain is @cpp 1.0f @ce.
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

#if defined(CORRADE_TARGET_WINDOWS) && !defined(__MINGW32__)
extern template class MAGNUM_AUDIO_EXPORT Playable<2>;
extern template class MAGNUM_AUDIO_EXPORT Playable<3>;
#endif

}}

#endif
