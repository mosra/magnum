#ifndef Magnum_Vk_Implementation_spirvPatching_h
#define Magnum_Vk_Implementation_spirvPatching_h
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

#include <Corrade/Containers/ArrayTuple.h>

#include "Magnum/Math/Functions.h"

/* All code in this file should be self-contained, with no link-time dependency
   on ShaderTools */
#include "Magnum/ShaderTools/Implementation/spirv.h"

namespace Magnum { namespace Vk { namespace Implementation { namespace {

/* This kinda throws const-correctness through the window -- the the SPIR-V
   utils work on const data, but this function actually does mutate them */
bool spirvPatchSwiftShaderConflictingMultiEntrypointLocations(Containers::ArrayView<const UnsignedInt> data) {
    /* Find vertex/fragment entrypoints and count how many is there in total */
    UnsignedInt entrypointCount = 0;
    Containers::Optional<ShaderTools::Implementation::SpirvEntrypoint> vertexEntryPoint, fragmentEntryPoint;
    while(Containers::Optional<ShaderTools::Implementation::SpirvEntrypoint> entrypoint = ShaderTools::Implementation::spirvNextEntrypoint(data)) {
        ++entrypointCount;
        if(entrypoint->executionModel == SpvExecutionModelVertex)
            vertexEntryPoint = *entrypoint;
        else if(entrypoint->executionModel == SpvExecutionModelFragment)
            fragmentEntryPoint = *entrypoint;
    }

    /* If there aren't both entrypoints, this bug doesn't affect the shader. If
       there are more, we won't attempt anything -- right now SwiftShader
       doesn't support geom/tess shaders, so the only possibility is that the
       module is a library of multiple different vertex / fragment
       implementations and that's too frightening as any patching would most
       likely break things *really bad*. */
    if(entrypointCount > 2 || !vertexEntryPoint || !fragmentEntryPoint)
        return false;

    /* Get locations and storage classes for all entrypoint interfaces */
    Containers::ArrayView<ShaderTools::Implementation::SpirvEntrypointInterface> vertexInterface, fragmentInterface;
    Containers::ArrayTuple interfaceData{
        {ValueInit, vertexEntryPoint->interfaces.size(), vertexInterface},
        {ValueInit, fragmentEntryPoint->interfaces.size(), fragmentInterface}
    };
    spirvEntrypointInterface(data, *vertexEntryPoint, vertexInterface);
    spirvEntrypointInterface(data, *fragmentEntryPoint, fragmentInterface);

    /* Calculate the max location so we know what to change to */
    UnsignedInt maxLocation = 0;
    for(const ShaderTools::Implementation::SpirvEntrypointInterface& i: vertexInterface)
        if(i.location) maxLocation = Math::max(maxLocation, *i.location);
    for(const ShaderTools::Implementation::SpirvEntrypointInterface& i: fragmentInterface)
        if(i.location) maxLocation = Math::max(maxLocation, *i.location);

    /* For all vertex outputs check if there are fragment outputs with the same
       locations */
    for(const ShaderTools::Implementation::SpirvEntrypointInterface& vertexOutput: vertexInterface) {
        /* Ignore what's not an output or what doesn't have a location (for
           example a builtin) */
        if(!vertexOutput.storageClass || *vertexOutput.storageClass != SpvStorageClassOutput || !vertexOutput.location)
            continue;

        for(const ShaderTools::Implementation::SpirvEntrypointInterface& fragmentOutput: fragmentInterface) {
            /* Ignore what's not an output or what doesn't have a location (for
               example a builtin) */
            if(!fragmentOutput.storageClass || *fragmentOutput.storageClass != SpvStorageClassOutput || !fragmentOutput.location) continue;

            /* The same location used, we need to remap. Use the next highest
               unused location and change also the corresponding fragment
               input, if there's any. */
            if(*vertexOutput.location == *fragmentOutput.location) {
                const UnsignedInt newLocation = ++maxLocation;
                for(const ShaderTools::Implementation::SpirvEntrypointInterface& fragmentInput: fragmentInterface) {
                    if(*fragmentInput.storageClass != SpvStorageClassInput)
                        continue;
                    if(*fragmentInput.location == *vertexOutput.location) {
                        /** @todo ugly! */
                        *const_cast<UnsignedInt*>(fragmentInput.location) = newLocation;
                        break;
                    }
                }
                /** @todo ugly! */
                *const_cast<UnsignedInt*>(vertexOutput.location) = newLocation;
                break;
            }
        }
    }

    return true;
}

}}}}

#endif
