#ifndef Magnum_SceneGraph_Drawable_h
#define Magnum_SceneGraph_Drawable_h
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

/** @file
 * @brief Class @ref Magnum::SceneGraph::Drawable, @ref Magnum::SceneGraph::DrawableGroup, alias @ref Magnum::SceneGraph::BasicDrawable2D, @ref Magnum::SceneGraph::BasicDrawable3D, @ref Magnum::SceneGraph::BasicDrawableGroup2D, @ref Magnum::SceneGraph::BasicDrawableGroup3D, typedef @ref Magnum::SceneGraph::Drawable2D, @ref Magnum::SceneGraph::Drawable3D, @ref Magnum::SceneGraph::DrawableGroup2D, @ref Magnum::SceneGraph::DrawableGroup3D
 */

#include "Magnum/SceneGraph/AbstractGroupedFeature.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Drawable

Adds drawing functionality to the object. Each Drawable is part of some
@ref DrawableGroup and the whole group can be drawn with particular camera
using @ref Camera::draw().

@section SceneGraph-Drawable-subclassing Subclassing

The class is used via subclassing and implementing the @ref draw() function.
The simplest option is to do it via single inheritance. Example drawable object
that draws a blue sphere:

@snippet MagnumSceneGraph-gl.cpp Drawable-usage

For brevity the class has its own shader and mesh instance and a hardcoded
light position, usually you'll have them stored in a central location, shared
by multiple objects, and pass only references around.

The @p transformationMatrix parameter in the @ref draw() function contains
transformation of the object (to which the drawable is attached) relative to
@p camera. The camera contains the projection matrix. Some shaders (like the
@ref Shaders::Phong used in the snippet) have separate functions for setting
transformation and projection matrix, but some (such as @ref Shaders::Flat)
have a single function to set composite transformation and projection matrix.
In that case you need to combine the two matrices manually like in the
following code. Some shaders might have additional requirements, see their
respective documentation for details.

@snippet MagnumSceneGraph-gl.cpp Drawable-usage-shader

@section SceneGraph-Drawable-usage Drawing the scene

There is no way to just draw all the drawables in the scene, you need to create
some drawable group and add the drawable objects to both the scene and the
group. You can also use @ref DrawableGroup::add() and
@ref DrawableGroup::remove() instead of passing the group in the constructor.
If you don't need to change any properties of the drawable later, you can just
"create and forget", the scene graph will take care of all memory management
from there, deleting the drawable when the object is attached to is deleted.

@snippet MagnumSceneGraph-gl.cpp Drawable-usage-instance

The last thing you need is a camera attached to some object (thus using its
transformation). With the camera and the drawable group you can perform drawing
in your @ref Platform::Sdl2Application::drawEvent() "drawEvent()"
implementation. See @ref Camera2D and @ref Camera3D documentation for more
information.

@snippet MagnumSceneGraph-gl.cpp Drawable-usage-camera

@section SceneGraph-Drawable-multiple-inheritance Using multiple inheritance

Besides single inheritance, it's also possible to derive your class from both
@ref Object and @ref Drawable. This for example allows you to directly access
object transformation and parent/child relationship from within the drawable
object.

@snippet MagnumSceneGraph-gl.cpp Drawable-usage-multiple-inheritance

The @ref draw() implementation is the same as in the above snippet. Note that,
in contrast to the single inheritance case, where the @ref Drawable constructor
got the holder @cpp object @ce, here we pass it @cpp *this @ce, because this
class is both the holder object and the drawable. Instantiating the drawable
object is then done in a single step:

@snippet MagnumSceneGraph-gl.cpp Drawable-usage-instance-multiple-inheritance

@section SceneGraph-Drawable-multiple-groups Using multiple drawable groups to improve performance

You can organize your drawables to multiple groups to minimize OpenGL state
changes --- for example put all objects using the same shader, the same light
setup etc into one group, then put all transparent into another and set common
parameters once for whole group instead of setting them again in each
@ref draw() implementation. Example:

@snippet MagnumSceneGraph-gl.cpp Drawable-multiple-groups

@section SceneGraph-Drawable-draw-order Custom draw order and object culling

By default all contents of a drawable group are drawn, in the order they were
added. In some cases you may want to draw them in a different order (for
example to have correctly sorted transparent objects) or draw just a subset
(for example to cull invisible objects away). That can be achieved using
@ref Camera::drawableTransformations() in combination with
@ref Camera::draw(const std::vector<std::pair<std::reference_wrapper<Drawable<dimensions, T>>, MatrixTypeFor<dimensions, T>>>&).
For example, to have the objects sorted back-to-front, apply @ref std::sort()
with a custom predicate on the drawable transformation list:

@snippet MagnumSceneGraph.cpp Drawable-draw-order

Another use case is object-level culling --- assuming each drawable instance
provides an *absolute* AABB, one can calculate the transformations, cull them
via e.g. @ref Math::Intersection::rangeFrustum() and then pass the filtered
vector to @ref Camera::draw(). To be clear, this approach depends on AABBs
provided as relative to world origin, the actual object transformations don't
get used in any way except being passed to the draw function:

