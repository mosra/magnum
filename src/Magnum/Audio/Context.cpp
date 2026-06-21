/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2015 Jonathan Hale <squareys@googlemail.com>
    Copyright © 2019 Guillaume Jacquemin <williamjcm@users.noreply.github.com>

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

#include <algorithm> /* std::lower_bound() */
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/Audio/Extensions.h"

#if defined(CORRADE_TARGET_WINDOWS) && defined(MAGNUM_BUILD_STATIC_UNIQUE_GLOBALS) && !defined(CORRADE_TARGET_WINDOWS_RT)
#include "Magnum/Implementation/WindowsWeakSymbol.h"
#endif

namespace Magnum { namespace Audio {

namespace {

/* Binary search is performed on the extensions, thus they have to be sorted
   alphabetically. */
constexpr Extension ExtensionList[]{
    #define _extension(prefix, vendor, extension) {Extensions::prefix::vendor::extension::Index, Extensions::prefix::vendor::extension::string()}
    _extension(ALC,EXT,ENUMERATION),
    _extension(ALC,SOFTX,HRTF),
    _extension(ALC,SOFT,HRTF),
    _extension(AL,EXT,ALAW),
    _extension(AL,EXT,DOUBLE),
    _extension(AL,EXT,FLOAT32),
    _extension(AL,EXT,MCFORMATS),
    _extension(AL,EXT,MULAW),
    _extension(AL,SOFT,loop_points),
    #undef _entension
};

const Extension* findExtension(const Containers::StringView extension) {
    const auto found = std::lower_bound(std::begin(ExtensionList), std::end(ExtensionList), extension, [](const Extension& a, const Containers::StringView& b) {
        return a.string() < b;
    });
    if(found != std::end(ExtensionList) && found->string() == extension)
        return found;

    return {};
}

}

Containers::ArrayView<const Extension> Extension::extensions() {
    /* GCC 4.8 needs an explicit cast */
    return Containers::arrayView(ExtensionList);
}

Debug& operator<<(Debug& debug, const Context::HrtfStatus value) {
    debug << "Audio::Context::HrtfStatus" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Context::HrtfStatus::value: return debug << "::" #value;
        _c(Disabled)
        _c(Enabled)
        _c(Denied)
        _c(Required)
        _c(Detected)
        _c(UnsupportedFormat)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << Debug::hex << ALenum(value) << Debug::nospace << ")";
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

Containers::Array<Containers::StringView> Context::deviceSpecifierStrings() {
    Containers::Array<Containers::StringView> list;
    const char* device = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);
    while(*device) {
        arrayAppend(list, {device, Containers::StringViewFlag::Global});
        device += list.back().size() + 1;
    }

    return list;
}

#if !defined(MAGNUM_BUILD_STATIC_UNIQUE_GLOBALS) || defined(CORRADE_TARGET_WINDOWS)
/* (Of course) can't be in an unnamed namespace in order to export it below
   (except for Windows, where we do extern "C" so this doesn't matter) */
namespace {
#endif

/* Unlike GL, this isn't thread-local. Would need to implement
   ALC_EXT_thread_local_context first */
/* What the hell is going on here with the #ifdefs?! */
#if !defined(MAGNUM_BUILD_STATIC) || !defined(MAGNUM_BUILD_STATIC_UNIQUE_GLOBALS) || (defined(MAGNUM_BUILD_STATIC_UNIQUE_GLOBALS) && !defined(CORRADE_TARGET_WINDOWS))
#ifdef MAGNUM_BUILD_STATIC_UNIQUE_GLOBALS
/* On static builds that get linked to multiple shared libraries and then used
   in a single app we want to ensure there's just one global symbol. On Linux
   it's apparently enough to just export, macOS needs the weak attribute.
   Windows not handled yet, as it needs a workaround using DllMain() and
   GetProcAddress(). */
CORRADE_VISIBILITY_EXPORT
    #ifdef CORRADE_TARGET_GCC
    __attribute__((weak))
    #else
    /* uh oh? the test will fail, probably */
    #endif
#endif
Context* currentContext = nullptr;
#else
/* On Windows the symbol is exported unmangled and then fetched via
   GetProcAddress() to emulate weak linking. Using an extern "C" block instead
   of just a function annotation because otherwise MinGW prints a warning:
   '...' initialized and declared 'extern' (uh?) */
extern "C" {
    CORRADE_VISIBILITY_EXPORT Context* magnumAudioUniqueCurrentContext = nullptr;
}
#endif

#if !defined(MAGNUM_BUILD_STATIC_UNIQUE_GLOBALS) || defined(CORRADE_TARGET_WINDOWS)
}
#endif

/* Windows don't have any concept of weak symbols, instead GetProcAddress() on
   GetModuleHandle(nullptr) "emulates" the weak linking as it's guaranteed to
   pick up the same symbol of the final exe independently of the DLL it was
   called from. To avoid #ifdef hell in code below, the currentContext is
   redefined to return a value from this uniqueness-ensuring function. */
#if defined(CORRADE_TARGET_WINDOWS) && defined(MAGNUM_BUILD_STATIC_UNIQUE_GLOBALS) && !defined(CORRADE_TARGET_WINDOWS_RT)
namespace {

Context*& windowsCurrentContext() {
    /* A function-local static to ensure it's only initialized once without any
       race conditions among threads */
    static Context** const uniqueGlobals = reinterpret_cast<Context**>(Magnum::Implementation::windowsWeakSymbol("magnumAudioUniqueCurrentContext", &magnumAudioUniqueCurrentContext));
    return *uniqueGlobals;
}

}

#define currentContext windowsCurrentContext()
#endif

bool Context::hasCurrent() { return currentContext; }

Context& Context::current() {
    CORRADE_ASSERT(currentContext, "Audio::Context::current(): no current context", *currentContext);
    return *currentContext;
}

Context::Context(const Int argc, const char* const* const argv): Context(Configuration{}, argc, argv) {}

Context::Context(NoCreateT, const Int argc, const char* const* const argv) noexcept: _device{}, _context{} {
    Utility::Arguments args{"magnum",
        Utility::Arguments::Flag::IgnoreUnknownOptions};
    args.addOption("log", "default").setHelp("log", "console logging", "default|quiet|verbose")
        .addOption("disable-extensions").setHelp("disable-extensions", "API extensions to disable", "LIST")
        .setFromEnvironment("log")
        .parse(argc, argv);

    /* Decide how to display initialization log */
    _displayInitializationLog = !(args.value("log") == "quiet" || args.value("log") == "QUIET");

    /* Disable extensions. Here we search for them among the known extensions
       and store the Extension objects instead, which avoids the string copying
       and another binary search in tryCreate(). */
    const Containers::StringView disabledExtensions = args.value<Containers::StringView>("disable-extensions");
    if(!disabledExtensions.isEmpty()) {
        const Containers::Array<Containers::StringView> split = disabledExtensions.splitOnWhitespaceWithoutEmptyParts();
        arrayReserve(_disabledExtensions, split.size());
        for(const Containers::StringView extension: split)
            if(const Extension* found = findExtension(extension))
                arrayAppend(_disabledExtensions, *found);
    }
}

void Context::create(const Configuration& configuration) {
    if(!tryCreate(configuration))
        std::exit(1);
}

bool Context::tryCreate(const Configuration& configuration) {
    CORRADE_ASSERT(!currentContext, "Audio::Context: context already created", false);

    /* Open the device */
    const ALCchar* const deviceSpecifier = configuration.deviceSpecifier().isEmpty() ? alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER) : configuration.deviceSpecifier().data();
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
    currentContext = this;

