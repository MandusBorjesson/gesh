#pragma once

#include <limits.h>
#include <string>
#include <variant>

using setting_t = std::variant<std::monostate, bool, int, std::string>;

class ISettingRule {
public:
    virtual bool Verify(setting_t value) = 0;
};

class SettingRuleBool: public ISettingRule {
public:
    SettingRuleBool() = default;
    bool Verify(setting_t value);
};

class SettingRuleRangedInt: public ISettingRule {
public:
    SettingRuleRangedInt(int min, int max) : m_min(min), m_max(max) {};
    bool Verify(setting_t value);

protected:
    int m_max;
    int m_min;
};

class SettingRuleInt: public SettingRuleRangedInt {
public:
    SettingRuleInt() : SettingRuleRangedInt(INT_MIN, INT_MAX) {};
};
