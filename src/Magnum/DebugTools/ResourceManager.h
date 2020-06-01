#ifndef Magnum_DebugTools_ResourceManager_h
#define Magnum_DebugTools_ResourceManager_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#ifdef MAGNUM_TARGET_GL
/** @file
 * @brief Class @ref Magnum::DebugTools::ResourceManager
 */
#endif

#include "Magnum/ResourceManager.h"

#include "Magnum/Magnum.h"
#include "Magnum/DebugTools/DebugTools.h"
#include "Magnum/DebugTools/visibility.h"
#include "Magnum/GL/GL.h"
#include "Magnum/SceneGraph/SceneGraph.h"

/** @todo fix this better */
#ifdef _MSC_VER /* This is not a bug, this is a feature :( Not even /permissive- helps. */
#include "Magnum/DebugTools/ForceRenderer.h"
#include "Magnum/DebugTools/ObjectRenderer.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/MeshView.h"
#endif

#ifdef MAGNUM_TARGET_GL
namespace Magnum { namespace DebugTools {

/**
@brief Resource manager for debug tools

Stores various data used by debug renderers. See @ref debug-tools for more
information.

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL "TARGET_GL" enabled (done by default). See
    @ref building-features for more information.
*/
class MAGNUM_DEBUGTOOLS_EXPORT ResourceManager: public Magnum::ResourceManager<GL::AbstractShaderProgram, GL::Buffer, GL::Mesh, GL::MeshView, DebugTools::ForceRendererOptions, DebugTools::ObjectRendererOptions>
{
    public:
        explicit ResourceManager();
        ~ResourceManager();
};
#else
#error this header is available only in the OpenGL build
#endif

}}

#endif
