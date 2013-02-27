#ifndef Magnum_SceneGraph_SceneGraph_h
#define Magnum_SceneGraph_SceneGraph_h
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
 * @brief Forward declarations for Magnum::SceneGraph namespace
 */

#include "Types.h"

#include "corradeCompatibility.h"

namespace Magnum { namespace SceneGraph {

/** @todoc remove when doxygen is sane again */
#ifndef DOXYGEN_GENERATING_OUTPUT
enum class AspectRatioPolicy: UnsignedByte;

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

template<class T = Float> class EuclideanMatrixTransformation2D;
template<class T = Float> class EuclideanMatrixTransformation3D;

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
template<class Transformation> class Scene;
#endif

}}

#endif
