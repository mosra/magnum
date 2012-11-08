#ifndef Magnum_SceneGraph_Camera_hpp
#define Magnum_SceneGraph_Camera_hpp
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
 * @brief @ref compilation-speedup-hpp "Template implementation" for Camera.h
 */

#include "Camera.h"

#include <algorithm>

#include "Drawable.h"
#include "Scene.h"

using namespace std;

namespace Magnum { namespace SceneGraph {

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {

template<std::uint8_t dimensions, class T> class Camera {};

template<class T> class Camera<2, T> {
    public:
        inline constexpr static Math::Matrix3<T> aspectRatioScale(const Math::Vector2<T>& scale) {
            return Math::Matrix3<T>::scaling({scale.x(), scale.y()});
        }
};
template<class T> class Camera<3, T> {
    public:
        inline constexpr static Math::Matrix4<T> aspectRatioScale(const Math::Vector2<T>& scale) {
            return Math::Matrix4<T>::scaling({scale.x(), scale.y(), 1.0f});
        }
};

template<std::uint8_t dimensions, class T> typename DimensionTraits<dimensions, T>::MatrixType aspectRatioFix(AspectRatioPolicy aspectRatioPolicy, const Math::Vector2<T>& projectionScale, const Math::Vector2<GLsizei>& viewport) {
    /* Don't divide by zero / don't preserve anything */
    if(projectionScale.x() == 0 || projectionScale.y() == 0 || viewport.x() == 0 || viewport.y() == 0 || aspectRatioPolicy == AspectRatioPolicy::NotPreserved)
        return {};

    Math::Vector2<T> relativeAspectRatio = Math::Vector2<T>::from(viewport)*projectionScale;

    /* Extend on larger side = scale larger side down
       Clip on smaller side = scale smaller side up */
    return Camera<dimensions, T>::aspectRatioScale(
        (relativeAspectRatio.x() > relativeAspectRatio.y()) == (aspectRatioPolicy == AspectRatioPolicy::Extend) ?
        Vector2(relativeAspectRatio.y()/relativeAspectRatio.x(), T(1.0)) :
        Vector2(T(1.0), relativeAspectRatio.x()/relativeAspectRatio.y()));
}

}
#endif

template<std::uint8_t dimensions, class T> AbstractCamera<dimensions, T>* AbstractCamera<dimensions, T>::setAspectRatioPolicy(AspectRatioPolicy policy) {
    _aspectRatioPolicy = policy;
    fixAspectRatio();
    return this;
}

template<std::uint8_t dimensions, class T> void AbstractCamera<dimensions, T>::setViewport(const Math::Vector2<GLsizei>& size) {
    _viewport = size;
    fixAspectRatio();
}

template<std::uint8_t dimensions, class T> void AbstractCamera<dimensions, T>::draw(DrawableGroup<dimensions, T>& group) {
    AbstractObject<dimensions, T>* scene = AbstractFeature<dimensions, T>::object()->sceneObject();
    CORRADE_ASSERT(scene, "Camera::draw(): cannot draw when camera is not part of any scene", );

    /* Compute camera matrix */
    AbstractFeature<dimensions, T>::object()->setClean();

    /* Compute transformations of all objects in the group relative to the camera */
    std::vector<AbstractObject<dimensions, T>*> objects(group.size());
    for(std::size_t i = 0; i != group.size(); ++i)
        objects[i] = group[i]->object();
    std::vector<typename DimensionTraits<dimensions, T>::MatrixType> transformations =
        scene->transformationMatrices(objects, _cameraMatrix);

    /* Perform the drawing */
    for(std::size_t i = 0; i != transformations.size(); ++i)
        group[i]->draw(transformations[i], this);
}

template<class T> Camera2D<T>* Camera2D<T>::setProjection(const Math::Vector2<T>& size) {
    /* Scale the volume down so it fits in (-1, 1) in all directions */
    AbstractCamera<2, T>::rawProjectionMatrix = Math::Matrix3<T>::scaling(2.0f/size);

    AbstractCamera<2, T>::fixAspectRatio();
    return this;
}

template<class T> Camera3D<T>* Camera3D<T>::setOrthographic(const Math::Vector2<T>& size, T near, T far) {
    _near = near;
    _far = far;

    Math::Vector2<T> xyScale = T(2.0)/size;
    T zScale = T(2.0)/(near-far);

    AbstractCamera<3, T>::rawProjectionMatrix = Math::Matrix4<T>(
        xyScale.x(),    T(0.0),         T(0.0),         T(0.0),
        T(0.0),         xyScale.y(),    T(0.0),         T(0.0),
        T(0.0),         T(0.0),         zScale,         T(0.0),
        T(0.0),         T(0.0),         near*zScale-1,  T(1.0)
    );

    AbstractCamera<3, T>::fixAspectRatio();
    return this;
}

template<class T> Camera3D<T>* Camera3D<T>::setPerspective(T fov, T near, T far) {
    _near = near;
    _far = far;

    T xyScale = T(1.0)/tan(fov/2); /* == near/size */
    T zScale = T(1.0)/(near-far);

    AbstractCamera<3, T>::rawProjectionMatrix = Matrix4(
        xyScale,    T(0.0),     T(0.0),                 T(0.0),
        T(0.0),     xyScale,    T(0.0),                 T(0.0),
        T(0.0),     T(0.0),     (far+near)*zScale,      T(-1.0),
        T(0.0),     T(0.0),     (2*far*near)*zScale,    T(0.0)
    );

    AbstractCamera<3, T>::fixAspectRatio();
    return this;
}

}}

#endif
