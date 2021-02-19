#ifndef Magnum_ShaderTools_Implementation_spirv_h
#define Magnum_ShaderTools_Implementation_spirv_h
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

#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Reference.h>
#include <Corrade/Containers/StringView.h>

#include "Magnum/Magnum.h"
#include "MagnumExternal/Vulkan/spirv.h"

namespace Magnum { namespace ShaderTools { namespace Implementation { namespace {

/* This is used by both magnum-shaderconverter and the Vk library for
   SwiftShader workarounds but we don't want the Vk library to depend on
   ShaderTools, so the minimal needed subset is made header-only.

   Eventually this should be turned into a public API, but so far it's just a
   bag of random functions with very specific usage patterns and isn't clear
   yet how to expose a usable interface. Moreover, the SwiftShader patching
   needs to mutate the original, which means the outputs are pointers to the
   original data. */

/* If the code looks like a valid SPIR-V, returns everything after the
   header. If not, nullptr. */
Containers::ArrayView<const UnsignedInt> spirvData(const void* code, UnsignedInt size) {
    const UnsignedInt* const spirv = static_cast<const UnsignedInt*>(code);
    /* Not >= 5*4 because just the header alone is useless also */
    return size % 4 == 0 && size > 5*4 && spirv[0] == SpvMagicNumber ?
        Containers::ArrayView<const UnsignedInt>{spirv, size/4}.suffix(5) : nullptr;
}

/* When an instruction is found, the `data` is advanced after it in order to
   allow calling this function in a loop. When not found, `data` is left
   untouched. */
Containers::ArrayView<const UnsignedInt> spirvFindInstruction(Containers::ArrayView<const UnsignedInt>& data, const SpvOp op) {
    /* Copy the view and iterate that. If we find the instruction, update the
       passed `data` reference, if not, keep it as it was -- that way, if the
       find fails, `data` won't become empty and can be used further */
    for(Containers::ArrayView<const UnsignedInt> dataIteration = data; !dataIteration.empty(); ) {
        const UnsignedInt instructionSize = dataIteration[0] >> 16;
        const UnsignedInt instructionOp = dataIteration[0] & 0xffff;

        /* Corrupted SPIR-V */
        /** @todo print a message here? */
        if(dataIteration.size() < instructionSize) {
            data = dataIteration;
            return nullptr;
        }

        /* This is the instruction we're looking for, return it and update the
           view to point after it. */
        if(instructionOp == op) {
            data = dataIteration.suffix(instructionSize);
            return dataIteration.prefix(instructionSize);
        }

        /* Otherwise advance the view for next round */
        dataIteration = dataIteration.suffix(instructionSize);
    }

    /* Nothing found. Leave the input data as-is. */
    return nullptr;
}

struct SpirvEntrypoint {
    Containers::Reference<const SpvExecutionModel> executionModel;
    Containers::StringView name;
    Containers::ArrayView<const UnsignedInt> interfaces;
};

/* When an entrypoint is found, the `data` is advanced after the instruction in
   order to allow calling this function in a loop. When not found, `data` is
   left untouched. Most of other SPIR-V code is meant to appear after the
   entrypoints, so it's fine to feed the resulting `data` to
   spirvEntrypointInterface() and others. */
Containers::Optional<SpirvEntrypoint> spirvNextEntrypoint(Containers::ArrayView<const UnsignedInt>& data) {
    while(const Containers::ArrayView<const UnsignedInt> entryPoint = spirvFindInstruction(data, SpvOpEntryPoint)) {
        /* Expecting at least op, execution model, ID, name. If less, it's an
           invalid SPIR-V. */
        /** @todo print a message here? */
        if(entryPoint.size() < 4) return {};

        /* Find where the name ends and interface IDs start. According to the
           spec, a string literal is null-terminated and all bytes after are
           zeros as well, so it should be enough to check that the last byte is
           zero. */
        Containers::ArrayView<const UnsignedInt> interfaces;
        for(std::size_t i = 3; i != entryPoint.size(); ++i) {
            if(entryPoint[i] >> 24 == 0) {
                interfaces = entryPoint.suffix(i + 1);
                break;
            }
        }

        return SpirvEntrypoint{
            *reinterpret_cast<const SpvExecutionModel*>(entryPoint + 1),
            reinterpret_cast<const char*>(entryPoint + 3),
            interfaces
        };
    }

    return {};
}

struct SpirvEntrypointInterface {
    /* If null, the interface might be for example builtin */
    const UnsignedInt* location;
    /* If null, the SPIR-V is probably invalid */
    const SpvStorageClass* storageClass;
};

/* Unlike above, `data` isn't modified by this function -- because the
   decoration and variable instructions are likely intermixed for different
   entrypoint, it makes sense to restart the search from the beginning for each
   entrypoint.

   The `out` array is expected to have the same size as entrypoint.interfaces
   and be zero-initialized (so the not found data stay null). */
void spirvEntrypointInterface(Containers::ArrayView<const UnsignedInt> data, const SpirvEntrypoint& entrypoint, Containers::ArrayView<SpirvEntrypointInterface> out) {
    CORRADE_INTERNAL_ASSERT(out.size() == entrypoint.interfaces.size());

    /* Find location decorations */
    while(const Containers::ArrayView<const UnsignedInt> decoration = spirvFindInstruction(data, SpvOpDecorate)) {
        /* Expecting at least op, ID, SpvDecorationLocation, location. The
           instruction can be three words, so if we get less than 4 it's not an
           error. */
        if(decoration.size() < 4 || decoration[2] != SpvDecorationLocation)
            continue;

        for(std::size_t i = 0; i != entrypoint.interfaces.size(); ++i) {
            if(decoration[1] == entrypoint.interfaces[i]) {
                out[i].location = decoration + 3;
                break;
            }
        }
    }

    /* Find storage classes. According to the spec, OpVariable is meant to
       appear after OpDecorate, so we don't need to restart from the
       beginning. */
    while(const Containers::ArrayView<const UnsignedInt> variable = spirvFindInstruction(data, SpvOpVariable)) {
        /* Expecting at least op, result, ID, SpvStorageClass. If less, it's an
           invalid SPIR-V. */
        /** @todo print a message here? */
        if(variable.size() < 4) return;

        for(std::size_t i = 0; i != entrypoint.interfaces.size(); ++i) {
            if(variable[2] == entrypoint.interfaces[i]) {
                out[i].storageClass = reinterpret_cast<const SpvStorageClass*>(variable + 3);
                break;
            }
        }
    }
}

}}}}

#endif
