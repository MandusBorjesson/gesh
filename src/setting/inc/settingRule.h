#pragma once

#include "settingTypes.h"
#include <limits.h>
#include <string>
#include <variant>
#include <vector>
#include <optional>


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
    setting_t ToSetting(const std::optional<setting_t> &value) {
        if (!value.has_value()) {
            return setting_t();
        }
        auto val = value.value();
        if (std::holds_alternative<std::monostate>(val)) {
            return setting_t();
        }
        if (std::holds_alternative<std::string>(val)) {
            val = _fromStr(std::get<std::string>(val));
        }
        _validate(val);
        return val;
    }
    virtual const std::string AsString() const = 0;

protected:
    virtual setting_t _fromStr(const std::string &value) = 0;
    virtual void _validate(const setting_t &setting) = 0;
};

class SettingRuleString: public ISettingRule {
public:
    SettingRuleString() = default;
    const std::string AsString() const override;

protected:
    setting_t _fromStr(const std::string &value) override;
    void _validate(const setting_t &setting) override;
};

class SettingRuleStringEnum: public SettingRuleString {
public:
    SettingRuleStringEnum(std::vector<std::string> valid_values) : m_valids(valid_values) {};
    const std::string AsString() const override;

protected:
    void _validate(const setting_t &setting) override;

private:
    std::vector<std::string> m_valids;
};

class SettingRuleBool: public ISettingRule {
public:
    SettingRuleBool() = default;
    const std::string AsString() const override;

protected:
    setting_t _fromStr(const std::string &value) override;
    void _validate(const setting_t &setting) override;
};

class SettingRuleRangedInt: public ISettingRule {
public:
    SettingRuleRangedInt(int min, int max) : m_min(min), m_max(max) {};
    const std::string AsString() const override;

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
