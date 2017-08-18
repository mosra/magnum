/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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
#include <cstring>

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
        _extension(AL,EXT,ALAW),
        _extension(AL,EXT,MULAW),
        _extension(AL,EXT,MCFORMATS),
        _extension(ALC,EXT,ENUMERATION),
        _extension(ALC,SOFTX,HRTF),
        _extension(ALC,SOFT,HRTF)
    };
    #undef _entension

    return extensions;
}

Debug& operator<<(Debug& debug, const Context::HrtfStatus value) {
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Context::HrtfStatus::value: return debug << "Audio::Context::HrtfStatus::" #value;
        _c(Disabled)
        _c(Enabled)
        _c(Denied)
        _c(Required)
        _c(Detected)
        _c(UnsupportedFormat)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "Audio::Context::HrtfStatus(" << Debug::nospace << reinterpret_cast<void*>(ALenum(value)) << Debug::nospace << ")";
}

namespace {

const char* alcErrorString(const ALenum error) {
    switch(error) {
        /* LCOV_EXCL_START */
        #define _c(value) case value: return #value;
        _c(ALC_NO_ERROR)
        _c(ALC_INVALID_DEVICE)
        _c(ALC_INVALID_CONTEXT)
        _c(ALC_INVALID_ENUM)
        _c(ALC_INVALID_VALUE)
        _c(ALC_OUT_OF_MEMORY)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return "ALC_(invalid)";
}

}

Context* Context::_current = nullptr;

std::vector<std::string> Context::deviceSpecifierStrings() {
    std::vector<std::string> list;
    const char* const devices = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);
    for(const char* device = devices; *device; device += std::strlen(device) + 1)
        list.emplace_back(device);

    return list;
}

bool Context::hasCurrent() { return _current; }

Context& Context::current() {
    CORRADE_ASSERT(_current, "Audio::Context::current(): no current context", *_current);
    return *_current;
}

Context::Context(): Context{Configuration{}} {}

Context::Context(const Configuration& config) {
    CORRADE_ASSERT(!_current, "Audio::Context: context already created", );

    /* Open the device */
    const ALCchar* const deviceSpecifier = config.deviceSpecifier().empty() ? alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER) : config.deviceSpecifier().data();
    if(!(_device = alcOpenDevice(deviceSpecifier))) {
        Error() << "Audio::Context: cannot open sound device" << deviceSpecifier;
        std::exit(1);
    }

    if(!tryCreateContext(config)) {
        Error() << "Audio::Context: cannot create context:" << alcErrorString(alcGetError(_device));
        std::exit(1);
    }

    alcMakeContextCurrent(_context);
    _current = this;

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

    /* Don't crash when alGetString() returns nullptr */
    const char* alExts = reinterpret_cast<const char*>(alGetString(AL_EXTENSIONS));
    if(alExts) extensions = Utility::String::splitWithoutEmptyParts(alExts, ' ');

    /* Add ALC extensions aswell */
    const char* alcExts = reinterpret_cast<const char*>(alcGetString(_device, ALC_EXTENSIONS));
    if(alcExts) {
        auto splitAlcExts = Utility::String::splitWithoutEmptyParts(alcExts, ' ');
        extensions.insert(extensions.end(), splitAlcExts.begin(), splitAlcExts.end());
    }

    return extensions;
}

bool Context::tryCreateContext(const Configuration& config) {
    /* The following parameters are order dependent!
       Make sure to always add sufficient space at end of the attributes
       array.*/
    Int attributes[]{
      0, 0,
      0, 0,
      0, 0,
      0, 0,
      0, 0,
      0 /* sentinel */
    };

    /* last valid index in the attributes array */
    Int last = 0;

    if(config.frequency() != -1) {
        attributes[last++] = ALC_FREQUENCY;
        attributes[last++] = config.frequency();
    }
    if(config.hrtf() != Configuration::Hrtf::Default) {
        attributes[last++] = ALC_HRTF_SOFT;
        attributes[last++] = (config.hrtf() == Configuration::Hrtf::Enabled)
                             ? ALC_TRUE : ALC_FALSE;
    }
    if(config.monoSourceCount() != -1) {
        attributes[last++] = ALC_MONO_SOURCES;
        attributes[last++] = config.monoSourceCount();
    }
    if(config.stereoSourceCount() != -1) {
        attributes[last++] = ALC_STEREO_SOURCES;
        attributes[last++] = config.stereoSourceCount();
    }
    if(config.refreshRate() != -1) {
        attributes[last++] = ALC_REFRESH;
        attributes[last++] = config.refreshRate();
    }

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    _context = alcCreateContext(_device, attributes);
    #else
    if(last != 0)
        Warning() << "Audio::Context::tryCreateContext(): specifying attributes is not supported with Emscripten, ignoring";
    _context = alcCreateContext(_device, nullptr);
    #endif
    return !!_context;
}

Context::Configuration::Configuration() = default;
Context::Configuration::~Configuration() = default;

Context::Configuration& Context::Configuration::setDeviceSpecifier(const std::string& specifier) {
    _deviceSpecifier = specifier;
    return *this;
}

Context::Configuration& Context::Configuration::setDeviceSpecifier(std::string&& specifier) {
    _deviceSpecifier = std::move(specifier);
    return *this;
}

}}
