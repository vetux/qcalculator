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

#include <stdexcept>

namespace FileOperations {
    std::vector<std::string> findFilesInDirectory(const std::string &directory,
                                                  const std::string &suffix,
                                                  bool recursive) {
        QDir dir(directory.c_str());

        std::vector<std::string> ret;

        QFileInfoList entries = dir.entryInfoList(QDir::NoFilter, QDir::Name);
        for (auto &entry : entries) {
            if (entry.isDir()) {
                if (recursive) {
                    std::vector<std::string> tmp = findFilesInDirectory(entry.absoluteFilePath().toStdString(),
                                                                        suffix,
                                                                        recursive);
                    for (auto &tmpPath : tmp) {
                        ret.emplace_back(tmpPath);
                    }
                }
            } else {
                if (suffix.empty() || entry.completeSuffix().toStdString() == suffix)
                    ret.emplace_back(entry.absoluteFilePath().toStdString());
            }
        }

        return ret;
    }

    std::string fileReadAllText(const std::string &filePath) {
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

    void fileWriteAllText(const std::string &filePath, const std::string &contents) {
        try {
            QFile file(filePath.c_str());

            file.open(QFile::WriteOnly | QFile::Truncate);

            QTextStream stream(&file);

            QString tmp(contents.c_str());
            stream << tmp;

            stream.flush();

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