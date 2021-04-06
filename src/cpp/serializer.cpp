#include "serializer.hpp"

#include "extern/json.hpp"

std::string Serializer::serializeTable(const SymbolTable &table) {
    nlohmann::json j;
    j["version"] = 0;

    std::vector<nlohmann::json> tmp;
    for (auto &p : table.getVariables()) {
        nlohmann::json t;
        t["name"] = p.first;
        t["value"] = p.second;
        tmp.emplace_back(t);
    }
    j["variables"] = tmp;
    tmp.clear();

    for (auto &p : table.getConstants()) {
        nlohmann::json t;
        t["name"] = p.first;
        t["value"] = p.second;
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
        ArithmeticType value = v["value"];
        ret.setVariable(name, value);
    }

    tmp = j["constants"].get<std::vector<nlohmann::json>>();
    for (auto &v : tmp) {
        std::string name = v["name"];
        ArithmeticType value = v["value"];
        ret.setConstant(name, value);
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
    throw std::runtime_error("not implemented");
}

Settings Serializer::deserializeSettings(const std::string &str) {
    throw std::runtime_error("not implemented");
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
