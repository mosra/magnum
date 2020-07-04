#ifndef Magnum_Audio_PlayableGroup_h
#define Magnum_Audio_PlayableGroup_h
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
 * @brief Class @ref Magnum::Audio::PlayableGroup, typedef @ref Magnum::Audio::PlayableGroup2D, @ref Magnum::Audio::PlayableGroup3D
 */

#include "Magnum/Magnum.h"
#include "Magnum/Audio/Audio.h"
#include "Magnum/Audio/visibility.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/SceneGraph/FeatureGroup.h"

namespace Magnum { namespace Audio {

/**
@brief PlayableGroup

Manages a group of @ref Playable instances with an ability to control gain,
transformation or state for all of them at once. See @ref Playable and
@ref Listener documentation for more information.
@see @ref PlayableGroup2D, @ref PlayableGroup3D
*/
template<UnsignedInt dimensions> class PlayableGroup: public SceneGraph::FeatureGroup<dimensions, Playable<dimensions>, Float> {
    public:
        explicit PlayableGroup();

        ~PlayableGroup();

        /**
         * @brief Play all sound sources in this group
         * @return Reference to self (for method chaining)
         *
         * @see @ref Source::play()
         */
        PlayableGroup<dimensions>& play();

        /**
         * @brief Pause all sound sources in this group
         * @return Reference to self (for method chaining)
         *
         * @see @ref Source::pause()
         */
        PlayableGroup<dimensions>& pause();

        /**
         * @brief Stop all sound sources in this group
         * @return Reference to self (for method chaining)
         *
         * @see @ref Source::stop()
         */
        PlayableGroup<dimensions>& stop();

        /** @brief Gain */
        Float gain() const { return _gain; }

        /**
         * @brief Set gain for all sound sources in this group
         * @return Reference to self (for method chaining)
         *
         * Will calculate source gain for each source relative to the gain of
         * given @ref Playable and this playable group. The source gain is
         * calculated as @cpp sourceGain = playableGain*groupGain @ce. Default
         * group gain is @cpp 1.0f @ce.
         */
        PlayableGroup<dimensions>& setGain(const Float gain);

        /** @brief Sound transformation */
        const Matrix4& soundTransformation() const { return _soundTransform; }

        /**
         * @brief Set transformation of all sound sources in this group
         * @return Reference to self (for method chaining)
         */
        PlayableGroup& setSoundTransformation(const Matrix4& matrix);

    private:
        friend Playable<dimensions>;

        Matrix4 _soundTransform;
        Float _gain;
};

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

#if defined(CORRADE_TARGET_WINDOWS) && !(defined(CORRADE_TARGET_MINGW) && !defined(CORRADE_TARGET_CLANG))
extern template class MAGNUM_AUDIO_EXPORT PlayableGroup<2>;
extern template class MAGNUM_AUDIO_EXPORT PlayableGroup<3>;
#endif

}}

#endif
