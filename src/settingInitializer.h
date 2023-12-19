#pragma once

#include <variant>
#include <vector>
#include <string>
#include "settingRule.h"
#include "setting.h"

class Setting;

class ISettingInitializer {
public:
    ISettingInitializer(std::string name) : m_alias(name) {};
    virtual std::vector<Setting> GetSettings() = 0;
    const std::string Alias() { return m_alias; };

protected:
    std::string m_alias;
};
