#include "paths.hpp"

#include <QString>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDir>


std::string Paths::getAppDataDirectory() {
    QString dirPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    if (!QDir(dirPath).exists())
        QDir().mkpath(dirPath);

    return dirPath.toStdString();
}

std::string Paths::getAddonDirectory() {
    return QCoreApplication::applicationDirPath().append("/addon").toStdString();
}

std::string Paths::getSystemDirectory() {
    return QCoreApplication::applicationDirPath().append("/system").toStdString();
}
