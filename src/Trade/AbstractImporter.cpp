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

#include "AbstractImporter.h"

using namespace std;

namespace Magnum { namespace Trade {

bool AbstractImporter::open(const std::string&) {
    Error() << plugin() << "doesn't support opening files";
    return false;
}

bool AbstractImporter::open(std::istream&) {
    Error() << plugin() << "doesn't support opening input streams";
    return false;
}

}}
