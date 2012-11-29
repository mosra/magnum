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

#include <cstdint>

#include "Magnum.h"

#include "corradeCompatibility.h"

namespace Magnum { namespace SceneGraph {

#ifndef CORRADE_GCC45_COMPATIBILITY
enum class AspectRatioPolicy: std::uint8_t;
#endif

template<std::uint8_t dimensions, class T = GLfloat> class AbstractCamera;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class T = GLfloat> using AbstractCamera2D = AbstractCamera<2, T>;
template<class T = GLfloat> using AbstractCamera3D = AbstractCamera<3, T>;
#endif

template<std::uint8_t dimensions, class T = GLfloat> class AbstractFeature;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class T = GLfloat> using AbstractFeature2D = AbstractFeature<2, T>;
template<class T = GLfloat> using AbstractFeature3D = AbstractFeature<3, T>;
#endif

template<std::uint8_t dimensions, class Derived, class T = GLfloat> class AbstractGroupedFeature;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class Derived, class T = GLfloat> using AbstractGroupedFeature2D = AbstractGroupedFeature<2, Derived, T>;
template<class Derived, class T = GLfloat> using AbstractGroupedFeature3D = AbstractGroupedFeature<3, Derived, T>;
#endif

template<std::uint8_t dimensions, class T = GLfloat> class AbstractObject;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class T = GLfloat> using AbstractObject2D = AbstractObject<2, T>;
template<class T = GLfloat> using AbstractObject3D = AbstractObject<3, T>;
#endif

#ifndef CORRADE_GCC45_COMPATIBILITY
enum class TransformationType: std::uint8_t;
#endif

template<std::uint8_t dimensions, class T = GLfloat> class AbstractTransformation;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class T = GLfloat> using AbstractTransformation2D = AbstractTransformation<2, T>;
template<class T = GLfloat> using AbstractTransformation3D = AbstractTransformation<3, T>;
#endif

template<class T = GLfloat> class AbstractTranslationRotation2D;
template<class T = GLfloat> class AbstractTranslationRotation3D;
template<class T = GLfloat> class AbstractTranslationRotationScaling2D;
template<class T = GLfloat> class AbstractTranslationRotationScaling3D;

template<class T = GLfloat> class Camera2D;
template<class T = GLfloat> class Camera3D;

template<std::uint8_t dimensions, class T = GLfloat> class Drawable;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class T = GLfloat> using Drawable2D = Drawable<2, T>;
template<class T = GLfloat> using Drawable3D = Drawable<3, T>;
#endif

template<std::uint8_t dimensions, class Feature, class T = GLfloat> class FeatureGroup;
#ifndef CORRADE_GCC46_COMPATIBILITY
template<class Feature, class T = GLfloat> using FeatureGroup2D = FeatureGroup<2, Feature, T>;
template<class Feature, class T = GLfloat> using FeatureGroup3D = FeatureGroup<3, Feature, T>;
#endif

#ifndef CORRADE_GCC46_COMPATIBILITY
template<std::uint8_t dimensions, class T = GLfloat> using DrawableGroup = FeatureGroup<dimensions, Drawable<dimensions, T>, T>;
template<class T = GLfloat> using DrawableGroup2D = DrawableGroup<2, T>;
template<class T = GLfloat> using DrawableGroup3D = DrawableGroup<3, T>;
#else
template<std::uint8_t dimensions, class T = GLfloat> class DrawableGroup;
#endif

template<class T = GLfloat> class MatrixTransformation2D;
template<class T = GLfloat> class MatrixTransformation3D;

template<class Transformation> class Object;
template<class Transformation> class Scene;

}}

#endif
