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

#ifndef CORRADE_GCC45_COMPATIBILITY
enum class AspectRatioPolicy: UnsignedByte;
#endif

template<UnsignedInt, class> class AbstractCamera;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class T> using AbstractBasicCamera2D = AbstractCamera<2, T>;
template<class T> using AbstractBasicCamera3D = AbstractCamera<3, T>;
typedef AbstractBasicCamera2D<Float> AbstractCamera2D;
typedef AbstractBasicCamera3D<Float> AbstractCamera3D;
#else
typedef AbstractCamera<2, Float> AbstractCamera2D;
typedef AbstractCamera<3, Float> AbstractCamera3D;
#endif

/* Enum CachedTransformation and CachedTransformations used only directly */

template<UnsignedInt, class> class AbstractFeature;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class T> using AbstractBasicFeature2D = AbstractFeature<2, T>;
template<class T> using AbstractBasicFeature3D = AbstractFeature<3, T>;
typedef AbstractBasicFeature2D<Float> AbstractFeature2D;
typedef AbstractBasicFeature3D<Float> AbstractFeature3D;
#else
typedef AbstractFeature<2, Float> AbstractFeature2D;
typedef AbstractFeature<3, Float> AbstractFeature3D;
#endif

/* AbstractFeatureGroup shouldn't be used directly */

template<UnsignedInt, class, class> class AbstractGroupedFeature;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class Derived, class T> using AbstractBasicGroupedFeature2D = AbstractGroupedFeature<2, Derived, T>;
template<class Derived, class T> using AbstractBasicGroupedFeature3D = AbstractGroupedFeature<3, Derived, T>;
template<class Derived> using AbstractGroupedFeature2D = AbstractBasicGroupedFeature2D<Derived, Float>;
template<class Derived> using AbstractGroupedFeature3D = AbstractBasicGroupedFeature3D<Derived, Float>;
#endif

template<UnsignedInt, class> class AbstractObject;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class T> using AbstractBasicObject2D = AbstractObject<2, T>;
template<class T> using AbstractBasicObject3D = AbstractObject<3, T>;
typedef AbstractBasicObject2D<Float> AbstractObject2D;
typedef AbstractBasicObject3D<Float> AbstractObject3D;
#else
typedef AbstractObject<2, Float> AbstractObject2D;
typedef AbstractObject<3, Float> AbstractObject3D;
#endif

#ifndef CORRADE_GCC45_COMPATIBILITY
enum class TransformationType: UnsignedByte;
#endif

template<UnsignedInt, class> class AbstractTransformation;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class T> using AbstractBasicTransformation2D = AbstractTransformation<2, T>;
template<class T> using AbstractBasicTransformation3D = AbstractTransformation<3, T>;
typedef AbstractBasicTransformation2D<Float> AbstractTransformation2D;
typedef AbstractBasicTransformation3D<Float> AbstractTransformation3D;
#else
typedef AbstractTransformation<2, Float> AbstractTransformation2D;
typedef AbstractTransformation<3, Float> AbstractTransformation3D;
#endif

template<UnsignedInt, class T, class = T> class AbstractTranslation;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class T, class TranslationType = T> using AbstractBasicTranslation2D = AbstractTranslation<2, T, TranslationType>;
template<class T, class TranslationType = T> using AbstractBasicTranslation3D = AbstractTranslation<3, T, TranslationType>;
typedef AbstractBasicTranslation2D<Float> AbstractTranslation2D;
typedef AbstractBasicTranslation3D<Float> AbstractTranslation3D;
#else
typedef AbstractTranslation<2, Float> AbstractTranslation2D;
typedef AbstractTranslation<3, Float> AbstractTranslation3D;
#endif

template<class> class AbstractBasicTranslationRotation2D;
template<class> class AbstractBasicTranslationRotation3D;
typedef AbstractBasicTranslationRotation2D<Float> AbstractTranslationRotation2D;
typedef AbstractBasicTranslationRotation3D<Float> AbstractTranslationRotation3D;

template<class> class AbstractBasicTranslationRotationScaling2D;
template<class> class AbstractBasicTranslationRotationScaling3D;
typedef AbstractBasicTranslationRotationScaling2D<Float> AbstractTranslationRotationScaling2D;
typedef AbstractBasicTranslationRotationScaling3D<Float> AbstractTranslationRotationScaling3D;

