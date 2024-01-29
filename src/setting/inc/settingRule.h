#pragma once

#include "settingTypes.h"
#include <limits.h>
#include <string>
#include <variant>


class SettingRuleException : public SettingException {
public:
    SettingRuleException(std::string& msg) : SettingException(msg) {}
};

class SettingRuleConversionException : public SettingRuleException {
public:
    SettingRuleConversionException(std::string& msg) : SettingRuleException(msg) {}
};

class ISettingRule {
public:
    setting_t ToSetting(const std::string &value) {
        auto val = _fromStr(value);
        Validate(val);
        return val;
    }

    void Validate(const setting_t &value) {
        _validate(value);
    }

protected:
    virtual setting_t _fromStr(const std::string &value) = 0;
    virtual void _validate(const setting_t &setting) = 0;
};

class SettingRuleString: public ISettingRule {
public:
    SettingRuleString() = default;

protected:
    setting_t _fromStr(const std::string &value) override;
    void _validate(const setting_t &setting) override;
};

class SettingRuleBool: public ISettingRule {
public:
    SettingRuleBool() = default;

protected:
    setting_t _fromStr(const std::string &value) override;
    void _validate(const setting_t &setting) override;
};

class SettingRuleRangedInt: public ISettingRule {
public:
    SettingRuleRangedInt(int min, int max) : m_min(min), m_max(max) {};

protected:
    setting_t _fromStr(const std::string &value) override;
    void _validate(const setting_t &setting) override;

    int m_max;
    int m_min;
};

class SettingRuleInt: public SettingRuleRangedInt {
public:
    SettingRuleInt() : SettingRuleRangedInt(INT_MIN, INT_MAX) {};
};
