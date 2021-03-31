#include "io.hpp"

#include <QFile>
#include <QDir>
#include <QTextStream>

#include "serializer.hpp"

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

    Settings loadSettings(const std::string &fileName) {
        try {
            QFile file(fileName.c_str());
            if (file.exists()) {
                file.open(QFile::ReadOnly);

                QTextStream stream(&file);
                QString settingsContents = stream.readAll();
                stream.flush();

                file.close();

                return Serializer::deserializeSettings(settingsContents.toStdString());
            } else {
                return {};
            }
        }
        catch (const std::exception &e) {
            std::string error = "Failed to read settings file at ";
            error += fileName;
            error += " Error: ";
            error += e.what();
            throw std::runtime_error(error);
        }
    }

    void saveSettings(const std::string &fileName, const Settings &settings) {
        try {
            QFile file(fileName.c_str());

            file.open(QFile::WriteOnly | QFile::Truncate);

            QTextStream stream(&file);
            QString str = Serializer::serializeSettings(settings).c_str();
            stream << str;
            stream.flush();

            file.close();
        }
        catch (const std::exception &e) {
            std::string error = "Failed to write settings file at ";
            error += fileName;
            error += " Error: ";
            error += e.what();
            throw std::runtime_error(error);
        }
    }


    SymbolTable loadSymbolTable(const std::string &filePath) {
        QFile file(filePath.c_str());
        if (!file.exists()) {
            std::string error = "File ";
            error += filePath;
            error += " not found.";
            throw std::runtime_error(error);
        }

        file.open(QFile::ReadOnly);

        QTextStream stream(&file);
        QString contents = stream.readAll();
        stream.flush();

        file.close();

        return Serializer::deserializeTable(contents.toStdString());
    }

    void saveSymbolTable(const std::string &filePath, const SymbolTable &symbolTable) {
        QFile file(filePath.c_str());
        QFileInfo info(file);

        //Failsafe, dont accept filepaths which point to an existing directory
        //as overwriting an existing directory with a file would not make sense in this case.
        if (info.isDir()) {
            std::string error = "File ";
            error += filePath;
            error += " is a directory.";
            throw std::runtime_error(error);
        }

        file.open(QFile::WriteOnly | QFile::Truncate);

        QTextStream stream(&file);
        QString contents = Serializer::serializeTable(symbolTable).c_str();
        stream << contents;
        stream.flush();

        file.close();
    }
}