template<UnsignedInt, class> class Animable;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class T> using BasicAnimable2D = Animable<2, T>;
template<class T> using BasicAnimable3D = Animable<3, T>;
typedef BasicAnimable2D<Float> Animable2D;
typedef BasicAnimable3D<Float> Animable3D;
#else
typedef Animable<2, Float> Animable2D;
typedef Animable<3, Float> Animable3D;
#endif

#ifndef CORRADE_GCC45_COMPATIBILITY
enum class AnimationState: UnsignedByte;
#endif

template<UnsignedInt, class> class AnimableGroup;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class T> using BasicAnimableGroup2D = AnimableGroup<2, T>;
template<class T> using BasicAnimableGroup3D = AnimableGroup<3, T>;
typedef BasicAnimableGroup2D<Float> AnimableGroup2D;
typedef BasicAnimableGroup3D<Float> AnimableGroup3D;
#else
typedef AnimableGroup<2, Float> AnimableGroup2D;
typedef AnimableGroup<3, Float> AnimableGroup3D;
#endif

template<class> class BasicCamera2D;
template<class> class BasicCamera3D;
typedef BasicCamera2D<Float> Camera2D;
typedef BasicCamera3D<Float> Camera3D;

template<UnsignedInt, class> class Drawable;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class T> using BasicDrawable2D = Drawable<2, T>;
template<class T> using BasicDrawable3D = Drawable<3, T>;
typedef BasicDrawable2D<Float> Drawable2D;
typedef BasicDrawable3D<Float> Drawable3D;
#endif

template<class> class BasicDualComplexTransformation;
template<class> class BasicDualQuaternionTransformation;
typedef BasicDualComplexTransformation<Float> DualComplexTransformation;
typedef BasicDualQuaternionTransformation<Float> DualQuaternionTransformation;

template<UnsignedInt, class, class> class FeatureGroup;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class Feature, class T> using BasicFeatureGroup2D = FeatureGroup<2, Feature, T>;
template<class Feature, class T> using BasicFeatureGroup3D = FeatureGroup<3, Feature, T>;
template<class Feature> using FeatureGroup2D = BasicFeatureGroup2D<Feature, Float>;
template<class Feature> using FeatureGroup3D = BasicFeatureGroup3D<Feature, Float>;
#endif

#ifndef CORRADE_GCC46_COMPATIBILITY
template<UnsignedInt dimensions, class T> using DrawableGroup = FeatureGroup<dimensions, Drawable<dimensions, T>, T>;
template<class T> using BasicDrawableGroup2D = DrawableGroup<2, T>;
template<class T> using BasicDrawableGroup3D = DrawableGroup<3, T>;
typedef BasicDrawableGroup2D<Float> DrawableGroup2D;
typedef BasicDrawableGroup3D<Float> DrawableGroup3D;
#else
template<UnsignedInt, class> class DrawableGroup;
typedef DrawableGroup<2, Float> DrawableGroup2D;
typedef DrawableGroup<3, Float> DrawableGroup3D;
#endif

template<class> class BasicMatrixTransformation2D;
template<class> class BasicMatrixTransformation3D;
typedef BasicMatrixTransformation2D<Float> MatrixTransformation2D;
typedef BasicMatrixTransformation3D<Float> MatrixTransformation3D;

template<class Transformation> class Object;

template<class> class BasicRigidMatrixTransformation2D;
template<class> class BasicRigidMatrixTransformation3D;
typedef BasicRigidMatrixTransformation2D<Float> RigidMatrixTransformation2D;
typedef BasicRigidMatrixTransformation3D<Float> RigidMatrixTransformation3D;

template<class Transformation> class Scene;

template<UnsignedInt, class T, class = T> class TranslationTransformation;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class T, class TranslationType = T> using BasicTranslationTransformation2D = TranslationTransformation<2, T, TranslationType>;
template<class T, class TranslationType = T> using BasicTranslationTransformation3D = TranslationTransformation<3, T, TranslationType>;
typedef BasicTranslationTransformation2D<Float> TranslationTransformation2D;
typedef BasicTranslationTransformation3D<Float> TranslationTransformation3D;
#else
typedef TranslationTransformation<2, Float> TranslationTransformation2D;
typedef TranslationTransformation<3, Float> TranslationTransformation3D;
#endif

namespace Implementation {
    template<class> struct Transformation;
}

}}

#endif
