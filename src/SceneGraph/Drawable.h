#ifndef Magnum_SceneGraph_Drawable_h
#define Magnum_SceneGraph_Drawable_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Magnum::SceneGraph::BasicDrawable, Magnum::SceneGraph::BasicDrawableGroup, typedef Magnum::SceneGraph::Drawable2D, Magnum::SceneGraph::Drawable3D, Magnum::SceneGraph::DrawableGroup2D, Magnum::SceneGraph::DrawableGroup3D
 */

#include "AbstractGroupedFeature.h"

namespace Magnum { namespace SceneGraph {

/**
@brief %Drawable

Adds drawing function to the object. Each Drawable is part of some DrawableGroup
and the whole group is drawn with particular camera using AbstractCamera::draw().

@section Drawable-usage Usage

First thing is add Drawable feature to some object and implement draw(). You
can do it conveniently using multiple inheritance (see @ref scenegraph-features
for introduction). Example:
@code
typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D<>> Object3D;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D<>> Scene3D;

class DrawableObject: public Object3D, SceneGraph::Drawable3D {
    public:
        DrawableObject(Object* parent = nullptr, SceneGraph::DrawableGroup3D* group = nullptr): Object3D(parent), SceneGraph::Drawable3D(this, group) {
            // ...
        }

        void draw(const Matrix4& transformationMatrix, AbstractCamera3D* camera) override {
            // ...
        }
}
@endcode

Then you add these objects to your scene and some drawable group and transform
them as you like. You can also use BasicDrawableGroup::add() and
BasicDrawableGroup::remove().
@code
Scene3D scene;
SceneGraph::DrawableGroup3D drawables;

(new DrawableObject(&scene, &drawables))
    ->translate(Vector3::yAxis(-0.3f))
    ->rotateX(30.0_degf);
(new AnotherDrawableObject(&scene, &drawables))
    ->translate(Vector3::zAxis(0.5f));
// ...
@endcode

The last thing you need is Camera attached to some object (thus using its
transformation) and with it you can perform drawing in your draw event
implementation. See Camera2D and Camera3D documentation for more information.
@code
Camera3D<> camera(&cameraObject);

void MyApplication::drawEvent() {
    camera.draw(drawables);

    swapBuffers();
    // ...
}
@endcode

@section Drawable-performance Using drawable groups to improve performance

You can organize your drawables to multiple groups to minimize OpenGL state
changes - for example put all objects using the same shader, the same light
setup etc into one group, then put all transparent into another and set common
parameters once for whole group instead of setting them again in each draw()
implementation. Example:
@code
Shaders::PhongShader* shader;
SceneGraph::DrawableGroup3D phongObjects, transparentObjects;

void MyApplication::drawEvent() {
    shader->setProjectionMatrix(camera->projectionMatrix())
          ->setLightPosition(lightPosition)
          ->setLightColor(lightColor)
          ->setAmbientColor(ambientColor);
    camera.draw(phongObjects);

    Renderer::setFeature(Renderer::Feature::Blending, true);
    camera.draw(transparentObjects);
    Renderer::setFeature(Renderer::Feature::Blending, false);

    // ...
}
@endcode

@see Drawable2D, Drawable3D, @ref scenegraph, DrawableGroup2D, DrawableGroup3D
*/
template<UnsignedInt dimensions, class T> class BasicDrawable: public AbstractBasicGroupedFeature<dimensions, BasicDrawable<dimensions, T>, T> {
    public:
        /**
         * @brief Constructor
         * @param object    %Object this drawable belongs to
         * @param drawables Group this drawable belongs to
         *
         * Adds the feature to the object and also to the group, if specified.
         * Otherwise you can use BasicDrawableGroup::add().
         */
        explicit BasicDrawable(AbstractBasicObject<dimensions, T>* object, BasicDrawableGroup<dimensions, T>* drawables = nullptr): AbstractBasicGroupedFeature<dimensions, BasicDrawable<dimensions, T>, T>(object, drawables) {}

        /**
         * @brief Draw the object using given camera
         * @param transformationMatrix      %Object transformation relative
         *      to camera
         * @param camera                    Camera
         *
         * Projection matrix can be retrieved from AbstractCamera::projectionMatrix().
         */
        virtual void draw(const typename DimensionTraits<dimensions, T>::MatrixType& transformationMatrix, AbstractBasicCamera<dimensions, T>* camera) = 0;
};

/**
@brief Two-dimensional drawable for float scenes

@see Drawable3D
*/
typedef BasicDrawable<2, Float> Drawable2D;

/**
@brief Three-dimensional drawable for float scenes

@see Drawable2D
*/
typedef BasicDrawable<3, Float> Drawable3D;

/**
@brief Group of drawables

See Drawable for more information.
@see @ref scenegraph, DrawableGroup2D, DrawableGroup3D
*/
#ifndef CORRADE_GCC46_COMPATIBILITY
template<UnsignedInt dimensions, class T> using BasicDrawableGroup = BasicFeatureGroup<dimensions, BasicDrawable<dimensions, T>, T>;
#else
template<UnsignedInt dimensions, class T> class BasicDrawableGroup: public BasicFeatureGroup<dimensions, BasicDrawable<dimensions, T>, T> {};
#endif

/**
@brief Group of two-dimensional drawables for float scenes

@see DrawableGroup3D
*/
typedef BasicDrawableGroup<2, Float> DrawableGroup2D;

/**
@brief Group of three-dimensional drawables for float scenes

@see DrawableGroup2D
*/
typedef BasicDrawableGroup<3, Float> DrawableGroup3D;

}}

#endif
