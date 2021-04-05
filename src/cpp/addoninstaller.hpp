#ifndef QCALC_ADDONINSTALLER_HPP
#define QCALC_ADDONINSTALLER_HPP

#include <string>

namespace AddonInstaller {
    /**
     * This routine attempts to install an addon by reading some source file and dropping the
     * appropriate files in the addons directory.
     *
     * @param sourceFile
     * @param addonsDirectory
     */
    void installAddon(std::string &sourceFile, const std::string &addonsDirectory);
}

#endif //QCALC_ADDONINSTALLER_HPP
