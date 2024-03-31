#include "settingInitializerHardcoded.h"
#include "settingInterface.h"
#include "settingRule.h"
#include "log.h"
#include <memory>

auto stringRule = SettingRuleString();
auto boolRule = SettingRuleBool();
auto intRule = SettingRuleInt();
auto intRuleRange = SettingRuleRangedInt(0, 10);

auto hwIface = new SettingInterface("hardware");
auto netIface = new SettingInterface("network");

SettingInitializerHardcoded::SettingInitializerHardcoded() : ISettingInitializer("DEFAULT", {hwIface, netIface}) {};

std::vector<Setting> SettingInitializerHardcoded::InitializeSettings() {
    auto source = std::make_shared<ISettingSource> (*this);

    return std::vector<Setting> {
        Setting("Anarchy/test1", std::nullopt, source, &stringRule, {netIface}, {netIface}),
        Setting("Anarchy/test2", "string!", source, &stringRule, {netIface}, {netIface}),
        Setting("Anarchy/test3", "true", source, &stringRule, {netIface}, {netIface}),
        Setting("Integer/test1", "1", source, &intRule, {netIface}, {hwIface, netIface}),
        Setting("Integer/test2", "123", source, &intRule, {netIface}, {hwIface, netIface}),
        Setting("Integer/ranged/test1", "-4000", source, &intRuleRange, {hwIface, netIface}, {hwIface, netIface}),
        Setting("Integer/ranged/test2", "0", source, &intRuleRange, {hwIface, netIface}, {hwIface, netIface}),
        Setting("Integer/ranged/test3", "5", source, &intRuleRange, {hwIface, netIface}, {hwIface, netIface}),
        Setting("Boolean/test1", "true", source, &boolRule, {hwIface, netIface}, {hwIface, netIface}),
        Setting("Boolean/test2", "False", source, &boolRule, {hwIface, netIface}, {hwIface, netIface}),
        Setting("Boolean/test3", "falsy", source, &boolRule, {hwIface, netIface}, {hwIface, netIface}),
        Setting("Boolean/test4", "1", source, &boolRule, {hwIface, netIface}, {hwIface, netIface}),
        Setting("Boolean/test5", "01", source, &boolRule, {hwIface, netIface}, {hwIface, netIface}),
    };
}
