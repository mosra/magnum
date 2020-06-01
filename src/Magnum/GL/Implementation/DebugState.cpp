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

#include "DebugState.h"

#include "Magnum/GL/AbstractObject.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"

namespace Magnum { namespace GL { namespace Implementation {

DebugState::DebugState(Context& context, std::vector<std::string>& extensions):
    maxLabelLength{0},
    maxLoggedMessages{0},
    maxMessageLength{0},
    maxStackDepth{0}
{
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::KHR::debug>())
    #else
    if(context.isVersionSupported(Version::GLES320))
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        extensions.emplace_back(Extensions::KHR::debug::string());
        #endif

        getLabelImplementation = &AbstractObject::getLabelImplementationKhrDesktopES32;
        labelImplementation = &AbstractObject::labelImplementationKhrDesktopES32;
        controlImplementation = &DebugOutput::controlImplementationKhrDesktopES32;
        callbackImplementation = &DebugOutput::callbackImplementationKhrDesktopES32;
        messageInsertImplementation = &DebugMessage::insertImplementationKhrDesktopES32;
        pushGroupImplementation = &DebugGroup::pushImplementationKhrDesktopES32;
        popGroupImplementation = &DebugGroup::popImplementationKhrDesktopES32;

    } else
    #endif
    #ifdef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::KHR::debug>()) {
        extensions.emplace_back(Extensions::KHR::debug::string());

        getLabelImplementation = &AbstractObject::getLabelImplementationKhrES;
        labelImplementation = &AbstractObject::labelImplementationKhrES;
        controlImplementation = &DebugOutput::controlImplementationKhrES;
        callbackImplementation = &DebugOutput::callbackImplementationKhrES;
        messageInsertImplementation = &DebugMessage::insertImplementationKhrES;
        pushGroupImplementation = &DebugGroup::pushImplementationKhrES;
        popGroupImplementation = &DebugGroup::popImplementationKhrES;

    } else
    #endif
    {
        if(context.isExtensionSupported<Extensions::EXT::debug_label>()) {
            extensions.emplace_back(Extensions::EXT::debug_label::string());

            getLabelImplementation = &AbstractObject::getLabelImplementationExt;
            labelImplementation = &AbstractObject::labelImplementationExt;
        } else {
            getLabelImplementation = &AbstractObject::getLabelImplementationNoOp;
            labelImplementation = &AbstractObject::labelImplementationNoOp;
        }

        if(context.isExtensionSupported<Extensions::EXT::debug_marker>()) {
            extensions.emplace_back(Extensions::EXT::debug_marker::string());

            pushGroupImplementation = &DebugGroup::pushImplementationExt;
            popGroupImplementation = &DebugGroup::popImplementationExt;
            messageInsertImplementation = &DebugMessage::insertImplementationExt;
        #ifndef MAGNUM_TARGET_GLES
        } else if(context.isExtensionSupported<Extensions::GREMEDY::string_marker>()) {
            extensions.emplace_back(Extensions::GREMEDY::string_marker::string());

            pushGroupImplementation = &DebugGroup::pushImplementationNoOp;
            popGroupImplementation = &DebugGroup::popImplementationNoOp;
            messageInsertImplementation = &DebugMessage::insertImplementationGremedy;
        #endif
        } else {
            pushGroupImplementation = &DebugGroup::pushImplementationNoOp;
            popGroupImplementation = &DebugGroup::popImplementationNoOp;
            messageInsertImplementation = &DebugMessage::insertImplementationNoOp;
        }

        controlImplementation = &DebugOutput::controlImplementationNoOp;
        callbackImplementation = &DebugOutput::callbackImplementationNoOp;
    }
}

}}}
