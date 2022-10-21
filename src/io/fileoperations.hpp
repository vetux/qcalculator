/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2022  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef QCALC_FILEOPERATIONS_HPP
#define QCALC_FILEOPERATIONS_HPP

#include <string>
#include <vector>

namespace FileOperations {
    /**
     * Find files in the passed directory with an optional suffix.
     *
     * @param directory The directory to search.
     * @param suffix If not empty only include file names which end with the suffix starting from the first dot.
     * @param recursive If true search recursively.
     * @return A list of absolute file paths.
     */
    std::vector<std::string> findFilesInDirectory(const std::string &directory, const std::string &suffix = "", bool recursive = false);

    std::string fileReadAll(const std::string &filePath);

    std::vector<char> fileReadAllVector(const std::string &filePath);

    void fileWriteAll(const std::string &filePath, const std::string &contents);
}

#endif //QCALC_FILEOPERATIONS_HPP
