/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#include <Corrade/Containers/Reference.h>

#include "Magnum/Audio/Buffer.h"
#include "Magnum/Audio/Listener.h"
#include "Magnum/Audio/Playable.h"
#include "Magnum/Audio/PlayableGroup.h"

#include "Magnum/SceneGraph/MatrixTransformation3D.h"
#include "Magnum/SceneGraph/Object.h"
#include "Magnum/SceneGraph/Scene.h"

using namespace Magnum;

int main() {

{
typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;

/* [Listener-usage] */
Scene3D scene;
Object3D object{&scene};
Audio::Listener3D listener{object};

// ...

// every frame, adapt the listener to changes in scene transformation
listener.update({});
/* [Listener-usage] */
}

{
typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;
Audio::PlayableGroup3D someOtherGroup;
/* [Playable-usage] */
Scene3D scene;
Object3D object{&scene};
Audio::Buffer buffer;

// ...
Audio::Listener3D listener{scene};
Audio::PlayableGroup3D group;

/* Attach the playable to an object and configure its source */
Audio::Playable3D playable{object, &group};
playable.source()
    .setBuffer(&buffer)
    .setLooping(true)
    .play();

// ...

// every frame, adapt the listener and all playables to changes in scene
// transformation
listener.update({group, someOtherGroup});
/* [Playable-usage] */
}

}
