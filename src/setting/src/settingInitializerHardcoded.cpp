#include "settingInitializerHardcoded.h"
#include "settingInterface.h"
#include "settingRule.h"
#include "settingStorageFile.h"
#include "log.h"
#include <memory>

auto stringRule = SettingRuleString();
auto stringEnumRule = SettingRuleStringEnum({"YES","NO","MAYBE"});
auto boolRule = SettingRuleBool();
auto intRule = SettingRuleInt();
auto intRuleRange = SettingRuleRangedInt(0, 10);

auto hwIface = new SettingInterface("hardware");
auto netIface = new SettingInterface("network");

auto logger = Log("storage");
auto storageFile = new SettingStorageFile(logger);

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

SettingInitializerDefault::SettingInitializerDefault() : ISettingInitializer({hwIface, netIface}, {storageFile}) {};

std::vector<Setting> SettingInitializerDefault::InitializeSettings() {

    return std::vector<Setting> {
        Setting("Anarchy/test1", &stringRule, storageFile, {netIface}, {netIface}),
        Setting("Anarchy/test2", &stringRule, storageFile, {netIface}, {}),
        Setting("Anarchy/test3", &stringRule, storageFile, {}, {netIface}),
        Setting("String/enum/test1", &stringEnumRule, storageFile, {hwIface, netIface}, {hwIface, netIface}),
        Setting("String/enum/test2", &stringEnumRule, storageFile, {hwIface, netIface}, {hwIface, netIface}),
        Setting("String/enum/test3", &stringEnumRule, storageFile, {hwIface, netIface}, {hwIface, netIface}),
        Setting("Integer/test1", &intRule, storageFile, {netIface}, {hwIface, netIface}),
        Setting("Integer/test2", &intRule, storageFile, {netIface}, {hwIface, netIface}),
        Setting("Integer/ranged/test1", &intRuleRange,  storageFile,{hwIface, netIface}, {hwIface, netIface}),
        Setting("Integer/ranged/test2", &intRuleRange, storageFile, {hwIface, netIface}, {hwIface, netIface}),
        Setting("Integer/ranged/test3", &intRuleRange, storageFile, {hwIface, netIface}, {hwIface, netIface}),
        Setting("Boolean/test1", &boolRule, storageFile, {hwIface, netIface}, {hwIface, netIface}),
        Setting("Boolean/test2", &boolRule, storageFile, {hwIface, netIface}, {hwIface, netIface}),
        Setting("Boolean/test3", &boolRule, storageFile, {hwIface, netIface}, {hwIface, netIface}),
        Setting("Boolean/test4", &boolRule, storageFile, {hwIface, netIface}, {hwIface, netIface}),
        Setting("Boolean/test5", &boolRule, storageFile, {hwIface, netIface}, {hwIface, netIface}),
    };
}
