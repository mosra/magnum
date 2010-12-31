#ifndef Magnum_Scene_h
#define Magnum_Scene_h
/*
    Copyright © 2010 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Magnum::Scene
 */

#include "Camera.h"

namespace Magnum {

/** @brief Scene */
class Scene: public Object {
    private:
        virtual void setParent(Object* parent) {}
        Object::setTransformation;
        Object::multiplyTransformation;
        Object::setTransformationFrom;
        Object::translate;
        Object::scale;
        Object::rotate;

    public:
        /** @brief Features */
        enum Feature {
            AlphaBlending   = 0x01, /**< @brief Alpha blending */
            DepthTest       = 0x02, /**< @brief Depth test */
            FaceCulling     = 0x04  /**< @brief Face culling */
        };

        /** @brief Constructor */
        inline Scene(): Object(0), _features(0), _camera(0) {
            _parent = this;
            setClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        }

        /** @brief Clear color */
        inline Vector4 clearColor() const { return _clearColor; }

        /** @brief Which features are set */
        inline unsigned int features() const { return _features; }

        /** @brief Camera */
        inline Camera* camera() const { return _camera; }

        /** @brief Set feature */
        void setFeature(Feature feature, bool enabled);

        /** @brief Set clear color */
        void setClearColor(const Vector4& color);

        /** @brief Set clear color */
        inline void setClearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
            setClearColor(Vector4(r, g, b, a));
        }

        /**
         * @brief Set viewport size
         *
         * Just passes the values to active camera.
         */
        inline void setViewport(int width, int height) {
            if(_camera) _camera->setViewport(width, height);
        }

        /**
         * @brief Set camera
         *
         * If the camera is not part of the scene, the function does nothing.
         */
        void setCamera(Camera* camera);

        /**
         * @brief Draw whole scene
         *
         * Recursively draws all child objects. If no camera is available, does
         * nothing.
         */
        virtual void draw();

    private:
        unsigned int _features;
        Vector4 _clearColor;
        Camera* _camera;

        unsigned int viewportWidth, viewportHeight;

        inline virtual void draw(const Magnum::Matrix4& transformationMatrix, const Magnum::Matrix4& projectionMatrix) {}
        void drawChildren(Object* object, const Matrix4& transformationMatrix);
};

}

#endif
