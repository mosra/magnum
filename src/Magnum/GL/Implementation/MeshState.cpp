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

#include "MeshState.h"

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/MeshView.h"

#include "State.h"

namespace Magnum { namespace GL { namespace Implementation {

MeshState::MeshState(Context& context, ContextState& contextState, std::vector<std::string>& extensions): currentVAO(0)
    #ifndef MAGNUM_TARGET_GLES2
    , maxElementIndex{0}, maxElementsIndices{0}, maxElementsVertices{0}
    #endif
{
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::ARB::vertex_array_object>())
    #elif defined(MAGNUM_TARGET_GLES2)
    if(context.isExtensionSupported<Extensions::OES::vertex_array_object>())
    #else
    static_cast<void>(context);
    static_cast<void>(extensions);
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        extensions.emplace_back(Extensions::ARB::vertex_array_object::string());
        #elif defined(MAGNUM_TARGET_GLES2)
        extensions.push_back(Extensions::OES::vertex_array_object::string());
        #endif

        #ifndef MAGNUM_TARGET_GLES
        if(context.isExtensionSupported<Extensions::ARB::direct_state_access>()
            #ifdef CORRADE_TARGET_WINDOWS
            && (!(context.detectedDriver() & Context::DetectedDriver::IntelWindows) ||
            context.isDriverWorkaroundDisabled("intel-windows-crazy-broken-vao-dsa"))
            #endif
        ) {
            extensions.emplace_back(Extensions::ARB::direct_state_access::string());

            /* Intel Windows drivers are ... special */
            #ifdef CORRADE_TARGET_WINDOWS
            if((context.detectedDriver() & Context::DetectedDriver::IntelWindows) &&
               !context.isDriverWorkaroundDisabled("intel-windows-broken-dsa-integer-vertex-attributes"))
            {
                attributePointerImplementation = &Mesh::attributePointerImplementationVAODSAIntelWindows;
            } else
            #endif
            {
                attributePointerImplementation = &Mesh::attributePointerImplementationVAODSA;
            }

            createImplementation = &Mesh::createImplementationVAODSA;
            bindIndexBufferImplementation = &Mesh::bindIndexBufferImplementationVAODSA;
        } else
        #endif
        {
            createImplementation = &Mesh::createImplementationVAO;
            attributePointerImplementation = &Mesh::attributePointerImplementationVAO;
            bindIndexBufferImplementation = &Mesh::bindIndexBufferImplementationVAO;
        }

        moveConstructImplementation = &Mesh::moveConstructImplementationVAO;
        moveAssignImplementation = &Mesh::moveAssignImplementationVAO;
        destroyImplementation = &Mesh::destroyImplementationVAO;
        acquireVertexBufferImplementation = &Mesh::acquireVertexBufferImplementationVAO;
        bindVAOImplementation = &Mesh::bindVAOImplementationVAO;
        bindImplementation = &Mesh::bindImplementationVAO;
        unbindImplementation = &Mesh::unbindImplementationVAO;
    }
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
    else {
        createImplementation = &Mesh::createImplementationDefault;
        moveConstructImplementation = &Mesh::moveConstructImplementationDefault;
        moveAssignImplementation = &Mesh::moveAssignImplementationDefault;
        destroyImplementation = &Mesh::destroyImplementationDefault;
        attributePointerImplementation = &Mesh::attributePointerImplementationDefault;
        acquireVertexBufferImplementation = &Mesh::acquireVertexBufferImplementationDefault;
        bindIndexBufferImplementation = &Mesh::bindIndexBufferImplementationDefault;
        bindVAOImplementation = &Mesh::bindVAOImplementationDefault;
        bindImplementation = &Mesh::bindImplementationDefault;
        unbindImplementation = &Mesh::unbindImplementationDefault;
    }
    #endif

    #ifdef MAGNUM_TARGET_GLES
    #ifndef MAGNUM_TARGET_WEBGL
    /* Multi draw implementation on ES */
    if(context.isExtensionSupported<Extensions::EXT::multi_draw_arrays>()) {
        extensions.push_back(Extensions::EXT::multi_draw_arrays::string());

        multiDrawImplementation = &MeshView::multiDrawImplementationDefault;
    } else multiDrawImplementation = &MeshView::multiDrawImplementationFallback;
    #else
    multiDrawImplementation = &MeshView::multiDrawImplementationFallback;
    #endif
    #endif

