#pragma once

#include "settingSource.h"
#include <vector>

class SettingReaderDefault : public ISettingReader {
public:
    SettingReaderDefault() : ISettingReader("default") {};
    std::map<std::string, std::optional<std::string>> GetSettings() override;
    void write(std::map<std::string, std::optional<std::string>> &settings) override {};
};

class SettingInitializerDefault : public ISettingInitializer {
public:
    SettingInitializerDefault();
    std::vector<Setting> InitializeSettings() override;
};
