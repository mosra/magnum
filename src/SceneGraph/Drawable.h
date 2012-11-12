#ifndef Magnum_SceneGraph_Drawable_h
#define Magnum_SceneGraph_Drawable_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Class Magnum::SceneGraph::Drawable, Magnum::SceneGraph::DrawableGroup, alias Magnum::SceneGraph::Drawable2D, Magnum::SceneGraph::Drawable3D, Magnum::SceneGraph::DrawableGroup2D, Magnum::SceneGraph::DrawableGroup3D
 */

#include "AbstractGroupedFeature.h"

namespace Magnum { namespace SceneGraph {

/**
@brief %Drawable

Adds drawing function to object. Each %Drawable is part of some DrawableGroup
and the whole group is drawn with particular camera using AbstractCamera::draw().

@section Drawable-usage Usage

First thing is add Drawable feature to some object and implement draw(). You
can do it conveniently using multiple inheritance (see @ref scenegraph-features
for introduction). Example:
@code
typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D<>> Object3D;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D<>> Scene3D;

class DrawableObject: public Object3D, SceneGraph::Drawable3D<> {
    public:
        DrawableObject(Object* parent = nullptr, SceneGraph::DrawableGroup3D<>* group = nullptr): Object3D(parent), SceneGraph::Drawable3D<>(this, group) {
            // ...
        }

        void draw(const Matrix4& transformationMatrix, AbstractCamera3D<>* camera) override {
            // ...
        }
}
@endcode

Then you add these objects to your scene and some drawable group and transform
them as you like. You can also use DrawableGroup::add() and
DrawableGroup::remove() for that.
@code
Scene3D scene;
SceneGraph::DrawableGroup3D<> group;

(new DrawableObject(&scene, &group))
    ->translate(Vector3::yAxis(-0.3f))
    ->rotateX(deg(30.0f));
(new AnotherDrawableObject(&scene, &group))
    ->translate(Vector3::zAxis(0.5f));
// ...
@endcode

The last thing you need is Camera attached to some object (thus using its
transformation) and with it you can perform drawing in your draw event:
@code
Camera3D<> camera(&cameraObject);

void MyApplication::drawEvent() {
    camera.draw(&group);
}
@endcode

@see Drawable2D, Drawable3D, DrawableGroup2D, DrawableGroup3D
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<std::uint8_t dimensions, class T>
#else
template<std::uint8_t dimensions, class T = GLfloat>
#endif
class Drawable: public AbstractGroupedFeature<dimensions, Drawable<dimensions, T>, T> {
    public:
        /** @copydoc AbstractGroupedFeature::AbstractGroupedFeature() */
        inline Drawable(AbstractObject<dimensions, T>* object, DrawableGroup<dimensions, T>* group = nullptr): AbstractGroupedFeature<dimensions, Drawable<dimensions, T>, T>(object, group) {}

        /**
         * @brief Draw the object using given camera
         * @param transformationMatrix      %Object transformation relative
         *      to camera
         * @param camera                    Camera
         *
         * Projection matrix can be retrieved from AbstractCamera::projectionMatrix().
         */
        virtual void draw(const typename DimensionTraits<dimensions, T>::MatrixType& transformationMatrix, AbstractCamera<dimensions, T>* camera) = 0;
};

/**
@brief Two-dimensional drawable

Convenience alternative to <tt>%Drawable<2, T></tt>. See Drawable for more
information.
@note Not available on GCC < 4.7. Use <tt>%Drawable<2, T></tt> instead.
@see Drawable3D
@todoc Remove workaround when Doxygen supports alias template
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_GCC46_COMPATIBILITY
template<class T = GLfloat> using Drawable2D = Drawable<2, T>;
#endif
#else
typedef Drawable<2, T = GLfloat> Drawable2D;
#endif

/**
@brief Three-dimensional drawable

Convenience alternative to <tt>%Drawable<3, T></tt>. See Drawable for more
information.
@note Not available on GCC < 4.7. Use <tt>%Drawable<3, T></tt> instead.
@see Drawable2D
@todoc Remove workaround when Doxygen supports alias template
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_GCC46_COMPATIBILITY
template<class T = GLfloat> using Drawable3D = Drawable<3, T>;
#endif
#else
typedef Drawable<3, T = GLfloat> Drawable3D;
#endif

/**
@brief Group of drawables

See Drawable for more information.
@see DrawableGroup2D, DrawableGroup3D
@todoc Remove workaround when Doxygen supports alias template
*/
#if !defined(MAGNUM_GCC46_COMPATIBILITY) && !defined(DOXYGEN_GENERATING_OUTPUT)
template<std::uint8_t dimensions, class T = GLfloat> using DrawableGroup = FeatureGroup<dimensions, Drawable<dimensions, T>, T>;
#else
#ifndef DOXYGEN_GENERATING_OUTPUT
template<std::uint8_t dimensions, class T>
#else
template<std::uint8_t dimensions, class T = GLfloat>
#endif
class DrawableGroup: public FeatureGroup<dimensions, Drawable<dimensions, T>, T> {};
#endif

/**
@brief Group of two-dimensional drawables

Convenience alternative to <tt>%DrawableGroup<2, T></tt>. See Drawable for
more information.
@note Not available on GCC < 4.7. Use <tt>%Drawable<2, T></tt> instead.
@see DrawableGroup3D
@todoc Remove workaround when Doxygen supports alias template
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_GCC46_COMPATIBILITY
template<class T = GLfloat> using DrawableGroup2D = DrawableGroup<2, T>;
#endif
#else
typedef DrawableGroup<2, T = GLfloat> DrawableGroup2D;
#endif

/**
@brief Group of three-dimensional drawables

Convenience alternative to <tt>%DrawableGroup<3, T></tt>. See Drawable for
more information.
@note Not available on GCC < 4.7. Use <tt>%Drawable<3, T></tt> instead.
@see DrawableGroup2D
@todoc Remove workaround when Doxygen supports alias template
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_GCC46_COMPATIBILITY
template<class T = GLfloat> using DrawableGroup3D = DrawableGroup<3, T>;
#endif
#else
typedef DrawableGroup<3, T = GLfloat> DrawableGroup3D;
#endif

}}

#endif
