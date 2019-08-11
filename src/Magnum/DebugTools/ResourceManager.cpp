/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include "Magnum/ResourceManager.hpp"
#include "Magnum/DebugTools/ForceRenderer.h"
#include "Magnum/DebugTools/ObjectRenderer.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/MeshView.h"

namespace Magnum {

namespace Implementation {
    template struct MAGNUM_DEBUGTOOLS_EXPORT ResourceManagerLocalInstanceImplementation<ResourceManagerLocalInstance, GL::AbstractShaderProgram, GL::Buffer, GL::Mesh, GL::MeshView, DebugTools::ForceRendererOptions, DebugTools::ObjectRendererOptions>;
}

namespace DebugTools {

namespace {
    #ifdef CORRADE_BUILD_MULTITHREADED
    CORRADE_THREAD_LOCAL
    #endif
    ResourceManager* resourceManagerInstance = nullptr;
}

ResourceManager& ResourceManager::instance() {
    CORRADE_ASSERT(resourceManagerInstance,
        "DebugTools::ResourceManager::instance(): no instance exists",
        *resourceManagerInstance);
    return *resourceManagerInstance;
}

ResourceManager::ResourceManager() {
    CORRADE_ASSERT(!resourceManagerInstance,
        "DebugTools::ResourceManager: another instance is already created", );
    resourceManagerInstance = this;

    setFallback(new ForceRendererOptions);
    setFallback(new ObjectRendererOptions);
}

ResourceManager::~ResourceManager() {
    CORRADE_INTERNAL_ASSERT(resourceManagerInstance == this);
    resourceManagerInstance = nullptr;
}

}}
