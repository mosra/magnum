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

#include "QueryState.h"

#include "Magnum/GL/AbstractQuery.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"

namespace Magnum { namespace GL { namespace Implementation {

QueryState::QueryState(Context& context, std::vector<std::string>& extensions) {
    /* Create implementation */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::ARB::direct_state_access>()) {
        #ifdef CORRADE_TARGET_WINDOWS
        if((context.detectedDriver() & Context::DetectedDriver::IntelWindows) && !context.isDriverWorkaroundDisabled("intel-windows-broken-dsa-indexed-queries")) {
            createImplementation = &AbstractQuery::createImplementationDefault;
        } else if((context.detectedDriver() & Context::DetectedDriver::Amd) && !context.isDriverWorkaroundDisabled("amd-windows-dsa-createquery-except-xfb-overflow")) {
            extensions.emplace_back(Extensions::ARB::direct_state_access::string());
            createImplementation = &AbstractQuery::createImplementationDSAExceptXfbOverflow;
        } else
        #endif
        if((context.detectedDriver() & Context::DetectedDriver::Mesa) && !context.isDriverWorkaroundDisabled("mesa-dsa-createquery-except-pipeline-stats")) {
            extensions.emplace_back(Extensions::ARB::direct_state_access::string());
            createImplementation = &AbstractQuery::createImplementationDSAExceptPipelineStats;
        } else {
            extensions.emplace_back(Extensions::ARB::direct_state_access::string());
            createImplementation = &AbstractQuery::createImplementationDSA;
        }
    } else
    #endif
    {
        createImplementation = &AbstractQuery::createImplementationDefault;
    }

    #ifdef MAGNUM_TARGET_GLES
    static_cast<void>(context);
    static_cast<void>(extensions);
    #endif
}

}}}
