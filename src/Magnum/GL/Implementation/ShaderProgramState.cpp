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

#include "ShaderProgramState.h"

#include <Corrade/Containers/StringView.h>

#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"

#include "State.h"

namespace Magnum { namespace GL { namespace Implementation {

using namespace Containers::Literals;

ShaderProgramState::ShaderProgramState(Context& context, Containers::StaticArrayView<Implementation::ExtensionCount, const char*> extensions): current(0), maxVertexAttributes(0)
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
        !context.isDriverWorkaroundDisabled("nv-windows-dangling-transform-feedback-varying-names"_s))
    {
        transformFeedbackVaryingsImplementation = &AbstractShaderProgram::transformFeedbackVaryingsImplementationDanglingWorkaround;
    } else
    #endif
    {
        transformFeedbackVaryingsImplementation = &AbstractShaderProgram::transformFeedbackVaryingsImplementationDefault;
    }
    #endif

    #if defined(CORRADE_TARGET_WINDOWS) && !defined(MAGNUM_TARGET_GLES)
    if((context.detectedDriver() & Context::DetectedDriver::IntelWindows) && !context.isDriverWorkaroundDisabled("intel-windows-chatty-shader-compiler"_s)) {
        cleanLogImplementation = &AbstractShaderProgram::cleanLogImplementationIntelWindows;
    } else
    #endif
    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    if((context.detectedDriver() & Context::DetectedDriver::Angle) && !context.isDriverWorkaroundDisabled("angle-chatty-shader-compiler"_s)) {
        cleanLogImplementation = &AbstractShaderProgram::cleanLogImplementationAngle;
    } else
    #endif
    {
        cleanLogImplementation = &AbstractShaderProgram::cleanLogImplementationNoOp;
    }

    #ifndef MAGNUM_TARGET_WEBGL
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::ARB::separate_shader_objects>())
    #else
    if(context.isVersionSupported(Version::GLES310))
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        extensions[Extensions::ARB::separate_shader_objects::Index] =
                   Extensions::ARB::separate_shader_objects::string();
        #endif

        uniform1fvImplementation = glProgramUniform1fv;
        uniform2fvImplementation = glProgramUniform2fv;
        uniform3fvImplementation = glProgramUniform3fv;
        uniform4fvImplementation = glProgramUniform4fv;
        uniform1ivImplementation = glProgramUniform1iv;
        uniform2ivImplementation = glProgramUniform2iv;
        uniform3ivImplementation = glProgramUniform3iv;
        uniform4ivImplementation = glProgramUniform4iv;
        uniform1uivImplementation = glProgramUniform1uiv;
        uniform2uivImplementation = glProgramUniform2uiv;
        uniform3uivImplementation = glProgramUniform3uiv;
        uniform4uivImplementation = glProgramUniform4uiv;
        #ifndef MAGNUM_TARGET_GLES
        uniform1dvImplementation = glProgramUniform1dv;
        uniform2dvImplementation = glProgramUniform2dv;
        uniform3dvImplementation = glProgramUniform3dv;
        uniform4dvImplementation = glProgramUniform4dv;
        #endif

