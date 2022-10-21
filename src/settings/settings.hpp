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

#ifndef QCALC_SETTINGS_HPP
#define QCALC_SETTINGS_HPP

#include <string>
#include <map>
#include <set>

struct Setting;

class Settings {
public:
    static Settings readSettings();

    static void saveSettings(const Settings &settings);

    enum Type {
        NONE,
        INT,
        FLOAT,
        STRING,
        STRING_LIST
    };

    class Entry {
    public:
        Entry() : entryType(NONE) {};

        Entry(int value) : entryType(INT), intValue(value) {}

        Entry(float value) : entryType(FLOAT), floatValue(value) {}

        Entry(std::string value) : entryType(STRING), strValue(std::move(value)) {}

        Entry(std::set<std::string> value) : entryType(STRING_LIST), strListValue(std::move(value)) {}

        int toInt() const { return intValue; }

        float toFloat() const { return floatValue; }

        const std::string &toString() const { return strValue; }

        const std::set<std::string> &toStringList() const { return strListValue; }

        Type type() const { return entryType; }

    private:
        Type entryType;
        int intValue = 0;
        float floatValue = 0;
        std::string strValue;
        std::set<std::string> strListValue;
    };

    Settings() : data() {}

    explicit Settings(std::map<std::string, Entry> entries) : data(std::move(entries)) {}

    void update(const std::string &key, const Entry &value) {
        data[key] = value;
    }

    void clear(const std::string &key) {
        data.erase(key);
    }

    void clear(const Setting &s);

    const Entry &value(const std::string &key, const Entry &defaultValue) const {
        if (data.find(key) == data.end())
            return defaultValue;
        else
            return data.at(key);
    }

    const Entry &value(const Setting &setting) const;

    const std::map<std::string, Entry> &entries() const {
        return data;
    }

    const Entry &value(const std::string &key) const {
        return data.at(key);
    }

    bool check(const std::string &key) const {
        return data.find(key) != data.end();
    }

private:
    std::map<std::string, Entry> data;
};

struct Setting {
    std::string key;
    Settings::Entry entry;
};

#endif //QCALC_SETTINGS_HPP
