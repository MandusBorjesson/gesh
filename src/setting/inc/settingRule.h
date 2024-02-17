#pragma once

#include "settingTypes.h"
#include <limits.h>
#include <string>
#include <variant>


class SettingRuleException : public SettingException {
public:
    SettingRuleException(const std::string& msg, const std::string child = "") : SettingException(msg, ".rule" + child) {}
};

class SettingRuleConversionException : public SettingRuleException {
public:
    SettingRuleConversionException(const std::string& msg) : SettingRuleException(msg, ".conversion") {}
};

class SettingRuleMissingException : public SettingRuleException {
public:
    SettingRuleMissingException(const std::string& msg) : SettingRuleException(msg, ".missing") {}
};

class ISettingRule {
public:
    setting_t ToSetting(const setting_t &value) {
        auto val = value;
        if (std::holds_alternative<std::string>(val)) {
            val = _fromStr(std::get<std::string>(value));
        }
        _validate(val);
        return val;
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
