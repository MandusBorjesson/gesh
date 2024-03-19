#pragma once

#include <variant>
#include <vector>
#include <map>
#include <optional>
#include <string>
#include "settingRule.h"
#include "settingTypes.h"
#include "settingInterface.h"
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
    ISettingInitializer(const std::string &name, std::vector<SettingInterface*> ifaces) : ISettingSource(name), m_interfaces(ifaces) {};
    virtual std::vector<Setting> InitializeSettings() = 0;
    std::vector<SettingInterface*> Interfaces() { return m_interfaces; };
protected:
    std::vector<SettingInterface*> m_interfaces;
};

class ISettingReader : public ISettingSource {
public:
    ISettingReader(const std::string &name) : ISettingSource(name) {};
    virtual std::map<std::string, std::optional<std::string>> GetSettings() = 0;
};
