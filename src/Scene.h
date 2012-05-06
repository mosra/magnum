#ifndef Magnum_Scene_h
#define Magnum_Scene_h
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
 * @brief Class Magnum::Scene
 */

#include "Object.h"

namespace Magnum {

/** @brief %Scene */
class MAGNUM_EXPORT Scene: public Object {
    public:
        /** @brief Features */
        enum Feature {
            AlphaBlending   = 0x01, /**< @brief Alpha blending */
            DepthTest       = 0x02, /**< @brief Depth test */
            FaceCulling     = 0x04  /**< @brief Face culling */
        };

        /** @brief Constructor */
        inline Scene(): _features(0) { _parent = this; }

        void setParent(Object* parent) = delete;
        void setTransformation(const Matrix4& transformation) = delete;
        void multiplyTransformation(const Matrix4& transformation, Transformation type = Transformation::Global) = delete;
        void translate(Vector3 vec, Transformation type = Transformation::Global) = delete;
        void scale(Vector3 vec, Transformation type = Transformation::Global) = delete;
        void rotate(GLfloat angle, Vector3 vec, Transformation type = Transformation::Global) = delete;

        /** @brief Which features are set */
        inline unsigned int features() const { return _features; }

        /** @brief Set feature */
        /** @todo Depth clamping (OpenGL 3.2, ARB_depth_clamp) */
        void setFeature(Feature feature, bool enabled);

    private:
        unsigned int _features;
        GLuint vao;

        inline virtual void draw(const Magnum::Matrix4& transformationMatrix, Camera* camera) {}
};

}

#endif
