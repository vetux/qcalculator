/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2021  Julian Zampiccoli
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

#include "fileoperations.hpp"

#include <QFile>
#include <QDir>
#include <QTextStream>

#include <filesystem>

#include <stdexcept>

namespace FileOperations {
    std::vector<std::string> findFilesInDirectory(const std::string &directory,
                                                  const std::string &suffix,
                                                  bool recursive) {
        std::vector<std::string> ret;

        for (auto &entry: std::filesystem::directory_iterator(directory)) {
            auto p = entry.path().string();
            if (entry.is_directory()) {
                if (recursive) {
                    std::vector<std::string> tmp = findFilesInDirectory(std::filesystem::absolute(p).string(),
                                                                        suffix,
                                                                        recursive);
                    for (auto &tmpPath: tmp) {
                        ret.emplace_back(tmpPath);
                    }
                }
            } else {
                if (suffix.empty() || entry.path().extension().string() == suffix)
                    ret.emplace_back(std::filesystem::absolute(p).string());
            }
        }

        return ret;
    }

    std::string fileReadAll(const std::string &filePath) {
        try {
            QFile file(filePath.c_str());
            if (file.exists()) {
                file.open(QFile::ReadOnly);

                QTextStream stream(&file);
                QString fileContents = stream.readAll();
                stream.flush();

                file.close();

                return fileContents.toStdString();
            } else {
                throw std::runtime_error("File not found.");
            }
        }
        catch (const std::exception &e) {
            std::string error = "Failed to read file at ";
            error += filePath;
            error += " Error: ";
            error += e.what();
            throw std::runtime_error(error);
        }
    }

    std::vector<char> fileReadAllVector(const std::string &filePath) {
        auto ret = fileReadAll(filePath);
        std::vector<char> rv;
        for (auto &v: ret)
            rv.emplace_back(v);
        return rv;
    }

    void fileWriteAll(const std::string &filePath, const std::string &contents) {
        try {
            QFile file(filePath.c_str());

            file.open(QFile::WriteOnly | QFile::Truncate);

            QTextStream stream(&file);

            QString tmp(contents.c_str());
            stream << tmp;

            stream.flush();

            file.flush();

            file.close();
        }
        catch (const std::exception &e) {
            std::string error = "Failed to write file at ";
            error += filePath;
            error += " Error: ";
            error += e.what();
            throw std::runtime_error(error);
        }
    }
}