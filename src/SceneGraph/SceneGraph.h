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
enum class AspectRatioPolicy: UnsignedByte;

template<UnsignedInt, class> class AbstractBasicCamera;
typedef AbstractBasicCamera<2, Float> AbstractCamera2D;
typedef AbstractBasicCamera<3, Float> AbstractCamera3D;

/* Enum CachedTransformation and CachedTransformations used only directly */

template<UnsignedInt, class> class AbstractBasicFeature;
typedef AbstractBasicFeature<2, Float> AbstractFeature2D;
typedef AbstractBasicFeature<3, Float> AbstractFeature3D;

template<UnsignedInt, class> class AbstractBasicFeatureGroup;
typedef AbstractBasicFeatureGroup<2, Float> AbstractFeatureGroup2D;
typedef AbstractBasicFeatureGroup<3, Float> AbstractFeatureGroup3D;

template<UnsignedInt dimensions, class Derived, class T> class AbstractBasicGroupedFeature;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class Derived> using AbstractGroupedFeature2D = AbstractBasicGroupedFeature<2, Derived, Float>;
template<class Derived> using AbstractGroupedFeature3D = AbstractBasicGroupedFeature<3, Derived, Float>;
#endif

template<UnsignedInt dimensions, class> class AbstractBasicObject;
typedef AbstractBasicObject<2, Float> AbstractObject2D;
typedef AbstractBasicObject<3, Float> AbstractObject3D;

enum class TransformationType: UnsignedByte;

template<UnsignedInt dimensions, class T = Float> class AbstractTransformation;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class T = Float> using AbstractTransformation2D = AbstractTransformation<2, T>;
template<class T = Float> using AbstractTransformation3D = AbstractTransformation<3, T>;
#endif

template<class T = Float> class AbstractTranslationRotation2D;
template<class T = Float> class AbstractTranslationRotation3D;
template<class T = Float> class AbstractTranslationRotationScaling2D;
template<class T = Float> class AbstractTranslationRotationScaling3D;

template<UnsignedInt, class> class BasicAnimable;
typedef BasicAnimable<2, Float> Animable2D;
typedef BasicAnimable<3, Float> Animable3D;

enum class AnimationState: UnsignedByte;

template<UnsignedInt, class> class BasicAnimableGroup;
typedef BasicAnimableGroup<2, Float> AnimableGroup2D;
typedef BasicAnimableGroup<3, Float> AnimableGroup3D;

template<class> class BasicCamera2D;
template<class> class BasicCamera3D;
typedef BasicCamera2D<Float> Camera2D;
typedef BasicCamera3D<Float> Camera3D;

template<UnsignedInt, class> class BasicDrawable;
typedef BasicDrawable<2, Float> Drawable2D;
typedef BasicDrawable<3, Float> Drawable3D;

template<class T = Float> class DualComplexTransformation;
template<class T = Float> class DualQuaternionTransformation;

template<UnsignedInt dimensions, class Feature, class T> class BasicFeatureGroup;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class Feature> using BasicFeatureGroup2D = BasicFeatureGroup<2, Feature, Float>;
template<class Feature> using BasicFeatureGroup3D = BasicFeatureGroup<3, Feature, Float>;
#endif

#ifndef CORRADE_GCC46_COMPATIBILITY
template<UnsignedInt dimensions, class T> using BasicDrawableGroup = BasicFeatureGroup<dimensions, BasicDrawable<dimensions, T>, T>;
#else
template<UnsignedInt, class> class BasicDrawableGroup;
#endif
typedef BasicDrawableGroup<2, Float> DrawableGroup2D;
typedef BasicDrawableGroup<3, Float> DrawableGroup3D;

template<class T = Float> class MatrixTransformation2D;
template<class T = Float> class MatrixTransformation3D;

template<class Transformation> class Object;

template<class T = Float> class RigidMatrixTransformation2D;
template<class T = Float> class RigidMatrixTransformation3D;

template<class Transformation> class Scene;
#endif

}}

#endif
