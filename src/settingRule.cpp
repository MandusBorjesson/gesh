#include "settingRule.h"

bool SettingRuleBool::Verify(setting_t value) {
    if (std::holds_alternative<bool>(value)) {
        return true;
    } else {
        return false;
    }
}

bool SettingRuleRangedInt::Verify(setting_t value) {
    if (std::holds_alternative<int>(value)) {
        auto val = std::get<int>(value);
        return val >= m_min && val <= m_max;
    } else {
        return false;
    }
}
