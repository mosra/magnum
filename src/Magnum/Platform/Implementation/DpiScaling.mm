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

#import "Foundation/NSBundle.h"
#import "Foundation/NSProcessInfo.h"

#include "DpiScaling.h"

/* We can't build both DpiScaling.cpp and DpiScaling.mm as they both result in
   DpiScaling.o and Xcode/CMake gets confused, so including it here instead */
#include "DpiScaling.cpp"

namespace Magnum { namespace Platform { namespace Implementation {

/* UGH! */
bool isAppleBundleHiDpiEnabled() {
    /* Retrieve the NSHighResolutionCapable property from the bundle
       Info.plist. A non-nil bundle may be returned even for executables that
       aren't a bundle, so one can't use that to decide about anything. */
    id nsHighResolutionCapable = nil;
    NSBundle* bundle = [NSBundle mainBundle];
    if(bundle) nsHighResolutionCapable = [bundle objectForInfoDictionaryKey:@"NSHighResolutionCapable"];

    /* If the nsHighResolutionCapable entry is not found, it might mean it was
       either not specified in the Info.plist file or the executable is not a
       bundle. In that case, it'll be a default and that's where it gets ugly.
       Before 10.15, HiDPI is available only for bundles, so (if the executable
       is not a bundle or) the key is not specified, returning false. On 10.15,
       according to the global consensus (but nothing really confirmed by
       apple) in the following issues / changes

        https://github.com/simbody/simbody/issues/674
        https://github.com/redeclipse/base/issues/920
        https://github.com/UltraStar-Deluxe/USDX/issues/492
        https://bugzilla.libsdl.org/show_bug.cgi?id=4822
        https://hg.libsdl.org/SDL/rev/46b094f7d20e
        https://github.com/deepmind/lab/issues/173
        https://github.com/widelands/widelands/issues/3542

       ... it seems that HiDPI is the default. THANKS APPLE FOR CONFIRMING IT
       SO CLEARLY IN YOUR CHANGELOG AND DEVELOPER DOCUMENTATION! So returning
       true if the key is not specified and version is 10.15+.

       Of course, I have absolutely no idea how is it on iOS. So there it'll
       assume it's since iOS 13 (which has the ~same release date as macOS
       10.15) so I suppose both changes were done in tandem. In any case it's
       less of a problem because almost nobody looks at the logs anyway. Eh. */
    if(!nsHighResolutionCapable) {
        /* Based on https://stackoverflow.com/a/24004701. If someone is on 10.9
           and below, this will fail to compile. Sorry about that --- if that
           affects you, please open an issue. On the SO link there are
           alternatives that should work on older versions as well, but right
           now I'll assume everyone has 10.10 at least. For iOS this goes back
           to version 8, which should be more than okay. */
        NSOperatingSystemVersion version = [[NSProcessInfo processInfo] operatingSystemVersion];
        #ifndef CORRADE_TARGET_IOS
        return version.majorVersion*100 + version.minorVersion >= 1015;
        #else
        return version.majorVersion >= 13;
        #endif
    }

    /* Doing bool(nsHighResolutionCapable) would always return true, so that
       is not what we want. This works. ObjC, thank you, I thought it's
       impossible to have a language designed in a way that allows you to make
       a deadly mistake on each line and yet it all seemingly ""works"" with no
       compiler errors, runtime crashes or anything whatsoever. */
    NSNumber* soIsItOrIsItNot = nsHighResolutionCapable;
    return soIsItOrIsItNot.boolValue;
}

}}}
