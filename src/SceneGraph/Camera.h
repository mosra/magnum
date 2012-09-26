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
 * @brief Class Magnum::SceneGraph::AbstractCamera, Magnum::SceneGraph::Camera2D, Magnum::SceneGraph::Camera3D
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
template<size_t dimensions> class SCENEGRAPH_EXPORT AbstractCamera: public AbstractObject<dimensions>::ObjectType {
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

        /** @copydoc AbstractObject::AbstractObject() */
        AbstractCamera(typename AbstractObject<dimensions>::ObjectType* parent = nullptr);

        virtual ~AbstractCamera() = 0;

        /** @brief Aspect ratio policy */
        inline AspectRatioPolicy aspectRatioPolicy() const { return _aspectRatioPolicy; }

        /**
         * @brief Set aspect ratio policy
         * @return Pointer to self (for method chaining)
         */
        typename AbstractObject<dimensions>::CameraType* setAspectRatioPolicy(AspectRatioPolicy policy);

        /**
         * @brief Camera matrix
         *
         * Camera matrix describes world position relative to the camera and is
         * applied as first.
         */
        inline typename AbstractObject<dimensions>::MatrixType cameraMatrix() {
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
        inline typename AbstractObject<dimensions>::MatrixType projectionMatrix() const { return _projectionMatrix; }

        /**
         * @brief Size of (near) XY plane in current projection
         *
         * Returns size of near XY plane computed from projection matrix.
         * @see projectionMatrix()
         */
        inline Vector2 projectionSize() const {
            return {2.0f/_projectionMatrix[0].x(), 2.0f/_projectionMatrix[1].y()};
        }

        /** @brief Viewport size */
        inline Math::Vector2<GLsizei> viewport() const { return _viewport; }

        /**
         * @brief Set viewport size
         *
         * Stores viewport size internally and recalculates projection matrix
         * according to aspect ratio policy.
         * @see setAspectRatioPolicy()
         */
        virtual void setViewport(const Math::Vector2<GLsizei>& size);

        /**
         * @brief Draw the scene
         *
         * Draws the scene using drawChildren().
         */
        virtual void draw();

        using AbstractObject<dimensions>::ObjectType::draw; /* Don't hide Object's draw() */

    protected:
        /**
         * Recalculates camera matrix.
         */
        void clean(const typename AbstractObject<dimensions>::MatrixType& absoluteTransformation);

        /**
         * @brief Draw object children
         *
         * Recursively draws all children of the object.
         */
        void drawChildren(typename AbstractObject<dimensions>::ObjectType* object, const typename AbstractObject<dimensions>::MatrixType& transformationMatrix);

        #ifndef DOXYGEN_GENERATING_OUTPUT
        inline void fixAspectRatio() {
            _projectionMatrix = Implementation::aspectRatioFix<typename AbstractObject<dimensions>::MatrixType>(_aspectRatioPolicy, {rawProjectionMatrix[0].x(), rawProjectionMatrix[1].y()}, _viewport)*rawProjectionMatrix;
        }

        typename AbstractObject<dimensions>::MatrixType rawProjectionMatrix;
        AspectRatioPolicy _aspectRatioPolicy;
        #endif

    private:
        typename AbstractObject<dimensions>::MatrixType _projectionMatrix;
        typename AbstractObject<dimensions>::MatrixType _cameraMatrix;

        Math::Vector2<GLsizei> _viewport;
};

template<size_t dimensions> inline AbstractCamera<dimensions>::~AbstractCamera() {}

#ifndef DOXYGEN_GENERATING_OUTPUT
/* These templates are instantiated in source file */
extern template class SCENEGRAPH_EXPORT AbstractCamera<2>;
extern template class SCENEGRAPH_EXPORT AbstractCamera<3>;

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

/**
@brief %Camera for two-dimensional scenes

@see Camera3D
*/
class SCENEGRAPH_EXPORT Camera2D: public AbstractCamera<2> {
    public:
        /**
         * @brief Constructor
         * @param parent    Parent object
         *
         * Sets orthographic projection to the default OpenGL cube (range @f$ [-1; 1] @f$ in all directions).
         * @see setOrthographic()
         */
        inline Camera2D(Object2D* parent = nullptr): AbstractCamera(parent) {}

        /**
         * @brief Set projection
         * @param size      Size of the view
         * @return Pointer to self (for method chaining)
         *
         * The area of given size will be scaled down to range @f$ [-1; 1] @f$
         * on all directions.
         */
        Camera2D* setProjection(const Vector2& size);
};

/**
@brief %Camera for three-dimensional scenes

@see Camera2D
*/
class SCENEGRAPH_EXPORT Camera3D: public AbstractCamera<3> {
    public:
        /**
         * @brief Constructor
         * @param parent    Parent object
         *
         * Sets orthographic projection to the default OpenGL cube (range @f$ [-1; 1] @f$ in all directions).
         * @see setOrthographic(), setPerspective()
         */
        inline Camera3D(Object3D* parent = nullptr): AbstractCamera(parent), _near(0.0f), _far(0.0f) {}

        /**
         * @brief Set orthographic projection
         * @param size      Size of the view
         * @param near      Near clipping plane
         * @param far       Far clipping plane
         * @return Pointer to self (for method chaining)
         *
         * The volume of given size will be scaled down to range @f$ [-1; 1] @f$
         * on all directions.
         */
        Camera3D* setOrthographic(const Vector2& size, GLfloat near, GLfloat far);

        /**
         * @brief Set perspective projection
         * @param fov       Field of view angle
         * @param near      Near clipping plane
         * @param far       Far clipping plane
         * @return Pointer to self (for method chaining)
         *
         * @todo Aspect ratio
         */
        Camera3D* setPerspective(GLfloat fov, GLfloat near, GLfloat far);

        /** @brief Near clipping plane */
        inline GLfloat near() const { return _near; }

        /** @brief Far clipping plane */
        inline GLfloat far() const { return _far; }

    private:
        GLfloat _near, _far;
};

}}

#endif
