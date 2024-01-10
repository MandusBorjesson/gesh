#include "settingRule.h"
#include "log.h"

setting_t SettingRuleBool::ToSetting(const std::string &value) {
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
    throw SettingRuleException(err);
}

setting_t SettingRuleRangedInt::ToSetting(const std::string &value) {
    int val;
    std::stringstream logstream;

    try {
        std::size_t pos{};
        val = std::stoi(value, &pos);
    } catch (std::invalid_argument const& ex) {
        std::string err = "Cannot convert \"" + value + "\" to integer (Invalid).";
        throw SettingRuleException(err);
    } catch (std::out_of_range const& ex) {
        std::string err = "Cannot convert \"" + value + "\" to integer (Out of range).";
        throw SettingRuleException(err);
    }

    if (val >= m_min && val <= m_max) {
        return val;
    } else {
        std::string err = "Value " + std::to_string(val) + " not in range [" + std::to_string(m_min) + ", " + std::to_string(m_max) + "].";
        throw SettingRuleException(err);
    }
}
