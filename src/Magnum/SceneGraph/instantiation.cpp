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

#include "Magnum/SceneGraph/AbstractFeature.hpp"
#include "Magnum/SceneGraph/Animable.hpp"
#include "Magnum/SceneGraph/Camera.hpp"
#include "Magnum/SceneGraph/Drawable.hpp"
#include "Magnum/SceneGraph/DualComplexTransformation.h"
#include "Magnum/SceneGraph/DualQuaternionTransformation.h"
#include "Magnum/SceneGraph/FeatureGroup.hpp"
#include "Magnum/SceneGraph/MatrixTransformation2D.hpp"
#include "Magnum/SceneGraph/MatrixTransformation3D.hpp"
#include "Magnum/SceneGraph/Object.hpp"
#include "Magnum/SceneGraph/RigidMatrixTransformation2D.hpp"
#include "Magnum/SceneGraph/RigidMatrixTransformation3D.hpp"
#include "Magnum/SceneGraph/TranslationTransformation.h"
#include "Magnum/SceneGraph/TranslationRotationScalingTransformation2D.h"
#include "Magnum/SceneGraph/TranslationRotationScalingTransformation3D.h"

namespace Magnum { namespace SceneGraph {

/* On non-MinGW Windows the instantiations are already marked with extern
   template. However Clang-CL doesn't propagate the export from the extern
   template, it seems. */
#if !defined(CORRADE_TARGET_WINDOWS) || defined(CORRADE_TARGET_MINGW) || defined(CORRADE_TARGET_CLANG_CL)
#define MAGNUM_SCENEGRAPH_EXPORT_HPP MAGNUM_SCENEGRAPH_EXPORT
#else
#define MAGNUM_SCENEGRAPH_EXPORT_HPP
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
template class MAGNUM_SCENEGRAPH_EXPORT_HPP AbstractObject<2, Float>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP AbstractObject<3, Float>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP AbstractTransformation<2, Float>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP AbstractTransformation<3, Float>;

template class MAGNUM_SCENEGRAPH_EXPORT_HPP AbstractFeature<2, Float>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP AbstractFeature<3, Float>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP AbstractFeatureGroup<2, Float>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP AbstractFeatureGroup<3, Float>;

template class MAGNUM_SCENEGRAPH_EXPORT_HPP Animable<2, Float>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP Animable<3, Float>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP AnimableGroup<2, Float>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP AnimableGroup<3, Float>;

template class MAGNUM_SCENEGRAPH_EXPORT_HPP Camera<2, Float>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP Camera<3, Float>;

template class MAGNUM_SCENEGRAPH_EXPORT_HPP Drawable<2, Float>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP Drawable<3, Float>;

/* These have rotation(const Complex&) and rotation(const Quaternion&) defined
   in a hpp to avoid dragging in Complex / Quaternion for every user */
template class MAGNUM_SCENEGRAPH_EXPORT_HPP BasicMatrixTransformation2D<Float>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP BasicMatrixTransformation3D<Float>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP BasicRigidMatrixTransformation2D<Float>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP BasicRigidMatrixTransformation3D<Float>;

template class MAGNUM_SCENEGRAPH_EXPORT_HPP Object<BasicDualComplexTransformation<Float>>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP Object<BasicDualQuaternionTransformation<Float>>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP Object<BasicMatrixTransformation2D<Float>>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP Object<BasicMatrixTransformation3D<Float>>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP Object<BasicRigidMatrixTransformation2D<Float>>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP Object<BasicRigidMatrixTransformation3D<Float>>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP Object<BasicTranslationRotationScalingTransformation2D<Float>>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP Object<BasicTranslationRotationScalingTransformation3D<Float>>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP Object<TranslationTransformation<2, Float>>;
template class MAGNUM_SCENEGRAPH_EXPORT_HPP Object<TranslationTransformation<3, Float>>;
#endif

}}
