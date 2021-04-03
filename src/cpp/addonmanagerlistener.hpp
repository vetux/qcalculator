#ifndef QCALC_ADDONMANAGERLISTENER_HPP
#define QCALC_ADDONMANAGERLISTENER_HPP

class AddonManagerListener {
public:
    virtual void onAddonLoadFail(const std::string &moduleName, const std::string &error) = 0;

    virtual void onAddonUnloadFail(const std::string &moduleName, const std::string &error) = 0;
};

#endif //QCALC_ADDONMANAGERLISTENER_HPP
