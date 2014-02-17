#ifndef Magnum_SceneGraph_AbstractCamera_hpp
#define Magnum_SceneGraph_AbstractCamera_hpp
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
              Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief @ref compilation-speedup-hpp "Template implementation" for @ref AbstractCamera.h
 */

#include "Magnum/SceneGraph/AbstractCamera.h"
#include "Magnum/SceneGraph/Drawable.h"

namespace Magnum { namespace SceneGraph {

namespace Implementation {

template<UnsignedInt dimensions, class T> class Camera {};

template<class T> class Camera<2, T> {
    public:
        constexpr static Math::Matrix3<T> aspectRatioScale(const Math::Vector2<T>& scale) {
            return Math::Matrix3<T>::scaling({scale.x(), scale.y()});
        }
};
template<class T> class Camera<3, T> {
    public:
        constexpr static Math::Matrix4<T> aspectRatioScale(const Math::Vector2<T>& scale) {
            return Math::Matrix4<T>::scaling({scale.x(), scale.y(), 1.0f});
        }
};

template<UnsignedInt dimensions, class T> typename DimensionTraits<dimensions, T>::MatrixType aspectRatioFix(AspectRatioPolicy aspectRatioPolicy, const Math::Vector2<T>& projectionScale, const Vector2i& viewport) {
    /* Don't divide by zero / don't preserve anything */
    if(projectionScale.x() == 0 || projectionScale.y() == 0 || viewport.x() == 0 || viewport.y() == 0 || aspectRatioPolicy == AspectRatioPolicy::NotPreserved)
        return {};

    Math::Vector2<T> relativeAspectRatio = Math::Vector2<T>(viewport)*projectionScale;

    /* Extend on larger side = scale larger side down
       Clip on smaller side = scale smaller side up */
    return Camera<dimensions, T>::aspectRatioScale(
        (relativeAspectRatio.x() > relativeAspectRatio.y()) == (aspectRatioPolicy == AspectRatioPolicy::Extend) ?
        Vector2(relativeAspectRatio.y()/relativeAspectRatio.x(), T(1.0)) :
        Vector2(T(1.0), relativeAspectRatio.x()/relativeAspectRatio.y()));
}

}

template<UnsignedInt dimensions, class T> AbstractCamera<dimensions, T>::AbstractCamera(AbstractObject<dimensions, T>& object): AbstractFeature<dimensions, T>(object), _aspectRatioPolicy(AspectRatioPolicy::NotPreserved) {
    AbstractFeature<dimensions, T>::setCachedTransformations(CachedTransformation::InvertedAbsolute);
}

/* `= default` causes linker errors in GCC 4.4 */
template<UnsignedInt dimensions, class T> AbstractCamera<dimensions, T>::~AbstractCamera() {}

template<UnsignedInt dimensions, class T> AbstractCamera<dimensions, T>& AbstractCamera<dimensions, T>::setAspectRatioPolicy(AspectRatioPolicy policy) {
    _aspectRatioPolicy = policy;
    fixAspectRatio();
    return *this;
}

template<UnsignedInt dimensions, class T> void AbstractCamera<dimensions, T>::setViewport(const Vector2i& size) {
    _viewport = size;
    fixAspectRatio();
}

template<UnsignedInt dimensions, class T> void AbstractCamera<dimensions, T>::draw(DrawableGroup<dimensions, T>& group) {
    AbstractObject<dimensions, T>* scene = AbstractFeature<dimensions, T>::object().scene();
    CORRADE_ASSERT(scene, "Camera::draw(): cannot draw when camera is not part of any scene", );

    /* Compute camera matrix */
    AbstractFeature<dimensions, T>::object().setClean();

    /* Compute transformations of all objects in the group relative to the camera */
    std::vector<std::reference_wrapper<AbstractObject<dimensions, T>>> objects;
    objects.reserve(group.size());
    for(std::size_t i = 0; i != group.size(); ++i)
        objects.push_back(group[i].object());
    std::vector<typename DimensionTraits<dimensions, T>::MatrixType> transformations =
        scene->transformationMatrices(objects, _cameraMatrix);

    /* Perform the drawing */
    for(std::size_t i = 0; i != transformations.size(); ++i)
        group[i].draw(transformations[i], *this);
}

}}

#endif
