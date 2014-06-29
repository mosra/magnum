/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"

namespace Magnum {

void Context::setupDriverWorkarounds() {
    #define _setRequiredVersion(extension, version)                           \
        if(_extensionRequiredVersion[Extensions::extension::Index] < Version::version) \
            _extensionRequiredVersion[Extensions::extension::Index] = Version::version

    #ifndef MAGNUM_TARGET_GLES
    /* This extension causes crash in GLSL compiler on AMD linux drivers 13.251 */
    if(detectedDriver() & DetectedDriver::AMD)
        _setRequiredVersion(GL::ARB::explicit_uniform_location, None);
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /* Layout qualifier causes compiler error with GLSL 1.20 on Mesa, GLSL 1.30
       on NVidia and 1.40 on Mac OS X */
    /** @todo Different version on different vendors? */
    _setRequiredVersion(GL::ARB::explicit_attrib_location, GL320);
    _setRequiredVersion(GL::ARB::explicit_uniform_location, GL320);
    _setRequiredVersion(GL::ARB::shading_language_420pack, GL320);
    #endif

    #ifdef MAGNUM_TARGET_GLES
    /* Disable extensions for which we need extension loader, as they would
       crash otherwise. */
    /** @todo Remove this when extension loader for ES is available */
    #ifndef CORRADE_TARGET_NACL
    _setRequiredVersion(GL::CHROMIUM::map_sub, None);
    #endif
    _setRequiredVersion(GL::EXT::multi_draw_arrays, None);
    _setRequiredVersion(GL::EXT::debug_label, None);
    _setRequiredVersion(GL::EXT::debug_marker, None);
    _setRequiredVersion(GL::EXT::disjoint_timer_query, None);
    _setRequiredVersion(GL::EXT::separate_shader_objects, None);
    _setRequiredVersion(GL::EXT::multisampled_render_to_texture, None);
    _setRequiredVersion(GL::EXT::robustness, None);
    _setRequiredVersion(GL::KHR::debug, None);
    _setRequiredVersion(GL::NV::read_buffer_front, None);
    _setRequiredVersion(GL::OES::mapbuffer, None);
    #ifdef MAGNUM_TARGET_GLES2
    _setRequiredVersion(GL::ANGLE::framebuffer_blit, None);
    _setRequiredVersion(GL::ANGLE::framebuffer_multisample, None);
    _setRequiredVersion(GL::ANGLE::instanced_arrays, None);
    _setRequiredVersion(GL::APPLE::framebuffer_multisample, None);
    _setRequiredVersion(GL::EXT::discard_framebuffer, None);
    _setRequiredVersion(GL::EXT::blend_minmax, None);
    #ifndef CORRADE_TARGET_NACL
    _setRequiredVersion(GL::EXT::occlusion_query_boolean, None);
    #endif
    _setRequiredVersion(GL::EXT::texture_storage, None);
    _setRequiredVersion(GL::EXT::map_buffer_range, None);
    _setRequiredVersion(GL::EXT::instanced_arrays, None);
    _setRequiredVersion(GL::EXT::draw_instanced, None);
    _setRequiredVersion(GL::NV::draw_buffers, None);
    _setRequiredVersion(GL::NV::fbo_color_attachments, None); // ??
    _setRequiredVersion(GL::NV::read_buffer, None);
    _setRequiredVersion(GL::NV::draw_instanced, None);
    _setRequiredVersion(GL::NV::framebuffer_blit, None);
    _setRequiredVersion(GL::NV::framebuffer_multisample, None);
    _setRequiredVersion(GL::NV::instanced_arrays, None);
    _setRequiredVersion(GL::OES::texture_3D, None);
    _setRequiredVersion(GL::OES::vertex_array_object, None);
    #endif
    #endif

    #undef _setRequiredVersion
}

}
