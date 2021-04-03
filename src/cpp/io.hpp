#ifndef QCALC_IO_HPP
#define QCALC_IO_HPP

#include <string>

#include "settings.hpp"

#include "calc/symboltable.hpp"

namespace IO {
    /**
     * Find files in the passed directory with an optional suffix.
     *
     * @param directory The directory to search.
     * @param suffix If not empty only include file names which end with the suffix starting from the first dot.
     * @param recursive If true search recursively.
     * @return A list of absolute file paths.
     */
    std::vector<std::string> findFilesInDirectory(const std::string &directory, const std::string &suffix = "", bool recursive = false);

    std::string fileReadAllText(const std::string &filePath);

    void fileWriteAllText(const std::string &filePath, const std::string &contents);
}

#endif //QCALC_IO_HPP
