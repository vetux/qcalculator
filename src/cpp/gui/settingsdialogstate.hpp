#ifndef QCALC_SETTINGSDIALOGSTATE_HPP
#define QCALC_SETTINGSDIALOGSTATE_HPP

#include <map>
#include <string>

#include "addonmetadata.hpp"
#include "settings.hpp"

struct SettingsDialogState {
    std::map<std::string, AddonMetadata> addonMetadata;

    Settings settings;
};

#endif //QCALC_SETTINGSDIALOGSTATE_HPP
