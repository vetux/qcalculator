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

int convertDockAreaToInt(Qt::DockWidgetArea area) {
    switch (area) {
        case Qt::DockWidgetArea::LeftDockWidgetArea:
            return Qt::DockWidgetArea::LeftDockWidgetArea;
        case Qt::DockWidgetArea::TopDockWidgetArea:
            return Qt::DockWidgetArea::TopDockWidgetArea;
        case Qt::DockWidgetArea::RightDockWidgetArea:
            return Qt::DockWidgetArea::RightDockWidgetArea;
        case Qt::DockWidgetArea::BottomDockWidgetArea:
        default:
            return Qt::DockWidgetArea::BottomDockWidgetArea;
    }
}

Qt::DockWidgetArea convertIntToDockArea(int i) {
    switch (i) {
        case Qt::DockWidgetArea::LeftDockWidgetArea:
            return Qt::DockWidgetArea::LeftDockWidgetArea;
        case Qt::DockWidgetArea::TopDockWidgetArea:
            return Qt::DockWidgetArea::TopDockWidgetArea;
        case Qt::DockWidgetArea::RightDockWidgetArea:
            return Qt::DockWidgetArea::RightDockWidgetArea;
        case Qt::DockWidgetArea::BottomDockWidgetArea:
        default:
            return Qt::DockWidgetArea::BottomDockWidgetArea;
    }
}

int clampTabIndex(int index) {
    if (index > 4)
        return 4;
    else if (index < 0)
        return 0;
    else
        return index;
}

std::string Serializer::serializeSettings(const Settings &settings) {
    nlohmann::json j;
    j["showKeypad"] = settings.showKeypad;
    j["showBitView"] = settings.showBitView;
    j["showDock"] = settings.showDock;
    j["historyLimit"] = settings.historyLimit;
    j["dockPosition"] = convertDockAreaToInt(settings.dockPosition);
    j["dockTab"] = clampTabIndex(settings.dockActiveTab);
    j["windowWidth"] = settings.windowSize.width();
    j["windowHeight"] = settings.windowSize.height();
    return nlohmann::to_string(j);
}

Settings Serializer::deserializeSettings(const std::string &str) {
    nlohmann::json j = nlohmann::json::parse(str);
    Settings ret;
    ret.showKeypad = j["showKeypad"];
    ret.showBitView = j["showBitView"];
    ret.showDock = j["showDock"];
    ret.historyLimit = j["historyLimit"];
    ret.dockPosition = convertIntToDockArea(j["dockPosition"]);
    ret.dockActiveTab = clampTabIndex(j["dockTab"]);
    ret.windowSize = {j["windowWidth"], j["windowHeight"]};
    return ret;
}
