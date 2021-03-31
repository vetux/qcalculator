#ifndef QCALC_PLUGIN_HPP
#define QCALC_PLUGIN_HPP

#include <string>

struct _object;
typedef _object PyObject;

/**
 * A Plugin is a python module which defines lifecycle callbacks.
 *
 * All other interaction with the plugin is done through the native interface.
 *
 * A Plugin gains access to the ui, the symbol table etc through the native interface module (qci).
 */
class Plugin {
public:
    Plugin();

    Plugin(std::string moduleName);

    /**
     * Import the module and call the load function.
     */
    void load();

    /**
     * Import the module and call the unload function.
     */
    void unload();

private:
    std::string moduleName;
};

#endif //QCALC_PLUGIN_HPP
