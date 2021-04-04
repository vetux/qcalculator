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
    nlohmann::json j;
    j["showKeypad"] = settings.showKeypad;
    j["showBitView"] = settings.showBitView;
    j["historyLimit"] = settings.historyLimit;
    j["activeTab"] = settings.activeTab;
    j["windowWidth"] = settings.windowSize.width();
    j["windowHeight"] = settings.windowSize.height();
    j["addons"] = settings.enabledAddonModules;
    j["settingsTab"] = settings.settingsTab;
    j["showAddonWarning"] = settings.showAddonWarning;
    return nlohmann::to_string(j);
}

Settings Serializer::deserializeSettings(const std::string &str) {
    nlohmann::json j = nlohmann::json::parse(str);

    Settings ret;

    if (j.contains("showKeypad"))
        ret.showKeypad = j["showKeypad"];

    if (j.contains("showBitView"))
        ret.showBitView = j["showBitView"];

    if (j.contains("historyLimit"))
        ret.historyLimit = j["historyLimit"];

    if (j.contains("activeTab"))
        ret.activeTab = j["activeTab"];

    if (j.contains("windowWidth") && j.contains("windowHeight"))
        ret.windowSize = {j["windowWidth"], j["windowHeight"]};

    if (j.contains("addons"))
        ret.enabledAddonModules = j["addons"].get<std::set<std::string>>();

    if (j.contains("settingsTab"))
        ret.settingsTab = j["settingsTab"];

    if (j.contains("showAddonWarning"))
        ret.showAddonWarning = j["showAddonWarning"];

    return ret;
}
