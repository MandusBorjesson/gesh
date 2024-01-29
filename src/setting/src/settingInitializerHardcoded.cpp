#include "settingInitializerHardcoded.h"
#include "settingRule.h"

auto stringRule = SettingRuleString();
auto boolRule = SettingRuleBool();
auto intRule = SettingRuleInt();
auto intRuleRange = SettingRuleRangedInt(0, 10);

std::vector<Setting> SettingInitializerHardcoded::InitializeSettings() {
    return std::vector<Setting> {
        Setting("Anarchy/test1", "1", this, &stringRule),
        Setting("Anarchy/test2", "string!", this, &stringRule),
        Setting("Anarchy/test3", "true", this, &stringRule),
        Setting("Integer/test1", "1", this, &intRule),
        Setting("Integer/test2", "123", this, &intRule),
        Setting("Integer/ranged/test1", "-4000", this, &intRuleRange),
        Setting("Integer/ranged/test2", "0", this, &intRuleRange),
        Setting("Integer/ranged/test3", "5", this, &intRuleRange),
        Setting("Boolean/test1", "true", this, &boolRule),
        Setting("Boolean/test2", "False", this, &boolRule),
        Setting("Boolean/test3", "falsy", this, &boolRule),
        Setting("Boolean/test4", "1", this, &boolRule),
        Setting("Boolean/test5", "01", this, &boolRule),
    };
}
