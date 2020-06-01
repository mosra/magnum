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

#include <algorithm>

#include "Magnum/Math/Matrix4.h"
#include "Magnum/Math/Intersection.h"
#include "Magnum/SceneGraph/Animable.h"
#include "Magnum/SceneGraph/AnimableGroup.h"
#include "Magnum/SceneGraph/AbstractGroupedFeature.h"
#include "Magnum/SceneGraph/AbstractTranslationRotation3D.h"
#include "Magnum/SceneGraph/MatrixTransformation2D.h"
#include "Magnum/SceneGraph/MatrixTransformation3D.h"
#include "Magnum/SceneGraph/Camera.h"
#include "Magnum/SceneGraph/Drawable.h"
#include "Magnum/SceneGraph/Object.h"
#include "Magnum/SceneGraph/Scene.h"

using namespace Magnum;
using namespace Magnum::Math::Literals;

/* [AbstractFeature-caching] */
class CachingFeature: public SceneGraph::AbstractFeature3D {
    public:
        explicit CachingFeature(SceneGraph::AbstractObject3D& object): SceneGraph::AbstractFeature3D{object} {
            setCachedTransformations(SceneGraph::CachedTransformation::Absolute);
        }

    private:
        void clean(const Matrix4& absoluteTransformationMatrix) override {
            _absolutePosition = absoluteTransformationMatrix.translation();
        }

        Vector3 _absolutePosition;
};
/* [AbstractFeature-caching] */

/* [AbstractFeature-object-transformation] */
class TransformingFeature: public SceneGraph::AbstractFeature3D {
    public:
        template<class T> explicit TransformingFeature(SceneGraph::Object<T>& object):
            SceneGraph::AbstractFeature3D{object}, _transformation{object} {}

    private:
        SceneGraph::AbstractTranslationRotation3D& _transformation;
};
/* [AbstractFeature-object-transformation] */

/* [AbstractGroupedFeature-subclassing] */
class Drawable: public SceneGraph::AbstractGroupedFeature3D<Drawable> {
    // ...
};

typedef SceneGraph::FeatureGroup3D<Drawable> DrawableGroup;
/* [AbstractGroupedFeature-subclassing] */

/* [Animable-usage-definition] */
typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;

class AnimableObject: public Object3D, public SceneGraph::Animable3D {
    public:
        AnimableObject(Object3D* parent = nullptr, SceneGraph::AnimableGroup3D* group = nullptr): Object3D{parent}, SceneGraph::Animable3D{*this, group} {
            setDuration(10.0f);
            // ...
        }

    private:
        void animationStep(Float, Float delta) override {
            rotateX(15.0_degf*delta); // rotate at 15 degrees per second
        }
};
/* [Animable-usage-definition] */

/* [typedef] */
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;
typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
/* [typedef] */

/* [Object-typedef] */
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;
typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
/* [Object-typedef] */

/* [feature-inherit] */
class BouncingBall: public Object3D, SceneGraph::Drawable3D, SceneGraph::Animable3D {
    public:
        explicit BouncingBall(Object3D* parent):
            Object3D{parent}, SceneGraph::Drawable3D{*this}, SceneGraph::Animable3D{*this} {}

    private:
        // drawing implementation for Drawable feature
        void draw(const Matrix4&, SceneGraph::Camera3D&) override;

        // animation step for Animable feature
        void animationStep(Float, Float) override;
};
/* [feature-inherit] */

/* [caching] */
class CachingObject: public Object3D, SceneGraph::AbstractFeature3D {
    public:
        explicit CachingObject(Object3D* parent): Object3D{parent}, SceneGraph::AbstractFeature3D{*this} {
            setCachedTransformations(SceneGraph::CachedTransformation::Absolute);
        }

    protected:
        void clean(const Matrix4& absoluteTransformation) override {
            _absolutePosition = absoluteTransformation.translation();
        }

    private:
        Vector3 _absolutePosition;
};
/* [caching] */

namespace {

#ifdef __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-private-field"
#endif
/* [transformation] */
class TransformingFeature: public SceneGraph::AbstractFeature3D {
    public:
        template<class T> explicit TransformingFeature(SceneGraph::Object<T>& object):
            SceneGraph::AbstractFeature3D(object), _transformation(object) {}

    private:
        SceneGraph::AbstractTranslationRotation3D& _transformation;
};
/* [transformation] */
#ifdef __clang__
#pragma GCC diagnostic pop
#endif

}

