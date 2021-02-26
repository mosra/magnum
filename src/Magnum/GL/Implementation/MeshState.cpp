/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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

#include <Corrade/Containers/StringView.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/MeshView.h"

#include "State.h"

namespace Magnum { namespace GL { namespace Implementation {

using namespace Containers::Literals;

MeshState::MeshState(Context& context, ContextState& contextState, Containers::StaticArrayView<Implementation::ExtensionCount, const char*> extensions): currentVAO(0)
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
        extensions[Extensions::ARB::vertex_array_object::Index] =
                   Extensions::ARB::vertex_array_object::string();
        #elif defined(MAGNUM_TARGET_GLES2)
        extensions[Extensions::OES::vertex_array_object::Index] =
                   Extensions::OES::vertex_array_object::string();
        #endif

        #ifndef MAGNUM_TARGET_GLES
        if(context.isExtensionSupported<Extensions::ARB::direct_state_access>()
            #ifdef CORRADE_TARGET_WINDOWS
            && (!(context.detectedDriver() & Context::DetectedDriver::IntelWindows) ||
            context.isDriverWorkaroundDisabled("intel-windows-crazy-broken-vao-dsa"_s))
            #endif
        ) {
            extensions[Extensions::ARB::direct_state_access::Index] =
                       Extensions::ARB::direct_state_access::string();

            /* Intel Windows drivers are ... special */
            #ifdef CORRADE_TARGET_WINDOWS
            if((context.detectedDriver() & Context::DetectedDriver::IntelWindows) &&
               !context.isDriverWorkaroundDisabled("intel-windows-broken-dsa-integer-vertex-attributes"_s))
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

    /* Base vertex draws on ES 2/3 and WebGL 2 */
    #if defined(MAGNUM_TARGET_GLES) && !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    #ifndef MAGNUM_TARGET_WEBGL
    #ifndef MAGNUM_TARGET_GLES2
    if(context.isVersionSupported(Version::GLES320)) {
        drawElementsBaseVertexImplementation = glDrawElementsBaseVertex;
        drawRangeElementsBaseVertexImplementation = glDrawRangeElementsBaseVertex;
        drawElementsInstancedBaseVertexImplementation = glDrawElementsInstancedBaseVertex;
    } else
    #endif
    if(context.isExtensionSupported<Extensions::EXT::draw_elements_base_vertex>()) {
        extensions[Extensions::EXT::draw_elements_base_vertex::Index] =
                   Extensions::EXT::draw_elements_base_vertex::string();

        drawElementsBaseVertexImplementation = glDrawElementsBaseVertexEXT;
        #ifndef MAGNUM_TARGET_GLES2
        drawRangeElementsBaseVertexImplementation = glDrawRangeElementsBaseVertexEXT;
        drawElementsInstancedBaseVertexImplementation = glDrawElementsInstancedBaseVertexEXT;
        #endif
    } else if(context.isExtensionSupported<Extensions::OES::draw_elements_base_vertex>()) {
        extensions[Extensions::OES::draw_elements_base_vertex::Index] =
                   Extensions::OES::draw_elements_base_vertex::string();

        drawElementsBaseVertexImplementation = glDrawElementsBaseVertexOES;
        #ifndef MAGNUM_TARGET_GLES2
        drawRangeElementsBaseVertexImplementation = glDrawRangeElementsBaseVertexOES;
        drawElementsInstancedBaseVertexImplementation = glDrawElementsInstancedBaseVertexOES;
        #endif
    } else
    #else
    if(context.isExtensionSupported<Extensions::WEBGL::draw_instanced_base_vertex_base_instance>()) {
        extensions[Extensions::WEBGL::draw_instanced_base_vertex_base_instance::Index] =
                   Extensions::WEBGL::draw_instanced_base_vertex_base_instance::string();

        /* The WEBGL extension uses the same entrypoints as the ANGLE extension
           it was based on, however we wrap it to supply trivial instance count
           because there's no non-instanced variant. Only available since
           1.39.15: https://github.com/emscripten-core/emscripten/pull/11054 */
        #if __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 13915
        drawElementsBaseVertexImplementation = Mesh::drawElementsBaseVertexImplementationANGLE;
        drawRangeElementsBaseVertexImplementation = Mesh::drawRangeElementsBaseVertexImplementationANGLE;
        drawElementsInstancedBaseVertexImplementation = Mesh::drawElementsInstancedBaseVertexImplementationANGLE;
        #else
        /* In Context::setupDriverWorkarounds() we make sure the extension is
           not even advertised, so this shouldn't be reached. */
        CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        #endif
    } else
    #endif
    {
        drawElementsBaseVertexImplementation = Mesh::drawElementsBaseVertexImplementationAssert;
        #ifndef MAGNUM_TARGET_GLES2
        drawRangeElementsBaseVertexImplementation = Mesh::drawRangeElementsBaseVertexImplementationAssert;
        drawElementsInstancedBaseVertexImplementation = Mesh::drawElementsInstancedBaseVertexImplementationAssert;
        #endif
    }
    #endif

    /* Base instance draws on ES3 and WebGL2 */
    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_WEBGL
    #ifndef MAGNUM_TARGET_GLES2
    if(context.isExtensionSupported<Extensions::ANGLE::base_vertex_base_instance>()) {
        extensions[Extensions::ANGLE::base_vertex_base_instance::Index] =
                   Extensions::ANGLE::base_vertex_base_instance::string();

        drawArraysInstancedBaseInstanceImplementation = glDrawArraysInstancedBaseInstanceANGLE;
        /* This variant isn't in the ext, emulated using
           glDrawElementsInstancedBaseVertexBaseInstanceANGLE */
        drawElementsInstancedBaseInstanceImplementation = Mesh::drawElementsInstancedBaseInstanceImplementationANGLE;
        drawElementsInstancedBaseVertexBaseInstanceImplementation = glDrawElementsInstancedBaseVertexBaseInstanceANGLE;
    } else
    #endif
    #else
    if(context.isExtensionSupported<Extensions::WEBGL::draw_instanced_base_vertex_base_instance>()) {
        extensions[Extensions::WEBGL::draw_instanced_base_vertex_base_instance::Index] =
                   Extensions::WEBGL::draw_instanced_base_vertex_base_instance::string();

        /* The WEBGL extension uses the same entrypoints as the ANGLE extension
           it was based on. Only available since 1.39.15:
           https://github.com/emscripten-core/emscripten/pull/11054 */
        #if __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 13915
        drawArraysInstancedBaseInstanceImplementation = glDrawArraysInstancedBaseInstanceANGLE;
        /* This variant isn't in the ext, emulated using
           glDrawElementsInstancedBaseVertexBaseInstanceANGLE */
        drawElementsInstancedBaseInstanceImplementation = Mesh::drawElementsInstancedBaseInstanceImplementationANGLE;
        drawElementsInstancedBaseVertexBaseInstanceImplementation = glDrawElementsInstancedBaseVertexBaseInstanceANGLE;
        #else
        /* In Context::setupDriverWorkarounds() we make sure the extension is
           not even advertised, so this shouldn't be reached. */
        CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        #endif
    } else
    #endif
    {
        drawArraysInstancedBaseInstanceImplementation = Mesh::drawArraysInstancedBaseInstanceImplementationAssert;
        drawElementsInstancedBaseInstanceImplementation = Mesh::drawElementsInstancedBaseInstanceImplementationAssert;
        drawElementsInstancedBaseVertexBaseInstanceImplementation = Mesh::drawElementsInstancedBaseVertexBaseInstanceImplementationAssert;
    }
    #endif

    #ifdef MAGNUM_TARGET_GLES
    /* Multi draw implementation on ES. Because there's a lot of dispatch logic
       involved, the multiDrawImplementationDefault then has internal
       extension-specific codepaths based on whether EXT, OES, ANGLE or
       whichever entrypoints are supported. */
    #ifndef MAGNUM_TARGET_WEBGL
    if(context.isExtensionSupported<Extensions::EXT::multi_draw_arrays>() ||
       context.isExtensionSupported<Extensions::ANGLE::multi_draw>())
    #else
    if(context.isExtensionSupported<Extensions::WEBGL::multi_draw>())
    #endif
    {
        #ifndef MAGNUM_TARGET_WEBGL
        if(context.isExtensionSupported<Extensions::EXT::multi_draw_arrays>()) {
            extensions[Extensions::EXT::multi_draw_arrays::Index] =
                       Extensions::EXT::multi_draw_arrays::string();

            multiDrawArraysImplementation = glMultiDrawArraysEXT;
            multiDrawElementsImplementation = glMultiDrawElementsEXT;
        } else if(context.isExtensionSupported<Extensions::ANGLE::multi_draw>()) {
            extensions[Extensions::ANGLE::multi_draw::Index] =
                       Extensions::ANGLE::multi_draw::string();

            multiDrawArraysImplementation = glMultiDrawArraysANGLE;
            multiDrawElementsImplementation = glMultiDrawElementsANGLE;
        } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        #else
        {
            extensions[Extensions::WEBGL::multi_draw::Index] =
                       Extensions::WEBGL::multi_draw::string();

            /* The WEBGL extension uses the same entrypoints as the ANGLE
               extension it was based on. Only available since 2.0.0:
               https://github.com/emscripten-core/emscripten/pull/11650 */
            #if __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 20000
            multiDrawArraysImplementation = glMultiDrawArraysANGLE;
            multiDrawElementsImplementation = glMultiDrawElementsANGLE;
            #else
            /* In Context::setupDriverWorkarounds() we make sure the extension
               is not even advertised, so this shouldn't be reached. */
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
            #endif
        }
        #endif

        /* These function pointers make sense only if the general multi-draw
           extension is supported. Also, not on WebGL 1 at all. */
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        #ifndef MAGNUM_TARGET_WEBGL
        if(context.isExtensionSupported<Extensions::EXT::draw_elements_base_vertex>()) {
            extensions[Extensions::EXT::draw_elements_base_vertex::Index] =
                       Extensions::EXT::draw_elements_base_vertex::string();

            multiDrawElementsBaseVertexImplementation = glMultiDrawElementsBaseVertexEXT;
        } else if(context.isExtensionSupported<Extensions::OES::draw_elements_base_vertex>()) {
            extensions[Extensions::OES::draw_elements_base_vertex::Index] =
                       Extensions::OES::draw_elements_base_vertex::string();

            /* Yes, it's really EXT, the same as with
               EXT_draw_elements_base_vertex. I have no idea why the two
               extensions exist and why it isn't just one. */
            multiDrawElementsBaseVertexImplementation = glMultiDrawElementsBaseVertexEXT;
        } else
        #else
        if(context.isExtensionSupported<Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance>()) {
            extensions[Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance::Index] =
                       Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance::string();

            /* The WEBGL extension uses the same entrypoints as the ANGLE
               extension it was based on, however we wrap it and supply trivial
               instance counts because there's no non-instanced variant. Only
               available since 2.0.5: https://github.com/emscripten-core/emscripten/pull/12282 */
            #if __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 20005
            multiDrawElementsBaseVertexImplementation = MeshView::multiDrawElementsBaseVertexImplementationANGLE;
            #else
            /* In Context::setupDriverWorkarounds() we make sure the extension
               is not even advertised, so this shouldn't be reached. */
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
            #endif
        } else
        #endif
        {
            multiDrawElementsBaseVertexImplementation = MeshView::multiDrawElementsBaseVertexImplementationAssert;
        }
        #endif

        multiDrawImplementation = &MeshView::multiDrawImplementationDefault;

    } else multiDrawImplementation = &MeshView::multiDrawImplementationFallback;
    #endif

    #ifdef MAGNUM_TARGET_GLES2
    /* Instanced draw ímplementation on ES2 */
    if(context.isExtensionSupported<Extensions::ANGLE::instanced_arrays>()) {
        extensions[Extensions::ANGLE::instanced_arrays::Index] =
                   Extensions::ANGLE::instanced_arrays::string();

        drawArraysInstancedImplementation = glDrawArraysInstancedANGLE;
        drawElementsInstancedImplementation = glDrawElementsInstancedANGLE;
    }
    #ifndef MAGNUM_TARGET_WEBGL
    else if(context.isExtensionSupported<Extensions::EXT::instanced_arrays>() ||
            context.isExtensionSupported<Extensions::EXT::draw_instanced>()) {
        if(context.isExtensionSupported<Extensions::EXT::instanced_arrays>())
            extensions[Extensions::EXT::instanced_arrays::Index] =
                       Extensions::EXT::instanced_arrays::string();
        else extensions[Extensions::EXT::draw_instanced::Index] =
                        Extensions::EXT::draw_instanced::string();

        drawArraysInstancedImplementation = glDrawArraysInstancedEXT;
        drawElementsInstancedImplementation = glDrawElementsInstancedEXT;

    } else if(context.isExtensionSupported<Extensions::NV::instanced_arrays>() ||
              context.isExtensionSupported<Extensions::NV::draw_instanced>()) {
        if(context.isExtensionSupported<Extensions::NV::instanced_arrays>())
            extensions[Extensions::NV::instanced_arrays::Index] =
                       Extensions::NV::instanced_arrays::string();
        else extensions[Extensions::NV::draw_instanced::Index] =
                        Extensions::NV::draw_instanced::string();

        drawArraysInstancedImplementation = glDrawArraysInstancedNV;
        drawElementsInstancedImplementation = glDrawElementsInstancedNV;
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
        context.isDriverWorkaroundDisabled("intel-windows-crazy-broken-vao-dsa"_s))
        #endif
    ) {
        extensions[Extensions::ARB::direct_state_access::Index] =
                   Extensions::ARB::direct_state_access::string();

        vertexAttribDivisorImplementation = &Mesh::vertexAttribDivisorImplementationVAODSA;
    } else if(context.isExtensionSupported<Extensions::ARB::vertex_array_object>()) {
        extensions[Extensions::ARB::vertex_array_object::Index] =
                   Extensions::ARB::vertex_array_object::string();

        vertexAttribDivisorImplementation = &Mesh::vertexAttribDivisorImplementationVAO;
    } else {
        vertexAttribDivisorImplementation = nullptr;
    }
    #elif defined(MAGNUM_TARGET_GLES2)
    /* Instanced arrays implementation on ES2 */
    if(context.isExtensionSupported<Extensions::ANGLE::instanced_arrays>()) {
        extensions[Extensions::ANGLE::instanced_arrays::Index] =
                   Extensions::ANGLE::instanced_arrays::string();

        vertexAttribDivisorImplementation = &Mesh::vertexAttribDivisorImplementationANGLE;
    }
    #ifndef MAGNUM_TARGET_WEBGL
    else if(context.isExtensionSupported<Extensions::EXT::instanced_arrays>()) {
        extensions[Extensions::EXT::instanced_arrays::Index] =
                   Extensions::EXT::instanced_arrays::string();

        vertexAttribDivisorImplementation = &Mesh::vertexAttribDivisorImplementationEXT;

    } else if(context.isExtensionSupported<Extensions::NV::instanced_arrays>()) {
        extensions[Extensions::NV::instanced_arrays::Index] =
                   Extensions::NV::instanced_arrays::string();

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

#ifndef MAGNUM_TARGET_GLES
MeshState::~MeshState() {
    /* If the default VAO was created, we need to delete it to avoid leaks.
       Delete also the scratch VAO if the engine was so unlucky to have to run
       awful external GL code (it was created in Context::resetState()). */
    if(defaultVAO) glDeleteVertexArrays(1, &defaultVAO);
    if(scratchVAO) glDeleteVertexArrays(1, &scratchVAO);
}
#endif

void MeshState::reset() {
    currentVAO = State::DisengagedBinding;
}

}}}
