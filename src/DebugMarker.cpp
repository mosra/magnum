/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include "DebugMarker.h"

#include <Utility/Debug.h>

#include "Context.h"
#include "Extensions.h"

namespace Magnum {

DebugMarker::MarkImplementation DebugMarker::markImplementation = &DebugMarker::markImplementationDefault;

void DebugMarker::initializeContextBasedFunctionality(Context& context) {
    /** @todo Re-enable when extension wrangler is available for ES */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::GREMEDY::string_marker>()) {
        Debug() << "DebugMarker: using" << Extensions::GL::GREMEDY::string_marker::string() << "features";

        markImplementation = &DebugMarker::markImplementationDebugger;
    }
    #else
    static_cast<void>(context);
    #endif
}

void DebugMarker::markImplementationDefault(const std::string&) {}

void DebugMarker::markImplementationDebugger(const std::string& string) {
    /** @todo Re-enable when extension wrangler is available for ES */
    #ifndef MAGNUM_TARGET_GLES
    glStringMarkerGREMEDY(string.length(), string.data());
    #else
    #if 0
    glInsertEventMarkerEXT(string.length(), string.data());
    #else
    static_cast<void>(string);
    #endif
    #endif
}

}
