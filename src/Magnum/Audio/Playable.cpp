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

#include "Playable.h"

#include "Magnum/Audio/PlayableGroup.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"

namespace Magnum { namespace Audio {

template<UnsignedInt dimensions> Playable<dimensions>::Playable(SceneGraph::AbstractObject<dimensions, Float>& object, const VectorTypeFor<dimensions, Float>& direction, PlayableGroup<dimensions>* group): SceneGraph::AbstractGroupedFeature<dimensions, Playable<dimensions>, Float>(object, group), _direction{direction}, _gain{1.0f} {
    SceneGraph::AbstractFeature<dimensions, Float>::setCachedTransformations(SceneGraph::CachedTransformation::Absolute);
}

template<UnsignedInt dimensions> Playable<dimensions>::~Playable() = default;

template<UnsignedInt dimensions> Playable<dimensions>& Playable<dimensions>::setGain(const Float gain) {
    _gain = gain;
    cleanGain();
    return *this;
}

template<UnsignedInt dimensions> void Playable<dimensions>::clean(const MatrixTypeFor<dimensions, Float>& absoluteTransformationMatrix) {
    Vector3 position = Vector3::pad(absoluteTransformationMatrix.translation());
    if(playables())
        position = playables()->soundTransformation().transformVector(position);

    _source.setPosition(position);
    _source.setDirection(Vector3::pad(absoluteTransformationMatrix.rotation()*_direction));

    /** @todo velocity */
}

template<UnsignedInt dimensions> PlayableGroup<dimensions>* Playable<dimensions>::playables()  {
    return static_cast<PlayableGroup<dimensions>*>(this->group());
}

template<UnsignedInt dimensions> const PlayableGroup<dimensions>* Playable<dimensions>::playables()  const {
    return static_cast<const PlayableGroup<dimensions>*>(this->group());
}

template<UnsignedInt dimensions> void Playable<dimensions>::cleanGain() {
    _source.setGain(playables() ? _gain*playables()->gain() : _gain);
}

/* On non-MinGW Windows the instantiations are already marked with extern
   template. However Clang-CL doesn't propagate the export from the extern
   template, it seems. */
#if !defined(CORRADE_TARGET_WINDOWS) || defined(CORRADE_TARGET_MINGW) || defined(CORRADE_TARGET_CLANG_CL)
#define MAGNUM_AUDIO_EXPORT_HPP MAGNUM_AUDIO_EXPORT
#else
#define MAGNUM_AUDIO_EXPORT_HPP
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
template class MAGNUM_AUDIO_EXPORT_HPP Playable<2>;
template class MAGNUM_AUDIO_EXPORT_HPP Playable<3>;
#endif

}}
