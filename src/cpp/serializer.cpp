#include "serializer.hpp"

#include "extern/json.hpp"

std::string Serializer::serializeTable(const SymbolTable &table) {
    nlohmann::json j;
    j["version"] = 0;

    std::vector<nlohmann::json> tmp;
    for (auto &p : table.variables) {
        nlohmann::json t;
        t["name"] = p.name;
        t["value"] = p.value;
        tmp.emplace_back(t);
    }
    j["variables"] = tmp;
    tmp.clear();

    for (auto &p : table.constants) {
        nlohmann::json t;
        t["name"] = p.name;
        t["value"] = p.value;
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
        t["enableArguments"] = p.enableArguments;
        tmp.emplace_back(t);
    }
    j["scripts"] = tmp;
    tmp.clear();

    return nlohmann::to_string(j);
}

SymbolTable Serializer::deserializeTable(const std::string &str) {
    nlohmann::json j = nlohmann::json::parse(str);
    SymbolTable ret;

    std::vector<nlohmann::json> tmp = j["variables"].get<std::vector<nlohmann::json>>();
    for (auto &v : tmp) {
        std::string name = v["name"];
        ValueType value = v["value"];
        ret.variables.emplace_back(Variable(name, value));
    }

    tmp = j["constants"].get<std::vector<nlohmann::json>>();
    for (auto &v : tmp) {
        std::string name = v["name"];
        ValueType value = v["value"];
        ret.constants.emplace_back(Constant(name, value));
    }

    tmp = j["functions"].get<std::vector<nlohmann::json>>();
    for (auto &v : tmp) {
        Function f;
        f.name = v["name"];
        f.expression = v["expression"];
        f.argumentNames = v["argumentNames"].get<std::vector<std::string>>();
        ret.functions.emplace_back(f);
    }

    tmp = j["scripts"].get<std::vector<nlohmann::json>>();
    for (auto &v : tmp) {
        Script s;
        s.name = v["name"];
        s.body = v["body"];
        s.enableArguments = v["enableArguments"];
        ret.scripts.emplace_back(s);
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

std::string Serializer::serializeSettings(const State &state) {
    nlohmann::json j;
    j["showKeypad"] = state.showKeypad;
    j["showBitView"] = state.showBitView;
    j["showDock"] = state.showDock;
    j["historyLimit"] = state.historyLimit;
    j["dockPosition"] = convertDockAreaToInt(state.dockPosition);
    j["dockTab"] = clampTabIndex(state.dockSelectedTab);
    j["windowWidth"] = state.windowSize.width();
    j["windowHeight"] = state.windowSize.height();
    return nlohmann::to_string(j);
}

State Serializer::deserializeSettings(const std::string &str) {
    nlohmann::json j = nlohmann::json::parse(str);
    State ret;
    ret.showKeypad = j["showKeypad"];
    ret.showBitView = j["showBitView"];
    ret.showDock = j["showDock"];
    ret.historyLimit = j["historyLimit"];
    ret.dockPosition = convertIntToDockArea(j["dockPosition"]);
    ret.dockSelectedTab = clampTabIndex(j["dockTab"]);
    ret.windowSize = {j["windowWidth"], j["windowHeight"]};
    return ret;
}
