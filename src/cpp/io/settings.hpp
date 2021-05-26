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

#ifndef QCALC_SETTINGS_HPP
#define QCALC_SETTINGS_HPP

#include <string>
#include <map>

class Settings {
public:
    enum Type {
        NONE,
        INT,
        FLOAT,
        STRING
    };

    class Entry {
    public:
        Entry() : entryType(NONE), intValue(0), floatValue(0), strValue() {};

        Entry(int value) : entryType(INT), intValue(value), floatValue(0), strValue() {}

        Entry(float value) : entryType(FLOAT), intValue(0), floatValue(value), strValue() {}

        Entry(std::string value) : entryType(STRING), intValue(0), floatValue(0), strValue(std::move(value)) {}

        int toInt() const { return intValue; }

        float toFloat() const { return floatValue; }

        std::string toString() const { return strValue; }

        Type type() const { return entryType; }

    private:
        Type entryType;
        int intValue;
        float floatValue;
        std::string strValue;
    };

    Settings() : data() {}

    explicit Settings(std::map<std::string, Entry> entries) : data(std::move(entries)) {}

    void setValue(const std::string &key, const Entry &value) {
        data[key] = value;
    }

    Entry value(const std::string &key, const Entry &defaultValue = 0) const {
        if (data.find(key) == data.end())
            return defaultValue;
        else
            return data.at(key);
    }

    const std::map<std::string, Entry> &entries() const {
        return data;
    }

private:
    std::map<std::string, Entry> data;
};

#endif //QCALC_SETTINGS_HPP
