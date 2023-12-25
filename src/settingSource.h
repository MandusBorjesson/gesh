#pragma once

#include <variant>
#include <vector>
#include <string>
#include "settingRule.h"
#include "setting.h"

using setting_t = std::variant<std::monostate, bool, int, std::string>;

class Setting;

class ISettingSource {
public:
    ISettingSource(std::string name) : m_alias(name) {};
    const std::string Alias() { return m_alias; };

protected:
    std::string m_alias;
};

class ISettingInitializer : public ISettingSource {
public:
    ISettingInitializer(std::string name) : ISettingSource(name) {};
    virtual std::vector<Setting> InitializeSettings() = 0;
};

class ISettingReader : public ISettingSource {
public:
    ISettingReader(std::string name) : ISettingSource(name) {};
    virtual std::vector<std::pair<std::string, setting_t>> GetSettings() = 0;
};