        uniformMatrix2fvImplementation = glProgramUniformMatrix2fv;
        uniformMatrix3fvImplementation = glProgramUniformMatrix3fv;
        uniformMatrix4fvImplementation = glProgramUniformMatrix4fv;
        uniformMatrix2x3fvImplementation = glProgramUniformMatrix2x3fv;
        uniformMatrix3x2fvImplementation = glProgramUniformMatrix3x2fv;
        uniformMatrix2x4fvImplementation = glProgramUniformMatrix2x4fv;
        uniformMatrix4x2fvImplementation = glProgramUniformMatrix4x2fv;
        uniformMatrix3x4fvImplementation = glProgramUniformMatrix3x4fv;
        uniformMatrix4x3fvImplementation = glProgramUniformMatrix4x3fv;
        #ifndef MAGNUM_TARGET_GLES
        uniformMatrix2dvImplementation = glProgramUniformMatrix2dv;
        uniformMatrix3dvImplementation = glProgramUniformMatrix3dv;
        uniformMatrix4dvImplementation = glProgramUniformMatrix4dv;
        uniformMatrix2x3dvImplementation = glProgramUniformMatrix2x3dv;
        uniformMatrix3x2dvImplementation = glProgramUniformMatrix3x2dv;
        uniformMatrix2x4dvImplementation = glProgramUniformMatrix2x4dv;
        uniformMatrix4x2dvImplementation = glProgramUniformMatrix4x2dv;
        uniformMatrix3x4dvImplementation = glProgramUniformMatrix3x4dv;
        uniformMatrix4x3dvImplementation = glProgramUniformMatrix4x3dv;
        #endif
    } else
    #endif

    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    if(context.isExtensionSupported<Extensions::EXT::separate_shader_objects>()) {
        extensions[Extensions::EXT::separate_shader_objects::Index] =
                   Extensions::EXT::separate_shader_objects::string();

        uniform1fvImplementation = glProgramUniform1fvEXT;
        uniform2fvImplementation = glProgramUniform2fvEXT;
        uniform3fvImplementation = glProgramUniform3fvEXT;
        uniform4fvImplementation = glProgramUniform4fvEXT;
        uniform1ivImplementation = glProgramUniform1ivEXT;
        uniform2ivImplementation = glProgramUniform2ivEXT;
        uniform3ivImplementation = glProgramUniform3ivEXT;
        uniform4ivImplementation = glProgramUniform4ivEXT;
        #ifndef MAGNUM_TARGET_GLES2
        uniform1uivImplementation = glProgramUniform1uivEXT;
        uniform2uivImplementation = glProgramUniform2uivEXT;
        uniform3uivImplementation = glProgramUniform3uivEXT;
        uniform4uivImplementation = glProgramUniform4uivEXT;
        #endif

        uniformMatrix2fvImplementation = glProgramUniformMatrix2fvEXT;
        uniformMatrix3fvImplementation = glProgramUniformMatrix3fvEXT;
        uniformMatrix4fvImplementation = glProgramUniformMatrix4fvEXT;
        #ifndef MAGNUM_TARGET_GLES2
        uniformMatrix2x3fvImplementation = glProgramUniformMatrix2x3fvEXT;
        uniformMatrix3x2fvImplementation = glProgramUniformMatrix3x2fvEXT;
        uniformMatrix2x4fvImplementation = glProgramUniformMatrix2x4fvEXT;
        uniformMatrix4x2fvImplementation = glProgramUniformMatrix4x2fvEXT;
        uniformMatrix3x4fvImplementation = glProgramUniformMatrix3x4fvEXT;
        uniformMatrix4x3fvImplementation = glProgramUniformMatrix4x3fvEXT;
        #endif
    } else
    #endif
    {
        uniform1fvImplementation = &AbstractShaderProgram::uniform1fvImplementationDefault;
        uniform2fvImplementation = &AbstractShaderProgram::uniform2fvImplementationDefault;
        uniform3fvImplementation = &AbstractShaderProgram::uniform3fvImplementationDefault;
        uniform4fvImplementation = &AbstractShaderProgram::uniform4fvImplementationDefault;
        uniform1ivImplementation = &AbstractShaderProgram::uniform1ivImplementationDefault;
        uniform2ivImplementation = &AbstractShaderProgram::uniform2ivImplementationDefault;
        uniform3ivImplementation = &AbstractShaderProgram::uniform3ivImplementationDefault;
        uniform4ivImplementation = &AbstractShaderProgram::uniform4ivImplementationDefault;
        #ifndef MAGNUM_TARGET_GLES2
        uniform1uivImplementation = &AbstractShaderProgram::uniform1uivImplementationDefault;
        uniform2uivImplementation = &AbstractShaderProgram::uniform2uivImplementationDefault;
        uniform3uivImplementation = &AbstractShaderProgram::uniform3uivImplementationDefault;
        uniform4uivImplementation = &AbstractShaderProgram::uniform4uivImplementationDefault;
        #endif
        #ifndef MAGNUM_TARGET_GLES
        uniform1dvImplementation = &AbstractShaderProgram::uniform1dvImplementationDefault;
        uniform2dvImplementation = &AbstractShaderProgram::uniform2dvImplementationDefault;
        uniform3dvImplementation = &AbstractShaderProgram::uniform3dvImplementationDefault;
        uniform4dvImplementation = &AbstractShaderProgram::uniform4dvImplementationDefault;
        #endif

        uniformMatrix2fvImplementation = &AbstractShaderProgram::uniformMatrix2fvImplementationDefault;
        uniformMatrix3fvImplementation = &AbstractShaderProgram::uniformMatrix3fvImplementationDefault;
        uniformMatrix4fvImplementation = &AbstractShaderProgram::uniformMatrix4fvImplementationDefault;
        #ifndef MAGNUM_TARGET_GLES2
        uniformMatrix2x3fvImplementation = &AbstractShaderProgram::uniformMatrix2x3fvImplementationDefault;
        uniformMatrix3x2fvImplementation = &AbstractShaderProgram::uniformMatrix3x2fvImplementationDefault;
        uniformMatrix2x4fvImplementation = &AbstractShaderProgram::uniformMatrix2x4fvImplementationDefault;
        uniformMatrix4x2fvImplementation = &AbstractShaderProgram::uniformMatrix4x2fvImplementationDefault;
        uniformMatrix3x4fvImplementation = &AbstractShaderProgram::uniformMatrix3x4fvImplementationDefault;
        uniformMatrix4x3fvImplementation = &AbstractShaderProgram::uniformMatrix4x3fvImplementationDefault;
        #endif
        #ifndef MAGNUM_TARGET_GLES
        uniformMatrix2dvImplementation = &AbstractShaderProgram::uniformMatrix2dvImplementationDefault;
        uniformMatrix3dvImplementation = &AbstractShaderProgram::uniformMatrix3dvImplementationDefault;
        uniformMatrix4dvImplementation = &AbstractShaderProgram::uniformMatrix4dvImplementationDefault;
        uniformMatrix2x3dvImplementation = &AbstractShaderProgram::uniformMatrix2x3dvImplementationDefault;
        uniformMatrix3x2dvImplementation = &AbstractShaderProgram::uniformMatrix3x2dvImplementationDefault;
        uniformMatrix2x4dvImplementation = &AbstractShaderProgram::uniformMatrix2x4dvImplementationDefault;
        uniformMatrix4x2dvImplementation = &AbstractShaderProgram::uniformMatrix4x2dvImplementationDefault;
        uniformMatrix3x4dvImplementation = &AbstractShaderProgram::uniformMatrix3x4dvImplementationDefault;
        uniformMatrix4x3dvImplementation = &AbstractShaderProgram::uniformMatrix4x3dvImplementationDefault;
        #endif
    }
    #endif

    #ifdef MAGNUM_TARGET_WEBGL
    static_cast<void>(context);
    static_cast<void>(extensions);
    #endif
}

void ShaderProgramState::reset() {
    current = State::DisengagedBinding;
}

}}}