@snippet MagnumSceneGraph.cpp Drawable-culling

@section SceneGraph-Drawable-explicit-specializations Explicit template specializations

The following specializations are explicitly compiled into @ref SceneGraph
library. For other specializations (e.g. using @ref Magnum::Double "Double"
type) you have to use @ref Drawable.hpp implementation file to avoid linker
errors. See also @ref compilation-speedup-hpp for more information.

-   @ref Drawable2D
-   @ref Drawable3D

@see @ref scenegraph, @ref BasicDrawable2D, @ref BasicDrawable3D,
    @ref Drawable2D, @ref Drawable3D, @ref DrawableGroup
*/
template<UnsignedInt dimensions, class T> class Drawable: public AbstractGroupedFeature<dimensions, Drawable<dimensions, T>, T> {
    public:
        /**
         * @brief Constructor
         * @param object    Object this drawable belongs to
         * @param drawables Group this drawable belongs to
         *
         * Adds the feature to the object and also to the group, if specified.
         * Otherwise you can use @ref DrawableGroup::add().
         */
        explicit Drawable(AbstractObject<dimensions, T>& object, DrawableGroup<dimensions, T>* drawables = nullptr);

        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* This is here to avoid ambiguity with deleted copy constructor when
           passing `*this` from class subclassing both Drawable and AbstractObject */
        template<class U, class = typename std::enable_if<std::is_base_of<AbstractObject<dimensions, T>, U>::value>::type> explicit Drawable(U& object): Drawable<dimensions, T>{static_cast<AbstractObject<dimensions, T>&>(object)} {}
        #endif

        /**
         * @brief Group containing this drawable
         *
         * If the drawable doesn't belong to any group, returns @cpp nullptr @ce.
         */
        DrawableGroup<dimensions, T>* drawables() {
            return AbstractGroupedFeature<dimensions, Drawable<dimensions, T>, T>::group();
        }

        /** @overload */
        const DrawableGroup<dimensions, T>* drawables() const {
            return AbstractGroupedFeature<dimensions, Drawable<dimensions, T>, T>::group();
        }

        /**
         * @brief Draw the object using given camera
         * @param transformationMatrix  Object transformation relative to camera
         * @param camera                Camera
         *
         * Projection matrix can be retrieved from
         * @ref SceneGraph::Camera::projectionMatrix() "Camera::projectionMatrix()".
         */
        virtual void draw(const MatrixTypeFor<dimensions, T>& transformationMatrix, Camera<dimensions, T>& camera) = 0;
};

/**
@brief Drawable for two-dimensional scenes

Convenience alternative to @cpp Drawable<2, T> @ce. See @ref Drawable for more
information.
@see @ref Drawable2D, @ref BasicDrawable3D
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using BasicDrawable2D = Drawable<2, T>;
#endif

/**
@brief Drawable for two-dimensional float scenes

@see @ref Drawable3D
*/
typedef BasicDrawable2D<Float> Drawable2D;

/**
@brief Drawable for three-dimensional scenes

Convenience alternative to @cpp Drawable<3, T> @ce. See @ref Drawable for more
information.
@see @ref Drawable3D, @ref BasicDrawable3D
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using BasicDrawable3D = Drawable<3, T>;
#endif

/**
@brief Drawable for three-dimensional float scenes

@see @ref Drawable2D
*/
typedef BasicDrawable3D<Float> Drawable3D;

/**
@brief Group of drawables

See @ref Drawable for more information.
@see @ref scenegraph, @ref BasicDrawableGroup2D, @ref BasicDrawableGroup3D,
    @ref DrawableGroup2D, @ref DrawableGroup3D
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<UnsignedInt dimensions, class T> using DrawableGroup = FeatureGroup<dimensions, Drawable<dimensions, T>, T>;
#endif

/**
@brief Group of drawables for two-dimensional scenes

Convenience alternative to @cpp DrawableGroup<2, T> @ce. See @ref Drawable for
more information.
@see @ref DrawableGroup2D, @ref BasicDrawableGroup3D
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using BasicDrawableGroup2D = DrawableGroup<2, T>;
#endif

/**
@brief Group of drawables for two-dimensional float scenes

@see @ref DrawableGroup3D
*/
typedef BasicDrawableGroup2D<Float> DrawableGroup2D;

/**
@brief Group of drawables for three-dimensional scenes

Convenience alternative to @cpp DrawableGroup<3, T> @ce. See @ref Drawable for
more information.
@see @ref DrawableGroup3D, @ref BasicDrawableGroup2D
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using BasicDrawableGroup3D = DrawableGroup<3, T>;
#endif

/**
@brief Group of drawables for three-dimensional float scenes

@see @ref DrawableGroup2D
*/
typedef BasicDrawableGroup3D<Float> DrawableGroup3D;

#if defined(CORRADE_TARGET_WINDOWS) && !(defined(CORRADE_TARGET_MINGW) && !defined(CORRADE_TARGET_CLANG))
extern template class MAGNUM_SCENEGRAPH_EXPORT Drawable<2, Float>;
extern template class MAGNUM_SCENEGRAPH_EXPORT Drawable<3, Float>;
#endif
}}

#endif
