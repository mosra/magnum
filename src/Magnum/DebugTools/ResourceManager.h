#ifndef Magnum_DebugTools_ResourceManager_h
#define Magnum_DebugTools_ResourceManager_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
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

#if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
/** @file
 * @brief Class @ref Magnum::DebugTools::ResourceManager
 * @m_deprecated_since_latest Use @ref Magnum/Primitives/Arrow.h /
 *      @ref Magnum/Primitives/Axis.h and @ref Primitives::arrow2D() /
 *      @relativeref{Primitives,axis2D()} or @relativeref{Primitives,arrow3D()}
 *      / @relativeref{Primitives,axis3D()} in a custom drawable instead
 */
#endif

#include "Magnum/configure.h"

#if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
#include "Magnum/ResourceManager.h"

#include "Magnum/Magnum.h"
#include "Magnum/DebugTools/DebugTools.h"
#include "Magnum/DebugTools/visibility.h"
#include "Magnum/GL/GL.h"
#include "Magnum/SceneGraph/SceneGraph.h"

#ifdef CORRADE_TARGET_MSVC
#include "Magnum/DebugTools/ForceRenderer.h"
#include "Magnum/DebugTools/ObjectRenderer.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/MeshView.h"
#endif

#ifndef _MAGNUM_NO_DEPRECATED_RESOURCEMANAGER
CORRADE_DEPRECATED_FILE("use Magnum/Primitives/Arrow.h / Magnum/Primitives/Axis.h and Primitives::arrow2D() / axis2D() or arrow3D() / axis3D() in a custom drawable instead")
#endif

namespace Magnum { namespace DebugTools {

/**
@brief Resource manager for debug tools
@m_deprecated_since_latest Use @ref Primitives::arrow2D() /
    @relativeref{Primitives,axis2D()} or @relativeref{Primitives,arrow3D()} /
    @relativeref{Primitives,axis3D()} in a custom drawable instead

Stores various data used by debug renderers. See @ref debug-tools for more
information.

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
CORRADE_IGNORE_DEPRECATED_PUSH /* GCC 4.8 warns due to the base types */
class CORRADE_DEPRECATED("use Primitives::arrow2D() / axis2D or arrow3D() / axis3D() in a custom drawable instead") MAGNUM_DEBUGTOOLS_EXPORT ResourceManager: public Magnum::ResourceManager<GL::AbstractShaderProgram, GL::Buffer, GL::Mesh, GL::MeshView, DebugTools::ForceRendererOptions, DebugTools::ObjectRendererOptions>
{
    public:
        explicit ResourceManager();
        ~ResourceManager();
};
CORRADE_IGNORE_DEPRECATED_POP
#elif !defined(MAGNUM_BUILD_DEPRECATED)
#error use Magnum/Primitives/Arrow.h / Magnum/Primitives/Axis.h and Primitives::arrow2D() / axis2D() or arrow3D() / axis3D() in a custom drawable instead
#elif !defined(MAGNUM_TARGET_GL)
#error this header is available only in the OpenGL build
#endif

}}

#endif
