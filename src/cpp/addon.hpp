#ifndef QCALC_ADDON_HPP
#define QCALC_ADDON_HPP

#include <string>
#include <vector>

namespace Addon {
    /**
     * Import the module and call the load function.
     *
     * @param moduleName
     */
    void load(const std::string &moduleName);

    /**
     * Import the module and call the unload function.
     *
     * @param moduleName
     */
    void unload(const std::string &moduleName);
}

#endif //QCALC_ADDON_HPP
