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

#include "ShaderProgramState.h"

#include "Magnum/AbstractShaderProgram.h"
#include "Magnum/Context.h"
#include "Magnum/Extensions.h"

namespace Magnum { namespace Implementation {

ShaderProgramState::ShaderProgramState(Context& context, std::vector<std::string>& extensions): current(0), maxVertexAttributes(0)
        #ifndef MAGNUM_TARGET_GLES
        , maxAtomicCounterBufferSize(0), maxComputeSharedMemorySize(0), maxComputeWorkGroupInvocations(0), maxImageUnits(0), maxImageSamples(0), maxCombinedShaderOutputResources(0), maxUniformLocations(0), maxShaderStorageBlockSize(0)
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        , minTexelOffset(0), maxTexelOffset(0), maxUniformBlockSize(0)
        #endif
{
    /** @todo OpenGL ES 2 has extension @es_extension{EXT,separate_shader_objects} for this */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::ARB::separate_shader_objects>() ||
       context.isExtensionSupported<Extensions::GL::EXT::direct_state_access>())
    {
        extensions.push_back(context.isExtensionSupported<Extensions::GL::ARB::separate_shader_objects>() ?
            Extensions::GL::ARB::separate_shader_objects::string() : Extensions::GL::EXT::direct_state_access::string());

        uniform1fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform2fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform3fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform4fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform1ivImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform2ivImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform3ivImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform4ivImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform1uivImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform2uivImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform3uivImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform4uivImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform1dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform2dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform3dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform4dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;

        uniformMatrix2fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix3fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix4fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix2x3fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix3x2fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix2x4fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix4x2fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix3x4fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix4x3fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix2dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix3dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix4dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix2x3dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix3x2dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix2x4dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix4x2dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix3x4dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix4x3dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
    } else
    #endif
    {
        #ifdef MAGNUM_TARGET_GLES
        static_cast<void>(context);
        static_cast<void>(extensions);
        #endif

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
}

}}
