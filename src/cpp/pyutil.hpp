#ifndef QCALC_PYUTIL_HPP
#define QCALC_PYUTIL_HPP

#include <string>
#include <vector>

#define PyNull NULL

namespace PyUtil {
    void initializePython();

    void finalizePython();

    std::vector<std::string> getModuleDirectories();

    void setModuleDirectories(std::vector<std::string> dirs);

    void addModuleDirectory(const std::string &directory);

    std::string getError();
}

#endif //QCALC_PYUTIL_HPP
