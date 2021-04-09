#include "addonhelper.hpp"

#include "cpython/pythoninclude.hpp"

#include <stdexcept>

#include <QFileInfo>
#include <QDir>

#include "cpython/pyutil.hpp"
#include "io.hpp"
#include "extern/json.hpp"

namespace AddonHelper {
    AddonMetadata deserializeMetadata(const std::string &text) {
        nlohmann::json j = nlohmann::json::parse(text);
        AddonMetadata ret;
        ret.displayName = j["displayName"];
        ret.description = j["description"];
        return ret;
    }

    std::map<std::string, AddonMetadata> getAvailableAddons(const std::string &addonDirectory) {
        std::map<std::string, AddonMetadata> ret;

        std::vector<std::string> addonFiles = IO::findFilesInDirectory(addonDirectory, "py");
        for (auto &filePath : addonFiles) {
            QFileInfo addonFile(filePath.c_str());

            std::string moduleName = addonFile.baseName().toStdString();
            QFileInfo metadataFile(addonFile.absoluteDir().absolutePath()
                                           .append("/")
                                           .append(moduleName.c_str())
                                           .append(".json"));

            AddonMetadata metadata;
            if (metadataFile.exists()) {
                //Has metadata file
                try {
                    metadata = deserializeMetadata(IO::fileReadAllText(metadataFile.absoluteFilePath().toStdString()));
                }
                catch (const std::exception &e) {
                    //Ignore exception and set default metadata
                    metadata.displayName = moduleName;
                    metadata.description = "No Description";
                }
            } else {
                //No metadata file
                metadata.displayName = moduleName;
                metadata.description = "No Description";
            }

            ret[moduleName] = metadata;
        }

        return ret;
    }

    void load(const std::string &moduleName) {
        PyObject *mod = PyImport_ImportModule(moduleName.c_str());
        if (mod == PyNull) {
            throw std::runtime_error(PyUtil::getError());
        }

        PyObject *dict = PyModule_GetDict(mod);
        if (dict == PyNull) {
            throw std::runtime_error(PyUtil::getError());
        }

        PyObject *key = PyUnicode_FromString("load");

        PyObject *function = PyDict_GetItem(dict, key);

        if (function != PyNull) {
            PyObject *result = PyObject_CallNoArgs(function);
            if (result == PyNull) {
                Py_DECREF(key);
                Py_DECREF(mod);
                throw std::runtime_error(PyUtil::getError());
            }
            Py_DECREF(result);
        } else {
            Py_DECREF(key);
            Py_DECREF(mod);
            throw std::runtime_error("load function not found in module");
        }

        Py_DECREF(key);
        Py_DECREF(mod);
    }

    void unload(const std::string &moduleName) {
        PyObject *mod = PyImport_ImportModule(moduleName.c_str());
        if (mod == PyNull) {
            throw std::runtime_error(PyUtil::getError());
        }

        PyObject *dict = PyModule_GetDict(mod);
        if (dict == PyNull) {
            throw std::runtime_error(PyUtil::getError());
        }

        PyObject *key = PyUnicode_FromString("unload");

        PyObject *function = PyDict_GetItem(dict, key);

        if (function != PyNull) {
            PyObject *result = PyObject_CallNoArgs(function);
            if (result == PyNull) {
                Py_DECREF(key);
                Py_DECREF(mod);
                throw std::runtime_error(PyUtil::getError());
            }
            Py_DECREF(result);
        } else {
            Py_DECREF(key);
            Py_DECREF(mod);
            throw std::runtime_error("unload function not found in module");
        }

        Py_DECREF(key);
        Py_DECREF(mod);
    }
}