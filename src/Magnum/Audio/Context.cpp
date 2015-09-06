/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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

#include "Context.h"

#include <alc.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/Magnum.h"

namespace Magnum { namespace Audio {

Context* Context::_current = nullptr;

Context::Context() {
    CORRADE_ASSERT(!_current, "Audio::Context: context already created", );

    /* Open default device */
    const ALCchar* const defaultDevice = alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
    _device = alcOpenDevice(defaultDevice);
    if(!_device) {
        Error() << "Audio::Context: cannot open sound device" << defaultDevice;
        std::exit(1);
    }

    _context = alcCreateContext(_device, nullptr);
    if(!_context) {
        Error() << "Audio::Context: cannot create context:" << alcGetError(_device);
        std::exit(1);
    }

    alcMakeContextCurrent(_context);
    _current = this;

    /* Print some info */
    Debug() << "Audio Renderer:" << rendererString() << "by" << vendorString();
    Debug() << "OpenAL version:" << versionString();
}

Context::~Context() {
    CORRADE_INTERNAL_ASSERT(_current == this);

    alcDestroyContext(_context);
    alcCloseDevice(_device);
}

}}
