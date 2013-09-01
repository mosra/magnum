#ifndef Magnum_Audio_Context_h
#define Magnum_Audio_Context_h
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

/** @file Audio/Context.h
 * @brief Class Magnum::Audio::Context
 */

#include <al.h>
#include <string>

#include "Audio/magnumAudioVisibility.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;
#endif

namespace Magnum { namespace Audio {

/**
@brief OpenAL context
 */
class MAGNUM_AUDIO_EXPORT Context {
    public:
        /** @brief Current context */
        static Context* current() { return _current; }

        /**
         * @brief Constructor
         *
         * Creates OpenAL context.
         */
        explicit Context();

        /**
         * @brief Destructor
         *
         * Destroys OpenAL context.
         */
        ~Context();

        /**
         * @brief Vendor string
         *
         * @see rendererString(), @fn_al{GetString} with @def_al{VENDOR}
         */
        std::string vendorString() const { return alGetString(AL_VENDOR); }

        /**
         * @brief %Renderer string
         *
         * @see vendorString(), @fn_al{GetString} with @def_al{RENDERER}
         */
        std::string rendererString() const { return alGetString(AL_RENDERER); }

        /**
         * @brief Version string
         *
         * @see @fn_al{GetString} with @def_al{VERSION}
         */
        std::string versionString() const { return alGetString(AL_VERSION); }

    private:
        static Context* _current;

        ALCdevice* _device;
        ALCcontext* _context;
};

}}

#endif
