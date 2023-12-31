#pragma once

#include "settingTypes.h"
#include <limits.h>
#include <string>
#include <variant>


class SettingRuleException : public SettingException {
public:
    SettingRuleException(std::string& msg) : SettingException(msg) {}
};

class ISettingRule {
public:
    virtual setting_t ToSetting(const std::string &value) { return value; };
};

class SettingRuleBool: public ISettingRule {
public:
    SettingRuleBool() = default;
    setting_t ToSetting(const std::string &value) override;
};

class SettingRuleRangedInt: public ISettingRule {
public:
    SettingRuleRangedInt(int min, int max) : m_min(min), m_max(max) {};
    setting_t ToSetting(const std::string &value) override;

protected:
    int m_max;
    int m_min;
};

class SettingRuleInt: public SettingRuleRangedInt {
public:
    SettingRuleInt() : SettingRuleRangedInt(INT_MIN, INT_MAX) {};
};
