#ifndef Magnum_SceneGraph_Camera_h
#define Magnum_SceneGraph_Camera_h
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
 * @brief Class Magnum::SceneGraph::Camera, Magnum::SceneGraph::Camera2D, Magnum::SceneGraph::Camera3D
 */

#include "Object.h"

#ifdef WIN32 /* I so HATE windows.h */
#undef near
#undef far
#endif

namespace Magnum { namespace SceneGraph {

/** @todo Export implementation symbols only for tests */

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {
    enum class AspectRatioPolicy {
        NotPreserved, Extend, Clip
    };

    template<size_t dimensions> class Camera {};

    template<class MatrixType> MatrixType aspectRatioFix(AspectRatioPolicy aspectRatioPolicy, const Vector2& projectionScale, const Math::Vector2<GLsizei>& viewport);

    /* These templates are instantiated in source file */
    extern template SCENEGRAPH_EXPORT Matrix3 aspectRatioFix<Matrix3>(AspectRatioPolicy, const Vector2&, const Math::Vector2<GLsizei>&);
    extern template SCENEGRAPH_EXPORT Matrix4 aspectRatioFix<Matrix4>(AspectRatioPolicy, const Vector2&, const Math::Vector2<GLsizei>&);
}
#endif

/**
@brief %Camera object
 */
template<class MatrixType, class VectorType, class ObjectType, class SceneType, class CameraType> class SCENEGRAPH_EXPORT Camera: public ObjectType {
    public:
        /**
         * @brief Aspect ratio policy
         *
         * @see aspectRatioPolicy(), setAspectRatioPolicy()
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        typedef Implementation::AspectRatioPolicy AspectRatioPolicy;
        #else
        enum class AspectRatioPolicy {
            NotPreserved,   /**< Don't preserve aspect ratio (default) */
            Extend,         /**< Extend on larger side of view */
            Clip            /**< Clip on smaller side of view */
        };
        #endif

        /** @copydoc Object::Object */
        Camera(ObjectType* parent = nullptr);

        /** @brief Aspect ratio policy */
        inline AspectRatioPolicy aspectRatioPolicy() const { return _aspectRatioPolicy; }

        /** @brief Set aspect ratio policy */
        void setAspectRatioPolicy(AspectRatioPolicy policy) { _aspectRatioPolicy = policy; }

        /**
         * @brief Camera matrix
         *
         * Camera matrix describes world position relative to the camera and is
         * applied as first.
         */
        inline MatrixType cameraMatrix() {
            this->setClean();
            return _cameraMatrix;
        }

        /**
         * @brief Projection matrix
         *
         * Projection matrix handles e.g. perspective distortion and is applied
         * as last.
         * @see projectionSize()
         */
        inline MatrixType projectionMatrix() const { return _projectionMatrix; }

        /**
         * @brief Size of (near) XY plane in current projection
         *
         * Returns size of near XY plane computed from projection matrix.
         * @see projectionMatrix()
         */
        inline Vector2 projectionSize() const {
            return {2.0f/rawProjectionMatrix[0].x(), 2.0f/rawProjectionMatrix[1].y()};
        }

        /** @brief Viewport size */
        inline Math::Vector2<GLsizei> viewport() const { return _viewport; }

        /**
         * @brief Set viewport size
         *
         * Call when window size changes.
         *
         * Calls Framebuffer::setViewport() and stores viewport size
         * internally.
         */
        virtual void setViewport(const Math::Vector2<GLsizei>& size);

        /**
         * @brief Draw the scene
         *
         * Calls Framebuffer::clear() and draws the scene using drawChildren().
         */
        virtual void draw();

        using ObjectType::draw; /* Don't hide Object's draw() */

    protected:
        /**
         * Recalculates camera matrix.
         */
        void clean(const MatrixType& absoluteTransformation);

        /**
         * @brief Draw object children
         *
         * Recursively draws all children of the object.
         */
        void drawChildren(ObjectType* object, const MatrixType& transformationMatrix);

        #ifndef DOXYGEN_GENERATING_OUTPUT
        inline void fixAspectRatio() {
            _projectionMatrix = Implementation::aspectRatioFix<MatrixType>(_aspectRatioPolicy, {rawProjectionMatrix[0].x(), rawProjectionMatrix[1].y()}, _viewport)*rawProjectionMatrix;
        }

        MatrixType rawProjectionMatrix;
        AspectRatioPolicy _aspectRatioPolicy;
        #endif

    private:
        MatrixType _projectionMatrix;
        MatrixType _cameraMatrix;

        Math::Vector2<GLsizei> _viewport;
};

#ifndef DOXYGEN_GENERATING_OUTPUT
/* These templates are instantiated in source file */
extern template class SCENEGRAPH_EXPORT Camera<Matrix3, Vector2, Object2D, Scene2D, Camera2D>;
extern template class SCENEGRAPH_EXPORT Camera<Matrix4, Vector3, Object3D, Scene3D, Camera3D>;

namespace Implementation {
    template<> class Camera<2> {
        public:
            inline constexpr static Matrix3 aspectRatioScale(const Vector2& scale) {
                return Matrix3::scaling({scale.x(), scale.y()});
            }
    };
    template<> class Camera<3> {
        public:
            inline constexpr static Matrix4 aspectRatioScale(const Vector2& scale) {
                return Matrix4::scaling({scale.x(), scale.y(), 1.0f});
            }
    };
}
#endif

/** @brief %Camera for two-dimensional scenes */
class SCENEGRAPH_EXPORT Camera2D: public Camera<Matrix3, Vector2, Object2D, Scene2D, Camera2D> {
    public:
        /**
         * @brief Constructor
         * @param parent    Parent object
         *
         * Sets orthographic projection to the default OpenGL cube (range @f$ [-1; 1] @f$ in all directions).
         * @see setOrthographic()
         */
        inline Camera2D(Object2D* parent = nullptr): Camera(parent) {}

        /**
         * @brief Set projection
         * @param size      Size of the view
         *
         * The area of given size will be scaled down to range @f$ [-1; 1] @f$
         * on all directions.
         */
        void setProjection(const Vector2& size);
};

/** @brief %Camera for three-dimensional scenes */
class SCENEGRAPH_EXPORT Camera3D: public Camera<Matrix4, Vector3, Object3D, Scene3D, Camera3D> {
    public:
        /**
         * @brief Constructor
         * @param parent    Parent object
         *
         * Sets orthographic projection to the default OpenGL cube (range @f$ [-1; 1] @f$ in all directions).
         * @see setOrthographic(), setPerspective()
         */
        inline Camera3D(Object3D* parent = nullptr): Camera(parent), _near(0.0f), _far(0.0f) {}

        /**
         * @brief Set orthographic projection
         * @param size      Size of the view
         * @param near      Near clipping plane
         * @param far       Far clipping plane
         *
         * The volume of given size will be scaled down to range @f$ [-1; 1] @f$
         * on all directions.
         */
        void setOrthographic(const Vector2& size, GLfloat near, GLfloat far);

        /**
         * @brief Set perspective projection
         * @param fov       Field of view angle
         * @param near      Near clipping plane
         * @param far       Far clipping plane
         *
         * @todo Aspect ratio
         */
        void setPerspective(GLfloat fov, GLfloat near, GLfloat far);

        /** @brief Near clipping plane */
        inline GLfloat near() const { return _near; }

        /** @brief Far clipping plane */
        inline GLfloat far() const { return _far; }

    private:
        GLfloat _near, _far;
};

}}

#endif
