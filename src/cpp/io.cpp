#include "io.hpp"

#include <QFile>
#include <QDir>
#include <QTextStream>

namespace IO {
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