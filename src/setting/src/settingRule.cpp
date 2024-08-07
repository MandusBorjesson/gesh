#include "settingRule.h"
#include <iterator>
#include <algorithm>
#include <string>

std::string toType(const setting_t &value) {
    if (std::holds_alternative<std::string>(value)) {
        return "string";
    } else if (std::holds_alternative<int>(value)) {
        return "integer";
    } else if (std::holds_alternative<bool>(value)) {
        return "boolean";
    } else if (std::holds_alternative<std::monostate>(value)) {
        return "<un-initialized>";
    } else {
        return "unknown";
    }
}

const std::string SettingRuleString::AsString() const {
    return "String()";
}

setting_t SettingRuleString::_fromStr(const std::string &value) {
    return value;
}

void SettingRuleString::_validate(const setting_t &setting) {
    if (!std::holds_alternative<std::string>(setting)) {
        std::string err = "Invalid setting type " + toType(setting) + ", expected string";
        throw SettingRuleException(err);
    }
}

const std::string SettingRuleStringEnum::AsString() const {
    const char* const delim = "|";
    std::ostringstream opt_ss;
    std::copy(m_valids.begin(), m_valids.end(), std::ostream_iterator<std::string>(opt_ss, delim));
    auto opt_str = opt_ss.str();
    opt_str.pop_back();  // Remove last "|" symbol
    return "StringSelect(" + opt_str + ")";
}

void SettingRuleStringEnum::_validate(const setting_t &setting) {
    SettingRuleString::_validate(setting);

    std::string val = std::get<std::string>(setting);

    if (std::find(m_valids.begin(), m_valids.end(), val) == m_valids.end()) {
        const char* const delim = "|";
        std::ostringstream opt_ss;
        std::copy(m_valids.begin(), m_valids.end(), std::ostream_iterator<std::string>(opt_ss, delim));
        auto opt_str = opt_ss.str();
        opt_str.pop_back();  // Remove last "|" symbol

        std::string err = "'" + val + "' not in allowed values (" + opt_str + ")";
        throw SettingRuleException(err);
    }
}

const std::string SettingRuleBool::AsString() const {
    return "Boolean()";
}

setting_t SettingRuleBool::_fromStr(const std::string &value) {
    std::string value_tmp = value;
    // Make the value lowercase to ease comparison
    for (int i = 0; i < value_tmp.length(); i++) {
        value_tmp[i] = static_cast<char>(std::tolower(value_tmp[i]));
    }

    if (value_tmp.compare("false") == 0 || value_tmp.compare("0") == 0) {
        return false;
    }
    if (value_tmp.compare("true") == 0 || value_tmp.compare("1") == 0) {
        return true;
    }
    std::string err = "Cannot convert \"" + value_tmp + "\" to boolean.";
    throw SettingRuleConversionException(err);
}

void SettingRuleBool::_validate(const setting_t &setting) {
    if (!std::holds_alternative<bool>(setting)) {
        std::string err = "Invalid setting type " + toType(setting) + ", expected boolean";
        throw SettingRuleException(err);
    }
}

const std::string SettingRuleRangedInt::AsString() const {
    return "Integer(" + std::to_string(m_min) + "|" + std::to_string(m_max) + ")";
}

setting_t SettingRuleRangedInt::_fromStr(const std::string &value) {
    std::stringstream logstream;

    try {
        std::size_t pos{};
        return std::stoi(value, &pos);
    } catch (std::invalid_argument const& ex) {
        std::string err = "Cannot convert \"" + value + "\" to integer (Invalid).";
        throw SettingRuleConversionException(err);
    } catch (std::out_of_range const& ex) {
        std::string err = "Cannot convert \"" + value + "\" to integer (Out of range).";
        throw SettingRuleConversionException(err);
    }
}

void SettingRuleRangedInt::_validate(const setting_t &setting) {
    if (!std::holds_alternative<int>(setting)) {
        std::string err = "Invalid setting type " + toType(setting) + ", expected integer";
        throw SettingRuleException(err);
    }

    int val = std::get<int>(setting);

    if (!(val >= m_min && val <= m_max)) {
        std::string err = "Value " + std::to_string(val) + " not in range [" + std::to_string(m_min) + ", " + std::to_string(m_max) + "].";
        throw SettingRuleException(err);
    }
}
