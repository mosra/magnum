/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "DebugMarker.h"

#include <Utility/Debug.h>

#include "Context.h"
#include "Extensions.h"

namespace Magnum {

DebugMarker::MarkImplementation DebugMarker::markImplementation = &DebugMarker::markImplementationDefault;

void DebugMarker::initializeContextBasedFunctionality(Context* context) {
    if(context->isExtensionSupported<Extensions::GL::GREMEDY::string_marker>()) {
        Debug() << "DebugMarker: using" << Extensions::GL::GREMEDY::string_marker::string() << "features";

        markImplementation = &DebugMarker::markImplementationDebugger;
    }
}

void DebugMarker::markImplementationDefault(const std::string&) {}

void DebugMarker::markImplementationDebugger(const std::string& string) {
    /** @todo Re-enable when extension wrangler is available for ES */
    #ifndef MAGNUM_TARGET_GLES
    glStringMarkerGREMEDY(string.length(), string.c_str());
    #else
    #if 0
    glInsertEventMarkerEXT(string.length(), string.c_str());
    #endif
    #endif
}

}
