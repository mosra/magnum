/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include "PlayableGroup.h"

#include "Magnum/Audio/Playable.h"
#include "Magnum/Audio/Source.h"
#include "Magnum/SceneGraph/AbstractObject.h"

namespace Magnum { namespace Audio {

namespace {

template<UnsignedInt dimensions> std::vector<std::reference_wrapper<Source>> sources(PlayableGroup<dimensions>& group) {
    std::vector<std::reference_wrapper<Source>> srcs;
    srcs.reserve(group.size());
    for(std::size_t i = 0; i != group.size(); ++i)
        srcs.push_back(group[i].source());
    return srcs;
}

}

template<UnsignedInt dimensions> PlayableGroup<dimensions>::PlayableGroup(): SceneGraph::FeatureGroup<dimensions, Playable<dimensions>, Float>(), _gain{1.0f} {}

template<UnsignedInt dimensions> PlayableGroup<dimensions>::~PlayableGroup() = default;

template<UnsignedInt dimensions> PlayableGroup<dimensions>& PlayableGroup<dimensions>::play() {
    Source::play(sources(*this));
    return *this;
}

template<UnsignedInt dimensions> PlayableGroup<dimensions>& PlayableGroup<dimensions>::pause() {
    Source::pause(sources(*this));
    return *this;
}

template<UnsignedInt dimensions> PlayableGroup<dimensions>& PlayableGroup<dimensions>::stop() {
    Source::stop(sources(*this));
    return *this;
}

template<UnsignedInt dimensions> PlayableGroup<dimensions>& PlayableGroup<dimensions>::setGain(const Float gain) {
    _gain = gain;
    for(UnsignedInt i = 0; i < this->size(); ++i)
        (*this)[i].cleanGain();

    return *this;
}

template<UnsignedInt dimensions> PlayableGroup<dimensions>& PlayableGroup<dimensions>::setSoundTransformation(const Matrix4& matrix) {
    _soundTransform = matrix;

    /* I cannot come up with a use case for which the sound transformation
       would be set frequently, so we are setting objects dirty whether the
       matrix changed or not. */
    for(UnsignedInt i = 0; i < this->size(); ++i)
        (*this)[i].object().setDirty();

    return *this;
}

#ifdef MAGNUM_BUILD_DEPRECATED
template<UnsignedInt dimensions> void PlayableGroup<dimensions>::setClean() {
    std::vector<std::reference_wrapper<SceneGraph::AbstractObject<dimensions, Float>>> objects;
    objects.reserve(this->size());

    for(UnsignedInt i = 0; i < this->size(); ++i)
        objects.push_back((*this)[i].object());

    SceneGraph::AbstractObject<dimensions, Float>::setClean(objects);
}
#endif

/* On non-MinGW Windows the instantiations are already marked with extern
   template */
#if !defined(CORRADE_TARGET_WINDOWS) || defined(__MINGW32__)
#define MAGNUM_AUDIO_EXPORT_HPP MAGNUM_AUDIO_EXPORT
#else
#define MAGNUM_AUDIO_EXPORT_HPP
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
template class MAGNUM_AUDIO_EXPORT_HPP PlayableGroup<2>;
template class MAGNUM_AUDIO_EXPORT_HPP PlayableGroup<3>;
#endif

}}
