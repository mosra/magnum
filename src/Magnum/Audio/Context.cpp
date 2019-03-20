/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/String.h>

#include "Magnum/Audio/Extensions.h"

namespace Magnum { namespace Audio {

namespace {

constexpr Extension ExtensionList[]{
    #define _extension(prefix, vendor, extension) {Extensions::prefix::vendor::extension::Index, Extensions::prefix::vendor::extension::string()}
    _extension(AL,EXT,FLOAT32),
    _extension(AL,EXT,DOUBLE),
    _extension(AL,EXT,ALAW),
    _extension(AL,EXT,MULAW),
    _extension(AL,EXT,MCFORMATS),
    _extension(ALC,EXT,ENUMERATION),
    _extension(ALC,SOFTX,HRTF),
    _extension(ALC,SOFT,HRTF)
    #undef _entension
};

}

Containers::ArrayView<const Extension> Extension::extensions() {
    /* GCC 4.8 needs an explicit cast */
    return Containers::arrayView(ExtensionList);
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

#ifndef DOXYGEN_GENERATING_OUTPUT
Context::Context(): Context{Configuration{}} {}
#endif

Context::Context(const Configuration& configuration) {
    create(configuration);
}

Context::Context(NoCreateT) noexcept: _device{}, _context{} {}

void Context::create(const Configuration& configuration) {
    if(!tryCreate(configuration)) std::exit(1);
}

bool Context::tryCreate(const Configuration& configuration) {
    CORRADE_ASSERT(!_current, "Audio::Context: context already created", false);

    /* Open the device */
    const ALCchar* const deviceSpecifier = configuration.deviceSpecifier().empty() ? alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER) : configuration.deviceSpecifier().data();
    if(!(_device = alcOpenDevice(deviceSpecifier))) {
        Error() << "Audio::Context: cannot open sound device" << deviceSpecifier;
        return false;
    }

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

    if(configuration.frequency() != -1) {
        attributes[last++] = ALC_FREQUENCY;
        attributes[last++] = configuration.frequency();
    }
    if(configuration.hrtf() != Configuration::Hrtf::Default) {
        attributes[last++] = ALC_HRTF_SOFT;
        attributes[last++] = (configuration.hrtf() == Configuration::Hrtf::Enabled)
                             ? ALC_TRUE : ALC_FALSE;
    }
    if(configuration.monoSourceCount() != -1) {
        attributes[last++] = ALC_MONO_SOURCES;
        attributes[last++] = configuration.monoSourceCount();
    }
    if(configuration.stereoSourceCount() != -1) {
        attributes[last++] = ALC_STEREO_SOURCES;
        attributes[last++] = configuration.stereoSourceCount();
    }
    if(configuration.refreshRate() != -1) {
        attributes[last++] = ALC_REFRESH;
        attributes[last++] = configuration.refreshRate();
    }

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    _context = alcCreateContext(_device, attributes);
    #else
    if(last != 0)
        Warning() << "Audio::Context::tryCreateContext(): specifying attributes is not supported with Emscripten, ignoring";
    _context = alcCreateContext(_device, nullptr);
    #endif
    if(!_context) {
        Error() << "Audio::Context: cannot create context:" << alcErrorString(alcGetError(_device));
        return false;
    }

    alcMakeContextCurrent(_context);
    _current = this;

    /* Add all extensions to a map for faster lookup */
    std::unordered_map<std::string, Extension> extensionMap;
    for(const Extension& extension: Extension::extensions())
        extensionMap.emplace(extension.string(), extension);

    /* Check for presence of extensions */
    const std::vector<std::string> extensions = extensionStrings();
    for(const std::string& extension: extensions) {
        const auto found = extensionMap.find(extension);
        if(found != extensionMap.end()) {
            _supportedExtensions.push_back(found->second);
            _extensionStatus.set(found->second.index());
        }
    }

    /* Print some info */
    Debug() << "Audio Renderer:" << rendererString() << "by" << vendorString();
    Debug() << "OpenAL version:" << versionString();

    return true;
}

Context::Context(Context&& other) noexcept: _device{other._device}, _context{other._context}, _extensionStatus{std::move(other._extensionStatus)}, _supportedExtensions{std::move(other._supportedExtensions)} {
    other._device = nullptr;
    other._context = nullptr;
    if(_current == &other) _current = this;
}

Context::~Context() {
    if(_context) alcDestroyContext(_context);
    if(_device) alcCloseDevice(_device);
    if(_current == this) _current = nullptr;
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

bool Context::isHrtfEnabled() const {
    Int enabled;
    alcGetIntegerv(_device, ALC_HRTF_SOFT, 1, &enabled);
    return enabled == ALC_TRUE;
}

Context::HrtfStatus Context::hrtfStatus() const {
    if(!isExtensionSupported<Extensions::ALC::SOFT::HRTF>())
        return isHrtfEnabled() ? HrtfStatus::Enabled : HrtfStatus::Disabled;

    Int status;
    alcGetIntegerv(_device, ALC_HRTF_STATUS_SOFT, 1, &status);
    return Context::HrtfStatus(status);
}

std::string Context::hrtfSpecifierString() const {
    /* Returns a string on ALC_SOFT_HRTF, nullptr on ALC_SOFTX_HRTF */
    return Utility::String::fromArray(alcGetString(_device, ALC_HRTF_SPECIFIER_SOFT));
}

std::string Context::deviceSpecifierString() const {
    return alcGetString(_device, ALC_DEVICE_SPECIFIER);
}

std::string Context::vendorString() const {
    return alGetString(AL_VENDOR);
}

std::string Context::rendererString() const {
    return alGetString(AL_RENDERER);
}

std::string Context::versionString() const {
    return alGetString(AL_VERSION);
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
