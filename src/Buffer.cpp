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

#include "Buffer.h"

#include <Utility/Debug.h>

#include "Context.h"
#include "Extensions.h"

namespace Magnum {

Buffer::CopyImplementation Buffer::copyImplementation = &Buffer::copyImplementationDefault;

void Buffer::initializeContextBasedFunctionality(Context* context) {
    if(context->isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
        Debug() << "Buffer: using" << Extensions::GL::EXT::direct_state_access::string() << "features";

        copyImplementation = &Buffer::copyImplementationDSA;
    }
}

void Buffer::copyImplementationDefault(Buffer* read, Buffer* write, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size) {
    read->bind(Target::CopyRead);
    write->bind(Target::CopyWrite);
    glCopyBufferSubData(static_cast<GLenum>(Target::CopyRead), static_cast<GLenum>(Target::CopyWrite), readOffset, writeOffset, size);
}

void Buffer::copyImplementationDSA(Buffer* read, Buffer* write, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size) {
    glNamedCopyBufferSubDataEXT(read->_id, write->_id, readOffset, writeOffset, size);
}

}
