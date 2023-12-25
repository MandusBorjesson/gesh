#pragma once

#include "settingSource.h"
#include <vector>

class SettingInitializerHardcoded : public ISettingInitializer {
public:
    SettingInitializerHardcoded() : ISettingInitializer("DEFAULT") {};
    std::vector<Setting> InitializeSettings() override;
};