    #ifdef MAGNUM_TARGET_GLES2
    /* Instanced draw ímplementation on ES2 */
    if(context.isExtensionSupported<Extensions::ANGLE::instanced_arrays>()) {
        extensions.push_back(Extensions::ANGLE::instanced_arrays::string());

        drawArraysInstancedImplementation = &Mesh::drawArraysInstancedImplementationANGLE;
        drawElementsInstancedImplementation = &Mesh::drawElementsInstancedImplementationANGLE;
    }
    #ifndef MAGNUM_TARGET_WEBGL
    else if(context.isExtensionSupported<Extensions::EXT::instanced_arrays>() ||
            context.isExtensionSupported<Extensions::EXT::draw_instanced>()) {
        extensions.push_back(context.isExtensionSupported<Extensions::EXT::instanced_arrays>() ?
            Extensions::EXT::instanced_arrays::string() :
            Extensions::EXT::draw_instanced::string());

        drawArraysInstancedImplementation = &Mesh::drawArraysInstancedImplementationEXT;
        drawElementsInstancedImplementation = &Mesh::drawElementsInstancedImplementationEXT;

    } else if(context.isExtensionSupported<Extensions::NV::instanced_arrays>() ||
              context.isExtensionSupported<Extensions::NV::draw_instanced>()) {
        extensions.push_back(context.isExtensionSupported<Extensions::NV::instanced_arrays>() ?
            Extensions::NV::instanced_arrays::string() :
            Extensions::NV::draw_instanced::string());

        drawArraysInstancedImplementation = &Mesh::drawArraysInstancedImplementationNV;
        drawElementsInstancedImplementation = &Mesh::drawElementsInstancedImplementationNV;
    }
    #endif
    else {
        drawArraysInstancedImplementation = nullptr;
        drawElementsInstancedImplementation = nullptr;
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::ARB::direct_state_access>()
        #ifdef CORRADE_TARGET_WINDOWS
        && (!(context.detectedDriver() & Context::DetectedDriver::IntelWindows) ||
        context.isDriverWorkaroundDisabled("intel-windows-crazy-broken-vao-dsa"))
        #endif
    )
        vertexAttribDivisorImplementation = &Mesh::vertexAttribDivisorImplementationVAODSA;
    else if(context.isExtensionSupported<Extensions::ARB::vertex_array_object>())
        vertexAttribDivisorImplementation = &Mesh::vertexAttribDivisorImplementationVAO;
    else
        vertexAttribDivisorImplementation = nullptr;
    #elif defined(MAGNUM_TARGET_GLES2)
    /* Instanced arrays implementation on ES2 */
    if(context.isExtensionSupported<Extensions::ANGLE::instanced_arrays>()) {
        /* Extension added above */

        vertexAttribDivisorImplementation = &Mesh::vertexAttribDivisorImplementationANGLE;
    }
    #ifndef MAGNUM_TARGET_WEBGL
    else if(context.isExtensionSupported<Extensions::EXT::instanced_arrays>()) {
        extensions.push_back(Extensions::EXT::instanced_arrays::string());

        vertexAttribDivisorImplementation = &Mesh::vertexAttribDivisorImplementationEXT;

    } else if(context.isExtensionSupported<Extensions::NV::instanced_arrays>()) {
        extensions.push_back(Extensions::NV::instanced_arrays::string());

        vertexAttribDivisorImplementation = &Mesh::vertexAttribDivisorImplementationNV;
    }
    #endif
    else vertexAttribDivisorImplementation = nullptr;
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /* If we are on core profile and ARB_VAO was explicitly disabled by the
       user, we need to bind a default VAO so we are still able to draw things.
       There's another "scratch" VAO that's used by Context::resetState() for
       crappy external code if ARB_VAO *isn't* disabled, but that one is
       generated on-demand as we optimistically hope crappy external code is
       not a norm. */
    if(context.isExtensionDisabled<Extensions::ARB::vertex_array_object>() && context.isCoreProfileInternal(contextState)) {
        glGenVertexArrays(1, &defaultVAO);
        glBindVertexArray(defaultVAO);
    }
    #else
    static_cast<void>(contextState);
    #endif
}

MeshState::~MeshState() {
    #ifndef MAGNUM_TARGET_GLES
    /* If the default VAO was created, we need to delete it to avoid leaks.
       Delete also the scratch VAO if the engine was so unlucky to have to run
       awful external GL code (it was created in Context::resetState()). */
    if(defaultVAO) glDeleteVertexArrays(1, &defaultVAO);
    if(scratchVAO) glDeleteVertexArrays(1, &scratchVAO);
    #endif
}

void MeshState::reset() {
    currentVAO = State::DisengagedBinding;
}

}}}
