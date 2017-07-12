Bug reports, feature requests or code contributions are always very welcome.
To make things easier, here are a few tips:

Reporting bugs, requesting features
-----------------------------------

*   Best way to report bugs and request new features is to use GitHub
    [issues](https://github.com/mosra/magnum/issues), but you can contact me
    also any other way.

Code contribution
-----------------

*   Building and installing Magnum is described in the [documentation](http://doc.magnum.graphics/magnum/building.html).
*   Follow the project coding guidelines. In short -- try to match style of the
    surrounding code and avoid any trailing whitespace. When in doubt, consult
    coding guidelines, which are available also [online](http://doc.magnum.graphics/magnum/coding-style.html).
*   Document your code. When updating or adding new API, make sure that Doxygen
    documentation is up to date. Run

        doxygen

    in project root to generate the documentation and check that your
    modifications didn't add any warnings.
*   Build unit tests (`-DBUILD_TESTS=ON` parameter to CMake) and run them
    using

        ctest --output-on-failure

    in build directory. All tests should *always* pass. Add new tests or modify
    the existing to make sure new code is properly covered (if possible). Here
    is a [short tutorial](http://doc.magnum.graphics/corrade/unit-testing.html)
    to help you with creating unit tests.
*   Best way to contribute is by using GitHub [pull requests](https://github.com/mosra/magnum/pulls)
    -- fork the repository and make pull request from feature branch. You can
    also send patches via e-mail or contact me any other way.
*   All your code will be released under license of the project (see [COPYING](COPYING)
    file for details), so make sure you and your collaborators (or employers)
    have no problems with it. If you create new files, don't forget to add
    license header (verbatim copied from other files) and don't forget to add
    yourself to license header of files you added or significantly modified,
    for example:

        /*
            This file is part of Magnum.

            Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
                      Vladimír Vondruš <mosra@centrum.cz>
            Copyright © YEAR YOUR_NAME <your@mail.com>

            Permission is hereby granted, free of charge, to any person obtaining a
            ...

Contact
-------

*   Website -- http://magnum.graphics
*   GitHub -- https://github.com/mosra/magnum
*   Gitter -- https://gitter.im/mosra/magnum
*   IRC -- join `#magnum-engine` channel on freenode
*   Google Groups -- https://groups.google.com/forum/#!forum/magnum-engine
*   Twitter -- https://twitter.com/czmosra
*   E-mail -- mosra@centrum.cz
*   Jabber -- mosra@jabbim.cz
