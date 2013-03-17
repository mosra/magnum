#ifndef Magnum_SceneGraph_SceneGraph_h
#define Magnum_SceneGraph_SceneGraph_h
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
 * @brief Forward declarations for Magnum::SceneGraph namespace
 */

#include "Types.h"

#include "corradeCompatibility.h"

namespace Magnum { namespace SceneGraph {

/** @todoc remove when doxygen is sane again */
#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef CORRADE_GCC45_COMPATIBILITY
enum class AspectRatioPolicy: UnsignedByte;
#endif

template<UnsignedInt dimensions, class T = Float> class AbstractCamera;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class T = Float> using AbstractCamera2D = AbstractCamera<2, T>;
template<class T = Float> using AbstractCamera3D = AbstractCamera<3, T>;
#endif

template<UnsignedInt dimensions, class T = Float> class AbstractFeature;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class T = Float> using AbstractFeature2D = AbstractFeature<2, T>;
template<class T = Float> using AbstractFeature3D = AbstractFeature<3, T>;
#endif

template<UnsignedInt dimensions, class Derived, class T = Float> class AbstractGroupedFeature;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class Derived, class T = Float> using AbstractGroupedFeature2D = AbstractGroupedFeature<2, Derived, T>;
template<class Derived, class T = Float> using AbstractGroupedFeature3D = AbstractGroupedFeature<3, Derived, T>;
#endif

template<UnsignedInt dimensions, class T = Float> class AbstractObject;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class T = Float> using AbstractObject2D = AbstractObject<2, T>;
template<class T = Float> using AbstractObject3D = AbstractObject<3, T>;
#endif

#ifndef CORRADE_GCC45_COMPATIBILITY
enum class TransformationType: UnsignedByte;
#endif

template<UnsignedInt dimensions, class T = Float> class AbstractTransformation;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class T = Float> using AbstractTransformation2D = AbstractTransformation<2, T>;
template<class T = Float> using AbstractTransformation3D = AbstractTransformation<3, T>;
#endif

template<class T = Float> class AbstractTranslationRotation2D;
template<class T = Float> class AbstractTranslationRotation3D;
template<class T = Float> class AbstractTranslationRotationScaling2D;
template<class T = Float> class AbstractTranslationRotationScaling3D;

template<UnsignedInt dimensions, class T = Float> class Animable;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class T = Float> using Animable2D = Animable<2, T>;
template<class T = Float> using Animable3D = Animable<3, T>;
#endif

enum class AnimationState: UnsignedByte;

template<UnsignedInt dimensions, class T = Float> class AnimableGroup;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class T = Float> using AnimableGroup2D = AnimableGroup<2, T>;
template<class T = Float> using AnimableGroup3D = AnimableGroup<3, T>;
#endif

template<class T = Float> class Camera2D;
template<class T = Float> class Camera3D;

template<UnsignedInt dimensions, class T = Float> class Drawable;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class T = Float> using Drawable2D = Drawable<2, T>;
template<class T = Float> using Drawable3D = Drawable<3, T>;
#endif

template<class T = Float> class DualComplexTransformation;
template<class T = Float> class DualQuaternionTransformation;

template<UnsignedInt dimensions, class Feature, class T = Float> class FeatureGroup;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class Feature, class T = Float> using FeatureGroup2D = FeatureGroup<2, Feature, T>;
template<class Feature, class T = Float> using FeatureGroup3D = FeatureGroup<3, Feature, T>;
#endif

#ifndef CORRADE_GCC46_COMPATIBILITY
template<UnsignedInt dimensions, class T = Float> using DrawableGroup = FeatureGroup<dimensions, Drawable<dimensions, T>, T>;
template<class T = Float> using DrawableGroup2D = DrawableGroup<2, T>;
template<class T = Float> using DrawableGroup3D = DrawableGroup<3, T>;
#else
template<UnsignedInt dimensions, class T = Float> class DrawableGroup;
#endif

template<class T = Float> class MatrixTransformation2D;
template<class T = Float> class MatrixTransformation3D;

template<class Transformation> class Object;

template<class T = Float> class RigidMatrixTransformation2D;
template<class T = Float> class RigidMatrixTransformation3D;

template<class Transformation> class Scene;
#endif

}}

#endif
