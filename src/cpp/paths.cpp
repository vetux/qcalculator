#include "paths.hpp"

#include <QStandardPaths>
#include <QCoreApplication>

std::string Paths::getAppDataDirectory() {
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
}

std::string Paths::getAddonDirectory() {
    return QCoreApplication::applicationDirPath().append("/addon").toStdString();
}

std::string Paths::getSystemDirectory() {
    return QCoreApplication::applicationDirPath().append("/system").toStdString();
}
