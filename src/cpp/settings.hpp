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
