#ifndef QCALC_ADDONHELPER_HPP
#define QCALC_ADDONHELPER_HPP

#include <string>
#include <map>

#include "addonmetadata.hpp"

namespace AddonHelper {
    /**
     * Find all python modules (non recursive) in the specified addon directory,
     * look for a corresponding modulename.json file, if the json file exists extract
     * the metadata otherwise set default metadata and return a map containing
     * the metadata objects and the module names as keys.
     *
     * @param addonDirectory The directory to search for addon module and metadata files.
     * @return The map of available addon metadata objects identified by the module name as key.
     */
    std::map<std::string, AddonMetadata> getAvailableAddons(const std::string &addonDirectory);

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

#endif //QCALC_ADDONHELPER_HPP
