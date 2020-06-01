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

#include "ShaderProgramState.h"

#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"

#include "State.h"

namespace Magnum { namespace GL { namespace Implementation {

ShaderProgramState::ShaderProgramState(Context& context, std::vector<std::string>& extensions): current(0), maxVertexAttributes(0)
        #ifndef MAGNUM_TARGET_GLES2
        #ifndef MAGNUM_TARGET_WEBGL
        , maxGeometryOutputVertices{0}, maxAtomicCounterBufferSize(0), maxComputeSharedMemorySize(0), maxComputeWorkGroupInvocations(0), maxImageUnits(0), maxCombinedShaderOutputResources(0), maxUniformLocations(0)
        #endif
        , minTexelOffset(0), maxTexelOffset(0), maxUniformBlockSize(0)
        #ifndef MAGNUM_TARGET_WEBGL
        , maxShaderStorageBlockSize(0)
        #endif
        #endif
        #ifndef MAGNUM_TARGET_GLES
        , maxImageSamples(0)
        #endif
{
    #ifndef MAGNUM_TARGET_GLES2
    #ifdef CORRADE_TARGET_WINDOWS
    if((context.detectedDriver() & Context::DetectedDriver::NVidia) &&
        !context.isDriverWorkaroundDisabled("nv-windows-dangling-transform-feedback-varying-names"))
    {
        transformFeedbackVaryingsImplementation = &AbstractShaderProgram::transformFeedbackVaryingsImplementationDanglingWorkaround;
    } else
    #endif
    {
        transformFeedbackVaryingsImplementation = &AbstractShaderProgram::transformFeedbackVaryingsImplementationDefault;
    }
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::ARB::separate_shader_objects>())
    #else
    if(context.isVersionSupported(Version::GLES310))
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        extensions.emplace_back(Extensions::ARB::separate_shader_objects::string());
        #endif

        uniform1fvImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniform2fvImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniform3fvImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniform4fvImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniform1ivImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniform2ivImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniform3ivImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniform4ivImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniform1uivImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniform2uivImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniform3uivImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniform4uivImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        #ifndef MAGNUM_TARGET_GLES
        uniform1dvImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniform2dvImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniform3dvImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniform4dvImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        #endif

        uniformMatrix2fvImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniformMatrix3fvImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniformMatrix4fvImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniformMatrix2x3fvImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniformMatrix3x2fvImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniformMatrix2x4fvImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniformMatrix4x2fvImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniformMatrix3x4fvImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniformMatrix4x3fvImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        #ifndef MAGNUM_TARGET_GLES
        uniformMatrix2dvImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniformMatrix3dvImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniformMatrix4dvImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniformMatrix2x3dvImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniformMatrix3x2dvImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniformMatrix2x4dvImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniformMatrix4x2dvImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniformMatrix3x4dvImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        uniformMatrix4x3dvImplementation = &AbstractShaderProgram::uniformImplementationSSO;
        #endif
    } else
    #endif

    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    if(context.isExtensionSupported<Extensions::EXT::separate_shader_objects>()) {
        extensions.push_back(Extensions::EXT::separate_shader_objects::string());

        uniform1fvImplementation = &AbstractShaderProgram::uniformImplementationSSOEXT;
        uniform2fvImplementation = &AbstractShaderProgram::uniformImplementationSSOEXT;
        uniform3fvImplementation = &AbstractShaderProgram::uniformImplementationSSOEXT;
        uniform4fvImplementation = &AbstractShaderProgram::uniformImplementationSSOEXT;
        uniform1ivImplementation = &AbstractShaderProgram::uniformImplementationSSOEXT;
        uniform2ivImplementation = &AbstractShaderProgram::uniformImplementationSSOEXT;
        uniform3ivImplementation = &AbstractShaderProgram::uniformImplementationSSOEXT;
        uniform4ivImplementation = &AbstractShaderProgram::uniformImplementationSSOEXT;
        #ifndef MAGNUM_TARGET_GLES2
        uniform1uivImplementation = &AbstractShaderProgram::uniformImplementationSSOEXT;
        uniform2uivImplementation = &AbstractShaderProgram::uniformImplementationSSOEXT;
        uniform3uivImplementation = &AbstractShaderProgram::uniformImplementationSSOEXT;
        uniform4uivImplementation = &AbstractShaderProgram::uniformImplementationSSOEXT;
        #endif

        uniformMatrix2fvImplementation = &AbstractShaderProgram::uniformImplementationSSOEXT;
        uniformMatrix3fvImplementation = &AbstractShaderProgram::uniformImplementationSSOEXT;
        uniformMatrix4fvImplementation = &AbstractShaderProgram::uniformImplementationSSOEXT;
        #ifndef MAGNUM_TARGET_GLES2
        uniformMatrix2x3fvImplementation = &AbstractShaderProgram::uniformImplementationSSOEXT;
        uniformMatrix3x2fvImplementation = &AbstractShaderProgram::uniformImplementationSSOEXT;
        uniformMatrix2x4fvImplementation = &AbstractShaderProgram::uniformImplementationSSOEXT;
        uniformMatrix4x2fvImplementation = &AbstractShaderProgram::uniformImplementationSSOEXT;
        uniformMatrix3x4fvImplementation = &AbstractShaderProgram::uniformImplementationSSOEXT;
        uniformMatrix4x3fvImplementation = &AbstractShaderProgram::uniformImplementationSSOEXT;
        #endif
    } else
    #endif
    {
        uniform1fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        uniform2fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        uniform3fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        uniform4fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        uniform1ivImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        uniform2ivImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        uniform3ivImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        uniform4ivImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        #ifndef MAGNUM_TARGET_GLES2
        uniform1uivImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        uniform2uivImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        uniform3uivImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        uniform4uivImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        #endif
        #ifndef MAGNUM_TARGET_GLES
        uniform1dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        uniform2dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        uniform3dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        uniform4dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        #endif

        uniformMatrix2fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        uniformMatrix3fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        uniformMatrix4fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        #ifndef MAGNUM_TARGET_GLES2
        uniformMatrix2x3fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        uniformMatrix3x2fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        uniformMatrix2x4fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        uniformMatrix4x2fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        uniformMatrix3x4fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        uniformMatrix4x3fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        #endif
        #ifndef MAGNUM_TARGET_GLES
        uniformMatrix2dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        uniformMatrix3dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        uniformMatrix4dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        uniformMatrix2x3dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        uniformMatrix3x2dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        uniformMatrix2x4dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        uniformMatrix4x2dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        uniformMatrix3x4dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        uniformMatrix4x3dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
        #endif
    }

    #ifdef MAGNUM_TARGET_WEBGL
    static_cast<void>(context);
    static_cast<void>(extensions);
    #endif
}

void ShaderProgramState::reset() {
    current = State::DisengagedBinding;
}

}}}
