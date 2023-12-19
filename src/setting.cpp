#include "setting.h"
#include <iostream>

SettingHandler::SettingHandler(ISettingSource &defaultSource,
                               std::vector<ISettingSource> &extraSources) {

    auto settings = defaultSource.GetSettings();

    for (auto setting : settings) {
        if (!setting.rule || setting.rule->verify(setting.value)) {
            m_settings[setting.name] = setting;
        } else {
            std::cout << "Rule check failed for " << setting.name << std::endl;
        }
    }
}

void SettingHandler::UpdateSettings(const std::map<std::string, int> newSettings) {
    return;
}
