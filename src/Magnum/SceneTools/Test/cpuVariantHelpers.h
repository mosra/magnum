#ifndef Magnum_SceneTools_Test_cpuVariantHelpers_h
#define Magnum_SceneTools_Test_cpuVariantHelpers_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
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

#include <Corrade/Cpu.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Utility/Assert.h>

namespace Magnum { namespace SceneTools { namespace Test {

template<class T, std::size_t size> constexpr std::size_t cpuVariantCount(T(&)[size]) {
    #ifdef MAGNUM_SCENETOOLS_FORCE_CPU_POINTER_DISPATCH
    return size;
    #else
    return 1;
    #endif
}

template<class T> inline Containers::String cpuVariantName(T& data) {
    Containers::String out;
    Utility::Debug{&out, Utility::Debug::Flag::NoNewlineAtTheEnd} << Utility::Debug::packed << data.features;
    return out;
}

template<class T, std::size_t size> inline const T& cpuVariantCompiled(const T(&data)[size]) {
    const Cpu::Features features =
        #ifdef MAGNUM_SCENETOOLS_FORCE_CPU_POINTER_DISPATCH
        Cpu::runtimeFeatures()
        #else
        Cpu::compiledFeatures()
        #endif
        ;
    for(std::size_t i = size; i != 0; --i)
        if(features >= data[i - 1].features)
            return data[i - 1];

    CORRADE_INTERNAL_ASSERT_UNREACHABLE();
}

template<class T> inline bool isCpuVariantSupported(T& data) {
    return Cpu::runtimeFeatures() >= data.features;
}

}}}

#endif
