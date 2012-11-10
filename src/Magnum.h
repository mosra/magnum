#ifndef Magnum_Magnum_h
#define Magnum_Magnum_h
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
 * @brief Basic definitions
 */

#include "magnumConfigure.h"

#ifndef MAGNUM_TARGET_GLES
#include <GL/glew.h>
#include <GL/glcorearb.h>
#else
#ifndef MAGNUM_TARGET_GLES2
#include <GLES3/gl3.h>
#else
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif
#endif

/**
 * @todo Link to libGL / libGLES based on which windowcontext is used in app
 *      and whether GLES is enabled or not -- this allows us to use glx with
 *      ES on nvidia/intel. Using libGL and EGL on nvidia is whole another
 *      problem, though. How about windows? It won't allow unlinked DLLs, so
 *      probably always link Magnum itself to GL library there. How about unit
 *      tests not needing any of GL? -- different testing library?
 */

namespace Corrade {
    namespace Utility {
        class Debug;
        class Warning;
        class Error;
    }
}

namespace Magnum {
    namespace Math {
        template<class> class Vector2;
        template<class> class Vector3;
        template<class> class Vector4;
        template<class> class Point2D;
        template<class> class Point3D;
        template<class> class Matrix3;
        template<class> class Matrix4;

        template<class T> constexpr T deg(T value);
        template<class T> constexpr T rad(T value);
    }

/* Bring debugging facility from Corrade::Utility namespace */
using Corrade::Utility::Debug;
using Corrade::Utility::Warning;
using Corrade::Utility::Error;

/** @brief Two-component floating-point vector */
typedef Math::Vector2<GLfloat> Vector2;

/** @brief Three-component floating-point vector */
typedef Math::Vector3<GLfloat> Vector3;

/** @brief Four-component floating-point vector */
typedef Math::Vector4<GLfloat> Vector4;

/** @brief Two-dimensional floating-point homogeneous coordinates */
typedef Math::Point2D<GLfloat> Point2D;

/** @brief Three-dimensional floating-point homogeneous coordinates */
typedef Math::Point3D<GLfloat> Point3D;

/** @brief 3x3 floating-point matrix */
typedef Math::Matrix3<GLfloat> Matrix3;

/** @brief 4x4 floating-point matrix */
typedef Math::Matrix4<GLfloat> Matrix4;

/* Copying angle converters from Math namespace */
using Math::deg;
using Math::rad;

}

#endif
