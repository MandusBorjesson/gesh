#include "setting.h"

SettingHandler::SettingHandler(ISettingSource &defaultSource,
                               std::vector<ISettingSource> &extraSources) {

    auto settings = defaultSource.GetSettings();

    for (auto setting : settings) {
        m_settings[setting.name] = setting;
    }
}

void SettingHandler::UpdateSettings(const std::map<std::string, int> newSettings) {
    return;
}
