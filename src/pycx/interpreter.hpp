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

#ifndef QCALC_INTERPRETER_HPP
#define QCALC_INTERPRETER_HPP

#include <vector>
#include <string>
#include <set>
#include <functional>

namespace Interpreter {
    enum ParseStyle {
        SINGLE_INPUT,
        FILE_INPUT,
        EVAL_INPUT,
        FUNC_TYPE_INPUT
    };

    void initialize();

    void finalize();

    char getPathSeparator();

    void setPath(const std::wstring &path);

    void setPaths(const std::set<std::string> &path);

    std::wstring getPath();

    void setModuleDirs(const std::vector<std::string> &moduleDirectories);

    std::vector<std::string> getModuleDirs();

    void addModuleDir(const std::string &dir);

    void removeModuleDir(const std::string &dir);

    int runInteractiveLoop();

    void runString(const std::string &expression,
                   ParseStyle style = SINGLE_INPUT,
                   const std::string &context = "__main__");

    void callFunctionNoArgs(const std::string &module, const std::string &function);

    void reloadModule(const std::string &module);

    void setStdStreams(std::function<void(const std::string &)> outCallback,
                       std::function<void(const std::string &)> errCallback);

    void clearStdStreams();

    std::string getError();

    std::string getVersion();

    std::string getCopyright();

    std::string getCompiler();

    std::string getBuildInfo();
}

#endif //QCALC_INTERPRETER_HPP
