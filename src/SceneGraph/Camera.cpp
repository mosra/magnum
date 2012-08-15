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

Matrix4 Camera<3>::fixAspectRatio(AspectRatioPolicy aspectRatioPolicy, const Math::Vector2<GLsizei>& viewport) {
    /* Don't divide by zero */
    if(viewport.x() == 0 || viewport.y() == 0)
        return Matrix4();

    /* Extend on larger side = scale larger side down */
    if(aspectRatioPolicy == AspectRatioPolicy::Extend)
        return ((viewport.x() > viewport.y()) ?
            Matrix4::scaling({GLfloat(viewport.y())/viewport.x(), 1, 1}) :
            Matrix4::scaling({1, GLfloat(viewport.x())/viewport.y(), 1})
        );

    /* Clip on smaller side = scale smaller side up */
    if(aspectRatioPolicy == AspectRatioPolicy::Clip)
        return ((viewport.x() > viewport.y()) ?
            Matrix4::scaling({1, GLfloat(viewport.x())/viewport.y(), 1}) :
            Matrix4::scaling({GLfloat(viewport.y())/viewport.x(), 1, 1})
        );

    /* Don't preserve anything */
    return Matrix4();
}

}
#endif

template<class MatrixType, class VectorType, class ObjectType, class SceneType, class CameraType> Camera<MatrixType, VectorType, ObjectType, SceneType, CameraType>::Camera(ObjectType* parent): ObjectType(parent), _aspectRatioPolicy(AspectRatioPolicy::Extend) {}

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

void Camera3D::setOrthographic(GLfloat size, GLfloat near, GLfloat far) {
    _near = near;
    _far = far;

    /* Scale the volume down so it fits in (-1, 1) in all directions */
    GLfloat xyScale = 2/size;
    GLfloat zScale = 2/(far-near);
    rawProjectionMatrix = Matrix4::scaling({xyScale, xyScale, -zScale});

    /* Move the volume on z into (-1, 1) range */
    rawProjectionMatrix = Matrix4::translation(Vector3::zAxis(-1-near*zScale))*rawProjectionMatrix;

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

    fixAspectRatio();
}

/* Explicitly instantiate the templates */
template class Camera<Matrix4, Vector3, Object3D, Scene3D, Camera3D>;

}}
