#ifndef QCALC_PLUGIN_HPP
#define QCALC_PLUGIN_HPP

#include <string>

/**
 * A Plugin is a python module which defines lifecycle callbacks.
 *
 * All other interaction with the plugin is done through the native interface.
 *
 * A Plugin gains access to the ui, the symbol table etc through the native interface module (qci).
 */
class Plugin {
public:
    /**
     * Import the module and call the load function.
     */
    static void load(const std::string &moduleName);

    /**
     * Import the module and call the unload function.
     */
    static void unload(const std::string &moduleName);
};

#endif //QCALC_PLUGIN_HPP
