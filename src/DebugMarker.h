#ifndef Magnum_DebugMarker_h
#define Magnum_DebugMarker_h
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

/** @file
 * @brief Class Magnum::DebugMarker
 */

#include <string>

#include "Magnum.h"

#include "magnumVisibility.h"

namespace Magnum {

/**
@brief Debug marker

Allows putting debug marker into OpenGL command stream for use with various
debuggers, such as ApiTrace or gDEBugger.
@requires_extension %Extension @extension{GREMEDY,string_marker}. If not
    available, this class does nothing.
@requires_es_extension %Extension @es_extension{EXT,debug_marker}. If not
    available, this class does nothing.
*/
class MAGNUM_EXPORT DebugMarker {
    friend class Context;

    public:
        DebugMarker() = delete;

        /** @brief Put string mark into OpenGL command stream */
        inline static void mark(const std::string& string) {
            markImplementation(string);
        }

    private:
        static void MAGNUM_LOCAL initializeContextBasedFunctionality(Context* context);

        typedef void(*MarkImplementation)(const std::string&);
        static MAGNUM_LOCAL void markImplementationDefault(const std::string& string);
        static MAGNUM_LOCAL void markImplementationDebugger(const std::string& string);
        static MarkImplementation markImplementation;
};

}

#endif
