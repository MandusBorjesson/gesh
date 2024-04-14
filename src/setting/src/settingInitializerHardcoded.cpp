#include "settingInitializerHardcoded.h"
#include "settingInterface.h"
#include "settingRule.h"
#include "log.h"
#include <memory>

auto stringRule = SettingRuleString();
auto stringEnumRule = SettingRuleStringEnum({"YES","NO","MAYBE"});
auto boolRule = SettingRuleBool();
auto intRule = SettingRuleInt();
auto intRuleRange = SettingRuleRangedInt(0, 10);

auto hwIface = new SettingInterface("hardware");
auto netIface = new SettingInterface("network");

std::map<std::string, std::optional<std::string>> SettingReaderDefault::GetSettings() {
    return {
        {"Anarchy/test2", "string!"},
        {"Anarchy/test3", "true"},
        {"String/enum/test2", "HEJ"},
        {"String/enum/test3", "YES"},
        {"Integer/test1", "1"},
        {"Integer/test2", "123"},
        {"Integer/ranged/test1", "-4000"},
        {"Integer/ranged/test2", "0"},
        {"Integer/ranged/test3", "5"},
        {"Boolean/test1", "true"},
        {"Boolean/test2", "False"},
        {"Boolean/test3", "falsy"},
        {"Boolean/test4", "1"},
        {"Boolean/test5", "01"},
    };
}

SettingInitializerDefault::SettingInitializerDefault() : ISettingInitializer("-", {hwIface, netIface}) {};

std::vector<Setting> SettingInitializerDefault::InitializeSettings() {
    auto source = std::make_shared<ISettingSource> (*this);

    return std::vector<Setting> {
        Setting("Anarchy/test1", source, &stringRule, {netIface}, {netIface}),
        Setting("Anarchy/test2", source, &stringRule, {netIface}, {}),
        Setting("Anarchy/test3", source, &stringRule, {}, {netIface}),
        Setting("String/enum/test1", source, &stringEnumRule, {hwIface, netIface}, {hwIface, netIface}),
        Setting("String/enum/test2", source, &stringEnumRule, {hwIface, netIface}, {hwIface, netIface}),
        Setting("String/enum/test3", source, &stringEnumRule, {hwIface, netIface}, {hwIface, netIface}),
        Setting("Integer/test1", source, &intRule, {netIface}, {hwIface, netIface}),
        Setting("Integer/test2", source, &intRule, {netIface}, {hwIface, netIface}),
        Setting("Integer/ranged/test1", source, &intRuleRange, {hwIface, netIface}, {hwIface, netIface}),
        Setting("Integer/ranged/test2", source, &intRuleRange, {hwIface, netIface}, {hwIface, netIface}),
        Setting("Integer/ranged/test3", source, &intRuleRange, {hwIface, netIface}, {hwIface, netIface}),
        Setting("Boolean/test1", source, &boolRule, {hwIface, netIface}, {hwIface, netIface}),
        Setting("Boolean/test2", source, &boolRule, {hwIface, netIface}, {hwIface, netIface}),
        Setting("Boolean/test3", source, &boolRule, {hwIface, netIface}, {hwIface, netIface}),
        Setting("Boolean/test4", source, &boolRule, {hwIface, netIface}, {hwIface, netIface}),
        Setting("Boolean/test5", source, &boolRule, {hwIface, netIface}, {hwIface, netIface}),
    };
}
