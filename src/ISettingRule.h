#pragma once

#include <limits.h>
#include <string>
#include <variant>

class ISettingRule {
public:
    virtual bool verify(std::variant<bool, int, std::string> value) = 0;
};

class SettingRuleBool: public ISettingRule {
public:
    SettingRuleBool() = default;
    bool verify(std::variant<bool, int, std::string> value);
};

class SettingRuleRangedInt: public ISettingRule {
public:
    SettingRuleRangedInt(int min, int max) : m_min(min), m_max(max) {};
    bool verify(std::variant<bool, int, std::string> value);

protected:
    int m_max;
    int m_min;
};

class SettingRuleInt: public SettingRuleRangedInt {
public:
    SettingRuleInt() : SettingRuleRangedInt(INT_MIN, INT_MAX) {};
};