int main() {
{
/* [method-chaining] */
Scene3D scene;

Object3D object;
object.setParent(&scene)
      .rotateY(15.0_degf)
      .translate(Vector3::xAxis(5.0f));
/* [method-chaining] */
}

{
/* [hierarchy] */
Scene3D scene;

Object3D* first = new Object3D{&scene};
Object3D* second = new Object3D{first};
/* [hierarchy] */
static_cast<void>(second);
}

{
/* [hierarchy-addChild] */
Scene3D scene;

Object3D& first = scene.addChild<Object3D>();
Object3D& second = first.addChild<Object3D>();
/* [hierarchy-addChild] */
static_cast<void>(second);
}

{
struct MyFeature {
    explicit MyFeature(SceneGraph::AbstractObject3D&, int, int) {}
};
int some{}, params{};
{
/* [feature] */
Object3D o;
new MyFeature{o, some, params};
/* [feature] */
}

{
/* [feature-addFeature] */
Object3D o;
o.addFeature<MyFeature>(some, params);
/* [feature-addFeature] */
}
}

{
/* [construction-order] */
{
    Scene3D scene;
    Object3D object(&scene);
}
/* [construction-order] */
}

{
/* [construction-order-crash] */
{
    Object3D object;
    Scene3D scene;

    object.setParent(&scene);
} // crash!
/* [construction-order-crash] */
}

{
struct MyFeature {
    explicit MyFeature(...) {}
};
{
/* [feature-construction-order] */
class MyObject: public Object3D, MyFeature {
    public:
        MyObject(Object3D* parent): Object3D(parent), MyFeature{*this} {}
};
/* [feature-construction-order] */
}
{
/* [feature-construction-order-crash] */
class MyObject: MyFeature, public Object3D {
    public:
        MyObject(Object3D* parent): MyFeature{*this}, Object3D(parent) {} // crash!
};
/* [feature-construction-order-crash] */
}
{
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreorder"
#endif
/* [feature-construction-order-crash-destruction] */
class MyObject: MyFeature, public Object3D {
    public:
        MyObject(Object3D* parent): Object3D(parent), MyFeature(*this) {}

        // crash on destruction!
};
/* [feature-construction-order-crash-destruction] */
}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
}

{
SceneGraph::Object<SceneGraph::MatrixTransformation3D> object;
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif
/* [AbstractObject-features-range] */
for(SceneGraph::AbstractFeature3D& feature: object.features()) {
    // ...
}
/* [AbstractObject-features-range] */

{
/* [Object-children-range] */
Object3D o;
for(Object3D& child: o.children()) {
    // ...
}
/* [Object-children-range] */
}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

/* [AbstractObject-features] */
for(SceneGraph::AbstractFeature3D* feature = object.features().first(); feature; feature = feature->nextFeature()) {
    // ...
}
/* [AbstractObject-features] */

{
Object3D o;
/* [Object-children] */
for(Object3D* child = o.children().first(); child; child = child->nextSibling()) {
    // ...
}
/* [Object-children] */
}

{
/* [Animable-usage] */
Scene3D scene;
SceneGraph::AnimableGroup3D animables;

(new AnimableObject(&scene, &animables))
    ->setState(SceneGraph::AnimationState::Running);
// ...
/* [Animable-usage] */
}

{
SceneGraph::Object<SceneGraph::MatrixTransformation2D> cameraObject;
/* [Camera-2D] */
SceneGraph::Camera2D camera{cameraObject};
camera.setProjectionMatrix(Matrix3::projection({4.0f/3.0f, 1.0f}))
      .setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend);
/* [Camera-2D] */
}

{
SceneGraph::Object<SceneGraph::MatrixTransformation3D> cameraObject;
/* [Camera-3D] */
SceneGraph::Camera3D camera{cameraObject};
camera.setProjectionMatrix(Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.001f, 100.0f))
      .setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend);
/* [Camera-3D] */
}

}

{
Object3D cameraObject;
SceneGraph::Camera3D camera{cameraObject};
SceneGraph::DrawableGroup3D drawableGroup;
/* [Drawable-draw-order] */
std::vector<std::pair<std::reference_wrapper<SceneGraph::Drawable3D>, Matrix4>>
    drawableTransformations = camera.drawableTransformations(drawableGroup);

std::sort(drawableTransformations.begin(), drawableTransformations.end(),
    [](const std::pair<std::reference_wrapper<SceneGraph::Drawable3D>, Matrix4>& a,
       const std::pair<std::reference_wrapper<SceneGraph::Drawable3D>, Matrix4>& b) {
        return a.second.translation().z() < b.second.translation().z();
    });

camera.draw(drawableTransformations);
/* [Drawable-draw-order] */
}

{
Object3D cameraObject;
SceneGraph::Camera3D camera{cameraObject};
SceneGraph::DrawableGroup3D drawableGroup;
/* [Drawable-culling] */
struct CullableDrawable3D: SceneGraph::Drawable3D {
    Range3D aabb; /* Relative to world origin */

    // ...
};

/* Camera frustum relative to world origin */
auto frustum = Frustum::fromMatrix(camera.projectionMatrix()*camera.cameraMatrix());

/* Erase all items that don't pass the frustum check */
std::vector<std::pair<std::reference_wrapper<SceneGraph::Drawable3D>, Matrix4>>
    drawableTransformations = camera.drawableTransformations(drawableGroup);
drawableTransformations.erase(std::remove_if(
    drawableTransformations.begin(), drawableTransformations.end(),
    [&](const std::pair<std::reference_wrapper<SceneGraph::Drawable3D>, Matrix4>& a) {
        Range3D aabb = static_cast<CullableDrawable3D&>(a.first.get()).aabb;
        return !Math::Intersection::rangeFrustum(aabb, frustum);
    }),
    drawableTransformations.end());

/* Draw just the visible part */
camera.draw(drawableTransformations);
/* [Drawable-culling] */
}

}
