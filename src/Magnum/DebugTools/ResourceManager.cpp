/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include "ResourceManager.h"

#include "Magnum/AbstractShaderProgram.h"
#include "Magnum/Buffer.h"
#include "Magnum/Mesh.h"
#include "Magnum/MeshView.h"
#include "Magnum/ResourceManager.hpp"
#include "Magnum/DebugTools/ForceRenderer.h"
#include "Magnum/DebugTools/ObjectRenderer.h"
#include "Magnum/DebugTools/ShapeRenderer.h"

namespace Magnum {

namespace Implementation {
    template struct MAGNUM_DEBUGTOOLS_EXPORT ResourceManagerLocalInstanceImplementation<ResourceManagerLocalInstance, AbstractShaderProgram, Buffer, Mesh, MeshView, DebugTools::ForceRendererOptions, DebugTools::ObjectRendererOptions, DebugTools::ShapeRendererOptions>;
}

namespace DebugTools {

ResourceManager::ResourceManager() {
    setFallback(new ForceRendererOptions);
    setFallback(new ObjectRendererOptions);
    setFallback(new ShapeRendererOptions);
}

ResourceManager::~ResourceManager() = default;

}}
