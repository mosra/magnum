/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2015 Jonathan Hale <squareys@googlemail.com>

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

#include <unordered_map>

#include <al.h>
#include <alc.h>

#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>
#include <Corrade/Utility/String.h>

#include "Magnum/Audio/Extensions.h"

namespace Magnum { namespace Audio {

const std::vector<Extension>& Extension::extensions() {
    #define _extension(prefix, vendor, extension) {Extensions::prefix::vendor::extension::Index, Extensions::prefix::vendor::extension::string()}
    static const std::vector<Extension> extensions{
        _extension(AL,EXT,FLOAT32),
        _extension(AL,EXT,DOUBLE),
        _extension(ALC,EXT,ENUMERATION),
        _extension(ALC,SOFTX,HRTF)
    };
    #undef _entension

    return extensions;
}

Context* Context::_current = nullptr;

Context::Context(): Context{Configuration{}} {}

Context::Context(const Configuration& config) {
    CORRADE_ASSERT(!_current, "Audio::Context: context already created", );

    /* Open default device */
    const ALCchar* const defaultDevice = alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
    _device = alcOpenDevice(defaultDevice);
    if(!_device) {
        Error() << "Audio::Context: cannot open sound device" << defaultDevice;
        std::exit(1);
    }

    if(!tryCreateContext(config)) {
        Error() << "Audio::Context: cannot create context:" << alcGetError(_device);
        std::exit(1);
    }

    /* Add all extensions to a map for faster lookup */
    std::unordered_map<std::string, Extension> extensionMap;
    for(const Extension& extension: Extension::extensions())
        extensionMap.emplace(extension._string, extension);

    /* Check for presence of extensions */
    const std::vector<std::string> extensions = extensionStrings();
    for(const std::string& extension: extensions) {
        const auto found = extensionMap.find(extension);
        if(found != extensionMap.end()) {
            _supportedExtensions.push_back(found->second);
            _extensionStatus.set(found->second._index);
        }
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

std::vector<std::string> Context::extensionStrings() const {
    std::vector<std::string> extensions;

    /* Don't crash when glGetString() returns nullptr */
    const char* e = reinterpret_cast<const char*>(alGetString(AL_EXTENSIONS));
    if(e) extensions = Utility::String::splitWithoutEmptyParts(e, ' ');

    return extensions;
}

bool Context::tryCreateContext(const Configuration& config) {
    /* The following parameters are order dependent!
       Make sure to always add sufficient space at end of the attributes
       array.*/
    Int attributes[]{
      ALC_FREQUENCY, config.frequency(),
      0, 0,
      0, 0,
      0, 0,
      0, 0,
      0 /* sentinel */
    };

    /* last valid index in the attributes array */
    int last = 1;

    if(config.isHrtfEnabled() != Configuration::EnabledState::Default) {
        attributes[++last] = ALC_HRTF_SOFT;
        attributes[++last] = (config.isHrtfEnabled() == Configuration::EnabledState::Enabled)
                             ? ALC_TRUE : ALC_FALSE;
    }

    if(config.monoSourcesCount() != -1) {
        attributes[++last] = ALC_MONO_SOURCES;
        attributes[++last] = config.monoSourcesCount();
    }

    if(config.stereoSourcesCount() != -1) {
        attributes[++last] = ALC_STEREO_SOURCES;
        attributes[++last] = config.stereoSourcesCount();
    }

    if(config.refreshRate() != -1) {
        attributes[++last] = ALC_REFRESH;
        attributes[++last] = config.refreshRate();
    }

    _context = alcCreateContext(_device, attributes);
    return !!_context;
}

}}
