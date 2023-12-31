#pragma once

#include <variant>
#include <vector>
#include <map>
#include <string>
#include "settingRule.h"
#include "settingTypes.h"
#include "setting.h"

class Setting;

class ISettingSource {
public:
    ISettingSource(const std::string &name) : m_alias(name) {};
    virtual ~ISettingSource() = default;
    const std::string Alias() { return m_alias; };

protected:
    std::string m_alias;
};

class ISettingInitializer : public ISettingSource {
public:
    ISettingInitializer(const std::string &name) : ISettingSource(name) {};
    virtual std::vector<Setting> InitializeSettings() = 0;
};

class ISettingReader : public ISettingSource {
public:
    ISettingReader(const std::string &name) : ISettingSource(name) {};
    virtual std::map<std::string, std::string> GetSettings() = 0;
};
