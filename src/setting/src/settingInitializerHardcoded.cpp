#include "settingInitializerHardcoded.h"
#include "settingInterface.h"
#include "settingRule.h"
#include <memory>

auto stringRule = SettingRuleString();
auto boolRule = SettingRuleBool();
auto intRule = SettingRuleInt();
auto intRuleRange = SettingRuleRangedInt(0, 10);

auto hwIface = new SettingInterface("hardware");
auto netIface = new SettingInterface("network");

SettingInitializerHardcoded::SettingInitializerHardcoded() : ISettingInitializer("DEFAULT", {hwIface, netIface}) {};

std::vector<Setting> SettingInitializerHardcoded::InitializeSettings() {
    return std::vector<Setting> {
        Setting("Anarchy/test1", std::nullopt, this, &stringRule, {netIface}, {netIface}),
        Setting("Anarchy/test2", "string!", this, &stringRule, {netIface}, {netIface}),
        Setting("Anarchy/test3", "true", this, &stringRule, {netIface}, {netIface}),
        Setting("Integer/test1", "1", this, &intRule, {netIface}, {hwIface, netIface}),
        Setting("Integer/test2", "123", this, &intRule, {netIface}, {hwIface, netIface}),
        Setting("Integer/ranged/test1", "-4000", this, &intRuleRange, {hwIface, netIface}, {hwIface, netIface}),
        Setting("Integer/ranged/test2", "0", this, &intRuleRange, {hwIface, netIface}, {hwIface, netIface}),
        Setting("Integer/ranged/test3", "5", this, &intRuleRange, {hwIface, netIface}, {hwIface, netIface}),
        Setting("Boolean/test1", "true", this, &boolRule, {hwIface, netIface}, {hwIface, netIface}),
        Setting("Boolean/test2", "False", this, &boolRule, {hwIface, netIface}, {hwIface, netIface}),
        Setting("Boolean/test3", "falsy", this, &boolRule, {hwIface, netIface}, {hwIface, netIface}),
        Setting("Boolean/test4", "1", this, &boolRule, {hwIface, netIface}, {hwIface, netIface}),
        Setting("Boolean/test5", "01", this, &boolRule, {hwIface, netIface}, {hwIface, netIface}),
    };
}