    /* Check for presence of extensions */
    const Containers::Array<Containers::StringView> extensions = extensionStrings();
    for(const Containers::StringView extension: extensions) {
        if(const Extension* found = findExtension(extension)) {
            #ifdef MAGNUM_BUILD_DEPRECATED
            arrayAppend(_supportedExtensions, *found);
            #endif
            _extensionStatus.set(found->index(), true);
        }
    }

    std::ostream* output = _displayInitializationLog ? Debug::output() : nullptr;

    /* Print some info */
    Debug{output} << "Audio renderer:" << rendererString() << "by" << vendorString();
    Debug{output} << "OpenAL version:" << versionString();

    /* Disable extensions as requested by the user */
    if(!_disabledExtensions.isEmpty()) {
        Debug{output} << "Disabling extensions:";

        /* Disable extensions that are known and supported and print a message
           for each */
        for(const Extension& extension: _disabledExtensions) {
            _extensionStatus.set(extension.index(), false);
            _extensionDisabledStatus.set(extension.index(), true);
            Debug{output} << "   " << extension.string();
        }
    }

    return true;
}

Context::Context(Context&& other) noexcept: _device{other._device}, _context{other._context}, _extensionStatus{Utility::move(other._extensionStatus)}
    #ifdef MAGNUM_BUILD_DEPRECATED
    , _supportedExtensions{Utility::move(other._supportedExtensions)}
    #endif
{
    other._device = nullptr;
    other._context = nullptr;
    if(currentContext == &other)
        currentContext = this;
}

Context::~Context() {
    if(_context)
        alcDestroyContext(_context);
    if(_device)
        alcCloseDevice(_device);
    if(currentContext == this)
        currentContext = nullptr;
}

Containers::Array<Containers::StringView> Context::extensionStrings() const {
    Containers::Array<Containers::StringView> extensions = Containers::StringView{reinterpret_cast<const char*>(alGetString(AL_EXTENSIONS)), Containers::StringViewFlag::Global}.splitWithoutEmptyParts(' ');

    /* Add ALC extensions as well. This reallocates the above array but that's
       fine, it's not a hot code path. */
    arrayAppend(extensions, Containers::StringView{reinterpret_cast<const char*>(alcGetString(_device, ALC_EXTENSIONS)), Containers::StringViewFlag::Global}.splitWithoutEmptyParts(' '));

    return extensions;
}

Int Context::frequency() const {
    Int count;
    alcGetIntegerv(_device, ALC_FREQUENCY, 1, &count);
    return count;
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

Containers::StringView Context::hrtfSpecifierString() const {
    /* Returns a string on ALC_SOFT_HRTF, nullptr on ALC_SOFTX_HRTF */
    return {alcGetString(_device, ALC_HRTF_SPECIFIER_SOFT), Containers::StringViewFlag::Global};
}

Int Context::monoSourceCount() const {
    Int count;
    alcGetIntegerv(_device, ALC_MONO_SOURCES, 1, &count);
    return count;
}

Int Context::stereoSourceCount() const {
    Int count;
    alcGetIntegerv(_device, ALC_STEREO_SOURCES, 1, &count);
    return count;
}

Int Context::refreshRate() const {
    Int count;
    alcGetIntegerv(_device, ALC_REFRESH, 1, &count);
    return count;
}

Containers::StringView Context::deviceSpecifierString() const {
    return {alcGetString(_device, ALC_DEVICE_SPECIFIER), Containers::StringViewFlag::Global};
}

Containers::StringView Context::vendorString() const {
    return {alGetString(AL_VENDOR), Containers::StringViewFlag::Global};
}

Containers::StringView Context::rendererString() const {
    return {alGetString(AL_RENDERER), Containers::StringViewFlag::Global};
}

Containers::StringView Context::versionString() const {
    return {alGetString(AL_VERSION), Containers::StringViewFlag::Global};
}

}}
