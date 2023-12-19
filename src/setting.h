#pragma once

#include "settingInitializer.h"
#include "settingRule.h"
#include <string>
#include <map>
#include <vector>
#include <ostream>

using setting_t = std::variant<std::monostate, bool, int, std::string>;

class ISettingRule;
class ISettingInitializer;

class Setting {
public:
    Setting() = default;
    Setting(std::string name, setting_t value, ISettingInitializer *source, ISettingRule *rule);
    bool Set(setting_t value);
    setting_t Get() const { return m_value; };
    std::string Name() const { return m_name; };
    std::string Source() const;
    bool Ok();

    friend std::ostream& operator<<(std::ostream& os, const Setting& s);
private:
    std::string m_name;
    setting_t m_value;
    ISettingInitializer *m_source;
    ISettingRule *m_rule;
};

class SettingHandler {
public:
    SettingHandler(ISettingInitializer &initializer,
                   std::vector<ISettingInitializer> &extraSources);

    void UpdateSettings(const std::map<std::string, int> newSettings);
    const std::map<std::string, Setting> Settings() { return m_settings; };

private:
    std::map<std::string, Setting> m_settings;
};
