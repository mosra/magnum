#ifndef Magnum_Audio_Listener_h
#define Magnum_Audio_Listener_h
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
 * @brief Class @ref Magnum::Audio::Listener, @ref Magnum::Audio::Listener2D, @ref Magnum::Audio::Listener3D
 */

#include "Magnum/Audio/Audio.h"
#include "Magnum/Audio/visibility.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/SceneGraph/AbstractFeature.h"

namespace Magnum { namespace Audio {

/**
@brief Listener

Feature which manages the position, orientation and gain of the OpenAL listener
for a @ref SceneGraph::Object.

@section Audio-Listener-usage Usage

The listener will be commonly used together with a bunch of @ref Playable
features, managed in one or more @ref PlayableGroup instances. In order to
reflect transformation changes affecting the scene, you need to call
@ref update() after each change (or simply every frame):

@snippet MagnumAudio-scenegraph.cpp Listener-usage

For two-dimensional scenes simply replace all `3D` with `2D`. See @ref Playable
for more info about how to set up and group audio sources.

@section Audio-Listener-active-listener Active listener

There can only be at most *one* active listener at a given time, i.e. the one
on which @ref Listener::update() was called last. This is because OpenAL
only supports notion of one listener. Having multiple @ref Listener2D or
@ref Listener3D instances can still be useful for conveniently switching
between them for cinematics for example.

@section Audio-Listener-sound-transformation Sound transformation

@ref Listener::setSoundTransformation() enables you to set a transformation
matrix which is applied to the listeners orientation and position before passed
onto OpenAL. This can be used for rotating two dimensional scenes as planes
into the three dimensional audio space or even scaling the audio scene to match
a certain world scale. In the later case you might want to instead consider
@ref Renderer::setSpeedOfSound().

@see @ref Listener2D, @ref Listener3D, @ref Renderer, @ref Playable,
    @ref PlayableGroup
*/
template<UnsignedInt dimensions> class Listener: public SceneGraph::AbstractFeature<dimensions, Float> {
    public:
        /**
         * @brief Constructor
         * @param object    Object this listener belongs to
         *
         * Creates a listener with a default orientation (i.e., forward vector
         * is @cpp {0.0f, 0.0f, -1.0f} @ce and up vector of @cpp {0.0f, 1.0f, 0.0f} @ce).
         * You can change this orientation by transforming the object this
         * listener is attached to or via @ref Listener::setSoundTransformation().
         * @see @ref setGain()
         */
        explicit Listener(SceneGraph::AbstractObject<dimensions, Float>& object);

        ~Listener();

        /** @brief Sound transformation */
        const Matrix4& soundTransformation() const {
            return _soundTransformation;
        }

        /**
         * @brief Set sound transformation
         * @return Reference to self (for method chaining)
         *
         * Global transformation for transforming from world to listener space.
         */
        Listener<dimensions>& setSoundTransformation(const Matrix4& soundTransformation) {
            _soundTransformation = soundTransformation;
            this->object().setDirty();
            return *this;
        }

        /**
         * @brief Update the listener
         * @param groups Groups to update
         *
         * Makes this instance the active listener and calls
         * @ref SceneGraph::AbstractObject::setClean() on its parent object and
         * all objects of the @ref Playable "Playables" in the group to reflect
         * transformation changes to spatial audio behavior. Also updates
         * listener-related configuration for @ref Renderer (position,
         * orientation, gain).
         */
        void update(std::initializer_list<Containers::Reference<PlayableGroup<dimensions>>> groups);

        /** @brief Listener gain */
        Float gain() const { return _gain; }

        /**
         * @brief Set listener gain
         * @return Reference to self (for method chaining)
         *
         * Default is @cpp 1.0f @ce (i.e., not affecting the global gain in any
         * way).
         */
        Listener<dimensions>& setGain(Float gain);

        /** @brief Whether this listener is the active listener */
        bool isActive() const;

    private:
        MAGNUM_AUDIO_LOCAL void clean(const MatrixTypeFor<dimensions, Float>& absoluteTransformationMatrix) override;

        Matrix4 _soundTransformation;
        Float _gain;
};

/**
 * @brief Listener for two-dimensional float scenes
 *
 * @see @ref Listener3D
 */
typedef Listener<2> Listener2D;

/**
 * @brief Listener for three-dimensional float scenes
 *
 * @see @ref Listener2D
 */
typedef Listener<3> Listener3D;

#if defined(CORRADE_TARGET_WINDOWS) && !(defined(CORRADE_TARGET_MINGW) && !defined(CORRADE_TARGET_CLANG))
extern template class MAGNUM_AUDIO_EXPORT Listener<2>;
extern template class MAGNUM_AUDIO_EXPORT Listener<3>;
#endif

}}

#endif
