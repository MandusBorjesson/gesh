#pragma once

#include "ISettingSource.h"
#include <string>
#include <map>
#include <vector>

class SettingHandler {
public:
    SettingHandler(ISettingSource &defaultSource,
                   std::vector<ISettingSource> &extraSources);

    void UpdateSettings(const std::map<std::string, int> newSettings);
    const std::map<std::string, ISettingSource::Setting> Settings() { return m_settings; };

private:
    std::map<std::string, ISettingSource::Setting> m_settings;
};
