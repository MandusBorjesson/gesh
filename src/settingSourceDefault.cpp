#include "settingSourceDefault.h"
#include "ISettingRule.h"

auto boolRule = SettingRuleBool();
auto intRule = SettingRuleInt();
auto intRuleRange = SettingRuleRangedInt(0, 10);

std::vector<ISettingSource::Setting> SettingSourceDefault::GetSettings() {
    return std::vector<ISettingSource::Setting> {
        (struct Setting){"Anarchy/test1", 1, this, nullptr},
        (struct Setting){"Anarchy/test2", "string!", this, nullptr},
        (struct Setting){"Anarchy/test3", true, this, nullptr},
        (struct Setting){"Integer/test1", 1, this, &intRule},
        (struct Setting){"Integer/test2", 123, this, &intRule},
        (struct Setting){"Integer/ranged/test1", -4000, this, &intRuleRange},
        (struct Setting){"Integer/ranged/test2", 0, this, &intRuleRange},
        (struct Setting){"Integer/ranged/test3", 5, this, &intRuleRange},
        (struct Setting){"Boolean/test1", true, this, &boolRule},
        (struct Setting){"Boolean/test2", false, this, &boolRule},
        (struct Setting){"Boolean/test3", "falsy", this, &boolRule},
    };
}
