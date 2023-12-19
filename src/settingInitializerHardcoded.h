#pragma once

#include "settingInitializer.h"
#include <vector>

class SettingInitializerHardcoded : public ISettingInitializer {
public:
    SettingInitializerHardcoded() : ISettingInitializer("DEFAULT") {};
    std::vector<Setting> GetSettings() override;
};
