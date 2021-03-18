#ifndef QT_CALC_SERIALIZER_HPP
#define QT_CALC_SERIALIZER_HPP

#include <string>

#include "symboltable.hpp"

#include "extern/json.hpp"

class Serializer {
public:
    static std::string serializeTable(const SymbolTable &table) {
        nlohmann::json j;
        j["version"] = 0;

        std::vector<nlohmann::json> tmp;
        for (auto &p : table.variables) {
            nlohmann::json t;
            t["name"] = p.name;
            t["value"] = std::to_string(p.value);
            tmp.emplace_back(t);
        }
        j["variables"] = tmp;
        tmp.clear();

        for (auto &p : table.constants) {
            nlohmann::json t;
            t["name"] = p.name;
            t["value"] = std::to_string(p.value);
            tmp.emplace_back(t);
        }
        j["constants"] = tmp;
        tmp.clear();

        for (auto &p : table.functions) {
            nlohmann::json t;
            t["name"] = p.name;
            t["expression"] = p.expression;
            t["argumentNames"] = p.argumentNames;
            tmp.emplace_back(t);
        }
        j["functions"] = tmp;
        tmp.clear();

        for (auto &p : table.scripts) {
            nlohmann::json t;
            t["name"] = p.name;
            t["body"] = p.body;
            t["argCount"] = std::to_string(p.argCount);
            tmp.emplace_back(t);
        }
        j["scripts"] = tmp;
        tmp.clear();

        return nlohmann::to_string(j);
    }

    static SymbolTable deserializeTable(const std::string &str) {
        nlohmann::json j = nlohmann::json::parse(str);
        SymbolTable ret;

        std::vector<nlohmann::json> tmp = j["variables"].get<std::vector<nlohmann::json>>();
        for (auto &v : tmp) {
            std::string name = v["name"].get<std::string>();
            ValueType value = std::stold(v["value"].get<std::string>());
            ret.variables.emplace_back(Variable(name, value));
        }

        tmp = j["constants"].get<std::vector<nlohmann::json>>();
        for (auto &v : tmp) {
            std::string name = v["name"].get<std::string>();
            ValueType value = std::stold(v["value"].get<std::string>());
            ret.constants.emplace_back(Constant(name, value));
        }

        tmp = j["functions"].get<std::vector<nlohmann::json>>();
        for (auto &v : tmp) {
            Function f;
            f.name = v["name"].get<std::string>();
            f.expression = v["expression"].get<std::string>();
            f.argumentNames = v["argumentNames"].get<std::vector<std::string>>();
            ret.functions.emplace_back(f);
        }

        tmp = j["scripts"].get<std::vector<nlohmann::json>>();
        for (auto &v : tmp) {
            Script s;
            s.name = v["name"].get<std::string>();
            s.body = v["body"].get<std::string>();
            s.argCount = std::stoul(v["argCount"].get<std::string>());
            ret.scripts.emplace_back(s);
        }

        return ret;
    }
};

#endif //QT_CALC_SERIALIZER_HPP
