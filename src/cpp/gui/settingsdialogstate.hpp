#ifndef QCALC_SETTINGSDIALOGSTATE_HPP
#define QCALC_SETTINGSDIALOGSTATE_HPP

#include <map>
#include <string>

#include "addonmetadata.hpp"

struct SettingsDialogState {
    std::map<std::string, bool> addonState;
    std::map<std::string, AddonMetadata> addonMetadata;
};

#endif //QCALC_SETTINGSDIALOGSTATE_HPP
