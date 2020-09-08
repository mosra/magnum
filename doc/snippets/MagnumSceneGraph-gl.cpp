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

#include "Magnum/Timeline.h"
#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/DefaultFramebuffer.h"
#include "Magnum/GL/Renderer.h"
#include "Magnum/Math/Color.h"
#include "Magnum/MeshTools/Compile.h"
#include "Magnum/Primitives/Cube.h"
#include "Magnum/Platform/Sdl2Application.h"
#include "Magnum/SceneGraph/AnimableGroup.h"
#include "Magnum/SceneGraph/Camera.h"
#include "Magnum/SceneGraph/Drawable.h"
#include "Magnum/SceneGraph/MatrixTransformation2D.h"
#include "Magnum/SceneGraph/MatrixTransformation3D.h"
#include "Magnum/SceneGraph/Object.h"
#include "Magnum/SceneGraph/Scene.h"
#include "Magnum/Shaders/Flat.h"
#include "Magnum/Shaders/Phong.h"
#include "Magnum/Trade/MeshData.h"

using namespace Magnum;
using namespace Magnum::Math::Literals;

struct MyApplication: Platform::Application {
    explicit MyApplication(const Arguments& arguments);

    void drawEvent() override;
    void mousePressEvent(MouseEvent& event) override;

    SceneGraph::AnimableGroup3D animables;
    Timeline timeline;
    SceneGraph::Object<SceneGraph::MatrixTransformation2D>* cameraObject{};
    SceneGraph::Camera2D camera{*cameraObject};
};

/* [Animable-usage-timeline] */
MyApplication::MyApplication(const Arguments& arguments): Platform::Application{arguments} {
    // ...

    timeline.start();
}

void MyApplication::drawEvent() {
    animables.step(timeline.previousFrameTime(), timeline.previousFrameDuration());

    // ...

    timeline.nextFrame();
}
/* [Animable-usage-timeline] */

void MyApplication::mousePressEvent(MouseEvent& event) {
/* [Camera-projectionSize] */
Vector2 position = (Vector2{event.position()}/Vector2{GL::defaultFramebuffer.viewport().size()}
    - Vector2{0.5f})*Vector2::yScale(-1.0f)*camera.projectionSize();
/* [Camera-projectionSize] */

/* [Camera-projectionSize-absolute] */
Vector2 absolutePosition = cameraObject->absoluteTransformation().transformPoint(position);
/* [Camera-projectionSize-absolute] */
static_cast<void>(absolutePosition);
}

/* [Drawable-usage] */
typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;

class RedCubeDrawable: public SceneGraph::Drawable3D {
    public:
        explicit RedCubeDrawable(Object3D& object, SceneGraph::DrawableGroup3D* group):
            SceneGraph::Drawable3D{object, group}
        {
            _mesh = MeshTools::compile(Primitives::cubeSolid());
        }

    private:
        void draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) override {
            using namespace Math::Literals;

            _shader.setDiffuseColor(0xa5c9ea_rgbf)
                .setTransformationMatrix(transformationMatrix)
                .setNormalMatrix(transformationMatrix.normalMatrix())
                .setProjectionMatrix(camera.projectionMatrix())
                .draw(_mesh);
        }

        GL::Mesh _mesh;
        Shaders::Phong _shader;
};
/* [Drawable-usage] */

/* [Drawable-usage-multiple-inheritance] */
class RedCube: public Object3D, public SceneGraph::Drawable3D {
    public:
        explicit RedCube(Object3D* parent, SceneGraph::DrawableGroup3D* group):
            Object3D{parent}, SceneGraph::Drawable3D{*this, group}
        {
            _mesh = MeshTools::compile(Primitives::cubeSolid());
        }

    private:
        void draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) override;

        GL::Mesh _mesh;
        Shaders::Phong _shader;
};
/* [Drawable-usage-multiple-inheritance] */

void draw(const Matrix4&, SceneGraph::Camera3D&);
void draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) {
/* [Drawable-usage-shader] */
Shaders::Flat3D shader;
shader.setTransformationProjectionMatrix(
    camera.projectionMatrix()*transformationMatrix);
/* [Drawable-usage-shader] */
}

namespace A {

struct MyApplication: Platform::Application {
    explicit MyApplication(const Arguments& arguments);

    void drawEvent() override;

    Scene3D _scene;
    SceneGraph::Object<SceneGraph::MatrixTransformation3D>* _cameraObject;
    SceneGraph::Camera3D* _camera;
    SceneGraph::DrawableGroup3D _drawables;
};

/* [Drawable-usage-camera] */
MyApplication::MyApplication(const Arguments& arguments): Platform::Application{arguments} {
    // ...

    _cameraObject = new Object3D{&_scene};
    _cameraObject->translate(Vector3::zAxis(5.0f));
    _camera = new SceneGraph::Camera3D(*_cameraObject);
    _camera->setProjectionMatrix(Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.001f, 100.0f));
}

void MyApplication::drawEvent() {
    _camera->draw(_drawables);

    // ...

    swapBuffers();
}
/* [Drawable-usage-camera] */

}

namespace B {

struct MyApplication: Platform::Application {
    explicit MyApplication(const Arguments& arguments);

    void drawEvent() override;

    SceneGraph::Object<SceneGraph::MatrixTransformation2D>* _cameraObject;
    SceneGraph::Camera3D* _camera;
    Vector4 _lightPositionRelativeToCamera;
    Color3 _lightColor, _ambientColor;
/* [Drawable-multiple-groups] */
    // ...

    Shaders::Phong _shader;
    SceneGraph::DrawableGroup3D _phongObjects, _transparentObjects;
};

void MyApplication::drawEvent() {
    _shader.setProjectionMatrix(_camera->projectionMatrix())
           .setLightPositions({_lightPositionRelativeToCamera})
           .setLightColors({_lightColor})
           .setAmbientColor(_ambientColor);

    /* Each drawable sets only unique properties such as transformation matrix
       and diffuse color */
    _camera->draw(_phongObjects);

    GL::Renderer::enable(GL::Renderer::Feature::Blending);

    /* Also here */
    _camera->draw(_transparentObjects);

    GL::Renderer::disable(GL::Renderer::Feature::Blending);

    // ...
}
/* [Drawable-multiple-groups] */

}

int main() {
/* [Drawable-usage-instance] */
Scene3D scene;
SceneGraph::DrawableGroup3D drawables;

Object3D* redCube = new Object3D{&scene};
(*redCube)
    .translate(Vector3::yAxis(-0.3f))
    .rotateX(30.0_degf);
new RedCubeDrawable{*redCube, &drawables};

// ...
/* [Drawable-usage-instance] */

/* [Drawable-usage-instance-multiple-inheritance] */
(new RedCube(&scene, &drawables))
    ->translate(Vector3::yAxis(-0.3f))
    .rotateX(30.0_degf);
/* [Drawable-usage-instance-multiple-inheritance] */

return 0; /* on iOS SDL redefines main to SDL_main and then return is needed */
}
