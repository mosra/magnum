#ifndef Magnum_Platform_AbstractApplication_h
#define Magnum_Platform_AbstractApplication_h
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
 * @brief Class Magnum::Platform::AbstractApplication
 */

namespace Magnum { namespace Platform {

/**
@brief Base class for applications

See subclasses documentation for more information. Subclasses are meant to be
used directly in `main()`, for example:
@code
class MyContext: public Magnum::Platform::GlutApplication {
    // implement required methods...
};
int main(int argc, char** argv) {
    MyContext c(argc, argv);
    return c.exec();
}
@endcode
*/
class AbstractApplication {
    public:
        virtual inline ~AbstractApplication() {}

        /**
         * @brief Execute main loop
         * @return Value for returning from `main()`.
         */
        virtual int exec() = 0;
};

}}

#endif
