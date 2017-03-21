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

#include "Magnum/Audio/Listener.h"
#include "Magnum/Audio/Playable.h"
#include "Magnum/Audio/PlayableGroup.h"


namespace Magnum { namespace Audio {

using namespace SceneGraph;

namespace Implementation {
    void* activeListener = nullptr;
}

namespace {

/* Create a Matrix4 from a Matrix3/Matrix4 */
inline Matrix4 padMatrix4(const Matrix3& m) {
    return Matrix4{Vector4::pad(m[0]), Vector4::pad(m[1]), Vector4::pad(m[2]), Vector4{}};
}

inline Matrix4 padMatrix4(const Matrix4& m) {
    return m;
}

}

template<UnsignedInt dimensions> void Listener<dimensions>::clean(const MatrixTypeFor<dimensions, Float>& absoluteTransformationMatrix) {
    if(!isActive()) {
        /* Only clean if this Listener is active */
        return;
    }

    Renderer::setListenerPosition(soundTransformation().transformVector(Vector3::pad(absoluteTransformationMatrix.translation())));

    const Vector3 fwd = soundTransformation().transformVector(-padMatrix4(absoluteTransformationMatrix).backward());
    const Vector3 up = soundTransformation().transformVector(padMatrix4(absoluteTransformationMatrix).up());

    Renderer::setListenerOrientation(fwd, up);

    Renderer::setListenerGain(_gain);

    /** @todo velocity */
}

template<UnsignedInt dimensions> void Listener<dimensions>::update(std::initializer_list<std::reference_wrapper<PlayableGroup<dimensions>>> groups) {

    /* Check if active listener just changed to this */
    if(this != Implementation::activeListener) {
        /* Ensure that clean() is called also when switching between (clean)
           listeners */
        Implementation::activeListener = this;
        this->object().setDirty();

        /* Listener gain needs to be updated only when active listener changed
           and in setGain() */
        Renderer::setListenerGain(_gain);
    }

    /* Add all objects of the Playables in the PlayableGroups to a vector to
       later setClean() */
    std::vector<std::reference_wrapper<AbstractObject<dimensions, Float>>> objects;

    objects.push_back(this->object());
    for(PlayableGroup<dimensions>& group : groups) {
        for(UnsignedInt i = 0; i < group.size(); ++i) {
            objects.push_back(group[i].object());
        }
    }

    /* Use the more performant way to set multiple objects clean */
    AbstractObject<dimensions, Float>::setClean(objects);
}

/* On non-MinGW Windows the instantiations are already marked with extern
   template */
#if !defined(CORRADE_TARGET_WINDOWS) || defined(__MINGW32__)
#define MAGNUM_AUDIO_EXPORT_HPP MAGNUM_AUDIO_EXPORT
#else
#define MAGNUM_AUDIO_EXPORT_HPP
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
template class MAGNUM_AUDIO_EXPORT_HPP Listener<2>;
template class MAGNUM_AUDIO_EXPORT_HPP Listener<3>;
#endif

}}
