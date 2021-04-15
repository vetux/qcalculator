# QCalculator

This project aims to provide an easy to use and extensible calculator application.

# Native

The native part is the C++ code which hosts a CPython instance and Qt application instance. It provides a minimal
functional GUI, implements the model of the application and makes this functionality available to addons through the
CPython embedding API.

Currently implemented is:

- ExprTk module : Allows addons to evaluate expressions including symbol tables.
- MpReal module : Allows addons to instantiate and use mpfr objects.

# Addon

The addon part is the python modules which can be loaded and unloaded at runtime. Addons can interact with the Qt GUI
through PySide.

# Dependencies

## Internal

These dependencies are bundled with the repository in the src/cpp/extern directory.

- ExprTk - https://github.com/ArashPartow/exprtk
- MpReal - https://github.com/advanpix/mpreal
- Nlohmann Json - https://github.com/nlohmann/json

## External

These dependencies have to be provided by the platform when building.

* MPFR - https://www.mpfr.org/
* CPython - https://www.github.com/python/cpython
    * PySide2 downloaded through pip
    
# Building

For information on how to build the application see: [BUILDING.md](BUILDING.md)

# Screenshots

![img](.docs/screenshot.png)