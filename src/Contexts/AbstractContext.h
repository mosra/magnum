#ifndef Magnum_Contexts_AbstractContext_h
#define Magnum_Contexts_AbstractContext_h
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
 * @brief Class Magnum::Contexts::GlutContext
 */

namespace Magnum { namespace Contexts {

/**
@brief Base class for context creation

See subclasses documentation for more information. Context classes subclasses
are meant to be used directly in `main()`, for example:
@code
class MyContext: public Magnum::Contexts::GlutContext {
    // implement required methods...
};
int main(int argc, char** argv) {
    MyContext c(argc, argv);
    return c.exec();
}
@endcode
*/
class AbstractContext {
    public:
        virtual inline ~AbstractContext() {}

        /**
         * @brief Execute main loop
         * @return Value for returning from `main()`.
         */
        virtual int exec() = 0;
};

}}

#endif
