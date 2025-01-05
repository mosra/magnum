/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include "ContextState.h"

#include <Corrade/Containers/StringView.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"

namespace Magnum { namespace GL { namespace Implementation {

using namespace Containers::Literals;

ContextState::ContextState(Context& context, const Containers::StaticArrayView<Implementation::ExtensionCount, const char*> extensions) {
    #ifndef MAGNUM_TARGET_GLES
    if((context.detectedDriver() & Context::DetectedDriver::NVidia) &&
        !context.isDriverWorkaroundDisabled("nv-zero-context-profile-mask"_s))
    {
        isCoreProfileImplementation = &Context::isCoreProfileImplementationNV;
    } else isCoreProfileImplementation = &Context::isCoreProfileImplementationDefault;
    #endif

    #ifdef MAGNUM_TARGET_WEBGL
    /* The rendererStringUnmasked() and vendorStringUnmasked() branch on the
       extension on their own, which is in-line with all other "limit" queries.
       It wouldn't make sense to create four new *Implementation()
       functions and two new pointers for something that gets called mostly
       just on application startup (and where it can't actually use the
       function pointer because at that point the state is still yet to be
       created).

       But since there's nothing else would add the extension to the used
       extension list, we're doing that here. */
    if(context.isExtensionSupported<Extensions::WEBGL::debug_renderer_info>()) {
        extensions[Extensions::WEBGL::debug_renderer_info::Index] =
                   Extensions::WEBGL::debug_renderer_info::string();
    }
    #endif

    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    static_cast<void>(context);
    #endif
    #ifndef MAGNUM_TARGET_WEBGL
    static_cast<void>(extensions);
    #endif
}

}}}
