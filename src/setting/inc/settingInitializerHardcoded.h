#pragma once

#include "settingSource.h"
#include <vector>

class SettingInitializerHardcoded : public ISettingInitializer {
public:
    SettingInitializerHardcoded();
    std::vector<Setting> InitializeSettings() override;
};
