/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

#define _MAGNUM_DO_NOT_WARN_DEPRECATED_SHAPES

#include "ResourceManager.h"

#include "Magnum/ResourceManager.hpp"
#include "Magnum/DebugTools/ForceRenderer.h"
#include "Magnum/DebugTools/ObjectRenderer.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/MeshView.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include "Magnum/DebugTools/ShapeRenderer.h"
#endif

namespace Magnum {

namespace Implementation {
    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_PUSH
    #endif
    template struct MAGNUM_DEBUGTOOLS_EXPORT ResourceManagerLocalInstanceImplementation<ResourceManagerLocalInstance, GL::AbstractShaderProgram, GL::Buffer, GL::Mesh, GL::MeshView, DebugTools::ForceRendererOptions, DebugTools::ObjectRendererOptions
        #ifdef MAGNUM_BUILD_DEPRECATED
        , DebugTools::ShapeRendererOptions
        #endif
    >;
    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_POP
    #endif
}

namespace DebugTools {

ResourceManager::ResourceManager() {
    setFallback(new ForceRendererOptions);
    setFallback(new ObjectRendererOptions);
    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_PUSH
    setFallback(new ShapeRendererOptions);
    CORRADE_IGNORE_DEPRECATED_POP
    #endif
}

ResourceManager::~ResourceManager() = default;

}}
