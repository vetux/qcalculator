#include "serializer.hpp"

#include "extern/json.hpp"

// When serializing symbol tables doubles are used for the mpreal types instead of the mpfr string routines
// to avoid possible security problems as the std double string routines should be more robust.
std::string Serializer::serializeTable(const SymbolTable &table) {
    nlohmann::json j;
    j["version"] = 0;

    std::vector<nlohmann::json> tmp;
    for (auto &p : table.getVariables()) {
        nlohmann::json t;
        t["name"] = p.first;
        t["value"] = p.second.toDouble();
        tmp.emplace_back(t);
    }
    j["variables"] = tmp;
    tmp.clear();

    for (auto &p : table.getConstants()) {
        nlohmann::json t;
        t["name"] = p.first;
        t["value"] = p.second.toDouble();
        tmp.emplace_back(t);
    }
    j["constants"] = tmp;
    tmp.clear();

    for (auto &p : table.getFunctions()) {
        nlohmann::json t;
        t["name"] = p.first;
        t["expression"] = p.second.expression;
        t["argumentNames"] = p.second.argumentNames;
        tmp.emplace_back(t);
    }
    j["functions"] = tmp;
    tmp.clear();

    return nlohmann::to_string(j);
}

SymbolTable Serializer::deserializeTable(const std::string &str) {
    nlohmann::json j = nlohmann::json::parse(str);
    SymbolTable ret;

    std::vector<nlohmann::json> tmp = j["variables"].get<std::vector<nlohmann::json>>();
    for (auto &v : tmp) {
        std::string name = v["name"];
        ArithmeticType value(v["value"].get<double>());
        ret.setVariable(name, 0);
    }

    tmp = j["constants"].get<std::vector<nlohmann::json>>();
    for (auto &v : tmp) {
        std::string name = v["name"];
        ArithmeticType value = v["value"].get<double>();
        ret.setConstant(name, 0);
    }

    tmp = j["functions"].get<std::vector<nlohmann::json>>();
    for (auto &v : tmp) {
        std::string name;
        name = v["name"];
        Function f;
        f.expression = v["expression"];
        f.argumentNames = v["argumentNames"].get<std::vector<std::string>>();
        ret.setFunction(name, f);
    }

    return ret;
}

std::string Serializer::serializeSettings(const Settings &settings) {
    auto &data = settings.entries();
    nlohmann::json j;
    for (auto &p : data) {
        switch (p.second.type()) {
            case Settings::NONE: //Ignore NONE type values.
                break;
            case Settings::INT:
                j[p.first] = p.second.toInt();
                break;
            case Settings::FLOAT:
                j[p.first] = p.second.toFloat();
                break;
            case Settings::STRING:
                j[p.first] = p.second.toString();
                break;
        }
    }
    return nlohmann::to_string(j);
}

Settings Serializer::deserializeSettings(const std::string &str) {
    Settings ret;
    nlohmann::json j = nlohmann::json::parse(str);
    for (auto &entry : j.items()) {
        const std::string &key = entry.key();
        const auto &value = entry.value();
        if (value.is_number_integer()) {
            ret.setValue(key, entry.value().get<int>());
        } else if (value.is_number_float()) {
            ret.setValue(key, entry.value().get<float>());
        } else if (value.is_string()) {
            ret.setValue(key, entry.value().get<std::string>());
        }
    }
    return ret;
}

std::string Serializer::serializeSet(const std::set<std::string> &set) {
    nlohmann::json j;
    j["data"] = set;
    return nlohmann::to_string(j);
}

std::set<std::string> Serializer::deserializeSet(const std::string &str) {
    nlohmann::json j = nlohmann::json::parse(str);
    return j["data"];
}

int Serializer::serializeRoundingMode(mpfr_rnd_t mode) {
    return (int) mode;
}

mpfr_rnd_t Serializer::deserializeRoundingMode(int mode) {
    switch (mode) {
        default:
        case (int) MPFR_RNDN:
            return MPFR_RNDN;
        case (int) MPFR_RNDZ:
            return MPFR_RNDZ;
        case (int) MPFR_RNDU:
            return MPFR_RNDU;
        case (int) MPFR_RNDD:
            return MPFR_RNDD;
        case (int) MPFR_RNDA:
            return MPFR_RNDA;
        case (int) MPFR_RNDF:
            return MPFR_RNDF;
    }
}
