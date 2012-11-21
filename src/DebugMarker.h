#ifndef Magnum_DebugMarker_h
#define Magnum_DebugMarker_h
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
