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

#include "Camera.h"
#include "Framebuffer.h"
#include "Scene.h"

using namespace std;

namespace Magnum { namespace SceneGraph {

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {

template<class MatrixType> MatrixType aspectRatioFix(AspectRatioPolicy aspectRatioPolicy, const Vector2& projectionAspectRatio, const Math::Vector2<GLsizei>& viewport) {
    /* Don't divide by zero / don't preserve anything */
    if(projectionAspectRatio.x() == 0 || projectionAspectRatio.y() == 0 || viewport.x() == 0 || viewport.y() == 0 || aspectRatioPolicy == AspectRatioPolicy::NotPreserved)
        return MatrixType();

    Vector2 relativeAspectRatio = Vector2::from(viewport)/projectionAspectRatio;

    /* Extend on larger side = scale larger side down
       Clip on smaller side = scale smaller side up */
    return Camera<MatrixType::Size-1>::aspectRatioScale(
        (relativeAspectRatio.x() > relativeAspectRatio.y()) == (aspectRatioPolicy == AspectRatioPolicy::Extend) ?
        Vector2(relativeAspectRatio.y()/relativeAspectRatio.x(), 1.0f) :
        Vector2(1.0f, relativeAspectRatio.x()/relativeAspectRatio.y()));
}

/* Explicitly instantiate the templates */
template Matrix3 aspectRatioFix<Matrix3>(AspectRatioPolicy, const Vector2&, const Math::Vector2<GLsizei>&);
template Matrix4 aspectRatioFix<Matrix4>(AspectRatioPolicy, const Vector2&, const Math::Vector2<GLsizei>&);

}
#endif

template<class MatrixType, class VectorType, class ObjectType, class SceneType, class CameraType> Camera<MatrixType, VectorType, ObjectType, SceneType, CameraType>::Camera(ObjectType* parent): ObjectType(parent), projectionAspectRatio(1.0f), _aspectRatioPolicy(AspectRatioPolicy::Extend) {}

template<class MatrixType, class VectorType, class ObjectType, class SceneType, class CameraType> void Camera<MatrixType, VectorType, ObjectType, SceneType, CameraType>::setViewport(const Math::Vector2<GLsizei>& size) {
    Framebuffer::setViewport({0, 0}, size);

    _viewport = size;
    fixAspectRatio();
}

template<class MatrixType, class VectorType, class ObjectType, class SceneType, class CameraType> void Camera<MatrixType, VectorType, ObjectType, SceneType, CameraType>::clean(const MatrixType& absoluteTransformation) {
    ObjectType::clean(absoluteTransformation);

    _cameraMatrix = absoluteTransformation.inverted();
}

template<class MatrixType, class VectorType, class ObjectType, class SceneType, class CameraType> void Camera<MatrixType, VectorType, ObjectType, SceneType, CameraType>::draw() {
    SceneType* s = this->scene();
    CORRADE_ASSERT(s, "Camera: cannot draw without camera attached to scene", );

    Framebuffer::clear();

    /* Recursively draw child objects */
    drawChildren(s, cameraMatrix());
}

template<class MatrixType, class VectorType, class ObjectType, class SceneType, class CameraType> void Camera<MatrixType, VectorType, ObjectType, SceneType, CameraType>::drawChildren(ObjectType* object, const MatrixType& transformationMatrix) {
    for(typename set<ObjectType*>::const_iterator it = object->children().begin(); it != object->children().end(); ++it) {
        /* Transformation matrix for the object */
        MatrixType matrix = transformationMatrix*(*it)->transformation();

        /* Draw the object and its children */
        (*it)->draw(matrix, static_cast<CameraType*>(this));
        drawChildren(*it, matrix);
    }
}

void Camera2D::setProjection(const Vector2& size) {
    /* Scale the volume down so it fits in (-1, 1) in all directions */
    rawProjectionMatrix = Matrix3::scaling(2.0f/size);

    projectionAspectRatio = size;
    fixAspectRatio();
}

void Camera3D::setOrthographic(const Vector2& size, GLfloat near, GLfloat far) {
    _near = near;
    _far = far;

    /* Scale the volume down so it fits in (-1, 1) in all directions */
    GLfloat zScale = 2/(far-near);
    rawProjectionMatrix = Matrix4::scaling({2.0f/size, -zScale});

    /* Move the volume on z into (-1, 1) range */
    rawProjectionMatrix = Matrix4::translation(Vector3::zAxis(-1-near*zScale))*rawProjectionMatrix;

    projectionAspectRatio = size;
    fixAspectRatio();
}

void Camera3D::setPerspective(GLfloat fov, GLfloat near, GLfloat far) {
    _near = near;
    _far = far;

    /* First move the volume on z in (-1, 1) range */
    rawProjectionMatrix = Matrix4::translation(Vector3::zAxis(2*far*near/(far+near)));

    /* Then apply magic perspective matrix (with reversed Z) */
    static const Matrix4 a(1.0f, 0.0f,  0.0f,  0.0f,
                           0.0f, 1.0f,  0.0f,  0.0f,
                           0.0f, 0.0f,  -1.0f,  -1.0f,
                           0.0f, 0.0f,  0.0f,  0.0f);
    rawProjectionMatrix = a*rawProjectionMatrix;

    /* Then scale the volume down so it fits in (-1, 1) in all directions */
    GLfloat xyScale = 1/tan(fov/2);
    GLfloat zScale = 1+2*near/(far-near);
    rawProjectionMatrix = Matrix4::scaling({xyScale, xyScale, zScale})*rawProjectionMatrix;

    /* And... another magic */
    rawProjectionMatrix[3][3] = 0;

    projectionAspectRatio = Vector2(xyScale);
    fixAspectRatio();
}

/* Explicitly instantiate the templates */
template class Camera<Matrix3, Vector2, Object2D, Scene2D, Camera2D>;
template class Camera<Matrix4, Vector3, Object3D, Scene3D, Camera3D>;

}}
